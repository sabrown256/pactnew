/*
 * TSCPR.C - test the isprint, isspace functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test character predicates */

int main(int c, char **v)
   {int i, n, t1, t2, t3;

    SC_setbuf(stdout, NULL);

/* process the command line arguments */
    t1 = TRUE;
    t2 = TRUE;
    t3 = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case '1' :
		      t1 = FALSE;
		      break;
                 case '2' :
		      t2 = FALSE;
		      break;
                 case '3' :
		      t3 = FALSE;
		      break;

	         case 'h' :
		      printf("\nUsage: tscpr [-1] [-2] [-3] [-h]\n");
		      printf("   Options:\n");
		      printf("      1 - do not do alphanumeric test\n");
		      printf("      2 - do not do punctuation test\n");
		      printf("      3 - do not do whitespace test\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);};}
         else
            break;};

    io_printf(stdout, "\n");

    if (t1)
       {io_printf(stdout, "Alphanumeric characters:\n   ");
	for (n = 0, i = 0; i < 256; i++)
	    {if (SC_isalnum(i) != 0)
	        {io_printf(stdout, "%c", i);
	         n++;};};
	io_printf(stdout, "   [%d]\n\n", n);};

    if (t2)
       {io_printf(stdout, "Punctuation characters:\n   ");
	for (n = 0, i = 0; i < 256; i++)
	    {if (ispunct(i) != 0)
	        {io_printf(stdout, "%c", i);
		 n++;};};
	io_printf(stdout, "   [%d]\n\n", n);};

    if (t3)
       {io_printf(stdout, "Whitespace characters:\n   ");
	for (n = 0, i = 0; i < 256; i++)
	    {if (isspace(i) != 0)
	        {io_printf(stdout, "^%c(0x%02x) ", '@'+i, i);
	         n++;};};
	io_printf(stdout, "   [%d]\n", n);
	io_printf(stdout, "   horizontal tab   \\t ^%c(0x%02x)\n", '@'+'\t', '\t');
	io_printf(stdout, "   newline          \\n ^%c(0x%02x)\n", '@'+'\n', '\n');
	io_printf(stdout, "   vertical tab     \\v ^%c(0x%02x)\n", '@'+'\v', '\v');
	io_printf(stdout, "   form feed        \\f ^%c(0x%02x)\n", '@'+'\f', '\f');
	io_printf(stdout, "   carriage return  \\r ^%c(0x%02x)\n", '@'+'\r', '\r');
	io_printf(stdout, "\n");};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
