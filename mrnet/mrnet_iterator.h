/****************************************************************************
 * Copyright � 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#if !defined(integer_iterator_h )
#define integer_iterator_h 1

#include "mrnet/Types.h"
#include <unistd.h>
#include <mrnet/Stream.h>
#include <pthread.h>
#include "AtomicSyncPrimitives.h"
#include "../process.h"
#include "mrnet_integration.h"

using namespace MRN;
using namespace std;
using namespace atomiccontrols;
using namespace sight;

namespace mrnstreaming {
    //
    class MRNetProducer{
        //stores input data
        std::map<Rank, std::vector<DataPckt>*> bufferData;
//        std::map<Rank, std::vector<char>*> bufferData;
        std::map<Rank, int> requestsByRank;
        AtomicSync* synchronizer ;

        //sync primitives
        std::map<Rank, atomic_cond_t*> inQueueSignals;
        atomic_mutex_t* inQueueMutex;

        //MRnet specific params
        Stream* stream;
        Network *net;
        std::set<Rank> peers;
        int wave ;

    public:
        MRNetProducer(std::map<Rank, std::vector<DataPckt>*> bufferData , atomic_mutex_t* inQueueMutex, std::map<Rank, atomic_cond_t*> inQueueSignals,
                std::set<Rank> peers, Stream* stream, Network *net, AtomicSync* s){
            this->bufferData = bufferData;
            this->inQueueMutex = inQueueMutex;
            this->inQueueSignals = inQueueSignals;
            this->stream = stream;
            this->net = net;
            this->peers = peers ;
            this->synchronizer = s ;
//            no_of_ranks = peers.size();
            for(std::set<Rank>::iterator it = peers.begin() ; it != peers.end() ; it++){
                requestsByRank[(*it)] = 0 ;
            }
            wave = 0 ;
        }
        void loadBuffer(std::vector< PacketPtr > &packets_in, const TopologyLocalInfo& info);
        bool be_node;
    }   ;

    class MRNetThread {
    private:
        std::map<Rank, atomic_cond_t*> inputSignals ;
        std::map<Rank, std::vector<DataPckt>*> bufferData;
        pthread_t* thread1 ;
        std::vector<sight::MRNetParser*> iterators;

        AtomicSync* synchronizer ;
        atomic_mutex_t* inQueueMutex;

        bool initialized;

        //MRNEt specific
        Stream *strm;
        int strm_id;
        int tag_id;
        Network *net;

    public:
        MRNetThread(AtomicSync* s, Stream* strm, int stream_id, int tag_id, Network* net):initialized(false){
            this->synchronizer = s ;
            this->strm = strm;
            this->strm_id = stream_id;
            this->tag_id = tag_id ;
            this->net = net;
        }

        void init(std::set<Rank> ranks);
        void destroy();
        void consumerFunc();
        static void* consumerFuncHelper(void *arg);

        std::map<Rank, std::vector<DataPckt>*> getInputBuffer(){
            return bufferData;
        };

        std::map<Rank, atomic_cond_t*> getPerRankSignals(){
            return inputSignals;
        };

        atomic_mutex_t* getInputMutex(){
            return inQueueMutex;
        }

    } ;

}
#endif /* integer_addition_h */
