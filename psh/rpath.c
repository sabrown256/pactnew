/*
 * RPATH.C - standalone wrapper for realpath function
 *
 * include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"

extern char
 *realpath(const char *fn, char *rn);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - report usage and options */

static void help(void)
   {

    fprintf(stdout, "\n");
    fprintf(stdout, "Usage: rpath [-h] [-n <nt>] [-t <dt>] [-w] [<fname>]*\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Resolve links to find path to actual file locations.\n");
    fprintf(stdout, "If no files specified query stdin.\n");
    fprintf(stdout, "   h      this help message\n");
    fprintf(stdout, "   n      call realpath nt times or until it returns non-null\n");
    fprintf(stdout, "   t      delay dt seconds between realpath calls\n");
    fprintf(stdout, "   w      do not resolve names from PATH environment variable\n");
    fprintf(stdout, "   fname  file to be resolved\n");
    fprintf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPAND_PATH - print expanded path name to stdout */

static int expand_path(const char *name, int fullp, int nt, int dt, int rv)
   {int i, nc, ok;
    char *fname;
    char pname[PATH_MAX], rname[PATH_MAX+1];

    nc = PATH_MAX - 1;

    ok = 1;
    if (fullp == TRUE)
       {ok = full_path(pname, nc, 1, NULL, name);
	if (ok != 0)
	   rv |= 1;};
    if (ok != 0)
       nstrncpy(pname, PATH_MAX, name, -1);

    fname = NULL;
    for (i = 0; i < nt; i++)
        {fname = realpath(pname, rname);
	 if (fname == NULL)
	    sleep(dt);
	 else
	    break;};
    if (fname == NULL)
       {if (nt > 0)
	   rv |= 2;
	fprintf(stdout, "%s\n", pname);}
    else
       fprintf(stdout, "%s\n", rname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN */

int main(int c, char **v)
   {int i, dt, rv, nt, nc, fullpth;
    char fmt[80];
    char *tname;

    rv      = 0;
    dt      = 0;
    nt      = 1;
    fullpth = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h':
                      help();
                      return(1);

                 case 'n':
                      nt = atoi(v[++i]);
                      break;

                 case 't':
                      dt = atoi(v[++i]);
                      break;

                 case 'w':
                      fullpth = FALSE;
                      break;};}
	 else
	    break;};

    if (i >= c)
       {tname = MAKE_N(char, PATH_MAX);
	nc = PATH_MAX - 1;
	snprintf(fmt, 80, "%%%ds", nc);

        while (scanf(fmt, tname) == 1)
           rv = expand_path(tname, fullpth, nt, dt, rv);

        FREE(tname);}

    else
       for ( ; i < c; i++)
           rv = expand_path(v[i], fullpth, nt, dt, rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
