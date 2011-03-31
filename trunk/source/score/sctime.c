/*
 * SCTIME.C - compute and return the time since 12:00 Jan 1, 2000
 *          - to have a standard, portable utility
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, n;
    char *fmt;
    double since;
    time_t now, then;
    struct tm td;

    td.tm_sec   = 0;        /* seconds */
    td.tm_min   = 0;        /* minutes */
    td.tm_hour  = 0;        /* hours */
    td.tm_mday  = 1;        /* day of the month */
    td.tm_mon   = 0;        /* month */
    td.tm_year  = 100;      /* year */
    td.tm_wday  = 0;        /* day of the week */
    td.tm_yday  = 0;        /* day in the year */
    td.tm_isdst = -1;       /* daylight saving time */

    now   = time(NULL);
    then  = mktime(&td);
    since = now - then;

    fmt = SC_dsnprintf(TRUE, "%%.2f\n");

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-min") == 0)
            since /= 60.0;
	 else if (strcmp(v[i], "-hour") == 0)
            since /= 3600.0;
	 else if (strcmp(v[i], "-day") == 0)
            since /= (3600.0*24.0);
	 else if (strcmp(v[i], "-week") == 0)
            since /= (3600.0*24.0*7.0);
	 else if (strcmp(v[i], "-p") == 0)
            {n = atoi((const char *) v[++i]);
             n = min(n, 16);
             n = max(n, 0);
	     SFREE(fmt);
	     fmt = SC_dsnprintf(TRUE, "%%.%df\n", n);}
         else
	    {printf("\n");
             printf("Usage: ptime [-p #] [-min] [-hour] [-day] [-week]\n");
             printf("\n");
             printf("Print time since 12:00 January 1, 2000\n");
             printf("By default the time is given in seconds\n");
             printf("\n");
             printf("    p     number of decimal places to print (default 2)\n");
             printf("    min   return minutes since then\n");
             printf("    hour  return hours since then\n");
             printf("    day   return days since then\n");
             printf("    week  return weeks since then\n");
             return(1);};};

    printf(fmt, since);

    SFREE(fmt);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
