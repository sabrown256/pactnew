/*
 * TMLCSM.C - test the checksum routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a sample program */

int main(int c, char **v)
   {int i, j, count, timit;
    long mn, mx;
    char *fname, *tname;
    double t0, t1, rate;
    FILE *fp;
    unsigned char chksum[MAXLINE];

    count = 1;
    timit = FALSE;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-r") == 0)
	    count = SC_stoi(v[++i]);

	 else if (strcmp(v[i], "-t") == 0)
	    timit = TRUE;

	 else
	    {fname = v[i]; 

	     fp = io_open(fname, "r");
	     mn = 0;
	     mx = SC_file_length(fname);

	     tname = SC_lasttok(fname, "/");

	     t0 = SC_wall_clock_time();

	     for (j = 0; j < count; j++)
	         PM_md5_checksum_file(fp, mn, mx, chksum);

	     t1 = SC_wall_clock_time() - t0;

	     PRINT(stdout, "%s: %s\n", tname, chksum);

	     if (timit == TRUE)
	        {rate = mx*count;
		 rate /= t1;
		 if (rate > 1.0e9)
		    PRINT(stdout,
			  "Iterations = %d  Time = %.2f  Rate = %.2f GB/sec\n",
			  count, t1, 1.0e-9*rate);
		 else if (rate > 1.0e6)
		    PRINT(stdout,
			  "Iterations = %d  Time = %.2f  Rate = %.2f MB/sec\n",
			  count, t1, 1.0e-6*rate);
		 else if (rate > 1.0e3)
		    PRINT(stdout,
			  "Iterations = %d  Time = %.2f  Rate = %.2f KB/sec\n",
			  count, t1, 1.0e-3*rate);
		 else
		    PRINT(stdout,
			  "Iterations = %d  Time = %.2f  Rate = %.2f By/sec\n",
			  count, t1, rate);};

	     if (fp != NULL)
	        io_close(fp);};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

