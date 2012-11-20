/*
 * LIBHASH.C - quick and dirty hash tables
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBHASH

# define LIBHASH

# include "common.h"

# ifndef SCOPE_SCORE_COMPILE

#define HSZSMALL   31             /* small table size */
#define HSZSMINT   67             /* small intermediate table size */
#define HSZLRINT  127             /* table size for variables*/
#define HSZLARGE  521             /* large table size */
#define HSZHUGE  4483             /* huge table size */

typedef int (*PFintu)(void *a);
typedef int (*PFintb)(void *a, void *b);
typedef long (*PFhashkey)(void *s, int size);
typedef struct s_hashen hashen;
typedef struct s_hashtab hashtab;

struct s_hashen                 
   {char *name;
    char *type;
    void *def;
    int free;
    hashen *next;};

struct s_hashtab
   {int size;              /* number of bins in hash table */
    int ne;                /* number of entries in hash table */
    PFhashkey hash;
    PFintb comp;
    hashen **table;};

# endif
# ifndef SCOPE_SCORE_PREPROC

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _HASH_NAME - compute hash value for string KEY in a table of SIZE */

static long _hash_name(void *key, int size)
   {long v;
    char *s;

    s = (char *) key;

    for (v = 0L; *s != '\0'; s++)
        v = (v << 1) ^ (*s);

    v = labs(v) % (long) size;

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_HASH_TABLE - make an undifferentiated hash table */

hashtab *make_hash_table(int sz)
   {hashtab *tab;
    hashen **tb;
    int i;

    if (sz < 0)
       sz = HSZLARGE;

/* allocate a new hash table */
    tab = MAKE(hashtab);

    if (tab != NULL)
       {tb = MAKE_N(hashen *, sz);

        if (tb != NULL)
           {tab->size  = sz;
            tab->ne    = 0;
            tab->table = tb;
            tab->hash  = _hash_name;
            tab->comp  = (PFintb) strcmp;

/* explicitly NULL the pointers */
            for (i = 0; i < sz; i++)
	        tb[i] = NULL;}

	else
           FREE(tab);};

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _FREE_HASHEN - free the hashen HP */

static void _free_hashen(hashen *hp, PFintu rel)
   {

    if (rel != NULL)
       rel(hp->def);

    FREE(hp->name);
    FREE(hp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SPLICE_OUT_HASHEN - if THS matches KEY splice it out of TAB */

static int _splice_out_hashen(hashtab *tab, void *key,
			      hashen **prv, hashen *ths)
   {int ok;
    PFintb comp;

    comp = tab->comp;

    ok = comp(key, ths->name);
    if (ok == 0)
       {*prv = ths->next;

	_free_hashen(ths, NULL);

	tab->ne--;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_REMOVE - remove the entry corresponding to the specified KEY 
 *             - return TRUE iff successfully removed 
 */

int hash_remove(hashtab *tab, void *key)
   {int sz, ok, rv;
    long i;
    hashen *hp, *curr, **tb;
    PFhashkey hash;

    rv = FALSE;

    sz   = tab->size;
    tb   = tab->table;
    hash = tab->hash;

    i  = hash(key, sz);
    hp = tb[i];

    if (hp != NULL)

/* check for match in first hash element */
       {if (_splice_out_hashen(tab, key, &tb[i], hp) == 0)
	   rv = TRUE;

/* otherwise search for the matching hash element */
        else
           for (i = 0;
		(hp != NULL) && (hp->next != NULL) && (rv == FALSE);
		hp = hp->next, i++)
               {curr = hp->next;
		ok   = _splice_out_hashen(tab, key, &hp->next, curr);
		if (ok == 0)
		   rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_CLEAR - remove all entries in the given hash table TAB
 *            - does not free the table
 */

void hash_clear(hashtab *tab)
   {int i, sz;
    hashen **tb, *hp, *next;

    sz = tab->size;
    tb = tab->table;

    for (i = 0; i < sz; i++)
        {for (hp = tb[i]; hp != NULL; hp = next)
             {next = hp->next;
	      _free_hashen(hp, NULL);};
         tb[i] = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_FREE - release a hash table */

void hash_free(hashtab *tab)
   {

    hash_clear(tab);

    FREE(tab->table);
    FREE(tab);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_LOOKUP - lookup the hashen for the given KEY */

hashen *hash_lookup(hashtab *tab, void *key)
   {int sz;
    long hv;
    hashen *rv, *hp, **tb;
    PFhashkey hash;
    PFintb comp;

    rv = NULL;

/* sanity checks */
    if ((key != NULL) && (tab != NULL))
       {hash = tab->hash;
	comp = tab->comp;

	sz = tab->size;
	tb = tab->table;
	hv = hash(key, sz);

	for (hp = tb[hv]; hp != NULL; hp = hp->next)
	    {if (comp(key, hp->name) == 0)
	        {rv = hp;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_DEF_LOOKUP - lookup the actual object for the given KEY */

void *hash_def_lookup(hashtab *tab, void *key)
   {hashen *hp;
    void *obj;
  
    obj = NULL;

    if (tab != NULL)
       {hp  = hash_lookup(tab, key);
	obj = (hp != NULL) ? hp->def : NULL;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_INSTALL - install an object in the hash table
 *              - WARNING: do NOT use literals or volatiles for the type;
 *              - for efficiency they are not strsave'd
 */

hashen *hash_install(hashtab *tab, void *key, void *obj, char *type,
		     int lookup)
   {hashen *hp;
    long hv;

/* sanity checks */
    if ((tab == NULL) || (key == NULL))
       return(NULL);

/* is it already in the hash table? */
    if (lookup)
       hp = hash_lookup(tab, key);
    else
       hp = NULL;

/* if not found, then insert it */
    if (hp == NULL)
       {hp = MAKE(hashen);
        if (hp != NULL)

/* setup the key */
	   {hp->name = STRSAVE(key);
	    if (hp->name == NULL)
	       FREE(hp);

	    if (hp != NULL)

/* add to the head of the bucket */
	       {hv = tab->hash(hp->name, tab->size);

	        hp->next = tab->table[hv];
		tab->table[hv] = hp;

		(tab->ne)++;};};};

/* update valid hash elements inserted or looked-up */
    if (hp != NULL)
       {hp->type = type;
        hp->def  = obj;};

    return(hp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_FOREACH - return an array of pointers whose values point to the
 *              - keys of the elements in the given hash table
 */

int hash_foreach(hashtab *tab, int (*fnc)(hashen *hp, void *a), void *a)
   {int i, rv, sz;
    hashen *hp, **tb;

    rv = TRUE;

    if (tab != NULL)
       {sz = tab->size;
	tb = tab->table;

	for (i = 0; i < sz; i++)
	    {for (hp = tb[i]; hp != NULL; hp = hp->next)
		 {if (hp != NULL)
		     rv &= fnc(hp, a);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HASH_DUMP - return an array of pointers whose values point to the
 *           - keys of the elements in the given hash table
 */

char **hash_dump(hashtab *tab, char *patt, char *type, int sort)
   {int i, sz, ns;
    char **sa;
    hashen *hp, **tb;

    if (tab == NULL)
       return(NULL);

/* allocate a list of pointers to the keys in the hash table */
    sa = MAKE_N(char *, tab->ne);

    if (sa == NULL)
       return(NULL);

/* fill in the list of pointers to keys in the hash table */
    sz = tab->size;
    tb = tab->table;

    ns = 0;

    for (i = 0; i < sz; i++)
        {for (hp = tb[i]; hp != NULL; hp = hp->next)
             {if ((type == NULL) || (strcmp(type, hp->type) == 0))
                 {if (patt == NULL)
                     sa[ns++] = hp->name;
                  else if (match(hp->name, patt))
                     sa[ns++] = hp->name;};};};

/* check that the number of names found is what is expected */
    if (ns > tab->ne)
       {FREE(sa);}

    else
       {REMAKE(sa, char *, ns + 1);
	if (sa != NULL)
	   {sa[ns] = NULL;

/* sort the names
 * GOTCHA: hashtab should maintain a sort function pointer
 */
	    if (sort)
	       string_sort(sa, ns);};};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

# endif
#endif
