/*
 * SHTTY.C - routines to handle low level terminal I/O
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

#define PRIMITIVE_PRINT (*_SS.pr_print)
#define PRIMITIVE_PUTS  (*_SS.pr_puts)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE int EOI(object *str)
   {int rv;
    char *p, *s;

    p = SS_PTR(str);
    s = SS_BUFFER(str);

/* NOTE: we were doing this for years
    rv = (((p != s) && (*(p - 1) == '\n')) || (*p == '\0'));
 *
 * but is was found using a 'here' document in a shell script that
 * the case of multiple expressions separated by '\n' requires
 * this:
 */
    rv = (((p != s) && (*(p - 1) == '\n') && (*p == '\0')) || (*p == '\0'));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_GET_INPUT - get more text to feed SS_get_ch
 *               - return -1 to break
 *               -         0 to continue
 *               -         1 for further processing
 */

static int _SS_get_input(object *str)
   {int rv;
    char *p;
    FILE *fp;

    rv = 0;

    if (_SS.pr_gets == NULL)
#ifdef NO_SHELL
       _SS.pr_gets = (PFfgets) PG_wind_fgets;
#else
       _SS.pr_gets = io_gets;
#endif

    fp = SS_INSTREAM(str);

    if (fp == NULL)
       rv = -1;

    if (fp == stdin)
       {p = SC_prompt(_SS.pr_prompt, SS_BUFFER(str), MAXLINE);
	_SS.pr_prompt = NULL;}
    else
       p = _SS.pr_gets(SS_BUFFER(str), MAXLINE, fp);

    if (p == NULL)
       {*SS_PTR(str) = (char) EOF;
	rv = -1;}
    else
       SS_PTR(str) = SS_BUFFER(str);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_CH - the do it right character reader
 *           - must get the next character from the given stream
 */

int SS_get_ch(object *str, int ign_ws)
   {int c, ok, st;
    SS_psides *si;

    si = &_SS_si;

    if (si->pr_gets == NULL)
       si->pr_gets = _SS_get_input;

    for (ok = TRUE; ok == TRUE; )
        {if (EOI(str))
	    {st = si->pr_gets(str);
	     if (st == -1)
	        {c = EOF;
		 break;}
	     else if (st == 0)
	        {c = '\n';
		 continue;};};

	 c = *SS_PTR(str)++;
	 SS_CHAR_INDEX(str)++;

	 if (c == EOF)
            SS_PTR(str)--;

	 if (ign_ws)
            {switch (c)
                {case '\n':
                 case '\r':
                      SS_LINE_NUMBER(str)++;
                      SS_CHAR_INDEX(str) = 1;
		      break;
                 case '\t':
                 case ' ' :
		      break;
                 case ';' :
		      while ((ok == TRUE) && ((c = *SS_PTR(str)++) != '\0'))
		         {if (c == EOF)
			     ok = FALSE;
			  else if ((c == '\n') || (c == '\r'))
			     {SS_LINE_NUMBER(str)++;
			      SS_CHAR_INDEX(str) = 1;
			      break;};};
		      break;

                 default :
		      ok = FALSE;
		      break;};}
	 else
	    ok = FALSE;};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_UNGET_CH - the do it right character unreader
 *             - must put the character back on the stream
 */

void SS_unget_ch(int c, object *str)
   {

    if (SS_PTR(str) > SS_BUFFER(str))
       {*(--SS_PTR(str)) = (char) c;
	SS_CHAR_INDEX(str)--;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PUT_CH - the do it right character printer
 *           - must put the character to the given stream
 */

void SS_put_ch(int c, object *str)
   {FILE *s;

    s = SS_INSTREAM(str);
    if (s != NULL)
       putc(c, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PRINTF - a print function that can handle transcripts
 *           - a NULL value for FP means we just want the specifications
 */

int SS_printf(FILE *fp, char *fmt, ...)
   {char *s;
    FILE *hp;
    SS_psides *si;

    si = &_SS_si;

    SC_VA_START(fmt);
    s = _SS_vdsnprintf(TRUE, fmt, __a__);
    SC_VA_END;

    if (fp != NULL)
       {if (_SS.pr_print == NULL)
#ifdef NO_SHELL
	   _SS.pr_print = (PFfprintf) PG_wind_fprintf;
#else
	   _SS.pr_print = io_printf;
#endif

	hp = SS_OUTSTREAM(si->histdev);
	if ((si->hist_flag != NO_LOG) && (fp != hp))
	   PRIMITIVE_PRINT(hp, "%s", s);

	PRIMITIVE_PRINT(fp, "%s", s);};

    CFREE(s);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_FPUTS - a print function that can handle transcripts
 *          - needed for long strings because of the fixed buffer
 *          - vsprintf based functions require
 */

int SS_fputs(char *s, FILE *fp)
   {int rv;
    FILE *hp;
    SS_psides *si;

    si = &_SS_si;

    rv = 0;
    if (fp != NULL)
       {if (_SS.pr_puts == NULL)
#ifdef NO_SHELL
	   _SS.pr_puts = (PFfputs) PG_wind_fputs;
#else
	   _SS.pr_puts = io_puts;
#endif

	hp = SS_OUTSTREAM(si->histdev);
	if ((si->hist_flag != NO_LOG) && (fp != hp))
	   rv = SS_puts(s, hp, _SS.pr_puts);

	rv = SS_puts(s, fp, _SS.pr_puts);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_STRING - convert a SCHEME object to a string */

char *SS_get_string(object *obj)
   {

    memset(_SS.str, 0, MAXLINE);

    if (obj == NULL)
       _SS.str[0] = '\0';

    else if (obj->print_name != NULL)
       strcpy(_SS.str, obj->print_name);
      
    else if (SS_integerp(obj))
       snprintf(_SS.str, MAXLINE, "%lld", (long long) SS_INTEGER_VALUE(obj));

    else if (SS_floatp(obj))
       snprintf(_SS.str, MAXLINE, "%g", SS_FLOAT_VALUE(obj));

    else if (SS_nullobjp(obj))
        strcpy(_SS.str, "nil");

    else if (obj == NULL)
        strcpy(_SS.str, "nil");

    else
        strcpy(_SS.str, "-none-");

    return(_SS.str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
