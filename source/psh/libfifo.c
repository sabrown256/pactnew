/*
 * LIBFIFO.C - routines to manage FIFO's
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBFIFO

#define LIBFIFO

#define WHICH_PROC()       ((fifost.ioc == CLIENT) ? "CLIENT" : "SERVER")

typedef struct s_fifo_connection fifo_connection;

struct s_fifo_connection
   {int debug;
    ckind ioc;};

static fifo_connection
 fifost = { FALSE, CLIENT };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NAME_FIFO - make conventional FIFO name from ROOT and channel
 *           - .../foo -> .../foo.0.=
 */

static char *name_fifo(char *root, int ch)
   {char *p;
    static char fifo[BFLRG];

    p = NULL;

    if (root == NULL)
       root = cgetenv(TRUE, "PERDB_PATH");

    if (root != NULL)
       {snprintf(fifo, BFLRG, "%s.%d.=", root, ch);
	p = fifo;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIFO_EXISTS - return TRUE iff FIFO exists */

static int fifo_exists(char *fmt, ...)
   {int ch, rv, ok;
    char s[BFLRG];
    char *fifo, *flog, *st, *wh;
    struct stat sb;

    rv = FALSE;

    if (fmt != NULL)
       {st = "no file";

	VA_START(fmt);
	VSNPRINTF(s, BFLRG, fmt);
	VA_END;

	rv = TRUE;
        for (ch = 0; ch < 2; ch++)
	    {fifo = name_fifo(s, ch);
	     if (stat(fifo, &sb) == 0)
	        {ok  = ((sb.st_mode & S_IFIFO) != 0);
		 st  = (ok == TRUE) ? "is fifo" : "file exists";
		 rv &= (ok == TRUE);}};

	wh = WHICH_PROC();

	flog = name_log(s);
	log_activity(flog, fifost.debug, 1, wh, "exist |%s| (%s)", fifo, st);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_FIFO - initialize the FIFO
 *           - analog of open_server in libsock.c
 */

static int open_fifo(char *root, ckind ioc, int auth)
   {int ch, st, rv;
    char *fifo, *flog, *wh;

    rv = TRUE;

    fifost.ioc = ioc;

    for (ch = 0; ch < 2; ch++)
        {fifo = name_fifo(root, ch);

	 unlink_safe(fifo);
	 st  = mkfifo(fifo, 0600);
	 rv &= (st == 0);};

    rv = fifo_exists(root);

    wh = WHICH_PROC();

    flog = name_log(root);
    log_activity(flog, fifost.debug, 1, wh, "open %d", rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_FIFO - close the FIFO */

static int close_fifo(char *root)
   {int ch, st, rv;
    char *fifo, *flog, *wh;

    rv = TRUE;

    for (ch = 0; ch < 2; ch++)
        {fifo = name_fifo(root, ch);
	 st   = unlink_safe(fifo);
	 rv  &= (st == 0);};

    wh = WHICH_PROC();

    flog = name_log(root);
    log_activity(flog, fifost.debug, 1, wh, "close %d", st);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_FIFO - read from the FIFO ROOT into S */

static int read_fifo(char *root, int ch, char *s, int nc)
   {int fd, nb;
    char *fifo, *flog, *wh;

    flog = name_log(root);
    fifo = name_fifo(root, ch);
    fd   = open(fifo, O_RDONLY, 0400);

    wh = WHICH_PROC();

    if (fd < 0)
       {nb = -1;
	log_activity(flog, fifost.debug, 1, wh, "read - no db");}

    else
       {nb = read_safe(fd, s, nc, FALSE);
	close(fd);

/* guarantee NULL termination */
	s[nc-1] = '\0';

	if (s[nb] != '\0')
	   s[nb] = '\0';

	if (nb < 0)
	   log_activity(flog, fifost.debug, 1, wh, "read |%s| (%s)",
			s, strerror(errno));
	else
	   log_activity(flog, fifost.debug, 1, wh, "read |%s| (%d)", s, nb);};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FIFO - write to the FIFO ROOT from S */

static int write_fifo(char *root, int ch, char *s, int nc)
   {int fd, nb;
    char *fifo, *flog, *wh;

    nb = -1;
    if ((root != NULL) && (s != NULL))
       {if (nc <= 0)
	   nc = strlen(s) + 1;

	flog = name_log(root);
	fifo = name_fifo(root, ch);
	fd   = open(fifo, O_WRONLY, 0200);

	wh = WHICH_PROC();

	if (fd < 0)
	   log_activity(flog, fifost.debug, 1, wh, "write no db");

	else
	   {nb = write_safe(fd, s, nc);
	    close(fd);

	    if (nb < 0)
	       log_activity(flog, fifost.debug, 1, wh, "write |%s| (%s)",
			    s, strerror(errno));
	    else
	       log_activity(flog, fifost.debug, 1, wh, "write |%s| (%d)",
			    s, nb);};};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef WHICH_PROC

#endif
