/*

stencil-2d.c

Grace McLewee

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdbool.h>  
#include <semaphore.h>
#include "utilities.h"
#include "timer.h"
#include <pthread.h>
#include <errno.h>

// 0 – no printing anything to the screen other than perhaps any error messages that happen when the program is run. 

// 1 – basic debug information (file sizes / names, etc), but not prints in side of the loops – minimal output

// 2 – verbose output that showed the state of the matrix after each step, like this:

int main(int argc, char** argv){

    int num_iterations;
    int debug_level = 0;
    char* i_file;
    char* o_file;
    char* all_iterations = NULL;
    int err;
    int num_threads;

    mytimer_t mytime = { .all_time = 0.0, .compute_time = 0.0 };

    GET_TIME(mytime.all_start);

    if(argc < 6){
        printf("usage: ./pth-stencil-2d <num iter.> <infile> <outfile> <debug level> <num threads> <all-stacked-file-name.raw (optional)>\n");
        
        return 1;

    }else{
        num_iterations = atoi(argv[1]);
        i_file = argv[2];
        o_file = argv[3]; 
        debug_level = atoi(argv[4]);
        num_threads = atoi(argv[5]);
        all_iterations = argv[6];
        
    }

    matrix_t x;
    matrix_t newx;

    //get data from the file
    err = get2D(i_file, &x);
    if(err == 1){
        perror("error getting data from file\n");
        return 1;
    }else if(debug_level == 1){
        printf("Got matrix: \n");
        print2D(x);
    }

    if(debug_level == 1){
        print2D(x);
        printf("debug lvl 2: \n size of files: %ld | input file: %s | output file: %s \n", (x.row*x.col*sizeof(double)), i_file, o_file );
    }


    newx.row = x.row;
    newx.col = x.col;

    err = malloc2d(&newx); //malloc2d new matrix 

    if(err == 1){
        printf(" malloc2d err = %d \n", err);
        return 1;
    }

    //copy data from x to new x
    initializeBoundaries(&newx);

    int thread = 0;
    pthread_t* thread_handles;
    pthread_barrier_t barrier;  
    pthread_barrier_t swapbarrier;
    int barrier_count = num_threads;

    thread_handles = malloc(num_threads*sizeof(pthread_t)); //malloc2d thread handle matrix

    thread_share_t thread_s;

    thread_s.num_threads = num_threads;
    thread_s.barrier = &barrier;
    thread_s.swapbarrier = &swapbarrier;
    thread_s.matrix_a = &x;
    thread_s.matrix_b = &newx;
    thread_s.num_iter = num_iterations;
    thread_s.debug = debug_level;
    thread_s.file = all_iterations;

    thread_private_t *thread_p[num_threads];
    

    int ret = pthread_barrier_init(&barrier, NULL, barrier_count);
    int ret2 = pthread_barrier_init(&swapbarrier, NULL, barrier_count);

    if(ret || ret2) {
        switch(ret) {
            case EAGAIN:
                printf(" Lacks the resources to initialize barrier\n"); 
                break;
            case EINVAL:
                printf(" Barrier Count is equal to zero\n");            
                break;
            case ENOMEM:
                printf(" Insufficient memory exists to init barrier\n");
                break;
        }
        return 1;
    }

    GET_TIME(mytime.compute_start);

    //create threads
    for (thread = 0; thread < num_threads; thread++){
        thread_p[thread] = (thread_private_t*) malloc(sizeof(thread_private_t));
        thread_p[thread]->rank = thread;
        thread_p[thread]->shared_data = &thread_s;
        if(debug_level == 1){
            printf("made thread rank: %d\n", thread_p[thread]->rank);
        }
        pthread_create(&thread_handles[thread], NULL, pthstencil, (void*) thread_p[thread]);
    }

    //join treads
    for (thread = 0; thread < num_threads; thread++){
        pthread_join(thread_handles[thread], NULL);
        free(thread_p[thread]);
    }

    GET_TIME(mytime.compute_finish);
    storeCompute(&mytime);

    pthread_barrier_destroy(&barrier);

    //write data to the file
    err = write2D(o_file, newx);
    
    if(err == 1){
        return 1;
    }else if(debug_level == 1){
        printf("Final matrix written to file: \n");
        print2D(x);
    }

    free(x.m);
    free(newx.m);
    free(thread_handles);

    GET_TIME(mytime.all_finish);
    storeAll(&mytime);

    //printf("--SIZE: %d THREADS: %d\n", x.row, num_threads);
    format_time(mytime, num_threads, x.row, x.col);


    return 0;
}
