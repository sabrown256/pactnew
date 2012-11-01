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

#define C_OR_S(_p)      ((_p) ? "CLIENT" : "SERVER")

#ifndef S_SPLINT_S
#define CLOG(_c, ...)                                                    \
    {if ((_c)->clog != NULL)                                             \
        (_c)->clog((_c), __VA_ARGS__);}  
#endif

enum {CONN_NAME = 0, CONN_PORT, CONN_IP, CONN_KEY, CONN_PID};

enum e_ckind
 {SERVER, CLIENT};

typedef enum e_ckind ckind;

typedef struct s_connection connection;
typedef struct s_client client;

struct s_connection
   {int sfd;                     /* server side descriptor */
    int port;
    int pid;
    struct sockaddr_in sck;
    fd_set afs;};

struct s_client
   {int cfd;                     /* client side descriptor */
    int auth;
    int nkey;
    char *key;
    char *root;
    ckind type;
    void *a;
    connection *scon;
    void (*clog)(client *cl, int lvl, char *fmt, ...);};

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
	rv   = (conn != NULL);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_SERVER_CONN - create a descriptive connection for the server
 *                  - on HOST at PORT at the root collection/location
 *                  - of CL
 *                  - return TRUE iff successful
 */

static int make_server_conn(client *cl, int auth, char *host, int port)
   {int i, c, rv;
    unsigned int rs;
    char key[N_AKEY+1], s[MAXLINE];
    FILE *fp;

    rv = FALSE;

    if ((cl != NULL) && (host != NULL))
       {snprintf(s, MAXLINE, "%s.conn", cl->root);
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

	    if (cl != NULL)
	       {FREE(cl->key);
		cl->key  = STRSAVE(key);
		cl->nkey = N_AKEY;};

	    rv = TRUE;};};

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

static int connect_close(int fd, client *cl)
   {

    close(fd);

    CLOG(cl, 1, "close socket %d", fd);

    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SERVER - setup the server side of communications
 *             - the server scans for a port it can bind
 *             - and writes the connection which clients
 *             - then parse to obtain the host and port
 *             - for connections to the server
 */

static int init_server(client *cl, int auth)
   {int iprt, port, rv;
    socklen_t sasz;
    char host[MAXLINE];
    char *hst;
    connection *srv;

    srv = cl->scon;

/* check for server being setup already */
    if ((srv->sfd != 0) && (srv->port != 0))
       rv = TRUE;

    else
       {rv   = FALSE;
	port = -1;
	sasz = sizeof(struct sockaddr_in);

	srv->sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (srv->sfd > 0)
	   {srv->sck.sin_family      = PF_INET;
	    srv->sck.sin_addr.s_addr = htonl(INADDR_ANY);

/* scan for a port that we can use */
	    for (iprt = 15000; iprt < 65000; iprt++)
	        {srv->sck.sin_port = htons(iprt);

		 if (bind(srv->sfd, (struct sockaddr *) &srv->sck, sasz) >= 0)
		    {port = iprt;
		     break;};};};

	srv->port = port;
	if (srv->port >= 0)
	   {gethostname(host, MAXLINE);

/* try to keep connections local by converting
 * complete host names 'abc.xyz.foo' to partial ones 'abc'
 * also simplifies connection file processing
 */
	    hst = strtok(host, ".\n");
	    rv = make_server_conn(cl, auth, hst, srv->port);};

/* initialize the set of active sockets */
	FD_ZERO(&srv->afs);
	FD_SET(srv->sfd, &srv->afs);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_SERVER - make a connection with the server */

static int connect_server(client *cl)
   {int fdc, fds, err;
    socklen_t len, sasz;
    struct sockaddr *sa;
    connection *srv;

    srv  = cl->scon;

    fdc  = -1;
    sasz = sizeof(struct sockaddr_in);
    len  = sasz;

    sa = (struct sockaddr *) &srv->sck;

    fds = srv->sfd;
    if (srv->port >= 0)
       {err = listen(fds, 5);
	if (err >= 0)
	   {err = getsockname(fds, sa, &len);
	    if (err == -1)
	       CLOG(cl, 1, "getsockname error %d - %s (%d)",
		    fds, strerror(errno), errno);

	    CLOG(cl, 2, "accept on %d ...", fds);
	    fdc = accept(fds, sa, &len);
	    if (fdc >= 0)
	       {CLOG(cl, 2, "accept ok %d", fdc);}
	    else
	       CLOG(cl, 1, "accept ng (%s - %d)",
		    strerror(errno), errno);}
	else
	   CLOG(cl, 1, "listen error on %d - (%s - %d)",
		fds, strerror(errno), errno);};

    cl->cfd  = fdc;
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
   {int fd, err;
    socklen_t sasz;
    char *key, *root, **ta;
    connection *srv;

    fd  = -1;
    if (cl != NULL)
       {root = cl->root;
	srv  = cl->scon;

	key = NULL;

	ta = get_connect_socket(&srv->sck, root);
	if (ta != NULL)
	   {fd = socket(PF_INET, SOCK_STREAM, 0);
	    if (fd >= 0)
	       {int ia, na;
		struct sockaddr *sa;

		sasz = sizeof(struct sockaddr_in);

/* try NATTEMPTS times to connect to the server */
		err = 0;
		na  = NATTEMPTS;
		for (ia = 0; ia < na; ia++)
		    {sleep(ia);

		     sa  = (struct sockaddr *) &srv->sck;
		     err = connect(fd, sa, sasz);
		     if (err >= 0)
		        break;};

/* report the connection status */
		if (err >= 0)
		   {CLOG(cl, 2, "connect %s@%s on %d (attempt %d)",
			 ta[0], ta[1], fd, ia);}
		else
		   {fd = connect_close(fd, cl);
		    CLOG(cl, 1, "connect error %s@%s  %d - %s (%d)",
			 ta[0], ta[1], fd,
			 strerror(errno), errno);};

		srv->sfd  = fd;
		srv->port = atoi(ta[1]);};

/* get the shared key */
	    key = STRSAVE(ta[CONN_KEY]);

	    free_strings(ta);};

	FREE(cl->key);
	cl->key  = key;
	cl->nkey = N_AKEY;
	cl->type = CLIENT;
	cl->cfd  = fd;};

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_ERROR - make a connection with the server */

static int connect_error(client *cl)
   {int fds;
    connection *srv;

    srv = cl->scon;
    fds = srv->sfd;
    if (fds < 0)
       CLOG(cl, 1, "no server connection available");

    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLIENT_FD - return the file descriptor of the connection to CL */

static int client_fd(client *cl)
   {int fd, fds;
    connection *srv;

    fd = cl->cfd;

    if (fd == -1)
       {srv = cl->scon;
	fds = srv->sfd;
	if (fds < 0)
	   fd = connect_error(cl);
        else if ((fds == 0) || (cl->type == CLIENT))
	   fd = connect_client(cl);
        else
	   fd = connect_server(cl);};

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_SERVER - initialize the server socket connection speficied by ROOT
 *           - analog of open_fifo in libfifo.c
 */

int open_server(client *cl, ckind ioc, int auth)
   {int rv;

    rv  = TRUE;
    rv &= init_server(cl, auth);
    rv &= conn_exists(cl->root);

    CLOG(cl, 2, "open %d", rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_SOCK - close the socket specified by ROOT and remove the connection */

int close_sock(client *cl)
   {int st, rv;
    char *conn, *root;
    connection *srv;

    rv = TRUE;

    root = cl->root;
    srv  = cl->scon;
    conn = find_conn(root, -1);
    if (conn != NULL)
       {st  = unlink_safe(conn);
	rv &= (st == 0);

	srv->sfd = connect_close(srv->sfd, cl);

	CLOG(cl, 2, "close %d", st);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_SOCK - read from the socket ROOT into S */

int read_sock(client *cl, char *s, int nc)
   {int nb, fd, ev;

    fd = client_fd(cl);

    if (fd < 0)
       {nb = -1;
	CLOG(cl, 1, "read - no connection");}

    else
       {nb = read(fd, s, nc);
	ev = errno;

/* guarantee NULL termination */
	s[nc-1] = '\0';

	if (nb > 0)
	   {if (s[nb] != '\0')
	       s[nb] = '\0';

	    if (strncmp(s, "auth: ", 5) == 0)
	       {int nk, nt;
		char *t;

		nk = cl->nkey + 6;
		t  = s + nk;
		nt = nb - nk;
		CLOG(cl, 1, "read %d auth|%s| (%d)", fd, t, nt);}
	    else
	       CLOG(cl, 1, "read %d |%s| (%d)", fd, s, nb);}

	else
	   {s[0] = '\0';

	    if (nb == 0)
	       {CLOG(cl, 2, "read %d - zero byte", fd);
/* GOCTHA: should probably be moved to async_server in perdb.c */
		cl->cfd = connect_close(fd, cl);
		nb = -1;}
	    else
	       {CLOG(cl, 1, "read %d error - %s (%d)",
		     fd, strerror(ev), ev);
		nb = -2;};};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_SOCK - write to the socket ROOT from S */

int write_sock(client *cl, char *s, int nc)
   {int nb, fd;

    nb = -1;

    if ((cl != NULL) && (s != NULL))
       {fd = client_fd(cl);

	if (fd < 0)
	   {CLOG(cl, 1, "write - no connection");}

	else
	   {if (strncmp(s, "auth:", 5) == 0)
	       {int nk;
		char *t;

		nk = cl->nkey + 6;
		t  = s + nk;
		CLOG(cl, 1, "write %d auth|%s| ... ", fd, t);}
	    else
	       CLOG(cl, 1, "write %d |%s| ... ", fd, s);

	    if (nc <= 0)
	       nc = strlen(s) + 1;

	    nb = write(fd, s, nc);

	    if ((nb >= 0) && (nb == nc))
	       {CLOG(cl, 2, "write %d ok (%d bytes sent)", fd, nb);}
	    else
	       CLOG(cl, 1, "write %d ng (%s - %d)",
		    fd, strerror(errno), errno);};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
