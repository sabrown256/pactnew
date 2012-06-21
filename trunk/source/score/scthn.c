/*
 * SCTHN.C - non threaded emulation of the SCORE THREAD interface
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

#ifndef HAVE_THREADS

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_TID - return the id of the current thread */

static void _SC_nth_tid(int *tid)
   {emu_thread_info *ti;

    ti   = _SC_get_thread_info(-1);
    *tid = ti->ide;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_LOCKON - fill SC_LOCKON role */

static void _SC_nth_lockon(SC_thread_lock *lock)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_LOCKOFF - fill SC_LOCKOFF role */

static void _SC_nth_lockoff(SC_thread_lock *lock)
   {

    return;}

/*--------------------------------------------------------------------------*/

/*                            THREAD ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SC_NTH_THREAD_EQUAL - fill SC_THREAD_EQUAL role */

static int _SC_nth_thread_equal(SC_thread t1, SC_thread t2)
   {int rv;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_THREAD_SELF - fill SC_THREAD_SELF role */

static SC_thread _SC_nth_thread_self(void)
   {SC_thread id;

    id = 0;

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_THREAD_CREATE - fill SC_THREAD_CREATE role */

static int _SC_nth_thread_create(SC_thread *thread, SC_thread_attr *attr,
				 void *(*func)(void *), void *arg)
   {int rv;

    (*func)(arg);

    *thread = 1;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_THREAD_JOIN - fill SC_THREAD_JOIN role */

static void _SC_nth_thread_join(SC_thread *thread, void **status)
   {

    *thread = 0;

    return;}

/*--------------------------------------------------------------------------*/

/*                           THREAD SAFE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_NTH_STRTOK - portable version of thread safe strtok_r */

static char *_SC_nth_strtok(char *s, char *dlm, char **p)
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

/* _SC_NTH_CTIME - portable version of thread safe ctime_r */

static char *_SC_nth_ctime(const time_t *t, char *s, int sz)
   {

    SC_strncpy(s, sz, ctime(t), -1);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_TTYNAME - fill SC_THREAD_BROADCAST role */

static char *_SC_nth_ttyname(int fd, char *name, size_t sz)
   {

    SC_strncpy(name, sz, SYS_TTYNAME(fd), -1);

    return(name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NTH_RAND - portable thread safe random number generator */

static int _SC_nth_rand(unsigned int *seed)
   {int rv;

    rv = rand();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

SC_scope_thread
 _SC_ts = { FALSE, 0, SC_LOCK_INIT_STATE, 0, 1,
            { NULL,
	      _SC_nth_thread_self,
	      _SC_nth_thread_equal,
	      _SC_nth_thread_create,
	      _SC_nth_tid,
	      _SC_nth_lockon,
	      _SC_nth_lockoff,
	      _SC_nth_thread_join,
	      _SC_nth_strtok,
	      _SC_nth_ctime,
	      _SC_nth_ttyname,
	      _SC_nth_rand,} };

#endif

