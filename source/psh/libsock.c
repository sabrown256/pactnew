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

#include "network.h"

#define NATTEMPTS   3
#define N_AKEY      32

#define C_OR_S(_p)    ((_p) ? "CLIENT" : "SERVER")

enum {CONN_NAME = 0, CONN_PORT, CONN_IP, CONN_KEY, CONN_PID};

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
    int nkey;
    char *key;
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

/* FIND_CONN - find conventionally named connection from ROOT
 *           - look in the head directory of ROOT
 *           - .../foo -> .../foo.host@port
 *           - return the unique file name
 */

static char *find_conn(char *root, int ch)
   {char *rv;
    static char s[MAXLINE];

    rv = NULL;

    if (root == NULL)
       root = cgetenv(TRUE, "PERDB_PATH");

    snprintf(s, MAXLINE, "%s.conn", root);
    if (file_exists(s) == TRUE)
       rv = s;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONN_EXISTS - return TRUE iff the connection specified exists */

static int conn_exists(char *fmt, ...)
   {int rv;
    char s[MAXLINE];
    char *conn;

    rv = FALSE;

    if (fmt != NULL)
       {VA_START(fmt);
	VSNPRINTF(s, MAXLINE, fmt);
	VA_END;

	conn = find_conn(s, -1);
	rv   = (conn != NULL);

#ifdef VERBOSE
	{char *flog, *wh;

	 flog = name_log(s);
	 wh   = C_OR_S(srv.server == 0);
	 log_activity(flog, dbg_sock, 2, wh, "exist |%s| (%s)",
		      sock,
		      (rv == TRUE) ? "yes" : "no");};
#endif
       };

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_SERVER_CONN - create a descriptive connection for the server
 *                  - on HOST at PORT at the ROOT collection/location
 *                  - return TRUE iff successful
 */

static int make_server_conn(client *cl, char *root, int auth, char *host, int port)
   {int i, c, rv;
    unsigned int rs;
    char key[N_AKEY+1], s[MAXLINE];
    FILE *fp;

    rv = FALSE;

    snprintf(s, MAXLINE, "%s.conn", root);
    fp = fopen(s, "w");
    if (fp != NULL)
       {in_addr_t haddr;

        fprintf(fp, "%s\n", host);
	fprintf(fp, "%d\n", port);

/* address */
	haddr = inet_addr(host);

	if (haddr == INADDR_NONE)
	   {struct hostent *hp;

	    hp = gethostbyname(host);
	    if (hp != NULL)
	       haddr = *(in_addr_t *) hp->h_addr_list[0];};

	fprintf(fp, "%ld\n", (long) haddr);

/* key */
        rs = ((unsigned int) time(NULL)) % 10091;
	srandom(rs);
        for (i = 0; i < N_AKEY; )
	    {c = '!' + (random() % 93);
	     if (c == ':')
	        continue;
	     else
	        key[i++] = c;};
	key[i] = '\0';
	fprintf(fp, "%s\n", key);

/* server PID */
	fprintf(fp, "%ld\n", (long) getpid());

	fclose(fp);

/* set the file permission */
	if (auth == TRUE)
	   chmod(s, S_IRUSR);
	else
	   chmod(s, S_IRUSR | S_IRGRP | S_IROTH);

#if 0
/* reopen the file */
        fp = fopen(s, "r");

/* unlink the file so that it goes away no matter how the server exits */
	unlink(s);
#endif

	if (cl != NULL)
	   {cl->nkey = N_AKEY;
	    cl->key  = STRSAVE(key);};

        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_CONN - parse out the connection info associated with ROOT
 *            - return a string array with the entries:
 *            -    0 host name
 *            -    1 port number
 *            -    2 IP address
 *            -    3 authentication key
 */

char **parse_conn(char *root)
   {char *res, **rv;

    rv = NULL;

    res = find_conn(root, -1);
    if (res != NULL)
       rv = file_text(FALSE, res);

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
    log_activity(flog, dbg_sock, 2, wh,
		 "close socket %d", fd);

    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SERVER - setup the server side of communications
 *             - the server scans for a port it can bind
 *             - and writes the connection which clients
 *             - then parse to obtain the host and port
 *             - for connections to the server
 */

static int init_server(client *cl, char *root, int auth)
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
 * also simplifies connection file processing
 */
	    hst = strtok(host, ".\n");
	    rv = make_server_conn(cl, root, auth, hst, srv.port);};

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
	       log_activity(flog, dbg_sock, 1, "SERVER",
			    "getsockname error %d - %s (%d)",
			    fds, strerror(errno), errno);

	    log_activity(flog, dbg_sock, 2, "SERVER",
			 "accept on %d ...", fds);
	    fdc = accept(fds, sa, &len);
	    if (fdc >= 0)
	       log_activity(flog, dbg_sock, 2, "SERVER",
			    "accept ok %d", fdc);
	    else
	       log_activity(flog, dbg_sock, 1, "SERVER",
			    "accept ng (%s - %d)",
			    strerror(errno), errno);}
	else
	   log_activity(flog, dbg_sock, 1, "SERVER",
			"listen error on %d - (%s - %d)",
			fds, strerror(errno), errno);};

    cl->fd   = fdc;
    cl->type = SERVER;

    return(fdc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_CONNECT_SOCKET - fill PS with required info for connect call
 *                    - return the connection string array iff successful
 *                    - useful for later logging
 */

char **get_connect_socket(struct sockaddr_in *ps, char *root)
   {int sasz, port;
    in_addr_t haddr;
    char *host, **sa;

    sa = parse_conn(root);
    if ((sa != NULL) && (ps != NULL))
       {host  = sa[CONN_NAME];
	port  = atoi(sa[CONN_PORT]);
	haddr = atol(sa[CONN_IP]);

	sasz = sizeof(struct sockaddr_in);

	memset(ps, 0, sasz);
	ps->sin_family = PF_INET;
	ps->sin_port   = htons(port);

	if (haddr == INADDR_NONE)
	   {haddr = inet_addr(host);

	    if (haddr == INADDR_NONE)
	       {struct hostent *hp;

		hp = gethostbyname(host);
		if (hp != NULL)
		   haddr = *(in_addr_t *) hp->h_addr_list[0];};};

	if (haddr != INADDR_NONE)
	   memcpy(&ps->sin_addr, &haddr, sizeof(long));};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CLIENT - client initiates connection with the server */

static int connect_client(client *cl)
   {int sasz, fd, err;
    char *key, *root, **ta;
    connection *srv;

    root = cl->root;
    srv  = cl->server;

    fd  = -1;
    key = NULL;

    ta = get_connect_socket(&srv->sck, root);
    if (ta != NULL)
       {fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd >= 0)
	   {int ia, na;
	    char *flog;
	    struct sockaddr *sa;

	    flog = name_log(root);

	    sasz = sizeof(struct sockaddr_in);

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
	       log_activity(flog, dbg_sock, 2, "CLIENT",
			    "connect %s@%s on %d (attempt %d)",
			    ta[0], ta[1], fd, ia);
	    else
	       {fd = connect_close(fd, cl, NULL);
		log_activity(flog, dbg_sock, 1, "CLIENT",
			     "connect error %s@%s  %d - %s (%d)",
			     ta[0], ta[1], fd,
			     strerror(errno), errno);};

	    srv->server = fd;
	    srv->port   = atoi(ta[1]);};

/* get the shared key */
	key = STRSAVE(ta[CONN_KEY]);

	free_strings(ta);};

    cl->fd   = fd;
    cl->nkey = N_AKEY;
    cl->key  = key;
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
       log_activity(flog, dbg_sock, 1, "CLIENT",
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
        else if ((fds == 0) || (cl->type == CLIENT))
	   fd = connect_client(cl);
        else
	   fd = connect_server(cl);};

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_SERVER - initialize the server socket connection speficied by ROOT */

static int open_server(client *cl, char *root, int auth)
   {int rv;
    char *flog, *wh;

    if ((root == NULL) && (cl != NULL))
       root = cl->root;

    rv  = TRUE;
    rv &= init_server(cl, root, auth);
    rv &= conn_exists(root);

    wh   = C_OR_S(srv.server == 0);
    flog = name_log(root);
    log_activity(flog, dbg_sock, 2, wh, "open %d", rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_SOCK - close the socket specified by ROOT and remove the connection */

static int close_sock(char *root)
   {int st, rv;
    char *conn, *flog, *wh;

    rv = TRUE;

    conn = find_conn(root, -1);
    if (conn != NULL)
       {st  = unlink_safe(conn);
	rv &= (st == 0);

	srv.server = connect_close(srv.server, NULL, root);

	wh = C_OR_S(srv.server == 0);

	flog = name_log(root);
	log_activity(flog, dbg_sock, 2, wh, "close %d", st);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_SOCK - read from the socket ROOT into S */

static int read_sock(client *cl, char *s, int nc)
   {int nb, fd, ev;
    char *flog, *wh, *root;

    wh   = C_OR_S(cl->type == CLIENT);
    root = cl->root;
    flog = name_log(root);
    fd   = client_fd(cl);

    if (fd < 0)
       {nb = -1;
	log_activity(flog, dbg_sock, 1, wh, "read - no connection");}

    else
       {nb = read(fd, s, nc);
	ev = errno;

/* guarantee NULL termination */
	s[nc-1] = '\0';

	if (nb > 0)
	   {if (s[nb] != '\0')
	       s[nb] = '\0';

	    log_activity(flog, dbg_sock, 1, wh,
			 "read %d |%s| (%d)",
			 fd, s, nb);}
	else
	   {s[0] = '\0';

	    if (nb == 0)
	       {log_activity(flog, dbg_sock, 2, wh,
			     "read %d - zero byte", fd);
/* GOCTHA: should probably be moved to async_server in perdb.c */
		cl->fd = connect_close(fd, cl, NULL);
		nb = -1;}
	    else
	       {log_activity(flog, dbg_sock, 1, wh,
			     "read %d error - %s (%d)",
			     fd, strerror(ev), ev);
		nb = -2;};};};

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
	   log_activity(flog, dbg_sock, 1, wh, "write - no connection");

	else
	   {log_activity(flog, dbg_sock, 1, wh, "write %d |%s| ... ",
			 fd, s);

	    if (nc <= 0)
	       nc = strlen(s) + 1;

	    nb = write(fd, s, nc);

	    if ((nb >= 0) && (nb == nc))
	       log_activity(flog, dbg_sock, 2, wh,
			    "write %d ok (%d bytes sent)",
			    fd, nb);
	    else
	       log_activity(flog, dbg_sock, 1, wh,
			    "write %d ng (%s - %d)",
			    fd, strerror(errno), errno);};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
