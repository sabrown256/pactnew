/*
 * SCOPE_THREAD.H - header defining SCORE thread scope
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_THREAD
#define PCK_SCOPE_THREAD

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                             GENERIC MACROS                               */

/*--------------------------------------------------------------------------*/

#define SC_NULL_THREAD           ((SC_thread) -2)
#define SC_THREAD_ID(_t)         int _t = SC_current_thread()
#define SC_LOCKON(_l)            _SC_eth_lockon(&(_l))
#define SC_LOCKOFF(_l)           _SC_eth_lockoff(&(_l))
#define SC_CREATE_KEY(_k, _d)    _SC_eth_create_key(&(_k), _d)
#define SC_SET_KEY(_t, _k, _v)   _SC_eth_set_key(&(_k), _v)
#define SC_GET_KEY(_t, _k)       *((_t *) _SC_eth_get_key(&(_k)))
#define SC_GET_PKEY(_t, _k)      ((_t *) _SC_eth_get_key(&(_k)))
#define SC_COND_WAIT(_c, _l)     _SC_eth_cond_wait(&(_c), &(_l))
#define SC_THREAD_SIGNAL(_c)     _SC_eth_thread_signal(&(_c))
#define SC_THREAD_BROADCAST(_c)  _SC_eth_thread_broadcast(&(_c))

#define SC_THREAD_INIT_SYS()                                                 \
   {if (_SC_ts.oper.init != NULL)                                        \
       (*_SC_ts.oper.init)();}

#define SC_THREAD_SELF()                                                     \
   ((_SC_ts.oper.self == NULL) ? (SC_thread) 0 :                         \
    (*_SC_ts.oper.self)())

#define SC_THREAD_CREATE(_t, _a, _f, _v)                                     \
   ((_SC_ts.oper.create == NULL) ? 0 :                                   \
    (*_SC_ts.oper.create)(&(_t), _a,                                     \
                              (PFPVoidAPV) (_f), (void *) (_v)))

#define SC_THREAD_JOIN(_t, _s)                                               \
   {if (_SC_ts.oper.join != NULL)                                        \
       (*_SC_ts.oper.join)(&(_t), _s);}

#define SC_strtok(_s, _d, _n)                                                \
   ((char *) ((_SC_ts.oper.strtok == NULL) ? NULL :                      \
	      (*_SC_ts.oper.strtok)(_s, _d, &(_n))))

#define SC_ctime(_t, _s, _n)                                                 \
   ((char *) ((_SC_ts.oper.ctime == NULL) ? NULL :                       \
	      (*_SC_ts.oper.ctime)(&(_t), _s, _n)))

#define SC_ttyname(_f, _s, _n)                                               \
   ((char *) ((_SC_ts.oper.ttyname == NULL) ? NULL :                     \
	      (*_SC_ts.oper.ttyname)(_f, _s, _n)))

#define SC_rand(_n)                                                          \
   ((_SC_ts.oper.rand == NULL) ? 0 :                                     \
    (*_SC_ts.oper.rand)(&(_n)))

#define SC_THREAD_ATTR  SC_DETACHED_THREAD

#define SC_DETACHED_THREAD(_a)                                              \
   SC_thread_attr *_a = &_SC_ts.detached

#define SC_ATTACHED_THREAD(_a)                                              \
   SC_thread_attr *_a = &_SC_ts.attached

#define SC_THREAD_LOCK(_l)             SC_thread_lock _l = SC_LOCK_INIT_STATE


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                              OMP THREADS                                 */

/*--------------------------------------------------------------------------*/

#ifdef SMP_OpenMP

# ifndef GCC_FPE_X86
#  include <omp.h>

#  define HAVE_THREADS

typedef int                   SC_thread;
typedef int                   SC_thread_cond;
typedef struct s_omp_lockdes  SC_thread_sys_lock;
typedef int                   SC_thread_attr;

struct s_omp_lockdes
   {int init;
    omp_lock_t lock;};

#  define SC_LOCK_INIT_STATE    { -1, 0, 0, 0, SC_NULL_THREAD, { 0, } }
#  define SC_COND_INIT_STATE    0
# endif

#endif

/*--------------------------------------------------------------------------*/

/*                             POSIX PTHREADS                               */

/*--------------------------------------------------------------------------*/

#if (defined SMP_Pthread)

# ifdef SOLARIS
#  define _REENTRANT
#  define _POSIX_PTHREAD_SEMANTICS
# endif

#ifdef S_SPLINT_S
# include <bits/pthreadtypes.h>
#else
# include <pthread.h>
#endif

# define HAVE_THREADS

typedef pthread_t         SC_thread;
typedef pthread_cond_t    SC_thread_cond;
typedef pthread_mutex_t   SC_thread_sys_lock;
typedef struct s_SC_thread_attr  SC_thread_attr;

struct s_SC_thread_attr
   {char *id;
    void (*init)(void);
    int state;
    pthread_once_t once;
    pthread_attr_t pa;};

# define SC_LOCK_INIT_STATE    { -1, 0, 0, 0, SC_NULL_THREAD, PTHREAD_MUTEX_INITIALIZER }
# define SC_COND_INIT_STATE    PTHREAD_COND_INITIALIZER

#endif

/*--------------------------------------------------------------------------*/

/*                           SGI SPROC THREADS                              */

/*--------------------------------------------------------------------------*/

#ifdef PTHREAD_SPROC

# include <sys/prctl.h>
# include <abi_mutex.h>

# define HAVE_THREADS

typedef struct s_sproc_lock sproc_lock;

typedef sproc_thread_info *SC_thread;
typedef int               *SC_thread_cond;
typedef sproc_lock        *SC_thread_sys_lock;
typedef int                SC_thread_attr;

# define SC_LOCK_INIT_STATE    { -1, 0, 0, 0, SC_NULL_THREAD, 0 }
# define SC_COND_INIT_STATE    0

#endif

/*--------------------------------------------------------------------------*/

/*                               NO THREADS                                 */

/*--------------------------------------------------------------------------*/

#ifndef HAVE_THREADS

typedef int  SC_thread;
typedef int  SC_thread_cond;
typedef int  SC_thread_sys_lock;
typedef int  SC_thread_attr;

# define SC_LOCK_INIT_STATE    { -1, 0, 0, 0, SC_NULL_THREAD, 0 }
# define SC_COND_INIT_STATE    0

#endif

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

#include <time.h>

typedef void (*PFTinit)(void *a, int id);
typedef void (*PFTid)(int *tid);
typedef int SC_thread_key;
typedef struct s_SC_scope_thread SC_scope_thread;
typedef struct s_SC_thread_lock SC_thread_lock;
typedef struct s_threades threades;
typedef struct s_thread_info thread_info;
typedef struct s_emu_thread_info emu_thread_info;
typedef struct s_emu_cond_info emu_cond_info;

struct s_SC_thread_lock
   {int ita;                    /* per thread state index for this lock */
    int next_run;               /* id of thread that will run next */
    int hold;                   /* hold locks flag */
    int ref_count;              /* count to control nesting of this lock */
    SC_thread active;           /* id of the active thread */
    SC_thread_sys_lock sys;};   /* thread system lock for bootstrapping */

struct s_threades
   {void (*init)(void);
    SC_thread (*self)(void);
    int (*equal)(SC_thread t1, SC_thread t2);
    int (*create)(SC_thread *thread, SC_thread_attr *attr,
		  void *(*func)(void *), void *arg);
    void (*id)(int *tid);
    void (*lockon)(SC_thread_lock *lock);
    void (*lockoff)(SC_thread_lock *lock);
    void (*join)(SC_thread *thread, void **status);
    char *(*strtok)(char *s, char *d, char **p);
    char *(*ctime)(const time_t *t, char *s, int sz);
    char *(*ttyname)(int fd, char *name, size_t sz);
    int (*rand)(unsigned *seed);

    int init_ikey;             /* TRUE iff ikey has been initialized */
    int use_pool;              /* using thread pool flag */
    SC_thread_key ikey;};

struct s_SC_scope_thread
   {int init_emu;
    int n_threads;
    int in_thread_region;
    SC_thread_lock lock;
    SC_thread_attr detached;
    SC_thread_attr attached;
    threades oper;};


/* PTHREAD emulation auxilliary structures */

struct s_emu_thread_info
   {int ide;                  /* external thread id */
    int running;
    int wrkid;                /* id of working thread */
    int exeid;                /* id of thread calling func */
    SC_thread tid;            /* thread system thread id */
    void *(*func)(void *);
    void *arg;
    void *val;};

struct s_emu_cond_info
   {int waiting;
    SC_thread_cond *var;};

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SC_scope_thread
 _SC_ts;

extern int
 SC_n_threads;

extern SC_thread_lock
 SC_ts_lock;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SCMEMT.C declarations */

extern void
 SC_register_thread_state(void),
 *SC_get_thread_element_nl(int *pit, int ie, int lck),
 *SC_get_thread_element(int id, int n),
 **SC_get_thread_elements(void *a, int *pne, int it, int lck),
 **SC_get_element_threads(void *a, int *pnt, int ie, int lck),
 **SC_get_threads_elements(void *a, int *pnt, int *pne, int lck),
 *SC_put_thread_state(int id, int n, void *vl),
 SC_single_thread(void);

extern int
 SC_register_thread_data(char *name, char *type, int ni, size_t bpi,
			 PFTinit init),
 SC_get_n_thread(void),
 SC_get_n_thread_elements(void);


/* SCTHE.C declarations */

extern emu_cond_info
 *_SC_eth_lookup_cond(SC_thread_cond *tc);

extern void
 _SC_eth_init_thread(emu_thread_info *ti, int id),
 _SC_eth_create_key(SC_thread_key *tk, void (*dest)(void *)),
 _SC_eth_set_key(SC_thread_key *tk, void *val),
 _SC_eth_thread_signal(SC_thread_cond *tc),
 _SC_eth_thread_broadcast(SC_thread_cond *tc),
 *_SC_eth_get_key(SC_thread_key *tk),
 _SC_eth_cond_wait(SC_thread_cond *tc, SC_thread_lock *lock),
 _SC_eth_thread_signal(SC_thread_cond *tc),
 _SC_eth_thread_broadcast(SC_thread_cond *tc),
 *SC_get_thread_array(int ita),
 _SC_thread_error(int id, char *msg);

extern int
 SC_define_thread_array(char *name, char *type, int ni,
			void (*init)(void *a, int id)),
 _SC_eth_lockon(SC_thread_lock *lck),
 _SC_eth_lockoff(SC_thread_lock *lck);


/* SCPTH.C declarations */

extern void
 _SC_pth_attr_init(void);


/* SCOMP.C declarations */

extern void
 _SC_omp_attr_init(void);


#ifdef __cplusplus
}
#endif

#endif

