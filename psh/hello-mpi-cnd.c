
#include <stdio.h>
#include <mpi.h>

int main(int c, char **v)
   {int dp, rank, size;

    dp = (strcmp(v[1], "yes") == 0);
    if (dp)
       {MPI_Init(&c, &v);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);}
    else
       rank = -1;

    printf("Hello process %d\n", rank);

    if (dp)
       {MPI_Barrier(MPI_COMM_WORLD);
        MPI_Finalize();};

    return(0);}
