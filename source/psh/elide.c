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

/* ELIDE - remove characters between PA and PB from FNAME
 *       - return TRUE iff successful
 */

int elide(char *pa, char *pb, char *fname)
   {int nc, nca, ncb, rv;
    char s[LRG], t[LRG];
    char *p, *ta, *tb, *wh, *ps;
    FILE *fp;

    if ((IS_NULL(pa) == TRUE) || (IS_NULL(pb) == TRUE) || (IS_NULL(fname) == TRUE))
       rv = FALSE;

    else
       {rv = FALSE;

	nca = strlen(pa);
	ncb = strlen(pb);

	if (strcmp(fname, "-") == 0)
	   fp = stdin;
	else
	   fp = fopen(fname, "r");

	nc = 0;
	while (TRUE)
	    {p = fgets(t, LRG, fp);
	     if (p == NULL)
	        break;

	     LAST_CHAR(p) = '\0';
	     
/* remove text between quotes */
             del_quotation(t);

	     s[0] = '\0';
	     for (ps = p; IS_NULL(ps) == FALSE; )
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
			 nstrcat(s, LRG, ps);
		      ps = NULL;}

/* if an instance of PA came first */
		  else if (wh == ta)
		     {nc++;
		      if (nc == 1)
			 {*ta = '\0';
			  nstrcat(s, LRG, ps);};
		      ps = ta + nca;}

/* if an instance of PB came first */
		  else if (wh == tb)
		     {nc--;
		      ps = tb + ncb;};};

	     if (nc == 0)
	        puts(s);};

	if (strcmp(fname, "-") != 0)
	   fclose(fp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv;
    char *pa, *pb;

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
	 else
	    {if (IS_NULL(pa) == TRUE)
	        pa = v[i];
	     else if (IS_NULL(pb) == TRUE)
	        pb = v[i];
	     else
	        rv &= elide(pa, pb, v[i]);};};

    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

