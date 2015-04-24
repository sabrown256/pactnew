/*
 * SCLPPSX.C - low level process control - POSIX routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

#if defined(HAVE_POSIX_SYS)

#include "scope_proc.h"

#undef NONBLOCK
#define NONBLOCK   O_NDELAY

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_TERMINAL_PROCESS - return a pointer to _SC.terminal */

PROCESS *SC_get_terminal_process(void)
   {

    return(_SC.terminal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_RELEASE - close the file descriptors */

static int _SC_posix_release(PROCESS *pp)
   {int rv;

    rv = TRUE;

#ifdef HAVE_PROCESS_CONTROL

    if (!SC_process_alive(pp))
       rv = FALSE;

#if defined(HAVE_POSIX_SYS)

    else
       {int i, ex, cnd, rsn, sts;
	int io[SC_N_IO_CH];

	sts = pp->flags & SC_PROC_IO;

/* because it is possible to get in here many times with the same process
 * make sure that we do the following only once
 */
	if (sts == 0)
	   {for (i = 0; i < SC_N_IO_CH; i++)
	        io[i] = pp->io[i];

/* check to see that the process has indeed exited */
	    if (pp->ischild == FALSE)
	       {ex = SC_child_status(pp->id, &cnd, &rsn);
	        rv = (ex == pp->id);};

/* stdin */
	    if (SC_time_allow(1) == 0)
	       {if (io[0] >= 0)
		   SC_close_safe(io[0]);
		SC_time_allow(0);};

/* stdout */
	    if ((io[0] != io[1]) && (io[1] >= 0))
	       SC_close_safe(io[1]);

/* stderr */
	    if (io[2] >= 0)
	       SC_close_safe(io[2]);

	    SC_process_state(pp, SC_PROC_IO);};};

#endif

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_SETUP_TTY - set up the communication channel to the child
 *                     - as if it were a terminal
 */

static int _SC_posix_setup_tty(PROCESS *pp, int child)
   {int rv, in, medium, p;
    SC_ttydes *tty;

    if (!SC_process_alive(pp))
       return(FALSE);

    medium = pp->medium;
    in     = pp->io[0];

/* for child */
    if (child)

/* when using PTY's now is the time to open the slave side of the PTY */
       {if (medium == USE_PTYS)
	   {p  = SC_get_perm(FALSE);
            in = SC_open_safe(pp->spty, O_RDWR & ~NONBLOCK, p);
            if (in < 0)
               SC_error(-1, "COULD NOT OPEN SLAVE %s - _SC_POSIX_SETUP_TTY",
                        pp->spty);

/* now close the master */
            if (in != pp->io[0])
               {if (SC_close_safe(pp->io[0]) < 0)
                   SC_error(-1, "COULD NOT CLOSE MASTER - _SC_POSIX_SETUP_TTY");};

	    tty = pp->tty;
	    if (tty != NULL)
	       SC_set_term_size(in, tty->nr, tty->nc, tty->ph, tty->pw);
	    else
	       SC_set_term_size(in, -1, -1, -1, -1);

	    SC_change_term_state(in, SC_TERM_RAW, FALSE, NULL);

            pp->io[1] = in;
            pp->io[0] = in;};};

    SC_block_fd(in);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_DUP_FD - determine and return the descriptor to assign
 *                - to FD[NFD]
 */

static int _SC_get_dup_fd(char *msg, int to, SC_iodes *fd, int nfd, int ofd)
   {int err;
    mode_t p;

/* if bonded get the already open descriptor */
    if ((fd[1].file == fd[2].file) && (fd[1].file != NULL))
       {if ((nfd == 2) && (fd[1].fd != -1))
	   ofd = fd[1].fd;
        else if ((nfd == 1) && (fd[2].fd != -1))
	   ofd = fd[2].fd;}

/* otherwise open the descriptor */
    else
       {p   = SC_get_perm(FALSE);
	ofd = SC_open_safe(fd[nfd].file, fd[nfd].flag, p);

/* GOTCHA: retry failed opens if the cause might be due to slow
 * NFS response or transient system issues
 *
 * no retry
 *    EFAULT       pathname points outside your accessible address space.
 *    ELOOP        Too  many symbolic links or O_NOFOLLOW
 *    EROFS        file on a read-only filesystem and write access was requested
 *    ENODEV       device special file and no corresponding device exists
 *    ENXIO        [complicated - run away]
 *    ENOTDIR      component used as a directory is not
 *    ENAMETOOLONG pathname was too long
 *    EACCES       permissions wrong
 *    EISDIR       cannot write a directory
 *
 * retry
 *    ENOSPC       no space
 *    ENOMEM       Insufficient kernel memory was available
 *    EMFILE       too many files in process
 *    ENFILE       too many files open on the system
 *    ENOENT       O_CREAT  is  not  set  and the named file does not exist
 *    EEXIST       pathname already exists and O_CREAT and O_EXCL were used
 *    ETXTBSY      write executable which is currently being executed
 *
 */
	err = errno;
	if ((ofd == -1) && (to > 0) &&
	    ((err == ENOSPC) || (err == ENOMEM) ||
	     (err == EMFILE) || (err == ENFILE) ||
	     (err == ENOENT) || (err == EEXIST) ||
	     (err == ETXTBSY)))
	   {SC_sleep(to);
	    ofd = SC_open_safe(fd[nfd].file, fd[nfd].flag, p);};};

    return(ofd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DUP_FD - if FILE is NULL duplicate file OFD to file NFD
 *            - otherwise open FILE and duplicate it to NFD
 *            - if TO is greater than zero and select open errors occur
 *            - retry the open in TO milliseconds
 *            - use MSG for error reporing purposes
 *            - return TRUE iff successful
 *            - NOTE: this is really file static but conditionalizations
 *            - cause compiler warnings which are silenced by
 *            - removing the static declaration
 */

int _SC_dup_fd(char *msg, int to, SC_iodes *fd, int nfd, int ofd)
   {int rv;

    if (fd[nfd].flag != -1)
       ofd = _SC_get_dup_fd(msg, to, fd, nfd, ofd);

    if (ofd < 0)
       SC_error(SC_EXIT_ERRNO(), "COULD NOT OPEN %s (%d/%d) - _SC_DUP_FD",
		fd[nfd].file, ofd, errno);

    rv = (dup2(ofd, nfd) >= 0);
    if (rv == TRUE)
       fd[nfd].fd = ofd;
    else
       {SC_close_safe(ofd);
	SC_error(SC_EXIT_ERRNO(), "COULD NOT DUP %s (%d/%d) - _SC_DUP_FD",
		 fd[nfd].file, ofd, errno);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_EXEC - exec the named program together with args and env
 *                - in the new pro
 *                - if successful this routine should not return
 *                - return FALSE indicating failure
 */

static int _SC_posix_exec(PROCESS *cp, char **argv, char **env,
			  const char *mode)
   {int err;

#ifdef HAVE_PROCESS_CONTROL

    int i, to, cio[SC_N_IO_CH];

    err = SC_NO_EXEC;

    if (cp == NULL)
       return(err);

    err = SC_ERR_TRAP();
    if (err == 0)
       {SC_disconnect_tty();

/* setup the tty first */
	cp->setup(cp, TRUE);

	for (i = 0; i < SC_N_IO_CH; i++)
	    cio[i] = cp->io[i];

	to = cp->open_retry;

/* set the process stdin, stdout, and stderr to those from the pipe */
	_SC_dup_fd("STDIN",  to, cp->fd, 0, cio[0]);
	_SC_dup_fd("STDOUT", to, cp->fd, 1, cio[1]);
	_SC_dup_fd("STDERR", to, cp->fd, 2, cio[1]);

/* now that they are copied release the old values */
	if (cio[1] != cio[0])
	   {if (SC_close_safe(cio[1]) < 0)
	       SC_error(SC_NO_CLOSE,
			"COULD NOT CLOSE STDOUT - _SC_POSIX_EXEC");};

	if (SC_close_safe(cio[0]) < 0)
	   SC_error(SC_NO_CLOSE, "COULD NOT CLOSE STDIN - _SC_POSIX_EXEC");

/* if this is a binary connection inform the parent of the binary formats */
	if (cp->data_type == SC_BINARY)

/* keep the info about the parent for use in binary I/O */
	   {for (i = 0; i < SC_N_IO_CH; i++)
	        cp->io[i] = i;

	    _SC.terminal = cp;
	    if (!(*cp->send_formats)())
	       SC_error(SC_NO_FMT, "COULD NOT SEND FORMATS - _SC_POSIX_EXEC");}
	else
	   _SC.terminal = NULL;

/* exec the process with args and environment - this won't return */
	err = execvp(argv[0], argv);

/* if we get here the exec failed */
	if (err == -1)
	   SC_error(SC_EXIT_ERRNO(),
		    "EXECVP ERROR (%d): '%s' - _SC_POSIX_EXEC",
		    errno, argv[0]);
	else
	   SC_error(SC_NO_EXEC, "EXECVP RETURNED: '%s' - _SC_POSIX_EXEC",
		    argv[0]);};

    SC_ERR_UNTRAP();

#else

    err = SC_NO_EXEC;

#endif

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_STATUS - return the status of the process */

static int _SC_posix_status(PROCESS *pp)
   {int rv;

    rv = -1;

    if (pp != NULL)

/* flush socket first to generate SIGPIPE if other end is gone */
       {if (pp->medium == USE_SOCKETS)
	   SC_flush(pp);

	if (!SC_process_alive(pp))
	   rv = SC_EXITED | SC_DEAD;

/* NOTE: if we are doing blocked reads and we have had reads of
 * zero bytes then assume the child has exited and the OS failed
 * to deliver SIGCHLD or ignored our handler or some such problem
 * this happens with OSF all the time - a serious bug
 */
	else if ((pp->blocking == TRUE) && (pp->n_zero != 0))
	   {_SC_set_process_status(pp, SC_EXITED | SC_KILLED, pp->reason, NULL);
	    rv = pp->status;}

	else
	   rv = pp->status & ~SC_CHANGED;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_GETS - get bytes from the input stream
 *                - and return the number of bytes placed in
 *                - the buffer
 */

static int _SC_posix_gets(char *bf, int len, PROCESS *pp)
   {int n, rv;

    rv = 0;

    if (pp != NULL)
       {SC_flush(pp);

	n = SC_read_sigsafe(pp->io[0], bf, len);
	switch (n)
	   {case 0 :
	    case -1 :
	         if ((pp->blocking == TRUE) && (pp->line_mode == FALSE))
		    pp->n_zero++;
		 rv = 0;
		 break;
	    default :
	         pp->n_zero = 0;
		 pp->n_read += n;
		 rv = n;
		 break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_IN_READY - return input ready status of process */

static int _SC_posix_in_ready(PROCESS *pp)
   {int rv, status;

    status = fcntl(pp->io[0], F_GETFL);
    rv     = (status & NONBLOCK);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_CLOSE - kill the process associated with PP
 *                 - we do NOT free the process structure because the
 *                 - SIGCHILD may come in later and the signal handler
 *                 - will consult the managed process list to find
 *                 - this process
 *                 - return TRUE iff the process was closed
 *                 - return FALSE otherwise
 */

static int _SC_posix_close(PROCESS *pp)
   {int rv, st, pid, sts;

    rv = FALSE;

    SC_flush(pp);

    if (SC_process_alive(pp))
       {rv  = TRUE;
	pid = pp->id;
        sts = pp->flags & SC_PROC_SIG;

/* because it is possible to get in here many times with the same process
 * make sure that we do the following only once
 */
        if (sts == 0)
           {SC_catch_io_interrupts(FALSE);

/* kill the process itself
 * NOTE: if the process is remote, then the pid will be 0, and
 *       there is nothing to kill here
 */
	    if ((pid > 0) && (SC_status(pp) == SC_RUNNING))
	       {st = SC_child_kill(pid);
		SC_ASSERT(st == TRUE);

		SC_process_state(pp, SC_PROC_SIG);};

/* close the communications pipe */
	    if (pp->release != NULL)
	       rv = pp->release(pp);

	    SC_catch_io_interrupts(TRUE);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_FLUSH - flush the input and output streams
 *                 - for the given process
 */

static int _SC_posix_flush(PROCESS *pp)
   {int iv, ov, rv;
    SC_thread_proc *ps;

    ps = pp->tstate;

    iv = -2;
    ov = -2;

    if (!SC_process_alive(pp))
       return(FALSE);

    ps->current_flushed_process = pp->id;

#ifdef BSD_TERMINAL
    static int one = 1;

    iv = ioctl(pp->io[0], TIOCFLUSH, &one);
    iv = ioctl(pp->io[2], TIOCFLUSH, &one);
    if (pp->io[1] != pp->io[0])
       ov = ioctl(pp->io[1], TIOCFLUSH, &one);
    else
       ov = iv;

#else

/* System V appears to interpret flush as "throw it away" */

    iv = ov = 0;

#endif

    rv = ((iv < 0) | ((ov < 0) << 1));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_PRINTF - do an fprintf style output to a process */

static int _SC_posix_printf(PROCESS *pp, const char *buffer)
   {int len, nb;

    if ((pp != NULL) && (buffer != NULL))
       {len = strlen(buffer);

	nb = SC_write_sigsafe(pp->io[1], buffer, len);
	if (nb != len)
	   SC_error(-1, "FAILED WRITE %d OF %d BYTES - _SC_POSIX_PRINTF",
		    nb, len);

	if (nb > 0)
	   pp->n_write += nb;

	SC_flush(pp);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POSIX_PROCESS - register the POSIX functions with process PP */

void _SC_posix_process(PROCESS *pp)
   {

    pp->release     = _SC_posix_release;
    pp->exec        = _SC_posix_exec;
    pp->close       = _SC_posix_close;
    pp->statusp     = _SC_posix_status;
    pp->flush       = _SC_posix_flush;
    pp->printf      = _SC_posix_printf;
    pp->gets        = _SC_posix_gets;
    pp->in_ready    = _SC_posix_in_ready;
    pp->setup       = _SC_posix_setup_tty;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
