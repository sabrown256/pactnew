/*
 * TSCMO.C - test the malloc override capability
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_mem.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - simplest test of malloc override */

static int test_1(int n, double *pdt)
   {int rv;
    char *p;
    double t0;

    rv = TRUE;

    t0 = SC_wall_clock_time();

    p = malloc(10);
    rv &= (p != NULL);

    p = realloc(p, 20);
    rv &= (p != NULL);

    free(p);

    *pdt = SC_wall_clock_time() - t0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - time malloc overrides */

static int test_2(int n, double *pdt)
   {int i, nb, rv;
    char **p;
    double t0;
    FILE *fp;

    rv = TRUE;

    p = CMAKE_N(char *, n);

    t0 = SC_wall_clock_time();

    for (i = 0; i < n; i++)
        {nb   = 10 + i*10000/n;
	 p[i] = malloc(nb);};

    *pdt = (SC_wall_clock_time() - t0)/((double) n);

    fp = fopen("test_2.map", "w");
    SC_mem_map(fp, 15);
    fclose(fp);

    for (i = 0; i < n; i++)
        free(p[i]);

    CFREE(p);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - time free overrides */

static int test_3(int n, double *pdt)
   {int i, rv;
    char **p;
    double t0;

    rv = TRUE;

    p = CMAKE_N(char *, n);

    for (i = 0; i < n; i++)
        p[i] = malloc(10);

    t0 = SC_wall_clock_time();

    for (i = 0; i < n; i++)
        free(p[i]);

    *pdt = (SC_wall_clock_time() - t0)/((double) n);

    CFREE(p);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - wrap a test in standard setup and checking infrastructure */

static int run_test(char *name, int n, int (*tst)(int n, double *pdt))
   {int rv, ln, nc, cs;
    char msg[MAXLINE], s[MAXLINE];
    double dt;
    static int dbg = 2;
    static int map = FALSE;

    if (map == TRUE)
       cs = SC_mem_monitor(-1, dbg, "MOR", msg);

    ln = 30;
    snprintf(s, MAXLINE, "\t\t\t%s ", name);
    nc = strlen(s);
    memset(s+nc, '.', ln-nc);
    s[ln] = '\0';

    printf("%s", s);

    rv = tst(n, &dt);

    printf(" %s (%8.2e)\n", (rv == TRUE) ? "ok" : "ng", dt);

    if (map == TRUE)
       cs = SC_mem_monitor(cs, dbg, "MOR", msg);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

static void help(void)
   {

    io_printf(STDOUT, "\n");

    io_printf(STDOUT, "Usage: tscmo [-h] [-z #]\n");
    io_printf(STDOUT, "   h    - this help message\n");
    io_printf(STDOUT, "   z    - reset memory\n");
    io_printf(STDOUT, "        - 0 none, 1 on both, 2 on alloc, 3 on free\n");

    io_printf(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the malloc override */

int main(int c, char **v, char **env)
   {int i, n, err;
    int zsp;

    SC_exe_init_api();

    SC_save_argv_env(c, v, env);

    SC_setbuf(stdout, NULL);

    zsp = 0;
    n   = 1000;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
		{case 'h' :
                      help();
                      return(1);
		      break;

		 case 'n' :
                      n = SC_stoi(v[++i]);
		      break;

		 case 'z' :
		      zsp = SC_stoi(v[++i]);
		      break;};};};

    SC_zero_space_n(zsp, -2);

    err = TRUE;
    
    err &= run_test("simple", 1, test_1);
    err &= run_test("malloc", n, test_2);
    err &= run_test("free",   n, test_3);

/* reverse the sense for exit status */
    err = (err != TRUE);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
