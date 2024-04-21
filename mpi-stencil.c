/*
 * 
 *   MPI-STENCIL.C
 * 
 *  by Grace Mclewee and Tanner Ambrosino
 *
 *   Last modification: 4/21/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "MyMPI.h"
#include "utilities.h"
#include <string.h>

typedef double dtype;
#define MPI_TYPE MPI_DOUBLE
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

int mpi_stencil(int id, int p, matrix_t* a, matrix_t*b, double *lower_tmp_send, double* lower_tmp_store, double* upper_tmp_send, double* upper_tmp_store);

int main (int argc, char *argv[]) {
   dtype** pointers_storage;         /* Doubly-subscripted array */
   dtype*  storage;   /* Local portion of array elements */
   char* in_file;     // in file
   int iterations;    // iteration count
   double  time, time2, comp_time, all_time;
   int     id;        /* Process rank */
   int     m;         /* Rows in matrix */
   int     n;         /* Columns in matrix */
   int     p;         /* Number of processes */
   char* output_file; // output file
   char* all_file = NULL; //optional all file
   void* mem_cpy_ret; //mem copy return value

   MPI_Init (&argc, &argv);
   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);

   //usage mpirun -np <p> ./mpi-stencil <iter> <inputfile> <outputfile> <debug level> <optional all file>

    MPI_Barrier (MPI_COMM_WORLD);
   time2 = -MPI_Wtime();

    if(argc < 5){
      printf("usage: mpirun -np <p> ./mpi-stencil <iter> <inputfile> <outputfile> <debug level> <optional all file>");
      return 1;
    }
    iterations = atoi(argv[1]);
    in_file = argv[2];
    output_file = argv[3];
    int debug_level = atoi(argv[4]);
    all_file = argv[5];

    read_row_striped_matrix (in_file, (void *) &pointers_storage, (void *) &storage, MPI_DOUBLE, &m, &n, MPI_COMM_WORLD);

    if(m==0){
      printf("error reading from file \n");
      return 1;
    }

    if(p>m){
      printf("number of threads cannot exceed number of rows \n");
      return 1;
    }

    if(debug_level == 1){
      printf("read in %d rows and %d cols\n", m, n);
    }

    matrix_t in_matrix = {.row=m, .col=n};

    matrix_t a;
    a.row = BLOCK_SIZE(id,p,in_matrix.row);
    a.col = in_matrix.col;

    malloc2d(&a);

    //copy the two array data into a single array with row pointers
    mem_cpy_ret = memcpy(&a.m[0], &pointers_storage[0], (sizeof(double) * a.row));
    if(mem_cpy_ret != &a.m[0]){
      printf("mem copy failure \n");
      return 1;
    }
    mem_cpy_ret = memcpy(&a.m[0][0], &pointers_storage[0][0], (sizeof(double) *a.row*a.col));
    if(mem_cpy_ret != &a.m[0][0]){
      printf("mem copy failure \n");
      return 1;
    }

    matrix_t b;
    b.row = a.row;
    b.col = a.col;

    malloc2d(&b);
    initializeBoundaries(&b);

    double* lower_tmp_send = (double*) malloc(n * sizeof(double));
    double* upper_tmp_send = (double*) malloc(n * sizeof(double));

    double* lower_tmp_store = (double*) malloc(n * sizeof(double));
    double* upper_tmp_store = (double*) malloc(n * sizeof(double));

    if(debug_level == 1){
        printf("original matrix: \n");
        mem_cpy_ret = memcpy(&a.m[0], &pointers_storage[0], (sizeof(double) * a.row));
        if(mem_cpy_ret != &a.m[0]){
          printf("mem copy failure \n");
          return 1;
        }
        print_row_striped_matrix ((void **) pointers_storage, MPI_TYPE, m, n, MPI_COMM_WORLD);
    }

    if(all_file != NULL){
      mem_cpy_ret = memcpy(&a.m[0], &pointers_storage[0], (sizeof(double) * a.row));
      if(mem_cpy_ret != &a.m[0]){
        printf("mem copy failure \n");
        return 1;
      }
      write_row_striped_matrix_data(all_file, pointers_storage, MPI_DOUBLE, in_matrix.row, in_matrix.col, MPI_COMM_WORLD);
    }

    MPI_Barrier (MPI_COMM_WORLD);
    //record computation time
    time = -MPI_Wtime();

    //begin stencil iteration
    for(int i = 0; i<iterations; i++){
      if(debug_level == 2){
        printf("iteration no. %d\n", i);
        mem_cpy_ret = memcpy(&a.m[0], &pointers_storage[0], (sizeof(double) * a.row));
        if(mem_cpy_ret != &a.m[0]){
          printf("mem copy failure \n");
          return 1;
        }
        print_row_striped_matrix ((void **) pointers_storage, MPI_TYPE, m, n, MPI_COMM_WORLD);
      }

      MPI_Barrier(MPI_COMM_WORLD);

      mpi_stencil(id, p, &a, &b, lower_tmp_send, lower_tmp_store, upper_tmp_send, upper_tmp_store);

      MPI_Barrier(MPI_COMM_WORLD);
      swap(&a, &b);

      if(all_file != NULL){
        mem_cpy_ret = memcpy(&pointers_storage[0], &a.m[0], (sizeof(double) * a.row));
        if(mem_cpy_ret != &pointers_storage[0]){
          printf("mem copy failure \n");
          return 1;
        }
        write_row_striped_matrix_data(all_file, pointers_storage, MPI_DOUBLE, in_matrix.row, in_matrix.col, MPI_COMM_WORLD);
      }
      
    }

    //record computation time
   time += MPI_Wtime();
   MPI_Reduce (&time, &comp_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // printf("my id is %d \n", id);
    // print2D(b);

    if(debug_level == 1){
        printf("matrix after stencil operation: \n");
        mem_cpy_ret = memcpy(&pointers_storage[0], &a.m[0], (sizeof(double) * a.row));
        if(mem_cpy_ret != &pointers_storage[0]){
          printf("mem copy failure \n");
          return 1;
        }
        print_row_striped_matrix ((void **) pointers_storage, MPI_TYPE, m, n, MPI_COMM_WORLD);
    }

    //double **pointers = (double**) malloc(sizeof(double) * a.row);

    mem_cpy_ret = memcpy(&pointers_storage[0], &a.m[0], (sizeof(double) * a.row));
    if(mem_cpy_ret != &pointers_storage[0]){
      printf("mem copy failure \n");
      return 1;
    }
   
    write_row_striped_matrix (output_file, pointers_storage, MPI_DOUBLE, in_matrix.row, in_matrix.col, MPI_COMM_WORLD);

    free(a.m);
    free(b.m);

    free(upper_tmp_send);
    free(lower_tmp_send);
    free(upper_tmp_store);
    free(lower_tmp_store);

    free(pointers_storage);
    free(storage);

    time2 += MPI_Wtime();
    MPI_Reduce (&time2, &all_time, 1, MPI_DOUBLE, MPI_MAX, 0,MPI_COMM_WORLD);

    if (!id) printf ("%f %f %d %d %d\n",all_time, comp_time, p, m, n);

   MPI_Finalize();
}

int mpi_stencil(int id, int p, matrix_t* a, matrix_t*b, double* lower_tmp_send, double* lower_tmp_store, double* upper_tmp_store, double*upper_tmp_send){

    int row = a->row;
    int col = a->col;

    //load up temproary arrays
    for(int i = 0; i < col; i++){
          if(row == 1){
            lower_tmp_send[i] = (double)(a->m)[0][i]; 
          }
          else{
            lower_tmp_send[i] = (double)(a->m)[row-1][i];
          } 
          upper_tmp_send[i] = (double)(a->m)[0][i];
    }

    // printf("my id is: %d these are my vectors to be sent\n", id);
    // for(int i = 0; i < col; i++){
    //     printf("%f ",upper_tmp_send[i] );
    // }printf("\n");
    //  for(int i = 0; i < col; i++){
    //     printf("%f ",lower_tmp_send[i]);
    // }printf("\n");


    if(p > 1){  //if only one process, no need to send temp arrays

      if(id == 0){
          MPI_Ssend(&lower_tmp_send[0], col, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
          MPI_Recv(&lower_tmp_store[0], col, MPI_DOUBLE, (id+1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }

      else if(id == p-1){
          MPI_Recv(&upper_tmp_store[0], col, MPI_DOUBLE, (id-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          MPI_Ssend(&upper_tmp_send[0], col, MPI_DOUBLE, (id-1), 0, MPI_COMM_WORLD);  
      }
              
      else{
        MPI_Recv(&upper_tmp_store[0], col, MPI_DOUBLE, (id-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Ssend(&upper_tmp_send[0], col, MPI_DOUBLE, (id-1), 0, MPI_COMM_WORLD);
        
        MPI_Ssend(&lower_tmp_send[0], col, MPI_DOUBLE, (id+1), 0, MPI_COMM_WORLD);
        MPI_Recv(&lower_tmp_store[0], col, MPI_DOUBLE, (id+1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
      }
    }

    // printf("my id is: %d these are my vectors stored \n", id);
    //  for(int i = 0; i < col; i++){
    //     printf("%.30f ",upper_tmp_store[i]);
    // }printf("\n");
    // for(int i = 0; i < col; i++){
    //     printf("%.30f ",lower_tmp_store[i] );
    // }printf("\n");

  if(id == 0){
    if(row != 1){
      stencil_main(a, b);
        if(p>1){
          stencil_lower_temp(a, b, lower_tmp_store);
        }
      }
    }
    else if(id == (p-1)){

      if(row != 1){
        stencil_upper_temp(a, b, upper_tmp_store);

        stencil_main(a, b);
      }
    }
    else{
      if(row==1){
        stencil_one_row(a, b, upper_tmp_store, lower_tmp_store);
      }else{
        //stencil upper
        stencil_upper_temp(a, b, upper_tmp_store);
        //stencil main
        stencil_main(a, b);
        //stencil lower
        stencil_lower_temp(a, b, lower_tmp_store);
      }
    }
    return 0;

}