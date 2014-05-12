/*
 * SCCTL.C - some core routines used by many packages
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 
#include "scope_mem.h" 

#ifndef ATOF_FUNCTION
# define ATOF_FUNCTION atof
#endif

#ifndef ATOL_FUNCTION
# define ATOL_FUNCTION atol
#endif

#ifndef STRTOD_FUNCTION
# define STRTOD_FUNCTION strtod
#endif

#ifndef STRTOL_FUNCTION
# define STRTOL_FUNCTION strtol
#endif

#ifndef STRTOLL_FUNCTION
# define STRTOLL_FUNCTION strtoll
#endif

#ifndef NGROUPS_MAX
# define NGROUPS_MAX 16
#endif

int
 *LAST = NULL;

SC_scope_public
 SC_gs = {PACT_VERSION,
	  0, 0, 0, 1, FALSE, 10, TRUE, FALSE,
	  0, NULL, NULL, NULL,
          _SC_DEF_TYP_, _SC_DEF_ITYP_,
          SC_sizeof,
          _SC_otol,
          _SC_htol,
          ATOL_FUNCTION,
          STRTOL_FUNCTION,
          ATOF_FUNCTION,
          STRTOD_FUNCTION,
	  SC_type_container,
#ifdef USE_FULL_MM
          {_SC_nalloc_wl, _SC_alloc_wl, _SC_realloc_wl,
	   _SC_free_wl, _SC_arrlen_wl,
	   _SC_alloc_nl, _SC_realloc_nl, _SC_free_nl},
#else
          {_SC_nalloc_ws, _SC_alloc_ws, _SC_realloc_ws,
	   _SC_free_ws, _SC_arrlen_ws,
	   _SC_alloc_ns, _SC_realloc_ns, _SC_free_ns},
#endif
	  };

SC_scope_private
 _SC = {-1, TRUE, -1,
        malloc, realloc, free,
	{-1.0, -1.0}, -1, -1, -1, SC_TERM_UNKNOWN,
	(PFSignal_handler) -1, {SIG_DFL, 0, NULL}, -1,
        _SC_TYPE_STATE_INIT_};

/*--------------------------------------------------------------------------*/

/*                      DOCUMENTATION FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* SC_BANNER - display a banner/header using the input string */

void SC_banner(char *s)
   {

    io_printf(STDOUT, "\n     %s     %s", s, _SC.banner);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_BANNER - set the contents of a banner/header */

void SC_set_banner(char *fmt, ...)
   {

    SC_VA_START(fmt);
    SC_VSNPRINTF(_SC.banner, MAXLINE, fmt);
    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_BANNER - set the contents of a banner/header */

char *SC_get_banner(void)
   {

    return(_SC.banner);}

/*--------------------------------------------------------------------------*/

/*                            AUXILLIARY FUNCTIONS                          */

/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_PATH_DOT - auxilliary helper for SC_full_path
 *                     - given a directory DIR and path NAME
 *                     - resolve out ./ and ../ elements of NAME
 *                     - return result in PATH
 *                     - DIR may be destroyed in this process
 */

static void _SC_handle_path_dot(char *path, char *dir, char *name)
   {char *s;

    if (dir != NULL)
       {s = name;

/* loop over any number of ./ or ../ elements of the path */
	while (s[0] == '.')
	   {if (s[1] == '.')
	       {s += 3;
		SC_lasttok(dir, "/");}
	    else
	       s += 2;};

/* construct the candidate path */
	sprintf(path, "%s/%s", dir, s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GETCWD - return a dynamically allocated string containing the
 *           - current working directory
 *           - this is intended to be safe from buffer overflow problems
 */

char *SC_getcwd(void)
   {long sz;
    char *vr, *wd;

    wd = NULL;

#if defined(MACOSX) || defined(FREEBSD) || defined(MSW)
    sz = PATH_MAX;
#else
    sz = pathconf(".", _PC_PATH_MAX);
#endif

    vr = CMAKE_N(char, sz);
    if (vr != NULL)
       wd = getcwd(vr, (size_t) sz);

    return(wd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FILE_PATH - given the name of a file return the full path
 *              - if FULL is TRUE
 *              - otherwise expand out '.' and '..' constructions
 *              - input is the NAME, output is the PATH which is
 *              - at most NC chars long
 *              - return:
 *              -    0 on success
 *              -    n NAME is there but NC is too few characters
 *              -      n is the number of characters needed to contain
 *              -      the full path
 */

int SC_file_path(char *name, char *path, int nc, int full)
   {int n;
    char fp[PATH_MAX];
    char *t, *p;

    switch (name[0])
       {case '/' :
	     n = strlen(name);
	     p = name;
             break;

	default :
	     n = strlen(name);
	     p = name;
             if (full == FALSE)
	        break;

        case '.' :
	     t = SC_getcwd();
             _SC_handle_path_dot(fp, t, name);
	     CFREE(t);

             n = strlen(fp);
	     p = fp;
             break;};

    if (n <= nc)
       {strcpy(path, p);
	n = 0;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FULL_PATH - given the name of a program return the full path
 *              - of the program
 *              - this is essentially the shell 'which' command for
 *              - which there is incredibly enough no standard library
 *              - function
 *              - input is the NAME, output is the PATH which is
 *              - at most NC chars long
 *              - return:
 *              -    0 on success
 *              -   -1 NAME is not on your path
 *              -    n NAME is on your path but NC is too few characters
 *              -      n is the number of characters needed to contain
 *              -      the full path
 */

int SC_full_path(char *name, char *path, int nc)
   {int n;

    n = PS_full_path(path, nc, TRUE, NULL, name);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_POP_PATH - pop the tail of the path off
 *             - return a pointer to the head component
 */

char *SC_pop_path(char *path)
   {char *p;

    p = strrchr(path, '/');
    if (p != NULL)
       *p++ = '\0';
    else
       p = path;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ADD_SEARCH_PATH - add DIR to the search path */

void SC_add_search_path(char *fmt, ...)
   {int is, ns, ne;
    char d[BFLRG], delim[10];
    char **sa;

    if (d != NULL)
       {if (_SC.path == NULL)
	   {_SC.path = CMAKE_ARRAY(char *, NULL, 0);
	    SC_array_string_add_copy(_SC.path, ".");};

	SC_PATH_DELIMITER(delim);

	SC_VA_START(fmt);
	SC_VSNPRINTF(d, BFLRG, fmt);
	SC_VA_END;

	ne = SC_array_get_n(_SC.path);
	if (ne > 1)
	   SC_array_pop(_SC.path);

	sa = PS_tokenize(d, delim, 0);
	ns = PS_lst_length(sa);
	for (is = 0; is < ns; is++)
	    SC_array_string_add_copy(_SC.path, sa[is]);

	PS_free_strings(sa);

	SC_array_string_add(_SC.path, NULL);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_PATH - set up the search path */

void SC_init_path(int nd, ...)
   {int j, ne;
    char delim[10];
    char *ptr, *var;

    if (_SC.path == NULL)
       {_SC.path = CMAKE_ARRAY(char *, NULL, 0);
	SC_array_string_add_copy(_SC.path, ".");};

    SC_PATH_DELIMITER(delim);

    SC_VA_START(nd);

    ne = SC_array_get_n(_SC.path);
    if (ne > 1)
       SC_array_pop(_SC.path);

    for (j = 0; j < nd; j++)
        {var = SC_VA_ARG(char *);
	 ptr = getenv(var);
         if (ptr != NULL)
	    SC_add_search_path(ptr);};

    SC_VA_END;

    SC_array_string_add(_SC.path, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_SEARCH_PATH - set up a search path array */

SC_array *SC_make_search_path(int nd, ...)
   {int j;
    char *bf, delim[10];
    char *ptr, *t, *token;
    SC_array *path;

    path = SC_STRING_ARRAY();

    SC_PATH_DELIMITER(delim);

    SC_VA_START(nd);

    for (j = 0; j < nd; j++)
        {ptr = getenv(SC_VA_ARG(char *));
         if (ptr != NULL)
            {bf  = CSTRSAVE(ptr);
             ptr = bf;

             while ((token = SC_strtok(ptr, delim, t)) != NULL)
                {SC_array_string_add_copy(path, token);
                 ptr = NULL;};

             CFREE(bf);};};

    SC_VA_END;

    SC_array_string_add(path, NULL);

    return(path);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_SEARCH_PATH - release a search path array */

void SC_free_search_path(SC_array *path)
   {

    SC_free_array(path, SC_array_free_n);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INTERRUPT_HANDLER - handle interrupts in a default sort of way */

void SC_interrupt_handler(int sig)
   {char bf[10];

    io_printf(stdout, "\nInterrupt - Quit (q) or Resume (r): ");
    GETLN(bf, 10, stdin);
    switch (bf[0])
       {case 'r' :
	     io_printf(stdout, "Resuming\n\n");
	     break;
        case 'q' :
	     io_printf(stdout, "Exiting program\n\n");
	     exit(SC_EXIT_SELF);};

    SC_signal_n(SIGINT, SC_interrupt_handler, NULL, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGX_MATCH - returns TRUE iff the first string arg matched the
 *               - regular expression defined by the second string arg
 *               -
 *               - regular expression specifiers (so far)
 *               -
 *               -   '*' matches any number of characters
 *               -   '?' matches any single character
 */

int SC_regx_match(char *s, char *patt)
   {int rv;

#if 0

    rv = fnmatch(patt, s, 0);

#else

    int c;
    char *ps, *pp;

    if (patt == NULL)
       rv = TRUE;
    else if (s == NULL)
       rv = FALSE;
    else
       {ps = s;
	pp = patt;
	while ((c = *pp++) != '\0')
	   {switch (c)
	       {case '*' :
		     while (*pp == '*')
		        pp++;
		     c = *pp;
		     while ((ps = strchr(ps, c)) != NULL)
		        {if (SC_regx_match(ps, pp))
			    return(TRUE);
			 ps++;};
		     return(FALSE);

		case '?' :
		     rv = SC_regx_match(++ps, pp);
		     return(rv);

		case '\\' :
		     c = *pp++;

		default :
		     if (*ps++ != c)
		        return(FALSE);};

	    if (((*ps == '\0') && (*pp != '\0') && (*pp != '*')) ||
		((*ps != '\0') && (*pp == '\0')))
	       return(FALSE);};

	rv = (*ps == '\0');};
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BIT_REVERSE - return an unsigned int whose bit pattern is reversed
 *                - relative to N bits of the input value, I
 */

unsigned int SC_bit_reverse(unsigned int i, int n)
   {unsigned j;
    int k;

    j = 0;
    for (k = 0; k < n; k++)
        {j <<= 1;
         j |= (i & 1);
         i >>= 1;};

    return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BIT_COUNT - count the number of set bits in a n byte quantity
 *              - the number of bytes must be less than or equal
 *              - to sizeof(long)
 */

long SC_bit_count(long c, int n)
   {long count;

/* best for sparse bit patterns */
    if (c < 65536L)
       {count = 0L;
        while (c)
           {c ^= (c & -c);
            count++;};}

/* best for dense bit patterns */
    else
       {count = SC_BITS_BYTE*n;
        while (c != -1L)
           {c |= (c + 1);
            count--;};};

    return(count);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RANDOM_INT - pick a random int between A and B */

int SC_random_int(int a, int b)
   {int n;
    unsigned int ni;
    double da, db, tu, t, x;

    ONCE_SAFE(TRUE, NULL)

/* seed the random number sequence with the microsecond part of the time
 * the C version of do-net shows why the seconds are insufficient
 */
       SC_get_time(NULL, &tu);
       ni = tu;
       srandom(ni);

    END_SAFE;

    t  = ((double) random())/((double) INT_MAX);
    da = a;
    db = b;

    x = (da - db)*t + db;
    n = (int) x;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FORM_FILE_AUX - help with SC_FORM_FILE
 *                   - return a path specification consisting of the
 *                   - directory and string s
 *                   - any UNIX style directory separators or relations
 *                   - are removed in favor of the host system's designations
 */

static char *_SC_form_file_aux(char *directory, char *s)
   {char *bp, *bf;

    if (directory == NULL)
       bf = SC_dsnprintf(TRUE, "%s", s);

    else if (directory[0] == '\0')
       bf = SC_dsnprintf(TRUE, "%s", s);

    else
       bf = SC_dsnprintf(TRUE, "%s%c%s", directory, directory_delim_c, s);

/* replace directory delimiters by the system directory delimiter */
    for (bp = bf; *bp != '\0'; bp++)
        {if ((*bp == directory_delim_c) || (*bp == '/'))
            *bp = directory_delim_c;};

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SEARCH_FILE - search for a file in the directories in path of the
 *                 - specified type that can be accessed in the given mode.
 *                 - if found, return the pathname; otherwise, return NULL.
 *                 - if path is NULL, search in the current directory only.
 */

char *_SC_search_file(char **path, char *name, char *mode, char *type)
   {int i;
    char *rv, *bp, *t, *lst[2];

    rv = NULL;

    if (name != NULL)

/* if this is a remote file just go find it */
       {if (strchr(name, ':') != NULL)
	   rv = name;

	else if (name[0] == '/')
	   {bp = _SC_form_file_aux("", name);
	    if (SC_query_file(bp, mode, type))
	       rv = bp;
	    else
	       CFREE(bp);}

	else if (name[0] == '~')
	   {char pth[MAXLINE];
	    char *cmnd, **output;

	    output = NULL;

	    pth[0] = '\0';

	    cmnd   = SC_dsnprintf(TRUE, "echo %s", name);
	    output = SC_syscmnd(cmnd);
	    CFREE(cmnd);

	    if ((output != NULL) &&
		(output[0] != NULL))
	       {strcpy(pth, output[0]);
		CFREE(output[0]);
		CFREE(output);}

	    if (SC_query_file(pth, mode, type))
	       rv = CSTRSAVE(pth);}

	else
	   {if (path == NULL)
	       {lst[0] = ".";
		lst[1] = NULL;
		path   = lst;};

	    for (i = 0; TRUE; i++)
	        {t = path[i];
		 if (t == NULL)
		    break;

		 bp = _SC_form_file_aux(t, name);

		 if (SC_query_file(bp, mode, type))
		    {rv = bp;
		     break;};

		 CFREE(bp);};};};

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_SEARCH_PATH - return a pointer to the path array
 *                    - the caller must CFREE it
 */

char **SC_get_search_path(void)
   {char **path;

    path = SC_array_array(_SC.path);

    return(path);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SEARCH_FILE - search for a file in the directories in PATH
 *                - if found, return the pathname; otherwise, return NULL
 *                - if PATH is NULL use internal path array
 */

char *SC_search_file(char **path, char *name)
   {int ok;
    char *s;

    ok = (path == NULL);
    if (ok == TRUE)
       path = SC_get_search_path();

    s = _SC_search_file(path, name, NULL, NULL);

    if (ok == TRUE)
       CFREE(path);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ADVANCE_NAME - increment a sequential name by one
 *                 - assumes name is of the form:
 *                 -    <base>.[a-zA-Z]dd
 *                 -    d = [0-9a-zA-Z]
 *
 * #bind SC_advance_name fortran() scheme() python()
 */

void SC_advance_name(char *s)
   {int d1, d2, n;
    char *p;

    p = strchr(s, '.') + 2;
    if (p != NULL)
       {n = STRTOL(p, NULL, 36) + 1;        

	d1 = n / 36;
	d2 = n % 36;

	if (d1 > 9)
	   d1 += 'a' - 10;
	else
	   d1 += '0';

	if (d2 > 9)
	   d2 += 'a' - 10;
	else
	   d2 += '0';

	p[0] = d1;
	p[1] = d2;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ITOA - return an ASCII representation of the given integer in
 *         - the given radix and pad with zeros to the given field width
 */

char *SC_itoa(int n, int radix, int nc)
   {int i, r;

    for (i = 0; n != 0; i++, n /= radix)
        {r = n % radix;

         if (r > 9)
            r += 'a' - 10;
         else
            r += '0';

         _SC.ibf[i] = r;};

/* pad with zeros out to field width of nc */
    for (; i < nc; i++)
        _SC.ibf[i] = '0';

    _SC.ibf[i] = '\0';
    SC_strrev(_SC.ibf);

    return(_SC.ibf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_QUERY_FILE - return TRUE if the file is of the given type and can be
 *               - opened in the specified mode; otherwise, return FALSE.
 */

int SC_query_file(char *name, char *mode, char *type)
   {int i, c, nr, ret;
    int64_t nn, np;
    char bf[MAX_BFSZ+1];
    double fr;
    FILE *fp;

    ret = SC_QUERY_MODE(name, mode);
    if (ret == TRUE)
       {if (type != NULL)
           {memset(bf, 0, MAX_BFSZ+1);

/* type may be const char * and will die when SC_str_lower
 * attempts to change values
 */
            strcpy(bf, type);
            SC_str_lower(bf);
            if (strcmp(bf, "ascii") == 0)
               {fp = io_open(name, "r");

/* count the number of printable and non-printable characters
 * in the first MAX_BFSZ bytes of the file
 */
		nn = 0;
		np = 0;
		nr = io_read(bf, 1, MAX_BFSZ, fp);
		for (i = 0; i < nr; i++)
		    {c = bf[i];
		     if (SC_is_print_char(c, 0))
			np++;
		     else
		        nn++;};

                io_close(fp);

/* consider the file text if the number of non-printable characters is
 * less than 5% of the printable characters
 */
		fr  = ((double) nn)/((double) np + SMALL);
                ret = (fr < 0.05);}
            else
               ret = FALSE;};};

    return(ret);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISFILE - return TRUE if the file exists, otherwise return FALSE */

int SC_isfile(char *name)
   {int rv;

    rv = SC_query_file(name, NULL, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISFILE_TEXT - return TRUE if the file exists and is ascii,
 *                - otherwise return FALSE
 */

int SC_isfile_text(char *name)
   {int rv;

    rv = SC_query_file(name, NULL, "ascii");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REMOVE - ANSI standard remove function
 *           - wrapped to overcome file system delays
 */

int SC_remove(char *s)
   {int i, ev, rv, na;
    struct stat sb;

/* maximum number of attempts */
    na = 100;

    for (i = 0, rv = -2; (i < na) && (rv != 0); i++)

/* try to unlink the file */
        {rv = unlink(s);
	 ev = errno;
	 switch (ev)

/* worth a retry */
	    {case EBUSY :
	     case EIO :
	          break;

/* path or permissions problems will never work so bail */
	     default :
                  i = na;
                  continue;};

/* check the for the continued existence of the file */
	 rv = stat(s, &sb);
	 ev = errno;
         if (rv == 0)
	    {rv = -1;
             i  = na;
	     sched_yield();}
	 else
	    rv = 0;};

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PAUSE - give the user a chance to think about things
 *
 * #bind SC_pause fortran() python()
 */

void SC_pause(void)
   {char s[10];

    GETLN(s, 9, stdin);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SAVE_ARGV_ENV - argc, argv, and env from main call */

void SC_save_argv_env(int argc, char **argv, char **env)
   {

    SC_gs.argc = argc;
    SC_gs.argv = argv;
    SC_gs.env  = env;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FORMAT_LOC - format the LOC info in D */

char *_SC_format_loc(char *d, int nc, SC_srcloc *loc, int sf, int tail)
   {int filep;
    char t[MAXLINE];
    char *p, *fnm;

    filep = ((loc->pfile != NULL) && (loc->pfile[0] != '\0'));

    t[0] = '\0';
    if (sf == TRUE)
       {if (filep == TRUE)
	   {fnm = (char *) loc->pfile;
	    if (tail == TRUE)
	       {p = strrchr(fnm, '/');
		if (p != NULL)
		   fnm = p + 1;};
	    SC_strncpy(t, MAXLINE, fnm, -1);};};

    if (filep == TRUE)
       snprintf(d, nc, "%s(%s:%d)", loc->pfunc, t, loc->line);

    else if (loc->pfunc != NULL)
       {if (loc->line != (unsigned int) -1)
	   snprintf(d, nc, "%s (%d)", loc->pfunc, loc->line);
	else
	   SC_strncpy(d, nc, (char *) loc->pfunc, -1);}

    else
       SC_strncpy(d, nc, "-none-", -1);

    return(d);}

/*--------------------------------------------------------------------------*/

/*                     ASSOCIATION LIST PRIMITIVES                          */

/*--------------------------------------------------------------------------*/

/* SC_ASSOC_ENTRY - return the pcons pointer from the item in the association
 *                - list associated with the given string
 */

pcons *SC_assoc_entry(pcons *alist, char *s)
   {char *t;
    pcons *pa, *c;

    for (pa = alist; pa != NULL; pa = (pcons *) pa->cdr)
        {c = (pcons *) pa->car;
         t = (char *) c->car;
         if (strcmp(t, s) == 0)
            return(c);};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ASSOC - return the value pointer from the item in the association
 *          - list associated with the given string
 */

void *SC_assoc(pcons *alist, char *s)
   {pcons *c;
    void *rv;

    c = SC_assoc_entry(alist, s);

    rv = (c != NULL) ? c->cdr : NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ADD_ALIST - add an item to the given association list */

pcons *SC_add_alist(pcons *alist, char *name, char *type, void *val)
   {pcons *rv;

    rv = SC_mk_pcons(SC_PCONS_P_S,
		     SC_mk_pcons(SC_STRING_S, CSTRSAVE(name), type, val),
		     SC_PCONS_P_S, alist);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REM_ALIST - remove an item from the given association list */

pcons *SC_rem_alist(pcons *alist, char *name)
   {pcons *ths, *nxt;
    char *s;

    if (alist != NULL)
       {s = (char *) ((pcons *) alist->car)->car;
	if (strcmp(s, name) == 0)
	   {SC_rl_pcons(alist->car, 3);
	    nxt = (pcons *) alist->cdr;

	    SC_rl_pcons(alist, 0);
	    alist = nxt;
	    SC_mark(alist, -1);}

	else
	   {for (ths = alist; ths->cdr != NULL; ths = nxt)
	        {nxt = (pcons *) ths->cdr;
		 s   = (char *) ((pcons *) nxt->car)->car;            
		 if (strcmp(s, name) == 0)
		    {SC_rl_pcons(nxt->car, 3);
		     ths->cdr = nxt->cdr;

		     SC_rl_pcons(nxt, 0);
		     nxt = (pcons *) ths->cdr;

		     if (nxt == NULL)
		        break;};};};};

    return(alist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHANGE_ALIST - change an item from the given association list
 *                 - add the item if necessary
 */

pcons *SC_change_alist(pcons *alist, char *name, char *type, void *val)
   {pcons *pc, *rv;

    pc = SC_assoc_entry(alist, name);
    if (pc == NULL)
       rv = SC_add_alist(alist, name, type, val);

    else
       {CFREE(pc->cdr_type);
        pc->cdr_type = CSTRSAVE(type);

        CFREE(pc->cdr);
        pc->cdr = val;
        SC_mark(val, 1);

        rv = alist;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALIST_MAP - map FNC over all element of an association list
 *              - FNC may return values through ARG
 *              - returns the list when done 
 */

pcons *SC_alist_map(pcons *alist, int (*fnc)(pcons *hp, void *arg), void *arg)
   {int st;
    pcons *ths;

    for (ths = alist; ths != NULL; ths = ths->cdr)
        {st = (*fnc)(ths, arg);
	 SC_ASSERT(st == TRUE);};

    return(alist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_PCONS - make a complete pcons and bind the car to the arg */

pcons *SC_make_pcons(char *cat, int ma, void *ca,
		     char *cdt, int md, void *cd)
   {pcons *cp;

    cp = CMAKE(pcons);
    cp->car = ca;
    cp->cdr = cd;

    if (ma && (ca != NULL))
       SC_mark(ca, 1);

    if (md && (cd != NULL))
       SC_mark(cd, 1);

    if (cat != NULL)
       cp->car_type = CSTRSAVE(cat);

    if (cdt != NULL)
       cp->cdr_type = CSTRSAVE(cdt);

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MK_PCONS - make a simple pcons and bind the car to the arg */

pcons *SC_mk_pcons(char *cat, void *ca, char *cdt, void *cd)
   {pcons *cp;

    cp = SC_make_pcons(cat, TRUE, ca, cdt, TRUE, cd);

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RL_PCONS - release a pcons
 *             - LEVEL specifies:
 *             -   bit 1 on - release car
 *             -   bit 2 on - release cdr
 */

void SC_rl_pcons(pcons *cp, int level)
   {

    if (cp != NULL)
       {if (level & 1)
	   {CFREE(cp->car);};
	cp->car = NULL;

	if (level & 2)
	   {CFREE(cp->cdr);};
	cp->cdr = NULL;

	CFREE(cp->car_type);
	CFREE(cp->cdr_type);

	CFREE(cp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ASSOC_AUX - extract values for the keys in the supplied
 *               - key, value pairs from an association list
 *               - depending on the flag NULL out missing items
 */

static int _SC_assoc_aux(pcons *alst, int flag, va_list SC_VA_VAR)
   {int count;
    char *name;
    pcons *asc, *nxt, *ths;
    void **pv;

    count = 0;
    ths   = NULL;

    while (TRUE)
       {name = SC_VA_ARG(char *);
        if (name == NULL)
           break;

        for (asc = alst; asc != NULL; asc = nxt)
            {nxt = (pcons *) asc->cdr;
             ths = (pcons *) asc->car;
             if (strcmp((char *) ths->car, name) == 0)
                break;};

        pv  = SC_VA_ARG(void **);
        if (asc != NULL)
           *pv = ths->cdr;
           
        else if (flag)
           *pv = NULL;};

    return(count);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ASSOC_INFO - extract values for the keys in the supplied
 *               - key, value pairs from an association list
 */

int SC_assoc_info(pcons *alst, ...)
   {int count;

    SC_VA_START(alst);
    count = _SC_assoc_aux(alst, TRUE, SC_VA_VAR);
    SC_VA_END;

    return(count);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ASSOC_INFO_ALT - extract values for the keys in the supplied
 *                   - key, value pairs from an association list
 *                   - don't change the value pointer if there is no key
 *                   - match
 */

int SC_assoc_info_alt(pcons *alst, ...)
   {int count;

    SC_VA_START(alst);
    count = _SC_assoc_aux(alst, FALSE, SC_VA_VAR);
    SC_VA_END;

    return(count);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_ALIST - release an entire association list */

void SC_free_alist(pcons *alst, int level)
   {pcons *pc, *nxt;

    for (pc = alst; pc != NULL; pc = nxt)
        {SC_rl_pcons(pc->car, level);
	 nxt = (pcons *) pc->cdr;
	 SC_rl_pcons(pc, 0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_COPY_ALIST - make a copy of an association list */

pcons *SC_copy_alist(pcons *alst)
   {pcons *pc, *pa, *pb;

    pc = NULL;
    for (pa = alst; pa != NULL; pa = (pcons *) pa->cdr)
        {pb = (pcons *) pa->car;
	 pc = SC_add_alist(pc, (char *) pb->car,
			   (char *) pb->cdr_type, pb->cdr);};

    return(pc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_APPEND_ALIST - take each item on alist2 and add to alist1
 *                 - NOTE: this puts the elements in alist2 on the front of
 *                 - alist1 in reverse order and destroys alist2!
 */

pcons *SC_append_alist(pcons *alist1, pcons *alist2)
   {pcons *pa, *c, *nxt;

    for (pa = alist2; pa != NULL; pa = nxt)
        {c   = (pcons *) pa->car;
         nxt = (pcons *) pa->cdr;

         alist1 = SC_mk_pcons(SC_PCONS_P_S, c,
			      SC_PCONS_P_S, alist1);

         SC_rl_pcons(pa, 1);
	 SC_mark(alist1->cdr, -1);};

    return(alist1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DAPRINT - print an alist for debug purposes */

void daprint(pcons *alst)
   {int id;
    char s[MAXLINE];
    pcons *pc, *ths;

    io_printf(stdout, "Name\t\t\t    Type\t  Address\t\tValue\n");

    for (pc = alst; pc != NULL; pc = (pcons *) pc->cdr)
        {ths = (pcons *) pc->car;

	 snprintf(s, MAXLINE, "%s", (char *) ths->car);
	 memset(s+strlen(s), ' ', 20);
	 s[20] = '\0';

         io_printf(stdout, "%s (%d)   %s\t0x%lx (%d)",
		   s, SC_ref_count(ths->car),
		   ths->cdr_type, ths->cdr, SC_ref_count(ths->cdr));

	 id = SC_type_id(ths->cdr_type, FALSE);

         if (id == SC_CHAR_I)
            io_printf(stdout, "\t%s", ((char *) ths->cdr));

         else if (id == SC_STRING_I)
            io_printf(stdout, "\t%s", ((char **) ths->cdr)[0]);

/* print the first value only */
	 else
	    {id = SC_deref_id(ths->cdr_type, FALSE);
	     SC_ntos(s, MAXLINE, id, ths->cdr, 0, 1);
	     io_printf(stdout, "\t%s", s);};

         io_printf(stdout, "\n");}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
