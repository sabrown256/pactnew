/*
 * SCARRS.C - SC_array routines
 *          - next generation of SC_dynamic_array
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 

#define GROWTH_FACTOR(x)   ((x)/pow(2.0, 32.0))

enum {SET, GET};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SET_1 - set a char size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_set_1(void *a, int bpi, int oper, long n, void *v)
    {char *s;

     s = (char *) a;
     if (oper == SET)
        s[n] = *(char *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SET_2 - set a short size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_set_2(void *a, int bpi, int oper, long n, void *v)
    {short *s;

     s = (short *) a;
     if (oper == SET)
        s[n] = *(short *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SET_4 - set a int size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_set_4(void *a, int bpi, int oper, long n, void *v)
    {int *s;

     s = (int *) a;
     if (oper == SET)
        s[n] = *(int *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SET_8 - set a BIGINT size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_set_8(void *a, int bpi, int oper, long n, void *v)
    {BIGINT *s;

     s = (BIGINT *) a;
     if (oper == SET)
        s[n] = *(BIGINT *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SET_N - set an arbitrary size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_set_n(void *a, int bpi, int oper, long n, void *v)
    {char *s;

     s = (char *) a;
     if (oper == SET)
        memcpy(s + n*bpi, v, bpi);
     else if (oper == GET)
        v = s + n*bpi;

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SET_METHOD - set the assignment method for A */

static void _SC_array_set_method(SC_array *a)
   {int bpi;

    bpi = a->bpi;

    if (bpi == sizeof(char))
       a->set = _SC_array_set_1;
    else if (bpi == sizeof(short))
       a->set = _SC_array_set_2;
    else if (bpi == sizeof(int))
       a->set = _SC_array_set_4;
    else if (bpi == sizeof(BIGINT))
       a->set = _SC_array_set_8;
    else
       a->set = _SC_array_set_n;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_GROW - grow the array A */

static void _SC_array_grow(SC_array *a, long nn)
   {int chg;
    long i, n, nx, bpi;
    double gf, fc;
    char *arr;
    void *e;

    arr = (char *) a->array;
    nx  = a->nx;
    bpi = a->bpi;
    chg = FALSE;

/* if new size not specified - grow exponentially from the old size */
    if (nn < 0)
       {n  = max(nx, a->n);
	gf = a->gf;
	fc = pow(10.0, 1.0 - gf*n);
	nn = fc*n;
        nn = 2*nx;};

/* if never allocated */
    if (arr == NULL)
       {if (nn == 0)
	   {nx = 0;
	    nn = 1;
	    nn = 512;};
	arr = FMAKE_N(char, nn*bpi, a->name);
	chg = TRUE;
	SC_mark(arr, 1);}

/* if too small */
    else if (nn > nx)
       {REMAKE_N(arr, char, nn*bpi);
	chg = TRUE;};

/* if we changed it - make it consistent */
    if (chg == TRUE)
       {if (a->init != NULL)
           {for (i = nx; i < nn; i++)
	        {e = arr + i*bpi;
		 a->init(e);};}
	else if (SC_zero_on_alloc() == FALSE)
	   memset(arr + nx*bpi, 0, (nn-nx)*bpi);

	a->nx    = nn;
	a->array = arr;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_ARRAY - initialize an SC_array A
 *                - array has NAME, TYPE, and BPI
 *                - NOTE: default growth factor is chosen to give
 *                - exponential with factor of 10 for 0 items
 *                - to 1% at 1 GB
 */

void _SC_init_array(SC_array *a, char *name, char *type, int bpi,
		    void (*init)(void *a))
    {int nt, nn, prm;
     char *ty, *nm;

     prm = (strncmp(name, "PERM|", 5) == 0);

     nn = strlen(name) + 1;
     nt = strlen(type) + 4;
     if (prm == TRUE)
        {nm  = NMAKE_N(char, nn, "PERM|char*:_SC_INIT_ARRAY:nm");
	 ty  = NMAKE_N(char, nt, "PERM|char*:_SC_INIT_ARRAY:ty");}
     else
        {nm  = FMAKE_N(char, nn, "char*:_SC_INIT_ARRAY:nm");
	 ty  = FMAKE_N(char, nt, "char*:_SC_INIT_ARRAY:ty");};

     snprintf(nm, nn, "%s",   name);
     snprintf(ty, nt, "%s *", type);

     a->name  = nm;
     a->type  = ty;
     a->bpi   = bpi;
     a->nx    = 0;
     a->n     = 0;
     a->nref  = 0;
     a->gf    = GROWTH_FACTOR(3.0);
     a->array = NULL;
     a->init  = init;

     _SC_array_set_method(a);

/*     _SC_array_grow(a, 0); */

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_ARRAY - initialize and return an SC_array
 *               - array has NAME, TYPE, and BPI
 */

SC_array *SC_make_array(char *name, char *type, int bpi,
			void (*init)(void *a))
    {SC_array *a;

     a = FMAKE(SC_array, name);
     if (a != NULL)
        _SC_init_array(a, name, type, bpi, init);

     return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRING_ARRAY - common specialization of SC_make_array */

SC_array *SC_string_array(char *name)
   {SC_array *a;

    a = SC_MAKE_ARRAY(name, char *, NULL);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_FREE_N - do SC_free for array element */

int SC_array_free_n(void *a)
   {void *t;

    if (a != NULL)
       {t = *(void **) a;
	SFREE(t);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_ARRAY - release storage from an SC_array */

void SC_free_array(SC_array *a, int (*rel)(void *a))
    {int bpi, rv;
     long i, nx;
     void *e;

     if (a != NULL)
        {nx  = a->nx;
	 bpi = a->bpi;

	 if (a->array != NULL)
	    {if (rel != NULL)
	        {for (i = 0; i < nx; i++)
		     {e = SC_array_get(a, i);
		      if (e != NULL)
			 rv = rel(e);};};

	     memset(a->array, 0, nx*bpi);};

	 SFREE(a->array);
	 SFREE(a->type);
	 SFREE(a->name);
	 SFREE(a);};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_DONE - release storage from an SC_array except for the array
 *               - return the array
 */

void *SC_array_done(SC_array *a)
    {void *rv;

     rv = NULL;

     if (a != NULL)
        {rv = a->array;
	 a->array = NULL;
	 SFREE(a->name); 
	 SFREE(a->type);
	 SFREE(a);};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_INIT - (re)initialize the Nth element of A */

void SC_array_init(SC_array *a, long n)
   {long bpi;
    char *arr;
    void *e;

    bpi = a->bpi;
    arr = a->array;
    e   = arr + n*bpi;

    if (a->init != NULL)
       a->init(e);
    else
       memset(e, 0, bpi);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_RESIZE - resize the array A to N elements
 *                 - used when you know a good working
 *                 - size for the array and don't want to incur
 *                 - the penalty for incrementally growing it
 *                 - also set growth factor to G
 *                 - growth is Nn = (No + 1)*(1 + 2^(-f*No))
 *                 - where f = G/2^32
 *                 - G = 3 means that for No = 2^32 (1 GB) grow by 1/8
 *                 - G = 2 means that for No = 2^32 (1 GB) grow by 1/4
 *                 - G = 1 means that for No = 2^32 (1 GB) grow by 1/2
 *                 - G < 0 is ignored to prevent big growth at large size
 *                 - return the old size
 */

long SC_array_resize(SC_array *a, long n, double g)
   {long nn;

    nn = a->nx;

/* disallow growth greater that 100% at 1 GB */
    if (0.0 <= g)
       a->gf = GROWTH_FACTOR(g);

    _SC_array_grow(a, n);

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_SET - set the Nth element of A to V
 *              - if N < 0 push V onto the end of A
 */

void *SC_array_set(SC_array *a, long n, void *v)
    {void *rv;

     rv = NULL;

     if (a != NULL)

/* this can happen after reading from PDBfile */
        {if (a->set == NULL)
	    _SC_array_set_method(a);

	 if (n < 0)
	    n = a->n;

	 a->n = max(a->n, n+1);

	 if (n >= a->nx)
	    _SC_array_grow(a, -1);

	 rv = a->set(a->array, a->bpi, SET, n, v);};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_GET - get the Nth element of A
 *              - if N < 0 get elements from the end
 *              -    N = -1  last
 *              -    N = -2  second to last
 *              -    etc
 */

void *SC_array_get(SC_array *a, long n)
    {void *rv;

     rv = NULL;

     if (a != NULL)

/* this can happen after reading from PDBfile */
        {if (a->set == NULL)
	    _SC_array_set_method(a);

	 if (n < 0)
	    {n = a->n + n;
	     n = max(n, 0);};

	 if (n >= a->nx)
	    _SC_array_grow(a, -1);

	 rv = a->set(a->array, a->bpi, GET, n, NULL);};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_ARRAY - return the actual array from A
 *                - FLAG is a bit array
 *                -   1   mark the array iff on
 */

void *SC_array_array(SC_array *a, int flag)
    {void *rv;

     rv = NULL;

     if (a != NULL)
        {if (a->array == NULL)
	    _SC_array_grow(a, -1);
	 rv = a->array;};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_UNARRAY - decrement the reference count
 *                  - bookending SC_array_array
 *                  - FLAG is a bit array
 *                  -   1   SFREE the array iff on
 */

void SC_array_unarray(SC_array *a, int flag)
    {int n;

#if 0
     n = a->nref;
     n--;
     a->nref = max(n, 0);

     if (flag & 1)
        SFREE(a->array);
#endif

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_GET_N - return the number of items in A */

long SC_array_get_n(SC_array *a)
    {long n;

     n = -1;

     if (a != NULL)
        n = a->n;

     return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_SET_N - set the number of items in A
 *                - if N < 0 decrement the number of items by N
 *                - return the new value
 */

long SC_array_set_n(SC_array *a, long n)
    {

     if (a == NULL)
        n = 0;

     else
        {if (n < 0)
	    {n = a->n + n;
	     n = max(n, 0);};

	 _SC_array_grow(a, n);

	 a->n = n;};

     return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_INC_N - increment the number of items in A by N
 *                - if WH < 0 return the new value (ala ++n)
 *                - otherwise return the old value (ala n++)
 */

long SC_array_inc_n(SC_array *a, long n, int wh)
    {long no, nn, rv;

     rv = 0;

     if (a != NULL)
        {no = a->n;
	 nn = no + n;
	 rv = (wh < 0) ? nn : no;

	 _SC_array_grow(a, nn);

	 a->n = nn;};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_DEC_N - decrement the number of items in A by N
 *                - if WH < 0 return the new value (ala ++n)
 *                - otherwise return the old value (ala n++)
 */

long SC_array_dec_n(SC_array *a, long n, int wh)
    {long no, nn, rv;

     rv = 0;

     if (a != NULL)
        {no = a->n;
	 nn = no - n;
	 nn = max(nn, 0);
	 rv = (wh < 0) ? nn : no;

	 a->n = nn;};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_STRING_ADD - add the string S to the array A */

void SC_array_string_add(SC_array *a, char *s)
    {

     SC_array_push(a, &s);
     if (s != NULL)
        SC_mark(s, 1);

/* add a terminating NULL for conventional usages */
     s = NULL;
     SC_array_push(a, &s);
     SC_array_dec_n(a, 1, -1);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_STRING_ADD_COPY - add the a copy of the string S to the array A */

void SC_array_string_add_copy(SC_array *a, char *s)
    {char *t;

     t = SC_strsavef(s, "char*:SC_ARRAY_STRING_ADD_COPY:t");

     SC_array_string_add(a, t);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_STRING_APPEND - append the array of strings IN to OUT
 *                        - return the number of strings added
 */

int SC_array_string_append(SC_array *out, SC_array *in)
   {int i, no;
    char **sa;

    sa = SC_array_array(in, 0);

    if (sa != NULL)
       {no = SC_array_get_n(in);
        for (i = 0; i < no; i++)
	    SC_array_string_add(out, sa[i]);}
    else
       no = 0;

    SC_array_unarray(in, 0);

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_STRING_JOIN - take the array of strings which may have '\n'
 *                       - any where and make a new array
 *                       - with each string ending with a '\n'
 */

char **_SC_array_string_join(SC_array *sa)
   {int i, no, c;
    char *s, *p, *bf, **sao, **san;
    SC_array *na;

    no  = SC_array_get_n(sa);
    sao = SC_array_array(sa, 0);

    na = SC_string_array("_SC_ARRAY_STRING_JOIN");

    bf = NULL;

    if (sao != NULL)
       {for (i = 0; i < no; i++)
	    {s = sao[i];
	     if (s != NULL)
	        {c = *s;
		 while (SC_is_print_char(c, 0))
		    {p = strchr(s, '\n');
		     if (p == NULL)
		        {bf = SC_dstrcat(bf, s);
			 break;}
		     else
		        {*p++ = '\0';
			 bf = SC_dstrcat(bf, s);
			 bf = SC_dstrcat(bf, "\n");
			 SC_array_string_add(na, bf);

			 bf = NULL;
			 s  = p;
			 if (SC_is_print_char(*s, 4))
			    bf = SC_dstrcat(bf, s);};};

		 SFREE(sao[i]);};};

	SFREE(sao);

/* add the whatever is left if anything */
	if (bf != NULL) 
	   {if (bf[0] != '\0')
	       SC_array_string_add(na, bf);
	    else
	       SFREE(bf);};};

/* add a terminating NULL */
    SC_array_string_add(na, NULL);

    san = SC_array_done(na);

    return(san);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_IS_MEMBER - return TRUE iff S is already in the array A */

int _SC_array_is_member(SC_array *a, char *s)
   {int i, n, rv;
    char **str;

    n   = SC_array_get_n(a);
    str = SC_array_array(a, 0);

    rv = FALSE;

/* NOTE: do not filter out barrier commands */
    if (!IS_BARRIER(s))
       {for (i = 0; i < n; i++)
	    {if (strcmp(s, str[i]) == 0)
	        {rv = TRUE;
		 break;};};};

    SC_array_unarray(a, 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_COPY - return a copy of A */

SC_array *SC_array_copy(SC_array *a)
    {long nb, nx, bpi;
     SC_array *ca;

     ca  = FMAKE(SC_array, "SC_ARRAY_COPY:ca");
     *ca = *a;

     nx  = a->nx;
     bpi = a->bpi;
     nb  = nx*bpi;

     ca->name = SC_strsavef(a->name, "SC_ARRAY_COPY:name");
     ca->type = SC_strsavef(a->type, "SC_ARRAY_COPY:type");

     ca->array = FMAKE_N(char, nb, "SC_ARRAY_COPY:array");
     memcpy(ca->array, a->array, nb);

     return(ca);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRINGS_ARRAY - wrap an array of strings with an SC_array */

SC_array *SC_strings_array(int n, char **sa)
   {SC_array *a;

    a = FMAKE(SC_array, "SC_STRINGS_ARRAY:a");

    a->name  = "SC_STRINGS_ARRAY";
    a->type  = "char *";
    a->bpi   = sizeof(char *);
    a->nx    = n;
    a->n     = n;
    a->array = sa;
    a->init  = NULL;

    _SC_array_set_method(a);

    return(a);}

/*--------------------------------------------------------------------------*/

/*                           STACK ROUTINES                                 */

/*--------------------------------------------------------------------------*/

/* SC_ARRAY_PUSH - push V onto A */

void *SC_array_push(SC_array *a, void *v)
    {void *rv;

     rv = SC_array_set(a, -1, v);

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_POP - pop A */

void *SC_array_pop(SC_array *a)
    {long n;
     void *rv;

     rv = NULL;

     if (a != NULL)

/* get the last element */
        {rv = SC_array_get(a, -1);

/* reduce the number of elements */
	 n  = SC_array_dec_n(a, 1L, -1);};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SWAP - swap two elements of A
 *                - checks on the validity of A are done
 *                - by SC_array_sort and are not repeated here
 */

static void _SC_array_swap(SC_array *a, long il, long ir)
   {void *xl, *xr;

    xl = SC_array_get(a, il);
    xr = SC_array_get(a, ir);
    SC_array_set(a, il, xr);
    SC_array_set(a, ir, xl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_PARTITION - do the partition step of the merge sort
 *                     - checks on the validity of A are done
 *                     - by SC_array_sort and are not repeated here
 */

static long _SC_array_partition(SC_array *a, PFIntBin pred,
				long il, long ir, long ip)
   {long i, is;
    void *pvl, *lvl;

    pvl = SC_array_get(a, ip);

/* move pivot to end */
    _SC_array_swap(a, ip, ir);

    is = il;
    for (i = il; i < ir; i++)
        {lvl = SC_array_get(a, i);
	 if (pred(lvl, pvl) == TRUE)
            {_SC_array_swap(a, i, is);
             is++;};};

/* move pivot to its final place */
    _SC_array_swap(a, is, ir);

    return(is);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SORT - do a merge sort on A
 *                - checks on the validity of A are done
 *                - by SC_array_sort and are not repeated here
 */

static int _SC_array_sort(SC_array *a, PFIntBin pred, long il, long ir)
   {long ip, np;

    if (ir > il)
       {ip = (il + ir)/2;
        np = _SC_array_partition(a, pred, il, ir, ip);
	_SC_array_sort(a, pred, il, np - 1);
	_SC_array_sort(a, pred, np + 1, ir);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_SORT - sort A according to PRED */

int SC_array_sort(SC_array *a, PFIntBin pred)
   {int rv;
    long ne;

    rv = FALSE;
    if (a != NULL)
       {ne = SC_array_get_n(a);
	rv = _SC_array_sort(a, pred, 0, ne-1);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

