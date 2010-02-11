/*
 * SCLPW32.C - low level process control - WIN32 routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_RELEASE - close the file descriptors */

static int _SC_win32_release(PROCESS *pp)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_SETUP_TTY - set up the communication channel to the child
 *                     - as if it were a terminal
 */

static int _SC_win32_setup_tty(PROCESS *pp, int child)
   {int rv;

    rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_EXEC - exec the named program together with args and env
 *                - in the new pro
 *                - if successful this routine should not return
 *                - return FALSE indicating failure
 */

static int _SC_win32_exec(PROCESS *cp, char **argv, char **env, char *mode)
   {int err;

    err = 0;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_STATUS - return the status of the process */

static int _SC_win32_status(PROCESS *pp)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_GETS - get bytes from the input stream
 *                - and return the number of bytes placed in
 *                - the buffer
 */

static int _SC_win32_gets(char *bf, int len, PROCESS *pp)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_IN_READY - return input ready status of process */

static int _SC_win32_in_ready(PROCESS *pp)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_CLOSE - kill the process associated with PP
 *                 - we do NOT free the process structure because the
 *                 - SIGCHILD may come in later and the signal handler
 *                 - will consult the managed process list to find
 *                 - this process
 *                 - return TRUE iff the process was closed
 *                 - return FALSE otherwise
 */

static int _SC_win32_close(PROCESS *pp)
   {int rv;

    rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_FLUSH - flush the input and output streams
 *                 - for the given process
 */

static int _SC_win32_flush(PROCESS *pp)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_PRINTF - do an fprintf style output to a process */

static int _SC_win32_printf(PROCESS *pp, char *buffer)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WIN32_PROCESS - register the WIN 32 functions with process PP */

void _SC_win32_process(PROCESS *pp)
   {

    pp->release     = _SC_win32_release;
    pp->exec        = _SC_win32_exec;
    pp->close       = _SC_win32_close;
    pp->statusp     = _SC_win32_status;
    pp->flush       = _SC_win32_flush;
    pp->printf      = _SC_win32_printf;
    pp->gets        = _SC_win32_gets;
    pp->in_ready    = _SC_win32_in_ready;
    pp->setup       = _SC_win32_setup_tty;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

