/*
 * SXIO.C - SX io routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SPRINT - C's fprintf for Scheme */

object *SX_print(SS_psides *si, object *argl)
   {int c;
    char forms[MAXLINE], local[MAXLINE], ce;
    char *fmt, *le, *lb, *pt;
    FILE *stream;
    object *str, *obj, *format;

    str  = SS_car(si, argl);
    argl = SS_cdr(si, argl);
    if (SS_nullobjp(str))
       str = si->outdev;

    if (!SS_outportp(str))
       SS_error(si, "BAD PORT - PRINT", str);
    stream = SS_OUTSTREAM(str);

    format = SS_car(si, argl);
    if (!SS_stringp(format))
       SS_error(si, "BAD FORMAT - PRINT", format);
    SC_strncpy(forms, MAXLINE, SS_STRING_TEXT(format), -1);
    fmt = forms;

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
        if (SS_nullobjp(argl))
           return(SS_f);
        argl = SS_cdr(si, argl);
        if (SS_nullobjp(argl))
           return(SS_f);
        obj  = SS_car(si, argl);

/* jump on the type spec to pull the correct arg type off the stack */
        switch (ce)
           {case 's' :
            case 'c' :
	         SS_print(si, str, obj, "", "");
		 break;

            case 'i' :
            case 'X' :
            case 'x' :
            case 'o' :
            case 'd' :
            case 'u' :
	         if (!SS_integerp(obj))
		    SS_error(si, "NON-INTEGER FOR INTEGER FIELD - PRINT",
			       obj);
		 PRINT(stream, local, SS_INTEGER_VALUE(obj));
		 break;

            case 'f' : 
            case 'e' :
            case 'E' : 
            case 'g' : 
            case 'G' :
	         if (!SS_floatp(obj))
		    SS_error(si, "NON-FLOAT FOR REAL FIELD - PRINT",
			       obj);
		 PRINT(stream, local, SS_FLOAT_VALUE(obj));
		 break;
            case '%' :
	         PRINT(stream, "%%");
		 break;};};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
