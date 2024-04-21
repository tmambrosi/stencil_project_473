#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdbool.h>  
#include "utilities.h"
#include "timer.h"


int main(int argc, char** argv){

    int num_iterations;
    char* i_file;
    char* o_file;
    char* all_iterations = NULL;
    int err;

    if(argc < 4){
        printf("usage: ./serial-2d <num iterations> <input file> <output file> <all-iterations> \n");
        return 1;

    }else{
        num_iterations = atoi(argv[1]);
        i_file = argv[2];
        o_file = argv[3]; //rowxcolxiterations.raw
        all_iterations = argv[4];
        
    }


    matrix_t x;
    matrix_t newx;
    //get data from the file
    err = get2D(i_file, &x);
    if(err == 1){
        perror("error getting data from file\n");
        return 1;
    }


    newx.row = x.row;
    newx.col = x.col;

    err = malloc2d(&newx);

    if(err == 1){
        printf(" allocate err = %d \n", err);
        return 1;
    }

    //copy data from x to new x
    initializeBoundaries(&newx);
    if(all_iterations != NULL){
        writeData(all_iterations, x); 
    }

    //iterate
    for(int i = 0; i < num_iterations; i++){

        //write data (w/o metadata) to file
        stencil(&x, &newx);
        swap(&x, &newx);
        if(all_iterations != NULL){
            writeData(all_iterations, x); 
        }

        // print2D(newx);
        // printf("\n");
    }

    //write data to the file
    err = write2D(o_file, x);
    
    if(err == 1){
        return 1;
    }

    free(x.m);
    free(newx.m);
    return 0;


}
