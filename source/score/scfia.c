/*
 * SCFIA.C - FORTRAN interface routines for SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 

#ifdef HAVE_POSIX_SYS

# define DEFAULT_SCANNER F77_FUNC(f77lxr, F77LXR)

#else

# define DEFAULT_SCANNER NULL

#endif

SC_thread_lock
 SC_ptr_lock    = SC_LOCK_INIT_STATE,
 SC_stream_lock = SC_LOCK_INIT_STATE;

extern int
 F77_FUNC(f77lxr, F77LXR)(void);

/*--------------------------------------------------------------------------*/

/*                        POINTER MANAGEMENT ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* SC_STASH_POINTER - add a pointer to the managed array and return
 *                  - its index
 */

int64_t SC_stash_pointer(void *p)
   {int64_t i;

    i = -1;

#ifdef F77_INT_SIZE_PTR_DIFFER
    int np;
    void *pl;

    SC_LOCKON(SC_ptr_lock);

    if (_SC.ptr_lst == NULL)
       _SC.ptr_lst = SC_MAKE_ARRAY("PERM|SC_STASH_POINTER", void *, NULL);

    np = SC_array_get_n(_SC.ptr_lst);

    for (; _SC.ip < np; _SC.ip++)
        {pl = *(void **) SC_array_get(_SC.ptr_lst, _SC.ip);
         if (pl == NULL)
            {SC_array_set(_SC.ptr_lst, _SC.ip, &p);
             i = ++_SC.ip;
             break;};}

    if (i == -1)
       {SC_array_push(_SC.ptr_lst, &p);
        i = ++_SC.ip;}

    SC_LOCKOFF(SC_ptr_lock);

#else

    i = _SC_to_number(p);

#endif

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_POINTER - return a pointer from the managed array */

void *SC_get_pointer(int64_t n)
   {void *p;


#ifdef F77_INT_SIZE_PTR_DIFFER
    int np;

    SC_LOCKON(SC_ptr_lock);

    p = NULL;

    np = SC_array_get_n(_SC.ptr_lst);

    if ((1 <= n) && (n <= np))
       {n--;
	p = *(void **) SC_array_get(_SC.ptr_lst, n);};

    SC_LOCKOFF(SC_ptr_lock);

#else

    p = _SC_to_address(n);

#endif

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_POINTER_INDEX - return the index of the stored pointer
 *
 */

int64_t SC_pointer_index(void *p)
   {int64_t i;

#ifdef F77_INT_SIZE_PTR_DIFFER
    int np;
    void **pl;

    SC_LOCKON(SC_ptr_lock);

    np = SC_array_get_n(_SC.ptr_lst);
    for (i = 0; i < np; i++)
        {pl = *(void **) SC_array_get(_SC.ptr_lst, i);
	 if (pl == p)
             break;}

    if (i >= np)
       i = -1;
    else
       i++;

    SC_LOCKOFF(SC_ptr_lock);

#else

    i = _SC_to_number(p);

#endif

    return(i);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DEL_POINTER - remove a pointer from the managed array
 *                - return the pointer so that somebody can free it maybe
 */

void *SC_del_pointer(int n)
   {void *p;
    
#ifdef F77_INT_SIZE_PTR_DIFFER
    int np;

    p = NULL;

    np = SC_array_get_n(_SC.ptr_lst);

    if ((1 <= n) && (n <= np))
       {n--;

	SC_LOCKON(SC_ptr_lock);

	p = *(void **) SC_array_get(_SC.ptr_lst, n);

/* NULL out this slot */
	SC_array_set(_SC.ptr_lst, n, NULL);

/* reset the first avaiable index */
	_SC.ip = min(_SC.ip, n);

	SC_LOCKOFF(SC_ptr_lock);};

#else

    p = _SC_to_address(n);

#endif

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_STASH - free the array of stashed pointers */

int SC_free_stash(void)
   {

    SC_free_array(_SC.ptr_lst, NULL);

    _SC.ptr_lst = NULL;
    _SC.ip      = 0;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                            FORTRAN ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* SCVERS - return the PACT version in NAME
 *        - and the number of characters in the version string
 *        - as the return value
 */

FIXNUM F77_FUNC(scvers, SCVERS)(FIXNUM *pn, F77_string name)
   {int n, m, l;
    FIXNUM rv;

    n = *pn;
    m = strlen(SC_gs.version);

    l = min(n, m);

    SC_strncpy(SC_F77_C_STRING(name), *pn, SC_gs.version, l);

    rv = m;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCELEN - return the length of an array
 *        - this version takes a CRAY pointee (e.g. a of pointer (ipa, a))
 *        - WARNING: only for F77 with CRAY pointer extensions
 */

FIXNUM F77_FUNC(scelen, SCELEN)(void *p)
   {FIXNUM rv;

    rv = SC_arrlen(p);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCPAUS - FORTRAN interface to SC_pause */

FIXNUM F77_FUNC(scpaus, SCPAUS)(void)
   {FIXNUM rv;

    SC_pause();

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFTCS - FORTRAN interface to convert FORTRAN string to C string */

FIXNUM F77_FUNC(scftcs, SCFTCS)(F77_string out, F77_string in, FIXNUM *pnc)
   {FIXNUM rv;

    SC_FORTRAN_STR_C(SC_F77_C_STRING(out), in , *pnc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCADVN - FORTRAN interface to SC_advance_name */

FIXNUM F77_FUNC(scadvn, SCADVN)(FIXNUM *pnc, F77_string name)
   {int n;
    FIXNUM rv;
    char bf[MAXLINE];

    n = *pnc;

    SC_FORTRAN_STR_C(bf, name, n);
    SC_advance_name(bf);
    SC_strncpy(SC_F77_C_STRING(name), *pnc, bf, n);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                       LEXICAL SCANNING ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* SC_LEX_STR_PTR - return the SC_lexical_stream pointer associated with
 *                - the index if the index is valid and NULL otherwise
 */

static SC_lexical_stream *SC_lex_str_ptr(int strid)
   {int n;
    SC_lexical_stream *ls;

    n = SC_array_get_n(_SC.lexs);
    if ((strid < 0) || (strid >= n))
       ls = NULL;
    else
       ls = SC_array_get(_SC.lexs, strid);

    return(ls);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCOPLS - open a SC_lexical_stream
 *        - save the SC_lexical_stream pointer in an internal array
 *        - and return an integer index to the pointer if successful
 *        - return -1 otherwise
 */

FIXNUM F77_FUNC(scopls, SCOPLS)(FIXNUM *pnchr, F77_string name,
                                FIXNUM *pinbs, FIXNUM *pstrbs,
                                PFInt scan, FIXNUM *pfl)
   {FIXNUM rv;
    char s[MAXLINE];
    SC_lexical_stream *str;

    SC_FORTRAN_STR_C(s, name, *pnchr);

    if (*pfl == 0)
       scan = DEFAULT_SCANNER;

    if (strcmp(s, "tty") == 0)
       str = SC_open_lexical_stream(NULL, 0, 0, scan,
				    NULL, NULL, NULL, NULL, NULL, NULL);
    else
       str = SC_open_lexical_stream(s, 0, 0, scan,
				    NULL, NULL, NULL, NULL, NULL, NULL);
    if (str == NULL)
       rv = -1;
    else
       {rv = SC_array_get_n(_SC.lexs);
	SC_array_push(_SC.lexs, &str);};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCCLLS - close the SC_lexical_stream associated with the integer index
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(scclls, SCCLLS)(FIXNUM *strid)
   {int id;
    FIXNUM rv;
    SC_lexical_stream *str, *v;

    id = *strid;
    v  = NULL;

    str = SC_lex_str_ptr(id);
    SC_close_lexical_stream(str);

    SC_array_set(_SC.lexs, id, &v);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRDLS - read a line from the SC_lexical_stream associated with
 *        - the integer index
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(scrdls, SCRDLS)(FIXNUM *strid, FIXNUM *pnc, F77_string ps)
   {int n, nr, ni;
    FIXNUM rv;
    char *s;
    SC_lexical_stream *str;

    str = SC_lex_str_ptr((int) *strid);
    if (str != NULL)
       {s = str->in_bf;
	n = SC_arrlen(s);
	GETLN(s, n, str->file);

	ni = *pnc;
	nr = strlen(s);    

	ni = min(ni, nr);

	memset(SC_F77_C_STRING(ps), ' ', ni);
	SC_strncpy(SC_F77_C_STRING(ps), *pnc, s, ni);

	*pnc = ni;};

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCSCAN - scan the given line of text and return arrays of tokens
 *        - the lexical rules are defined by the routine attached to the
 *        - lexical stream when it is opened. However a default set of
 *        - rules implements a FORTRANish syntax.
 *        - The string to be scanned is contained in the lexical stream
 *        - and must be supplied by a call to SCRDLS
 *        - Arguments:
 *        - 
 *        -    STRID  - id number of lexical stream (scalar)
 *        -    PMXTOK - maximum number of tokens to be returned (scalar)
 *        -    PWIDTH - character field width (e.g. char*8 => 8) (scalar)
 *        -    TOK    - character array char*PWIDTH(PMXTOK) for returned
 *        -           - tokens (array)
 *        -    PNTOK  - actual number of tokens available (scalar)
 *        -    NCTOK  - character length of each non-numeric token (array)
 *        -    IXTOK  - index in TOK for each non-numeric token (array)
 *        -    TOKTYP - type for each token (array)
 *        -           - default scanner uses:
 *        -           -  TYPE   NAME         EXAMPLE
 *        -           -     1   DELIMITER    & ( ) , : < = > _ |
 *        -           -     2   ALPHANUM     abc
 *        -           -     3   INTEGER      10
 *        -           -     4   REAL         1.2 6.0e10
 *        -           -     5   OCTAL        17b
 *        -           -     6   HEX          #17
 *        -           -     7   OPERAND      .and.
 *        -           -     8   STRING       "foo"
 *        -           -  1000   HOLLERITH    3hFOO
 *        -    TOKVAL - numerical value for numerical tokens as REAL (array)
 *        -
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(scscan, SCSCAN)(FIXNUM *strid, FIXNUM *pmxtok, FIXNUM *pwidth,
                                F77_string tok, FIXNUM *pntok, FIXNUM *nctok,
                                FIXNUM *ixtok, FIXNUM *toktyp, double *tokval)
   {int i, indx, nc;
    int n_tok, n_tok_max, width;
    FIXNUM rv;
    long lval;
    double dval;
    char *s;
    SC_lexical_stream *str;

    str = SC_lex_str_ptr((int) *strid);
    if (str != NULL)
       {SC_scan(str, FALSE);

	n_tok  = str->n_tokens;
	*pntok = (FIXNUM) n_tok;

	width     = *pwidth;
	n_tok_max = *pmxtok;
	n_tok     = min(n_tok, n_tok_max);
	for (indx = 0, i = 0; i < n_tok; i++)
	    {toktyp[i] = SC_TOKEN_TYPE(str, i);
	     ixtok[i]  = 0;
	     nctok[i]  = 0;

	     switch (toktyp[i])
	        {case SC_DINT_TOK :
		 case SC_HINT_TOK :
		 case SC_OINT_TOK :
		      lval = SC_TOKEN_INTEGER(str, i);
		      tokval[i] = lval;
		      break;

		 case SC_REAL_TOK :
		      dval = SC_TOKEN_REAL(str, i);
		      tokval[i] = dval;
		      break;
   
		 case SC_STRING_TOK :
		      s  = SC_TOKEN_STRING(str, i);
		      nc = strlen(s);
		      strncpy(SC_F77_C_STRING(tok) + indx*width,
			      s, nc);

		      ixtok[i] = indx;
		      nctok[i] = nc;

		      indx += (nc + width - 1)/width;
		      break;
   
		   default :
			break;};};};

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         ASSOCIATION LIST ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* SCCHAL - change an item from the given association list
 *        - add the item if necessary
 *        - assumes PV points to static space
 *        - NV then tells how many items are pointed to!
 *        - PTYPE names the type and has one less indirection than PV
 */

FIXNUM F77_FUNC(scchal, SCCHAL)(FIXNUM *pal, FIXNUM *nn, F77_string pname,
                                FIXNUM *nt, F77_string ptype, FIXNUM *nv,
                                void *pv)
   {long nb, ni;
    FIXNUM rv;
    char lname[MAXLINE], ltype[MAXLINE];
    void *val, *p;
    pcons *pc;

    if (*pal == 0)
       pc = NULL;
    else
       pc = SC_GET_POINTER(pcons, *pal);

    SC_FORTRAN_STR_C(lname, pname, *nn);
    SC_FORTRAN_STR_C(ltype, ptype, *nt);

/* copy the incoming values */
    nb  = SIZEOF(ltype);
    ni  = *nv;
    val = CMAKE_N(char, ni*nb);
    memcpy(val, pv, ni*nb);

/* add the extra level of indirection needed */
    SC_strcat(ltype, MAXLINE, " *");

    pc = SC_change_alist(pc, lname, ltype, val);
    if (*pal != 0)
       {p = SC_DEL_POINTER(void, *pal);
	SC_ASSERT(p != NULL);};

    *pal = (FIXNUM) SC_ADD_POINTER(pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRMAL - remove an item from the given association list */

FIXNUM F77_FUNC(scrmal, SCRMAL)(FIXNUM *pal, FIXNUM *nn, F77_string pname)
   {FIXNUM rv;
    char lname[MAXLINE];
    pcons *pc;
    void *p;

    pc = SC_GET_POINTER(pcons, *pal);

/* cheap insurance */
    if (pc == NULL)
       rv = FALSE;

    else
       {SC_FORTRAN_STR_C(lname, pname, *nn);

	pc = SC_rem_alist(pc, lname);
	p  = SC_DEL_POINTER(void, *pal);
	SC_ASSERT(p != NULL);

	*pal = (FIXNUM) SC_ADD_POINTER(pc);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRLAL - release an association list */

FIXNUM F77_FUNC(scrlal, SCRLAL)(FIXNUM *pal, FIXNUM *pl)
   {FIXNUM rv;
    pcons *pc;
    void *p;

    pc = SC_GET_POINTER(pcons, *pal);

/* cheap insurance */
    if (pc == NULL)
       rv = FALSE;

    else
       {SC_free_alist(pc, *pl);

	p = SC_DEL_POINTER(void, *pal);    
	SC_ASSERT(p != NULL);

	*pal = 0L;

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCAPAL - append the contents of PAL2 to PAL1 */

FIXNUM F77_FUNC(scapal, SCAPAL)(FIXNUM *pal1, FIXNUM *pal2)
   {FIXNUM rv;
    pcons *pc1, *pc2;
    void *p;

    pc1 = SC_GET_POINTER(pcons, *pal1);
    pc2 = SC_GET_POINTER(pcons, *pal2);

    pc2 = SC_copy_alist(pc2);
    pc1 = SC_append_alist(pc1, pc2);

    p = SC_DEL_POINTER(void, *pal1);
    SC_ASSERT(p != NULL);

    *pal1 = (FIXNUM) SC_ADD_POINTER(pc1);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         TIMING ROUTINES                                  */

/*--------------------------------------------------------------------------*/

/* SCCTIM - return the cpu time used in seconds and microseconds
 *          SINCE THE FIRST CALL
 */

FIXNUM F77_FUNC(scctim, SCCTIM)(double *ptim)
   {FIXNUM rv;

    *ptim = (double) SC_cpu_time();

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCWTIM - return the wall clock time in seconds and microseconds
 *          SINCE THE FIRST CALL
 */

FIXNUM F77_FUNC(scwtim, SCWTIM)(double *ptim)
   {FIXNUM rv;

    *ptim = (double) SC_wall_clock_time();

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCDATE - return a string with the time and date as defined by the
 *          ANSI function ctime
 */

FIXNUM F77_FUNC(scdate, SCDATE)(FIXNUM *pnc, F77_string date)
   {int nc, lc;
    FIXNUM rv;
    char *cdate;

    nc    = *pnc;
    cdate = SC_date();
    lc    = strlen(cdate);    

    SC_strncpy(SC_F77_C_STRING(date), *pnc, cdate, nc);
    CFREE(cdate);

    *pnc = lc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

