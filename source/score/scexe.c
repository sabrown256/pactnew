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

#ifdef HAVE_BFD

#include "scope_exe.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_DEMANGLE_STYLE - set the C++ name demangling style */

void _SC_set_demangle_style(char *opt)
   {

#if 0
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

# ifdef HAVE_BFD_DEMANGLE

    char *p, *nm;
    
    nm = (char *) st->where.func;
    p = bfd_demangle(st->et, nm, SC_DEMANGLE_ARG);
    if (p != NULL)
       {SC_strncpy(d, nc, p, -1);
	free(fnc);}

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

#if 1
    char **matching;

    if (bfd_check_format_matches(et, bfd_object, &matching) == 0)
       {ok = FALSE;
	if (bfd_get_error() == bfd_error_file_ambiguously_recognized)
	   {extern void list_matching_formats(char **p);
/*
            list_matching_formats(matching);
 */
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

    ns = -1;
    if ((bfd_get_file_flags(st->et) & HAS_SYMS) != 0)
       {ns = bfd_read_minisymbols(st->et, FALSE, (void *) &st->symt, &sz);
	if (ns == 0)
	   ns = bfd_read_minisymbols(st->et, TRUE, (void *) &st->symt, &sz);};

    return(ns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_FIND_ADDR - look for an address in a section
 *                   - this is called via bfd_map_over_sections
 */

static void _SC_exe_find_addr(bfd *et, asection *es, void *data)
   {bfd_vma vma;
    bfd_size_type sz;
    exedes *st;

    st = (exedes *) data;

    if ((st->found == FALSE) &&
	((bfd_get_section_flags(et, es) & SEC_ALLOC) != 0))
       {vma = bfd_get_section_vma(et, es);
	if (st->pc >= vma)
	   {sz = bfd_get_section_size(es);
	    if (st->pc < vma + sz)
	       {csrcloc sl;

	        st->found = bfd_find_nearest_line(et, es,
						  st->symt, st->pc - vma,
						  &sl.file,
						  &sl.func,
						  &sl.line);
		if (st->found == TRUE)
		   {SC_strncpy(st->where.file, MAXLINE, (char *) sl.file, -1);
		    SC_strncpy(st->where.func, MAXLINE, (char *) sl.func, -1);
/* GOTCHA: coming up with line number 0 always
printf("-> fz |%s| |%s| |%d|\n", sl.func, sl.file, sl.line);
*/
		    st->where.line = sl.line;};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_FIND_OFFS - look for an offset in a section */

static void _SC_exe_find_offs(exedes *st)
   {bfd_size_type sz;

    if ((st->found == FALSE) &&
	((bfd_get_section_flags(st->et, st->es) & SEC_ALLOC) != 0))
       {sz = bfd_get_section_size(st->es);
	if (st->pc < sz)
	   {csrcloc sl;

	    st->found = bfd_find_nearest_line(st->et, st->es,
					      st->symt, st->pc,
					      &sl.file,
					      &sl.func,
					      &sl.line);
	    if (st->found == TRUE)
	       {SC_strncpy(st->where.file, MAXLINE, (char *) sl.file, -1);
		SC_strncpy(st->where.func, MAXLINE, (char *) sl.func, -1);
		st->where.line = sl.line;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXE_MAP_ADDR - map hex address AD to a srcloc PSL */

static void _SC_exe_map_addr(srcloc *psl, exedes *st, char *ad)
   {char *fnc;
    srcloc sl;
    static char *none = "unknown";

    memset(&st->where, 0, sizeof(csrcloc));

    sl.func[0] = '\0';
    SC_strncpy(sl.file, MAXLINE, none, -1);
    sl.line = -1;

    st->pc = bfd_scan_vma(ad, NULL, 16);

    st->found = FALSE;
    if (st->es != NULL)
       _SC_exe_find_offs(st);
    else
       bfd_map_over_sections(st->et, _SC_exe_find_addr, st);

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
	       SC_strncpy(sl.file, MAXLINE, (char *) st->where.file, -1);

/* get the line number */
	    sl.line = st->where.line;

	    if (st->unwin == 0)
	       st->found = FALSE;
	    else
	       {csrcloc sl;

		st->found = bfd_find_inliner_info(st->et,
						  &sl.file,
						  &sl.func,
						  &sl.line);
		if (st->found == TRUE)
		   {SC_strncpy(st->where.file, MAXLINE, (char *) sl.file, -1);
		    SC_strncpy(st->where.func, MAXLINE, (char *) sl.func, -1);
		    st->where.line = sl.line;};};}
	while (st->found == TRUE);};

    *psl = sl;

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
    char *fn, *p;
    srcloc sl;

    rv = TRUE;

    for (i = 0; i < na; i++)
        {_SC_exe_map_addr(&sl, st, ad[i]);

/* get the file name into the requested form */
	 fn = sl.file;
	 if ((st->tailf == TRUE) && (fn != NULL))
	       {p = strrchr(fn, '/');
		if (p != NULL)
		   fn = p + 1;};

	 if (st->showf == TRUE)
	    printf("%s %s:%d\n", sl.func, fn, sl.line);
	 else
	    printf(" %s:%d\n", fn, sl.line);};

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
			 "%s: cannot get addresses from archive", ename);};

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
   {

    if (st != NULL)
       {if (st->symt != NULL)
	   {free(st->symt);
	    st->symt = NULL;};

	bfd_close(st->et);

	CFREE(st);};

    return;}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* _SC_EXE_MAP_ADDR - map hex address AD to a srcloc PSL */

static void _SC_exe_map_addr(srcloc *psl, exedes *st, char *ad)
   {srcloc sl;

    snprintf(sl.func, MAXLINE, "@%s", ad);
    sl.file[0] = '\0';
    sl.line    = -1;

    *psl = sl;

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

    _SC.exe.open      = _SC_exe_open;
    _SC.exe.close     = _SC_exe_close;
    _SC.exe.map_addr  = _SC_exe_map_addr;
    _SC.exe.map_addrs = _SC_exe_map_addrs;
    
    return;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
