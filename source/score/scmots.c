/*
 * SCMOTS.C - test the malloc override capability
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_mem.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

static void help(void)
   {

    io_printf(STDOUT, "\n");

    io_printf(STDOUT, "Usage: scmots [-h] [-z #]\n");
    io_printf(STDOUT, "   h    - this help message\n");
    io_printf(STDOUT, "   z    - reset memory\n");
    io_printf(STDOUT, "        - 0 none, 1 on both, 2 on alloc, 3 on free\n");

    io_printf(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the malloc override */

int main(int c, char **v)
   {int i, err;
    int zsp;
    char *p;

    zsp = 0;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
		{case 'h' :
                      help();
                      return(1);
		      break;

		 case 'z' :
		      zsp = SC_stoi(v[++i]);
		      break;};};};

    SC_zero_space_n(zsp, -2);

    err = TRUE;
    
    p = malloc(10);
    err &= (p != NULL);

    p = realloc(p, 20);
    err &= (p != NULL);

    free(p);

/* reverse the sense for exit status */
    err = (err != TRUE);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
