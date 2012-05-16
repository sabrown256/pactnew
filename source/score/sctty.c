/*
 * SCTTY.C - work I/O right when doing MPI
 *
 *   char *ctermid(char *ptr);                        
 *   int isatty(int filedes);                         
 *   char *ttyname(int filedes);                      
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"
#include "scope_mpi.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LPRINTF - print formatted output */

int lprintf(FILE *fp, int flag, char *fmt, ...)
   {char *s;

    SC_VDSNPRINTF(TRUE, s, fmt);

    if (fp == stdout)
       {if ((flag < 0) || (SC_gs.comm_rank == flag))
	   SC_mpi_fputs(s, fp);}

    else
       io_puts(s, fp);

    CFREE(s);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CYCLE - do one read/print cycle */

static int cycle(int ne)
   {char s[MAXLINE], emsg[MAXLINE];
    char *p;
    static int begin = TRUE;

    if (begin == TRUE)
       lprintf(stdout, 0, "-> ");

    s[MAXLINE] = '\0';
    p = SC_fgets(s, MAXLINE, stdin);

    if (p != NULL)
       {ne = 0;
	if (begin == TRUE)
	   lprintf(stdout, 0, "Response: ");

	if (strchr(p, '\n') == NULL)
	   {lprintf(stdout, 0, "%s", s);
	    begin = FALSE;}
	else
	   {lprintf(stdout, 0, "%s\n", s);
	    begin = TRUE;};

	if (strncmp(s, "end\n", 4) == 0)
	   {lprintf(stdout, 0, "Done\n");
	    ne = INT_MAX;};}

    else
       {SC_strerror(errno, emsg, MAXLINE);
	lprintf(stdout, 0, "FGETS: %s\n", emsg);

	ne++;};

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, noth, ne;
    int tststdio, quiet;
    char s[MAXLINE+1];
    char *p;

    tststdio = FALSE;
    noth     = TRUE;
    quiet    = FALSE;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
	    {io_printf(stdout, "Usage: sctty [-h] [-o] [-q] [-t]\n");
	     io_printf(stdout, "    h  this help message\n");
	     io_printf(stdout, "    o  run OpenMP threads\n");
	     io_printf(stdout, "    q  quiet mode\n");
	     io_printf(stdout, "    t  test whether all processes get stdin\n");
	     return(1);}
	 else if (strcmp(v[i], "-o") == 0)
	    noth = FALSE;
	 else if (strcmp(v[i], "-q") == 0)
             quiet = TRUE;
	 else if (strcmp(v[i], "-t") == 0)
             tststdio = TRUE;};

    if (noth == TRUE)
       _SC_dethread();

#ifdef HAVE_MPI
    MPI_Init(&c, &v);
    MPI_Comm_rank(MPI_COMM_WORLD, &SC_gs.comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &SC_gs.comm_size);
#endif

    SC_setbuf(stdout, NULL);

    if (tststdio == TRUE)
       {SC_unblock_file(stdin);
	p = SC_fgets(s, MAXLINE, stdin);
	if (p != NULL)
	   lprintf(stdout, -1, "Proc %d : %s\n", SC_gs.comm_rank, s);
	SC_block_file(stdin);}

    else
       {if (quiet == FALSE)
	   lprintf(stdout, 0, "Testing tty I/O");

#ifdef HAVE_MPI_STDIN_ALL

/* all processes get the input */
	if (quiet == FALSE)
	   lprintf(stdout, 0, " (all processes get input)\n");

	for (ne = -1; ne < 4; )
	    ne = cycle(ne);

#else
	if (quiet == FALSE)
	   lprintf(stdout, 0, " (only master process gets input)\n");

/* only process 0 gets the input */
	if (SC_gs.comm_rank == 0)
	   {for (ne = -1; ne < 4; )
	        ne = cycle(ne);};

# ifdef HAVE_MPI
	MPI_Barrier(MPI_COMM_WORLD);
# endif

#endif

	lprintf(stdout, 0, "\n");};

#ifdef HAVE_MPI
    MPI_Finalize();
#endif

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
