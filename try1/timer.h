/* File:     timer.h
 *
 * Purpose:  Define a macro that returns the number of seconds that 
 *           have elapsed since some point in the past.  The timer
 *           should return times with microsecond accuracy.
 *
 * Note:     The argument passed to the GET_TIME macro should be
 *           a double, *not* a pointer to a double.
 *
 * Example:  
 *    #include "timer.h"
 *    . . .
 *    double start, finish, elapsed;
 *    . . .
 *    GET_TIME(start);
 *    . . .
 *    Code to be timed
 *    . . .
 *    GET_TIME(finish);
 *    elapsed = finish - start;
 *    printf("The code to be timed took %e seconds\n", elapsed);
 *
 * IPP:  Section 3.6.1 (pp. 121 and ff.) and Section 6.1.2 (pp. 273 and ff.)
 * 
 *      modifications by Grace McLewee
 */
#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>
#include <stdlib.h>

/* The argument now should be a double (not a pointer to a double) */
#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}



typedef struct timer{

    double all_start, all_finish;
    double compute_start, compute_finish;

    double all_time;
    double compute_time;

}mytimer_t;

int storeAll(mytimer_t*);

int storeCompute(mytimer_t*);

void format_time(mytimer_t, int num_threads, int rows, int cols);

#endif




