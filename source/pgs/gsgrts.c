/*
 * GSGRTS.C - test grotrian plotting routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

typedef int (*gtest)(PG_device *dev);
typedef int (*dtest)(char *name, int color, int debug_mode,
		     int nt, gtest tst);

#define N_TEST   1
#define N_DEV    4

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - basic grotrian plot test */

static int test_1(PG_device *dev)
   {int ns, nt;
    double *x, *y, *tr, *up, *dn;
    PM_mapping *f;
    PG_graph *g;

    ns = 5;
    nt = 3;

    x = FMAKE_N(double, ns, "TEST_1:x");
    y = FMAKE_N(double, ns, "TEST_1:y");

    x[0] = 1.0;
    x[1] = 1.0;
    x[2] = 1.0;
    x[3] = 2.0;
    x[4] = 2.0;

    y[0] = 1.0;
    y[1] = 4.0;
    y[2] = 5.0;
    y[3] = 1.5;
    y[4] = 4.5;

    tr = FMAKE_N(double, nt, "TEST_1:tr");
    up = FMAKE_N(double, nt, "TEST_1:up");
    dn = FMAKE_N(double, nt, "TEST_1:dn");

/* transition #1 */
    tr[0] = 10.0;
    up[0] = 2.0;
    dn[0] = 4.0;

/* transition #2 */
    tr[1] = 100.0;
    up[1] = 4.0;
    dn[1] = 0.0;

/* transition #3 */
    tr[2] = 200.0;
    up[2] = 2.0;
    dn[2] = 0.0;

    f = PM_build_grotrian("diagram #1", "double", FALSE, x, y,
			  NULL, ns, tr, up, dn, nt);
    g = PG_make_graph_from_mapping(f, NULL, NULL, 'A', NULL);

    PG_set_attrs_set(f->domain,
		     "LINE-COLOR", SC_INT_I, FALSE, dev->BLUE,
		     NULL);

    PG_grotrian_plot(dev, g);

    PG_rl_graph(g, TRUE, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_CHECK - begin/end memory checks for each test */

static long test_check(name, which, debug_mode)
   char *name;
   long which;
   int debug_mode;
   {long bytaa, bytfa, rv = 0;
    char msg[MAXLINE];
    static long bytab, bytfb;

    if (debug_mode)
       rv = SC_mem_monitor(which, 2, "G", msg);

    if (which == -1L)
       SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    else if (which != -1L)
       {SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

	bytaa -= bytab;
	bytfa -= bytfb;

	ONCE_SAFE(TRUE, NULL)
	   PRINT(STDOUT, "\n\n\t\t   Allocated      Freed       Diff\n");
	END_SAFE;

	PRINT(STDOUT, "\t%-10s %9d  %9d  %9d\n",
	      name, bytaa, bytfa, bytaa - bytfa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_DEVICES - register the devices needed for the tests */

static void register_devices()
   {

    PG_register_device("IMAGE", PG_setup_image_device);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCR_TEST - test the SCREEN device */

static int scr_test(char *name, int color, int debug_mode, int nt, gtest tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = TRUE;

    sc = test_check(name, -1L, debug_mode);

    register_devices();

    if (color)
       dev = PG_make_device("SCREEN", "COLOR", name);
    else
       dev = PG_make_device("SCREEN", "MONOCHROME", name);

    PG_open_device(dev, 0.81, 0.01, 0.3, 0.3);

    ok &= (*tst)(dev);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, debug_mode);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_TEST - test the PostScript device */

static int ps_test(char *name, int color, int debug_mode, int nt, gtest tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = TRUE;

    sc = test_check(name, -1L, debug_mode);

    register_devices();

    if (color)
       dev = PG_make_device("PS", "COLOR", name);
    else
       dev = PG_make_device("PS", "MONOCHROME PORTRAIT", name);

    PG_open_device(dev, 0.0, 0.0, 0.0, 0.0);

    ok &= (*tst)(dev);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, debug_mode);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, j, nd, nt, color;
    int debug_mode;
    gtest tst[N_TEST];
    dtest dev[N_DEV];
    char *name[N_DEV];

    name[0] = "scr_test";
    name[1] = "ps_test";

    dev[0] = scr_test;
    dev[1] = ps_test;
/*
    dev[2] = jpeg_test;
    dev[3] = image_test;
*/

    tst[0] = test_1;

    nt = 1;
    nd = 2;

    SC_zero_space(0);

    color      = 4;
    debug_mode = FALSE;

    PG_set_attrs_glb(TRUE,
		     "plot-date", FALSE,
		     NULL);

    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {switch (v[i][1])
	       {case '1' :
		case '2' :
		case '3' :
		case '4' :
		     dev[v[i][1]-'1'] = NULL;
		     break;
	        case 'd' :
		     debug_mode = TRUE;
		     break;
		default  :
		case 'h' :
		     return(1);
		     break;};};

    for (i = 0; i < nd; i++)
        for (j = 0; j < nt; j++)
	    if (dev[i] != NULL)
	       (*dev[i])(name[i], color, debug_mode, nt, tst[j]);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
