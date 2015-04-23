/*
 * SCTAB.C - next generation hash array/B-tree facility
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "score_int.h"
#include "scope_hash.h"
#include "scope_mem.h"

SC_thread_lock
 SC_ha_lock = SC_LOCK_INIT_STATE;

#define HA_STRING_KEY(_h)    (_h == (PFKeyHash) _SC_hasharr_name)
#define HA_ADDR_KEY(_h)      (_h == (PFKeyHash) _SC_hasharr_addr)

/*--------------------------------------------------------------------------*/

/*                            HASHARR FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* _SC_HASHARR_NAME - compute hash value for string KEY in a array of SIZE */

long _SC_hasharr_name(const void *key, int size)
   {long v;
    char *s;

    s = (char *) key;

    for (v = 0L; *s != '\0'; s++)
        v = (v << 1) ^ (*s);

    v = labs(v) % (long) size;

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HASHARR_ADDR - compute hash value for pointer S in a array of SIZE */

static long _SC_hasharr_addr(const void *key, int size)
   {SC_address ad;
    int c;
    long a, v, r, n, i, m;

    ad.memaddr = (void *) key;

    a = ad.mdiskaddr >> 3;
    m = (long) size;

    v = 0L;
    n = sizeof(long);
    for (i = 0L; i < n; i++)
        {c = (a >> 8*i) & 0xff;
	 v = (v << 1L) ^ c;};

    r = labs(v) % m;

    return(r);}

/*--------------------------------------------------------------------------*/

/*                         COMPARISON FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* _SC_ADDR_COMP - compare two addresses */

static int _SC_addr_comp(const void *key, const void *s)
   {int r;

    r = (key != s);

    return(r);}

/*--------------------------------------------------------------------------*/

/*                             HAELEM FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* _SC_MAKE_HAELEM - initialize and return a haelem */

static haelem *_SC_make_haelem(hasharr *ha, const void *key)
   {void *lkey;
    haelem *hp;

    hp = CPMAKE(haelem, ha->memfl);
    if (hp != NULL)

/* setup the key */
       {if (HA_ADDR_KEY(ha->hash))
	   lkey = (void *) key;
        else
	   {lkey = CSTRDUP(key, ha->memfl);
	    SC_mark(lkey, 1);};

	hp->iht  = ha->hash(lkey, ha->size);
	hp->iar  = SC_array_get_n(ha->a);
	hp->next = ha->table[hp->iht];
	hp->name = lkey;
        hp->def  = NULL;};

    return(hp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_HAELEM - release an haelem */

static void _SC_free_haelem(hasharr *ha, haelem *hp)
   {int addr;

/* undo the MARK in SC_hasharr_install */
    if (hp->free == TRUE)
       CFREE(hp->def);

    addr = HA_ADDR_KEY(ha->hash);
    if (addr == FALSE)
       CFREE(hp->name);

/*    CFREE(hp->type); */
    hp->type = NULL;

    hp->iht = -10000;
    hp->iar = -10000;

    SC_mark(hp, -2);

    CFREE(hp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HAELEM_RESET - reset the contents of HP */

static void _SC_haelem_reset(haelem *hp, const char *type, void *obj,
			     int64_t flags)
   {int mark, rel;

    mark = ((flags & 1) != 0);
    rel  = ((flags & 4) != 0);

    hp->type = (char *) type;

/* do any marking (reference counting) for the new obj */
    if (mark == FALSE)
       {hp->def  = obj;
	hp->free = FALSE;}
	
    else
       {if (rel == TRUE)
	   CFREE(hp->def);
	hp->def  = obj;
	hp->free = (SC_mark(obj, 1) != -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INSERT_HAELEM - initialize and return a haelem */

static void _SC_insert_haelem(hasharr *ha, haelem *hp)
   {

    if (hp != NULL)

/* mark hp with 2 references */
       {SC_mark(hp, 2);

/* add hp to the array and head of bucket */
	SC_array_set(ha->a, hp->iar, &hp);
	ha->table[hp->iht] = hp;

	ha->ne++;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HAELEM_REMOVE - remove HP from HA */

static int _SC_haelem_remove(hasharr *ha, haelem *hp)
   {int rv;
    void *v;

    rv = TRUE;

/* remove from array */
    v = NULL;
    SC_array_set(ha->a, hp->iar, &v);

/* remove from haelem list */
    _SC_free_haelem(ha, hp);

    ha->ne--;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HAELEM_DATA - return the name and object from a hash element
 *                - return TRUE iff successful
 *                - if SVR is TRUE an PO is non-NULL set hp->def to NULL
 *                - severing the connection between *PO and hp->def
 */

int SC_haelem_data(haelem *hp, char **pname, char **ptype, void **po,
		   int svr)
   {int rv;

    rv = FALSE;

    if (hp != NULL)
       {if (pname != NULL)
	   *pname = hp->name;

	if (ptype != NULL)
	   *ptype = hp->type;

	if (po != NULL)
	   {*po = hp->def;
	    if (svr == TRUE)
	       hp->def = NULL;};

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           HASHARRAY FUNCTIONS                            */

/*--------------------------------------------------------------------------*/

/* _SC_HASHARR_INIT - initialize structures in HA */

static void _SC_hasharr_init(hasharr *ha, const char *lm)
   {int i, sz;
    haelem **tb;
    SC_array *a;

    sz = ha->size;
    tb = ha->table;

    SC_hasharr_rekey(ha, lm);

/* explicitly NULL the pointers */
    for (i = 0; i < sz; i++)
        tb[i] = NULL;

    a = CMAKE_ARRAY(haelem *, NULL, ha->memfl);
    SC_array_resize(a, sz, -1.0);

    ha->a  = a;
    ha->ne = 0L;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_HASHARR - make an inhomogeneous hasharr
 *
 * #bind SC_make_hasharr fortran() scheme() python()
 */

hasharr *SC_make_hasharr(int sz, int docflag, const char *lm, int flags)
   {hasharr *ha;
    haelem **tb;

/* allocate a new hash array */
    ha = CPMAKE(hasharr, flags);
    if (ha != NULL)
       {tb = CPMAKE_N(haelem *, sz, flags);
        if (tb != NULL)
           {ha->size     = sz;
	    ha->memfl    = flags;
            ha->docp     = docflag;
            ha->table    = tb;
            ha->hash     = NULL;
            ha->comp     = NULL;
            ha->key_type = NULL;
	    ha->a        = NULL;

	    _SC_hasharr_init(ha, lm);}

	else
	   CFREE(ha);};

    return(ha);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_HA_ELEM - release an haelem */

static int _SC_free_ha_elem(haelem *hp, void *a)
   {hasharr *ha;

    if (hp != NULL)
       {ha = (hasharr *) a;
	_SC_free_haelem(ha, hp);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HASHARR_FREE_ELEMENTS - free all the hash elements from HA */

static void _SC_hasharr_free_elements(hasharr *ha)
   {int (*g)(void *a);

    SC_hasharr_foreach(ha, _SC_free_ha_elem, ha);

    g = NULL;
    SC_free_array(ha->a, g);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_CLEAR - make an empty hasharr out of ha
 *                  - apply F with A to each haelem of HA in order to
 *                  - free objects installed by application
 *                  - then free all of the hash elements
 *                  - leaves a pristine hash array
 *                  - for new installs, lookups, ...
 *
 * #bind SC_hasharr_clear fortran() scheme() python()
 */

int SC_hasharr_clear(hasharr *ha ARG(,,cls),
		     int (*f)(haelem *hp, void *a), void *a)
   {int rv;

    rv = FALSE;
    if (ha != NULL)
       {rv = SC_hasharr_foreach(ha, f, a);

	_SC_hasharr_free_elements(ha);

        _SC_hasharr_init(ha, NULL);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_HASHARR - release a hasharr
 *                 - like SC_hasharr_clear except that the
 *                 - hash array is freed and cannot be used again
 *
 * #bind SC_free_hasharr fortran() scheme() python()
 */

void SC_free_hasharr(hasharr *ha ARG(,,cls),
		     int (*f)(haelem *hp, void *a), void *a)
   {int rv;

    if (SC_safe_to_free(ha))
       {rv = SC_hasharr_foreach(ha, f, a);
	SC_ASSERT(rv == TRUE);

	_SC_hasharr_free_elements(ha);

        CFREE(ha->key_type);
        CFREE(ha->table);};

    CFREE(ha);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_PRUNE - remove table entries for which PRED returns TRUE
 *                  - return the number of entries removed
 */

long SC_hasharr_prune(hasharr *ha,
		      int (*pred)(hasharr *ha, haelem *hp, void *a),
		      void *a)
   {long i, nr;
    long sz;
    haelem **tb, *hp, *prv, *nxt;

    nr = 0;

    sz = ha->size;
    tb = ha->table;
    for (i = 0; i < sz; i++)
        {prv = tb[i];
	 for (hp = tb[i]; hp != NULL; hp = nxt)
	     {nxt = hp->next;
	      if (pred(ha, hp, a) == TRUE)
		 {if (hp == prv)
		     {tb[i] = nxt;
		      prv   = nxt;}
		  else
		     prv->next = nxt;

		  nr += _SC_haelem_remove(ha, hp);}
	    else
	       prv = hp;};};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_INSTALL - install OBJ of type TYPE in HA under KEY
 *                    - FLAGS is a bit array with the following meanings:
 *                    -   bit 1   mark OBJ if TRUE
 *                    -   bit 2   lookup existing entry and reuse
 *                    -   bit 4   free existing hp->def before assigning
 *
 * #bind SC_hasharr_install fortran() scheme() python()
 */

haelem *SC_hasharr_install(hasharr *ha, const void *key,
			   void *obj, const char *type,
			   int64_t flags, int lookup)
   {haelem *hp;

    if ((flags > 1) || (lookup < 0))
       lookup = ((flags & 2) != 0);

    hp = NULL;

/* sanity checks */
    if ((ha != NULL) && (key != NULL))
       {SC_LOCKON(SC_ha_lock);

/* is it already in the array */
	if (lookup == TRUE)
	   hp = SC_hasharr_lookup(ha, key);

/* if not found, then insert it */
	if (hp == NULL)
	   {hp = _SC_make_haelem(ha, key);
	    _SC_insert_haelem(ha, hp);};

/* update valid hash elements inserted or looked-up */
	if (hp != NULL)
	   _SC_haelem_reset(hp, type, obj, flags);

	SC_LOCKOFF(SC_ha_lock);};

    return(hp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_FREE_N - do CFREE for hasharr element */

int SC_hasharr_free_n(void *d, void *a)
   {void *t;

    if (a != NULL)
       {t = *(void **) d;
	CFREE(t);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_FOREACH - apply F with A to each haelem of HA */

int SC_hasharr_foreach(hasharr *ha, int (*f)(haelem *hp, void *a), void *a)
   {int rv;
    long i, n;
    SC_array *arr;
    haelem **hp;

    rv = TRUE;

    arr = ha->a;
    if ((arr != NULL) && (f != NULL))
       {n  = SC_array_get_n(arr);
	hp = SC_array_array(arr);

	for (i = 0; i < n; i++)
	    {if (hp[i] != NULL)
	        rv &= f(hp[i], a);};

	CFREE(hp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_NEXT - return the next non-NULL element in HA
 *                 - I is returned as the index of the current element
 *                 - as are the name, type, and object
 */

int SC_hasharr_next(hasharr *ha, long *pi,
		    char **pname, char **ptype, void **po)
   {int rv;
    long i, ne;
    haelem **tb, *hp;
    SC_array *a;

    rv = FALSE;

    if ((ha != NULL) && (ha->a != NULL))
       {a = ha->a;

	ne = SC_array_get_n(a);
        tb = SC_array_array(a);

	for (i = *pi; i < ne; i++)
	    {hp = tb[i];
	     if (hp != NULL)
	        {if (pname != NULL)
		    *pname = hp->name;

		 if (ptype != NULL)
		    *ptype = hp->type;

		 if (po != NULL)
		    *po = hp->def;

		 *pi = i;
		 rv  = TRUE;

		 break;};};

	CFREE(tb);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_LOOKUP - lookup the haelem for the given KEY
 *
 * #bind SC_hasharr_lookup fortran() scheme() python()
 */

haelem *SC_hasharr_lookup(hasharr *ha ARG(,,cls), const void *key)
   {int sz, lck;
    long iht;
    haelem *rv, *hp, **tb;
    PFKeyHash hash;
    PFIntBinC comp;

    rv = NULL;

/* sanity checks */
    if ((key != NULL) && (ha != NULL))
       {lck = ((ha->memfl & 4) == 0);
	if (lck == TRUE)
	   SC_LOCKON(SC_ha_lock);

        hash = ha->hash;
	comp = ha->comp;

	sz  = ha->size;
	tb  = ha->table;
	iht = (*hash)(key, sz);

	for (hp = tb[iht]; hp != NULL; hp = hp->next)
	    {if ((*comp)(key, hp->name) == 0)
	        {rv = hp;
		 break;};};

	if (lck == TRUE)
	   SC_LOCKOFF(SC_ha_lock);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_DEF_LOOKUP - lookup the actual object for the given KEY
 *
 * #bind SC_hasharr_def_lookup fortran() scheme() python()
 */

void *SC_hasharr_def_lookup(hasharr *ha ARG(,,cls), const void *key)
   {haelem *hp;
    void *obj;
  
    obj = NULL;

    if (ha != NULL)
       {hp  = SC_hasharr_lookup(ha, key);
	obj = (hp != NULL) ? hp->def : NULL;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_GET - return the Nth element of HA
 *
 * #bind SC_hasharr_get fortran() scheme() python()
 */

void *SC_hasharr_get(hasharr *ha, long n)
   {void *rv;

    rv = NULL;
    if ((ha != NULL) && (ha->a != NULL))
       rv = SC_array_get(ha->a, n);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_GET_N - return the number of elements in HA
 *
 * #bind SC_hasharr_get_n fortran() scheme() python()
 */

long SC_hasharr_get_n(hasharr *ha)
   {long ne;

    ne = 0L;
    if (ha != NULL)
       ne = ha->ne;

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_KEY - set the hash key method */

int SC_hasharr_key(hasharr *ha, PFKeyHash hash, PFIntBinC comp)
   {int ret;

    ret = FALSE;

    if (ha != NULL)
       {ha->hash = hash;
	ha->comp = comp;

	ret = TRUE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_REKEY - reset the hash key method */

int SC_hasharr_rekey(hasharr *ha, const char *method)
   {int ret;

    ret = FALSE;

    if ((ha != NULL) && (method != NULL))
       {if (strcmp(method, SC_HA_ADDR_KEY) == 0)
	   {ha->hash = _SC_hasharr_addr;
	    ha->comp = _SC_addr_comp;}
        else if (strcmp(method, SC_HA_NAME_KEY) == 0)
	   {ha->hash = _SC_hasharr_name;
	    ha->comp = (PFIntBinC) strcmp;}
	else
	   {ha->hash = NULL;
	    ha->comp = NULL;};

	if (ha->hash != NULL)
	   {SC_set_count(ha->key_type, 0);
	    CFREE(ha->key_type);
	    ha->key_type = CSTRDUP(method, ha->memfl);};

/* force array to reset access method
 * needed when hasharr has been read from PDB file
 */
	if (ha->a != NULL)
	   ha->a->access = NULL;

	ret = TRUE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_SORT - sort the array part of HA using PRED */

int SC_hasharr_sort(hasharr *ha, PFIntBinC pred)
   {int ret;

    ret = SC_array_sort(ha->a, (PFIntBinC) pred);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPLICE_OUT_HAELEM - if THS matches KEY splice it out of HA */

static int _SC_splice_out_haelem(hasharr *ha, const void *key,
				 haelem **prv, haelem *ths)
   {int ok;
    PFIntBinC comp;

    comp = ha->comp;

    ok = (*comp)(key, ths->name);
    if (ok == 0)
       {*prv = ths->next;
	_SC_haelem_remove(ha, ths);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_REMOVE - remove the entry corresponding to the specified key 
 *                   - return TRUE iff successfully removed 
 *
 * #bind SC_hasharr_remove fortran() scheme() python()
 */

int SC_hasharr_remove(hasharr *ha ARG(,,cls), const void *key)
   {int sz, rv;
    long i;
    haelem *hp, *curr, **tb;
    PFKeyHash hash;

    rv = FALSE;

    sz   = ha->size;
    tb   = ha->table;
    hash = ha->hash;

    i  = (*hash)(key, sz);
    hp = tb[i];

    if (hp != NULL)
       {SC_LOCKON(SC_ha_lock);

/* check for match in first hash element */
	if (_SC_splice_out_haelem(ha, key, &tb[i], hp) == 0)
	   rv = TRUE;

/* otherwise search for the matching hash element */
        else
           for (i = 0;
		(hp != NULL) && (hp->next != NULL) && (rv == FALSE);
		hp = hp->next, i++)
               {curr = hp->next;
		if (_SC_splice_out_haelem(ha, key, &hp->next, curr) == 0)
		   rv = TRUE;};
   
	SC_LOCKOFF(SC_ha_lock);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHARR_DUMP - return an array of strings which are the
 *                 - hash keys matching PATT and TYPE
 *                 - no sorting is done but holes are compressed out
 *                 - the return array is terminated by a NULL entry
 *                 - caller should do SC_free_strings on result when done
 *
 * #bind SC_hasharr_dump fortran() scheme() python()
 */

char **SC_hasharr_dump(hasharr *ha, const char *patt, const char *type,
		       int sort)
   {int ie, ne, ns;
    char *s, **sa;
    haelem *hp, **tb;
    SC_array *arr;

    sa = NULL;

    if ((ha != NULL) && (ha->a != NULL))
       {arr = ha->a;
	ne  = SC_array_get_n(arr);
	tb  = SC_array_array(arr);

	sa = CMAKE_N(char *, ne+1);
	if (sa != NULL)
	   {ns = 0;
	    for (ie = 0; ie < ne; ie++)
	        {hp = tb[ie];
		 if ((hp != NULL) &&
		     ((type == NULL) || (strcmp(type, hp->type) == 0)))
		    {s = hp->name;
		     if ((patt == NULL) || (SC_regx_match(s, patt) == TRUE))
		        {sa[ns++] = s;
		         SC_mark(s, 1);};};};

	    sa[ns] = NULL;

/* sort the names */
	    if ((sort == TRUE) && (HA_ADDR_KEY(ha->hash) == FALSE))
	       SC_string_sort(sa, ns);};

	CFREE(tb);};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DHASH - debugging printout of names in hash array */

void dhash(hasharr *ha, const char *patt, int sort)
   {int i, n;
    char **sa;

    sa = SC_hasharr_dump(ha, patt, NULL, FALSE);
    if (sa != NULL)
       {SC_ptr_arr_len(n, sa);

	if (sort == TRUE)
	   SC_string_sort(sa, n);

        for (i = 0; i < n; i++)
            {if (sa[i] != NULL)
	        io_printf(stdout, "%4d\t%s\n", i, sa[i]);};

        SC_free_strings(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
