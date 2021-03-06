/*
 * SCFNCT.C - time functions for SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 

#ifdef HAVE_RESOURCE_USAGE
# include <sys/resource.h>
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LOCALTIME - wrap various thread standards notion of localtime_r */

struct tm *SC_localtime(const time_t *t, struct tm *tms)
   {struct tm *rv;

#if ((defined SMP_Pthread) || (defined PTHREAD_DRAFT4))
    rv = localtime_r(t, tms);
#else
    rv = localtime(t);
    if ((tms != NULL) && (rv != NULL))
       *tms = *rv;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DATE - Return a string with the time and date as defined by the
 *         - ANSI function ctime.  The string is dynamically allocated
 *         - and the caller is responsible for freeing it.
 *
 * #bind SC_date fortran() scheme(date) python()
 */

char *SC_date(void)
   {time_t tm;
    char t[MAXLINE], *pt, *d, *s;

    tm = time(NULL);
    pt = SC_ctime(tm, t, MAXLINE);
    if (pt != NULL)
       d = CSTRSAVE(SC_strtok(t, "\n", s));
    else
       d = NULL;

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_TIME - return the seconds and microseconds since the epoc */

void SC_get_time(double *psec, double *pmusec)
   {double t[2];

/* NOTE: timespec give resolution in nanoseconds and timeval gives
 * resolution in microseconds
 * however clock_gettime requires the use of librt.a on Linux systems
 * and is not appreciably faster than gettimeofday
 * not sure that we are really getting nanosecond resolution anyway
 * so go the simpler route with gettimeofday
 */

    struct timeval ts;

    gettimeofday(&ts, NULL);

    t[0] = ts.tv_sec;
    t[1] = ts.tv_usec;

    if (psec != NULL)
       *psec = t[0];

    if (pmusec != NULL)
       *pmusec = t[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DATEF - return a string with the time and date in the form
 *          -    yy/mm/dd hh:mm:ss
 *          - and <ss> is seconds to the microsecond
 *          - this is finer resolution than SC_date and different
 *          - format
 */

char *SC_datef(void)
   {double ts, tus;
    char *d;
    struct tm tc, *ptc, safe;
    time_t ti;

    SC_get_time(&ts, &tus);

    ti  = ts;
    ptc = SC_localtime(&ti, &safe);
    if (ptc != NULL)
       {tc = *ptc;
	d  = SC_dsnprintf(TRUE, "%4d/%02d/%02d %02d:%02d:%02d.%06ld",
			  1900+tc.tm_year, tc.tm_mon+1, tc.tm_mday,
			  tc.tm_hour, tc.tm_min, tc.tm_sec, (long) tus);}
    else
       d = NULL;

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CPU_TIME - Return the CPU time used in seconds and microseconds
 *             - since the first call.  The first call returns zero.
 *
 * #bind SC_cpu_time fortran() scheme(cpu-time) python()
 */

double SC_cpu_time(void)
   {double dt;

#ifdef HAVE_RESOURCE_USAGE

    double ut, st;
    struct rusage r;
    static struct rusage r0;

    getrusage(RUSAGE_SELF, &r);

    ONCE_SAFE(TRUE, NULL)
       r0 = r;
    END_SAFE;

    ut = (r.ru_utime.tv_sec  - r0.ru_utime.tv_sec) +
         (r.ru_utime.tv_usec - r0.ru_utime.tv_usec)*1.0e-6;

    st = (r.ru_stime.tv_sec  - r0.ru_stime.tv_sec) +
         (r.ru_stime.tv_usec - r0.ru_stime.tv_usec)*1.0e-6;

    dt = ut + st;

#else

    double dtwc;
    time_t syst;
    clock_t s;
    static double dtref;
    static time_t systref;
    static clock_t NA = (clock_t) -1;

    syst = time(NULL);

    while ((s = clock()) == NA);

    dt = ((double) s)/((double) TICKS_SECOND);

    ONCE_SAFE(TRUE, NULL)
       dtref   = dt;
       systref = syst;
    END_SAFE;

    dtwc = syst - systref;

    if (dtwc >= ((double) LONG_MAX / (double) TICKS_SECOND - 1.0))
       dt = dtwc;

    else if (dt >= dtref)
       dt = dt - dtref;

    else
       dt = LONG_MAX + dt - dtref;

#endif

    return(dt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_WALL_CLOCK_TIME - Return the wall clock time in seconds and microseconds
 *                    - since the first call.  The first call returns zero.
 *
 * #bind SC_wall_clock_time fortran() scheme(wall-clock-time) python()
 */

double SC_wall_clock_time(void)
   {double dt;

#ifdef HAVE_RESOURCE_USAGE

    double sc, ms;
    double t[2];
    static double t0[2];

    SC_get_time(&t[0], &t[1]);

    ONCE_SAFE(TRUE, NULL)
       t0[0] = t[0];
       t0[1] = t[1];
    END_SAFE;

    sc = t[0] - t0[0];
    ms = t[1] - t0[1];
    dt = sc + ms*1.0e-6;

#else

    time_t t;
    static time_t t0;

    t = time(NULL);

    ONCE_SAFE(TRUE, NULL)
       t0 = t;
    END_SAFE;

    dt = t - t0;

#endif

    return(dt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STR_TIME - given a time D in [[[dd-]hh:]mm:]ss form
 *             - return the time in seconds
 */

double SC_str_time(const char *d)
   {int i;
    double t, dy;
    char s[MAXLINE];
    char *p, *ps;

    strncpy(s, d, MAXLINE-1);
    s[MAXLINE-1] = '\0';

/* handle days */
    p = strchr(s, '-');
    if (p != NULL)
       {*p++ = '\0';
	ps   = p;
	dy   = SC_stoi(s);}
    else
       {ps = s;
	dy = 0.0;};

/* handle [[hh:]mm:]ss */
    t = 0.0;
    for (i = 0; i < 3; i++)
        {p = strchr(ps, ':');
	 if (p != NULL)
	    {*p++ = '\0';
	     t    = 60.0*t + SC_stoi(ps);
	     ps   = p;}
	 else
	    {t += 60.0*t + SC_stof(ps);
	     break;};};

/* add in day contribution */
    t += 24.0*3600.0*dy;

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TIME_STR - given a time T in seconds from the epoch
 *             - return a string in D with the time and date in the form
 *             -    yy/mm/dd hh:mm:ss
 */

void SC_time_str(unsigned long t, char *d, long nb)
   {struct tm tc, safe, *ptc;
    time_t ti;

    ti  = (time_t) t;
    ptc = SC_localtime(&ti, &safe);
    if (ptc != NULL)
       {tc = *ptc;
	snprintf(d, nb, "%4d/%02d/%02d %02d:%02d:%02d",
		 1900+tc.tm_year, tc.tm_mon+1, tc.tm_mday,
		 tc.tm_hour, tc.tm_min, tc.tm_sec);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SEC_STR - given a time T in seconds
 *            - return a string in D with the time and date in the form
 *            -    hh:mm:ss
 */

void SC_sec_str(double t, char *d, long nb)
   {int ft, sc, mn, hr;

    sc = (int) t;
    ft = 100.0*(t - sc);

    mn = sc / 60;
    sc = sc % 60;

    hr = mn / 60;
    mn = mn % 60;

    if (hr != 0)
       snprintf(d, nb, "%d:%02d:%02d", hr, mn, sc);
    else if (mn != 0)
       snprintf(d, nb, "%d:%02d.%02d", mn, sc, ft);
    else
       snprintf(d, nb, "%d.%02d", sc, ft);

    return;}

/*--------------------------------------------------------------------------*/

/*                            TIMEOUT ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _SC_GET_TO_BUF - return the JMP_BUF for timeouts for thread ID
 *                - use current thread if ID == -1
 */

JMP_BUF *_SC_get_to_buf(int id)
   {JMP_BUF *bf;
    SC_smp_state *pa;

    pa = _SC_get_state(id);
    bf = &pa->time_out;

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TIMEOUT_ERROR - tell somebody that we timed out and exit */

static void _SC_timeout_error(int sig)
   {

#if defined(HAVE_POSIX_SYS)

    char s[MAXLINE];

    snprintf(s, MAXLINE, "\nProcess %d timed out\n\n", (int) getpid());

/* NOTE: do not use io_puts here because timeouts often occur because
 * of memory deadlocks and io_puts allocates more memory so we would
 * hang here
 */
    fputs(s, stdout);

#endif

    exit(SC_EXIT_TIMEOUT);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TIMEOUT - process will timeout and call FNC in TO seconds if
 *            - a second call to SC_timeout isn't made with
 *            - a value of TO = 0 before then
 *            - if FNC is NULL the process will exit on timeout
 */

void SC_timeout(int to, PFSignal_handler fnc, void *a, int nb)
   {

#if defined(HAVE_POSIX_SYS)

    int nn, ns;
    void *an;
    PFSignal_handler fn;

/* since alarms do not nest (without much more logic than we have here)
 * do nothing if there is an active handler going
 */
    if ((_SC.to_lst == (PFSignal_handler) -1) || (_SC.to_lst == fnc))

/* settle on the handler */
       {fn = fnc;
	an = a;
	nn = nb;
	if (fnc == NULL)
	   {if (to > 0)
	       {fn = _SC_timeout_error;
		an = a;
	        nn = -1;}
	    else
	       {fn = _SC.to_err.f;
		an = _SC.to_err.a;
	        nn = _SC.to_err.nb;};};

/* set the handler and the alarm */
	_SC.to_err = SC_signal_n(SIGALRM, fn, an, nn);
	ns         = ALARM(to);
	SC_ASSERT(ns >= 0);

	if (_SC.to_err.f == SIG_ERR)
	   SC_error(-1, "SETTING SIGALRM FAILED (%d) - SC_TIMEOUT\n", errno);

/* set _SC.to_lst appropriately */
	if (to == 0)
	   _SC.to_lst = (PFSignal_handler) -1;
        else
	   _SC.to_lst = fnc;};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TIMEOUT_CONT - handle timeouts for SC_time_allow */

NORETURN void _SC_timeout_cont(int sig)
   {JMP_BUF *cpu;

/* io_printf(stdout, "Timeout %d\n", sig); */
    SC_timeout(0, _SC_timeout_cont, NULL, 0);
    
    cpu = _SC_get_to_buf(-1);

    LONGJMP(*cpu, 1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
