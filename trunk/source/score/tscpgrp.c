/*
 * TSCPGRP.C - test SC_process_group handling
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CAT_FILE - print the FILE alternating lines between stdout and stderr */

static void cat_file(char *file)
   {int i;
    char *s, **sa;

    sa = SC_file_strings(file);

    for (i = 0; sa[i] != NULL; i++)
        {s = sa[i];
	 if (i & 1)
	    fprintf(stderr, "err(%d)-> %s", i, s);
	 else
	    fprintf(stdout, "out(%d)-> %s", i, s);};

    SC_free_strings(sa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_CMD - run a command for test_exec */

static int do_cmd(int n, char **sa)
   {int i, l, st;
    char *s, **ra;

    st = 0;

    for (i = 0; i < n; i++)
        {s = sa[i];
	 printf("command-> %s\n", s);

	 ra = SC_syscmnd(s);
	 if (ra != NULL)
	    {printf("results:\n");
	     for (l = 0; ra[l] != NULL; l++)
	         printf("%3d> %s", l, ra[l]);}
	 else
	    {printf("failed\n");
	     st |= TRUE;};

	 SC_free_strings(ra);};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_PIPE_1 - test simple pipelines */

static int test_pipe_1(void)
   {int n, st, os;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    os = ps->msh_syntax;
    ps->msh_syntax = TRUE;

    st = 0;

/* simple pipeline variants - all equivalent */
    char *tpipe[] = { "date | cat -n", 
		      "date @ cat -n",
		      "date @o cat -n",
		      "date @o2i cat -n",
		      "date @o+1i cat -n" };
    n = sizeof(tpipe)/sizeof(char *);

    printf("\t\t\tsimple pipes .....\n");

    st = do_cmd(n, tpipe);

    printf("\t\t\tsimple pipes ..... %s\n", (st == 0) ? "ok" : "ng");

    ps->msh_syntax = os;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_PIPE_2 - test more general process groups */

static int test_pipe_2(void)
   {int n, st, os;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    os = ps->msh_syntax;
    ps->msh_syntax = TRUE;

    st = 0;

/* simple pipeline variants - all equivalent */
    char *tpipe[] = { "tscpgrp -f tests/pgrp.dat | cat -E", 
		      "tscpgrp -f tests/pgrp.dat @ cat -E",
		      "tscpgrp -f tests/pgrp.dat @o+1 cat -n > /dev/null",
		      "tscpgrp -f tests/pgrp.dat @e+1 cat -n > /dev/null",
		      "tscpgrp -f tests/pgrp.dat @o+1e+2 cat -n @ cat -E" };

    n = sizeof(tpipe)/sizeof(char *);

    printf("\t\t\tprocess groups .....\n");

    st = do_cmd(n, tpipe);

    printf("\t\t\tprocess groups ..... %s\n", (st == 0) ? "ok" : "ng");

    ps->msh_syntax = os;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_FILE_1 - test simple file variants */

static int test_file_1(void)
   {int n, st, os;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    os = ps->msh_syntax;
    ps->msh_syntax = TRUE;

    st = 0;

/* file variants */
    char *tfile[] = { "pwd >! date.out",
		      "cat < date.out",
                      "pwd @owdate.out",
                      "date @oadate.out",
		      "cat @ifdate.out" };

    n = sizeof(tfile)/sizeof(char *);

    printf("\t\t\tsimple files .....\n");

    st = do_cmd(n, tfile);

    printf("\t\t\tsimple files ..... %s\n", (st == 0) ? "ok" : "ng");

    ps->msh_syntax = os;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test SC_exec_server */

int main(int c, char **v, char **envp)
   {int i, n, err;
    int ton[10];
    char mode[5];

    SC_setbuf(stdout, NULL);

    for (i = 0; i < 10; i++)
        ton[i] = TRUE;

    strcpy(mode, "ap");
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'f' :
		      cat_file(v[++i]);
		      return(0);

                 case 'h' :
		      printf("\nUsage: tscpgrp [-pst] [-f <file>] [-h] [-0] [-1]\n");
		      printf("   Options:\n");
		      printf("      f - cat the file alternating between stdout and stderr\n");
		      printf("      h - print this help message\n");
		      printf("      p - use pipes for communications\n");
		      printf("      s - use sockets for communications\n");
		      printf("      t - use pseudo terminals for communications\n");
		      printf("\n");
		      exit(1);

                 case 'p' :
                      strcpy(mode, "ap");
		      break;
                 case 's' :
                      strcpy(mode, "as");
		      break;
                 case 't' :
                      strcpy(mode, "at");
		      break;
                 default:
                      n = -SC_stoi(v[i]);
		      ton[n] = FALSE;
		      break;};}
         else
            break;};

    err = 0;

    if (ton[0])
       err += test_pipe_1();
    if (ton[1])
       err += test_pipe_2();
    if (ton[2])
       err += test_file_1();

    io_printf(stdout, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
