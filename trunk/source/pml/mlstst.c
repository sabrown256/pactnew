/*
 * MLSTST.C - test the topological sort routine
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

#define NDEP 10
#define NPTS  9

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, rel[20], *ord;

    rel[0] = 9;
    rel[1] = 2;

    rel[2] = 3;
    rel[3] = 7;

    rel[4] = 7;
    rel[5] = 5;

    rel[6] = 5;
    rel[7] = 8;

    rel[8] = 8;
    rel[9] = 6;

    rel[10] = 4;
    rel[11] = 6;

    rel[12] = 1;
    rel[13] = 3;

    rel[14] = 7;
    rel[15] = 4;

    rel[16] = 9;
    rel[17] = 5;

    rel[18] = 2;
    rel[19] = 8;

    printf("\nRelations List:\n");
    for (i = 0; i < 2*NDEP; i += 2)
        printf("%2d < %2d\n", rel[i], rel[i+1]);
    printf("\n");

    ord = PM_t_sort(rel, NDEP, NPTS, NULL);

    printf("Partially ordered List:\n");
    for (i = 0; i < NPTS; i++)
        printf("A(%2d) = %2d\n", i, ord[i]);

    printf("\nThe correct order is:\n 1 9 3 2 7 4 5 8 6\n\n");

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
