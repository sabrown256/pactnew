/*
 * SCMEMO.C - interpose SCORE memory manager functions
 *          - for malloc, realloc, and free
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h"
#include "scope_mem.h"
#include "scope_exe.h"

#ifdef HAVE_DYNAMIC_LINKER

#define __USE_GNU

#include <dlfcn.h>

#endif

/*--------------------------------------------------------------------------*/

#ifdef HAVE_DYNAMIC_LINKER

/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_OVER - allocate NB bytes of memory */

void *_SC_alloc_over(size_t nb)
   {void *pr;

    if (_SC_mf.malloc == NULL)
       _SC_mf.malloc = (PFMalloc) dlsym(RTLD_NEXT, "malloc");

    pr = _SC_mf.malloc(nb);

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_OVER - free memory allocated by _SC_ALLOC_OVER */

void _SC_free_over(void *p)
   {

    if (_SC_mf.free == NULL)
       _SC_mf.free = (PFFree) dlsym(RTLD_NEXT, "free");

    _SC_mf.free(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_OVER - reallocate to NB bytes of memory a pointer P */

void *_SC_realloc_over(void *p, size_t nb)
   {void *pr;

    if (_SC_mf.realloc == NULL)
       _SC_mf.realloc = (PFRealloc) dlsym(RTLD_NEXT, "realloc");

    pr = _SC_mf.realloc(p, nb);

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USE_OVER_MEM - setup to use override memory functions
 *
 * #bind SC_use_over_mem fortran() python()
 */

void SC_use_over_mem(int on)
   {

    if (_SC_mf.malloc == NULL)
       _SC_mf.malloc = (PFMalloc) dlsym(RTLD_NEXT, "malloc");

    if (_SC_mf.free == NULL)
       _SC_mf.free = (PFFree) dlsym(RTLD_NEXT, "free");

    if (_SC_mf.realloc == NULL)
       _SC_mf.realloc = (PFRealloc) dlsym(RTLD_NEXT, "realloc");

    if (on)
       {_SC.alloc   = _SC_alloc_over;
	_SC.realloc = _SC_realloc_over;
	_SC.free    = _SC_free_over;}
    else
       {_SC.alloc   = malloc;
	_SC.realloc = realloc;
	_SC.free    = free;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MALLOC_LOC - look up the stack to see who called the malloc */

#ifdef HAVE_BFD

#include <execinfo.h>

#endif

static char *_SC_malloc_loc(void)
   {char *rv;

    rv = NULL;

#ifdef HAVE_BFD
    int n;
    char d[MAXLINE];
    char **out;
    void *bf[3];
    static int count = 0;
    static int resolv = TRUE;

    count++;

    SC_mem_over_mark(1);

    snprintf(d, MAXLINE, "BARE_MALLOC");

    if (count == 1)
       {n = backtrace(bf, 3);
	if (n > 2)
	   {char s[MAXLINE];
	    char *pt;

	    out = backtrace_symbols(bf, n);

	    pt = out[2];

/* get the file and line number */
            if ((pt != NULL) && (resolv == TRUE) && (SC_gs.argv != NULL))
	       {char pname[PATH_MAX];
		exedes *st;

		st = SC_gs.exe_info;
		if (st == NULL)
		   {SC_full_path(SC_gs.argv[0], pname, PATH_MAX);
		    st = SC_exe_open(pname, NULL, NULL,
				    TRUE, TRUE, TRUE, TRUE);
		    SC_gs.exe_info = st;};

		if (st != NULL)
		   {SC_storloc sl;

		    SC_exe_map_addr(&sl, st, pt);
		    _SC_format_loc(s, MAXLINE, &sl.loc, TRUE, TRUE);

		    pt = s;

/*		    SC_exe_close(st); */};};

	    snprintf(d, MAXLINE, "BARE_MALLOC:%s", pt);

	    free(out);};};

    rv = strdup(d);

    SC_mem_over_mark(-1);

    count--;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MALLOC - allocate NB bytes of memory */

void *malloc(size_t nb)
   {char *d;
    void *pr;

    SC_use_over_mem(TRUE);

    if ((_SC_init_emu_threads == 0) || (_SC_mf.sys > 0))
       pr = _SC_mf.malloc(nb);

    else
       {d = _SC_malloc_loc();

	pr = SC_alloc_n(nb, 1,
			SC_MEM_ATTR_FUNC, d,
			0);};

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE - free memory allocated by malloc */

void free(void *p)
   {

    SC_use_over_mem(TRUE);

    if ((_SC_init_emu_threads == 0) || (_SC_mf.sys > 0))
       _SC_mf.free(p);

    else
       CFREE(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REALLOC - reallocate to NB bytes of memory a pointer P */

void *realloc(void *p, size_t nb)
   {

    SC_use_over_mem(TRUE);

    if ((_SC_init_emu_threads == 0) || (_SC_mf.sys > 0))
       p = _SC_mf.realloc(p, nb);
    else
       CREMAKE(p, char, nb);

    return(p);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
