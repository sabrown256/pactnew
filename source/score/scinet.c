/*
 * SCINET.C - TCP/IP interface routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

typedef struct s_tcp_loop tcp_loop;

struct s_tcp_loop
   {void *a;
    sckades ad;
    SC_evlpdes *pe;
    PFFileCallback acc;
    PFFileCallback rej;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DSOCKET - diagnostic print of socket info */

void dsocket(int s)
   {

#ifdef HAVE_SOCKETS

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

/* _SC_TCP_ADDRESS - allocate and initialize a socket address
 *                 - to connect to HOST on PORT
 */

sckades _SC_tcp_address(char *host, int port)
   {sckades ad;

    ad.in = NULL;

#if defined(HAVE_POSIX_SYS)

    ad = PS_tcp_get_address(host, port, INADDR_NONE);

#endif

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_BIND - bind the socket FD to PORT
 *              - return the socket address of the bound descriptor
 *              - if PORT == -1 search for a port
 */

sckades _SC_tcp_bind(int fd, int port)
   {sckades ad;

    ad.in = NULL;

#if defined(HAVE_POSIX_SYS)

    int ok;

    ad = PS_tcp_get_address(NULL, 0, INADDR_ANY);
    if (ad.in != NULL)
       {ok = PS_tcp_bind_port(fd, ad, port, 5000);
	if (ok >= 0)
	   _SC_diagnostic("      Bind succeeded: %d\n", port);
	else
	   CFREE(ad.in);};

#endif

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_ACCEPTOR - register newly accepted connections on FD */

static void _SC_tcp_acceptor(int fd, int mask, void *a)
   {

#if defined(HAVE_POSIX_SYS)

    int nfd, pi;
    sckades ad;
    PFFileCallback acc;
    PFFileCallback rej;
    SC_evlpdes *pe;
    tcp_loop *lp;

    lp  = (tcp_loop *) a;
    pe  = lp->pe;
    ad  = lp->ad;
    acc = lp->acc;
    rej = lp->rej;

    nfd = PS_tcp_accept_connection(fd, ad, TRUE);
    if (nfd > 0)
       {pi = SC_register_event_loop_callback(pe, SC_INT_I, &nfd,
					     acc, rej, -1);
	SC_ASSERT(pi != 0);}
    else
       close(nfd);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_SERVE - listen on bound socket (FD, AD)
 *               - accept connections and run an event loop on them
 *               - this is the core of a server
 */
 
int _SC_tcp_serve(int fd, sckades ad, void *a,
		  int (*ex)(int *rv, void *a),
		  PFFileCallback acc, PFFileCallback rej)
   {int ok;

    ok = -1;

#if defined(HAVE_POSIX_SYS)

    int pi;
    SC_evlpdes *pe;
    tcp_loop lp;

    if (listen(fd, 5) >= 0)

/* create the event loop state */
       {pe = SC_make_event_loop(NULL, NULL, ex, -1, -1, -1);

/* register the I/O channels for the event loop to monitor */
	pi = SC_register_event_loop_callback(pe, SC_INT_I, &fd,
					     _SC_tcp_acceptor, NULL, -1);

/* if all channels are OK activate the interrupt handling */
	SC_gs.io_interrupts_on = pi;
	if (pi)
	   SC_catch_event_loop_interrupts(pe, SC_gs.io_interrupts_on);

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

int _SC_tcp_accept_connection(int fd, sckades ad)
   {int nfd;
    
    nfd = -1;

#if defined(HAVE_POSIX_SYS)

    nfd = PS_tcp_accept_connection(fd, ad, FALSE);

#endif

    return(nfd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CONNECT_TO - initiate connection to AD
 *                - timeout after TO milliseconds
 *                - if TO is -1 let connect finish or timeout on its own
 *                - return the socket iff successful
 *                - if FM is TRUE timeout is non-fatal and
 *                - ETIMEDOUT is returned
 *                - NOTE: this should be simply the connect system call
 *                - with a timeout determined by the application instead
 *                - some anonymous implementer somewhere
 */

static int _SC_connect_to(sckades ad, int to, int fm)
   {int fd, ok;

    fd = -1;

#if defined(HAVE_POSIX_SYS)

    int blck, nrdy, dt, ta, rv;
    socklen_t ln, sz;
    SC_poll_desc pd;

/* do a blocking connect if an infinite timeout has been requested */
    if (to < 0)
       fd = PS_tcp_initiate_connection(ad);

    else

/* choose a good sampling interval */
       {if (to < 100)
	   dt = to / 16;
	else if (to < 10000)
	   dt = 100;
	else
	   dt = 1000;

	dt = max(dt, 1);

	fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	   SC_error(-1, "CAN'T OPEN SOCKET - _SC_CONNECT_TO");
		
/* find out whether the socket is already blocked */
	blck = SC_isblocked_fd(fd);
        SC_unblock_fd(fd);

/* do the non-blocking connect */
	SOCKADDR_SIZE(ln);
	ok = connect(fd, ad.uq, ln);
	if ((ok == -1) && (errno == EINPROGRESS))
	   {ok = ETIMEDOUT;

/* wait for the connection to be made */
	    pd.fd     = fd;
	    pd.events = POLLOUT;
	    for (ta = 0; ta < to; ta += dt)
	        {nrdy = SC_poll(&pd, 1, dt);
		 if ((nrdy > 0) && (pd.revents & POLLOUT))
		    {sz = sizeof(int);
		     rv = getsockopt(fd, SOL_SOCKET, SO_ERROR, &ok, &sz);
		     SC_ASSERT(rv == 0);
		     break;};};

	    if (fm == FALSE)
	       fd = (ok != 0) ? -1 : fd;};

/* restore the socket status wrt blocking */
	if (blck == TRUE)
	   SC_block_fd(fd);};

#endif

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TCP_CONNECT - initialize a TCP connection to HOST on PORT
 *                 - return the socket which connects to the server
 *                 - timeout after TO milliseconds
 *                 - if FM is TRUE timeout is non-fatal
 */

int _SC_tcp_connect(char *host, int port, int to, int fm)
   {int fd;

    fd = -1;

#if defined(HAVE_POSIX_SYS)

    int ok;

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {if ((host != NULL) && (port >= 0))
	   {sckades srva;

	    srva = _SC_tcp_address(host, port);
	    if (srva.in != NULL)
	       {fd = _SC_connect_to(srva, to, fm);
		if (fd < 0)
		   SC_error(-1, "CAN'T CONNECT TO SERVER (%d) - _SC_TCP_CONNECT",
			    errno);};

	    CFREE(srva.in);};};

    SC_ERR_UNTRAP();

#endif

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
