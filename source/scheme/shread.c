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

#define PUSH_CHAR         si->pr_ch_un
#define READ_EXPR(_o)     ((si->read == NULL) ? SS_null : si->read(si, _o))

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

/* _SSI_SET_SYNTAX - add an entry to the syntax table */

static object *_SSI_set_syntax(SS_psides *si, object *argl)
   {char *nm, *body;
    object *rv;

    nm   = NULL;
    body = NULL;

    SS_args(si, argl,
            SC_STRING_I, &nm,
            0);

    if (_SS.sub == NULL)
       _SS.sub = PS_make_substdes();

    body = _SS_sprintf(si, "%s", SS_cdr(si, argl));

    PS_polysubst_add(_SS.sub, nm, body);

    rv = SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RD_LINE - read a line of text */

static object *_SSI_rd_line(SS_psides *si, object *str)
   {char *t, *p;
    FILE *fp;
    object *ret;

    if (SS_nullobjp(str))
       str = si->indev;
    else if (!SS_inportp(str = SS_car(si, str)))
       SS_error(si, "ARGUMENT NOT INPUT-PORT - READ-LINE", str);

    fp = SS_INSTREAM(str);
    t  = SS_BUFFER(str);
    SS_PTR(str) = t;

    if (GETLN(t, MAXLINE, fp) == NULL)
       ret = SS_eof;

    else
       {p = strchr(t, '\n');
	if (p != NULL)
	   *p = '\0';

	ret = SS_mk_string(si, t);
	*t = '\0';};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RD_LST - read a list or cons */

static object *_SS_rd_lst(SS_psides *si, object *str, int c)
   {int ok;
    object *frst, *ths, *nxt, *o;

    frst = SS_null;
    ths  = SS_null;
    for (ok = TRUE; ok == TRUE; )
        {switch (c = SS_get_ch(si, str, TRUE))
            {case ')' :
	          ok = FALSE;
	          o  = frst;
		  break;

	     case '.' :
	          c = SS_get_ch(si, str, FALSE);
		  if (strchr(" \t\r\n", c) != NULL)
		     {SS_setcdr(si, ths, READ_EXPR(str));
		      while ((c = SS_get_ch(si, str, TRUE)) != ')');
		      ok = FALSE;
		      o  = frst;
		      break;}

		  else
		     {PUSH_CHAR(c, str);
		      PUSH_CHAR('.', str);
		      c = ' ';};

	     default :
	          PUSH_CHAR(c, str);
		  nxt = READ_EXPR(str);
		  if (SS_eofobjp(nxt))
		     {if ((c = SS_get_ch(si, str, TRUE)) == EOF)
			 {SS_clr_strm(str);
			  SS_error(si,
				     "UNEXPECTED END OF FILE - READ-LIST",
				     SS_null);}
		      else
			 PUSH_CHAR(c, str);};
		  SS_end_cons(si, frst, ths, nxt);
		  break;};};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RD_VCT - read a vector */

static object *_SS_rd_vct(SS_psides *si, object *str)
   {object *lst, *vct;

    lst = _SS_rd_lst(si, str, 0);
    SS_mark(lst);
    vct = SS_lstvct(si, lst);
    SS_gc(si, lst);

    return(vct);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RD_CHR - read-char for Scheme */

static object *_SSI_rd_chr(SS_psides *si, object *arg)
   {object *str, *o;

    o = SS_null;

    if (SS_nullobjp(arg))
       {*SS_PTR(si->indev) = '\0';
        o = SS_mk_char(si, (int) SS_get_ch(si, si->indev, FALSE));}

    else if (SS_inportp(str = SS_car(si, arg)))
       {*SS_PTR(str) = '\0';
        o = SS_mk_char(si, (int) SS_get_ch(si, str, FALSE));}

    else
       SS_error(si, "ARGUMENT TO READ NOT INPUT-PORT", arg);

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

    else if (strncmp(s, "0x", 2) == 0)
       {iv = STRTOLL(s, &pt, 16);
        rv = TRUE;}

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

/* _SS_RD_ATM - read and make an atomic object */

static object *_SS_rd_atm(SS_psides *si, object *str)
   {int c, inbrackets;
    int64_t iv;
    char token[MAXLINE], *pt;
    object *o;

    inbrackets = FALSE;
    pt         = token;
    while ((c = SS_get_ch(si, str, FALSE)) != EOF)
       {if (strchr(" \t\n\r();", c) == NULL)
           {*pt++ = c;
            if (c == '[')
               inbrackets = TRUE;
            if (c == ']')
               inbrackets = FALSE;}
        else if ((si->bracket_flag) && (inbrackets) && (c == ' '))
           continue;
        else
           {if ((c == '(') || (c == ')') || (c == ';'))
               PUSH_CHAR(c, str);
            break;};};
    *pt = '\0';

    if (_SS_intstrp(token, &iv))
       o = SS_mk_integer(si, iv);

#ifdef LARGE
    else if (strncmp(token, "#\\", 2) == 0)
       o = SS_mk_char(si, token[2]);
#endif

    else if (SC_fltstrp(token))
       o = SS_mk_float(si, ATOF(token));

    else
       o = SS_add_variable(si, token);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RD_STR - read and make an string object */

static object *_SS_rd_str(SS_psides *si, object *str, int c)
   {int i, nc, bfsz;
    int delta;
    char *pt, *bf;
    object *ret;

    bfsz = MAXLINE;
    bf   = CMAKE_N(char, bfsz + 2);
    while (TRUE)
       {pt = bf + bfsz - MAXLINE;
        for (i = 0; i < MAXLINE;)
            {c = SS_get_ch(si, str, FALSE);
             if (c == '\\')
                {nc = SS_get_ch(si, str, FALSE);
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
    ret   = SS_mk_string(si, bf);

    CFREE(bf);

/* help out with the byte count */
    SC_mem_stats_acc(-delta, -delta);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_EOF - process an EOF */

static object *_SS_chr_eof(SS_psides *si, object *str, int c)
   {object *rv;

    rv = SS_eof;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_QUOTE - read from a ' */

static object *_SS_chr_quote(SS_psides *si, object *str, int c)
   {object *rv;

    rv = SS_mk_cons(si, SS_quoteproc,
		    SS_mk_cons(si, READ_EXPR(str),
			       SS_null));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_QUASIQUOTE - read from a ` */

static object *_SS_chr_quasiquote(SS_psides *si, object *str, int c)
   {object *rv;

    rv = SS_mk_cons(si, SS_quasiproc,
		    SS_mk_cons(si, READ_EXPR(str),
			       SS_null));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_UNQUOTE - read from a , */

static object *_SS_chr_unquote(SS_psides *si, object *str, int c)
   {object *rv;

    c = SS_get_ch(si, str, TRUE);
    if (c == '@')
       rv = SS_mk_cons(si, SS_unqspproc,
		       SS_mk_cons(si, READ_EXPR(str),
				  SS_null));
    else
       {PUSH_CHAR(c, str);
	rv = SS_mk_cons(si, SS_unqproc,
			SS_mk_cons(si, READ_EXPR(str),
				   SS_null));};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_ERROR - you have an error if you get a ')' */

static object *_SS_chr_error(SS_psides *si, object *str, int c)
   {object *rv;
    input_port *prt;

    rv = SS_f;

    prt = SS_GET(input_port, str);

    PRINT(stdout, "SYNTAX ERROR: '%c' on line %d char %d in file %s\n",
	  c, prt->iln, prt->ichr-1, prt->name);

    SS_error(si, "BAILING OUT ON READ - _SS_CHR_ERROR", NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_ATOM - read an atom */

static object *_SS_chr_atom(SS_psides *si, object *str, int c)
   {object *rv;

    PUSH_CHAR(c, str);
    rv = _SS_rd_atm(si, str);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_BLOCK - read a C block */

static object *_SS_chr_block(SS_psides *si, object *str, int c)
   {object *rv;
    extern object *SS_syntax_c(SS_psides *si, object *str);

    PUSH_CHAR(c, str);
    rv = SS_syntax_c(si, str);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHR_POUND - read an object beginning with '#' */

static object *_SS_chr_pound(SS_psides *si, object *str, int c)
   {object *rv;

    c = SS_get_ch(si, str, FALSE);
    if (c == '(')
       rv = _SS_rd_vct(si, str);
    else
       {PUSH_CHAR(c, str);
	PUSH_CHAR('#', str);
	rv = _SS_rd_atm(si, str);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SET_CHAR_MAP - setup the character map for the C level reader */

void _SS_set_char_map(void)
   {int i;
    unsigned char leof = EOF;

    for (i = 0; i < 256; i++)
        _SS.chr_tab[i] = _SS_chr_atom;

    _SS.chr_tab[leof] = _SS_chr_eof;
    _SS.chr_tab['(']  = _SS_rd_lst;
    _SS.chr_tab['\"'] = _SS_rd_str;
    _SS.chr_tab['\''] = _SS_chr_quote;
    _SS.chr_tab['`']  = _SS_chr_quasiquote;
    _SS.chr_tab[',']  = _SS_chr_unquote;
    _SS.chr_tab[')']  = _SS_chr_error;


#ifdef HAVE_C_SYNTAX
    _SS.chr_tab['{']  = _SS_chr_block;
#endif

#ifdef LARGE
    _SS.chr_tab['#']  = _SS_chr_pound;
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PR_READ - the C level reader */

static object *_SS_pr_read(SS_psides *si, object *str)
   {int c;
    object *rv;

    c = SS_get_ch(si, str, TRUE);

#if 1
    unsigned char uc;

    uc = (unsigned char) c;
    rv = _SS.chr_tab[uc](si, str, c);
#else
    char *s;

    s = SS_BUFFER(str);
    SC_ASSERT(s != NULL);

    switch (c)
       {case EOF :
	     rv = SS_eof;
	     break;

#ifdef HAVE_C_SYNTAX
        case '{' :
	     {extern object *SS_syntax_c(SS_psides *si, object *str);

	      PUSH_CHAR(c, str);

	      rv = SS_syntax_c(si, str);};
	     break;
#endif

        case '(' :
	     rv = _SS_rd_lst(si, str, c);
	     break;

        case '\"':
	     rv = _SS_rd_str(si, str, c);
	     break;

        case '\'':
	     rv = SS_mk_cons(si, SS_quoteproc,
			     SS_mk_cons(si, READ_EXPR(str),
					SS_null));
	     break;
        case '`' :
	     rv = SS_mk_cons(si, SS_quasiproc,
			     SS_mk_cons(si, READ_EXPR(str),
					SS_null));
	     break;
        case ',' :
	     c = SS_get_ch(si, str, TRUE);
	     if (c == '@')
	        rv = SS_mk_cons(si, SS_unqspproc,
				SS_mk_cons(si, READ_EXPR(str),
					   SS_null));
	     else
	        {PUSH_CHAR(c, str);
		 rv = SS_mk_cons(si, SS_unqproc,
				 SS_mk_cons(si, READ_EXPR(str),
					    SS_null));};
	     break;
#ifdef LARGE
        case '#' :
	     c = SS_get_ch(si, str, FALSE);
	     if (c == '(')
	        rv = _SS_rd_vct(si, str);
	     else
	        {PUSH_CHAR(c, str);
		 PUSH_CHAR('#', str);
		 rv = _SS_rd_atm(si, str);};
	     break;
#endif

	case ')' :
	     input_port *prt;

	     prt = SS_GET(input_port, str);
             PRINT(stdout, "SYNTAX ERROR: '%c' on line %d char %d in file %s\n",
		   c, prt->iln, prt->ichr-1, prt->name);
	     SS_error(si, "BAILING OUT ON READ - _SS_PR_READ", NULL);

        default :
	     PUSH_CHAR(c, str);
	     rv = _SS_rd_atm(si, str);
	     break;};
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_READ - the C level reader
 *         - this level is here to implement the transcript mechanism
 */

object *SS_read(SS_psides *si, object *str)
   {object *obj, *fp;
    char *pr;

    fp = NULL;
    pr = NULL;
    SS_args(si, str,
	    SS_OBJECT_I, &fp,
	    SC_STRING_I, &pr,
	    0);

    if (si->read == NULL)
       si->read = _SS_pr_read;

    if (pr != NULL)
       _SS.pr_prompt = pr;

    obj = READ_EXPR(fp);

    switch (si->hist_flag)
       {case STDIN_ONLY :
	     if (fp != si->indev)
	        break;

        case ALL :
	     SS_print(si, si->histdev, obj, "", "\r\n");

        default :
	     break;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_READ - the Scheme level reader which invokes the C level reader */

static object *_SSI_read(SS_psides *si, object *args)
   {object *o, *fp;
    char *pr;

    fp = NULL;
    pr = NULL;
    SS_args(si, args,
	    SS_OBJECT_I, &fp,
	    SC_STRING_I, &pr,
	    0);

    if (pr != NULL)
       _SS.pr_prompt = pr;

    o = SS_null;

    if (SS_nullobjp(fp))
       o = SS_read(si, si->indev);

    else if (SS_inportp(fp))
       o = SS_read(si, fp);

    else
       SS_error(si, "ARGUMENT TO READ NOT INPUT-PORT", fp);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_OPN_IN - open-input-file in Scheme */

static object *_SSI_opn_in(SS_psides *si, object *obj)
   {char *s, *t, *msg;
    FILE *str;
    object *rv;

    s = NULL;
    SS_args(si, obj,
	    SC_STRING_I, &s,
	    0);

    if (s == NULL)
       SS_error(si, "BAD STRING TO OPEN-INPUT-FILE", obj);

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
	    SS_error(si, msg, obj);};

        str = io_open(t, "r");
        if (str == NULL)
	   {msg = SC_dsnprintf(FALSE, "CAN'T OPEN '%s' - OPEN-INPUT-FILE", t);
	    SS_error(si, msg, obj);};

	CFREE(t);};

    rv = SS_mk_inport(si, str, s);

    CFREE(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CLS_IN - close-input-port in Scheme */

static object *_SSI_cls_in(SS_psides *si, object *obj)
   {

    if (!SS_inportp(obj))
       SS_error(si, "BAD INPUT-PORT TO CLOSE-INPUT-PORT", obj);

    io_close(SS_INSTREAM(obj));

    return(SS_t);}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SSI_CALL_IF - call-with-input-file in Scheme */

static object *_SSI_call_if(SS_psides *si, object *argl)
   {FILE *str;
    char *s;
    object *obj, *old_indev, *ret;

    s   = NULL;
    obj = SS_car(si, argl);

    if (SS_stringp(obj))
       s = SS_STRING_TEXT(obj);
    else if (SS_variablep(obj))
       s = SS_VARIABLE_NAME(obj);
    else
       SS_error(si, "BAD STRING TO OPEN-INPUT-FILE", obj);

    str = io_open(s, BINARY_MODE_R);
    if (str == NULL)
       SS_error(si, "CAN'T OPEN FILE - OPEN-INPUT-FILE", obj);

    old_indev = si->indev;
    si->indev  = SS_mk_inport(si, str, s);
    ret       = SS_exp_eval(si, SS_cdr(si, argl));

    _SSI_cls_in(si, si->indev);

    si->indev = old_indev;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CURR_IP - current-input-port in Scheme */

object *_SSI_curr_ip(SS_psides *si)
   {

    return(si->indev);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SSI_IPORTP - input-port? at Scheme level */

static object *_SSI_iportp(SS_psides *si, object *obj)
   {object *o;

    o = SS_inportp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRPRT - string->port at the Scheme level */

static object *_SSI_strprt(SS_psides *si, object *arg)
   {object *port;

    if (!SS_stringp(arg))
       SS_error(si, "BAD STRING TO STRING->PORT", arg);

    port = SS_mk_inport(si, NULL, "string");
    strcpy(SS_BUFFER(port), SS_STRING_TEXT(arg));
    SS_PTR(port) = SS_BUFFER(port);

    return(port);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ADD_VARIABLE - add a symbol the the hash table at parse time */

object *SS_add_variable(SS_psides *si, char *name)
   {object *op;
    haelem *hp;

    op = (object *) SC_hasharr_def_lookup(si->symtab, name);
    if (op != NULL)
       return(op);

    op = SS_mk_variable(si, name, SS_null);
    SS_UNCOLLECT(op);
    hp = SC_hasharr_install(si->symtab, name, op, SS_POBJECT_S, 3, -1);
    if (hp == NULL)
       LONGJMP(SC_gs.cpu, ABORT);

    return(op);}

/*--------------------------------------------------------------------------*/

/*                           PARSER MANAGER FUNCTIONS                       */

/*--------------------------------------------------------------------------*/

/* SS_ADD_PARSER - add a new parser function which is to be
 *               - associated with a file extension
 */

void SS_add_parser(char *ext, object *(*prs)(SS_psides *si))
   {SC_address ad;

    if (_SS.parser_tab == NULL)
       {_SS.parser_tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);

	SS_add_parser(".scm", _SSI_scheme_mode);};

    ad.funcaddr = (PFInt) prs;
    SC_hasharr_install(_SS.parser_tab, ext, ad.memaddr, "PFPObject", 2, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_PARSER - return the current syntax mode */

static PFPOprs SS_get_parser(int id)
   {SS_smp_state *sa;
    PFPOprs prs;

    sa = _SS_get_state(id);

    prs = sa->parser;

    return(prs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_USE_PARSER - change the current parser according to
 *               - the canonical file suffix, SFX
 *               -   "c"  C parser
 *               -   "f"  Fortran parser
 */

PFPOprs SS_use_parser(SS_psides *si, char *sfx)
   {char t[MAXLINE];
    PFPOprs np, op;
    SC_address ad;

/* NOTE: unconditionally get the current parser in case the file
 * has no extension at all
 */
    op = SS_get_parser(-1);

    snprintf(t, MAXLINE, ".%s", sfx);
    ad.memaddr = SC_hasharr_def_lookup(_SS.parser_tab, t);

    np = (PFPOprs) ad.funcaddr;
    if (np != NULL)
       np(si);

    SS_set_parser(np);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LOOKUP_PARSER - lookup the parser under SFX and return it */

PFPOprs _SS_lookup_parser(SS_psides *si, char *sfx)
   {PFPOprs np;
    SC_address ad;

    np = NULL;

    if (sfx != NULL)
       {ad.memaddr = SC_hasharr_def_lookup(_SS.parser_tab, sfx);

	np = (PFPOprs) ad.funcaddr;
	if (np != NULL)
	   {np(si);
	    SS_set_parser(np);};};

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHANGE_PARSER - change the current read function to be consistent
 *                   - with the file being loaded
 *                   - return the current one so that it may be restored
 *                   - after the file is loaded
 */

static PFPOprs _SS_change_parser(SS_psides *si, object *fnm)
   {char *s, *t;
    PFPOprs np, op;

    op = NULL;
    s  = NULL;
    SS_args(si, fnm,
	    SC_STRING_I, &s,
	    0);

    if (s == NULL)
       SS_error(si, "BAD STRING TO SS_CHANGE_PARSER", fnm);

    else
       {op = SS_get_parser(-1);

	t  = strrchr(s, '.');
	np = _SS_lookup_parser(si, t);
	if (np != NULL)
	   {np(si);
	    SS_set_parser(np);};

	CFREE(s);};

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RESTORE_PARSER - restore the current read function from the one
 *                    - passed in
 */

static void _SS_restore_parser(SS_psides *si, PFPOprs op)
   {

    op(si);
    SS_set_parser(op);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_PARSER - set the current parser function from the one
 *               - passed in
 */

void SS_set_parser(object *(*prs)(SS_psides *si))
   {SS_smp_state *sa;

    sa = _SS_get_state(-1);

    sa->parser = prs;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SCHEME_MODE - switch to native SCHEME parsing */

object *_SSI_scheme_mode(SS_psides *si)
   {PFPOprs cp;

/* if the current parser is already SCHEME mode assume that the prompt
 * is set to something the user intended and leave it alone
 */
    cp = SS_get_parser(-1);
    if (cp != _SSI_scheme_mode)
       SS_set_prompt(si, "Scheme-> ");

    si->read        = _SS_pr_read;
    si->name_reproc = NULL;

    SS_set_parser(_SSI_scheme_mode);

    return(SS_t);}

/*--------------------------------------------------------------------------*/

/*                            LOAD FUNCTIONS                                */

/*--------------------------------------------------------------------------*/

/* SS_LOAD - Scheme load
 *         - an optional final argument causes the load to happen in the
 *         - global environment if SS_t
 */

object *SS_load(SS_psides *si, object *argl)
   {int c;
    object *fnm, *strm, *flag;
    PFPOprs prs;

    flag = SS_f;
    fnm  = SS_car(si, argl);
    argl = SS_cdr(si, argl);
    if (SS_consp(argl))
       {flag = SS_car(si, argl);
        if (SS_true(flag))
           {SS_save(si, si->env);
            si->env = si->global_env;};};

    strm = _SSI_opn_in(si, fnm);

/* check for the first line starting with #! */
    c = SS_get_ch(si, strm, TRUE);
    if (c == '#')
       {c = SS_get_ch(si, strm, FALSE);
        if (c == '!')
           *SS_PTR(strm) = '\0';
        else
           {PUSH_CHAR(c, strm);
            PUSH_CHAR('#', strm);};}
    else
       PUSH_CHAR(c, strm);

    SS_save(si, si->rdobj);
    SS_save(si, si->evobj);

    prs = _SS_change_parser(si, fnm);

    while (TRUE)
       {SS_assign(si, si->rdobj, SS_read(si, strm));
        if (si->post_read != NULL)
           (*si->post_read)(si, strm);

        if (SS_eofobjp(si->rdobj))
           {_SSI_cls_in(si, strm);
	    SS_gc(si, strm);
            break;};
        SS_save(si, si->env);
        SS_assign(si, si->evobj, SS_exp_eval(si, si->rdobj));
        SS_restore(si, si->env);

        if (si->post_eval != NULL)
           (*si->post_eval)(si, strm);};

    _SS_restore_parser(si, prs);

    SS_restore(si, si->evobj);
    SS_restore(si, si->rdobj);

    if (SS_true(flag))
       {SS_restore(si, si->env);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_READ - install the Scheme input primitives */

void _SS_inst_read(SS_psides *si)
   {

    _SS_set_char_map();

    SS_install(si, "close-input-file",
               "Procedure: Close the specified input port and release the IN_PORT object",
               SS_sargs,
               _SSI_cls_in, SS_PR_PROC);

    SS_install(si, "input-port?",
               "Returns #t if the object is an input port (IN_PORT), and #f otherwise",
               SS_sargs, 
               _SSI_iportp, SS_PR_PROC);

    SS_install(si, "load",
               "Procedure: Open a file of Scheme forms and eval all the objects in it\n     Usage: (load <file> [#t])",
               SS_nargs,
               SS_load, SS_PR_PROC);

    SS_install(si, "open-input-file",
               "Procedure: Open the specified file for input and return an IN_PORT object",
               SS_sargs,
               _SSI_opn_in, SS_PR_PROC);

    SS_install(si, "read",
               "Procedure: reads an ASCII representation of an object and returns the object",
               SS_nargs,
               _SSI_read, SS_PR_PROC);

    SS_install(si, "read-char",
               "Procedure: Read and return a single character",
               SS_nargs,
               _SSI_rd_chr, SS_PR_PROC);

    SS_install(si, "scheme-mode",
               "Procedure: set SCHEME syntax parsing mode",
               SS_zargs,
               _SSI_scheme_mode, SS_PR_PROC);

    SS_install(si, "string->port",
               "Procedure: Encapsulate a string as a pseudo input-port for reading",
               SS_sargs,
               _SSI_strprt, SS_PR_PROC);

#ifdef LARGE

    SS_install(si, "call-with-input-file",
               "Procedure: open the named file and eval a procedure using the port for input",
               SS_nargs,
               _SSI_call_if, SS_PR_PROC);

    SS_install(si, "current-input-port",
               "Procedure: Returns the current default input port",
               SS_zargs,
               _SSI_curr_ip, SS_PR_PROC);

    SS_install(si, "read-line",
               "Procedure: Read a line of text and return a string",
               SS_nargs,
               _SSI_rd_line, SS_PR_PROC);

    SS_install(si, "set-syntax!",
               "Add interpreter syntax: (set-syntax! <name> <body>)",
               SS_nargs, 
               _SSI_set_syntax, SS_UR_MACRO);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
