/*
Grace McLewee

MPI00

3/21/2023

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){

    int id;
    int p;
    int adder;
    int start = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    adder = atoi(argv[1]);

    if(id == 0){

        printf("rank - %d starting with %d \n", id, adder);
    
        adder = adder + 1;

        printf("rank - %d sending %d to rank %d \n", id, adder, id+1);

        fflush(stdout);

        MPI_Ssend(&adder, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        start++;

        MPI_Recv(&adder, 1, MPI_INT, (p-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("rank - %d final answer %d \n", id, adder);

    }else if(id == (p-1)){

        MPI_Recv(&adder, 1, MPI_INT, (id-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("rank - %d recieved %d from rank %d\n", id, adder, id-1);

        adder = adder + 1;

        printf("rank - %d sending %d to rank %d \n", id, adder, 0);

        fflush(stdout);

        MPI_Ssend(&adder, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    
    }else{

        MPI_Recv(&adder, 1, MPI_INT, (id-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("rank - %d recieved %d from rank %d\n", id, adder, id-1);

        adder = adder + 1;

        printf("rank - %d sending %d to rank %d \n", id, adder, id+1);

        fflush(stdout);

        MPI_Ssend(&adder, 1, MPI_INT, (id + 1), 0, MPI_COMM_WORLD);

    }

    MPI_Finalize();

    return 0;
}