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
   {int n;              /* number of sets passed */
    int range[2];       /* range of sets to elide */
    int depth;          /* delimiter depth */
    char *delim[2];     /* delimiters */
    int dlen[2];};      /* character length of delimiters */

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
	        nstrcat(d, BFLRG, ps);
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
			 nstrcat(d, BFLRG, ps);
			 *tb = c;}
		     else
		        {nstrcat(d, BFLRG, ps);
			 break;};}

/* remove if in the range */
		 else
		    {*ta = '\0';
		     nstrcat(d, BFLRG, ps);};};

	     ps = ta + nca;}

/* if an instance of PB came first */
	 else if (wh == tb)
	    {nc--;
	     if (nc < 0)
	        {nc++;
		 nstrcat(d, BFLRG, ps);};
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
	    {p = fgets(t, BFLRG, fp);
	     if (p == NULL)
	        break;

	     LAST_CHAR(p) = '\0';
	     
/* remove text between quotes */
             del_quotation(t);

	     retained_text(s, BFLRG, p, ip);

	     if (ip->depth == 0)
	        puts(s);};

	if (strcmp(fname, "-") != 0)
	   fclose_safe(fp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, ne, ns, rv;
    char *pa, *pb;
    parse ip;

    ns = -1;
    ne = INT_MAX;

    pa = "";
    pb = "";

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("Usage: elide [-h] <start> <stop> <file>*\n");
             printf("   h          this help message\n");
             printf("   <start>    starting pattern\n");
             printf("   <stop>     ending pattern\n");
             printf("Examples:\n");
             printf("   elide '{' '}' foo.c\n");
             printf("   elide '/*' '*/' foo.c\n");
             printf("   elide '#if' '#endif' foo.c\n");
             printf("\n");}
	 else if (strcmp(v[i], "-ns") == 0)
	     ns = atol(v[++i]);
	 else if (strcmp(v[i], "-ne") == 0)
	     ne = atol(v[++i]);
	 else
	    {if (IS_NULL(pa) == TRUE)
	        pa = v[i];
	     else if (IS_NULL(pb) == TRUE)
	        pb = v[i];
	     else
	        {ip.n        = 0;
		 ip.range[0] = ns;
		 ip.range[1] = ne;
		 ip.depth    = 0;
		 ip.delim[0] = pa;
		 ip.delim[1] = pb;
		 ip.dlen[0]  = strlen(pa);
		 ip.dlen[1]  = strlen(pb);

		 rv &= elide(v[i], &ip);};};};

    log_safe("dump", 0, NULL, NULL);

    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

