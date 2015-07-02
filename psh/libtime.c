/*
 * LIBTIME.C - routines for timing
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 */

#ifndef LIBTIME

# define LIBTIME

# include "common.h"

# ifndef SCOPE_SCORE_PREPROC

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WALL_CLOCK_TIME - return the wall clock time in seconds and microseconds
 *                 - since the first call
 */

double wall_clock_time(void)
   {double sec, usec, rv;
    struct timeval t;
    static struct timeval t0;
    static int first = TRUE;

    gettimeofday(&t, NULL);

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

/* GET_DATE_S - return the date as a string */

char *get_date_s(void)
   {time_t t;
    char *p;
    static char s[BFLRG];

    p = NULL;
    t = time(NULL);
    if (t > 0)
       {nstrncpy(s, BFLRG, ctime(&t), -1);
	LAST_CHAR(s) = '\0';
	p = s;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_DATE_P - return the date as an array of integers
 *            - fill an integer array T with broken down time as follows:
 *            -  T[0] = second
 *            -  T[1] = minute
 *            -  T[2] = hour
 *            -  T[3] = day
 *            -  T[4] = month
 *            -  T[5] = year
 *            -  T[6] = week day
 *            -  T[7] = year day
 *            -  T[8] = daylight savings time flag
 *            - only fill in upto NT of these
 */

int get_date_p(int *t, int nt)
   {int it, rv;
    time_t ti;
    struct tm *bt;

    ti = time(NULL);
    bt = localtime(&ti);
    if (bt != NULL)
       {rv = TRUE;
	nt = min(nt, 9);
	for (it = 0; it < nt; it++)
	    {switch (it)
	        {case 0 :
		      t[it] = bt->tm_sec;
		      break;
		 case 1 :
		      t[it] = bt->tm_min;
		      break;
		 case 2 :
		      t[it] = bt->tm_hour;
		      break;
		 case 3 :
		      t[it] = bt->tm_mday;
		      break;
		 case 4 :
		      t[it] = bt->tm_mon + 1;
		      break;
		 case 5 :
		      t[it] = bt->tm_year + 1900;
		      break;
		 case 6 :
		      t[it] = bt->tm_wday + 1;
		      break;
		 case 7 :
		      t[it] = bt->tm_yday + 1;
		      break;
		 case 8 :
		      t[it] = bt->tm_isdst;
		      break;};};}
    else
       {rv = FALSE;
	for (it = 0; it < nt; it++)
	    t[it] = -1;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

# endif
#endif
