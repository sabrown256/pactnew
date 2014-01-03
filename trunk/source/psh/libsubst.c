/*
 * LIBSUBST.C - routines supporting polysubst
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBSUBST

# define LIBSUBST

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

typedef struct s_substdes substdes;

struct s_substdes
   {int lmt[2];        /* min and max of level key character */
    int nc;            /* number of characters bins at this level */
    char **val;        /* array of substitution values for this level */
    substdes **lev;
    char bf[BFLRG];};

/*--------------------------------------------------------------------------*/

# endif

# ifndef SCOPE_SCORE_PREPROC

/*--------------------------------------------------------------------------*/

/* MAKE_SUBSTDES - instantiate a substdes */

substdes *make_substdes(void)
   {substdes *sd;

    sd = MAKE(substdes);
    if (sd != NULL)
       memset(sd, 0, sizeof(substdes));

    return(sd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _RESIZE_SUBST - resize SD */

static void _resize_subst(substdes *sd, int mn, int mx)
   {int i, dn, nn, no, on, ox;
    char **vo, **vn;
    substdes **so, **sn;

    if (sd != NULL)
       {nn = mx - mn + 1;
        sn = MAKE_N(substdes *, nn);
	vn = MAKE_N(char *, nn);

	memset(sn, 0, nn*sizeof(substdes *));
	memset(vn, 0, nn*sizeof(char *));

	on = sd->lmt[0];
	ox = sd->lmt[1];
	so = sd->lev;
	vo = sd->val;

/* copy over the old info */
	if ((so != NULL) && (vo != NULL))
	   {dn = on - mn;
	    no = ox - on + 1;
	    for (i = 0; i < no; i++)
	        {sn[i + dn] = so[i];
		 vn[i + dn] = vo[i];};};

/* free the old info */
	FREE(so);
	FREE(vo);

/* set the current to the new info */
        sd->nc     = mx - mn + 1;
	sd->lmt[0] = mn;
	sd->lmt[1] = mx;
	sd->lev = sn;
	sd->val = vn;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _INSERT_KEY - recursively install a key in SD */

static int _insert_key(substdes *sd, char *key, char *val)
   {int c, mn, mx, rv;
    substdes *sn;

    rv = FALSE;

    if ((sd != NULL) && (key != NULL))
       {mn = sd->lmt[0];
	mx = sd->lmt[1];

/* check the first character of the key and
 * resize SD if it is out of range
 */
	c = key[0];
        if ((c < mn) || (mx <= c))
	   {if ((mn == 0) && (mx == 0))
	       {mn = c;
		mx = c;};

	    mn = min(mn, c);
	    mx = max(mx, c);
	    _resize_subst(sd, mn, mx);};

	c -= sd->lmt[0];
	if (key[1] == '\0')
           {sd->val[c] = STRSAVE(val);
	    rv = TRUE;}
        else
	   {if (sd->lev[c] == NULL)
	       sd->lev[c] = make_substdes();

	    sn = sd->lev[c];

	    rv = _insert_key(sn, key+1, val);};};

    return(rv);}
	     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLYSUBST_ADD - add a KEY/VALUE pair to the given substdes SD */

int polysubst_add(substdes *sd, char *key, char *val)
   {int rv;

    rv = FALSE;

    if ((sd != NULL) && (IS_NULL(val) == FALSE))
       rv = _insert_key(sd, key, val);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLYSUBST_LOOKUP - lookup a KEY in the given substdes SD */

char *polysubst_lookup(int *plev, substdes *sd, char *key)
   {int c;
    char *vn, *rv;
    substdes *sn;

    rv = NULL;

    if ((sd != NULL) && (IS_NULL(key) == FALSE))
       {(*plev)++;
	c  = key[0] - sd->lmt[0];
	if ((0 <= c) && (c < sd->nc))
	   {sn = sd->lev[c];
	    vn = sd->val[c];
	    if (vn == NULL)
	       rv = polysubst_lookup(plev, sn, key+1);
	    else
	       rv = vn;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLYSUBST - make all substitutions specified in SD to the string S
 *           - return the number of matches found
 */

int polysubst(substdes *sd, char *s, int nc)
   {int i, ic, c, nb, nr, ns, mn, mx, lev;
    char bf[BFLRG];
    char *pb, *ps, *vn;
    substdes *sn;

    ns = 0;
    if (sd != NULL)
       {bf[0] = '\0';

	pb = bf;
	nr = BFLRG;
	mn = sd->lmt[0];
	mx = sd->lmt[1];

	for (ps = s; TRUE; ps++)
	    {c = *ps;
	     if (c == '\0')
	        break;

	     else if ((mn <= c) && (c <= mx))
	        {ic = c - sd->lmt[0];
		 sn = sd->lev[ic];
		 vn = sd->val[ic];
		 lev = 1;
		 if (vn == NULL)
		    vn = polysubst_lookup(&lev, sn, ps+1);

		 if (vn == NULL)
		    {*pb++ = c;
		     nr--;}
		 else
		    {nstrncpy(pb, nr, vn, -1);
		     nb  = strlen(vn);
		     pb += nb;
		     nr -= nb;
		     ps += (lev - 1);
		     ns++;};}
	     else
	        {*pb++ = c;
		  nr--;};};

	if (ns > 0)
	   {*pb++ = '\0';
	    nr--;
	    nstrncpy(s, nc, bf, BFLRG-nr);};};

    return(ns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* undefine when passing out of SCOPE_SCORE_COMPILE */

#undef UNDEFINED

# endif
#endif
