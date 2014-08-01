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

using namespace MRN;
using namespace std;
using namespace atomiccontrols;
using namespace sight;

namespace mrnstreaming {
    //
    class MRNetProducer{
        //stores input data
        std::map<Rank, std::vector<char>*> bufferData;
        std::map<Rank, int> requestsByRank;
        AtomicSync* synchronizer ;

        //sync primitives
        std::map<Rank, atomic_cond_t*> inQueueSignals;
        atomic_mutex_t* inQueueMutex;

        //MRnet specific params
        Stream* stream;
        Network *net;
        std::set<Rank> peers;
        //for eostream checks
        std::map<Rank, int*> stream_end_flags;
        atomic_mutex_t* flagsMutex;

    public:
        MRNetProducer(std::map<Rank, std::vector<char>*> bufferData , atomic_mutex_t* inQueueMutex, std::map<Rank, atomic_cond_t*> inQueueSignals,
                std::set<Rank> peers, Stream* stream, Network *net, AtomicSync* s, std::map<Rank, int*> endFlags, atomic_mutex_t* flagMutex){
            this->bufferData = bufferData;
            this->inQueueMutex = inQueueMutex;
            this->inQueueSignals = inQueueSignals;
            this->stream = stream;
            this->net = net;
            this->peers = peers ;
            this->stream_end_flags = endFlags;
            this->flagsMutex = flagMutex;

            this->synchronizer = s ;
//            no_of_ranks = peers.size();
            for(std::set<Rank>::iterator it = peers.begin() ; it != peers.end() ; it++){
                requestsByRank[(*it)] = 0 ;
            }
        }
        void loadBuffer(std::vector< PacketPtr > &packets_in, const TopologyLocalInfo& info);
        void checkEOStream(std::vector< PacketPtr > &packets_in);
        bool be_node;
    }   ;

    class MRNetThread {
    private:
        std::map<Rank, atomic_cond_t*> inputSignals ;
        std::map<Rank, std::vector<char>*> bufferData;
        pthread_t* thread1 ;
        std::vector<MRNetParser> iterators;

        AtomicSync* synchronizer ;
        atomic_mutex_t* inQueueMutex;

        bool initialized;

        //MRNEt specific
        Stream *strm;
        int strm_id;
        int tag_id;
        Network *net;
        //for eostream checks
        std::map<Rank, int*> stream_end_flags;
        atomic_mutex_t* flagsMutex;

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

        std::map<Rank, std::vector<char>*> getInputBuffer(){
            return bufferData;
        };

        std::map<Rank, atomic_cond_t*> getPerRankSignals(){
            return inputSignals;
        };

        atomic_mutex_t* getInputMutex(){
            return inQueueMutex;
        }

        atomic_mutex_t* getFlagsMutex(){
            return flagsMutex;
        }

        std::map<Rank, int*> getStreamFlags(){
            return stream_end_flags;
        }

    } ;

}
#endif /* integer_addition_h */
