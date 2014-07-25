/****************************************************************************
 * Copyright � 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#include "mrnet_iterator.h"
#include "MRNetProdCon.h"

using namespace mrnstreaming;
using namespace MRN;


void MRNetMerger::merge(std::vector<MRNetQueueIterator> parsers) {

    //for each parser try to acquire next set of integers
    //calculate sums
    //output into the queue
    int *sum_ar = new int [TOTAL_PACKET_SIZE];

    std::vector<MRNetQueueIterator>::iterator it;
    int num_active_parsers = parsers.size();
#ifdef DEBUG_ON
    fprintf(stdout, "[MERGER INIT.. PID : %d  ]\n", getpid());
#endif
    while (num_active_parsers > 0) {
        //init sums
        for (int i = 0; i < TOTAL_PACKET_SIZE ; i++) {
            sum_ar[i] = 0;
        }
#ifdef DEBUG_ON
        fprintf(stdout, "[MERGER Iterate through parsers.. PID : %d  ]\n", getpid());
#endif
        for (it = parsers.begin(); it != parsers.end(); ) {
            std::vector<char> inputInts = it->next();
            std::vector<char>::iterator inputInts_it = inputInts.begin();
            for (int i = 0; inputInts_it != inputInts.end(), i < TOTAL_PACKET_SIZE ; inputInts_it++, i++) {
                sum_ar[i] += *inputInts_it;
            }
            //check if this input stream has come to an end
            if (it->end()) {
#ifdef DEBUG_ON
                fprintf(stdout, "[MERGER a Iterate queue END REACHED.. PID : %d  total_ints : %d ]\n", getpid(), it->total_ints);
#endif
                num_active_parsers--;
                std::vector<MRNetQueueIterator>::iterator del = it;
                it = parsers.erase(del);
            } else {
                it++;
            }

        }
        //handle output in a thread safe way
        this->handleOutput(sum_ar);

#ifdef DEBUG_ON
        fprintf(stdout, "[MERGER Iteration complete!!.. continue until all incoming streams are handled..  "   \
                "PID : %d active parsers : %d ]\n", getpid(), num_active_parsers);
#endif
    }

}



