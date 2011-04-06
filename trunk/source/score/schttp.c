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

/* SC_OPEN_PORT - open a connnection to HOST on PORT
 *              - timeout after TO milliseconds if connection cannot
 *              - be established
 *              - return the socket which connects to the server
 */

int SC_open_port(char *host, int port, int to, int fm)
   {int fd;

#ifdef HAVE_PROCESS_CONTROL

     int lport;

     lport = (port < 0) ? 80 : port;

     fd = _SC_tcp_connect(host, lport, to, fm);
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

    fd = SC_open_port(host, port, DEFAULT_TIMEOUT, FALSE);

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

    CFREE(s);

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
