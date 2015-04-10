/*
 * TMLFPE.C - test SIGFPE routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

#undef SUCCESS
#define SUCCESS   7

#define NEWWAY

typedef struct s_testdes testdes;

struct s_testdes
   {int method;     /* 1 use PM_enable_fpe_n; 2 use PM_enable_fpe_t */
    int nt;
    int et;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DFPU - diagnostic print of FPU state */

#if 0

typedef union u_fpu_state fpu_state;

union u_fpu_state
   {double a;
    char b[512];};

void dfpu(void)
   {int16_t *ps;
    fpu_state fs;
    extern fpu_state *PM_save_fpu_x86(void);

    fs = *PM_save_fpu_x86();
    ps = (int16_t *) fs.b;

    printf("fpu  %d   0x%x  0x%x\n", SC_current_thread(), ps[0], ps[1]);

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MSG_OUT - thread safe message output */

static void msg_out(char *fmt, ...)
   {int it;
    char s[MAXLINE], t[MAXLINE];

    SC_VA_START(fmt);
    SC_VSNPRINTF(s, MAXLINE, fmt);
    SC_VA_END;

    it = SC_current_thread();
    snprintf(t, MAXLINE, "%d | %s\n", it, s);

    write(1, t, strlen(t));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HANDLER - handle SIGFPE */

#include <fenv.h>

static void handler(int sig)
   {JMP_BUF *cpu;

    cpu = SC_GET_CONTEXT(handler);

    PM_fpu_status(TRUE);

    PM_clear_fpu();

    PM_enable_fpe_n(-1, handler, cpu);
    msg_out(">>> Caught signal %d", sig);

    LONGJMP(*cpu, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_HND - print SIGFPE hander for diagnostic purposes */

void show_hnd(char *msg)
   {SC_contextdes cd;

    cd = SC_which_signal_handler(SIGFPE);
    if (cd.f == SIG_DFL)
       msg_out("SIG_DFL - %s", msg);

    else if (cd.f == SIG_IGN)
       msg_out("SIG_IGN - %s", msg);

    else if (cd.f == handler)
       msg_out("handler - %s", msg);

    else
       msg_out("unknow %p - %s", cd.f, msg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIV_ZERO - divide by zero test
 *          - return TRUE iff successful
 */

static int div_zero(int et)
   {int id, rv;
    volatile double a, x, y;
    JMP_BUF *cpu;

    rv = TRUE;

    cpu = SC_GET_CONTEXT(handler);

    a = 0.0;
    x = 1.0;
    y = 0.0;

    if (SETJMP(*cpu) == 0)
       {id = -1;
	id = SC_current_thread();
	if ((et == -1) || (et == id))
	   {a = x/y;
	    msg_out("      divide by zero ... ng");
	    rv = FALSE;}
	else
	   msg_out("      divide by zero ... pass");}
    else
       msg_out("      divide by zero ... ok");

    SC_ASSERT(a == 0.0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OVERFLOW - overflow test
 *          - return TRUE iff successful
 */

static int overflow(int et)
   {int i, id, rv;
    volatile double a, x;
    JMP_BUF *cpu;

    rv = TRUE;

    cpu = SC_GET_CONTEXT(handler);

    x = 1.0;

    if (SETJMP(*cpu) == 0)
       {id = -1;
	id = SC_current_thread();
	if ((et == -1) || (et == id))
	   {for (i = 0, a = x; i < 100000; i++)
	        a *= 2.0;
#ifdef IBM_BGP
	    msg_out("      overflow ......... ok");
#else
	    msg_out("      overflow ......... ng");
	    rv = FALSE;
#endif
	    }
	else
	   msg_out("      overflow ......... pass");}
    else
       msg_out("      overflow ......... ok");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNDERFLOW - underflow test
 *           - return TRUE iff successful
 */

static int underflow(int et)
   {int i, id, rv;
    volatile double a, x;
    JMP_BUF *cpu;

    cpu = SC_GET_CONTEXT(handler);

/* we don't trap underflows so return TRUE always */
    rv = TRUE;
    x  = 1.0;

    if (SETJMP(*cpu) == 0)
       {id = -1;
	id = SC_current_thread();
	if ((et == -1) || (et == id))
	   {for (i = 0, a = x; i < 100000; i++)
	        a *= 0.5;
	    msg_out("      underflow ........ pass");}
	else
	   msg_out("      underflow ........ pass");}
    else
       msg_out("      underflow ........ ok");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK - do the FPE test work
 *      - return TRUE iff successful
 */

static void *work(void *a)
   {int rv, et;
    int *prv;
    testdes *st;
    JMP_BUF cpu;

    st = (testdes *) a;

    if (st->method == 1)
       PM_enable_fpe_t(TRUE, handler, &cpu, sizeof(JMP_BUF));

    et = st->et;
    rv  = 0;
    prv = CMAKE(int);

    msg_out("   Trapping exceptions ... ");

/* try divide by zero - should be trapped */
    rv += div_zero(et);

/* try overflow - should be trapped */
    rv += (overflow(et) << 1);

/* try underflow - should not be trapped */
    rv += (underflow(et) << 2);

    *prv = rv;

    return(prv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK_SER - serial wrapper for work */

int work_ser(testdes *st)
   {int rv;
    int *prv;
    char msg[MAXLINE];

    snprintf(msg, MAXLINE, "\nSerial\n");
    write(1, msg, strlen(msg));

    prv = work(st);
    rv  = *prv;
    CFREE(prv);

    snprintf(msg, MAXLINE, "Serial ... %s\n",
	     ((rv == SUCCESS) ? "passed" : "failed"));
    write(1, msg, strlen(msg));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK_OMP - do work in OMP threads */

int work_omp(testdes *st)
   {int rv, nt;
    char msg[MAXLINE];

    rv = SUCCESS;
    nt = st->nt;

#ifdef SMP_OpenMP

    int *prv;

    snprintf(msg, MAXLINE, "\nOpenMP with %d threads\n", nt);
    write(1, msg, strlen(msg));

    if (getenv("OMP_NUM_THREADS") == NULL)
       omp_set_num_threads(nt);

    prv = NULL;

#pragma omp parallel private(prv) shared(rv)
    {
     prv = work(st);
     rv &= *prv;
     CFREE(prv);
    }

#endif

    snprintf(msg, MAXLINE, "OpenMP with %d threads ... %s\n",
	     nt, ((rv == SUCCESS) ? "passed" : "failed"));
    write(1, msg, strlen(msg));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK_THR - do work in threads */

int work_thr(testdes *st)
   {int i, rv, nt, et;
    int *ps;
    char msg[MAXLINE];
    void **pe, **ra;
    void *(*fnc[1])(void *);

    nt = st->nt;
    et = min(st->et, SC_n_threads);

    if (et == -1)
       snprintf(msg, MAXLINE, "\nThreaded with %d threads - FPE on all\n",
		nt);
    else
       snprintf(msg, MAXLINE, "\nThreaded with %d threads - FPE on %d\n",
		nt, et);
    write(1, msg, strlen(msg));

    rv = SUCCESS;

    ra = CMAKE_N(void *, nt+1);
    pe = CMAKE_N(void *, nt+1);
    fnc[0] = work;
    pe[0]  = st;

    SC_do_threads(1, &nt, fnc, pe, ra);

    for (i = 0; i < nt; i++)
        {ps  = ra[i];
	 rv &= *ps;
         CFREE(ps);};

    CFREE(ra);
    CFREE(pe);

    if (et == -1)
       snprintf(msg, MAXLINE, "Threaded with %d threads - FPE on all ... %s\n",
		nt, ((rv == SUCCESS) ? "passed" : "failed"));
    else
       snprintf(msg, MAXLINE, "Threaded with %d threads - FPE on %d ... %s\n",
		nt, et, ((rv == SUCCESS) ? "passed" : "failed"));
    write(1, msg, strlen(msg));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, nt, rv, tgt, verbose;
    JMP_BUF cpu;
    testdes st;

    verbose = FALSE;
    nt      = 1;
    tgt     = 0xff;

    st.method = 2;
    st.et     = -1;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("\n");
	     printf("Usage: tmlfpe [-h] [-m #] [-n #] [-t #] [-v]\n");
	     printf("   h   this help message\n");
	     printf("   m   1 unthreaded, 2 threaded FPE handlling\n");
	     printf("   n   number of threads to use\n");
	     printf("   t   bit array of tests\n");
	     printf("   v   report FPE traps\n");
	     printf("\n");
	     return(1);}
	 else if (strcmp(v[i], "-m") == 0)
            st.method = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-n") == 0)
            nt = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-t") == 0)
            tgt = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-v") == 0)
            verbose = TRUE;};

    st.nt = max(nt, 1);

    if (verbose == TRUE)
       PM_fpe_traps(TRUE);

    SC_init_threads(nt+1, NULL);

    if (st.method == 2)
       PM_enable_fpe_t(TRUE, handler, &cpu, sizeof(JMP_BUF));

    rv = SUCCESS;

    if ((tgt & 1) != 0)
       rv &= work_ser(&st);

    if ((tgt & 2) != 0)
       rv &= work_omp(&st);

    if ((tgt & 4) != 0)
       rv &= work_thr(&st);

    if ((tgt & 8) != 0)
       {st.et = 2;
	rv &= work_thr(&st);};

    if (verbose == TRUE)
       PM_fpu_status(TRUE);

/* change correct result into correct exit status
 * we do not expect to get underflow FPE
 */
    rv = (rv != SUCCESS);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

