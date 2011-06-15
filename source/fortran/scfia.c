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

# define DEFAULT_SCANNER FF_ID(f77lxr, F77LXR)

#else

# define DEFAULT_SCANNER NULL

#endif

extern int
 FF_ID(f77lxr, F77LXR)(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCVERS - return the PACT version in NAME
 *        - and the number of characters in the version string
 *        - as the return value
 */

FIXNUM FF_ID(scvers, SCVERS)(FIXNUM *sncn, char *name)
   {int n, m, l;
    FIXNUM rv;

    n = *sncn;
    m = strlen(SC_gs.version);

    l = min(n, m);

    SC_strncpy(name, *sncn, SC_gs.version, l);

    rv = m;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCALEN - return the length of an array
 *        - this version takes a CRAY pointee (e.g. a of pointer (ipa, a))
 *        - WARNING: only for Fortran with CRAY pointer extensions
 */

FIXNUM FF_ID(scalen, SCALEN)(void *p)
   {FIXNUM rv;

    rv = SC_arrlen(p);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCPAUS - FORTRAN interface to SC_pause */

FIXNUM FF_ID(scpaus, SCPAUS)(void)
   {FIXNUM rv;

    SC_pause();

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFTCS - FORTRAN interface to convert FORTRAN string to C string */

FIXNUM FF_ID(scftcs, SCFTCS)(char *out, char *in, FIXNUM *snci)
   {FIXNUM rv;

    SC_FORTRAN_STR_C(out, in, *snci);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCADVN - FORTRAN interface to SC_advance_name */

FIXNUM FF_ID(scadvn, SCADVN)(FIXNUM *sncn, char *name)
   {int n;
    FIXNUM rv;
    char bf[MAXLINE];

    n = *sncn;

    SC_FORTRAN_STR_C(bf, name, n);
    SC_advance_name(bf);
    SC_strncpy(name, *sncn, bf, n);

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

FIXNUM FF_ID(scopls, SCOPLS)(FIXNUM *sncn, char *name,
			     PFInt scan, FIXNUM *sfl)
   {FIXNUM rv;
    char s[MAXLINE];
    SC_lexical_stream *str;

    SC_FORTRAN_STR_C(s, name, *sncn);

    if (*sfl == 0)
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

FIXNUM FF_ID(scclls, SCCLLS)(FIXNUM *sid)
   {int id;
    FIXNUM rv;
    SC_lexical_stream *str, *v;

    id = *sid;
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

FIXNUM FF_ID(scrdls, SCRDLS)(FIXNUM *sid, FIXNUM *sncs, char *ps)
   {int n, nr, ni;
    FIXNUM rv;
    char *s;
    SC_lexical_stream *str;

    str = SC_lex_str_ptr((int) *sid);
    if (str != NULL)
       {s = str->in_bf;
	n = SC_arrlen(s);
	GETLN(s, n, str->file);

	ni = *sncs;
	nr = strlen(s);    

	ni = min(ni, nr);

	memset(ps, ' ', ni);
	SC_strncpy(ps, *sncs, s, ni);

	*sncs = ni;};

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
 *        -    SID    - id number of lexical stream (scalar)
 *        -    SMXTOK - maximum number of tokens to be returned (scalar)
 *        -    SWIDTH - character field width (e.g. char*8 => 8) (scalar)
 *        -    TOK    - character array char*SWIDTH(SMXTOK) for returned
 *        -           - tokens (array)
 *        -    SNTOK  - actual number of tokens available (scalar)
 *        -    ANCTOK - character length of each non-numeric token (array)
 *        -    AIXTOK - index in TOK for each non-numeric token (array)
 *        -    ATOKTY - type for each token (array)
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
 *        -    ATOKVL - numerical value for numerical tokens as REAL (array)
 *        -
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM FF_ID(scscan, SCSCAN)(FIXNUM *sid, FIXNUM *smxtok, FIXNUM *swidth,
			     char *tok, FIXNUM *sntok, FIXNUM *anctok,
			     FIXNUM *aixtok, FIXNUM *atokty, double *atokvl)
   {int i, indx, nc;
    int n_tok, n_tok_max, width;
    FIXNUM rv;
    long lval;
    double dval;
    char *s;
    SC_lexical_stream *str;

    str = SC_lex_str_ptr((int) *sid);
    if (str != NULL)
       {SC_scan(str, FALSE);

	n_tok  = str->n_tokens;
	*sntok = (FIXNUM) n_tok;

	width     = *swidth;
	n_tok_max = *smxtok;
	n_tok     = min(n_tok, n_tok_max);
	for (indx = 0, i = 0; i < n_tok; i++)
	    {atokty[i] = SC_TOKEN_TYPE(str, i);
	     aixtok[i]  = 0;
	     anctok[i]  = 0;

	     switch (atokty[i])
	        {case SC_DINT_TOK :
		 case SC_HINT_TOK :
		 case SC_OINT_TOK :
		      lval = SC_TOKEN_INTEGER(str, i);
		      atokvl[i] = lval;
		      break;

		 case SC_REAL_TOK :
		      dval = SC_TOKEN_REAL(str, i);
		      atokvl[i] = dval;
		      break;
   
		 case SC_STRING_TOK :
		      s  = SC_TOKEN_STRING(str, i);
		      nc = strlen(s);
		      strncpy(tok + indx*width,
			      s, nc);

		      aixtok[i] = indx;
		      anctok[i] = nc;

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

FIXNUM FF_ID(scchal, SCCHAL)(FIXNUM *sal, FIXNUM *sncn, char *pname,
			     FIXNUM *scnt, char *ptype, FIXNUM *snv,
			     void *av)
   {long nb, ni;
    FIXNUM rv;
    char lname[MAXLINE], ltype[MAXLINE];
    void *val, *p;
    pcons *pc;

    if (*sal == 0)
       pc = NULL;
    else
       pc = SC_GET_POINTER(pcons, *sal);

    SC_FORTRAN_STR_C(lname, pname, *sncn);
    SC_FORTRAN_STR_C(ltype, ptype, *scnt);

/* copy the incoming values */
    nb  = SIZEOF(ltype);
    ni  = *snv;
    val = CMAKE_N(char, ni*nb);
    memcpy(val, av, ni*nb);

/* add the extra level of indirection needed */
    SC_strcat(ltype, MAXLINE, " *");

    pc = SC_change_alist(pc, lname, ltype, val);
    if (*sal != 0)
       {p = SC_DEL_POINTER(void, *sal);
	SC_ASSERT(p != NULL);};

    *sal = (FIXNUM) SC_ADD_POINTER(pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRMAL - remove an item from the given association list */

FIXNUM FF_ID(scrmal, SCRMAL)(FIXNUM *sal, FIXNUM *sncn, char *pname)
   {FIXNUM rv;
    char lname[MAXLINE];
    pcons *pc;
    void *p;

    pc = SC_GET_POINTER(pcons, *sal);

/* cheap insurance */
    if (pc == NULL)
       rv = FALSE;

    else
       {SC_FORTRAN_STR_C(lname, pname, *sncn);

	pc = SC_rem_alist(pc, lname);
	p  = SC_DEL_POINTER(void, *sal);
	SC_ASSERT(p != NULL);

	*sal = (FIXNUM) SC_ADD_POINTER(pc);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRLAL - release an association list */

FIXNUM FF_ID(scrlal, SCRLAL)(FIXNUM *sal, FIXNUM *al)
   {FIXNUM rv;
    pcons *pc;
    void *p;

    pc = SC_GET_POINTER(pcons, *sal);

/* cheap insurance */
    if (pc == NULL)
       rv = FALSE;

    else
       {SC_free_alist(pc, *al);

	p = SC_DEL_POINTER(void, *sal);    
	SC_ASSERT(p != NULL);

	*sal = 0L;

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCAPAL - append the contents of SAL2 to SAL1 */

FIXNUM FF_ID(scapal, SCAPAL)(FIXNUM *sal1, FIXNUM *sal2)
   {FIXNUM rv;
    pcons *pc1, *pc2;
    void *p;

    pc1 = SC_GET_POINTER(pcons, *sal1);
    pc2 = SC_GET_POINTER(pcons, *sal2);

    pc2 = SC_copy_alist(pc2);
    pc1 = SC_append_alist(pc1, pc2);

    p = SC_DEL_POINTER(void, *sal1);
    SC_ASSERT(p != NULL);

    *sal1 = (FIXNUM) SC_ADD_POINTER(pc1);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         TIMING ROUTINES                                  */

/*--------------------------------------------------------------------------*/

/* SCCTIM - return the cpu time used in seconds and microseconds
 *          SINCE THE FIRST CALL
 */

FIXNUM FF_ID(scctim, SCCTIM)(double *stim)
   {FIXNUM rv;

    *stim = (double) SC_cpu_time();

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCWTIM - return the wall clock time in seconds and microseconds
 *          SINCE THE FIRST CALL
 */

FIXNUM FF_ID(scwtim, SCWTIM)(double *stim)
   {FIXNUM rv;

    *stim = (double) SC_wall_clock_time();

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCDATE - return a string with the time and date as defined by the
 *          ANSI function ctime
 */

FIXNUM FF_ID(scdate, SCDATE)(FIXNUM *sncd, char *date)
   {int nc, lc;
    FIXNUM rv;
    char *cdate;

    nc    = *sncd;
    cdate = SC_date();
    lc    = strlen(cdate);    

    SC_strncpy(date, *sncd, cdate, nc);
    CFREE(cdate);

    *sncd = lc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

