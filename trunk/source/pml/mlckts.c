/*
 * MLCKTS.C - test the checksum routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a sample program */

int main(int c, char **v)
   {int i;
    long sum, mn, mx;
    char *fname, *tname;
    FILE *fp;
    unsigned char chksum[MAXLINE];

    fname = "mlcmsh.c";
    for (i = 1; i < c; i++)
        fname = v[i]; 

    fp = io_open(fname, "r");

    tname = SC_lasttok(fname, "/");
    PRINT(stdout, "Checksum results for %s:\n", tname);

    mn = 0;

    for (mx = -1; mx < 2400; mx += 980)
        {PRINT(stdout, "   Address range %ld to %ld:\n", mn, mx);

/* test PM_checksum */
	 for (i = 23; i < 30; i += 2)
	     {sum = PM_checksum_file(fp, mn, mx, i);
	      PRINT(stdout, " %d bit    %ld\n", i, sum);};

/* test PM_md5_checksum */
	 PM_md5_checksum_file(fp, mn, mx, chksum);
	 PRINT(stdout, "    MD5    %s\n", chksum);

	 PRINT(stdout, "\n");};
         
    PRINT(stdout, "Checksum finished\n");

    if (fp != NULL)
       io_close(fp);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

