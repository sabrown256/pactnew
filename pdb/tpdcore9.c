/*
 * TPDCORE9.C - core PDB test #9
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_9 - test the PDBLib checksum logic */

int test_9(char *base, char *tgt, int n)
   {int err, status, var;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *file;
    FILE *fp;

    err = TRUE;
    var = 42;
  
/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

    if (tgt != NULL)
       snprintf(datfile, MAXLINE, "%s-%s.db9a", base, tgt);
    else
       snprintf(datfile, MAXLINE, "%s-nat.db9a", base);

    if (read_only == FALSE)

/* create the named file */
       {file = PD_create(datfile);
	if (file == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* turn on the checksum logic */
	PD_activate_cksum(file, PD_MD5_FILE);
	PD_write(file, "test", "int", &var);
	PD_close(file);};

/* reopen the file and see if the checksum is valid */
    file = PD_open(datfile, "a");
    if (file == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s reopened\n", datfile);

    PD_activate_cksum(file, PD_MD5_FILE);

/* make sure the checksum checks out */
    status = PD_verify(file);
   
    if (status != TRUE)
       err = FALSE;
    else
       PRINT(fp, "File %s verified\n", datfile);

    PD_close(file);

    if (tgt != NULL)
       snprintf(datfile, MAXLINE, "%s-%s.db9b", base, tgt);
    else
       snprintf(datfile, MAXLINE, "%s-nat.db9b", base);

    if (read_only == FALSE)

/* do it again, except this time, don't activate the checksum
 * create the named file
 */
       {file = PD_create(datfile);
	if (file == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* leave the checksum logic off */
	PD_write(file, "test", "int", &var);
	PD_close(file);};

/* reopen the file and see if the checksum is valid */
    file = PD_open(datfile, "a");
    if (file == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s reopened\n", datfile);

    PD_activate_cksum(file, PD_MD5_FILE);

/* make sure it returns -1 meaning integrity cannot be determined */
    status = PD_verify(file);
   
    if (status != -1)
       err = FALSE;
    else
       PRINT(fp, "File %s cannot be verified (as expected)\n", datfile);

    PD_close(file);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
