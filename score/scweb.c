/*
 * SCWEB.C - HTTP routines test driver
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - get and print a web page
 *      -
 *      - Usage: scweb <url>
 *      -
 *      - where <url> is a Uniform Resource Locator
 *      -
 *      - e.g. http://www.htmlhelp.com/reference/wilbur/
 */

int main(c, v)
   int c;
   char **v;
   {int i, fd, err, nb;
    char bf[MAXLINE], vers[MAXLINE];
    char host[MAXLINE], page[MAXLINE];
    char *url;

    url = NULL;
    SC_strncpy(vers, MAXLINE, "1.1", -1);

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'v' :
		      SC_strncpy(vers, MAXLINE, v[++i], -1);
		      break;};}
         else
            url = v[i];};

    SC_split_http(url, MAXLINE, host, page);

    fd = SC_open_http(host, -1);
    if (fd < 1)
       {PRINT(stdout, "%s\n", SC_error_msg());
	PRINT(stdout, "CAN'T CONNECT TO %s\n", host);}

    else
       {err = SC_request_http(fd, "GET", url, vers);
        if (!err)
	   {while (TRUE)
	       {nb = SC_read_sigsafe(fd, bf, MAXLINE);
		if (nb != 0)
                   io_write(bf, 1, nb, stdout);
		else
		   break;};};

	SC_close_http(fd);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
