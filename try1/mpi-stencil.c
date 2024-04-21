/*
 * 
 *  mpi-stencil.c
 * 
 * by Grace McLewee and Tanner Ambrosino
 * 
 *
 *   Last modification: 4/11/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "MyMPI.h"
#include "utilities.h"

typedef double dtype;
#define MPI_TYPE MPI_DOUBLE

int mpi_stencil(int id, int p, matrix_t *a, matrix_t *b, MPI_Comm comm);

int main (int argc, char *argv[]) {
   //dtype** a;         /* Doubly-subscripted array */
   dtype*  storage;   /* Local portion of array elements */
   int     id;        /* Process rank */
   int     m;         /* Rows in matrix */
   int     n;         /* Columns in matrix */
   int     p;         /* Number of processes */
   //double  time, max_time;
   //char* output_file;

   MPI_Init (&argc, &argv);
   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);

   //read_row_striped_matrix (argv[1], (void *) &a, (void *) &storage, MPI_TYPE, &m, &n, MPI_COMM_WORLD);

   matrix_t a;
   a.row = 8;
   a.col = 8;
   malloc2d(&a);

   initializeBoundaries(&a);

   matrix_t b;
   b.row = a.row;
   b.col = a.col;

   malloc2d(&b);
   initializeBoundaries(&b);

    //init structs

   //output_file = argv[2];
   // print_row_striped_matrix ((void **) a, MPI_TYPE, m, n,
   //    MPI_COMM_WORLD);
   int err;
    //function call
   err = mpi_stencil(id, p, &a, &b, MPI_COMM_WORLD);
   if(err == 1){
        printf("mpi stencil function error \n");
        return 1; 
   }

   // print_row_striped_matrix ((void **) a, MPI_TYPE, m, n,
   //    MPI_COMM_WORLD);

   MPI_Finalize();
}


int mpi_stencil(int id, int p, matrix_t *a, matrix_t *b, MPI_Comm comm){

    // lower = physically lower llike if it was a diagram 
    // upper = physically upper

    //    [][][][]  -> upper
    //    [][][][]
    //    [][][][]
    //    [][][][]  -> lower

    // sorry kind of counter intuitive

    int row = a->row;
    int col = a->col;

    long my_first_row = BLOCK_LOW(id, p, row);
    long my_last_row = BLOCK_HIGH(id, p, row);

    //set up temp array

    double* lower_tmp_send = (double*) malloc(row * sizeof(double));
    double* upper_tmp_send = (double*) malloc(row * sizeof(double));

    double* lower_tmp_store = (double*) malloc(row * sizeof(double));
    double* upper_tmp_store = (double*) malloc(row * sizeof(double));

    //load up arrays with row data
    for(int i = 0; i < col; i++){
        lower_tmp_send[i] = a->m[row-1][i]; 
        upper_tmp_send[i] = a->m[0][i];
    }

   //potential area for deadlock
    if(id == 0){
         MPI_Ssend(&lower_tmp_send[0], row, MPI_DOUBLE, 1, 0, comm);
         MPI_Recv(&lower_tmp_store[0], row, MPI_DOUBLE, (id+1), 0, comm, MPI_STATUS_IGNORE);
        //MPI_Send(void* data, int count, MPI_Datatype datatype, int destination, int tag, MPI_Comm communicator)
    }

    else if(id == p-1){
        MPI_Recv(&upper_tmp_store[0], row, MPI_DOUBLE, (id-1), 0, comm, MPI_STATUS_IGNORE);
        MPI_Ssend(&upper_tmp_send[0], row, MPI_DOUBLE, (id-1), 0, comm);  
    }
            
    else{
      MPI_Recv(&upper_tmp_store[0], row, MPI_DOUBLE, (id-1), 0, comm, MPI_STATUS_IGNORE);
      MPI_Ssend(&upper_tmp_send[0], row, MPI_DOUBLE, (id-1), 0, comm);
      
      MPI_Ssend(&lower_tmp_send[0], row, MPI_DOUBLE, (id+1), 0, comm);
      MPI_Recv(&lower_tmp_store[0], row, MPI_DOUBLE, (id+1), 0, comm, MPI_STATUS_IGNORE);
      
      
    }

    if(id == 0){
      // printf("id 0 my first row is %ld \n", my_first_row);
      // printf("id 0 my last row is %ld \n", my_last_row);

      // print2D(*a);

      stencil_main(a, b, my_first_row+1, my_last_row);
      
      // if(my_first_row != my_last_row){
      //    stencil_lower_temp(a, b, my_last_row, lower_tmp_store);
      // }

      // print2D(*b);

    }
    else if(id == (p-1)){
      //stencil upper tmp
      //stencil_upper_temp(a, b, my_first_row, upper_tmp_store);
      //stencil main
      stencil_main(a, b, my_first_row, my_last_row-1);

      print2D(*b);
    }
    else{
      //stencil upper
      stencil_upper_temp(a, b, my_first_row, upper_tmp_store);
      //stencil main
      stencil_main(a, b, my_first_row+1, my_last_row - 1);
      //stencil lower
      stencil_lower_temp(a, b, my_last_row, lower_tmp_store);
    }

   // //  //swap
   MPI_Barrier(MPI_COMM_WORLD);
    //set up for gather
    int* displs = (int *)malloc(p*sizeof(int)); 
    int* all_sizes = (int *)malloc(p*sizeof(int)); 
    for (int i=0; i<p; i++) { 
        all_sizes[i] =  BLOCK_SIZE(i,p,row)*col; 
        displs[i] = i*BLOCK_SIZE(i,p,row)*col;
    } 

    int number_of_elements = BLOCK_SIZE(id,p,a->row);
    //gather all answers to root b matrix
   MPI_Gatherv( &(a->m)[my_first_row][0], number_of_elements*col, MPI_DOUBLE, &(b->m)[0][0], all_sizes, displs, MPI_DOUBLE, 0, comm); 

   MPI_Barrier(MPI_COMM_WORLD);
   // // //  //broadcast
   if(id == 0){
      printf("b matrix was: \n");
      print2D(*b);
   }

   //    swap(b, a);
      
   //    printf("a matrix is: \n");
   //    print2D(*a);
   // }else{
   //    printf("not main root, my b looks like: \n");
   //    print2D(*b);
   // }
   
   // MPI_Barrier(MPI_COMM_WORLD);
   // MPI_Bcast(&(a->m)[0][0], (row * col), MPI_DOUBLE, 0, comm);
   // MPI_Barrier(MPI_COMM_WORLD);

   // if(id > 0){
   //    printf("my new a matrix is: \n");
   //    print2D(*a);
   // }

   return 0;

}
