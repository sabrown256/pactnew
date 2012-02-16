/*
 * SCPAR.C - parallel support routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

typedef struct s_thread_work thread_work;
typedef struct s_thread_queue thread_queue;
typedef struct s_thread_chunk thread_chunk;
typedef struct s_thread_pool thread_pool;

struct s_thread_work
   {int n_thread;              /* number of threads that will do this work unit */
    void *(*func)(void *a);    /* function that will do the work */
    void *arg;                 /* the single argument that the function will take */
    void **ret;};              /* the n_thread return values from the function */

struct s_thread_queue
   {int nxt;
    SC_thread_lock lock;};

struct s_thread_chunk
   {int wid;
    int *index;
    SC_thread_lock lock;};

struct s_thread_pool
   {int size;
    int nxt;                   /* next item */
    int ns;                    /* number started */
    int nf;                    /* number finished */
    int ni;                    /* number of items */
    SC_thread_lock lock;
    SC_thread_lock fin;        /* finish lock */
    SC_thread_cond go;         /* cond to start doing work */
    SC_thread_cond init;       /* cond to complete initialization */
    SC_array *wrk;};           /* items of work for pool */

static thread_queue
  _SC_tq = { 0, SC_LOCK_INIT_STATE };

static thread_chunk
  _SC_tc = { 0, NULL, SC_LOCK_INIT_STATE };

#ifdef HAVE_THREADS

static thread_pool
  _SC_tp = { 0, 0, 0, 0, 0,
	     SC_LOCK_INIT_STATE,
	     SC_LOCK_INIT_STATE,
	     SC_COND_INIT_STATE,
	     SC_COND_INIT_STATE };
#endif

static void
 _SC_do_threads(int n, thread_work *tw);

SC_thread_attr
 *_SC_pool_attr = &_SC_attr_attached,
 *_SC_new_attr  = &_SC_attr_attached;

int
 SC_n_threads = -1;

/*--------------------------------------------------------------------------*/

/*                               SMP ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* SC_THREAD_SET_TID - set _SC.tid_hook to F */

PFTid SC_thread_set_tid(PFTid f)
   {PFTid of;

    of = _SC.tid_hook;
    if (f != NULL)
       _SC.tid_hook = f;
    else
       _SC.tid_hook = SC_thread_oper->id;

    return(of);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CURRENT_THREAD - return the id of the current thread
 *                   - this is expected to be between 0 and the
 *                   - maximum number of threads (maybe plus one)
 *                   - we cannot use something like the PTHREAD_SELF result
 */

int SC_current_thread(void)
   {int tid;

    tid = 0;

    if (_SC.tid_hook != NULL)
       (*_SC.tid_hook)(&tid);

    return(tid);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USING_TPOOL - return value of SC_thread_oper->use_pool */

int SC_using_tpool(void)
   {

    return(SC_thread_oper->use_pool);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_THREAD_WORK - initialize an thread_work instance with
 *                      - number of threads NT, function FNC, and
 *                      - arg list ARGL
 */

static void _SC_init_thread_work(thread_work *tw, int nt,
				 PFPVoidAPV f, void *a)
   {int i, np;
    void **r;

    nt = max(nt, 1);
    np = nt + 1;

    r = CMAKE_N(void *, np);
    for (i = 0; i < np; i++)
        r[i] = NULL;

    tw->n_thread = nt;
    tw->func     = f;
    tw->arg      = a;
    tw->ret      = r;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_THREAD_WORK - instantiate a thread_work with
 *                      - number of threads NT, function FNC, and
 *                      - arg list ARGL
 */

static thread_work *_SC_make_thread_work(int nt, PFPVoidAPV f, void *a)
   {thread_work *tw;

    tw = CMAKE(thread_work);

    _SC_init_thread_work(tw, nt, f, a);

    return(tw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_THREAD_WORK - release the thread_work TW which has N entries */

static void _SC_free_thread_work(thread_work *tw, int n)
   {int i;

    for (i = 0; i < n; i++)
        CFREE(tw[i].ret);
    CFREE(tw);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DO_THREADS - execute the given function NT times
 *               - serial execution follows from NT equal to 1
 *
 *                  n: number of entries in the following arguments
 *              nt[i]: number of threads that should execute fnc[i]
 *             fnc[i]: array of functions to execute
 *             arg[i]: argument list (struct *) for fnc[i]
 *
 *             ret: array to hold return values of all invocations
 *                  of all functions
 */

void SC_do_threads(int n, int *nt, PFPVoidAPV fnc[],
		   void **arg, void **ret)
   {int i, k, it, lt;
    PFPVoidAPV f;
    void *a;
    thread_work *tw;

    tw = CMAKE_N(thread_work, n);

/* load up the thread work array */
    for (i = 0; i < n; i++)
        {f = (fnc != NULL) ? fnc[i] : NULL;
	 a = (arg != NULL) ? arg[i] : NULL;
	 _SC_init_thread_work(tw+i, nt[i], f, a);};

/* do the work according to the thread work array */
    _SC_do_threads(n, tw);

/* get the results out of the thread work array */
    if (ret != NULL)
       {k = 0;
	for (i = 0; i < n; i++)
	    {lt = tw[i].n_thread;
	     for (it = 0; it < lt; it++)
	         ret[k++] = tw[i].ret[it];};};

    _SC_free_thread_work(tw, n);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ONE_THREAD - do a single thread_work sequentially */

static void _SC_one_thread(int tid, thread_work *tw)
   {int i, n, chg;
    int *otid;
    void *a, **r;
    PFPVoidAPV f;

    n = tw->n_thread;
    f = tw->func;
    a = tw->arg;
    r = tw->ret;

/* handle thread emulation better by using assigned thread id */
    chg = FALSE;
    if (tid != -1)
       {chg = TRUE;
        otid = SC_GET_PKEY(int, SC_thread_oper->ikey);
	SC_SET_KEY(int, SC_thread_oper->ikey, &tid);};

    if (r == NULL)
       {for (i = 0; i < n; i++)
	    (*f)(a);}

    else
       {for (i = 0; i < n; i++)
            r[i] = (*f)(a);};

/* reset if changed */
    if ((chg == TRUE) && (otid != NULL))
       SC_SET_KEY(int, SC_thread_oper->ikey, otid);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DTID - return the id of the current thread - debugging version */

int dtid(void)
   {int id;

    id = SC_current_thread();

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_THREADS - initialize SCORE threads
 *                 - return the number of threads initialized in this call or
 *                 - the number of threads originally initialized
 *                 - if it has already been done
 *                 - NOTE: policy is that SC_init_threads really works
 *                 -       once, that is you can call it all you want but
 *                 -       only the first one will allocate any threads!
 *
 * #bind SC_init_threads fortran() python()
 */

int SC_init_threads(int nt, PFTid tid)
   {int ret;
    emu_thread_info *ti;

    ret = SC_n_threads;

    ONCE_SAFE(TRUE, NULL)
        SC_n_threads = nt;

        ti = _SC_get_thread_info(-1);

	SC_CREATE_KEY(SC_thread_oper->ikey, NULL);
	SC_SET_KEY(int, SC_thread_oper->ikey, &ti->ide);
	SC_thread_oper->init_ikey = TRUE;

	_SC.tid_hook = (nt > 1) ? tid : NULL;
	if (_SC.tid_hook == NULL)
	   _SC.tid_hook = SC_thread_oper->id;

        SC_THREAD_INIT_SYS();

	ret = nt;
    END_SAFE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_OMP - initialize OMP operation */

int SC_init_omp(int c, char **v)
   {int i, nt;
    static int first = TRUE;

    if (first == TRUE)
       {first = FALSE;

	nt = -1;

	for (i = 1; i < c; i++)
	    {if (strcmp(v[i], "-t") == 0)
		{nt = SC_stoi(v[++i]);
		 break;};};

#ifdef PTHREAD_OMP
	int ip, rs;
	char *p;

/* check for number of threads */
	p = getenv("OMP_NUM_THREADS");
	if (p != NULL)
	   {nt = SC_stoi(p);
	    omp_set_num_threads(nt);};

/* check for dynamic threads */
	p = getenv("OMP_DYNAMIC");
	if (p != NULL)
	   {rs = ((p[0] == 'y') || (p[0] == 'Y'));
	    omp_set_dynamic(rs);};

/* if we asked for threads, force OMP to create threads now
 * later the problem may have gotten bigger and run out of memory
 */
	if (nt > 0)
	   {ip = 0;

#pragma omp parallel default(none) shared(ip)
#pragma omp critical
	    ip = ip + 1;

	   };
#endif

       };

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                                CHUNK PARALLEL                            */

/*--------------------------------------------------------------------------*/

/* SC_CHUNK_LOOP - do up the threads in a controlled diagnosable way */

void SC_chunk_loop(PFPVoidAPV fnc, int mn, int mx, int serial, void *argl)
   {int i, j, dj, n1, n2, np, nt;
    int nmn, nmx;
    int *it;
    void **ret;
    PFTid otid;
    thread_work *tw;
    static int debug = FALSE;

/* if the threads haven't been initialized before now - do it */
    if (SC_n_threads < 0)
       SC_init_threads(1, SC_thread_oper->id);

    otid        = _SC.tid_hook;
    _SC.tid_hook = SC_thread_oper->id;
    _SC_tc.wid  = 0;

    nt = (serial) ? 1 : SC_n_threads;
    nt = max(nt, 1);
    np = nt + 1;

    tw = _SC_make_thread_work(nt, fnc, argl);

    _SC_tc.index = CMAKE_N(int, np);

/* compute the index range each thread is to do */
    dj = (mx - mn)/nt;

    _SC_tc.index[0] = mn;
    for (j = 1; j < nt; j++)
        _SC_tc.index[j] = _SC_tc.index[j-1] + dj;
    _SC_tc.index[j] = mx;

/* have the threads do their pieces */
    _SC_do_threads(1, tw);

/* check that each piece has been done */
    nmn = INT_MAX;
    nmx = -INT_MAX;
    ret = tw->ret;
    for (i = 0; i < nt; i++)
        {it = (int *) ret[i];
	 n1 = *it++;
	 n2 = *it++;

	 nmn = min(nmn, n1);
	 nmx = max(nmx, n2);

	 if (debug)
	    io_printf(stdout, "%3d\t%d-%d\n", i, n1, n2);};

    if ((nmn != mn) || (nmx != mx))
       io_printf(stdout,
		 "MISSED CHUNK IN PARALLEL LOOP - SC_CHUNK_LOOP");

    _SC_free_thread_work(tw, 1);
    CFREE(_SC_tc.index);

    _SC.tid_hook = otid;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHUNK_SPLIT - return the chunk of the loop for the current thread */

void SC_chunk_split(int *pmn, int *pmx, void **prv)
   {int *pc;

    SC_LOCKON(_SC_tc.lock);

    _SC_tc.wid = (_SC_tc.wid < SC_n_threads) ? _SC_tc.wid + 1 : 1;

    pc = _SC_tc.index + _SC_tc.wid - 1;

    SC_LOCKOFF(_SC_tc.lock);

    *pmn = pc[0];
    *pmx = pc[1];
    *prv = pc;

    return;}

/*--------------------------------------------------------------------------*/

/*                                QUEUE PARALLEL                            */

/*--------------------------------------------------------------------------*/

/* SC_QUEUE_NEXT_ITEM - return index for next item in queue */

int SC_queue_next_item(int ng)
   {int ig;

    SC_LOCKON(_SC_tq.lock);

/* mark the sequence as ended */
    if (_SC_tq.nxt >= ng)
       ig = -1;

/* pop the next index in the sequence */
    else
       ig = _SC_tq.nxt++;

    SC_LOCKOFF(_SC_tq.lock);

    return(ig);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_QUEUE_WORK - execute the given function FNC on NT threads
 *               - this is to process queues of work
 *               - serial execution follows from NT equal to 1
 */

void SC_queue_work(PFPVoidAPV fnc, int serial, void *argl)
   {int nt;
    thread_work *tw;

/* if the threads haven't been initialized before now - do it */
    if (SC_n_threads < 0)
       SC_init_threads(1, SC_thread_oper->id);

    _SC_tq.nxt = 0;

    nt = (serial) ? 1 : SC_n_threads;

    tw = _SC_make_thread_work(nt, fnc, argl);

    _SC_do_threads(1, tw);

    _SC_free_thread_work(tw, 1);

    return;}

/*--------------------------------------------------------------------------*/

/*                                 TASK PARALLEL                            */

/*--------------------------------------------------------------------------*/

/* _SC_DO_TASK - do a task from a set of tasks
 *             - the function FNC must be FORTRAN compatible
 *             - it will be passed addresses!!
 */

static void *_SC_do_task(void *arg)
   {int it, is, off;
    void *rv, *a;
    int (*fnc)(void **a, int *it);
    SC_address *tsk;

    tsk = (SC_address *) arg;

    fnc = ( int (*)(void **a, int *it) ) tsk[0].funcaddr;
    a   = tsk[1].memaddr;
    off = tsk[2].diskaddr;

    SC_chunk_split(&it, &is, &rv);

    it += off;

    (*fnc)(&a, &it);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DO_TASKS - do tasks in parallel */

void SC_do_tasks(PFInt fnc, void *a, int np, int off, int on)
   {int serial;
    SC_address tsk[3];

    tsk[0].funcaddr = fnc;
    tsk[1].memaddr  = a;
    tsk[2].diskaddr = off;

    if (on == FALSE)
       np = 1;

    serial = (np < 2);

    SC_chunk_loop(_SC_do_task, 0, np, serial, &tsk);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DO_TASKS_F - do tasks in parallel
 *               - in case the task manager is in Fortran
 *               - this allows the application to have a call compatible
 *               - alternative (use different function pointer)
 */

void SC_do_tasks_f(PFInt fnc, void **pa, int *pnt, int *poff, int *pon)
   {int nt, off, on;
    void *a;

    a   = (void *) *pa;
    nt  = *pnt;
    off = *poff;
    on  = *pon;

    SC_do_tasks(fnc, a, nt, off, on);

    return;}

/*--------------------------------------------------------------------------*/

#ifdef HAVE_THREADS

/*--------------------------------------------------------------------------*/

/* _SC_DO_NEW_THREAD - provide a place to do some SCORE level book keeping
 *                   - in the new thread before the worker function is
 *                   - called
 */

static void *_SC_do_new_thread(void *x)
   {void *arg, *val;
    PFPVoidAPV func;
    emu_thread_info *ti;

    ti = (emu_thread_info *) x;

    func = ti->func;
    arg  = ti->arg;

    SC_LOCKON(SC_ts_lock);
    
    ti->tid   = SC_THREAD_SELF();
    ti->exeid = ti->ide;
/* printf("newt>                 ->    %p   (%4d %8llx)\n",
       ti, ti->ide, (long long) ti->tid); */

    SC_LOCKOFF(SC_ts_lock);

    val = (*func)(arg);

    ti->exeid = -1;

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_NEW_THREADS - do N pieces of thread_work TW
 *                    - create and join threads
 */

static void _SC_do_new_threads(int n, thread_work *tw)
   {int i, j, k, nt, nx, ok;
    void *(*f)(void *a);
    void *a, **r;
    emu_thread_info *ti;

    nx = 0;
    for (i = 0; i < n; i++)
        nx += tw[i].n_thread;

    for (k = 0, i = 0; i < n; i++)
        {nt = tw[i].n_thread;
	 f  = tw[i].func;
	 a  = tw[i].arg;
	 for (j = 0; j < nt; j++, k++)
	     {ti = _SC_get_thread_info(k+1);

	      ti->func  = f;
	      ti->arg   = a;
	      ti->exeid = -1;

	      ok = SC_THREAD_CREATE(ti->tid, _SC_new_attr,
				    _SC_do_new_thread, ti);
	      if (ok != 0)
		 io_printf(stdout,
			   "THREAD CREATE FAILED %d (%d) - _SC_DO_NEW_THEADS\n",
			   ok, k);};};

    k = 0;
    for (i = 0; i < n; i++)
        {nt = tw[i].n_thread;
	 for (j = 0; j < nt; j++)
	     {ti = _SC_get_thread_info(k+1);

	      r = (tw[i].ret == NULL) ? NULL : &tw[i].ret[k];

	      SC_THREAD_JOIN(ti->tid, r);

	      k++;};};

    return;}

/*--------------------------------------------------------------------------*/

/*                             THREAD POOL ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_FIN_POOL_THREAD - place holder to tell pool worker thread to exit */

static void *_SC_fin_pool_thread(void *x)
   {

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TW_WHICH_ITEM - get the Ith item from the dynamic array SC_tw
 *                   - also return the index J into the return value
 *                   - array for the thread_work item
 */

static void _SC_tw_which_item(thread_pool *tp, thread_work **ptw, int *pj)
   {int i, li;
    thread_work *tw, *ltw;

    i  = tp->nxt++;
    tw = SC_array_get(tp->wrk, i);

    for (li = i-1; li >= 0; li--)
        {ltw = SC_array_get(tp->wrk, li);
	 if (tw != ltw)
            break;};

    *ptw = tw;
    *pj  = i - li - 1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_POOL_THREAD - manage work for pool thread
 *                    - stays alive looking for work
 */

static void _SC_do_pool_thread(void *x)
   {int j;
    void *a, **r;
    PFPVoidAPV f;
    thread_work *tw;
    thread_pool *tp;

    tp = &_SC_tp;

    SC_SET_KEY(int, SC_thread_oper->ikey, x);

    while (TRUE)
       {SC_LOCKON(tp->lock);
	tp->nf++;
           
	if (tp->ns >= tp->ni)
	   tp->ni = 0;

	if ((tp->ns == tp->nf) && (tp->ni == 0))
	   SC_THREAD_SIGNAL(tp->init);

/* wait until _SC_do_pool_threads tells this thread that there is work to do */
	while (tp->ni == 0)
	   SC_COND_WAIT(tp->go, tp->lock);
           
/* get the tp->nxt function and arguments */
	f = NULL;
	if (tp->ns < tp->ni)
	   {_SC_tw_which_item(tp, &tw, &j);
	    f = tw->func;
	    a = tw->arg;
	    r = tw->ret + j;};

	tp->ns++;

	SC_LOCKOFF(tp->lock);

/* if this thread is told to finish then wrap it up */
	if (f == (PFPVoidAPV) _SC_fin_pool_thread)
	   {SC_LOCKON(tp->fin);
	    SC_THREAD_SIGNAL(tp->init);
	    tp->nf++;
	    SC_LOCKOFF(tp->fin);
	    break;}
	    
/* call the work function and save the result */
	else if (f != NULL)
	   {if (r != NULL)
	       *r = (*f)(a);
	    else
	       (*f)(a);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_POOL - initialize a pool of NT threads */

void _SC_init_pool(thread_pool *tp, int nt)
   {int i, ok;
    emu_thread_info *ti;

    if (nt >= 1)
       {tp->size = nt;
	tp->ns   = nt;
	tp->nf   =  0;
	tp->ni   =  0;

	tp->wrk = CMAKE_ARRAY(thread_work *, NULL, 3);

	for (i = 0; i < nt; i++)
	    {ti = _SC_get_thread_info(i+1);

	     ok = SC_THREAD_CREATE(ti->tid, _SC_pool_attr,
				   _SC_do_pool_thread, &ti->wrkid);
	     if (ok != 0)
		io_printf(stdout,
			  "THREAD CREATE FAILED %d (%d) - _SC_INIT_POOL\n",
			  ok, i);};

/* wait until all threads are initialized before returning */
	SC_LOCKON(tp->lock);

	while (tp->nf < tp->ns)
	   SC_COND_WAIT(tp->init, tp->lock);

	SC_LOCKOFF(tp->lock);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_POOL_THREADS - do N pieces of thread_work TW
 *                     - use threads in the pool
 */

static void _SC_do_pool_threads(thread_pool *tp, int n, thread_work *tw)
   {int i, j, nt;
    thread_work *twc;

    SC_LOCKON(tp->lock);

    SC_array_set_n(tp->wrk, 0);
    tp->nf = 0;
    tp->ns = 0;
    tp->ni = 0;

    for (i = 0; i < n; i++)
        {twc = tw + i;
	 nt  = twc->n_thread;
	 for (j = 0; j < nt; j++)
	     {SC_array_push(tp->wrk, &twc);
	      tp->ni++;};};

    tp->nxt = 0;

    SC_THREAD_BROADCAST(tp->go);

/* GOTCHA: the definition of cond_wait would seem to preclude the
 * necessity of looping while waiting for the threads to signal
 * their completion of their assigned work
 * Without this the thread pool termination function returns immediately
 * it does no work and hence gets all the signals out immediately
 * this does not cause a hang on the second cond_wait
 */
    SC_COND_WAIT(tp->init, tp->lock);

    SC_LOCKOFF(tp->lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_TPOOL - create a pool of NT threads
 *               - serial execution follows from NT less than 2
 */

void SC_init_tpool(int nt, PFTid tid)
   {

    if (nt > 1)
       {SC_init_threads(nt, tid);

#ifdef THREAD_CONTROL
	_SC_init_pool(&_SC_tp, nt);

	SC_thread_oper->use_pool = TRUE;
#endif
       };
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIN_TPOOL - close the threads in the pool and cleanup */

void SC_fin_tpool(void)
   {

#ifdef THREAD_CONTROL
    if (SC_n_threads > 1)
       {int it, nt;
	thread_work *tw;
	emu_thread_info *ti;

        nt = SC_n_threads;
	tw = _SC_make_thread_work(nt, _SC_fin_pool_thread, NULL);

	_SC_do_threads(1, tw);

	_SC_free_thread_work(tw, 1);

        for (it = 0; it < nt; it++)
	    {ti = _SC_get_thread_info(it+1);
             SC_THREAD_JOIN(ti->tid, NULL);};

	SC_free_array(_SC_tp, NULL);};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_THREADS - execute N pieces of thread_work TW
 *                - in a thread parallel fashion
 */

static void _SC_do_threads(int n, thread_work *tw)
   {

/* there is no work to do if n <= 0 */
    if (n > 0)
       {if ((n == 1) && (tw->n_thread == 1))
	   _SC_one_thread(-1, tw);

	else if (SC_thread_oper->use_pool)
           _SC_do_pool_threads(&_SC_tp, n, tw);

	else
	   _SC_do_new_threads(n, tw);};

    return;}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* SC_INIT_TPOOL - create a pool of NT threads
 *               - serial execution follows from NT less than 2
 */

void SC_init_tpool(int nt, PFTid tid)
   {

    if (nt > 1)
       SC_init_threads(nt, tid);
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIN_TPOOL - close the threads in the pool and cleanup */

void SC_fin_tpool(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_THREADS - do the N pieces of thread_work TW sequentially */

static void _SC_do_threads(int n, thread_work *tw)
   {int i;

    for (i = 0; i < n; i++)
        _SC_one_thread(i, tw+i);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
