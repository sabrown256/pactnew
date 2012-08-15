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
 *           - <pgr-spec>  := '@' <io-kind> |
 *           -                '@' <io-kind> <io-id> |
 *           -                '@' <io-kind> <io-id> <io-kind>
 *           - <io-kind>   := 'i' | 'o' | 'e' | 'b' |
 *           -                'r' | 'x' | 'l' | 'v'
 *           - <io-id>     := +<n> | -<n> | <n>
 *           - <n>         := unsigned integer value
 *           - 
 *           - Illegal cases:
 *           -   @i<n>i       input to input
 *           -   @o<n>o, @e<n>e, @o<n>e, or @e<n>o    output to output
 *           -   @o<n> @e<n>  where <n> is the same   use @b<n> to express that
 *
 * read http://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html#Implementing-a-Shell
 * for a good discussion of shell-like job control
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

typedef struct s_statement statement;
typedef struct s_process_session process_session;
typedef int (*PFPCAL)(char *db, io_mode m, FILE **fp,
		      char *name, int c, char **v);

struct s_statement
   {int np;                 /* number of processes in group */
    int nf;                 /* number of functions in group */
    int ne;                 /* number of entries in group */
    st_sep terminator;      /* action terminator of group */
    char *text;             /* text of group */
    char *shell;            /* shell which runs the individual processes */
    char **env;
    int *st;                /* exit status array */
    process_group *pg;
    PFPCAL (*map)(char *nm);};

struct s_process_session
   {pid_t pgid;                     /* OS process group id */
    int terminal;                   /* file descriptor of stdin */
    int interactive;                /* TRUE iff interactive session */
    int foreground;                 /* TRUE iff current job is foreground */
    struct termios attr;};          /* terminal attributes */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NAME_IO - return the name of io_kind K */

char *_name_io(io_kind k)
   {char *rv;

    switch (k)
       {case IO_STD_IN :
	     rv = "stdin";
	     break;
        case IO_STD_OUT :
	     rv = "stdout";
	     break;
        case IO_STD_ERR :
	     rv = "stderr";
	     break;
        case IO_STD_BOND :
	     rv = "bonded";
	     break;
        case IO_STD_STATUS :                       /* exit status - output */
	     rv = "status";
	     break;
        case IO_STD_RESOURCE :                  /* resource usage - output */
	     rv = "rusage";
	     break;
        case IO_STD_LIMIT :                     /* resource limits - input */
	     rv = "rlimit";
	     break;
        case IO_STD_ENV_VAR :             /* environment variables - input */
	     rv = "env";
	     break;
        default :
	     rv = "none";
	     break;};

    return(rv);}

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
        case IO_STD_STATUS :                        /* exit status - output */
	     rv = 'x';
	     break;
        case IO_STD_RESOURCE :                   /* resource usage - output */
	     rv = 'r';
	     break;
        case IO_STD_LIMIT :                      /* resource limits - input */
	     rv = 'l';
	     break;
        case IO_STD_ENV_VAR :              /* environment variables - input */
	     rv = 'v';
	     break;
        default :
	     rv = '\0';
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _IO_KIND - return the io_kind matching character C */

int _io_kind(int c)
   {io_kind rv;

    switch (c)
       {case 'b' :
	     rv = IO_STD_BOND;
	     break;
        case 'e' :
	     rv = IO_STD_ERR;
	     break;
        case 'i' :
	     rv = IO_STD_IN;
	     break;
        case 'o' :
	     rv = IO_STD_OUT;
	     break;
        case 'x' :                                  /* exit status - output */
	     rv = IO_STD_STATUS;
	     break;
        case 'r' :                               /* resource usage - output */
	     rv = IO_STD_RESOURCE;
	     break;
        case 'l' :                               /* resource limits - input */
	     rv = IO_STD_LIMIT;
	     break;
        case 'v' :                         /* environment variables - input */
	     rv = IO_STD_ENV_VAR;
	     break;
        default :
	     rv = IO_STD_NONE;
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _IO_MODE - return the io_mode matching character C */

int _io_mode(io_kind knd)
   {io_mode rv;
    static char *err_exist;

    err_exist = getenv("REDIRECT_EXIST_ERROR");

    switch (knd)
       {case IO_STD_IN :
	     rv = IO_MODE_RO;
	     break;
        case IO_STD_OUT :
        case IO_STD_ERR :
        case IO_STD_BOND :
	     rv = (err_exist != NULL) ? IO_MODE_WO : IO_MODE_WD;
	     break;
        default :
	     rv = IO_MODE_NONE;
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRIOC - print NC io_connectors from IOC */

void dprioc(char *tag, int np, io_connector *ioc)
   {int i, fd, io, gid, nc;
    char *hnd, *knd, *dev;
    io_connector *pioc;
    static char *hn[]  = {"none", "clos", "pipe", "fnc", "poll"};
    static char *kn[]  = {"none", "in", "out", "err", "bond",
			  "status", "rsrc", "limit", "env"};
    static char *dn[]  = {"none", "pipe", "sock", "pty", "term", "fnc"};

    nc = N_IO_CHANNELS*np;

    _dbg(-1, "-----------------------------------------------");
    _dbg(-1, "%s  ioc", tag);
    _dbg(-1, "%d processes   %d connections", np, nc);

    _dbg(-1, "Unit       fd gid  hnd  knd  dev #");
    for (i = 0; i < nc; i++)
        {pioc = ioc + i;

	 io = i % N_IO_CHANNELS;
	 if (io == 0)
	    _dbg(-1, "");

	 fd  = pioc->in.fd;
	 gid = pioc->in.gid;
	 hnd = hn[pioc->in.hnd];
	 knd = kn[pioc->in.knd + 1];
	 dev = dn[pioc->in.dev];

	 if (pioc->in.dev == IO_DEV_PIPE)
	    _dbg(-1, "%2d  read  %3d %3d %4s %4s %4s %d",
		 io, fd, gid, hnd, knd, dev, gid);
	 else
	    _dbg(-1, "%2d  read  %3d %3d %4s %4s %4s",
		 io, fd, gid, hnd, knd, dev);

	 fd  = pioc->out.fd;
	 gid = pioc->out.gid;
	 hnd = hn[pioc->out.hnd];
	 knd = kn[pioc->out.knd + 1];
	 dev = dn[pioc->out.dev];

	 if (pioc->out.dev == IO_DEV_PIPE)
	    _dbg(-1, "    write %3d %3d %4s %4s %4s %d",
		 fd, gid, hnd, knd, dev, gid);
	 else
	    _dbg(-1, "    write %3d %3d %4s %4s %4s",
		 fd, gid, hnd, knd, dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRGRP - diagnostic print of process_group tasks */

void dprgrp(char *tag, process_group *pg)
   {int i, n;
    process *pp, *cp;

    n = pg->np;

    _dbg(-1, "-----------------------------------------------");
    _dbg(-1, "%s  group", tag);
    _dbg(-1, "%d processes", n);

    _dbg(-1, "         Unit  fd  dev  knd  hnd gid");
    for (i = 0; i < n; i++)
        {pp = pg->parents[i];
	 cp = pg->children[i];

         _dbg(-1, "process #%d: %s", i, pp->cmd);

	 if (cp != NULL)
	    {dprdio("child",  cp->io + IO_STD_IN);
	     dprdio("child",  cp->io + IO_STD_OUT);
	     dprdio("child",  cp->io + IO_STD_ERR);};

	 if (pp != NULL)
	    {dprdio("parent", pp->io + IO_STD_IN);
	     dprdio("parent", pp->io + IO_STD_OUT);
	     dprdio("parent", pp->io + IO_STD_ERR);};

	 _dbg(-1, "");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRGIO - print the file descriptors from the processes of a group */

void dprgio(char *tag, int n, process **pa, process **ca)
   {int i;
    char *hnd;
    process *pp, *cp;

    _dbg(-1, "-----------------------------------------------");
    _dbg(-1, "%s", tag);
    _dbg(-1, "");

    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];
	 if ((pp != NULL) && (cp != NULL))
	    {_dbg(-1, "command: %s", pp->cmd);
             switch (pp->io[1].hnd)
	        {case IO_HND_PIPE :
		      hnd = "pipe";
		      break;
	         case IO_HND_POLL :
		      hnd = "poll";
		      break;
	         case IO_HND_CLOSE :
		      hnd = "clsd";
		      break;
	         default :
		      hnd = "none";
		      break;};
	     _dbg(-1, "   stdin:  %3d(%s) -> %3d",
		  pp->io[1].fd, hnd, cp->io[0].fd);

             switch (pp->io[0].hnd)
	        {case IO_HND_PIPE :
		      hnd = "pipe";
		      break;
	         case IO_HND_POLL :
		      hnd = "poll";
		      break;
	         case IO_HND_CLOSE :
		      hnd = "clsd";
		      break;
	         default :
		      hnd = "none";
		      break;};
	     _dbg(-1, "   stdout: %3d(%s) <- %3d",
		  pp->io[0].fd, hnd, cp->io[1].fd);

             switch (pp->io[2].hnd)
	        {case IO_HND_PIPE :
		      hnd = "pipe";
		      break;
	         case IO_HND_POLL :
		      hnd = "poll";
		      break;
	         case IO_HND_CLOSE :
		      hnd = "clsd";
		      break;
	         default :
		      hnd = "none";
		      break;};
	     _dbg(-1, "   stderr: %3d(%s) <- %3d",
		     pp->io[2].fd, hnd, cp->io[2].fd);

	     _dbg(-1, "");};};

    _dbg(-1, "-----------------------------------------------");

    return;}

/*--------------------------------------------------------------------------*/

/*                               GROUP ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* REDIRECT_IO - modify the I/O channel of the IP process in PG
 *             - according to IO
 */

void redirect_io(process_group *pg, int ip, iodes *io)
   {iodes *pio;
    process *pp, *cp;

    pp = pg->parents[ip];
    cp = pg->children[ip];

/* add the redirect specifications to the filedes */
    switch (io->knd)
       {case IO_STD_IN :
	     pio    = &pp->ioc[io->knd].in;
	     io->fd = pio->fd;
	     *pio   = *io;

	     pio    = pp->io + io->knd;
	     io->fd = pio->fd;
	     *pio   = *io;
	     break;

        case IO_STD_OUT :
        case IO_STD_ERR :
	     pio    = &pp->ioc[io->knd].out;
	     io->fd = pio->fd;
	     *pio   = *io;

	     pio    = pp->io + io->knd;
	     io->fd = pio->fd;
	     *pio   = *io;
	     break;

        case IO_STD_BOND :
	     pio    = &pp->ioc[IO_STD_OUT].out;
	     io->fd = pio->fd;
	     *pio   = *io;
	     pio    = &pp->ioc[IO_STD_ERR].out;
	     *pio   = *io;

	     pio    = &cp->ioc[IO_STD_OUT].out;
	     io->fd = pio->fd;
	     *pio   = *io;
	     pio    = &cp->ioc[IO_STD_ERR].out;
	     *pio   = *io;

	     pio    = pp->io + IO_STD_OUT;
	     io->fd = pio->fd;
	     *pio   = *io;
	     pio    = pp->io + IO_STD_ERR;
	     *pio   = *io;

	     pio    = cp->io + IO_STD_OUT;
	     io->fd = pio->fd;
	     *pio   = *io;
	     pio    = cp->io + IO_STD_ERR;
	     *pio   = *io;
	     break;

        default :
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDIRECT_FD - determine the I/O redirections in PP->ARG
 *             - and fill in PP->IO from them
 */

static int redirect_fd(process_group *pg, int ip, int i)
   {int ck, cd, aip, bip, nc, ni, rel, pos;
    char *t, **ta;
    char *p;
    io_mode amd, bmd;
    io_device dev;
    io_kind aknd, bknd;
    iodes ca, cb;
    process *pp;

    pp = pg->parents[ip];

/* parse out the redirect related specifications */
    ta = pp->ios;
    t  = ta[i];
	
    aip  = ip;
    bip  = ip;
    amd  = IO_MODE_NONE;
    bmd  = IO_MODE_NONE;
    dev  = IO_DEV_NONE;
    aknd = IO_STD_NONE;
    bknd = IO_STD_NONE;

    if (t[0] == PROCESS_DELIM)
       t++;

    ni = strlen(t);
    nc = 0;
    ck = t[nc++];
    cd = (nc < ni) ? t[nc++] : '?';

/* look for special devices */
    if (strchr("rxlv", t[0]) != NULL)
       {}

/* determine the device and mode */
    else if (strchr("ioeb", t[0]) != NULL)
       {dev = IO_DEV_PIPE;

/* get the device kind */
	aknd = _io_kind(ck);

/* get the device mode */
	amd = _io_mode(aknd);
	if (aknd == IO_STD_IN)
	   {bmd  = _io_mode(IO_STD_OUT);
	    bknd = IO_STD_OUT;}
	else
	   {bmd = _io_mode(IO_STD_IN);
	    bknd = IO_STD_IN;};

/* find the group id of the other end of the I/O connection */
	rel = TRUE;
	if (strchr("0123456789", cd) != NULL)
	   {rel = FALSE;
	    nc--;};

	if (nc >= ni)
	   pos = 1;
	else
	   {p   = t + nc;
	    pos = (IS_NULL(p) == TRUE) ? 1 : atoi(p);};
	if (rel == TRUE)
	   aip += pos;
	else
	   aip = pos - 1;};

    _init_iodes(1, &ca);

    ca.knd  = aknd;
    ca.dev  = dev;
    ca.gid  = aip;
    ca.mode = amd;

    redirect_io(pg, bip, &ca);

    _init_iodes(1, &cb);

    cb.knd  = bknd;
    cb.dev  = dev;
    cb.gid  = bip;
    cb.mode = bmd;

    redirect_io(pg, aip, &cb);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDIRECT_COPY - copy the redirections from PP to CP */

static void redirect_copy(process *cp, process *pp)
   {int i, fd;

    for (i = 0; i < N_IO_CHANNELS; i++)
        {fd = cp->io[i].fd;
	 cp->io[i]    = pp->io[i];
	 cp->io[i].fd = fd;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDIRECT_PROCESS - look through the I/O specifications of PP */

static void redirect_process(process_group *pg, int it)
   {int i, n;
    char **ta;
    process *pp;

    pp = pg->parents[it];
    ta = pp->ios;
    n  = lst_length(ta);

/* find self-specifications in each process */
    for (i = 0; i < n; i++)
        i = redirect_fd(pg, it, i);

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
   {int i, ip, np, gid;
    io_connector *ioc, *pioc;
    process **pa, *pp;
    iodes *pio, *dio;

    np  = pg->np;
    pa  = pg->parents;
    ioc = pg->ioc;

    for (ip = 0; ip < np; ip++)
        {pp = pa[ip];

	 for (i = 0; i < N_IO_CHANNELS; i++)
	     {pioc = ioc + N_IO_CHANNELS*ip + i;

	      if (pioc->in.hnd == IO_HND_NONE)
		 {pioc->in.hnd  = (pp->isfunc) ? IO_HND_FNC : IO_HND_POLL;
		  pioc->in.mode = IO_MODE_RO;}

	      if (pioc->out.hnd == IO_HND_NONE)
		 {pioc->out.hnd  = (pp->isfunc) ? IO_HND_FNC : IO_HND_POLL;
		  pioc->out.mode = IO_MODE_WD;};};

/* stdout */
         pio = pp->io + 1;
	 switch (pio->dev)
            {case IO_DEV_PIPE :
	          gid = pio->gid;
	          dio = pa[gid]->io + IO_STD_IN;
		  dio->gid = ip;
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
		  dio->gid = ip;
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
    iodes *pio, *cio;
    FILE *fp;

    pio = pn->io + pk;
    cio = cn->io + ck;

    fd = pio->fd;
    fp = pio->fp;

    pio->hnd = IO_HND_PIPE;
    pio->fp  = NULL;

    pio->fd  *= -1;
    pio->hnd  = IO_HND_CLOSE;

/* GOTCHA: we can close something we need in the bonded case
 * or leak descriptors in the other cases
 * better bookkeeping is needed here
    _fd_close(cio->fd);
 */
    cio->hnd = IO_HND_PIPE;
    cio->fp  = fp;
    cio->fd  = fd;

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH_FD - register the KND descriptor of PN to be polled */

static int watch_fd(process *pn, io_kind pk)
   {int ip, fd;
    io_hand hnd;
    io_connector *ioc;

    ip  = pn->ip;
    ioc = pn->ioc + pk;
    fd  = (pk == IO_STD_IN) ? ioc->out.fd  : ioc->in.fd;
    hnd = (pk == IO_STD_IN) ? ioc->out.hnd : ioc->in.hnd;
    fd  = ioc->in.fd;
    hnd = ioc->in.hnd;

    fd  = pn->io[pk].fd;
    hnd = pn->io[pk].hnd;

    if ((fd > 2) && (hnd != IO_HND_PIPE) && (hnd != IO_HND_CLOSE))
       {_awatch_fd(pn, pk, ip);

	_dbg(1, "watch fd=%d on %d @ %s", fd, ip, _name_io(pk));

	ioc->out.hnd   = IO_HND_POLL;
	pn->io[pk].hnd = IO_HND_POLL;};

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRANSFER_IO - transfer the B connector to A
 *             - the A connector is specified by IOC, IA, and AK
 *             - the B connector is specified by IOC, IB, and BK
 */

static void transfer_io(io_connector *ioc, int ia, io_kind ak,
			int ib, io_kind bk)
   {io_connector *pia, *pib;

    pia = ioc + N_IO_CHANNELS*ia + ak;
    pib = ioc + N_IO_CHANNELS*ib + bk;

    pib->in.fd   = pia->in.fd;
    pib->in.dev  = IO_DEV_PIPE;
    pib->in.hnd  = IO_HND_PIPE;
    pib->in.gid  = ia;

    pib->out.fd  = pia->out.fd;
    pia->out.dev = IO_DEV_PIPE;
    pia->out.hnd = IO_HND_PIPE;
    pia->out.gid = ib;
    pia->in.dev  = IO_DEV_PIPE;
    pia->in.hnd  = IO_HND_PIPE;
    pia->in.gid  = ib;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_PARENT_CHILD - close all non-terminal parent to child lines */

void close_parent_child(process_group *pg)
   {int ip, np, nm;
    iodes *pio, *cio;
    process *pp, *cp, **pa, **ca;

    np = pg->np;
    pa = pg->parents;
    ca = pg->children;
    nm = np - 1;

    for (ip = 1; ip < nm; ip++)
        {pp = pa[ip];
	 cp = ca[ip];

	 pio = pp->io + IO_STD_OUT;
	 cio = cp->io + IO_STD_IN;

	 if (pio->fd > 2)
	    {_fd_close(pio->fd);
	     _fd_close(cio->fd);
	     pio->fd *= -1;};

	 pio->hnd = IO_HND_CLOSE;
	 cio->hnd = IO_HND_CLOSE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CHILD_OUT_IN - connect all non-terminal child output
 *                      - to appropriate child input
 */

void connect_child_out_in(process_group *pg)
   {int ip, np, gi, nm;
    io_device dv;
    iodes *pio;
    process *pp, **pa, **ca;

    np = pg->np;
    pa = pg->parents;
    ca = pg->children;
    nm = np - 1;

    for (ip = 0; ip < nm; ip++)
        {pp = pa[ip];

/* stdout */
	 pio = pp->io + IO_STD_OUT;
	 gi  = pio->gid;
	 dv  = pio->dev;

/* default to the next process in group */
	 if ((dv == IO_DEV_PIPE) && (gi == -1))
	    gi = ip + 1;

	 if (gi >= 0)
	    transfer_fd(pp, IO_STD_IN, ca[gi], IO_STD_IN);

/* stderr */
	 pio = pp->io + IO_STD_ERR;
	 if (pio->knd != IO_STD_BOND)
	    {gi  = pio->gid;
	     dv  = pio->dev;

/* default to the next process in group */
	     if ((dv == IO_DEV_PIPE) && (gi == -1))
	        gi = ip + 1;

	     if (gi >= 0)
	        transfer_fd(pp, IO_STD_ERR, ca[gi], IO_STD_IN);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRANSFER_IN - transfer I/O channel KND from process IA to IB */

void transfer_in(process_group *pg, int ia, int ib, io_kind knd)
   {int np, fd;
    iodes *pia, *pib, *cia, *cib;
    process *pa, *pb, *ca, *cb;

    np = pg->np;
    if ((0 <= ia) && (ia < np) && (0 <= ib) && (ib < np))
       {pa = pg->parents[ia];
	ca = pg->children[ia];
	pb = pg->parents[ib];
	cb = pg->children[ib];

	pia = pa->io + knd;
	cia = ca->io + knd;
	pib = pb->io + knd;
	cib = cb->io + knd;
	if ((cib->gid == ia) &&
	    ((cia->dev == IO_DEV_PIPE) && (cia->gid == -1)))
	   {fd = pia->fd;

	    transfer_fd(pa, knd, cb, knd);
	    pia->hnd = IO_HND_NONE;
	    pia->dev = IO_DEV_NONE;
	    pia->fd  = -2;
	    pia->fp  = NULL;

	    if (pib->fd == fd)
	       {pib->fd = -2;
		pib->fp = NULL;
		pib->hnd = IO_HND_NONE;
		pib->dev = IO_DEV_NONE;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CHILD_IN_OUT - connect all non-terminal child input to
 *                      - appropriate child output
 */

void connect_child_in_out(process_group *pg)
   {int i0, ip, np, gi, nm;
    io_device dv;
    iodes *pia;
    process *pp, **pa;

    np = pg->np;
    pa = pg->parents;
    nm = np - 1;
    i0 = 0;

    for (ip = i0; ip < nm; ip++)
        {pp = pa[ip];

/* stdin */
	 pia = pp->io + IO_STD_IN;
	 gi  = pia->gid;
	 dv  = pia->dev;

/* default to the next process in group */
	 if ((dv == IO_DEV_PIPE) && (gi == -1))
	    gi = ip + 1;

	 if (gi >= 0)
	    {transfer_in(pg, ip, gi, IO_STD_OUT);
	     transfer_in(pg, ip, gi, IO_STD_ERR);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRANSFER_FNC_CHILD - transfer all function call I/O from child
 *                    - to parent and free child
 */

void transfer_fnc_child(process_group *pg)
   {int i, io, n;
    char **ta;
    iodes *pio;
    process *pp, *cp;
    process **pa, **ca;

    n   = pg->np;
    pa  = pg->parents;
    ca  = pg->children;
 
    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];

	 ta = pp->arg;
	 if ((strcmp(ta[0], "gexec") == 0) && (strcmp(ta[1], "-p") == 0))
	    {pp->isfunc = TRUE;
	     cp->isfunc = TRUE;};

	 if (pp->isfunc == TRUE)
	    {for (io = 0; io < N_IO_CHANNELS; io++)
		 {pio = pp->io + io;
		  if ((i == n-1) && (io == IO_STD_OUT))
		     {if (pio->dev == IO_DEV_TERM)
			 {
/* need this for date @o pw:test but
 * cannot tolerate it for cat @i fr:foo
 */
			  pio->fd  = io;
			  pio->hnd = IO_HND_NONE;
			  pio->dev = IO_DEV_TERM;};}

		  else if (pio->dev == IO_DEV_PIPE)
		     transfer_fd(cp, io, pp, io);

		  else
		     {pio->fd  = io;
		      pio->hnd = IO_HND_POLL;};

		  pio->dev = IO_DEV_FNC;};

	      _job_free(cp);

	      ca[i] = NULL;};};

    return;}

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
   {int i, nm, n;
    io_connector *ioc;
    process *pt, *pp, *cp;
    process **pa, **ca;

    n   = pg->np;
    pa  = pg->parents;
    ca  = pg->children;
    ioc = pg->ioc;
    pt  = pg->terminal;
    nm  = n - 1;

    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];

/* process out I/O redirections if we don't need a shell to do the command */
	 redirect_process(pg, i);
	 redirect_copy(cp, pp);

/* remove a layer of quotes if we don't need a shell to do the command */
	 unquote_process(pp);
	 unquote_process(cp);};

    fillin_pgrp(pg);

/* remove the parent to child channels except for the last one
 * the final parent out gets connected to the first child in
 */
    if (nm > 0)
       {int ig, ip;
	io_connector *sio;

	transfer_io(ioc, 0, IO_STD_IN, nm, IO_STD_OUT);
        for (ip = 0; ip < n; ip++)
            {for (i = 0; i < N_IO_CHANNELS; i++)
                 {sio = ioc + N_IO_CHANNELS*ip + i;
		  ig  = sio->out.gid;
		  if ((sio->out.dev == IO_DEV_PIPE) &&
		      (sio->out.hnd != IO_HND_PIPE))
		     transfer_io(ioc, ig, IO_STD_IN, ip, i);
		  else if ((sio->in.dev == IO_DEV_PIPE) &&
			   (sio->in.hnd != IO_HND_PIPE))
		     transfer_io(ioc, ig, IO_STD_OUT, ip, i);};};
                     
        for (ip = 0; ip < n; ip++)
            {for (i = 0; i < N_IO_CHANNELS; i++)
                 {sio = ioc + N_IO_CHANNELS*ip + i;
		  if ((sio->in.dev == IO_DEV_PIPE) &&
		      (sio->out.hnd != IO_HND_PIPE))
		     {sio->out.dev = IO_DEV_PIPE;
		      sio->out.hnd = IO_HND_PIPE;
		      sio->out.gid = sio->in.gid;};};};

/* reconnect terminal process output to first process */
	transfer_fd(pa[0], IO_STD_OUT, pt, IO_STD_OUT);

/* close all other parent to child lines */
	close_parent_child(pg);

/* connect all non-terminal children output to appropriate child input
 * there are from output specifications @o, @e, @b, @r, and @x 
 */
	connect_child_out_in(pg);

/* connect all non-terminal children input to appropriate child output
 * there are from input specifications @i, @l, and @v
 */
	connect_child_in_out(pg);

/* transfer all function call I/O to parent and free child */
	transfer_fnc_child(pg);};

    if (dbg_level & 1)
       dprgrp("reconnect_pgrp", pg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_PGRP - fill the ITth process in PG from the tokens TA */

static void setup_pgrp(process_group *pg, int it,
		       char **ta, int dosh, char **ios)
   {process *pp, *cp, **pa, **ca;
    io_connector *ioc;

    pa  = pg->parents;
    ca  = pg->children;
    ioc = pg->ioc + N_IO_CHANNELS*it;

    pg->to = _job_setup_proc(&pp, &cp, ioc, ta, pg->env, pg->shell);

    pp->ios = ios;
    pp->ip  = it;
    cp->ip  = it;

    pa[it] = pp;
    ca[it] = cp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPAND_SHORTHAND - expand shorthand notations:
 *                  -  file       =>  f<x>:<name>  -> gexec -p file -<x> <name>
 *                  -  variable   =>  v<x>:<name>  -> gexec -p var -<x> <name>
 *                  -  procedure  =>  p<x>:<name>  -> gexec -p <name> -<x>
 *                  -  executable =>  x<x>:<name>  -> <name>
 */

static char **expand_shorthand(char **ta, char *t)
   {char s[MAXLINE];
    char *p, *md, *nm;

    if (t != NULL)
       {nstrncpy(s, MAXLINE, t + 1, -1);
	p = strchr(s, ':');
	*p++ = '\0';
	md = s;
	nm = p;

	switch (t[0])

/* file */
	   {case 'f' :
	         ta = lst_add(ta, "gexec");
	         ta = lst_add(ta, "-p");
	         ta = lst_add(ta, "file");
	         ta = lst_push(ta, "-%s", md);
	         ta = lst_add(ta, nm);
		 break;

/* variable */
	    case 'v' :
	         ta = lst_add(ta, "gexec");
	         ta = lst_add(ta, "-p");
	         ta = lst_add(ta, "var");
	         ta = lst_push(ta, "-%s", md);
	         ta = lst_add(ta, nm);
		 break;

/* procedure */
	    case 'p' :
	         ta = lst_add(ta, "gexec");
	         ta = lst_add(ta, "-p");
	         ta = lst_add(ta, nm);
	         ta = lst_push(ta, "-%s", md);
		 break;

/* executable */
	    case 'x' :
	         ta = lst_add(ta, t);
		 break;};};

    return(ta);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SUBST_SYNTAX - substitute new constructions for old ones
 *              -   '< <name>'    ->  @i pr:file <name>
 *              -   '> <name>'    ->  @o pw:file <name>
 *              -   '>! <name>'   ->  @o pw:file <name>
 *              -   '1> <name>'   ->  @o pw:file <name>
 *              -   '>> <name>'   ->  @o pa:file <name>
 *              -   '2> <name>'   ->  @e pw:file <name>
 *              -   '>& <name>'   ->  @b pw:file <name>
 *              -   '&> <name>'   ->  @b pw:file <name>
 *              -   '>&! <name>'  ->  @b pw:file <name>
 *              -   '>>& <name>'  ->  @b pa:file <name>
 *              -   '|'           ->  @o
 *              -   '|&'          ->  @b
 */

char **subst_syntax(char **sa)
   {int i, nc;
    char *t, **ra;

    ra = NULL;
    nc = lst_length(sa);
    for (i = 0; i < nc; i++)
        {t = sa[i];

/* stdin replacements */
	 if (strcmp(t, "<") == 0)
	    {ra = lst_add(ra, "@i");
	     ra = lst_add(ra, "fr:file");}

/* stdout replacements */
	 else if ((strcmp(t, ">") == 0) ||
	     (strcmp(t, ">!") == 0) ||
	     (strcmp(t, "1>") == 0))
	    {ra = lst_add(ra, "@o");
	     ra = lst_push(ra, "fw:%s", sa[++i]);}

	 else if (strcmp(t, ">>") == 0)
	    {ra = lst_add(ra, "@o");
	     ra = lst_push(ra, "fa:%s", sa[++i]);}

/* stderr replacements */
	 else if (strcmp(t, "2>") == 0)
	    {ra = lst_add(ra, "@e");
	     ra = lst_push(ra, "fw:%s", sa[++i]);}

/* bonded replacements */
	 else if ((strcmp(t, ">&") == 0) ||
		  (strcmp(t, "&>") == 0) ||
		  (strcmp(t, ">&!") == 0))
	    {ra = lst_add(ra, "@e");
	     ra = lst_push(ra, "fw:%s", sa[++i]);}

	 else if (strcmp(t, ">>&") == 0)
	    {ra = lst_add(ra, "@e");
	     ra = lst_push(ra, "fa:%s", sa[++i]);}

/* pipe replacements */
	 else if (strcmp(t, "|") == 0)
	    ra = lst_add(ra, "@o");

/* bonded pipe replacements */
	 else if (strcmp(t, "|&") == 0)
	    ra = lst_add(ra, "@b");

/* everything else */
	 else
	    ra = lst_add(ra, t);};

    free_strings(sa);

    return(ra);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MK_CONNECTORS - initialize a set of io_connectors for NP processes */

io_connector *mk_connectors(int np)
   {int ip, i;
    io_device dev;
    io_mode md;
    io_connector *ioc, *pioc;

    ioc = MAKE_N(io_connector, N_IO_CHANNELS*np);
    for (ip = 0; ip < np; ip++)
        {for (i = 0; i < N_IO_CHANNELS; i++)
	     {pioc = ioc + N_IO_CHANNELS*ip + i;

	      dev = IO_DEV_TERM;
	      md  = _io_mode(i);

	      pioc->in.knd   = i;
	      pioc->in.dev   = dev;
	      pioc->in.mode  = md;
	      pioc->in.gid   = -1;
	      pioc->in.fd    = -1;

	      pioc->out.knd  = i;
	      pioc->out.dev  = dev;
	      pioc->out.mode = md;
	      pioc->out.gid  = -1;
	      pioc->out.fd   = -1;};};

    return(ioc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_PGRP - parse out specifications in S to initialize PG */

static void parse_pgrp(statement *s)
   {int i, j, it, nc, dosh, doif, term;
    char *t, *shell;
    char **sa, **ta, **env, **ios;
    io_connector *ioc;
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

    sa = subst_syntax(sa);

/* maximum number of process would be the number of tokens */
    pa  = MAKE_N(process *, nc);
    ca  = MAKE_N(process *, nc);

    ioc = mk_connectors(nc);

    pg = MAKE(process_group);
    if (pg != NULL)
       {pg->np       = 0;
	pg->to       = 0;
	pg->mode     = NULL;
	pg->shell    = shell;
	pg->env      = env;
	pg->terminal = NULL;
	pg->ioc      = ioc;
	pg->parents  = pa;
	pg->children = ca;};

    for (i = 0; i < nc; i++)
        {t   = sa[i];
	 ios = NULL;

	 if (t == NULL)
	    continue;

	 else if (strchr(t, PROCESS_DELIM) != NULL)
	    {for (j = i; j < nc; j++)
	         {if (strchr(sa[j], PROCESS_DELIM) != NULL)
		     {ios = lst_add(ios, sa[j]);
		      sa[j] = NULL;}
		  else
		     break;};
	     term  = TRUE;}

	 else if (strpbrk(t, "[]()$*`") != NULL)
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
	    {if ((strchr(t, ':') != NULL) && (strchr("fvpx", t[0]) != NULL))
	        ta = expand_shorthand(ta, t);
	     else
	        ta = lst_add(ta, t);};};

    if ((i >= nc) && (lst_length(ta) > 0))
       setup_pgrp(pg, it++, ta, dosh, ios);

    pg->np       = it;
    pg->terminal = pa[it - 1];

    reconnect_pgrp(pg);

    _dbg(1, "process group: %s", s->text);

    s->ne = it;
    s->pg = pg;

    return;}

/*--------------------------------------------------------------------------*/

/*                            RUN GROUP ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PGRP_ACCEPT - accept messages read from PP */

int _pgrp_accept(int fd, process *pp, char *s)
   {int rv;

    _dbg(2, "accept from %d", fd);

    rv = fputs(s, stdout);
    rv = (rv >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_REJECT - reject messages read from PP */

int _pgrp_reject(int fd, process *pp, char *s)
   {int rv;

    rv = TRUE;

    _dbg(1, "reject: fd(%d) cmd(%s) txt(%s)", fd, pp->cmd, s);

    if (dbg_level & 2)
       fputs(s, stderr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_WAIT - call when wait says its done */

void _pgrp_wait(process *pp)
   {int rv;

    if (dbg_level & 2)
       {char *st;

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

	if (pp->isfunc == TRUE)
	   _dbg(2, "wait function %d: %s", pp->ip, pp->cmd);
	else
	   _dbg(2, "wait process %d: (%d) %s", pp->ip, pp->id, pp->cmd);

	_dbg(2, "   status %s (%d) (alive %d)",
	     st, pp->reason, job_alive(pp));};

    rv = job_done(pp, SIGTERM);
    ASSERT(rv == 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_TTY - check the TTY
 *           - return TRUE if we are done
 */

int _pgrp_tty(char *tag)
   {int nf, np, rv;

    rv = FALSE;
    np = stck.np;

    nf = acheck();
    rv = (nf == np);

    if (rv == TRUE)
       _dbg(2, "all done");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _FNC_WAIT - analog of job_wait for functions
 *           - return TRUE iff function is still running
 */

int _fnc_wait(process_group *pg, int ip, int st)
   {int io, rv, sts, cnd;
    process *pp;

    rv = TRUE;

    pp = pg->parents[ip];

    if (job_alive(pp) == TRUE)
       {if (st == 0)
	   pp->status = JOB_RUNNING;

/* negative status means error exit */
	else if (st < 0)
	   {cnd = JOB_EXITED;
	    sts = -st;
	    rv  = FALSE;}

/* positive status means "normal" exit */
	else if (st > 0)
	   {cnd = JOB_EXITED;
	    sts = 0;
	    rv  = FALSE;};

	if (st != 0)
	   {int fd, jo;
            iodes *pio;
	    process *pd;
	    FILE *fp;

	    _block_all_sig(TRUE);

	    for (io = 0; io < N_IO_CHANNELS; io++)
	        {pio = pp->io + io;
		 if (pio->gid != -1)
		    {fd = pio->fd;
		     fp = pio->fp;
		     if (fd > 2)
		        _fd_close(fd);
		     if (fp != NULL)
		        fclose(fp);

		     pio->fd  = -1;
		     pio->fp  = NULL;
		     pio->hnd = IO_HND_NONE;
		     pio->dev = IO_DEV_NONE;
		     pio->knd = IO_STD_NONE;

		     pd = pg->parents[pio->gid];

		     for (jo = 0; jo < N_IO_CHANNELS; jo++)
		         {if (pd->io[jo].gid == ip)
			     {job_read(pd->io[jo].fd, pd, pd->accept);
			      _job_io_close(pd, jo);};};

		     kill(pd->id, SIGHUP);

		     job_wait(pd);};};

	    pp->stop_time = wall_clock_time();
	    pp->status    = cnd;
	    pp->reason    = sts;

	    if ((pp->wait != NULL) && (pp->isfunc == FALSE))
	       pp->wait(pp);

	    _block_all_sig(FALSE);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_WORK - main worker handling I/O connections for
 *            - running process group
 *            - strong version of function/procedure call
 *            - function name is mapped to function
 *            - it is then called and the result
 *            - sent to the appropiate output channel
 */

void _pgrp_work(int i, char *tag, void *a, int nd, int np, int tc, int tf)
   {int c, io, ip, ne, rv;
    io_mode md;
    io_device dv;
    io_hand hnd;
    iodes *pio;
    char *ms, *db, *fn, **v;
    FILE *fp[3];
    statement *s;
    process_group *pg;
    process *pp, **pa;
    PFPCAL f;
    PFPCAL (*map)(char *nm);

    s   = (statement *) a;
    map = s->map;
    if (map != NULL)
       {pg = s->pg;
        ne = s->ne;
	db = getenv("PERDB_PATH");

        pa = pg->parents;
	for (ip = 0; ip < ne; ip++)
	    {pp = pa[ip];
	     if (job_running(pp) != TRUE)
	        continue;

/* if the current process is a function execute it */
	     for (io = 0; io < N_IO_CHANNELS; io++)
	         {pio = pp->io + io;
		  md  = pio->mode;
		  dv  = pio->dev;
		  hnd = pio->hnd;
		  if (((hnd == IO_HND_FNC) || (dv == IO_DEV_FNC)) &&
		      (pio->gid != -1))
		     {v  = pp->arg;
		      c  = lst_length(pp->arg);
		      fn = v[2];
		      ms = v[3];
		      f  = map(fn);
		      if (f != NULL)
			 {v += 4;
			  c -= 4;

			  switch (ms[1])
			     {case 'r' :
				   md = IO_MODE_RO;
				   break;
			      case 'w' :
				   md = IO_MODE_WD;
				   break;
			      case 'a' :
				   md = IO_MODE_APPEND;
				   break;
			      default :
				   if (io == IO_STD_IN)
				      md = IO_MODE_WD;
				   else
				      md = IO_MODE_RO;
				   break;};

			  fp[0] = _io_file_ptr(pp, IO_STD_IN);
			  fp[1] = _io_file_ptr(pp, IO_STD_OUT);
			  fp[2] = _io_file_ptr(pp, IO_STD_ERR);

/*			  _dbg(2, "call '%s' (%d)", fn, i); */

			  nsleep(1);

			  rv = f(db, md, fp, fn, c, v);
			  rv =_fnc_wait(pg, ip, rv);
			  if (rv == FALSE)
			     break;};};};};};

    sched_yield();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_FIN - record the exit status of PP in A */

void _pgrp_fin(process *pp, void *a)
   {int i;
    int *st;
    statement *s;

    i  = pp->ip;
    s  = (statement *) a;
    st = s->st;
    st[i] = pp->reason;

    job_done(pp, SIGKILL);

    _dbg(2, "fin %d with status %d (alive %d)",
	 pp->id, st[i], job_alive(pp));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_IO_PGRP - setup to poll all remaining process side descriptors */

void register_io_pgrp(process_group *pg)
   {int i, ip, np;
    process *pp, **pa;

    np = pg->np;
    pa = pg->parents;

    for (ip = 0; ip < np; ip++)
        {pp = pa[ip];
         for (i = 0; i < N_IO_CHANNELS; i++)
	     watch_fd(pp, i);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_PGRP - run the process group PG */

static int run_pgrp(statement *s)
   {int i, io, ne, np, rv, tc, fd;
    char vl[MAXLINE];
    char *db;
    process *pp, *cp;
    process_group *pg;

    rv = -1;

    if (s != NULL)
       {pg = s->pg;
	ne = s->ne;

	s->st = MAKE_N(int, ne);

	for (i = 0; i < ne; i++)
	    {pp = pg->parents[i];
	     if (pp->isfunc == TRUE)
	        s->nf++;
	     else
	        s->np++;};

	np = s->np;

	asetup(ne, 1);

	register_io_pgrp(pg);

/* launch the jobs - io_data passed to accept, reject, and wait methods */
	for (i = 0; i < ne; i++)
	    {pp = pg->parents[i];
	     cp = pg->children[i];

	     if (pp->isfunc == FALSE)
	        {pp = _job_fork(pp, cp, NULL, "rw", s);

		 _dbg(2, "launch %d (%d,%d,%d)       %s",
		      pp->id,
		      pp->io[0].fd, pp->io[1].fd, pp->io[2].fd,
		      pp->cmd);}
 	     else
 	        {pp->a = s;
		 for (io = 0; io < N_IO_CHANNELS; io++)
 		     _io_file_ptr(pp, io);};

	     pp->accept   = _pgrp_accept;
	     pp->reject   = _pgrp_reject;
	     pp->wait     = _pgrp_wait;
	     pp->nattempt = 1;
	     pp->ip       = i;

	     ASSERT(rv == 0);};

/* load up the process stack */
	for (i = 0; i < np; i++)
	    {pp = pg->parents[i];
	     fd = pp->io[0].fd;

	     stck.proc[i]       = pp;
	     stck.fd[i].fd      = fd;
	     stck.fd[i].events  = stck.mask_acc;
	     stck.fd[i].revents = 0;};

/* wait for the work to complete - _pgrp_work does the work */
	tc = await(-1, 1, "commands", _pgrp_tty, _pgrp_work, s);
	ASSERT(tc >= 0);

/* close out the jobs */
	afin(_pgrp_fin);

/* process the exit statuses */
	rv    = 0;
	vl[0] = '\0';
	for (i = 0; i < ne; i++)
	    {rv |= s->st[i];
	     vstrcat(vl, MAXLINE, "%d ", s->st[i]);};
	LAST_CHAR(vl) = '\0';

	db = getenv("PERDB_PATH");
	if (IS_NULL(db) == FALSE)
	   dbset(NULL, "gstatus", vl);
	else
	   {printf("setenv gstatus \"%s\"\n", vl);
	    fflush(stdout);};

        FREE(s->st);};

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
	 FREE(sl[n].st);
	 free_pgrp(sl[n].pg, sl[n].ne);};

    FREE(sl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_STATEMENT - parse a string S into an array of statements
 *                 - delimited by: ';', '&&', or '||'
 */

statement *parse_statement(char *s, char **env, char *shell,
			   PFPCAL (*map)(char *s))
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

/* handle ';' statement separator */
		  if (c == ';')
		     {ps[-1] = '\0';
		      trm    = ST_NEXT;
		      break;}

/* handle '&&' statement separator */
		  else if (c == '&')
		     {c = *ps++;
		      if (c == '&')
			 {ps[-2] = '\0';
			  trm    = ST_AND;
			  break;}
		      else if (c == '\0')
			 break;}

/* handle '||' statement separator */
		  else if (c == '|')
		     {c = *ps++;
		      if (c == '|')
			 {ps[-2] = '\0';
			  trm    = ST_OR;
			  break;};};};

	     if (sa == NULL)
	        sa = MAKE_N(statement, 1000);

	     if (sa != NULL)
	        {sa[i].np         = 0;
	         sa[i].nf         = 0;
	         sa[i].ne         = 0;
                 sa[i].terminator = trm;
		 sa[i].text       = STRSAVE(trim(pt, BOTH, " \t\n\r\f"));
		 sa[i].shell      = STRSAVE(shell);
		 sa[i].env        = env;
		 sa[i].st         = NULL;
		 sa[i].map        = map;
		 i++;};

	     pt = ps;};

	FREE(t);};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
  
/* INIT_SESSION - initialize a process session instance
 *              - setup OS level process group properly
 *              - make sure the session is running interactively
 *              - as the foreground job before proceeding
 */
     
process_session *init_session(void)
   {int pgid, tid, fin, iact;
    struct termios attr;
    process_session *ps;
     
    ps   = NULL;
    fin  = STDIN_FILENO;
    iact = isatty(fin);
     
    if (iact == TRUE)

/* make sure we are in the foreground */
       {while (TRUE)
	  {pgid = getpgrp();
	   tid  = tcgetpgrp(fin);
	   if (tid == pgid)
	      break;
	   kill(-pgid, SIGTTIN);};
     
/* ignore interactive and job-control signals */
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
     
/* put the current process in its own group */
	pgid = getpid();
	if (setpgid(pgid, pgid) < 0)
	   _dbg(-1, "Couldn't put the session in its own process group");

	else
     
/* take control of the terminal */
	   {tcsetpgrp(fin, pgid);
     
/* save default terminal attributes for session */
	    tcgetattr(fin, &attr);

	    ps = MAKE(process_session);
	    if (ps != NULL)
	       {ps->pgid        = pgid;
		ps->terminal    = fin;
		ps->interactive = iact;
		ps->foreground  = TRUE;
		ps->attr        = attr;};};};

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_FOREGROUND - put PP in the foreground
 *                - if cont is TRUE restore the saved terminal modes and
 *                - send the process group a SIGCONT signal to
 *                - wake it up before we block
 */
     
void job_foreground(process_session *ps, process *pp, int cont)
   {int pgid, st;
    struct termios attr;

/*    attr = pp->trm_attr; */

/* put the job into the foreground  */
    tcsetpgrp(ps->terminal, pp->pgid);
     
/* send the job a continue signal, if necessary */
    if (cont == TRUE)
       {pgid = pp->pgid;
	st = tcsetattr(ps->terminal, TCSADRAIN, &attr);
	st = kill(-pgid, SIGCONT);
	if (st < 0)
	   perror("SIGCONT");};
     
/* wait for it to report */
    job_wait(pp);
     
/* put the shell back in the foreground */
    tcsetpgrp(ps->terminal, ps->pgid);
     
/* restore the shell's terminal modes */
    tcgetattr(ps->terminal, &attr);
    tcsetattr(ps->terminal, TCSADRAIN, &ps->attr);

    ps->foreground = TRUE;

/*    pp->trm_attr = attr; */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_BACKGROUND - put PP in the background
 *                - if CONT is TRUE send the process group
 *                - a SIGCONT signal to wake it up
 */
     
void job_background(process_session *ps, process *pp, int cont)
   {int pgid, st;

/* send the job a continue signal */
    if (cont == TRUE)
       {pgid = pp->pgid;
	st = kill(-pgid, SIGCONT);
        if (st < 0)
           perror("SIGCONT");};

    ps->foreground = FALSE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GEXEC - execute a <statement> */

int gexec(char *db, int c, char **v, char **env, PFPCAL (*map)(char *s))
   {int i, nc, rv, st;
    char *s, *shell;
    statement *sl;

    shell = "/bin/csh";

/* concatenate command line arguments into one big string */
    s = NULL;
    for (i = 0; i < c; i++)
        s = append_tok(s, ' ', "%s", v[i]);

/* parse command to list of statements - ;, &&, or || */
    sl = parse_statement(s, env, shell, map);
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

/*                             VARIABLE METHOD                              */

/*--------------------------------------------------------------------------*/

/* GEXEC_VAR - function to access variables */

int gexec_var(char *db, io_mode md, FILE **fio,
	      char *name, int c, char **v)
   {int i, nc, ns, rv;
    char t[MAXLINE];
    char *p, *vr, *vl, **sa;
    static int ic = 0;

    rv = 0;
    vr = v[0];
    nc = (c > 1) ? atol(v[1]) : 1;

    if ((vr != NULL) && (md != IO_MODE_NONE))
       {sa = NULL;

        vl = getenv(vr);
	if (vl != NULL)
	   sa = lst_add(sa, vl);

	switch (md)

/* variable to stdout NC times */
	   {case IO_MODE_RO :
		 ns = lst_length(sa);
		 for (i = 0; i < ns; i++)
		     fprintf(fio[1], "%s ", sa[i]);
		 fprintf(fio[1], "\n");
		 ic++;
		 if (ic >= nc)
		    rv = 1;
		 break;

/* stdin to variable */
	    case IO_MODE_WO :
	    case IO_MODE_WD :
	         for (i = 0; rv == 0; i++)
		     {if (feof(fio[0]) == TRUE)
			 rv = 1;
		      else
			 {p = fgets(t, MAXLINE, fio[0]);
			  if (p != NULL)
			     {LAST_CHAR(t) = '\0';
			      sa = lst_add(sa, t);}
			  else if (errno == EBADF)
			     rv = 1;};};

		 vl = concatenate(t, MAXLINE, sa, " ");

		 if (IS_NULL(db) == FALSE)
		    dbset(NULL, vr, vl);
		 else
		    fprintf(fio[1], "setenv %s \"%s\" ; ", vr, vl);

		 break;

/* stdin append to variable */
	    case IO_MODE_APPEND :
	         for (i = 0; rv == 0; i++)
		     {if (feof(fio[0]) == TRUE)
			 rv = 1;
		      else
			 {p = fgets(t, MAXLINE, fio[0]);
			  if (p != NULL)
			     {LAST_CHAR(t) = '\0';
			      sa = lst_add(sa, t);
			      vl = concatenate(t, MAXLINE, sa, " ");
			      fprintf(fio[1], "%s\n", vl);}
			  else if (errno == EBADF)
			     rv = 1;};};

		 if (IS_NULL(db) == FALSE)
		    dbset(NULL, vr, vl);
		 else
		    fprintf(fio[1], "setenv %s \"%s\" ; ", vr, vl);

		 break;

	    default :
		 break;};

	free_strings(sa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GEXEC_FILE - function to access files */

int gexec_file(char *db, io_mode md, FILE **fio,
	       char *name, int c, char **v)
   {int i, rv, ev;
    char t[MAXLINE];
    char *p, *fn;
    static FILE *fp = NULL;

    rv = 0;
    fn = v[0];

    if ((fn != NULL) && (md != IO_MODE_NONE))
       {if (fp == NULL)

/* open the file */
	   {switch (md)
	       {case IO_MODE_RO :
		     fp = fopen(fn, "r");
		     if (fp == NULL)
		        fprintf(stderr, "Cannot open '%s' for reading\n", fn);
		     break;

	        case IO_MODE_WO :
		     if (file_exists(fn) == TRUE)
		        return(-1);

	        case IO_MODE_WD :
		     fp = fopen(fn, "w");
		     if (fp == NULL)
		        fprintf(stderr, "Cannot open '%s' for writing\n", fn);
		     break;

	        case IO_MODE_APPEND :
		     fp = fopen(fn, "a");
		     if (fp == NULL)
		        fprintf(stderr, "Cannot open '%s' for append\n", fn);
		     break;

	       default :
		     break;};};

	if (fp != NULL)

/* do the I/O */
	   {switch (md)

/* file to stdout */
	       {case IO_MODE_RO :
		     for (i = 0; rv == 0; i++)
		         {if (feof(fp) == TRUE)
			     rv = 1;
			  else
			     {p = fgets(t, MAXLINE, fp);
			      if (p != NULL)
				 fputs(p, fio[1]);};};
		     break;

/* stdin to file */
	        case IO_MODE_WO :
	        case IO_MODE_WD :
	        case IO_MODE_APPEND :
		     for (i = 0; rv == 0; i++)
		         {if (feof(fio[0]) == TRUE)
			     rv = 1;
			  else
			     {p  = fgets(t, MAXLINE, fio[0]);
			      ev = errno;
			      if (p != NULL)
				 fputs(p, fp);
			      else if (feof(fio[0]) == TRUE)
				 rv = 1;
			      else if (ev == EBADF)
			         rv = 2;};};

		     fflush(fp);
		     break;

	        default :
		     break;};

/*
	    if (rv == 1)
	       fclose(fp);
*/
	   };};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
