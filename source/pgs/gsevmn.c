/*
 * GSEVMN.C - event loop management
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

#define DEBUG_TEXT 0

SC_evlpdes
 *_PG_ev_loop = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CATCH_INTERRUPTS - catch interrupts in the event loop if FLG it TRUE */

void PG_catch_interrupts(int flg)
   {

    SC_catch_event_loop_interrupts(_PG_ev_loop, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REMOVE_CALLBACK - remove a callback from the event loop */

void PG_remove_callback(int *pfd)
   {

    SC_remove_event_loop_callback(_PG_ev_loop, 0, pfd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LOOP_CALLBACK - register a callback with the event loop */

int PG_loop_callback(int type, void *p,
		     PFFileCallback acc, PFFileCallback rej,
		     int pid)
   {int fd;

    fd = SC_register_event_loop_callback(_PG_ev_loop,
					 type, p, acc, rej, -1);

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_POLL - poll the event loop for TO milliseconds
 *         - if IND is TRUE poll indefinitely
 *         - if IND is FALSE poll once
 */

void PG_poll(int ind, int to)
   {

    if (ind == TRUE)
       SC_event_loop(_PG_ev_loop, NULL, to);
    else
       SC_event_loop_poll(_PG_ev_loop, NULL, to);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CHECK_EVENTS - go wait TO milliseconds for any events to come in
 *                 - return negative value on error
 */

int PG_check_events(int flag, int to)
   {int ret;
    
    PG_catch_interrupts(flag);
    ret = SC_event_loop_poll(_PG_ev_loop, NULL, to);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FLUSH_EVENTS - throw away any pending events */

void PG_flush_events(PG_device *dev)
   {
    
    if ((dev != NULL) && (dev->flush_events != NULL))
       (*dev->flush_events)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_INPUT - respond to an input available interrupt */

static void _PG_get_input(int fd, int mask, void *a)
   {int n, nc;
    char bf[MAX_BFSZ], eof;
    char *p;

    _PG_gcont.input_nc = 0;

    fd  = fileno(stdin);
    eof = (char) EOF;

    if (_PG_gcont.input_nr != -1)
       n = SC_read_sigsafe(fd, bf, _PG_gcont.input_nr);
    else
       n = SC_read_sigsafe(fd, bf, MAX_BFSZ);

/* if we have no input buffer throw the input away (nobody asked for it)
 * and return to whoever called
 */
    if (_PG_gcont.input_bf != NULL)

/* find number of effective characters as the smaller
 * of the remaining available characters and the number read
 */
       {nc = _PG_gcont.input_ncx - 1 - _PG_gcont.input_nc;
	nc = min(nc, n);
	nc = max(nc, 0);

	if (n > 0)
	   {p  = _PG_gcont.input_bf + _PG_gcont.input_nc;
	    strncpy(p, bf, nc);
	    _PG_gcont.input_nc += nc;
	    _PG_gcont.input_bf[_PG_gcont.input_nc] = '\0';}

	else if (n == 0)
	   {if (bf[0] == eof)
	       _PG_gcont.input_bf = NULL;}

/* check for eof */
	else if (_PG_gcont.input_bf[_PG_gcont.input_nc-1] == eof)
	   {if (_PG_gcont.input_nc == 1)
	       _PG_gcont.input_bf = NULL;
	    else
	       _PG_gcont.input_bf[_PG_gcont.input_nc-1] = '\0';}

	else if (_PG_gcont.input_bf[_PG_gcont.input_nc-2] == eof)
	   {if (_PG_gcont.input_nc == 2)
	       _PG_gcont.input_bf = NULL;
	    else
	       _PG_gcont.input_bf[_PG_gcont.input_nc-2] = '\0';};

	LONGJMP(io_avail, ERR_FREE);};

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_INIT_EVENT_HANDLING - setup the event loop */

int PG_init_event_handling(void)
   {int fd;
    
/* create the event loop state */
    if (_PG_ev_loop == NULL)
       _PG_ev_loop = SC_make_event_loop(NULL, NULL, NULL, -1, -1, -1);

    ONCE_SAFE(TRUE, NULL)
       fd = PG_loop_callback(SC_FILE_I, stdin,
			     _PG_get_input, NULL, -1);
    END_SAFE;

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_EVENT_LOOP - return a pointer to the event loop */

SC_evlpdes *PG_get_event_loop(void)
   {
    
    return(_PG_ev_loop);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_NEXT_EVENT - return the next available event */

int PG_get_next_event(PG_event *ev)
   {int rv;

    rv = FALSE;
    if (_PG_gcont.get_event != NULL)
       {if (ev == NULL)
	   ev = &_PG_gcont.current_event;

	rv = (*_PG_gcont.get_event)(ev);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_EVENT_DEVICE - return the device which got the event EV */

PG_device *PG_get_event_device(PG_event *ev)
   {PG_device *dev;

    dev = NULL;

    if (_PG_gcont.event_device != NULL)
       {if (ev == NULL)
	   ev = &_PG_gcont.current_event;

	dev = (*_PG_gcont.event_device)(ev);};

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
