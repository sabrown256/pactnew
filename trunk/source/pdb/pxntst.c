/*
 * PXNTST.C - Test attribute features of PDBX
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, ne;
    int *rank, *nrank, *center, **pc;
    float d[10];
    char **names;
    PDBfile *file;
    hasharr tab;

    if ((file = PD_open("ntest.dat", "w")) == NULL)
       {printf("Error creating ntest.dat\n");
	exit(1);};

/* write out one "physics" variable */
    for (i = 0; i < 10; i++)
	d[i] = (float) i;

    PD_write(file, "d(10)", "float", d);

/* define & set some attributes */
    if (PD_def_attribute(file, "rank", "integer") == FALSE)
       printf("Error defining RANK\n");

    rank = FMAKE(int, "PXNTST.C:rank");
    *rank = 1;

    pc = FMAKE(int *, "PXNTST.C:pc");
    *pc = center = FMAKE_N (int, 2, "PXNTST.C:*pc");
    center[0] = 44;
    center[1] = 55;

    if (PD_set_attribute(file, "d", "rank", (void *) rank) == FALSE)
       printf("Error setting rank attribute\n");

/* print contents of attribute table */
    ne    = SC_hasharr_get_n(file->attrtab);
    names = SC_hasharr_dump(file->attrtab, NULL, NULL, FALSE);
    printf("\nAttribute Table:\n");
    for (i = 0; i < ne; i++)
	printf("%s\n", names[i]);
    printf("--------\n");

    if ((nrank = (int *) PD_get_attribute(file, "d", "rank")) == NULL)
       printf("Error getting rank attribute\n");
    printf("Rank of d : %d\n", *nrank);

/* try writing then reading hash table from file */
    if (!PD_write(file, "foo", "hasharr", file->attrtab))
       printf("PD_err = %s\n", PD_get_error());

/* close file */
    PD_close(file);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
