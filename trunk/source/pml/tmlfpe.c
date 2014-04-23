/*
 * TMLFPE.C - test SIGFPE routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

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

static void handler(int sig)
   {JMP_BUF *cpu;

    cpu = SC_GET_CONTEXT(handler);

    PM_fpu_status(TRUE);
    PM_clear_fpu();
    PM_enable_fpe_n(TRUE, handler, cpu);

    msg_out(">>> Caught signal %d", sig);

    LONGJMP(*cpu, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIV_ZERO - divide by zero test */

static int div_zero(void)
   {int rv;
    volatile double a, x, y;
    JMP_BUF *cpu;

    rv = 0;

    cpu = SC_GET_CONTEXT(handler);

    x = 1.0;
    y = 0.0;

    if (SETJMP(*cpu) == 0)
       {a = x/y;
	msg_out("      divide by zero ... none");}
    else
       {rv = 1;
	msg_out("      divide by zero ... ok");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OVERFLOW - overflow test */

static int overflow(void)
   {int rv;
    int i;
    volatile double a, x;
    JMP_BUF *cpu;

    rv = 0;

    cpu = SC_GET_CONTEXT(handler);

    x = 1.0;

    if (SETJMP(*cpu) == 0)
       {for (i = 0, a = x; i < 100000; i++)
	    a *= 2.0;
#ifdef IBM_BGP
        rv = 1;
	msg_out("      overflow ......... ok");
#else
	msg_out("      overflow ......... none");
#endif
        }
    else
       {rv = 1;
	msg_out("      overflow ......... ok");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNDERFLOW - underflow test */

static int underflow(void)
   {int i, rv;
    volatile double a, x;
    JMP_BUF *cpu;

    cpu = SC_GET_CONTEXT(handler);

    rv = 0;
    x  = 1.0;

    if (SETJMP(*cpu) == 0)
       {for (i = 0, a = x; i < 100000; i++)
	    a *= 0.5;
	msg_out("      underflow ........ none");}
    else
       {rv = 1;
	msg_out("      underflow ........ ok");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK - do the FPE test work */

static void *work(void *a)
   {int rv;
    int *prv;
    JMP_BUF cpu;

    PM_enable_fpe_n(TRUE, handler, &cpu);

    rv = 0;

    msg_out("   Trapping exceptions ... ");

/* try divide by zero - should be trapped */
    rv |= div_zero();

/* try overflow - should be trapped */
    rv |= (overflow() << 1);

/* try underflow - should not be trapped */
    rv |= (underflow() << 2);

    prv  = CMAKE(int);
    *prv = rv;

    return(prv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK_OMP - do work in OMP threads */

int work_omp(int nt)
   {int rv;

    rv = 0;

#ifdef SMP_OpenMP

    int *prv;
    char msg[MAXLINE];

    snprintf(msg, MAXLINE, "OpenMP with %d threads\n", nt);
    write(1, msg, strlen(msg));


    if (getenv("OMP_NUM_THREADS") == NULL)
       omp_set_num_threads(nt);

    prv = NULL;

#pragma omp parallel private(prv) shared(rv)
    {
     prv = work(NULL);
     rv |= *prv;
     CFREE(prv);
    }

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK_PT - do work in PTHREADS threads */

int work_pt(int nt)
   {int rv;

    rv = 0;

#ifdef SMP_Pthread
    char msg[MAXLINE];

    snprintf(msg, MAXLINE, "PThreaded with %d threads\n", nt);
    write(1, msg, strlen(msg));

    work(&rv);

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK_THR - do work in threads */

int work_thr(int nt)
   {int i, rv;
    int *ra;
    char msg[MAXLINE];
    void *(*fnc[1])(void *);

    snprintf(msg, MAXLINE, "Threaded with %d threads\n", nt);
    write(1, msg, strlen(msg));

    rv = 0;

    ra = CMAKE_N(int, nt);
    fnc[0] = work;

    SC_do_threads(1, &nt, fnc, NULL, (void **) &ra);

    for (i = 0; i < nt; i++)
        rv |= ra[i];

    CFREE(ra);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, nt, rv, verbose;
    int *prv;

    verbose = FALSE;
    nt      = 0;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("\n");
	     printf("Usage: tmlfpe [-h] [-n #] [-v]\n");
	     printf("   h   this help message\n");
	     printf("   n   number of threads to use\n");
	     printf("   v   report FPE traps\n");
	     printf("\n");
	     return(1);}
	 else if (strcmp(v[i], "-n") == 0)
            nt = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-v") == 0)
            verbose = TRUE;};

    if (verbose == TRUE)
       PM_fpe_traps(TRUE);

    SC_init_threads(nt+1, NULL);

    rv = 0;

    prv = work(NULL);
    rv |= *prv;
    CFREE(prv);
    rv |= work_omp(nt);
/*
    rv |= work_pt(nt);
*/
    rv |= work_thr(nt);

    if (verbose == TRUE)
       PM_fpu_status(TRUE);

/* change correct result into correct exit status
 * we do not expect to get underflow FPE
 */
    rv = (rv != 3);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

