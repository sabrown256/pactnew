/*
 * SCSTR.C - string manipulation functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 
#include "scope_mem.h" 

#ifndef EDOM
# define EDOM 16
#endif

#define MBASE 32

#define FIN_TOKEN(_s, _p)                                                   \
   {if (nl == TRUE)                                                         \
       *_p++ = '\n';                                                        \
    *_p++ = '\0';                                                           \
    _s    = CSTRSAVE(bf);}

#define CAT(_d, _nd, _n, _s)                                                \
   {int _ns;                                                                \
    _ns = strlen(_s);                                                       \
    if (_ns > 0)                                                            \
       {if (_n + _ns >= _nd)                                                \
           return(-1);                                                      \
        strcat(_d, _s);                                                     \
        _n = strlen(_d);};}

#define PAD(_d, _nd, _ln, _m)                                               \
   {int _i, _lm;                                                            \
    _lm = (_m);                                                             \
    for (_i = 0; (_i < _lm) && (_ln < _nd); _i++)                           \
        _d[_ln++] = ' ';                                                    \
    if (_ln < _nd)                                                          \
       _d[_ln++] = '\0';}

/* Python on 10.4 cannot resolve/find vsnprintf dynamically */
#if defined(MACOSX) && defined(HAVE_PYTHON)
# define VSNPRINTF_FUNCTION SC_vsnprintf
#endif

#ifndef VSNPRINTF_FUNCTION
# ifdef HAVE_VSNPRINTF
#   define VSNPRINTF_FUNCTION vsnprintf
# else
#   define VSNPRINTF_FUNCTION SC_vsnprintf
# endif
#endif

SC_THREAD_LOCK(_SC_str_lock);

int
 (*SC_vsnprintf_hook)(char *dst, size_t nc,
		      const char *fmt, va_list a) = VSNPRINTF_FUNCTION;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SC_FMT_STRCAT - cat S onto D according to FMT
 *                - where D is ND long
 *                - LN is the current length of D
 *                - return the new length of D
 */

int _SC_fmt_strcat(char *d, int nd, int ln, char *fmt, char *s)
   {int ns, wd;
    char *pf;

    ns = strlen(s);
    pf = fmt + 1;
    SC_LAST_CHAR(pf) = '\0';
    wd = SC_stoi(pf);

/* no width or string longer than specified width - print it all */
    if (ns >= ABS(wd))
       {CAT(d, nd, ln, s);}

/* right justified field */
    else if (wd > 0)
       {PAD(d, nd, ln, wd - ns);
	CAT(d, nd, ln, s);}

/* left justified field */
    else
       {wd = -wd;
	CAT(d, nd, ln, s);
	PAD(d, nd, ln, wd - ns);};

    return(ln);}

/*---------------------------------------------------------------------------*/

/*                                STRING FUNCTIONS                           */

/*--------------------------------------------------------------------------*/

/* SC_STRSAVEC - save string s somewhere remember its name
 *             - allocate one extra character so that firsttok won't kill
 *             - things in the one bad case
 */

char *SC_strsavec(char *s, int memfl,
		  const char *file, const char *fnc, int line)
   {int prm, na, sz;
    char *p;
    SC_mem_opt opt;

    p = NULL;

    if (s != NULL)
       {sz = strlen(s) + 2;

	prm = ((memfl & 1) != 0);
	na  = ((memfl & 2) != 0);

	opt.perm = prm;
	opt.na   = na;
	opt.zsp  = -1;
	opt.typ  = SC_STRING_I;

	opt.where.pfunc = file;
	opt.where.pfile = fnc;
	opt.where.line  = line;

	p = _SC_alloc_n(sz, sizeof(char), &opt);
	if (p != NULL)
	   {strcpy(p, s);
	    p[sz-1] = '\0';};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHAR_INDEX - return a version of the index N which
 *               - is guaranteed to be in the string
 *               - if N > 0 return the index of the Nth character
 *               - if N < 0 return the index of the Nth to last character
 */

unsigned int SC_char_index(char *s, int n)
    {unsigned int nc, rv;

     nc = strlen(s);

     if (n < 0)
        n += nc;

     rv = min(n, nc);

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRCAT - this is to strcat as snprintf is to sprintf
 *           - a safe string concatentation function
 */

char *SC_strcat(char *dest, size_t lnd, char *src)
   {size_t ld, ls;
    char *s;
        
    ld = strlen(dest);
    ls = strlen(src);
    if (ls + ld < lnd)
       s = strcat(dest, src);
    else
       s = strncat(dest, src, lnd - ld - 1);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_VSTRCAT - this is to strcat as snprintf is to sprintf
 *            - a safe string concatentation function
 */

char *SC_vstrcat(char *dest, size_t lnd, char *fmt, ...)
   {size_t ld, ls;
    char *s, *t;
        
    t = CMAKE_N(char, lnd);

    SC_VA_START(fmt);
    SC_VSNPRINTF(t, lnd, fmt);
    SC_VA_END;

    ld = strlen(dest);
    ls = strlen(t);
    if (ls + ld < lnd)
       s = strcat(dest, t);
    else
       s = strncat(dest, t, lnd - ld - 1);

    CFREE(t);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DSTRCAT - this is to strcat as SC_dsnprintf is to sprintf
 *            - a safe string concatentation function
 */

char *SC_dstrcat(char *dest, char *src)
   {size_t ln, lnd, ld, ls;
    char *s;
        
    ls = strlen(src);

    if (dest == NULL)
       {lnd  = 2*max(ls, MAXLINE);
	dest = CMAKE_N(char, lnd);
        dest[0] = '\0';};

    lnd = SC_arrlen(dest);
    ld  = strlen(dest);

    if (ls + ld >= lnd)
       {ln = 2*(ls + ld);
	CREMAKE(dest, char, ln);};

    s = strcat(dest, src);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRNCPY - this is to strncpy as snprintf is to sprintf
 *            - a safe string copy function
 *            - unlike strncpy this always returns a null terminated string
 */

char *SC_strncpy(char *d, size_t nd, char *s, size_t ns)
   {size_t nc;
        
    if (s == NULL)
       nc = 0;
    else
       {nc = min(ns, nd-1);
	strncpy(d, s, nc);};

    if (d != NULL)
       d[nc] = '\0';

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DSTRCPY - this is to strcpy as SC_dsnprintf is to sprintf
 *            - a safe string copy function
 */

char *SC_dstrcpy(char *dest, char *src)
   {size_t ld, ls;
    char *s;
        
    ls = strlen(src);

    if (dest == NULL)
       {ld   = 2*max(ls, MAXLINE);
	dest = CMAKE_N(char, ld);
        dest[0] = '\0';};

    ld = SC_arrlen(dest);

    if (ls + 2 >= ld)
       {ld = 2*ls;
	CREMAKE(dest, char, ld);};

    s = strcpy(dest, src);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRSTR - finds first occurance of string2 in string1
 *           - if not there returns NULL
 *           - this is for UNIX which doesn't have strstr
 */

char *SC_strstr(char *string1, char *string2)

#ifndef ANSI

   {char *s1, *s2, *s3;
        
    s1 = string1;
    while (*s1 != '\0')
       {for (s2 = string2, s3 = s1;
             (*s2 == *s3) && (*s3 != '\0') && (*s2 != '\0');
             s3++, s2++);

/* if s2 makes it to the end the string is found */
        if (*s2 == '\0')
           return(s1);
        else
           s1++;};

    return(NULL);}

#else

   {char *rv;

    rv = strstr(string1, string2);

    return(rv);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRSTRI - finds first occurance of string2 in string1 (case insensitive)
 *            - if not there returns NULL
 */

char *SC_strstri(char *string1, char *string2)

   {char *s1, *s2, *s3, *rv;
        
    rv = NULL;
    s1 = string1;
    while (*s1 != '\0')
       {for (s2 = string2, s3 = s1;
             (toupper((int) *s2) == toupper((int) *s3)) &&
	     (*s3 != '\0') && (*s2 != '\0');
             s3++, s2++);

/* if s2 makes it to the end the string is found */
        if (*s2 == '\0')
	   {rv = s1;
	    break;}
        else
           s1++;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRREV - copy the string onto itself in reverse order */

char *SC_strrev(char *s)
   {int i;
    char *t, *p;

    p = s;
    i = strlen(s) + 1;
    t = CMAKE_N(char, i);

    if (t == NULL)
       return(NULL);

    t[--i] = '\0';
    while (*p)
       t[--i] = *p++;

    strcpy(s, t);
    CFREE(t);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SQUEEZE_BLANKS - replace consecutive blanks with a single blank
 *                   - and remove leading and trailing blanks
 */

char *SC_squeeze_blanks(char *s)
   {char *sn, *so;

    if ((s != NULL) && (*s != '\0'))
       {for (so = s; *so == ' '; so++);

        for (sn = s; (*sn = *so) != '\0'; sn++, so++)
	    {while ((*so == ' ') && (*(so + 1) == ' '))
	        so++;};

        if ((sn > s) && (*(sn - 1) == ' '))
           *(sn - 1) = '\0';};

    return(s);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SQUEEZE_CHARS - remove any occurences of any characters in Q from S */

char *SC_squeeze_chars(char *s, char *q)
   {char *sb, *se;

    if ((s != NULL) && (*s != '\0'))
       {while (TRUE)
	   {for (sb = s; (*sb != '\0') && (strchr(q, *sb) == NULL); sb++);
	    if (*sb == '\0')
	       break;
	    else
	       {for (se = sb; (*se != '\0') && (strchr(q, *se) != NULL); se++);
		memmove(sb, se, strlen(se)+1);};};};

    return(s);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DSTRSUBST - substitute N occurences of A with B in S
 *              - like: sed 's/A/B/g'
 *              - returns dynamically allocated copy which caller must free
 */

char *SC_dstrsubst(char *s, char *a, char *b, size_t n)
   {int i, o;
    char *p, *pr, *ps, *pa, *pt;

    ps = SC_dstrcpy(NULL, s);
    pt = NULL;

    o = 0;
    for (i = 0; i < n; i++)
        {p = strstr(ps+o, a);
	 if (p != NULL)
	    {pr = p + strlen(a);
	     *p = '\0';
	     pt = SC_dstrcpy(pt, ps);
	     pt = SC_dstrcat(pt, b);
	     o  = strlen(pt);
	     pt = SC_dstrcat(pt, pr);

	     pa = ps;
	     ps = pt;
             pt = pa;

	     pt[0] = '\0';}

	 else
	    break;};

    CFREE(pt);

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRSUBST - substitute N occurences of A with B in S
 *             - like: sed 's/A/B/g'
 *             - returns dynamically allocated copy which caller must free
 */

char *SC_strsubst(char *d, int nc, char *s, char *a, char *b, size_t n)
   {char *ps;

    ps = SC_dstrsubst(s, a, b, n);

    if (d != NULL)
       SC_strncpy(d, nc, ps, -1);

    CFREE(ps);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STR_UPPER - upper casify the given string (in place) */

char *SC_str_upper(char *s)
   {int c;
    char *ps;

    if (s != NULL)
       {ps = s;
	while ((c = *ps) != '\0')
	   *ps++ = toupper(c);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STR_LOWER - lower casify the given string (in place) */

char *SC_str_lower(char *s)
   {int c;
    char *ps;

    if (s != NULL)
       {ps = s;
	while ((c = *ps) != '\0')
	   *ps++ = tolower(c);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STR_ICMP - case insensitive string comparison */

int SC_str_icmp(char *s, char *t)
   {int rv;
    char p[MAXLINE], q[MAXLINE];

    rv = 0;

    if ((s != NULL) && (t != NULL))
       {strcpy(p, s);
	strcpy(q, t);
	SC_str_upper(p);
	SC_str_upper(q);

	rv = strcmp(p, q);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STR_REPLACE - replace each occurrence in string s of old pattern po
 *                - with new pattern pn. start at the beginning of s and
 *                - ignore overlapping occurrences.
 */

char *SC_str_replace(char *s, char *po, char *pn)
   {char t[MAXLINE];
    char *ps, *pp;
    char *pt = t;
    int lo = (int) strlen(po);
    int i;

    for (ps = s, pp = ps;
         ((ps = SC_strstr(ps, po)) != NULL);
         ps += lo, pp = ps)
        {while (pp < ps)
             *pt++ = *pp++;
         for (i = 0; ((*pt = *(pn + i)) != '\0'); pt++, i++);};

    if (pt != t)
       {while ((*pt++ = *pp++) != '\0');
        strcpy(s, t);}

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STRERR_EMU - emulate strerror_r using strerror */

int _SC_strerr_emu(int err, char *msg, size_t nc)
   {int rv;
    int ln;
    char *s;

    s  = strerror(errno);
    ln = strlen(s);

    SC_strncpy(msg, nc, s, ln);

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STRERR_PSX - strerror_r worker using the POSIX standard version
 *                - or emulating it
 */

int _SC_strerr_psx(int err, char *msg, size_t nc)
   {int rv;

    rv = 0;

#ifdef HAVE_POSIX_STRERROR_R

    rv = strerror_r(errno, msg, nc);

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_STRERR_GNU - wrapper for odd GNU non-standard strerror_r */

int _SC_strerr_gnu(int err, char *msg, size_t nc)
   {int rv;

#if defined(HAVE_GNU_STRERROR_R)

    int ln;
    char *s;

    s  = strerror_r(errno, msg, nc);
    ln = strlen(s);

    SC_strncpy(msg, nc, s, ln);

#endif

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRERROR - a portable, thread safe version of strerror which
 *             - handles the glibc incompatibility
 */

int SC_strerror(int err, char *msg, size_t nc)
   {int rv;

#if defined(HAVE_GNU_STRERROR_R)
    rv = _SC_strerr_gnu(err, msg, nc);

#elif defined(HAVE_POSIX_STRERROR_R)
    rv = _SC_strerr_psx(err, msg, nc);

#else
    rv = _SC_strerr_emu(err, msg, nc);

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TRIM_LEFT - trim any characters contained in DELIM from the front
 *              - of the string S
 *              - do not actually remove anything just return
 *              - a pointer to the first non-matching character
 */

char *SC_trim_left(char *s, char *delim)
   {

    for (; (*s != '\0') && (strchr(delim, *s) != NULL); s++);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TRIM_RIGHT - trim any characters contained in DELIM from the back
 *               - of the string S
 */

char *SC_trim_right(char *s, char *delim)
   {int ic, nc, c;

/* do nothing with zero length strings */
    nc = strlen(s);
    for (ic = nc - 1; ic >= 0; ic--)
        {c = s[ic];
	 if (strchr(delim, c) == NULL)
	    break;
	 s[ic] = '\0';};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISALNUM - wrap isalnum because BlueGene is seriously messed up */

int SC_isalnum(int c)
   {int rv;

#if 0
    rv = ((('0' <= c) && (c <= '9')) ||
	  (('A' <= c) && (c <= 'Z')) ||
	  (('a' <= c) && (c <= 'z')));
#else
    rv = isalnum(c);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISHEXDIGIT - wrap ishexdigit because BlueGene is seriously messed up */

int SC_ishexdigit(int c)
   {int rv;

#if 0
    rv = ((('0' <= c) && (c <= '9')) ||
	  (('A' <= c) && (c <= 'F')) ||
	  (('a' <= c) && (c <= 'f')));
#else
    rv = (isdigit(c) ? (c) - '0' :
                       islower(c) ? (c) + 10 - 'a' : (c) + 10 - 'A');
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISXDIGIT - wrap isxdigit because BlueGene is seriously messed up */

int SC_isxdigit(int c)
   {int rv;

#if 0
    rv = ((c == 'x') || (c == 'X'));
#else
    rv = isxdigit(c);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISDIGIT - wrap isdigit because BlueGene is seriously messed up */

int SC_isdigit(int c)
   {int rv;

#if 0
    rv = (('0' <= c) && (c <= '9'));
#else
    rv = isdigit(c);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISPUNCT - wrap ispunct because BlueGene is seriously messed up */

int SC_ispunct(int c)
   {int rv;

#if 0
    rv = (strchr("!\"#$%&'()*+,-./@[\\]^_`{|}~", c) != NULL);
#else
    rv = ispunct(c);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ISSPACE - wrap isspace because BlueGene is seriously messed up */

int SC_isspace(int c)
   {int rv;

#if 0
    rv = (strchr(" \t\r\n\f\v", c) != NULL);
#else
    rv = isspace(c);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           TOKENIZERS                                     */

/*--------------------------------------------------------------------------*/

/* SC_FIRSTTOK - returns a pointer to the first token
 *             - and points s to the next element in the string
 */

char *SC_firsttok(char *s, char *delim)
   {int nb;
    char *t, *r, *tb;
        
    if (*s == '\0')
       return(NULL);

/* t is the pointer to the token */
    for (t = s; strchr(delim, *t) != NULL; t++)
        {if (*t == '\0')
            {*s = '\0';
             return(NULL);};};

/* r is the pointer to the remainder */
    for (r = t; strchr(delim, *r) == NULL; r++);

    nb = strlen(t) + 2;
    tb = CMAKE_N(char, nb);

/* if we aren't at the end of the string */
    if (*r != '\0')
       {*r++ = '\0';

/* copy the token into a temporary */
        strcpy(tb, t);

/* copy the remainder down into the original string */
        memmove(s, r, strlen(r)+1);

/* copy the token in the space left over */
        t = s + strlen(s) + 1;
        strcpy(t, tb);}

/* if we are at the end of the string we may overindex the string
 * by adding one more character (sigh)
 */
    else
       {strcpy(tb, t);
        *s = '\0';
        t  = s + 1;
        strcpy(t, tb);};

    CFREE(tb);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUOTED_TOK - returns a pointer to the first quote of a quoted string
 *                - and points s to the next element in the string
 */

char *_SC_quoted_tok(char *s, char *qdelim)
   {char qmatch, *t, *r, tokbuffer[MAXLINE];
        
    if (*s == '\0')
       return(NULL);

/* qmatch is the pointer to the first quote character */
    for (t = s; strchr(qdelim, *t) == NULL; t++);
    qmatch = *t++;
    if (*t == '\0')
       return(NULL);

/* is there a  matching quote character? */
    r = strchr(t, qmatch);
    if ((r == NULL) || (*r == '\0'))
       return(NULL);

/* copy the quoted token into a temporary
 * t is the pointer to the matching quote */
    strcpy(tokbuffer, (t-1));
    t = strchr((tokbuffer+1), qmatch);
    if (t != NULL)
       t[1] = '\0';

/* if we aren't at the end of the string */
/* copy the remainder down into the original string
 * GOTCHA: this should be replaced by MEMMOVE (ANSI standard C function)
 */
    if (r[1] != '\0')
       {strcpy(s, r+1);

/* copy the token in the space left over */
        t = s + strlen(s) + 1;
        strcpy(t, tokbuffer);}

/* if we are at the end of the string we may overindex the string
 * by adding one more character (sigh)
 */
    else
       {*s = '\0';
        t = s + 1;
        strcpy(t, tokbuffer);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIRSTTOKQ - returns a pointer to the first token or 
 *              - the first quote of a quoted string
 *              - and points s to the next element in the string
 *              - following the token or quoted string
 */

char *SC_firsttokq(char *s, char *delim, char *quotes)
   {int len, k, kmatch;
    char tokbuffer[MAXLINE];
    char *t, *rv;
        
    if (*s == '\0')
       return(NULL);

/* remove leading delimiters
 * t is the pointer to the first non-delimiter character
 */
    for (t = s; strchr(delim, *t) != NULL; t++)
        if (*t == '\0')
           return(NULL);

    if (t != s)
       memmove(s, t, strlen(t)+1);
        
    len = strlen(s);
    k   = strcspn(s, quotes);
    
    if ((k > 1) || (k == len)) 
       rv = SC_firsttok(s, delim);

    else
       {strcpy(tokbuffer, s+k);
	tokbuffer[1] = '\0';
	kmatch = strcspn(s+k+1, tokbuffer);

	if ((k + kmatch + 1) < len)
	   rv = _SC_quoted_tok(s, quotes);
	else
	   rv = SC_firsttok(s, delim);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PR_TOK - returns a pointer to the first token
 *            - and points s to the next element in the string
 */

char *_SC_pr_tok(char *s, char *delim)
   {int i, j;
    char tokbuffer[MAXLINE];
    char *t, *rv;

    i = strcspn(s, delim);
    j = strlen(s);
    if ((i == 0) && (i != j))
       {s++;
        rv = SC_firsttok(s, delim);}

    else
       {s[i] = '\0';
	strcpy(tokbuffer, s);

/* take care of last token in string */
	if (i == j)
	   *s = '\0';
	else
	   {t = s + i + 1;
	    memmove(s, t, strlen(t) + 1);};

	s += strlen(s) + 1;
	strcpy(s, tokbuffer);

        rv = s;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LASTTOK - find last token on a string, return it and
 *            - the preceeding string
 */

char *SC_lasttok(char *s, char *delim)
   {char *rv, *r;

    rv = NULL;

    r = SC_strrev(s);
    if (r != NULL)
       {rv = _SC_pr_tok(r, delim);
	s  = SC_strrev(r);};

    if (rv != NULL)
       rv = SC_strrev(rv);

    return(rv);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_NTOK - find the Nth token in a string S according to DELIM
 *         - return it in the array D which is NC long
 *         - this does not destroy S
 */

char *SC_ntok(char *d, int nc, char *s, int n, char *delim)
   {int i, ln;
    char *cp, *p, *t, *pp;

    t = NULL;
    p = NULL;
    if (s != NULL)
       {cp = CSTRSAVE(s);

	for (p = cp, i = 0; i < n; p = NULL, i++)
	    t = SC_strtok(p, delim, pp);

	if (t != NULL)
	   {ln = strlen(t);
	    SC_strncpy(d, nc, t, ln);
	    p = d;};

	CFREE(cp);};

    return(p);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TOKENIZE - return an array of strings obtained by
 *             - tokenizing the string S according to the delimiters DELIM
 *             - the array is terminated by a NULL string
 *             - it can be released by SC_free_strings
 */

char **SC_tokenize(char *s, char *delim)
   {int n;
    char *tok, *t, *ps, *u, **sa;
    SC_array *arr;

    n = strlen(s);
    t = CMAKE_N(char, n+100);
    if (t != NULL)
       {strcpy(t, s);

	arr = SC_STRING_ARRAY();
	SC_array_resize(arr, n/2, -1.0);

	for (ps = t; TRUE; ps = NULL)
	    {tok = SC_strtok(ps, delim, u);
	     if (tok == NULL)
	        break;

	     SC_array_string_add_copy(arr, tok);};

	SC_array_string_add(arr, NULL);

	CFREE(t);};

    sa = SC_array_done(arr);

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MATCH_QUOTE - scan down the string PPI copying characters into PPO
 *                 - the quote delimiter QC
 *                 - define when the string PPO is complete
 *                 - delimiters must be properly nested
 */

static void _SC_match_quote(char **ppi, char **ppo, int qc,
			    char *bf, char *delim)
   {int c, i, n, ks, ke;
    char *pi, *po, *s;

    po = *ppo;

/* count the characters for the target string */
    pi = *ppi;
    for (n = 0, c = *pi++; (c != qc) && (c != '\0'); c = *pi++, n++)
        {if (c == '\\')
	    c = *pi++;};

/* make a copy of the target string */
    s = CMAKE_N(char, n+1);

    pi = *ppi;
    for (i = 0, c = *pi++; (c != qc) && (c != '\0'); c = *pi++, i++)
        {if (c == '\\')
	    c = *pi++;
	 s[i] = c;};
    s[i] = '\0';

/* see what the next character is */
    c = *pi;

/* if the quoted string starts the current token BF */
    ks = (po-bf <= 1);

/* if the quoted string ends the current token BF */
    ke = ((delim == NULL) || (strchr(delim, c) != NULL));

/* leave the delimiter on the output stream */
    if ((ks == TRUE) && (ke == TRUE))
       {strncpy(po, s, n);
        po   += n;
	*po++ = qc;}

/* remove the delimiter from the output stream */
    else
       {po--;
	strncpy(po, s, n);
        po += n;};

    *ppi = pi;
    *ppo = po;

    CFREE(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MATCH_DELIM - scan down the string PPI copying characters into PPO
 *                 - the open delimiter OC and the close delimeter CC
 *                 - define when the string PPO is complete
 *                 - delimiters must be properly nested
 */

static int _SC_match_delim(char **ppi, char **ppo, int oc, int cc)
   {int c, level;
    char *pi, *po;

    pi = *ppi;
    po = *ppo;

    level = 1;
    for (c = *pi++; (level > 0) && (c != '\0'); c = *pi++)
        {*po++ = c;
	 if (c == '\\')
	    c = *pi++;

/* skip past quoted substrings */
	 if ((c == '\"') || (c == '\''))
	    _SC_match_quote(&pi, &po, c, po, NULL);

	 else if (c == oc)
	    level++;

	 else if (c == cc)
	    level--;};

    pi--;

    *ppi = pi;
    *ppo = po;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TOKENIZE_LITERAL - like SC_tokenize except
 *                     - that quoted strings
 *                     -   QU = 1 check "
 *                     -   QU = 2 check '
 *                     -   QU = 3 check both
 *                     - are preserved with the quotes as a single token
 */

char **SC_tokenize_literal(char *s, char *delim, int nl, int qu)
   {int c, n, front, more;
    char *bf, *pi, *po, *t, *u, *sb, **sa;
    SC_array *arr;

    t = CSTRSAVE(s);
    if (t == NULL)
       return(NULL);

    n   = strlen(s);
    sa  = NULL;
    arr = SC_STRING_ARRAY();
    SC_array_resize(arr, n/2, -1.0);

    bf   = CMAKE_N(char, MAX_BFSZ);
    pi   = t;
    more = TRUE;
    while (more)

/* pull a token without damaging the string
 * strtok does not do exactly what is needed here
 */
       {front = TRUE;
	po    = bf;
	for (c = *pi++; TRUE; c = *pi++)
	    {u = strchr(delim, c);

/* treat ';' specially - it must be its own token */
	     if (c == ';')
	        {if (front == FALSE)
		    {FIN_TOKEN(sb, po);
		     pi--;}
		 else
		    {po    = bf;
		     *po++ = ';';
		     FIN_TOKEN(sb, po);};
		 break;}

	     if (u == NULL)
	        {front = FALSE;

/* handle escaped characters now */
		 if (c == '\\')
		    {c = *pi++;

/* handle an escaped delimiter specially */
		     if (strchr("{\'\")", c) != NULL)
		        {*po++ = c;
			 continue;};};

	         *po++ = c;

/* look for strings with " or ' as the delimiter */
		 if (((c == '\"') && ((qu & 1) != 0)) ||
		     ((c == '\'') && ((qu & 2) != 0)))
		    _SC_match_quote(&pi, &po, c, bf, delim);
	        
/* look for strings with {} as the delimiter */
		 else if (c == '{')
		    c = _SC_match_delim(&pi, &po, '{', '}');

/* look for strings with () as the delimiter */
		 else if (c == '(')
		    c = _SC_match_delim(&pi, &po, '(', ')');

/* look for strings with [] as the delimiter */
		 else if (c == '[')
		    c = _SC_match_delim(&pi, &po, '[', ']');}

/* end of token but not end of input string
 * or end of string and partial token
 */
	     else if ((front == FALSE) || ((c == '\0') && (po != bf)))
	        {FIN_TOKEN(sb, po);
		 break;}

/* end of input string */
	     else if (c == '\0')
	       {sb   = NULL;
		more = FALSE;
	        break;};};

	SC_array_string_add(arr, sb);};

    CFREE(t);
    CFREE(bf);

    sa = SC_array_done(arr);

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FILE_STRINGS - return the lines of a text file as
 *                 - an array of strings
 */

char **SC_file_strings(char *fname)
   {char s[MAX_BFSZ];
    char **sa;
    SC_array *a;
    FILE *fp;

    a = SC_STRING_ARRAY();

    fp = fopen(fname, "r");
    while (fgets(s, MAX_BFSZ, fp) != NULL)
       SC_array_string_add_copy(a, s);

    fclose(fp);

    sa = SC_array_done(a);

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRINGS_FILE - write the arrray of strings SA to the file FNAME */

int SC_strings_file(char **sa, char *fname, char *mode)
   {int i, rv;
    FILE *fp;

    rv = FALSE;

    fp = fopen(fname, mode);
    if (fp != NULL)
       {for (i = 0; sa[i] != NULL; i++)
	    fputs(sa[i], fp);
	fclose(fp);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRINGS_PRINT - print each string of SA preceded by PRE to file FP
 *                  - FP must be stdout, stderr, or have been opened
 *                  - with io_open
 */

int SC_strings_print(FILE *fp, char **sa, char *pre)
   {int i, rv;

    rv = FALSE;

    if (fp != NULL)
       {for (i = 0; sa[i] != NULL; i++)
	    io_printf(fp, "%s%s\n", pre, sa[i]);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_STRINGS - free the an array of strings SA
 *                 - and each string in the array
 *
 * #bind SC_free_strings fortran() python()
 */

void SC_free_strings(char **sa)
   {int i;
    char *s;

    if (sa != NULL)
       {for (i = 0; TRUE; i++)
	    {s = sa[i];
	     if (s == NULL)
	        break;
	     CFREE(s);};

	CFREE(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REMOVE_STRING - remove the Nth string from the array */

void SC_remove_string(char **sa, int n)
   {int bpi, ns;
    char *pa;

    if (sa != NULL)
       {SC_ptr_arr_len(ns, sa);
	CFREE(sa[n]);

	bpi = sizeof(char *);
	pa  = (char *) sa;
	memmove(pa + n*bpi, pa + (n+1)*bpi, (ns - n)*bpi);

	sa[--ns] = NULL;};


    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONCATENATE - concatenate an array of strings A into S
 *                - this is the opposite of SC_tokenize
 */

char *SC_concatenate(char *s, int nc, int n, char **a, char *delim, int add)
   {int i;
    char *rv;

    rv = NULL;

    if ((s != NULL) && (nc > 0))
       {if (add == FALSE)
	   s[0] = '\0';
	for (i = 0; i < n; i++)
	    SC_vstrcat(s, nc, "%s%s", a[i], delim);

	SC_LAST_CHAR(s) = '\0';

        rv = s;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DCONCATENATE - concatenate an array of strings A
 *                 - this is the opposite of SC_tokenize
 *                 - return string is dynamically allocated
 *                 - and the user is responsible freeing it
 */

char *SC_dconcatenate(int n, char **a, char *delim)
   {int i, nc;
    char *rv;
    char *s;

    rv = NULL;

    if ((a != NULL) && (n > 0))
       {nc = 2;
	for (i = 0; i < n; i++)
	    nc += strlen(a[i]);
	nc += n*strlen(delim);

	s = CMAKE_N(char, nc);

	rv = SC_concatenate(s, nc, n, a, delim, FALSE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_PRINT_CHAR - return TRUE if C is a printable character
 *                  - this is a combination of isalnum, isspace, and
 *                  - ispunct
 *                  - FLAG is a bit mask for character types:
 *                  -    1  text characters cannot match
 *                  -       a-z A-Z 0-9   (modulo locale)
 *                  -    2  punctuation characters cannot match
 *                  -       !"#$%&'()*+,-./@[\]^_`{|}~ (modulo locale)
 *                  -    4  whitespace characters cannot match
 *                  -       ' ' '\t' '\r' '\n' '\f' '\v'
 */

int SC_is_print_char(int c, int flag)
   {int ipr, ipu, isp, rv;

    ipr = (SC_isalnum(c) != 0) && ((flag & 1) == 0);
    ipu = (SC_ispunct(c) != 0) && ((flag & 2) == 0);
    isp = (SC_isspace(c) != 0) && ((flag & 4) == 0);

    rv = (ipr || ipu || isp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WHITESPACE - return TRUE iff S contains only whitespace */

static int _SC_whitespace(char *s)
   {int c, rv;

    rv = FALSE;
    switch (*s)
       {case '\0' :
	case '\n' :
	     rv = TRUE;
	     break;

	default :
	     while (TRUE)
	        {c = *s++;
		 if (c == '\0')
		    {rv = TRUE;
		     break;};

		 if (strchr(" \t\r\n", c) == NULL)
		    break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SC_BLANKP - return TRUE if the string s is blank or a comment
 *           - and FALSE otherwise
 *           - this uses a different metric than SC_blankl
 *           - for example if chr = "#"
 *           - S = "#foo" is not a comment
 *           - S = "# foo" is a comment
 */
 
int SC_blankp(char *s, char *chr)
   {int rv;

    rv = FALSE;
    if (strchr(chr, (int) s[0]) != NULL)
       {if (strchr(" \t\r\n", s[1]) != NULL)
           rv = TRUE;}

    else
       rv = _SC_whitespace(s);

    return(rv);}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SC_BLANKL - return TRUE if the string s is blank or a comment
 *           - and FALSE otherwise
 *           - this uses a different metric than SC_blankp
 *           - for example if chr = "#"
 *           - S = "#foo" is a comment
 *           - S = "# foo" is a comment
 */
 
int SC_blankl(char *s, char *chr)
   {int rv;

    rv = FALSE;
    if (strchr(chr, (int) s[0]) != NULL)
       rv = TRUE;

    else
       rv = _SC_whitespace(s);

    return(rv);}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SC_PRINT_CHARSP - return TRUE if the string S contains only
 *                 - alphabetic, numeric, or punctuation characters
 *                 - and FALSE otherwise
 *                 - if SP is TRUE blanks are OK
 */
 
int SC_print_charsp(char *s, int sp)
   {int i, c, n, wsp, ret;

    ret = TRUE;
    wsp = (sp == TRUE) ? 0 : 4;

    n = strlen(s);
    for (i = 0; i < n; i++)
        {c = s[i];
	 if (!SC_is_print_char(c, wsp))
	    {ret = FALSE;
	     break;};};
	    
    return(ret);}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_NUMSTRP - tests to see if a string is a number */

int SC_numstrp(char *s)
   {int rv;

    rv = (SC_intstrp(s, SC_gs.radix) || SC_fltstrp(s));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INTSTRP - tests to see if a string is an integer in radix BASE */

int SC_intstrp(char *s, int base)
   {int rv;
    char *pt;

    if (s == NULL)
       rv = FALSE;

    else if (!SC_gs.unary_plus && (*s == '+'))
       rv = FALSE;

    else if ((strcmp(s, "+") == 0) || (strcmp(s, "-") == 0))
       rv = FALSE;

    else
       {pt = s;
	if ((strlen(pt) == 1) && (strchr("0123456789", (int) *pt) == NULL))
	   rv = FALSE;

	else
	   {STRTOL(s, &pt, base);

	    rv = (pt == (s + strlen(s)));};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FLTSTRP - tests to see if a string is a float */

int SC_fltstrp(char *s)
   {int n, rv;
    char *pt;

    if (s == NULL)
       rv = FALSE;

    else if (!SC_gs.unary_plus && (*s == '+'))
       rv = FALSE;

    else if ((strcmp(s, "+") == 0) ||
	     (strcmp(s, "-") == 0) ||
	     (strcmp(s, ".") == 0) ||
	     (strcmp(s, "..") == 0))
       rv = FALSE;

    else
       {n  = strlen(s);
	pt = s;
	if ((strchr("+-.0123456789", (int) s[0]) == NULL) ||
	    (strchr(".0123456789", (int) s[n-1]) == NULL))
	   rv = FALSE;

	else
	   {STRTOD(s, &pt);

	    rv = (pt == (s + n));};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHRSTRP - test whether a string is made up of printable characters */

int SC_chrstrp(char *s)
   {int rv;
    char *sp;

    rv = TRUE;
    sp = s;

    if (s == NULL)
       rv = FALSE;

    else
       {while ((rv == TRUE) && (*sp != '\0'))
	   {rv = SC_is_print_char(*sp, 4);
	    sp++;};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                       STRING TO NUMBER ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* SC_STOC - string to double _Complex
 *         - returns 0.0 if string is NULL
 */

long double _Complex SC_stoc(char *s)
   {int c, num;
    long double re, im, p1;
    long double _Complex rv;
    char *ps;

    rv = 0.0;
    if (s != NULL)
       {p1 = STRTOD(s, &ps);
	re = 0.0;
	im = 0.0;

	num = 0;
	while ((c = *ps++) != 0)
	   {if ((c == ' ') || (c == '\t') || (c == '*'))
	       continue;
	    else if ((isdigit(c) != FALSE) || (c == '+') || (c == '-'))
               {re  = p1;
                num = 1;
		break;}
	    else if (c == 'I')
	       {im  = p1;
                num = -1;
		break;}
	    else
	       break;};

	if (num == 1)
	   {num = 2;
	    im  = STRTOD(ps, &ps);}
	else if (num == -1)
	   {num = -2;
	    re  = STRTOD(ps, &ps);};

	rv = re + im*I;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STOF - string to double
 *         - returns 0.0 if string is NULL
 */

double SC_stof(char *s)
   {double rv;

    rv = 0.0;
    if (s != NULL)
       rv = ATOF(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STOI - string to integer
 *         - returns 0 if string is NULL
 */

int64_t SC_stoi(char *s)
   {int64_t rv;

    rv = 0;
    if (s != NULL)
       rv = atoi(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STOL - string to long integer
 *         - returns 0 if string is NULL
 */

int64_t SC_stol(char *s)
   {int64_t rv;

    rv = 0L;
    if (s != NULL)
       rv = atoll(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
        
/* SC_ATOF - a fast, working version of ATOF */

double SC_atof(const char *ps)
   {int sign;
    int val, i, pwr, exponent;
    double accum, rv;
    char *s, *p;

    rv = 0.0;
    if (ps != NULL)
       {sign  = 1;
	s     = (char *) ps;
	accum = 0;

/* get past white space */
	for (p = s; (*p == ' ') || (*p == '\n') || (*p == '\t'); p++);
        
	if ((*p == '+') || (*p == '-'))
	   sign = (*(p++) == '+') ? 1 : -1;

	for (val = 0, i = 0; (*p >= '0') && (*p <= '9'); p++, i++)
	    {if (i < 4)
	        val = 10*val + *p - '0';
	     else
	        {accum = accum*1.0e4 + val;
		 val   = *p - '0';
		 i     = 0;};};

	if (*p == '.')
	   p++;

	for (pwr = 0; (*p >= '0') && (*p <= '9'); p++, i++)
	    {if (i < 4)
	        {val = 10*val + *p - '0';
		 pwr--;}
	     else
	        {accum = accum*1.0e4 + val;
		 val   = *p - '0';
		 pwr--;
		 i = 0;};};

	if (i != 0)
	   {for (; i < 4; i++, pwr--)
	        val *= 10;
	    accum = accum*1.0e4 + val;};

	accum *= sign;
        
	if ((*p == 'D') || (*p == 'E') ||
	    (*p == 'd') || (*p == 'e'))
	   p++;

	sign = 1;
	if ((*p == '+') || (*p == '-'))
	   sign = (*(p++) == '+') ? 1 : -1;

	for (exponent = 0; (*p >= '0') && (*p <= '9'); p++)
	    exponent = 10*exponent + *p - '0';

	exponent *= sign;
	pwr      += exponent;

	rv = accum*pow(10.0, (double) pwr);
	ps = p;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRTOD - correct version of C library function strtod */

double SC_strtod(const char *nptr, char **endptr)
   {int dpseen;
    char next;                            /* next character in ascii string */
    char *ptr;                                   /* pointer to ascii string */
    double rv;

    dpseen = FALSE;

/* skip the white space */
    ptr  = (char *) nptr;
    next = *ptr;
    while (SC_is_print_char(next, 3))
       next = *++ptr;

/* collect the fraction digits, record decimal point position if present */
    if ((next == '+') || (next == '-') || (next == '.'))
       {next = *++ptr;
        if (next == '.')
           {next = *++ptr;
            dpseen = TRUE;};};

/* illegal digit string in fraction */
    if (((next < '0') || (next > '9')) && (next != '.'))
       {errno = EDOM;
        if (endptr != NULL)
           *endptr = ptr;
        return(0);};

    while (((next >= '0') && (next <= '9')) || (next == '.'))
       {switch(next)
           {case '.' : if (dpseen)   /* illegal string - two decimal points */
                          {errno = EDOM;
                           if (endptr != NULL)
                              *endptr = ptr;
                           return(0);};
            default  : break;};
        next = *++ptr;};

/* collect exponent digits, if present */
    if ((next == 'e') || (next == 'E') ||
        (next == 'd') || (next == 'D') ||
	(next == '+') || (next == '-'))
       {next = *++ptr;

        if ((next == '+') || (next == '-'))
           next = *++ptr;

/* illegal digit string in exponent */
        if (!SC_isdigit((int) next))
           {errno = EDOM;
            if (endptr != NULL)
               *endptr = ptr;
            return(0.0);};

        while (SC_isdigit((int) next))
           next = *++ptr;};

/* return, setting endptr appropriately */
    if (endptr != NULL)
       *endptr = ptr;

    rv = SC_atof(nptr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRTOL - strtol done right (since some libraries are bad) */

long SC_strtol(char *str, char **ptr, int base)
   {int c, xx, neg;
    long val, rv;

    neg = 0;

/* in case no number is formed */
    if (ptr != (char **) 0)
       *ptr = str;

/* base is invalid -- should be a fatal error */
    if (base < 0 || base > MBASE)
       return(0);

    if (!SC_isalnum(c = *str))
       {while (SC_is_print_char(c, 3))
           c = *++str;
        switch (c)
           {case '-' : neg++;
            case '+' : c = *++str;};};                      /* fall-through */

    if (base == 0)
       {if (c != '0')
           base = 10;
        else if (str[1] == 'x' || str[1] == 'X')
           base = 16;
	else
           base = 8;};

/* for any base > 10, the digits incrementally following
 * 9 are assumed to be "abc...z" or "ABC...Z"
 */
    if (!SC_isalnum(c) || (xx = SC_ishexdigit(c)) >= base)
       return(0);                                       /* no number formed */

/* skip over leading "0x" or "0X" */
    if ((base == 16) && (c == '0') && SC_isxdigit((int) str[2]) &&
        ((str[1] == 'x') || (str[1] == 'X')))
       c = *(str += 2);

/* accumulate neg avoids surprises near MAXLONG */
    for (val = -SC_ishexdigit(c);
         SC_isalnum(c = *++str) && (xx = SC_ishexdigit(c)) < base; )
        val = base * val - xx;

    if (ptr != (char **) 0)
       *ptr = str;

    rv = neg ? val : -val;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OTOL - string representation of octal number to long int converter */

long int _SC_otol(char *str)
   {long int rv;

    rv = STRTOL(str, NULL, 8);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HTOL - string representation of hex number to long int converter */

long int _SC_htol(char *str)
   {long int rv;

    rv = STRTOL(str, NULL, 16);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_VDSNPRINTF - sort of dynamic version of vnsprintf
 *               - if CP is TRUE return a newly allocate string which the
 *               - caller must free
 *               - if CP is FALSE return a string which the caller
 *               - caller must NOT free
 *               - use in place of vsnprintf
 *               - thread safe
 */

char *SC_vdsnprintf(int cp, const char *format, va_list lst)
   {int rv, nc;
    char *ps, *a, **pa;
    static int ita = -1;

    if (ita < 0)
       ita = SC_define_thread_array("vdsnprintf", "char *", 1, NULL);

    pa = (char **) SC_get_thread_array(ita);
    if (*pa == NULL)
       *pa = CPMAKE_N(char, 4096, 3);

    a  = *pa;
    nc = SC_arrlen(a);

    SC_VA_SAVE(&lst);

    rv = vsnprintf(a, nc, format, lst);

/* on error return NULL */
    if (rv < 0)
       ps = NULL;

/* if too small, resize the buffer and try again */
    else if (rv >= nc)
       {nc = 2*rv;
        CREMAKE(a, char, nc);
	*pa = a;
        rv  = vsnprintf(a, nc, format, __a__);
        ps  = a;}
 
 /* return the buffer */
    else
       ps = a;

    SC_VA_RESTORE(&lst);

    if ((cp == TRUE) && (ps != NULL))
       ps = CSTRSAVE(ps);

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DSNPRINTF - sort of dynamic version of vnsprintf
 *              - if CP is TRUE return a newly allocate string which the
 *              - caller must free
 *              - if CP is FALSE return a string which the caller
 *              - caller must NOT free
 *              - use in place of snprintf
 */

char *SC_dsnprintf(int cp, char *fmt, ...)
   {char *s;

    SC_VDSNPRINTF(cp, s, fmt);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_VSNPRINTF - SCORE version of vsprintf when system one is buggy
 *              - or absent as it is on IRIX
 */

int SC_vsnprintf(char *dst, size_t nc, const char *fmt, va_list __a__)
   {int c, nd;
    char s[MAXLINE], local[MAXLINE], chr[2], ce;
    char *le, *lb, *pt, *t;
    void *p;
    int iv;
    long lv;
    int64_t bv;
    double dv;

    dst[0] = '\0';
    chr[1] = '\0';

    nd = 0;
    while (TRUE)
       {for (pt = local; (((c = *fmt++) != '%') && (c != '\0')); pt++)
	    *pt = c;

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
	         t  = SC_VA_ARG(char *);
		 nd = _SC_fmt_strcat(dst, nc, nd, local, t);
		 if (nd == -1)
		    return(-1);
                 break;

            case 'c' :
	         chr[0] = (char) SC_VA_ARG(int);
		 CAT(dst, nc, nd, chr);
                 break;

            case 'p' :
	         p = SC_VA_ARG(void *);
		 snprintf(s, MAXLINE, local, p);
		 CAT(dst, nc, nd, s);
                 break;

            case 'i' :
            case 'X' :
            case 'x' :
            case 'o' :
            case 'd' :
            case 'u' :
	         if (le[-1] == 'l')
		    {if (le[-2] == 'l')
		        {bv = SC_VA_ARG(long long);
			 snprintf(s, MAXLINE, local, bv);}
		     else
		        {lv = SC_VA_ARG(long);
			 snprintf(s, MAXLINE, local, lv);};}
		 else
		    {iv = SC_VA_ARG(int);
		     snprintf(s, MAXLINE, local, iv);};
		 CAT(dst, nc, nd, s);
                 break;

            case 'f' : 
            case 'e' :
            case 'E' : 
            case 'g' : 
            case 'G' :
	         dv = SC_VA_ARG(double);
		 snprintf(s, MAXLINE, local, dv);
		 CAT(dst, nc, nd, s);
                 break;

            case '%' :
                 CAT(dst, nc, nd, "%");
                 break;};};

    nc = strlen(dst);

    return(nc);}

/*--------------------------------------------------------------------------*/

/*                           STRING SORT ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_STRING_SORT - sort an array of character pointers
 *                - by what they point to
 */

void SC_string_sort(char **v, int n)
   {int gap, i, j;
    char *temp;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i-gap; j >= 0; j -= gap)
                {if (strcmp(v[j], v[j+gap]) <= 0)
                    break;
                 temp     = v[j];
                 v[j]     = v[j+gap];
                 v[j+gap] = temp;};
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TEXT_SORT - sort an array of text whose entries are NC long
 *              - NOTE: to avoid allocating space when the heap may
 *              - be trashed (called by _SC_mem_list) and a thread lock
 *              - (also via _SC_mem_list) use a static buffer and accept
 *              - a limit on the entry size, NC, of MAX_BFSZ
 */

void SC_text_sort(char *v, int n, int nc)
   {int gap, i, j;
    char vt[MAX_BFSZ];
    char *v1, *v2;

    nc = min(nc, MAX_BFSZ);

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i-gap; j >= 0; j -= gap)
                {v1 = v + nc*j;
		 v2 = v + nc*(j+gap);
		 if (strcmp(v1, v2) <= 0)
                    break;
                 SC_strncpy(vt, MAX_BFSZ, v1, nc);
                 strncpy(v1, v2, nc);
                 strncpy(v2, vt, nc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/