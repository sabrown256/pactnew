/*
 * SCTRACE.C - routines for tracing code flow
 *           - not THREADED or THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

typedef struct s_stackent stackent;

struct s_stackent
   {const char *file;
    const char *fnc;
    int line;};

static SC_array
 *_SC_stack = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TRACE_ENTERING - note when entering a function */

void _SC_trace_entering(const char *file, const char *fnc, int line)
   {stackent s;

    if (_SC_stack == NULL)
       _SC_stack = SC_MAKE_ARRAY("TRACE", stackent, NULL);

    s.file = file;
    s.fnc  = fnc;
    s.line = line;

    SC_array_push(_SC_stack, &s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TRACE_LEAVING - note when leaving a function */

void _SC_trace_leaving(const char *file, const char *fnc, int line)
   {stackent *s;

    if (_SC_stack == NULL)
       _SC_stack = SC_MAKE_ARRAY("TRACE", stackent, NULL);

    s = SC_array_pop(_SC_stack);

    SC_ASSERT(s != NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TRACE_SHOW - print the stack trace */

void _SC_trace_show(int n)
   {int i, ns;
    stackent *s;

    if (_SC_stack != NULL)
       {ns = SC_array_get_n(_SC_stack);
	n  = min(n, ns);
	for (i = 0; i < n; i++)
	    {s = SC_array_get(_SC_stack, i);
	     io_printf(STDOUT, "%8d: %-32s  (%s:%d)\n",
		       i, s->fnc, s->file, s->line);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
