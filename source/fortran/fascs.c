/*
 * FASCS.C - FORTRAN usable string handling facilities
 *         - there is a convention is some applications in which
 *         - FORTRAN strings are blank filled to the end and this
 *         - implies the possibliity of computing string length by
 *         - finding the position of the last non-blank character
 *         - and using that as the string length
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

#define CAT(_d, _nd, _n, _s)                                                \
   {int _ns;                                                                \
    _ns = strlen(_s);                                                       \
    if (_ns > 0)                                                            \
       {if (_n + _ns >= _nd)                                                \
           return;                                                          \
        strcat(_d, _s);                                                     \
       _n = strlen(_d);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STRLEN - helper to find string length in the Fortran convention */

int _SC_strlen(char *s, int nx)
   {int n;

    for (n = nx-1; n >= 0; n--)
        if (s[n] != ' ')
	   break;

    s[++n] = '\0';

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCSTLN - Fortran string length function
 *        - looks from end for first non-blank character in S
 *        - this is simply a convention but one used by some other codes
 *        - NX is the length of the character array
 */

FIXNUM FF_ID(scstln, SCSTLN)(char *s, FIXNUM *snx)
   {int n, nx;
    char *t;

    t  = s;
    nx = *snx;

    n = _SC_strlen(t, nx);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCSTTK - return the first token of S in D
 *        - both S and D are character arrays PNC long
 *        - and points S to the next element in the string
 */

void FF_ID(scsttk, SCSTTK)(FIXNUM *sncs, char *d, char *s,
			   FIXNUM sndl, char *dl)
   {int j, n, nc, flag;
    char *dst, *src, *delim;
    char c;

    nc    = *sncs;
    dst   = d;
    src   = s;
    delim = dl;

    n = _SC_strlen(s, nc);

/* clear D */
    memset(dst, ' ', nc);

    flag = FALSE;
    for (; n > 0; n--)
        {c = *src;

/* if the character c is not a delimiter record it */
	 if (strchr(delim, c) == NULL)
	    {*dst++ = c;
	     flag   = TRUE;}

/* if we hit another delimiter we're done */
	 else if (flag)
	    break;

	 for (j = 1; j < n; j++)
	     src[j-1] = src[j];

	 src[j-1] = ' ';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCSPNT - FORTRAN version of snprintf
 *        - PNC is the length of the character array D
 *        - and is taken to be the character array length of any strings
 *        - implied in the argument list
 */

void FF_ID(scspnt, SCSPNT)(FIXNUM *sncd, char *d, char *f, ...)
   {int c, nc, nd;
    char local[MAXLINE], tb[10], ce;
    char *fmt, *le, *lb, *pt, *dst, *s;
    FIXNUM *lv;
    double *dv;
    char *sv;

    nc     = (int) *sncd;
    fmt    = f;
    dst    = d;
    dst[0] = '\0';

    SC_VA_START(f);

    nd = 0;
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

/* don't exceed the return buffer size */
	CAT(dst, nc, nd, local);

        if (c == '\0')
           break;

/* copy from the % to the type specifier to get the format descriptor for
 * this item
 */
        le = strpbrk(fmt, "sdouxXfeEgGcp%");
        ce = (le != NULL) ? *le : '\0';
        local[0] = '%';
        for (lb = &local[1]; le != fmt; *lb++ = *fmt++);
        fmt++;
        *lb++ = ce;
        *lb = '\0';

/* jump on the type spec to pull the correct arg type off the stack */
        switch (ce)
           {case 's' :
                 if (_SC.ta == NULL)
                    {_SC.nt = MAX_BFSZ;
                     _SC.ta = CPMAKE_N(char, _SC.nt, 3);
                     memset(_SC.ta, 0, _SC.nt);}

	         sv = SC_VA_ARG(char *);
		 SC_FORTRAN_STR_C(_SC.ta, sv, min(nc, _SC.nt-1));
		 nd = _SC_fmt_strcat(dst, nc, nd, local, _SC.ta);
		 if (nd == -1)
		    return;
                 break;

            case 'c' :
	         sv = SC_VA_ARG(char *);
		 SC_FORTRAN_STR_C(tb, sv, 1);
		 CAT(dst, nc, nd, tb);
                 break;

            case 'i' :
            case 'X' :
            case 'x' :
            case 'o' :
            case 'd' :
            case 'u' :
	         lv = SC_VA_ARG(FIXNUM *);
		 s  = SC_dsnprintf(FALSE, local, (long) *lv);
		 CAT(dst, nc, nd, s);
                 break;

            case 'f' : 
            case 'e' :
            case 'E' : 
            case 'g' : 
            case 'G' :
	         dv = SC_VA_ARG(double *);
		 s  = SC_dsnprintf(FALSE, local, (double) *dv);
		 CAT(dst, nc, nd, s);
                 break;

            case '%' :
                 CAT(dst, nc, nd, "%");
                 break;};};

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

