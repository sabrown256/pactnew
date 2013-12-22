/*
 * LIBSUBST.C - routines supporting polysubst
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBSUBST

# define LIBSUBST

# ifndef SCOPE_SCORE_PREPROC

/* algorithm:
 *     nb     = cmx - cmn + 1          number of bins
 *     bin(c) = c - cmn
 */

typedef struct s_substlev substlev;
typedef struct s_substdes substdes;

struct s_substlev
   {int lmt[2];        /* min and max of level key character */
    int nc;            /* number of characters bins at this level */
    char **val;        /* array of substitution values for this level */
    substlev **nxt;};  /* array of substlev for next level */

struct s_substdes
   {int nlev;
    substlev **lev;
    char bf[BFLRG];};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

substlev **insert_key(substlev **lev, char *key, char *val)
   {

    if (IS_NULL(key) == FALSE)
       {c  = *key++ - lmt[0];
	sl = lev[c];
	lev = insert_key(lev, key, val);};

    return(lev);}
	     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLYSUBST_ADD - add a KEY/VALUE pair to the given substdes SD */

int polysubst_add(substdes *sd, char *key, char *val)
   {

    if ((sd != NULL) && (IS_NULL(key) == FALSE) && (IS_NULL(val) == FALSE))
       sd->lev = insert_key(sd->lev, key, val);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLYSUBST - make all substitutions specified in SD to the string S */

char *polysubst_add(substdes *sd, char *s, int nc)
   {char *bf;

    bf = NULL;
    if (sd != NULL)
       {bf = sd->bf;

	};

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* undefine when passing out of SCOPE_SCORE_COMPILE */

#undef UNDEFINED

# endif
#endif
