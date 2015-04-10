/*
 * SCEXE.C - routines to access executable symbol table
 *         - derived from addr2line.c
 *         - see http://sourceware.org/binutils/docs/bfd/BFD-front-end.html
 *         -  for documentation of BFD API
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_dl.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_STORLOC - setup SL to that its SC_strloc points to its spaces */

SC_srcloc *_SC_init_storloc(SC_storloc *sl)
   {SC_srcloc *loc;

    memset(sl, 0, sizeof(SC_storloc));

    loc = &sl->loc;
    loc->line  = -1;
    loc->pfile = sl->file;
    loc->pfunc = sl->func;

    return(loc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_COPY_STORLOC - copy SC_storloc S into D */

void _SC_copy_storloc(SC_storloc *d, SC_storloc *s)
   {int nc;

    if ((d != NULL) && (s != NULL))
       {_SC_init_storloc(d);

	nc = sizeof(s->file);

	d->loc.line = s->loc.line;

	strncpy(d->file, s->file, nc);
	strncpy(d->func, s->func, nc);};

    return;}

/*--------------------------------------------------------------------------*/

#ifdef HAVE_BFD

/*--------------------------------------------------------------------------*/

#include <bfd.h>

#include "bfd.api"

/* NOTE: bfd functional macros are invisible to import-api because
 * the preprocessor expands them
 * consequently we have to define our own versions
 */
#define GET_FILE_FLAGS         bfd_get_file_flags
#define GET_SECTION_FLAGS      bfd_get_section_flags
#define GET_SECTION_SIZE       bfd_get_section_size
#define GET_SECTION_VMA        bfd_get_section_vma
#define FIND_NEAREST_LINE      bfd_find_nearest_line
#define FIND_INLINER_INFO      bfd_find_inliner_info
#define READ_MINISYMBOLS       bfd_read_minisymbols

/* NOTE: Ubuntu 10.4 has version 2.20,
 * Ubuntu 10.10 has version 2.20.51, and
 * RHE 5 has 2.17.50
 */

#ifdef HAVE_DEMANGLE
# include <demangle.h>
# define SC_DEMANGLE_ARG   (DMGL_ANSI | DMGL_PARAMS)
#else
# define SC_DEMANGLE_ARG   3
#endif

/* the following macros hide the conversion from the generic
 * pointers in the exedes to the specific BFD related ones
 * used here
 */

#define GET_ET(_st)    ((bfd *)      (_st)->et)
#define GET_ES(_st)    ((asection *) (_st)->es)
#define GET_SYMT(_st)  ((asymbol **) (_st)->symt)

/* SAVE_LOC - do the save location code in one place */

#define SAVE_LOC(st, sl)                                                    \
   {SC_strncpy(st->where.file, MAXLINE, (char *) sl.pfile, -1);             \
    SC_strncpy(st->where.func, MAXLINE, (char *) sl.pfunc, -1);             \
    st->where.loc.line = sl.line;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_DEMANGLE_STYLE - set the C++ name demangling style */

void _SC_set_demangle_style(char *opt)
   {

#ifdef HAVE_DEMANGLE
    enum demangling_styles style;

    if (opt != NULL)
       {style = cplus_demangle_name_to_style(opt);
	if (style == unknown_demangling)
	   printf("unknown demangling style '%s'\n", opt);
	cplus_demangle_set_style(style);};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_DEMANGLE_NAME - demangle the current location name in ET */

void _SC_exe_demangle_name(char *d, int nc, exedes *st)
   {

#ifdef HAVE_DEMANGLE

    char *p, *nm;
    bfd *et;
    
    et = GET_ET(st);

    nm = (char *) st->where.func;
    p = bfd_demangle(et, nm, SC_DEMANGLE_ARG);
    if (p != NULL)
       {SC_strncpy(d, nc, p, -1);
	free(p);}

    else
       SC_strncpy(d, nc, nm, -1);
       
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_CHECK_FORMATS - report possible format matches for ET */

static int _SC_exe_check_formats(bfd *et)
   {int ok;

    ok = TRUE;

#if defined(BFD_MAJOR_VERSION)
    char **matching;

    if (bfd_check_format_matches(et, bfd_object, &matching) == 0)
       {ok = FALSE;
	if (bfd_get_error() == bfd_error_file_ambiguously_recognized)
	   {SC_strings_print(stdout, matching, "-> ");
	    free(matching);};};
#endif

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_RD_SYMT - read in the symbol table
 *                 - return the number of symbols or
 *                 - on error -1
 */

static long _SC_exe_rd_symt(exedes *st)
   {unsigned int sz;
    long ns;
    bfd *et;

    et = GET_ET(st);

    ns = -1;
    if ((GET_FILE_FLAGS(et) & HAS_SYMS) != 0)
       {ns = READ_MINISYMBOLS(et, FALSE, (void *) &st->symt, &sz);
	if (ns == 0)
	   ns = READ_MINISYMBOLS(et, TRUE, (void *) &st->symt, &sz);};

    return(ns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_FIND_ADDR - look for an address in a section
 *                   - this is called via bfd_map_over_sections
 */

static void _SC_exe_find_addr(bfd *et, asection *es, void *data)
   {off_t vma, adf, sz;
    asymbol **symt;
    exedes *st;
    SC_srcloc sl;

    st   = (exedes *) data;
    symt = GET_SYMT(st);

    if ((st->found == FALSE) &&
	((GET_SECTION_FLAGS(et, es) & SEC_ALLOC) != 0))
       {vma = GET_SECTION_VMA(et, es);
	adf = st->pc - vma;
	if (0 <= adf)
	   {sz = GET_SECTION_SIZE(es);
	    if (adf < sz)
	       {st->found = FIND_NEAREST_LINE(et, es, symt, adf,
					      &sl.pfile,
					      &sl.pfunc,
					      &sl.line);
		if (st->found == TRUE)
		   SAVE_LOC(st, sl);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_FIND_OFFS - look for an offset in a section */

static void _SC_exe_find_offs(exedes *st)
   {off_t ad, sz;
    bfd *et;
    asection *es;
    asymbol **symt;
    SC_srcloc sl;

    et   = GET_ET(st);
    es   = GET_ES(st);
    symt = GET_SYMT(st);

    if ((st->found == FALSE) &&
	((GET_SECTION_FLAGS(et, es) & SEC_ALLOC) != 0))
       {ad = st->pc;
	sz = GET_SECTION_SIZE(es);
	if (ad < sz)
	   {st->found = FIND_NEAREST_LINE(et, es, symt, ad,
					  &sl.pfile,
					  &sl.pfunc,
					  &sl.line);
	    if (st->found == TRUE)
	       SAVE_LOC(st, sl);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_FIND_INLINE - find the current location for inlines */

static void _SC_exe_find_inline(exedes *st)
   {bfd *et;
    SC_srcloc sl;

    et = GET_ET(st);

    st->found = FIND_INLINER_INFO(et,
				  &sl.pfile,
				  &sl.pfunc,
				  &sl.line);
    if (st->found == TRUE)
       SAVE_LOC(st, sl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_MAP_ADDR - map hex address AD to a SC_storloc PSL */

static void _SC_exe_map_addr(SC_storloc *psl, exedes *st, char *ad)
   {int na;
    char s[MAXLINE];
    char *p, *fnc, **ta;
    SC_storloc sl;
    SC_srcloc *loc;
    bfd *et;
    asection *es;
    static char *none = "unknown";

    et = GET_ET(st);
    es = GET_ES(st);

    loc = _SC_init_storloc(&sl);
    SC_strncpy(sl.file, MAXLINE, none, -1);

    SC_strncpy(s, MAXLINE, ad, -1);

/* address form: "file(tag+offs) [addr]" */
    ta = SC_tokenize(s, " \t[]()");
    SC_ptr_arr_len(na, ta);

    switch (na)
       {case 0 :
	     break;
        case 1 :
	     break;
        default :
	     ad = ta[1];
	     break;};

    memset(&st->where, 0, sizeof(SC_srcloc));

    st->pc = bfd_scan_vma(ad, NULL, 16);

    st->found = FALSE;
    if (es != NULL)
       _SC_exe_find_offs(st);
    else
       bfd_map_over_sections(et, _SC_exe_find_addr, st);

    if (st->found == TRUE)
       {do

/* get the function name */
	   {fnc = st->where.func;
	    if (*fnc == '\0')
	       SC_strncpy(sl.func, MAXLINE, none, -1);
	    else if (st->demang == TRUE)
	       _SC_exe_demangle_name(sl.func, MAXLINE, st);
	    else
	       SC_strncpy(sl.func, MAXLINE, fnc, -1);

/* get the file name */
	    if (st->where.file[0] != '\0')
	       {char *fn;

		fn = (char *) st->where.file;
		if ((st->tailf == TRUE) && (fn != NULL))
		   {p = strrchr(fn, '/');
		    if (p != NULL)
		       fn = p + 1;};
		SC_strncpy(sl.file, MAXLINE, fn, -1);};

/* get the line number */
	    loc->line = st->where.loc.line;

	    if (st->unwin == 0)
	       st->found = FALSE;
	    else
               _SC_exe_find_inline(st);}
	while (st->found == TRUE);}

/* if no address found do the best possible from the address */
    else
       {if (na == 1)
	   {SC_strncpy(sl.file, MAXLINE, none, -1);
	    loc->line = SC_strtol(ta[0], NULL, 16);}

	else if (na == 2)
	   {SC_strncpy(sl.file, MAXLINE, ta[0], -1);
	    loc->line = SC_strtol(ta[1], NULL, 16);}

	else if (na > 2)
	   {SC_strncpy(sl.file, MAXLINE, ta[0], -1);
	    SC_strncpy(sl.func, MAXLINE, ta[1], -1);
	    loc->line = 0;};};

    SC_free_strings(ta);

    _SC_copy_storloc(psl, &sl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_MAP_ADDRS - map NA hex addresses from AD
 *                   - into the form
 *                   -    [fnc ]file:line
 *                   - and print them
 */

static int _SC_exe_map_addrs(exedes *st, int na, char **ad)
   {int i, rv;
    char t[MAXLINE];
    SC_storloc sl;
    SC_srcloc *loc;

    rv = TRUE;

    loc = &sl.loc;

    for (i = 0; i < na; i++)
        {_SC_exe_map_addr(&sl, st, ad[i]);
	 _SC_format_loc(t, MAXLINE, loc, st->showf, st->tailf);
	 printf("%s\n", t);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_OPEN - open the named executable and return an exedes
 *              - describing its contents
 */

static exedes *_SC_exe_open(char *ename, char *sname, char *tgt,
			    int showf, int bnm, int unw, int dem)
   {int ok;
    bfd *et;
    asection *es;
    exedes *st;

    ONCE_SAFE(TRUE, NULL)
       bfd_init();
    END_SAFE;

    st = NULL;
    if (SC_file_length(ename) >= 1)
       {ok = TRUE;

	st = CMAKE(exedes);
	memset(st, 0, sizeof(exedes));

	et = bfd_openr(ename, tgt);
	if (et != NULL)
	   {if (bfd_check_format(et, bfd_archive) != 0)
	       {ok = FALSE;
	        snprintf(st->err, MAXLINE,
			 "%s: cannot get addresses from archive", ename);}
	    else
	       ok = _SC_exe_check_formats(et);};

	es = NULL;
	if (sname != NULL)
	   {es = bfd_get_section_by_name(et, sname);
	    if (es == NULL)
	       {ok = FALSE;
		snprintf(st->err, MAXLINE,
			 "%s: cannot find section %s", ename, sname);};};

	if (ok == TRUE)
	   {st->ename   = CSTRSAVE(ename);
	    st->sname   = CSTRSAVE(sname);
	    st->target  = CSTRSAVE(tgt);
	    st->et      = et;
	    st->es      = es;

#ifdef HAVE_BFD_DEMANGLE
	    st->demang = dem;
#else
	    st->demang = FALSE;
#endif
	    st->showf  = showf;
	    st->unwin  = unw;
	    st->tailf  = bnm;

	    _SC_exe_rd_symt(st);}
	else
	   CFREE(st);};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_CLOSE - close ST and free it */

static void _SC_exe_close(exedes *st)
   {bfd *et;
    asymbol **symt;

    if (st != NULL)
       {et   = GET_ET(st);
	symt = GET_SYMT(st);

        if (symt != NULL)
	   {free(symt);
	    st->symt = NULL;};

	bfd_close(et);

	CFREE(st);};

    return;}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* _SC_EXE_MAP_ADDR - map hex address AD to a SC_storloc PSL */

static void _SC_exe_map_addr(SC_storloc *psl, exedes *st, char *ad)
   {

    _SC_init_storloc(psl);

    snprintf(psl->func, MAXLINE, "@%s", ad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_MAP_ADDRS - map NA hex addresses from AD
 *                   - into the form
 *                   -    [fnc ]file:line
 *                   - and print them
 */

static int _SC_exe_map_addrs(exedes *st, int na, char **ad)
   {int rv;

    rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_OPEN - open the named executable and return an exedes
 *              - describing its contents
 */

static exedes *_SC_exe_open(char *ename, char *sname, char *tgt,
			    int showf, int bnm, int unw, int dem)
   {exedes *st;

    st = NULL;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_CLOSE - close ST and free it */

static void _SC_exe_close(exedes *st)
   {

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* SC_EXE_INIT_API - setup the API calls to access executables */

void SC_exe_init_api(void)
   {

#ifdef HAVE_BFD
    _BFD_set_api();
#endif

    _SC.exe.open      = _SC_exe_open;
    _SC.exe.close     = _SC_exe_close;
    _SC.exe.map_addr  = _SC_exe_map_addr;
    _SC.exe.map_addrs = _SC_exe_map_addrs;
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
