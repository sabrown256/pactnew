/*
 * PXRDTS.C - Test attribute features of PDBX (READ version)
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

    PRINT(STDOUT, "\nPDXRDTS - test attribute handling\n\n");
    PRINT(STDOUT, "Usage: pdxrdts [-h] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, ne;
    int *rank, *center;
    float d[10];
    char **names;
    PDBfile *file;
    hasharr *tab;

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

    file = PD_open("xtest.dat", "r");
    if (file == NULL)
       {printf("Error opening xtest.dat\n");
	exit(1);};

/* print contents of symbol table */
    ne    = SC_hasharr_get_n(file->symtab);
    names = SC_hasharr_dump(file->symtab, NULL, NULL, FALSE);
    printf("\nSymbol Table:\n");
    for (i = 0; i < ne; i++)
	if (names[i] != NULL)
	   printf("%s\n", names[i]);
    printf("--------\n");

/* print contents of attribute table */
    printf("\nAttribute Table:\n");
    ne    = SC_hasharr_get_n(file->attrtab);
    names = SC_hasharr_dump(file->attrtab, NULL, NULL, FALSE);
    if (names == NULL)
       printf("Couldn't dump out attribute table\n");
    else
       {for (i = 0; i < ne; i++)
	    if (names[i] != NULL)
	       printf("%s\n", names[i]);};
    printf("--------\n");

/* read the one "physics" variable */
    PD_read(file, "d", d);

    for (i = 0; i < 10; i++)
	printf("d[%d] = %f\n", i, d[i]);
    printf("\n");

/* get some attribute values */
    rank = (int *) PD_get_attribute(file, "d", "rank");
    if (rank == NULL)
       printf("Error getting rank attribute\n");
    printf("rank = %d\n", *rank);

    center = *(int **) PD_get_attribute(file, "d", "centering");
    if (center == NULL)
       printf("Error getting center attribute\n");
    printf("center[0] = %d ; [1] = %d\n", center[0], center[1]);

/* can we read second copy of att hash tab? */
    tab = CMAKE(hasharr);

    if ((PD_read(file, "foo", tab)) == FALSE)
       {printf("Error reading attribute hash table from file.\n");
	printf("PD_err = %s\n", PD_get_error());};

/* print contents of hash tab we just read */
    ne    = SC_hasharr_get_n(tab);
    names = SC_hasharr_dump(tab, NULL, NULL, FALSE);
    if (names == NULL)
       printf("Couldn't dump hash tab from PD_read_table\n");
    else
       {printf("\nAttribute Table from PD_read_table:\n");
	for (i = 0; i < ne; i++)
	    if (names[i] != NULL)
	       printf("%s\n", names[i]);};
    printf("--------\n");

/* close file */
    PD_close(file);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
