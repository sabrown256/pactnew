/*
 * OSRC.C - reliable output source for testing
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CAT - behave like cat -n only print TAG instead
 *     - return TRUE if any lines were processed
 */

static int cat(char *tag, int nc, int ne, int no)
   {int i, rv;
    char s[MAXLINE];
    char *p;

    for (i = 0; TRUE; i++)
        {p = fgets(s, MAXLINE, stdin);
	 if (p != NULL)
	    fprintf(stdout, "%s> %s", tag, s);
	 else
	    break;};

    rv = (i > 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GEN - generate messages for stdout and stderr
 *     - return TRUE if nothing was written to stderr
 */

static int gen(char *tag, int nc, int ne, int no)
   {int ic, io, ie, lo, le, rv;

    lo = 1;
    le = 1;

    for (ic = 0; ic < nc; ic++)
        {for (io = 0; io < no; io++)
	     fprintf(stdout, "%s> out message #%d\n", tag, lo++);
	 for (ie = 0; ie < ne; ie++)
	     fprintf(stderr, "%s> err message #%d\n", tag, le++);};

    rv = (ne == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, nc, ne, no, rv;
    char *tag;

    nc = 2;
    ne = 1;
    no = 2;
    tag = "";

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
                {case 'c' :
                      nc = atol(v[++i]);
                      break;
                 case 'e' :
                      ne = atol(v[++i]);
		      break;
		 case 'o' :
                      no = atol(v[++i]);
		      break;
		 case 't' :
                      tag = v[++i];
                      break;};}
	  else if (strcmp(v[i], "cat") == 0)
	     rv = cat(tag, nc, ne, no);
	  else
	     rv = gen(tag, nc, ne, no);};

/* reverse the sense of the exit status */
    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
