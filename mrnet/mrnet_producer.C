/****************************************************************************
 * Copyright � 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#include "mrnet_iterator.h"
#include "mrnet_integration.h"

using namespace mrnstreaming;
using namespace MRN;

void MRNetProducer::loadBuffer(std::vector< PacketPtr > &packets_in, const TopologyLocalInfo &info) {
//#ifdef DEBUG_ON
    printf("[LOAD METHOD of Input Producer...pid : %d size : %d ] \n", getpid(), packets_in.size());
//#endif
    unsigned length;
    for (unsigned int i = 0; i < packets_in.size(); i++) {
        PacketPtr curr_packet = packets_in[i];
        Rank cur_inlet_rank = curr_packet->get_InletNodeRank();
        if (cur_inlet_rank == -1) {
            be_node = true;
        }
        //handle special case - BE sync and case where node is down
        if (cur_inlet_rank != UNKNOWN_NODE && packets_in.size() != 1) {
            if (net->node_Failed(cur_inlet_rank)) {
                printf("[NODE FAILED ] \n");
                // drop packets from failed node
                continue;
            }
        }
        char* val;
        curr_packet->unpack("%ac", &val, &length);

        /*REMOVEEEEEE*/
//        fprintf(stdout, "Procuder: RECV wave %d ..\n",wave++);
//        fprintf(stdout, "Procuder: RECV wave wait send Auc.. bytes read : %d \n", length);
//        for(int j = 0 ; j < length ; j++){
//            printf("%c",val[j]);
//        }
//        printf("\n\n\n\n");
//        return;
        /*REMOVEEEEEE*/
//        char* val2 ;
//        curr_packet->unpack("%ac", &val2, &length);
//        printf("LOAD METHOD unpack len :%d, val[0]: %c , val[1]: %c , val[2]: %c \n", length, val2[0], val2[1], val2[2]);
//        sleep(3);

        //Start locking the queue
        synchronizer->set_mutex_lock(inQueueMutex);
        std::map<Rank, std::vector<DataPckt> *>::iterator it = this->bufferData.find(cur_inlet_rank);

        //if no packets already present in vector array then create new one
        if (it == this->bufferData.end()) {
            this->bufferData[cur_inlet_rank] = new std::vector<DataPckt>;
        }

        int tag_id = curr_packet->get_Tag();
        bool is_final = false ;
        if(tag_id == PROT_END_PHASE){
            is_final = true;
        }
        DataPckt* pkt = new DataPckt(val, length, is_final);
//        pkt->printData();
//        sleep(3);
        //insert unpacked values to relevant buffer indexed by rank
        this->bufferData[cur_inlet_rank]->push_back(*pkt);

        //signal the relevant input consumer  - should be done within lock
        std::map<Rank, atomic_cond_t *>::iterator q = this->inQueueSignals.find(cur_inlet_rank);
        atomic_cond_t *cond = q->second;
        //todo check if handled properly
        requestsByRank[cur_inlet_rank] += 1;
        synchronizer->set_cond_signal(cond);
        //unlock the input queue
        synchronizer->set_mutex_unlock(inQueueMutex);

    }
//#ifdef DEBUG_ON
    printf("[LOAD METHOD Done !!...pid : %d  ] \n", getpid());
//#endif
}

