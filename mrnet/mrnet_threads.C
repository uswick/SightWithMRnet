/****************************************************************************
 * Copyright � 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#include "mrnet_iterator.h"

using namespace mrnstreaming;
using namespace MRN;

void MRNetThread::init(std::set<Rank> ranks) {
    if (!initialized) {
        //initialize threads for producer and consumer
        // producer - callback thread (main)
        // consumer - merger thread
        // init conditions,mutexes
        //there are condition variable for each leaf rank, there are condition vars for each
        //create iterators
        //for each rank create iterator and relevant signal handlers
#ifdef DEBUG_ON
        fprintf(stdout, "[MRNetThread Initialization started.. PID : %d rank %d ]\n", getpid(), r);
#endif
        //input/out sync handling
        outQueueMutex = new atomic_mutex_t;
        *outQueueMutex = ATOMIC_SYNC_MUTEX_INITIALIZER;
        inQueueMutex = new atomic_mutex_t;
        *inQueueMutex = ATOMIC_SYNC_MUTEX_INITIALIZER;

        for (std::set<Rank>::iterator ranks_it = ranks.begin(); ranks_it != ranks.end(); ranks_it++) {
            Rank r = *ranks_it;
            std::vector<char> *inputQueue = new std::vector<char>;
            atomic_cond_t *signal = new atomic_cond_t;
            *signal = ATOMIC_SYNC_COND_INITIALIZER ;

            inputSignals[r] = signal;
            bufferData[r] = inputQueue;
            //each iterator owns a input queue
            MRNetQueueIterator *it = new MRNetQueueIterator(*bufferData[r], inputSignals[r], inQueueMutex, synchronizer);
            iterators.push_back(*it);
        }

        //output handling
        out_signal = new atomic_cond_t;
        *out_signal = ATOMIC_SYNC_COND_INITIALIZER;


        //create ouput queue
        std::vector<char> *outQueue = new std::vector<char>;

        //create a Merger and merge()
        //todo init merge obj properly
        merger = new MRNetMerger(outQueue, out_signal, outQueueMutex, synchronizer);

        //create therads
        thread1 = new pthread_t;
        pthread_create(thread1, NULL, &MRNetThread::consumerFuncHelper, this);
//        pthread_create( thread1, NULL, &MRNetThread::producerFuncHelper, this);

        //exit thread now
        initialized = true;
#ifdef DEBUG_ON
        fprintf(stdout, "[MRNetThread Initialization completed!!.. PID : %d rank %d ]\n", getpid(), r);
#endif
    }
}

void *MRNetThread::consumerFuncHelper(void *arg) {
    MRNetThread *thisPtr = (MRNetThread *) arg;
    thisPtr->consumerFunc();
}

void *MRNetThread::producerFuncHelper(void *arg) {
    MRNetThread *thisPtr = (MRNetThread *) arg;
    thisPtr->prodFunc();
}

void MRNetThread::consumerFunc() {
#ifdef DEBUG_ON
    fprintf(stdout, "[MRNetThread Merge thread Started.. PID : %d number of iterators : %d ]\n", getpid(), iterators.size());
#endif
    //do merging in this seperate thread
    merger->merge(iterators);
}

void MRNetThread::prodFunc() {
    fprintf(stdout, "[PRODUCER MOCK THREAD INIT filter PID : %d  ]\n", getpid());
    fprintf(stdout, "[PRODUCER SLEEPING PID : %d  ]\n", getpid());
    //do merging in this seperate thread
//    sleep(15);
    fprintf(stdout, "[PRODUCER AWAKE!!! SIGNAL PID : %d  condition :  %p mutex : %p ]\n", getpid(), inputSignals[-1],
            inQueueMutex);
    synchronizer->set_mutex_lock(inQueueMutex);
    synchronizer->set_cond_signal(inputSignals[-1]);
    synchronizer->set_mutex_unlock(inQueueMutex);
}

void MRNetThread::destroy() {
    delete thread1;
    delete outQueueMutex;
    delete inQueueMutex;
    delete out_signal;
    //todo remove all heap buffers and signals ,etc safely
}


