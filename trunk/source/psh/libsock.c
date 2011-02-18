/*
 * LIBSOCK.C - routines to manage client/server connection via sockets
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/socket.h>

#ifndef INADDR_NONE
# define INADDR_NONE NULL
#endif

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

/* NAME_SOCK - make conventional socket name from ROOT and channel
 *           - .../foo -> .../foo.host@port
 */

static char *name_sock(char *root, int ch)
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

    na = 3;
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

/* SOCK_EXISTS - return TRUE iff SOCK exists */

static int sock_exists(char *fmt, ...)
   {int rv;
    char s[MAXLINE];
    char *sock, *wh;

    rv = FALSE;

    if (fmt != NULL)
       {VA_START(fmt);
	VSNPRINTF(s, MAXLINE, fmt);
	VA_END;

	sock = name_sock(s, -1);
	rv   = (sock != NULL);

	if (srv.server == 0)
	   wh = "CLIENT";
	else
	   wh = "SERVER";

#ifdef VERBOSE
	{char *flog;
	 flog = name_log(s);
	 log_activity(flog, dbg_sock, wh, "exist |%s| (%s)",
		      sock,
		      (rv == TRUE) ? "yes" : "no");};
#endif
       };

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SERVER - make a connection with the server */

static int init_server(char *root)
   {int ip, port, sasz, rv, fd;
    char s[MAXLINE], host[MAXLINE];
    char *hst;

    sasz = sizeof(struct sockaddr_in);
    port = -1;
    rv   = FALSE;

    srv.server = socket(PF_INET, SOCK_STREAM, 0);
    if (srv.server > 0)
       {srv.sck.sin_family      = PF_INET;
	srv.sck.sin_addr.s_addr = htonl(INADDR_ANY);

	for (ip = 15000; ip < 65000; ip++)
	    {srv.sck.sin_port = htons(ip);

	     if (bind(srv.server, (struct sockaddr *) &srv.sck, sasz) >= 0)
	        {port = ip;
		 break;};};};

    srv.port = port;
    if (srv.port >= 0)
       {gethostname(host, MAXLINE);
	hst = strtok(host, ".\n");
        snprintf(s, MAXLINE, "%s.%s@%d", root, hst, srv.port);
	fd = open(s, O_WRONLY | O_CREAT, 0600);
        close(fd);

	rv = TRUE;};

/* initialize the set of active sockets */
    FD_ZERO(&srv.afs);
    FD_SET(srv.server, &srv.afs);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_SERVER - close the server socket down */

static void fin_server()
   {

    close(srv.server);
    srv.server = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_SERVER - make a connection with the server */

static int connect_server(client *cl)
   {int fd, err, sasz;
    char *root, *flog;
    socklen_t len;
    struct sockaddr *sa;
    connection *srv;

    root = cl->root;
    srv  = cl->server;

    flog = name_log(root);

    fd   = -1;
    sasz = sizeof(struct sockaddr_in);
    len  = sasz;

    sa = (struct sockaddr *) &srv->sck;

    if (srv->port >= 0)
       {getsockname(srv->server, sa, &len);

	err = listen(srv->server, 5);
	if (err >= 0)
	   {fd = accept(srv->server, sa, &len);
	    if (fd >= 0)
	       log_activity(flog, dbg_sock, "SERVER", "listening %d", fd);
	    else
	       log_activity(flog, dbg_sock, "SERVER", "accept error %d - s",
			    errno, strerror(errno));}
	else
	   log_activity(flog, dbg_sock, "SERVER", "listen error %d - %s",
			errno, strerror(errno));};

    cl->fd   = fd;
    cl->type = SERVER;

    return(fd);}

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

    res = name_sock(root, -1);
    if (res != NULL)
       {nstrncpy(s, MAXLINE, path_tail(res), -1);
	res = strrchr(s, '.') + 1;
	key_val(&host, &prt, res, "@");
	port = atoi(prt);

	if ((host != NULL) && (port > 0))
	   {sasz = sizeof(struct sockaddr_in);

	    memset(&srv->sck, 0, sasz);
	    srv->sck.sin_family = PF_INET;
	    srv->sck.sin_port   = htons(port);

	    haddr = inet_addr(host);

	    if (haddr == INADDR_NONE)
	       {int sz;
		struct hostent *hp;

		hp = gethostbyname(host);
		if (hp != NULL)
		   {sz = sizeof(in_addr_t);
		    memcpy(&haddr, hp->h_addr, sz);};};

	    if (haddr != INADDR_NONE)
	       {memcpy(&srv->sck.sin_addr, &haddr, sizeof(long));

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd >= 0)
		   {err = connect(fd, (struct sockaddr *) &srv->sck, sasz);

/*#ifdef VERBOSE*/
#if 1
		    {char *flog;
		     flog = name_log(root);
		     log_activity(flog, dbg_sock,
				  "CLIENT", "connect %s@%s  %d  %s (%d)",
				  host, prt, fd,
				  strerror(errno), errno);};
#endif
		    if (err < 0)
		       {close(fd);
			fd = -1;};

#ifdef NEWWAY
		    srv->server = fd;
#endif
		    srv->port   = port;};};};};

    cl->fd   = fd;
    cl->type = CLIENT;

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_SOCK - initialize the SOCK */

static int open_sock(char *root)
   {int rv;
    char *flog, *wh;

    rv = TRUE;
    rv = init_server(root);
    rv = sock_exists(root);

    if (srv.server == 0)
       wh = "CLIENT";
    else
       wh = "SERVER";

    flog = name_log(root);
    log_activity(flog, dbg_sock, wh, "open %d", rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_SOCK - close the SOCK */

static int close_sock(char *root)
   {int st, rv;
    char *sock, *flog, *wh;

    rv = TRUE;

    sock = name_sock(root, -1);
    st   = unlink(sock);
    rv  &= (st == 0);

    fin_server();

    if (srv.server == 0)
       wh = "CLIENT";
    else
       wh = "SERVER";

    flog = name_log(root);
    log_activity(flog, dbg_sock, wh, "close %d", st);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_SOCK - read from the socket ROOT into S */

static int read_sock(client *cl, char *s, int nc)
   {int nb, fd;
    char *flog, *wh, *root;
    connection *srv;

    root = cl->root;
    fd   = cl->fd;
    srv  = cl->server;

    flog = name_log(root);

    if (fd == -1)
       {if (srv->server == 0)
	   fd = connect_client(cl);
        else
	   fd = connect_server(cl);};

    if (cl->type == CLIENT)
       wh = "CLIENT";
    else
       wh = "SERVER";

    if (fd < 0)
       {nb = -1;
	log_activity(flog, dbg_sock, wh, "read - no db");}

    else
       {nb = read(fd, s, nc);

#ifndef NEWWAY
	close(fd);
	cl->fd = -1;
#endif

	if (s[nb] != '\0')
	   s[nb] = '\0';

	if (nb < 0)
	   log_activity(flog, dbg_sock, wh, "read |%s| (%s)",
			s, strerror(errno));
	else if (nb > 0)
	   log_activity(flog, dbg_sock, wh, "read |%s| (%d)", s, nb);};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_SOCK - write to the socket ROOT from S */

static int write_sock(client *cl, char *s, int nc)
   {int nb, fd;
    char *flog, *wh, *root;
    connection *srv;

    root = cl->root;
    fd   = cl->fd;
    srv  = cl->server;

    if (nc <= 0)
       nc = strlen(s) + 1;

    flog = name_log(root);

    if (fd == -1)
       {if (srv->server == 0)
	   fd = connect_client(cl);
        else
	   fd = connect_server(cl);};

    if (cl->type == CLIENT)
       wh = "CLIENT";
    else
       wh = "SERVER";

    if (fd < 0)
       {nb = -1;
	log_activity(flog, dbg_sock, wh, "write - no db");}

    else
       {nb = write(fd, s, nc);

#ifndef NEWWAY
	close(fd);
	cl->fd = -1;
#endif

	if ((nb < 0) || (nb != nc))
	   log_activity(flog, dbg_sock, wh, "write |%s| (%s)",
			s, strerror(errno));
	else
	   log_activity(flog, dbg_sock, wh, "write |%s| (%d)", s, nb);};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

