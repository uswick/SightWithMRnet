/****************************************************************************
 * Copyright � 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#include "mrnet_iterator.h"
#include "MRNetProdCon.h"

using namespace mrnstreaming;
using namespace MRN;

void GenericMerger::handleOutput(int *sum_ar) {
#ifdef DEBUG_ON
        fprintf(stdout, "[MERGER Parse iteration handle OUT.. PID :%d  condition :  %p mutex : %p ]\n", getpid(),
                 outSignal, outQueueMutex);
#endif
    synchronizer->set_mutex_lock(outQueueMutex);
    //todo do output sums properly
    outputQueue->insert(outputQueue->begin(), sum_ar, sum_ar + TOTAL_PACKET_SIZE);
#ifdef DEBUG_ON
        printf("[MERGER Write to output Queue... [OUTQ values ]  --> ");
        for (std::vector<char>::iterator it2 = outputQueue->begin(); it2 != outputQueue->end(); it2++) {
            printf(" : < %d > ", *it2);
        }
        printf("[END OUTQ values ]  --> ");
#endif
    //indicate output ready to any consumer who is waiting
    synchronizer->set_cond_signal(outSignal);
    synchronizer->set_mutex_unlock(outQueueMutex);

#ifdef DEBUG_ON
    fprintf(stdout, "[MERGER Parse iteration EXIT OUT.. PID :%d  condition :  %p mutex : %p ]\n", getpid(),
            outSignal, outQueueMutex);
#endif
}



