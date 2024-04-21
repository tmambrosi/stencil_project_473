/*
print-2d.c

Grace McLewee

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "utilities.h"


int main(int argc, char** argv){

    char* inputfile;

    if(argc != 2){
        printf("usage: ./print-2d <input_file> \n");
        return 1;
    }else{
        inputfile = argv[1];
    }

    matrix_t matrix;

    int err = get2D(inputfile, &matrix);

    print2D(matrix);

    if(err == 0){free(matrix.m);}

    return 0;


}