/*
 * LIBLOG.C - routines supporting logging
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBLOG

# define LIBLOG

# ifndef SCOPE_SCORE_PREPROC

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ACCESS_LOG_FILE - wrap this accessor function around the log file */

FILE *_access_log_file(int set, FILE *in)
   {static FILE *lg = NULL;

    if (set == TRUE)
       lg = in;

    return(lg);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NOTE - write to FP only */

void note(FILE *fp, char *fmt, ...)
   {char bf[BFMG];

    if (fp == NULL)
       fp = _access_log_file(FALSE, NULL);

    if (fp != NULL)
       {VA_START(fmt);
	VSNPRINTF(bf, BFMG, fmt);
	VA_END;

	fputs(bf, fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NOTED - write to FP and to TTY */

void noted(FILE *fp, char *fmt, ...)
   {char bf[BFMG];

    VA_START(fmt);
    VSNPRINTF(bf, BFMG, fmt);
    VA_END;

    if (fp == NULL)
       fp = _access_log_file(FALSE, NULL);

    if (fp != NULL)
       fputs(bf, fp);

    fputs(bf, stdout);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEPARATOR - write a line of dashes to FP */

void separator(FILE *fp)
   {

    if (fp == NULL)
       fp = _access_log_file(FALSE, NULL);

    if (fp != NULL)
       fprintf(fp, "--------------------------------------------------------------------------\n\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CAT - add NCAT lines FNAME to OUT
 *     - skip the first NSKIP lines
 *     - a value of -1 for NCAT becomes INT_MAX which is the whole file
 */

void cat(FILE *out, size_t nskip, size_t ncat, char *fmt, ...)
   {int i;
    char fname[BFLRG], s[BFLRG];
    FILE *in;

    if (out == NULL)
       out = _access_log_file(FALSE, NULL);

    VA_START(fmt);
    VSNPRINTF(fname, BFLRG, fmt);
    VA_END;

    in = fopen_safe(fname, "r");
    if (in != NULL)
       {for (i = 0; fgets(s, BFLRG, in) != NULL; i++)
            {if ((nskip <= i) && (i < ncat))
	        fputs(s, out);};

	fclose_safe(in);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INITLOG - setup a log file for use by run, note, noted, and separator */

FILE *initlog(char *mode, char *fmt, ...)
   {char bf[BFMG];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(bf, BFMG, fmt);
    VA_END;

    fp = fopen_safe(bf, mode);
    setbuf(fp, NULL);

    fp = _access_log_file(TRUE, fp);

    if (mode[0] == 'w')
       {note(fp, "Date: \n");
	separator(fp);};

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINLOG - close the log file */

void finlog(void)
   {FILE *fp;

    fp = _access_log_file(FALSE, NULL);

    fclose_safe(fp);

    _access_log_file(TRUE, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* undefine when passing out of SCOPE_SCORE_COMPILE */

#undef UNDEFINED

# endif
#endif
