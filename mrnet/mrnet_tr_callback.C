/****************************************************************************
 * Copyright � 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#include <vector>

#include "mrnet/Packet.h"
#include "mrnet/NetworkTopology.h"
#include "mrnet_integration.h"
#include "mrnet_iterator.h"
#include "AtomicSyncPrimitives.h"
#include <string.h>

using namespace MRN;

using namespace mrnstreaming;
using namespace atomiccontrols;

extern "C" {

const char *SightStreamAggregator_format_string = "%ac";

typedef struct {
    MRNetThread *th;
    MRNetProducer *prod;
    AtomicSync *synchronizer;
    std::vector<DataPckt> *inputQueue;
} glst_t;


glst_t *initAndGetGlobal(void **, Stream *stream, set<Rank> &peers, Network *net, int strm_id, int tag_id);

void SightStreamAggregator(std::vector< PacketPtr > &packets_in,
        std::vector< PacketPtr > &packets_out,
        std::vector< PacketPtr > & /* packets_out_reverse */,
        void **state_data,
        PacketPtr & /* params */,
        const TopologyLocalInfo &inf) {

#ifdef DEBUG_ON
    fprintf(stdout, "[MRNet FILTER METHOD just started.. PID : %d ]\n", getpid());
    fflush(stdout);
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
        //special BE optimization
        std::vector< PacketPtr >::iterator in;
        for( in = packets_in.begin() ; in != packets_in.end(); in++) {
            packets_out.push_back(*in);
        }
        return;

#ifdef DEBUG_ON
        fprintf(stdout, "[MRNet FILTER - case BE node.. PID : %d ]\n", getpid());
#endif
    }
    glst_t *state = initAndGetGlobal(state_data, stream, peers, net, stream_id, tag_id);
    state->prod->loadBuffer(packets_in, inf);

#ifdef DEBUG_ON
    printf("[MRNet FILTER method completed!...pid : %d ] \n", getpid());
#endif
}

glst_t *initAndGetGlobal(void **state_data, Stream *stream, set<Rank> &peers, Network *net, int strm_id, int tag_id) {
    glst_t *global_state;
    if (*state_data == NULL) {
        global_state = new glst_t;
        global_state->synchronizer = new AtomicSync;
//        global_state->it = new MRNetIterator();
        //create thread at init and initalize threads,shared data,etc
        MRNetThread *th = new MRNetThread(global_state->synchronizer,stream, strm_id, tag_id, net);
        th->init(peers);
        global_state->th = th;
        //create producer for call back
        global_state->prod = new MRNetProducer(th->getInputBuffer(), th->getInputMutex(), th->getPerRankSignals(),
                peers, stream, net, global_state->synchronizer);
        *state_data = global_state;
    } else {
        global_state = (glst_t *) (*state_data);
    }
#ifdef DEBUG_ON
    fprintf(stdout, "MRNet FILTER method - Init Phase completed - magic num :  %d PID : %d \n", global_state->a, getpid());
#endif
    return global_state;
}


} /* extern "C" */
