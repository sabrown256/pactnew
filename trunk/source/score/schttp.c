/*
 * SCHTTP.C - HTTP routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONNECT - initiate connection to ADDR over socket FD
 *            - timeout after TO milliseconds
 *            - if TO is -1 let connect finish or timeout on its own
 *            - return the usual connect result
 *            - NOTE: this should be simply the connect system call
 *            - with a timeout determined by the application instead
 *            - some anonymous implementer somewhere
 */

int SC_connect(int fd, struct sockaddr *addr, socklen_t ln, int to)
   {int ok, blck, nrdy, dt, ta, sz, rv;
    SC_poll_desc pd;

/* do a blocking connect if an infinite timeout has been requested */
    if (to < 0)
       ok = connect(fd, (struct sockaddr *) addr, ln);

    else
       {pd.fd     = fd;
	pd.events = POLLOUT;

/* choose a good sampling interval */
	if (to < 100)
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
	    for (ta = 0; ta < to; ta += dt)
	        {nrdy = SC_poll(&pd, 1, dt);
		 if ((nrdy > 0) && (pd.revents & POLLOUT))
		    {sz = sizeof(int);
		     rv = getsockopt(fd, SOL_SOCKET, SO_ERROR,
				     &ok, (socklen_t *) &sz);
		     break;};};};

/* restore the socket status wrt blocking */
	if (blck == TRUE)
	   SC_block_fd(fd);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_PORT - open a connnection to HOST on PORT
 *              - timeout after TO milliseconds if connection cannot
 *              - be established
 *              - return the socket which connects to the server
 */

int SC_open_port(char *host, int port, int to)
   {int fd;

#ifdef HAVE_PROCESS_CONTROL

    int sasz, err, lport, ok;
    unsigned long haddr;
    struct sockaddr_in *srvr;

    if (host == NULL)
       return(-1);

    sasz = sizeof(long);

    fd = -1;
    ok = SC_ERR_TRAP();
    if (ok == 0)
       {lport = 0;
	lport = (port < 0) ? 80 : port;

	srvr = FMAKE(struct sockaddr_in, "SC_OPEN_PORT:srvr");
	srvr->sin_family = PF_INET;
	srvr->sin_port   = htons(lport);

	haddr = inet_addr(host);

#if defined(HAVE_GETHOSTBYNAME)
	if (haddr == (unsigned long) INADDR_NONE)
	   {struct hostent *hp;

	    hp = gethostbyname(host);
	    if (hp != NULL)
	       memcpy(&haddr, hp->h_addr, hp->h_length);
	    else
	       SC_error(-1, "CAN'T GET HOST BY NAME - SC_OPEN_PORT");};
#endif

	if (haddr == (unsigned long) INADDR_NONE)
	   SC_error(-1, "CAN'T GET SERVER ADDRESS - SC_OPEN_PORT");

	else
	   {memcpy(&(srvr->sin_addr), &haddr, sizeof(long));

	    fd = socket(PF_INET, SOCK_STREAM, 0);
	    if (fd < 0)
	       SC_error(-1, "CAN'T OPEN SOCKET - SC_OPEN_PORT");

	    sasz = sizeof(struct sockaddr_in);
	    err  = SC_connect(fd, (struct sockaddr *) srvr, sasz, to);
	    if (err < 0)
	       {close(fd);
		SC_error(-1, "CAN'T CONNECT TO SERVER (%d) - SC_OPEN_PORT",
			 errno);};};

	SFREE(srvr);};

    SC_ERR_UNTRAP();

#else
    fd = 0;
#endif

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SPLIT_HTTP - return the host and path part of a URI */

void SC_split_http(char *url, char *host, char *page)
   {char s[MAXLINE];
    char *ps, *t;

    if (strncmp(url, "http://", 7) == 0)
       ps  = url + 7;
    else
       ps = url;

    ps += (*ps == '/');
    strcpy(s, ps);

    t = strtok(s, "/\n");
    if (t != NULL)
       strcpy(host, t);

    t = strtok(NULL, "\n");
    if (t != NULL)
       strcpy(page, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_HTTP - open an HTTP connnection to HOST on PORT
 *              - return the socket which connects to the server
 */

int SC_open_http(char *host, int port)
   {int fd;

    fd = SC_open_port(host, port, DEFAULT_TIMEOUT);

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REQUEST_HTTP - issue http command, CMND, to http connection FD
 *                 - return TRUE iff the command is successfully sent
 */

int SC_request_http(int fd, char *cmnd, char *url, char *vers)
   {int ni, no, err;
    char page[MAXLINE], host[MAXLINE];
    char *s;

    SC_split_http(url, host, page);

    s = SC_dsnprintf(TRUE, "%s %s HTTP/%s\nHost: %s\n\n",
		     cmnd, url, vers, host);
    io_printf(stdout, "Request:\n%s", s);
    ni = strlen(s);

    no = SC_write_sigsafe(fd, s, ni);
    err = (ni != no);

    SFREE(s);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CLOSE_HTTP - close an HTTP connnection on FD */

void SC_close_http(int fd)
   {

    close(fd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HTTP_URL_FILE - write the contents at the ULR into the named FILE
 *                  - write it to stdout iff FILE is NULL
 *                  - also specify ascii representation of HTTP protocol
 *                  - version
 *                  - return TRUE iff successful
 */

int SC_http_url_file(char *url, char *file, char *vrs)
   {int fd, nb, err;
    char vers[MAXLINE], bf[MAXLINE];
    char host[MAXLINE], page[MAXLINE];
    FILE *fp;

    err = FALSE;

    if (vrs == NULL)
       strcpy(vers, "1.1");
    else
       strcpy(vers, vrs);

    SC_split_http(url, host, page);

    if (file == NULL)
       fp = stdout;
    else
       fp = io_open(file, "w");

    fd = SC_open_http(host, -1);
    if (fd < 1)
       {io_printf(stdout, "%s\n", SC_error_msg());
	io_printf(stdout, "CAN'T CONNECT TO %s\n", host);}

    else
       {err = SC_request_http(fd, "GET", url, vers);
        if (!err)
	   {while (TRUE)
	       {nb = SC_read_sigsafe(fd, bf, MAXLINE);
		if (nb != 0)
                   io_write(bf, 1, nb, fp);
		else
		   break;};

	    err = TRUE;};

	SC_close_http(fd);};

    io_close(fp);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
