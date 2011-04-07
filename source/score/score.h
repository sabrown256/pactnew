/*
 * SCORE.H - include file for PACT standard core package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCORE

#define PCK_SCORE                                 /* SCORE package resident */

#include "cpyright.h"
#include "scstd.h"
#include "score_old.h"
#include "scope_hash.h"
#include "scope_typeh.h"
#include "scope_io.h"
#include "scope_array.h"
#include "scope_lex.h"

/* version designation of CODE */
#ifndef VERSION
# define VERSION   SC_gs.version
#endif

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

#define UNCOLLECT       SHRT_MAX

#define ERRDEV  stderr                /* device to dump diagnostic messages */

#define SC_NSIG 65

#undef SIZEOF
#define SIZEOF (*SC_sizeof_hook)

#undef CONTAINER
#define CONTAINER (*SC_gs.type_container)

#define OTOL    (*SC_gs.otol)
#define HTOL    (*SC_gs.htol)
#define ATOL    (*SC_gs.atol)
#define ATOF    (*SC_gs.atof)
#define STRTOD  (*SC_gs.strtod)
#define STRTOL  (*SC_gs.strtol)
#define STRTOLL strtoll

#ifndef SYSTEM
# define SYSTEM SC_system
#endif

#ifndef ALARM
# define ALARM alarm
#endif

#ifndef SIGNAL_FUNCTION
# define SIGNAL_FUNCTION signal
#endif

#define SIGNAL(sig, fnc)  SIGNAL_FUNCTION(sig, fnc)


#ifndef REMOVE
#define REMOVE remove
#endif

#undef CMAKE
#undef CMAKE_N
#undef CREMAKE
#undef CFREE

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* NREMAKE - reallocate a block of type _t and return a pointer to it */

#define NREMAKE(p, _t, n)                                                    \
   (p = (_t *) (*SC_gs.mm.realloc)((void *) p, (long) (n),                   \
				   (long) sizeof(_t), TRUE, -1))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSTRSAVE - copy a string */

#define CSTRSAVE(_t)    SC_strsavec(_t, __func__, __FILE__, __LINE__)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CMAKE - memory allocation and bookkeeping macro */

#define CMAKE(_t)                                                            \
    ((_t *) (*SC_gs.mm.nalloc)(1L, (long) sizeof(_t), FALSE, -1,             \
			       __func__, __FILE__, __LINE__))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CMAKE_N - allocate a block of type _t and return a pointer to it */

#define CMAKE_N(_t, n)                                                       \
    ((_t *) (*SC_gs.mm.nalloc)((long) n, (long) sizeof(_t), FALSE, -1,       \
			       __func__, __FILE__, __LINE__))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CREMAKE - reallocate a block of type _t and return a pointer to it */

#define CREMAKE(p, _t, n)                                                    \
   (p = (_t *) (*SC_gs.mm.realloc)((void *) p, (long) (n),                   \
				   (long) sizeof(_t), FALSE, -1))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CFREE - release memory and do bookkeeping */

#define CFREE(x)                                                             \
   {(*SC_gs.mm.free)(x, -1);                                                 \
    x = NULL;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ADD_VALUE_ALIST - add a scalar value to an alist */

#define SC_ADD_VALUE_ALIST(lst, _t, str, name, val)                          \
   {_t *pt;                                                                  \
    pt  = CMAKE(_t);                                                         \
    *pt = val;                                                               \
    lst = SC_add_alist(lst, name, str, pt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHANGE_VALUE_ALIST - change a scalar value in an alist */

#define SC_CHANGE_VALUE_ALIST(lst, _t, str, name, val)                       \
   {_t *pt;                                                                  \
    pt  = CMAKE(_t);                                                         \
    *pt = val;                                                               \
    lst = SC_change_alist(lst, name, str, (void *) pt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT - setup certain universal things for applications, i.e.
 *         - interrupt handler
 *         - top level longjump
 *         - output buffering
 *         - this must be a macro to circumvent implementations that
 *         - won't let you LONGJMP back into function from which a
 *         - return has been done
 *         - Prototype arguments would be:
 *         -   char *_msg;
 *         -   void (*_ef)(int err);
 *         -   int _sh;
 *         -   void (*_sf)(int sig);
 *         -   int _bh;
 *         -   char *_bf;
 *         -   int _bsz;
 */

#define SC_init(_msg, _ef, _sh, _sf, _bh, _bf, _bsz)                         \
   {void (*_lsf)(int sig);                                                   \
    static void (*_lef)(int err) = NULL;                                     \
    switch (SETJMP(SC_gs.cpu))                                               \
       {case ABORT :                                                         \
             io_printf(STDOUT, "\n%s\n\n", _msg);                            \
             if (_lef != NULL)                                               \
                (*_lef)(ABORT);                                              \
             exit(1);                                                        \
        case ERR_FREE :                                                      \
             if (_lef != NULL)                                               \
                (*_lef)(ERR_FREE);                                           \
             exit(0);                                                        \
        default :                                                            \
             _lef = _ef;                                                     \
             break;};                                                        \
    if (_sh == TRUE)                                                         \
       {_lsf = _sf;                                                          \
        if (_lsf != NULL)                                                    \
           SC_signal(SIGINT, _lsf);                                          \
        else                                                                 \
           SC_signal(SIGINT, SC_interrupt_handler);};                        \
    if (_bh == TRUE)                                                         \
       {if ((_bf == NULL) || (_bsz <= 0))                                    \
           {SC_setbuf(stdout, NULL);}                                        \
        else                                                                 \
           {io_setvbuf(stdout, _bf, _IOFBF, _bsz);};};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ITEMLEN - return the length of an array in items */

#define SC_ITEMLEN(array, _t)                                                \
    ((array == NULL) ? 0 : SC_arrlen(array) / sizeof(_t))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SWAP_VALUE - exchange two values */

#define SC_SWAP_VALUE(_t, v1, v2)                                           \
   {_t t;                                                                   \
    t    = (v1);                                                            \
    (v1) = (v2);                                                            \
    (v2) = t;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REMOVE_ELEMENTS - remove the NR element starting at Ith
 *                    - from array A which is N long
 *                    - N is decremented by NR and all the elements
 *                    - are copied down
 *                    - NOTE: not the most efficient thing to do but
 *                    - it works for any type of array
 */

#define SC_REMOVE_ELEMENTS(_a, _n, _i, _nr)                                 \
   {long _j, _l, _m, _k;                                                    \
    _k = _nr;                                                               \
    _m = ((_n) -= _k);                                                      \
    _l = (_i);                                                              \
    for (_j = _l; _j < _m; _j++)                                            \
        (_a)[_j] = (_a)[_j+_k];                                             \
    memset(&(_a[_j]), 0, _k*sizeof(_a[_j]));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REVERSE_LIST - generic linked list reverser macro */

#define SC_REVERSE_LIST(_t, var, member)                                    \
   {_t *ths, *nxt, *prv;                                                    \
    for (ths = NULL, nxt = var; nxt != NULL; )                              \
        {prv = ths;                                                         \
         ths = nxt;                                                         \
         nxt = ths->member;                                                 \
         ths->member = prv;};                                               \
    var = ths;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_POP - pop a stack */

#define SC_POP(p, pend) ((p) < (pend)) ? (p)++ : (pend)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PUSH - push onto a stack */

#define SC_PUSH(p, pfirst) ((p) > (pfirst)) ? (p)-- : (pfirst)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PTR_ARR_LEN - find the length of an array of pointers which
 *                - is terminated by a NULL pointer
 *                - this is a very common idiom
 */

#define SC_ptr_arr_len(_n, _a)                                               \
   {if (_a == NULL)                                                          \
       _n = 0;                                                               \
    else                                                                     \
       for (_n = 0; _a[_n] != NULL; _n++);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LAST_CHAR - access the last character in a string safely */

#define SC_LAST_CHAR(_s)          ((_s)[SC_char_index(_s, -1)])

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_NTH_LAST_CHAR - access the Nth to last character in a string safely */

#define SC_NTH_LAST_CHAR(_s, _n)  ((_s)[SC_char_index(_s, -(_n))])

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define SC_VDSNPRINTF(_c, _s, _f)                                            \
    {SC_VA_START(_f);                                                        \
     _s = SC_vdsnprintf(_c, _f, __a__);                                      \
     SC_VA_END;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define SC_GET_POINTER(_t, n)        ((_t *) SC_get_pointer(n))
#define SC_GET_INDEX(p)              SC_pointer_index(p)
#define SC_DEL_POINTER(_t, n)        ((_t *) SC_del_pointer(n))
#define SC_ADD_POINTER(p)            SC_stash_pointer(p)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define SC_ERR_TRAP()                                                        \
   SETJMP(SC_error_push()->cpu)

#define SC_ERR_UNTRAP()                                                      \
   SC_error_pop()

#define SC_TOKEN_TYPE(x, i)             ((x)->tokens[i].type)
#define SC_TOKEN_INTEGER(x, i)          ((x)->tokens[i].val.l)
#define SC_TOKEN_REAL(x, i)             ((x)->tokens[i].val.d)
#define SC_TOKEN_STRING(x, i)           ((x)->tokens[i].val.s)
#define SC_LEX_SCAN(x)                  (*((x)->scan))()

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef IBMMP

# define FLUSH_ON(_f, _x)   io_flush(_f)

#else

# define FLUSH_ON(_f, _x)

#endif

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_SC_tree_node SC_tree_node;
typedef struct s_SC_ntree SC_ntree;
typedef struct s_SC_sigstate SC_sigstate;
typedef struct s_SC_errdes SC_errdes;
typedef struct s_SC_proc_info SC_proc_info;

typedef union u_SC_address SC_address;
typedef struct s_SC_process_rusedes SC_process_rusedes;
typedef struct s_PROCESS PROCESS;
typedef struct s_SC_message SC_message;
typedef struct s_SC_pending_msg SC_pending_msg;
typedef struct pollfd SC_poll_desc;
typedef struct s_SC_logfile SC_logfile;
typedef struct s_SC_evlpdes SC_evlpdes;
typedef struct s_SC_rusedes SC_rusedes;
typedef struct s_SC_mem_fnc SC_mem_fnc;
typedef struct s_SC_global_state SC_global_state;

typedef void (*PFFileCallback)(int fd, int mask, void *a);
typedef PROCESS *(*PFProcInit)(char **argv, char *mode, int type);
typedef void (*PFProcExit)(PROCESS *pp, void *a);
typedef int (*PFIVA)(void *a);

FUNCTION_POINTER(size_t, (*PFSize_t));
FUNCTION_POINTER(pcons, *(*PFPPcons));

#if defined(HAVE_OPENMPI)
typedef void *SC_communicator;
#elif defined(SGI)
typedef unsigned int SC_communicator;
#else
typedef long SC_communicator;
#endif

enum e_SC_jmp_return
   {ERR_FREE = 2,                 /* error free return flag for LONGJMPs */
    ABORT,                        /* error return flag for LONGJMPs */
    RETURN_OK};                   /* simple return flag for LONGJMPs */

typedef enum e_SC_jmp_return SC_jmp_return;

enum e_SC_ipc_mode
   {NO_IPC,
    USE_PTYS,
    USE_SOCKETS,
    USE_PIPES};

typedef enum e_SC_ipc_mode SC_ipc_mode;


struct s_SC_tree_node
   {int nd;
    int balance;
    int *rank;
    void *key;
    void *val;
    SC_tree_node **branch;};

struct s_SC_ntree
   {int nd;
    int n_entry;
    SC_tree_node *head;
    PFIntBin compare;};

union u_SC_address
   {long mdiskaddr;
    int64_t diskaddr;
    PFInt funcaddr;
    char *memaddr;};

struct s_SC_process_rusedes
   {double since;            /* time since last resources used */
    double wcr;              /* wall clock time reference */
    double wct;              /* wall clock time */
    double idlet;            /* wall clock time process is idle */
    double syst;             /* system time */
    double usrt;             /* user time */
    double mem;              /* memory used */
    double dsyst;            /* rate of system time */
    double dusrt;            /* rate of user time */
    double dmem;};           /* rate of memory change */

struct s_PROCESS
   {int status;              /* process status - SC_RUNNING, SC_EXITED, ... */
    int reason; /* reason for latest status change - also child exit status */
    int type;            /* process type (SC_LOCAL, SC_REMOTE, SC_PARALLEL) */
    int id;                                      /* process id of the child */
    int root;                          /* process id of the original parent */
    int in;
    int out;
    int ischild;                             /* TRUE iff started as a child */
    int flags;
    SC_ipc_mode medium;                 /* USE_PTYS, USE_SOCKETS, USE_PIPES */
    int gone;      /* TRUE if this is no longer in the system process table */
    int index;             /* index of this process in managed process list */
    int blocking;
    int line_mode;
    int line_sep;
    SC_file_type data_type;                    /* data type (BINARY, ASCII) */
    int data;                           /* socket for separate data channel */
    int rcpu;                                       /* requested CPU number */
    int acpu;                                        /* assigned CPU number */
    int n_zero;                      /* flag to work around OSF SIGCHLD bug */
    long n_read;                      /* total number of bytes read from in */
    long n_write;                   /* total number of bytes written to out */
    char *spty;
    SC_ttydes *tty;
    unsigned int nb_ring;
    unsigned char *in_ring;
    unsigned int ib_in;
    unsigned int ob_in;
    char *data_buffer;
    unsigned long nb_data;
    unsigned long nx_data;
    void *vif;

    int n_pending;
    SC_pending_msg *pending;

    char *start_time;
    char *stop_time;

    SC_process_rusedes *pru;

    int open_retry;     /* time in milliseconds to retry select open failures */
    SC_filedes fd[3];   /* file desc for stdin, stdout, stderr redirection */

    void *exit_arg;     /* store argument to pass to on_exit function */
    void (*on_exit)(PROCESS *pp, void *a);
    int (*release)(PROCESS *pp);
    int (*exec)(PROCESS *cp, char **argv, char **env,
			     char *mode);
    int (*statusp)(PROCESS *pp);
    int (*close)(PROCESS *pp);
    int (*flush)(PROCESS *pp);
    int (*read)(void *ptr, char *type, size_t nitems,
		PROCESS *pp);
    int (*write)(void *ptr, char *type, size_t nitems,
		 PROCESS *pp);
    int (*printf)(PROCESS *pp, char *buffer);
    int (*gets)(char *bf, int len, PROCESS *pp);
    int (*in_ready)(PROCESS *pp);
    int (*setup)(PROCESS *pp, int child);
    int (*lost)(PROCESS *pp);

    int (*recv_formats)(PROCESS *pp);
    int (*send_formats)(void);
    void (*rl_vif)(void *p);};

/* this is system process info ala ps */

struct s_SC_proc_info
   {int pid;                 /* process id */
    int ppid;                /* parent process id */
    int pgid;                /* process group id */
    int uid;                 /* user id */
    int priority;            /* process priority */
    int time;                /* cumulative CPU time */
    int etime;               /* wall clock time from process start */
    int tstart;              /* process start time */
    double memory;           /* size of process in memory */
    char command[MAXLINE];}; /* command line */

struct s_SC_pending_msg
   {char *bf;
    char *type;
    size_t nitems;
    void *vr;
    int req;
    int oper;
    SC_pending_msg *nxt;};

struct s_SC_message
   {int n;
    int nx;
    int *ni;
    char **type;
    char **msg;};

/* put a STREAMS like interface together to smooth over the
 * differences between various multiplex I/O functionalities
 */

#ifndef HAVE_STREAMS_P

# ifndef SOLARIS

struct pollfd
   {int fd;
    short events;
    short revents;};

# endif

#endif


struct s_SC_evlpdes
   {int wait;                       /* wait time when polling descriptors */
    int raw;                        /* assert raw mode for stdin */
    short maccpt;                   /* accept mask */
    short mrejct;                   /* reject mask */
    void *state;                    /* caller state holder - via arg list */
    SC_array *fd;                   /* file descriptors */
    SC_array *faccpt;               /* call back functions for accepted messages */
    SC_array *frejct;               /* call back functions for rejected messages */
    int (*exitf)(int *rv, void *a); /* exit method used to determine when
                                     * to leave the event loop
                                     * returns TRUE to exit
                                     * and return value passed in rv */
    PFSignal_handler sigio;         /* handler for SIGIO */
    PFSignal_handler sigchld;};     /* handler for SIGCHLD */


typedef void *(*PFMalloc)(size_t size);
typedef void *(*PFRealloc)(void *ptr, size_t size);

struct s_SC_sigstate
   {int mn;
    int mx;
    PFSignal_handler ignore;
    PFSignal_handler deflt;
    PFSignal_handler error;
    PFSignal_handler hnd[SC_NSIG];};

struct s_SC_errdes
   {int active;
    char message[MAXLINE];
    JMP_BUF cpu;};

struct s_SC_logfile
   {FILE *file;
    pid_t pid;
    int entry;};

/* portable work-alike for the common struct rusage */

struct s_SC_rusedes
   {int pid;            /* process id */
    int ppid;           /* process parent id */
    int uid;            /* process user id */
    int priority;       /* process scheduling priority */
    double ut;          /* total user time used */
    double st;          /* total system time used */
    double maxrss;      /* maximum resident set size (in kilobytes) */
    double idrss;       /* data segment memory used (kilobyte-seconds) */
    double isrss;       /* stack memory used (kilobyte-seconds) */
    double minflt;      /* soft page faults - reclaims */
    double majflt;      /* hard page faults - requiring I/O */
    double nswap;       /* times a process was swapped out of memory */
    double inblock;     /* block input operations via the file system */
    double outblock;    /* block output operations via the file system */
    double msgsnd;      /* IPC messages sent */
    double msgrcv;      /* IPC messages received */
    double nsignals;    /* signals delivered */
    double nvcsw;       /* voluntary context switches */
    double nivcsw;      /* involuntary context switches */
    double nsysc;       /* system calls */
    char cmd[MAXLINE];};


struct s_SC_mem_fnc
   {void *(*nalloc)(long nitems, long bpi, int na, int zsp,
		    const char *fnc, const char *file, int line);
    void *(*alloc)(long nitems, long bpi, char *name, int na, int zsp);
    void *(*realloc)(void *p, long nitems, long bpi, int na, int zsp);
    int (*free)(void *p, int zsp);};

struct s_SC_global_state
   {char version[32];
    int assert_fail;
    int comm_rank;
    int comm_size;
    int errn;
    int io_interrupts_on;
    int mm_debug;
    int radix;
    int unary_plus;
    int verify_writes;

    double (*atof)(const char *s);
    double (*strtod)(const char *s, char **endp);
    long (*otol)(char *s);
    long (*htol)(char *s);
    long (*atol)(const char *s);
    long (*strtol)(const char *s, char **endp, int base);
    void (*type_container)(char *dtype, char *stype);
    SC_mem_fnc mm;
    JMP_BUF cpu;};       /* top level environment - mainly for error return */

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern int
 Zero_I,
 One_I;

extern SC_global_state
 SC_gs;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* SCBIO.C declarations */

extern int
 SC_check_file(FILE *fp);


/* SCCTL.C declarations */

extern void
 *SC_assoc(pcons *alist, char *s),
 SC_banner(char *s),
 SC_set_banner(char *fmt, ...),
 SC_pause(void),
 SC_init_path(int nd, ...),
 SC_advance_name(char *s),
 SC_interrupt_handler(int sig),
 SC_rl_pcons(pcons *cp, int level),
 SC_free_alist(pcons *alst, int level),
 SC_free_search_path(SC_array *path);

extern long
 SC_bit_count(long c, int n);

extern unsigned int
 SC_bit_reverse(unsigned int i, int n);

extern int
 SC_random_int(int a, int b),
 SC_query_file(char *name, char *mode, char *type),
 SC_isfile(char *name),
 SC_isfile_text(char *name),
 SC_file_path(char *name, char *path, int nc, int full),
 SC_full_path(char *name, char *path, int nc),
 SC_regx_match(char *s, char *patt),
 SC_char_count(char *s, int c),
 SC_remove(char *s),
 SC_assoc_info(pcons *alst, ...),
 SC_assoc_info_alt(pcons *alst, ...);

extern char
 *SC_get_banner(void),
 *SC_itoa(int n, int radix, int nc),
 *SC_search_file(char **path, char *name),
 **SC_get_search_path(void),
 *SC_pop_path(char *path),
 *SC_getcwd(void);

extern pcons
 *SC_assoc_entry(pcons *alist, char *s),
 *SC_make_pcons(char *cat, int ma, void *ca,
		char *cdt, int md, void *cd),
 *SC_mk_pcons(char *cat, void *ca, char *cdt, void *cd),
 *SC_add_alist(pcons *alist, char *name, char *type, void *val),
 *SC_change_alist(pcons *alist, char *name, char *type, void *val),
 *SC_alist_map(pcons *alist, int (*fnc)(pcons *hp, void *arg), void *arg),
 *SC_rem_alist(pcons *alist, char *name),
 *SC_copy_alist(pcons *alist),
 *SC_append_alist(pcons *alist1, pcons *alist2);

extern SC_array
 *SC_make_search_path(int nd, ...);


/* SCERR.C declarations */

extern SC_errdes
 *SC_error_push(void),
 *SC_error_pop(void);

extern char
 *SC_error_msg(void);

extern void
 SC_warn(char *fmt, ...),
 SC_error(int st, char *fmt, ...);

extern int
 SC_error_explanation(int st, char *s, int nc);


/* SCFIA.C declarations */

extern int
 SC_free_stash(void);

extern int64_t
 SC_stash_pointer(void *p),
 SC_pointer_index(void *p);

extern void
 *SC_get_pointer(int64_t n),
 *SC_del_pointer(int n);


/* SCFNCA.C declarations */

extern void
 SC_type_container(char *dtype, char *stype);

extern int
 SC_fix_lmt(int nb, int64_t *pmn, int64_t *pmx, int64_t *pumx),
 SC_unpack_bits(char *out, char *in, int ityp, int nbits,
			     int padsz, int fpp, long nitems, long offs),
 SC_sizeof(char *s),
 SC_pointer_ok(void *p),
 SC_putenv(char *s);

extern long
 SC_extract_field(char *in, int offs, int nbi, int nby, int *ord);

extern mode_t
 SC_get_umask(void),
 SC_get_perm(int exe);


/* SCFNCB.C declarations */

extern SC_proc_info
 *SC_get_processes(int uid);

extern int
 SC_load_ave(double *av),
 SC_free_mem(double *mem),
 SC_get_ncpu(void),
 SC_get_pname(char *path, int nc, int pid),
 SC_attach_dbg(int pid),
 SC_retrace_exe(char ***pout, int pid, int to),
 SC_retrace_dbg(char ***pout, int pid),
 SC_nfs_monitor(int *st, int nc),
 SC_yield(void);

extern void
 SC_sleep(int to),
 SC_get_latencies(double *ptmp, double *phm, double *pnet, double *pprc);

extern char
 *SC_get_uname(char *name, int nc, int uid),
 **SC_get_env(void);


/* SCFNCT.C declarations */

extern struct tm
 *SC_localtime(const time_t *t, struct tm *tms);

extern void
 SC_get_time(double *psec, double *pmusec),
 SC_timeout(int to, PFSignal_handler fnc),
 SC_time_str(unsigned long t, char *d),
 SC_sec_str(double t, char *d);

extern double
 SC_cpu_time(void),
 SC_wall_clock_time(void),
 SC_str_time(char *d);

extern char
 *SC_date(void),
 *SC_datef(void);


/* SCHP.C declarations */

extern PROCESS
 *SC_mk_process(char **argv, char *mode, int type, int iex),
 *SC_open(char **argv, char **envp, char *mode, ...);

extern void
 SC_set_processor_info(int size, int rank);

extern int
 SC_get_number_processors(void),
 SC_get_processor_number(void),
 SC_buffer_in(char *bf, int n, PROCESS *pp),
 SC_buffer_out(char *bf, PROCESS *pp, int nb, int binf),
 SC_buffer_data_in(PROCESS *pp),
 SC_buffer_data_out(PROCESS *pp, char *bf,
		    int nbi, int block_state),
 SC_printf(PROCESS *pp, char *fmt, ...),
 SC_init_client(char *host, int port),
 SC_init_server(int step, int closep);

extern char
 *SC_gets(char *bf, int len, PROCESS *pp);


/* SCHSRV.C declarations */

extern int
 SC_host_server_fin(void),
 SC_host_server_init(char *file, int reset, int vrb),
 SC_host_server_query(char *out, int nc, char *fmt, ...),
 SC_verify_host(char *hst, int to),
 SC_get_sys_length_max(int local, int full),
 SC_get_host_length_max(char *sys, int local, int full),
 SC_get_host_name(char *hst, int nc, char *sys),
 SC_get_nhosts(char *sys),
 SC_hostname(char *s, int nc),
 SC_hosttype(char *s, int nc, char *x),
 SC_hostsystem(char *type, int nc, char *node);

extern char
 **SC_get_host_types(int whch, char *net),
 **SC_get_system_list(char *sys),
 **SC_get_host_list(char *sys, int single);


/* SCHTTP.C declarations */

extern void
 SC_split_http(char *url, char *host, char *page),
 SC_close_http(int fd);

extern int
 SC_open_port(char *host, int port, int to, int fm),
 SC_open_http(char *host, int port),
 SC_request_http(int fd, char *cmnd, char *url, char *vrs),
 SC_http_url_file(char *url, char *file, char *vrs);


/* SCIOCTL.C declarations */

extern SC_evlpdes
 *SC_make_event_loop(void (*sigio)(int sig), PFSignal_handler sigchld,
		     int (*ex)(int *rv, void *a),
		     int wait, short accept, short reject),
 *SC_make_event_loop_current(SC_evlpdes *pe);

extern void
 SC_catch_event_loop_interrupts(SC_evlpdes *pe, int flag),
 SC_free_event_loop(SC_evlpdes *pe),
 SC_event_loop_set_masks(SC_evlpdes *pe, int accept, int reject),
 SC_event_loop_get_masks(SC_evlpdes *pe, int *paccept, int *preject),
 SC_remove_event_loop_callback(SC_evlpdes *pe, int type, void *p),
 SC_set_poll_masks(int accept, int reject),
 SC_get_poll_masks(int *paccept, int *preject),
 SC_rl_io_callback(int fd),
 SC_rl_io_callback_file(FILE *fp),
 SC_catch_io_interrupts(int flag);

extern int
 SC_register_event_loop_callback(SC_evlpdes *pe, int type, void *p,
				 PFFileCallback acc, PFFileCallback rej,
				 int pid),
 SC_replace_event_loop_accept(SC_evlpdes *pe, int type, void *p,
			      PFFileCallback acc),
 SC_replace_event_loop_reject(SC_evlpdes *pe, int type, void *p,
			      PFFileCallback rej),
 SC_event_loop_poll(SC_evlpdes *pe, void *a, int to),
 SC_event_loop(SC_evlpdes *pe, void *a, int to),
 SC_process_event_loop(PROCESS *pp, void *a,
		       int (*ex)(int *rv, void *a),
		       PFFileCallback acc, PFFileCallback rej),
 SC_set_attr(PROCESS *pp, int attr, int val),
 SC_set_fd_async(int fd, int state, int pid),
 SC_echo_on_file(FILE *fp),
 SC_echo_on_fd(int fd),
 SC_echo_off_file(FILE *fp),
 SC_echo_off_fd(int fd),
 SC_isblocked(PROCESS *pp),
 SC_isblocked_file(FILE *fp),
 SC_isblocked_fd(int fd),
 SC_unblock(PROCESS *pp),
 SC_unblock_file(FILE *fp),
 SC_unblock_fd(int fd),
 SC_block(PROCESS *pp),
 SC_block_file(FILE *fp),
 SC_block_fd(int fd),
 SC_file_flags(FILE *fp),
 SC_fd_flags(int fd),
 SC_io_callback_pid(int fd, PFFileCallback acc, PFFileCallback rej, int pid),
 SC_io_callback_fd(int fd, PFFileCallback acc, PFFileCallback rej),
 SC_io_callback_file(FILE *fp, PFFileCallback acc, PFFileCallback rej),
 SC_io_callback(PROCESS *pp, PFFileCallback acc, PFFileCallback rej),
 SC_replace_accept_fd(int fd, PFFileCallback acc),
 SC_replace_accept_file(FILE *fp, PFFileCallback acc),
 SC_replace_accept(PROCESS *pp, PFFileCallback acc),
 SC_replace_reject_fd(int fd, PFFileCallback rej),
 SC_replace_reject_file(FILE *fp, PFFileCallback rej),
 SC_replace_reject(PROCESS *pp, PFFileCallback rej),
 SC_poll_descriptors(int to);

#ifndef HAVE_STREAMS_P

extern int
 SC_poll(SC_poll_desc *fds, long nfds, int timeout);

#endif


/* SCMEMC.C declarations */

extern void
 SC_configure_mm(long mxl, long mxm, long bsz, double r),
 SC_mem_statb(uint64_t *al, uint64_t *fr, uint64_t *df, uint64_t *mx),
 SC_mem_statb_set(uint64_t a, uint64_t f),
 SC_mem_stats(long *al, long *fr, long *df, long *mx),
 SC_mem_stats_acc(long a, long f),
 SC_mem_stats_set(long a, long f);

extern int
 SC_zero_space_n(int flag, int tid),
 SC_zero_on_alloc_n(int tid);


/* SCMEMDA.C declarations */

extern SC_mem_fnc
 SC_use_mm(SC_mem_fnc *mf),
 SC_use_score_mm(void),
 SC_use_c_mm(void),
 SC_trap_pointer(void *p, int sig);

extern void
 *SC_permanent(void *p),
 *SC_mem_attrs(void *p, int attr),
 SC_untrap_pointer(void *p),
 *SC_copy_item(void *in);

extern char
 *SC_arrname(void *p);

extern int
 SC_mem_info(void *p, long *pl, int *pt, int *pr, char **pn),
 SC_mem_trace(void),
 SC_reg_mem(void *p, long length, char *name),
 SC_dereg_mem(void *p),
 SC_is_score_ptr(void *p),
 SC_mark(void *p, int n),
 SC_set_count(void *p, int n),
 SC_ref_count(void *p),
 SC_safe_to_free(void *p),
 SC_arrtype(void *p, int type),
 SC_set_arrtype(void *p, int type);

extern long
 SC_mem_chk(int typ),
 SC_arrlen(void *p);

extern char
 *SC_mem_lookup(void *p);


/* SCMEMDB.C declarations */

extern void
 *SC_mem_diff(FILE *fp, void *a, void *b, size_t nb);

extern char
 *SC_mem_list(int flag, int show);

extern int
 SC_mem_corrupt(int flag),
 SC_mem_map(FILE *fp, int flag),
 SC_mem_neighbor(void *p, void *pa, void *pb);

extern long
 SC_mem_monitor(int old, int lev, char *id, char *msg),
 SC_mem_object_trace(long nb, int type,
		     void (*f)(char *name, char *addr, long length,
			       int count, int type));

extern void
 SC_mem_print(void *p);


/* SCMEMG.C declarations */

extern void
 SC_use_guarded_mem(int on),
 SC_mem_guard_high(int on);


/* SCMEMH.C declarations */

extern long
 SC_mem_history(long sz);

extern void
 SC_mem_history_out(FILE *fp, int tid);


/* SCPAR.C declarations */

extern void
 SC_init_tpool(int np, PFTid tid),
 SC_fin_tpool(void),
 SC_do_threads(int n, int *np, PFPVoidAPV fnc[],
	       void **arg, void **ret),
 SC_chunk_loop(PFPVoidAPV fnc, int mn, int mx,
	       int serial, void *argl),
 SC_chunk_split(int *pmn, int *pmx, void **prv),
 SC_queue_work(PFPVoidAPV fnc, int serial, void *argl),
 SC_do_tasks(PFInt fnc, void *a, int np, int off, int on),
 SC_do_tasks_f(PFInt fnc, void **pa, int *pnp, int *poff, int *pon);

extern int
 SC_init_threads(int np, PFTid tid), 
 SC_using_tpool(void),
 SC_current_thread(void),
 SC_queue_next_item(int ng);

extern PFTid
 SC_thread_set_tid(PFTid f);


/* SCRSCA.C declarations */

extern void
 SC_show_resource_usage(SC_rusedes *ru);

extern int
 SC_resource_usage(SC_rusedes *ru, int pid),
 SC_set_resource_limits(int64_t mem, int64_t cpu, int64_t fsz,
			int nfd, int nprc),
 SC_get_resource_limits(int64_t *pmem, int64_t *pcpu, int64_t *pfsz,
			int *pnfd, int *pnprc);


/* SCRSCB.C declarations */

extern int
 *SC_process_ids(void);

extern void
 SC_i_sort(int *ind, int n);


/* SCSTR.C declarations */

extern char
 *SC_strsavec(char *s, const char *file, const char *fnc, int line),
 *SC_strcat(char *dest, size_t lnd, char *src),
 *SC_vstrcat(char *dest, size_t lnd, char *fmt, ...),
 *SC_dstrcat(char *dest, char *src),
 *SC_strncpy(char *d, size_t nd, char *s, size_t ns),
 *SC_dstrcpy(char *dest, char *src),
 *SC_strstr(char *string1, char *string2),
 *SC_strstri(char *string1, char *string2),
 *SC_strrev(char *s),
 *SC_squeeze_blanks(char *s),
 *SC_squeeze_chars(char *s, char *q),
 *SC_dstrsubst(char *s, char *a, char *b, size_t n),
 *SC_strsubst(char *d, int nc, char *s, char *a, char *b, size_t n),
 *SC_str_upper(char *s),
 *SC_str_lower(char *s),
 *SC_str_replace(char *s, char *po, char *pn),
 *SC_trim_left(char *s, char *delim),
 *SC_trim_right(char *s, char *delim),
 *SC_firsttok(char *s, char *delim),
 *SC_lasttok(char *s, char *delim),
 *SC_ntok(char *d, int nc, char *s, int n, char *delim),
 **SC_tokenize(char *s, char *delim),
 **SC_tokenize_literal(char *s, char *delim, int nl, int qu),
 **SC_file_strings(char *fname),
 *SC_firsttokq(char *s, char *delim, char *qdelim),
 *SC_concatenate(char *s, int nc, int n, char **a, char *delim, int add),
 *SC_dconcatenate(int n, char **a, char *delim),
 *SC_vdsnprintf(int cp, const char *format, va_list lst),
 *SC_dsnprintf(int cp, char *fmt, ...);

extern double
 SC_stof(char *s),
 SC_atof(const char *ps),
 SC_strtod(const char *nptr, char **endptr);

extern long
 SC_strtol(char *str, char **ptr, int base);

extern int64_t
 SC_stol(char *s),
 SC_stoi(char *s);

extern long double _Complex
 SC_stoc(char *s);

extern unsigned int
 SC_char_index(char *s, int n);

extern int
 SC_strerror(int err, char *msg, size_t nc),
 SC_isalnum(int c),
 SC_ishexdigit(int c),
 SC_isxdigit(int c),
 SC_isdigit(int c),
 SC_ispunct(int c),
 SC_isspace(int c),
 SC_blankp(char *s, char *chr),
 SC_blankl(char *s, char *chr),
 SC_is_print_char(int c, int flag),
 SC_strings_file(char **sa, char *fname, char *mode),
 SC_print_charsp(char *s, int sp),
 SC_numstrp(char *s),
 SC_intstrp(char *s, int base),
 SC_fltstrp(char *s),
 SC_chrstrp(char *s),
 SC_str_icmp(char *s, char *t),
 SC_vsnprintf(char *dst, size_t nc, const char *fmt, va_list a);

extern void
 SC_free_strings(char **sa),
 SC_remove_string(char **sa, int n),
 SC_string_sort(char **v, int n),
 SC_text_sort(char *v, int n, int nc);


/* SCSIG.C declarations */

extern PFSignal_handler
 SC_signal(int sig, PFSignal_handler fnc),
 SC_signal_action(int sig, PFSignal_handler fnc, int flags, ...),
 SC_which_signal_handler(int sig);

extern SC_sigstate
 *SC_save_signal_handlers(int sigmn, int sigmx);

extern int
 SC_signal_block(sigset_t *pos, ...),
 SC_signal_unblock(sigset_t *pos, ...),
 SC_send_signal(int pid, int sig);

extern char
 *SC_signal_name(int sig);

extern void
 SC_restore_signal_handlers(SC_sigstate *ss, int rel),
 SC_set_signal_handlers(PFSignal_handler hnd, int mn, int mx),
 SC_setup_sig_handlers(PFSignal_handler hand, int fl);


/* SCSYSA.C declarations */

extern int
 SC_exec(char ***out, char *cmnd, char *shell, int to),
 SC_execa(char ***out, char *shell, int to, char *fmt, ...),
 SC_execs(char *out, int nc, char *shell, int to, char *fmt, ...),
 SC_exec_commands(char *shell, char **cmnds, char **env, int to,
		  char *lname, char *fname, int na, int show,
		  int ignore, int dmp),
 SC_signal_async(int sig),
 SC_exec_async(char *shell, char **cmnds, char **dirs,
	       char *consh, char **conenv, int nc,
	       char *lname, char *fname, int na, int show,
	       int ignore, int log),
 SC_exec_async_h(char *shell, char **env, char **hstl, char **cmnds, char **dirs,
		 int to, char *fname, int na, int show, int ignore),
 SC_exec_async_s(char *shell, char **env, char **sysl, char **cmnds, char **dirs,
		 int to, char *fname, int na, int show, int ignore),
 SC_exec_server(char *shell, char *fname, int na, int show,
		int ignore, int debug),
 SC_system(char *cmd);

extern char
 **SC_syscmnd(char *fmt, ...);


/* SCSYSC.C declarations */

/* SCSYSE.C declarations */

extern int
 SC_exec_job(char **argv, char *mode, int flags,
	     PFFileCallback in, PFFileCallback irej,
	     PFFileCallback out, PFFileCallback orej);


/* SCLOG.C declarations */

extern int
 SC_close_log(SC_logfile log),
 SC_log(SC_logfile log, char *format, ...);

extern SC_logfile
 SC_open_log(void);


/* SCTREE.C declarations */

extern SC_ntree
 *SC_make_ntree(int nd, int (*cmp)(void *a, void *b));

extern void
 SC_rel_ntree(SC_ntree *tree),
 *SC_tree_install(void *k, void *val, SC_ntree *tree),
 SC_map_tree(SC_ntree *tr, void (*f)(SC_tree_node *pn, int depth, int idp));

extern SC_tree_node
 *SC_tree_lookup_key(void *k, SC_ntree *tree),
 *SC_tree_lookup_n(int n, SC_ntree *tree);


/* SCTRK.C declarations */

extern int
 SC_send_tracker(char *code, char *version, int status, char *dst);


#ifdef __cplusplus
}
#endif

#endif

