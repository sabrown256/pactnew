/*
 * SCSIG.C - signal control routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SEND_SIGNAL - send a signal SIG to process PID */

int SC_send_signal(int pid, int sig)
   {int rv;

    rv = kill(pid, sig);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_N - portable wrapper for signal semantics
 *             - use sigaction if at all possible
 *             - register context A with FNC so that it can
 *             - be looked up when FNC is called
 */

PFSignal_handler SC_signal_n(int sig, PFSignal_handler fnc, void *a)
   {PFSignal_handler rv;

#ifdef USE_POSIX_SIGNALS

    struct sigaction na, oa;
     
    rv = NULL;

    if ((0 < sig) && (sig < SC_NSIG) &&
	(sig != SIGKILL) && (sig != SIGSTOP))
       {na.sa_flags   = SA_RESTART;
	na.sa_handler = fnc;
	sigemptyset(&na.sa_mask);

	if (sigaction(sig, &na, &oa) != -1)
	   rv = oa.sa_handler;};

#else

    rv = signal(sig, fnc);

#endif

    SC_register_context(fnc, a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_ACTION_N - portable wrapper for sigaction semantics
 *                    - set FNC to handle SIG signals
 *                    - register context A with FNC so that it can
 *                    - be looked up when FNC is called
 *                    - FLAGS modifies the signal handling process
 *                    - any remaining non-negative arguments are
 *                    - taken to be signals that are to be blocked during
 *                    - the execution of FNC
 *                    - terminate the list with negative integer
 */

PFSignal_handler SC_signal_action_n(int sig, PFSignal_handler fnc, void *a,
				    int flags, ...)
   {PFSignal_handler rv;

#ifdef USE_POSIX_SIGNALS

    int is;
    struct sigaction na, oa;
    sigset_t *set;

    rv = NULL;

    if ((0 < sig) && (sig < SC_NSIG) &&
	(sig != SIGKILL) && (sig != SIGSTOP))
       {SC_MEM_INIT(struct sigaction, &oa);

	if (sigaction(sig, NULL, &oa) == 0)
	   rv = oa.sa_handler;

/* do nothing if the handler is the same as what is already in place */
	if (rv != fnc)
	   {na.sa_flags   = flags;
	    na.sa_handler = fnc;

	    set = &na.sa_mask;
	    sigemptyset(set);

	    SC_VA_START(flags);

	    while (TRUE)
	       {is = SC_VA_ARG(int);
		if (is < 0)
		   break;
		sigaddset(set, is);};

	    SC_VA_END;

	    sigaction(sig, &na, NULL);};};

#else

    rv = signal(sig, fnc);

#endif

    SC_register_context(fnc, a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_BLOCK - block a list of signals
 *                 - the last list item should be -1
 *                 - return TRUE iff successful
 */

int SC_signal_block(sigset_t *pos, ...)
   {int is, rv;

#ifdef USE_POSIX_SIGNALS

    sigset_t ns;

    rv = FALSE;

    sigemptyset(&ns);
    SC_VA_START(pos);

    while (TRUE)
       {is = SC_VA_ARG(int);
	if (is < 0)
	   break;
	else if (is == SC_ALL_SIGNALS)
	   {sigfillset(&ns);
	    break;}
	else if ((0 < is) && (is < SC_NSIG) &&
		 (is != SIGKILL) && (is != SIGSTOP))
	   sigaddset(&ns, is);};

    SC_VA_END;

    rv = sigprocmask(SIG_BLOCK, &ns, pos);

#else

/* GOTCHA: this does not really emulate blocking signals because,
 * although it ignores them, the original handlers are lost
 */

    rv = FALSE;

    SC_VA_START(pos);

    while (TRUE)
       {is = SC_VA_ARG(int);
	if (is < 0)
	   break;
	if ((0 < is) && (is < SC_NSIG) &&
	    (is != SIGKILL) && (is != SIGSTOP))
	   signal(is, SIG_IGN);};

    SC_VA_END;

#endif

    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_UNBLOCK - unblock a list of signals
 *                   - the last list item should be -1
 *                   - return TRUE iff successful
 */

int SC_signal_unblock(sigset_t *pos, ...)
   {int is, rv;

#ifdef USE_POSIX_SIGNALS

    sigset_t ns;

    rv = FALSE;

    sigemptyset(&ns);
    SC_VA_START(pos);

    while (TRUE)
       {is = SC_VA_ARG(int);
	if (is < 0)
	   break;
	else if (is == SC_ALL_SIGNALS)
	   {sigfillset(&ns);
	    break;}
	else if ((0 < is) && (is < SC_NSIG) &&
	    (is != SIGKILL) && (is != SIGSTOP))
	   sigaddset(&ns, is);};

    SC_VA_END;

    rv = sigprocmask(SIG_UNBLOCK, &ns, pos);

#else

/* GOTCHA: this does not really emulate unblocking signals because
 * the original handlers were lost and all we can do is set the
 * default handlers
 */

    rv = FALSE;

    SC_VA_START(pos);

    while (TRUE)
       {is = SC_VA_ARG(int);
	if (is < 0)
	   break;
	if ((0 < is) && (is < SC_NSIG) &&
	    (is != SIGKILL) && (is != SIGSTOP))
	   signal(is, SIG_DFL);};

    SC_VA_END;

#endif

    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DSIGACT - report signals blocked when SIG has been raised */

void dsigact(int sig)
   {

#ifdef USE_POSIX_SIGNALS

    int i;
    struct sigaction oa;
    sigset_t *set;

    sigaction(sig, NULL, &oa);
    set = &oa.sa_mask;

    io_printf(stdout, "Blocked during %s:\n", SC_signal_name(sig));
    for (i = 1; i < NSIG; i++)
        {if (sigismember(set, i) == TRUE)
	    io_printf(stdout, "   %s\n", SC_signal_name(i));};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_WHICH_SIGNAL_HANDLER - return the function currently attached to
 *                         - the signal SIG
 */

PFSignal_handler SC_which_signal_handler(int sig)
   {PFSignal_handler hnd;

    hnd = SC_signal(sig, SIG_IGN);
    SC_signal(sig, hnd);

    return(hnd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_NAME - return the symbolic name of the given signal
 *                - return "unknown" if it cannot be determined
 */

char *SC_signal_name(int sig)
   {int i;

    if (_SC.signame == NULL)
       {_SC.signame = SC_permanent(CMAKE_N(char *, SC_NSIG));

	for (i = 0; i < SC_NSIG; i++)
	    _SC.signame[i] = "unknown";

#ifdef SIGPOLL
	_SC.signame[SIGPOLL]   = "SIGPOLL";
#endif
#ifdef SIGMSG
	_SC.signame[SIGMSG]    = "SIGMSG";
#endif
#ifdef SIGEMT
	_SC.signame[SIGEMT]    = "SIGEMT";
#endif
#ifdef SIGLOST
	_SC.signame[SIGLOST]   = "SIGLOST";
#endif
#ifdef SIGPWR
	_SC.signame[SIGPWR]    = "SIGPWR";
#endif
#ifdef SIGVTALRM
	_SC.signame[SIGVTALRM] = "SIGVTALRM";
#endif
#ifdef SIGTRMIN
	_SC.signame[SIGRTMIN]  = "SIGRTMIN";
#endif
#ifdef SIGTRMAX
	_SC.signame[SIGRTMAX]  = "SIGRTMAX";
#endif

	_SC.signame[SIGHUP]   = "SIGHUP";
	_SC.signame[SIGINT]   = "SIGINT";
	_SC.signame[SIGQUIT]  = "SIGQUIT";
	_SC.signame[SIGKILL]  = "SIGKILL";
	_SC.signame[SIGABRT]  = "SIGABRT";
	_SC.signame[SIGTERM]  = "SIGTERM";
	_SC.signame[SIGSTOP]  = "SIGSTOP";

	_SC.signame[SIGTRAP]  = "SIGTRAP";
#ifdef SIGIOT
	_SC.signame[SIGIOT]   = "SIGIOT";
#endif
	_SC.signame[SIGSEGV]  = "SIGSEGV";
	_SC.signame[SIGBUS]   = "SIGBUS";
	_SC.signame[SIGFPE]   = "SIGFPE";
	_SC.signame[SIGILL]   = "SIGILL";

	_SC.signame[SIGUSR1]  = "SIGUSR1";
	_SC.signame[SIGUSR2]  = "SIGUSR2";

	_SC.signame[SIGPIPE]  = "SIGPIPE";
	_SC.signame[SIGIO]    = "SIGIO";

	_SC.signame[SIGALRM]  = "SIGALRM";
	_SC.signame[SIGPROF]  = "SIGPROF";

	_SC.signame[SIGCHLD]  = "SIGCHLD";
	_SC.signame[SIGCONT]  = "SIGCONT";
	_SC.signame[SIGTSTP]  = "SIGTSTP";
	_SC.signame[SIGTTIN]  = "SIGTTIN";
	_SC.signame[SIGTTOU]  = "SIGTTOU";
	_SC.signame[SIGURG]   = "SIGURG";
	_SC.signame[SIGXCPU]  = "SIGXCPU";
	_SC.signame[SIGXFSZ]  = "SIGXFSZ";
	_SC.signame[SIGWINCH] = "SIGWINCH";

	_SC.signame[SIGSYS]   = "SIGSYS";};

    return(_SC.signame[sig]);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DSIGHND - diagnostic print of signal handlers */

void dsighnd(int sigmn, int sigmx)
   {int i;
    char *sn;
    PFSignal_handler hnd;

    if (sigmn > sigmx)
       SC_SWAP_VALUE(int, sigmn, sigmx);

    sigmn = max(1,          sigmn);
    sigmx = min(SC_NSIG-1, sigmx);

    for (i = sigmn; i <= sigmx; i++)
        {sn  = SC_signal_name(i);
	 hnd = SC_signal(i, SIG_IGN);
	 if (hnd != SIG_ERR)
	    SC_signal(i, hnd);

	 if (hnd == SIG_IGN)
	    io_printf(stdout, "%12s(%2d) ignore     (SIG_IGN)\n", sn, i);

	 else if (hnd == SIG_ERR)
	    io_printf(stdout, "%12s(%2d) error      (SIG_ERR)\n", sn, i);

	 else if (hnd == SIG_DFL)
	    io_printf(stdout, "%12s(%2d) default    (SIG_DFL)\n", sn, i);

	 else

#if defined(AIX) || defined(SOLARIS)
	    io_printf(stdout, "%12s(%2d) 0x%08p\n", sn, i, hnd);
#else
	    io_printf(stdout, "%12s(%2d) %08p\n", sn, i, hnd);
#endif

        };

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SAVE_SIGNAL_HANDLERS - save all signal handlers from SIGMN to SIGMX
 *                         - in SIGARR
 */

SC_sigstate *SC_save_signal_handlers(int sigmn, int sigmx)
   {int i;
    PFSignal_handler hnd;
    SC_sigstate *ss;

    if (sigmn > sigmx)
       {i     = sigmn;
	sigmn = sigmx;
	sigmx = i;};

    sigmn = max(1,          sigmn);
    sigmx = min(SC_NSIG-1, sigmx);

    ss = CMAKE(SC_sigstate);

    ss->ignore = SIG_IGN;
    ss->deflt  = SIG_DFL;
    ss->error  = SIG_ERR;

    ss->mn = sigmn;
    ss->mx = sigmx;
    for (i = 0; i < SC_NSIG; i++)
	ss->hnd[i] = NULL;
      
    for (i = sigmn; i <= sigmx; i++)
        {hnd = SC_signal(i, SIG_IGN);
	 if (hnd != SIG_ERR)
	    SC_signal(i, hnd);

	 ss->hnd[i] = hnd;};

    return(ss);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RESTORE_SIGNAL_HANDLERS - restore all signal handlers from SS */

void SC_restore_signal_handlers(SC_sigstate *ss, int rel)
   {int i, mn, mx;
    PFSignal_handler hnd;

    mn = ss->mn;
    mx = ss->mx;

    for (i = mn; i <= mx; i++)
        {hnd = ss->hnd[i];
	 SC_signal(i, hnd);};

    if (rel == TRUE)
       {CFREE(ss);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_SIGNAL_HANDLERS - restore all signal handlers from SS */

void SC_set_signal_handlers(PFSignal_handler hnd, int mn, int mx)
   {int i;

    for (i = mn; i <= mx; i++)
        SC_signal(i, hnd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SETUP_SIG_HANDLERS - setup to handle a variety of signals */

void SC_setup_sig_handlers(PFSignal_handler hand, void *a, int fl)
   {PFSignal_handler hnd;

    if (fl == TRUE)
       hnd = hand;
    else
       hnd = SIG_IGN;

    SC_signal_n(SIGSEGV, a, hnd);
    SC_signal_n(SIGBUS,  a, hnd);
    SC_signal_n(SIGFPE,  a, hnd);
    SC_signal_n(SIGILL,  a, hnd);

    SC_signal_n(SIGINT,  a, hnd);
    SC_signal_n(SIGHUP,  a, hnd);
    SC_signal_n(SIGQUIT, a, hnd);
    SC_signal_n(SIGTERM, a, hnd);

    SC_signal_n(SIGALRM, a, hnd);

    SC_signal_n(SIGTRAP, a, hnd);
    SC_signal_n(SIGABRT, a, hnd);

#ifdef SIGIOT
    SC_signal_n(SIGIOT,  a, hnd);
#endif

    SC_signal_n(SIGPIPE, a, hnd);
    SC_signal_n(SIGSYS,  a, hnd);

    SC_signal_n(SIGUSR1, a, hnd);
    SC_signal_n(SIGUSR2, a, hnd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_HOLD - portable sighold */

int SC_signal_hold(int sig)
   {int rv;

#ifdef USE_POSIX_SIGNALS

    sigset_t os, ns;

    sigemptyset(&ns);
    sigaddset(&ns, sig);

    rv = sigprocmask(SIG_BLOCK, &ns, &os);

#else

# ifdef USE_SYSV
    rv = sighold(sig);
# endif

# ifdef USE_BSD
    rv = sigblock(sigmask(sig));
# endif

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_RELEASE - portable sigrelse */

int SC_signal_release(int sig)
   {int rv;

#ifdef USE_POSIX_SIGNALS

    sigset_t os, ns;

    sigemptyset(&ns);
    sigaddset(&ns, sig);

    rv = sigprocmask(SIG_UNBLOCK, &ns, &os);

#else

# ifdef USE_SYSV
    rv = sigrelse(sig);
# endif

# ifdef USE_BSD
    rv = sigsetmask(sig);
# endif

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
