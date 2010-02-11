
#include <stdio.h>
#include <mpi.h>

int main(int c, char **v)
   {int rank, size;

    MPI_Init(&c, &v);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Hello process %d\n", rank);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return(0);}
