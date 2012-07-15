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
/*--------------------------------------------------------------------------*/

/* DPRDIO - print the file descriptors from an iodes PIO */

void dprdio(iodes *pio)
   {int fd, gid;
    char *io, *hnd, *knd, *dev;
    static char *std[] = {"none", "in", "out", "err"};
    static char *hn[]  = {"none", "clos", "pipe", "poll"};
    static char *kn[]  = {"none", "in", "out", "err", "bond"};
    static char *dn[]  = {"none", "pipe", "sock", "pty", "term",
                          "file", "var", "expr"};

    io  = std[pio->knd + 1];
    fd  = pio->fd;
    gid = pio->gid;
    hnd = hn[pio->hnd];
    knd = kn[pio->knd + 1];
    dev = dn[pio->dev];

    printf("%4s %3d %3d %4s %4s %4s\n",
	   io, fd, gid, hnd, knd, dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRPIO - print the file descriptors from a process PP */

void dprpio(char *tag, process *pp)
   {int i;
    iodes *pio;

    printf("%s\n", tag);
    printf("Unit  fd gid  hnd  knd  dev\n");
    for (i = 0; i < N_IO_CHANNELS; i++)
        {pio = pp->io + i;
	 dprdio(pio);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRGIO - print the file descriptors from the processes of a group */

void dprgio(char *tag, int n, process **pa, process **ca)
   {int i;
    char *hnd;
    process *pp, *cp;

    printf("----------------------------------------------------\n");
    printf("%s\n\n", tag);

    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];
	 if ((pp != NULL) && (cp != NULL))
	    {printf("command: %s\n", pp->cmd);
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
	     printf("   stdin:  %3d(%s) -> %3d\n",
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
	     printf("   stdout: %3d(%s) <- %3d\n",
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
	     printf("   stderr: %3d(%s) <- %3d\n",
		    pp->io[2].fd, hnd, cp->io[2].fd);

	     printf("\n");};};

    printf("----------------------------------------------------\n");

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
	       {case 'b' :
		     knd = IO_STD_BOND;
		     break;
		case 'e' :
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
	if (strchr("0123456789", cd) != NULL)
	   {dev = IO_DEV_PTY;
	    dev = IO_DEV_SOCKET;
	    dev = IO_DEV_PIPE;
	    nc--;};

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
	       gid = pos - 1;};

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

    _init_iodes(1, &io);

/* parse out the redirect related specifications */
    rv = parse_redirect(&io, pp, i);

/* add the redirect specifications to the filedes */
    switch (io.knd)
       {case IO_STD_IN :
	     io.fd = pp->io[0].fd;
	     pp->io[0] = io;
	     redir_io(pp->io, N_IO_CH, 0, &io);
	     break;
        case IO_STD_OUT :
	     io.fd = pp->io[1].fd;
	     pp->io[1] = io;
	     redir_io(pp->io, N_IO_CH, 1, &io);
	     break;
        case IO_STD_ERR :
	     io.fd = pp->io[2].fd;
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

    pn->io[pk].hnd = IO_HND_PIPE;
    fd             = pn->io[pk].fd;

    close(cn->io[ck].fd);
    cn->io[ck].hnd = IO_HND_PIPE;
    cn->io[ck].fd  = fd;

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH_FD - register the KND descriptor of PN to be polled */

static int watch_fd(process *pn, io_kind pk)
   {int fd;
    io_hand hnd;

    fd  = pn->io[pk].fd;
    hnd = pn->io[pk].hnd;

#if 1
    if ((fd > 0) && (hnd != IO_HND_PIPE))
#else
    io_device dev;
    dev = pn->io[pk].dev;
    if ((fd > 0) && (dev == IO_DEV_TERM) && (hnd != IO_HND_PIPE))
#endif

       {_awatch_fd(pn, pk, pn->ip);

	pn->io[pk].hnd = IO_HND_POLL;};

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
   {int i, gi, nm, n;
    process *pp, *cp, *pt;
    process **pa, **ca;
    io_device dv;

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

/* reconnect terminal process output to first process */
	transfer_fd(pa[0], IO_STD_OUT, pt, IO_STD_OUT);

/* close all other parent to child lines */
	for (i = 1; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

	     close(pp->io[1].fd);
	     close(cp->io[0].fd);
/*
	     pp->io[1].fd = -2;
	     cp->io[0].fd = -2;
*/
	     pp->io[1].hnd = IO_HND_CLOSE;
	     cp->io[0].hnd = IO_HND_CLOSE;};

/* connect all non-terminal children output to appropriate child input */
	for (i = 0; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

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

/* connect all non-terminal children input to appropriate child output */
	for (i = 1; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

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
		     fd             = ca[gi]->io[IO_STD_OUT].fd;

		     pp->io[IO_STD_OUT].hnd = IO_HND_PIPE;
		     pp->io[IO_STD_OUT].fd  = fd;};

		 if (ca[gi]->io[IO_STD_ERR].gid == i)
		    {ca[gi]->io[IO_STD_ERR].hnd = IO_HND_PIPE;
		     fd             = ca[gi]->io[IO_STD_ERR].fd;

		     pp->io[IO_STD_IN].hnd = IO_HND_PIPE;
		     pp->io[IO_STD_IN].fd  = fd;};};};};


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

int _pgrp_accept(int fd, process *pp, char *s)
   {int rv;

    rv = fputs(s, stdout);
    rv = (rv >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_REJECT - reject messages read from PP */

int _pgrp_reject(int fd, process *pp, char *s)
   {int rv;

    rv = TRUE;
    printf("reject> %d %s (%s)\n", fd, pp->cmd, s);

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

    ASSERT(st != NULL);

/* drain text from the job - apoll will no longer check it after this */
    rv = job_read(pp->io[IO_STD_IN].fd, pp, pp->accept);
    ASSERT(rv == 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PGRP_TTY - check the TTY
 *           - return TRUE if we are done
 */

int _pgrp_tty(char *tag)
   {int i, n, np, rv;
    process *pp;


/*    printf("tty method pid=%d  fd=%d\n", getpid(), fileno(stdin)); */
    rv = FALSE;

    np = stck.np;

/* count the running jobs */
    for (n = 0, i = 0; i < np; i++)
        {pp = stck.proc[i];
	 n += job_running(pp);};

    rv = (n != 0);
/*
    if (rv == TRUE)
       printf("all done\n");
*/
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

/* REGISTER_IO_PGRP - setup to poll all remaining process side descriptors */

void register_io_pgrp(process_group *pg)
   {int i, np;
    process *pp, **pa;

    np = pg->np;
    pa = pg->parents;

    for (i = 0; i < np; i++)
        {pp = pa[i];
	 watch_fd(pp, IO_STD_IN);
	 watch_fd(pp, IO_STD_OUT);
	 watch_fd(pp, IO_STD_ERR);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_PGRP - run the process group PG */

static int run_pgrp(statement *s)
   {int i, np, rv, fd, tc;
    int *st;
    char t[MAXLINE];
    process *pp;
    process_group *pg;

    rv = -1;

    if (s != NULL)
       {pg = s->pg;
	np = s->np;

	st = MAKE_N(int, np);

	asetup(np, 1);

	register_io_pgrp(pg);
dprgio("run_pgrp", np, pg->parents, pg->children);

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
	ASSERT(tc >= 0);

/* close out the jobs */
	afin(_pgrp_fin);

/* find weak scalar return value */
	rv = 0;
	for (i = 0; i < np; i++)
	    rv |= st[i];

/* export group status */
        snprintf(t, MAXLINE, "set xstatus = (");
	for (i = 0; i < np; i++)
	    vstrcat(t, MAXLINE, " %d", st[i]);
        vstrcat(t, MAXLINE, " )");
        printf("%s\n", t);

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
