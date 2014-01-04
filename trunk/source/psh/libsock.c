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

# define LIBSOCK

# include "network.h"

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

#define SOCKADDR_SIZE(_x)     ((_x) = sizeof(struct sockaddr_in))

#ifndef S_SPLINT_S
#define CLOG(_c, ...)                                                    \
    {if (((_c) != NULL) && ((_c)->clog != NULL))                         \
        (_c)->clog((_c), __VA_ARGS__);}  
#endif

enum e_sock_size_constants
   {NATTEMPTS = 3,
    N_AKEY    = 32};

typedef enum e_sock_size_constants sock_size_constants;

enum e_ckind
 {SERVER, CLIENT};

typedef enum e_ckind ckind;

typedef union u_sckades sckades;
typedef struct s_connection connection;
typedef struct s_client client;

union u_sckades
   {struct sockaddr *uq;
    struct sockaddr_in *in;};

struct s_connection
   {int sfd;                     /* server side descriptor */
    int port;
    int pid;
    sckades sck;
    fd_set afs;};

struct s_client
   {int cfd;                     /* client side descriptor */
    int port;
    int auth;
    int nkey;
    char *key;
    char *root;
    char *fcon;                 /* name of the connection file */
    ckind type;
    void *a;
    connection *scon;
    io_ring ior;
    void (*clog)(client *cl, int lvl, char *fmt, ...);
    int (*cauth)(client *cl, int nc, char *ans, char *res);};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

#define C_OR_S(_p)            ((_p) ? "CLIENT" : "SERVER")

enum {CONN_NAME = 0, CONN_PORT, CONN_IP, CONN_KEY, CONN_PID};

/*--------------------------------------------------------------------------*/

/*                            TCP LEVEL ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* TCP_SET_PORT - set the port of AD to PORT
 *              - return the old value
 */

int tcp_set_port(sckades ad, int port)
   {int rv;

    rv = -1;

    if (ad.in != NULL)
       {rv = ad.in->sin_port;
	ad.in->sin_port = htons(port);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TCP_GET_ADDRESS - initialize and return a socket address
 *                 - for a connection to HOST/HADDR and PORT
 */

sckades tcp_get_address(char *host, int port, in_addr_t haddr)
   {socklen_t sasz;
    in_addr_t nad;
    sckades ad;

    ad.in = NULL;
    SOCKADDR_SIZE(sasz);

/* work out a valid host address */
    if (haddr == INADDR_NONE)
       {haddr = inet_addr(host);

	if (haddr == INADDR_NONE)
	   {struct hostent *hn, *ha;

	    hn = gethostbyname(host);
	    if (hn != NULL)
	       {nad = *(in_addr_t *) hn->h_addr_list[0];

/* verify that host associated with the address matches the specified host
 * things like OpenDNS give you a special address for unknown hosts
 * which will not match your original host
 */
		ha = gethostbyaddr(&nad, sizeof(nad), AF_INET);
		if ((ha != NULL) && (strcmp(ha->h_name, hn->h_name) == 0))
		   haddr = nad;};};};

/* initialize the socket address */
    if (haddr != INADDR_NONE)
       {ad.in = MAKE(struct sockaddr_in);
	if (ad.in != NULL)
	   {memset(ad.in, 0, sasz);
	    ad.in->sin_family = PF_INET;
	    if (haddr == INADDR_ANY)
	       ad.in->sin_addr.s_addr = htonl(haddr);
	    else
	       {tcp_set_port(ad, port);
		memcpy(&ad.in->sin_addr, &haddr, sizeof(in_addr_t));};};};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TCP_BIND_PORT - bind FD to AD and PORT
 *               - if successful return the PORT
 *               - otherwise return -1
 */

int tcp_bind_port(int fd, sckades ad, int port, int pmin)
   {int rv;
    socklen_t sasz;

    rv = -1;

    if (ad.uq != NULL)
       {SOCKADDR_SIZE(sasz);

	if (port > 0)
	   {tcp_set_port(ad, port);
	    rv = bind(fd, ad.uq, sasz);
	    rv = (rv >= 0) ? port : -1;}

	else
	   {for (port = pmin; port < 65000; port++)
	        {tcp_set_port(ad, port);
		 rv = bind(fd, ad.uq, sasz);
		 if (rv >= 0)
		    {rv = port;
		     break;};};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TCP_ACCEPT_CONNECTION - listen on bound socket FD, AD
 *                       - and return the descriptor of an accepted
 *                       - connection
 */

int tcp_accept_connection(int fd, sckades ad, int ao)
   {int nfd, st;
    socklen_t sasz;
    
    nfd = -1;

    SOCKADDR_SIZE(sasz);

    if (ao == FALSE)
       {getsockname(fd, ad.uq, &sasz);

	st = listen(fd, 5);}
    else
       st = 0;

    if (st >= 0)
       nfd = accept(fd, ad.uq, &sasz);

    return(nfd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TCP_INITIATE_CONNECTION - initiate a connection with FD and AD
 *                         - return 0 iff successful
 */

int tcp_initiate_connection(sckades ad)
   {int ia, na, fd, rv;
    socklen_t sasz;

    SOCKADDR_SIZE(sasz);

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd >= 0)

/* try NATTEMPTS times to connect to the server */
       {rv = 0;
	na  = NATTEMPTS;
	for (ia = 0; ia < na; ia++)
	    {sleep(ia);
	     rv = connect(fd, ad.uq, sasz);
	     if (rv >= 0)
	        break;};

	fd = (rv == 0) ? fd : -1;};

    return(fd);}

/*--------------------------------------------------------------------------*/

/*                             MID-LEVEL ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* FIND_CONN - find conventionally named connection from ROOT
 *           - look in the head directory of ROOT
 *           - .../foo -> .../foo.host@port
 *           - return the unique file name
 */

char *find_conn(char *root, int ch)
   {char *rv;
    static char s[BFLRG];

    rv = NULL;

    if (root == NULL)
       root = cgetenv(TRUE, "PERDB_PATH");

    snprintf(s, BFLRG, "%s.conn", root);
    if (file_exists(s) == TRUE)
       rv = s;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONN_EXISTS - return TRUE iff the connection specified exists */

static int conn_exists(char *fmt, ...)
   {int rv;
    char s[BFLRG];
    char *conn;

    rv = FALSE;

    if (fmt != NULL)
       {VA_START(fmt);
	VSNPRINTF(s, BFLRG, fmt);
	VA_END;

	conn = find_conn(s, -1);
	rv   = (conn != NULL);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _GET_HOST_ADDR - return the IP address of HOST as a long */

long _get_host_addr(char *host)
   {long rv;
    in_addr_t haddr;

    rv = -1;
    if (host != NULL)

/* address */
       {haddr = inet_addr(host);
	if (haddr == INADDR_NONE)
	   {struct hostent *hp;

	    hp = gethostbyname(host);
	    if (hp != NULL)
	       rv = *(in_addr_t *) hp->h_addr_list[0];};};

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
    char key[N_AKEY+1], s[BFLRG];
    FILE *fp;

    rv = FALSE;

    if ((cl != NULL) && (host != NULL))
       {snprintf(s, BFLRG, "%s.conn", cl->root);
	fp = fopen_safe(s, "w");
	if (fp != NULL)
	   {long pid, haddr;

/* address */
	    haddr = _get_host_addr(host);

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

/* server PID */
	    pid = getpid();

/* write fcon info */
	    fprintf(fp, "%s\n", host);
	    fprintf(fp, "%d\n", port);
	    fprintf(fp, "%ld\n", haddr);
	    fprintf(fp, "%s\n", key);
	    fprintf(fp, "%ld\n", pid);

	    fclose_safe(fp);

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

    close_safe(fd);

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
   {int port, rv;
    char host[BFLRG];
    char *hst;
    connection *srv;

    srv = cl->scon;

/* check for server being setup already */
    if ((srv->sfd != 0) && (srv->port != 0))
       rv = TRUE;

    else
       {rv   = FALSE;
	port = -1;

	srv->sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (srv->sfd > 0)
	   {srv->sck = tcp_get_address(NULL, 0, INADDR_ANY);
	    port     = tcp_bind_port(srv->sfd, srv->sck, -1, cl->port);};

	srv->port = port;
	if (srv->port >= 0)
	   {gethostname(host, BFLRG);

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
   {int fdc, fds;
    connection *srv;

    srv = cl->scon;
    fdc = -1;

    fds = srv->sfd;
    if (srv->port >= 0)
       {CLOG(cl, 2, "accept on %d ...", fds);
	fdc = tcp_accept_connection(fds, srv->sck, FALSE);
	if (fdc >= 0)
	   {CLOG(cl, 2, "accept ok %d", fdc);}
	else
	   CLOG(cl, 1, "accept ng (%s - %d)",
		strerror(errno), errno);};

    cl->cfd  = fdc;
    cl->type = SERVER;

    return(fdc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_CONNECT_SOCKET - fill the connection socket address of CL
 *                    - with required info for subsequent connect call
 *                    - return the connection string array iff successful
 *                    - useful for later logging
 */

char **get_connect_socket(client *cl)
   {int port;
    in_addr_t haddr;
    char *root, *host, **sa;
    connection *srv;

    sa = NULL;

    if (cl != NULL)
       {root = cl->root;
	srv  = cl->scon;

	sa = parse_conn(root);
	if ((sa != NULL) && (srv->sck.in == NULL))
	   {if ((IS_NULL(sa[CONN_NAME]) == FALSE) &&
		(IS_NULL(sa[CONN_PORT]) == FALSE) &&
		(IS_NULL(sa[CONN_IP]) == FALSE))
	       {host  = sa[CONN_NAME];
		port  = atoi(sa[CONN_PORT]);
		haddr = atol(sa[CONN_IP]);

		srv->sck = tcp_get_address(host, port, haddr);};};}

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_CONN_CLIENT - set the server connection info in CL
 *                 - return the PID of the server
 */

int get_conn_client(client *cl)
   {int pid;
    char *fcon, **sa;

    pid = -1;

    if (cl != NULL)
       {fcon = cl->fcon;

	sa = file_text(FALSE, fcon);
	if (sa != NULL)
	   {if (sa[CONN_PID] != NULL)
	       pid = atoi(sa[CONN_PID]);

	    if ((sa[CONN_KEY] != NULL) && (cl != NULL))
	       {FREE(cl->key);
		cl->key  = STRSAVE(sa[CONN_KEY]);
		cl->nkey = N_AKEY;};};

	free_strings(sa);};

    return(pid);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_CLIENT - client initiates connection with the server */

static int connect_client(client *cl)
   {int fd;
    char *key, **ta;
    connection *srv;

    fd  = -1;
    if (cl != NULL)
       {srv = cl->scon;

	key = NULL;

	ta = get_connect_socket(cl);
	if (ta != NULL)
	   {fd = tcp_initiate_connection(srv->sck);
	    if (fd >= 0)
	       {CLOG(cl, 2, "connect %s@%s on %d",
		     ta[0], ta[1], fd);

		srv->sfd  = fd;
		srv->port = atoi(ta[1]);

/* get the shared key */
		key = STRSAVE(ta[CONN_KEY]);}

	    else
	       {fd = connect_close(fd, cl);
		CLOG(cl, 1, "connect error %s@%s  %d - %s (%d)",
		     ta[0], ta[1], fd,
		     strerror(errno), errno);};

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
       {nb = read_safe(fd, s, nc, FALSE);
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

	    nb = write_safe(fd, s, nc);

	    if ((nb >= 0) && (nb == nc))
	       {CLOG(cl, 2, "write %d ok (%d bytes sent)", fd, nb);}
	    else
	       CLOG(cl, 1, "write %d ng (%s - %d)",
		    fd, strerror(errno), errno);};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

# endif
#endif
