/*
 * AEXEC.C - process group management
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libpgrp.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_FILE - abstract file as process for AEXEC */

int do_file(int c, char **v)
   {int i, rv;
    io_mode md;
    char t[MAXLINE];
    char *p, *fn;
    FILE *fp;

    rv = -1;
    md = IO_MODE_NONE;
    fn = NULL;

    for (i = 0; i < c; i++)
        {if (strcmp(v[i], "-r") == 0)
	    md = IO_MODE_RO;
	 else if (strcmp(v[i], "-wo") == 0)
	    md = IO_MODE_WO;
	 else if (strcmp(v[i], "-wd") == 0)
	    md = IO_MODE_WD;
	 else if (strcmp(v[i], "-a") == 0)
	    md = IO_MODE_APPEND;
	 else
	    fn = v[i];};

    if ((fn != NULL) && (md != IO_MODE_NONE))
       {rv = 0;

	switch (md)
	   {case IO_MODE_RO :
	         fp = fopen(fn, "r");
		 while (feof(fp) == FALSE)
		    {p = fgets(t, MAXLINE, fp);
		     if (p != NULL)
		        fputs(p, stdout);};

		 fclose(fp);
		 break;

	    case IO_MODE_WO :
		 if (file_exists(fn) == TRUE)
		    return(-1);

	    case IO_MODE_WD :
		 fp = fopen(fn, "w");
		 while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        fputs(p, fp);};

		 fclose(fp);
		 break;

	    case IO_MODE_APPEND :
		 fp = fopen(fn, "a");
		 while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        fputs(p, fp);};

		 fclose(fp);
		 break;

	    default :
		 break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_VAR - abstract variable as process for AEXEC */

int do_var(int c, char **v)
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
	   {case IO_MODE_RO :
	         while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 sa = lst_add(sa, t);};};

		 vl = concatenate(t, MAXLINE, sa, " ");
		 printf("setenv %s \"%s\"\n", vr, vl);

		 break;

	    case IO_MODE_WO :
	         nc = 1;
		 ns = lst_length(sa);
		 for (n = 0; n < nc; n++)
		     {for (i = 0; i < ns; i++)
			  printf("%s ", sa[i]);
		      printf("\n");};
		 break;

	    case IO_MODE_APPEND :
	         while (feof(stdin) == FALSE)
		    {p = fgets(t, MAXLINE, stdin);
		     if (p != NULL)
		        {LAST_CHAR(t) = '\0';
			 sa = lst_add(sa, t);
			 vl = concatenate(t, MAXLINE, sa, " ");
			 fprintf(stdout, "%s\n", vl);};};

		 fprintf(stdout, "setenv %s \"%s\"\n", vr, vl);

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

/* MAIN - start here */

int main(int c, char **v, char **env)
   {int i, rv;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-f") == 0)
            {rv = do_file(c-i-1, v+i+1);
	     break;}
         else if (strcmp(v[i], "-h") == 0)
            {rv = help();
	     break;}
	 else if (strcmp(v[i], "-v") == 0)
            {rv = do_var(c-i-1, v+i+1);
	     break;}
	 else
	    {rv = aexec(c-1, v+1, env);
	     break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
