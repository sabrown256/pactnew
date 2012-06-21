/*
 * TSCRS.C - test resource usage functionality
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define TEST   1
#define PS     2
#define N_ITER 100000

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_1 - find resource for current process */

static int test_1(void)
   {int i, err, pid;
    double ti, tf;
    SC_rusedes ru;

    pid = SYS_GETPID();

/* take up some time at least */
#if 0
    {double f;

     f = 0.0;
     for (i = 0; i < 20000000; i++)
         f += cos(1.0/(i + 1.0));};
#endif

/* NOTE: cannot use SC_time_allow because we may use SC_exec which
 * uses it internally
 */
    ti = SC_wall_clock_time();
    for (i = 0; i < N_ITER; i++)
        {err = SC_resource_usage(&ru, pid);
	 tf  = SC_wall_clock_time() - ti;
	 if (tf > 2.0)
	    break;};

    io_printf(STDOUT, "\t\t\t%2d\t%8.2f\t%8.1e\t%.2e\n",
	      1, ru.ut, ru.maxrss, tf/(i + 1.0));

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_2 - find resource for init */

static int test_2(void)
   {int i, err;
    double ti, tf;
    SC_rusedes ru;

/* NOTE: cannot use SC_time_allow because we may use SC_exec which
 * uses it internally
 */
    ti = SC_wall_clock_time();
    for (i = 0; i < N_ITER; i++)
        {err = SC_resource_usage(&ru, 1);
	 tf  = SC_wall_clock_time() - ti;
	 if (tf > 2.0)
	    break;};

    if (err == 0)
       {io_printf(STDOUT, "\t\t\t%2d\t    Permission denied\t\t%.2e\n",
		  2, tf/(i + 1.0));
	err = 1;}
    else
       io_printf(STDOUT, "\t\t\t%2d\t%8.2f\t%8.1e\t%.2e\n",
		 2, ru.ut, ru.maxrss, tf/(i + 1.0));

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST - test mode */

int test(void)
   {int err;

    io_printf(STDOUT, "\n\t\t\tTest\tCPU Time\t     RSS\tTime\n");

    err = 0;

    err += test_1();
    err += test_2();

    io_printf(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_HELP - show the usage and options */

static void show_help(void)
   {

    io_printf(stdout, "Usage: tscrs [-h]\n\n");

    io_printf(stdout, "   h      this help message\n");
    io_printf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* MAIN - start here */

int main(int c, char **v)
   {int i, err, rv;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h':
                      show_help();
                      return(1);
                      break;
		 default:
		      io_printf(stdout, "Unknown option: %s\n", v[i]);
		      return(1);
                      break;};}
         else
	    break;};


    err = test();

    rv = (err != 2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
