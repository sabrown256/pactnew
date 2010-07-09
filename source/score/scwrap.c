/*
 * SCWRAP.C - really have a chat with MPI
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#if !defined(IBM_BGP) && !defined(IBM_BGL)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - show the usage and options */

static void help(void)
   {

    io_printf(stdout, "\n");
    io_printf(stdout, "Usage: mpi-io-wrap [-b] [-c p | s | t] [-h] [-l] [-s] [-t]\n");
    io_printf(stdout, "   b    - use non-blocking input\n");
    io_printf(stdout, "   c    - communicate with child via pipe, socket, or PTY\n");
    io_printf(stdout, "        - p, s, or t respectively (default is p)\n");
    io_printf(stdout, "   h    - this help message\n");
    io_printf(stdout, "   l    - run in line mode (not recommended)\n");
    io_printf(stdout, "   s    - suppress messages from MPI front end\n");
    io_printf(stdout, "   t    - do not tag all messages\n");
    io_printf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here
 *      - usage:  mpi-io-wrap <program>
 */

int main(int c, char **v)
   {int i, rv, flags, comm;
    int use_tag, line_mode, blocking, suppress;
    char mode[10];
    char **argl;

    SC_single_thread();

    comm      = 'p';
    use_tag   = TRUE;
    line_mode = TRUE;
    blocking  = TRUE;
    suppress  = FALSE;

    for (i = 1; i < c; i++)
	{if (strcmp(v[i], "-b") == 0)
	    blocking = FALSE;
	 else if (strcmp(v[i], "-c") == 0)
	    comm = v[++i][0];
	 else if (strcmp(v[i], "-h") == 0)
	    {help();
	     return(1);}
	 else if (strcmp(v[i], "-l") == 0)
	    line_mode = FALSE;
	 else if (strcmp(v[i], "-s") == 0)
	    suppress = TRUE;
	 else if (strcmp(v[i], "-t") == 0)
	    use_tag = FALSE;
	 else
	    break;};
	 
    argl = v + i;

/* to cause untagged messages to be suppressed set the variable */
    if (suppress == TRUE)
       rv = setenv("SC_MPI_SUPPRESS_UNTAGGED", "TRUE", TRUE);

/* supply the value of the tag appened to messages with the variable */
    if (use_tag == TRUE)
       rv = setenv("SC_MPI_TAG_IO", "+SC_MPI_IO+", TRUE);

    flags = 2*blocking;

#ifdef HAVE_BAD_MPI_IO
    flags |= line_mode;
#endif

    switch (comm)
       {case 'p' :
        default :
             strcpy(mode, "apo");
	     break;
	case 's' :
             strcpy(mode, "aso");
	     break;
	case 't' :
             strcpy(mode, "aTo");
	     break;};

    rv = SC_exec_job(argl, mode, flags, NULL, NULL, NULL, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - stub here */

int main(int c, char **v)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

