/*
 * GEXEC.C - process group management
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libdb.c"
#include "libpgrp.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_TEST - do the I/O work for the test function */

static int do_test(FILE **fio, char *tag)
   {int i, rv, ev;
    char t[BFLRG];
    char *p;
    static int count = 0;

    rv = 0;

    for (i = 0; rv == 0; i++)
        {if (feof(fio[0]) == TRUE)
	    rv = 1;
	 else
	    {p  = fgets(t, BFLRG, fio[0]);
             ev = errno;
	     if (p != NULL)
	        {count = 0;
		 fprintf(fio[1], "%s> out: %s", tag, p);}
	     else if (feof(fio[0]) == TRUE)
	        rv = 1;
	     else if (ev == EBADF)
	        rv = 2;
	     else
	        {fprintf(fio[1], "%s> gen: %d\n", tag, ++count);
		 rv = (count > 6);};

	     nsleep(10);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STR_TEST - function to test */

static int str_test(char *db, io_mode md, FILE **fio,
		    char *name, int c, char **v)
   {int rv;
    char *tag;

    rv  = 0;
    tag = v[0];

    if (md == IO_MODE_RO)
       block_fd(fileno(stdin), FALSE);

    rv = do_test(fio, tag);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GEXEC_CONV - run conversation between processes
 *            - to test both functions and processes for GEXEC
 */

int gexec_conv(char *db, io_mode md, FILE **fio,
	       char *name, int c, char **v)
   {int rv, nx;
    char s[BFLRG];
    char *p, *side;

    nx = 5;
    rv = 0;
    side = v[0];

    if ((side != NULL) && (md != IO_MODE_NONE))
       {if (strcmp(side, "a") == 0)
	   {static int ia = 0;

	    while (rv == 0)
	       {if (ia <= nx)
		   {fprintf(stderr, "A sent: %d\n", ia);
		    fprintf(fio[1], "%d\n", ia);
		    ia++;};
		p = fgets(s, BFLRG, fio[0]);
		if (p == NULL)
		   break;
		else
		   {fprintf(stderr, "A recv: %s", s);
		    if (strcmp(s, "end\n") == 0)
		       {fprintf(stderr, "Conv side a concluded\n");
			rv = 1;};};};}

	else if (strcmp(side, "b") == 0)
	   {static int ib = 0;

	    for (; (ib < 2*nx) && (rv == 0); )
	        {p = fgets(s, BFLRG, fio[0]);
		 if (p == NULL)
		    break;
		 else
		    {fprintf(stderr, "B recv: %s", s);
		     if (ib >= nx)
		        {fprintf(stderr, "Conv side b concluded\n");
			 fprintf(fio[1], "end\n");
			 rv = 1;}
		     else
		        {fprintf(stderr, "B sent: %c\n", 'a' + ib);
		         fprintf(fio[1], "%c\n", 'a' + ib);
		         ib++;};};};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAPS - map function name to procedure for strong function execution */

static PFPCAL maps(char *s)
   {PFPCAL f;

    f = NULL;

    if (strcmp(s, "var") == 0)
       f = gexec_var;
    else if (strcmp(s, "file") == 0)
       f = gexec_file;
    else if (strcmp(s, "test") == 0)
       f = str_test;
    else if (strcmp(s, "conv") == 0)
       f = gexec_conv;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

static int help(void)
   {int rv;

    printf("\n");
    printf("Usage: gexec [-cev] [-csv] [-d #] [-h] [-nv] [-p] [-s]\n");
    printf("             [-sev] [-ssv] [-st #] [-t] [-to #] <cmd>\n");
    printf("   cev    output status as environment variable in CSH form\n");
    printf("   csv    output status as shell variable in CSH form\n");
    printf("   d      debug level\n");
    printf("   h      this help message\n");
    printf("   nv     do not print output status\n");
    printf("   p      use pipes between processes (default)\n");
    printf("   s      use sockets between processes\n");
    printf("   sev    output status as environment variable in SH form\n");
    printf("   ssv    output status as shell variable in SH form\n");
    printf("   st     set exit status bit mask\n");
    printf("   t      use PTY's between processes (not available yet)\n");
    printf("   to     timeout after # seconds (default -1 infinite)\n");
    printf("   <cmd>  see gexec man page for details\n");
    printf("\n");

    rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v, char **env)
   {int i, rv;
    char *db, *s;
    process_state *ps;

    rv = 0;
    ps = get_process_state();
    if (ps != NULL)
       {db = getenv("PERDB_PATH");
	s  = getenv("GEXEC_DEBUG_LEVEL");

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	ps->dbg_level   = (s == NULL) ? 0 : atol(s);
	ps->status_mask = -1;

	for (i = 1; i < c; i++)
	    {if (strcmp(v[i], "-cev") == 0)
	        ps->ofmt = GEX_CSH_EV;

	     else if (strcmp(v[i], "-csv") == 0)
	        ps->ofmt = GEX_CSH_SV;

	     else if (strcmp(v[i], "-d") == 0)
	        {ps->dbg_level = atol(v[++i]);
		 csetenv("GEXEC_DEBUG_LEVEL", "%d", ps->dbg_level);}

	     else if (strcmp(v[i], "-h") == 0)
	        {rv = help();
		 break;}

	     else if (strcmp(v[i], "-nv") == 0)
	        ps->ofmt = GEX_NONE;

	     else if (strcmp(v[i], "-p") == 0)
	        ps->medium = IO_DEV_PIPE;

	     else if (strcmp(v[i], "-s") == 0)
	        ps->medium = IO_DEV_SOCKET;

	     else if (strcmp(v[i], "-sev") == 0)
	        ps->ofmt = GEX_SH_EV;

	     else if (strcmp(v[i], "-ssv") == 0)
	        ps->ofmt = GEX_SH_SV;

	     else if (strcmp(v[i], "-st") == 0)
	        ps->status_mask = atoll(v[++i]);

	     else if (strcmp(v[i], "-t") == 0)
	        ps->medium = IO_DEV_PTY;

	     else if (strcmp(v[i], "-to") == 0)
	        ps->to_sec = atol(v[++i]);

	     else
	        {rv = gexeca(db, c-i, v+i, env, maps);
		 break;};};};

    log_safe("dump", 0, NULL, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
