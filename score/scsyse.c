/*
 * SCSYSE.C - more routines for execing jobs
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

typedef struct s_descriptors descriptors;

struct s_descriptors
   {int fd;
    PROCESS *pp;
    SC_evlpdes *pe;};

#ifdef DEBUG

# define START_LOG                                                           \
   {SC_thread_proc _ps;                                                      \
    _ps = SC_get_thr_processes(-1);                                          \
    _lg = _ps->lgf;                                                          \
    _lg = io_open("log.wrap", "w")

# define WRITE_LOG(_x, _s)                                                   \
    io_printf(_lg, "\n%s %s", _x, _s)

# define END_LOG                                                             \
    io_close(_lg);}

#else

# define START_LOG
# define WRITE_LOG(_x, _s)
# define END_LOG

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RESET_STDIN - fix up stdin for poll depending
 *                 - on the presence of LEH
 *                 - WH FALSE puts stdin in raw mode with echo off
 *                 - WH TRUE puts stdin in line mode with echo on
 */

int _SC_reset_stdin(int wh)
   {int rv, fd;

    rv = TRUE;
    fd = fileno(stdin);

    if (wh == FALSE)
       {SC_unblock_fd(fd);

	rv = SC_set_io_attrs(fd,
			     SC_NDELAY, SC_TERM_DESC,     FALSE,
			     ICANON,    SC_TERM_LOCAL,    FALSE,
			     ECHO,      SC_TERM_LOCAL,    FALSE,
			     0);}
    else
       rv = SC_set_io_attrs(fd,
			    ICANON,    SC_TERM_LOCAL,    TRUE,
			    ECHO,      SC_TERM_LOCAL,    TRUE,
			    0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SIGNAL_RELAY - relay signals received by the parent
 *                  - to the child
 */

static void _SC_signal_relay(int sig)
   {PROCESS *pp;

    if (sig < 0)
       return;

    pp = SC_GET_CONTEXT(_SC_signal_relay);

    if (SC_process_status(pp) == SC_RUNNING)
       SC_send_signal(pp->id, sig);

    SC_signal_n(sig, _SC_signal_relay, pp, sizeof(PROCESS));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_INT_HND - special relay for SIGINT */

static void _SC_ex_int_hnd(int sig)
   {PROCESS *pp;

    pp = SC_GET_CONTEXT(_SC_ex_int_hnd);

    SC_block_file(stdin);

    _SC_reset_stdin(TRUE);

    _SC_signal_relay(sig);

    SC_signal_n(sig, _SC_ex_int_hnd, pp, sizeof(PROCESS));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_IO_HND - special handling for SIGIO */

static void _SC_ex_io_hnd(int sig)
   {int n;
    char bf[1024];

    n = SC_read_sigsafe(0, bf, 1024);
    if (n > 0)
       io_printf(stdout, "--> |%s|\n", bf);

/*    io_printf(stdout, "I did too trap the SIGIO\n"); */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_RELAY - setup to relay signals received by the parent
 *                 - to the child
 */

static void _SC_setup_relay(PROCESS *pp)
   {int sz;

    sz = sizeof(PROCESS);

    SC_signal_n(SIGHUP,  _SC_signal_relay, pp, sz);
    SC_signal_n(SIGINT,  _SC_signal_relay, pp, sz);
    SC_signal_n(SIGQUIT, _SC_signal_relay, pp, sz);
#ifdef SIGIOT
    SC_signal_n(SIGIOT,  _SC_signal_relay, pp, sz);
#endif
    SC_signal_n(SIGUSR1, _SC_signal_relay, pp, sz);
    SC_signal_n(SIGUSR2, _SC_signal_relay, pp, sz);
    SC_signal_n(SIGALRM, _SC_signal_relay, pp, sz);
    SC_signal_n(SIGTERM, _SC_signal_relay, pp, sz);
    SC_signal_n(SIGCONT, _SC_signal_relay, pp, sz);

    SC_signal_n(SIGINT,   _SC_ex_int_hnd, pp, sz);
    SC_signal_n(SC_SIGIO, _SC_ex_io_hnd,  pp, sz);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_TRM_IN - handle input from the terminal */

static void _SC_ex_trm_in(int fd, int mask, void *a)
   {char s[MAXLINE+1];
    char *p;
    descriptors *pd;
    SC_evlpdes *pe;
    PROCESS *pp;
    static int count = 0;

    pd = (descriptors *) a;
    pp = pd->pp;
    pe = pd->pe;

/* null out initial character in order to be clear that something
 * has been read or not
 */
    s[0] = '\0';

    _SC_reset_stdin(TRUE);

    if ((SC_isblocked_fd(fd) == 0) || (isatty(fd) == 0))
       {while ((p = SC_prompt(NULL, s, MAXLINE)) != NULL)
	   {WRITE_LOG(">", p);
	    SC_printf(pp, "%s", p);
	    SC_yield();};}
    else
       {p = SC_prompt(NULL, s, MAXLINE);
	if (p != NULL)
	   {WRITE_LOG(">", p);
	    SC_printf(pp, "%s", p);
	    SC_yield();};};

    _SC_reset_stdin(FALSE);

    _SC.tty_n_rej = 0;

/* if we have more than 8 consecutive blank strings remove the interrupt
 * callback on stdin - chances are the terminal has gone away and we
 * will only be getting EOFs which in turn means the application will eat
 * the whole CPU handling the infinite streams of EOFs
 */
    if (s[0] == '\0')
       {if (count > 8)
           SC_remove_event_loop_callback(pe, G_FILE_I, stdin);

	count++;}

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_TRM_REJ - handle poll reject messages from the terminal */

static void _SC_ex_trm_rej(int fd, int mask, void *a)
   {int nr;
    descriptors *pd;
    SC_evlpdes *pe;

    pd = (descriptors *) a;
    pe = pd->pe;

    nr = SC_running_children();
    SC_ASSERT(nr >= 0);

/* if we have more than 16 consecutive tty rejects remove the interrupt
 * callback on stdin - chances are the terminal has gone away and we
 * will only be getting EOFs which in turn means the application will eat
 * the whole CPU handling the infinite streams of EOFs
 */
    if (_SC.tty_n_rej > 16)
       SC_remove_event_loop_callback(pe, G_FILE_I, stdin);

    _SC.tty_n_rej++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SQUEEZE_TAG - remove all occurences of TAG from S
 *                 - return the number of occurences
 */

static int _SC_squeeze_tag(char *s, int ns, const char *tag)
   {int no, nt;
    char *ps, *p;
    static long nb = -1L;

    if (nb < ns)
       {nb = 2*ns;
	if (_SC.sqbf != NULL)
	   {CFREE(_SC.sqbf);};

	_SC.sqbf = CPMAKE_N(char, nb, 3);};

    no = 0;
    if ((s != NULL) && (tag != NULL))
       {nt = strlen(tag);

	_SC.sqbf[0] = '\0';

/* squeeze all occurances of TAG out of S into B and count occurences */
	for (ps = s, no = 0; TRUE; no++)
	    {p = strstr(ps, tag);
	     if (p == NULL)
	        {SC_strcat(_SC.sqbf, nb, ps);
		 break;}
	     else
	        {*p = '\0';
		 SC_strcat(_SC.sqbf, nb, ps);
		 ps = p + nt;};};

	SC_strncpy(s, ns, _SC.sqbf, -1);};

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_CH_OUT - handle output from the child process */

static void _SC_ex_ch_out(int fd, int mask, void *a)
   {int no, suppress;
    long ns;
    char *tag;
    PROCESS *pp;
    descriptors *pd;
    static long nb = -1;

    suppress = SC_mpi_suppress(-1);

    pd = (descriptors *) a;
    pp = pd->pp;

    tag = getenv("SC_MPI_TAG_IO");

/* make a big buffer to start out with */
    if (nb == -1L)
       {nb             = SIZE_BUF;
	_SC.ecbf       = CPMAKE_N(char, nb, 3);
	_SC.ecbf[nb-1] = '\0';
	_SC.elbf       = CPMAKE_N(char, nb, 3);
	_SC.elbf[nb-1] = '\0';};

    while (SC_gets(_SC.ecbf, nb-1, pp) != NULL)
       {WRITE_LOG("<", _SC.ecbf);

	ns = strlen(_SC.ecbf);

/* mixing C and Fortran output can result in a bare tag - ignore the line */
	if ((tag != NULL) && (strcmp(_SC.ecbf, tag) == 0))
	   continue;

/* remove suppress-on tags and if there was one turn suppression on */
	no = _SC_squeeze_tag(_SC.ecbf, nb, "+SC_SUPPRESS_UNTAGGED_ON+\n");
        if (no > 0)
	   SC_mpi_suppress(TRUE);

/* remove suppress-off tags and if there was one turn suppression off */
	no = _SC_squeeze_tag(_SC.ecbf, nb, "+SC_SUPPRESS_UNTAGGED_OFF+\n");
        if (no > 0)
	   SC_mpi_suppress(FALSE);

/* remove marker tags and get the number of them in the message */
	no = _SC_squeeze_tag(_SC.ecbf, nb, tag);
	    
/* get the suppression state */
	suppress = SC_mpi_suppress(-1);

/* filter out occurence of SC_DEFEAT_MPI_BUG */
	if ((no > 0) || (suppress == FALSE))
	   {_SC_squeeze_tag(_SC.ecbf, nb, SC_DEFEAT_MPI_BUG);
            if (_SC.ecbf[0] != '\0')
	       {printf("%s", _SC.ecbf);
	        SC_strncpy(_SC.elbf, nb, _SC.ecbf, -1);};};

/* if we are even close enlarge the buffer */
	if (2*ns > nb)
	   {nb = 2*ns;
	    CREMAKE(_SC.ecbf, char, nb);
	    _SC.ecbf[nb-1] = '\0';};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_CH_REJ - handle poll rejects from the child process */

static void _SC_ex_ch_rej(int fd, int mask, void *a)
   {int nr;

    nr = SC_check_children();
    SC_ASSERT(nr >= 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_END - return TRUE iff the process has exited
 *                 - return the exit status in RV
 */

static int _SC_process_end(int *prv, void *a)
   {int ex, fd;
    PROCESS *pp;
    descriptors *pd;

    pd = (descriptors *) a;
    pp = pd->pp;
    fd = pd->fd;

    ex = FALSE;
    if (SC_process_status(pp) != SC_RUNNING)
       {*prv = pp->reason;

/*      SC_sleep(100); */
	_SC_ex_ch_out(fd, 0, a);

	ex = TRUE;};

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_SESSION - run a session */

static int _SC_do_session(PROCESS *pp,
			  PFFileCallback in, PFFileCallback irej,
			  PFFileCallback out, PFFileCallback orej)
   {int pi, rv;
    descriptors d;
    SC_evlpdes *pe;

    SC_set_io_interrupts(FALSE);

/* create the event loop state */
    pe = SC_make_event_loop(NULL, NULL, _SC_process_end, -1, -1, -1);

/* register the I/O channels for the event loop to monitor */
    pi  = SC_register_event_loop_callback(pe, G_FILE_I, stdin,
					  in, irej, -1);
    pi &= SC_register_event_loop_callback(pe, G_PROCESS_I, pp,
					  out, orej, -1);

    d.fd = -1;
    d.pp = pp;
    d.pe = pe;

    _SC_reset_stdin(FALSE);

    rv = SC_event_loop(pe, &d, -1);

    _SC_reset_stdin(TRUE);

    SC_free_event_loop(pe);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC_JOB - run the job specified by ARGV in MODE
 *             - passing signals from parent to child
 */

int SC_exec_job(char **argv, const char *mode, int flags,
		PFFileCallback in, PFFileCallback irej,
		PFFileCallback out, PFFileCallback orej)
   {int rv;
    SC_sigstate *ost;
    PROCESS *pp;

    if (in == NULL)
       in = _SC_ex_trm_in;

    if (irej == NULL)
       irej = _SC_ex_trm_rej;

    if (out == NULL)
       out = _SC_ex_ch_out;

    if (orej == NULL)
       orej = _SC_ex_ch_rej;

    ost = SC_save_signal_handlers(1, 40);

    rv  = 0;

    SC_setbuf(stdout, NULL);

/* set handlers so that child gets them */
    _SC_setup_relay(NULL);

    pp = SC_open(argv, NULL, mode, NULL);

/* set handlers with the process state */
    _SC_setup_relay(pp);

    if (SC_process_alive(pp))
       {

/* set line at a time mode on input from the process */
        if (flags & 1)
           SC_set_attr(pp, SC_LINE, TRUE);

/* set non-blocking mode on input from the process */
        if (flags & 2)
           SC_set_attr(pp, SC_NDELAY, TRUE);

/* set asynchronous mode on input from the process */
        if (flags & 4)
           SC_set_attr(pp, SC_ASYNC, TRUE);

	START_LOG;

	rv = _SC_do_session(pp, in, irej, out, orej);

	SC_close(pp);

	END_LOG;}

    else
       rv = 1;

    SC_restore_signal_handlers(ost, TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
