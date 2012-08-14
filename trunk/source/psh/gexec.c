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
	    {p  = fgets(t, MAXLINE, fio[0]);
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
/*	     sched_yield(); */

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STR_VAR - function to access variables */

static int str_var(char *db, io_mode md, FILE **fio,
		   char *name, int c, char **v)
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

static int str_file(char *db, io_mode md, FILE **fio,
		    char *name, int c, char **v)
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
			      else if (feof(fio[0]) == TRUE)
				 rv = 1;
			      else if (ev == EBADF)
			         rv = 2;};};

		     fflush(fp);
		     break;

	        default :
		     break;};

/*
	    if (rv == 1)
	       fclose(fp);
*/
	   };};

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

    db = getenv("PERDB_PATH");
    s  = getenv("GEXEC_DEBUG_LEVEL");

    dbg_level = (s == NULL) ? 0 : atol(s);

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {rv = help();
	     break;}

	 else if (strcmp(v[i], "-d") == 0)
            {dbg_level = atol(v[++i]);
             csetenv("GEXEC_DEBUG_LEVEL", "%d", dbg_level);}

	 else
	    {rv = gexec(db, c-i, v+i, env, maps);
	     break;}};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
