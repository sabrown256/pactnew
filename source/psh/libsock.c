/*
 * LIBSOCK.C - routines to manage client/server connection via sockets
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBSOCK

#define LIBSOCK

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/select.h>

#ifndef INADDR_NONE
# define INADDR_NONE NULL
#endif

#define NATTEMPTS   3

#define C_OR_S(_p)    ((_p) ? "CLIENT" : "SERVER")

typedef struct s_connection connection;
typedef struct s_client client;

struct s_connection
   {int server;                  /* server side descriptor */
    int port;
    int pid;
    struct sockaddr_in sck;
    fd_set afs;};

struct s_client
   {int fd;                      /* client side descriptor */
    char *root;
    ckind type;
    connection *server;
    database *db;};

static connection
 srv;

static int
 dbg_sock = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_SEMA - find conventionally named semaphore from ROOT
 *           - look in the head directory of ROOT
 *           - .../foo -> .../foo.host@port
 *           - return the unique file name
 */

static char *find_sema(char *root, int ch)
   {int ia, na;
    char *hd, *tl, *rv, *h, *p;
    DIR *dir;
    struct dirent *dp;
    static char s[MAXLINE];

    rv = NULL;

    if (root == NULL)
       root = cgetenv(TRUE, "PERDB_PATH");

    if (IS_NULL(root) == TRUE)
       {hd = ".";
	tl = "defdb";}
    else
       {hd = path_head(root);
	tl = path_tail(root);};

    na = NATTEMPTS;
    for (ia = 0; (ia < na) && (rv == NULL); ia++)
        {sleep(ia);
	 dir = opendir(hd);
	 if (dir != NULL)
	    {while (TRUE)
	        {dp = (struct dirent *) readdir(dir);
		 if (dp == NULL)
		    break;
		 else
		    {strncpy(s, dp->d_name, MAXLINE);
		     h = strstr(s, tl);
		     p = strchr(s, '@');
		     if ((h != NULL) && (p != NULL))
		        {snprintf(s, MAXLINE, "%s/%s", hd, dp->d_name);
			 rv = s;
			 break;};};};
	     closedir(dir);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEMA_EXISTS - return TRUE iff the semaphore specified exists */

static int sema_exists(char *fmt, ...)
   {int rv;
    char s[MAXLINE];
    char *sema;

    rv = FALSE;

    if (fmt != NULL)
       {VA_START(fmt);
	VSNPRINTF(s, MAXLINE, fmt);
	VA_END;

	sema = find_sema(s, -1);
	rv   = (sema != NULL);

#ifdef VERBOSE
	{char *flog, *wh;

	 flog = name_log(s);
	 wh   = C_OR_S(srv.server == 0);
	 log_activity(flog, dbg_sock, wh, "exist |%s| (%s)",
		      sock,
		      (rv == TRUE) ? "yes" : "no");};
#endif
       };

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CLOSE - close the socket connection */

static int connect_close(int fd, client *cl, char *root)
   {char *wh, *flog;

    wh = "SERVER";
    if (cl != NULL)
       {root = cl->root;
	wh   = C_OR_S(cl->type == CLIENT);};

    close(fd);

    flog = name_log(root);
    log_activity(flog, dbg_sock, wh,
		 "close socket %d", fd);

    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_SERVER_SEMA - create a descriptive semaphore for the server
 *                  - on HOST at PORT at the ROOT collection/location
 *                  - return TRUE iff successful
 */

static int make_server_sema(char *root, char *host, int port)
   {int i, fd, rv;
    char s[MAXLINE];

    snprintf(s, MAXLINE, "%s.%s@%d", root, host, port);

    rv = 0;

/* try NATTEMPTS times to make the semaphore */
    for (i = 0; i < NATTEMPTS; i++)
        {fd = open(s, O_WRONLY | O_CREAT, 0600);
	 if (fd >= 0)
	    {rv = fsync(fd);
	     if (rv != 0)
	        continue;
	     rv = close(fd);
	     if (rv == 0)
	        break;};};

/* change the sense of the return value */
    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SERVER - setup the server side of communications
 *             - the server scans for a port it can bind
 *             - and writes the semaphore which clients
 *             - then parse to obtain the host and port
 *             - for connections to the server
 */

static int init_server(char *root)
   {int iprt, port, sasz, rv;
    char host[MAXLINE];
    char *hst;

/* check for server being setup already */
    if ((srv.server != 0) && (srv.port != 0))
       rv = TRUE;

    else
       {rv   = FALSE;
	port = -1;
	sasz = sizeof(struct sockaddr_in);

	srv.server = socket(PF_INET, SOCK_STREAM, 0);
	if (srv.server > 0)
	   {srv.sck.sin_family      = PF_INET;
	    srv.sck.sin_addr.s_addr = htonl(INADDR_ANY);

/* scan for a port that we can use */
	    for (iprt = 15000; iprt < 65000; iprt++)
	        {srv.sck.sin_port = htons(iprt);

		 if (bind(srv.server, (struct sockaddr *) &srv.sck, sasz) >= 0)
		    {port = iprt;
		     break;};};};

	srv.port = port;
	if (srv.port >= 0)
	   {gethostname(host, MAXLINE);

/* try to keep connections local by converting
 * complete host names 'abc.xyz.foo' to partial ones 'abc'
 * also simplifies semaphore file processing
 */
	    hst = strtok(host, ".\n");
	    rv = make_server_sema(root, hst, srv.port);};

/* initialize the set of active sockets */
	FD_ZERO(&srv.afs);
	FD_SET(srv.server, &srv.afs);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_SERVER - make a connection with the server */

static int connect_server(client *cl)
   {int fdc, fds, err, sasz;
    char *root, *flog;
    socklen_t len;
    struct sockaddr *sa;
    connection *srv;

    root = cl->root;
    srv  = cl->server;

    flog = name_log(root);

    fdc  = -1;
    sasz = sizeof(struct sockaddr_in);
    len  = sasz;

    sa = (struct sockaddr *) &srv->sck;

    fds = srv->server;
    if (srv->port >= 0)
       {err = listen(fds, 5);
	if (err >= 0)
	   {err = getsockname(fds, sa, &len);
	    if (err == -1)
	       log_activity(flog, dbg_sock, "SERVER",
			    "getsockname error %d - %s (%d)",
			    fds, strerror(errno), errno);

	    log_activity(flog, dbg_sock, "SERVER", "accept on %d ...", fds);
	    fdc = accept(fds, sa, &len);
	    if (fdc >= 0)
	       log_activity(flog, dbg_sock, "SERVER", "accept ok %d", fdc);
	    else
	       log_activity(flog, dbg_sock, "SERVER",
			    "accept ng (%s - %d)",
			    strerror(errno), errno);}
	else
	   log_activity(flog, dbg_sock, "SERVER",
			"listen error on %d - (%s - %d)",
			fds, strerror(errno), errno);};

    cl->fd   = fdc;
    cl->type = SERVER;

    return(fdc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CLIENT - client initiates connection with the server */

static int connect_client(client *cl)
   {int sasz, fd, err, port;
    char s[MAXLINE];
    char *res, *host, *prt, *root;
    in_addr_t haddr;
    connection *srv;

    root = cl->root;
    srv  = cl->server;

    fd = -1;

    res = find_sema(root, -1);
    if (res != NULL)
       {nstrncpy(s, MAXLINE, path_tail(res), -1);
	res = strrchr(s, '.') + 1;
	key_val(&host, &prt, res, "@");
	if (prt != NULL)
	   port = atoi(prt);
	else
	   port = 0;

	if ((host != NULL) && (port > 0))
	   {sasz = sizeof(struct sockaddr_in);

	    memset(&srv->sck, 0, sasz);
	    srv->sck.sin_family = PF_INET;
	    srv->sck.sin_port   = htons(port);

	    haddr = inet_addr(host);

	    if (haddr == INADDR_NONE)
	       {struct hostent *hp;

		hp = gethostbyname(host);
		if (hp != NULL)
		   haddr = *(in_addr_t *) hp->h_addr_list[0];};

	    if (haddr != INADDR_NONE)
	       {memcpy(&srv->sck.sin_addr, &haddr, sizeof(long));

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd >= 0)
		   {int ia, na;
		    char *flog;
		    struct sockaddr *sa;

		    flog = name_log(root);

/* try NATTEMPTS times to connect to the server */
		    na = NATTEMPTS;
		    for (ia = 0; ia < na; ia++)
		        {sleep(ia);

			 sa  = (struct sockaddr *) &srv->sck;
			 err = connect(fd, sa, sasz);
			 if (err >= 0)
			    break;};

/* report the connection status */
		    if (err >= 0)
		       log_activity(flog, dbg_sock,
				    "CLIENT", "connect %s@%s on %d (attempt %d)",
				    host, prt, fd, ia);
		    else
		       {fd = connect_close(fd, cl, NULL);
			log_activity(flog, dbg_sock, "CLIENT",
				     "connect error %s@%s  %d - %s (%d)",
				     host, prt, fd,
				     strerror(errno), errno);};

		    srv->server = fd;
		    srv->port   = port;};};};};

    cl->fd   = fd;
    cl->type = CLIENT;

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_ERROR - make a connection with the server */

static int connect_error(client *cl)
   {int fds;
    char *root, *flog;
    connection *srv;

    root = cl->root;
    flog = name_log(root);

    srv = cl->server;
    fds = srv->server;
    if (fds < 0)
       log_activity(flog, dbg_sock, "CLIENT",
		    "no server connection available");

    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLIENT_FD - return the file descriptor of the connection to CL */

static int client_fd(client *cl)
   {int fd, fds;
    connection *srv;

    fd = cl->fd;

    if (fd == -1)
       {srv = cl->server;
	fds = srv->server;
	if (fds < 0)
	   fd = connect_error(cl);
        else if (fds == 0)
	   fd = connect_client(cl);
        else
	   fd = connect_server(cl);};

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_SERVER - initialize the server socket connection speficied by ROOT */

static int open_server(char *root)
   {int rv;
    char *flog, *wh;

    rv  = TRUE;
    rv &= init_server(root);
    rv &= sema_exists(root);

    wh   = C_OR_S(srv.server == 0);
    flog = name_log(root);
    log_activity(flog, dbg_sock, wh, "open %d", rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_SOCK - close the socket specified by ROOT and remove the semaphore */

static int close_sock(char *root)
   {int st, rv;
    char *sema, *flog, *wh;

    rv = TRUE;

    sema = find_sema(root, -1);
    if (sema != NULL)
       {st  = unlink(sema);
	rv &= (st == 0);

	srv.server = connect_close(srv.server, NULL, root);

	wh = C_OR_S(srv.server == 0);

	flog = name_log(root);
	log_activity(flog, dbg_sock, wh, "close %d", st);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_SOCK - read from the socket ROOT into S */

static int read_sock(client *cl, char *s, int nc)
   {int nb, fd;
    char *flog, *wh, *root;

    wh   = C_OR_S(cl->type == CLIENT);
    root = cl->root;
    flog = name_log(root);
    fd   = client_fd(cl);

    if (fd < 0)
       {nb = -1;
	log_activity(flog, dbg_sock, wh, "read - no connection");}

    else
       {nb = read(fd, s, nc);

/* guarantee NULL termination */
	s[nc-1] = '\0';

	if (s[nb] != '\0')
	   s[nb] = '\0';

	if (nb > 0)
	   log_activity(flog, dbg_sock, wh, "read %d |%s| (%d)",
			fd, s, nb);
	else if (nb == 0)
	   nb = -1;
	else
	   {log_activity(flog, dbg_sock, wh, "read %d |%s| - %s (%d)",
			 fd, s, strerror(errno), errno);
	    nb = -2;};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_SOCK - write to the socket ROOT from S */

static int write_sock(client *cl, char *s, int nc)
   {int nb, fd;
    char *flog, *wh, *root;

    nb = -1;

    if ((cl != NULL) && (s != NULL))
       {wh   = C_OR_S(cl->type == CLIENT);
	root = cl->root;
	flog = name_log(root);
	fd   = client_fd(cl);

	if (fd < 0)
	   log_activity(flog, dbg_sock, wh, "write - no connection");

	else
	   {log_activity(flog, dbg_sock, wh, "write %d |%s| ... ",
			 fd, s);

	    if (nc <= 0)
	       nc = strlen(s) + 1;

	    nb = write(fd, s, nc);

	    if ((nb >= 0) && (nb == nc))
	       log_activity(flog, dbg_sock, wh, "write %d ok (%d bytes sent)",
			    fd, nb);
	    else
	       log_activity(flog, dbg_sock, wh, "write %d ng (%s - %d)",
			    fd, strerror(errno), errno);};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
