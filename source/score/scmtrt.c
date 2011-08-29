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

#include "score_int.h"

#define N_ITER   5
#define N_ITEMS  3

typedef struct s_statedes statedes;

struct s_statedes
   {int golp;
    JMP_BUF cpu;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test registered memory and memory monitor
 *        - NOTE: recently moved to be first test because some platforms
 *        - have problems with all the overwrites in what is now test_2
 *        - 32-bit GNU BG/P is the first culprit
 */

int test_1(int nir, int nim)
   {int i, err, na, nb, ma, mb;
    char msg[MAXLINE];
    float b[10];
    double **a;

    err = FALSE;

    io_printf(stdout, "\n-----------------------------\n");
    io_printf(stdout, "\nMemory registration test\n\n");

    SC_reg_mem(&i, sizeof(int), "TEST_1:i");

    na = SC_mem_monitor(-1, 2, "t2a", msg);
    nb = SC_mem_monitor(-1, 2, "t2b", msg);

    SC_reg_mem(b, sizeof(b), "TEST_1:b");

    a = CMAKE_N(double *, nir);

    io_printf(stdout, "   First leak check\n\n");
    ma = SC_mem_monitor(na, 2, "t2a", msg);
    SC_ASSERT(ma > 0);

    SC_dereg_mem(b);
    CFREE(a);

    io_printf(stdout, "\n");
    io_printf(stdout, "   Second leak check\n\n");
    mb = SC_mem_monitor(nb, 2, "t2b", msg);
    SC_ASSERT(mb > 0);

    if (err == FALSE)
       io_printf(stdout, "\nMemory registration test passed\n");
    else
       io_printf(stdout, "\nMemory registration test failed\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGH_2 - handle signals for test #2 */

static void sigh_2(int sig)
   {statedes *st;

    st = SC_GET_CONTEXT(sigh_2);

    SC_signal_n(SIGSEGV, sigh_2, st);

    st->golp = FALSE;

    LONGJMP(st->cpu, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - simple test of memory manager
 *        - allocate some space and over index some of it
 *        - checking detection of bad blocks
 */

int test_2(int nir, int nim)
   {int i, j, jmx, ok, err;
    double **a;
    statedes st;

    err = FALSE;

    io_printf(stdout, "\n-----------------------------\n");
    io_printf(stdout, "\nSimple memory allocation test\n\n");

    a = CMAKE_N(double *, nir);

    for (i = 0; i < nir; i++)
        {jmx = (i == 3) ? nim+10 : nim;

	 SC_signal_n(SIGSEGV, sigh_2, &st);

	 if (SETJMP(st.cpu) == 0)
	    {a[i] = CMAKE_N(double, nim);
	     for (j = 0; j < jmx; j++)
	         a[i][j] = (j + 1.0)*(i + 1.0);

	     ok = SC_mem_chk(3);
	     if (ok < 0)
	        io_printf(STDOUT, "   Detected bad block on pass %d : %d\n",
			  i, ok);
	     else
	        io_printf(STDOUT, "   Number of memory blocks on pass %d : %d\n",
			  i, ok);};};

    for (i = 0; i < nir; i++)
       CFREE(a[i]);

    CFREE(a);

    if (err == FALSE)
       io_printf(stdout, "\nSimple memory allocation test passed\n");
    else
       io_printf(stdout, "\nSimple memory allocation test failed\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRNT_3 - helper for test 3 */

int prnt_3(double *fb, double *f, double *fa)
   {int ok, oka, okb, rv, ty, rf;
    long nb;
    char *name;
    double *a, *b;

    SC_mem_neighbor(f, 1, &b, &a);

    oka = ((a == fa) || (fa == NULL));
    okb = ((b == fb) || (fb == NULL));
    rv  = (oka && okb);

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
   {int err, proceed;
    long ne;
    double *f1, *f2, *f3, *f4;

    io_printf(stdout, "\n-----------------------------\n");
    io_printf(stdout, "\nNeighbor location test\n\n");

    ne = 106L;

/* simple allocation and free test */
    f1 = CMAKE_N(double,   ne);
    f2 = CMAKE_N(double, ne+4);
    f3 = CMAKE_N(double, ne+2);
    f4 = CMAKE_N(double, ne+3);

/* depending on the memory manager these spaces are right
 * next to each other in which case we can do this test
 * NOTE: the full memory manager will get these in order
 * while the reduced one will not
 */
    proceed  = TRUE;
    proceed &= (((f2 - f1) - ne) == 0);
    proceed &= (((f3 - f2) - ne-4) == 0);
    proceed &= (((f4 - f3) - ne-2) == 0);

    SC_set_arrtype(f1, SC_DOUBLE_I);
    SC_set_arrtype(f2, SC_DOUBLE_I);
    SC_set_arrtype(f3, SC_DOUBLE_I);
    SC_set_arrtype(f4, SC_DOUBLE_I);

    err = TRUE;

    if (proceed == TRUE)
       {err &= prnt_3(NULL, f1, f2);
	err &= prnt_3(f1,   f2, f3);
	err &= prnt_3(f2,   f3, f4);
	err &= prnt_3(f3,   f4, NULL);};

#if 0

    statedes st;

    memset(&st, 0, sizeof(st));

/* test over and under indexing the space */
    a = CMAKE_N(double, ne);

    SC_signal_n(SIGSEGV, sigh_3, &st);
    SC_signal_n(SIGBUS,  sigh_3, &st);

    if (SETJMP(st.cpu) == 0)
       {st.golp = TRUE;
	for (iu = ne-1L; golp; iu++)
	    a[iu] = 1.0;};

/* NOTE: curiously FREEBSD will get a SIGBUS instead of a SIGSEGV
 * in the next loop so put the handler on both signals
 */
    SC_signal_n(SIGSEGV, sigh_3, &st);
    SC_signal_n(SIGBUS,  sigh_3, &st);

    if (SETJMP(st.cpu) == 0)
       {st.golp = TRUE;
	for (il = 0L; ; il--)
	    a[il] = -1.0;};

/* a loop can result if a subsequent SEGV happens
 * (going back to sigh_3 and its longjmp)
 * and we are through with the signal handler now
 * so revert to default signal handler
 */
    SC_signal_n(SIGSEGV, SIG_DFL, NULL);
    SC_signal_n(SIGBUS,  SIG_DFL, NULL);

    io_printf(stdout,
	      "   Unprotected memory area: (%ld < i < %ld) and (%ld < i < %ld)\n",
	      il, 0L, ne, iu);
    io_printf(stdout,
	      "   Memory descriptor size: %d\n",
	      N_DOUBLES_MD);

    CFREE(a);
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
   {statedes *st;

    st = SC_GET_CONTEXT(sigh_4);

    SC_signal_n(SIGSEGV, sigh_4, st);

    st->golp = FALSE;

    LONGJMP(st->cpu, TRUE);

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
     double *a;
     double *ad;
     float *af;
     long *al;
     int *ai;
     short *as;
     char *ac;
     statedes st;

     SC_use_guarded_mem(TRUE);

     il = 0;
     iu = 0;
     ne = 10013L;

/* simple allocation and free test */
     ad = CMAKE_N(double, ne);
     CREMAKE(ad, double, 2*ne);
     CFREE(ad);

     af = CMAKE_N(float, ne);
     CREMAKE(af, float, 2*ne);
     CFREE(af);

     al = CMAKE_N(long, ne);
     CREMAKE(al, long, 2*ne);
     CFREE(al);

     ai = CMAKE_N(int, ne);
     CREMAKE(ai, int, 2*ne);
     CFREE(ai);

     as = CMAKE_N(short, ne);
     CREMAKE(as, short, 2*ne);
     CFREE(as);

     ac = CMAKE_N(char, ne);
     CREMAKE(ac, char, 2*ne);
     CFREE(ac);

/* test over and under indexing the space */
     a = CMAKE_N(double, ne);

     SC_signal_n(SIGSEGV, sigh_4, &st);
     SC_signal_n(SIGBUS,  sigh_4, &st);

     if (SETJMP(st.cpu) == 0)
        {st.golp = TRUE;
	 for (iu = ne-1L; st.golp; iu++)
	     a[iu] = 1.0;};

/* NOTE: curiously FREEBSD will get a SIGBUS instead of a SIGSEGV
 * in the next loop so put the handler on both signals
 */
     SC_signal_n(SIGSEGV, sigh_4, &st);
     SC_signal_n(SIGBUS,  sigh_4, &st);

     if (SETJMP(st.cpu) == 0)
        {st.golp = TRUE;
	 for (il = 0L; ; il--)
	     a[il] = -1.0;};

/* a loop can result if a subsequent SEGV happens
 * (going back to sigh_4 and its longjmp)
 * and we are through with the signal handler now
 * so revert to default signal handler
 */
     SC_signal_n(SIGSEGV, SIG_DFL, NULL);
     SC_signal_n(SIGBUS,  SIG_DFL, NULL);

/* do not use io_printf because it may allocate memory and the heap
 * has been corrupted with overwrites
 */
     fprintf(stdout,
	     "   Unprotected memory area: (%ld < i < %ld) and (%ld < i < %ld)\n",
	     il, 0L, ne, iu);
     fprintf(stdout,
	     "   Memory descriptor size: %d\n",
	     N_DOUBLES_MD);

     CFREE(a);};

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
    SC_zero_space_n(1, -2);

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
