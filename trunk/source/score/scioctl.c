/*
 * SCIOCTL.C - IO Control for IP and FILE communication
 *           - formerly a part of PPC
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

int
 _SC_unblock = FALSE;

/*--------------------------------------------------------------------------*/

/*                           EVENT LOOP ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* SC_CATCH_EVENT_LOOP_INTERRUPTS - activate the SIGIO handler */

void SC_catch_event_loop_interrupts(SC_evlpdes *pe, int flag)
   {

    if ((pe != NULL) && flag && SC_gs.io_interrupts_on)
       SC_signal_action_n(SC_SIGIO, pe->sigio, NULL, 0, BLOCK_WITH_SIGIO, -1);
    else
       SC_signal_action_n(SC_SIGIO, SIG_IGN, NULL, 0, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EVENT_LOOP_HANDLER - respond to an input available interrupt */

static void _SC_event_loop_handler(int sig)
   {SC_evlpdes *pe, **ev;

    ev = _SC_get_ev_loop(-1);

/* init the event loop descriptor */
    if (ev != NULL)
       pe = *ev;

    else if (_SC.evloop != NULL)
       pe = _SC.evloop;

    else
       pe = NULL;

/* if the above inits went well, respond to the interrupt */
    if (pe != NULL)
       {SC_event_loop_poll(pe, pe->state, pe->wait);
        SC_catch_event_loop_interrupts(pe, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_EVENT_LOOP - setup and return an event loop descriptor */

SC_evlpdes *SC_make_event_loop(PFSignal_handler sigio, PFSignal_handler sigchld,
			       int (*ex)(int *rv, void *a),
			       int wait, short accept, short reject)
   {int nfd;
    SC_evlpdes *pe;

    pe = CPMAKE(SC_evlpdes, 3);

    pe->fd     = CMAKE_ARRAY(SC_poll_desc, NULL, 3);
    pe->faccpt = CMAKE_ARRAY(PFFileCallback, NULL, 3);
    pe->frejct = CMAKE_ARRAY(PFFileCallback, NULL, 3);

/* guess number of descriptors that will be needed
 * not crucial but start out with enough to prevent
 * fatal thrashing of dynamic arrays at the outset
 */
    nfd = 512;

    SC_array_resize(pe->fd,     nfd, -1.0);
    SC_array_resize(pe->faccpt, nfd, -1.0);
    SC_array_resize(pe->frejct, nfd, -1.0);

    pe->wait    = wait;
    pe->raw     = FALSE;
    pe->maccpt  = (accept == -1) ? (POLLIN | POLLPRI) : accept;
    pe->mrejct  = (reject == -1) ? (POLLERR | POLLHUP | POLLNVAL) : reject;
    pe->exitf   = ex;
    pe->sigio   = (sigio == NULL) ? _SC_event_loop_handler : sigio;
    pe->sigchld = sigchld;

    return(pe);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_EVENT_LOOP - release an SC_evlpdes instance */

void SC_free_event_loop(SC_evlpdes *pe)
   {

    if (pe != NULL)
       {SC_free_array(pe->fd, NULL);
	SC_free_array(pe->faccpt, NULL);
	SC_free_array(pe->frejct, NULL);

	CFREE(pe);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_EVENT_LOOP_CURRENT - set the global current event loop to be
 *                            - the specified one and
 *                            - return the old value
 */

SC_evlpdes *SC_make_event_loop_current(SC_evlpdes *pe)
   {SC_evlpdes *old, **ev;

    ev = _SC_get_ev_loop(-1);

    old = *ev;
    *ev = pe;

    return(old);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EVENT_LOOP_SET_MASKS - set the value of the ACCEPT and REJECT mask for
 *                         - the given event loop descriptor
 */

void SC_event_loop_set_masks(SC_evlpdes *pe, int accept, int reject)
   {

    pe->maccpt = accept;
    pe->mrejct = reject;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EVENT_LOOP_GET_MASKS - get the value of the ACCEPT and REJECT mask for
 *                         - the given event loop descriptor
 */

void SC_event_loop_get_masks(SC_evlpdes *pe, int *paccept, int *preject)
   {

    *paccept = pe->maccpt;
    *preject = pe->mrejct;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_EVENT_LOOP_CALLBACK - add the implied I/O descriptor
 *                                 - (TYPE and P) and
 *                                 - its callback function to the list
 *                                 - of callbacks for the specified
 *                                 - event loop PE
 */

int SC_register_event_loop_callback(SC_evlpdes *pe, int type, void *p,
				    PFFileCallback acc, PFFileCallback rej,
				    int pid)
   {int rv, fd;
    SC_poll_desc pd;

    if (type == SC_PROCESS_I)
       fd = ((PROCESS *) p)->in;

    else if (type == SC_FILE_I)
       fd = fileno((FILE *) p);

    else
       fd = *(int *) p;

    pd.fd      = fd;
    pd.events  = pe->maccpt;
    pd.revents = 0;

    SC_array_push(pe->fd, &pd);
    SC_array_push(pe->faccpt, &acc);
    SC_array_push(pe->frejct, &rej);

    rv = SC_set_fd_async(fd, TRUE, pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_EVENT_LOOP_ACCEPT - replace the accept function
 *                              - of the implied I/O descriptor
 *                              - (TYPE and P) in the
 *                              - event loop PE
 */

int SC_replace_event_loop_accept(SC_evlpdes *pe, int type, void *p,
				 PFFileCallback acc)
   {int i, n, fd;
    SC_poll_desc *pd;

    if (type == SC_PROCESS_I)
       fd = ((PROCESS *) p)->in;

    else if (type == SC_FILE_I)
       fd = fileno((FILE *) p);

    else
       fd = *(int *) p;

    n = SC_array_get_n(pe->fd);
    for (i = 0; i < n; i++)
        {pd = SC_array_get(pe->fd, i);
	 if (pd->fd == fd)
	    {SC_array_set(pe->faccpt, i, &acc);
             break;};};

    if (i >= n)
       i = SC_register_event_loop_callback(pe, SC_INT_I, &fd,
					   acc, NULL, -1);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_EVENT_LOOP_REJECT - replace the reject function
 *                              - of the implied I/O descriptor
 *                              - (TYPE and P) in the
 *                              - event loop PE
 */

int SC_replace_event_loop_reject(SC_evlpdes *pe, int type, void *p,
				 PFFileCallback rej)
   {int i, n, fd;
    SC_poll_desc *pd;

    if (type == SC_PROCESS_I)
       fd = ((PROCESS *) p)->in;

    else if (type == SC_FILE_I)
       fd = fileno((FILE *) p);

    else
       fd = *(int *) p;

    n = SC_array_get_n(pe->fd);
    for (i = 0; i < n; i++)
        {pd = SC_array_get(pe->fd, i);
	 if (pd->fd == fd)
	    {SC_array_set(pe->frejct, i, &rej);
             break;};};

    if (i >= n)
       i = SC_register_event_loop_callback(pe, SC_INT_I, &fd,
					   NULL, rej, -1);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REMOVE_EVENT_LOOP_CALLBACK - remove the callback function
 *                               - of the implied I/O descriptor
 *                               - (TYPE and P) in the
 *                               - event loop PE
 */

void SC_remove_event_loop_callback(SC_evlpdes *pe, int type, void *p)
   {int i, n, fd;
    SC_poll_desc *pd;
    PROCESS *pp;
    FILE *fp;

    if (p == NULL)
       return;

    if (type == SC_PROCESS_I)
       {pp = (PROCESS *) p;
	fd = pp->in;}

    else if (type == SC_FILE_I)
       {fp = (FILE *) p;
	fd = fileno(fp);}

    else
       fd = *(int *) p;

    n = SC_array_get_n(pe->fd);
    for (i = 0; i < n; i++)
        {pd = SC_array_get(pe->fd, i);
	 if (pd->fd == fd)
            {SC_array_remove(pe->fd, i);
	     SC_array_remove(pe->faccpt, i);
	     SC_array_remove(pe->frejct, i);
             break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_EVENT_LOOP_CALLBACK - return the callback functions associated
 *                            - with the implied I/O descriptor
 *                            - (TYPE and P) in the
 *                            - event loop PE
 */

void SC_get_event_loop_callback(SC_evlpdes *pe, int type, void *p,
				PFSignal_handler *psigio,
				PFSignal_handler *psigchld,
				PFEVExit *pex,
				PFFileCallback *pacc, PFFileCallback *prej)
   {int i, n, fd;
    SC_poll_desc *pd;
    SC_address ad;
    PROCESS *pp;
    FILE *fp;

    if (p == NULL)
       return;

    if (type == SC_PROCESS_I)
       {pp = (PROCESS *) p;
	fd = pp->in;}

    else if (type == SC_FILE_I)
       {fp = (FILE *) p;
	fd = fileno(fp);}

    else
       fd = *(int *) p;

    n = SC_array_get_n(pe->fd);
    for (i = 0; i < n; i++)
        {pd = SC_array_get(pe->fd, i);
	 if (pd->fd == fd)
            {if (pacc != NULL)
	        {ad.memaddr = *(void **) SC_array_get(pe->faccpt, i);
		 *pacc      = (PFFileCallback) ad.funcaddr;};
	     if (prej != NULL)
	        {ad.memaddr = *(void **) SC_array_get(pe->frejct, i);
		 *prej      = (PFFileCallback) ad.funcaddr;};
             break;};};

    if (psigio != NULL)
       *psigio = pe->sigio;

    if (psigchld != NULL)
       *psigchld = pe->sigchld;

    if (pex != NULL)
       *pex = pe->exitf;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EVENT_LOOP_POLL - poll the registered file descriptors and dispatch
 *                    - to their assigned functions
 *                    - these are registered via SC_register_event_loop_callback
 *                    - TO is in milliseconds
 *                    - TO of -1 means block
 *                    - return the number of descriptors ready for I/O
 *                    - that were not handled here iff successful
 *                    - on error return -ERRNO
 */

int SC_event_loop_poll(SC_evlpdes *pe, void *a, int to)
   {int i, n, nrdy, nacc, nrej, in;
    short rev;
    SC_poll_desc *pd;
    PFFileCallback fn;
    PFSignal_handler sc;
    SC_contextdes hnd;
    SC_evlpdes *old, **ev;

    ev = _SC_get_ev_loop(-1);

    pe->state = a;

/* save the current event loop descriptor */
    if (ev != NULL)
       {old = *ev;

/* make this one the current event loop descriptor */
	*ev = pe;};

/* NOTE: we mark the arrays
 * PD, PA, PR so that they will not be freed in an interrupt handler
 * for example a job finishes and a job is removed from the list
 * the dynamic arrays will change to reflect the new list
 * but these pointers will remain valid
 */
    n = SC_array_get_n(pe->fd);

    for (i = 0; i < n; i++)
        {pd = SC_array_get(pe->fd, i);
	 pd->revents = 0;};

/* use the SIGCHLD handler specified for the loop */
    sc = pe->sigchld;
    if (sc != NULL)
       hnd = SC_signal_n(SIGCHLD, sc, NULL);

/* reassert raw mode */
    if (pe->raw == TRUE)
       {in = fileno(stdin);
	SC_set_raw_state(in, FALSE);
	SC_set_io_attrs(in,
			OPOST, SC_TERM_OUTPUT, TRUE,
			0);};

    pd   = SC_array_array(pe->fd);
    nrdy = SC_poll(pd, n, to);
    CFREE(pd);

/* restore the original SIGCHLD handler */
    if (sc != NULL)
       SC_signal_n(SIGCHLD, hnd.f, hnd.a);

    if (nrdy > 0)
       {nacc = 0;
	nrej = 0;
	for (i = 0; i < n; i++)
	    {pd  = SC_array_get(pe->fd, i);
	     rev = pd->revents;
             if (rev & pe->maccpt)
                {nacc++;
		 fn = *(PFFileCallback *) SC_array_get(pe->faccpt, i);
		 if (fn != NULL)
		    (*fn)(pd->fd, rev, a);}

             else if (rev & pe->mrejct)
                {nrej++;
		 fn = *(PFFileCallback *) SC_array_get(pe->frejct, i);
		 if (fn != NULL)
		    (*fn)(pd->fd, rev, a);};

             SC_catch_event_loop_interrupts(pe, SC_gs.io_interrupts_on);};

	nrdy -= (nacc + nrej);}

    else if (nrdy == -1)
       nrdy = -errno;

/* restore the current event loop descriptor */
    if (ev != NULL)
       *ev = old;

    return(nrdy);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EVENT_LOOP - run the event loop PE
 *               - until the program decides to end or an error
 *               - is signalled by the event loop
 *               - the argument A is passed to the event handlers
 *               - registered with the event loop PE
 *               - call the exit function EXITF every TO milliseconds
 */

int SC_event_loop(SC_evlpdes *pe, void *a, int to)
   {int nr, rv, err;
    SC_contextdes osi;
    int (*exitf)(int *rv, void *a);

    if (pe == NULL)
       pe = _SC.evloop;

    osi = SC_signal_action_n(SC_SIGIO, pe->sigio, NULL,
			     0, BLOCK_WITH_SIGIO, -1);

    exitf = pe->exitf;

    rv = -1;
    while (TRUE)
       {if (exitf != NULL)
	   {if ((*exitf)(&rv, a))
	       break;

/* if we have lost any children we have no reasonable expectation of success
 * so bail out right away with an error
 */
	    nr = SC_running_children();
	    if (nr < 0)
	       {rv = nr;
	        break;};};

	err = SC_event_loop_poll(pe, a, to);
	if (err < 0)
	   rv = err;};

    SC_signal_action_n(SC_SIGIO, osi.f, osi.a, 0, -1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROCESS_EVENT_LOOP - run an event loop to get output from a 
 *                       - single process
 */

int SC_process_event_loop(PROCESS *pp, void *a,
			  int (*ex)(int *rv, void *a),
			  PFFileCallback acc, PFFileCallback rej)
   {int pi, rv;
    SC_evlpdes *pe;

/* create the event loop state */
    pe = SC_make_event_loop(NULL, NULL, ex, -1, -1, -1);

/* register the I/O channels for the event loop to monitor */
    pi = SC_register_event_loop_callback(pe, SC_PROCESS_I, pp,
					 acc, rej, -1);

/* if all channels are OK activate the interrupt handling */
    SC_gs.io_interrupts_on = pi;
    if (pi)
       SC_catch_event_loop_interrupts(pe, SC_gs.io_interrupts_on);

    rv = SC_event_loop(pe, a, -1);

    SC_free_event_loop(pe);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           ASYNC I/O ROUTINES                             */

/*--------------------------------------------------------------------------*/

#ifdef HAVE_STREAMS_P

/* SC_SET_FD_ASYNC_STREAMS - setup asynchronous I/O when using STREAMS */

static int SC_set_fd_async_streams(int fd, int state)
   {int rv;

    rv = FALSE;

    if (SC_gs.io_interrupts_on)
       {

#if defined(HAVE_ASYNC_STREAMS)
        int arg, na, status;
	SC_contextdes hnd;

        arg = 0;

	status = ioctl(fd, I_GETSIG, &arg);
	if (status < 0)
	   arg = 0;

	if (state)
	   na = arg | (S_INPUT | S_HIPRI);
	else
	   na = arg & ~(S_INPUT | S_HIPRI);

	status = ioctl(fd, I_SETSIG, na);
	if (status < 0)
	   SC_error(-1, "SETSIG IOCTL FAILED (%s) - SC_SET_FD_ASYNC_STREAMS",
		    strerror(errno));

	if (state)
	   {ONCE_SAFE(TRUE, NULL)
	       hnd = SC_signal_action_n(SC_SIGIO, _SC_event_loop_handler, NULL,
					0, BLOCK_WITH_SIGIO, -1);
	       if (hnd.f == SIG_ERR)
	          SC_error(-1, "CAN'T SET SIGIO HANDLER - SC_SET_FD_ASYNC_STREAMS");
	    END_SAFE;};
#endif

	rv = TRUE;};

    return(rv);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef FASYNC

/* SC_SET_FD_ASYNC_FASYNC - setup asynchronous I/O when using FASYNC
 *                        - NOTE: this is a file static scope function
 *                        - but due to conditionalizations GCC may think
 *                        - it is defined but not used
 *                        - so we end up removing the static
 */

int SC_set_fd_async_fasync(int fd, int state, int pid)
   {int arg, status, rstrct, rv;
    SC_contextdes hnd;

    rv = FALSE;

    if (SC_gs.io_interrupts_on)

/* get the control flag */
       {arg = fcntl(fd, F_GETFL);
	if (arg == -1)
	   SC_error(-1, "GETFL FCNTL FAILED - SC_SET_FD_ASYNC_FASYNC");

/* toggle FASYNC in the control flag */
	if (state)
	   arg |= FASYNC;
	else
	   arg &= ~FASYNC;

/* set the control flag */
	status = fcntl(fd, F_SETFL, arg);
	if (status == -1)
	   SC_error(-1, "SETFL FCNTL FAILED - SC_SET_FD_ASYNC_FASYNC");

#ifdef SGI
	rstrct = TRUE;
#else
	rstrct = (fd == STDIN_FILENO);
#endif

/* restrict SIGIO only to me - NOT to my process group */
	if (rstrct)
	   {if (pid == -1)
	       pid = getpid();

	    if (pid > 0)
	       status = fcntl(fd, F_SETOWN, pid);};

/* set the signal handler */
	if (state)
	   {ONCE_SAFE(TRUE, NULL)
	       hnd = SC_signal_action_n(SC_SIGIO, _SC_event_loop_handler, NULL,
					0, BLOCK_WITH_SIGIO, -1);
	       if (hnd.f == SIG_ERR)
	          SC_error(-1, "CAN'T SET SIGIO HANDLER - SC_SET_FD_ASYNC_FASYNC");
	    END_SAFE;};

	rv = TRUE;};

    return(rv);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_FD_ASYNC - set the given FD to be asynchronous
 *                 - that means when FD is ready for input/output
 *                 - SIGIO is raised
 *                 - if PID > 0 then it is taken to be the process id that
 *                 - will get the SIGIO
 *                 - if PID = -1 then the current process will get the SIGIO
 *                 - if PID = -2 then the no action will be taken to
 *                 - set which process gets the SIGIO (useful for X windows)
 *                 - return TRUE iff successful
 */

int SC_set_fd_async(int fd, int state, int pid)
   {int rv, ok;

    rv = FALSE;

    ok = SC_ERR_TRAP();
    if (ok == 0)

#ifdef HAVE_POSIX_SYS

# ifdef HAVE_STREAMS_P

       rv = SC_set_fd_async_streams(fd, state);

# elif FASYNC

       rv = SC_set_fd_async_fasync(fd, state, pid);

# else

       {char *s;

	s = SC_error_msg();
	snprintf(s, MAXLINE,
		 "INTERRUPT DRIVEN I/O NOT SUPPORTED HERE - SC_SET_FD_ASYNC");};

# endif

#endif

    SC_ERR_UNTRAP();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_ATTR - set the control flags ATTR on a PROCESS
 *             -
 *             - SC_LINE    line at a time input
 *             - SC_NDELAY  non-blocking I/O
 *             - SC_APPEND  append (writes guaranteed at the end)
 *             - SC_SYNC    synchronous write option
 *             - SC_ASYNC   interrupt-driven I/O for sockets
 */

int SC_set_attr(PROCESS *pp, int attr, int val)
   {int rv;

    rv = TRUE;

    switch (attr)
       {case SC_LINE :
             pp->line_mode = val;
             break;

        default :
	     if (attr == SC_ASYNC)
	        rv = SC_set_fd_async(pp->in, val, pp->id);
	     else
	        rv = SC_set_fd_attr(pp->in, attr, val);
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ECHO_ON_FILE - set the file input to be echoed */

int SC_echo_on_file(FILE *fp)
   {int fd, rv;

    fd = fileno(fp);

    rv = SC_echo_on_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ECHO_ON_FD - set the file descriptor input to be echoed */

int SC_echo_on_fd(int fd)
   {int rv;

    rv = SC_set_io_attr(fd, ECHO, TRUE);
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ECHO_OFF_FILE - set the file input to be echoed */

int SC_echo_off_file(FILE *fp)
   {int fd, rv;

    fd = fileno(fp);
    rv = SC_echo_off_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ECHO_OFF_FD - set the file descriptor input to be echoed */

int SC_echo_off_fd(int fd)
   {int rv;

    rv = SC_set_io_attr(fd, ECHO, FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISBLOCKED_FD - return TRUE iff the file descriptor is blocked */

int SC_isblocked_fd(int fd)
   {int status;

    status = 0;

#ifdef UNIX
    status = fcntl(fd, F_GETFL, status);
    status = ((status & O_NDELAY) == 0);
#endif

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISBLOCKED_FILE - return TRUE iff the file is blocked */

int SC_isblocked_file(FILE *fp)
   {int fd, rv;

/* useful for debug
   if (fp == stdin) return(TRUE);
 */

    fd = fileno(fp);
    rv = SC_isblocked_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISBLOCKED - return TRUE iff the PROCESS is blocked */

int SC_isblocked(PROCESS *pp)
   {int rv;

    rv = (pp->blocking == TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_UNBLOCK_FD - set the file descriptor to be unblocked */

int SC_unblock_fd(int fd)
   {int status;

    status = 0;

#ifdef UNIX
    status = fcntl(fd, F_GETFL, status);
    status = fcntl(fd, F_SETFL, status | O_NDELAY);
    if (status == -1)
       status = FALSE;
    else
       status = TRUE;
#endif

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_UNBLOCK_FILE - set the file to be unblocked */

int SC_unblock_file(FILE *fp)
   {int fd, rv;

/* useful for debug
   if (fp == stdin) return(TRUE);
 */

    fd = fileno(fp);
    rv = SC_unblock_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_UNBLOCK - set the PROCESS to be unblocked */

int SC_unblock(PROCESS *pp)
   {

    pp->blocking = FALSE;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BLOCK_FD - set the file descriptor to be blocked */

int SC_block_fd(int fd)
   {int status;

    status = 0;

#ifdef UNIX
    status = fcntl(fd, F_GETFL, status);
    status = fcntl(fd, F_SETFL, status & ~O_NDELAY);
    if (status == -1)
       status = FALSE;
    else
       status = TRUE;
#endif

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BLOCK_FILE - set the file to be blocked */

int SC_block_file(FILE *fp)
   {int fd, rv;

    fd = fileno(fp);
    rv = SC_block_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BLOCK - set the PROCESS to be blocked */

int SC_block(PROCESS *pp)
   {

    pp->blocking = TRUE;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FD_FLAGS - return the control flags associated with FD */

int SC_fd_flags(int fd)
   {int status;

    status = 0;

#ifdef UNIX
    status = fcntl(fd, F_GETFL, status);
#endif

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FILE_FLAGS - return the control flags associated with FP */

int SC_file_flags(FILE *fp)
   {int fd, rv;

    fd = fileno(fp);
    rv = SC_fd_flags(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IO_CALLBACK_PID - set I/O on file descriptor FD to be interrupt driven
 *                    - ACC is called when FD has something
 *                    - matching accept mask
 *                    - REJ is called when FD has something
 *                    - matching reject mask
 *                    - the process PID will get the SIGIO
 */

int SC_io_callback_pid(int fd, PFFileCallback acc, PFFileCallback rej,
                       int pid)
   {int rv;

    if (_SC.evloop == NULL)
       _SC.evloop = SC_make_event_loop(NULL, NULL, NULL, 0, -1, -1);

    rv = SC_register_event_loop_callback(_SC.evloop, 0, &fd, acc, rej, pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IO_CALLBACK_FD - set the file descriptor I/O to be interrupt driven */

int SC_io_callback_fd(int fd, PFFileCallback acc, PFFileCallback rej)
   {int rv;

    rv = SC_io_callback_pid(fd, acc, rej, -1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IO_CALLBACK_FILE - set the file I/O to be interrupt driven */

int SC_io_callback_file(FILE *fp, PFFileCallback acc, PFFileCallback rej)
   {int fd, rv;

    fd = fileno(fp);
    rv = SC_io_callback_pid(fd, acc, rej, -1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IO_CALLBACK - set the process I/O to be interrupt driven */

int SC_io_callback(PROCESS *pp, PFFileCallback acc, PFFileCallback rej)
   {int fd, rv;

    fd = pp->in;
    rv = SC_io_callback_pid(fd, acc, rej, pp->id);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_ACCEPT_FD - substitute ACC as the accept function associated
 *                      - with the file descriptor FD
 *                      - add it as a new one if it doesn't exist already
 */

int SC_replace_accept_fd(int fd, PFFileCallback acc)
   {int i;

    if (_SC.evloop != NULL)
       i = SC_replace_event_loop_accept(_SC.evloop, 0, &fd, acc);
    else
       i = -1;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_ACCEPT_FILE - substitute ACC as the accept function associated
 *                        - with the file FILE
 *                        - add it as a new one if it doesn't exist already
 */

int SC_replace_accept_file(FILE *fp, PFFileCallback acc)
   {int fd, rv;

    fd = fileno(fp);

    rv = SC_replace_accept_fd(fd, acc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_ACCEPT - substitute ACC as the accept function associated
 *                   - with the PROCESS PP
 *                   - add it as a new one if it doesn't exist already
 */

int SC_replace_accept(PROCESS *pp, PFFileCallback acc)
   {int fd, rv;

    fd = pp->in;

    rv = SC_replace_accept_fd(fd, acc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_REJECT_FD - substitute REJ as the reject function associated
 *                      - with the file descriptor FD
 *                      - add it as a new one if it doesn't exist already
 */

int SC_replace_reject_fd(int fd, PFFileCallback rej)
   {int i;

    if (_SC.evloop != NULL)
       i = SC_replace_event_loop_reject(_SC.evloop, 0, &fd, rej);
    else
       i = -1;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_REJECT_FILE - substitute REJ as the reject function associated
 *                        - with the file FILE
 *                        - add it as a new one if it doesn't exist already
 */

int SC_replace_reject_file(FILE *fp, PFFileCallback rej)
   {int fd, rv;

    fd = fileno(fp);

    rv = SC_replace_reject_fd(fd, rej);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REPLACE_REJECT - substitute REJ as the reject function associated
 *                   - with the PROCESS PP
 *                   - add it as a new one if it doesn't exist already
 */

int SC_replace_reject(PROCESS *pp, PFFileCallback rej)
   {int fd, rv;

    fd = pp->in;

    rv = SC_replace_reject_fd(fd, rej);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RL_IO_CALLBACK - remove any callback associated with
 *                   - a file descriptor
 */

void SC_rl_io_callback(int fd)
   {

    if (_SC.evloop != NULL)
       SC_remove_event_loop_callback(_SC.evloop, 0, &fd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RL_IO_CALLBACK_FILE - remove any callback associated with
 *                        - the given file
 */

void SC_rl_io_callback_file(FILE *fp)
   {int fd;

    fd = fileno(fp);
    SC_rl_io_callback(fd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CATCH_IO_INTERRUPTS - activate the SIGIO handler */

void SC_catch_io_interrupts(int flag)
   {

    if (_SC.evloop != NULL)
       SC_catch_event_loop_interrupts(_SC.evloop, flag);

    else if (flag && SC_gs.io_interrupts_on)
       SC_signal_action_n(SC_SIGIO, _SC_event_loop_handler, NULL,
			  0, BLOCK_WITH_SIGIO, -1);
    else
       SC_signal_action_n(SC_SIGIO, SIG_IGN, NULL, 0, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_POLL_MASKS - set the value of the ACCEPT and REJECT mask for
 *                   - SC_poll_descriptors
 */

void SC_set_poll_masks(int accept, int reject)
   {

    _SC.evloop->maccpt = accept;
    _SC.evloop->mrejct = reject;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_POLL_MASKS - get the value of the ACCEPT and REJECT mask for
 *                   - SC_poll_descriptors
 */

void SC_get_poll_masks(int *paccept, int *preject)
   {

    *paccept = _SC.evloop->maccpt;
    *preject = _SC.evloop->mrejct;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_POLL_DESCRIPTORS - poll the registered file descriptors and dispatch
 *                     - to their assigned functions
 *                     - these are registered via SC_io_callback_file and
 *                     - SC_io_callback_fd calls
 *                     - TO is in milliseconds
 *                     - TO of -1 means block
 *                     - return a value less than zero on error
 *                     - otherwise return the number of descriptors
 *                     - ready for I/O
 */

int SC_poll_descriptors(int to)
   {int n;

    n = SC_event_loop_poll(_SC.evloop, NULL, to);

    return(n);}

/*--------------------------------------------------------------------------*/

#ifndef HAVE_STREAMS_P

#ifdef HAVE_SELECT_P

/*--------------------------------------------------------------------------*/

/* SC_POLL - implement poll in terms of select
 *         - timeout is the number of milliseconds to wait for input
 *         - timeout = -1 means block
 */

int SC_poll(SC_poll_desc *fds, long nfds, int timeout)
   {int i, n, nfd, tmo;
    short rev;
    char *msg;
    fd_set rfd, efd;
    struct timeval to, *pto;

    nfd = min(FD_SETSIZE, nfds);

/* block if timeout is -1 */
    if (timeout == -1)
       pto = NULL;

    else
       {tmo        = 1000*timeout;   /* convert milliseconds to microseconds */
	to.tv_sec  = tmo / 1000000L;
        to.tv_usec = tmo % 1000000L;

        pto = &to;};

/* map the descriptors from the SC_poll_desc */
    FD_ZERO(&rfd);
    FD_ZERO(&efd);
    for (i = 0; i < nfd; i++)
        {FD_SET(fds[i].fd, &rfd);
         FD_SET(fds[i].fd, &efd);};

#ifdef HPUX_9

/* the HP folks do it wrong before HPUX 10 */
    n = select(FD_SETSIZE, (int *) &rfd, NULL, (int *) &efd, pto);

#else

    n = select(FD_SETSIZE, &rfd, NULL, &efd, pto);

#endif

    if (n < 0)
       {msg = SC_error_msg();
	snprintf(msg, MAXLINE, "SELECT ERROR %d - SC_POLL", errno);};

/* map the return values to the SC_poll_desc */
    for (i = 0; i < nfd; i++)
        {rev = 0;
         if (FD_ISSET(fds[i].fd, &rfd))
            rev |= POLLIN;

         if (FD_ISSET(fds[i].fd, &efd))
            rev |= POLLERR;

         fds[i].revents = rev;};

    return(n);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* SC_POLL - poll stub for systems without poll or select */

int SC_poll(SC_poll_desc *fds, long nfds, int timeout)
   {int i, n, nfd, fd;

    nfd = 0;
    for (i = 0; i < nfds; i++)
        {fd = fds[i].fd;
         SC_unblock_fd(fd);

/* ask how many characters are available */
         n = 0;

         SC_block_fd(fd);

         if (n > 0)
            {fds[i].revents = POLLIN;
             nfd++;}
         else
            fds[i].revents = 0;};

    SC_sleep(timeout);

    return(nfd);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
