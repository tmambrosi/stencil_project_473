/*
Grace McLewee

TANNER -> THIS IS AN EXAMPLE OF WHAT OUR MPI CODE DOES

MPI00

3/21/2023

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

#define MIN(a,b)           ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)
#define BLOCK_OWNER(j,p,n) (((p)*((j)+1)-1)/(n))
#define PTR_SIZE           (sizeof(void*))
#define CEILING(i,j)       (((i)+(j)-1)/(j))

int main (int argc, char *argv[]){

    int id;
    int p;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    matrix_t a;
    a.row = 4;
    a.col = 4;

    matrix_t b;     // we'll make 2 nodes and stack 2 rows from each matrix a and store it in matrix b 
    b.row = 4;
    b.col = 4;

    malloc2d(&a);
    malloc2d(&b);

    for(int i = 0; i < a.row; i++){
        for(int j = 0; j < a.col; j++){
            a.m[i][j] = (id*4) + i;
        }
    }

    printf("id: %d my a matrix looks like: \n", id);
    print2D(a);



    long my_first_row = BLOCK_LOW(id, p, a.row);
    //long my_last_row = BLOCK_HIGH(id, p, a.row);

    int* displs = (int *)malloc(p*sizeof(int)); 
    int* all_sizes = (int *)malloc(p*sizeof(int)); 
    for (int i=0; i<p; i++) { 
        all_sizes[i] =  BLOCK_SIZE(i,p,a.row)*a.col;     /* note change from previous example */ 
        displs[i] = i*BLOCK_SIZE(i,p,a.row)*a.col;
    } 

    

    printf("my block size is %d and id %d \n", BLOCK_SIZE(id,p,a.row), id);

    //MPI_Gather(&(a.m)[my_first_row][0], BLOCK_SIZE(id,p,a.row)*a.col, MPI_DOUBLE, &(b.m)[0][0], BLOCK_SIZE(id,p,a.row)*a.col, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int number_of_elements = BLOCK_SIZE(id,p,a.row)*a.col;

    MPI_Gatherv( &(a.m)[my_first_row][0], number_of_elements, MPI_DOUBLE, &(b.m)[0][0], all_sizes, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
// int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm)
    
    if(id == 0){
        printf("the final b matrix looks like this: \n");
        print2D(b);
    }

    MPI_Finalize();

    return 0;
}
