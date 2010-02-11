/*
 * SCMTRT.C - test the heap check routines
 *          - testing for correctness
 *          - see scmpft.c for performance testing
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define N_ITER   5
#define N_ITEMS  3

static JMP_BUF
 cpu;

static int
 golp = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - simple test of memory manager
 *        - allocate some space and over index some of it
 *        - checking detection of bad blocks
 */

int test_1(int nir, int nim)
   {int i, j, jmx, ok, err;
    REAL **a;

    err = FALSE;

    io_printf(stdout, "\n-----------------------------\n");
    io_printf(stdout, "\nSimple memory allocation test\n\n");

    a = FMAKE_N(REAL *, nir, "TEST_1:a");

    for (i = 0; i < nir; i++)
        {jmx = (i == 3) ? nim+10 : nim;

	 a[i] = FMAKE_N(REAL, nim, "TEST_1:a[i]");
	 for (j = 0; j < jmx; j++)
	     a[i][j] = (j + 1.0)*(i + 1.0);

	 ok = SC_mem_chk(3);
	 if (ok < 0)
	    io_printf(STDOUT, "   Detected bad block on pass %d : %d\n",
		      i, ok);
	 else
	    io_printf(STDOUT, "   Number of memory blocks on pass %d : %d\n",
		      i, ok);};

    for (i = 0; i < nir; i++)
        SFREE(a[i]);

    SFREE(a);

    if (err == FALSE)
       io_printf(stdout, "\nSimple memory allocation test passed\n");
    else
       io_printf(stdout, "\nSimple memory allocation test failed\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test registered memory and memory monitor */

int test_2(int nir, int nim)
   {int i, err, na, nb, ma, mb;
    char msg[MAXLINE];
    float b[10];
    REAL **a;

    err = FALSE;

    io_printf(stdout, "\n-----------------------------\n");
    io_printf(stdout, "\nMemory registration test\n\n");

    SC_reg_mem(&i, sizeof(int), "TEST_2:i");

    na = SC_mem_monitor(-1, 2, "t2a", msg);
    nb = SC_mem_monitor(-1, 2, "t2b", msg);

    SC_reg_mem(b, sizeof(b), "TEST_2:b");

    a = FMAKE_N(REAL *, nir, "TEST_2:a");

    io_printf(stdout, "   First leak check\n\n");
    ma = SC_mem_monitor(na, 2, "t2a", msg);

    SC_dereg_mem(b);
    SFREE(a);

    io_printf(stdout, "\n");
    io_printf(stdout, "   Second leak check\n\n");
    mb = SC_mem_monitor(nb, 2, "t2b", msg);

    if (err == FALSE)
       io_printf(stdout, "\nMemory registration test passed\n");
    else
       io_printf(stdout, "\nMemory registration test failed\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRNT_3 - helper for test 3 */

int prnt_3(double *f, double *fb, double *fa)
   {int ok, rv, ty, rf;
    long nb;
    char *name;
    double *a, *b;

    SC_mem_neighbor(f, &a, &b);

    rv = ((a == fa) && (b == fb));

    ok = SC_mem_info(b, &nb, &ty, &rf, &name);
    if (ok == TRUE)
       io_printf(stdout, "      before %10p %8d %4d %4d   %s\n",
		 b, nb, ty, rf, name);
    else
       io_printf(stdout, "      before none\n");

    ok = SC_mem_info(f, &nb, &ty, &rf, &name);
    if (ok == TRUE)
       io_printf(stdout, "      space  %10p %8d %4d %4d   %s\n",
		 f, nb, ty, rf, name);
    else
       io_printf(stdout, "      before none\n");

    ok = SC_mem_info(a, &nb, &ty, &rf, &name);
    if (ok == TRUE)
       io_printf(stdout, "      after  %10p %8d %4d %4d   %s\n",
		 a, nb, ty, rf, name);
    else
       io_printf(stdout, "      after  none\n");

    io_printf(stdout, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test finding neighbor spaces */

int test_3(int nir, int nim)
   {int err;
    long ne;
    double *f1, *f2, *f3, *f4;

    io_printf(stdout, "\n-----------------------------\n");
    io_printf(stdout, "\nNeighbor location test\n\n");

    ne = 106L;

/* simple allocation and free test */
    f1 = FMAKE_N(double,   ne, "TEST_3:f1");
    f2 = FMAKE_N(double, ne+4, "TEST_3:f2");
    f3 = FMAKE_N(double, ne+2, "TEST_3:f3");
    f4 = FMAKE_N(double, ne+3, "TEST_3:f4");

    SC_set_arrtype(f1, SC_DOUBLE_I);
    SC_set_arrtype(f2, SC_DOUBLE_I);
    SC_set_arrtype(f3, SC_DOUBLE_I);
    SC_set_arrtype(f4, SC_DOUBLE_I);

    err = TRUE;

    err &= prnt_3(f1, NULL, f2);
    err &= prnt_3(f2,   f1, f3);
    err &= prnt_3(f3,   f2, f4);
    err &= prnt_3(f4,   f3, NULL);

#if 0

/* test over and under indexing the space */
    a = FMAKE_N(REAL, ne, "TEST_3:a");

    SC_signal(SIGSEGV, sigh_3);
    SC_signal(SIGBUS,  sigh_3);

    if (SETJMP(cpu) == 0)
       {golp = TRUE;
	for (iu = ne-1L; golp; iu++)
	    a[iu] = 1.0;};

/* NOTE: curiously FREEBSD will get a SIGBUS instead of a SIGSEGV
 * in the next loop so put the handler on both signals
 */
    SC_signal(SIGSEGV, sigh_3);
    SC_signal(SIGBUS,  sigh_3);

    if (SETJMP(cpu) == 0)
       {golp = TRUE;
	for (il = 0L; ; il--)
	    a[il] = -1.0;};

/* a loop can result if a subsequent SEGV happens
 * (going back to sigh_3 and its longjmp)
 * and we are through with the signal handler now
 * so revert to default signal handler
 */
    SC_signal(SIGSEGV, SIG_DFL);
    SC_signal(SIGBUS,  SIG_DFL);

    io_printf(stdout,
	      "   Unprotected memory area: (%ld < i < %ld) and (%ld < i < %ld)\n",
	      il, 0L, ne, iu);
    io_printf(stdout,
	      "   Memory descriptor size: %d\n",
	      N_DOUBLES_MD);

    SFREE(a);
#endif

/* reverse the sense of err */
    err = (err == FALSE);

    if (err == FALSE)
       io_printf(stdout, "\nNeighbor location test passed\n");
    else
       io_printf(stdout, "\nNeighbor location test failed\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGH_4 - handle signals for test #4 */

static void sigh_4(int sig)
   {

    SC_signal(SIGSEGV, sigh_4);

    golp = FALSE;

    LONGJMP(cpu, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test of memory protection */

int test_4(int nir, int nim)
   {int err;

    io_printf(stdout, "\n-----------------------------\n");
    io_printf(stdout, "\nMemory protection test\n\n");

    err = FALSE;

#if defined(MACOSX)
    fprintf(stdout, "\nNot available on Mac OS X\n");

#elif !defined(HAVE_MMAP)
    fprintf(stdout, "\nNot available this platform\n");

#else
    {long il, iu, ne;
     REAL *a;
     double *ad;
     float *af;
     long *al;
     int *ai;
     short *as;
     char *ac;

     SC_use_guarded_mem(TRUE);

     il = 0;
     iu = 0;
     ne = 10013L;

/* simple allocation and free test */
     ad = FMAKE_N(double, ne, "TEST_4:a");
     REMAKE_N(ad, double, 2*ne);
     SFREE(ad);

     af = FMAKE_N(float, ne, "TEST_4:a");
     REMAKE_N(af, float, 2*ne);
     SFREE(af);

     al = FMAKE_N(long, ne, "TEST_4:a");
     REMAKE_N(al, long, 2*ne);
     SFREE(al);

     ai = FMAKE_N(int, ne, "TEST_4:a");
     REMAKE_N(ai, int, 2*ne);
     SFREE(ai);

     as = FMAKE_N(short, ne, "TEST_4:a");
     REMAKE_N(as, short, 2*ne);
     SFREE(as);

     ac = FMAKE_N(char, ne, "TEST_4:a");
     REMAKE_N(ac, char, 2*ne);
     SFREE(ac);

/* test over and under indexing the space */
     a = FMAKE_N(REAL, ne, "TEST_4:a");

     SC_signal(SIGSEGV, sigh_4);
     SC_signal(SIGBUS,  sigh_4);

     if (SETJMP(cpu) == 0)
        {golp = TRUE;
	 for (iu = ne-1L; golp; iu++)
	     a[iu] = 1.0;};

/* NOTE: curiously FREEBSD will get a SIGBUS instead of a SIGSEGV
 * in the next loop so put the handler on both signals
 */
     SC_signal(SIGSEGV, sigh_4);
     SC_signal(SIGBUS,  sigh_4);

     if (SETJMP(cpu) == 0)
        {golp = TRUE;
	 for (il = 0L; ; il--)
	     a[il] = -1.0;};

/* a loop can result if a subsequent SEGV happens
 * (going back to sigh_4 and its longjmp)
 * and we are through with the signal handler now
 * so revert to default signal handler
 */
     SC_signal(SIGSEGV, SIG_DFL);
     SC_signal(SIGBUS,  SIG_DFL);

/* do not use io_printf because it may allocate memory and the heap
 * has been corrupted with overwrites
 */
     fprintf(stdout,
	     "   Unprotected memory area: (%ld < i < %ld) and (%ld < i < %ld)\n",
	     il, 0L, ne, iu);
     fprintf(stdout,
	     "   Memory descriptor size: %d\n",
	     N_DOUBLES_MD);

     SFREE(a);};

#endif

    if (err == FALSE)
       fprintf(stdout, "\nMemory protection test passed\n");
    else
       fprintf(stdout, "\nMemory protection test failed\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the heap checkers */

int main(int c, char **v)
   {int i, err;
    int nir, nim;
    int i1, i2, i3, i4;

    nim = N_ITEMS;
    nir = N_ITER;

    i1 = TRUE;
    i2 = TRUE;
    i3 = TRUE;
    i4 = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
	        {case '1' :
                      i1 = FALSE;
		      break;

	         case '2' :
                      i2 = FALSE;
		      break;

	         case '3' :
                      i3 = FALSE;
		      break;

	         case '4' :
                      i4 = FALSE;
		      break;

		 case 'h' :
		      io_printf(STDOUT, "\n\n");
		      io_printf(STDOUT,
				"Usage: scmtrt [-g] [-l <# loops>] [-n <# doubles>] [-s]\n");
                      return(1);
		      break;

		 case 'g' :
		      SC_use_guarded_mem(TRUE);
		      break;
                    
		 case 's' :
		       SC_configure_mm(0L, 0L, 0L, 1.0);
		      break;

		 case 'l' :
		      nir = SC_stoi(v[++i]);
		      break;
                    
		 case 'n' :
		      nim = SC_stoi(v[++i]);
		      break;};};};

    err = FALSE;

/* turn this on to test the efficient memset replacement */
    SC_zero_space(1);

/* NOTE: we can use io_printf up until the heap is corrupted by
 * deliberate overwrites - thereafter we must use fprintf
 */

#ifndef CYGWIN
    if (i1)
       err |= test_1(nir, nim);
    if (i2)
       err |= test_2(nir, nim);
    if (i3)
       err |= test_3(nir, nim);
    if (i4)
       err |= test_4(nir, nim);
#endif

    fprintf(stdout, "\n-----------------------------\n");
    if (err == FALSE)
       fprintf(stdout, "\nMemory test completed successfully\n\n");
    else
       fprintf(stdout, "\nMemory test completed with failures\n\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
