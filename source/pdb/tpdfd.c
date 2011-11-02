/*
 * TPDFD.C - test PDB for file descriptor leaks
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_FD - check the descriptors
 *          - they should all be the same
 */

static int check_fd(int n, int *fd)
   {int i, ok;

    ok = TRUE;

    for (i = 1; i < n; i++)
        ok &= (fd[i] == fd[0]);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEXT_FILE - return TRUE iff PD_open a text file releases all descriptors */

static int text_file(void)
   {int n, ok;
    int fd[10];
    FILE *fp;
    PDBfile *pf;

    n = 0;

/* create a text file */
    fp  = fopen("fd.dat", "w");
    fd[n++] = fileno(fp);
    fprintf(fp, "text\n");
    fclose(fp);

/* check reading the text file */
    fp = fopen("fd.dat", "r");
    fd[n++] = fileno(fp);
    fclose(fp);

/* check append mode */
    pf = PD_open("fd.dat", "a");
    SC_ASSERT(pf != NULL);
    
    fp = fopen("fd.dat", "r");
    fd[n++] = fileno(fp);
    fclose(fp);

    ok = PD_isfile("fd.dat");

/* check read only mode */
    pf = PD_open("fd.dat", "r");
    SC_ASSERT(pf != NULL);

    fp = fopen("fd.dat", "r");
    fd[n++] = fileno(fp);
    fclose(fp);

    unlink("fd.dat");

    ok = check_fd(n, fd);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PDB_FILE - return TRUE iff PD_open a PDB file releases all descriptors */

static int pdb_file(void)
   {int n, ok;
    int fd[10];
    FILE *fp;
    PDBfile *pf;

    n = 0;

/* create a text file */
    pf = PD_open("fd.dat", "w");
    PD_write(pf, "foo", "int", &n);
    PD_close(pf);

/* check reading the text file */
    fp = fopen("fd.dat", "r");
    fd[n++] = fileno(fp);
    fclose(fp);

    ok = PD_isfile("fd.dat");

/* check append mode */
    pf = PD_open("fd.dat", "a");
    PD_close(pf);

/* check read only mode */
    pf = PD_open("fd.dat", "r");
    PD_close(pf);

/* check reading the text file */
    fp = fopen("fd.dat", "r");
    fd[n++] = fileno(fp);
    fclose(fp);

    unlink("fd.dat");

    ok = check_fd(n, fd);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "Usage: tpdfd [-h]\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test PDB file descriptor usage */

int main(int c, char **v)
   {int i, err;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
	    {print_help();
	     return(1);};};

    err = TRUE;

    err &= text_file();
    err &= pdb_file();

    err = (err != TRUE);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
