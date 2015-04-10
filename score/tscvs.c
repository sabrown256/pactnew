/*
 * TSCVS.C - test SC_vsnprintf
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST - test mode */

static int test(int ne, char *fmt, ...)
   {int err, ns;
    char *msg;

    SC_VDSNPRINTF(FALSE, msg, fmt);

    ns  = strlen(msg);
    err = ((msg != NULL) && (ne == ns));

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_HELP - show the usage and options */

static void show_help(void)
   {

    io_printf(stdout, "Usage: tscvs [-h]\n\n");

    io_printf(stdout, "   h      this help message\n");
    io_printf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* MAIN - start here */

int main(int c, char **v)
   {int i, err, rv;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h':
                      show_help();
                      return(1);
		 default:
		      io_printf(stdout, "Unknown option: %s\n", v[i]);
		      return(1);};}
         else
	    break;};


    err = TRUE;

    err &= test(1,  "\n");
    err &= test(2,  "\\n");
    err &= test(1,  "\"");
    err &= test(1,  "%%");
    err &= test(2,  "%%%s", "d");
    err &= test(10, "%-10s", "foo");
    err &= test(10, "%10s", "foo");
    err &= test(13, "%-10s", "foobarbazblat");
    err &= test(13, "%10s", "foobarbazblat");

    rv = (err != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
