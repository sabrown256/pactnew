/*
 * SCOPE_PROC.H - define PROC scope
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_PROC

#define PCK_SCOPE_PROC

#include "cpyright.h"
#include "score_int.h"
#include "scope_term.h"

#define DEFAULT_WAIT       5000
#define DEFAULT_TIMEOUT    30000
#define DEFAULT_HEARTBEAT  30

#ifndef WIN32
# undef IN
# undef OUT
# define IN  0
# define OUT 1
#endif

#define SC_RUNNING      0x0
#define SC_STOPPED      0x1
#define SC_CHANGED      0x2
#define SC_EXITED       0x4
#define SC_COREDUMPED   0x8
#define SC_SIGNALED     0x10
#define SC_KILLED       0x20
#define SC_DEAD         0x40
#define SC_LOST         0x80

#define SC_LINE      0x10000000

#define SC_ALL_SIGNALS  0x7fffff

/*
#define BLOCK_WITH_SIGIO                                                     \
   SIGCHLD, SIGPIPE, SIGSTOP, SIGTSTP, SIGTTIN, SIGTTOU
*/
#define BLOCK_WITH_SIGIO                                                     \
   SIGCHLD

/*--------------------------------------------------------------------------*/

/*                             EXEC ERROR STATUS                            */

/*--------------------------------------------------------------------------*/

/* ERROR Exit Statuses
 *   signo    - range   1 <= 63
 *   internal - range  64 <= 71    (defined in score_int.h)
 *   exec     - range  72 <= 79    (some in scsysd.c)
 *   future   - range  80 <= 127
 *   errno    - range 128 <= 255
 */

#define SC_NO_SETSID  72
#define SC_NO_TTY     73
#define SC_NO_CLOSE   74
#define SC_NO_FMT     75
#define SC_NO_EXEC    76

/*--------------------------------------------------------------------------*/

/*                           MESSAGE PASSING SUPPORT                        */

/*--------------------------------------------------------------------------*/

#ifdef PARALLEL_HOST        
# define SC_open_node _SC_open_node
#else
# define SC_open_node _SC_open_proc
#endif

#define SC_TYPE_MATCH   0x40000000
#define SC_CLASS_MATCH  0x20000000
#define SC_TYPE_MASK    0x1FFF0000
#define SC_TAG_MASK     0x0000FFFF

/*--------------------------------------------------------------------------*/

/*                               WAIT HANDLING                              */

/*--------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/wait.h>

/*--------------------------------------------------------------------------*/

/*                    REMOTE FILE/PROCESS I/O HANDLING                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                              EXEC_ASYNC SUPPORT                          */

/*--------------------------------------------------------------------------*/

#define _SC_EXEC_ENV         "--env---"                   /* setenv request */
#define _SC_EXEC_JOB         "--job---"                      /* job request */
#define _SC_EXEC_HEARTBEAT   "--beat--"                 /* server heartbeat */
#define _SC_EXEC_COMPLETE    "--done--"                    /* a job is done */
#define _SC_EXEC_KILLED      "--term--"            /* a job has been killed */
#define _SC_EXEC_KILL        "--kill--"                       /* kill a job */
#define _SC_EXEC_NCPU        "--ncpu--"    /* number of CPUs on server host */
#define _SC_EXEC_RESTART     "--rstrt-"     /* server self restart on error */
#define _SC_EXEC_LOG         "--log---"              /* show the server log */
#define _SC_EXEC_EXIT        "--exit--"               /* the server is done */
#define _SC_EXEC_INFO        "--info--"    /* the server sends info message */
#define _SC_EXEC_SRV_ID      "Srv:"                /* the server message id */

#define SC_EXEC_MSG_MATCH(_r, _s, _m)                                        \
    (_r = _s + strlen(_m), (strncmp(_s, _m, strlen(_m)) == 0))

/*--------------------------------------------------------------------------*/

/*                             PROCEDURAL MACROS                            */

/*--------------------------------------------------------------------------*/

#define SC_BINARY_MODEP(x)  (strchr(&((x)[1]), 'b') != NULL)
#define SC_OTHER_HOSTP(x)   (strchr((x), ':') != NULL)
#define SC_OTHER_CPUP(x)    (strchr((x), '@') != NULL)

#define SC_flush(_p)                                                         \
   ((((_p) != NULL) && ((_p)->flush != NULL)) ?                              \
   (_p)->flush(_p) :                                                         \
   0)

#define SC_close(_p)                                                         \
   ((((_p) != NULL) && ((_p)->close != NULL)) ?                              \
    (_p)->close(_p) :                                                        \
    0)

#define SC_process_alive(_p)                                                 \
   (((_p) != NULL) && ((_p)->in != -1))

#define SC_status(_p)                                                        \
   ((SC_process_alive(_p)) ? (_p)->statusp(_p) : SC_DEAD)

#define SC_read(ptr, type, _ni, _p)                                          \
   ((((_p) != NULL) && ((_p)->read != NULL)) ?                               \
    (_p)->read(ptr, type, _ni, _p) :                                         \
    -1)

#define SC_write(ptr, type, _ni, _p)                                         \
   ((((_p) != NULL) && ((_p)->write != NULL)) ?                              \
    (_p)->write(ptr, type, _ni, _p) :                                        \
    -1)


/* diagnostic aids - we have to help ourselves because some debuggers won't */

#define SC_START_ACTIVITY(_s, _a)                                            \
   {int _oa;                                                                 \
    _oa         = (_s)->doing;                                               \
    (_s)->doing = _a

#define SC_END_ACTIVITY(_s)                                                  \
    (_s)->doing = _oa;}

/*--------------------------------------------------------------------------*/

/*                             TYPEDEFS AND STRUCTS                         */

/*--------------------------------------------------------------------------*/

/* FINISHED_PROC - there is a long story about when we are done with a process
 * there are four conditions that must be met
 *  1) the child process must have been exec'd
 *     if that never happens the PROCESS can be released any time
 *  2) the child must have exited
 *     there are two main ways this can happen:
 *       a) the child process exits
 *       b) the parent process kills the child
 *  3) the I/O from the child must be processed
 *     there can be output from the child in the buffers after it
 *     exits and this should be read before closing the I/O descriptors
 *  4) the PROCESS should be removed from the managed process list
 * if (1) happens the PROCESS can be released only when (2), (3) and (4)
 * have happened
 * these can happen in a variety of orders, mainly following from how
 * (2) happens
 * consequently we have to use the flags member of the PROCESS structure
 * to tell us what has happened and if it is safe to free the PROCESS
 */ 

enum e_finished_proc
   { SC_PROC_EXEC = 0x01,      /* the child was exec'd following the fork */
     SC_PROC_SIG  = 0x02,      /* the SIGCHILD was processed */
     SC_PROC_IO   = 0x04,      /* the I/O descriptors were closed */
     SC_PROC_RM   = 0x08};     /* removed from the managed process list */

enum e_managed_proc
   { SC_PROC_FREE    = -4,      /* freed */
     SC_PROC_ALLOC   = -3,      /* allocated but not yet initialized with pid */
     SC_PROC_DELETED = -2,      /* removed from the managed process list */
     SC_PROC_CLOSED  = -1 };    /* process killed and descriptors closed */

typedef enum e_finished_proc finished_proc;
typedef enum e_managed_proc managed_proc;

/* filter specification */

typedef struct s_fspec fspec;

struct s_fspec
   {int itok;              /* token index (0 for whole line) */
    char text[MAXLINE];};  /* text to match */

typedef struct s_conpool conpool;
typedef struct s_jobinfo jobinfo;
typedef struct s_taskdesc taskdesc;
typedef struct s_tasklst tasklst;
typedef struct s_subtask subtask;
typedef struct s_parstate parstate;
typedef struct s_asyncstate asyncstate;
typedef struct s_SC_scope_proc SC_scope_proc;

struct s_conpool
   {int n_jobs;                    /* number of jobs launched in pool */
    int n_hosts;                   /* number of possible hosts of sys type */
    int show;
    int ignore;
    int na;
    int ref_net[8];
    int net[8];
    int heartbeat;                 /* time expected between heartbeats */
    char **hosts;                  /* names of possible hosts */
    fspec *filter;
    SC_evlpdes *loop;
    SC_array *pool;
    asyncstate *as;
    int active;
    JMP_BUF cpu;};

struct s_parstate
   {int na;
    int heartbeat_dt;      /* do heartbeat every dt seconds */
    int wait_dt;           /* done after waiting dt seconds and no jobs or input */
    int wait_ref;          /* time of last input or any job was running */
    int open_retry;        /* time to wait before retrying redirect file opens */
    int show;              /* flag governing printing command */
    int ignore;            /* ignore failed commands and continue */
    int tagio;             /* label each line of output and print status */
    int server;            /* TRUE if this is a server */
    int n_running;
    int n_complete;
    int done;
    int is_stdout;         /* TRUE while printing to stdout - for SIGPIPE */
    int time_limit;        /* limit for all the work to be done (in milliseconds) */
    int doing;             /* diagnostic indicating activity at moment of interrupt */
    double tstart;
    char *directory;       /* initial or starting directory */
    char *shell;
    char **env;
    fspec *filter;
    PFFileCallback acc;
    PFFileCallback rej;
    SC_evlpdes *loop;
    SC_array *tasks;
    SC_array *log;
    int (*finish)(taskdesc *job, char *msg);
    int (*print)(parstate *state, char *fmt, ...);
    void (*free_logs)(parstate *state);
    void (*free_tasks)(parstate *state);};

struct s_asyncstate
   {int active;
    int debug;
    int ppid;
    int to_stdout;
    int nchar_max;
    conpool *pool;
    parstate *server;
    FILE *log;
    JMP_BUF cpu;
    int (*print)(asyncstate *as, char *fmt, ...);};


/* command task description */

/* TASKDESC - describe and manage a task which has the BNF
 *          - <task>           := <simple-command> [ ; <simple-command>]*
 *          - <simple-command> := a single command, e.g. uname -a
 */

struct s_subtask
   {int need;              /* TRUE if shell is needed to run the subtask */
    int pipe;              /* TRUE if the subtask is pipelined */
    int nt;                /* number of tokens of the subtask */
    char *shell;
    char *command;         /* full text of the subtask */
    char **argf;           /* tokenized version suitable for SC_open */
    SC_filedes fd[3];};

struct s_tasklst
   {int nl;                /* number of subtasks launched - current subtask index */
    int nc;                /* number of completed subtasks */
    int nt;                /* number of subtasks */
    int status;
    subtask *tasks;};

struct s_jobinfo
   {int id;                /* job id number - assigned */
    int ia;                /* current attempt index */
    int na;                /* number of attempts */
    int signal;            /* signal on which command list exited */
    int status;            /* exit status of the command list */
    char stop_time[32];    /* time when command exited */
    char *shell;           /* run commands under this shell */
    char *directory;       /* launch commands starting in this directory */
    char *full;            /* full compound command string */
    double tstart;
    SC_array *out;};

struct s_taskdesc
   {jobinfo inf;
    int ic;                /* current member of task list being executed */
    int nzip;              /* number of consecutive zero reads */
    int finished;          /* TRUE when all commands are finished */
    char *host;
    char *arch;
    tasklst *command;      /* simple commands to run */
    PROCESS *pp;
    fspec *filter;
    parstate *context;
    void (*check)(taskdesc *job, asyncstate *as, int *pnr, int *pnc);
    int (*start)(taskdesc *job, asyncstate *as, int launch);
    int (*launch)(taskdesc *job, asyncstate *as);
    int (*exec)(taskdesc *job, int sigp);
    int (*done)(taskdesc *job, int setst);
    int (*retryp)(taskdesc *job, subtask *sub, int sts, int sgn, int setst);
    int (*print)(taskdesc *job, asyncstate *as, char *fmt, ...);
    void (*redir)(taskdesc *job, asyncstate *as,
		  SC_filedes *fd, char *s, int newl);
    void (*tag)(taskdesc *job, char *tag, int nc, char *tm);
    void (*report)(taskdesc *job, char *where);
    int (*finish)(taskdesc *job, asyncstate *as, int srv);
    int (*close)(taskdesc *job, int setst);
    void (*add)(taskdesc *job);
    int (*remove)(taskdesc *job);};


struct s_SC_scope_proc
   {int current_flushed_process;
    int debug;
    FILE *diag;};

enum e_SC_proc_kind
   {SC_CHILD,
    SC_PARENT};

typedef enum e_SC_proc_kind SC_proc_kind;

enum e_SC_proc_disposition
   {NOT_FINISHED = -1000,
    REISSUED     = -1001,
    NO_REISSUE   = -1002};

typedef enum e_SC_proc_disposition SC_proc_disposition;

/* remote file/process I/O handling */

enum e_SC_file_oper
   {SC_FOPEN,
    SC_FSETVBUF,
    SC_FCLOSE,
    SC_FFLUSH,
    SC_FTELL,
    SC_FSEEK,
    SC_FREAD,
    SC_FWRITE,
    SC_FPUTS,
    SC_FGETS,
    SC_FGETC,
    SC_FUNGETC,
    SC_FEXIT,
    SC_NUM_NODES};

typedef enum e_SC_file_oper SC_file_oper;

/* message passing support */

enum e_SC_mp_oper
   {SC_READ_MSG,
    SC_WRITE_MSG};

typedef enum e_SC_mp_oper SC_mp_oper;

enum e_SC_mp_tag
   {SC_MATCH_TYPE,
    SC_MATCH_TAG,
    SC_MATCH_NODE,
    SC_MATCH_PID,
    SC_GROUP_LEADER,
    SC_BLOCK_STATE,
    SC_BUFFER_SIZE};

typedef enum e_SC_mp_tag SC_mp_tag;


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern SC_scope_proc
 _SC_ps;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/


/* SCHP.C declarations */

extern int
 _SC_redir_fail(SC_filedes *fd);

extern void
 SC_reset_terminal(void),
 _SC_dethread(void),
 _SC_redir_filedes(SC_filedes *fd, int nfd, int ifd, char *redir, char *name),
 _SC_set_filedes(SC_filedes *fd, int ifd, char *name, int fl),
 _SC_fin_filedes(SC_filedes *file),
 _SC_init_filedes(SC_filedes *fd),
 _SC_copy_filedes(SC_filedes *fb, SC_filedes *fa);

extern PROCESS
 *SC_open_remote(char *host, char *cmnd,
		 char **argv, char **envp, char *mode,
		 PROCESS *(*init)(char **argv, char *mode, int type));


/* SCHSRV.C declarations */

extern int
 _SC_ping_host(char *host, int to, int fm);


/* SCHTTP.C declarations */

extern int
 SC_connect(int fd, struct sockaddr *addr, socklen_t ln, int to);


/* SCINET.C declarations */

extern struct sockaddr_in
 *_SC_tcp_address(char *host, int port),
 *_SC_tcp_bind(int fd, int port);

extern int
 _SC_tcp_serve(int fd, struct sockaddr_in *ad, void *a,
	       int (*ex)(int *rv, void *a),
	       PFFileCallback acc, PFFileCallback rej),
 _SC_tcp_accept_connection(int fd, struct sockaddr_in *ad),
 _SC_tcp_connect(char *host, int port, int to, int fm);


/* SCLPPSX.C declarations */

extern PROCESS
 *SC_get_terminal_process(void);


/* SCPMAN.C declarations */

extern PROCESS
 *SC_hasharr_lookup_process(int pid);

extern SC_process_rusedes
 *SC_process_init_rusage(void);

extern int
 SC_child_kill(int pid),
 SC_child_status(int pid, int *pcnd, int *psts),
 SC_process_status(PROCESS *pp),
 SC_process_state(PROCESS *pp, int ev),
 SC_check_children(void),
 SC_running_children(void);

extern void
 SC_process_free_rusage(SC_process_rusedes *pru),
 _SC_set_process_status(PROCESS *pp, int sts, int rsn, char *tm),
 _SC_manage_process(PROCESS *pp),
 SC_save_exited_children(void),
 SC_handle_sigchld(int signo);


/* SCSYSA.C declarations */

extern void
 _SC_setup_output(jobinfo *inf, char *name),
 _SC_process_output(int fd, int mask, void *a),
 _SC_process_out_reject(int fd, int mask, void *a);


/* SCSYSB.C declarations */

extern int
 _SC_server_heartbeat(int *prv, void *a),
 _SC_exec_printf(asyncstate *as, char *fmt, ...);

extern void
 _SC_setup_async_state(asyncstate *as, int ln);


/* SCSYSC.C declarations */

extern fspec
 *_SC_read_filter(char *fname);

extern taskdesc
 *SC_make_taskdesc(parstate *state, int jid,
		   char *host, char *shell, char *dir, char *cmd);

extern char
 *_SC_show_command(asyncstate *as, char *s, int show),
 *_SC_put_command(SC_array *out, char *s, int show),
 *SC_get_shell(char *shell);

extern void
 _SC_set_run_task_state(parstate *state),
 _SC_exec_setup_state(parstate *state, char *shell, char **env,
		      int na, int show, int tag, int srv,
		      int ign, fspec *flt, SC_evlpdes *pe,
		      PFFileCallback acc, PFFileCallback rej,
		      int (*finish)(taskdesc *job, char *msg)),
 _SC_exec_free_state(parstate *state, int flt),
 _SC_free_filter(fspec *filter),
 _SC_print_filtered(asyncstate *as, char **msg, fspec *filter,
		    int jid, char *host);

extern int
 _SC_chg_dir(char *dir, char **pndir),
 _SC_get_command(tasklst *tl, int off),
 _SC_decide_retry(asyncstate *as, jobinfo *inf, tasklst *tl, int st),
 _SC_server_printf(asyncstate *as, parstate *state,
		   char *tag, char *fmt, ...);


/* SCSYSD.C declarations */

extern conpool
 *SC_open_connection_pool(int n, char *sys, char *shell, char **env,
			  int na, int show, int ignore,
			  int th, fspec *filter);

extern void
 SC_close_connection_pool(conpool *cp, int log, int sum),
 SC_show_pool_logs(conpool *cp, int n),
 SC_show_pool_stats(conpool *cp, int n, int full);

extern int
 SC_launch_pool_job(conpool *cp, int na, int reset,
		    char *shell, char *dir, char *cmnd),
 SC_wait_pool_job(conpool *cp, int to),
 SC_connection_pool_status(conpool *cp),
 SC_query_connection_pool(conpool *cp);


#ifdef __cplusplus
}
#endif

#endif

