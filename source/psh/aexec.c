/*
 * AEXEC.C - process group management
 *
 * #include "cpyright.h"
 *
 */

/* look at process group parsing and I/O connections */
/* #define DEBUG */

/* look at process execution and I/O */
/* #define TRACE */

#include "common.h"
#include "libpsh.c"
#include "libpgrp.c"
#include "libdb.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_FILE - abstract file as process for AEXEC
 *         - mode -r means stdin to file
 *         -      -w means file to stdout
 *         -      -a means stdin to file to stdout
 */

int do_file(char *db, int c, char **v)
   {int i, err_exist, rv;
    io_mode md;
    char t[MAXLINE];
    char *p, *fn;
    FILE *fp;

    rv = -1;
    md = IO_MODE_NONE;
    fn = NULL;

    err_exist = FALSE;

    for (i = 0; i < c; i++)
        {if (strcmp(v[i], "-r") == 0)
	    md = IO_MODE_RO;
	 else if (strcmp(v[i], "-w") == 0)
	    md = err_exist ? IO_MODE_WO : IO_MODE_WD;
	 else if ((strcmp(v[i], "-rw") == 0) || (strcmp(v[i], "-a") == 0))
	    md = IO_MODE_APPEND;
	 else
	    fn = v[i];};

    if ((fn != NULL) && (md != IO_MODE_NONE))
       {rv = 0;

	switch (md)

/* file to stdout */
	   {case IO_MODE_RO :
	         fp = fopen(fn, "r");
		 if (fp == NULL)
		    fprintf(stderr, "Cannot open '%s' for reading\n", fn);
		 else
		    {while (feof(fp) == FALSE)
		        {p = fgets(t, MAXLINE, fp);
			 if (p != NULL)
			    fputs(p, stdout);
			 sched_yield();};

		     fclose(fp);};
		 break;

	    case IO_MODE_WO :
		 if (file_exists(fn) == TRUE)
		    return(-1);

/* stdin to file */
	    case IO_MODE_WD :
		 fp = fopen(fn, "w");
		 if (fp == NULL)
		    fprintf(stderr, "Cannot open '%s' for writing\n", fn);
		 else
		    {while (feof(stdin) == FALSE)
		        {p = fgets(t, MAXLINE, stdin);
			 if (p != NULL)
			    fputs(p, fp);
			 sched_yield();};

		     fclose(fp);};
		 break;

/* stdin append to file */
	    case IO_MODE_APPEND :
		 fp = fopen(fn, "a");
		 if (fp == NULL)
		    fprintf(stderr, "Cannot open '%s' for append\n", fn);
		 else
		    {while (feof(stdin) == FALSE)
		        {p = fgets(t, MAXLINE, stdin);
			 if (p != NULL)
			    fputs(p, fp);
			 sched_yield();};

		     fclose(fp);};
		 break;

	    default :
		 break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_VAR - abstract variable as process for AEXEC
 *        - mode -r means stdin to var
 *        -      -w means var to stdout
 *        -      -a means stdin to var to stdout
 */

int do_var(char *db, int c, char **v)
   {int i, n, nc, ns, rv;
    io_mode md;
    char t[MAXLINE];
    char *p, *vr, *vl, **sa;

    rv = -1;
    vl = NULL;
    sa = NULL;
    md = IO_MODE_NONE;

    for (i = 0; i < c; i++)
        {if (strcmp(v[i], "-r") == 0)
	    md = IO_MODE_RO;
	 else if (strcmp(v[i], "-w") == 0)
	    md = IO_MODE_WO;
	 else if ((strcmp(v[i], "-rw") == 0) || (strcmp(v[i], "-a") == 0))
	    md = IO_MODE_APPEND;
	 else
	    vr = v[i];};

    if ((vr != NULL) && (md != IO_MODE_NONE))
       {vl = getenv(vr);
	if (vl != NULL)
	   sa = lst_add(sa, vl);

	rv = 0;

	switch (md)

/* variable to stdout NC times */
	   {case IO_MODE_RO :
	         nc = 1;
		 ns = lst_length(sa);
		 for (n = 0; n < nc; n++)
		     {for (i = 0; i < ns; i++)
			  printf("%s ", sa[i]);
		      printf("\n");};
		 break;

/* stdin to variable */
	    case IO_MODE_WO :
	         while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 sa = lst_add(sa, t);
		       sched_yield();};};

		 vl = concatenate(t, MAXLINE, sa, " ");
		 if (db == NULL)
		    printf("setenv %s \"%s\" ; ", vr, vl);
		 else
		    dbset(NULL, vr, vl);

		 break;

/* stdin append to variable */
	    case IO_MODE_APPEND :
	         while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 sa = lst_add(sa, t);
			 vl = concatenate(t, MAXLINE, sa, " ");
			 fprintf(stdout, "%s\n", vl);};
		       sched_yield();};

		 if (db == NULL)
		    printf("setenv %s \"%s\" ; ", vr, vl);
		 else
		    dbset(NULL, vr, vl);

		 break;

	    default :
		 break;};

	free_strings(sa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_FNC - abstract function/procedure as process for AEXEC
 *        - mode -r means stdin to fnc
 *        -      -w means fnc to stdout
 *        -      -a means stdin to fnc to stdout
 *        - this is a weak form of implementation because a separate process
 *        - is launched to wrap the function - context for F is lost
 *        - a strong form will involve changing run_pgrp, reconnect_pgrp,
 *        - and parse_pgrp to poll the input descriptors for the function
 *        - also to call the function for output
 */

int do_fnc(char *db, int c, char **v, char *(*f)(char *x))
   {int i, n, nc, rv;
    io_mode md;
    char t[MAXLINE];
    char *p, *r, *fn, *vl, **sa;

    rv = -1;
    vl = NULL;
    sa = NULL;
    md = IO_MODE_NONE;

    for (i = 0; i < c; i++)
        {if (strcmp(v[i], "-w") == 0)
	    md = IO_MODE_RO;
	 else if (strcmp(v[i], "-r") == 0)
	    md = IO_MODE_WO;
	 else if ((strcmp(v[i], "-rw") == 0) || (strcmp(v[i], "-a") == 0))
	    md = IO_MODE_APPEND;
	 else
	    fn = v[i];};

    if ((fn != NULL) && (md != IO_MODE_NONE))
       {

/* real application would map name FN to actual function F to call */
	if (f == NULL)
	   return(rv);

	rv = 0;

	switch (md)
	   {case IO_MODE_RO :
	         while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 r  = f(t);
			 sa = lst_add(sa, r);
		       sched_yield();};};

		 vl = concatenate(t, MAXLINE, sa, " ");
		 printf("> %s\n", vl);

		 break;

	    case IO_MODE_WO :
	         nc = 1;
		 for (n = 0; n < nc; n++)
		     {r = f(NULL);
		      printf("> %s\n", r);};
		 break;

	    case IO_MODE_APPEND :
	         while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 r  = f(t);
			 sa = lst_add(sa, r);
			 vl = concatenate(t, MAXLINE, sa, " ");
			 fprintf(stdout, "%s\n", vl);};
		       sched_yield();};

		 vl = concatenate(t, MAXLINE, sa, " ");
		 printf("> %s\n", vl);

		 break;

	    default :
		 break;};

	free_strings(sa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

int help(void)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TESTF - test function for procedure execution */

char *testf(char *s)
   {static int count = 0;
    static char t[MAXLINE];

    if (s == NULL)
       snprintf(t, MAXLINE, "originate: %d", ++count);
    else
       snprintf(t, MAXLINE, "reply: %s", s);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v, char **env)
   {int i, rv;
    char *db;

#ifdef DEBUG
printf("main> a c = %d\n", c);
#endif

    db = getenv("PERDB_PATH");

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-f") == 0)
            {rv = do_file(db, c-i-1, v+i+1);
	     break;}
         else if (strcmp(v[i], "-h") == 0)
            {rv = help();
	     break;}
	 else if (strcmp(v[i], "-p") == 0)
            {rv = do_fnc(db, c-i-1, v+i+1, testf);
	     break;}
	 else if (strcmp(v[i], "-v") == 0)
            {rv = do_var(db, c-i-1, v+i+1);
	     break;}
	 else
	    {rv = aexec(db, c-1, v+1, env);
	     break;};};

#ifdef DEBUG
printf("main> z\n");
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
