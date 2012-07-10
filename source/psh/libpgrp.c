/*
 * LIBPGRP.C - process group management
 *           - grammar:
 *           - 
 *           - <command>   := <block> | <statement>
 *           - <block>     := '{' <statement> '}'
 *           - <statement> := <pgrp> | <pgrp> <st-sep> <statement>
 *           - <st-sep>    := ';'  |  '&&'  |  '||'
 *
 *           - <pgrp>      := <job> | <job> <pgrp-io> <pgrp>
 *           - <job>       := <program> <args> | <program> <args> <file-io>
 *           - <program>   := 'executable'
 *           - <args>      := <arg> | <arg> <args>
 *           - <arg>       := 'argument'
 *
 *           - <file-io>   := <shell-io> | <pgr-io>
 *           - <shell-io>  := <shell-op> | <shell-op> <file-name>
 *           - <file-name> := 'name of file'
 *           - <shell-op>  := '<' | '>' | '>&' | '&>' | 
 *                            '>>' | '>>&' |
 *           -                '>!' | '>&!' |
 *           -                '1>' | '2>' | '2>&1'
 *
 *           - <pgrp-io>   := <pipe-io> | <pgr-io>
 *           - <pipe-io>   := '|'  |  '|&'  | '&|'
 *
 *           - <pgr-io>    := <pgr-spec> | <pgr-spec> <pgr-io>
 *           - <pgr-spec>  := '@' <io-kind> <io-dev> <io-dst>
 *           - <io-kind>   := 'i' | 'o' | 'e' | 'b'
 *           - <io-dev>    := 'f' <file-name> |    // WO write only
 *                            'w' <file-name> |    // WD delete first
 *                            'a' <file-name> |    // WA append
 *                            'v' <var-name>  |
 *                            'e' <expr>
 *           - <io-dst>    := <io-kind>
 *           - 
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBPGRP

#define LIBPGRP

#include "common.h"
#include "libtime.c"
#include "libasync.c"

#define PROCESS_DELIM   '@'
#define PIPE_DELIM      "|"
#define N_IO_CH         3

typedef struct s_statement statement;

struct s_statement
   {int np;                 /* number of processes in group */
    st_sep terminator;      /* action terminator of group */
    char *text;             /* text of group */
    char *shell;            /* shell which runs the individual processes */
    char **env;
    process_group *pg;};

#if 0

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

typedef struct s_jobinfo jobinfo;
typedef struct s_taskdesc taskdesc;
typedef struct s_tasklst tasklst;
typedef struct s_subtask subtask;
typedef struct s_SC_process_group SC_process_group;
typedef struct s_SC_scope_proc SC_scope_proc;

/* command task description */

struct s_subtask
   {int need;              /* TRUE if shell is needed to run the subtask */
    int pipe;              /* TRUE if the subtask is a process group */
    int nt;                /* number of tokens of the subtask */
    int id;                /* id of process in group */
    int exit;              /* exit status value */
    SC_task_kind kind;     /* compound command or process group */
    char *shell;
    char *command;         /* full text of the subtask */
    char *ios;             /* text representation of I/O connections */
    char **argf;           /* tokenized version suitable for SC_open */
    char **env;            /* environment variables for command */
    SC_iodes fd[SC_N_IO_CH];};

struct s_SC_process_group
   {int np;                 /* number of processes in group */
    int to;                 /* group time out */
    int rcpu;               /* */
    char *mode;             /* IPC mode */
    subtask *jobs;          /* array of jobs in group */
    PROCESS *terminal;      /* terminal process */
    PROCESS **parents;      /* parent process array */
    PROCESS **children;};   /* child process array */


/*--------------------------------------------------------------------------*/

/*                             EXEC ERROR STATUS                            */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                    REMOTE FILE/PROCESS I/O HANDLING                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                              EXEC_ASYNC SUPPORT                          */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                             PROCEDURAL MACROS                            */

/*--------------------------------------------------------------------------*/

#ifdef HAVE_PROCESS_CONTROL

static char
 *_SC_not_stopped = "running";

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MK_PROCESS - initialize and return a PROCESS */

PROCESS *SC_mk_process(char **argv, char *mode, int type, int iex)
   {int i;
    unsigned int nb;
    PROCESS *pp;

    pp = CMAKE(PROCESS);

    if ((iex < 10) || (30 < iex))
       nb = SIZE_BUF;
    else
       nb = 1 << iex;

/* NOTE: the value of pp->id can be passed to waitpid so the initial
 * value of -1 is essential!
 */
    pp->id          = -1;

    pp->status      = NOT_FINISHED;
    pp->reason      = NOT_FINISHED;

    for (i = 0; i < SC_N_IO_CH; i++)
        pp->io[i] = -1;

    pp->data        = -1;
    pp->medium      = NO_IPC;
    pp->gone        = -1;
    pp->data_type   = SC_UNKNOWN;

    pp->nb_ring     = nb;
    pp->in_ring     = CMAKE_N(unsigned char, nb);
    pp->ib_in       = 0;
    pp->ob_in       = 0;

    memset(pp->in_ring, 0, nb);

    pp->type        = SC_LOCAL;
    pp->rcpu        = -1;
    pp->acpu        = -1;

    pp->line_mode   = FALSE;
    pp->line_sep    = 0x0a;
    pp->blocking    = FALSE;

    pp->n_pending   = 0;
    pp->pending     = NULL;
    pp->vif         = NULL;
    pp->spty        = NULL;

    pp->index       = SC_PROC_ALLOC;
    pp->flags       = 0;

    pp->data_buffer = NULL;
    pp->nb_data     = 0L;
    pp->nx_data     = 0L;

    pp->n_zero      = 0;                     /* work around OSF SIGCHLD bug */
    pp->n_read      = 0L;
    pp->n_write     = 0L;

    pp->start_time  = NULL;
    pp->stop_time   = NULL;

/* function and arg to call when process finishes */
    pp->on_exit     = NULL;
    pp->exit_arg    = NULL;

/* for binary data */
    pp->read         = NULL;
    pp->write        = NULL;
    pp->recv_formats = NULL;
    pp->send_formats = NULL;
    pp->rl_vif       = NULL;

    pp->open_retry   = -1;
    _SC_init_filedes(pp->fd);

    pp->tty = NULL;

    if (type == SC_PARENT)
       {pp->ischild = FALSE;
        _SC_manage_process(pp);}
    else
       pp->ischild = TRUE;

/* application supplied heuristic function to identify lost processes */
    pp->lost = NULL;

    SC_PROCESS_METHODS(pp);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_IPC - establish two inter-process communications channels
 *              - use one of three media:
 *              -  1) open an available pseudo terminal, pty
 *              -     connect the child to the master side and
 *              -     the parent to the slave side (thanks to Neale Smith
 *              -     for the explanation of PTY's)
 *              -  2) socket
 *              -  3) pipe
 *              - the input channel should always be unblocked
 *              - return TRUE iff successful
 */

#ifdef HAVE_PROCESS_CONTROL

static int _SC_init_ipc(PROCESS *pp, PROCESS *cp)
   {int ret;

    ret = TRUE;

#ifdef HAVE_PROCESS_CONTROL

    switch (pp->medium)
       {case USE_PTYS :
	     ret = _SC_init_pty(pp, cp);
             break;

        case USE_PIPES :
             {int ports[2];

/* child stdin */
              if (pipe(ports) < 0)
                 {close(pp->io[0]);
                  close(cp->io[1]);
                  SC_error(-1, "COULDN'T CREATE PIPE #1 - _SC_INIT_IPC");};

              cp->io[0] = ports[0];
              pp->io[1] = ports[1];

/* child stdout */
              if (pipe(ports) < 0)
                 SC_error(-1, "COULDN'T CREATE PIPE #2 - _SC_INIT_IPC");

              cp->io[1] = ports[1];
              pp->io[0] = ports[0];

/* child stderr */
              if (pipe(ports) < 0)
                 {close(pp->io[0]);
                  close(cp->io[1]);
                  SC_error(-1, "COULDN'T CREATE PIPE #3 - _SC_INIT_IPC");};

              cp->io[2] = ports[0];
              pp->io[2] = ports[1];

              SC_set_fd_attr(pp->io[0], O_RDONLY | O_NDELAY, TRUE);
              SC_set_fd_attr(pp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(pp->io[2], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[0], O_RDONLY & ~O_NDELAY, TRUE);
              SC_set_fd_attr(cp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[2], O_WRONLY, TRUE);};

              break;

# ifdef HAVE_SOCKETS_P

        case USE_SOCKETS :
             {int ports[2];

/* child stdin */
              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 {close(pp->io[0]);
                  close(cp->io[1]);
                  SC_error(-1, "COULDN'T CREATE SOCKET PAIR #1 - _SC_INIT_IPC");};
              cp->io[0] = ports[0];
              pp->io[1] = ports[1];

/* child stdout */
              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 SC_error(-1, "COULDN'T CREATE SOCKET PAIR #2 - _SC_INIT_IPC");
              cp->io[1] = ports[1];
              pp->io[0] = ports[0];

/* child stderr */
              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 {close(pp->io[0]);
                  close(cp->io[1]);
                  SC_error(-1, "COULDN'T CREATE SOCKET PAIR #3 - _SC_INIT_IPC");};
              cp->io[2] = ports[1];
              pp->io[2] = ports[0];

              SC_set_fd_attr(pp->io[0], O_RDONLY | O_NDELAY, TRUE);
              SC_set_fd_attr(pp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(pp->io[2], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[0], O_RDONLY & ~O_NDELAY, TRUE);
              SC_set_fd_attr(cp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[2], O_WRONLY, TRUE);};

             break;

# endif

        default :
             ret = FALSE;};

#endif

    return(ret);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHILD_FORK - the child process comes here and
 *                - it will never return
 */

#ifdef HAVE_PROCESS_CONTROL

static void _SC_child_fork(PROCESS *pp, PROCESS *cp, int to,
			   char **argv, char **envp, char *mode)
   {int rv;

/* clear the timer */
    ALARM(0);

/* set all signal handlers to their default state */
    SC_set_signal_handlers(SIG_DFL, NULL, 0, SC_NSIG);

/* free the parent state which the child does not need */
    pp->release(pp);

/* setup the running child state */
    cp->ischild    = TRUE;
    cp->start_time = SC_datef();
    cp->stop_time  = _SC_not_stopped;

    rv = SC_EXIT_SELF;
    if (cp->exec != NULL)
       {rv = cp->exec(cp, argv, envp, mode);

	io_printf(stdout, "%s\n", SC_error_msg());};

    exit(rv);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PARENT_FORK - the parent process comes here */

#ifdef HAVE_PROCESS_CONTROL

static int _SC_parent_fork(PROCESS *pp, PROCESS *cp, int to,
                           int rcpu, char *mode)
   {int i, st;
   
    st = TRUE;

    pp->id         = cp->id;
    pp->rcpu       = rcpu;
    pp->start_time = SC_datef();
    pp->stop_time  = _SC_not_stopped;
    pp->pru        = SC_process_init_rusage();

    SC_process_state(pp, SC_PROC_EXEC);

#ifdef F_SETOWN
    if (pp->medium == USE_SOCKETS)
       fcntl(pp->io[0], F_SETOWN, pp->root);
#endif

    if (strchr(mode, 'o') != NULL)
       _SC_dethread();

    if (SC_BINARY_MODEP(mode))
       {if (!(*pp->recv_formats)(pp))
	   pp = NULL;};

    if (SC_process_alive(pp) && (pp->medium == USE_PTYS))
       {if (_SC.orig_tty != NULL)
	   SC_set_raw_state(0, FALSE);

	SC_set_raw_state(pp->io[0], FALSE);

	for (i = 0; i < SC_N_IO_CH; i++)
	    cp->io[i] = -1;};

    cp->release(cp);
    cp = NULL;

    return(st);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ERROR_FORK - come here if fork failed
 *                - just cleanup the remains
 */

#ifdef HAVE_PROCESS_CONTROL

static void _SC_error_fork(PROCESS *pp, PROCESS *cp)
   {

/* mark this as having to do with the child so that
 * the managed process list works right
 */
    pp->ischild = TRUE;

    pp->release(pp);

    cp->release(cp);

    SC_error(-1, "COULDN'T FORK PROCESS - _SC_ERROR_FORK");

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_GROUP_TASKS - scan ARGV for pipe indicators and
 *                       - NULL out those entries
 *                       - return a list of offsets into ARGV for each
 *                       - individual process
 *                       - in effect break ARGV into multiple ARGVs
 *                       - one for each process
 */

#ifdef HAVE_PROCESS_CONTROL

static subtask *_SC_make_group_tasks(char **argv, char **env)
   {int i, n, na;
    char cmd[MAXLINE];
    char **al;
    subtask *pg;

    n = 1;
    for (i = 0; argv[i] != NULL; i++)
        n += ((strcmp(argv[i], SC_PIPE_DELIM) == 0) ||
	      (argv[i][0] == SC_PROCESS_DELIM));

    pg = CMAKE_N(subtask, n+1);
    SC_MEM_INIT_N(subtask, pg, n+1);

    n          = 0;
    pg[n].argf = argv;
    pg[n].env  = env;

    for (i = 0; argv[i] != NULL; i++)
        {if ((strcmp(argv[i], SC_PIPE_DELIM) == 0) ||
	     (argv[i][0] == SC_PROCESS_DELIM))
            {pg[n].ios  = argv[i];

	     if (argv[i+1] != NULL)
	        {n++;
		 pg[n].argf = argv + i + 1;
		 pg[n].env  = env;};

	     argv[i] = NULL;};};

    for (i = 0; i <= n; i++)
        {al = pg[i].argf;
	 SC_ptr_arr_len(na, al);
	 SC_concatenate(cmd, MAXLINE, na, al, " ", FALSE);
	 pg[i].command = STRSAVE(cmd);
	 pg[i].kind    = TASK_GROUP;};

    n++;
    pg[n].command = NULL;

    return(pg);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_GROUP_N - return the number of processes
 *                     - in the process_group PG
 */

#ifdef HAVE_PROCESS_CONTROL

static int _SC_process_group_n(subtask *pg)
   {int n;

    for (n = 0; pg[n].command != NULL; n++);

    return(n);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_IO_SPEC - initialize an SC_iodes specification PG->fd[KIND] */

static void _SC_init_io_spec(subtask *pg, int n, int id)
   {

    pg->fd[IO_STD_IN].knd = IO_STD_IN;
    pg->fd[IO_STD_IN].fd  = -1;
    pg->fd[IO_STD_IN].gid = pg->id - 1;

    pg->fd[IO_STD_OUT].knd = IO_STD_OUT;
    pg->fd[IO_STD_OUT].fd  = -1;
    pg->fd[IO_STD_OUT].gid = (id < n-1) ? pg->id + 1 : -1;

    pg->fd[IO_STD_ERR].knd = IO_STD_ERR;
    pg->fd[IO_STD_ERR].fd  = -1;
    pg->fd[IO_STD_ERR].gid = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_KIND_IO - return the character matching SC_io_kind K */

int _SC_kind_io(SC_io_kind k)
   {int rv;

    switch (k)
       {case IO_STD_IN :
	     rv = 'i';
	     break;
        case IO_STD_OUT :
	     rv = 'o';
	     break;
        case IO_STD_ERR :
	     rv = 'e';
	     break;
        case IO_STD_BOND :
	     rv = 'b';
	     break;
        default :
	     rv = '\0';
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_IO_SPEC - initialize an SC_iodes specification PG->fd[KIND]
 *                 - according to LINK whose syntax is:
 *                 -   <ios><gid><iod>
 *                 - where
 *                 -   <ios>  := 'i' | 'o' | 'e' | 'b'
 *                 -   <gid>  := <n> | +<n> | -<n> | <expr>
 *                 -   <expr> := 't' | <var> | <file>
 *                 -   <var>  := environment variable
 *                 -   <file> := file name
 */

static void _SC_set_io_spec(subtask *pg, int n, int id,
			    SC_io_kind kind, char *link)
   {int fd, gid, nc, md, prm, excl;
    SC_io_device dev;
/*    SC_io_kind ios, iod; */
    SC_iodes ios, iod;
    char s[MAXLINE];
    char *p, *ps, *nm;
    SC_iodes *ips, *ipd;
    subtask *pgs, *pgd;

    if ((0 <= id) && (id < n))
       {pgs  = pg + id;
	fd  = -1;
	gid = -1;
	dev = IO_DEV_NONE;

	SC_strncpy(s, MAXLINE, link, -1);
	nc  = strlen(s);
	_SC_io_kind(&ios, s);
	_SC_io_kind(&iod, s + nc - 1);
/*
	_SC_io_kind(s, &ios.knd, NULL, NULL);
	_SC_io_kind(s + nc - 1, &iod.knd, NULL, NULL);
*/
	SC_ASSERT(ios.knd == kind);

	ps = s + 1;
	switch (ps[0])
	   {case '+' :
	    case '-' :
	         dev = IO_DEV_PIPE;
		 gid = pgs->id + SC_stoi(ps);
		 break;
	    case 'e' :
		 dev = IO_DEV_EXPR;
		 break;

	    case 'a' :
	    case 'f' :
	    case 'w' :
	         nm  = ps + 1;
		 dev = IO_DEV_FILE;

/* GOTCHA: what should the real permissions be? */
		 prm = 0600;
		 prm = SC_get_perm(FALSE);

/* do not try to use O_EXCL bit with devices - think about it */
		 if (strncmp(nm, "/dev/", 5) == 0)
		    excl = 0;
		 else
		    excl = O_EXCL;

/*  <   ->  O_RDONLY */
		 if (kind == IO_STD_IN)
		    fd = open(nm, O_RDONLY);
		 else
		    {switch (ps[0])

/*  >   ->  O_WRONLY | O_CREAT | O_EXCL */
		        {case 'f' :
			      md = O_WRONLY | O_CREAT | excl;
			      break;

/*  >!  ->  O_WRONLY | O_CREAT | O_TRUNC */
			 case 'w' :
			      md = O_WRONLY | O_CREAT | O_TRUNC;
			      break;

/*  >>  ->  O_WRONLY | O_APPEND */
			 case 'a' :
			      md = O_WRONLY | O_CREAT | O_APPEND;
			      break;};
		     fd = open(nm, md, prm);};
		 break;

	    case 's' :
		 dev = IO_DEV_SOCKET;
		 {int to, fm, port;
		  char *host;

		  to = 10000;         /* timeout after 10 seconds */
		  fm = FALSE;         /* fatal on timeout */

		  host = ps + 1;
		  p    = strchr(host, ':');
		  *p++ = '\0';
		  port = SC_stoi(p);

		  fd = _SC_tcp_connect(host, port, to, fm);};
		 break;
	    case 't' :
		 dev = IO_DEV_TERM;
		 s[nc-1] = '\0';
		 break;
	    case 'v' :
		 dev = IO_DEV_VAR;
		 break;
	    default :
		 dev = IO_DEV_PIPE;
		 if (SC_numstrp(ps) == TRUE)
		    gid = SC_stoi(ps) - 1;
		 else
		    {s[nc-1] = '\0';
		     if (SC_numstrp(ps) == TRUE)
		        gid = SC_stoi(ps) - 1;};
		 break;};

/* set the destination io spec for a pipe */
	if ((dev == IO_DEV_PIPE) &&
	    (0 <= gid) && (gid < n))
	   {pgd = pg + gid;
	    if ((0 <= iod.knd) && (iod.knd < SC_N_IO_CH))
	       {ipd = pgd->fd + iod.knd;
		if (ipd != NULL)
		   {ipd->knd = iod.knd;
		    ipd->dev = dev;
		    ipd->fd  = fd;
		    ipd->gid = id;};};};

/* set the source io spec */
	if ((0 <= ios.knd) && (ios.knd < SC_N_IO_CH))
	   {ips = pgs->fd + ios.knd;
	    if (ips != NULL)
	       {ips->knd = ios.knd;
		ips->dev = dev;
		ips->fd  = fd;
		ips->gid = gid;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_GROUP_PARSE - parse out the I/O specifications for the ID job
 *                         - out of the N PG
 */

static void _SC_process_group_parse(subtask *pg, int n, int id)
   {int i;
    char **sa;
    subtask *pgs;

    if ((pg != NULL) && (id >= 0))
       {pgs = pg + id;

	pgs->id   = id;
	pgs->exit = -1;

	_SC_init_io_spec(pgs, n, id);
	if (pgs->ios != NULL)
	   {sa = SC_tokenize(pgs->ios, "@");

	    for (i = 0; sa[i] != NULL; i++)
	        {switch (sa[i][0])
		    {case 'i' :
		          _SC_set_io_spec(pg, n, id, IO_STD_IN,  sa[i]);
			  break;
		     case 'o' :
			  _SC_set_io_spec(pg, n, id, IO_STD_OUT, sa[i]);
			  break;
		     case 'e' :
			  _SC_set_io_spec(pg, n, id, IO_STD_ERR, sa[i]);
			  break;
		     case 'b' :
			  _SC_set_io_spec(pg, n, id, IO_STD_OUT, sa[i]);
			  _SC_set_io_spec(pg, n, id, IO_STD_ERR, sa[i]);
			  break;};};

	    SC_free_strings(sa);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RECONNECT_PROCESS_GROUP - reconnect the N PROCCESS's in PA and CA
 *                             - from the canonical parent/child topology
 *                             - into a process_group topology
 *                             - NOTE: this logic is good for both pipes and
 *                             - sockets but bad for PTYs
 *                             - on the other hand why do PTYs
 *                             - in a process_group
 */

#ifdef HAVE_PROCESS_CONTROL

static void _SC_reconnect_process_group(int n, subtask *pg,
					PROCESS **pa, PROCESS **ca)
   {int i, ide, ido, nm;
    subtask *g;
    PROCESS *pp, *cp, *pn, *cn;

    nm = n - 1;

/* remove the parent to child channels except for the last one
 * the final parent out gets connected to the first child in
 */
    if (nm > 0)

/* set any sockets to read/write mode */
       {for (i = 0; i < n; i++)
	    {pp = pa[i];
	     cp = ca[i];
	     if (cp->medium == USE_SOCKETS)
	        {SC_set_fd_attr(pp->io[0], O_RDWR,   -1);
		 SC_set_fd_attr(pp->io[1], O_RDWR,   -1);
		 SC_set_fd_attr(pp->io[2], O_WRONLY, -1);
		 SC_set_fd_attr(cp->io[0], O_RDWR,   -1);
		 SC_set_fd_attr(cp->io[1], O_RDWR,   -1);
		 SC_set_fd_attr(cp->io[2], O_WRONLY, -1);};};

/* reconnect terminal process output to first process */
	pp = pa[nm];
	pn = pa[0];
	close(pp->io[1]);
	pp->io[1] = pn->io[1];
	pn->io[1] = -2;

/* close all other parent to child lines */
	for (i = 1; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

	     close(pp->io[1]);
	     close(cp->io[0]);

	     pp->io[1] = -2;
	     cp->io[0] = -2;};

/* connect all non-terminal child out to next child in */
	for (i = 0; i < nm; i++)
	    {g = pg + i;

	     ido = g->fd[IO_STD_OUT].gid;
	     ide = g->fd[IO_STD_ERR].gid;

	     ide = i + 1;

	     pp = pa[i];
	     cp = ca[i];
	     cn = ca[ido];

	     close(cn->io[0]);

	     cn->io[0] = pp->io[0];
	     pp->io[0] = -2;};};

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_PROC - allocate PROCESS structures for the parent (PP)
 *                - and the child (CP)
 *                - do the initial I/O setup and any other
 *                - preparation prior to the fork
 *                - return the timeout in milliseconds for the post-fork
 *                - setup and exec
 */

#ifdef HAVE_PROCESS_CONTROL

static int _SC_setup_proc(PROCESS **ppp, PROCESS **pcp,
			  char **argv, char *mode,
			  SC_iodes *fd, int retry, int iex,
			  PFProcInit initf, PFProcExit exitf, void *exita)
   {int to, flag, rpid, savetty;
    PROCESS *pp, *cp;

    if (initf == NULL)
       {cp = SC_mk_process(argv, mode, SC_CHILD, iex);
        pp = SC_mk_process(argv, mode, SC_PARENT, iex);}
    else
       {cp = (*initf)(argv, mode, SC_CHILD);
        pp = (*initf)(argv, mode, SC_PARENT);};

    if (fd != NULL)
       _SC_copy_filedes(cp->fd, fd);

    savetty = FALSE;
    switch (mode[1])
       {case 't' :
             savetty = TRUE;
        case 'T' :
             pp->medium = USE_PTYS;
             cp->medium = USE_PTYS;
             break;
        case 's' :
             pp->medium = USE_SOCKETS;
             cp->medium = USE_SOCKETS;
             break;
        default  :
        case 'p' :
             pp->medium = USE_PIPES;
             cp->medium = USE_PIPES;
             break;};

    if (SC_BINARY_MODEP(mode))
       {pp->data_type = SC_BINARY;
        cp->data_type = SC_BINARY;}
    else
       {pp->data_type = SC_ASCII;
        cp->data_type = SC_ASCII;};

/* mixing PROCESS's using PTYs and other media is a bad idea at
 * this point
 */
    if ((savetty == TRUE) && (_SC.orig_tty == NULL))
       _SC.orig_tty = SC_get_term_state(0, TRUE);

/* set up the communications pipe */
    flag = _SC_init_ipc(pp, cp);
    if (flag == FALSE)
       {CFREE(pp->tty);
	CFREE(pp);
	CFREE(cp->tty);
        CFREE(cp);
        SC_error(-1, "COULDN'T CREATE IPC CHANNELS - _SC_SETUP_PROC");
	return(-1);};

/* disable SIGTTOU when running in background */
    if (SC_is_background_process(-1) & 1)
       SC_io_suspend(FALSE);

    if (exitf != NULL)
       {pp->on_exit  = exitf;
	pp->exit_arg = exita;};

    pp->gone       = FALSE;
    pp->status     = SC_RUNNING;
    pp->reason     = 0;
    cp->gone       = FALSE;
    cp->status     = SC_RUNNING;
    cp->reason     = 0;
    cp->open_retry = retry;

    rpid     = getpid();
    pp->root = rpid;
    cp->root = rpid;

    *ppp = pp;
    *pcp = cp;

#ifdef SOLARIS
    to = 2000;
#else
    to = 60000;
#endif

    return(to);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_PROCESS_GROUP - setup and return a process group
 *                         - as specified in ARGV
 */

static SC_process_group *_SC_setup_process_group(char **argv, char **envp,
						 char *mode, SC_iodes *fd,
						 int retry, int iex,
						 int rcpu,
						 PFProcInit initf,
						 PFProcExit exitf,
						 void *exita)
   {int i, n, to;
    char *mod, **al;
    PROCESS **pa, **ca;
    subtask *pg;
    SC_iodes *pfd;
    SC_process_group *pgr;

    pg = _SC_make_group_tasks(argv, envp);
    n  = _SC_process_group_n(pg);

    pa = CMAKE_N(PROCESS *, n);
    ca = CMAKE_N(PROCESS *, n);

/* setup each process in the process_group */
    for (i = 0; i < n; i++)
        {al = pg[i].argf;

/* NOTE: use sockets for the non-terminal process */
	 mod = (i < n-1) ? "as" : mode;
	 pfd = (i < n-1) ? NULL : fd;

	 to = _SC_setup_proc(&pa[i], &ca[i], al, mod,
			     pfd, retry, iex, initf, exitf, exita);};

/* parse out the process group links */
    for (i = 0; i < n; i++)
        _SC_process_group_parse(pg, n, i);

    _SC_reconnect_process_group(n, pg, pa, ca);

    pgr = CMAKE(SC_process_group);
    if (pgr != NULL)
       {pgr->np       = n;
	pgr->to       = to;
	pgr->rcpu     = rcpu;
	pgr->mode     = mode;
	pgr->jobs     = pg;
	pgr->terminal = NULL;
	pgr->parents  = pa;
	pgr->children = ca;};

    return(pgr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_PROCESS_GROUP - release a process group instance */

static void _SC_free_process_group(SC_process_group *pgr)
   {

    if (pgr != NULL)
       {CFREE(pgr->jobs);
	CFREE(pgr->parents);
	CFREE(pgr->children);
	CFREE(pgr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LAUNCH_PROCESS_GROUP - fork and exec each job
 *                          - in the process group PGR
 *                          - return the terminal process
 */

static PROCESS *_SC_launch_process_group(SC_process_group *pgr)
   {int i, n, pid, to, st, rcpu;
    char *md, **al, **el;
    PROCESS *pp, *cp, **pa, **ca;
    subtask *pg;

    n    = pgr->np;
    to   = pgr->to;
    pg   = pgr->jobs;
    pa   = pgr->parents;
    ca   = pgr->children;
    md   = pgr->mode;
    rcpu = pgr->rcpu;

/* launch each process in the process_group */
    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];

         al = pg[i].argf;
	 el = pg[i].env;

/* fork the process */
         pid    = fork();
	 pp->id = pid;
	 cp->id = pid;

	 switch (pid)
	    {case -1 :
	          _SC_error_fork(pp, cp);
		  pp = NULL;
		  break;

/* the child process comes here and if successful will never return */
	     case 0 :
		  _SC_child_fork(pp, cp, to, al, el, md);
		  break;

/* the parent process comes here */
	     default :
	          st = _SC_parent_fork(pp, cp, to, rcpu, md);
		  if (st == FALSE)
		     {SC_close(pp);
		      pp = NULL;};
		  break;};};

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OPEN_PROC - start up a process on the current CPU
 *               - and open a connection to it for further communications
 */

static PROCESS *_SC_open_proc(int rcpu, char *name, char **argv,
			      char **envp, char *mode,
			      SC_iodes *fd, int retry, int iex,
			      PFProcInit initf, PFProcExit exitf, void *exita)
   {PROCESS *pp;

    pp = NULL;

#ifdef HAVE_PROCESS_CONTROL

    SC_process_group *pgr;

    if (_SC_redir_fail(fd) != -1)
       return(NULL);

    pgr = _SC_setup_process_group(argv, envp, mode, fd, retry, iex, rcpu,
				  initf, exitf, exita);

    pp  = _SC_launch_process_group(pgr);

    _SC_free_process_group(pgr);

#endif

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_REMOTE - start up CMND with arguments ARGV on a remote HOST
 *                - and open a connection to it for further communications
 *                - INIT is a function to special initializations in the
 *                - PROCESS
 *                - return (PROCESS *) -1    if the open fails
 *                - return (PROCESS *) st-2  if the host cannot be verified
 */

PROCESS *SC_open_remote(char *host, char *cmnd,
			char **argv, char **envp, char *mode,
			PFProcInit init)
   {int i, ia, na, st, ok;
    char **ca;
    PROCESS *pp;

    pp         = NULL;
    SC_gs.errn = 0;

    if (argv == NULL)
       na = 0;
    else
       SC_ptr_arr_len(na, argv);

    ca = CMAKE_N(char *, na+7);

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {st = SC_verify_host(host, -2);
	if (st > 0)
	   {i = 0;
	    ca[i++] = "ssh";
	    ca[i++] = "-o";
	    ca[i++] = "SendEnv=SC_EXEC_RLIMIT_AS";
	    ca[i++] = "-q";
	    ca[i++] = host;
	    ca[i++] = cmnd;

	    for (ia = 0; ia < na; ia++)
	        ca[i++] = argv[ia];

	    ca[i++] = NULL;

	    pp = SC_open(ca, envp, mode, "INIT", init, NULL);}

	else
	   SC_error(st-10, "CANNOT VERIFY HOST %s - SC_OPEN_REMOTE", host);

	if (pp == NULL)
	   SC_error(SC_gs.errn, "OPEN FAILED %s - SC_OPEN_REMOTE", host);}

    else
      {SC_gs.errn = ok;
       pp         = NULL;};

    SC_ERR_UNTRAP();

    CFREE(ca);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BUFFER_IN - put new bytes on the ring
 *              - everything must go into the buffer
 *              - the routine that extracts from the buffer will
 *              - decide whether ASCII or BINARY information is expected
 *              - return TRUE iff there is enough room to hold the bytes
 */

int SC_buffer_in(char *bf, int n, PROCESS *pp)
   {int i;
    unsigned int ib, ob, ab, db, mb, nb, nnb;
    unsigned char *ring, *po, *pn;

    ib   = pp->ib_in;
    ob   = pp->ob_in;
    nb   = pp->nb_ring;
    ring = pp->in_ring;    

/* compute available space remaining in the ring */
    while (TRUE)
       {if (ib >= ob)
	   ab = nb - ib + ob;
        else
	   ab = ob - ib;

	if (ab >= n)
	   break;

/* find the size delta - try doubling */
	db = nb;

/* resize the ring */
	nnb = nb + db;
	CREMAKE(ring, unsigned char, nnb);

/* adjust the ring if the "in use" section is discontiguous */
	if (ib < ob)
	   {mb = nb - ob;
	    pn = ring + nnb - 1;
	    po = ring + nb - 1;
	    for (i = 0; i < mb; i++)
	        *pn-- = *po--;

	    ob += db;};

	nb = nnb;

	pp->in_ring = ring;
	pp->nb_ring = nb;
	pp->ob_in   = ob;};

    if (bf != NULL)
       {for (i = 0; i < n; i++, ib = (ib + 1) % nb)
            ring[ib] = *bf++;

        pp->ib_in = ib;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BUFFER_OUT - return the oldest message from the ring in BF */

int SC_buffer_out(char *bf, PROCESS *pp, int n, int binf)
   {int i, ok;
    unsigned int ib, nb, ob, ls;
    unsigned char *ring, c;
    char *pbf;

/* setup to copy from the ring to the buffer */
    ib   = pp->ib_in;
    ob   = pp->ob_in;
    nb   = pp->nb_ring;
    ring = pp->in_ring;    
    ls   = pp->line_sep;

/* copy until hitting a line separator or N characters have been copied */
    pbf = bf;
    ok  = TRUE;
    for (i = 0; ok && (ob != ib) && (i < n); i++, ob = (ob + 1) % nb)
        {c        = ring[ob];
	 *pbf++   = c;
	 ring[ob] = '\0';

	 if ((c == ls) && (binf == FALSE))
	    ok = FALSE;};

/* null terminate unless the character count maxed out */
    if (i < n)
       *pbf++ = '\0';

    pp->ob_in = ob;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_SIGPIPE - handle a SIGPIPE for PROCESS I/O */

static void _SC_handle_sigpipe(int signo)
   {

    SC_error(-2, "BROKEN PIPE - _SC_HANDLE_SIGPIPE");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_INPUT - get a string from the specified process
 *               - if none are available or there is an error return NULL
 */

#ifdef HAVE_PROCESS_CONTROL

static char *_SC_get_input(char *bf, int len, PROCESS *pp)
   {int status, blck, reset, run, nb, st;
    char *pbf;
    int (*get)(char *bf, int len, PROCESS *pp);

    if (pp == NULL)
       return(NULL);

    st = SC_status(pp);

/* if the job is not running then it must not be blocked */
    run           = (st == SC_RUNNING);
    pp->blocking &= run;

/* NOTE: attempt to be a bit safer by accessing pp->gets just one time */
    get = pp->gets;

    blck   = pp->blocking;
    status = pp->in_ready(pp);
    reset  = -1;

/* if unblocked when should be blocked */
    if (status && blck)
       {SC_block_fd(pp->io[0]);
        status = FALSE;
        reset  = FALSE;}

/* if blocked when should be unblocked */
    else if (!status && !blck)
       {SC_unblock_fd(pp->io[0]);
        status = TRUE;
        reset  = TRUE;};

/* if we are unblocked check incoming messages */
    pbf = bf;
    if (status)
       {nb = (*get)(bf, len, pp);
        if (!SC_buffer_in(bf, nb, pp))
           SC_error(-1, "NOT ENOUGH ROOM IN BUFFER - _SC_GET_INPUT");

        if (pp->line_mode)
           {if (_SC_complete_messagep(pp))
               pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;
            else
               pbf = NULL;}
        else
           pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;}

/* if we are blocked and there are already messages return the next one */
    else if (_SC_complete_messagep(pp))
       pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;

/* if we are blocked and there are no messages go wait for one with
 * full FGETS semantics (i.e. terminated with \n)
 */
    else
       {while (TRUE)
          {nb = (*get)(bf, len, pp);
           if (!SC_buffer_in(bf, nb, pp))
              SC_error(-1, "NOT ENOUGH ROOM IN BUFFER - _SC_GET_INPUT");

	   if (_SC_complete_messagep(pp))
	      {pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;
	       break;};

	   SC_sleep(100);};};

/* reset the blocking to its state upon entry */
    switch (reset)
       {case 0 :
	     SC_unblock_fd(pp->io[0]);
	     break;
        case 1 :
	     SC_block_fd(pp->io[0]);
	     break;};

    return(pbf);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GETS - get a string from the specified process
 *         - if none are available or there is an error return NULL
 */

char *SC_gets(char *bf, int len, PROCESS *pp)
   {char *pbf;

#ifdef HAVE_PROCESS_CONTROL

    int ok, st;
    SC_contextdes oph;

    pbf = NULL;
    oph = SC_signal_n(SIGPIPE, _SC_handle_sigpipe, NULL);

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {if (!SC_process_alive(pp))
	   SC_error(-1, "BAD PROCESS - SC_GETS");

	st = SC_status(pp);
	if ((st & SC_DEAD) == 0)
	   pbf = _SC_get_input(bf, len, pp);}

    else if (ok == -2)
       {SC_close(pp);};

    SC_ERR_UNTRAP();

    SC_signal_n(SIGPIPE, oph.f, oph.a);

#else

    pbf = bf;

#endif

    return(pbf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PRINTF - do an fprintf style output to a process */

int SC_printf(PROCESS *pp, char *fmt, ...)
   {int ret, ok;
    int (*prnt)(PROCESS *pp, char *bf);
    char *bf;
    SC_contextdes oph;

    ret = FALSE;
    oph = SC_signal_n(SIGPIPE, _SC_handle_sigpipe, NULL);

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {if (!SC_process_alive(pp))
	   SC_error(-1, "BAD PROCESS - SC_PRINTF");

/* NOTE: attempt to be a bit safer by accessing pp->printf just one time */
	if (pp != NULL)
	   prnt = pp->printf;

        SC_VDSNPRINTF(TRUE, bf, fmt);

	if (SC_process_alive(pp) && (prnt != NULL))
	   ret = (*prnt)(pp, bf);

	CFREE(bf);}

    else if (ok == -2)
       {SC_close(pp);};

    SC_ERR_UNTRAP();

    SC_signal_n(SIGPIPE, oph.f, oph.a);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REDIR_FAIL - return the index of the first of the redirections
 *                - which will fail or -1 if all will succeed
 *                - so that we can quit early and not even fork
 */

int _SC_redir_fail(SC_iodes *fd)
   {int fl, fail;
    char *nm;

    fail = -1;

/* shell redirection syntax and file mode correspondence
 *  <   ->  O_RDONLY
 *  >   ->  O_WRONLY | O_CREAT | O_EXCL
 *  >!  ->  O_WRONLY | O_CREAT | O_TRUNC
 *  >>  ->  O_WRONLY | O_APPEND
 */

/* input redirection fails if file does not exist */
    if (fail == -1)
       {nm = fd[0].file;
	fl = fd[0].flag;
	if ((nm != NULL) && (SC_isfile(nm) == FALSE))
	   fail = 0;};

/* stdout redirection fails if file exists and mode has exclusive bit */
    if (fail == -1)
       {nm = fd[1].file;
	fl = fd[1].flag;
	if ((nm != NULL) && (SC_isfile(nm) == TRUE) && (fl & O_EXCL))
	   fail = 1;};

/* stderr redirection fails if file exists and mode has exclusive bit */
    if (fail == -1)
       {nm = fd[2].file;
	fl = fd[2].flag;
	if ((nm != NULL) && (SC_isfile(nm) == TRUE) && (fl & O_EXCL))
	   fail = 2;};

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_FILEDES - set the parts of FD implied by REDIR
 *                 - NAME specifies the file to be used
 */

void _SC_set_filedes(SC_iodes *fd, int ifd, char *name, int fl)
   {char *nm;

    if (name != NULL)
       {nm = STRSAVE(name);

	fd[ifd].file = nm;
	fd[ifd].flag = fl;
	fd[ifd].dev  = IO_DEV_FILE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIN_FILEDES - release the contents of the SC_iodes' FD */

void _SC_fin_filedes(SC_iodes *fd)
   {int i, n;

    if (fd[2].file != fd[1].file)
       n = 3;
    else
       n = 2;

    for (i = 0; i < n; i++)
        {CFREE(fd[i].file);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_COPY_FILEDES - copy a set of SC_N_IO_CH SC_iodes FA
 *                  - into another FB
 */

void _SC_copy_filedes(SC_iodes *fb, SC_iodes *fa)
   {int i;

    for (i = 0; i < SC_N_IO_CH; i++)
        fb[i] = fa[i];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN - start up a process and open a pipe to it for further
 *         - communications
 *         - there are the following variants for the "name" of the process
 *         - to be opened:
 *         -
 *         -  1) <name>              - process "name" to be forked on
 *         -                         - the current CPU/host
 *         -  2) <host>:<name>       - process "name" to be exec'd
 *         -                         - on remote "host"
 *         -  3) <CPU>@<name>        - process "name" to be forked
 *         -                         - on processor number "CPU" of
 *         -                         - the current, parallel machine
 *         -  4) <host>:<CPU>@<name> - process "name" to be forked on
 *         -                         - processor number "CPU" of the
 *         -                         - remote, parallel "host"
 *         - the legal modes are:
 *         -  r    - read only (child stdout only connected to parent)
 *         -  w    - write only (child stdin only connected to parent)
 *         -  a    - read/write (child stdin and stdout connnect to parent)
 *         -
 *         - after rwa can come (in any order)
 *         -  b    - binary data exchanged
 *         -  o    - set OMP_NUM_THREADS to 1 in the parent
 *         -  p    - communicate via pipe
 *         -  s    - communicate via socket
 *         -  t    - communicate via pty (save original terminal state)
 *         -  T    - communicate via pty (do not save original terminal state)
 *         -  v    - process variable argument list of key, value pairs
 *         -         ending in LAST
 *         - only one of "pst" can be used and only for current CPU/host
 *         - processes
 *         - NOTE: for ftp or telnet the "t" option is useful
 *         - return NULL and set SC_gs.errn on failure
 */

PROCESS *SC_open(char **argv, char **envp, char *mode, ...)
   {int ok, ifd, iex, retry;
    char name[MAXLINE];
    char *key, *host, *proc, *s, *nm;
    SC_iodes fd[SC_N_IO_CH];
    PROCESS *pp;
    PFProcInit initf;
    PFProcExit exitf;
    void *exita;

    SC_gs.errn = 0;

    if ((argv == NULL) || (argv[0] == NULL))
       return(NULL);

    ok = SC_ERR_TRAP();
    if (ok != 0)
       {SC_gs.errn = ok;
	pp         = NULL;}

    else
       {initf = NULL;
	exitf = NULL;
	exita = NULL;
	retry = -1;
	iex   = -1;

	_SC_init_filedes(fd);

/* get any optional arguments */
	SC_VA_START(mode);

	if (strchr(mode, 'v') != NULL)
	   {while (TRUE)
	       {key = SC_VA_ARG(char *);
		if ((key == (char *) LAST) || (key == NULL))
		   break;
		else if (strcmp(key, "INIT") == 0)
		   initf = SC_VA_ARG(PFProcInit);
		else if (strcmp(key, "OPEN-RETRY-TIME") == 0)
		   retry = SC_VA_ARG(int);
		else if (strcmp(key, "RING-EXP") == 0)
		   iex = SC_VA_ARG(int);
		else if (strcmp(key, "EXIT") == 0)
		   {exitf = SC_VA_ARG(PFProcExit);
		    exita = SC_VA_ARG(void *);}
		else if (strcmp(key, "IO-DES") == 0)
		   {int ifd, nfd;
		    SC_iodes *pfd;

		    pfd = SC_VA_ARG(SC_iodes *);
		    nfd = SC_VA_ARG(int);
		    for (ifd = 0; ifd < nfd; ifd++)
		        fd[ifd] = pfd[ifd];}
		else if (strcmp(key, "STDIN") == 0)
		   {nm  = SC_VA_ARG(char *);
		    ifd = SC_VA_ARG(int);
		    _SC_set_filedes(fd, 0, nm, ifd);}
		else if (strcmp(key, "STDOUT") == 0)
		   {nm  = SC_VA_ARG(char *);
		    ifd = SC_VA_ARG(int);
		    _SC_set_filedes(fd, 1, nm, ifd);}
		else if (strcmp(key, "STDERR") == 0)
		   {nm  = SC_VA_ARG(char *);
		    ifd = SC_VA_ARG(int);
		    _SC_set_filedes(fd, 2, nm, ifd);}
		else
		   SC_error(-1, "Unknown option: %s", key);};};

	SC_VA_END;

/* copy the name to see where we are going */
	SC_strncpy(name, MAXLINE, argv[0], -1);

	if (SC_OTHER_HOSTP(name))
	   {host = SC_strtok(name, ":", s);
	    proc = SC_strtok(NULL, "\n", s);
	    pp   = SC_open_remote(host, proc, argv+1, envp, mode, initf);}

	else
	   pp = _SC_open_proc(-1, name, argv, envp, mode, fd, retry, iex,
			      initf, exitf, exita);};

    SC_ERR_UNTRAP();

    return(pp);}

/*--------------------------------------------------------------------------*/

/*                         CLIENT/SERVER ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_INIT_SERVER - perform steps to initialize a server
 *                - for SC_GET_PORT return the port for the client connection
 *                - for SC_GET_CONNECTION return the socket for the client
 *                - connection
 */

int SC_init_server(int step, int closep)
   {int rv;

#ifdef HAVE_PROCESS_CONTROL

    int ok;
    
    if (_SC_ps.debug)
       {fprintf(_SC_ps.diag, "   SC_init_server: %d\n", step);
        fflush(_SC_ps.diag);};

    rv = -1;
    ok = SC_ERR_TRAP();
    if (ok != 0)
       {if (closep)
	   {if (_SC.sfd >= 0)
	       close(_SC.sfd);
	    _SC.sfd = -1;};

	rv = _SC.sfd;}

    else
       {switch (step)
	   {case SC_GET_PORT :
	         _SC.sfd = socket(PF_INET, SOCK_STREAM, 0);
		 if (_SC.sfd < 0)
		    SC_error(-1, "COULDN'T OPEN SOCKET - SC_INIT_SERVER");

		 if (_SC_ps.debug)
		    {fprintf(_SC_ps.diag, "      Socket opened: %d\n", _SC.sfd);
		     fflush(_SC_ps.diag);};

		 _SC.srvr = _SC_tcp_bind(_SC.sfd, -1);
		 if (_SC.srvr == NULL)
		    SC_error(-1, "BIND FAILED - SC_INIT_SERVER");
		 else
		    rv = ntohs(_SC.srvr->sin_port);

		 break;

	    case SC_GET_CONNECTION :
	         _SC.nfd = _SC_tcp_accept_connection(_SC.sfd, _SC.srvr);
		 if (_SC.nfd < 0)
		    SC_error(-1, "ACCEPT FAILED - SC_INIT_SERVER");

		 if (closep)
		    {close(_SC.sfd);

		     if (_SC_ps.debug)
		        {fprintf(_SC_ps.diag, "      Socket closed: %d\n", _SC.sfd);
			 fflush(_SC_ps.diag);};

		     _SC.sfd = -1;
		     CFREE(_SC.srvr);};

		 rv = _SC.nfd;

		 break;};};

    SC_ERR_UNTRAP();

#else
    rv = 0;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_CLIENT - initialize a client
 *                - return the socket which connects to the server
 */

int SC_init_client(char *host, int port)
   {int fd;

#ifdef HAVE_PROCESS_CONTROL
    fd = _SC_tcp_connect(host, port, -1, FALSE);
#else
    fd = 0;
#endif

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_NUMBER_PROCESSORS - return the number of processors in the 
 *                          - current simulation
 */

int SC_get_number_processors(void)
   {int n;

    n = SC_gs.comm_size;
    n = max(n, 1);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_PROCESSOR_NUMBER - return the index of the current processor */

int SC_get_processor_number(void)
   {int n;

    n = SC_gs.comm_rank;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_PROCESSOR_INFO - set the SCORE rank and size if application
 *                       - initialized the parallel environment
 */

void SC_set_processor_info(int size, int rank)
   {

    SC_gs.comm_size = size;
    SC_gs.comm_rank = rank;

    return;}

/*--------------------------------------------------------------------------*/

/* generic state activities */

#define STATE_FREE_TASKS      300
#define STATE_FREE_LOG        301
#define STATE_PRINTF          302
#define HANDLE_ECHO           303
#define HANDLE_SLEEP          304
#define HANDLE_CD             305
#define MANAGE_LAUNCHED_JOB   306
#define COMMAND_EXEC          307
#define RUN_NEXT_TASK         308
#define TASK_DONE             309
#define START_JOB             310
#define FIN_JOB               311
#define CLOSE_JOB             312
#define REPORT_EXIT           313
#define SERVER_PRINTF         314

#define BUILD_STRING(_s, _t)                                                 \
   {strcat(_s, _t);                                                          \
    SC_LAST_CHAR(_s) = '\0';                                                 \
    strcat(_s, " ");}

static parstate
 *_SC_run_task_state = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRSUBTASK - print the subtask T */

void dprsubtask(subtask *t)
   {int i;

    if (t != NULL)
       {printf("Task: %s\n", t->command);
        printf("   Tokens: %d\n", t->nt);
	printf("   Argv:\n");
	for (i = 0; t->argf[i] != NULL; i++)
	    printf("      (%d) %s\n", i, t->argf[i]);

	if (t->shell != NULL)
	   printf("   Shell: %s\n", t->shell);
	if (t->ios != NULL)
	   printf("   Redirections: %s\n", t->ios);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_RUN_TASK_STATE - set the _SC_run_task_state to STATE */

void _SC_set_run_task_state(parstate *state)
   {

    _SC_run_task_state = state;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHG_DIR - change to directory DIR
 *             - then copy the current directory name into JOB
 *             - if DIR is NULL and the JOB has a directory use it
 *             - return 0 iff successful
 */

int _SC_chg_dir(char *dir, char **pndir)
   {int st, dot;
    char *ndr, *cwd;

    ndr = (pndir == NULL) ? NULL : *pndir;
    st  = 0;

/* determine whether dir state or implies staying in the current directory */
    dot = ((dir == NULL) || (strcmp(dir, ".") == 0));
    if ((dot == TRUE) && (ndr != NULL))
       {dir = ndr;
	dot = ((dir == NULL) || (strcmp(dir, ".") == 0));};

    if (dot == FALSE)
       st = chdir(dir);

    if ((st == 0) && (pndir != NULL))
       {cwd = SC_getcwd();
	if (cwd != NULL)
	   {if ((ndr == NULL) || (strcmp(cwd, ndr) != 0))
	       {CFREE(ndr);
		*pndir = STRSAVE(cwd);};
	    CFREE(cwd);};};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_SHELL - given a string SHELL return an explicit shell */

char *SC_get_shell(char *shell)
   {

    if (shell == NULL)
       shell = getenv("SHELL");

    if (shell == NULL)
       shell = DEFAULT_SHELL;

    return(shell);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STATE_LOG - log messages for STATE
 *               - this can be used on a client or server
 */

static int _SC_state_log(parstate *state, char *fmt, ...)
   {int rv, ifl;
    char chst[MAXLINE];
    char *msg;

    rv = FALSE;

    ifl = (strcmp(fmt, "server> init(%d)") == 0);
    if (ifl == TRUE)
       {gethostname(chst, MAXLINE);

	msg = SC_dsnprintf(TRUE, "Host: %s\n", chst);
	SC_array_string_add(state->log, msg);

	msg = SC_dsnprintf(TRUE, "Date: %s\n", SC_datef());
	SC_array_string_add(state->log, msg);

	msg = SC_dsnprintf(TRUE,
			   "---------------------------------------------------\n");
	SC_array_string_add(state->log, msg);};

    SC_VDSNPRINTF(TRUE, msg, fmt);

    SC_array_string_add(state->log, msg);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_READ_FILTER - parse the filter file FNAME */

fspec *_SC_read_filter(char *fname)
   {int n;
    char s[MAXLINE];
    char *tok, *txt, *p;
    FILE *fp;
    fspec *filter;

    if (fname == NULL)
       fp = NULL;
    else
       fp = fopen(fname, "r");

    if (fp != NULL)
       {filter = CMAKE_N(fspec, 1000);

	n  = 0;
	while (SC_fgets(s, MAXLINE, fp) != NULL)
	   {tok = SC_strtok(s, " \t\n\r", p);
	    txt = SC_strtok(NULL, "\n\r", p);

	    if ((tok != NULL) && (txt != NULL) && (*tok != '#'))
	       {filter[n].itok = SC_stoi(tok);

/* whack leading white space off of txt */
		for (; strchr(" \t", *txt) != NULL; txt++);

		strcpy(filter[n].text, txt);
		n++;};};

	filter[n].text[0] = '\0';
	filter[n].itok    = -1;
	n++;

	fclose(fp);}

    else
       filter = NULL;

    return(filter);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_FILTER - free the space FILTER */

void _SC_free_filter(fspec *filter)
   {

    if (filter != NULL)
       {CFREE(filter);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MATCH - return TRUE iff T matches a specification in FILTER */

static int _SC_match(char *t, fspec *filter)
   {int i, j, id, ok;
    char *f, *ps, *tok, *s, *p;

    ok = FALSE;
    if (filter != NULL)
       {for (i = 0; !ok; i++)
	    {id = filter[i].itok;
	     f  = filter[i].text;
	     if (f[0] == '\0')
	        break;

	     else if (id == 0)
	        ok = (strstr(t, f) != NULL);

	     else
	        {s  = STRSAVE(t);
		 ps = s;

		 tok = NULL;
		 for (j = 1; j <= id; j++)
		     {tok = SC_strtok(ps, " \t", p);
		      ps  = NULL;};

		 if (tok != NULL)
		    ok = (strstr(tok, f) != NULL);

		 CFREE(s);};};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_TASKLST - free up the space for a task list */

static void _SC_free_tasklst(tasklst *tl)
   {int it, nt;
    subtask *sub;

    if (tl->tasks != NULL)
       {nt = tl->nt;
	for (it = 0; it < nt; it++)
	    {sub = tl->tasks + it;

	     CFREE(sub->shell);
	     CFREE(sub->command);
	     CFREE(sub->ios);

	     SC_free_strings(sub->argf);};

	CFREE(tl->tasks);};

    CFREE(tl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_TASKDESC - free a taskdesc instance JOB */

static int _SC_free_taskdesc(void *a)
   {jobinfo *inf;
    taskdesc *job;

    if (a != NULL)
       {job = *(taskdesc **) a;
	if (job != NULL)
	   {inf = &job->inf;

	    SC_free_array(inf->out, SC_array_free_n);

	    _SC_free_tasklst(job->command);

	    CFREE(inf->full);
	    CFREE(inf->directory);
	    CFREE(inf->shell);
	    CFREE(job->host);
	    CFREE(job);

	    *(taskdesc **) a = NULL;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_FREE_TASKS - free the task of the given STATE */

static void _SC_exec_free_tasks(parstate *state)
   {

    SC_START_ACTIVITY(state, STATE_FREE_TASKS);

    SC_free_array(state->tasks, _SC_free_taskdesc);
    state->tasks = NULL;

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_FREE_LOGS - free the logs of the given STATE */

static void _SC_exec_free_logs(parstate *state)
   {

    SC_START_ACTIVITY(state, STATE_FREE_LOG);

    SC_free_array(state->log, SC_array_free_n);
    state->log = NULL;

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_SETUP_STATE - setup a parstate instance for a member of
 *                      - the SC_exec family of functions 
 */

void _SC_exec_setup_state(parstate *state, char *shell, char **env,
			  int na, int show, int tag, int srv,
			  int ign, fspec *flt, SC_evlpdes *pe,
			  PFFileCallback acc, PFFileCallback rej,
			  int (*finish)(taskdesc *job, char *msg))
   {int dtw, to;
    char *s;

    s = getenv("SC_EXEC_SERVER_WAIT");
    if (s == NULL)
       dtw = 300;
    else
       dtw = SC_stoi(s);

    s = getenv("SC_EXEC_OPEN_RETRY");
    if (s == NULL)
       to = 100;
    else
       to = SC_stoi(s);

    state->shell        = shell;
    state->env          = env;
    state->na           = na;
    state->heartbeat_dt = DEFAULT_HEARTBEAT;
    state->wait_ref     = 0;
    state->n_running    = 0;
    state->n_complete   = 0;
    state->doing        = 0;
    state->show         = show;
    state->tagio        = tag;
    state->server       = srv;
    state->ignore       = ign;
    state->done         = FALSE;
    state->filter       = flt;
    state->loop         = pe;

    state->wait_dt      = dtw;
    state->open_retry   = to;

    state->directory    = NULL;
    _SC_chg_dir(".", &state->directory);

    state->tasks = CMAKE_ARRAY(taskdesc *, NULL, 0);
    state->log   = SC_STRING_ARRAY();

    state->acc        = acc;
    state->rej        = rej;
    state->finish     = finish;
    state->print      = _SC_state_log;
    state->free_logs  = _SC_exec_free_logs;
    state->free_tasks = _SC_exec_free_tasks;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_FREE_STATE - cleanup a parstate */

void _SC_exec_free_state(parstate *state, int flt)
   {

    _SC_chg_dir(state->directory, NULL);
    CFREE(state->directory);

    if (flt == TRUE)
       _SC_free_filter(state->filter);

    SC_free_event_loop(state->loop);

    state->free_logs(state);
    state->free_tasks(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_PRINTF - print to both the STATE log
 *                   - and if STATE is a server state to the parent
 *                   - and AS log if it exists
 *                   - this version is for situations when there is no JOB
 */

int _SC_server_printf(asyncstate *as, parstate *state, char *tag,
		      char *fmt, ...)
   {int rv;
    char *msg;

    SC_START_ACTIVITY(state, SERVER_PRINTF);

    rv = TRUE;

    SC_VDSNPRINTF(TRUE, msg, fmt);

    state->print(state, "%s %s", tag, msg);

    _SC_exec_printf(as, "%s %s %s", _SC_EXEC_SRV_ID, tag, msg);

    CFREE(msg);

    SC_END_ACTIVITY(state);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DSTATELOG - help with generic debug logging */

void dstatelog(char *fmt, ...)
   {char tag[MAXLINE];
    char *tm, *tms, *msg;
    parstate *state;

    SC_VDSNPRINTF(TRUE, msg, fmt);

    state = _SC_run_task_state;
    if (state != NULL)
       {tm  = SC_datef();
	tms = tm + 11;

	snprintf(tag, MAXLINE, "[Job  -/- %s]", tms);

	CFREE(tm);

	state->print(state, "%s (%d) %s", tag, state->doing, msg);};

    CFREE(msg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_FILTERED - print each entry in MSG that is not rejects by
 *                    - a match with a specification in FILTER
 *                    - label each line with JID iff not equal -1
 */

void _SC_print_filtered(asyncstate *as, char **msg,
			fspec *filter, int jid, char *host)
   {int i, nm;
    char fmt[MAXLINE];
    char *t;

    SC_ptr_arr_len(nm, msg);

    for (i = 0; i < nm; i++)
        {t = msg[i];
	 if (!_SC_match(t, filter))
	    {if (host != NULL)
	        {if (as->nchar_max > 0)
		    snprintf(fmt, MAXLINE, "%%-%ds| %%s", as->nchar_max);
		 else
		    snprintf(fmt, MAXLINE, "%%s| %%s");
		 _SC_exec_printf(as, fmt, host, t);}

	     else if (jid == -1)
	        _SC_exec_printf(as, "%s", t);

	     else
	        _SC_exec_printf(as, "[Job %2d out]: %s", jid, t);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SHOW_COMMAND - decide whether to show commands
 *                  - if so print the relevant part of S using AS
 */

char *_SC_show_command(asyncstate *as, char *s, int show)
   {int c;

    c = s[0];
    if ((show == 2) || ((show == 1) && (c != '@')))
       _SC_exec_printf(as, "%s\n", s);

    if (c == '@')
       s++;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PUT_COMMAND - decide whether to show commands
 *                 - if so put the relevant part of S into OUT
 */

char *_SC_put_command(SC_array *out, char *s, int show)
   {int c;
    char *bf;

    c = s[0];
    if ((show == 2) || ((show == 1) && (c != '@')))
       {bf = SC_dsnprintf(TRUE, "%s\n", s);
	SC_array_string_add(out, bf);};

    if (c == '@')
       s++;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DECIDE_RETRY - decide whether or not a retry is neccessary
 *                  - if a retry will be done this routine sleeps
 *                  - until it is time to retry
 *                  - return
 *                  -    0  if successful
 *                  -    1  if failed and retry
 *                  -   -1  if failed finally
 */

int _SC_decide_retry(asyncstate *as, jobinfo *inf, tasklst *tl, int st)
   {int rv, ts, warn;
    int ia, na, it;
    char *cmd, *time, *stmsg, *cmmsg;
    subtask *sub;

    ia = inf->ia;
    na = inf->na;

    warn = FALSE;

    if (st != 0)
       {time = SC_datef();

	rv = -1;
	if (ia < na)
	   {warn = TRUE;
	    rv   = 1;
	    if (ia < 2)
	       ts = SC_random_int(100, 1000);
	    else
	       ts = SC_random_int(30000, 40000);

	    stmsg = SC_dsnprintf(TRUE,
				 "***> failed (%d) [%s] - attempt %d in %.2f seconds\n",
				 st, time, ia + 1, ts/1000.0);

	    SC_sleep(ts);}

        else if (na > 1)
	   {warn  = TRUE;
	    stmsg = SC_dsnprintf(TRUE,
				 "***> failed (%d) [%s] - quitting after %d attempts\n",
				 st, time, na);};

	CFREE(time);}

    else
       {if ((ia > 1) && (na > 1))
	   {warn  = TRUE;
	    time  = SC_datef();
	    stmsg = SC_dsnprintf(TRUE,
				 "***> succeeded [%s] - on attempt %d\n",
				 time, ia);

	    CFREE(time);};

	rv = 0;};

    if (warn == TRUE)
       {SC_array_string_add(inf->out, stmsg);

	cmd   = inf->full;
	cmmsg = SC_dsnprintf(TRUE, "***>       %s\n", cmd);
	SC_array_string_add(inf->out, cmmsg);

	if ((st != 0) && (tl != NULL))
	   {it  = _SC_get_command(tl, 1);
	    sub = tl->tasks + it;
	    cmd = sub->command;

	    if ((ia < na) && (na > 1))
	       {cmmsg = SC_dsnprintf(TRUE,
				     "***> retry '%s' - task %d\n", cmd, it+1);
		SC_array_string_add(inf->out, cmmsg);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           TASK PARSING                                   */

/*--------------------------------------------------------------------------*/

/* _SC_PREP_COMMAND - prepare the command IN
 *                  - transform ( ... ) to ' ... ' and place it in OUT
 */

static char *_SC_prep_command(char *in, int check)
   {int nc;
    char *out;

    out = NULL;
    if (in != NULL)
       {SC_trim_right(in, " ");

/* transform a command of the form ( ... ) to ' ... '
 * this will save a process being created by the shell SH
 */
	nc = strlen(in) - 1;
	nc = max(nc, 0);
	if ((in[0] == '(') && (in[nc] == ')'))
	   {out = STRSAVE(in+1);
	    SC_LAST_CHAR(out) = '\0';}

/* copy in the command if not of the form ( ... ) */
	else
	   out = STRSAVE(in);};

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SUBST_TASK_ENV - substitute recognized environment variables values */

static void _SC_subst_task_env(int na, char **ta)
   {int i, c, nb, nc;
    char *r, *t, *ps, *pe, *val;
    char var[MAXLINE];

    for (i = 0; i < na; i++)
        {t = ta[i];
	 c = *t;
	 if (strchr("\'({[", c) != NULL)
	    continue;

	 r = NULL;

	 while (TRUE)
	    {ps = strchr(t, '$');
	     if (ps == NULL)
	        break;
	     else
	        {for (pe = ps+1; TRUE; pe++)
		     {c = *pe;
		      if (c == '{')
                         continue;
		      else if (!SC_isalnum(c) && (c != '_') && (c != '}'))
			 break;};
		 nb = ps - t;
		 ps++;
		 nc = pe - ps;

/* handle ${VAR} forms */
		 if (*ps == '{')
		    SC_strncpy(var, MAXLINE, ps+1, nc-2);

/* handle $VAR forms */
		 else
		    SC_strncpy(var, MAXLINE, ps, nc);

		 val = getenv(var);
		 if (val == NULL)
		    break;
		 else
		    {r = SC_dstrcat(r, t);
		     r[nb] = '\0';
		     r = SC_dstrcat(r, val);
		     r = SC_dstrcat(r, pe);

		     CFREE(t);
		     t = r;};};};

	 ta[i] = t;};

    return;}

/*--------------------------------------------------------------------------*/

/*                              TASK EXECUTION                              */

/*--------------------------------------------------------------------------*/

/* _SC_INCR_TASK_COUNT - increment the task count of JOB
 *                     - usually called after a task has been
 *                     - launched
 */

static void _SC_incr_task_count(taskdesc *job, int ni, int done)
   {jobinfo *inf;
    tasklst *tl;

    if (job != NULL)
       {inf = &job->inf;
	tl  = job->command;
	if (ni < 0)
	   ni = tl->nt;

	tl->nl += ni;

	inf->ia++;

	if (done == TRUE)
	   {tl->nc++;
	    inf->ia = 0;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_ECHO - handle an echo command internally
 *                 - return TRUE iff successful
 */

static int _SC_handle_echo(taskdesc *job, asyncstate *as, subtask *sub)
   {int c, n, m, sc, nc, na, newl, rv;
    char *s, *t, **ta;
    jobinfo *inf;
    SC_iodes *fd;
    parstate *state;

    rv = TRUE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, HANDLE_ECHO);

	inf = &job->inf;
	na  = sub->nt;
	ta  = sub->argf;

	m    = 1;
	newl = TRUE;
	if ((m < na) && (strcmp(ta[m], "-n") == 0))
	   {newl = FALSE;
	    m++;};

/* using T as a temporary concatenate the strings in TA to S */
	s = NULL;
	t = NULL;
	for (n = m; n < na; n++)
	    {t = SC_dstrcat(t, ta[n]);

	     c  = t[0];
	     sc = 0;
	     nc = strlen(t);
	     switch (c)
	        {case '\"' :
		 case '\'' :
		      sc++;
		      nc--;

		 default:
		      if (0 <= nc)
			 {t[nc++] = ' ';
			  t[nc++] = '\0';};
		      s = SC_dstrcat(s, t+sc);
		      break;};

	     t[0] = '\0';};

	CFREE(t);

/* handle the dispostion of S */
	if (n != -1)
	   {if (s == NULL)
	       s = STRSAVE("");
	    else
	       SC_LAST_CHAR(s) = '\0';

	    fd = sub->fd;
	    if ((fd[1].file == NULL) && (fd[2].file == NULL))
	       {job->print(job, as, "echo '%s'\n", s);

		if (newl == TRUE)
		   s = SC_dstrcat(s, "\n");

		SC_array_string_add(inf->out, s);}

	    else
	       {job->print(job, as, "%s\n", sub->command);
		job->redir(job, as, fd, s, newl);
		CFREE(s);};

	    inf->status = 0;
	    inf->signal = SC_EXITED;};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_SLEEP - handle a command of the form
 *                  -     sleep <n>
 *                  - internally
 *                  - return TRUE iff successful
 */

static int _SC_handle_sleep(taskdesc *job, asyncstate *as, subtask *sub)
   {int ns, rv;
    double ds;
    char *t, **ta;
    jobinfo *inf;
    parstate *state;

    rv = TRUE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, HANDLE_SLEEP);

	inf = &job->inf;
	ta  = sub->argf;

	t = ta[1];
	if (t != NULL)
	   {ds = SC_stof(t);
	    ns = 1000*ds;
	    if (ds > 0.0)
	       ns = max(ns, 1);
	    else
	       ns = 0;

	    job->print(job, as, "sleep %d msec\n", ns);

	    SC_sleep(ns);

	    job->print(job, as, "awake after %d msec\n", ns);

	    inf->status = 0;
	    inf->signal = SC_EXITED;};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_CD - handle a command of the form
 *               -     cd <path>
 *               - internally
 *               - return TRUE iff successful
 */

static int _SC_handle_cd(taskdesc *job, asyncstate *as, subtask *sub)
   {int st, rv, ok;
    char *dir, **ta;
    jobinfo *inf;
    tasklst *tl;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, HANDLE_CD);

	inf = &job->inf;
	tl  = job->command;
	ta  = sub->argf;

	dir = ta[1];
	if (dir != NULL)
	   {job->print(job, as, "cdir '%s'\n", dir);

	    for (ok = TRUE; ok; )
	        {inf->ia++;

/* in order to handle retries properly advance the launch count
 * temporarily to refer to the correct job
 * each retry backs off the count to do the task again
 */
		 tl->nl++;

		 st = _SC_chg_dir(dir, &inf->directory);
		 if (st == 0)
		    {job->print(job, as,
				"new  '%s' (%d)\n", inf->directory, st);
		     rv = TRUE;
		     ok = FALSE;}

		 else
		    ok = job->retryp(job, sub, st, 0, TRUE);};

/* restore the launch count so that 
 * _SC_incr_task_count can do its job properly
 */
	    tl->nl--;

	    inf->status = st;
	    inf->signal = SC_EXITED;};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_COMMAND - get the command for the current task - OFF */

int _SC_get_command(tasklst *tl, int off)
   {int it, ntm;

    ntm = tl->nt - 1;
    it  = tl->nl - off;

    it = min(it, ntm);
    it = max(it, 0);

    return(it);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_LOST - return TRUE if the process has been idle too long */

static int _SC_process_lost(PROCESS *pp)
   {int idle;
    double t, ifh, ifs, ift;
    SC_process_rusedes *pru;
    static double ft = -1.0, fit = -1.0, fis = -1.0, fih = -1.0;

    idle = FALSE;

    if (pp != NULL)
       {pru = pp->pru;
	if (pru != NULL)
	   {t   = pru->wct - pru->wcr;
	    t   = max(t, SMALL);
	    ift = pru->idlet / t;
	    ifs = pru->since / t;
	    ifh = pru->since / ((double) DEFAULT_HEARTBEAT);

	    idle = TRUE;

/* do not worry until the job has run more than a 60 seconds */
	    if (ft < 0.0)
	       {ft = SC_stof(getenv("SC_EXEC_LOST_TIME"));
		if (ft == 0.0)
		   ft = 60.0;};
	    idle &= (t > ft);

/* do not kill off shell scripts which are mostly idle
 * it must have been idle more than 90% of its lifetime to be considered lost
 */
	    if (fit < 0.0)
	       {fit = SC_stof(getenv("SC_EXEC_LOST_IDLE"));
		if (fit == 0.0)
		   fit = 0.9;};
	    idle &= (ift < fit);

/* kill off process that has been idle for the last 50% of its life */
	    if (fis < 0.0)
	       {fis = SC_stof(getenv("SC_EXEC_LOST_SINCE"));
		if (fis == 0.0)
		   fis = 0.5;};
	    idle &= (ifs > fis);

/* do not kill off servers unless they have lost at least 5 heartbeats */
	    if (fih < 0.0)
	       {fih = SC_stof(getenv("SC_EXEC_LOST_BEATS"));
		if (fih == 0.0)
		   fih = 5.0;};
	    idle &= (ifh > fih);};};

/* log lost processes which can be extremely hard to find without this info */
    if (idle == TRUE)
       _SC_exec_printf(NULL,
		       "***> idle process %d  sec(%.2e) idle(%.0f%%) since(%.0f%%) beats(%.2f)\n",
		       pp->id, t, 100.0*ift, 100.0*ifs, ifh);

    return(idle);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MANAGE_LAUNCHED_JOB - enter the newly opened PROCESS PP into
 *                         - the taskdesc JOB
 *                         - return TRUE iff succesfully entered
 */

static int _SC_manage_launched_job(taskdesc *job, asyncstate *as, PROCESS *pp)
   {int idp, ifd, it, pi, rv;
    char *cmd;
    tasklst *tl;
    subtask *sub;
    jobinfo *inf;
    SC_evlpdes *pe;
    SC_iodes *fd;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, MANAGE_LAUNCHED_JOB);

	pe  = state->loop;
	inf = &job->inf;

	tl  = job->command;
	it  = _SC_get_command(tl, 0);
	sub = tl->tasks + it;

	if (SC_process_alive(pp))
	   {cmd = sub->command;
	    idp = pp->id;

	    job->print(job, as,
		       "exec '%s' (pid %d)\n",
		       cmd, idp);

	    if (SC_status(pp) == SC_RUNNING)
	       {SC_unblock(pp);
		SC_set_attr(pp, SC_LINE, TRUE);

		pi = SC_register_event_loop_callback(pe, SC_PROCESS_I, pp,
						     state->acc, state->rej,
						     -1);
		SC_ASSERT(pi >= 0);};

	    job->pp = pp;

	    state->n_running++;

	    rv = TRUE;}

	else
	   {job->pp = NULL;

	    fd  = sub->fd;
	    ifd = _SC_redir_fail(fd);
	    switch (ifd)
	       {case 0 :
		     job->print(job, as,
				"no such file %s\n",
				fd[ifd].file);
		     break;
		case 1 :
		case 2 :
		     job->print(job, as,
				"%s already exists\n",
				fd[ifd].file);
		     break;

		default :
		     job->print(job, as, "exec failed\n");
		     break;};};

/* GOTCHA: what if process is already finished? */
	inf->tstart   = SC_wall_clock_time();
	inf->status   = NOT_FINISHED;
	inf->signal   = NOT_FINISHED;
	job->finished = FALSE;

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CMND_EXEC - execute a command for _SC_handle_cmnd
 *               - return TRUE if successfully launched
 *               - otherwise return FALSE
 */

static int _SC_cmnd_exec(taskdesc *job, asyncstate *as, subtask *sub)
   {int st, to;
    char **ca, *icmnd[5];
    jobinfo *inf;
    PROCESS *pp;
    SC_iodes *fd;
    parstate *state;

    st = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, COMMAND_EXEC);

	inf = &job->inf;
	fd  = sub->fd;

	if (sub->need == FALSE)
	   ca = sub->argf;

	else
	   {icmnd[0] = sub->shell;
	    icmnd[1] = "-c";
	    icmnd[2] = sub->command;
	    icmnd[3] = NULL;

	    ca = icmnd;

	    job->print(job, as, "shell %s\n", sub->shell);};
    
	to = state->open_retry;

	st = _SC_chg_dir(NULL, &inf->directory);

	pp = SC_open(ca, state->env, "apvo",
#if 0
		     "EXIT", _SC_next_task, state,
#endif
                     "IO-DES", fd, SC_N_IO_CH,
#if 0
		     "STDIN",  fd[0].file, fd[0].flag,
		     "STDOUT", fd[1].file, fd[1].flag,
		     "STDERR", fd[2].file, fd[2].flag,
#endif
		     "OPEN-RETRY-TIME", to,
		     "RING-EXP", 18,
		     NULL);

	if (pp == NULL)
	   job->print(job, as, "exec failed %d\n", SC_gs.errn);
	else
	   pp->lost = _SC_process_lost;

	st = _SC_manage_launched_job(job, as, pp);

	SC_END_ACTIVITY(state);};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_COMMAND - do a command that is guaranteed to be simple
 *                - return the job status this will be
 *                -    NOT_FINISHED if the command was launched
 *                -    0            if the command completes and succeeds
 *                -    n            if the command failed
 *                - launched may or may not mean completed
 */

static int _SC_do_command(taskdesc *job, asyncstate *as, int it)
   {int rv, st, done, fl;
    char *t, **ta;
    tasklst *tl;
    subtask *sub;

    st = TRUE;

    if (job != NULL)
       {tl  = job->command;
	sub = tl->tasks + it;
	ta  = sub->argf;

/* initialize the zero read count for each new task */
	job->nzip = 0;

	t  = ta[0];
	fl = ((sub->need == FALSE) && (sub->pipe == FALSE));

/* handle echo */
	if ((strcmp(t, "echo") == 0) && (fl == TRUE))
	   done = _SC_handle_echo(job, as, sub);

/* handle sleep */
	else if ((strcmp(t, "sleep") == 0) && (fl == TRUE))
	   done = _SC_handle_sleep(job, as, sub);

/* handle cd */
	else if ((strcmp(t, "cd") == 0) && (fl == TRUE))
	   done = _SC_handle_cd(job, as, sub);

/* anything else is a command that must be exec'd one way or the other */
	else
	   {rv = _SC_cmnd_exec(job, as, sub);
	    SC_ASSERT(rv == TRUE);
	    done = FALSE;};

	_SC_incr_task_count(job, 1, done);

	st = job->inf.status;};

    return(st);}

/*--------------------------------------------------------------------------*/

/*                         TASKLIST MANAGEMENT                              */

/*--------------------------------------------------------------------------*/

/* _SC_MAKE_TASKLST - initialize a task list from a command string CMD */

static tasklst *_SC_make_tasklst(char *shell, char *cmd)
   {int n, na, nt;
    char **ta, *s;
    subtask *sub;
    tasklst *tl;

    tl = CMAKE(tasklst);

/* convert the command from ( ... ) form to just ... form */
    s = _SC_prep_command(cmd, FALSE);

/* tokenize the command to facilitate looking for simple commands */
    ta = SC_tokenize_literal(s, " \t\n\r", TRUE, 3);
    SC_ptr_arr_len(na, ta);

    _SC_subst_task_env(na, ta);

    CFREE(s);

/* count the apparent subtasks
 * constructs such as (cd .. ; ) would look like 2 subtasks
 */
    for (nt = 1, n = 0; n < na; n++)
        nt += (strcmp(ta[n], ";\n") == 0);

    sub = CMAKE_N(subtask, nt);

/* initialize and get the real number of subtasks
 * constructs such as (cd .. ; ) would actually have 1 subtask
 */
    nt = _SC_init_subtasks(sub, shell, ta, na);

    SC_free_strings(ta);

    tl->nl     = 0;
    tl->nc     = 0;
    tl->nt     = nt;
    tl->status = 0;
    tl->tasks  = sub;

    return(tl);}

/*--------------------------------------------------------------------------*/

/*                                 JOB MANAGEMENT                           */

/*--------------------------------------------------------------------------*/

/* _SC_RUN_NEXT_TASK - execute the next task on the task list of JOB
 *                   - if called from SIGCHLD handler SIGP is TRUE
 *                   - return FALSE if there are no more jobs to launch
 *                   - otherwise return TRUE
 */

static int _SC_run_next_task(taskdesc *job, int sigp)
   {int rtry, st, rv;
    tasklst *tl;
    asyncstate *as;
    parstate *state;

    state = job->context;

    SC_START_ACTIVITY(state, RUN_NEXT_TASK);

    _SC_set_run_task_state(state);

    as = NULL;
    tl = job->command;
    st = 0;

    job->print(job, as,
	       "next task fin(%d) cmp(%d) lch(%d) n(%d) st(%d)\n",
	       job->finished, tl->nc, tl->nl, tl->nt, tl->status);

    if (job->finished == FALSE)
       {while (tl->nl < tl->nt)
	   {rtry = job->close(job, TRUE);
	    if (job->finished == TRUE)
	       break;

	    st = _SC_do_command(job, as, tl->nl);
	    if (st != 0)
	       break;};}

    rv = !job->finished;

    if ((tl->nl >= tl->nt) && (st != NOT_FINISHED))
       {if (state->finish != NULL)
	   rtry = state->finish(job, _SC_EXEC_COMPLETE);
	else
	   rtry = job->close(job, TRUE);

	if (rtry == TRUE)
	   {rv            = TRUE;
	    job->finished = FALSE;
	    job->exec(job, sigp);}
	else
	   rv = FALSE;};

    SC_END_ACTIVITY(state);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TASK_RETRY_P - look at task status and decide whether to retry
 *                  - return TRUE iff retry is to be done
 */

static int _SC_task_retry_p(taskdesc *job, subtask *sub,
			    int sts, int sgn, int setst)
   {int it, ok, rv;
    char *cmd;
    tasklst *tl;
    jobinfo *inf;
    parstate *state;
    SC_process_rusedes *pru;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;
	inf   = &job->inf;
	tl    = job->command;
	it    = _SC_get_command(tl, 1);
	cmd   = sub->command;

	ok = _SC_decide_retry(NULL, inf, tl, sts);

/* if retrying - report and backup the tasklist to make the
 * failed one be the next one launched in job->exec
 */
	if (ok == 1)
	   {if (inf->na > 1)
	       {job->print(job, NULL, "retry '%s' attempt %d\n", cmd, it);
		tl->nl = it;};

	    rv = TRUE;}

/* if done wrap up for this task */
	else
	   {job->print(job, NULL, "done (%d/%d) '%s'\n", sts, sgn, cmd);

	    if (job->pp != NULL)
	       {pru = job->pp->pru;
		job->print(job, NULL,
			   "used wc(%.2f) usr(%.2f) sys(%.2f) idle(%.2f) tail(%.2f) mem(%.2f)\n",
			   pru->wct, pru->usrt, pru->syst,
			   pru->idlet, pru->since, pru->mem);};

	    if ((sts != 0) && (state->ignore == FALSE))
	       {tl->nl        = tl->nt;
		tl->nc        = tl->nt;
		job->finished = TRUE;}
	    else
	       tl->nc++;

	    inf->ia = 0;

	    if (setst == TRUE)
	       {inf->status = sts;
		inf->signal = sgn;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TASK_DONE - process a task that is finished
 *               - all tasks that are successfully launched
 *               - should come through here after they exit
 *               - return TRUE iff retry is to be done
 */

static int _SC_task_done(taskdesc *job, int setst)
   {int it, rv, sts, sgn;
    PROCESS *pp;
    tasklst *tl;
    subtask *sub;
    PFFileCallback acc;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, TASK_DONE);

	tl = job->command;

/* if the current task is already done then the
 * number launched, nl, will be equal to number complete, nc
 */
	if (tl->nl > tl->nc)
	   {pp  = job->pp;

	    it  = _SC_get_command(tl, 1);
	    sub = tl->tasks + it;

	    sts = pp->reason;
	    sgn = pp->status;

	    acc = state->acc;
	    if (acc != NULL)
	       (*acc)(pp->io[0], 0, state);

	    if (job->nzip > 3)
	       job->print(job, NULL,
			  "%d empty reads during execution\n", job->nzip);

	    rv = job->retryp(job, sub, sts, sgn, setst);};

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_REDIR - write the string S to the appropriate
 *                 - file descriptors of FD
 */

static void _SC_print_redir(taskdesc *job, asyncstate *as,
			    SC_iodes *fd, char *s, int newl)
   {int ffe, ffo, df, nc;
    char *fne, *fno;
    mode_t p;

    if (s != NULL)
       {nc = strlen(s);

	fno = fd[1].file;
	ffo = fd[1].flag;
	fne = fd[2].file;
	ffe = fd[2].flag;

	p = SC_get_perm(FALSE);

/* if the files have the same name only write stdout */
	if ((fno != NULL) && (fne != NULL))
	   {if (strcmp(fno, fne) == 0)
	       {ffe = 0;
		fne = NULL;};};

/* write stdout */
	if (fno != NULL)
	   {df = open(fno, ffo, p);
	    if (df != -1)
	       {SC_write_sigsafe(df, s, nc);
		if (newl == TRUE)
		   SC_write_sigsafe(df, "\n", 1);
		close(df);}
	    else
	       job->print(job, as,
			  "error opening file '%s' mode 0%o - %d\n",
			  fno, ffo, df);};

/* write stderr */
	if (fne != NULL)
	   {df = open(fne, ffe, p);
	    if (df != -1)
	       {SC_write_sigsafe(df, s, nc);
		if (newl == TRUE)
		   SC_write_sigsafe(df, "\n", 1);
		close(df);}
	    else
	       job->print(job, as,
			  "error opening file '%s' mode 0o%o - %d\n",
			  fne, ffe, df);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STATE_CHECK_PROCESS - examine the status of
 *                         - the current process in JOB
 */

static void _SC_state_check_process(taskdesc *job, asyncstate *as,
				    int *pnr, int *pnc)
   {int id, more, sgn, sts;
    int nr, nc;
    PROCESS *pp;
    jobinfo *inf;
    parstate *state;

    if (job != NULL)
       {state = job->context;
	pp    = job->pp;
	inf   = &job->inf;
	id    = inf->id;
	sgn   = inf->signal;
	sts   = inf->status;

	nr = *pnr;
	nc = *pnc;

	if (pp != NULL)
	   pp->lost = _SC_process_lost;

	if (SC_process_status(pp) != SC_RUNNING)
	   {if ((sgn != NOT_FINISHED) && ((sgn & SC_EXITED) == 0))
	       _SC_server_printf(as, state, _SC_EXEC_INFO,
				 "a task in job %d died abnormally %d/%d\n",
				 id, sts, sgn);

	    if ((sgn != NOT_FINISHED) && ((sgn & SC_LOST) == 0))
	       _SC_server_printf(as, state, _SC_EXEC_INFO,
				 "a task in job %d was lost %d/%d\n",
				 id, sts, sgn);

	    more = job->exec(job, FALSE);
	    if (more == FALSE)
	       nc++;
	    else
	       nr++;}

	else
	   nr++;

	*pnr = nr;
	*pnc = nc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LAUNCH_JOB - exec the JOB command
 *                - return TRUE iff successful
 *                - this is a server side routine
 */

static int _SC_launch_job(taskdesc *job, asyncstate *as)
   {int rv, more;
    char *cm, *dr, *hs, *cmc;
    jobinfo *inf;
    PROCESS *pp;

    rv = FALSE;

    if (job != NULL)
       {hs  = job->host;
	inf = &job->inf;

	dr = inf->directory;
	cm = inf->full;

	_SC_setup_output(inf, "_SC_LAUNCH_JOB");

/* launch job on remote host
 * NOTE: by doing SC_open_remote we do a SC_verify_host
 * this means two SSHes and the consequent extra time
 * on the other hand a simple _SC_ping_host has resulted in
 * a hang
 */
	if (hs != NULL)
	   {pp = SC_open_remote(hs, inf->full, NULL, NULL, "ap", NULL);
	    if (pp == NULL)
	       job->print(job, as,
			  "remote exec on %s failed %d\n",
			  hs, SC_gs.errn);
	    else
	       pp->lost = _SC_process_lost;

	    rv = _SC_manage_launched_job(job, as, pp);

	    _SC_incr_task_count(job, -1, FALSE);}

/* launch job on current host */
	else
	   {rv  = TRUE;
	    cmc = STRSAVE(cm);

	    _SC_chg_dir(dr, &inf->directory);

	    more = job->exec(job, FALSE);
	    if (more == FALSE)
	       {inf->status = 0;
		inf->signal = SC_EXITED;};

	    CFREE(cmc);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIN_JOB - when a JOB is finished
 *             - handle the termination, output, and retry logic
 *             - return TRUE iff the JOB is to be retried
 */

static int _SC_fin_job(taskdesc *job, asyncstate *as, int srv)
   {int pid, rtry;
    char *p, **out;
    jobinfo *inf;
    parstate *state;

    rtry = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, FIN_JOB);

	inf = &job->inf;
	if (inf == NULL)
	   job->print(job, as,
		      "***> no job info - _SC_FIN_JOB\n");

	pid = (state->tagio == TRUE) ? inf->id : -1;

	rtry = job->close(job, TRUE);
	if (rtry == FALSE)
	   {if (srv == FALSE)
	       {p = _SC_show_command(as, inf->full, state->show);
		SC_ASSERT(p != NULL);};

/* print job output thru filter */
	    if (SC_array_get_n(inf->out) == 0L)
	       job->print(job, as,
			  "***> no output for job - _SC_FIN_JOB\n");

	    else
	       {out = _SC_array_string_join(&inf->out);

		_SC_print_filtered(as, out, job->filter, pid, job->host);

		SC_free_strings(out);

		inf->out = CMAKE_ARRAY(char *, NULL, 0);};

	    state->n_complete++;};

	SC_END_ACTIVITY(state);};

    return(rtry);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_START_JOB - execute command CM using SHELL
 *               - upto NA times before reporting failure
 *               - output is passed through FILTER before printing
 *               - SHOW controls command echoing
 *               - this is a server side routine
 */

static int _SC_start_job(taskdesc *job, asyncstate *as, int launch)
   {int ok;
    jobinfo *inf;
    parstate *state;

    ok = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, START_JOB);

	inf = &job->inf;

	if (launch == TRUE)
	   {ok = job->launch(job, as);
	    if (ok == FALSE)
	       CFREE(job);}
	else
	   {_SC_setup_output(inf, "_SC_START_JOB");
	    ok = TRUE;};

	SC_END_ACTIVITY(state);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CLOSE_JOB_PROCESS - close the current process of the JOB properly */

static int _SC_close_job_process(taskdesc *job, int setst)
   {int rtry, ok;
    char *p;
    PROCESS *pp;
    jobinfo *inf;
    parstate *state;

    rtry = FALSE;

    if (job != NULL)
       {state = job->context;

	if (state != NULL)
	   {SC_START_ACTIVITY(state, CLOSE_JOB);

	    inf = &job->inf;

	    p = SC_datef();
	    if (p != NULL)
	       {strcpy(inf->stop_time, p);
		CFREE(p);}
	    else
	       strcpy(inf->stop_time, "0");

	    pp = job->pp;
	    if (SC_process_alive(pp))
	       {if (pp->stop_time != NULL)
		   strcpy(inf->stop_time, pp->stop_time);

		rtry = job->done(job, setst);

		if (state != NULL)
		   SC_remove_event_loop_callback(state->loop, SC_PROCESS_I, pp);

		ok = SC_close(pp);
		SC_ASSERT(ok == TRUE);};

	    job->pp = NULL;

	    SC_END_ACTIVITY(state);};};

    return(rtry);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_JOB_TAG - return the tag with JOB id, task, and time */

static void _SC_job_tag(taskdesc *job, char *tag, int nc, char *tm)
   {int id, ic, il, it, rl;
    char *tms;

    rl = FALSE;
    if (tm == NULL)
       {tm = SC_datef();
	rl = TRUE;};

    tms = tm + 11;

    if (job != NULL)
       {id = job->inf.id;
	il = job->command->nl;
	ic = job->command->nc;
	it = min(il, ic);

	snprintf(tag, nc, "[Job %2d/%d %s]", id, it, tms);}
    else
       snprintf(tag, nc, "[Job --/- %s]", tms);

    if (rl)
       CFREE(tm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_JOB_PRINT - print to both the STATE log
 *               - and if STATE is a server state to the parent
 *               - and AS log if it exists
 */

int _SC_job_print(taskdesc *job, asyncstate *as, char *fmt, ...)
   {int rv;
    char tag[MAXLINE];
    char *msg;
    parstate *state;

    rv = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, STATE_PRINTF);

	rv = TRUE;

	SC_VDSNPRINTF(TRUE, msg, fmt);

	job->tag(job, tag, MAXLINE, NULL);

	state->print(state, "%s %s", tag, msg);

	if (state->server == TRUE)
	   _SC_exec_printf(as, "%s %s %s", _SC_EXEC_SRV_ID, tag, msg);

	CFREE(msg);

	SC_END_ACTIVITY(state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REPORT_JOB_EXIT - log the fact that the process of JOB has exited */

static void _SC_report_job_exit(taskdesc *job, char *where)
   {int it, idp;
    char *cmd, *hnd;
    PROCESS *pp;
    tasklst *tl;
    subtask *sub;
    SC_contextdes sc;
    parstate *state;

    state = job->context;

    SC_START_ACTIVITY(state, REPORT_EXIT);

    tl  = job->command;
    pp  = job->pp;

    it  = _SC_get_command(tl, 1);
    sub = tl->tasks + it;

    cmd = sub->command;
    idp = pp->id;

    sc = SC_which_signal_handler(SIGCHLD);
    if (sc.f == SIG_IGN)
       hnd = "off";
    else
       hnd = "on";

    job->print(job, NULL,
	       "term %s %s '%s' (pid %d)\n",
	       where, hnd, cmd, idp);

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ADD_JOB - add a JOB to an array of jobs TA */

static void _SC_add_job(taskdesc *job)
   {int i, n;
    taskdesc *tsk;
    SC_array *ta;

    ta = job->context->tasks;
    n  = SC_array_get_n(ta);
    for (i = 0; i < n; i++)
        {tsk = *(taskdesc **) SC_array_get(ta, i);
	 if (tsk == NULL)
	    {SC_array_set(ta, i, &job);
	     break;};};

    if (i >= n)
       SC_array_push(ta, &job);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REMOVE_JOB - remove a JOB from an array of jobs TA
 *                - return TRUE if JOB was freed
 */

static int _SC_remove_job(taskdesc *job)
   {int i, n, ok;
    taskdesc *tsk;
    SC_array *ta;

    ok = FALSE;
    ta = job->context->tasks;
    n  = SC_array_get_n(ta);
    for (i = 0; i < n; i++)
        {tsk = *(taskdesc **) SC_array_get(ta, i);
	 if (tsk == job)
	    {SC_array_set(ta, i, NULL);
	     CFREE(job);
	     SC_array_set(ta, i, &job);
	     ok = TRUE;
	     break;};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_TASKDESC - allocate and initialize a taskdesc */

taskdesc *SC_make_taskdesc(parstate *state, int jid,
			   char *host, char *shell, char *dir, char *cmd)
   {taskdesc *job;
    jobinfo *inf;

    job = CMAKE(taskdesc);
    if (job != NULL)
       {inf = &job->inf;

	shell = SC_get_shell(shell);

	if ((dir == NULL) || (strcmp(dir, ".") == 0))
	   dir = state->directory;

	if (dir != NULL)
	   dir = STRSAVE(dir);

	inf->id        = jid;
	inf->ia        = 0;
	inf->na        = state->na;
	inf->shell     = STRSAVE(shell);
	inf->directory = dir;
	inf->full      = STRSAVE(cmd);
	inf->tstart    = 0.0;

	job->host     = STRSAVE(host);
	job->arch     = NULL;
	job->command  = _SC_make_tasklst(shell, cmd);
	job->finished = FALSE;
	job->pp       = NULL;
	job->filter   = state->filter;
	job->context  = state;
	job->nzip     = 0;

	job->start    = _SC_start_job;
	job->launch   = _SC_launch_job;
	job->done     = _SC_task_done;
	job->retryp   = _SC_task_retry_p;
	job->check    = _SC_state_check_process;
	job->finish   = _SC_fin_job;
	job->close    = _SC_close_job_process;
	job->redir    = _SC_print_redir;
	job->tag      = _SC_job_tag;
	job->print    = _SC_job_print;
	job->report   = _SC_report_job_exit;
	job->exec     = _SC_run_next_task;

	job->add      = _SC_add_job;
	job->remove   = _SC_remove_job;};

    return(job);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _KIND_IO - return the character matching io_kind K */

int _kind_io(io_kind k)
   {int rv;

    switch (k)
       {case IO_STD_IN :
	     rv = 'i';
	     break;
        case IO_STD_OUT :
	     rv = 'o';
	     break;
        case IO_STD_ERR :
	     rv = 'e';
	     break;
        case IO_STD_BOND :
	     rv = 'b';
	     break;
        default :
	     rv = '\0';
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRGRP - diagnostic print of process_group tasks */

void dprgrp(process_group *pg)
   {int i, j, n, c, gid;
    char s[MAXLINE];
    char **sa;
    io_device dev;
    process *pps, *ppd;
    iodes *fds;

    n = pg->np;

    sa = MAKE_N(char *, n+1);
    sa[0] = '\0';

    for (i = 0; i < n; i++)
        {pps = pg->parents[i];

	 if (pps->ios == NULL)
	    snprintf(s, MAXLINE, "%3d: end\n     ", i);
	 else
	    {snprintf(s, MAXLINE, "%3d:", i);
	     for (j = 0; pps->ios[j] != NULL; j++)
	         vstrcat(s, MAXLINE, " %s ", pps->ios[j]);
	     vstrcat(s, MAXLINE, "\n     ");};

/* stdin */
	 fds = pps->io + IO_STD_IN;
	 dev = fds->dev;
         gid = fds->gid;
	 if (gid == -1)
	    vstrcat(s, MAXLINE, "i(ttyin) ", fds->gid);
	 else
	    {ppd = pg->parents[gid];

	     c = _kind_io(IO_STD_OUT);
	     for (j = 0; j < N_IO_CH; j++)
	         {if (ppd->io[j].gid == i)
		     c = _kind_io(j);};

	     vstrcat(s, MAXLINE, "i(%d)%c    ", fds->gid, c);};

/* stdout */
	 fds = pps->io + IO_STD_OUT;
	 dev = fds->dev;
         gid = fds->gid;
	 if (gid == -1)
	    {switch (dev)
	        {case IO_DEV_PIPE :
		 case IO_DEV_NONE :
		 case IO_DEV_TERM :
		      vstrcat(s, MAXLINE, "o(ttyout) ");
		      break;
		 case IO_DEV_SOCKET :
		      break;
		 case IO_DEV_PTY :
		      break;
		 case IO_DEV_FILE :
		      vstrcat(s, MAXLINE, "o(file)   ");
		      break;
		 case IO_DEV_VAR :
		      vstrcat(s, MAXLINE, "o(var)    ");
		      break;
		 case IO_DEV_EXPR :
		      vstrcat(s, MAXLINE, "o(expr)   ");
		      break;};}

	 else
	    {c = _kind_io(IO_STD_IN);
	     vstrcat(s, MAXLINE, "o(%d)%c     ", fds->gid, c);};

/* stderr */
	 fds = pps->io + IO_STD_ERR;
	 dev = fds->dev;
         gid = fds->gid;
	 if (gid == -1)
	    {switch (dev)
	        {case IO_DEV_PIPE :
		 case IO_DEV_NONE :
		 case IO_DEV_TERM :
		      vstrcat(s, MAXLINE, "o(ttyerr) ");
		      break;
		 case IO_DEV_SOCKET :
		      break;
		 case IO_DEV_PTY :
		      break;
		 case IO_DEV_FILE :
		      vstrcat(s, MAXLINE, "o(file)   ");
		      break;
		 case IO_DEV_VAR :
		      vstrcat(s, MAXLINE, "o(var)    ");
		      break;
		 case IO_DEV_EXPR :
		      vstrcat(s, MAXLINE, "o(expr)   ");
		      break;};}

	 else
	    {c = _kind_io(IO_STD_IN);
	     vstrcat(s, MAXLINE, "e(%d)%c     ", fds->gid, c);};

	 vstrcat(s, MAXLINE, " %s", pps->cmd);
         sa[i] = STRSAVE(s);};

    sa[n] = NULL;

    for (i = 0; i < n; i++)
        printf("%s\n", sa[i]);

    free_strings(sa);

    return;}

/*--------------------------------------------------------------------------*/

/*                               GROUP ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* SET_IODES - parse IOS and set the members of IOS accordingly */

int set_iodes(iodes *pio, char *ios)
   {int ck, cd, fid, gid, nc, rel, pos, rv;
    io_mode mode;
    io_device dev;
    io_kind knd;

    rv = FALSE;

    dev  = IO_DEV_NONE;
    knd  = IO_STD_NONE;
    mode = IO_MODE_NONE;

    gid = pio->gid;
    fid = pio->fid;

    if (ios[0] == PROCESS_DELIM)
       ios++;

    nc = 0;
    ck = ios[nc++];
    cd = ios[nc++];

    if (ios != NULL)

/* get >&, &>, >>&, and &>> */
       {if ((strstr(ios, ">&") != NULL) ||
	    (strstr(ios, "&>") != NULL))
	   {knd = IO_STD_BOND;
	    dev = IO_DEV_FILE;}

        else if (strncmp(ios, "1>", 2) == 0)
	   {knd = IO_STD_OUT;
	    dev = IO_DEV_FILE;}

	else if (strncmp(ios, "2>", 2) == 0)
	   {knd = IO_STD_ERR;
	    dev = IO_DEV_FILE;}
#if 1
	else if ((strstr(ios, "|&") != NULL) ||
		 (strstr(ios, "&|") != NULL))
	   {knd = IO_STD_BOND;
	    dev = IO_DEV_PIPE;}

	else if (strchr(ios, '|') != NULL)
	   {knd = IO_STD_OUT;
	    dev = IO_DEV_PIPE;}
#endif
	else
	   {switch (ck)
	       {case 'e' :
		     knd = IO_STD_ERR;
		     break;
		case '<' :
		     cd = ck;
	        case 'i' :
		     knd = IO_STD_IN;
		     break;
		case '>' :
		     cd = ck;
	        case 'o' :
		     knd = IO_STD_OUT;
		     break;};};

/* next for the device type */
        rel = FALSE;
	switch (cd)
	   {case '+' :
	    case '-' :
	         dev = IO_DEV_PTY;
	         dev = IO_DEV_SOCKET;
	         dev = IO_DEV_PIPE;
		 rel = TRUE;
		 break;
	    case 'e' :
	    case '&' :
	         dev = IO_DEV_EXPR;
		 break;
	    case 'f' :
	    case 'w' :
	    case '!' :
	    case 'a' :
	    case '>' :
	         dev = IO_DEV_FILE;
		 break;
	    case 'v' :
	         dev = IO_DEV_VAR;
		 break;
	    case 't' :
	         dev = IO_DEV_TERM;
		 break;};

	if (dev == IO_DEV_PIPE)
	   {pos = atoi(ios+nc);
	    if (rel == TRUE)
	       gid += pos;
	    else
	       gid = pos;};

/* now for the mode */
	if (strstr(ios, "<") != NULL)
	   mode = IO_MODE_RO;
	else if (strstr(ios, ">>") != NULL)
	   mode = IO_MODE_APPEND;
	else if (strchr(ios, '!') != NULL)
	   mode = IO_MODE_WD;
	else if (strstr(ios, ">") != NULL)
	   mode = IO_MODE_WO;
	else if (strchr("ioeb", ios[0]) != NULL)
	   {switch (cd)
	       {case 'f' :
                     mode = IO_MODE_WO;
                     break;
		case 'w' :
                     mode = IO_MODE_WD;
                     break;
		case 'a' :
                     mode = IO_MODE_APPEND;
		     break;};};

	rv = TRUE;};

    pio->knd  = knd;
    pio->fid  = fid;
    pio->gid  = gid;
    pio->dev  = dev;
    pio->mode = mode;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDIR_IO - set the parts of FD implied by REDIR
 *          - NAME specifies the file to be used
 */

void redir_io(iodes *fd, int nfd, int ifd, iodes *pio)
   {int ofd, excl, fl;
    io_kind knd;
    io_mode md;
    char *nm, *name;

    name = pio->file;
    md   = pio->mode;
    knd  = pio->knd;

    if (name != NULL)
       {if (strcspn(name, "+-0123456789") == 0)
	   {ofd = atol(name);
	    if (ofd >= nfd)
	       return;
	    nm  = fd[ofd].file;
	    fl  = fd[ofd].flag;}
	else
	   {ofd = ifd;
	    nm  = name;
	    fl  = -1;};

/* shell redirection syntax and file mode correspondence */
	switch (md)

/*  <   ->  O_RDONLY */
	   {case IO_MODE_RO :
	         fl = O_RDONLY;
		 break;

/*  >!  ->  O_WRONLY | O_CREAT | O_TRUNC */
	    case IO_MODE_WD :
	         fl = O_WRONLY | O_CREAT | O_TRUNC;
		 break;

/*  >>  ->  O_WRONLY | O_APPEND */
	    case IO_MODE_APPEND :
		 fl = O_WRONLY | O_CREAT | O_APPEND;
		 break;

/*  >   ->  O_WRONLY | O_CREAT | O_EXCL */
	    default :

/* do not try to use O_EXCL bit with devices - think about it */
	         if ((nm != NULL) && (strncmp(nm, "/dev/", 5) == 0))
		    excl = 0;
		 else
		    excl = O_EXCL;

		 fl = O_WRONLY | O_CREAT | excl;
		 break;};

	switch (knd)
	   {case IO_STD_IN :
		 fd[ifd].flag = fl;
	         break;
	    case IO_STD_OUT :
		 fd[ifd].flag = fl;
	         break;
	    case IO_STD_ERR :
		 fd[2].flag = fl;
	         break;
	    case IO_STD_BOND :
		 fd[2].file = nm;
		 fd[1].flag = fl;
#ifdef NEWWAY
		 fd[2].flag = -1;
#else
		 fd[2].flag = fl;
#endif
	         break;
	    default :
	         break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_REDIRECT - parse tokens until
 *                    - a redirection of the form <src><oper><dst>
 *                    - is obtained
 *                    -
 *                    - output redirections (covers sh and csh families)
 *                    -   [src]>[&] dst     src to dst, create name, error if it exists
 *                    -   [src]>![&] dst    src to dst, create name, truncate if it exists
 *                    -   [src]>>[&] dst    src to dst, append
 *                    -
 *                    -   dst := <filename> | <fd>
 *                    -   src := <fd> | &
 *                    -   fd  := [digits]+  (defaults to 1, stdout, if absent)
 */

static int parse_redirect(iodes *pio, process *pp, int i)
   {int fid, gid;
    char file[MAXLINE];
    char *t, *fn, **ta;

    ta = pp->ios;

    if (pio != NULL)
       {t = ta[i];
	
	gid = pp->ip;
	fid = -1;
	fn  = NULL;

/* determine the name of the file for redirection */
	if (strcmp(t, "2>&1") == 0)
	   fid = 1;

/* determine the name of the file for redirection */
	else if (strpbrk(t, "<>") != NULL)
	   {t = ta[++i];
	    if (t != NULL)
	       {nstrncpy(file, MAXLINE, t, -1);
		fn = file;};}

	else if (t[0] == PROCESS_DELIM)
	   fn = t + 3;

	pio->gid = gid;
	pio->fid = fid;
	if (fn != NULL)
	   pio->file = STRSAVE(fn);

/* parse out the specification - results in PIO */
	set_iodes(pio, t);};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDIRECT_FD - determine the I/O redirections in PP->ARG
 *             - and fill in PP->IO from them
 */

static int redirect_fd(process *pp, int i)
   {int rv;
    iodes io;

    memset(&io, 0, sizeof(io));

/* parse out the redirect related specifications */
    rv = parse_redirect(&io, pp, i);

/* add the redirect specifications to the filedes */
    switch (io.knd)
       {case IO_STD_IN :
	     pp->io[0] = io;
	     redir_io(pp->io, N_IO_CH, 0, &io);
	     break;
        case IO_STD_OUT :
	     pp->io[1] = io;
	     redir_io(pp->io, N_IO_CH, 1, &io);
	     break;
        case IO_STD_ERR :
	     pp->io[2] = io;
	     redir_io(pp->io, N_IO_CH, 2, &io);
	     break;
        case IO_STD_BOND :
	     pp->io[1] = io;
#ifdef NEWWAY

/* since 2>&1 syntax does not specify a file name this is split up */
	     if ((io.file != NULL) && (io.fid != -1))
	        redir_io(pp->io, N_IO_CH, 1, &io);
	     else
	        {io.file = pp->io[1].name;
		 io.raw  = STRSAVE(p);};
#else
	     redir_io(pp->io, N_IO_CH, 1, &io);
#endif
	     pp->io[2] = io;
	     redir_io(pp->io, N_IO_CH, 2, &io);
	     break;
        default :
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDIRECT_COPY - copy the redirections from PP to CP */

static void redirect_copy(process *cp, process *pp)
   {int i, fd;

    for (i = 0; i < N_IO_CH; i++)
        {fd = cp->io[i].fd;
	 cp->io[i]    = pp->io[i];
	 cp->io[i].fd = fd;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDIRECT_PROCESS - look through the I/O specifications of PP */

static void redirect_process(process *pp)
   {int i, n;
    char **ta;

    ta = pp->ios;
    n  = lst_length(ta);

/* find self-specifications in each process */
    for (i = 0; i < n; i++)
        i = redirect_fd(pp, i);

    _default_iodes(pp->io);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNQUOTE_PROCESS - look through the tokens of the process PP
 *                 - and remove enclosing quote marks
 *                 - unless this an 'echo' command
 *                 - this mimics a shell having done its processing
 */

static void unquote_process(process *pp)
   {int i, n;
    char **ta, *t, *p;

    ta = pp->arg;
    n  = lst_length(ta);

    if ((ta != NULL) && (strcmp(ta[0], "echo") != 0))
       {for (i = 0; i < n; i++)
	    {t = ta[i];
	     if (strchr("'\"", *t) != NULL)
	        {p = STRSAVE(t+1);
		 LAST_CHAR(p) = '\0';
		 FREE(t);
		 ta[i] = p;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILLIN_PGRP - to this point each process in the group has its own
 *             - specifications determined
 *             - now fill in the implied connections of the other processes
 */

void fillin_pgrp(process_group *pg)
   {int i, n, gid;
    process **pa, *pp;
    iodes *pio, *dio;

    n  = pg->np;
    pa = pg->parents;

    for (i = 0; i < n; i++)
        {pp = pa[i];

/* stdin */
         pio = pp->io + 0;
	 switch (pio->dev)
            {case IO_DEV_PIPE :
	          break;
	     default :
	          break;};

/* stdout */
         pio = pp->io + 1;
	 switch (pio->dev)
            {case IO_DEV_PIPE :
	          gid = pio->gid;
	          dio = pa[gid]->io + IO_STD_IN;
		  dio->gid = i;
		  dio->dev = IO_DEV_PIPE;
	          break;
	     default :
	          break;};

/* stderr */
         pio = pp->io + 2;
	 switch (pio->dev)
            {case IO_DEV_PIPE :
	          gid = pio->gid;
	          dio = pa[gid]->io + IO_STD_IN;
		  dio->gid = i;
		  dio->dev = IO_DEV_PIPE;
	          break;
	     default :
	          break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRANSFER_FD - transfer the KND descriptor of PN to CN */

static int transfer_fd(process *pn, io_kind pk, process *cn, io_kind ck)
   {int fd;

    fd            = pn->io[pk].fd;
    pn->io[pk].fd = -2;

    close(cn->io[ck].fd);
    cn->io[ck].fd = fd;

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RECONNECT_PGRP - reconnect the N proccess's in PG
 *                - from the canonical parent/child topology
 *                - into a process_group topology
 *                - NOTE: this logic is good for both pipes and
 *                - sockets but bad for PTYs
 *                - on the other hand why do PTYs
 *                - in a process_group
 */

static void reconnect_pgrp(process_group *pg)
   {int i, gie, gio, nm, n;
    int fdo[N_IO_CHANNELS];
    process *pp, *cp, *pt;
    process **pa, **ca;
    io_device dve, dvo;

    n  = pg->np;
    pa = pg->parents;
    ca = pg->children;
    pt = pg->terminal;
    nm = n - 1;

    fillin_pgrp(pg);

/* remove the parent to child channels except for the last one
 * the final parent out gets connected to the first child in
 */
    if (nm > 0)

       {

#if 0
/* set any sockets to read/write mode */
	for (i = 0; i < n; i++)
	    {pp = pa[i];
	     cp = ca[i];
	     if (cp->medium == USE_SOCKETS)
	        {SC_set_fd_attr(pp->io[0], O_RDWR,   -1);
		 SC_set_fd_attr(pp->io[1], O_RDWR,   -1);
		 SC_set_fd_attr(pp->io[2], O_WRONLY, -1);
		 SC_set_fd_attr(cp->io[0], O_RDWR,   -1);
		 SC_set_fd_attr(cp->io[1], O_RDWR,   -1);
		 SC_set_fd_attr(cp->io[2], O_WRONLY, -1);};};
#endif

/* get the output fds of the first member of the group */
	pp = pa[0];
        for (i = 0; i < 1; i++)
	    {fdo[i] = pp->io[i+1].fd;
	     pp->io[i+1].fd = -2;};

/* reconnect terminal process output to first process */
        for (i = 0; i < 1; i++)
	    {close(pt->io[i+1].fd);
	     pt->io[i+1].fd = fdo[i];};

/* close all other parent to child lines */
	for (i = 1; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

	     close(pp->io[1].fd);
	     close(cp->io[0].fd);

	     pp->io[1].fd = -2;
	     cp->io[0].fd = -2;};

/* connect all non-terminal child out to next child in */
	for (i = 0; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

	     gio = pp->io[IO_STD_OUT].gid;
	     dvo = pp->io[IO_STD_OUT].dev;
	     gie = pp->io[IO_STD_ERR].gid;
	     dve = pp->io[IO_STD_ERR].dev;

/* default to the next process in line */
	     if ((dvo == IO_DEV_PIPE) && (gio == -1))
	        gio = i + 1;
	     if ((dve == IO_DEV_PIPE) && (gie == -1))
	        gie = i + 1;

/* stdout */
	     if (gio >= 0)
	        transfer_fd(pa[i], IO_STD_IN, ca[gio], IO_STD_IN);

/* stderr */
	     if (gie >= 0)
	        transfer_fd(pa[i], IO_STD_ERR, ca[gie], IO_STD_IN);};};

dprgrp(pg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_PGRP - fill the ITth process in PG from the tokens TA */

static void setup_pgrp(process_group *pg, int it,
		       char **ta, int dosh, char **ios)
   {process *pp, *cp, **pa, **ca;

    pa = pg->parents;
    ca = pg->children;

    pg->to = _job_setup_proc(&pp, &cp, ta, pg->env, pg->shell);

    pp->ios = ios;
    pp->ip  = it;
    cp->ip  = it;

    pa[it] = pp;
    ca[it] = cp;

    if (dosh == FALSE)

/* process out I/O redirections if we don't need a shell to do the command */
       {redirect_process(pp);
	redirect_copy(cp, pp);

/* remove a layer of quotes if we don't need a shell to do the command */
	unquote_process(pp);
	unquote_process(cp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_PGRP - parse out specifications in S to initialize PG */

static void parse_pgrp(statement *s)
   {int i, j, it, nc, dosh, doif, term;
    char *t, *shell;
    char **sa, **ta, **env, **ios;
    process **pa, **ca;
    process_group *pg;

    it    = 0;
    doif  = FALSE;
    dosh  = FALSE;
    term  = FALSE;

    shell = s->shell;
    env   = s->env;

    ta = NULL;
    sa = tokenize(s->text, " \t\n\r\f");
    nc = lst_length(sa);

/* maximum number of process would be the number of tokens */
    pa = MAKE_N(process *, nc);
    ca = MAKE_N(process *, nc);

    pg = MAKE(process_group);
    if (pg != NULL)
       {pg->np       = 0;
	pg->to       = 0;
	pg->mode     = NULL;
	pg->shell    = shell;
	pg->env      = env;
	pg->terminal = NULL;
	pg->parents  = pa;
	pg->children = ca;};

    for (i = 0; i < nc; i++)
        {t   = sa[i];
	 ios = NULL;

	 if (t == NULL)
	    continue;

	 else if (strpbrk(t, "@|<>") != NULL)
	    {for (j = i; j < nc; j++)
	         {if (strpbrk(sa[j], "@|<>") != NULL)
		     {ios = lst_add(ios, sa[j]);
		      sa[j] = NULL;}
		  else
		     break;};
	     term  = TRUE;}

	 else if (strpbrk(t, "[]()@$*`") != NULL)
	    dosh = TRUE;

	 else if (strncmp(t, "if", 2) == 0)
	    {doif = TRUE;
	     dosh = TRUE;}

/* complete an entry at a terminal 'fi' */
	 else if ((strcmp(t, "fi\n") == 0) || (strcmp(t, "fi;\n") == 0))
	    {ta   = lst_add(ta, t);
	     doif = FALSE;
	     term = TRUE;}

/* complete an entry at a terminal 'endif' */
	 else if (strcmp(t, "endif\n") == 0)
	    {ta   = lst_add(ta, t);
	     doif = FALSE;
	     term = TRUE;}

/* complete an entry at a terminal ';' */
	 else if ((strcmp(t, ";\n") == 0) && (doif == FALSE))
	    term = TRUE;

	 if (term == TRUE)
	    {if (lst_length(ta) > 0)
	        {setup_pgrp(pg, it++, ta, dosh, ios);
		 ta = NULL;};
	     ios  = NULL;
	     term = FALSE;
	     dosh = FALSE;}

/* build up an entry from non-terminal tokens */
	 else
	    ta = lst_add(ta, t);};

    if ((i >= nc) && (lst_length(ta) > 0))
       setup_pgrp(pg, it++, ta, dosh, ios);

    pg->np       = it;
    pg->terminal = pa[it - 1];

    reconnect_pgrp(pg);
    printf("-> %s\n", s->text);

    s->np = it;
    s->pg = pg;

    return;}

/*--------------------------------------------------------------------------*/

/*                            RUN GROUP ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PGRP_ACCEPT - accept messages read from PP */

int _pgrp_accept(process *pp, char *s)
   {int rv;

    rv = fputs(s, stdout);
    rv = (rv >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_REJECT - reject messages read from PP */

int _pgrp_reject(process *pp, char *s)
   {int rv;

    rv = TRUE;
    printf("reject method\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_WAIT - call when wait says its done */

void _pgrp_wait(process *pp)
   {int rv;
    char *st;

    switch (pp->status)
       {case JOB_RUNNING :
	     st = "run";
	     break;
	case JOB_STOPPED :
	     st = "stop";
	     break;
	case JOB_CHANGED :
	     st = "chng";
	     break;
	case JOB_EXITED :
	     st = "exit";
	     break;
	case JOB_COREDUMPED :
	     st = "core";
	     break;
	case JOB_SIGNALED :
	     st = "sgnl";
	     break;
	case JOB_KILLED :
	     st = "kill";
	     break;
	case JOB_DEAD :
	     st = "dead";
	     break;
	default :
	     st = "unk";
	     break;};
/*
    printf("exited (%s/%d)\n", st, pp->reason);
*/
/* drain text from the job - apoll will no longer check it after this */
    rv = job_read(pp, pp->accept);
    ASSERT(rv == 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_TTY - check the TTY
 *           - return TRUE if we are done
 */

int _pgrp_tty(char *tag)
   {int rv;

/*    printf("tty method pid=%d  fd=%d\n", getpid(), fileno(stdin)); */
    rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_WORK - main worker handling I/O connections for
 *            - running process group
 */

void _pgrp_work(int i, char *tag, void *a, int nd, int np, int tc, int tf)
   {

/*     printf("work method pid=%d i=%d\n", getpid(), i); */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_FIN - record the exit status of PP in A */

void _pgrp_fin(process *pp, void *a)
   {int i;
    int *st;

    i  = pp->ip;
    st = (int *) a;
    st[i] = pp->reason;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_PGRP - run the process group PG */

static int run_pgrp(statement *s)
   {int i, np, rv, fd, tc;
    int *st;
    process *pp;
    process_group *pg;

    rv = -1;

    if (s != NULL)
       {pg = s->pg;
	np = s->np;

	st = MAKE_N(int, np);

	asetup(np, 1);

/* launch the jobs - io_data passed to macc, myrej, and mydone */
	for (i = 0; i < np; i++)
	    {pp = _job_fork(pg->parents[i], pg->children[i],
			    NULL, "rw", st);
	     pp->accept   = _pgrp_accept;
	     pp->reject   = _pgrp_reject;
	     pp->wait     = _pgrp_wait;
	     pp->nattempt = 1;
	     pp->ip       = i;

	     fd = pp->io[0].fd;
	     rv = block_fd(fd, FALSE);
	     ASSERT(rv == 0);

	     stck.proc[i]       = pp;
	     stck.fd[i].fd      = fd;
	     stck.fd[i].events  = stck.mask_acc;
	     stck.fd[i].revents = 0;

	     apoll(1);};

/* wait for the work to complete - _pgrp_work does the work */
	tc = await(300, "commands", _pgrp_tty, _pgrp_work, st);

/* close out the jobs */
	afin(_pgrp_fin);

	rv = 0;
	for (i = 0; i < np; i++)
	    rv |= st[i];

        FREE(st);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_PGRP - free the process group PG */

static void free_pgrp(process_group *pg, int np)
   {

    FREE(pg);

    return;}

/*--------------------------------------------------------------------------*/

/*                          STATEMENT ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* STATEMENTS_N - return the number of statement in SL */

static int statements_n(statement *sl)
   {int n;

    for (n = 0; sl[n].text != NULL; n++);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_STATEMENTS - free the statements SL */

static void free_statements(statement *sl)
   {int n;

    for (n = 0; sl[n].text != NULL; n++)
        {FREE(sl[n].text);
	 free_pgrp(sl[n].pg, sl[n].np);};

    FREE(sl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_STATEMENT - parse a string S into an array of statements
 *                 - delimited by: ';', '&&', '||'
 */

statement *parse_statement(char *s, char **env, char *shell)
   {int i, n, c;
    char *pt, *t, *ps;
    st_sep trm;
    statement *sa;

    sa = NULL;
    n  = strlen(s);
    t  = MAKE_N(char, n+100);
    if (t != NULL)
       {nstrncpy(t, n+100, s, -1);

	for (i = 0, pt = t; *pt != '\0'; )
	    {trm = ST_NEXT;
	     for (ps = pt, c = *ps++; c != '\0'; c = *ps++)
	         {if (c == '\\')
		     continue;
		  if (c == ';')
		     {ps[-1] = '\0';
		      trm    = ST_NEXT;
		      break;}

		  else if (c == '&')
		     {c = *ps++;
		      if (c == '&')
			 {ps[-2] = '\0';
			  trm    = ST_AND;
			  break;}
		      else if (c == '\0')
			 break;}

		  else if (c == '|')
		     {c = *ps++;
		      if (c == '|')
			 {ps[-2] = '\0';
			  trm    = ST_OR;
			  break;};};};

	     if (sa == NULL)
	        sa = MAKE_N(statement, 1000);

	     if (sa != NULL)
	        {sa[i].terminator = trm;
		 sa[i].text       = STRSAVE(trim(pt, BOTH, " \t\n\r\f"));
		 sa[i].shell      = STRSAVE(shell);
		 sa[i].env        = env;
		 i++;};

	     pt = ps;};

	FREE(t);};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AEXEC - execute a <statement> */

int aexec(int c, char **v, char **env)
   {int i, nc, rv, st;
    char *s, *shell;
    statement *sl;

    shell = "/bin/csh";

/* concatenate command line arguments into one big string */
    s = NULL;
    for (i = 0; i < c; i++)
        s = append_tok(s, ' ', "%s", v[i]);

/* parse command to list of statements - ;, &&, or || */
    sl = parse_statement(s, env, shell);
    nc = statements_n(sl);

/* parse each statement into process groups */
    for (i = 0; i < nc; i++)
        parse_pgrp(sl+i);

/* execute each process group */
    st = 0;
    rv = 1;
    for (i = 0; (i < nc) && (rv == 1); i++)
        {st = run_pgrp(sl+i);
	 switch (sl[i].terminator)
	    {case ST_AND :
	          rv = (st == 0);
	          break;
	     case ST_OR :
	          rv = (st != 0);
	          break;
	     default :
	          break;};};

    free_statements(sl);
    FREE(s);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
