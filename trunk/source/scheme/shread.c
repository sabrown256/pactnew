/*
 * SHREAD.C - C and Scheme Primitive Input Routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"
#include "syntax.h"

#define PUSH_CHAR (*_SS_si.pr_ch_un)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CLR_STRM - clear the buffer associated with the stream object */

static void SS_clr_strm(object *str)
   {

    SS_PTR(str) = SS_BUFFER(str);
    *SS_PTR(str) = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RD_LINE - read a line of text */

static object *_SSI_rd_line(object *str)
   {FILE *s;
    char *t, *t1;
    object *ret;

    if (SS_nullobjp(str))
       str = _SS_si.indev;
    else if (!SS_inportp(str = SS_car(str)))
       SS_error("ARGUMENT NOT INPUT-PORT - READ-LINE", str);

    s = SS_INSTREAM(str);
    t = SS_BUFFER(str);
    SS_PTR(str) = t;
    if (GETLN(t, MAXLINE, s) == NULL)
       return(SS_eof);

    for (t1 = t; *t1 != '\n'; t1++);
    *t1 = '\0';

    ret = SS_mk_string(t);
    *t = '\0';

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RD_LST - read a list or cons */

static object *SS_rd_lst(object *str)
   {int c, ok;
    object *frst, *ths, *nxt, *o;

    frst = SS_null;
    ths  = SS_null;
    for (ok = TRUE; ok == TRUE; )
        {switch (c = SS_get_ch(str, TRUE))
            {case ')' :
	          ok = FALSE;
	          o  = frst;
		  break;

	     case '.' :
	          c = SS_get_ch(str, FALSE);
		  if (strchr(" \t\r\n", c) != NULL)
		     {SS_setcdr(ths, SS_READ_EXPR(str));
		      while ((c = SS_get_ch(str, TRUE)) != ')');
		      ok = FALSE;
		      o  = frst;
		      break;}

		  else
		     {PUSH_CHAR(c, str);
		      PUSH_CHAR('.', str);
		      c = ' ';};

	     default :
	          PUSH_CHAR(c, str);
		  nxt = SS_READ_EXPR(str);
		  if (SS_eofobjp(nxt))
		     {if ((c = SS_get_ch(str, TRUE)) == EOF)
			 {SS_clr_strm(str);
			  SS_error("UNEXPECTED END OF FILE - READ-LIST",
				   SS_null);}
		      else
			 PUSH_CHAR(c, str);};
		  SS_end_cons(frst, ths, nxt);
		  break;};};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RD_VCT - read a vector */

static object *SS_rd_vct(object *str)
   {object *lst, *vct;

    lst = SS_rd_lst(str);
    SS_MARK(lst);
    vct = SS_lstvct(lst);
    SS_GC(lst);

    return(vct);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RD_CHR - read-char for Scheme */

static object *_SSI_rd_chr(object *arg)
   {object *str, *o;

    o = SS_null;

    if (SS_nullobjp(arg))
       {*SS_PTR(_SS_si.indev) = '\0';
        o = SS_mk_char((int) SS_get_ch(_SS_si.indev, FALSE));}

    else if (SS_inportp(str = SS_car(arg)))
       {*SS_PTR(str) = '\0';
        o = SS_mk_char((int) SS_get_ch(str, FALSE));}

    else
       SS_error("ARGUMENT TO READ NOT INPUT-PORT", arg);

    return(o);}

/*--------------------------------------------------------------------------*/

/*                               READ FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* _SS_INTSTRP - return TRUE if S is a SCHEME integer
 *             - return the value in PIV
 *             - NOTE: we parse, but do not use, exactness specifications
 */

static int _SS_intstrp(char *s, int64_t *piv)
   {int rv, sgn, rdx, c;
    int64_t iv;
    char *pt;

    iv = 0;

    if (s == NULL)
       rv = FALSE;

    else if (!SC_gs.unary_plus && (*s == '+'))
       rv = FALSE;

    else if ((strcmp(s, "+") == 0) || (strcmp(s, "-") == 0))
       rv = FALSE;

    else if (strchr("#+-0123456789abcdefABCDEF", (int) *s) == NULL)
       rv = FALSE;

    else
       {sgn = 1;
        rdx = 10;
	pt  = s;

/* check for sign */
	c = *pt;
        switch (c)
           {case '-' :
                 sgn = -1;
                 c = *++pt;
		 break;
            case '+' :
                 sgn = 1;
                 c = *++pt;
		 break;};

/* check for radix or exactness */
	while (c == '#')
           {c = *++pt;
	    switch (c)
	       {case 'b' :
                     rdx = 2;
		     c   = *++pt;
                     break;
	        case 'o' :
                     rdx = 8;
		     c   = *++pt;
                     break;
	        case 'd' :
                     rdx = 10;
		     c   = *++pt;
                     break;
	        case 'x' :
                     rdx = 16;
		     c   = *++pt;
                     break;
	        case 'e' :
		     c = *++pt;
                     break;
	        case 'i' :
		     c = *++pt;
                     break;
	        default :
                     *piv = iv;
                     return(FALSE);
                     break;};};

	if ((strlen(pt) == 1) && (strchr("0123456789", (int) *pt) == NULL))
	   rv = FALSE;

	else
	   {iv = sgn*STRTOLL(pt, &pt, rdx);
	    rv = (pt == (s + strlen(s)));};};

    *piv = iv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RD_ATM - read and make an atomic object */

static object *SS_rd_atm(object *str)
   {int c, inbrackets;
    int64_t iv;
    char token[MAXLINE], *pt;
    object *o;

    inbrackets = FALSE;
    pt         = token;
    while ((c = SS_get_ch(str, FALSE)) != EOF)
       {if (strchr(" \t\n\r();", c) == NULL)
           {*pt++ = c;
            if (c == '[')
               inbrackets = TRUE;
            if (c == ']')
               inbrackets = FALSE;}
        else if ((_SS_si.bracket_flag) && (inbrackets) && (c == ' '))
           continue;
        else
           {if ((c == '(') || (c == ')') || (c == ';'))
               PUSH_CHAR(c, str);
            break;};};
    *pt = '\0';

    if (_SS_intstrp(token, &iv))
       o = SS_mk_integer(iv);

#ifdef LARGE
    else if (strncmp(token, "#\\", 2) == 0)
       o = SS_mk_char(token[2]);
#endif

    else if (SC_fltstrp(token))
       o = SS_mk_float(ATOF(token));

    else
       o = SS_add_variable(token);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RD_STR - read and make an string object */

static object *SS_rd_str(object *str)
   {int i, c, nc, bfsz;
    int delta;
    char *pt, *bf;
    object *ret;

    bfsz = MAXLINE;
    bf   = CMAKE_N(char, bfsz + 2);
    while (TRUE)
       {pt = bf + bfsz - MAXLINE;
        for (i = 0; i < MAXLINE;)
            {c = SS_get_ch(str, FALSE);
             if (c == '\\')
                {nc = SS_get_ch(str, FALSE);
                 if (nc == '\"')
                    pt[i++] = nc;
                 else if (nc == '\n')
		    {SS_LINE_NUMBER(str)++;
                     SS_CHAR_INDEX(str) = 1;
		     continue;}
                 else
                    {pt[i++] = c;
                     pt[i++] = nc;};}
             else if (c != '\"')
                pt[i++] = c;
             else
                break;};

        if (i < MAXLINE)
           break;

        else
           {bfsz += MAXLINE;
            CREMAKE(bf, char, bfsz + 2);};};

    pt[i] = '\0';
    delta = SC_arrlen(bf);
    ret   = SS_mk_string(bf);

    CFREE(bf);

/* help out with the byte count */
    SC_mem_stats_acc(-delta, -delta);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PR_READ - the C level reader */

static object *_SS_pr_read(object *str)
   {int c;
    input_port *prt;
    object *rv;

    prt = SS_GET(input_port, str);

    c = SS_get_ch(str, TRUE);
    switch (c)
       {case EOF :
	     rv = SS_eof;
	     break;

#ifdef HAVE_C_SYNTAX
        case '{' :
	     {extern object *SS_syntax_c(object *str);

	      PUSH_CHAR(c, str);

	      rv = SS_syntax_c(str);};
	     break;
#endif

        case '(' :
	     rv = SS_rd_lst(str);
	     break;

        case '\"':
	     rv = SS_rd_str(str);
	     break;

        case '\'':
	     rv = SS_mk_cons(SS_quoteproc,
			     SS_mk_cons(SS_READ_EXPR(str),
					SS_null));
	     break;
        case '`' :
	     rv = SS_mk_cons(SS_quasiproc,
			     SS_mk_cons(SS_READ_EXPR(str),
					SS_null));
	     break;
        case ',' :
	     c = SS_get_ch(str, TRUE);
	     if (c == '@')
	        rv = SS_mk_cons(SS_unqspproc,
				SS_mk_cons(SS_READ_EXPR(str),
					   SS_null));
	     else
	        {PUSH_CHAR(c, str);
		 rv = SS_mk_cons(SS_unqproc,
				 SS_mk_cons(SS_READ_EXPR(str),
					    SS_null));};
	     break;
#ifdef LARGE
        case '#' :
	     c = SS_get_ch(str, FALSE);
	     if (c == '(')
	        rv = SS_rd_vct(str);
	     else
	        {PUSH_CHAR(c, str);
		 PUSH_CHAR('#', str);
		 rv = SS_rd_atm(str);};
	     break;
#endif

	case ')' :
             PRINT(stdout, "SYNTAX ERROR: '%c' on line %d char %d in file %s\n",
		   c, prt->iln, prt->ichr-1, prt->name);
	     SS_error("BAILING OUT ON READ - _SS_PR_READ", NULL);

        default :
	     PUSH_CHAR(c, str);
	     rv = SS_rd_atm(str);
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_READ - the C level reader
 *         - this level is here to implement the transcript mechanism
 */

object *SS_read(object *str)
   {object *obj;

    if (_SS_si.read == NULL)
       _SS_si.read = _SS_pr_read;

    obj = SS_READ_EXPR(str);

    switch (_SS_si.hist_flag)
       {case STDIN_ONLY :
	     if (str != _SS_si.indev)
	        break;

        case ALL :
	     SS_print(obj, "", "\r\n", _SS_si.histdev);

        default :
	     break;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_READ - the Scheme level reader which invokes the C level reader */

static object *_SSI_read(object *obj)
   {object *op, *o;

    o = SS_null;

    if (SS_nullobjp(obj))
       o = SS_read(_SS_si.indev);

    else if (SS_inportp(op = SS_car(obj)))
       o = SS_read(op);

    else
       SS_error("ARGUMENT TO READ NOT INPUT-PORT", obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_OPN_IN - open-input-file in Scheme */

static object *_SSI_opn_in(object *obj)
   {char *s, *t, *msg;
    FILE *str;
    object *rv;

    s = NULL;
    SS_args(obj,
	    SC_STRING_I, &s,
	    0);

    if (s == NULL)
       SS_error("BAD STRING TO OPEN-INPUT-FILE", obj);

/* GOTCHA: this causes a memory leak as coded
    int st;
    char path[MAXLINE];
    st = SC_file_path(s, path, MAXLINE, FALSE);
    if (st == 0)
       s = path;
*/
    str = io_open(s, "r");
    if (str == NULL)
       {t = SC_search_file(NULL, s);
        if (t == NULL)
	   {msg = SC_dsnprintf(FALSE, "CAN'T FIND FILE '%s' - OPEN-INPUT-FILE", s);
	    SS_error(msg, obj);};

        str = io_open(t, "r");
        if (str == NULL)
	   {msg = SC_dsnprintf(FALSE, "CAN'T OPEN '%s' - OPEN-INPUT-FILE", t);
	    SS_error(msg, obj);};

	CFREE(t);};

    rv = SS_mk_inport(str, s);

    CFREE(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CLS_IN - close-input-port in Scheme */

static object *_SSI_cls_in(object *obj)
   {

    if (!SS_inportp(obj))
       SS_error("BAD INPUT-PORT TO CLOSE-INPUT-PORT", obj);

    io_close(SS_INSTREAM(obj));

    return(SS_t);}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SSI_CALL_IF - call-with-input-file in Scheme */

static object *_SSI_call_if(object *argl)
   {FILE *str;
    char *s;
    object *obj, *old_indev, *ret;

    s   = NULL;
    obj = SS_car(argl);

    if (SS_stringp(obj))
       s = SS_STRING_TEXT(obj);
    else if (SS_variablep(obj))
       s = SS_VARIABLE_NAME(obj);
    else
       SS_error("BAD STRING TO OPEN-INPUT-FILE", obj);

    str = io_open(s, BINARY_MODE_R);
    if (str == NULL)
       SS_error("CAN'T OPEN FILE - OPEN-INPUT-FILE", obj);

    old_indev = _SS_si.indev;
    _SS_si.indev  = SS_mk_inport(str, s);
    ret       = SS_exp_eval(&_SS_si, SS_cdr(argl));

    _SSI_cls_in(_SS_si.indev);

    _SS_si.indev = old_indev;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CURR_IP - current-input-port in Scheme */

object *_SSI_curr_ip(void)
    {

     return(_SS_si.indev);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SSI_IPORTP - input-port? at Scheme level */

static object *_SSI_iportp(object *obj)
   {object *o;

    o = SS_inportp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRPRT - string->port at the Scheme level */

static object *_SSI_strprt(object *arg)
   {object *port;

    if (!SS_stringp(arg))
       SS_error("BAD STRING TO STRING->PORT", arg);

    port = SS_mk_inport(NULL, "string");
    strcpy(SS_BUFFER(port), SS_STRING_TEXT(arg));
    SS_PTR(port) = SS_BUFFER(port);

    return(port);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ADD_VARIABLE - add a symbol the the hash table at parse time */

object *SS_add_variable(char *name)
   {object *op;

    op = (object *) SC_hasharr_def_lookup(_SS_si.symtab, name);
    if (op != NULL)
       return(op);

    op = SS_mk_variable(name, SS_null);
    SS_UNCOLLECT(op);
    if (SC_hasharr_install(_SS_si.symtab, name, op, SS_POBJECT_S, TRUE, TRUE) == NULL)
       LONGJMP(SC_gs.cpu, ABORT);

    return(op);}

/*--------------------------------------------------------------------------*/

/*                           PARSER MANAGER FUNCTIONS                       */

/*--------------------------------------------------------------------------*/

/* SS_ADD_PARSER - add a new parser function which is to be
 *               - associated with a file extension
 */

void SS_add_parser(char *ext, PFPObject fnc)
   {SC_address ad;

    if (_SS.parser_tab == NULL)
       {_SS.parser_tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

	SS_add_parser(".scm", _SSI_scheme_mode);};

    ad.funcaddr = (PFInt) fnc;
    SC_hasharr_install(_SS.parser_tab, ext, ad.memaddr, "PFPObject", FALSE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_PARSER - return the current syntax mode */

static PFPObject SS_get_parser(int id)
   {SS_smp_state *sa;
    PFPObject op;

    sa = _SS_get_state(id);

    op = sa->parser;

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_USE_PARSER - change the current parser according to
 *               - the canonical file suffix, SFX
 *               -   "c"  C parser
 *               -   "f"  Fortran parser
 */

PFPObject SS_use_parser(char *sfx)
   {char t[MAXLINE];
    PFPObject np, op;
    SC_address ad;

/* NOTE: unconditionally get the current parser in case the file
 * has no extension at all
 */
    op = SS_get_parser(-1);

    snprintf(t, MAXLINE, ".%s", sfx);
    ad.memaddr = SC_hasharr_def_lookup(_SS.parser_tab, t);

    np = (PFPObject) ad.funcaddr;
    if (np != NULL)
       (*np)();

    SS_set_parser(np);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CHANGE_PARSER - change the current read function to be consistent
 *                  - with the file being loaded
 *                  - return the current one so that it may be restored
 *                  - after the file is loaded
 */

static PFPObject SS_change_parser(object *fnm)
   {char *s, *t;
    PFPObject np, op;
    SC_address ad;

    op = NULL;
    s  = NULL;
    SS_args(fnm,
	    SC_STRING_I, &s,
	    0);

    if (s == NULL)
       SS_error("BAD STRING TO SS_CHANGE_PARSER", fnm);

    else

/* NOTE: unconditionally get the current parser in case the file
 * has no extension at all
 */
       {op = SS_get_parser(-1);

	t = strchr(s, '.');
	if (t != NULL)
	   {ad.memaddr = SC_hasharr_def_lookup(_SS.parser_tab, t);

	    np = (PFPObject) ad.funcaddr;
	    if (np != NULL)
	       {(*np)();
		SS_set_parser(np);};};

	CFREE(s);};

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RESTORE_PARSER - restore the current read function from the one
 *                   - passed in
 */

static void SS_restore_parser(PFPObject op)
   {

    (*op)();
    SS_set_parser(op);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_PARSER - set the current parser function from the one
 *               - passed in
 */

void SS_set_parser(PFPObject op)
   {SS_smp_state *sa;

    sa = _SS_get_state(-1);

    sa->parser = op;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SCHEME_MODE - switch to native SCHEME parsing */

object *_SSI_scheme_mode(void)
   {PFPObject cp;

/* if the current parser is already SCHEME mode assume that the prompt
 * is set to something the user intended and leave it alone
 */
    cp = SS_get_parser(-1);
    if (cp != _SSI_scheme_mode)
       SS_set_prompt("Scheme-> ");

    _SS_si.read        = _SS_pr_read;
    _SS_si.name_reproc = NULL;

    SS_set_parser(_SSI_scheme_mode);

    return(SS_t);}

/*--------------------------------------------------------------------------*/

/*                            LOAD FUNCTIONS                                */

/*--------------------------------------------------------------------------*/

/* SS_LOAD - Scheme load
 *         - an optional final argument causes the load to happen in the
 *         - global environment if SS_t
 */

object *SS_load(object *argl)
   {int c;
    object *fnm, *strm, *flag;
    PFPObject prs;

    flag = SS_f;
    fnm  = SS_car(argl);
    argl = SS_cdr(argl);
    if (SS_consp(argl))
       {flag = SS_car(argl);
        if (SS_true(flag))
           {SS_Save(_SS_si.env);
            _SS_si.env = _SS_si.global_env;};};

    strm = _SSI_opn_in(fnm);

/* check for the first line starting with #! */
    c = SS_get_ch(strm, TRUE);
    if (c == '#')
       {c = SS_get_ch(strm, FALSE);
        if (c == '!')
           *SS_PTR(strm) = '\0';
        else
           {PUSH_CHAR(c, strm);
            PUSH_CHAR('#', strm);};}
    else
       PUSH_CHAR(c, strm);

    SS_Save(_SS_si.rdobj);
    SS_Save(_SS_si.evobj);

    prs = SS_change_parser(fnm);

    while (TRUE)
       {SS_Assign(_SS_si.rdobj, SS_read(strm));
        if (_SS_si.post_read != NULL)
           (*_SS_si.post_read)(strm);

        if (SS_eofobjp(_SS_si.rdobj))
           {_SSI_cls_in(strm);
	    SS_GC(strm);
            break;};
        SS_Save(_SS_si.env);
        SS_Assign(_SS_si.evobj, SS_exp_eval(&_SS_si, _SS_si.rdobj));
        SS_Restore(_SS_si.env);

        if (_SS_si.post_eval != NULL)
           (*_SS_si.post_eval)(strm);};

    SS_restore_parser(prs);

    SS_Restore(_SS_si.evobj);
    SS_Restore(_SS_si.rdobj);

    if (SS_true(flag))
       {SS_Restore(_SS_si.env);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_READ - install the Scheme input primitives */

void _SS_inst_read(void)
   {

    SS_install("close-input-file",
               "Procedure: Close the specified input port and release the IN_PORT object",
               SS_sargs,
               _SSI_cls_in, SS_PR_PROC);

    SS_install("input-port?",
               "Returns #t if the object is an input port (IN_PORT), and #f otherwise",
               SS_sargs, 
               _SSI_iportp, SS_PR_PROC);

    SS_install("load",
               "Procedure: Open a file of Scheme forms and eval all the objects in it\n     Usage: (load <file> [#t])",
               SS_nargs,
               SS_load, SS_PR_PROC);

    SS_install("open-input-file",
               "Procedure: Open the specified file for input and return an IN_PORT object",
               SS_sargs,
               _SSI_opn_in, SS_PR_PROC);

    SS_install("read",
               "Procedure: reads an ASCII representation of an object and returns the object",
               SS_nargs,
               _SSI_read, SS_PR_PROC);

    SS_install("read-char",
               "Procedure: Read and return a single character",
               SS_nargs,
               _SSI_rd_chr, SS_PR_PROC);

    SS_install("scheme-mode",
               "Procedure: set SCHEME syntax parsing mode",
               SS_zargs,
               _SSI_scheme_mode, SS_PR_PROC);

    SS_install("string->port",
               "Procedure: Encapsulate a string as a pseudo input-port for reading",
               SS_sargs,
               _SSI_strprt, SS_PR_PROC);

#ifdef LARGE

    SS_install("call-with-input-file",
               "Procedure: open the named file and eval a procedure using the port for input",
               SS_nargs,
               _SSI_call_if, SS_PR_PROC);

    SS_install("current-input-port",
               "Procedure: Returns the current default input port",
               SS_zargs,
               _SSI_curr_ip, SS_PR_PROC);

    SS_install("read-line",
               "Procedure: Read a line of text and return a string",
               SS_nargs,
               _SSI_rd_line, SS_PR_PROC);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
