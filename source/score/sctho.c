/*
 * SCTHO.C - OMP parallel support routines
 *         - emulate the SCORE THREAD interface with OMP calls
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

#ifdef PTHREAD_OMP

typedef struct s_omp_funarg omp_funarg;

struct s_omp_funarg
   {void *a;
    void *(*f)(void *a);};

static SC_array
 *_SC_omp_arr = NULL;

SC_thread_attr
 _SC_attr_detached = 0,
 _SC_attr_attached = 1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_INIT - wrapper to setup OpenMP session */

static void _SC_omp_init(void)
   {int nt;

    if (_SC_init_emu_threads == TRUE)
       {ONCE_SAFE(FALSE, NULL)
	   nt = omp_get_num_threads();
	   omp_set_num_threads(nt);
	END_SAFE;

	if (SC_n_threads > 0)
	   omp_set_num_threads(SC_n_threads);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_INIT_LOCK - initialize a lock */

static void _SC_omp_init_lock(SC_thread_sys_lock *sys)
   {omp_lock_t *lck;

    lck = &sys->lock;

    if (SC_n_threads < 2)
       {_SC_omp_init();
	if (sys->init == FALSE)
	   {omp_init_lock(lck);
	    sys->init = TRUE;};}

    else
       {

#pragma omp critical (lockit)
	{while (sys->init == FALSE)
	    {omp_init_lock(lck);
	     sys->init = TRUE;};};

       };

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_DO_WORK - do threaded work */

static void _SC_omp_do_work(void **rv)
   {int i, n;
    omp_funarg *a;

    n = SC_array_get_n(_SC_omp_arr);

    if (n > 0)
       {a = SC_array_array(_SC_omp_arr);

	if (rv == NULL)
	   {

#pragma omp parallel for schedule(static, 1)
	for (i = 0; i < n; i++)
	    (*a[i].f)(a[i].a);}

	else
	   {

#pragma omp parallel for schedule(static, 1)
	for (i = 0; i < n; i++)
	    rv[i] = (*a[i].f)(a[i].a);};};

    SC_array_set_n(_SC_omp_arr, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_TID - return the id of the current thread */

static void _SC_omp_tid(int *tid)
   {

    *tid = omp_get_thread_num();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_LOCKON - fill SC_LOCKON role
 *                - check to see if lock has been initialized
 *                - then wait/set it
 */

static void _SC_omp_lockon(SC_thread_lock *lock)
   {SC_thread_sys_lock *sys;
    omp_lock_t *lck;

    if (_SC_init_emu_threads == TRUE)
       {sys = &lock->sys;
	lck = &sys->lock;

	_SC_omp_init_lock(sys);

	omp_set_lock(lck);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_LOCKOFF - fill SC_LOCKOFF role */

static void _SC_omp_lockoff(SC_thread_lock *lock)
   {SC_thread_sys_lock *sys;
    omp_lock_t *lck;

    if (_SC_init_emu_threads == TRUE)
       {sys = &lock->sys;
	lck = &sys->lock;

	_SC_omp_init_lock(sys);

	omp_unset_lock(lck);};

    return;}

/*--------------------------------------------------------------------------*/

/*                            THREAD ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_OMP_THREAD_EQUAL - fill SC_THREAD_EQUAL role */

static int _SC_omp_thread_equal(SC_thread t1, SC_thread t2)
   {int rv;

    rv = (t1 == t2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_THREAD_SELF - fill SC_THREAD_SELF role */

static SC_thread _SC_omp_thread_self(void)
   {SC_thread id;

    id = omp_get_thread_num();

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_THREAD_CREATE - fill SC_THREAD_CREATE role */

static int _SC_omp_thread_create(SC_thread *thread, SC_thread_attr *attr,
				 void *(*func)(void *a), void *arg)
   {int rv;
    omp_funarg a;

    a.a = arg;
    a.f = func;

    if (_SC_omp_arr == NULL)
       _SC_omp_arr = SC_MAKE_ARRAY("_SC_OMP_THREAD_CREATE", omp_funarg, NULL);

    SC_array_push(_SC_omp_arr, &a);

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_THREAD_JOIN - fill SC_THREAD_JOIN role */

static void _SC_omp_thread_join(SC_thread *thread, void **rv)
   {

    _SC_omp_do_work(rv);

    return;}

/*--------------------------------------------------------------------------*/

/*                           THREAD SAFE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_OMP_STRTOK - portable version of thread safe strtok_r */

static char *_SC_omp_strtok(char *s, char *dlm, char **p)
   {char *rv;

    if (s != NULL)
       {*p = NULL;
	rv = strtok_r(s, dlm, p);}

    else
       rv = strtok_r(s, dlm, p);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_CTIME - portable version of thread safe ctime_r */

static char *_SC_omp_ctime(const time_t *t, char *s, int sz)
   {

#ifdef HAVE_3ARG_CTIME
    ctime_r(t, s, sz);
#else
    ctime_r(t, s);
#endif

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_TTYNAME - fill SC_THREAD_BROADCAST role */

static char *_SC_omp_ttyname(int fd, char *name, size_t sz)
   {int rv;

    rv = ttyname_r(fd, name, sz);

    return(name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OMP_RAND - portable thread safe random number generator */

static int _SC_omp_rand(unsigned int *seed)
   {int rv;

    rv = rand_r(seed);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

threades
 _SC_omp_oper = { _SC_omp_init,
	          _SC_omp_thread_self,
	          _SC_omp_thread_equal,
	          _SC_omp_thread_create,
	          _SC_omp_tid,
	          _SC_omp_lockon,
	          _SC_omp_lockoff,
	          _SC_omp_thread_join,
	          _SC_omp_strtok,
	          _SC_omp_ctime,
	          _SC_omp_ttyname,
	          _SC_omp_rand,},
 *SC_thread_oper = &_SC_omp_oper;

#endif

