/*
 * LIBSRV.C - simple asynchronous server routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libdb.c"

#define SLOG(...)       {if (slog != NULL) slog(__VA_ARGS__);}  

typedef struct s_srvdes srvdes;

struct s_srvdes
   {void *a;
    connection *server;
    void (*setup)(srvdes *sv, int *ptmax, int *pdt);
    int (*process)(srvdes *sv, int fd);
    void (*slog)(srvdes *sv, char *fmt, ...);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CHECK_FD - verify/log fd set in SV */

static void _check_fd(srvdes *sv)
   {int fd;
    char s[MAXLINE];
    fd_set rfs;
    connection *srv;
    void (*slog)(srvdes *sv, char *fmt, ...);

    slog = sv->slog;
    srv  = sv->server;
    rfs  = srv->afs;

    s[0] = '\0';
    for (fd = 0; fd < FD_SETSIZE; ++fd)
        {if (FD_ISSET(fd, &rfs))
            vstrcat(s, MAXLINE, " %d", fd);};

    SLOG(sv, "monitoring: %s", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_FD - add FD to the set in SV */

static void add_fd(srvdes *sv, int fd)
   {connection *srv;
    void (*slog)(srvdes *sv, char *fmt, ...);

    slog = sv->slog;
    srv  = sv->server;

    FD_SET(fd, &srv->afs);
    SLOG(sv, "add fd %d", fd);

    _check_fd(sv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMOVE_FD - add FD to the set in SV */

static void remove_fd(srvdes *sv, int fd)
   {connection *srv;
    void (*slog)(srvdes *sv, char *fmt, ...);

    srv  = sv->server;
    slog = sv->slog;

    FD_CLR(fd, &srv->afs);
    SLOG(sv, "remove fd %d", fd);

    _check_fd(sv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NEW_CONNECTION - make a new connection on SFD and add it to AFS */

static void _new_connection(srvdes *sv)
   {int fd;
    socklen_t sz;
    connection *srv;
    void (*slog)(srvdes *sv, char *fmt, ...);

    srv  = sv->server;
    slog = sv->slog;
    sz   = sizeof(srv->sck);

    fd = accept(srv->server, (struct sockaddr *) &srv->sck, &sz);
    if (fd > 0)
       add_fd(sv, fd);
    else
       {close(fd);
	SLOG(sv, "accept error - %s (%d)", strerror(errno), errno);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASYNC_SERVER - run a fully asynchronous server */

void async_server(srvdes *sv)
   {int fd, nr, ok;
    fd_set rfs;
    connection *srv;
    void (*slog)(srvdes *sv, char *fmt, ...);

    srv  = sv->server;
    slog = sv->slog;

    if (listen(srv->server, 5) >= 0)
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
			 {SLOG(sv, "data available on %d (server %d)",
			       fd, srv->server);

/* new connection request */
			  if (fd == srv->server)
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
	   {SLOG(sv, "done by command");}
	else if (ng >= tmax)
	   {SLOG(sv, "done by time: %d >= %d", ng, tmax);}
	else if (nb >= nbmax)
	   {SLOG(sv, "done by failed reads: %d >= %d", nb, nbmax);};}

    else
       SLOG(sv, "async_server error (%s - %d)", strerror(errno), errno);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
