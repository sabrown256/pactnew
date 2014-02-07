/*
 * LIBSTACK.C - routines supporting stacks of pointers
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBSTACK

# define LIBSTACK

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

typedef struct s_vstack vstack;

struct s_vstack
   {int n;
    int nx;
    char *type;
    void **stk;};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

/* define only for SCOPE_SCORE_COMPILE */

#define UNDEFINED  "--undefd--"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_STK - initialize and return a vstack */

vstack *make_stk(char *type, int n)
   {vstack *stk;

    stk = MAKE(vstack);
    stk->n    = 0;
    stk->nx   = n;
    stk->type = STRSAVE(type);
    stk->stk  = MAKE_N(void *, n);

    return(stk);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_FREE - free the list */

void stk_free(vstack *stk)
   {

    if (stk != NULL)
       {FREE(stk->type);
	FREE(stk->stk);
	FREE(stk);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_LENGTH - return the length of the stack */

int stk_length(vstack *stk)
   {int n;

    n = 0;
    if (stk != NULL)
       n = stk->n;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_PUSH - add S to stack STK */

int stk_push(vstack *stk, void *a)
   {int n, nx;

    n = -1;

    if (stk != NULL)
       {n  = stk->n;
	nx = stk->nx;
	if (nx - n < 2)
	   {nx = (nx << 1);
	    REMAKE(stk->stk, void *, nx);
	    if (stk != NULL)
	       memset(stk->stk+nx, 0, nx*sizeof(void *));};

	stk->stk[n] = a;

        stk->n++;
	stk->nx = nx;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_POP - pop stack STK and return the object */

void *stk_pop(vstack *stk)
   {int n;
    void *rv;

    rv = NULL;

    if (stk != NULL)
       {n = stk->n;
	n--;
	n = max(n, 0);

	rv = stk->stk[n];
	stk->stk[n] = NULL;
        stk->n      = n;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_GET - for positive I get the Ith element of stack STK
 *         - for negative I get the (N-I)th element of the stack STK
 */

void *stk_get(vstack *stk, int i)
   {int n, m;
    void *rv;

    rv = NULL;

    if (stk != NULL)
       {n = stk->n;

/* for positive I index from the beginning */
	if ((0 <= i) && (i < n))
	   m = i;

/* for negative I index from the end */
	else if ((1 <= -i) && (-i < n))
	   m = n - i;

	rv = stk->stk[m];};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_COPY - return a copy of STK */

vstack *stk_copy(vstack *stk)
   {int i, nx;
    vstack *nstk;

    nstk = NULL;

    if (stk != NULL)
       {nx = stk->nx;

        nstk = make_stk(stk->type, nx);
	if (nstk != NULL)
	   {nstk->n = stk->n;
	    for (i = 0; i < nx; i++)
	        nstk->stk[i] = stk->stk[i];};};

    return(nstk);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_UNIQ - remove duplicate entries from STK */

void stk_uniq(vstack *stk)
   {int i, j, k, n;
    void *s;

    if (stk != NULL)
       {n = stk->n;
        for (i = 0; i < n; i++)
	    {s = stk->stk[i];
	     for (j = i+1; j < n; j++)
	         {if (s == stk->stk[j])
		     {for (k = j+1; k < n; k++)
			  stk->stk[k-1] = stk->stk[k];
		      stk->stk[k-1] = NULL;
		      n--;
		      i--;
		      break;};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STK_REMOVE - remove A from STK */

void stk_remove(vstack *stk, void *a)
   {int i, j, n;
    void *s;

    if (stk != NULL)
       {n = stk->n;

        for (i = n-1; i >= 0; i--)
	    {s = stk->stk[i];
             if (s == a)
	        {for (j = i+1; j < n; j++)
		     stk->stk[j-1] = stk->stk[j];
		 stk->stk[j-1] = NULL;
		 n--;
		 break;};};

	stk->n = n;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* undefine when passing out of SCOPE_SCORE_COMPILE */

#undef UNDEFINED

# endif
#endif
