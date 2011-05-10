/*
 * SCARRD.C - SC_dynamic_array routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 
#include "scope_mem.h" 

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_CLEAR - initialize an SC_dynamic_array with a size
 *             - increment of D
 */

void SC_da_clear(SC_dynamic_array *a, int d, int bpi)
    {

     a->delta = d;
     a->n     = 0;
     a->nx    = 0;
     a->bpi   = bpi;
     a->type  = NULL;
     a->array = NULL;

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_INIT - initialize an SC_dynamic_array with a size
 *            - increment of D
 */

void SC_da_init(SC_dynamic_array *a, int bpi, char *tn, int d, char *name)
    {int n, flags;
     char *s, *lst;

     flags = (strncmp(name, "PERM|", 5) == 0) ? 3 : 0;

     n = strlen(tn) + 4;

     s = CPMAKE_N(char, n, flags);
     snprintf(s, n, "%s *", tn);

     n   = d*bpi;
     lst = CPMAKE_N(char, n, flags);

     SC_mark(lst, 1);

     a->delta = d;
     a->type  = s;
     a->nx    = d;
     a->n     = 0;
     a->bpi   = bpi;
     a->array = lst;

    if (SC_zero_on_alloc_n(-1) == FALSE)
       memset(lst, 0, n);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_FREE - release storage from an SC_dynamic_array */

void SC_da_free(SC_dynamic_array *a)
    {

     if (a != NULL)
        {CFREE(a->type);
	 CFREE(a->array);
	 CFREE(a);};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_REL - release storage from an SC_dynamic_array */

void SC_da_rel(SC_dynamic_array *a)
    {

     if (a != NULL)
        {CFREE(a->type);
	 CFREE(a->array);

	 SC_da_clear(a, 0, 0);};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_COPY - return a copy of the SC_dynamic_array A */

SC_dynamic_array *SC_da_copy(SC_dynamic_array *a)
    {long nb;
     SC_dynamic_array *ca;

     ca  = CMAKE(SC_dynamic_array);
     *ca = *a;

     nb = SC_arrlen(a->array);

     ca->type  = CSTRSAVE(a->type);
     ca->array = CMAKE_N(char, nb);

     memcpy(ca->array, a->array, nb);

     return(ca);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_DONE - return the array data, release other storage, and
 *            - reset an SC_dynamic_array
 */

void *SC_da_done(SC_dynamic_array *a)
    {void *rv;

     rv = a->array;

     CFREE(a->type);

     SC_da_clear(a, 0, 0);

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DA_ALLOC - do initial allocation for SC_dynamic_array A */

void _SC_da_alloc(SC_dynamic_array *a, char *name)
    {int nx, d, bpi;
     char *lst;

     lst = (char *) a->array;
     if (lst == NULL)
        {d   = a->delta;
	 bpi = a->bpi;
         nx  = d;

         lst = CMAKE_N(char, nx*bpi);
	 if (SC_zero_on_alloc_n(-1) == FALSE)
	    memset(lst, 0, nx*bpi);

         a->array = lst;
         a->nx    = nx;
         a->n     = 0;

	 SC_mark(lst, 1);};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DA_EXTEND - reallocate the SC_dynamic_array space to hold more */

void _SC_da_extend(SC_dynamic_array *a, double pad)
    {int n, nx, nn, d, dn, bpi;
     char *lst, *nlst;

     n = a->n;
     d = a->delta;
     if (pad > 0.0)
        n += pad*d;

     nx = a->nx;
     if (n >= nx)
        {dn = n - nx;
	 d  = max(d, dn);

	 lst = (char *) a->array;
	 bpi = a->bpi;
         nn  = nx + d;

         nlst = CMAKE_N(char, nn*bpi);

	 memcpy(nlst, lst, nx*bpi);
	 if (SC_zero_on_alloc_n(-1) == FALSE)
	    memset(nlst + nx*bpi, 0, d*bpi);

         a->array = nlst;
         a->nx    = nn;

	 SC_mark(nlst, 1);

	 CFREE(lst);};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_GROW - size/resize the SC_dynamic_array A more than M items long
 *            - this will not make the array smaller for any value of M
 */

void SC_da_grow(SC_dynamic_array *a, int m)
    {int n, nx, na, d;
     char *name;

     n   = a->n;
     nx  = a->nx;
     d   = a->delta;
     na  = d*((m + d - 1)/d);

     if (na > nx)

/* modify delta to make _SC_da_alloc do what is needed */
        {a->delta = na;
	 name     = SC_arrname(a->array);

	 _SC_da_alloc(a, name);

/* modify delta and n to make _SC_da_extend do what is needed */
	 a->delta = na - nx;
	 a->n     = na;

	 _SC_da_extend(a, 0.0);

/* restore original values */
	 a->delta = d;
	 a->n     = n;};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_SHRINK - shrink the SC_dynamic_array A to contain N items */

void SC_da_shrink(SC_dynamic_array *a, int n)
    {int bpi;
     char *lst;

     bpi = a->bpi;
     lst = (char *) a->array;
     if (lst == NULL)
        {lst = CMAKE_N(char, n*bpi);
	 if (SC_zero_on_alloc_n(-1) == FALSE)
	    memset(lst, 0, n*bpi);
	 SC_mark(lst, 1);}

     else
        {CREMAKE(lst, char, n*bpi);};

     a->array = lst;
     a->n     = n;
     a->nx    = n;

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_REMEMBER - add the BPI long space that ITEM points to
 *                - to the SC_dynamic_array A
 *                - return the index of the new entry
 */

int SC_da_remember(SC_dynamic_array *a, void *item)
    {int n, bpi;
     int *pis, *pid;
     double *pds, *pdd;
     char **pps, **ppd, *lst;

     _SC_da_alloc(a, "SC_DA_REMEMBER:array");

     bpi = a->bpi;
     n   = (a->n)++;

     if (item == NULL)
        {ppd    = (char **) a->array;
	 ppd[n] = NULL;}
     else if (bpi == sizeof(int))
        {pis    = (int *) item;
	 pid    = (int *) a->array;
	 pid[n] = *pis;}
     else if (bpi == sizeof(double))
        {pds    = (double *) item;
	 pdd    = (double *) a->array;
	 pdd[n] = *pds;}
     else if (bpi == sizeof(char *))
        {pps    = (char **) item;
	 ppd    = (char **) a->array;
	 ppd[n] = *pps;}
     else
        {lst = (char *) a->array;
	 memcpy(lst + n*bpi, item, bpi);};

     _SC_da_extend(a, 0.0);

     return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_RESET - reset each member in an array of SC_dynamic_arrays,
 *             - i.e. remove elements of each SC_dynamic_array, and zero each
 *             - SC_dynamic_array length
 *             - if elem == TRUE, restrict reset to addressed SC_dynamic_array
 *
 *   DA  - Dynamic Array
 *   DAA - Dynamic Array Array
 */           

void SC_da_reset(SC_dynamic_array *daa, int elem)
   {int nda, ida;
    SC_dynamic_array *da;

    if (elem)
       nda = 1;
    else
       nda = SC_MEM_GET_N(SC_dynamic_array, daa);

    for (ida = 0; ida < nda; ida++)
        {da = daa + ida;

	 CFREE(da->array);
	 da->n = 0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_CLEAN - clean each member in an array of SC_dynamic_arrays,
 *             - i.e. remove elements of each SC_dynamic_array, zero each
 *             - SC_dynamic_array length, and free each SC_dynamic_array type
 *             - if elem == TRUE, restrict clean to addressed SC_dynamic_array
 *
 *   DA  - Dynamic Array
 *   DAA - Dynamic Array Array
 */

void SC_da_clean(SC_dynamic_array *daa, int elem)
   {int nda, ida;
    SC_dynamic_array *da;

    if (elem)
       nda = 1;
    else
       nda = SC_MEM_GET_N(SC_dynamic_array, daa);

    for (ida = 0; ida < nda; ida++)
        {da = daa + ida;

	 CFREE(da->array);
	 CFREE(da->type);
	 da->n  = 0;
	 da->nx = 0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_RELEASE - clean each member in an array of SC_dynamic_arrays, then
 *               - free the array pointer
 *
 *   DA  - Dynamic Array
 *   DAA - Dynamic Array Array
 */

void SC_da_release(SC_dynamic_array *daa)
   {

    SC_da_clean(daa, FALSE);
    CFREE(daa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_RELEASE_POINTEES - free pointees of each member in an array of
 *                        - SC_dynamic_arrays
 *                        - if elem == TRUE, restrict release of pointees to
 *                        - addressed SC_dynamic_array
 *
 *   DA  - Dynamic Array
 *   DAA - Dynamic Array Array
 */

void SC_da_release_pointees(SC_dynamic_array *daa, int elem)
   {int nda, ida, np, ip;
    void **ppa;
    SC_dynamic_array *da;

    if (elem)
       nda = 1;
    else
       nda = SC_MEM_GET_N(SC_dynamic_array, daa);

    for (ida = 0; ida < nda; ida++)
        {da  = daa + ida;
	 np  = SC_N_DYNAMIC((*(daa+ida)));
	 ppa = da->array;
	 for (ip = 0; ip < np; ip++)
             CFREE(ppa[ip]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DA_RELEASE_ALL - free pointees of each member in an array of
 *                   - SC_dynamic_arrays, then release the array of
 *                   - SC_dynamic_arrays
 *
 *   DA  - Dynamic Array
 *   DAA - Dynamic Array Array
 */

void SC_da_release_all(SC_dynamic_array *daa)
   {

    SC_da_release_pointees(daa, FALSE);
    SC_da_release(daa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DADA_CLEAN - release each bottom level SC_dynamic_array in an
 *               - array of SC_dynamic_arrays of SC_dynamic_arrays,
 *               - then clean all top level SC_dynamic_arrays
 *               - if elem == TRUE, restrict clean to addressed
 *               - SC_dynamic_array of SC_dynamic_arrays
 *
 *    DA     -  Dynamic Array
 *    DADA   -  Dynamic Array Dynamic Array
 *    DADAA  -  Dynamic Array Dynamic Array Array
 */

void SC_dada_clean(SC_dynamic_array *dadaa, int elem)
   {int ndada, idada, nda, ida;
    SC_dynamic_array **dapa, *pd;

    if (elem)
       ndada = 1;
    else
       ndada = SC_MEM_GET_N(SC_dynamic_array, dadaa);

    for (idada = 0; idada < ndada; idada++) 
        {pd   = dadaa + idada;
	 nda  = SC_N_DYNAMIC((*pd));
	 dapa = (SC_dynamic_array **) (pd->array);
	 for (ida = 0; ida < nda; ida++)
	     SC_da_release(dapa[ida]);};

    SC_da_clean(dadaa, elem);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DADA_RESET - release each bottom level SC_dynamic_array in an
 *               - array of SC_dynamic_arrays of SC_dynamic_arrays,
 *               - then reset all top level SC_dynamic_arrays
 *               - if elem == TRUE, restrict reset to addressed
 *               - SC_dynamic_array of SC_dynamic_arrays
 *
 *    DA     -  Dynamic Array
 *    DADA   -  Dynamic Array Dynamic Array
 *    DADAA  -  Dynamic Array Dynamic Array Array
 */

void SC_dada_reset(SC_dynamic_array *dadaa, int elem)
   {int ndada, idada, nda, ida;
    SC_dynamic_array **dapa, *pd;

    if (elem)
       ndada = 1;
    else
       ndada = SC_MEM_GET_N(SC_dynamic_array, dadaa);

    for (idada = 0; idada < ndada; idada++) 
        {pd   = dadaa + idada;
	 nda  = SC_N_DYNAMIC((*pd));
	 dapa = (SC_dynamic_array **) (pd->array);
	 for (ida = 0; ida < nda; ida++)
	     SC_da_release(dapa[ida]);};

    SC_da_reset(dadaa, elem);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DADA_RELEASE - release each bottom level SC_dynamic_array in an
 *                 - array of SC_dynamic_arrays of SC_dynamic_arrays,
 *                 - then release all top level SC_dynamic_arrays
 *
 *    DA    -  Dynamic Array
 *    DADA  -  Dynamic Array Dynamic Array
 *    DADAA -  Dynamic Array Dynamic Array Array
 */

void SC_dada_release(SC_dynamic_array *dadaa)
   {int ndada, idada, nda, ida;
    SC_dynamic_array  **dapa, *pd;

    ndada = SC_MEM_GET_N(SC_dynamic_array, dadaa);

    for (idada = 0; idada < ndada; idada++)
        {pd   = dadaa + idada;
	 nda  = SC_N_DYNAMIC((*pd));
	 dapa = (SC_dynamic_array **) (pd->array);
	 for (ida = 0; ida < nda; ida++)
	     SC_da_release(dapa[ida]);};

    SC_da_release(dadaa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REMEMBER_STRING - add the string S to the array A */

void SC_remember_string(char *s, SC_dynamic_array *a)
    {

     SC_da_remember(a, &s);
     if (s != NULL)
        SC_mark(s, 1);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REMEMBER_STRING_COPY - add the a copy of the string S to the array A */

void SC_remember_string_copy(char *s, SC_dynamic_array *a)
    {char *t;

     t = CSTRSAVE(s);

     SC_remember_string(t, a);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_APPEND_STRINGS - append the array of strings IN to OUT
 *                   - return the number of strings added
 */

int SC_append_strings(SC_dynamic_array *out, SC_dynamic_array *in)
   {int i, no;
    char **sa;

    sa = SC_ARRAY_DYNAMIC(char *, *in);

    if (sa != NULL)
       {no = SC_N_DYNAMIC(*in);
        for (i = 0; i < no; i++)
	    SC_remember_string(sa[i], out);}
    else
       no = 0;

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_JOIN_LINES - take the array of strings which may have '\n'
 *                - any where and make a new array
 *                - with each string ending with a '\n'
 */

char **_SC_join_lines(SC_dynamic_array *sa)
   {int i, no, c;
    char *s, *p, *bf, **sao, **san;
    SC_dynamic_array na;

    no  = SC_N_DYNAMIC(*sa);
    sao = sa->array;

    SC_mark(sao, 1);

    SC_START_DYNAMIC_ARRAY(na, char *, 100, "_SC_JOIN_LINES:array");

    bf = NULL;

    if (sao != NULL)
       {for (i = 0; i < no; i++)
	    {s = sao[i];
	     c = *s;
	     while (SC_is_print_char(c, 0))
	        {p = strchr(s, '\n');
		 if (p == NULL)
		    {bf = SC_dstrcat(bf, s);
		     break;}
		 else
		    {*p++ = '\0';
		     bf = SC_dstrcat(bf, s);
		     bf = SC_dstrcat(bf, "\n");
		     SC_remember_string(bf, &na);

		     bf = NULL;
		     s  = p;
		     if (SC_is_print_char(*s, 4))
		        bf = SC_dstrcat(bf, s);};};

	     CFREE(sao[i]);};

	CFREE(sao);

/* add the whatever is left if anything */
	if (bf != NULL) 
	   {if (bf[0] != '\0')
	       {SC_remember_string(bf, &na);}
	    else
	       {CFREE(bf);};};};

/* add a terminating NULL */
    SC_remember_string(NULL, &na);

    san = SC_da_done(&na);

    return(san);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IS_MEMBER - return TRUE iff S is already in the array A */

int _SC_is_member(SC_dynamic_array *a, char *s)
   {int i, n, rv;
    char **str;

    n   = SC_N_DYNAMIC(*a);
    str = SC_ARRAY_DYNAMIC(char *, *a);

    rv = FALSE;

/* NOTE: do not filter out barrier commands */
    if (!IS_BARRIER(s))
       {for (i = 0; i < n; i++)
	    {if (strcmp(s, str[i]) == 0)
	        {rv = TRUE;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
