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

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDXNTST - test attribute handling\n\n");
    PRINT(STDOUT, "Usage: pdxntst [-h] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, ne;
    int *rank, *nrank, *center, **pc;
    float d[10];
    char **names;
    PDBfile *file;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
		      print_help();
		      return(1);
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    file = PD_open("ntest.dat", "w");
    if (file == NULL)
       {printf("Error creating ntest.dat\n");
	exit(1);};

/* write out one "physics" variable */
    for (i = 0; i < 10; i++)
	d[i] = (float) i;

    PD_write(file, "d(10)", "float", d);

/* define & set some attributes */
    if (PD_def_attribute(file, "rank", "integer") == FALSE)
       printf("Error defining RANK\n");

    rank = CMAKE(int);
    *rank = 1;

    pc = CMAKE(int *);
    *pc = center = CMAKE_N (int, 2);
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
