/*
 * PDCTRNS.C - convert text files from one standard to another
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONVERT - convert a file FNAME */

static int convert(FILE *fo, char *fname,
		   PD_character_standard istd, PD_character_standard ostd)
   {int cs, nbi, nbo, nca, nci, nco, rv;
    char s[MAXLINE], msg[MAXLINE];
    char *t, *u;
    double time;
    FILE *fi;
    static int debug = 0;

/* NOTE: under the debugger set debug to 1 or 2 for additional
 *       memory leak monitoring
 */
    cs = SC_mem_monitor(-1, debug, "B", msg);

    rv   = FALSE;
    time = SC_wall_clock_time();

    fi = io_open(fname, "r");
    if (fi != NULL)
       {nbi = PD_n_bit_char_std(istd);
	nbo = PD_n_bit_char_std(ostd);

	nca = (MAXLINE*8)/nbi;
	nca = 2*MAXLINE;
	t   = CMAKE_N(char, nca);
	u   = CMAKE_N(char, nca);

	while (io_eof(fi) != TRUE)
           {nci = io_read(s, 1, MAXLINE, fi);

/* convert from ISTD to 7-bit ASCII */
	    nca = (nci*8)/nbi;
	    PD_convert_ascii(t, nca, istd, s, nci);

/* convert from 7-bit ASCII to OSTD */
	    nco = (nca*8)/nbo;
	    PD_conv_from_ascii(u, nco, ostd, t, nca);

	    io_write(u, 1, nco, fo);};

	CFREE(t);
	CFREE(u);

	io_close(fi);

        rv = TRUE;};

    time = SC_wall_clock_time() - time;

    cs = SC_mem_monitor(cs, debug, "B", msg);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDCTRNS - translate text files from one character standard to another\n\n");
    PRINT(STDOUT, "Usage: pdctrns [-f <file>] [-h] [-ie] [-i5] [-i6] [-i7] [-oe] [-o5] [-o6] [-o7] <file>\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       f    output file\n");
    PRINT(STDOUT, "       h    print this help message and exit\n");
    PRINT(STDOUT, "       ie   input file is EBCDIC\n");
    PRINT(STDOUT, "       i5   input file is ITA2\n");
    PRINT(STDOUT, "       i6   input file is 6-bit ASCII\n");
    PRINT(STDOUT, "       i7   input file is 7-bit ASCII\n");
    PRINT(STDOUT, "       oe   convert to EBCDIC\n");
    PRINT(STDOUT, "       o5   convert to ITA2\n");
    PRINT(STDOUT, "       o6   convert to 6-bit ASCII\n");
    PRINT(STDOUT, "       o7   convert to 7-bit ASCII\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - translate text */

int main(int c, char **v)
   {int i, nf, rv;
    PD_character_standard istd, ostd;
    char **fl;
    FILE *fo;

    rv = TRUE;

    nf = 0;
    fl = CMAKE_N(char *, c);

    fo   = stdout;
    istd = PD_ASCII_7;
    ostd = PD_ASCII_7;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
	        {case 'f' :
		      fo = io_open(v[++i], "w");
		      break;
	         case 'i' :
		      switch (v[i][2])
			 {case 'e' :
			       istd = PD_EBCDIC;
			       break;
			  case '5' :
			       istd = PD_ITA2_UPPER;
			       break;
			  case '6' :
			       istd = PD_ASCII_6_UPPER;
			       break;
			  case '7' :
			       istd = PD_ASCII_7;
			       break;};
		      break;
		 case 'h' :
		      print_help();
		      return(1);
	         case 'o' :
		      switch (v[i][2])
			 {case 'e' :
			       ostd = PD_EBCDIC;
			       break;
			  case '5' :
			       ostd = PD_ITA2_UPPER;
			       break;
			  case '6' :
			       ostd = PD_ASCII_6_UPPER;
			       break;
			  case '7' :
			       ostd = PD_ASCII_7;
			       break;};
		      break;};}
	 else
            fl[nf++] = v[i];};

    for (i = 0; i < nf; i++)
        convert(fo, fl[i], istd, ostd);

    if (fo != stdout)
       io_close(fo);

    CFREE(fl);

    rv = (rv == FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
