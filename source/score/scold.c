/*
 * SCOLD.C - for backward compatibility
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_mem.h"

void
 *(*SC_make_hook)(long ni, long bpi, char *name, int na) = SC_alloc_na,
 *(*SC_remake_hook)(void *p, long ni, long bpi, int na)  = SC_realloc_na;

int
 (*SC_sfree_hook)(void *p) = SC_free;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC - add a layer of control over the C level memory management
 *          - system to store extra information about the allocated spaces
 */

void *SC_alloc(long ni, long bpi, char *name)
   {void *rv;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.where.pfunc = name;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_ALLOC_N(ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC_NA - add a layer of control over the C level memory management
 *             - system to store extra information about the allocated spaces
 *             - iff NA TRUE fudge the accounting so that this block
 *             - will not show up in the bytes allocated count
 */

void *SC_alloc_na(long ni, long bpi, char *name, int na)
   {void *p;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = na;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.where.pfunc = name;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    p = _SC_ALLOC_N(ni, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC_NZ - add a layer of control over the C level memory management
 *             - system to store extra information about the allocated spaces
 *             - iff NA TRUE fudge the accounting so that this block
 *             - will not show up in the bytes allocated count
 *             - ZSP controls the initialization of the block
 *             - see SC_zero_space_n for values
 */

void *SC_alloc_nz(long ni, long bpi, char *name, int na, int zsp)
   {void *p;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.where.pfunc = name;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    p = _SC_ALLOC_N(ni, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC - add a layer of control over the C level memory management
 *            - system to store extra information about the allocated spaces
 */

void *SC_realloc(void *p, long ni, long bpi)
   {void *rv;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_REALLOC_N(p, ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC_NA - add a layer of control over the C level memory management
 *               - system to store extra information about the allocated spaces
 *               - iff NA TRUE fudge the accounting so that this block
 *               - will not show up in the bytes allocated count
 */

void *SC_realloc_na(void *p, long ni, long bpi, int na)
   {void *rv;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = na;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_REALLOC_N(p, ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC_NZ - add a layer of control over the C level memory management
 *               - system to store extra information about the allocated spaces
 *               - iff NA TRUE fudge the accounting so that this block
 *               - will not show up in the bytes allocated count
 *               - ZSP controls the initialization of the block
 *               - see SC_zero_space_n for values
 */

void *SC_realloc_nz(void *p, long ni, long bpi, int na, int zsp)
   {void *rv;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_REALLOC_N(p, ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE - the complementary routine for _SC_ALLOC_N
 *         - free all the space including the counter
 *         - return TRUE if successful and FALSE otherwise
 */

int SC_free(void *p)
   {int rv;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = -1;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_FREE_N(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_Z - the complementary routine for SC_alloc
 *           - free all the space including the counter
 *           - return TRUE if successful and FALSE otherwise
 */

int SC_free_z(void *p, int zsp)
   {int rv;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    opt.perm = FALSE;
    opt.na   = -1;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_FREE_N(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SC_STRSAVE - save string s somewhere
 *            - allocate one extra character so that firsttok won't kill
 *            - things in the one bad case
 */

char *SC_strsave(char *s)
   {char *p;
    int sz;

    DEPRECATED(2011-01-01);

    p = NULL;

    if (s != NULL)
       {sz = strlen(s) + 2;
	p  = CMAKE_N(char, sz);
	if (p != NULL)
	   {SC_strncpy(p, sz, s, -1);
	    p[sz-1] = '\0';};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRSAVEF - save string s somewhere remember its name
 *             - allocate one extra character so that firsttok won't kill
 *             - things in the one bad case
 */

char *SC_strsavef(char *s, char *name)
   {char *p;
    int sz;

    DEPRECATED(2011-01-01);

    p = NULL;

    if (s != NULL)
       {sz = strlen(s) + 2;
	p  = CMAKE_N(char, sz);
	if (p != NULL)
	   {SC_strncpy(p, sz, s, -1);
	    p[sz-1] = '\0';};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRSAVEN - save string s somewhere remember its name
 *             - allocate one extra character so that firsttok won't kill
 *             - things in the one bad case
 */

char *SC_strsaven(char *s, char *name)
   {int sz;
    char *p;
    SC_mem_opt opt;

    DEPRECATED(2011-01-01);

    p = NULL;

    if (s != NULL)
       {sz = strlen(s) + 2;

	opt.perm = FALSE;
	opt.na   = TRUE;
	opt.zsp  = -1;
	opt.typ  = G_STRING_I;
	opt.where.pfunc = name;
	opt.where.pfile = NULL;
	opt.where.line  = -1;

	p = _SC_ALLOC_N(sz, 1, &opt);
	if (p != NULL)
	   {SC_strncpy(p, sz, s, -1);
	    p[sz-1] = '\0';};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ZERO_SPACE - set zero space flag globally
 *               - return the original global value
 */

int SC_zero_space(int flag)
   {int rv;

    DEPRECATED(2011-01-01);

    rv = _SC.zero_space;
    if ((0 <= flag) && (flag <= 5))
       {SC_zero_space_n(flag, -2);
	_SC.zero_space = flag;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ZERO_ON_ALLOC - return TRUE iff zero_space is 1 or 2 */

int SC_zero_on_alloc(void)
   {int rv;

    DEPRECATED(2011-01-01);

    rv = SC_zero_on_alloc_n(-2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL - portable wrapper for signal semantics
 *           - use sigaction if at all possible
 */

PFSignal_handler SC_signal(int sig, PFSignal_handler fnc)
   {SC_contextdes rv;

    DEPRECATED(2011-01-01);

    rv = SC_signal_n(sig, fnc, NULL, 0);

    return(rv.f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_TERM_STATE - set the terminal state on the given descriptor
 *                   - don't do anything else if you can't get the
 *                   - parameters for the stream
 *                   - this also weeds out descriptors (e.g. sockets and
 *                   - pipes) for which this is inappropriate (PTY's need
 *                   - this stuff)
 *                   - return TRUE iff successful
 */

int SC_set_term_state(void *a, int fd)
   {int rv;

    rv = SC_term_set_state(fd, a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_TERM_STATE - get the terminal state on the given descriptor
 *                   - allocate and return a TERMINAL_STATE
 *                   - containing the old state
 *                   - don't do anything else if you can't get the
 *                   - parameters for the stream
 *                   - this also weeds out descriptors (e.g. sockets and
 *                   - pipes) for which this is inappropriate (PTY's need
 *                   - this stuff)
 */

void *SC_get_term_state(int fd, int size)
   {void *rv;

    rv = SC_term_get_state(fd, size);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

