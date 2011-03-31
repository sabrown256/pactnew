/*
 * SCINET.C - TCP/IP interface routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

typedef struct s_tcp_loop tcp_loop;

struct s_tcp_loop
   {void *a;
    struct sockaddr_in *ad;
    SC_evlpdes *pe;
    PFFileCallback acc;
    PFFileCallback rej;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DSOCKET - diagnostic print of socket info */

void dsocket(int s)
   {

#ifdef HAVE_SOCKETS_P

    int ok;
    socklen_t iv, nb;

# ifdef SO_DEBUG
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_DEBUG, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Socket debugging: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_REUSEADDR
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_REUSEADDR, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Address reuse: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_REUSEPORT
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_REUSEPORT, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Port reuse: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_KEEPALIVE
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Keep alive: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_DONTROUTE
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_DONTROUTE, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Routing bypass: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_LINGER
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_LINGER, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Linger on close: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_BROADCAST
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_BROADCAST, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Permission to broadcast: %d (%d)\n",
	     (int) iv, (int) nb);
# endif

# ifdef SO_OOBINLINE
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_OOBINLINE, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Out-of-band reception: %d (%d)\n",
	     (int) iv, (int) nb);
# endif

# ifdef SO_SNDBUF
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_SNDBUF, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Output buffer size: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_RCVBUF
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_RCVBUF, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Input buffer size: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_SNDLOWAT
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_SNDLOWAT, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Output threshold: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_RCVLOWAT
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_RCVLOWAT, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Input threshold: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_SNDTIMEO
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Output timeout: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_RCVTIMEO
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Input timeout: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_TYPE
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_TYPE, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Socket type: %d (%d)\n", (int) iv, (int) nb);
# endif

# ifdef SO_NOSIGPIPE
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Return EPIPE instead of SIGPIPE: %d (%d)\n",
	     (int) iv, (int) nb);
# endif

# ifdef SO_ERROR
    nb = sizeof(socklen_t);
    ok = getsockopt(s, SOL_SOCKET, SO_ERROR, &iv, &nb);
    if (ok == 0)
       io_printf(stdout, "Socket error: %d (%d)\n", (int) iv, (int) nb);
# endif

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_ADDRESS - allocate and initialize a sockaddr
 *                 - to connect to HOST on PORT
 */

struct sockaddr_in *_SC_tcp_address(char *host, int port)
   {struct sockaddr_in *ad;

    ad = NULL;

#ifdef HAVE_PROCESS_CONTROL

    in_addr_t haddr;

    haddr = inet_addr(host);

#if defined(HAVE_GETHOSTBYNAME)
    if (haddr == INADDR_NONE)
       {struct hostent *hen, *hea;

	hen = gethostbyname(host);
	if (hen != NULL)
	   {in_addr_t nad;

	    nad = *(in_addr_t *) hen->h_addr_list[0];

#if 0
/* diagnostic print host name and IP address */
	    {char *sad;
	     struct in_addr iad;

	     iad.s_addr = nad;
	     sad = inet_ntoa(iad);

             printf("%s -> %s\n", host, sad);};
#endif

/* verify that host associated with the address matches the specified host
 * things like OpenDNS give you a special address for unknown hosts
 * which will not match your original host
 */
	    hea = gethostbyaddr(&nad, sizeof(nad), AF_INET);
	    if (strcmp(hea->h_name, hen->h_name) == 0)
	       haddr = nad;};};
#endif

    if (haddr != INADDR_NONE)
       {ad = FMAKE(struct sockaddr_in, "SC_TCP_ADDRESS:ad");
	ad->sin_family = PF_INET;
	ad->sin_port   = htons(port);

	memcpy(&ad->sin_addr, &haddr, sizeof(in_addr_t));};

#endif

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_BIND - bind the socket FD to PORT
 *              - return the sockaddr of the bound descriptor
 *              - if PORT == -1 search for a port
 */

struct sockaddr_in *_SC_tcp_bind(int fd, int port)
   {int ok, sasz;
    struct sockaddr_in *ad;

    ad = NULL;

#ifdef HAVE_PROCESS_CONTROL

    ad = FMAKE(struct sockaddr_in, "_SC_TCP_BIND:ad");
    if (ad != NULL)
       {ad->sin_family      = PF_INET;
	ad->sin_addr.s_addr = htonl(INADDR_ANY);

	ok = -1;

	sasz = sizeof(struct sockaddr_in);

	if (port > 0)
	   {ad->sin_port = htons(port);
	    ok = bind(_SC.sfd, (struct sockaddr *) ad, sasz);}

	else
	   {for (port = 5000; port < 65000; port++)
	        {ad->sin_port = htons(port);
		 ok = bind(_SC.sfd, (struct sockaddr *) ad, sasz);
		 if (ok >= 0)
		    break;};};

	if (ok >= 0)
	   {if (_SC_debug)
	       {fprintf(_SC_diag, "      Bind succeeded: %d\n", port);
		fflush(_SC_diag);};}
	else
	   {SFREE(ad);};};

#endif

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_ACCEPTOR - register newly accepted connections on FD */

static void _SC_tcp_acceptor(int fd, int mask, void *a)
   {

#ifdef HAVE_PROCESS_CONTROL

    int nfd, pi;
    socklen_t sasz;
    struct sockaddr_in *ad;
    PFFileCallback acc;
    PFFileCallback rej;
    SC_evlpdes *pe;
    tcp_loop *lp;

    lp  = (tcp_loop *) a;
    pe  = lp->pe;
    ad  = lp->ad;
    acc = lp->acc;
    rej = lp->rej;

    sasz = sizeof(struct sockaddr_in);

    nfd = accept(fd, (struct sockaddr *) ad, &sasz);
    if (nfd > 0)
       {pi = SC_register_event_loop_callback(pe, SC_INT_I, &nfd,
					     acc, rej, -1);
	SC_ASSERT(pi != 0);};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_SERVE - listen on bound socket (FD, AD)
 *               - accept connections and run an event loop on them
 *               - this is the core of a server
 */
 
int _SC_tcp_serve(int fd, struct sockaddr_in *ad, void *a,
		  int (*ex)(int *rv, void *a),
		  PFFileCallback acc, PFFileCallback rej)
   {int pi, ok;
    SC_evlpdes *pe;
    tcp_loop lp;

    ok = -1;

#ifdef HAVE_PROCESS_CONTROL

    if (listen(fd, 5) >= 0)

/* create the event loop state */
       {pe = SC_make_event_loop(NULL, NULL, ex, -1, -1, -1);

/* register the I/O channels for the event loop to monitor */
	pi = SC_register_event_loop_callback(pe, SC_INT_I, &fd,
					     _SC_tcp_acceptor, NULL, -1);

/* if all channels are OK activate the interrupt handling */
	SC_io_interrupts_on = pi;
	if (pi)
	   SC_catch_event_loop_interrupts(pe, SC_io_interrupts_on);

	lp.a   = a;
	lp.ad  = ad;
	lp.pe  = pe;
	lp.acc = acc;
	lp.rej = rej;

	ok = SC_event_loop(pe, &lp, -1);

	SC_free_event_loop(pe);};

#endif

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_ACCEPT_CONNECTION - listen on bound socket FD, AD
 *                           - and return the descriptor of an accepted
 *                           - connection
 */

int _SC_tcp_accept_connection(int fd, struct sockaddr_in *ad)
   {int err, nfd;
    socklen_t sasz;
    struct sockaddr *sa;
    
    nfd = -1;

#ifdef HAVE_PROCESS_CONTROL

    sa   = (struct sockaddr *) ad;
    sasz = sizeof(struct sockaddr_in);

    getsockname(fd, sa, &sasz);

    err = listen(fd, 5);
    if (err >= 0)
       nfd = accept(fd, sa, &sasz);

#endif

    return(nfd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CONNECT_TO - initiate connection to ADDR over socket FD
 *                - timeout after TO milliseconds
 *                - if TO is -1 let connect finish or timeout on its own
 *                - return the usual connect result
 *                - NOTE: this should be simply the connect system call
 *                - with a timeout determined by the application instead
 *                - some anonymous implementer somewhere
 */

static int _SC_connect_to(int fd, struct sockaddr *addr, socklen_t ln, int to)
   {int ok, blck, nrdy, dt, ta, sz, rv;
    SC_poll_desc pd;

    ok = -1;

#ifdef HAVE_PROCESS_CONTROL

/* do a blocking connect if an infinite timeout has been requested */
    if (to < 0)
       ok = connect(fd, (struct sockaddr *) addr, ln);

    else

/* choose a good sampling interval */
       {if (to < 100)
	   dt = to / 16;
	else if (to < 10000)
	   dt = 100;
	else
	   dt = 1000;

	dt = max(dt, 1);

/* find out whether the socket is already blocked */
	blck = SC_isblocked_fd(fd);
        SC_unblock_fd(fd);

/* do the non-blocking connect */
	ok = connect(fd, (struct sockaddr *) addr, ln);
	if ((ok == -1) && (errno == EINPROGRESS))
	   {ok = ETIMEDOUT;

/* wait for the connection to be made */
	    pd.fd     = fd;
	    pd.events = POLLOUT;
	    for (ta = 0; ta < to; ta += dt)
	        {nrdy = SC_poll(&pd, 1, dt);
		 if ((nrdy > 0) && (pd.revents & POLLOUT))
		    {sz = sizeof(int);
		     rv = getsockopt(fd, SOL_SOCKET, SO_ERROR,
				     &ok, (socklen_t *) &sz);
		     SC_ASSERT(rv == 0);
		     break;};};

	    ok = (ok != 0) ? -1 : 0;};

/* restore the socket status wrt blocking */
	if (blck == TRUE)
	   SC_block_fd(fd);};

#endif

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_CONNECT - initialize a TCP connection to HOST on PORT
 *                 - return the socket which connects to the server
 *                 - timeout after TO milliseconds
 */

int _SC_tcp_connect(char *host, int port, int to)
   {int fd;

    fd = -1;

#ifdef HAVE_PROCESS_CONTROL

    int ok;

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {if ((host != NULL) && (port >= 0))
	   {int sasz, err;
	    struct sockaddr_in *srvr;

	    sasz = sizeof(in_addr_t);
	    srvr = _SC_tcp_address(host, port);
	    if (srvr != NULL)
	       {fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd < 0)
		   SC_error(-1, "CAN'T OPEN SOCKET - _SC_TCP_CONNECT");

		sasz = sizeof(struct sockaddr_in);
		err  = _SC_connect_to(fd, (struct sockaddr *) srvr, sasz, to);
		if (err < 0)
		   {close(fd);
		    fd = -1;
		    SC_error(-1, "CAN'T CONNECT TO SERVER (%d) - _SC_TCP_CONNECT",
			      errno);};};

	    SFREE(srvr);};};

    SC_ERR_UNTRAP();

#endif

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
