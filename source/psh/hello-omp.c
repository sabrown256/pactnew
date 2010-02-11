
#include <stdio.h>
#include <omp.h>

int main(int c, char **v)
   {int nt;

    nt = 2;

    omp_set_num_threads(nt);

#pragma omp parallel
    printf("Hello thread %d\n", omp_get_thread_num());

    return(0);}
