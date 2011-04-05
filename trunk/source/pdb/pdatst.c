/*
 * PDATST.C - Test attribute features of PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

static int
 debug_mode = FALSE,
 read_only  = FALSE;

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
       {PRINT(fp, "Error getting rank attribute\n");
        ret = FALSE;}
    else
       PRINT(fp, "Rank of %s : %d\n", s, *nrank);

    center = (int **) PD_get_attribute(file, s, "centering");
    if (center == NULL)
       {PRINT(fp, "Error getting centering attribute\n");
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
       {file = PD_open("pdatst.db", "w");
	if ((file == NULL) || (fp == NULL))
	   {PRINT(stderr, "Error creating pdatst.db\n");
	    err = FALSE;
	    return(err);};
	PRINT(fp, "\nCreated pdatst.db\n\n");

	PD_write(file, "d(10)", "float", d);

/* define & set some attributes */
	if (PD_def_attribute(file, "rank", "integer") == FALSE)
	   {PRINT(fp, "Error defining RANK\n");
	    err = FALSE;};

	if (PD_def_attribute(file, "centering", "integer *") == FALSE)
	   {PRINT(fp, "Error defining CENTERING\n");
	    err = FALSE;};

	rank  = FMAKE(int, "WRITE_TEST:rank");
	*rank = 1;

	pc  = FMAKE(int *, "WRITE_TEST:pc");
	*pc = center = FMAKE_N (int, 2, "WRITE_TEST:*pc");
	center[0] = 44;
	center[1] = 55;

	if (PD_set_attribute(file, "d", "rank", (void *) rank) == FALSE)
	   {PRINT(fp, "Error setting rank attribute\n");
	    err = FALSE;};

	if (PD_set_attribute(file, "d", "centering", (void *) pc) == FALSE)
	   {PRINT(fp, "Error setting center attribute\n");
	    err = FALSE;};

	err &= print_info(file, fp, file->attrtab, "d", "Attribute Table");

	PD_close(file);
	SFREE(rank);
	SFREE(center);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_TEST - read/write hash table */

static int hash_test(FILE *fp)
   {int err;
    hasharr *tab;
    PDBfile *file;

    err = TRUE;

    file = PD_open("pdatst.db", "a");
    PRINT(fp, "\nRe-opened pdatst.db\n\n");

    err &= print_info(file, fp, file->attrtab, "d", "Attribute Table");

    if (read_only == FALSE)

/* try writing then reading hash table from file */
       {if (!PD_write(file, "foo", "hasharr", file->attrtab))
	   {PRINT(fp, "Error writing attribute hash table to file\n");
	    PRINT(fp, "PD_err = %s\n", PD_get_error());
	    err = FALSE;};};

    tab = FMAKE(hasharr, "HASH_TEST:tab");

    if (PD_read(file, "foo", tab) == FALSE)
       {PRINT(fp, "Error reading attribute hash table from file\n");
        PRINT(fp, "PD_err = %s\n", PD_get_error());
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

    PRINT(STDOUT, "\nPDATST - run PDB attribute tests\n\n");
    PRINT(STDOUT, "Usage: pdatst [-d] [-h] [-r] [-v #]\n");
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

    SC_zero_space_n(0, -2);

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
                      PD_set_format_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    err = TRUE;

    fp = io_open("pdatst.rs", "w");

    err &= write_test(fp);
    err &= hash_test(fp);

    io_close(fp);

    return(!err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
