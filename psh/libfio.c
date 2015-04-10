/*
 * LIBFIO.C - routines supporting file I/O for PSH
 *          - mostly wrappers of NFS vulnerable operations
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBFIO

# define LIBFIO

# ifndef SCOPE_SCORE_PREPROC

/* define only for SCOPE_SCORE_COMPILE */

#ifndef EWOULDBLOCK
# define EWOULDBLOCK  EAGAIN
#endif

typedef struct s_ioerrdes ioerrdes;

struct s_ioerrdes
   {int n;
    int err;
    char *fnc;
    char *type;
    void *a;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOG_SAFE - track failures of the functions wrapped as xxx_safe */

#define NERR  64

void log_safe(char *fnc, int err, char *type, void *a)
   {ioerrdes *pe;
    static ioerrdes errev[NERR];
    static int ne = -1;

    if (ne == -1)
       {memset(errev, 0, sizeof(errev));
	ne = 0;};

/* report errors */
    if (strcmp(fnc, "dump") == 0)
       {int i, n;

	for (n = 0, i = ne; n < NERR; n++, i = (i + 1) % NERR)
	    {pe = errev + i;
	     if (pe->fnc != NULL)
	        {if (strcmp(pe->type, "char *") == 0)
		    printf("%4d %12s %4d %12s %s\n",
			   pe->n, pe->fnc, pe->err,
			   pe->type, (char *) pe->a);
		 else if (strcmp(pe->type, "int *") == 0)
		    printf("%4d %12s %4d %12s %d\n",
			   pe->n, pe->fnc, pe->err,
			   pe->type, *(int *) pe->a);
		 else
		    printf("%4d %12s %4d %12s %p\n",
			   pe->n, pe->fnc, pe->err,
			   pe->type, pe->a);};};}

/* record errors */
    else
       {pe = errev + ne;
	ne = (ne + 1) % NERR;

	pe->n    = ne;
	pe->err  = err;
	pe->fnc  = fnc;
	pe->type = type;
	pe->a    = a;};

    return;}

#undef NERR

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FOPEN_SAFE - make fopen safe for bad file systems
 *            - necessitated by networks that cannot keep up with CPUs
 */

FILE *fopen_safe(const char *path, const char *mode)
   {int ok, ev;
    FILE *rv;

    for (ok = TRUE; ok == TRUE; )
        {rv = fopen(path, mode);
	 ev = errno;
	 if (rv != NULL)
	    ok = FALSE;

/* these errors have a chance of being temporary */
	 else if ((ev == EINTR) ||
		  (ev == EAGAIN) ||
		  (ev == EWOULDBLOCK) ||
		  (ev == ENOSPC) ||
		  (ev == ETXTBSY))
	    {log_safe("fopen", ev, "char *", (char *) path);
	     ok = TRUE;}

/* these don't */
	 else
	    ok = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_SAFE - make open safe for bad file systems
 *           - necessitated by networks that cannot keep up with CPUs
 */

int open_safe(const char *path, int flags, mode_t mode)
   {int ok, ev, rv;

    for (ok = TRUE; ok == TRUE; )
        {rv = open(path, flags, mode);
	 ev = errno;
	 if (rv != -1)
	    ok = FALSE;

/* these errors have a chance of being temporary */
	 else if ((ev == EINTR) ||
		  (ev == EAGAIN) ||
		  (ev == EWOULDBLOCK) ||
		  (ev == ENOSPC) ||
		  (ev == ETXTBSY))
	    {log_safe("open", ev, "char *", (char *) path);
	     ok = TRUE;}

/* these don't */
	 else
	    ok = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_SAFE - make close safe for bad file systems
 *            - necessitated by networks that cannot keep up with CPUs
 */

int close_safe(int fd)
   {int ok, ev, rv;

    for (ok = TRUE; ok == TRUE; )
        {rv = close(fd);
	 ev = errno;
	 if (rv != -1)
	    ok = FALSE;

/* these errors have a chance of being temporary */
	 else if ((ev == EINTR) ||
		  (ev == EIO))
	    {log_safe("close", ev, "int *", &fd);
	     ok = TRUE;}

/* these don't */
	 else
	    ok = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FCLOSE_SAFE - make close safe for bad file systems
 *             - necessitated by networks that cannot keep up with CPUs
 */

int fclose_safe(FILE *fp)
   {int ok, ev, rv;

    for (ok = TRUE; ok == TRUE; )
        {rv = fclose(fp);
	 ev = errno;
	 if (rv != -1)
	    ok = FALSE;

/* these errors have a chance of being temporary */
	 else if ((ev == EINTR) ||
		  (ev == EAGAIN) ||
		  (ev == EWOULDBLOCK) ||
		  (ev == EIO))
	    {log_safe("fclose", ev, "FILE *", fp);
	     ok = TRUE;}

/* these don't */
	 else
	    ok = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FFLUSH_SAFE - make fflush safe for bad file systems
 *             - necessitated by networks that cannot keep up with CPUs
 */

int fflush_safe(FILE *fp)
   {int ok, ev, rv;

    for (ok = TRUE; ok == TRUE; )
        {rv = fflush(fp);
	 ev = errno;
	 if (rv != -1)
	    ok = FALSE;

/* these errors have a chance of being temporary */
	 else if ((ev == EINTR) ||
		  (ev == EWOULDBLOCK) ||
		  (ev == EAGAIN))
	    {log_safe("fflush", ev, "FILE *", fp);
	     ok = TRUE;}

/* these don't */
	 else
	    ok = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLOCK_FD - set the file descriptor to be blocked/unblocked
 *          - if ON is  1  blocked
 *          - if ON is  0  unblocked
 *          - if ON is -1  do not change status
 *          - return TRUE if original status was blocked
 *          - and FALSE if original status was unblocked
 *          - NOTE: on Linux O_NDELAY == O_NONBLOCK
 */

int block_fd(int fd, int on)
   {int ov, nv, rv, ev, zc, nz;

    nz = 10;
    for (zc = 0; zc < nz; zc++)
        {nv = -2;
	 ov = 0;
	 ov = fcntl(fd, F_GETFL, ov);
	 rv = ((ov & O_NDELAY) == 0);

/* block */
	 if (on == TRUE)
	    nv = fcntl(fd, F_SETFL, ov & ~O_NDELAY);

/* unblock */
	 else if (on == FALSE)
	    nv = fcntl(fd, F_SETFL, ov | O_NDELAY);

         if (nv == -1)
	    {ev = errno;
	     if ((ev == EACCES) ||
		 (ev == EAGAIN) ||
		 (ev == EINTR) ||
		 (ev == ENOLCK))
	        {log_safe("block_fd", ev, "int *", &fd);
	         sleep(1);
		 continue;};};

	 zc = nz;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_SAFE - make read more reliable
 *           - retry on recoverable errors
 *           - if REQ == TRUE ensure that the requested number of bytes
 *           - are read if at all possible
 *           - if REQ == FALSE read the bytes that are
 *           - available without error
 *           - return number of bytes read or -1
 */

ssize_t read_safe(int fd, void *s, size_t nb, int req)
   {int ev, blk, zc, nz, lwait;
    size_t ns;
    ssize_t n, nr;
    char *ps;

    blk = block_fd(fd, -1);
    zc  = 0;
    nz  = 10;

/* non-blocking read or terminal - take what you get
 * retry on errors but do not wait (i.e. sleep) for the bytes
 */
    if ((blk == FALSE) || (isatty(fd) == TRUE))
       lwait = FALSE;

/* blocking read - insist on the specified number of bytes or an error */
    else
       lwait = TRUE;

    ns = nb;
    nr = 0;
    ps = s;

    for (zc = 0; (ns > 0) && (zc < nz); zc++)
        {n  = read(fd, ps, ns);
	 ev = errno;
	 if (n < 0)

/* if the error is recoverable, sleep and try again */
	    {if ((ev == EAGAIN) ||
		 (ev == EWOULDBLOCK) ||
		 (ev == EINTR))
	        {if ((zc < nz-1) && (lwait == TRUE))
		    {log_safe("read", ev, "int *", &fd);
		     sleep(1);};}

/* if the error is unrecoverable, stop now */
	     else
	        zc = nz;}

	 else if (n > 0)
	    {zc  = (req == FALSE) ? nz : 0;
	     ps += n;
	     ns -= n;
	     nr += n;};};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREAD_SAFE - make fread more reliable
 *            - retry on recoverable errors
 *            - if REQ == TRUE ensure that the requested number of bytes
 *            - are read if at all possible
 *            - if REQ == FALSE read the bytes that are
 *            - available without error
 *            - return number of bytes read or -1
 */

size_t fread_safe(void *s, size_t bpi, size_t ni, FILE *fp, int req)
   {size_t zc, n, ns, nr;
    char *ps;

    zc = 0;
    ns = ni;
    nr = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n = fread(ps, bpi, ns, fp);
	if (ferror(fp) != 0)
	   {log_safe("fread", n, "FILE *", fp);
	    zc++;
	    sleep(1);}

	else if (n == 0)
	   zc++;

	else if (n > 0)
	   {zc = (req == TRUE) ? 0 : 10;
	    ps += bpi*n;
	    ns -= n;
	    nr += n;};};

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_SAFE - make write ensure that the requested number of bytes
 *            - is written if at all possible
 *            - return number of bytes written or -1
 */

ssize_t write_safe(int fd, const void *s, size_t nb)
   {int ev, zc;
    size_t ns;
    ssize_t n, nw;
    char *ps;

    zc = 0;
    ns = nb;
    nw = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n  = write(fd, ps, ns);
        ev = errno;
	if (n < 0)

/* if the error is recoverable, sleep and try again */
	   {if ((ev == EAGAIN) ||
		(ev == EWOULDBLOCK) ||
		(ev == EINTR))
	       {log_safe("write", ev, "int *", &fd);
	        zc++;
		sleep(10);}

/* if the error is unrecoverable, stop now */
	    else
	       {nw = -1;
                zc = 10;};}

/* no error but no bytes either */
	else if (n == 0)
	   zc++;

/* finally something read */
	else
	   {zc  = 0;
	    ps += n;
	    ns -= n;
	    nw += n;};};

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FWRITE_SAFE - make fwrite ensure that the requested number of bytes
 *             - is written if at all possible
 *             - return number of bytes written or -1
 */

size_t fwrite_safe(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t zc, n, ns, nw;
    char *ps;

    zc = 0;
    ns = nitems;
    nw = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n = fwrite(ps, bpi, ns, fp);
	if (ferror(fp) != 0)
	   {log_safe("fwrite", n, "FILE *", fp);
	    clearerr(fp);
	    sleep(10);};

	zc = (n == 0) ? zc + 1 : 0;

        if (n < ns)
           fflush_safe(fp);

	ps += bpi*n;
	ns -= n;
        nw += n;};

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNLINK_SAFE - make unlink ensure that the file is really removed
 *             - if at all possible
 *             - return 0 iff successful
 */

int unlink_safe(char *fmt, ...)
   {int i, ev, rv, na;
    char s[BFLRG];
    struct stat sb;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

/* maximum number of attempts */
    na = 100;

    for (i = 0, rv = -2; (i < na) && (rv != 0); i++)

/* try to unlink the file */
        {rv = unlink(s);
	 ev = errno;
	 switch (ev)

/* worth a retry */
	    {case EBUSY :
	     case EIO :
	          log_safe("unlink", ev, "char *", s);
	          break;

/* path or permissions problems will never work so bail */
	     default :
                  i = na;
                  continue;};

/* check the for the continued existence of the file */
	 rv = stat(s, &sb);
	 ev = errno;
         if (rv == 0)
	    {rv = -1;
             i  = na;
	     sched_yield();}
	 else
	    rv = 0;};

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WAIT_FS - wait for the file system to catch up and
 *         - flush the file FN to at least the expected length LN
 *         - make NA attempts
 *         - return the file size found
 */

int64_t wait_fs(char *fn, int64_t ln, int na)
   {int i;
    int64_t fsz;
    struct stat sbf;

/* maximum number of attempts */
    if (na < 1)
       na = 100;

    fsz = 0;
    for (i = 0; (fsz <= ln) && (i < na); i++)
        {if (stat(fn, &sbf) == 0)
	    fsz = sbf.st_size;
	 else
	    fprintf(stderr, "PARSE_CONN: Stat of %s failed - %s (%d)",
		    fn, strerror(errno), errno);
	 if (i > 0)
	    sleep(1);};

    if (fsz == 0)
       fprintf(stderr, "PARSE_CONN: Zero length conn %s\n", fn);

    return(fsz);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* undefine when passing out of SCOPE_SCORE_COMPILE */

#undef UNDEFINED

# endif
#endif
