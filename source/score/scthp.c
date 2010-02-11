/*
 * SCTHP.C - PTHREAD parallel support routines
 *         - emulate the SCORE THREAD interface with PTHREAD calls
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

#ifdef PTHREAD_POSIX

#ifdef SOLARIS
# define HAVE_PTHREAD_STRUCT
#endif

#ifdef HAVE_PTHREAD_STRUCT
# define SC_PTHREAD_ONCE_INIT    { PTHREAD_ONCE_INIT }
#else
# define SC_PTHREAD_ONCE_INIT    PTHREAD_ONCE_INIT
#endif

#ifdef AIX
# define SC_CREATE_ATTACHED            PTHREAD_CREATE_UNDETACHED
#else
# define SC_CREATE_ATTACHED            PTHREAD_CREATE_JOINABLE
#endif
#define SC_CREATE_DETACHED             PTHREAD_CREATE_DETACHED

struct s_SC_thread_attr
   {char *id;
    void (*init)(void);
    int state;
    pthread_once_t once;
    pthread_attr_t pa;};

SC_thread_attr
 _SC_attr_detached = { "detached",
		       _SC_pth_attr_init,
		       SC_CREATE_DETACHED,
		       SC_PTHREAD_ONCE_INIT, },
 _SC_attr_attached = { "attached",
                       _SC_pth_attr_init,
		       SC_CREATE_ATTACHED,
		       SC_PTHREAD_ONCE_INIT, };

static SC_thread_attr
  *_SC_current_attr;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_ATTR_INIT - wrapper to setup pthread attr */

void _SC_pth_attr_init(void)
   {int state;
    pthread_attr_t *a;

    a     = &_SC_current_attr->pa;
    state = _SC_current_attr->state;

    pthread_attr_init(a);
    pthread_attr_setdetachstate(a, state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_TID - return the id of the current thread */

static void _SC_pth_tid(int *tid)
   {emu_thread_info *ti;

    ti   = _SC_get_thread_info(-1);
    *tid = ti->ide;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_LOCKON - fill SC_LOCKON role
 *                - check to see if lock has been initialized
 *                - then wait/set it
 */

static void _SC_pth_lockon(SC_thread_lock *lock)
   {

    if (SC_n_threads >= 1)
       pthread_mutex_lock(&lock->sys);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_LOCKOFF - fill SC_LOCKOFF role */

static void _SC_pth_lockoff(SC_thread_lock *lock)
   {

    if (SC_n_threads >= 1)
       pthread_mutex_unlock(&lock->sys);

    return;}

/*--------------------------------------------------------------------------*/

/*                            THREAD ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_PTH_THREAD_EQUAL - fill SC_THREAD_EQUAL role */

static int _SC_pth_thread_equal(SC_thread t1, SC_thread t2)
   {int rv;

    rv = pthread_equal(t1, t2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_THREAD_SELF - fill SC_THREAD_SELF role */

static SC_thread _SC_pth_thread_self(void)
   {SC_thread id;

    id = pthread_self();

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_THREAD_CREATE - fill SC_THREAD_CREATE role */

static int _SC_pth_thread_create(SC_thread *thread, SC_thread_attr *attr,
				 void *(*func)(void *), void *arg)
   {int rv;

    _SC_current_attr = attr;

    pthread_once(&attr->once, attr->init);

    rv = pthread_create(thread, &attr->pa, func, arg);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_THREAD_JOIN - fill SC_THREAD_JOIN role */

static void _SC_pth_thread_join(SC_thread *thread, void **status)
   {

    pthread_join(*thread, status);

    return;}

/*--------------------------------------------------------------------------*/

/*                           THREAD SAFE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_PTH_STRTOK - portable version of thread safe strtok_r */

static char *_SC_pth_strtok(char *s, char *dlm, char **p)
   {char *rv;

    if (s != NULL)
       {*p = NULL;
	rv = strtok_r(s, dlm, p);}

    else
       rv = strtok_r(s, dlm, p);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_CTIME - portable version of thread safe ctime_r */

static char *_SC_pth_ctime(const time_t *t, char *s, int sz)
   {

#ifdef HAVE_3ARG_CTIME
    ctime_r(t, s, sz);
#else
    ctime_r(t, s);
#endif

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_TTYNAME - fill SC_THREAD_BROADCAST role */

static char *_SC_pth_ttyname(int fd, char *name, size_t sz)
   {int rv;

    rv = ttyname_r(fd, name, sz);

    return(name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PTH_RAND - portable thread safe random number generator */

static int _SC_pth_rand(unsigned int *seed)
   {int rv;

    rv = rand_r(seed);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

threades
 _SC_pth_oper = { NULL,
	          _SC_pth_thread_self,
	          _SC_pth_thread_equal,
	          _SC_pth_thread_create,
	          _SC_pth_tid,
	          _SC_pth_lockon,
	          _SC_pth_lockoff,
	          _SC_pth_thread_join,
	          _SC_pth_strtok,
	          _SC_pth_ctime,
	          _SC_pth_ttyname,
	          _SC_pth_rand,},
 *SC_thread_oper = &_SC_pth_oper;
#endif

