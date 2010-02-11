/*
 * SCFCSC.C - scan file container
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - scan file container */

int main(int c, char **v)
   {int i, full;
    char fcn[MAXLINE], outf[MAXLINE];
    fcontainer *fcntnr;
    SC_file_type type;
    FILE *f;

    f    = stdout;
    type = SC_UNKNOWN;
    full = FALSE;

    SC_setbuf(stdout, NULL);
    fcn[0]  = '\0';
    outf[0] = '\0';

/* process the command line arguments */
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'a' :
		      full = TRUE;
		      break;

                 case 'f' :
		      strcpy(outf, v[++i]);
		      break;

	         case 'h' :
		      printf("\nUsage: fcscan [-a] [-f <outfile>] [-t <type>]\n");
		      printf("   Options:\n");
		      printf("      a - full file info\n");
		      printf("      f - write output to outfile, default is stdout\n");
		      printf("      t - type of container file, either TAR or AR, default is TAR\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);

		 case 't' :
		      if (strcmp(SC_str_upper(v[++i]),"AR") == 0)
                         type = SC_AR;
		      break;};}
         else
            strcpy(fcn, v[i]);};

    if (strlen(fcn) == 0)
       {io_printf(stdout, "No fcontainer specified\n");
        exit(1);};

    if (strlen(outf) != 0)
        {f = io_open(outf, "w");
	 if (f == NULL)
            {io_printf(stdout, "Couldn't create output file writing to stdout\n");
             f = stdout;};};

    fcntnr = SC_open_fcontainer(fcn, type);
    if (fcntnr == NULL)
       {io_printf(stdout, "Error opening fcontainer %s\n", fcn);
        exit(1);}

    SC_describe_fcontainer(f, fcntnr, full);

    SC_close_fcontainer(fcntnr);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

