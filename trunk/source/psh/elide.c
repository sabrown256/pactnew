/*
 * ELIDE.C - remove text from streams
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libtime.c"
#include "libasync.c"

typedef struct s_parse parse;

struct s_parse
   {int il;                 /* file line number */
    int ne;                 /* number of expression sets passed */
    int depth;              /* delimiter depth */
    int keep;               /* keep the current set */
    int dquote;             /* delete text in quotes to simplify work */
    int dlen[2];            /* character length of delimiters */
    int range[2];           /* range of sets to elide */
    token_flag fl;          /* modifiers for quote handling */
    char *delim[2];         /* delimiters */
    char *subst;            /* substitution text */
    char *part[4];          /* partitions of current line */
    char bf[2][BFLRG];};    /* working text buffers */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEL_QUOTATION - remove any text between double quotes */

void del_quotation(char *t)
   {char *pu, *pt;

    for (pt = t; *pt != '\0'; pt++)
        {if (*pt == '"')
            {for (pu = pt+1; *pu != '\0'; pu++)
	         {if (*pu == '"')
		     break;};
	     if (*pu == '"')
	        memmove(pt, pu+1, strlen(pu));};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARTITION_TEXT - partition PS */

static void partition_text(parse *ip, char *ps)
   {int i, ns;
    int ln[2], acc[2];
    char *ta[2], *wh;

    ns = strlen(ps);

/* find tokens defined by both delimiters */
    for (i = 0; i < 2; i++)
        {ln[i]  = strcpy_str(ip->bf[i], BFLRG, ps, ns,
			     ip->delim[i], ip->fl);
	 ta[i]  = (ln[i] == ns) ? NULL : ps + ln[i];
	 acc[i] = ((IS_NULL(ta[i]) == FALSE) &&
		   (ta[i][ip->dlen[i]] != '\''));

	 ip->part[i+1] = ta[i];};   /* start of delimiter in fragment */

    if (acc[0] && acc[1])
       wh = min(ta[0], ta[1]);
    else if (acc[0])
       wh = ta[0];
    else if (acc[1])
       wh = ta[1];
    else
       wh = NULL;

    ip->keep    = (ip->depth > 0) ? ip->keep : FALSE;
    ip->part[0] = ps;      /* text fragment */
    ip->part[3] = wh;      /* actionable part of fragment */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DELIM_BEGIN - handle case where PS has a beginning delimiter first */

static int delim_begin(parse *ip, char *d, int nd)
   {int c, nset, ns, ne, nca, ncb, depth, rv;
    char *ps, *pb, *ta, *tb;

    rv = TRUE;

    nset  = ip->ne;
    depth = ip->depth;
    ns    = ip->range[0];
    ne    = ip->range[1];
    ps    = ip->part[0];
    ta    = ip->part[1];
    tb    = ip->part[2];
    pb    = ip->delim[1];
    nca   = ip->dlen[0];
    ncb   = ip->dlen[1];

    depth++;
    if (depth == 1)

/* keep if out of the range */
       {nset++;
	if ((nset < ns) || (ne < nset))
	   {ip->keep = TRUE;
	    if (tb != NULL)
	       {ta  = tb++;
		c   = *tb;
		*tb = '\0';
		nstrcat(d, nd, ps);
		*tb = c;
		if (strncmp(ta, pb, ncb) == 0)
		   depth--;}
	    else
	       {nstrcat(d, nd, ps);
		rv = FALSE;};}

/* remove if in the range */
	else
	   {*ta = '\0';
	    nstrcat(d, nd, ps);
	    nstrcat(d, nd, ip->subst);};

        ps = ta + nca;}

    else if (ip->keep == TRUE)
       {nstrcat(d, nd, ps);
        ps = NULL;}

    else
       ps = ta + nca;

    ip->ne      = nset;
    ip->depth   = depth;
    ip->part[0] = ps;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DELIM_END - handle case where PS has a ending delimiter first */

static int delim_end(parse *ip, char *d, int nd)
   {int ncb, depth, rv;
    char *ps, *tb;

    rv = TRUE;

    depth = ip->depth;
    ps    = ip->part[0];
    tb    = ip->part[2];
    ncb   = ip->dlen[1];

    depth--;
    if (depth < 0)
       {depth++;
	nstrcat(d, nd, ps);
	ps = NULL;
	rv = FALSE;}

    else if (ip->keep == TRUE)
       {nstrcat(d, nd, ps);
	ps = NULL;}

    else
       ps = tb + ncb;

    ip->depth   = depth;
    ip->part[0] = ps;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NON_DELIM_TEXT - handle text with no delimiters */

static void non_delim_text(parse *ip, char *d, int nd)
   {char *ps;

    if ((ip->depth == 0) || (ip->keep == TRUE))
       {ps = ip->part[0];
	nstrcat(d, nd, ps);};

    ip->part[0] = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RETAINED_TEXT - find the text in S that is to be kept
 *               - return it in D
 */

void retained_text(char *d, int nd, char *s, parse *ip)
   {int ok;
    char *ps;

    d[0] = '\0';

    ok = TRUE;

    for (ps = s; (IS_NULL(ps) == FALSE) && (ok == TRUE); )
        {partition_text(ip, ps);
		     
/* no instance of PA or PB */
	 if (ip->part[3] == NULL)
            non_delim_text(ip, d, nd);

/* if an instance of PA came first */
	 else if (ip->part[3] == ip->part[1])
	    ok = delim_begin(ip, d, nd);

/* if an instance of PB came first */
	 else if (ip->part[3] == ip->part[2])
	    ok = delim_end(ip, d, nd);

	 ps = ip->part[0];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ELIDE - remove characters between PA and PB from FNAME
 *       - return TRUE iff successful
 */

int elide(char *fname, parse *ip)
   {int il, rv;
    char s[BFLRG], t[BFLRG];
    char *p;
    FILE *fp;

    if ((IS_NULL(ip->delim[0]) == TRUE) ||
	(IS_NULL(ip->delim[1]) == TRUE) ||
	(IS_NULL(fname) == TRUE))
       rv = FALSE;

    else
       {rv = FALSE;

	if (strcmp(fname, "-") == 0)
	   fp = stdin;
	else
	   fp = fopen_safe(fname, "r");

	if (fp != NULL)
	   {for (il = 0; TRUE; il++)
	        {ip->il = il;

		 p = fgets(t, BFLRG, fp);
		 if (p == NULL)
		    break;

		 LAST_CHAR(p) = '\0';
	     
/* remove text between quotes
 * this appears to be the right thing to do only when
 * doing all sets
 * perhaps this needs its own switch
 */
		 if (ip->dquote == TRUE)
		    del_quotation(t);

		 retained_text(s, BFLRG, p, ip);

		 if ((ip->depth == 0) ||
		     (ip->keep == TRUE) ||
		     ((IS_NULL(s) == FALSE) &&
		      (IS_NULL(ip->subst) == FALSE)))
		    puts(s);};

	    if (strcmp(fname, "-") != 0)
	       fclose_safe(fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv;
    parse ip;

    ip.ne       = 0;
    ip.keep     = FALSE;
    ip.depth    = 0;
    ip.dquote   = FALSE;
    ip.fl       = 0;
    ip.range[0] = -1;
    ip.range[1] = INT_MAX;
    ip.delim[0] = "";
    ip.delim[1] = "";
    ip.subst    = "";

    rv = TRUE;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("Usage: elide [-d] [-h] [-ns #] [-ne #] [-q] [-s <text>] <start> <stop> <file>*\n");
             printf("   d          delete text in quotes to simplify\n");
             printf("   h          this help message\n");
             printf("   ns         remove starting with this occurence\n");
             printf("   ne         do not remove after this occurence\n");
             printf("   q          ignore single quotes\n");
             printf("   s          substitute <text> for each occurence\n");
             printf("   <text>     quoted text\n");
             printf("   <start>    starting pattern\n");
             printf("   <stop>     ending pattern\n");
             printf("Examples:\n");
             printf("   elide '{' '}' foo.c\n");
             printf("   elide '/*' '*/' foo.c\n");
             printf("   elide '#if' '#endif' foo.c\n");
             printf("\n");
             return(1);}
	 else if (strcmp(v[i], "-d") == 0)
	     ip.dquote = TRUE;
	 else if (strcmp(v[i], "-ns") == 0)
	     ip.range[0] = atol(v[++i]);
	 else if (strcmp(v[i], "-ne") == 0)
	     ip.range[1] = atol(v[++i]);
	 else if (strcmp(v[i], "-q") == 0)
	     ip.fl |= SINGLE_QUOTES;
	 else if (strcmp(v[i], "-s") == 0)
	     ip.subst = v[++i];
	 else
	    {if (IS_NULL(ip.delim[0]) == TRUE)
	        ip.delim[0] = v[i];
	     else if (IS_NULL(ip.delim[1]) == TRUE)
	        ip.delim[1] = v[i];
	     else
	        {ip.dlen[0] = strlen(ip.delim[0]);
		 ip.dlen[1] = strlen(ip.delim[1]);

		 rv &= elide(v[i], &ip);};};};

    log_safe("dump", 0, NULL, NULL);

    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

