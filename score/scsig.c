/*
 * SCSIG.C - signal control routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

typedef struct s_SC_thrsig SC_thrsig;

struct s_SC_thrsig
   {int sig;          /* signal number */
    int flg;          /* signal flagss */
    int nb;           /* size in bytes of space pointed to by ARG */
    PFSignal_handler hnd;
    void *arg;        /* argument for HND */
    va_list *va;};    /* arg list for sigaction place holder */

/* NOTE: Linux defines NSIG as the highest signal number (32)
 *       Nobody else seems to but the idea is very nice
 */
#ifndef NSIG
# define NSIG  32
#endif

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
 *             - return the old context
 */

SC_contextdes SC_signal_n(int sig, PFSignal_handler fn, void *a, int nb)
   {PFSignal_handler fo;
    SC_contextdes rv;

    fo = NULL;

    if ((0 < sig) && (sig < SC_NSIG) &&
	(sig != SIGKILL) && (sig != SIGSTOP))
       {

#ifdef HAVE_POSIX_SIGNALS

	struct sigaction na, oa;
     
        na.sa_flags   = SA_RESTART;
	na.sa_handler = fn;
	sigemptyset(&na.sa_mask);

	if (sigaction(sig, &na, &oa) != -1)
	   fo = oa.sa_handler;

#else

	fo = signal(sig, fn);

#endif
	};

    rv = SC_LOOKUP_CONTEXT(fo);

    SC_register_context((void *) fn, a, nb);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_ACTION_N - portable wrapper for sigaction semantics
 *                    - set FN to handle SIG signals
 *                    - register context A with FN so that it can
 *                    - be looked up when FN is called
 *                    - FLAGS modifies the signal handling process
 *                    - any remaining non-negative arguments are
 *                    - taken to be signals that are to be blocked during
 *                    - the execution of FN
 *                    - terminate the list with negative integer
 *                    - return the old context of FN
 */

SC_contextdes SC_signal_action_n(int sig, PFSignal_handler fn,
				 void *a, int nb,
				 int flags, ...)
   {PFSignal_handler fo;
    SC_contextdes rv;

#ifdef HAVE_POSIX_SIGNALS

    int st;
    struct sigaction oa;

    SC_VA_START(flags);

    st = _PS_nsigaction(&oa, sig, fn, flags, SC_VA_VAR);
    fo = (st == 0) ? oa.sa_handler : NULL;

    SC_VA_END;

#else

    fo = signal(sig, fn);

#endif

    rv = SC_LOOKUP_CONTEXT(fo);

    SC_register_context((void *) fn, a, nb);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RESTORE_SIGNAL_N - restore all signal handlers from SS */

void SC_restore_signal_n(int sig, SC_contextdes cd)
   {

    SC_signal_n(sig, cd.f, cd.a, cd.nb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SIGNAL_ACTION_T - per thread worker for SMP _SC_signal_action_t */

void *_SC_signal_action_t(void *a)
   {int sig, flg, st, nb;
    void *arg, *rv;
    va_list *va;
    struct sigaction oa;
    PFSignal_handler fo, fn;
    SC_thrsig *at;

    at = (SC_thrsig *) a;

    sig = at->sig;
    fn  = at->hnd;
    nb  = at->nb;
    flg = at->flg;
    va  = at->va;

    if (nb > 0)
       {arg = CMAKE_N(char, nb);
	memcpy(arg, at->arg, nb);}
    else
       arg = NULL;

    st = _PS_nsigaction(&oa, sig, fn, flg, *va);
    fo = (st == 0) ? oa.sa_handler : NULL;

/* set the context for this thread */
    SC_register_context((void *) fn, arg, nb);

    rv = (void *) fo;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_ACTION_T - portable wrapper for sigaction semantics
 *                    - do the following on each thread:
 *                    - set FN to handle SIG signals
 *                    - FLAGS modifies the signal handling process
 *                    - any remaining non-negative arguments are
 *                    - taken to be signals that are to be blocked during
 *                    - the execution of FN
 *                    - terminate the list with negative integer
 *                    - do the following globally:
 *                    - register context A with FN so that it can
 *                    - save byte size of space pointed to by A so that
 *                    - it may be copied in the threads
 *                    - be looked up when FN is called
 *                    - return the old context of FN
 */

SC_contextdes SC_signal_action_t(int sig, PFSignal_handler fn,
				 void *a, int nb,
				 int flags, ...)
   {int n, id;
    void *pa[1], *rt[1];
    PFPVoidAPV pf[1];
    PFSignal_handler fo;
    SC_thrsig at;
    SC_contextdes rv;

    id = SC_current_thread();

    SC_VA_START(flags);

/* set the handler for all threads only from the master thread */
    if ((id == 0) && (SC_in_threaded_region(-1) == FALSE))
       {at.sig = sig;
	at.hnd = fn;
	at.arg = a;
	at.nb  = nb;
	at.flg = flags;
	at.va  = &SC_VA_VAR;

	n     = SC_n_threads;
	pa[0] = &at;
	pf[0] = _SC_signal_action_t;

	SC_do_threads(1, &n, pf, pa, rt);

	fo = (PFSignal_handler) rt[0];}

/* set the handler for this thread only if not the master thread */
    else
       {int st;
	struct sigaction oa;

	st = _PS_nsigaction(&oa, sig, fn, flags, SC_VA_VAR);
	fo = (st == 0) ? oa.sa_handler : NULL;};

    SC_VA_END;

/* get the context for thread 0 */
    rv = SC_LOOKUP_CONTEXT(fo);

/* set the context for thread 0 */
    SC_register_context((void *) fn, a, nb);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_BLOCK - block a list of signals
 *                 - the last list item should be -1
 *                 - return TRUE iff successful
 */

int SC_signal_block(sigset_t *pos, ...)
   {int is, rv;

#ifdef HAVE_POSIX_SIGNALS

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

#ifdef HAVE_POSIX_SIGNALS

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

#ifdef HAVE_POSIX_SIGNALS

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

SC_contextdes SC_which_signal_handler(int sig)
   {PFSignal_handler fo;
    SC_contextdes rv;

    fo = NULL;

    if ((0 < sig) && (sig < SC_NSIG) &&
	(sig != SIGKILL) && (sig != SIGSTOP))
       {

#ifdef HAVE_POSIX_SIGNALS

	struct sigaction oa;
     
	if (sigaction(sig, NULL, &oa) != -1)
	   fo = oa.sa_handler;

#else

	fo = signal(sig, SIG_IGN);
	fo = signal(sig, fo);

#endif
	};

    rv = SC_LOOKUP_CONTEXT(fo);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_NAME - return the symbolic name of the given signal
 *                - return "unknown" if it cannot be determined
 */

char *SC_signal_name(int sig)
   {int i;

    if (_SC.signame == NULL)
       {_SC.signame = CPMAKE_N(char *, SC_NSIG, 3);

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
    SC_contextdes hnd;

    if (sigmn > sigmx)
       SC_SWAP_VALUE(int, sigmn, sigmx);

    sigmn = max(1,          sigmn);
    sigmx = min(SC_NSIG-1, sigmx);

    for (i = sigmn; i <= sigmx; i++)
        {sn  = SC_signal_name(i);
	 hnd = SC_signal_n(i, SIG_IGN, NULL, 0);
	 if (hnd.f != SIG_ERR)
	    SC_restore_signal_n(i, hnd);

	 if (hnd.f == SIG_IGN)
	    io_printf(stdout, "%12s(%2d) ignore     (SIG_IGN)\n", sn, i);

	 else if (hnd.f == SIG_ERR)
	    io_printf(stdout, "%12s(%2d) error      (SIG_ERR)\n", sn, i);

	 else if (hnd.f == SIG_DFL)
	    io_printf(stdout, "%12s(%2d) default    (SIG_DFL)\n", sn, i);

	 else

#if defined(AIX) || defined(SOLARIS)
	    io_printf(stdout, "%12s(%2d) 0x%08p\n", sn, i, hnd.f);
#else
	    io_printf(stdout, "%12s(%2d) %08p\n", sn, i, hnd.f);
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
    SC_contextdes hnd;
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
	{ss->hnd[i].f = NULL;
	 ss->hnd[i].a = NULL;};
      
    for (i = sigmn; i <= sigmx; i++)
        {hnd = SC_signal_n(i, SIG_IGN, NULL, 0);
	 if (hnd.f != SIG_ERR)
	    SC_restore_signal_n(i, hnd);

	 ss->hnd[i] = hnd;};

    return(ss);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RESTORE_SIGNAL_HANDLERS - restore all signal handlers from SS */

void SC_restore_signal_handlers(SC_sigstate *ss, int rel)
   {int i, mn, mx;
    SC_contextdes hnd;

    mn = ss->mn;
    mx = ss->mx;

    for (i = mn; i <= mx; i++)
        {hnd = ss->hnd[i];
	 SC_restore_signal_n(i, hnd);};

    if (rel == TRUE)
       {CFREE(ss);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_SIGNAL_HANDLERS - restore all signal handlers from SS */

void SC_set_signal_handlers(PFSignal_handler f, void *a, int nb,
			    int mn, int mx)
   {int i;

    for (i = mn; i <= mx; i++)
        SC_signal_n(i, f, a, nb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SETUP_SIG_HANDLERS - setup to handle a variety of signals */

void SC_setup_sig_handlers(PFSignal_handler hand, void *a, int nb, int fl)
   {PFSignal_handler hnd;

    if (fl == TRUE)
       hnd = hand;
    else
       hnd = SIG_IGN;

    SC_signal_n(SIGSEGV, hnd, a, nb);
    SC_signal_n(SIGBUS,  hnd, a, nb);
    SC_signal_n(SIGFPE,  hnd, a, nb);
    SC_signal_n(SIGILL,  hnd, a, nb);

    SC_signal_n(SIGINT,  hnd, a, nb);
    SC_signal_n(SIGHUP,  hnd, a, nb);
    SC_signal_n(SIGQUIT, hnd, a, nb);
    SC_signal_n(SIGTERM, hnd, a, nb);

    SC_signal_n(SIGALRM, hnd, a, nb);

    SC_signal_n(SIGTRAP, hnd, a, nb);
    SC_signal_n(SIGABRT, hnd, a, nb);

#ifdef SIGIOT
    SC_signal_n(SIGIOT,  hnd, a, nb);
#endif

    SC_signal_n(SIGPIPE, hnd, a, nb);
    SC_signal_n(SIGSYS,  hnd, a, nb);

    SC_signal_n(SIGUSR1, hnd, a, nb);
    SC_signal_n(SIGUSR2, hnd, a, nb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_HOLD - portable sighold */

int SC_signal_hold(int sig)
   {int rv;

    rv = -1;

#ifdef HAVE_POSIX_SIGNALS

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

    rv = -1;

#ifdef HAVE_POSIX_SIGNALS

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
