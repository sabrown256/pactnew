/*
 * SCSYSE.C - more routines for execing jobs
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_proc.h"

/* NOTE: Linux defines NSIG as the highest signal number (32)
 *       Nobody else seems to but the idea is very nice
 */
#ifndef NSIG
# define NSIG  32
#endif

typedef struct s_descriptors descriptors;

struct s_descriptors
   {int fd;
    PROCESS *pp;
    SC_evlpdes *pe;};

#ifdef DEBUG

# define START_LOG                                                           \
   {lgf = io_open("log.wrap", "w")

# define WRITE_LOG(_x, _s)                                                   \
    io_printf(lgf, "\n%s %s", _x, _s)

# define END_LOG                                                             \
    io_close(lgf);}

static FILE *lgf = NULL;

#else

# define START_LOG
# define WRITE_LOG(_x, _s)
# define END_LOG

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SIGNAL_RELAY - relay signals received by the parent
 *                  - to the child
 */

static void _SC_signal_relay(int sig)
   {

    if (sig < 0)
       return;

    if (SC_process_status(_SC.ex_pp) == SC_RUNNING)
       SC_send_signal(_SC.ex_pp->id, sig);

    SC_signal(sig, _SC_signal_relay);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_INT_HND - special relay for SIGINT */

static void _SC_ex_int_hnd(int sig)
   {

    SC_block_file(stdin);

    _SC_signal_relay(sig);

    SC_signal(sig, _SC_ex_int_hnd);

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

static void _SC_setup_relay(void)
   {

    SC_signal(SIGHUP,  _SC_signal_relay);
    SC_signal(SIGINT,  _SC_signal_relay);
    SC_signal(SIGQUIT, _SC_signal_relay);
#ifdef SIGIOT
    SC_signal(SIGIOT,  _SC_signal_relay);
#endif
    SC_signal(SIGUSR1, _SC_signal_relay);
    SC_signal(SIGUSR2, _SC_signal_relay);
    SC_signal(SIGALRM, _SC_signal_relay);
    SC_signal(SIGTERM, _SC_signal_relay);
    SC_signal(SIGCONT, _SC_signal_relay);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_TRM_IN - handle input from the terminal */

static void _SC_ex_trm_in(int fd, int mask, void *a)
   {char s[MAXLINE+1];
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

#ifdef HAVE_READLINE

    SC_set_io_attrs(fileno(stdin),
		    ICANON,    SC_TERM_LOCAL,    TRUE,
		    ECHO,      SC_TERM_LOCAL,    TRUE,
		    0);

    SC_block_file(stdin);

    if (SC_prompt(NULL, s, MAXLINE) != NULL)
       {WRITE_LOG(">", s);
	SC_printf(pp, "%s", s);};

    SC_set_io_attrs(fileno(stdin),
		    ICANON,    SC_TERM_LOCAL,    FALSE,
		    ECHO,      SC_TERM_LOCAL,    FALSE,
		    0);

#else

/* NOTE: we must unblock stdin here because we really do not
 * know how many "messages" there are
 * this way we take input until there is nothing available
 * now if you suspend with ctl-z and stdin is blocked the terminal
 * will get a stream of ctl-d and logout
 */
    SC_unblock_file(stdin);

    while (SC_fgets(s, MAXLINE, stdin) != NULL)
       {WRITE_LOG(">", s);
	SC_printf(pp, "%s", s);
	count = 0;
	s[0]  = '\0';};

    SC_block_file(stdin);

#endif

    _SC.tty_n_rej = 0;

/* if we have more than 8 consecutive blank strings remove the interrupt
 * callback on stdin - chances are the terminal has gone away and we
 * will only be getting EOFs which in turn means the application will eat
 * the whole CPU handling the infinite streams of EOFs
 */
    if (s[0] == '\0')
       {if (count > 8)
           SC_remove_event_loop_callback(pe, SC_FILE_I, stdin);

	count++;}

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_TRM_REJ - handle poll reject messages from the terminal */

static void _SC_ex_trm_rej(int fd, int mask, void *a)
   {int nr;
    descriptors *pd;
    SC_evlpdes *pe;
    PROCESS *pp;

    pd = (descriptors *) a;
    pp = pd->pp;
    pe = pd->pe;

    nr = SC_running_children();

/* if we have more than 16 consecutive tty rejects remove the interrupt
 * callback on stdin - chances are the terminal has gone away and we
 * will only be getting EOFs which in turn means the application will eat
 * the whole CPU handling the infinite streams of EOFs
 */
    if (_SC.tty_n_rej > 16)
       SC_remove_event_loop_callback(pe, SC_FILE_I, stdin);

    _SC.tty_n_rej++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SQUEEZE_TAG - remove all occurences of TAG from S
 *                 - return the number of occurences
 */

static int _SC_squeeze_tag(char *s, int ns, char *tag)
   {int no, nt;
    char *ps, *p;
    static long nb = -1L;

    if (nb < ns)
       {nb = 2*ns;
	if (_SC.sqbf != NULL)
	   {SFREE(_SC.sqbf);};

	_SC.sqbf = FMAKE_N(char, nb, "PERM|_SC_SQUEEZE_TAG:bf");};

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
   {int nc, no, suppress;
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
	_SC.ecbf       = FMAKE_N(char, nb, "PERM|_SC_EX_CH_OUT:ecbf");
	_SC.ecbf[nb-1] = '\0';
	_SC.elbf       = FMAKE_N(char, nb, "PERM|_SC_EX_CH_OUT:elbf");
	_SC.elbf[nb-1] = '\0';};

    nc = strlen(SC_DEFEAT_MPI_BUG);
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
	    REMAKE_N(_SC.ecbf, char, nb);
	    _SC.ecbf[nb-1] = '\0';};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EX_CH_REJ - handle poll rejects from the child process */

static void _SC_ex_ch_rej(int fd, int mask, void *a)
   {int nr;
    PROCESS *pp;
    descriptors *pd;

    pd = (descriptors *) a;
    pp = pd->pp;

    nr = SC_check_children();

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

    SC_io_interrupts_on = FALSE;

/* create the event loop state */
    pe = SC_make_event_loop(NULL, NULL, _SC_process_end, -1, -1, -1);

/* register the I/O channels for the event loop to monitor */
    pi  = SC_register_event_loop_callback(pe, SC_FILE_I, stdin,
					  in, irej, -1);
    pi &= SC_register_event_loop_callback(pe, SC_PROCESS_I, pp,
					  out, orej, -1);

    d.fd = -1;
    d.pp = pp;
    d.pe = pe;

#ifdef HAVE_READLINE

    int fd;

    fd = fileno(stdin);

    SC_unblock_fd(fd);

    SC_set_io_attrs(fd,
		    SC_NDELAY, SC_TERM_DESC,     FALSE,
		    ICANON,    SC_TERM_LOCAL,    FALSE,
		    ECHO,      SC_TERM_LOCAL,    FALSE,
		    0);
#endif

    rv = SC_event_loop(pe, &d, -1);

    SC_free_event_loop(pe);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC_JOB - run the job specified by ARGV in MODE
 *             - passing signals from parent to child
 */

int SC_exec_job(char **argv, char *mode, int flags,
		PFFileCallback in, PFFileCallback irej,
		PFFileCallback out, PFFileCallback orej)
   {int rv;
    SC_sigstate *ost;

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

    _SC_setup_relay();

    SC_signal(SIGINT, _SC_ex_int_hnd);
    SC_signal(SC_SIGIO, _SC_ex_io_hnd);

    SC_setbuf(stdout, NULL);

    _SC.ex_pp = SC_open(argv, NULL, mode, NULL);
    if (SC_process_alive(_SC.ex_pp))
       {

/* set line at a time mode on input from the process */
        if (flags & 1)
           SC_set_attr(_SC.ex_pp, SC_LINE, TRUE);

/* set non-blocking mode on input from the process */
        if (flags & 2)
           SC_set_attr(_SC.ex_pp, SC_NDELAY, TRUE);

/* set asynchronous mode on input from the process */
        if (flags & 4)
           SC_set_attr(_SC.ex_pp, SC_ASYNC, TRUE);

	START_LOG;

	rv = _SC_do_session(_SC.ex_pp, in, irej, out, orej);

	SC_close(_SC.ex_pp);

	END_LOG;}

    else
       rv = 1;

    SC_restore_signal_handlers(ost, TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
