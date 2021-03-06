/*
 * LIBSRV.C - simple asynchronous server and client routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBSRV

# define LIBSRV

# include "common.h"
# include "libpsh.c"
# include "libio.c"
# include "libsock.c"

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

#ifndef S_SPLINT_S
#define SLOG(_s, ...)                                                       \
    {if ((_s)->slog != NULL)                                                \
        (_s)->slog((_s), __VA_ARGS__);}  
#endif

typedef struct s_srvdes srvdes;
typedef struct s_svr_session svr_session;

struct s_srvdes
   {void *a;
    void (*setup)(srvdes *sv, int *ptmax, int *pdt);
    char **(*process)(srvdes *sv, char *s);
    void (*slog)(srvdes *sv, int lvl, const char *fmt, ...);};

struct s_svr_session
   {int debug;
    int init;
    int auth;
    int daemon;
    client *cl;};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

#define EOM     "++ok++"

static svr_session
  svs = { FALSE, FALSE, FALSE, FALSE, NULL };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NAME_CONN - derive the name of the connection file from ROOT */

char *name_conn(const char *root)
   {static char fname[BFLRG];

    snprintf(fname, BFLRG, "%s.conn", root);

    return(fname);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NAME_LOG - make conventional LOG name from ROOT
 *          - .../foo -> .../foo.log
 */

char *name_log(const char *root)
   {char *p;
    static char lg[BFLRG];

    p = NULL;

    if (root == NULL)
       root = cgetenv(TRUE, "PERDB_PATH");

    if (root != NULL)
       {snprintf(lg, BFLRG, "%s.log", root);
	p = lg;};

    return(p);}

/*--------------------------------------------------------------------------*/

/*                           I/O CONNECTION ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* CL_LOGGER - log messages for the client CL */

void cl_logger(client *cl, int lvl, const char *fmt, ...)
   {char s[BFLRG];
    char *wh, *flog;
    const char *root;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    wh   = C_OR_S(cl->type == CLIENT);
    root = cl->root;
    flog = name_log(root);

    log_activity(flog, svs.debug, lvl, wh, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMM_READY - return TRUE iff there are bytes ready to be read on CL */

int comm_ready(client *cl, int to)
   {int ok;
    struct pollfd fds[1];

    ok = FALSE;
    if ((cl != NULL) && (cl->cfd >= 0))
       {fds[0].fd      = cl->cfd;
	fds[0].events  = (POLLIN | POLLPRI);
	fds[0].revents = 0;

	ok = poll(fds, 1, to);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _COMM_READ_WRK - read worker routine */

static int _comm_read_wrk(client *cl, char *t, int nt, int to)
   {int nb;

#ifdef IO_ALARM
/*    nsigaction(NULL, SIGALRM, sigtimeout, SA_RESTART, -1); */
    signal(SIGALRM, sigtimeout);

    alarm(to);

    if (setjmp(cpu) == 0)
       nb = read_sock(cl, t, nt);
    else
       nb = -1;

    alarm(0);
#else
    nb = read_sock(cl, t, nt);
#endif

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _COMM_WRITE_WRK - write worker routine */

static int _comm_write_wrk(client *cl, char *t, int nt, int to)
   {int nb;

#ifdef IO_ALARM
/*    nsigaction(NULL, SIGALRM, sigtimeout, SA_RESTART, -1); */
    signal(SIGALRM, sigtimeout);

    alarm(to);

    if (setjmp(cpu) == 0)
       nb = write_sock(cl, t, nt);
    else
       nb = -1;

    alarm(0);
#else
    nb = write_sock(cl, t, nt);
#endif

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMM_READ - read from CL into S
 *           - quit if it hasn't heard anything in TO seconds
 *           - return -2 if authentication is denied
 */

int comm_read(client *cl, char *s, int nc, int to)
   {int nb, nt, ok;
    char *p;

    if (cl->type == SERVER)
       nb = _comm_read_wrk(cl, s, nc, to);
    else
       {int nz, nzx, tl;
	char u[BFVLG];

/* there is about a 6 second delay down in _comm_read_wrk due
 * to retry logic so only give it a few chances here
 */
	nzx = 4;
	tl  = 0;

	for (nz = 0, nb = 0; (nb == 0) && (nz < nzx); )
	    {nb = ring_pop(&cl->ior, s, nc, '\0');
	     if (nb == 0)
	        {memset(u, 0, BFVLG);
		 nt = _comm_read_wrk(cl, u, BFVLG, tl);
		 ok = ring_push(&cl->ior, u, nt);
	         if (nt <= 0)
		    nz++;
	         else
		    nz = 0;};};

	if ((nb == 0) && (nz >= nzx))
	   return(-1);};

    if (cl->cauth != NULL)
       ok = cl->cauth(cl, nb, s, NULL);
    else
       ok = TRUE;

    nb = strlen(s);

    if (ok == FALSE)
       {p = s + nb - 4;
	if (strcmp(p, "fin:") == 0)
	   nstrncpy(s, nc, p, -1);
	else
	   nstrncpy(s, nc, "reject:", -1);}

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMM_WRITE - write S to CL
 *            - quit if it hasn't heard anything in TO seconds
 *            - return -2 if authentication is denied
 */

int comm_write(client *cl, char *s, int nc, int to)
   {int nb, nk, nt, ok;
    char ans[N_AKEY+1];
    char *t;

    if (cl->cauth != NULL)
       ok = cl->cauth(cl, 0, NULL, ans);
    else
       ok = TRUE;

    if (ok == FALSE)
       nb = -2;

    else
       {if (nc <= 0)
	   nc = strlen(s);

	nk = (svs.auth == TRUE) ? cl->nkey : 0;
	nt = nc + nk + 7;
	t  = MAKE_N(char, nt);
	if (svs.auth == TRUE)
	   snprintf(t, nt, "auth:%s %s", ans, s);
	else
	   nstrncpy(t, nt, s, -1);
	nt = strlen(t) + 1;

	nb  = _comm_write_wrk(cl, t, nt, to);
	nb -= nk;

	FREE(t);};

    return(nb);}

/*--------------------------------------------------------------------------*/

/*                            SERVER SIDE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _CHECK_FD - verify/log fd set in SV */

void _check_fd(srvdes *sv)
   {int fd, nfd;
    char s[BFLRG];
    fd_set rfs;
    client *cl;
    connection *srv;

    cl  = (client *) sv->a;
    srv = cl->scon;
    rfs = srv->afs;

    s[0] = '\0';
    nfd = FD_SETSIZE;
    for (fd = 0; fd < nfd; ++fd)
        {if (FD_ISSET(fd, &rfs))
            vstrcat(s, BFLRG, " %d", fd);};

    SLOG(sv, 4, "monitoring: %s", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_FD - add FD to the set in SV */

static void add_fd(srvdes *sv, int fd)
   {client *cl;
    connection *srv;

    cl  = (client *) sv->a;
    srv = cl->scon;

    FD_SET(fd, &srv->afs);
    SLOG(sv, 1, "add fd %d", fd);

    _check_fd(sv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMOVE_FD - add FD to the set in SV */

void remove_fd(srvdes *sv, int fd)
   {client *cl;
    connection *srv;

    cl  = (client *) sv->a;
    srv = cl->scon;

    FD_CLR(fd, &srv->afs);
    SLOG(sv, 1, "remove fd %d", fd);

    _check_fd(sv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NEW_CONNECTION - make a new connection on SFD and add it to AFS */

void _new_connection(srvdes *sv)
   {int fd;
    client *cl;
    connection *srv;

    cl  = (client *) sv->a;
    srv = cl->scon;

    fd = tcp_accept_connection(srv->sfd, srv->sck, TRUE);
    if (fd > 0)
       add_fd(sv, fd);
    else
       {close_safe(fd);
	SLOG(sv, 1, "accept error - %s (%d)", strerror(errno), errno);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _TERM_CONNECTION - finish off client connection CL */

static void _term_connection(srvdes *sv)
   {int fd;
    client *cl;

    cl = (client *) sv->a;
    fd = cl->cfd;

    remove_fd(sv, fd);

    cl->cfd = connect_close(fd, cl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PROCCESS_ACT - process transactions in the server SV
 *               - process input on client connection CL
 *               - return -1 on error
 *               -         0 on end of connection
 *               -         1 if otherwise successful
 */

int _process_act(srvdes *sv, int fd)
   {int rv, nb, to;
    char s[BFLRG];
    char **val;
    client *cl;

    cl = (client *) sv->a;
    cl->cfd = fd;

    to = 60;
    rv = 1;

    memset(s, 0, BFLRG);

    nb = comm_read(cl, s, BFLRG, to);

/* decide on the action to take */
    if (nb <= 0)
       rv = -1;

    else
       {val = NULL;

/* request is not properly authenticated */
	if (strncmp(s, "reject:", 7) == 0)
	   val = lst_push(val, "rejected");

/* client is exiting */
	else if (strncmp(s, "fin:", 4) == 0)
	   _term_connection(sv);

/* quit session */
	else if (strncmp(s, "quit:", 5) == 0)
	   {rv  = 0;
	    val = lst_push(val, "done");}

	else
	   val = sv->process(sv, s);

	if (val != NULL)
	   {int i, nv;

	    nv = lst_length(val);
	    for (i = 0; i < nv; i++)
	        nb = comm_write(cl, val[i], 0, 10);
	    nb = comm_write(cl, EOM, 0, 10);

	    free_strings(val);

	    if (rv < 1)
	       unlink(cl->fcon);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASYNC_SERVER - run a fully asynchronous server
 *              - return value of TRUE means that it would be
 *              - safe for the caller to write the database
 *              - otherwise it should not do so
 */

int async_server(srvdes *sv)
   {int fd, nr, nfd, ok, rv;
    fd_set rfs;
    client *cl;
    connection *srv;

    rv  = FALSE;
    cl  = (client *) sv->a;
    srv = cl->scon;

    if (listen(srv->sfd, 5) >= 0)
       {int ng, nb, nbmax, dt, tmax;
        struct timeval t;

/* set idle timeout and idle interval
 * if no input comes in within the idle timeout window stop and return
 * have the select timeout at the idle interval
 */
	sv->setup(sv, &tmax, &dt);

/* maximum number of consecutive 0 length reads
 * indicating dropped connection
 */
	nb    = 0;
	nbmax = 10;

	ng = 0;
	for (ok = 1; (ok == 1) && (ng < tmax) && (nb < nbmax); )
	    {_check_fd(sv);

/* timeout the select every DT seconds */
	     t.tv_sec  = dt;
	     t.tv_usec = 0;

	     rfs = srv->afs;
	     nr  = select(FD_SETSIZE, &rfs, NULL, NULL, &t);
	     if (nr > 0)
	        {nfd = FD_SETSIZE;
		 for (fd = 0; (fd < nfd) && (ok == 1); ++fd)
		     {if (FD_ISSET(fd, &rfs))
			 {SLOG(sv, 4, "data available on %d (server %d)",
			       fd, srv->sfd);

/* new connection request */
			  if (fd == srv->sfd)
			     _new_connection(sv);

/* data arriving on an existing connection */
			  else
			     {ok = _process_act(sv, fd);
                              if (ok == -1)
				 {ok = 1;
				  remove_fd(sv, fd);
				  nb++;}
			      else
				 nb = 0;

			      sched_yield();};};};
		  ng = 0;}
	     else
	        ng += dt;

/* if the connection file goes away - quit immediately */
	     if ((ok != 0) && (find_conn(cl->root, -1) == NULL))
	        ok = 2;};

	if (ok == 2)
	   {SLOG(sv, 1, "server exit by missing connection file");}
	else if (ok == 0)
	   {rv = TRUE;
	    SLOG(sv, 1, "server exit by command");}
	else if (ng >= tmax)
	   {rv = TRUE;
	    SLOG(sv, 1, "server exit by time: %d >= %d", ng, tmax);}
	else if (nb >= nbmax)
	   {SLOG(sv, 1, "server exit by failed reads: %d >= %d",
		 nb, nbmax);};}

    else
       {SLOG(sv, 1, "server exit by launch failure (%s - %d)",
	     strerror(errno), errno);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            CLIENT SIDE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* SIGTIMEOUT - handle signals meant to timeout session */

#ifdef IO_ALARM

static jmp_buf
 cpu;

static void sigtimeout(int sig)
   {

    longjmp(cpu, 1);

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_CLIENT - initialize and return a client connection instance */

client *make_client(ckind type, int port, int auth, const char *root, 
		    void (*clg)(client *cl, int lvl, const char *fmt, ...),
                    int (*cauth)(client *cl, int nc, char *ans, char *res))
   {char *fcon, *t;
    client *cl;
    static connection global_srv;

    cl = MAKE(client);
    if (cl != NULL)
       {t = name_conn(root);
	fcon = (t == NULL) ? NULL : STRSAVE(t);

	cl->cfd    = -1;
        cl->port   = port;
        cl->auth   = auth;
	cl->nkey   = 0;
	cl->key    = NULL;
	cl->root   = root;
	cl->fcon   = fcon;
	cl->type   = type;
        cl->a      = NULL;
	cl->scon   = &global_srv;
        cl->clog   = clg;
        cl->cauth  = cauth;

	ring_init(&cl->ior, BFVLG);

	CLOG(cl, 1, "----- start client -----");};

    return(cl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_CLIENT - release a client connection instance */

void free_client(client *cl)
   {

    if (cl != NULL)
       {if (cl->type == CLIENT)
	   comm_write(cl, "fin:", 0, 10);

	if (cl->cfd >= 0)
	   cl->cfd = connect_close(cl->cfd, cl);

	CLOG(cl, 1, "----- end client -----");

	cl->clog = NULL;

	FREE(cl->fcon);
	FREE(cl->key);
	FREE(cl);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLIENT_EX - do a transaction request REQ from the client side */

char **client_ex(client *cl, char *req)
   {int nb, ne, nx;
    char **p;
    static char s[BFLRG];

    p = NULL;

    CLOG(cl, 1, "begin request |%s|", req);

/* send the request */
    nb = comm_write(cl, req, 0, 10);

/* get the reply */
    if (nb > 0)
       {int nc, to, ok;
	char *t;

	ne = 0;
	nx = (cl->type == SERVER) ? 100 : 2;
	to = 4;
	for (ok = TRUE; ok == TRUE; )
	    {nb = comm_read(cl, s, BFLRG, to);
	     ne = (nb < 0) ? ne+1 : 0;

/* if more than NX consecutive read failures bail */
	     if (ne >= nx)
	        {p  = NULL;
		 ok = FALSE;}

	     else
	        {for (t = s; nb > 0; t += nc, nb -= nc)
		     {if (strcmp(t, EOM) == 0)
			 {ok = FALSE;
			  break;}
		      else if (strcmp(t, "reject:") == 0)
			 {p  = lst_push(p, "rejected");
			  ok = FALSE;
			  break;}
		      else
			 p = lst_push(p, t);

		      nc = strlen(t) + 1;};

		 if (ok == TRUE)
		    nsleep(0);};};};

    CLOG(cl, 1, "end request");

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef EOM

# endif
#endif
