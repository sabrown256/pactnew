/*
 * SHCHR.C - support for the Scheme characters
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHARP - function version of SS_charobjp macro */

static object *_SSI_charp(object *obj)
   {object *o;

    o = SS_charobjp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHREQ - char=? for Scheme */

static object *_SSI_chreq(object *argl)
   {object *o1, *o2, *o;

    o2 = SS_null;

    if (!SS_charobjp(o1 = SS_car(argl)) || !SS_charobjp(o2 = SS_cadr(argl)))
       SS_error("ARGUMENTS NOT CHARACTERS - CHAR=?", argl);

    o = (SS_CHARACTER_VALUE(o1) == SS_CHARACTER_VALUE(o2)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHRGE - char>=? for Scheme */

static object *_SSI_chrge(object *argl)
   {object *o1, *o2, *o;

    o2 = SS_null;

    if (!SS_charobjp(o1 = SS_car(argl)) || !SS_charobjp(o2 = SS_cadr(argl)))
       SS_error("ARGUMENTS NOT CHARACTERS - CHAR>=?", argl);

    o = (SS_CHARACTER_VALUE(o1) >= SS_CHARACTER_VALUE(o2)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHRGT - char>? for Scheme */

static object *_SSI_chrgt(object *argl)
   {object *o1, *o2, *o;

    o2 = SS_null;

    if (!SS_charobjp(o1 = SS_car(argl)) || !SS_charobjp(o2 = SS_cadr(argl)))
       SS_error("ARGUMENTS NOT CHARACTERS - CHAR>?", argl);

    o = (SS_CHARACTER_VALUE(o1) > SS_CHARACTER_VALUE(o2)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHRLE - char<=? for Scheme */

static object *_SSI_chrle(object *argl)
   {object *o1, *o2, *o;

    o2 = SS_null;

    if (!SS_charobjp(o1 = SS_car(argl)) || !SS_charobjp(o2 = SS_cadr(argl)))
       SS_error("ARGUMENTS NOT CHARACTERS - CHAR<=?", argl);

    o = (SS_CHARACTER_VALUE(o1) <= SS_CHARACTER_VALUE(o2)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHRLT - char<? for Scheme */

static object *_SSI_chrlt(object *argl)
   {object *o1, *o2, *o;

    o2 = SS_null;

    if (!SS_charobjp(o1 = SS_car(argl)) || !SS_charobjp(o2 = SS_cadr(argl)))
       SS_error("ARGUMENTS NOT CHARACTERS - CHAR<?", argl);

    o = (SS_CHARACTER_VALUE(o1) < SS_CHARACTER_VALUE(o2)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHRINT - char->integer for Scheme */

static object *_SSI_chrint(object *chr)
   {object *o;

    if (!SS_charobjp(chr))
       SS_error("ARGUMENT NOT CHARACTER - CHAR->INTEGER", chr);

    o = SS_mk_integer((BIGINT) SS_CHARACTER_VALUE(chr));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_INTCHR - integer->char for Scheme */

static object *_SSI_intchr(object *obj)
   {object *o;

    if (!SS_integerp(obj))
       SS_error("ARGUMENT NOT INTEGER - INTEGER->CHAR", obj);

    o = SS_mk_char((int) SS_INTEGER_VALUE(obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_CHR - install the character primitives */

void _SS_inst_chr(void)
   {

    SS_install("char?",
               "Procedure: Returns #t iff the object is of type char",
               SS_sargs,
               _SSI_charp, SS_PR_PROC);

    SS_install("char=?",
               "Procedure: Returns #t iff the chars are the same",
               SS_nargs,
               _SSI_chreq, SS_PR_PROC);

    SS_install("char>=?",
               "Procedure: Returns #t iff the first character is 'greater than or equal to' the second",
               SS_nargs,
               _SSI_chrge, SS_PR_PROC);

    SS_install("char>?",
               "Procedure: Returns #t iff the first character is 'greater than' the second",
               SS_nargs,
               _SSI_chrgt, SS_PR_PROC);

    SS_install("char<=?",
               "Procedure: Returns #t iff the first character is 'less than or equal to' the second",
               SS_nargs,
               _SSI_chrle, SS_PR_PROC);

    SS_install("char<?",
               "Procedure: Returns #t iff the first character is 'less than' the second",
               SS_nargs,
               _SSI_chrlt, SS_PR_PROC);

    SS_install("char->integer",
               "Procedure: Returns the integer representation of the given integer",
               SS_sargs,
               _SSI_chrint, SS_PR_PROC);

    SS_install("integer->char",
               "Procedure: Returns the character representation of the given integer",
               SS_sargs,
               _SSI_intchr, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
