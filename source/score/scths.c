/*
 * SCTHS.C - SGI sproc parallel support routines
 *         - emulate the SCORE THREAD interface with SPROC calls
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

#ifdef PTHREAD_SPROC

#include <ulocks.h>

#define N_TICKS          1

/* these are increments for growing quantities */
#define N_KEYS        8
#define N_CONDS       8
#define N_THREADS    64

struct s_sproc_lock
   {int initted;
    abilock_t lock;};

SC_thread_attr
 _SC_attr_detached = 0,
 _SC_attr_attached = 1;

SC_array
 *_SC_sproc_threads,
 *_SC_sproc_conds,
 *_SC_sproc_keys;

extern SC_thread_lock
 SC_mm_lock,
 SC_mc_lock;

static SC_thread_lock
 SC_LOOK_LOCK   = SC_LOCK_INIT_STATE,
 SC_LOOK_KEY    = SC_LOCK_INIT_STATE,
 SC_LOOK_THREAD = SC_LOCK_INIT_STATE,
 SC_LOOK_COND   = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/

/*                              LOCK ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_SPROC_INIT - initialize locks for SCORE use 
 *                - must be done in SC_init_threads 
 */

void _SC_sproc_init(void)
   {

    init_lock(&SC_mm_lock.lock);
    SC_mm_lock.initted = TRUE;

    init_lock(&SC_mc_lock.lock);
    SC_mc_lock.initted = TRUE;

    init_lock(&SC_LOOK_LOCK.lock);
    SC_LOOK_LOCK.initted = TRUE;

    init_lock(&SC_LOOK_KEY.lock);
    SC_LOOK_KEY.initted = TRUE;

    init_lock(&SC_LOOK_THREAD.lock);
    SC_LOOK_THREAD.initted = TRUE;

    init_lock(&SC_LOOK_COND.lock);
    SC_LOOK_COND.initted = TRUE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_LOCKON - fill SC_LOCKON role
 *                  - check to see if lock has been initialized
 *                  - then wait/set it
 */

static void _SC_sproc_lockon(SC_thread_lock *lock)
   {int i;

    if (!lock->initted)
       {spin_lock(&SC_LOOK_LOCK.lock);
    
        if (!lock->initted)
           {init_lock(&(lock->lock));
            lock->initted = TRUE;}

        release_lock(&SC_LOOK_LOCK.lock);}
        
    spin_lock(&(lock->lock));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_LOCKOFF - fill SC_LOCKOFF role */

static void _SC_sproc_lockoff(SC_thread_lock *lock)
   {

    release_lock(&(lock->lock));

    return;}

/*--------------------------------------------------------------------------*/

/*                            THREAD ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_SPROC_INIT_THREAD - initialize an emu_thread_info struct */

static void _SC_sproc_init_thread(emu_thread_info *pt,
				  SC_thread *th, int nk, int set)
   {

    pt->pid      = -1;
    pt->thread   = th;
    pt->running  = FALSE;
    pt->func     = NULL;
    pt->arg      = NULL;
    pt->val      = NULL;

    if (set)
       pt->key_data = NULL;

    if (pt->key_data != NULL)
       SFREE(pt->key_data);

    if (nk > 0)
       pt->key_data = NMAKE_N(void *, nk, "_SC_SPROC_INIT_THREAD:key_data");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_LOOKUP_THREAD - manage list of thread variables */

static emu_thread_info *_SC_sproc_lookup_thread(SC_thread *thread)
   {int n;
    emu_thread_info tv, *rv, *ta;

    if (_SC_sproc_threads == NULL)
       _SC_sproc_threads = SC_MAKE_ARRAY("_SC_SPROC_LOOKUP_THREAD",
					 _emu_thread_info, NULL);

    nt = SC_array_get_n(_SC_sproc_threads);
    ta = SC_array_array(_SC_sproc_threads);

/* is there a current thread? */
    for (n = 0; n < nt; n++)
        {if (ta[n].thread == thread)
	    break;};

/* if not setup a thread for use */
    if (n >= nt)
       {SC_LOCKON(SC_LOOK_THREAD);

	_SC_sproc_init_thread(&tv, thread, SC_array_get_n(_SC_sproc_keys), TRUE);

/* is there an available idle thread? */
	for (n = 0; n < nt; n++)
	    {if (ta[n]->thread == NULL)
	        {ta[n] = tv;
		 break;};};

/* if not add a new one */
	if (n >= nt)
	   SC_array_push(_SC_sproc_threads, &tv);

	SC_LOCKOFF(SC_LOOK_THREAD);};

    rv = ta + n;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_THREAD_SELF - fill SC_THREAD_SELF role */

static SC_thread _SC_sproc_thread_self(void)
   {int pid, i, np;
    SC_thread id;
    emu_thread_info *ta;

    pid = getpid();
    id  = 0;
    np  = SC_array_get_n(_SC_sproc_threads);
    ta  = SC_array_array(_SC_sproc_threads);
    for (i = 0; i < np; i++)
        {if (ta[i].pid == pid)
	    {id = ta + i;
	     break;};};

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_DO_THREAD - help out here */

static void _SC_sproc_do_thread(void *x)
   {PFPVoidAPV func;
    void *arg;
    emu_thread_info *pdat;

    pdat = (emu_thread_info *) x;

    func = pdat->func;
    arg  = pdat->arg;

    pdat->pid     = getpid();
    pdat->val     = (*func)(arg);
    pdat->running = FALSE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_TID - return the id of the current thread */

static void _SC_sproc_tid(int *tid)
   {emu_thread_info *ti;

    ti   = _SC_get_thread_info(-1);
    *tid = ti->ide;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_THREAD_CREATE - fill SC_THREAD_CREATE role */

static int _SC_sproc_thread_create(SC_thread *th, int attr,
				   void *(*func)(void *), void *arg)
   {int inh, id, rv;
    pid_t ok;
    emu_thread_info *ti;

    ONCE_SAFE(TRUE, NULL)
       if (SC_n_threads >= 8)
	  {id = usconfig(CONF_INITUSERS, SC_n_threads+1);
	   if (id == -1)
	      io_printf(stdout,
			"USCONFIG FAILED %d (%d) - _SC_SPROC_THREAD_CREATE\n",
			errno, SC_n_threads);};
    END_SAFE;

    inh = PR_SADDR | PR_SUMASK | PR_SID;

    ti = _SC_sproc_lookup_thread(th);

    ti->running = TRUE;
    ti->func    = func;
    ti->arg     = arg;
    ti->val     = NULL;

    rv  = 0;
    *th = ti;

    ok = sproc(_SC_sproc_do_thread, inh, ti);

    if (ok == -1)
       {rv          = errno;
	*th         = NULL;
	ti->thread  = NULL;
	ti->running = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_THREAD_JOIN - fill SC_THREAD_JOIN role */

static void _SC_sproc_thread_join(SC_thread *thread, void **status)
   {void *val;
    emu_thread_info *ti;

    ti = *thread;

    while (ti->running)
       sginap(N_TICKS);

    if (status != NULL)
       {val     = ti->val;
        *status = val;};

    _SC_sproc_init_thread(ti, NULL, 0, FALSE);

    return;}

/*--------------------------------------------------------------------------*/

/*                           THREAD SAFE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_SPROC_STRTOK - portable version of thread safe strtok_r */

static char *_SC_sproc_strtok(char *s, char *dlm, char **p)
   {char *rv;

    if (s != NULL)
       {*p = NULL;
	rv = strtok(s, dlm);}

    else
       rv = strtok(s, dlm);

    *p = rv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_CTIME - portable version of thread safe ctime_r */

static char *_SC_sproc_ctime(const time_t *t, char *s, int sz)
   {

    SC_nstrcpy(s, sz, ctime(t), -1);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_TTYNAME - fill SC_THREAD_BROADCAST role */

static char *_SC_sproc_ttyname(int fd, char *name, size_t sz)
   {

    SC_strncpy(name, sz, ttyname(fd), -1);

    return(name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPROC_RAND - portable thread safe random number generator */

static int _SC_sproc_rand(unsigned int *seed)
   {int rv;

    rv = rand();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

threades
 _SC_sproc_oper = { _SC_sproc_init,
		    _SC_sproc_thread_self,
		    _SC_sproc_thread_equal,
		    _SC_sproc_thread_create,
		    _SC_sproc_tid,
		    _SC_sproc_lockon,
		    _SC_sproc_lockoff,
		    _SC_sproc_thread_join,
		    _SC_sproc_strtok,
		    _SC_sproc_ctime,
		    _SC_sproc_ttyname,
		    _SC_sproc_rand,},
 SC_thread_oper = &_SC_sproc_oper;

#endif

