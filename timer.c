/*
timer.c

Grace McLewee

*/

#include "timer.h"
#include <stdio.h>

void format_time(mytimer_t t, int num_threads, int rows, int cols){

    printf("%f %f %d %d %d\n", t.all_time, t.compute_time, num_threads, rows, cols);
}

int storeAll(mytimer_t* t){
    t->all_time = t->all_time + (t->all_finish - t->all_start);

    return 0;
}

int storeCompute(mytimer_t* t){
    t->compute_time = t->compute_time + (t->compute_finish - t->compute_start);

    return 0;

}