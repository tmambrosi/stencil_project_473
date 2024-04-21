/*
utilities.c

Grace McLewee and Tanner Ambrosino

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdbool.h>  
#include "utilities.h"
#include <pthread.h>


#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)


int malloc2d(matrix_t *a) //returns by reference 
{

    int rows = a -> row;
    int cols = a -> col;
   // first malloc2d a block of memory for the row pointers and the 2D array
   double **x = (double **)malloc(rows*sizeof(double *) + rows*cols*sizeof(double));

   if(x == NULL){
    perror("error: failed to malloc \n");
    return 1;
   }

   // Now assign the start of the block of memory for the 2D array after the row pointers
   x[0] = (double *)x + rows;

   // Last, assign the memory location to point to for each row pointer
   for (int j = 1; j < rows; j++) {
      x[j] = x[j-1] + cols;
   }

    a->m = x;
    return 0;
}


int initializeBoundaries(matrix_t *a){

    for(int i = 0; i < a->row; i++){
        for( int j = 0; j < a->col; j++){
            if(j == 0 || j == (a->col - 1)){
                (a->m)[i][j] = 1.0;
            }else{
                (a->m)[i][j] = 0;
            }
        }
    }

    return 0;
}


int write2D(char* outputFile, matrix_t a){

    int write;

    FILE *f = fopen(outputFile, "w");

    if(f==NULL){
        perror("error writing to file \n");
        return 1;
    }

    //write rows to file
    write = fwrite(&(a.row), sizeof(int), 1, f);
    if(write == 0){
        perror("error writing rows to file \n");
        return 1;
    }
    
    //write cols to file
    write = fwrite(&(a.col), sizeof(int), 1, f);
    if(write == 0){
        perror("error writing cols to file \n");
        return 1;
    }

    //write matrix data to file
    write = fwrite(&(a.m[0][0]), sizeof(double), a.row*a.col, f);
    if(write == 0){ //error check write 
            perror("failed to write matrix file \n ");
            return 1;
        }
    //printf("wrote %d many things \n", write);

    fclose(f);

    return 0;
}


int get2D(char* filename, matrix_t *a){

    FILE *f = fopen(filename , "r"  );
    if(f == NULL){
        perror("fail to find file \n");
        return 1;
    }

    int read;

   //read rows from file
    read = fread(&(a->row), sizeof(int), 1, f);
    if(read == 0){ //check read success
        perror("failed to read file \n ");
        return 1;
    }
    

    //read columns from file
    read = fread(&(a->col), sizeof(int), 1, f); 
    if(read == 0){
        perror("failed to read file \n ");
        return 1;
    }

    //malloc2d space for matrix
    read = malloc2d(a);
    if(read == 1){
        printf("./get2D fail to malloc");
        return 1;
    }

    //load matrix data from file
    read = fread(&((a->m)[0][0]), sizeof(double), ((a->row)*(a->col)), f);
    if(read != a->col * a->row){
        printf("metadata does not match file data \n");
        return 1;
    }

    fclose(f); 

    return 0;
}


void print2D(matrix_t a) {

    for(int j=0; j<a.row; j++) {
        for(int i=0; i<a.col; i++) {
            printf("%06.2lf\t", a.m[j][i]);
            //printf("%.30f\t", a.m[j][i]);
        } printf("\n");
    }
}


int swap(matrix_t *x, matrix_t *newx) {

    matrix_t temp;
    temp.m = x->m;
    x->m = newx->m;
    newx->m = temp.m; 
    return 0;
}


int stencil(matrix_t *x, matrix_t *newx){

    double **old = x->m;
    
    for(int i = 1; i < (x->row-1); i++){
        for(int j = 1 ; j < (x->col-1) ; j++){
            //printf("accessing element: %d, %d\n", i, j); // goes from 1, 1 to 10, 10 for entire 12x12 matrix

            (newx->m)[i][j] = ((old)[i-1][j-1] + (old)[i-1][j] + (old)[i-1][j+1] + \
                (old)[i][j+1] + (old)[i+1][j+1] + (old)[i+1][j] + \
                (old)[i+1][j-1] + (old)[i][j-1] + (old)[i][j])/9.0;
        }
    }

    return 0;

}

int stencil_cust(matrix_t *x, matrix_t *newx, int firstrow, int lastrow){

    double **old = x->m;
    
    for(int i = firstrow; i <= lastrow; i++){
        for(int j = 1 ; j < (x->col-1) ; j++){
            //printf("accessing element: %d, %d\n", i, j);
            (newx->m)[i][j] = ((old)[i-1][j-1] + (old)[i-1][j] + (old)[i-1][j+1] + \
                (old)[i][j+1] + (old)[i+1][j+1] + (old)[i+1][j] + \
                (old)[i+1][j-1] + (old)[i][j-1] + (old)[i][j])/9.0;
            
        }
    }
    return 0;
}

int stencil_one_row(matrix_t* a, matrix_t *b, double* upper_tmp, double* lower_tmp){
    
    //when both temp arrays are needed to do stencil operation
    int col = a->col;
    double** new = b->m;
    double** old = a->m;
    int i = 0;
    for(int j = 1 ; j < (col-1) ; j++){

            (new)[i][j] = ((upper_tmp)[j-1] + (upper_tmp)[j] + (upper_tmp)[j+1] + \
            (old)[i][j+1] + (lower_tmp)[j+1] + (lower_tmp)[j] + \
            (lower_tmp)[j-1] + (old)[i][j-1] + (old)[i][j])/9.0;
    }

    return 0;

}

int stencil_upper_temp(matrix_t* a, matrix_t *b, double* upper_tmp){
    int i = 0;  // on first row in matrix a
    int col = a->col;
    double** new = b->m;
    double** old = a->m;

    for(int j = 1 ; j < col-1 ; j++){
        (new)[i][j] =  ((upper_tmp)[j-1] + (upper_tmp)[j] + (upper_tmp)[j+1] + \
                        (old)[i][j+1] + (old)[i+1][j+1] + (old)[i+1][j] + \
                        (old)[i+1][j-1] + (old)[i][j-1] + (old)[i][j])/9.0;
                        
    }

    return 0;
    
}

int stencil_lower_temp(matrix_t* a, matrix_t *b, double* lower_tmp){

    int col = a->col;
    double** new = b->m;
    double** old = a->m;

    //on last row in matrix a
    int i = a->row-1;

    for(int j = 1 ; j < (col-1) ; j++){
        (new)[i][j] = ((old)[i-1][j-1] + (old)[i-1][j] + (old)[i-1][j+1] + \
                (old)[i][j+1] +  (lower_tmp)[j+1] +  (lower_tmp)[j] + \
                 (lower_tmp)[j-1] + (old)[i][j-1] + (old)[i][j])/9.0;
                    
        //printf("\n");
        //printf(" assigning value %f \n", (new)[i][j]);
    }

    return 0;

}

int stencil_main(matrix_t* a, matrix_t *b){

    int row = a->row;
    int col = a->col;
    double** new = b->m;
    double** old = a->m;

    for(int i = 1; i < row-1; i++){
        for(int j = 1 ; j < (col-1) ; j++){
            //printf("accessing element: %d, %d\n", i, j);
            (new)[i][j] =  ((old)[i-1][j-1] + (old)[i-1][j] + (old)[i-1][j+1] + \
                (old)[i][j+1] + (old)[i+1][j+1] + (old)[i+1][j] + \
                (old)[i+1][j-1] + (old)[i][j-1] + (old)[i][j])/9.0;
            //printf(" assigning value %f \n", (new)[i][j]);
        }
    }

    return 0;

}


int writeData(char* filename, matrix_t a){

    int write;

    FILE *f = fopen(filename, "a");

    //write matrix data to file 
    write = fwrite(&(a.m[0][0]), sizeof(double), a.row*a.col, f);
    if(write == 0){ //error check write 
            perror("failed to write matrix file \n ");
            return 1;
        }

    fclose(f);

    return 0;

}

void *pthstencil(void* my_data){

    thread_private_t *thread_d = (thread_private_t*) my_data;
    thread_share_t share_data = *(thread_d->shared_data);

    int my_rank = (long) thread_d->rank;
    long m = (long)share_data.matrix_a->row;
    //long local_m = m/(long)share_data.num_threads; 
    long my_first_row = BLOCK_LOW(my_rank, share_data.num_threads, m);
    long my_last_row = BLOCK_HIGH(my_rank, share_data.num_threads, m);

    //print2D(*(share_data.matrix_b));
    //stencil(share_data.matrix_a, share_data.matrix_b);

    if(my_rank == 0){
        my_first_row = 1;
    }
    if(my_rank == (share_data.num_threads-1)){
        my_last_row = my_last_row - 1;
    }


    for(int i = 0; i < share_data.num_iter; i++){

        //printf("My thread id is: %d  I am on iteration # %d\n", thread_d->rank, i);
        //printf("My thread id is: %d  my first row is: %ld my last row is: %ld\n", thread_d->rank, my_first_row, my_last_row)

        stencil_cust(share_data.matrix_a, share_data.matrix_b, my_first_row, my_last_row);

        pthread_barrier_wait(share_data.barrier);

        if(my_rank == 0){
            if(share_data.debug == 2){
                print2D(*(share_data.matrix_b));
            }

            swap(share_data.matrix_a, share_data.matrix_b);

            if(share_data.file != NULL){
                writeData(share_data.file, *(share_data.matrix_a));
            }
            //printf("swapped \n");
        }

        pthread_barrier_wait(share_data.swapbarrier);

        //printf("My thread id is: %d I passed the barrier \n", thread_d->rank);
    
    }
    
    return NULL;
}

