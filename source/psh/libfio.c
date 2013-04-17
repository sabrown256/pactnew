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
	    ok = TRUE;

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
	    ok = TRUE;

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
	    ok = TRUE;

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
	    ok = TRUE;

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
		  (ev == EAGAIN) ||
		  (ev == EWOULDBLOCK))
	    ok = TRUE;

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
	        {sleep(1);
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
   {int ev, blk, zc, nz, wait;
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
       wait = FALSE;

/* blocking read - insist on the specified number of bytes or an error */
    else
       wait = TRUE;

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
		{if ((zc < nz-1) && (wait == TRUE))
		    sleep(1);}

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
	   {zc++;
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
	       {zc++;
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
	   {clearerr(fp);
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

int unlink_safe(char *s)
   {int i, ev, rv, na;
    struct stat sb;

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

/* undefine when passing out of SCOPE_SCORE_COMPILE */

#undef UNDEFINED

# endif
#endif
