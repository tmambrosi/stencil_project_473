/*

utilities.h

Grace McLewee

USE: for the most part, if there is a problem, the function will return 1. 
If there is no problem, the function will return 0

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdbool.h>  


typedef struct matrix{
    double** m; //pointer to a 2d matrix
	int row;    //number of rows
    int col;    //number of cols
} matrix_t;


typedef struct thread_share{ //what data needs to be shared between all threads?
    int num_threads;
    int num_iter;
    int debug;
    char* file;
    matrix_t *matrix_a;
    matrix_t *matrix_b;
    pthread_barrier_t* barrier;
    pthread_barrier_t* swapbarrier;
    


}thread_share_t;

typedef struct thread_data_private {
    int rank;
    thread_share_t *shared_data;

}thread_private_t;




//malloc2d space for a 2D matrix
int malloc2d(matrix_t *a);

//initialize a matrix with 1's at the end and 0's in the middle
int initializeBoundaries(matrix_t *a);

//write a 2D to a file 
int write2D(char* outputFile, matrix_t);

//get a 2D from a file, malloc2ds and loads a matrix
int get2D(char* filename, matrix_t*);

//print the 2D
void print2D(matrix_t a);

//copy matrix data x into new matrix
int swap(matrix_t *x, matrix_t *newx);

//do stencil operation
int stencil(matrix_t *x, matrix_t *newx);

//write just matrix data (no metadata) to a file
int writeData(char* filename, matrix_t a);

//thread function for stencil
void* pthstencil(void* my_data);   

//custom stencil method
int stencil_cust(matrix_t *x, matrix_t *newx, int firstrow, int lastrow);

//custom stencil for MPI 
int stencil_upper_temp(matrix_t* a, matrix_t *b, int row, double* upper_tmp);
int stencil_lower_temp(matrix_t* a, matrix_t *b, int row, double* lower_tmp);
int stencil_main(matrix_t* a, matrix_t *b, int firstrow, int lastrow);