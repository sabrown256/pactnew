/*
 * SCORE_INT.H - internal interfaces for PACT standard core package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCORE_INT

#define PCK_SCORE_INT                         /* SCORE package resident */

#include "cpyright.h"
#include "score.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/* ERROR Exit Statuses
 *   signo    - range   1 <= 63
 *   internal - range  64 <= 71
 *   exec     - range  72 <= 79   (defined in sclppsx.c and scsysd.c)
 *   future   - range  80 <= 127
 *   errno    - range 128 <= 255
 */

#define SC_EXIT_TIMEOUT  64      /* timeout exit status */
#define SC_EXIT_SELF     65      /* self termination exit status (may not be error) */
#define SC_EXIT_INIT     66      /* initialization failure exit status */
#define SC_EXIT_CORRUPT  67      /* corruption detected exit status */
#define SC_EXIT_BAD      68      /* conditions do not warrant continuing */
#define SC_EXIT_IO       69      /* lost stdin/stdout */

#define SC_EXIT_ERRNO() (128 + errno)

#define SC_TERM_INPUT    101
#define SC_TERM_OUTPUT   102
#define SC_TERM_CONTROL  103
#define SC_TERM_LOCAL    104
#define SC_TERM_CHAR     105
#define SC_TERM_DESC     106

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* DEPRECATE - mark deprecated functions */

#ifdef DEPRECATE_RUNTIME

#define DEPRECATED(_date)                                                    \
   printf("WARNING: %s deprecated on %s\n", __func__, #_date)

#else

#define DEPRECATED(_date)

#endif

/* helpers for bindings */

/* variadic macro example:
 * #define eprintf(...) fprintf (stderr, __VA_ARGS__)
 */

/* ARG - specify default value, in/out, etc
 *     - ignored by CC but parsed by BLANG
 */

#undef ARG
#define ARG(...)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define SC_PATH_DELIMITER   _SC_os.path_delimiter
#define SC_FILE_LENGTH      _SC_os.file_length
#define SC_QUERY_MODE       _SC_os.query_mode
#define SC_QUERY_EXEC       _SC_os.query_exec
#define SC_PROCESS_METHODS  _SC_os.process_methods

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define SC_HASHARR_LOOKUP_FUNCTION(tab, t, nm)  ((t) _SC_hasharr_lookup_function(tab, nm))
#define SC_ASSOC_FUNCTION(alst, t, nm)  ((t) _SC_assoc_function(alst, nm))

#define SC_GET_FUNC_ADDR(_t, a)         ((_t) a.funcaddr)
#define SC_GET_MEM_ADDR(_t, a)          ((_t) a.memaddr)
#define SC_GET_DISK_ADDR(_t, a)         ((_t) a.diskaddr)

#define SC_MEM_INIT(_t, _v)             memset(_v, 0, sizeof(_t))
#define SC_MEM_INIT_N(_t, _v, _n)       memset(_v, 0, (_n)*sizeof(_t))
#define SC_MEM_INIT_V(_v)               memset(_v, 0, sizeof(_v))

#define SC_MEM_GET_N(_t, _v)            (SC_arrlen(_v)/sizeof(_t))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TIME_ALLOW - a time based SETJMP/LONGJMP capability
 *               - called with TO > 0
 *               - it starts a timer using alarm and returns FALSE
 *               - if the time elapses before a second call with TO <= 0
 *               - it LONGJMPs back and returns TRUE
 *               - see sclppsx.c for an example
 */

#define SC_time_allow(to)                                                    \
   (SC_timeout(to, _SC_timeout_cont, NULL),                                  \
    (to > 0) ? SETJMP(*_SC_get_to_buf(-1)) : TRUE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef SC_DEBUG
# define SC_ENTERING _SC_trace_entering(__FILE__, __func__, __LINE__)
# define SC_LEAVING  _SC_trace_leaving(__FILE__, __func__, __LINE__)
#else
# define SC_ENTERING
# define SC_LEAVING
#endif

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_SC_oscapdes SC_oscapdes;
typedef struct s_SC_state SC_state;

struct s_SC_oscapdes
   {void (*path_delimiter)(char *delim);
    int64_t (*file_length)(char *path);
    int (*query_mode)(char *name, char *mode);
    int (*query_exec)(char *path);
    void (*process_methods)(PROCESS *pp);};

/* SC_STATE - single container to hold "static" state
 *          - helps to identify state which may become per thread state
 *          - easier to spot patterns in state
 *          - helps to identify scopes for grouping routines
 *          - down side - combines what was separate, file or function static
 */

struct s_SC_state
   {

/* initializes to non-zero values */

/* SCHP.C */
    int sfd;

/* SCMEMG.C */
    int mem_guard_high;
    int page_size;
    void *(*alloc)(size_t nb);
    void *(*realloc)(void *p, size_t nb);
    void (*free)(void *p);

/* SCMEMH.C */
    double t0[2];

/* SCSYSB.C */
    int elapsed;

/* SCTERM.C */
    int suppress;

/* SCFNCT.C */
    PFSignal_handler to_lst;
    SC_contextdes to_err;

/* SCBIO.C */
    int64_t buffer_size;

/* SCTYP.C */
   SC_type_manager types;

/* initializes to 0 bytes */

/* SCCNTX.C */

   hasharr *context_table;

/* SCFIA.C */
   int ip;                                        /* pointer list variables */
   SC_array *ptr_lst;
   int ne;                                   /* Fortran hash dump variables */
   char **hash_entries;
   SC_array *lexs;                              /* lexical stream variables */

/* SCFIS.C */
   char *ta;                                   /* Fortran sprintf variables */
   int nt;

/* SCFIO.C */
   hasharr *hosts;

/* SCFNCA.C */
   int64_t bmn;
   int64_t bmx;

/* SCHP.C */
   int nfd;
   void *orig_tty;
   struct sockaddr_in *srvr;

/* SCHSRV.C */
   char *hsdb;                                     /* host server variables */
   hasharr *hsst;

/* SCIOCTL.C */
   SC_evlpdes *evloop;                    /* for the old async I/O routines */

/* SCLOG.C */
   int nlog;

/* SCLPPSX.C */
   PROCESS *terminal;

/* SCMEMC.C */
   int zero_space;

/* SCMEMDA.C */
   hasharr *mem_table;

/* SCMEMG.C */

#ifdef HAVE_DEV_ZERO 
    char *start;
#endif

/* SCMEMH.C */
   long mem_hst_sz;
   void (*mem_hst)(int act, void *a);

/* SCPAR.C */
   PFTid tid_hook;

/* SCSIG.C */
   char **signame;

/* SCSYSB.C */
    int slog_env;

/* SCSYSD.C */
   int log_env;
   char *die;
   int idln;

/* SCSYSE.C */
   int tty_n_rej;
   char *sqbf;
   char *ecbf;
   char *elbf;

/* SCTHE.C */
   SC_array *eth_locks;
   SC_array *eth_conds;
   SC_array *eth_keys;

/* SCCTL.C */
   SC_array *path;
   char ibf[MAXLINE];                         /* working buffer for SC_itoa */
   char banner[MAXLINE];              /* Code and Version string for banner */

/* SCPMAN.C */
   int debug_proc;
   SC_array *wait_list;
   SC_array *process_list;

   JMP_BUF btt;                                                 /* scterm.c */
   JMP_BUF srv_rstrt;};                                         /* scsysb.c */

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SC_state
 _SC;

extern SC_oscapdes
 _SC_os;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* SCCTL.C declarations */

extern void
 _SC_timeout_cont(int sig);

extern char
 *_SC_search_file(char **path, char *name, char *mode, char *type);


/* SCERR.C declarations */

extern void
 _SC_init_error_stack(SC_array *err, int id);


/* SCFIS.C declarations */

extern int
 _SC_strlen(char *s, int nx);


/* SCFNCA.C declarations */

extern int64_t
 _SC_to_number(void *a);

extern void
 *_SC_to_address(int64_t a);

extern SC_address
 _SC_set_func_addr(PFInt x),
 _SC_set_mem_addr(void *x),
 _SC_set_disk_addr(int64_t x);

extern PFInt
 _SC_hasharr_lookup_function(hasharr *tab, char *nm),
 _SC_assoc_function(pcons *alist, char *nm);


/* SCMEMT.C declarations */

extern SC_array
 *_SC_get_error_stack(int id);

extern JMP_BUF
 *_SC_get_to_buf(int id);

extern emu_thread_info
 *_SC_get_thread_info(int id);

extern SC_evlpdes
 **_SC_get_ev_loop(int id);


/* SCSTR.C declarations */

extern int
 _SC_fmt_strcat(char *d, int nd, int ln, char *fmt, char *s);

extern long
 _SC_otol(char *str),
 _SC_htol(char *str);

extern char
 *_SC_pr_tok(char *s, char *delim),
 *_SC_quoted_tok(char *s, char *qdelim);


/* SCTRACE.C declarations */

extern void
 _SC_trace_entering(const char *file, const char *fnc, int line),
 _SC_trace_leaving(const char *file, const char *fnc, int line),
 _SC_trace_show(int n);

#ifdef __cplusplus
}
#endif

#endif

