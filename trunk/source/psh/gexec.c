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
   {int i, rv;
    char t[MAXLINE];
    char *p;
    static int count = 0;

    rv = 0;

/*
printf("test> (%d,%d,%d)  stdin=%d  stdout=%d  stderr=%d\n",
       fileno(fio[0]), fileno(fio[1]), fileno(fio[2]),
       fileno(stdin), fileno(stdout), fileno(stderr));
*/

    for (i = 0; rv == 0; i++)
        {if (feof(fio[0]) == TRUE)
	    rv = 1;
	 else
	    {p = fgets(t, MAXLINE, fio[0]);
	     if (p != NULL)
	        {count = 0;
		 fprintf(fio[1], "%s> out: %s", tag, p);}
	     else
	        {fprintf(fio[1], "%s> gen: %d\n", tag, ++count);
		 rv = (count > 6);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_FNC - abstract function/procedure as process for GEXEC
 *        - mode -r means stdin to fnc
 *        -      -w means fnc to stdout
 *        -      -a means stdin to fnc to stdout
 *        - this is a weak form of implementation because a separate process
 *        - is launched to wrap the function - context for F is lost
 *        - a strong form will involve changing run_pgrp, reconnect_pgrp,
 *        - and parse_pgrp to poll the input descriptors for the function
 *        - also to call the function for output
 */

static int do_fnc(char *db, int c, char **v, PFPCAL (*map)(char *x))
   {int i, rv, err_exist;
    io_mode md;
    char *fn;
    FILE *fio[2];
    PFPCAL f;

    rv = -1;
    md = IO_MODE_NONE;

    err_exist = FALSE;

    for (i = 0; i < 2; i++)
        {if (strcmp(v[i], "-r") == 0)
	    md = IO_MODE_RO;
	 else if (strcmp(v[i], "-w") == 0)
	    md = err_exist ? IO_MODE_WO : IO_MODE_WD;
	 else if ((strcmp(v[i], "-rw") == 0) || (strcmp(v[i], "-a") == 0))
	    md = IO_MODE_APPEND;
	 else
	    fn = v[i];};

    if ((fn != NULL) && (md != IO_MODE_NONE))
       {c -= 2;
	v += 2;

/* real application would map name FN to actual function F to call */
	if (map != NULL)
	   {f = map(fn);
	    if (f != NULL)
	       {fio[0] = stdin;
		fio[1] = stdout;

		rv = f(db, md, fio, c, v);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STR_VAR - function to access variables */

static int str_var(char *db, io_mode md, FILE **fio, int c, char **v)
   {int i, nc, ns, rv;
    char t[MAXLINE];
    char *p, *vr, *vl, **sa;
    static int ic = 0;

    rv = 0;
    vr = v[0];
    nc = (c > 1) ? atol(v[1]) : 1;

    if ((vr != NULL) && (md != IO_MODE_NONE))
       {sa = NULL;

        vl = getenv(vr);
	if (vl != NULL)
	   sa = lst_add(sa, vl);

	switch (md)

/* variable to stdout NC times */
	   {case IO_MODE_RO :
		 ns = lst_length(sa);
		 for (i = 0; i < ns; i++)
		     fprintf(fio[1], "%s ", sa[i]);
		 fprintf(fio[1], "\n");
		 ic++;
		 if (ic >= nc)
		    rv = 1;
		 break;

/* stdin to variable */
	    case IO_MODE_WO :
	    case IO_MODE_WD :
	         for (i = 0; rv == 0; i++)
		     {if (feof(fio[0]) == TRUE)
			 rv = 1;
		      else
			 {p = fgets(t, MAXLINE, fio[0]);
			  if (p != NULL)
			     {LAST_CHAR(t) = '\0';
			      sa = lst_add(sa, t);}
			  else if (errno == EBADF)
			     rv = 1;};};

		 vl = concatenate(t, MAXLINE, sa, " ");

		 if (IS_NULL(db) == FALSE)
		    dbset(NULL, vr, vl);
		 else
		    fprintf(fio[1], "setenv %s \"%s\" ; ", vr, vl);

		 break;

/* stdin append to variable */
	    case IO_MODE_APPEND :
	         for (i = 0; rv == 0; i++)
		     {if (feof(fio[0]) == TRUE)
			 rv = 1;
		      else
			 {p = fgets(t, MAXLINE, fio[0]);
			  if (p != NULL)
			     {LAST_CHAR(t) = '\0';
			      sa = lst_add(sa, t);
			      vl = concatenate(t, MAXLINE, sa, " ");
			      fprintf(fio[1], "%s\n", vl);}
			  else if (errno == EBADF)
			     rv = 1;};};

		 if (IS_NULL(db) == FALSE)
		    dbset(NULL, vr, vl);
		 else
		    fprintf(fio[1], "setenv %s \"%s\" ; ", vr, vl);

		 break;

	    default :
		 break;};

	free_strings(sa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STR_FILE - function to access files */

static int str_file(char *db, io_mode md, FILE **fio, int c, char **v)
   {int i, rv, ev;
    char t[MAXLINE];
    char *p, *fn;
    static FILE *fp = NULL;

    rv = 0;
    fn = v[0];

    if ((fn != NULL) && (md != IO_MODE_NONE))
       {if (fp == NULL)

/* open the file */
	   {switch (md)
	       {case IO_MODE_RO :
		     fp = fopen(fn, "r");
		     if (fp == NULL)
		        fprintf(stderr, "Cannot open '%s' for reading\n", fn);
		     break;

	        case IO_MODE_WO :
		     if (file_exists(fn) == TRUE)
		        return(-1);

	        case IO_MODE_WD :
		     fp = fopen(fn, "w");
		     if (fp == NULL)
		        fprintf(stderr, "Cannot open '%s' for writing\n", fn);
		     break;

	        case IO_MODE_APPEND :
		     fp = fopen(fn, "a");
		     if (fp == NULL)
		        fprintf(stderr, "Cannot open '%s' for append\n", fn);
		     break;

	       default :
		     break;};};

	if (fp != NULL)

/* do the I/O */
	   {switch (md)

/* file to stdout */
	       {case IO_MODE_RO :
		     for (i = 0; rv == 0; i++)
		         {if (feof(fp) == TRUE)
			     rv = 1;
			  else
			     {p = fgets(t, MAXLINE, fp);
			      if (p != NULL)
				 fputs(p, fio[1]);};};
		     break;

/* stdin to file */
	        case IO_MODE_WO :
	        case IO_MODE_WD :
	        case IO_MODE_APPEND :
		     for (i = 0; rv == 0; i++)
		         {if (feof(fio[0]) == TRUE)
			     rv = 1;
			  else
			     {p  = fgets(t, MAXLINE, fio[0]);
			      ev = errno;
			      if (p != NULL)
				 fputs(p, fp);
			      else if (ev == EBADF)
			         rv = 2;};};

		     fflush(fp);
		     break;

	        default :
		     break;};

	    if (rv == 1)
	       fclose(fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STR_TEST - function to test */

static int str_test(char *db, io_mode md, FILE **fio, int c, char **v)
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

/* MAPS - map function name to procedure for strong function execution */

static PFPCAL maps(char *s)
   {PFPCAL f;

    if (strcmp(s, "var") == 0)
       f = str_var;
    else if (strcmp(s, "file") == 0)
       f = str_file;
    else if (strcmp(s, "test") == 0)
       f = str_test;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXE_VAR - function to access variables */

static int exe_var(char *db, io_mode md, FILE **fio, int c, char **v)
   {int i, n, nc, ns, rv;
    char t[MAXLINE];
    char *p, *vr, *vl, **sa;

    rv = 0;
    vr = v[0];

    if ((vr != NULL) && (md != IO_MODE_NONE))
       {sa = NULL;

        vl = getenv(vr);
	if (vl != NULL)
	   sa = lst_add(sa, vl);

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
	    case IO_MODE_WD :
	         while (feof(fio[0]) == FALSE)
		    {p = fgets(t, MAXLINE, fio[0]);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 sa = lst_add(sa, t);
		       sched_yield();};};

		 vl = concatenate(t, MAXLINE, sa, " ");

		 if (IS_NULL(db) == FALSE)
		    dbset(NULL, vr, vl);
		 else
		    printf("setenv %s \"%s\" ; ", vr, vl);

		 break;

/* stdin append to variable */
	    case IO_MODE_APPEND :
	         while (feof(fio[0]) == FALSE)
		    {p = fgets(t, MAXLINE, fio[0]);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 sa = lst_add(sa, t);
			 vl = concatenate(t, MAXLINE, sa, " ");
			 fprintf(fio[1], "%s\n", vl);};
		       sched_yield();};

		 if (IS_NULL(db) == FALSE)
		    dbset(NULL, vr, vl);
		 else
		    printf("setenv %s \"%s\" ; ", vr, vl);

		 break;

	    default :
		 break;};

	free_strings(sa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXE_FILE - function to access files */

static int exe_file(char *db, io_mode md, FILE **fio, int c, char **v)
   {int rv;
    char t[MAXLINE];
    char *p, *fn;
    FILE *fp;

    rv = 0;
    fn = v[0];

    if ((fn != NULL) && (md != IO_MODE_NONE))
       {switch (md)

/* file to stdout */
	   {case IO_MODE_RO :
	         fp = fopen(fn, "r");
		 if (fp == NULL)
		    fprintf(stderr, "Cannot open '%s' for reading\n", fn);
		 else
		    {while (feof(fp) == FALSE)
		        {p = fgets(t, MAXLINE, fp);
			 if (p != NULL)
			    fputs(p, fio[1]);
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
		    {while (feof(fio[0]) == FALSE)
		        {p = fgets(t, MAXLINE, fio[0]);
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
		    {while (feof(fio[0]) == FALSE)
		        {p = fgets(t, MAXLINE, fio[0]);
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

/* EXE_TEST - function to test */

static int exe_test(char *db, io_mode md, FILE **fio, int c, char **v)
   {int rv;
    char *tag;
    static int count = 0;

    rv  = 0;
    tag = v[0];

    switch (md)
       {case IO_MODE_RO :
	     fprintf(fio[1], "%s> gen: %d\n", tag, ++count);
	     break;
        default :
	     rv = do_test(fio, tag);
	     rv = 0;
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAPF - map function name to procedure for execution */

static PFPCAL mapf(char *s)
   {PFPCAL f;

    if (strcmp(s, "var") == 0)
       f = exe_var;
    else if (strcmp(s, "file") == 0)
       f = exe_file;
    else if (strcmp(s, "test") == 0)
       f = exe_test;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

static int help(void)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v, char **env)
   {int i, rv;
    char *db, *s;

    db = cgetenv(TRUE, "PERDB_PATH");
    s  = cgetenv(TRUE, "GEXEC_DEBUG_LEVEL");

    dbg_level = (s == NULL) ? 0 : atol(s);

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {rv = help();
	     break;}

	 else if (strcmp(v[i], "-d") == 0)
            {dbg_level = atol(v[++i]);
             csetenv("GEXEC_DEBUG_LEVEL", "%d", dbg_level);}

	 else if (strcmp(v[i], "-s") == 0)
            strong_functions = TRUE;

	 else if (strong_functions == TRUE)
	    {rv = gexec(db, c-i, v+i, env, maps);
	     break;}

	 else if (strcmp(v[i], "-p") == 0)
            {rv = do_fnc(db, c-i-1, v+i+1, mapf);
	     break;}
	 else
            {rv = gexec(db, c-i, v+i, env, NULL);
	     break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
