/****************************************************************************
 * Copyright � 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#include <vector>

#include "mrnet/Packet.h"
#include "mrnet/NetworkTopology.h"
#include "MRNetProdCon.h"
#include "mrnet_iterator.h"
#include "AtomicSyncPrimitives.h"
#include <string.h>

using namespace MRN;

using namespace mrnstreaming;
using namespace atomiccontrols;

extern "C" {

const char *SightStreamAggregator_format_string = "%d";

typedef struct {
    int a;
    MRNetThread *th;
    MRNetProducer *prod;
    AtomicSync *synchronizer;
    std::vector<char> *inputQueue;
    std::vector<char> *outputQueue;
} glst_t;


glst_t *initAndGetGlobal(void **, Stream *stream, set<Rank> &peers, Network *net);
void testUpstream(vector<PacketPtr> &packets_in, vector<PacketPtr> &packets_out);

void SightStreamAggregator(std::vector< PacketPtr > &packets_in,
        std::vector< PacketPtr > &packets_out,
        std::vector< PacketPtr > & /* packets_out_reverse */,
        void **state_data,
        PacketPtr & /* params */,
        const TopologyLocalInfo &inf) {

#ifdef DEBUG_ON
    fprintf(stdout, "[MRNet FILTER METHOD started.. PID : %d ]\n", getpid());
#endif
    Network *net = const_cast< Network * >( inf.get_Network() );
    PacketPtr first_packet = packets_in[0];

    int stream_id = first_packet->get_StreamId();
    int tag_id = first_packet->get_Tag();
    Stream *stream = net->get_Stream(stream_id);
    set< Rank > peers;
    stream->get_ChildRanks(peers);
    //handle special BE case
    if (peers.size() == 0 && first_packet->get_InletNodeRank() == -1) {
        Rank r = -1;
        peers.insert(-1);
    }
    glst_t *state = initAndGetGlobal(state_data, stream, peers, net);
    state->prod->loadBuffer(packets_in, inf);
    //wait until output is ready
    //wait only if output condition is met
#ifdef DEBUG_ON
    printf("[MRNet FILTER method load completed!!...pid : %d ] \n", getpid());
#endif
//    sleep(5000);

    if (state->prod->getWaitCondition()) {
#ifdef DEBUG_ON
        printf("[MRNet FILTER method entered into output wait phase.. PID : %d condition :  %p mutex : %p] \n", getpid(), state->th->getOutSignal(),
                state->th->getOutputMutex());
#endif
        //todo wait on out signal
        atomic_mutex_t *outMutex = state->th->getOutputMutex();
        atomic_cond_t *outSignal = state->th->getOutSignal();

        //lock out queue  and wait
        state->synchronizer->set_mutex_lock(outMutex);
        state->synchronizer->set_cond_wait(outSignal, outMutex);

        //consume output
        std::vector<char> *outQ = state->th->getOutputQueue();
        int size = outQ->size();
        char *mergerdStr = new char[size];
        int i = 0;
        std::vector<char>::iterator del;
        //drain from ouput-queue  and send the str output through out packet
        for (std::vector<char>::iterator it = outQ->begin(); it != outQ->end();) {
            mergerdStr[i] = *it;
            del = it;
            it = outQ->erase(del);
            i++;
        }
#ifdef DEBUG_ON
        printf("[MRNet FILTER method pushing calculated output to outgoing packets.. pid : %d  Q SIZE : %d size[2] : %d sum[0] : %d ..] \n", getpid(), outQ->size(), size, mergerdStr[0]);
#endif
        PacketPtr new_packet (new Packet(stream_id, tag_id, "%ac", mergerdStr, size));
        packets_out.push_back(new_packet);

        state->synchronizer->set_mutex_unlock(outMutex);
    }
#ifdef DEBUG_ON
    printf("[MRNet FILTER method completed!...pid : %d ] \n", getpid());
#endif
//    testUpstream(packets_in, packets_out);
}

glst_t *initAndGetGlobal(void **state_data, Stream *stream, set<Rank> &peers, Network *net) {
    glst_t *global_state;
    if (*state_data == NULL) {
        global_state = new glst_t;
        global_state->a = 10;
        global_state->synchronizer = new AtomicSync;
//        global_state->it = new MRNetIterator();
        //create thread at init and initalize threads,shared data,etc
        MRNetThread *th = new MRNetThread(global_state->synchronizer);
        th->init(peers);
        global_state->th = th;
        //create producer for call back
        global_state->prod = new MRNetProducer(th->getInputBuffer(), th->getInputMutex(), th->getPerRankSignals(),
                peers, stream, net, global_state->synchronizer);
        *state_data = global_state;
    } else {
        global_state = (glst_t *) (*state_data);
        global_state->a = 105;
    }
#ifdef DEBUG_ON
    fprintf(stdout, "MRNet FILTER method - Init Phase completed - magic num :  %d PID : %d \n", global_state->a, getpid());
#endif
    return global_state;
}

void testUpstream(vector<PacketPtr> &packets_in, vector<PacketPtr> &packets_out) {
    int *sumAr = (int *) malloc(sizeof(int) * TOTAL_PACKET_SIZE);
    for (int k = 0; k < TOTAL_PACKET_SIZE ; k++) {
        sumAr[k] = 0;
    }

    unsigned length;
    for (unsigned int i = 0; i < packets_in.size(); i++) {
        PacketPtr cur_packet = packets_in[i];
        int *val;
        cur_packet->unpack("%ad", &val, &length);
#ifdef DEBUG_ON
        fprintf(stdout, "inside Filter val[0] : %d  length : %d \n", val[0], length);
#endif

        if (length == 0) {
            fprintf(stdout, "invalid buffer length (0) \n");
            return;
        }

        for (int j = 0; j < length; j++) {
            sumAr[j] = sumAr[j] + val[j];
        }
    }
#ifdef DEBUG_ON
    fprintf(stdout, "end filter : sum length : %d  sumAr[0] : %d sumAr[19] : %d \n", length, sumAr[0], sumAr[19]);
#endif

    PacketPtr new_packet (new Packet(packets_in[0]->get_StreamId(),
            packets_in[0]->get_Tag(), "%ad", sumAr, length));
    packets_out.push_back(new_packet);
}


} /* extern "C" */
