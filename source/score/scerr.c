/*
 * SCERR.C - error handling routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_ERROR_STACK - initialize error stack for thread ID */

void _SC_init_error_stack(SC_array *err, int id)
   {

    SC_INIT_ARRAY(err, "PERM|ERROR_STACK", SC_errdes, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ERROR_WHICH - set the initial state of the current error descriptor */

static SC_errdes *_SC_error_which(int wh)
   {int n, nr;
    SC_errdes *pe;
    SC_array *arr;

    arr = _SC_get_error_stack(-1);

    n = SC_array_get_n(arr);
    n = SC_array_set_n(arr, n+wh);

    if (wh < 0)
       nr = n + 1;
    else
       nr = n;

    pe = SC_array_get(arr, nr);

    return(pe);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ERROR_PUSH - push a new error state on the stack */

SC_errdes *SC_error_push(void)
   {SC_errdes *pe;

    pe = _SC_error_which(1);
    
    pe->message[0] = '\0';
    pe->active     = TRUE;

    return(pe);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ERROR_POP - pop the current error state off the stack
 *              - and reset it to an initial state
 */

SC_errdes *SC_error_pop(void)
   {SC_errdes *pe;

    pe = _SC_error_which(-1);

    pe->message[0] = '\0';
    pe->active     = FALSE;

    return(pe);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ERROR_MSG - return a pointer to the message in the previous
 *              - error state
 */

char *SC_error_msg(void)
   {char *s;
    SC_errdes *pe;

    pe = _SC_error_which(0);
    if (pe[1].message[0] != '\0')
       s = pe[1].message;
    else
       s = pe[0].message;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ERROR_EXPLANATION - explain an error exit status code
 *                      - return a text message of length NC in S
 *                      - for a given SCORE error code ST
 *                      - if S is NULL print the message to stdout
 *                      -   signo    - range   1 <= 63
 *                      -   internal - range  64 <= 71    (defined in score.h)
 *                      -   exec     - range  72 <= 79    (some in sclppsx.c)
 *                      -   future   - range  80 <= 127
 *                      -   errno    - range 128 <= 255
 */

int SC_error_explanation(int st, char *s, int nc)
   {int rv;
    char t[MAXLINE];
    char *ps;

    rv = TRUE;
    if (s == NULL)
       {ps = t;
	nc = MAXLINE;}
    else
       ps = s;

/* signalled */
    if ((1 <= st) && (st <= 63))
       snprintf(ps, nc, "Received signal %s", SC_signal_name(st));

/* internal */
    else if ((64 <= st) && (st <= 71))
       {switch (st)
	   {case SC_EXIT_TIMEOUT :
	         snprintf(ps, nc, "Timed out (%d)", st);
		 break;
	    case SC_EXIT_SELF :
	         snprintf(ps, nc, "Self termination (%d)", st);
		 break;
	    case SC_EXIT_INIT :
	         snprintf(ps, nc, "Initialization failure (%d)", st);
		 break;
	    case SC_EXIT_CORRUPT :
	         snprintf(ps, nc, "Corruption detected (%d)", st);
		 break;
	    case SC_EXIT_BAD :
	         snprintf(ps, nc, "Cannot continue (%d)", st);
		 break;
	    default :
	         snprintf(ps, nc, "Unknown internal error (%d)", st);
		 break;};}

/* exec */
    else if ((72 <= st) && (st <= 79))
       {switch (st)
	   {case SC_NO_SETSID :
	         snprintf(ps, nc, "setsid failed (%d)", st);
		 break;
	    case SC_NO_TTY :
	         snprintf(ps, nc, "Could not open /dev/tty (%d)", st);
		 break;
	    case SC_NO_CLOSE :
	         snprintf(ps, nc, "Could not close descriptor (%d)", st);
		 break;
	    case SC_NO_FMT :
	         snprintf(ps, nc, "Could not send binary formats (%d)", st);
		 break;
	    case SC_NO_EXEC :
	         snprintf(ps, nc, "execvp failed (%d)", st);
		 break;
	    default :
	         snprintf(ps, nc, "Unknown exec error (%d)", st);
		 break;};}

/* reserved */
    else if ((80 <= st) && (st <= 127))
       {snprintf(ps, nc, "Unknown system error (%d)", st);
        rv = FALSE;}

/* errno */
    else if ((128 <= st) && (st <= 255))
       {int ierr;
	char bf[MAXLINE];

	ierr = st - 128;
	SC_strerror(ierr, bf, MAXLINE);

	snprintf(ps, nc, "Encountered system error (%d) - %s", ierr, bf);};

    if (s == NULL)
       io_printf(stdout, "%s\n", t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_WARN - handle process related warnings */

void SC_warn(char *fmt, ...)
   {char *s, *err;
    SC_errdes *pe;

    pe = _SC_error_which(0);
    s  = pe->message;

    snprintf(s, MAXLINE, "\nWARNING: ");

    SC_VDSNPRINTF(TRUE, err, fmt);

    strcat(s, err);

    SFREE(err);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ERROR - handle process related error conditions */

void SC_error(int st, char *fmt, ...)
   {char *s, *err;
    SC_errdes *pe;

    pe = _SC_error_which(0);
    s  = pe->message;

    snprintf(s, MAXLINE, "\nERROR: ");

    SC_VDSNPRINTF(TRUE, err, fmt);

    strcat(s, err);

    SFREE(err);

    if (pe->active == TRUE)
       {

/* NOTE: we do not have to deactivate this frame just because
 * we got here - it is ok to throw this longjmp many times
 * this needs some other explicit control
        pe->active = FALSE;
 */
	LONGJMP(pe->cpu, st);}

    else
       io_printf(stdout, "uncaught process error>>> %s\n", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
