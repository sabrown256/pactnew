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
 *           - <io-kind>   := 'i' | 'o' | 'e' | 'b'
 *           - <io-id>     := +<n> | -<n> | <n>
 *           - <n>         := unsigned integer value
 *           - 
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
typedef char *(*PFPChar)(char *x);
typedef int (*PFPCAL)(char *db, io_mode m, FILE **fp, int c, char **v);

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
			  "status", "rsrc"};
    static char *dn[]  = {"none", "pipe", "sock", "pty", "term", "fnc"};

    nc = N_IO_CHANNELS*np;

    fprintf(stderr, "dbg> -----------------------------------------------\n");
    fprintf(stderr, "dbg> %d  %s  ioc\n", getpid(), tag);
    fprintf(stderr, "dbg> %d processes   %d connections\n", np, nc);

    fprintf(stderr, "dbg> Unit       fd gid  hnd  knd  dev #\n");
    for (i = 0; i < nc; i++)
        {pioc = ioc + i;

	 io = i % N_IO_CHANNELS;
	 if (io == 0)
	    fprintf(stderr, "dbg>\n");

	 fd  = pioc->in.fd;
	 gid = pioc->in.gid;
	 hnd = hn[pioc->in.hnd];
	 knd = kn[pioc->in.knd + 1];
	 dev = dn[pioc->in.dev];

	 if (pioc->in.dev == IO_DEV_PIPE)
	    fprintf(stderr, "dbg> %2d  read  %3d %3d %4s %4s %4s %d\n",
		    io, fd, gid, hnd, knd, dev, gid);
	 else
	    fprintf(stderr, "dbg> %2d  read  %3d %3d %4s %4s %4s\n",
		    io, fd, gid, hnd, knd, dev);

	 fd  = pioc->out.fd;
	 gid = pioc->out.gid;
	 hnd = hn[pioc->out.hnd];
	 knd = kn[pioc->out.knd + 1];
	 dev = dn[pioc->out.dev];

	 if (pioc->out.dev == IO_DEV_PIPE)
	    fprintf(stderr, "dbg>     write %3d %3d %4s %4s %4s %d\n",
		    fd, gid, hnd, knd, dev, gid);
	 else
	    fprintf(stderr, "dbg>     write %3d %3d %4s %4s %4s\n",
		    fd, gid, hnd, knd, dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRGRP - diagnostic print of process_group tasks */

void dprgrp(char *tag, process_group *pg)
   {int i, n;
    process *pp, *cp;

    n = pg->np;

    fprintf(stderr, "dbg> -----------------------------------------------\n");
    fprintf(stderr, "dbg> %d  %s  group\n", getpid(), tag);
    fprintf(stderr, "dbg> %d processes\n", n);

    fprintf(stderr, "dbg> Unit  fd gid  hnd  knd  dev\n");
    for (i = 0; i < n; i++)
        {pp = pg->parents[i];
	 cp = pg->children[i];

	 dprdio("child",  cp->io + IO_STD_IN);
	 dprdio("parent", pp->io + IO_STD_OUT);

	 dprdio("parent", pp->io + IO_STD_IN);
	 dprdio("child",  cp->io + IO_STD_OUT);

	 dprdio("parent", pp->io + IO_STD_ERR);
	 dprdio("child",  cp->io + IO_STD_ERR);

	 fprintf(stderr, "dbg>\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRGIO - print the file descriptors from the processes of a group */

void dprgio(char *tag, int n, process **pa, process **ca)
   {int i;
    char *hnd;
    process *pp, *cp;

    fprintf(stderr, "dbg> -----------------------------------------------\n");
    fprintf(stderr, "dbg> %s\ndbg>\n", tag);

    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];
	 if ((pp != NULL) && (cp != NULL))
	    {fprintf(stderr, "dbg> command: %s\n", pp->cmd);
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
	     fprintf(stderr, "dbg>    stdin:  %3d(%s) -> %3d\n",
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
	     fprintf(stderr, "dbg>    stdout: %3d(%s) <- %3d\n",
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
	     fprintf(stderr, "dbg>    stderr: %3d(%s) <- %3d\n",
		     pp->io[2].fd, hnd, cp->io[2].fd);

	     fprintf(stderr, "dbg>\n");};};

    fprintf(stderr, "dbg> -----------------------------------------------\n");

    return;}

/*--------------------------------------------------------------------------*/

/*                               GROUP ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* REDIRECT_IO - modify the I/O channel of PP according to IO */

void redirect_io(process *pp, iodes *io)
   {iodes *pio;

/* add the redirect specifications to the filedes */
    switch (io->knd)
       {case IO_STD_IN :
	     pio    = &pp->ioc[io->knd].in;
	     io->fd = pio->fd;
	     *pio   = *io;

#ifdef OLDWAY
	     pio    = pp->io + io->knd;
	     io->fd = pio->fd;
	     *pio   = *io;
#endif
	     break;

        case IO_STD_OUT :
        case IO_STD_ERR :
	     pio    = &pp->ioc[io->knd].out;
	     io->fd = pio->fd;
	     *pio   = *io;

#ifdef OLDWAY
	     pio    = pp->io + io->knd;
	     io->fd = pio->fd;
	     *pio   = *io;
#endif
	     break;

        case IO_STD_BOND :
	     pio    = &pp->ioc[IO_STD_ERR].out;
	     io->fd = pio->fd;
	     *pio   = *io;
	     pio    = &pp->ioc[IO_STD_OUT].out;
	     io->fd = pio->fd;
	     *pio   = *io;

#ifdef OLDWAY
	     pio    = pp->io + IO_STD_ERR;
	     io->fd = pio->fd;
	     *pio   = *io;
	     pio    = pp->io + IO_STD_OUT;
	     io->fd = pio->fd;
	     *pio   = *io;
#endif
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

/* determine the device and mode */
    if (strchr("ioeb", t[0]) != NULL)
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

	p   = t + nc;
	pos = (IS_NULL(p) == TRUE) ? 1 : atoi(p);
	if (rel == TRUE)
	   aip += pos;
	else
	   aip = pos - 1;};

    _init_iodes(1, &ca);

    ca.knd  = aknd;
    ca.dev  = dev;
    ca.gid  = aip;
    ca.mode = amd;

    redirect_io(pg->parents[bip], &ca);

    _init_iodes(1, &cb);

    cb.knd  = bknd;
    cb.dev  = dev;
    cb.gid  = bip;
    cb.mode = bmd;

    redirect_io(pg->parents[aip], &cb);

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

#ifndef STRONG_FUNCTIONS
    ta = pp->arg;
    if ((strcmp(ta[0], "gexec") == 0) && (strcmp(ta[1], "-p") == 0))
       {pp->isfunc = TRUE;
	for (i = 0; i < N_IO_CHANNELS; i++)
	    pp->io[i].dev = IO_DEV_FNC;};
#endif

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

#ifdef OLDWAY

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
	          break;};
#endif
        };

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRANSFER_FD - transfer the KND descriptor of PN to CN */

static int transfer_fd(process *pn, io_kind pk, process *cn, io_kind ck)
   {int fd;
    FILE *fp;

    fd = pn->io[pk].fd;
    fp = pn->io[pk].fp;

    pn->io[pk].hnd = IO_HND_PIPE;
    pn->io[pk].fp  = NULL;
#ifdef STRONG_FUNCTIONS
    pn->io[pk].fd  = -100;
#endif

/* GOTCHA: we can close something we need in the bonded case
 * or leak descriptors in the other cases
 * better bookkeeping is needed here
    _fd_close(cn->io[ck].fd);
 */
    cn->io[ck].hnd = IO_HND_PIPE;
    cn->io[ck].fp  = fp;
    cn->io[ck].fd  = fd;

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

#ifndef NEWWAY
    fd  = pn->io[pk].fd;
    hnd = pn->io[pk].hnd;
#endif
    if ((fd > 0) && (hnd != IO_HND_PIPE))
       {_awatch_fd(pn, pk, ip);

	_dbg(1, "watch fd=%d on %d @ %d", fd, ip, pk);

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

/*    _fd_close(pib->in.fd); */
    pib->in.fd   = pia->in.fd;
    pib->in.dev  = IO_DEV_PIPE;
    pib->in.hnd  = IO_HND_PIPE;
    pib->in.gid  = ia;
/*
    pib->out.dev = IO_DEV_PIPE;
    pib->out.hnd = IO_HND_PIPE;
    pib->out.gid = ia;
*/
/*    _fd_close(pib->out.fd); */
    pib->out.fd  = pia->out.fd;
    pia->out.dev = IO_DEV_PIPE;
    pia->out.hnd = IO_HND_PIPE;
    pia->out.gid = ib;
    pia->in.dev  = IO_DEV_PIPE;
    pia->in.hnd  = IO_HND_PIPE;
    pia->in.gid  = ib;

    return;}

/*--------------------------------------------------------------------------*/

#ifdef OLDWAY

/*--------------------------------------------------------------------------*/

/* CLOSE_PARENT_CHILD - close all non-terminal parent to child lines */

void close_parent_child(int n, process **pa, process **ca)
   {int i, nm;
    process *pp, *cp;

    nm = n - 1;

    for (i = 1; i < nm; i++)
        {pp = pa[i];
	 cp = ca[i];

	 _fd_close(pp->io[1].fd);
	 _fd_close(cp->io[0].fd);

	 pp->io[1].hnd = IO_HND_CLOSE;
	 cp->io[0].hnd = IO_HND_CLOSE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CHILD_OUT_IN - connect all non-terminal child output
 *                      - to appropriate child input
 */

void connect_child_out_in(int n, process **pa, process **ca)
   {int i, gi, nm;
    io_device dv;
    process *pp;

    nm = n - 1;

    for (i = 0; i < nm; i++)
        {pp = pa[i];

/* stdout */
	 gi = pp->io[IO_STD_OUT].gid;
	 dv = pp->io[IO_STD_OUT].dev;

/* default to the next process in line */
	 if ((dv == IO_DEV_PIPE) && (gi == -1))
	    gi = i + 1;

	 if (gi >= 0)
	    transfer_fd(pa[i], IO_STD_IN, ca[gi], IO_STD_IN);

/* stderr */
	 gi = pp->io[IO_STD_ERR].gid;
	 dv = pp->io[IO_STD_ERR].dev;

/* default to the next process in line */
	 if ((dv == IO_DEV_PIPE) && (gi == -1))
	    gi = i + 1;

	 if (gi >= 0)
	    transfer_fd(pa[i], IO_STD_ERR, ca[gi], IO_STD_IN);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CHILD_IN_OUT - connect all non-terminal child input to
 *                      - appropriate child output
 */

void connect_child_in_out(int n, process **pa, process **ca)
   {int i, gi, nm;
    io_device dv;
    process *pp;

    nm = n - 1;

    for (i = 1; i < nm; i++)
        {pp = pa[i];

/* stdin */
	 gi = pp->io[IO_STD_IN].gid;
	 dv = pp->io[IO_STD_IN].dev;

/* default to the previous process in line */
	 if ((dv == IO_DEV_PIPE) && (gi == -1))
	    gi = i - 1;

	 if (gi >= 0)
	    {int fd;

	     if (ca[gi]->io[IO_STD_OUT].gid == i)
	        {ca[gi]->io[IO_STD_OUT].hnd = IO_HND_PIPE;

		  fd = ca[gi]->io[IO_STD_OUT].fd;

		  pp->io[IO_STD_OUT].hnd = IO_HND_PIPE;
		  pp->io[IO_STD_OUT].fd  = fd;};

	     if (ca[gi]->io[IO_STD_ERR].gid == i)
	        {ca[gi]->io[IO_STD_ERR].hnd = IO_HND_PIPE;

		 fd = ca[gi]->io[IO_STD_ERR].fd;

		 pp->io[IO_STD_IN].hnd = IO_HND_PIPE;
		 pp->io[IO_STD_IN].fd  = fd;};};};

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* TRANSFER_FNC_CHILD - transfer all function call I/O from child
 *                    - to parent and free child
 */

void transfer_fnc_child(int n, process **pa, process **ca)
   {

#ifdef STRONG_FUNCTIONS
    int i, io;
    char **ta;
    iodes *pio;
    process *pp, *cp;

    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];

	 ta = pp->arg;
	 if ((strcmp(ta[0], "gexec") == 0) && (strcmp(ta[1], "-p") == 0))
	    {pp->isfunc = TRUE;
	     for (i = 0; i < N_IO_CHANNELS; i++)
	         {pio = pp->io + i;
/*		  if (pio->gid != -1) */
		     pio->dev = IO_DEV_FNC;};};

	 if (pp->isfunc == TRUE)
	    {for (io = 0; io < N_IO_CHANNELS; io++)
		 transfer_fd(cp, io, pp, io);
	     _job_free(cp);

	     ca[i] = NULL;};};
#endif

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

#ifdef OLDWAY

/* reconnect terminal process output to first process */
	transfer_fd(pa[0], IO_STD_OUT, pt, IO_STD_OUT);

/* close all other parent to child lines */
	close_parent_child(n, pa, ca);

/* connect all non-terminal children output to appropriate child input */
	connect_child_out_in(n, pa, ca);

/* connect all non-terminal children input to appropriate child output */
	connect_child_in_out(n, pa, ca);

/* transfer all function call I/O to parent and free child */
	transfer_fnc_child(n, pa, ca);
#endif
    };

    if (dbg_level & 1)
       {dprioc("reconnect_pgrp", pg->np, pg->ioc);
	dprgrp("reconnect_pgrp", pg);};

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
#if 0
	 else if (strpbrk(t, "[]()@$*`") != NULL)
#else
	 else if (strpbrk(t, "[]()$*`") != NULL)
#endif
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

	_dbg(2, "wait %d with status %s (%d) (alive %d)",
	     pp->id, st, pp->reason, job_alive(pp));};

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
    char *db, *fn, **v;
    FILE *fp[2];
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
	db = NULL;

        pa = pg->parents;
	for (ip = 0; ip < ne; ip++)
	    {pp = pa[ip];

/* if the current process is a function execute it */
	     for (io = 0; io < N_IO_CHANNELS; io++)
	         {pio = pp->io + io;
		  md  = pio->mode;
		  dv  = pio->dev;
		  hnd = pio->hnd;
		  if ((hnd == IO_HND_FNC) || (dv == IO_DEV_FNC))
		     {v  = pp->arg;
		      c  = lst_length(pp->arg);
		      fn = v[2];
		      f  = map(fn);
		      if (f != NULL)
			 {v += 4;
			  c -= 4;

			  if (io == IO_STD_IN)
			     {fp[0] = _io_file_ptr(pp, io);
			      fp[1] = _io_file_ptr(pp, IO_STD_OUT);
			      md    = IO_MODE_WD;}
			  else
			     {fp[0] = _io_file_ptr(pp, IO_STD_IN);
			      fp[1] = _io_file_ptr(pp, io);
			      md    = IO_MODE_RO;};

			  _dbg(2, "call '%s' (%d)", fn, i);

			  rv = f(db, md, fp, c, v);
			  pp->reason = rv;
			  ASSERT(rv >= -1);};};};};};

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
   {int i, ne, np, rv, tc, fd;
    char vl[MAXLINE];
    char *db;
    process *pp, *cp;
    process_group *pg;

    rv = -1;

    if (s != NULL)
       {pg = s->pg;
	ne = s->ne;

	s->st = MAKE_N(int, ne);

#ifdef STRONG_FUNCTIONS
	for (i = 0; i < ne; i++)
	    {pp = pg->parents[i];
	     if (pp->isfunc == TRUE)
                 s->nf++;
	     else
                 s->np++;};
#else
        s->np = s->ne;
#endif

	np = s->np;

	asetup(np, 1);

	register_io_pgrp(pg);

	if (dbg_level & 1)
	   dprgio("run_pgrp", np, pg->parents, pg->children);

/* launch the jobs - io_data passed to accept, reject, and wait methods */
	for (i = 0; i < np; i++)
	    {pp = pg->parents[i];
	     cp = pg->children[i];

#ifdef STRONG_FUNCTIONS
	     if (pp->isfunc == FALSE)
#endif
	        pp = _job_fork(pp, cp, NULL, "rw", s);

	     _dbg(2, "launch %d (%d,%d,%d)       %s",
		  pp->id,
		  pp->io[0].fd, pp->io[1].fd, pp->io[2].fd,
		  pp->cmd);

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
	if (db != NULL)
	   dbset(NULL, "gstatus", vl);
	else
	   printf("setenv gstatus \"%s\"\n", vl);

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
	   fprintf(stderr,
		   "Couldn't put the session in its own process group");

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
/*--------------------------------------------------------------------------*/

#endif
