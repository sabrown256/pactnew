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
   {int il;             /* file line number */
    int n;              /* number of sets passed */
    int range[2];       /* range of sets to elide */
    int depth;          /* delimiter depth */
    int dlen[2];        /* character length of delimiters */
    char *delim[2];     /* delimiters */
    char *subst;};      /* substitution text */

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

/* RETAINED_TEXT - find the text in S that is to be kept
 *               - return it in D
 */

void retained_text(char *d, int nd, char *s, parse *ip)
   {int c, nc, ns, in, ne, nca, ncb;
    char *pa, *pb, *ta, *tb, *wh, *ps;

    in  = ip->n;
    nc  = ip->depth;
    ns  = ip->range[0];
    ne  = ip->range[1];
    pa  = ip->delim[0];
    pb  = ip->delim[1];
    nca = ip->dlen[0];
    ncb = ip->dlen[1];

    d[0] = '\0';

    for (ps = s; IS_NULL(ps) == FALSE; )
        {ta = strstr(ps, pa);
	 tb = strstr(ps, pb);
	 if ((ta != NULL) && (ta[nca] != '\'') &&
	     (tb != NULL) && (tb[ncb] != '\''))
	    wh = (ta < tb) ? ta : tb;
	 else if ((ta != NULL) && (ta[nca] != '\''))
	    wh = ta;
	 else if ((tb != NULL) && (tb[ncb] != '\''))
	    wh = tb;
	 else
	    wh = NULL;

/* no instance of PA or PB */
	 if (wh == NULL)
	    {if (nc == 0)
	        nstrcat(d, nd, ps);
	     ps = NULL;}

/* if an instance of PA came first */
	 else if (wh == ta)
	    {nc++;
	     if (nc == 1)

/* keep if out of the range */
	        {in++;
		 if ((in < ns) || (ne < in))
		    {nc--;
		     if (tb != NULL)
		        {ta  = tb++;
			 c   = *tb;
			 *tb = '\0';
			 nstrcat(d, nd, ps);
			 *tb = c;}
		     else
		        {nstrcat(d, nd, ps);
			 break;};}

/* remove if in the range */
		 else
		    {*ta = '\0';
		     nstrcat(d, nd, ps);
		     nstrcat(d, nd, ip->subst);};};

	     ps = ta + nca;}

/* if an instance of PB came first */
	 else if (wh == tb)
	    {nc--;
	     if (nc < 0)
	        {nc++;
		 nstrcat(d, nd, ps);
		 break;}
	     else
	        ps = tb + ncb;};};

    ip->n     = in;
    ip->depth = max(nc, 0);

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

	for (il = 0; TRUE; il++)
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
	     if ((ip->range[0] == -1) && (ip->range[1] == INT_MAX))
	        del_quotation(t);

	     retained_text(s, BFLRG, p, ip);

	     if ((ip->depth == 0) ||
		 ((IS_NULL(s) == FALSE) && (IS_NULL(ip->subst) == FALSE)))
	        puts(s);};

	if (strcmp(fname, "-") != 0)
	   fclose_safe(fp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv;
    parse ip;

    ip.n        = 0;
    ip.range[0] = -1;
    ip.range[1] = INT_MAX;
    ip.depth    = 0;
    ip.delim[0] = "";
    ip.delim[1] = "";
    ip.subst    = "";

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("Usage: elide [-h] [-ns #] [-ne #] [-s <text>] <start> <stop> <file>*\n");
             printf("   h          this help message\n");
             printf("   ns         remove starting with this occurence\n");
             printf("   ne         do not remove after this occurence\n");
             printf("   s          substitute <text> for each occurence\n");
             printf("   <text>     quoted text\n");
             printf("   <start>    starting pattern\n");
             printf("   <stop>     ending pattern\n");
             printf("Examples:\n");
             printf("   elide '{' '}' foo.c\n");
             printf("   elide '/*' '*/' foo.c\n");
             printf("   elide '#if' '#endif' foo.c\n");
             printf("\n");}
	 else if (strcmp(v[i], "-ns") == 0)
	     ip.range[0] = atol(v[++i]);
	 else if (strcmp(v[i], "-ne") == 0)
	     ip.range[1] = atol(v[++i]);
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

