/*

file: make-2d




*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "utilities.h"

int main(int argc, char** argv){

    int rows;
    int cols;
    char* output_file; 

    if(argc != 4){
        printf("useage : ./make-2d <rows> <cols> <output_file> \n");
        return 1;
    }else{
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
        output_file = argv[3];
    }

    matrix_t matrix;

    matrix.row = rows;
    matrix.col = cols;

    int err = malloc2d(&matrix);

    if(err != 0){
        return 1;
    }

   initializeBoundaries(&matrix);

    //print2D(matrix);

    int w = write2D(output_file, matrix);
    if(w != 0){
        return 1;
    }

    if(err == 0) {free(matrix.m);}

    return 0;

}