/*
 * LIBPSH.C - routines supporting simply portable C programs
 *          - which do the same kind of work as shell scripts
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBPSH

# define LIBPSH

# include "libfio.c"
# include "liblog.c"

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

enum e_token_flag
   {NO_TOKEN           = 0,
    EXPAND_ESC         = 0x1,
    ADD_DELIMITER      = 0x2,
    TRANSPARENT_QUOTES = 0x4,
    REMOVE_QUOTES      = 0x8,
    SINGLE_QUOTES      = 0x10};

typedef enum e_token_flag token_flag;

typedef struct s_dir_stack dir_stack;

struct s_dir_stack
   {int n;
    char *dir[N_STACK];};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

/* define only for SCOPE_SCORE_COMPILE */

#define UNDEFINED  "--undefd--"

static char
 epath[BFLRG],
 lpath[BFLRG];

static dir_stack
 dstck;

static int
 db_log_level = 1;

int
 _assert_fail = 0;

extern void
 unamef(char *s, int nc, char *wh);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSTRSAVE - safe strdup function */

char *nstrsave(char *s)
   {char *d;

    d = NULL;
    if (s != NULL)
       d = strdup(s);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSTRNCPY - this is to strncpy as snprintf is to sprintf
 *          - a safe string copy function
 *          - unlike strncpy this always returns a null terminated string
 */

char *nstrncpy(char *d, size_t nd, char *s, size_t ns)
   {size_t nc;
        
    if (d != NULL)
       d[0] = '\0';

    if ((IS_NULL(s) == FALSE) && (d != NULL))
       {nc = min(ns, nd-1);
	nc = max(nc, 0);
	strncpy(d, s, nc);
	d[nc] = '\0';};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSTRCAT - safe strcat function */

char *nstrcat(char *d, int nc, char *s)
   {int n, nd, ns;

    if (d != NULL)
       {nd = strlen(d);
	ns = strlen(s);
	n  = nc - 1 - nd;
	n  = vlimit(n, 0, ns);

	nstrncpy(d+nd, n+1, s, -1);
	d[nd+n] = '\0';};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VSTRCAT - safe strcat function */

char *vstrcat(char *d, int nc, char *fmt, ...)
   {char s[BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    d = nstrcat(d, nc, s);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRING_SORT - sort an array of character pointers
 *             - by what they point to
 */

void string_sort(char **v, int n)
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

/* LST_LENGTH - return the length of the list */

int lst_length(char **lst)
   {int n;

    n = 0;
    if (lst != NULL)
       {for (n = 0; lst[n] != NULL; n++);};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_PUSH - add S to list LST */

char **lst_push(char **lst, char *fmt, ...)
   {int n, m;
    char s[BFLRG];

    if (lst == NULL)
       {lst = MAKE_N(char *, 100);
	if (lst != NULL)
	   memset(lst, 0, 100*sizeof(char *));};

    if (lst != NULL)
       {n = lst_length(lst);
	if (n % 100 == 98)
	   {m = n + 102;
	    REMAKE(lst, char *, m);
	    if (lst != NULL)
	       memset(lst+n, 0, 102*sizeof(char *));};

	if (lst != NULL)
	   {if (fmt == NULL)
	       lst[n] = NULL;
	    else
	       {VA_START(fmt);
		VSNPRINTF(s, BFLRG, fmt);
		VA_END;

		lst[n] = STRSAVE(s);};};};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_ADD - add S to list LST */

char **lst_add(char **lst, char *s)
   {int n, m;

    if (lst == NULL)
       {lst = MAKE_N(char *, 100);
	if (lst != NULL)
	   memset(lst, 0, 100*sizeof(char *));};

    if (lst != NULL)
       {n = lst_length(lst);
	if (n % 100 == 98)
	   {m = n + 102;
	    REMAKE(lst, char *, m);
	    if (lst != NULL)
	       memset(lst+n, 0, 102*sizeof(char *));};

	if (lst != NULL)
	   {if (s == NULL)
	       lst[n] = NULL;
	    else
	       lst[n] = STRSAVE(s);};};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_COPY - return a copy of LST */

char **lst_copy(char **lst)
   {int i, h, n, m;
    char **nlst;

    nlst = NULL;

    if (lst != NULL)
       {n = lst_length(lst);
        h = (n + 99) / 100;
        m = 100 * h;

        nlst = MAKE_N(char *, m);
	if (nlst != NULL)
	   {memset(nlst, 0, m*sizeof(char *));

	    for (i = 0; i < n; i++)
	        nlst[i] = STRSAVE(lst[i]);};};

    return(nlst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_UNIQ - remove duplicate entries from LST */

char **lst_uniq(char **lst)
   {int i, j, k, n;
    char *s;

    if (lst != NULL)
       {n = lst_length(lst);
        for (i = 0; i < n; i++)
	    {s = lst[i];
	     for (j = i+1; j < n; j++)
	         {if (strcmp(s, lst[j]) == 0)
		     {for (k = j+1; k < n; k++)
			  lst[k-1] = lst[k];
		      lst[k-1] = NULL;
		      n--;
		      i--;
		      break;};};};};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_FREE - free the list */

void lst_free(char **lst)
   {int i, n;

    if (lst != NULL)
       {n = lst_length(lst);
	for (i = 0; i < n; i++)
	    FREE(lst[i]);

	FREE(lst);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONCATENATE - concatenate tokens from MN to MX of SA
 *             - to string make S
 */

char *concatenate(char *s, int nc, char **sa,
		  unsigned int mn, unsigned int mx, char *dlm)
   {int ns, nd, n;
    unsigned int i;

    if ((s != NULL) && (nc > 0))
       {memset(s, 0, nc);

	if (sa != NULL)
	   {mn = max(mn, 0);
	    for (i = mn; (sa[i] != NULL) && (i < mx); i++)
	        vstrcat(s, nc, "%s%s", sa[i], dlm);

	    ns = strlen(s);
	    nd = strlen(dlm);
	    n  = ns - nd;
	    n  = max(n, 0);
	    s[n] = '\0';};};

    return(s);}

/*--------------------------------------------------------------------------*/

#ifdef NO_UNSETENV

/*--------------------------------------------------------------------------*/

/* UNSETENV - unset the environment variable VAR */

int unsetenv(char *var)
   {int i, nc, ne, err;
    char s[BFLRG];
    extern char **environ;

    err = 0;

/* count the environment variables */
    for (ne = 0; environ[ne] != NULL; ne++);

    snprintf(s, BFLRG, "%s=", var);
    nc = strlen(s);

    for (i = 0; i < ne; i++)
        {if (strncmp(s, environ[i], nc) == 0)
            {ne--;
	     environ[i]  = environ[ne];
             environ[ne] = NULL;
	     break;};};

    return(err);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* TRIM - trim any characters contained in DELIM from the front or back
 *      - of the string S
 */

char *trim(char *s, int dir, char *delim)
   {int ic, nc, c;

    if (s != NULL)
       {if ((dir & FRONT) != 0)
	   for (; (*s != '\0') && (strchr(delim, *s) != NULL); s++);

/* do nothing with zero length strings */
	if ((dir & BACK) != 0)
	   {nc = strlen(s);
	    for (ic = nc - 1; ic >= 0; ic--)
	        {c = s[ic];
		 if (strchr(delim, c) == NULL)
		    break;
		 s[ic] = '\0';};};};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRCNTC - return the number of occurences of C in S
 *         - if EX is TRUE do not count escaped instances
 */

int strcntc(char *s, int c, int ex)
   {int i, nc;

    nc = 0;
    if (s != NULL)
       {for (i = 0, nc = 0; s[i] != '\0'; i++)
	    nc += (s[i] == c);};
        
    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRCNTS - count the number of occurences of any of the
 *         - the specified characters R
 *         - in the given string S
 *         - if EX is TRUE do not count escaped instances
 */

int strcnts(char *s, char *r, int ex)
   {int l, count;

    count = 0;
    while ((l = *s++) != '\0')
       count += (strchr(r, l) != NULL);

    return(count);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _STRCPY_NEXT - copy S into D upto the first unescaped occurence of
 *              - any character in R
 *              - return number of characters copied
 *              - features:
 *              -   1) copy no more than min of ND and NS characters
 *              -   2) do not check delimiters in quoted substrings
 *              -      S = "a 'b;c' ; d" and R = ";"
 *              -      gives D = "a 'b;c' "
 *              -   3) return the number of characters copied from S
 *              -      this can be greater than the number copied into D
 *              -      strlen will tell you how many were copied into D
 *              -      but cannot account for escaped characters from S
 *              -   4) FLAGS it a bit array controlling additional aspects
 *              -      of operation:
 *              -        EXPAND_ESC     
 *              -           if set make the copy of an escaped
 *              -           character unescaped, that is:
 *              -              "a\bc" -> "abc" or "a\\\"bc" -> "a\"bc"
 *              -           otherwise make the copy of an escaped
 *              -           character escaped, that is:
 *              -              "a\bc" -> "a\bc" or "a\\\"bc" -> "a\\\"bc"
 *              -        ADD_DELIMITER
 *              -           if set include the end delimiter in the
 *              -           resultant string D
 *              -              "a b;" with " ;" -> "a " and "b;"
 *              -        TRANSPARENT_QUOTES
 *              -           if set treat quoted substrings as transparent
 *              -           and look at the characters inside them
 *              -           otherwise treat them as opaque and copy them
 *              -           to D without looking at the characters inside
 *              -        REMOVE_QUOTES
 *              -           if set do not copy quote characters into D
 *              -           otherwise do copy them into D
 *              -              "ab'cd'ef" -> "abcdef"
 *              -   5) this is designed to handle things like:
 *              -         "(a\(bc\)de)" + ")" -> "(a\(bc\)de"
 *              -      but NOT searches for matching delimiters
 *              -         "(a(bc)de)" + ")" -> "(a(bc)de)"
 */

static int _strcpy_next(char *d, size_t nd, char *s, size_t ns,
			char *r, int flags)
   {int ins, ind, c, ex, ad, tr, rq;
    size_t n, nc;

    n = 0;

    if ((s != NULL) && (d != NULL))
       {ex = ((flags & EXPAND_ESC) != 0);
	ad = ((flags & ADD_DELIMITER) != 0);
	tr = ((flags & TRANSPARENT_QUOTES) != 0);
	rq = ((flags & REMOVE_QUOTES) != 0);

/* quotes must be transparent if they are delimiters */
	tr |= ((strchr(r, '\"') != NULL) || (strchr(r, '\'') != NULL));

	ins = FALSE;
	ind = FALSE;

	nc = strlen(s);
	nc = min(nc, nd-1);
	nc = min(nc, ns);

	for (n = 0; n < nc; n++)
	    {c = *s++;

/* handle escaped characters */
             if (c == '\\')
                {if (ex == FALSE)
		    *d++ = c;
                 *d++ = *s++;
		 n++;}

/* arrange to skip over quoted substrings - making them opaque */
	     else if ((tr == FALSE) && (c == '\"'))
	        {ind = !ind;
                 if (rq == FALSE)
		    *d++ = c;}

	     else if ((tr == FALSE) && (c == '\''))
	        {ins = !ins;
                 if (rq == FALSE)
		    *d++ = c;}

/* copy over non-delimiting characters */
	     else if ((ins == TRUE) || (ind == TRUE) ||
		      (strchr(r, c) == NULL))
	        *d++ = c;

/* it is not escaped and it is a delimiting character */
	     else
                {if (ad == TRUE)
		    *d++ = c;
		 break;};};

	if (ad == TRUE)
	   *d++ = ' ';
	*d++ = '\0';};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRCPY_STR - copy S into D upto the first unescaped occurence of
 *            - the string R
 *            - this is to be used instead of strstr when dealing
 *            - with text that has delimited (hence off limits) regions
 *            - especially with escaped characters in the text
 *            - features:
 *            -   1) copy no more than min of ND and NS characters
 *            -   2) do not check delimiters in quoted substrings
 *            -      S = "a, 'b;c,' c," and R = "c,"
 *            -      gives D = "a, 'b;c,' "
 *            -   3) return the number of characters copied from S
 *            -      this can be greater than the number copied into D
 *            -      strlen will tell you how many were copied into D
 *            -      but cannot account for escaped characters from S
 *            -   4) FLAGS it a bit array controlling additional aspects
 *            -      of operation:
 *            -        EXPAND_ESC     
 *            -           if set make the copy of an escaped
 *            -           character unescaped, that is:
 *            -              "a\bc" -> "abc" or "a\\\"bc" -> "a\"bc"
 *            -           otherwise make the copy of an escaped
 *            -           character escaped, that is:
 *            -              "a\bc" -> "a\bc" or "a\\\"bc" -> "a\\\"bc"
 *            -        ADD_DELIMITER
 *            -           if set include the delimiter in the resultant
 *            -           token
 *            -        TRANSPARENT_QUOTES
 *            -           if set treat quoted substrings as transparent
 *            -           and look at the characters inside them
 *            -           otherwise treat them as opaque and copy them
 *            -           to D without looking at the characters inside
 *            -        REMOVE_QUOTES
 *            -           if set do not copy quote characters into D
 *            -           otherwise do copy them into D
 *            -              "ab'cd'ef" -> "abcdef"
 *            -        SINGLE_QUOTES
 *            -           if set ignore unmatched quote characters
 *            -           at the place where an ending delimiter is found
 *            -              "(ab'cd)" + ")" -> "(ab'cd)"
 *            -   5) this is designed to handle things like:
 *            -         "(a\(be\)de)" + "e)" -> "(a\(be\)d"
 *            -      but NOT searches for matching delimiters
 *            -         "(a(be)de)" + "e)" -> "(a(be)de)"
 */

int strcpy_str(char *d, size_t nd, char *s, size_t ns, char *r, int flags)
   {int ins, ind, c, ex, ad, tr, rq, sq;
    size_t n, nc, nr;

    n = 0;

    if ((s != NULL) && (d != NULL))
       {ex = ((flags & EXPAND_ESC) != 0);
	ad = ((flags & ADD_DELIMITER) != 0);
	tr = ((flags & TRANSPARENT_QUOTES) != 0);
	rq = ((flags & REMOVE_QUOTES) != 0);
	sq = ((flags & SINGLE_QUOTES) != 0);

/* quotes must be transparent if they are delimiters */
	tr |= ((strchr(r, '\"') != NULL) || (strchr(r, '\'') != NULL));

	ins = FALSE;
	ind = FALSE;

	nr = strlen(r);
	nc = strlen(s);
	nc = min(nc, nd-1);
	nc = min(nc, ns);

	for (n = 0; n < nc; n++)
	    {c = *s++;

/* handle escaped characters */
             if (c == '\\')
                {if (ex == FALSE)
		    *d++ = c;
                 *d++ = *s++;
		 n++;}

/* arrange to skip over quoted substrings - making them opaque */
	     else if ((tr == FALSE) && (c == '\"'))
	        {ins = FALSE;      /* clear any lone single quote */
		 ind = !ind;
		 if (strchr(s, '\"') != NULL)
		    sq = FALSE;
                 if (rq == FALSE)
		    *d++ = c;}

	     else if ((tr == FALSE) && (c == '\''))
	        {ind = FALSE;      /* clear any lone double quote */
		 ins = !ins;
		 if (strchr(s, '\'') != NULL)
		    sq = FALSE;
                 if (rq == FALSE)
		    *d++ = c;}

/* copy over non-delimiting characters */
	     else if (((sq == FALSE) && (ins == TRUE)) ||
		      ((sq == FALSE) && (ind == TRUE)) ||
		      (c != r[0]) || (strncmp(r, s-1, nr) != 0))
	        *d++ = c;

/* it is not escaped and it is a delimiting string */
	     else
                {if (ad == TRUE)
		    {nstrncpy(d, nd-nr, r, -1);
		     d += nr;
		     n += nr;
		     n = min(n, nc);};
		 break;};};

	if (ad == TRUE)
	   *d++ = ' ';
	*d++ = '\0';};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _STRCPY_BAL - copy S into D upto the first unescaped occurence of
 *             - any character in B and E that is not balanced
 *             - features:
 *             -   1) copy no more than min of ND and NS characters
 *             -   2) do not check delimiters in quoted substrings
 *             -      S = "(a '(bc)' d)efg" and B = "(" and E = ")"
 *             -      gives D = "(a '(bc)' d)"
 *             -   3) return the number of characters copied from S
 *             -      this can be greater than the number copied into D
 *             -      strlen will tell you how many were copied into D
 *             -      but cannot account for escaped characters from S
 *             -   4) FLAGS it a bit array controlling additional aspects
 *             -      of operation:
 *             -        EXPAND_ESC     
 *             -           if set make the copy of an escaped
 *             -           character unescaped, that is:
 *             -              "a\bc" -> "abc" or "a\\\"bc" -> "a\"bc"
 *             -           otherwise make the copy of an escaped
 *             -           character escaped, that is:
 *             -              "a\bc" -> "a\bc" or "a\\\"bc" -> "a\\\"bc"
 *             -        ADD_DELIMITER
 *             -           if set include the end delimiter in the
 *             -           resultant string D
 *             -        TRANSPARENT_QUOTES
 *             -           if set treat quoted substrings as transparent
 *             -           and look at the characters inside them
 *             -           otherwise treat them as opaque and copy them
 *             -           to D without looking at the characters inside
 *             -        REMOVE_QUOTES
 *             -           if set do not copy quote characters into D
 *             -           otherwise do copy them into D
 *             -              "ab'cd'ef" -> "abcdef"
 *             -   5) this is designed to handle matching delimiters
 *             -         "(a(bc)de)" + ")" -> "(a(bc)de)"
 *             -      but NOT searches for things like:
 *             -         "(a\(bc\)de)" + ")" -> "(a\(bc\)de"
 */

static int _strcpy_bal(char *d, size_t nd, char *s, size_t ns,
		       char *b, char *e, int flags)
   {int ins, ind, lev, c, ex, ad, tr, rq;
    size_t n, nc;

    n = 0;

    if ((s != NULL) && (d != NULL))
       {ex = ((flags & EXPAND_ESC) != 0);
	ad = ((flags & ADD_DELIMITER) != 0);
	tr = ((flags & TRANSPARENT_QUOTES) != 0);
	rq = ((flags & REMOVE_QUOTES) != 0);

/* quotes must be transparent if they are delimiters */
	tr |= ((strchr(b, '\"') != NULL) || (strchr(b, '\'') != NULL));
	tr |= ((strchr(e, '\"') != NULL) || (strchr(e, '\'') != NULL));

	ins = FALSE;
	ind = FALSE;

	nc = strlen(s);
	nc = min(nc, nd-1);
	nc = min(nc, ns);

/* start lev properly
 *   "(a b c)" with b = "(" should have lev = 0
 *   "a b c)" with b = "(" should have lev = 1
 */
	lev = ((b == NULL) || (strchr(b, s[0]) == NULL));

	for (n = 0; (n < nc) && ((0 < lev) || (n == 0)); n++)
	    {c = *s++;

/* handle escaped characters */
             if (c == '\\')
                {if (ex == FALSE)
		    *d++ = c;
                 *d++ = *s++;
		 n++;}

/* arrange to skip over quoted substrings - making them opaque */
	     else if ((tr == FALSE) && (c == '\"'))
	        {ind = !ind;
                 if (rq == FALSE)
		    *d++ = c;}

	     else if ((tr == FALSE) && (c == '\''))
	        {ins = !ins;
                 if (rq == FALSE)
		    *d++ = c;}

/* copy over non-delimiting characters */
	     else if ((ins == TRUE) || (ind == TRUE))
	        *d++ = c;

	     else if ((b != NULL) && (strchr(b, c) != NULL))
	        {lev++;
		 *d++ = c;}

	     else if ((e != NULL) && (strchr(e, c) != NULL))
	        {lev--;
		 *d++ = c;}

/* it is not escaped and it is not a delimiting character */
	     else
	        *d++ = c;};

	if (ad == TRUE)
	   *d++ = c;
	*d++ = '\0';};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRCPY_TOK - better tokenizer
 *            - this is to be used instead of strtok when dealing
 *            - with text that has delimited (hence off limits) regions
 *            - especially with escaped characters in the text
 */

int strcpy_tok(char *d, size_t nd, char *s, size_t ns,
	       char *b, char *e, int flags)
   {int rv;

    rv = -1;
    if ((d != NULL) && (s != NULL))
       {d[0] = '\0';
	if ((b == NULL) && (e == NULL))
	   rv = 0;
        else if (b == NULL)
	   rv = _strcpy_next(d, nd, s, ns, e, flags);
	else if (e == NULL)
	   rv = 0;
	else
	   rv = _strcpy_bal(d, nd, s, ns, b, e, flags);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DELIMITED - return substring of S between matched delimiters BGN and END
 *           - example:
 *           -   "aa (foo(bar)) zz"  with  "(" and ")"
 *           - should return "foo(bar)"
 *           - NOTE: S will be terminated at the first delimiter instance
 */

char *delimited(char *s, char *bgn, char *end)
   {int nc, ncb, nce;
    char *tb, *te, *val, *ps, *wh;

    val = NULL;

    if ((s != NULL) && (bgn != NULL) && (end != NULL))
       {tb    = s + strcspn(s, bgn);
	*tb++ = '\0';

	val = tb;
	nc  = 1;
	ncb = strlen(bgn);
	nce = strlen(end);

	for (ps = tb; IS_NULL(ps) == FALSE; )
	    {tb = strstr(ps, bgn);
	     te = strstr(ps, end);
	     if ((tb != NULL) && (tb[ncb] != '\'') &&
		 (te != NULL) && (te[nce] != '\''))
	        wh = (tb < te) ? tb : te;
	     else if ((tb != NULL) && (tb[ncb] != '\''))
	        wh = tb;
	     else if ((te != NULL) && (te[nce] != '\''))
	        wh = te;
	     else
	        wh = NULL;

/* no instance of BGN or END */
	     if (wh == NULL)
	        {if (nc == 0)
		    nstrcat(s, BFLRG, ps);
		 ps = NULL;}

/* if an instance of END came first
 * check end first to handle case where BGN and END are the same
 * as it is for quotes
 */
	     else if (wh == te)
	        {nc--;
		 if (nc == 0)
		    {*te = '\0';
		     ps  = NULL;}
		 else
		    ps = te + nce;}

/* if an instance of BGN came first */
	     else if (wh == tb)
	        {nc++;
		 if (nc == 1)
		    val = tb;
		 ps = tb + ncb;};};};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TOKENIZE - return an array of strings obtained by
 *          - tokenizing the string S according to the
 *          - single-character delimiters DLM
 *          - the array is terminated by a NULL string
 *          - it can be released by free_strings
 *          - FLAGS it a bit array controlling additional aspects of
 *          - the tokenization:
 *          -   EXPAND_ESC     
 *          -   ADD_DELIMITER
 */

char **tokenize(char *s, char *dlm, int flags)
   {int i, n, nw;
    char *t, *ps, **sa;

    sa = NULL;
    if (s != NULL)
       {n  = strlen(s);
	nw = n + 100;
	t  = MAKE_N(char, nw);
	if (t != NULL)
	   {nstrncpy(t, nw, s, -1);

	    sa = MAKE_N(char *, 1000);
	    if (sa != NULL)
	       {int nc;
		char *w;

		w = MAKE_N(char, nw);

		for (i = 0, ps = t; *ps != '\0'; ps++)
                    {nc = strcpy_tok(w, nw, ps, -1, NULL, dlm, flags);
		     if (nc > 0)
		        {sa[i++] = STRSAVE(w);
			 ps += nc;};};

		FREE(w);};

	    if (sa != NULL)
	       sa[i++] = NULL;

	    FREE(t);};};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TOKENIZED - return an array of strings obtained by
 *           - tokenizing the string S according to the
 *           - multi-character delimiter DLM
 *           - the array is terminated by a NULL string
 *           - it can be released by free_strings
 */

char **tokenized(char *s, char *dlm, int flags)
   {int i, n, nw;
    char *t, *ps, **sa;

    sa = NULL;
    if ((s != NULL) && (dlm != NULL))
       {n  = strlen(s);
	nw = n + 100;
	t  = MAKE_N(char, nw);
	if (t != NULL)
	   {nstrncpy(t, nw, s, -1);

	    sa = MAKE_N(char *, 1000);
	    if (sa != NULL)
	       {int nc;
		char *w;

		w = MAKE_N(char, nw);

		for (i = 0, ps = t; *ps != '\0'; ps++)
                    {nc = strcpy_str(w, nw, ps, -1, dlm, flags);
		     if (nc > 0)
		        {sa[i++] = STRSAVE(w);
			 ps += nc;};};

		FREE(w);};

	    if (sa != NULL)
	       sa[i++] = NULL;

	    FREE(t);};};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_STRINGS - free the strings in the array lst */

void free_strings(char **lst)
   {int i;

    if (lst != NULL)
       {for (i = 0; lst[i] != NULL; i++)
	    {FREE(lst[i]);};
        FREE(lst);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_NSTRINGS - free the N strings in the array lst
 *               - use this instead of free_strings if any of the
 *               - strings have been NULL'd
 */

void free_nstrings(char **lst, int n)
   {int i;

    if (lst != NULL)
       {for (i = 0; i < n; i++)
	    {if (lst[i] != NULL)
	        FREE(lst[i]);};
	FREE(lst);}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAST_CHAR - return a valid last character index into S */

int last_char(char *s)
   {int nc;

    nc = 0;
    if (s != NULL)
       {nc = strlen(s) - 1;
	nc = max(0, nc);};

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_NUMBER - return TRUE if S is a number of type mask KND
 *           - bit values of KND:
 *           -   1  integer
 *           -   2  float
 *           - so KND = 0xff would be any kind of number
 */

int is_number(char *s, int knd)
   {int rv, nc;
    char *ps;

    rv = FALSE;

    if (s != NULL)
       {nc = strlen(s);

	if (knd & 1)
           {errno = 0;
	    strtoll(s, &ps, 0);
	    rv |= (ps-s == nc);};

	if (knd & 1)
           {errno = 0;
	    strtold(s, &ps);
	    rv |= (ps-s == nc);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STOI - return the integer value of the string S
 *      - take base into account
 */

int64_t stoi(char *s)
   {int64_t rv;
    char *t, *ps;

    rv = 0;

    if (s != NULL)
       {t = trim(s, FRONT, "+- \t");

/* octal numbers are: 0d* where d is [0-7] */
	if (t[0] == '0')
	   rv = strtoll(s, &ps, 8);

/* hexidecimal numbers are: 0xd* where d is [0-9A-Fa-f] */
	else if (strncmp(t, "0x", 2) == 0)
	   rv = strtoll(s, &ps, 16);

/* Clang/GCC extension for binary numbers are: 0bd* where d is [01] */
	else if (strncmp(t, "0b", 2) == 0)
	   rv = strtoll(s, &ps, 2);

	else
	   rv = strtoll(s, &ps, 10);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UPCASE - map S to upper case */

char *upcase(char *s)
   {size_t i, nc;
        
    if (s != NULL)
       {nc = strlen(s);
	for (i = 0; i < nc; i++)
	    s[i] = toupper((int) s[i]);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOWNCASE - map S to lower case */

char *downcase(char *s)
   {size_t i, nc;
        
    if (s != NULL)
       {nc = strlen(s);
	for (i = 0; i < nc; i++)
	    s[i] = tolower((int) s[i]);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILL_STRING - fill ragged S name with "." to column N */

char *fill_string(char *s, int n)
   {int nc, nd;
    static char fill[BFLRG];

    nstrncpy(fill, BFLRG, s, -1);
    nc = strlen(fill);
    nd = n - nc;

    memset(fill+nc, '.', nd);
    fill[nd] = '\0';

    return(fill);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRCLEAN - make a copy of S which has any non-printing characters
 *          - replaced by sequences of printable characters
 *          - if the line lenght would be exceeded put '...\n'
 *          - in the end of D
 */

char *strclean(char *d, size_t nd, char *s, size_t ns)
   {size_t i, j, c;
    char t[10];
        
    if ((d != NULL) && (s != NULL))
       {c  = strlen(s);
	ns = min(ns, c);

	for (i = 0, j = 0; i < ns; i++)
	    {c = s[i];
	     if (c < ' ')
	        {switch (c)
		    {case '\f' :
		     case '\n' :
		     case '\r' :
		     case '\t' :
		          d[j++] = c;
		          break;
		     default :
		          d[j++] = '^';
		          d[j++] = 'A' + c - 1;
		          break;};}
	     else if (c > '~')
	        {d[j++] = '[';
                 snprintf(t, 10, "%03lo", (unsigned long) (c - 128));
		 d[j++] = t[0];
		 d[j++] = t[1];
		 d[j++] = t[2];}
	     else
	        d[j++] = c;

	    if (j + 7 >= nd)
	       {d[j++] = ' ';
		d[j++] = '.';
		d[j++] = '.';
		d[j++] = '.';
		d[j++] = ' ';
	        break;};};

	d[j++] = '\0';};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRSTRI - finds first occurance of string2 in string1 (case insensitive)
 *         - if not there returns NULL
 */

char *strstri(char *string1, char *string2)
   {char *s1, *s2, *s3, *rv;
        
    rv = NULL;
    s1 = string1;
    while ((*s1 != '\0') && (rv == NULL))
       {for ((s2 = string2, s3 = s1);
             (toupper((int) *s2) == toupper((int) *s3)) &&
	     (*s3 != '\0') && (*s2 != '\0');
             (s3++, s2++));

/* if s2 makes it to the end the string is found */
        if (*s2 == '\0')
           rv = s1;
        else
           s1++;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SUBST - substitute N occurences of A with B in S
 *       - like: sed 's/A/B/g'
 *       - GOTCHA: make the internal buffers dynamic
 */

char *subst(char *s, char *a, char *b, size_t n)
   {size_t i, o, nc;
    char *p, *pr, *ps, *pa, *pb, *r;
    static char bfa[1024*BFLRG], bfb[1024*BFLRG];

    if (s == bfa)
       {pa = bfb;
	pb = bfa;}
    else
       {pa = bfa;
	pb = bfb;};

    ps = pa;
	
    if ((s != NULL) && (IS_NULL(a) == FALSE) && (b != NULL))
       {nc = 1024*BFLRG;

/* make sure s is not bfa to avoid buffer overlap errors */
	nstrncpy(ps, nc, s, -1);

	o = 0;
	for (i = 0; i < n; i++)
	    {p = strstr(ps+o, a);
	     if (p != NULL)
	        {pr = p + strlen(a);
		 *p = '\0';
		 r  = (i & 1) ? pa : pb;
		 nstrncpy(r, nc, ps, -1);
		 nstrcat(r, nc, b);
		 o = strlen(r);
		 nstrcat(r, nc, pr);
		 ps = r;}
	     else
	        break;};};

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY - copy the file IN to OUT */

void copy(char *out, char *fmt, ...)
   {char in[BFLRG];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(in, BFLRG, fmt);
    VA_END;

    fp = fopen_safe(out, "w");
    if (fp != NULL)
       {cat(fp, 0, -1, in);
	fclose_safe(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_TAIL - do what ':t' does to S */

char *path_tail(char *s)
   {int nc;
    char *pd;
    static char d[BFLRG];

/* handle case of path_tail(path_tail(s)) */
    if (s != d)
       nstrncpy(d, BFLRG, s, -1);

    nc = strlen(d);
    if (nc >= BFLRG)
       nc = BFLRG;
    if (nc < 1)
       nc = 1;

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '/')
	    break;};

    pd++;

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_HEAD - do what ':h' does to S */

char *path_head(char *s)
   {int nc;
    char *pd;
    static char d[BFLRG];

/* handle case of path_head(path_head(s)) */
    if (s != d)
       nstrncpy(d, BFLRG, s, -1);

    nc = strlen(d);
    if (nc >= BFLRG)
       nc = BFLRG;
    if (nc < 1)
       nc = 1;

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '/')
	    {*pd = '\0';
	     break;};};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_BASE - do what ':r' does to S */

char *path_base(char *s)
   {int nc;
    char *pd;
    static char d[BFLRG];

/* handle case of path_base(path_base(s)) */
    if (s != d)
       nstrncpy(d, BFLRG, s, -1);

    nc = strlen(d);
    nc = vlimit(nc, 1, BFLRG);

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '.')
	    {*pd = '\0';
	     break;};};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_SUFFIX - do what :e does and return the suffix of S */

char *path_suffix(char *s)
   {int nc;
    char *pd;
    static char d[BFLRG];

    nstrncpy(d, BFLRG, s, -1);
    nc = strlen(d);
    nc = vlimit(nc, 1, BFLRG);

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '.')
	    break;};

    pd++;

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_SIMPLIFY - remove redundant entries from path type string S */

char *path_simplify(char *s, int dlm)
   {int i, j, ok;
    char *t, **sa;
    static char d[BFLRG];

    d[0] = '\0';

    sa = tokenize(s, " :", 0);
    
    if (sa != NULL)
       {nstrncpy(d, BFLRG, sa[0], -1);
	for (i = 1; sa[i] != NULL; i++)
	    {t = sa[i];

	     ok = FALSE;
	     for (j = i-1; (j >= 0) && (ok == FALSE); j--)
	         ok = (strcmp(sa[j], t) == 0);

	     if (ok == FALSE)
	        vstrcat(d, BFLRG, "%c%s", dlm, t);};

	free_strings(sa);};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PERMISSION_MATCH - return TRUE if the specified permissions MD
 *                   - match those of the stat BF
 *                   - special bits ignored currently
 *
 *           S_ISUID    0004000   set UID bit
 *           S_ISGID    0002000   set-group-ID bit (see below)
 *           S_ISVTX    0001000   sticky bit (see below)
 *
 *           S_IRUSR    0000400   owner has read permission
 *           S_IWUSR    0000200   owner has write permission
 *           S_IXUSR    0000100   owner has execute permission
 *
 *           S_IRGRP    0000040   group has read permission
 *           S_IWGRP    0000020   group has write permission
 *           S_IXGRP    0000010   group has execute permission
 *
 *           S_IROTH    0000004   others have read permission
 *           S_IWOTH    0000002   others have write permission
 *           S_IXOTH    0000001   others have execute permission
 */

#undef NGROUPX
#ifdef NGROUPS_MAX
# define NGROUPX NGROUPS_MAX
#else
# define NGROUPX 16
#endif

int _permission_match(struct stat bf, int md, int only)
   {int i, rv;
    unsigned int muid, mgid;
    int ugo[3], id[3];

/* given rwx replicate into rwxrwxrwx */
    if (md < 8)
       md = (md << 6) | (md << 3) | md;

    for (i = 0; i < 3; i++)
        ugo[i] = ((bf.st_mode & md) != 0);

/* determine user identity match */
    muid  = getuid();
    id[0] = (muid == bf.st_uid);

/* determine group identity match */
    if (only == TRUE)
       {mgid  = getgid();
	id[1] = (mgid == bf.st_gid);}
    else
       {int ig, ng;
	gid_t gl[NGROUPX+1];

	ng    = getgroups(NGROUPX, gl);
	id[1] = 0;
	for (ig = 0; (id[1] == 0) && (ig < ng); ig++)
	    id[1] |= (gl[ig] == bf.st_gid);};

/* other identity match is automatic by definition */
    id[2] = TRUE;

/* determine whether we have permissions and identity match */
    rv = FALSE;
    for (i = 0; i < 3; i++)
        rv |= (ugo[i] && id[i]);

    return(rv);}

#undef NGROUPX

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_KIND - return TRUE iff the specified path is of type KND
 *           - where KND is one of the POSIX file type flags
 *           - S_IFDIR, S_IFREG, S_IFLNK, ...
 *           - and if permissions match MD
 *
 *           S_IFMT     0170000   mask for file type
 *           S_IFSOCK   0140000   socket
 *           S_IFLNK    0120000   symbolic link
 *           S_IFREG    0100000   regular file
 *           S_IFBLK    0060000   block device
 *           S_IFDIR    0040000   directory
 *           S_IFCHR    0020000   character device
 *           S_IFIFO    0010000   FIFO
 */

int file_kind(int knd, int md, char *fnm)
   {int rv, st;
    char err[BFLRG];
    struct stat bf;

    rv = FALSE;
    st = stat(fnm, &bf);
    if (st != 0)
       nstrncpy(err, BFLRG, strerror(errno), -1);

    else if ((bf.st_mode & knd) != 0)
       rv = _permission_match(bf, md, FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIR_EXISTS - return TRUE iff the named directory exists */

int dir_exists(char *fmt, ...)
   {int rv;
    char s[BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    rv = file_kind(S_IFDIR, 7, s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_EXECUTABLE - return TRUE iff the named file is executable */

int file_executable(char *fmt, ...)
   {int rv;
    char s[BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    rv = file_kind(S_IFREG, 0111, s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_EXISTS - return TRUE iff the named file exists */

int file_exists(char *fmt, ...)
   {int rv;
    char s[BFLRG];
    struct stat sb;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    rv = FALSE;

    memset(&sb, 0, sizeof(sb));

/* regular file or character device */
    if (stat(s, &sb) == 0)
       rv = ((sb.st_mode & (S_IFREG | S_IFCHR)) != 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_SCRIPT - return TRUE iff the named file is an executable script */

int file_script(char *fmt, ...)
   {int rv;
    char s[BFLRG], t[BFLRG];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    rv = file_kind(S_IFREG | S_IFCHR, 7, s);
    if (rv == TRUE)
       {fp = fopen_safe(s, "r");
	if (fp != NULL)
	   {if (fgets(t, BFLRG, fp) != NULL)
	       rv = (strncmp(t, "#!", 2) == 0);
	    fclose_safe(fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXECUTE - execute a command and return the exit status */

int execute(int err, char *fmt, ...)
   {int rv;
    char s[BFLRG], cmd[BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (err == TRUE)
       snprintf(cmd, BFLRG, "PATH=.:%s:%s ; %s",
		epath, lpath, s);
    else
       snprintf(cmd, BFLRG, "PATH=.:%s:%s ; %s 2> /dev/null",
		epath, lpath, s);

    rv = system(cmd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN - execute a command and return the output in a string
 *     - echo the command to the log iff ECHO is TRUE
 */

char *run(int echo, char *fmt, ...)
   {char s[BFLRG], cmd[BFLRG];
    char *p;
    FILE *fp;
    static char bf[100*BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (echo & CMD_LINE)
       note(NULL, "Command: %s\n", s);

    snprintf(cmd, BFLRG, "PATH=.:%s:%s ; %s", epath, lpath, s);

    bf[0] = '\0';

    fp = popen(cmd, "r");
    if (fp != NULL)
       {while (TRUE)
	   {p = fgets(s, BFLRG, fp);
	    if (p == NULL)
	       break;
	    nstrcat(bf, 100*BFLRG, s);};

	pclose(fp);};

    LAST_CHAR(bf) = '\0';

    if (echo & CMD_OUT)
       note(NULL, "%s\n", bf);

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GREP - search FP for S
 *      - S is a simple string (no regular expressions)
 *      - assume FP is an open file an position the pointer
 *      - at the end when finished
 *      - GOTCHA: make the internal buffer dynamic
 */

char *grep(FILE *fp, char *name, char *fmt, ...)
   {int i, nc, clf, err;
    char t[BFLRG], s[BFLRG];
    char *p;
    static char r[1024*BFLRG];

    r[0] = '\0';

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (fp == NULL)
       {clf = TRUE;
	fp  = fopen_safe(name, "r");}
    else
       clf = FALSE;

    if (fp != NULL)
       {nc = 1024*BFLRG;

	fseek(fp, 0, SEEK_SET);

	for (i = 0; TRUE; i++)
	    {p = fgets(t, BFLRG, fp);
	     if (p == NULL)
	        break;

	     p = strstr(t, s);
	     if (p != NULL)
	        nstrcat(r, nc, t);};

	if (clf == TRUE)
	   err = fclose_safe(fp);
	else
	   err = fseek(fp, 0, SEEK_END);

	ASSERT(err == 0);};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_DATE - return the date */

char *get_date(void)
   {time_t t;
    char *p;
    static char s[BFLRG];

    p = NULL;
    t = time(NULL);
    if (t > 0)
       {nstrncpy(s, BFLRG, ctime(&t), -1);
	LAST_CHAR(s) = '\0';
	p = s;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSLEEP - millisecond sleep */

void nsleep(int ms)
   {

    if (ms <= 0)
       sched_yield();
    else
       poll(NULL, 0, ms);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEXT - do the required text printing with \n and \t handled
 *            - properly (some csh's print them literally instead of the
 *            - C way)
 */

void print_text(FILE *fp, char *fmt, ...)
   {char bf[BFLRG];

    VA_START(fmt);
    VSNPRINTF(bf, BFLRG, fmt);
    VA_END;

    fprintf(fp, "%s\n", bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAN_SPACE - replace '+sp+' with ' '
 *             - this has been used in connection with shell scripts
 */

void clean_space(char *s)
   {char t[BFLRG];
    char *p, *ps;

    t[0] = '\0';

    for (ps = s; *ps != '\0'; )
        {p = strstr(ps, "+sp+");
	 if (p != NULL)
	    {*p = '\0';
	     vstrcat(t, BFLRG, "%s ", ps);
	     ps = p + 4;}
	 else
	    {nstrcat(t, BFLRG, ps);
	     ps += strlen(ps);};};

    strcpy(s, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRIP_QUOTE - strip off quotes around T
 *             - also any leading whitespace before the quotes
 */

char *strip_quote(char *t)
   {int n;
    static char bf[BFLRG];

    n = strspn(t, " \t");
    nstrncpy(bf, BFLRG, t+n, -1);
    if (bf[0] == '"')
       {for (t = &LAST_CHAR(bf); (*t =='"') && (t != bf); *t-- = '\0');
	t = bf + strspn(bf, "\"");};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSETENV - set the environment variable VAR to VAL */

int csetenv(char *var, char *fmt, ...)
   {int err, nc;
    char s[BFLRG];
    char *t;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    err = 0;
    nc  = strlen(var) + strlen(s) + 2;
    t   = malloc(nc);
    if (t != NULL)
       {snprintf(t, nc, "%s=%s", var, s);
	err = putenv(t);

	note(NULL, "setenv %s %s\n", var, s);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CUNSETENV - unset the environment variable VAR */

int cunsetenv(char *var)
   {int err;

    err = unsetenv(var);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CNOVAL - return a non-NULL, empty string which uniquely identifies
 *        - a non-existent variable value
 */

char *cnoval(void)
   {static char none[1] = "";

    return(none);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CGETENV - get the value of environment variable VAR
 *         - if LIT is TRUE return as is otherwise strip off
 *         - surrounding quotes
 */

char *cgetenv(int lit, char *fmt, ...)
   {char var[BFLRG];
    char *t;

    VA_START(fmt);
    VSNPRINTF(var, BFLRG, fmt);
    VA_END;

    t = getenv(var);
    if ((t == NULL) || (strcmp(t, UNDEFINED) == 0))
       t = cnoval();

    else if (lit == FALSE)
       t = strip_quote(t);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CDEFENV - return TRUE iff the environment variable VAR is defined */

int cdefenv(char *fmt, ...)
   {int rv;
    char var[BFLRG];
    char *t;

    VA_START(fmt);
    VSNPRINTF(var, BFLRG, fmt);
    VA_END;

    t  = getenv(var);
    rv = ((t != NULL) && (strcmp(t, UNDEFINED) != 0));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CENV - return a sorted list of environment variables
 *      - exclude variables named in REJ
 */

char **cenv(int sort, char **rej)
   {int i, n;
    char t[BFLRG];
    char *p, *s, *bf, **ta;

    bf = run(FALSE, "env");
    ta = tokenize(bf, "\n", 0);
    if (ta != NULL)
       {n = lst_length(ta);

/* eliminate the rejected variable entries */
        if (rej != NULL)
	   {int j, nc;

	    for (i = 0; i < n; i++)
	        {for (j = 0; rej[j] != NULL; j++)
		     {nc = strlen(rej[j]);

/* if it matches, free the existing entry, move the last one
 * into this slot and decrement both n and i
 * because this new entry will have to be evaluated
 */
		      if ((strncmp(ta[i], rej[j], nc) == 0) &&
			  (ta[i][nc] == '='))
			 {FREE(ta[i]);
			  ta[i--] = ta[--n];
			  ta[n]   = NULL;
			  break;};};};};
	        

/* fix SHELL_Cont because the escaped newline is a problem */
	for (i = 0; i < n; i++)
	    {if (strncmp(ta[i], "SHELL_Cont=", 10) == 0)
	        {p = strchr(ta[i], '\n');
		 if (p != NULL)
		    {*p++ = '\0';
		     s = STRSAVE(ta[i]);
		     FREE(ta[i]);
		     ta[i] = STRSAVE(p);
		     ta[n++] = s;};
		 break;};};

/* fix PATH because RUN must change the PATH to work */
	for (i = 0; i < n; i++)
	    {if (strncmp(ta[i], "PATH=", 5) == 0)
	        {snprintf(t, BFLRG, "PATH=%s", cgetenv(FALSE, "PATH"));
		 FREE(ta[i]);
		 ta[i] = STRSAVE(t);
		 break;};};

	if (sort == TRUE)
	   string_sort(ta, n);};

    return(ta);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CMPENV - return a strcmp style value for the
 *        - comparison of the environment variable VAR value and VAL
 */

int cmpenv(char *var, char *val)
   {int rv;
    char *t;

    t = getenv(var);
    if ((t != NULL) && (strcmp(t, UNDEFINED) != 0))
       rv = strcmp(t, val);
    else
       rv = -2;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CINITENV - set the environment variable VAR to VAL
 *          - only if it is undefined
 */

int cinitenv(char *var, char *fmt, ...)
   {int err;
    char s[BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    err = 0;
    if (cdefenv(var) == FALSE)
       err = csetenv(var, s);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CCLEARENV - clear out the environment for safety
 *           - do not remove variables in NULL terminated EXCEPT
 */

int cclearenv(char **except)
   {int i, j, ne, ok, err;
    char s[BFLRG];
    char *t, **pe, **env;
    extern char **environ;

    err = 0;

/* make a copy of environ because
 * removing elements with unsetenv scrambles environ
 */
    env = NULL;
    for (pe = environ, ne = 0; *pe != NULL; pe++, ne++)
        env = lst_add(env, *pe);
    env = lst_add(env, NULL);

/* traverse env to find names of variables to remove */
    for (i = 0; i < ne; i++)
        {nstrncpy(s, BFLRG, env[i], -1);
	 t = strchr(s, '=');
	 if (t != NULL)
	    *t = '\0';

	 ok = TRUE;
	 if (except != NULL)
	    {for (j = 0; (except[j] != NULL) && (ok == TRUE); j++)
		 ok = (strcmp(s, except[j]) != 0);};

	 if (ok == TRUE)
	    err |= unsetenv(s);};

    free_strings(env);

/* let us know if there was nothing to do */
    if (i == 0)
       err = -1;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_FILE - open the specified file */

FILE *open_file(char *mode, char *fmt, ...)
   {char bf[BFLRG];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(bf, BFLRG, fmt);
    VA_END;

    fp = fopen_safe(bf, mode);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANK_LINE - return TRUE iff the string S represents a blank line */

int blank_line(char *s)
   {int rv;

    rv = ((strspn(s, " \t\f\n") == strlen(s)) || (s[0] == '#'));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_TOK - push token T onto the end of string S */

int push_tok(char *s, int nc, int dlm, char *fmt, ...)
   {int rv;
    char t[BFLRG], delim[2];

    VA_START(fmt);
    VSNPRINTF(t, BFLRG, fmt);
    VA_END;

    rv = TRUE;

    if (IS_NULL(s) == FALSE)
       {delim[0] = dlm;
	delim[1] = '\0';
	nstrcat(s, nc, delim);};

    if (s != NULL)
       nstrcat(s, nc, t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPEND_TOK - append token T onto the end of string S
 *            - this is a dynamic version of push_tok
 */

char *append_tok(char *s, int dlm, char *fmt, ...)
   {int nc;
    char t[BFLRG], delim[2];
    char *p;

    VA_START(fmt);
    VSNPRINTF(t, BFLRG, fmt);
    VA_END;

    if (s != NULL)
       {delim[0] = dlm;
	delim[1] = '\0';

	nc = strlen(s) + strlen(t) + strlen(delim) + 10;
	p  = MAKE_N(char, nc);
	if (p != NULL)
	   {nstrncpy(p, nc, s, -1);
	    nstrcat(p, nc, delim);
	    nstrcat(p, nc, t);};
	FREE(s);}

    else
       p = STRSAVE(t);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_TOK_BEG - push token T onto the beginning of string S */

int push_tok_beg(char *s, int nc, int dlm, char *fmt, ...)
   {int rv;
    char t[BFLRG], bf[BFLRG];

    VA_START(fmt);
    VSNPRINTF(t, BFLRG, fmt);
    VA_END;

    rv = TRUE;

/* fill the local buffer BF with the result */
    nstrncpy(bf, BFLRG, t, -1);
    if (IS_NULL(s) == FALSE)
       vstrcat(bf, BFLRG, "%c%s", dlm, s);

/* copy BF back to S */
    if (s != NULL)
       nstrncpy(s, nc, bf, -1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_DIR - push D onto the directory stack */

int push_dir(char *fmt, ...)
   {int n, rv;
    char d[BFLRG];

    getcwd(d, BFLRG);

    n  = dstck.n++;
    dstck.dir[n] = STRSAVE(d);

    VA_START(fmt);
    VSNPRINTF(d, BFLRG, fmt);
    VA_END;

    rv = chdir(d);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_DIR - pop the directory stack */

int pop_dir(void)
   {int n, rv;

    rv = 0;

    n = dstck.n - 1;
    if (n >= 0)
       {rv = chdir(dstck.dir[n]);
	FREE(dstck.dir[n]);};

    dstck.n = max(n, 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_PATH - pop the tail of the path off
 *          - return a pointer to the head component
 */

char *pop_path(char *path)
   {char *p;

    p = strrchr(path, '/');
    if (p != NULL)
       *p++ = '\0';
    else
       p = path;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_EXECUTABLE_FILE - if PATH is the name of an executable file
 *                    - and if the length of path is <= NCX
 *                    - copy it into FP and return the length
 */

static int is_executable_file(char *fp, char *path, int ncx)
   {int n, muid, mgid, fuid, fgid;
    int usrx, grpx, othx, file;
    struct stat bf;

    n = -1;

    muid = getuid();
    mgid = getgid();

    n = -1;
    if (stat(path, &bf) == 0)
       {fuid = bf.st_uid;
	fgid = bf.st_gid;
	file = bf.st_mode & S_IFREG;
	usrx = ((muid == fuid) && (bf.st_mode & S_IXUSR));
	grpx = ((mgid == fgid) && (bf.st_mode & S_IXGRP));
	othx = (bf.st_mode & S_IXOTH);
	if (file && (usrx || grpx || othx))
	   {n = strlen(path);
	    if (n <= ncx)
	       {strcpy(fp, path);
		n = 0;};};}

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HANDLE_PATH_DOT - given a directory DIR and path NAME
 *                 - resolve out ./ and ../ elements of NAME
 *                 - return result in PATH
 *                 - DIR may be destroyed in this process
 */

static void handle_path_dot(char *path, long nb, char *dir, char *name)
   {char *s;

    if (dir != NULL)
       {s = name;

/* loop over any number of ./ or ../ elements of the path */
	while (s[0] == '.')
	   {if (s[1] == '.')
	       {s += 3;
		pop_path(dir);}
	    else
	       s += 2;};

/* construct the candidate path */
	snprintf(path, nb, "%s/%s", dir, s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_PATH - given the name of a file return the full path
 *           - with reference to the PATH environment variable
 *           - expand out '.' and '..' constructions
 *           - input is the NAME, output is the PATH which is
 *           - at most NC chars long
 *           - return:
 *           -    0 on success
 *           -    n NAME is there but NC is too few characters
 *           -      n is the number of characters needed to contain
 *           -      the full path
 */

int file_path(char *name, char *path, int nc)
   {int n;
    size_t nb;
    char pathvar[BFLRG], fp[BFLRG];
    char *t, *p;

    n  = -1;
    nb = BFLRG - 1;
    switch (name[0])
       {case '/' :
	     n = is_executable_file(path, name, nc);
             break;

        case '.' :
             t = getcwd(pathvar, nb);
             handle_path_dot(fp, BFLRG, t, name);
	     n = is_executable_file(path, fp, nc);
             break;

        default:
	     p = getenv("PATH");
	     if (p != NULL)
	        {strncpy(pathvar, p, BFLRG);
		 pathvar[BFLRG-1] = '\0';

		 for (t = strtok(pathvar, ":");
		      t != NULL;
		      t = strtok(NULL, ":"))
		     {handle_path_dot(fp, BFLRG, t, name);
		      n = is_executable_file(path, fp, nc);
		      if (n == 0)
			 break;};};
             break;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FULL_PATH - make a full path out of DIR and NAME
 *           - with reference to the PATH environment variable if FL is TRUE
 *           - otherwise without
 *           - this is essentially the shell 'which' command if FL is TRUE
 *           - input is the NAME, output is the PATH which is
 *           - at most NC chars long
 *           - return:
 *           -    0 on success
 *           -   -1 if FL is TRUE and not on your PATH
 *           -    n if NC is too small to contain the full path
 */

int full_path(char *path, int nc, int fl, char *dir, char *name)
   {int rv, st;
    int is, ns, nx;
    char d[BFLRG], s[BFLRG];
    char **sa, *p;

    rv = 0;

    if (name[0] == '/')
       nstrncpy(s, BFLRG, name, -1);

    else if (dir != NULL)
       snprintf(s, BFLRG, "%s/%s", dir, name);

    else if (fl == TRUE)
       {sa = tokenize(cgetenv(TRUE, "PATH"), ":", 0);
	ns = lst_length(sa);
	for (is = 0; is < ns; is++)
	    {snprintf(s, BFLRG, "%s/%s", sa[is], name);
	     st = file_kind(S_IFREG, 0111, s);
	     if (st == TRUE)
	        break;};

	if (is >= ns)
	   {rv = -1;
	    s[0] = '\0';};

	free_strings(sa);}

    else
       nstrncpy(s, BFLRG, name, -1);

    if (rv == 0)

/* make it an absolute path */
       {if (s[0] != '/')
	   {getcwd(d, BFLRG);
	    snprintf(path, nc, "%s/%s", d, s);
	    nstrncpy(s, BFLRG, path, -1);};

/* remove ../ and ./ from s */
	sa = tokenize(s, "/", 0);
	ns = lst_length(sa);
	path[0] = '\0';
	for (is = 0; is < ns; is++)
	    {if (strcmp(sa[is], "..") == 0)
	        {p = strrchr(path, '/');
		 *p = '\0';}
	     else if (strcmp(sa[is], ".") != 0)
	        vstrcat(path, nc, "/%s", sa[is]);};

/* compute the return value */
	if (rv == 0)
	   {nx = strlen(path);
	    rv = (nx < nc) ? 0 : nx;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CWHICH - check the path for EXE */

char *cwhich(char *fmt, ...)
   {int st;
    char prg[BFLRG];
    static char exe[BFLRG];

    if (fmt != NULL)
       {VA_START(fmt);
	VSNPRINTF(prg, BFLRG, fmt);
	VA_END;}
    else
       prg[0] = '\0';

#if 0
    int ok;
    char d[BFLRG];
    char *path;

    if (prg[0] == '/')
       snprintf(exe, BFLRG, "%s", prg);
       
    else if (strncmp(prg, "./", 2) == 0)
       {getcwd(d, BFLRG);
	snprintf(exe, BFLRG, "%s/%s", d, prg+2);}
       
    else if (strncmp(prg, "../", 3) == 0)
       {getcwd(d, BFLRG);
	snprintf(exe, BFLRG, "%s/%s", path_head(d), prg+3);}
       
    else
       {ok   = FALSE;
	path = cgetenv(TRUE, "PATH");
	FOREACH(dir, path, " :\n")
	   snprintf(exe, BFLRG, "%s/%s", dir, prg);
	   if (file_executable(exe) == TRUE)
	      {ok = TRUE;
	       break;};
	ENDFOR;

	if (ok == FALSE)
	   nstrncpy(exe, BFLRG, "none", -1);};
#else
    st = full_path(exe, BFLRG, TRUE, NULL, prg);
    if (st != 0)
       exe[0] = '\0';
#endif

    return(exe);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LS - return an array of file names
 *    - the last entry is NULL and marks the end of the list
 */

char **ls(char *opt, char *fmt, ...)
   {char s[BFLRG];
    static char **lst;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (opt == NULL)
       opt = "";

    lst = NULL;
    FOREACH(ph, run(FALSE, "ls %s %s 2>&1", opt, s), " \n")
       if (file_exists(ph) == TRUE)
	  lst = lst_add(lst, ph);
    ENDFOR;
    lst = lst_add(lst, NULL);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MATCH - match the first string arg against the
 *       - regular expression defined by the second string arg
 *       - return -1 if it is lexically less than the pattern
 *       - return  0 if it matches the pattern
 *       - return  1 if it is lexically greater than the pattern
 *       -
 *       - regular expression specifiers (so far)
 *       -
 *       -   '*' matches any number of characters
 *       -   '?' matches any single character
 *       -
 *       - examples:
 *       -    3.2 < 4.*      =>  -1
 *       -    4.2 = 4.*      =>   0
 *       -    5.2 > 4.*      =>   1
 */

#define END_CHECK(ps, pp)                                                    \
   {if ((*ps == '\0') && (*pp != '\0') && (*pp != '*'))                      \
       return(-1);                                                           \
    else if ((*ps != '\0') && (*pp == '\0'))                                 \
       return(1);}

int match(char *s, char *patt)
   {int rv;

#if 0
    int flags;

    flags = 0;

/* flags:
 *   FNM_NOESCAPE treat backslash as an ordinary character,
 *                instead of an escape character
 *   FNM_PATHNAME match a slash in string only with a slash
 *                in pattern and not by an asterisk or a question mark
 *                metacharacter, nor by a bracket expression ([])
 *                containing a slash
 *   FNM_PERIOD   a leading period in string has to be matched exactly
 *                by a period in pattern
 *                a period is considered to be leading if it is the
 *                first character in string, or if both
 */
    rv = fnmatch(patt, s, flags);

#else
    int b, c;
    char *ps, *pp;

    ps = s;
    pp = patt;
	
    if (pp == NULL)
       rv = 0;
    else if (ps == NULL)
       rv = -1;
    else if ((*ps == '\0') && (*pp == '\0'))
       rv = 0;
    else
       {rv = -1;
	while ((c = *pp++) != '\0')
	   {switch (c)
	       {case '*' :
		     while (*pp == '*')
		        pp++;
		     b = 0;
		     c = *pp;
		     while ((ps = strchr(ps, c)) != NULL)
		        {if (match(ps, pp) == 0)
			    return(0);
			 END_CHECK(ps, pp);
			 if ((*ps == '\0') && (*pp == '\0'))
			    break;
			 else
			    b = *(++ps);};
		     c = *(++pp);
		     if (b < c)
		        rv = -1;
		     else
		        rv = 1;
		     return(rv);

		case '?' :
		     rv = match(++ps, pp);
		     return(rv);

		case '\\' :
		     c = *pp++;

		default :
		     b = *ps++;
		     if (b < c)
		        return(-1);
		     else if (b > c)
		        return(1);};

	    END_CHECK(ps, pp);};

	if (*ps == '\0')
	   rv = -1;};
#endif

    return(rv);}

#undef END_CHECK

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNIQUE - remove duplicate tokens from a list */

char *unique(char *lst, int beg, int dlm)
   {int ok;
    char olst[BFLRG], delim[10];
    char *item, *p;
    static char nlst[BFLRG];

    nlst[0] = '\0';

    if (lst != NULL)
       {nstrncpy(olst, BFLRG, lst, -1);

	delim[0] = dlm;
	delim[1] = '\0';

	for (item = olst; item != NULL; item = p)
	    {p = strchr(item, dlm);
	     if (p == NULL)
	        {if (IS_NULL(item) == TRUE)
		    break;}
	     else
	        *p++ = '\0';

	     ok = TRUE;
	     FOREACH(j, nlst, delim)
	        if (strcmp(j, item) == 0)
		   {ok = FALSE;
		    break;}
	     ENDFOR
	     if (ok == TRUE)
	        {if (beg == TRUE)
		    push_tok_beg(nlst, BFLRG, dlm, item);
		 else
		    push_tok(nlst, BFLRG, dlm, item);};};};

    return(nlst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLICE_OUT_PATH - look thru PATH and remove any
 *                 - occurences of $PATH or ${PATH}
 *                 - while we are at it remove duplicates
 *                 - return the result in PATH
 */

void splice_out_path(char *path)
   {char bf[BFLRG], lpth[BFLRG];
    char *ps, *t, *ts, *te;

    nstrncpy(lpth, BFLRG, path, -1);

/* check for $PATH */
    ps = lpth + strspn(lpth, " \t");
    t  = strstr(ps, "$PATH");
    if (t != NULL)
       {ts = t;
	te = ts + 5;}
    else
       {t  = strstr(ps, "${PATH}");
	ts = (t == NULL) ? NULL : t;
	te = ts + 7;};
            
/* splice out $PATH */
    if (ts != NULL)
       {bf[0] = '\0';

	if (ts != ps)
	   {*(ts-1) = '\0';
	    nstrncpy(bf, BFLRG, ps, -1);}

	else if (*te == ':')
	   te++;

	if (*te != '\0')
	   nstrcat(bf, BFLRG, te);

	ps = bf;};

    strcpy(path, unique(ps, FALSE, ':'));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_PATH - look thru PATH for things to add to destination DPATH */

void push_path(int end, char *dpath, char *path)
   {char lpth[BFLRG], tp[BFLRG];

    if (IS_NULL(path) == FALSE)
       {nstrncpy(lpth, BFLRG, strip_quote(path), -1);

	splice_out_path(lpth);

/* add the new item */
	if (IS_NULL(dpath) == TRUE)
	   nstrncpy(tp, BFLRG, lpth, -1);

	else
	   {if (end == P_APPEND)
	       snprintf(tp, BFLRG, "%s:%s", dpath, lpth);
	    else if (end == P_PREPEND)
	       snprintf(tp, BFLRG, "%s:%s", lpth, dpath);};

	nstrncpy(dpath, BFLRG, path_simplify(tp, ':'), -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BUILD_PATH - build up the global LPATH to include all specified programs
 *            - BASE is a string containing colon separated
 *            - directories ala PATH
 */

void build_path(char *base, ...)
   {char exe[BFLRG], pth[BFLRG], os[BFLRG];
    char *s, *t;

    unamef(os, BFLRG, "s");

    if (strcmp(os, "AIX") == 0)
       snprintf(pth, BFLRG,
		"/bin:/usr/bin:/sw/bin:/sbin:/usr/sbin:/usr/local/bin:");
    else
       snprintf(pth, BFLRG,
		"/bin:/usr/bin:/sw/bin:/sbin:/usr/sbin:/usr/xpg4/bin:");

    if (base == NULL)
       nstrcat(pth, BFLRG, cgetenv(TRUE, "PATH"));
    else
       nstrcat(pth, BFLRG, base);

    VA_START(base);

    while (TRUE)
       {s = VA_ARG(char *);
	if (s == NULL)
	   break;

	t = NULL;

	FOREACH(dir, pth, ":")
	   snprintf(exe, BFLRG, "%s/%s", dir, s);
	   if (file_executable(exe) == TRUE)
	      {t = STRSAVE(exe);
	       push_path(P_PREPEND, lpath, dir);
	       break;};
	ENDFOR;

	if (t == NULL)
	   {printf("\nError: cannot find %s - exiting\n\n", s);
	    exit(1);}
	else
	   free(t);};

    VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TOUCH - create an empty file */

int touch(char *fmt, ...)
   {int fd;
    char path[BFLRG];

    VA_START(fmt);
    VSNPRINTF(path, BFLRG, fmt);
    VA_END;

    fd = creat(path, 0660);
    if (fd > 0)
       close_safe(fd);

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* KEY_VAL - parse out a key/value pair from S using DLM delimiters
 *         - avoid use of strtok so as not to conflict with FOREACH
 */

void key_val(char **key, char **val, char *s, char *dlm)
   {char *k, *p, *v;

    k = NULL;
    v = NULL;
    if (s != NULL)
       {k = trim(s, FRONT, dlm);
	p = strpbrk(k, dlm);
	if (p != NULL)
	   {*p++ = '\0';
	    v    = trim(p, BOTH, dlm);};};

    if (key != NULL)
       *key = k;

    if (val != NULL)
       *val = v;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNAMEF - function to do what uname utility does
 *        - return info in S which is NC long
 *        - WH is one of "s", "n", "m", "r", and "v" corresponding to
 *        - the uname command line args
 */

void unamef(char *s, int nc, char *wh)
   {static int first = TRUE;
    static struct utsname uts;

    if (first == TRUE)
       {first = FALSE;
	uname(&uts);};

    s[0] = '\0';

    switch (*wh)
       {case 'm' :
             nstrncpy(s, nc, uts.machine, -1);
             break;
        case 'n' :
	     nstrncpy(s, nc, uts.nodename, -1);
             break;
        case 'r' :
	     nstrncpy(s, nc, uts.release, -1);
             break;
        case 's' :
	     nstrncpy(s, nc, uts.sysname, -1);
             break;
        case 'v' :
	     nstrncpy(s, nc, uts.version, -1);
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEMONIZE - turn the running job into a daemon */

int demonize(void)
   {int st, rv, fd;

    rv = FALSE;

/* get into the background by running as a child
 * the parent will return and exit
 */
    st = fork();
    if (st == 0)

/* close stdin, stdout, and stderr */
       {close_safe(0);
	close_safe(1);
	close_safe(2);

/* disassociate from the controlling terminal */
	st = setsid();

/* prevent possibility of reacquiring a controlling terminal
 * the parent will return and exit
 * the child will belong to init and
 * will not be process group or session leader
 */
/*
	st = fork();
	if (st != 0)
*/
	   rv = TRUE;

/* go to direct attached directory so that mounts won't
 * be compromised (i.e. hung)
 * better be using absolute paths from here on out
 */
	chdir("/");

/* whack the umask */
	umask(0);

/* attach stdin, stdout, and stderr to /dev/null */
	fd = open_safe("/dev/null", O_RDWR, 0);     /* stdin */
	dup(fd);                                    /* stdout */
	dup(fd);};                                  /* stderr */

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRINGS_OUT - write the strings SA[I] from MN <= I < MX to a file FP
 *             - if NEWL is TRUE add a newline to each string
 */

int strings_out(FILE *fp, char **sa, int mn, int mx, int newl)
   {int i, n, rv;

    rv = FALSE;
    if ((fp != NULL) && (sa != NULL))
       {n = lst_length(sa);
	if (mx < 0)
	   mx = n;
	else
	   mx = min(mx, n);
	mn = max(mn, 0);
	for (i = mn; i < mx; i++)
	    {fputs(sa[i], fp);
             if (newl == TRUE)
                fputc('\n', fp);};

        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRINGS_FILE - write the strings SA to a file FNAME */

int strings_file(char **sa, char *fname, char *mode)
   {int rv;
    FILE *fp;

    rv = FALSE;
    if ((fname != NULL) && (sa != NULL))
       {fp = fopen_safe(fname, mode);

	rv = strings_out(fp, sa, 0, -1, FALSE);

	fclose_safe(fp);

        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_STRINGS_PUSH - get the text of FP as a
 *                   - NULL terminated list of strings
 *                   - appended to the list PSA
 *                   - if SNL is TRUE strip off trailing newline
 *                   - NBR is the number of blocking reads to do
 *                   - reads after NBR are non-blocking
 *                   - NBR equal -1 means all blocking reads
 *                   - the extra work here is to be able to get
 *                   - strings from pipes and sockets as well as files
 *                   - return TRUE iff successful
 */

int file_strings_push(FILE *fp, char ***psa, int snl, unsigned int nbr)
   {int ne, nx, ev, fd, ost, rv;
    unsigned int i;
    char t[BFVLG];
    char *p;

    rv = FALSE;

    if (fp != NULL)
       {rv = TRUE;
	ne = 0;
	nx = 100000;

	fd  = fileno(fp);
	ost = block_fd(fd, -1);

	for (i = 0; (rv == TRUE) && (ne < nx); i++)
	    {if (feof(fp) == TRUE)
	        rv = FALSE;
	     else
	        {ne++;
		 block_fd(fd, i < nbr);
		 p  = fgets(t, BFVLG, fp);
		 ev = errno;
		 if (p != NULL)
		    {ne = 0;
		     if ((snl == TRUE) && (LAST_CHAR(t) == '\n'))
		        LAST_CHAR(t) = '\0';
		     *psa = lst_add(*psa, t);}
		 else if (ev == EBADF)
		    rv = FALSE;};};

/* if number of consecutive zero length reads exceeds NX count it as EOF
 * SOLARIS does not seem to get feof equal TRUE when
 * FP is stdout or stderr
 */
	if (ne >= nx)
	   rv = FALSE;

	*psa = lst_add(*psa, NULL);

	ost = block_fd(fd, ost);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_STRINGS - return the text of FP as a NULL terminated list of strings */

char **file_strings(FILE *fp)
   {char **sa;

    sa = NULL;

    file_strings_push(fp, &sa, TRUE, -1);

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_TEXT - return the text of FNAME as a NULL terminated list of strings
 *           - if SORT is TRUE sort the file first
 */

char **file_text(int sort, char *fname, ...)
   {char file[BFLRG];
    char **sa;
    FILE *in;

    VA_START(fname);
    VSNPRINTF(file, BFLRG, fname);
    VA_END;

    sa = NULL;
	
    if (IS_NULL(file) == FALSE)
       {if (sort == TRUE)
	   {run(FALSE, "rm -f %s.srt ; sort %s > %s.srt", file, file, file);
	    nstrcat(file, BFLRG, ".srt");};

	in = fopen_safe(file, "r");
	if (in != NULL)
	   {sa = file_strings(in);
	    fclose_safe(in);}
	else
	   fprintf(stderr, "FILE_TEXT: Failed to open '%s'\n", file);

	if (sort == TRUE)
	   run(FALSE, "rm -f %s", file);};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_RUNNING - return the TRUE iff PID is running process */

int is_running(int pid)
   {int st, ok, err;

    ok  = TRUE;
    err = 0;
    st  = kill(pid, 0);
    if (st == -1)
       {err = errno;
	if (err == EPERM)
	   ok = TRUE;
	else if (err == ESRCH)
	   ok = FALSE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _NSIGACTION - portable wrapper for sigaction semantics
 *             - set FNC to handle SIG signals
 *             - FLAGS modifies the signal handling process
 *             - any remaining non-negative arguments are
 *             - taken to be signals that are to be blocked during
 *             - the execution of FNC
 *             - terminate the list with negative integer
 *             - return the old action in OA if non-NULL
 *             - return 0 iff successful otherwise -1
 */

int _nsigaction(struct sigaction *oa, int sig, void (*fn)(int sig),
		int flags, va_list __a__)
   {int is, rv;
    struct sigaction na, ta;
    sigset_t *set;
    void (*fo)(int sig);
    static int nsx = -1;

    rv = -1;

    if (nsx == -1)
       nsx = 8*sizeof(sigset_t);

    if ((0 < sig) && (sig < nsx) &&
	(sig != SIGKILL) && (sig != SIGSTOP))
       {if (oa == NULL)
	   oa = &ta;

	memset(oa, 0, sizeof(struct sigaction));

	if (sigaction(sig, NULL, oa) == 0)
	   fo = oa->sa_handler;
	else
	   fo = NULL;

/* if the handler differs from the one already in place */
	if (fo != fn)
	   {na.sa_flags   = flags;
	    na.sa_handler = fn;

	    set = &na.sa_mask;
	    sigemptyset(set);

	    while (TRUE)
	       {is = VA_ARG(int);
		if (is < 0)
		   break;
		sigaddset(set, is);};

	    rv = sigaction(sig, &na, NULL);}
	else
	   rv = 0;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSIGACTION - portable wrapper for sigaction semantics
 *            - set FNC to handle SIG signals
 *            - FLAGS modifies the signal handling process
 *            - any remaining non-negative arguments are
 *            - taken to be signals that are to be blocked during
 *            - the execution of FNC
 *            - terminate the list with negative integer
 *            - return the old action in OA if non-NULL
 *            - return 0 iff successful otherwise -1
 */

int nsigaction(struct sigaction *oa, int sig, void (*fn)(int sig),
	       int flags, ...)
   {int rv;

    VA_START(flags);
    rv = _nsigaction(oa, sig, fn, flags, __a__);
    VA_END;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOG_ACTIVITY - log messages to FLOG */

void log_activity(char *flog, int ilog, int ilev, char *oper, char *fmt, ...)
   {char msg[BFLRG];
    FILE *lg;

    if ((ilog == TRUE) && (flog != NULL) && (ilev <= db_log_level))
       {lg = fopen_safe(flog, "a");
	if (lg != NULL)
	   {VA_START(fmt);
	    VSNPRINTF(msg, BFLRG, fmt);
	    VA_END;
	    fprintf(lg, "%s\t(%d)\t: %s\n", oper, (int) getpid(), msg);
	    fclose_safe(lg);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MEM_CHK - crude memory check */

int mem_chk(int fl)
   {int i, n, rv;
    char *p;
    static int sz[] = { 1, 101, 1001, 10001 };

    rv = 0;

    n = sizeof(sz)/sizeof(int);

    for (i = 0; i < n; i++)
        {p = malloc(sz[i]);
	 rv += (p != NULL);
	 free(p);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* undefine when passing out of SCOPE_SCORE_COMPILE */

#undef UNDEFINED

# endif
#endif
