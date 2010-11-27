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

/* #define DEBUG */

#define GROWTH_FACTOR(x)   ((x)/pow(2.0, 32.0))

enum {SET, GET};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_ERR - look for problems */

static INLINE void _SC_array_err(SC_array *a, char *fmt, ...)
    {

#ifdef DEBUG

     int nr;
     char s[MAXLINE];

     SC_VA_START(fmt);
     SC_VSNPRINTF(s, MAXLINE, fmt);
     SC_VA_END;

     nr = SC_ref_count(a->array);

     if (nr > 1)
        io_printf(stdout, "\nERROR: %s (%ld references)\n\n", s, nr);

#else

     SC_VA_START(fmt);
     SC_VA_END;

#endif

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_ACC_1 - set/get a char size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_acc_1(int oper, void *a, int bpi, long n, void *v)
    {char *s;

     s = (char *) a;
     if (oper == SET)
        s[n] = (v == NULL) ? '\0' : *(char *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_ACC_2 - set/get a short size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_acc_2(int oper, void *a, int bpi, long n, void *v)
    {short *s;

     s = (short *) a;
     if (oper == SET)
        s[n] = (v == NULL) ? 0 : *(short *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_ACC_4 - set/get a int size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_acc_4(int oper, void *a, int bpi, long n, void *v)
    {int *s;

     s = (int *) a;
     if (oper == SET)
        s[n] = (v == NULL) ? 0 : *(int *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_ACC_8 - set/get a int64_t size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_acc_8(int oper, void *a, int bpi, long n, void *v)
    {int64_t *s;

     s = (int64_t *) a;
     if (oper == SET)
        s[n] = (v == NULL) ? 0 : *(int64_t *) v;
     else if (oper == GET)
        v = &s[n];

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_ACC_N - set/get an arbitrary size element of A
 *                 - A is guaranteed to be non-NULL
 */

static void *_SC_array_acc_n(int oper, void *a, int bpi, long n, void *v)
    {char *s;

     s = (char *) a;
     if (oper == SET)
        {if (v == NULL)
	    memset(s + n*bpi, 0, bpi);
	 else
	    memcpy(s + n*bpi, v, bpi);}
     else if (oper == GET)
        v = s + n*bpi;

     return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_ACC_METHOD - set the assignment method for A */

static void _SC_array_acc_method(SC_array *a)
   {int bpi;

    bpi = a->bpi;

    if (bpi == sizeof(char))
       a->access = _SC_array_acc_1;
    else if (bpi == sizeof(short))
       a->access = _SC_array_acc_2;
    else if (bpi == sizeof(int))
       a->access = _SC_array_acc_4;
    else if (bpi == sizeof(int64_t))
       a->access = _SC_array_acc_8;
    else
       a->access = _SC_array_acc_n;

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
	fc = pow(2.0, -gf*n);
	nn = (n + 1)*(1.0 + fc);};

/* if never allocated */
    if (arr == NULL)
       {if (nn == 0)
	   {nx = 0;
	    nn = 1;};
	arr = FMAKE_N(char, nn*bpi, a->name);
	chg = TRUE;}

/* if too small */
    else if (nn > nx)
       {_SC_array_err(a, "growing array %s from %ld to %ld", a->name, nx, nn);

	REMAKE_N(arr, char, nn*bpi);
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
	a->array = arr;

	SC_mark(arr, 1);};

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
     a->gf    = GROWTH_FACTOR(3.0);
     a->array = NULL;
     a->init  = init;

     _SC_array_acc_method(a);

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

/* SC_ARRAY_FRAC_RESIZE - resize the array A
 *                      - to have (1 + F) times a many elements
 *                      - as are in current use
 *                      - return the old size
 */

long SC_array_frac_resize(SC_array *a, double f)
   {long n, nn;

    nn = 0;

    if (a != NULL)
       {n  = a->n;
	nn = (1.0 + f)*n;

	_SC_array_grow(a, nn);};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_SET - set the Nth element of A to V
 *              - if N < 0 push V onto the end of A
 */

void *SC_array_set(SC_array *a, long n, void *v)
    {long m;
     void *rv;

     rv = NULL;

     if (a != NULL)

/* this can happen after reading from PDBfile */
        {if (a->access == NULL)
	    _SC_array_acc_method(a);

	 if (n < 0)
	    n = a->n;

	 m    = n + 1;
	 a->n = max(a->n, m);

	 if ((m >= a->nx) || (a->array == NULL))
	    _SC_array_grow(a, -1);

	 _SC_array_err(a, "set array %s %ld", a->name, n);

	 rv = a->access(SET, a->array, a->bpi, n, v);};

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
        {if (a->access == NULL)
	    _SC_array_acc_method(a);

	 if (n < 0)
	    {n = a->n + n;
	     n = max(n, 0);};

	 if (n >= a->nx)
	    _SC_array_grow(a, -1);

	 _SC_array_err(a, "get array %s %ld", a->name, n);

	 rv = a->access(GET, a->array, a->bpi, n, NULL);};

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_ARRAY - return the actual array from A
 *                - the array is marked and the caller
 *                - must SFREE the returned pointer
 *                - this convention makes it possible for _SC_array_grow
 *                - to detect whether the array it will grow is in use
 */

void *SC_array_array(SC_array *a)
    {void *rv;

     rv = NULL;

     if (a != NULL)
        {if (a->array == NULL)
	    _SC_array_grow(a, -1);

	 rv = a->array;

	 SC_mark(rv, 1);};

     return(rv);}

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
 *                - if WH < 0 return the new value (ala --n)
 *                - otherwise return the old value (ala n--)
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

/* SC_ARRAY_REMOVE - remove the Nth element from A
 *                 - slide the others down so there is no hole
 *                 - return the new number of elements iff successful
 *                 - and -1 otherwise
 */

long SC_array_remove(SC_array *a, long n)
   {long no, bpi;
    char *arr;

    no = a->n - 1;
    if (n <= no)
       {_SC_array_err(a, "remove array %s %ld", a->name, n);

        bpi = a->bpi;
	arr = a->array;

	if (n < no)
	   memmove(arr + n*bpi, arr + (n+1)*bpi, (no - n)*bpi);

	SC_array_init(a, no);

	a->n--;}
    else
       no = -1;

    return(no);}

/*--------------------------------------------------------------------------*/

/*                            STRING ARRAY ROUTINES                         */

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

/* SC_ARRAY_STRING_ADD_VCOPY - add the a copy of the string S to the array A */

void SC_array_string_add_vcopy(SC_array *a, char *fmt, ...)
   {char *t;

    SC_VDSNPRINTF(TRUE, t, fmt);

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

    if ((in != NULL) && (in->array != NULL))
       {no = in->n;
	sa = in->array;
        for (i = 0; i < no; i++)
	    SC_array_string_add(out, sa[i]);}
    else
       no = 0;

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_STRING_JOIN - take the array of strings which may have '\n'
 *                       - any where and make a new array
 *                       - with each string ending with a '\n'
 */

char **_SC_array_string_join(SC_array **psa)
   {int i, no, c;
    char *s, *p, *bf, **sao, **san;
    SC_array *na, *sa;

    sa   = *psa;
    *psa = NULL;

    no  = SC_array_get_n(sa);
    sao = SC_array_done(sa);

    na = SC_STRING_ARRAY();
    SC_array_resize(na, 2*no, -1.0);

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
			    bf = SC_dstrcat(bf, s);};};};};

	SC_free_strings(sao);

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

    rv = FALSE;

/* NOTE: do not filter out barrier commands */
    if (!IS_BARRIER(s))
       {n   = a->n;
	str = a->array;
	for (i = 0; i < n; i++)
	    {if (strcmp(s, str[i]) == 0)
	        {rv = TRUE;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_COPY - return a copy of A */

SC_array *SC_array_copy(SC_array *a)
    {long nb, nx, bpi;
     void *arr;
     SC_array *ca;

     nx  = a->nx;
     bpi = a->bpi;
     nb  = nx*bpi;

     arr = FMAKE_N(char, nb, "SC_ARRAY_COPY:arr");
     memcpy(arr, a->array, nb);

     ca  = FMAKE(SC_array, "SC_ARRAY_COPY:ca");
     *ca = *a;

     ca->name  = SC_strsavef(a->name, "SC_ARRAY_COPY:name");
     ca->type  = SC_strsavef(a->type, "SC_ARRAY_COPY:type");
     ca->array = arr;

     SC_mark(arr, 1);

     return(ca);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_STRINGS_ARRAY - wrap an array of strings with an SC_array */

SC_array *SC_strings_array(int n, char **sa)
   {SC_array *a;

    a = FMAKE(SC_array, "SC_STRINGS_ARRAY:a");

    a->name  = "SC_STRINGS_ARRAY";
    a->type  = SC_STRING_S;
    a->bpi   = sizeof(char *);
    a->nx    = n;
    a->n     = n;
    a->array = sa;
    a->init  = NULL;

    _SC_array_acc_method(a);

    return(a);}

/*--------------------------------------------------------------------------*/

/*                              STACK ROUTINES                              */

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

/*                               SORT ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SWAP - swap two elements of A
 *                - checks on the validity of A are done
 *                - by SC_array_sort and are not repeated here
 */

static void _SC_array_swap(SC_array *a, long il, long ir)
   {long n;
    void *xl, *xr, *t;

    if (il != ir)
       {n  = SC_array_get_n(a);

	xl = SC_array_get(a, il);
	xr = SC_array_get(a, ir);

	SC_array_set(a, n+1, xl);
	t = SC_array_get(a, n+1);

	SC_array_set(a, il, xr);
	SC_array_set(a, ir, t);

	SC_array_set(a, n+1, NULL);
        SC_array_set_n(a, n);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRAY_SORT - do a merge sort on A
 *                - checks on the validity of A are done
 *                - by SC_array_sort and are not repeated here
 */

static int _SC_array_sort(SC_array *a, PFIntBin pred, long il, long ir)
   {long i, ip, in;
    void *pvl, *lvl;

    if (il < ir)
       {ip = (il + ir)/2;

	_SC_array_swap(a, il, ip);

	in = il;

	pvl = SC_array_get(a, il);

	for (i = il+1; i <= ir; i++)
	    {lvl = SC_array_get(a, i);
	     if (pred(lvl, pvl) == TRUE)
	        _SC_array_swap(a, ++in, i);};

	_SC_array_swap(a, il, in);

	_SC_array_sort(a, pred, il, in-1);
	_SC_array_sort(a, pred, in+1, ir);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRAY_SORT - sort A according to PRED */

int SC_array_sort(SC_array *a, PFIntBin pred)
   {int rv;
    long n, nx;

    rv = FALSE;
    if (a != NULL)
       {n  = a->n;
	nx = a->nx;

/* make sure there is room for a temporary element to
 * be used in swaps
 */
	if (n >= nx-1)
	   _SC_array_grow(a, -1);

	rv = _SC_array_sort(a, pred, 0, n-1);
        SC_array_set_n(a, n);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

