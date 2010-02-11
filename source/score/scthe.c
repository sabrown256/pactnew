/*
 * SCTHRE.C - pthread emulation support routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_mem.h"
#include <sched.h>

/* these are increments for growing quantities */
#define N_KEYS        8
#define N_CONDS       8
#define N_LOCKS      32
#define N_THREADS    64

enum e_emu_lock_state
   {UNLOCKED = 0,
    LOCKED,
    ACTIVE,
    RELEASED};

typedef enum e_emu_lock_state emu_lock_state;
typedef struct s_emu_lock_info emu_lock_info;

struct s_emu_lock_info
   {int ide;
    int active;
    emu_lock_state cond;};

int
 _SC_init_emu_threads = FALSE;

SC_thread_lock
 SC_ts_lock = SC_LOCK_INIT_STATE;

static int
 _SC_use_emu_lock = FALSE;

static void
 _SC_eth_init(void);

/*--------------------------------------------------------------------------*/

/*                            THREAD ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_ETH_INIT_THREAD - initialize an emu_thread_info struct */

void _SC_eth_init_thread(emu_thread_info *ti, int id)
   {

    _SC_thread_error(id, "initializing thread state");

    ti->ide     = id;
    ti->running = FALSE;
    ti->tid     = SC_NULL_THREAD;
    ti->func    = NULL;
    ti->arg     = NULL;
    ti->val     = NULL;

    return;}

/*--------------------------------------------------------------------------*/

/*                              LOCK ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_ETH_PUSH_LOCK - add lock TL to _SC.eth_locks */

static void _SC_eth_push_lock(SC_thread_lock *tl, int n)
   {char name[MAXLINE];

    SC_array_push(_SC.eth_locks, &tl);

    snprintf(name, MAXLINE, "__lock__%d", n);

    tl->ita = SC_register_thread_data(name, "emu_lock_info", 1,
				      sizeof(emu_lock_info), NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_LOOKUP_LOCK - manage list of lock variables */

static int _SC_eth_lookup_lock(SC_thread_lock *tl)
   {int n, nl;
    SC_thread_lock **la;

    _SC_eth_init();

    nl = SC_array_get_n(_SC.eth_locks);
    la = SC_array_array(_SC.eth_locks);

    for (n = 0; n < nl; n++)
        {if (tl == la[n])
	    break;};

    if (n >= nl)
       {SC_LOCKON(SC_ts_lock);

	_SC_eth_push_lock(tl, n);

	SC_LOCKOFF(SC_ts_lock);};

    n = tl->ita;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_LOCK_INFO - return the emu_lock_info for this thread */

static emu_lock_info *_SC_eth_lock_info(int id, int il)
   {emu_lock_info *lock;

    lock = (emu_lock_info *) SC_get_thread_element_nl(&id, il, FALSE);
    lock->ide = id;

    return(lock);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_LOCK - emulated lock independent of the thread system used
 *              - each thread holds the state of its part of the lock
 *              -   UNLOCKED  - not yet locked wrt a LOCKON/LOCKOFF pair
 *              -   LOCKED    - currently locked
 *              -   ACTIVE    - the only active thread
 *              -   RELEASED  - unlocked after having been ACTIVE
 *              - at LOCKON each thread marks itself as LOCKED and
 *              - goes to sleep, until it is marked ACTIVE,
 *              - except for one thread specified by
 *              - the next_run member of the lock, LCK, which is
 *              - marked ACTIVE and returns
 *              - at LOCKOFF the sole ACTIVE thread marks itself as
 *              - RELEASED then marks another LOCKED thread as ACTIVE
 *              - and sets the next_run member of LCK to that thread
 *              - the last thread to complete reset all threads to
 *              - the UNLOCKED state
 *              - NOTE: incomplete - this needs be fixed for the following:
 *              -  1) st.nt is incremented before the new threads are
 *              -     completely initialized which hangs the locks
 *              -  2) need safe computation of the first thread to
 *              -     reach a lock rather than potentially having to
 *              -     wait for thread 0 to reach the lock
 */

static int _SC_eth_lock(SC_thread_lock *lck, int mode)
   {int i, nl, na, ni, la, ln, lu;
    int nt, il, lckd, init;
    emu_lock_info *lock, *locki;

    _SC_eth_init();

    nt   = SC_get_n_thread();
    il   = _SC_eth_lookup_lock(lck);
    lock = _SC_eth_lock_info(-1, il);

/* on */
    if (mode == TRUE)
       {while (lck->hold == TRUE);

	if (lock->ide != lck->next_run)
	   {lock->cond = LOCKED;
	    while (lock->cond == LOCKED)
	       sched_yield();}
	else
	   lock->cond = ACTIVE;

        lckd = TRUE;}

/* off */
    else if (mode == FALSE)
       {nl = 0;
	na = 0;
	ni = 0;
	ln = -1;
	la = -1;
	lu = -1;

	lck->hold = TRUE;

        for (i = 0; i < nt; i++)
	    {locki = _SC_eth_lock_info(i, il);

	     init = TRUE;
             if (init == TRUE)
	        {ni++;
		 switch (locki->cond)
		    {case LOCKED :
		          ln = i;
			  nl++;
			  break;

		     case ACTIVE :
			  la = i;
			  na++;
			  break;

		     case UNLOCKED :
			  if (lu == -1)
			     lu = i;
			  break;

		     default :
			  break;};};}

	lck->hold = FALSE;

/* complain if more than one active thread */
	if (na != 1)
           printf("error: more than one active thread %d\n", na);

	if (lock->ide != la)
           printf("error: unlock wrong thread %d not %d\n", lock->ide, la);

	lock->cond = RELEASED;
	na--;

/* if there is a next locked thread - mark and unlock it */
        if (ln != -1)
	   {lck->next_run = ln;
	    locki         = _SC_eth_lock_info(ln, il);
	    locki->cond   = ACTIVE;}

/* if all threads are released - unlock all threads and mark thread 0 */
	else if (lu == -1)
	   {lck->next_run = 0;
	    for (i = 0; i < nt; i++)
	        {locki       = _SC_eth_lock_info(i, il);
		 locki->cond = UNLOCKED;};}

/* if there are no locked threads - mark and unlock the next unlocked thread */
	else if ((nl == 0) && (na == 0))
	   {lck->next_run = lu;
	    locki         = _SC_eth_lock_info(lu, il);
	    locki->cond   = ACTIVE;};

	lckd = FALSE;}

/* init */
    else
       lckd = FALSE;

    return(lckd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_LOCKON - lock the threads */

int _SC_eth_lockon(SC_thread_lock *lck)
   {int rv;
    SC_thread tid;

    rv = TRUE;

    _SC_eth_init();

    if ((lck != NULL) && (_SC_init_emu_threads == TRUE))
       {tid = SC_THREAD_SELF();

/* NOTE: the outcome of this test cannot be effected by any thread except
 * the one that got through the lock to set lck->active
 * so this is a race but it can never cause incorrect behavior
 */
	if (tid != lck->active)
	   {if (_SC_use_emu_lock == TRUE)
	       rv = _SC_eth_lock(lck, TRUE);

	    else if (SC_thread_oper->lockon != NULL)
	       (*SC_thread_oper->lockon)(lck);

	    lck->active = tid;};

/* only the active thread manages the reference count */
	if (tid == lck->active)
	   lck->ref_count++;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_LOCKOFF - unlock the threads */

int _SC_eth_lockoff(SC_thread_lock *lck)
   {int rv;
    SC_thread tid;

    rv = TRUE;

    _SC_eth_init();

    if ((lck != NULL) && (_SC_init_emu_threads == TRUE))
       {tid = SC_THREAD_SELF();
	
/* only the active thread manages the reference count */
	if (tid == lck->active)
	   lck->ref_count--;

	if (lck->ref_count < 1)
	   {lck->ref_count = 0;
	    lck->active    = SC_NULL_THREAD;

	    if (_SC_use_emu_lock == TRUE)
	       rv = _SC_eth_lock(lck, FALSE);

	    else if (SC_thread_oper->lockoff != NULL)
	       (*SC_thread_oper->lockoff)(lck);};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               KEY ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_ETH_PUSH_KEY - add key TL to _SC.eth_keys */

static void _SC_eth_push_key(SC_thread_key *tk, int n)
   {char name[MAXLINE];

    SC_array_push(_SC.eth_keys, &tk);

    snprintf(name, MAXLINE, "__key__%d", n);

    *tk = SC_register_thread_data(name, "void *", 1,
				  sizeof(void *), NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_LOOKUP_KEY - manage list of key variables */

static int _SC_eth_lookup_key(SC_thread_key *tk)
   {int n, nk;
    SC_thread_key **ka;

    _SC_eth_init();

    SC_LOCKON(SC_ts_lock);

    nk = SC_array_get_n(_SC.eth_keys);
    ka = SC_array_array(_SC.eth_keys);

    for (n = 0; n < nk; n++)
        {if (tk == ka[n])
	    break;};

    if (n >= nk)
       _SC_eth_push_key(tk, n);

    n = *tk;

    SC_LOCKOFF(SC_ts_lock);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_CREATE_KEY - fill SC_CREATE_KEY role */

void _SC_eth_create_key(SC_thread_key *tk, void (*dest)(void *))
   {

    if (dest != NULL)
       (*dest)((void *) tk);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_SET_KEY - fill SC_SET_KEY role */

void _SC_eth_set_key(SC_thread_key *tk, void *val)
   {int itk;
    void **kd;

    SC_LOCKON(SC_ts_lock);

    itk = _SC_eth_lookup_key(tk);
    kd  = (void **) SC_get_thread_element(-1, itk);
    *kd = val;

    SC_LOCKOFF(SC_ts_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_GET_KEY - fill SC_GET_KEY role */

void *_SC_eth_get_key(SC_thread_key *tk)
   {int itk;
    void **kd, *rv;

    SC_LOCKON(SC_ts_lock);

    itk = _SC_eth_lookup_key(tk);
    kd  = (void **) SC_get_thread_element(-1, itk);
    rv  = *kd;

    SC_LOCKOFF(SC_ts_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              COND ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_ETH_LOOKUP_COND - manage list of cond variables */

emu_cond_info *_SC_eth_lookup_cond(SC_thread_cond *tc)
   {int n, nc;
    emu_cond_info cv, *ci, *rv;

    _SC_eth_init();

    nc = SC_array_get_n(_SC.eth_conds);
    ci = SC_array_array(_SC.eth_conds);

    for (n = 0; n < nc; n++)
        {if (tc == ci[n].var)
	    break;};

    IF_SAFE(TRUE, &SC_ts_lock, n >= nc)
       cv.var     = tc;
       cv.waiting = FALSE;

       SC_array_push(_SC.eth_conds, &cv);

       ci = SC_array_array(_SC.eth_conds);
    END_SAFE;

    rv = ci + n;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_COND_WAIT - fill SC_COND_WAIT role */

void _SC_eth_cond_wait(SC_thread_cond *tc, SC_thread_lock *lock)
   {emu_cond_info *ci;

    ci = _SC_eth_lookup_cond(tc);

    SC_LOCKOFF(*lock);

    ci->waiting = TRUE;

    while (ci->waiting)
       SC_sleep(1);

    SC_LOCKON(*lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_THREAD_SIGNAL - fill SC_THREAD_SIGNAL role */

void _SC_eth_thread_signal(SC_thread_cond *tc)
   {emu_cond_info *ci;

    ci = _SC_eth_lookup_cond(tc);

    ci->waiting = FALSE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_THREAD_BROADCAST - fill SC_THREAD_BROADCAST role */

void _SC_eth_thread_broadcast(SC_thread_cond *tc)
   {int i, nc;
    emu_cond_info *ci;

    nc = SC_array_get_n(_SC.eth_conds);
    ci = SC_array_array(_SC.eth_conds);

    for (i = 0; i < nc; i++)
        ci[i].waiting = FALSE;

    return;}

/*--------------------------------------------------------------------------*/

/*                            THREAD ARRAY ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* SC_DEFINE_THREAD_ARRAY - define a new per thread quantity */

int SC_define_thread_array(char *name, char *type, int ni,
			   void (*init)(void *a, int id))
   {int ita, bpi;

    SC_LOCKON(SC_ts_lock);

    bpi = SC_sizeof(type);
    ita = SC_register_thread_data(name, type, ni, bpi, init);

    SC_LOCKOFF(SC_ts_lock);

    return(ita);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_THREAD_ARRAY - return the array specified by ITA
 *                     - for the current thread
 */

void *SC_get_thread_array(int ita)
   {int id;
    void *rv;

    SC_LOCKON(SC_ts_lock);

    id = SC_current_thread();
    rv = SC_get_thread_element(id, ita);

    SC_LOCKOFF(SC_ts_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ETH_INIT - initialize emulated thread state */

static void _SC_eth_init(void)
   {

    ONCE_SAFE(_SC_init_emu_threads == TRUE, &SC_ts_lock)

       SC_THREAD_INIT_SYS();

       _SC.eth_locks = SC_MAKE_ARRAY("PERM|_SC_ETH_INIT", SC_thread_lock *, NULL);

       _SC_eth_push_lock(&SC_mm_lock,   0);
       _SC_eth_push_lock(&SC_mc_lock,   1);
       _SC_eth_push_lock(&SC_ts_lock,   2);

       _SC.eth_keys = SC_MAKE_ARRAY("PERM|_SC_ETH_INIT", SC_thread_key *, NULL);

       _SC_eth_push_key(&SC_thread_oper->ikey, 0);

       _SC.eth_conds = SC_MAKE_ARRAY("PERM|_SC_ETH_INIT", emu_cond_info, NULL);

       _SC_init_emu_threads = TRUE;

    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_THREAD_ERROR - complain with MSG if the current thread is not ID */

void _SC_thread_error(int id, char *msg)
   {

#if 0

/* NOTE: cannot use a pthread key based thread id scheme because
 * it relies on the per thread state being initialized and this
 * is often called from those initializers
 */

#ifndef PTHREAD_POSIX

    int ld;

    ld = SC_current_thread();
    if ((id != ld) && (ld != 0))
       fprintf(stdout, "THREAD ERROR: '%s' for %d from %d\n",
	       msg, id, ld);

#endif

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
