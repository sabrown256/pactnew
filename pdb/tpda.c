/*
 * TPDA.C - test attribute features of PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"
#include "pdbtfr.h"

int
 debug_mode = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_INFO - print attribute table info */

static int print_info(PDBfile *file, FILE *fp, hasharr *tab,
		      char *s, char *var)
   {int i, ne, ret, *nrank, **center;
    char **names;

    ret = TRUE;

    PRINT(fp, "-------------------------------------------------------\n\n");

    PRINT(fp, "Variable: %s\n", var);

/* print contents of attribute table */
    ne    = SC_hasharr_get_n(tab);
    names = SC_hasharr_dump(tab, NULL, NULL, FALSE);
    for (i = 0; i < ne; i++)
        PRINT(fp, "\t%s\n", names[i]);

    SC_free_strings(names);
    PRINT(fp, "\n");

/* get attribute associated with var */
    nrank = (int *) PD_get_attribute(file, s, "rank");
    if (nrank == NULL)
       {error(-1, fp, "Error getting rank attribute\n");
        ret = FALSE;}
    else
       PRINT(fp, "Rank of %s : %d\n", s, *nrank);

    center = (int **) PD_get_attribute(file, s, "centering");
    if (center == NULL)
       {error(-1, fp, "Error getting centering attribute\n");
        ret = FALSE;}
    else
       PRINT(fp, "Centering of %s : %d %d\n", s, center[0][0], center[0][1]);

    PRINT(fp, "\n-------------------------------------------------------\n");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST - write file with attributes */

static int write_test(FILE *fp)
   {int i, err;
    int *rank, *center, **pc;
    float d[10];
    PDBfile *file;

    err = TRUE;

/* write out one variable */
    for (i = 0; i < 10; i++)
        d[i] = (float) i;

    if (read_only == FALSE)
       {file = PD_open("tpda.db", "w");
	if ((file == NULL) || (fp == NULL))
	   {error(-1, fp, "Error creating tpda.db\n");
	    err = FALSE;
	    return(err);};
	PRINT(fp, "\nCreated tpda.db\n\n");

	PD_write(file, "d(10)", "float", d);

/* define & set some attributes */
	if (PD_def_attribute(file, "rank", "integer") == FALSE)
	   {error(-1, fp, "Error defining RANK\n");
	    err = FALSE;};

	if (PD_def_attribute(file, "centering", "integer *") == FALSE)
	   {error(-1, fp, "Error defining CENTERING\n");
	    err = FALSE;};

	rank  = CMAKE(int);
	*rank = 1;
	SC_mark(rank, 1);

	pc  = CMAKE(int *);
	*pc = center = CMAKE_N (int, 2);
	center[0] = 44;
	center[1] = 55;

	if (PD_set_attribute(file, "d", "rank", (void *) rank) == FALSE)
	   {error(-1, fp, "Error setting rank attribute\n");
	    err = FALSE;};

	if (PD_set_attribute(file, "d", "centering", (void *) pc) == FALSE)
	   {error(-1, fp, "Error setting center attribute\n");
	    err = FALSE;};

	err &= print_info(file, fp, file->attrtab, "d", "Attribute Table");

	PD_close(file);

	CFREE(rank);
	CFREE(center);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_TEST - read/write hash table */

static int hash_test(FILE *fp)
   {int err;
    hasharr *tab;
    PDBfile *file;

    err = TRUE;

    file = PD_open("tpda.db", "a");
    PRINT(fp, "\nRe-opened tpda.db\n\n");

    err &= print_info(file, fp, file->attrtab, "d", "Attribute Table");

    if (read_only == FALSE)

/* try writing then reading hash table from file */
       {if (!PD_write(file, "foo", "hasharr", file->attrtab))
	   {error(-1, fp, "Error writing attribute hash table to file\n");
	    error(-1, fp, "PD_err = %s\n", PD_get_error());
	    err = FALSE;};};

    tab = CMAKE(hasharr);

    if (PD_read(file, "foo", tab) == FALSE)
       {error(-1, fp, "Error reading attribute hash table from file\n");
        error(-1, fp, "PD_err = %s\n", PD_get_error());
        err = FALSE;};

/* connect up the hash methods */
    tab->hash = file->attrtab->hash;
    tab->comp = file->attrtab->comp;

    err &= print_info(file, fp, tab, "d", "foo");

    PD_close(file);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

void print_help(void)
   {

    PRINT(STDOUT, "\nTPDA - run PDB attribute tests\n\n");
    PRINT(STDOUT, "Usage: tpda [-d] [-h] [-r] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       r - read only mode (assuming file from previous run)\n");
    PRINT(STDOUT, "       v - use specified format version (default 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, err;
    FILE *fp;
    
    PD_init_threads(1, NULL);

    SC_zero_space_n(1, -2);

    debug_mode = FALSE;
    read_only  = FALSE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'r' :
		      read_only = TRUE;
                      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    err = TRUE;

    fp = io_open("tpda.rs", "w");

    err &= write_test(fp);
    err &= hash_test(fp);

    io_close(fp);

    return(!err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
