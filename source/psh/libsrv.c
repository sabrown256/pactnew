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

#define LIBSRV

#include "common.h"
#include "libpsh.c"
#include "libsock.c"

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
    int (*process)(srvdes *sv, int fd);
    void (*slog)(srvdes *sv, int lvl, char *fmt, ...);};

struct s_svr_session
   {int debug;
    int init;
    int auth;
    int daemon;
    client *cl;};

static svr_session
  svs = { FALSE, FALSE, FALSE, FALSE, NULL };

/*--------------------------------------------------------------------------*/

/*                            SERVER SIDE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _CHECK_FD - verify/log fd set in SV */

static void _check_fd(srvdes *sv)
   {int fd;
    char s[MAXLINE];
    fd_set rfs;
    client *cl;
    connection *srv;

    cl  = (client *) sv->a;
    srv = cl->scon;
    rfs = srv->afs;

    s[0] = '\0';
    for (fd = 0; fd < FD_SETSIZE; ++fd)
        {if (FD_ISSET(fd, &rfs))
            vstrcat(s, MAXLINE, " %d", fd);};

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

static void remove_fd(srvdes *sv, int fd)
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

static void _new_connection(srvdes *sv)
   {int fd;
    socklen_t sz;
    client *cl;
    connection *srv;

    cl  = (client *) sv->a;
    srv = cl->scon;
    sz  = sizeof(srv->sck);

    fd = accept(srv->sfd, (struct sockaddr *) &srv->sck, &sz);
    if (fd > 0)
       add_fd(sv, fd);
    else
       {close(fd);
	SLOG(sv, 1, "accept error - %s (%d)", strerror(errno), errno);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASYNC_SERVER - run a fully asynchronous server */

void async_server(srvdes *sv)
   {int fd, nr, ok;
    fd_set rfs;
    client *cl;
    connection *srv;

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
	for (ok = TRUE; (ok == TRUE) && (ng < tmax) && (nb < nbmax); )
	    {_check_fd(sv);

/* timeout the select every DT seconds */
	     t.tv_sec  = dt;
	     t.tv_usec = 0;

	     rfs = srv->afs;
	     nr  = select(FD_SETSIZE, &rfs, NULL, NULL, &t);
	     if (nr > 0)
	        {for (fd = 0; (fd < FD_SETSIZE) && (ok == TRUE); ++fd)
		     {if (FD_ISSET(fd, &rfs))
			 {SLOG(sv, 4, "data available on %d (server %d)",
			       fd, srv->sfd);

/* new connection request */
			  if (fd == srv->sfd)
			     _new_connection(sv);

/* data arriving on an existing connection */
			  else
			     {ok = sv->process(sv, fd);
                              if (ok == -1)
				 {ok = TRUE;
				  remove_fd(sv, fd);
				  nb++;}
			      else
				 nb = 0;

			      sched_yield();};};};
		  ng = 0;}
	     else
	        ng += dt;};

	if (ok != TRUE)
	   {SLOG(sv, 4, "done by command");}
	else if (ng >= tmax)
	   {SLOG(sv, 4, "done by time: %d >= %d", ng, tmax);}
	else if (nb >= nbmax)
	   {SLOG(sv, 4, "done by failed reads: %d >= %d", nb, nbmax);};}

    else
       SLOG(sv, 1, "async_server error (%s - %d)", strerror(errno), errno);

    return;}

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

/* VERIFYX - verify the transaction request
 *         - return TRUE if ANS is correct
 */

int verifyx(client *cl, char *ans, char *res)
   {int nk, rv;
    char lres[MAXLINE];
    char *key;

    rv = TRUE;

    nk  = cl->nkey;
    key = cl->key;

/* without authentication */
    if ((nk == 0) && (key == NULL))
       rv = TRUE;

/* client wants the answer */
    else if (res != NULL)
       {snprintf(res, nk+1, "%s", key);
	rv = TRUE;}

/* server matches the answer */
    else
       {res = lres;
	snprintf(res, nk+1, "%s", key);
	rv = (strncmp(res, ans, nk) == 0);};

    if (rv == FALSE)
       CLOG(cl, 1, "access denied - bad key");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _COMM_READ_WRK - read worker routine */

static int _comm_read_wrk(client *cl, char *t, int nt, int to)
   {int nb;

#ifdef IO_ALARM
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
   {int nb, nk, no, nt, ok;
    char *p, *t;

    nb = _comm_read_wrk(cl, s, nc, to);

    no = 0;
    nk = cl->nkey;
    nt = nc + nk + 2;
    t  = MAKE_N(char, nt);
    p  = t;

    nstrncpy(t, nt, s, -1);

    ok = FALSE;
    if (strncmp(t, "auth:", 5) == 0)
       {if (svs.auth == TRUE)
	   {no  = nk + 6;
	    nb -= no;
	    p   = t + no;
            ok  = verifyx(cl, t+5, NULL);};}
	   
    else if (svs.auth != TRUE)
       ok = TRUE;

    if (ok == FALSE)
       {p = t + strlen(t) - 4;
	if (strcmp(p, "fin:") == 0)
	   nstrncpy(s, nc, p, -1);
	else
	   nstrncpy(s, nc, "reject:", -1);

	nb = strlen(s);}

    else if (strcmp(s, p) != 0)
       nstrncpy(s, nc, p, -1);

    FREE(t);

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

    ok = verifyx(cl, NULL, ans);
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
/*--------------------------------------------------------------------------*/

/* MAKE_CLIENT - initialize and return a client connection instance */

client *make_client(ckind type, int auth, char *root, 
		    void (*clog)(client *cl, int lvl, char *fmt, ...))
   {client *cl;
    static connection global_srv;

    cl = MAKE(client);
    if (cl != NULL)
       {cl->cfd    = -1;
        cl->auth   = auth;
	cl->nkey   = 0;
	cl->key    = NULL;
	cl->root   = root;
	cl->type   = type;
        cl->a      = NULL;
	cl->scon   = &global_srv;
        cl->clog   = clog;

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

	FREE(cl->key);
	FREE(cl);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
