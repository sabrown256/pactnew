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

int main(int c, char **v)
   {int i, ic, ie, io, lo, le, nc, ne, no, rv;
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
                      break;};};};

    lo = 1;
    le = 1;

    for (ic = 0; ic < nc; ic++)
        {for (io = 0; io < no; io++)
	     fprintf(stdout, "%s> out message #%d\n", tag, lo++);
	 for (ie = 0; ie < ne; ie++)
	     fprintf(stderr, "%s> err message #%d\n", tag, le++);};

    rv = (ne != 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
