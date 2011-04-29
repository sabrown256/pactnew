/*
 * SHSTRG.C - string functions for the LARGE Scheme system
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_STRCMP - Scheme version of strcmp */

static object *_SS_strcmp(object *argl, int (*func)(char *s1, char *s2))
   {int cmp;
    char *s1, *s2;
    object *rv;

    s1 = NULL;
    s2 = NULL;
    SS_args(argl,
            SC_STRING_I, &s1,
            SC_STRING_I, &s2,
            0);

    cmp = func(s1, s2);
    rv  = (cmp == TRUE) ? SS_t : SS_f;

    CFREE(s1);
    CFREE(s2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CISTRCMP - case insensitive Scheme version of strcmp */

static object *_SS_cistrcmp(object *argl, int (*func)(char *s1, char *s2))
   {int cmp;
    char *s1, *s2;
    object *rv;

    s1 = NULL;
    s2 = NULL;
    SS_args(argl,
            SC_STRING_I, &s1,
            SC_STRING_I, &s2,
            0);

    SC_str_lower(s1);
    SC_str_lower(s2);

    cmp = func(s1, s2);
    rv  = (cmp == TRUE) ? SS_t : SS_f;

    CFREE(s1);
    CFREE(s2);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              C LEVEL PRIMITIVES                          */

/*--------------------------------------------------------------------------*/

/* _SS_STREQ - string=? at C level */

static int _SS_streq(char *s1, char *s2)
   {int rv;

    rv = (strcmp(s1, s2) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_STRGE - string>=? at C level */

static int _SS_strge(char *s1, char *s2)
   {int rv;

    rv = (strcmp(s1, s2) >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_STRGT - string>? at C level */

static int _SS_strgt(char *s1, char *s2)
   {int rv;

    rv = (strcmp(s1, s2) > 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_STRLE - string<=? at C level */

static int _SS_strle(char *s1, char *s2)
   {int rv;

    rv = (strcmp(s1, s2) <= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_STRLT - string<? at C level */

static int _SS_strlt(char *s1, char *s2)
   {int rv;

    rv = (strcmp(s1, s2) < 0);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            SCHEME LEVEL PRIMITIVES                       */

/*--------------------------------------------------------------------------*/

/* _SSI_STREQ - string=? in Scheme */

static object *_SSI_streq(SS_psides *si, object *argl)
   {object *o;

    o = _SS_strcmp(argl, _SS_streq);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRGE - string>=? in Scheme */

static object *_SSI_strge(SS_psides *si, object *argl)
   {object *o;

    o = _SS_strcmp(argl, _SS_strge);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRGT - string>? in Scheme */

static object *_SSI_strgt(SS_psides *si, object *argl)
   {object *o;

    o = _SS_strcmp(argl, _SS_strgt);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRLE - string<=? in Scheme */

static object *_SSI_strle(SS_psides *si, object *argl)
   {object *o;

    o = _SS_strcmp(argl, _SS_strle);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRLT - string<? in Scheme */

static object *_SSI_strlt(SS_psides *si, object *argl)
   {object *o;

    o = _SS_strcmp(argl, _SS_strlt);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CISTREQ - string-ci=? in Scheme */

static object *_SSI_cistreq(SS_psides *si, object *argl)
   {object *o;

    o = _SS_cistrcmp(argl, _SS_streq);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CISTRGE - string-ci>=? in Scheme */

static object *_SSI_cistrge(SS_psides *si, object *argl)
   {object *o;

    o = _SS_cistrcmp(argl, _SS_strge);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CISTRGT - string-ci>? in Scheme */

static object *_SSI_cistrgt(SS_psides *si, object *argl)
   {object *o;

    o = _SS_cistrcmp(argl, _SS_strgt);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CISTRLE - string-ci<=? in Scheme */

static object *_SSI_cistrle(SS_psides *si, object *argl)
   {object *o;

    o = _SS_cistrcmp(argl, _SS_strle);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CISTRLT - string-ci<? in Scheme */

static object *_SSI_cistrlt(SS_psides *si, object *argl)
   {object *o;

    o = _SS_cistrcmp(argl, _SS_strlt);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRLEN - string-length in Scheme */

static object *_SSI_strlen(SS_psides *si, object *str)
   {int64_t ln;
    object *rv;

    if (!SS_stringp(str))
       SS_error("ARGUMENT NOT STRING - STRING-LENGTH", str);

/* when tokenizing strings this definition makes more sense */
    ln = strlen(SS_STRING_TEXT(str));
    rv = SS_mk_integer(ln);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRREF - string-ref in Scheme */

static object *_SSI_strref(SS_psides *si, object *argl)
   {int c, n;
    char *s;
    object *rv;

    s = NULL;
    n = 0;
    SS_args(argl,
            SC_STRING_I, &s,
            SC_INT_I, &n,
            0);

    if (n > strlen(s))
       SS_error("INDEX PAST END OF STRING - STRING-REF", argl);

    c  = s[n];
    rv = SS_mk_char(c);

    CFREE(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRCPY - string-copy in Scheme */

static object *_SSI_strcpy(SS_psides *si, object *argl)
   {object *str;
    char *s;

    s = NULL;
    SS_args(argl,
            SC_STRING_I, &s,
            0);

    str = SS_mk_string(s);

    CFREE(s);

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRFIL - string-fill! in Scheme */

static object *_SSI_strfil(SS_psides *si, object *argl)
   {int c;
    char *s;
    object *str;

    s = NULL;
    c = ' ';
    SS_args(argl,
            SS_OBJECT_I, &str,
            SC_INT_I, &c,
            0);

    s = SS_STRING_TEXT(str);
    memset(s, c, strlen(s));

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRSET - string-set! in Scheme */

static object *_SSI_strset(SS_psides *si, object *argl)
   {int c, n;
    char *s;
    object *str;

    s = NULL;
    n = 0;
    c = ' ';
    SS_args(argl,
            SS_OBJECT_I, &str,
            SC_INT_I, &n,
            SC_INT_I, &c,
            0);

    s = SS_STRING_TEXT(str);
    s[n] = c;

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRSUB - substring in Scheme */

static object *_SSI_strsub(SS_psides *si, object *argl)
   {int n, n1, n2;
    char *s;
    object *str;

    s  = NULL;
    n1 = 0;
    n2 = 0;
    SS_args(argl,
            SC_STRING_I, &s,
            SC_INT_I, &n1,
            SC_INT_I, &n2,
            0);

    n = strlen(s);
    if ((n1 > n) || (n1 < 0))
       n1 = 0;

    if ((n2 > n) || (n2 < 0))
       n2 = n;

    s[n2] = '\0';
    str   = SS_mk_string(&s[n1]);

    CFREE(s);

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRING - string for Scheme */

static object *_SSI_string(SS_psides *si, object *argl)
   {int i, c, n;
    char *t;
    object *str;

    n = SS_length(argl);
    t = CMAKE_N(char, n+2);
    for (i = 0; i < n; i++, argl = SS_cdr(argl))
        {str = SS_car(argl);
         if (SS_charobjp(str))
            {c = SS_CHARACTER_VALUE(str);
	     t[i] = c;};};

    t[i] = '\0';
    str  = SS_mk_string(t);
    CFREE(t);

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRAPP - string-append for Scheme */

static object *_SSI_strapp(SS_psides *si, object *argl)
   {int n;
    char *s, *t;
    object *ths;

    t = CMAKE(char);
    *t = '\0';
    for (n = 1; SS_consp(argl); argl = SS_cdr(argl))
        {ths = SS_car(argl);
         if (SS_stringp(ths))
            {n += SS_STRING_LENGTH(ths);
             s  = CMAKE_N(char, n);
             strcpy(s, t);
             SC_strcat(s, n, SS_STRING_TEXT(ths));
             CFREE(t);
             t = s;};};

    ths = SS_mk_string(t);
    CFREE(t);

    return(ths);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRLST - string->list in Scheme */

static object *_SSI_strlst(SS_psides *si, object *str)
   {int i, n;
    char *s;
    object *ret, *o;

    if (!SS_stringp(str))
       SS_error("ARGUMENT NOT STRING - STRING->LIST", str);

    s = SS_STRING_TEXT(str);
    n = SS_STRING_LENGTH(str);
    for (ret = SS_null, i = 0; i < n; i++)
        ret = SS_mk_cons(SS_mk_char(s[i]), ret);

    o = SS_reverse(ret);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LSTSTR - list->string for Scheme */

static object *_SSI_lststr(SS_psides *si, object *argl)
   {int i, n;
    char *s;
    object *str, *ths;

    argl = SS_car(argl);
    if (!SS_consp(argl))
       SS_error("ARGUMENT MUST BE LIST - LIST->STRING", argl);

    n = SS_length(argl);
    s = CMAKE_N(char, n+1);
    for (i = 0; SS_consp(argl); argl = SS_cdr(argl))
        {ths = SS_car(argl);
         if (SS_charobjp(ths))
            s[i++] = (char) SS_CHARACTER_VALUE(ths);};
    s[i] = '\0';

    str = SS_mk_string(s);
    CFREE(s);

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SYMSTR - symbol->string for Scheme */

static object *_SSI_symstr(SS_psides *si, object *arg)
   {char *s;
    object *rv;

    if (!SS_variablep(arg))
       SS_error("ARGUMENT NOT VARIABLE - SYMBOL->STRING", arg);

    s  = SS_VARIABLE_NAME(arg);
    rv = SS_mk_string(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRSYM - string->symbol for Scheme */

static object *_SSI_strsym(SS_psides *si, object *str)
   {char *s;
    object *rv;

    if (!SS_stringp(str))
       SS_error("ARGUMENT NOT STRING - STRING->SYMBOL", str);

    s  = SS_STRING_TEXT(str);
    rv = SS_mk_variable(s, SS_null);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_UPCASE - up-case for Scheme */

static object *_SSI_upcase(SS_psides *si, object *str)
   {char *s;
    object *rv;

    if (SS_stringp(str))
       {s  = SS_STRING_TEXT(str);
	s  = SC_str_upper(s);
	rv = SS_mk_string(s);}
    else
       rv = SS_null;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_DNCASE - down-case for Scheme */

static object *_SSI_dncase(SS_psides *si, object *str)
   {char *s;
    object *rv;

    if (SS_stringp(str))
       {s  = SS_STRING_TEXT(str);
	s  = SC_str_lower(s);
	rv = SS_mk_string(s);}
    else
       rv = SS_null;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MK_STR - make a new string */

static object *_SSI_mk_str(SS_psides *si, object *argl)
   {int n, c;
    char *s;
    object *o;

    n = 0;
    c = ' ';
    SS_args(argl,
            SC_INT_I, &n,
            SC_INT_I, &c,
            0);

    s = CMAKE_N(char, n+2);

    memset(s, c, n);
    s[n] = '\0';

    o = SS_mk_string(s);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRNUM - return the number represented in the given string */

static object *_SSI_strnum(SS_psides *si, object *argl)
   {int64_t l;
    double r;
    char *text, *pt;
    object *rv;

    text  = NULL;
    SS_args(argl,
            SC_STRING_I, &text,
            0);

    rv = SS_f;
    if (SC_intstrp(text, SC_gs.radix))
       {l  = STRTOL(text, &pt, SC_gs.radix);
	rv = SS_mk_integer(l);}

    else if (SC_fltstrp(text))
       {r  = ATOF(text);
	rv = SS_mk_float(r);};

    CFREE(text);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRCHR - do what the C standard library function strchr does */

static object *_SSI_strchr(SS_psides *si, object *argl)
   {char *text, *delim, *s;
    object *rv;

    text  = NULL;
    delim = NULL;
    SS_args(argl,
            SC_STRING_I, &text,
            SC_STRING_I, &delim,
            0);

    s  = strchr(text, (int) delim[0]);
    rv = (s == NULL) ? SS_null : SS_mk_string(s);

    CFREE(text);
    CFREE(delim);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRSTR - do what the C standard library function strstr does */

static object *_SSI_strstr(SS_psides *si, object *argl)
   {char *cs, *ct, *s;
    object *rv;

    cs = NULL;
    ct = NULL;
    SS_args(argl,
            SC_STRING_I, &cs,
            SC_STRING_I, &ct,
            0);

    s  = strstr(cs, ct);
    rv = (s == NULL) ? SS_null : SS_mk_string(s);

    CFREE(cs);
    CFREE(ct);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRCASESTR - do what the C standard library function strcasestr does */

static object *_SSI_strcasestr(SS_psides *si, object *argl)
   {char *cs, *ct, *s;
    object *rv;

    cs = NULL;
    ct = NULL;
    SS_args(argl,
            SC_STRING_I, &cs,
            SC_STRING_I, &ct,
            0);

    cs = SC_str_lower(cs);
    ct = SC_str_lower(ct);

    s  = strstr(cs, ct);
    rv = (s == NULL) ? SS_null : SS_mk_string(s);

    CFREE(cs);
    CFREE(ct);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ISTRCHR - do what the C standard library function strchr does
 *              - except return the index into the original string
 *              - rather than the string which _SSI_strchr would
 */

static object *_SSI_istrchr(SS_psides *si, object *argl)
   {int64_t ind;
    char *text, *delim, *s;
    object *rv;

    text  = NULL;
    delim = NULL;
    SS_args(argl,
            SC_STRING_I, &text,
            SC_STRING_I, &delim,
            0);

    s = strchr(text, (int) delim[0]);
    if (s == NULL)
       ind = strlen(text);
    else
       ind = s - text;

    rv = SS_mk_integer(ind);

    CFREE(text);
    CFREE(delim);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ISTRSTR - do what the C standard library function strstr does */

static object *_SSI_istrstr(SS_psides *si, object *argl)
   {int64_t ind;
    char *cs, *ct, *s;
    object *rv;

    cs = NULL;
    ct = NULL;
    SS_args(argl,
            SC_STRING_I, &cs,
            SC_STRING_I, &ct,
            0);

    s = strstr(cs, ct);
    if (s == NULL)
       ind = strlen(cs);
    else
       ind = s - cs;

    rv = SS_mk_integer(ind);

    CFREE(cs);
    CFREE(ct);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_BLANKSTR - wrapper for SCORE */

static object *_SSI_blankstr(SS_psides *si, object *argl)
   {int ok;
    char *cs, *ct;
    object *rv;

    cs = NULL;
    ct = "#";
    SS_args(argl,
            SC_STRING_I, &cs,
            SC_STRING_I, &ct,
            0);

    ok = SC_blankp(cs, ct);

    rv = (ok == TRUE) ? SS_t : SS_f;

    CFREE(cs);
    CFREE(ct);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_TRIM - wrapper for SCORE trim functions */

static object *_SSI_trim(SS_psides *si, object *argl)
   {int dir;
    char *s, *d, *r;
    object *rv;

    s   = NULL;
    d   = NULL;
    dir = -1;
    SS_args(argl,
            SC_STRING_I, &s,
            SC_STRING_I, &d,
            SC_INT_I, &dir,
            0);

    if (dir < 0)
       r = SC_trim_left(s, d);
    else if (dir == 0)
       r = SC_trim_right(SC_trim_left(s, d), d);
    else
       r = SC_trim_right(s, d);

    rv = SS_mk_string(r);

    CFREE(s);
    CFREE(d);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_STRTOK - tokenize the string via FUNC
 *            - if the optional third arg is #t then use a copy of the
 *            - string (this is useful for quick one shot token grabbing)
 */

static object *_SS_strtok(object *argl, char *(*fnc)(char *, char *))
   {int c;
    char *text, *delim, t[MAXLINE], d[MAXLINE], *ps, *pt;
    object *obj, *flag, *rv, *str;

    str   = SS_null;
    delim = NULL;
    flag  = SS_f;
    SS_args(argl,
            SS_OBJECT_I, &str,
            SC_STRING_I, &delim,
            SS_OBJECT_I, &flag,
            0);

    text = SS_STRING_TEXT(str);

    if (SS_true(flag))
       {strcpy(t, text);
        text = t;}

/* this may look weird but it is correct!!!! */
    else
       {obj = SS_car(argl);
        if (!SS_stringp(obj))
           SS_error("BAD STRING - _SS_STRTOK", obj);

        text = SS_STRING_TEXT(obj);};

    for (ps = delim, pt = d; (c = *ps) != '\0'; ps++)
        {if (c == '\\')
            {c = *(++ps);
	     switch (c)
	        {case 'n' :
		      *pt++ = '\n';
		      break;
                 case 't' :
		      *pt++ = '\t';
		      break;
                 case 'r' :
		      *pt++ = '\r';
		      break;
                 case 'f' :
		      *pt++ = '\f';
		      break;
		 default :
		      *pt++ = c;
		      break;};}
	 else
	    *pt++ = c;};

    *pt = '\0';

    ps = (*fnc)(text, d);

    rv = (ps == NULL) ? SS_null : SS_mk_string(ps);

    CFREE(delim);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRTOK - tokenize the string via SC_firsttok */

static object *_SSI_strtok(SS_psides *si, object *argl)
   {object *o;

    o = _SS_strtok(argl, SC_firsttok);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LASTTOK - tokenize the string via SC_lasttok */

static object *_SSI_lasttok(SS_psides *si, object *argl)
   {object *o;

    o = _SS_strtok(argl, SC_lasttok);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_STR - install the string primitives */

void _SS_inst_str(void)
   {

    SS_install("blank?",
               "Procedure: Return #t if a string is blank or a comment",
               SS_nargs,
               _SSI_blankstr, SS_PR_PROC);

    SS_install("down-case",
               "Procedure: Return a string containing only lower case char made from input",
               SS_sargs,
               _SSI_dncase, SS_PR_PROC);

    SS_install("lasttok",
               "Procedure: Extract the last token from the string ala SC_lasttok",
               SS_nargs,
               _SSI_lasttok, SS_PR_PROC);

    SS_install("list->string",
               "Procedure: Returns a string constructed from a list of characters",
               SS_nargs,
               _SSI_lststr, SS_PR_PROC);

    SS_install("make-string",
               "Procedure: Return a blank filled string of the given length",
               SS_sargs,
               _SSI_mk_str, SS_PR_PROC);

    SS_install("strchr",
               "Procedure: Like the C function",
               SS_nargs,
               _SSI_strchr, SS_PR_PROC);

    SS_install("istrchr",
               "Procedure: Like strchr but return starting index in original string",
               SS_nargs,
               _SSI_istrchr, SS_PR_PROC);

    SS_install("strstr",
               "Procedure: Like the C function",
               SS_nargs,
               _SSI_strstr, SS_PR_PROC);

    SS_install("strcasestr",
               "Procedure: Like the C function",
               SS_nargs,
               _SSI_strcasestr, SS_PR_PROC);

    SS_install("istrstr",
               "Procedure: Like strstr but return starting index in original string",
               SS_nargs,
               _SSI_istrstr, SS_PR_PROC);

    SS_install("string",
               "Procedure: Return a string from the given list of characters",
               SS_nargs,
               _SSI_string, SS_PR_PROC);

    SS_install("string=?",
               "Procedure: Returns #t iff the strings are the same (length too)",
               SS_nargs,
               _SSI_streq, SS_PR_PROC);

    SS_install("string>=?",
               "Procedure: Returns #t iff the first string is 'greater than or equal to' the second",
               SS_nargs,
               _SSI_strge, SS_PR_PROC);

    SS_install("string>?",
               "Procedure: Returns #t iff the first string is 'greater than' the second",
               SS_nargs,
               _SSI_strgt, SS_PR_PROC);

    SS_install("string<=?",
               "Procedure: Returns #t iff the first string is 'less than or equal to' the second",
               SS_nargs,
               _SSI_strle, SS_PR_PROC);

    SS_install("string<?",
               "Procedure: Returns #t iff the first string is 'less than' the second",
               SS_nargs,
               _SSI_strlt, SS_PR_PROC);

    SS_install("string-ci=?",
               "Procedure: Returns #t iff the strings are the same (length too)",
               SS_nargs,
               _SSI_cistreq, SS_PR_PROC);

    SS_install("string-ci>=?",
               "Procedure: Returns #t iff the first string is 'greater than or equal to' the second",
               SS_nargs,
               _SSI_cistrge, SS_PR_PROC);

    SS_install("string-ci>?",
               "Procedure: Returns #t iff the first string is 'greater than' the second",
               SS_nargs,
               _SSI_cistrgt, SS_PR_PROC);

    SS_install("string-ci<=?",
               "Procedure: Returns #t iff the first string is 'less than or equal to' the second",
               SS_nargs,
               _SSI_cistrle, SS_PR_PROC);

    SS_install("string-ci<?",
               "Procedure: Returns #t iff the first string is 'less than' the second",
               SS_nargs,
               _SSI_cistrlt, SS_PR_PROC);

    SS_install("string->list",
               "Procedure: Construct a list of the characters in the given string",
               SS_sargs,
               _SSI_strlst, SS_PR_PROC);

    SS_install("string->number",
               "Procedure: Return the number represented by the given string",
               SS_sargs,
               _SSI_strnum, SS_PR_PROC);

    SS_install("string->symbol",
               "Procedure: Make a new variable with name given by the string",
               SS_sargs,
               _SSI_strsym, SS_PR_PROC);

    SS_install("string-append",
               "Procedure: Append the argument strings together into a new string and return it",
               SS_nargs,
               _SSI_strapp, SS_PR_PROC);

    SS_install("string-copy",
               "Procedure: Return a copy of the given string",
               SS_sargs,
               _SSI_strcpy, SS_PR_PROC);

    SS_install("string-fill!",
               "Procedure: Fill the given string with the given character",
               SS_nargs,
               _SSI_strfil, SS_PR_PROC);

    SS_install("string-length",
               "Procedure: Returns the number of characters in the given string",
               SS_sargs,
               _SSI_strlen, SS_PR_PROC);

    SS_install("string-ref",
               "Procedure: Returns the nth character in the given string",
               SS_nargs,
               _SSI_strref, SS_PR_PROC);

    SS_install("string-set!",
               "Procedure: Set the Nth character of the string to the given character",
               SS_nargs,
               _SSI_strset, SS_PR_PROC);

    SS_install("strtok",
               "Procedure: Extract the next token from the string ala SC_firsttok",
               SS_nargs,
               _SSI_strtok, SS_PR_PROC);

    SS_install("substring",
               "Procedure: Extract the substring zero-origin indexed by the last two args",
               SS_nargs,
               _SSI_strsub, SS_PR_PROC);

    SS_install("symbol->string",
               "Procedure: Make a new string out of the given variable name",
               SS_sargs,
               _SSI_symstr, SS_PR_PROC);

    SS_install("string-trim",
               "Procedure: Trim characters off the front, back, or both of a string",
               SS_nargs,
               _SSI_trim, SS_PR_PROC);

    SS_install("up-case",
               "Procedure: Return a string containing only upper case char made from input",
               SS_sargs,
               _SSI_upcase, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

