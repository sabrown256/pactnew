/*
 * LIBTIME.C - routines for timing
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 */

#include <sys/time.h>

#define TIME_SEC     500
#define TIME_HMS     501

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WALL_CLOCK_TIME - return the wall clock time in seconds and microseconds
 *                 - since the first call
 */

double wall_clock_time(void)
   {double sec, usec, rv;
    struct timeval t;
    struct timezone tz;
    static struct timeval t0;
    static int first = TRUE;

    gettimeofday(&t, &tz);

    if (first)
       {first = FALSE;
        t0 = t;};

    sec  = t.tv_sec  - t0.tv_sec;
    usec = t.tv_usec - t0.tv_usec;
    rv   = sec + usec*1.0e-6;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TIME_STRING - put an ASCII representation of the time T in TS */

char *time_string(char *ts, int nc, int fmt, double t)
   {int ti, hr, mn, sc;

    if (fmt == TIME_SEC)
       snprintf(ts, nc, "%.1f", t);

    else if (fmt == TIME_HMS)
       {ti = t;
	hr = ti / 3600;
	mn = (ti - hr*3600) / 60;
	sc = ti - 60*(mn + 60*hr);
	if (hr > 0)
	   snprintf(ts, nc, "%d:%02d:%02d", hr, mn, sc);
	else if (mn > 0)
	   snprintf(ts, nc, "%d:%02d", mn, sc);
	else
	   snprintf(ts, nc, "%d", sc);};

    return(ts);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/