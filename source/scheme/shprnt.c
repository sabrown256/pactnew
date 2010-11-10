/*
 * SHPRNT.C - C and Scheme Primitive Output Routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

object
 *SS_histdev,
 *SS_outdev;

char
 SS_prompt[MAXLINE],
 SS_ans_prompt[MAXLINE];

SS_io_logging
 SS_hist_flag = NO_LOG;

void
 (*SS_pr_ch_out)(int, object *);

static void
 _SS_xprintf(object *str, object *argl);

static int
 _SS_push_chars(FILE *fp, char *fmt, ...),
 _SS_push_string(char *s, FILE *fp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_PROMPT - set the interpreter prompt string */

void SS_set_prompt(char *fmt, ...)
   {

    SC_VA_START(fmt);
    SC_VSNPRINTF(SS_prompt, MAXLINE, fmt);
    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SET_ANS_PROMPT - set the answer "prompt" */

void _SS_set_ans_prompt(void)
   {

    snprintf(SS_ans_prompt, MAXLINE, "(%d): ", SS_errlev - 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_DISPLAY_FLAG - return the value of _SS.disp_flag */

int SS_get_display_flag(void)
   {

    return(_SS.disp_flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_DISPLAY_FLAG - set the value of _SS.disp_flag
 *                     - return the old value
 */

int SS_set_display_flag(int flg)
   {int old;

    old           = _SS.disp_flag;
    _SS.disp_flag = flg;

    return(old);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PRINT - takes an object, produces a printable representation, and
 *          - prints it
 */

void SS_print(object *obj, char *begin, char *end, object *strm)
   {FILE *str;

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    str = SS_OUTSTREAM(strm);
    PRINT(str, "%s", begin);

    SS_OBJECT_PRINT(obj, strm);

    PRINT(str, "%s", end);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SS_VDSNPRINTF - so a vsnprintf operation
 *                - copy the result into _SS.vbf for later use
 *                - and return it
 */

char *_SS_vdsnprintf(int cp, char *fmt, va_list __a__)
   {char *s;

    s = SC_vdsnprintf(cp, fmt, __a__);

    SFREE(_SS.vbf);
    _SS.vbf = SC_strsavef(s, "PERM|char*:_SS_VDSNPRINTF:s");

    return(s);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SS_SPRINTF - takes an object, produces a printable representation, and
 *             - leaves it in _SS.vbf for later use
 *             - this is used by _SS_xprintf to realize format specifications
 *             - such as "%10s"
 */

char *_SS_sprintf(char *fmt, object *obj)
   {char *s;
    PFfprintf pr;
    PFfputs ps;

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    if (SS_stringp(obj))
       {s = SS_STRING_TEXT(obj);
	s = SC_strsavef(s, "_SS_SPRINT:s");}
    else
       {SFREE(_SS.vbf);

	SC_get_put_line(pr);
	SC_get_put_string(ps);
	SC_set_put_line(SS_printf);
	SC_set_put_string(SS_fputs);

	SS_OBJECT_PRINT(obj, SS_null);

	SC_set_put_line(pr);
	SC_set_put_string(ps);

	s = SC_strsavef(_SS.vbf, "_SS_SPRINT:s");};

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(s);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SS_SPRINT - takes an object and produces a printable representation */

static object *_SS_sprint(object *obj, char *fmt, object *strm)
   {char *s;
    FILE *str;

    s   = _SS_sprintf(fmt, obj);
    str = SS_OUTSTREAM(strm);

    PRINT(str, fmt, s);

    SFREE(s);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(SS_f);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* DPREG - print all of the SCHEME registers */

void dpreg(void)
   {FILE *str;

    str = SS_OUTSTREAM(SS_outdev);
    PRINT(str, "Scheme registers:\n");

    SS_print(SS_Exn,   "   Exn  : ", "\n", SS_outdev);
    SS_print(SS_Argl,  "   Argl : ", "\n", SS_outdev);
    SS_print(SS_Fun,   "   Fun  : ", "\n", SS_outdev);
    SS_print(SS_Unev,  "   Unev : ", "\n", SS_outdev);
    SS_print(SS_Val,   "   Val  : ", "\n", SS_outdev);
    SS_print(SS_This,  "   This : ", "\n", SS_outdev);
    SS_print(SS_Env,   "   Env  : ", "\n", SS_outdev);
    SS_print(SS_rdobj, "   Read : ", "\n", SS_outdev);
    SS_print(SS_evobj, "   Eval : ", "\n", SS_outdev);

    return;}

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* DPRINT - an easy to use debug time object printer */

void dprint(object *obj)
   {

    if (obj != NULL)
       SS_print(obj, "", "\n", SS_outdev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_WRITE - invoke at Scheme level the C level print function */

static object *_SSI_write(object *obj)
   {object *str;

    str = SS_cdr(obj);
    obj = SS_car(obj);
    if (SS_nullobjp(str))
       SS_print(obj, "", "", SS_outdev);

    else if (SS_consp(str))
       {str = SS_car(str);
        if (SS_outportp(str))
           SS_print(obj, "", "", str);}

    else
       SS_error("LAST ARGUMENT NOT OUTPUT-PORT - WRITE", str);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SPRINTF - a C type sprintf for SCHEME */

static object *_SSI_sprintf(object *argl)
   {int odf;
    PFfprintf pr;
    PFfputs ps;
    object *o;

    SC_get_put_line(pr);
    SC_get_put_string(ps);
    SC_set_put_line(_SS_push_chars);
    SC_set_put_string(_SS_push_string);

    odf = SS_set_display_flag(TRUE);
    _SS_xprintf(SS_outdev, argl);
    SS_set_display_flag(odf);

    SC_set_put_line(pr);
    SC_set_put_string(ps);

    o = SS_mk_string(_SS.bf);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FPRINTF - a C type fprintf for SCHEME */

static object *_SSI_fprintf(object *argl)
   {object *str;
    PFfprintf pr;
    PFfputs ps;

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    str  = SS_car(argl);
    argl = SS_cdr(argl);
    if (SS_nullobjp(str))
       str = SS_outdev;

    SC_get_put_line(pr);
    SC_get_put_string(ps);
    SC_set_put_line(SS_printf);
    SC_set_put_string(SS_fputs);

    _SS_xprintf(str, argl);

    SC_set_put_line(pr);
    SC_set_put_string(ps);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_XPRINTF - interpret C style format specifcation and get values
 *             - from arg list to fulfill them
 *             - return a string with the result
 */

static void _SS_xprintf(object *str, object *argl)
   {int c;
    char forms[MAXLINE], local[MAXLINE], ce;
    char *fmt, *le, *lb, *pt;
    double dv;
    FILE *stream;
    object *obj, *format;

    obj = SS_null;

    if (!SS_outportp(str))
       SS_error("BAD PORT - _SS_XPRINTF", str);
    stream = SS_OUTSTREAM(str);

    format = SS_car(argl);
    if (!SS_stringp(format))
       SS_error("BAD FORMAT - _SS_XPRINTF", format);
    strcpy(forms, SS_STRING_TEXT(format));
    fmt = forms;

    _SS.bf = NULL;
    while (TRUE)
       {for (pt = local; (((c = *fmt++) != '%') && (c != '\0')); pt++)
            {if (c == '\\')
                {switch (c = *fmt++)
                    {case 't' :
		          *pt = '\t';
			  break;
                     case 'r' :
		          *pt = '\r';
			  break;
                     case 'n' :
		          *pt = '\n';
			  break;
                     default :
		          *pt = c;
			  break;};}
             else
                *pt = c;};
        *pt = '\0';
        PRINT(stream, local);

        if (c == '\0')
           break;

/* copy from the % to the type specifier to get the format descriptor for
 * this item
 */
        le = strpbrk(fmt, "sdouxXfeEgGc%");
	ce = (le != NULL) ? *le : '\0';
        local[0] = '%';
        for (lb = &local[1]; le != fmt; *lb++ = *fmt++);
        fmt++;
        *lb++ = ce;
        *lb = '\0';

/* get the object now */
        if (ce != '%')
           {if (SS_nullobjp(argl))
               return;
            argl = SS_cdr(argl);
            if (SS_nullobjp(argl))
               return;
            obj = SS_car(argl);};

/* jump on the type spec to pull the correct arg type off the stack */
        switch (ce)
           {case 's' :
                 _SS.disp_flag = _SS.disp_flag_ext;
	         if (strlen(local) == 2)
		    SS_print(obj, "", "", str);
                 else
		    _SS_sprint(obj, local, str);
/*                 _SS.disp_flag = FALSE; */
                 break;

            case 'c' :
                 if (SS_integerp(obj))
                    PRINT(stream, local, SS_INTEGER_VALUE(obj));
                 else
                    {/* _SS.disp_flag = _SS.disp_flag_ext; */
                     SS_print(obj, "", "", str);
                     /* _SS.disp_flag = FALSE; */};
                 break;

            case 'i' :
            case 'X' :
            case 'x' :
            case 'o' :
            case 'd' :
            case 'u' :
                 if (!SS_integerp(obj))
                    SS_error("NON-INTEGER FOR INTEGER FIELD - _SS_XPRINTF",
                             obj);
                 PRINT(stream, local, (long) SS_INTEGER_VALUE(obj));
                 break;

            case 'f' : 
            case 'e' :
            case 'E' : 
            case 'g' : 
            case 'G' :
	         dv = 0.0;
                 if (SS_integerp(obj))
		    dv = SS_INTEGER_VALUE(obj);
                 else if (SS_floatp(obj))
		    dv = SS_FLOAT_VALUE(obj);
		 else
                    SS_error("NON-NUMERIC VALUE FOR REAL FIELD - _SS_XPRINTF",
                             obj);
                 PRINT(stream, local, dv);
                 break;

            case '%' :
                 PRINT(stream, "%%");
                 break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PUSH_CHARS - do an sprintf on the tailing arg and push
 *                - them onto the first arg resizing if neceassary
 *                - NOTE: fp NULL should be fine, in fact it is crucial
 *                - to make formatted output specifications such as
 *                - "%10s" work
 */

static int _SS_push_chars(FILE *fp, char *fmt, ...)
   {int ns, nb;
    char *bf;

    SC_VDSNPRINTF(FALSE, bf, fmt);

    if (_SS.bf == NULL)
       {_SS.bf = FMAKE_N(char, MAXLINE, "_SS_PUSH_CHARS:_SS.bf");
	_SS.bf[0] = '\0';};

    nb = strlen(_SS.bf);
    ns = strlen(bf);
    if ((ns + nb + 2) >= SC_arrlen(_SS.bf))
       REMAKE_N(_SS.bf, char, ns+nb+MAXLINE);

    strcat(_SS.bf, bf);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PUSH_STRING - do a string copy of S into the buffer
 *                 - NOTE: cannot use _SS_push_char because S
 *                 - may exceed the static buffer size
 */

static int _SS_push_string(char *s, FILE *fp)
   {int ns, nb;

    if (_SS.bf == NULL)
       {_SS.bf = FMAKE_N(char, MAXLINE, "_SS_PUSH_STRING:_SS.bf");
	_SS.bf[0] = '\0';};

    nb = strlen(_SS.bf);
    ns = strlen(s);
    if ((ns + nb + 2) >= SC_arrlen(_SS.bf))
       REMAKE_N(_SS.bf, char, ns+nb+MAXLINE);

    if (_SS.disp_flag == TRUE)
       strcat(_SS.bf, s);
    else
       {strcat(_SS.bf, "\"");
	strcat(_SS.bf, s);
        strcat(_SS.bf, "\"");};

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PUTS - C level string print routine */

int SS_puts(char *s, FILE *fp, PFfputs put)
   {int rv;

    if (_SS.disp_flag == TRUE)
       rv = (*put)(s, fp);

    else
       {(*put)("\"", fp);
	rv = (*put)(s, fp);
	(*put)("\"", fp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_DISPLAY - the human readable print function in Scheme */

static object *_SSI_display(object *obj)
   {PFfprintf pr;
    PFfputs ps;

    SC_get_put_line(pr);
    SC_get_put_string(ps);
    SC_set_put_line(SS_printf);
    SC_set_put_string(SS_fputs);

    _SS.disp_flag = _SS.disp_flag_ext;
    _SSI_write(obj);
    _SS.disp_flag = FALSE;

    SC_set_put_line(pr);
    SC_set_put_string(ps);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PRINT_TOGGLE - toggle printing of values in Scheme */

static object *_SSI_print_toggle(void)
   {object *rv;

    SS_print_flag = !SS_print_flag;

    rv = (SS_print_flag) ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STATS_TOGGLE - toggle printing of statistics in Scheme */

static object *_SSI_stats_toggle(void)
   {object *rv;

    SS_stat_flag = !SS_stat_flag;
    rv = (SS_stat_flag) ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_TRANS_ON - turn on a transcript of the Scheme session */

static object *_SSI_trans_on(object *obj)
   {FILE *str;
    char *s;

    s = NULL;
    if (!SS_nullobjp(SS_histdev))
       SS_error("TRANSCRIPT ALREADY ACTIVE - TRANSCRIPT-ON", obj);

    s = NULL;
    SS_args(obj,
	    SC_STRING_I, &s,
	    0);

    str = io_open(s, "a");
    if (str == NULL)
       SS_error("CAN'T OPEN FILE - TRANSCRIPT-ON", obj);

    SS_histdev = SS_mk_outport(str, s);
    SS_hist_flag = ALL;

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_TRANS_OFF - turn off the transcript of the Scheme session */

object *SS_trans_off(void)
   {

    if (SS_nullobjp(SS_histdev))
       return(SS_f);

    io_close(SS_OUTSTREAM(SS_histdev));

    SS_OBJECT_FREE(SS_histdev);
    SS_histdev   = SS_null;
    SS_hist_flag = NO_LOG;

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_OPN_OUT - open-output-file in Scheme */

static object *_SSI_opn_out(object *obj)
   {FILE *str;
    char *s;
    object *prt;

    s = NULL;
    SS_args(obj,
	    SC_STRING_I, &s,
	    0);

    str = io_open(s, "w");
    if (str == NULL)
       SS_error("CAN'T OPEN FILE - OPEN-OUTPUT-FILE", obj);

    prt = SS_mk_outport(str, s);

    SFREE(s);

    return(prt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CLS_OUT - close-output-file in Scheme */

static object *_SSI_cls_out(object *obj)
   {

    if (!SS_outportp(obj))
       SS_error("BAD OUTPUT-PORT TO CLOSE-OUTPUT-FILE", obj);

    io_close(SS_OUTSTREAM(obj));

    return(SS_t);}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SSI_CALL_OF - call-with-output-file in Scheme */

object *_SSI_call_of(object *argl)
   {FILE *str;
    char *s;
    object *obj, *old_outdev, *ret;

    s   = NULL;
    obj = SS_car(argl);

    s = NULL;
    SS_args(obj,
	    SC_STRING_I, &s,
	    0);

    str = io_open(s, BINARY_MODE_W);
    if (str == NULL)
       SS_error("CAN'T OPEN FILE - CALL-WITH-OUTPUT-FILE", obj);

    old_outdev = SS_outdev;
    SS_outdev  = SS_mk_outport(str, s);
    ret        = SS_exp_eval(SS_cdr(argl));
    _SSI_cls_out(SS_outdev);
    SS_outdev = old_outdev;

    SFREE(s);

    return(ret);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SSI_OPORTP - output-port? at Scheme level */

static object *_SSI_oportp(object *obj)
   {object *o;

    o = SS_outportp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_WR_PROC - print a procedure */

void SS_wr_proc(object *obj, object *strm)
   {FILE *str;
    char *s;

    str = SS_OUTSTREAM(strm);

    s = SS_PROCEDURE_NAME(obj);
    if (s != NULL)
       {switch (SS_PROCEDURE_TYPE(obj))
           {case SS_ESC_PROC :
	         PRINT(str, "%s(cont=%d, stack=%d)",
		       s,
		       SS_ESCAPE_CONTINUATION(obj),
		       SS_ESCAPE_STACK(obj));
		 break;
            default :
	         PRINT(str, "%s", s);
		 break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BANNER - print the banner */

object *SS_banner(object *obj)
   {char *s;

    SS_args(obj,
	    SC_STRING_I, &s,
	    SC_INT_I, &_SS.override,
	    0);

    if (_SS.override)
       {PRINT(stdout, "\n\t%s - %s\n", s, VERSION);
	_SS.override = 0;};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PRIM_DES - primitive part of describe facility */

int SS_prim_des(object *strm, object *obj)
   {int fmt;
    char *s;
    object *desc, *bdy;
    FILE *str;

    str = SS_OUTSTREAM(strm);

    fmt = (SS_bound_name("format-expr") != SS_null);

/* take anything that will give a name - procedure, string, variable ... */
    obj = SS_lookup_object(obj);

    if (!SS_procedurep(obj))
       {if (SS_variablep(obj))
           {desc = SS_lk_var_val(obj, SS_Env);
            if (SS_procedurep(desc))
               obj = desc;
	    else
               {PRINT(str, "     Variable: %s\n", SS_VARIABLE_NAME(obj));
                SS_print(desc, "     Value: ", "\n", strm);
                return(TRUE);};}
        else
           return(FALSE);};

    switch (SS_PROCEDURE_TYPE(obj))
       {case SS_PROC :
             bdy  = SS_proc_body(obj);
             desc = SS_CAR_MACRO(bdy);
             if (SS_stringp(desc))
                PRINT(str, "     %s", SS_STRING_TEXT(desc));
             else
                {desc = SS_mk_cons(obj, bdy);
                 SS_MARK(desc);
                 PRINT(str, "     Compound procedure:\n");
		 bdy = SS_make_form(SS_quoteproc, bdy, LAST);
		 if (fmt == TRUE)
		    SS_call_scheme("format-expr",
				   SS_OBJECT_I, strm,
				   SS_OBJECT_I, bdy,
				   SC_STRING_I, "     ",
				   0);
		 else
		    SS_wr_lst(desc, strm);
                 SS_GC(desc);};
             break;

        case SS_MACRO :
             bdy  = SS_proc_body(obj);
             desc = SS_CAR_MACRO(bdy);
             if (SS_stringp(desc))
                PRINT(str, "     %s", SS_STRING_TEXT(desc));
             else
                {desc = SS_mk_cons(obj, bdy);
                 SS_MARK(desc);
                 PRINT(str, "     Compound macro:\n");
		 bdy = SS_make_form(SS_quoteproc, bdy, LAST);
		 if (fmt == TRUE)
		    SS_call_scheme("format-expr",
				   SS_OBJECT_I, strm,
				   SS_OBJECT_I, bdy,
				   SC_STRING_I, "     ",
				   0);
		 else
		    SS_wr_lst(desc, strm);
                 SS_GC(desc);};
             break;

        case SS_PR_PROC  : 
        case SS_EE_MACRO :
        case SS_UR_MACRO :
        case SS_UE_MACRO :
        default :
             s = SS_PROCEDURE_DOC(obj);
             if (s != NULL)
                PRINT(str, "     %s", s);};

    PRINT(str, "\n");

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_DESCRIBE - print out the documentation on the function */

static object *_SSI_describe(object *argl)
   {int ok;
    object *obj, *strm;

    obj  = SS_null;
    strm = SS_null;
    SS_args(argl,
	    SS_OBJECT_I, &obj,
	    SS_OBJECT_I, &strm,
	    0);

    if (SS_nullobjp(strm))
       strm = SS_outdev;

    if (!SS_outportp(strm))
       SS_error("LAST ARGUMENT NOT OUTPUT-PORT - DESCRIBE", strm);

    ok = SS_prim_des(strm, obj);
/*
    if (ok == FALSE)
       SS_error("DESCRIPTIONS ONLY AVAIBLE FOR PROCEDURES", obj);
*/
    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_APR_PROC - do apropos output for a procedure */

static void _SS_apr_proc(FILE *str, char *s, object *op,
			 int *pnmore, int *pflag)
   {int c;
    char *name, *doc;

    doc  = SS_PROCEDURE_DOC(op);
    name = SS_PROCEDURE_NAME(op);
    c    = SS_PROCEDURE_TYPE(op);
    if ((c == SS_PR_PROC)  || (c == SS_UR_MACRO) ||
	(c == SS_UE_MACRO) || (c == SS_EE_MACRO))
       {if ((SC_strstri(doc, s) != NULL) || (SC_strstri(name, s) != NULL))
	   {PRINT(str, "%s :\n%s\n\n", name, doc);
	    *pnmore += 3;
	    *pflag   = 1;};}

    else if (c == SS_PROC)
       {if (SC_strstri(name, s) != NULL)
	   {PRINT(str, "%s : Compound procedure\n\n", name);
	    *pnmore += 2;
	    *pflag   = 1;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PRIM_APR - finds all commands which contain the substring s in their 
 *              - name or description
 */

static int _SS_prim_apr(FILE *str, char *s, hasharr *tab)
   {int flag, nlp, nmore, vcnt;
    long i;
    char bf[10];
    char *name;
    object *op, *obj;
        
    flag  = 0;
    vcnt  = 0;
    nmore = 0;
    nlp   = SS_lines_page;
    if (nlp == 0)
       nlp = INT_MAX;

    for (i = 0; SC_hasharr_next(tab, &i, &name, NULL, (void **) &obj); i++)
        {if (nmore > nlp)
	    {PRINT(str, "More ... (n to stop)");
	     GETLN(bf, 10, stdin);
	     if (bf[0] == 'n')
	        return(flag);

	     nmore = 0;};

/* compound procedures */
	 if (SS_procedurep(obj))
	    _SS_apr_proc(str, s, obj, &nmore, &flag);

	 else if (SS_variablep(obj))
	    {op = SS_VARIABLE_VALUE(obj);

/* procedures named by a variable */
	     if (SS_procedurep(op))
	        _SS_apr_proc(str, s, op, &nmore, &flag);

/* variables with non-null values */
	     else if ((SC_strstri(name, s) != NULL) && (op != SS_null))
	        {PRINT(str, "%s : Variable\n\n", name);
		 nmore += 2;
		 flag   = 1;}

/* variables whose names match the patter exactly - prevent useless repeats */
	     else if (strcmp(name, s) == 0)
	        {if (vcnt < 1)
		    {PRINT(str, "%s : Variable\n\n", name);
		     nmore += 2;
		     flag   = 1;};
		 vcnt++;};};};

    return(flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PRIM_APR - finds all commands which contain the substring s in their 
 *             - name or description
 */

int SS_prim_apr(FILE *str, char *s)
   {int i, flag;
    hasharr *tab;
    object *penv, *l, *v;

    flag = _SS_prim_apr(str, s, SS_symtab);

    penv = SS_Env;
    for (i = 0; !SS_nullobjp(penv); penv = SS_cdr(penv), i++)
        {l   = SS_car(penv);
	 v   = SS_car(l);
	 tab = SS_GET(hasharr, SS_cadr(l));
	 flag |= _SS_prim_apr(str, s, tab);};

    return(flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_APROPOS - search the hash table for information about the given
 *              - string
 */

static object *_SSI_apropos(object *argl)
   {object *obj, *strm;
    char *token;
    FILE *str;

    obj  = SS_null;
    strm = SS_null;
    SS_args(argl,
	    SS_OBJECT_I, &obj,
	    SS_OBJECT_I, &strm,
	    0);

    if (SS_nullobjp(strm))
       strm = SS_outdev;

    if (!SS_outportp(strm))
       SS_error("LAST ARGUMENT NOT OUTPUT-PORT - DESCRIBE", strm);

    str   = SS_OUTSTREAM(strm);
    token = NULL;

    if (SS_stringp(obj))
       token = SS_STRING_TEXT(obj);

    else if (SS_variablep(obj))
       token = SS_VARIABLE_NAME(obj);

    else if (SS_procedurep(obj))
       token = SS_PROCEDURE_NAME(obj);

    else
       SS_error("BAD OBJECT - APROPOS", obj);

    PRINT(str, "\nApropos search string: %s\n\n", token);
    if (!SS_prim_apr(str, token))
       PRINT(str, "No documentation on %s\n\n", token);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_WR_LST - print a list */

void SS_wr_lst(object *obj, object *strm)
   {FILE *str;
    object *cd;

    str = SS_OUTSTREAM(strm);
    PRINT(str, "(");

    while (TRUE)
       {SS_print(SS_car(obj), "", "", strm);

        if (SS_nullobjp(cd = SS_cdr(obj)))
           {PRINT(str, ")");
            break;}
        else if (SS_consp(cd))
           {obj = cd;
            PRINT(str, " ");}
        else
           {SS_print(cd, " . ", ")", strm);
            break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_WR_ATM - print an atom or string */

void SS_wr_atm(object *obj, object *strm)
   {int ityp;
    char t[MAXLINE];
    char *s;
    FILE *str;

    str  = SS_OUTSTREAM(strm);
    ityp = SS_OBJECT_TYPE(obj);

/* use PUTS for strings to get double quotes right */
    if (ityp == SC_STRING_I)
       {s = SS_STRING_TEXT(obj);
	PUTS(s, str);}

/* everyone else can use PRINT
 * NOTE: use sprintf then PRINT rather than direct PRINT to get
 * around SUN compiler bug regarding va_args
 */
    else
       {s = t;

	if (ityp == SC_INT_I)
#ifdef NO_LONG_LONG
	   snprintf(t, MAXLINE, "%ld", SS_INTEGER_VALUE(obj));
#else
	   snprintf(t, MAXLINE, "%lld", SS_INTEGER_VALUE(obj));
#endif

	else if (ityp == SC_FLOAT_I)
	   snprintf(t, MAXLINE, "%g", SS_FLOAT_VALUE(obj));

#ifdef LARGE
	else if (ityp == SS_CHARACTER_I)
	   snprintf(t, MAXLINE, "%c", SS_CHARACTER_VALUE(obj));
#endif

	else if (ityp == SS_VARIABLE_I)
	   s = SS_VARIABLE_NAME(obj);

	else if (ityp == SS_NULL_I)
	   strcpy(t, "()");

	else if ((ityp == SS_EOF_I) || (ityp == SS_BOOLEAN_I))
	   snprintf(t, MAXLINE, "%s", SS_BOOLEAN_NAME(obj));

	else
	   snprintf(t, MAXLINE, "UNKOWN OR NON-ATOMIC OBJECT TYPE");

	PRINT(str, "%s", s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_NEWLINE - Scheme newline function */

static object *_SSI_newline(object *strm)
   {FILE *fp;
   
    if (SS_nullobjp(strm))
       {fp = SS_OUTSTREAM(SS_outdev);
        if (fp == stdout)
           PRINT(fp,  "\n");
        else
           PRINT(fp,  "\r\n");}

    else if (SS_consp(strm))
       {strm = SS_car(strm);
        if (SS_outportp(strm))
           {fp = SS_OUTSTREAM(strm);
            if (fp == stdout)
               PRINT(fp,  "\n");
            else
               PRINT(fp,  "\r\n");};}

    else
       SS_error("BAD OUTPUT PORT - NEWLINE", strm);

    return(SS_f);}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SSI_CURR_OP - current-output-port in Scheme */

object *_SSI_curr_op(void)
    {

     return(SS_outdev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_WR_CHR - write-char for Scheme */

object *_SSI_wr_chr(object *argl)
   {object *str;

    str = SS_cdr(argl);
    if (!SS_charobjp(argl = SS_car(argl)))
       SS_error("BAD CHARACTER - WRITE-CHAR", argl);

    if (SS_nullobjp(str))
       SS_print(argl, "", "", SS_outdev);

    else if (SS_outportp(str = SS_car(str)))
       SS_print(argl, "", "", str);

    else
       SS_error("LAST ARGUMENT NOT OUTPUT-PORT - WRITE-CHAR", str);

    return(SS_f);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SS_INST_PRINT - install the Scheme output primitives */

void _SS_inst_print(void)
   {

    SS_install_cf("suppress-quote", 
		  "Variable: Suppress display of double quotes iff on\n     Usage: suppress-quote [0|1]",
		  SS_acc_int,
		  &_SS.disp_flag_ext);

    SS_install("apropos",
               "Procedure: searches the symbol table for documentation",
               SS_nargs,
               _SSI_apropos, SS_PR_PROC);

    SS_install("banner",
               "Procedure: print the code system banner",
               SS_nargs,
               SS_banner, SS_PR_PROC);

    SS_install("close-output-file",
               "Procedure: Close the specified output port and release the OUT_PORT object",
               SS_sargs,
               _SSI_cls_out, SS_PR_PROC);

    SS_install("describe",
               "Procedure: prints the documentation for a procedure to the specified device",
               SS_nargs,
               _SSI_describe, SS_PR_PROC);

    SS_install("display",
               "Procedure: prints an object to the specified device in human readable form",
               SS_nargs,
               _SSI_display, SS_PR_PROC);

    SS_install("newline",
               "Procedure: print a <CR><LF> or equivalent to the specified device",
               SS_nargs,
               _SSI_newline, SS_PR_PROC);

    SS_install("open-output-file",
               "Procedure: Open the specified file for output and return an OUT_PORT object",
               SS_sargs,
               _SSI_opn_out, SS_PR_PROC);

    SS_install("output-port?",
               "Returns #t if the object is an output port (OUT_PORT), and #f otherwise",
               SS_sargs, 
               _SSI_oportp, SS_PR_PROC);

    SS_install("printf",
               "C-like formatted print function: (printf port format . rest)",
               SS_nargs, 
               _SSI_fprintf, SS_PR_PROC);

    SS_install("print-toggle",
               "Procedure: Toggle printing of values",
               SS_zargs,
               _SSI_print_toggle, SS_PR_PROC);

    SS_install("sprintf",
               "C style sprintf string builder function: (sprintf format . rest)",
               SS_nargs, 
               _SSI_sprintf, SS_PR_PROC);

    SS_install("stats-toggle",
               "Procedure: Toggle printing of statistics",
               SS_zargs,
               _SSI_stats_toggle, SS_PR_PROC);

    SS_install("transcript-off",
               "Procedure: close the transcript file to stop recording a Scheme session",
               SS_zargs,
               SS_trans_off, SS_UR_MACRO);

    SS_install("transcript-on",
               "Procedure: open the specified transcript file to start recording a Scheme session",
               SS_sargs,
               _SSI_trans_on, SS_PR_PROC);

    SS_install("write",
               "Procedure: puts the printed representation of an object to the specified device",
               SS_nargs,
               _SSI_write, SS_PR_PROC);

#ifdef LARGE

    SS_install("call-with-output-file",
               "Procedure: open the named file and eval a procedure using the port for output",
               SS_nargs,
               _SSI_call_of, SS_PR_PROC);

    SS_install("current-output-port",
               "Procedure: Returns the current default output port",
               SS_zargs,
               _SSI_curr_op, SS_PR_PROC);

    SS_install("write-char",
               "Procedure: Write a single character to the specified port",
               SS_nargs,
               _SSI_wr_chr, SS_PR_PROC);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
