/*
 * SCHASH.C - old Hash table structures, functions, and variables
 *          - deprecated 01/2010
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h"

#ifdef HAVE_HASHTAB

#define STRING_KEY(_h)    (_h == (PFKeyHash) _SC_hash_name)

SC_thread_lock
 SC_hash_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/

/*                          HASH FUNCTIONS                                  */

/*--------------------------------------------------------------------------*/

/* _SC_HASH_NAME - compute hash value for string KEY in a table of SIZE */

static long _SC_hash_name(void *key, int size)
   {long v;
    char *s;

    s = (char *) key;

    for (v = 0L; *s != '\0'; s++)
        v = (v << 1) ^ (*s);

    v = labs(v) % (long) size;

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HASH_ADDR - compute hash value for pointer S in a table of SIZE */

static long _SC_hash_addr(void *key, int size)
   {int c;
    long a, v, r, n, i, m;
    SC_address ad;

    ad.memaddr = key;

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

/*                        COMPARISON FUNCTIONS                              */

/*--------------------------------------------------------------------------*/

/* _SC_ADDR_COMP - compare two addresses */

static int _SC_addr_comp(void *key, void *s)
   {int r;

    r = (key != s);

    return(r);}

/*--------------------------------------------------------------------------*/

/*                       CONSTRUCTOR HELPERS                                */

/*--------------------------------------------------------------------------*/

/* SC_HASH_KEY_TYPE - set key strategy for the hash table TAB */

int SC_hash_key_type(HASHTAB *self, char *name, PFKeyHash hash, PFIntBin comp)
   {int ret;

    ret = FALSE;

    if (self != NULL)
       {if (self->key_type != NULL)
           {SC_hash_key_clear(self);}
 
        self->key_type = CSTRSAVE(name);
        self->hash     = hash;
        self->comp     = comp;

        ret = TRUE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_KEY_ADDR - use addresses (void *) as the key */

int SC_hash_key_addr(HASHTAB *self)
   {int ret;

    ret = SC_hash_key_type(self, SC_HASH_ADDR_KEY, 
                           (PFKeyHash) _SC_hash_addr, (PFIntBin) _SC_addr_comp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_KEY_NAME - use names (char *) as the key */

int SC_hash_key_name(HASHTAB *self)
   {int ret;

    ret = SC_hash_key_type(self, SC_HASH_NAME_KEY, 
                           (PFKeyHash) _SC_hash_name, (PFIntBin) strcmp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_KEY_TYPE - return the type of key used */

char *SC_hash_type(HASHTAB *self)
   {

    return(self->key_type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_HASH_TABLE - make an undifferentiated hash table */

HASHTAB *_SC_make_hash_table(int sz, int docflag)
   {HASHTAB *self;
    hashel **tb;
    int i;

/* allocate a new hash table */
    self = CMAKE(HASHTAB);

    if (self != NULL)
       {tb = CMAKE_N(hashel *, sz);

        if (tb != NULL)
           {self->size      = sz;
            self->docp      = docflag;
            self->nelements = 0;
            self->table     = tb;
            self->key_type  = NULL;
            self->hash      = NULL;
            self->comp      = NULL;

/* explicitly NULL the pointers */
            for (i = 0; i < sz; i++)
	        tb[i] = NULL;}

	else
           {CFREE(self);};};

    return(self);}

/*--------------------------------------------------------------------------*/

/*                            CONSTRUCTORS                                  */

/*--------------------------------------------------------------------------*/

/* SC_MAKE_HASH_TABLE - allocate and initialize a hash table of size SZ
 *                    - that will use names (char *) as keys
 *                    - returns a HASHTAB pointer
 */

HASHTAB *SC_make_hash_table(int sz, int docflag)
   {HASHTAB *self;

    self = _SC_make_hash_table(sz, docflag);
    SC_hash_key_name(self);

    return(self);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_ADDR_TABLE - allocate and initialize a hash table of size SZ
 *                    - that will use addresses (void *) as keys
 *                    - returns a HASHTAB pointer
 */

HASHTAB *SC_make_addr_table(int sz, int docflag)
   {HASHTAB *self;

    self = _SC_make_hash_table(sz, docflag);
    SC_hash_key_addr(self);

    return(self);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_ALLOC - allocate and initialize a default hash table 
 *               - that will use names (char *) as keys
 *               - and will be an intermediate size, with doc turned on
 *               - returns a HASHTAB pointer
 */

HASHTAB *SC_hash_alloc(void)
   {HASHTAB *tb;

    tb = SC_make_hash_table(HSZLRINT, DOC);

    return(tb);}

/*--------------------------------------------------------------------------*/

/*                             DESTRUCTOR                                   */

/*--------------------------------------------------------------------------*/

/* SC_RL_HASH_TABLE - release a hash table */

void SC_rl_hash_table(HASHTAB *self)
   {

/* call SC_HASH_CLR first to release the contents of the table */
    if (SC_safe_to_free(self))
       {SC_hash_clr(self);

	if (self != NULL)
	   {CFREE(self->key_type);
	    CFREE(self->table);};};

    CFREE(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_FREE - Synonym (matching SC_hash_alloc) for SC_rl_hash_table
 *              - release a hash table 
 */

void SC_hash_free(HASHTAB *self)
   {
  
    SC_rl_hash_table(self);

    return;}
 
/*--------------------------------------------------------------------------*/

/*                           KEY LISTS                                      */

/*--------------------------------------------------------------------------*/

/* _SC_DUMP_HASH - return an array of pointers whose values point to the
 *               - keys of the elements in the given hash table
 */

char **_SC_dump_hash(HASHTAB *self, char *patt, char *type, int sort)
   {int i, sz, nkeys;
    char **keyptr;
    hashel *hp, **tb;

    if (self == NULL)
       return(NULL);

/* allocate a list of pointers to the keys in the hash table */
    keyptr = CMAKE_N(char *, self->nelements);

    if (keyptr == NULL)
       return(NULL);

/* fill in the list of pointers to keys in the hash table */
    sz = self->size;
    tb = self->table;

    nkeys = 0;

    for (i = 0; i < sz; i++)
        {for (hp = tb[i]; hp != NULL; hp = hp->next)
             {if ((type == NULL) || (strcmp(type, hp->type) == 0))
                 {if (patt == NULL)
                     {keyptr[nkeys++] = hp->name;}
                  else if (SC_regx_match(hp->name, patt))
                     {keyptr[nkeys++] = hp->name;}
                  else
                     { /* DO NOTHING */ };};};}

/* check that the number of names found is what is expected */
    if (nkeys > self->nelements)
       return(NULL);

    CREMAKE(keyptr, char *, nkeys + 1);
    keyptr[nkeys] = NULL;

/* sort the names
 * GOTCHA: HASHTAB should maintain a sort function pointer
 */
    if (sort && STRING_KEY(self->hash))
       {SC_string_sort(keyptr, nkeys);}

    return(keyptr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DUMP_HASH - return an array of pointers whose entries point to the
 *              - keys of the elements in the given hash table
 *              - GOTCHA: HASHTAB should maintain a sort function pointer
 */

char **SC_dump_hash(HASHTAB *self, char *patt, int sort)
   {char **str;

    str = _SC_dump_hash(self, patt, NULL, sort);

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_DUMP - return an array of pointers whose entries point to the
 *              - keys of the elements in the given hash table
 *              - and are alphabetically ordered (by strcmp)
 *              - GOTCHA: HASHTAB should maintain a sort function pointer
 */

char **SC_hash_dump(HASHTAB *self, char *patt)
   {char **str;

    str = _SC_dump_hash(self, patt, NULL, TRUE);

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_KEYS - return an array of pointers whose entries point to the
 *              - keys of the elements in the given hash table
 *              - GOTCHA: HASHTAB should maintain a sort function pointer
 */

char **SC_hash_keys(HASHTAB *self)
   {char **str;

    str = _SC_dump_hash(self, NULL, NULL, FALSE);

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_SPECIFIC_KEYS - return an array of pointers whose entries point to the
 *                       - keys of the elements in the given hash table
 *                       - that match a pattern patt and may be sorted
 *                       - GOTCHA: HASHTAB should maintain a sort function pointer
 */

char **SC_hash_specific_keys(HASHTAB *self, char *patt, int sort)
   {char **str;

    str = _SC_dump_hash(self, patt, NULL, sort);

    return(str);}

/*--------------------------------------------------------------------------*/

/*                             INSERTION                                    */

/*--------------------------------------------------------------------------*/

/* _SC_INSTALL - install an object in the hash table
 *             - WARNING: do NOT use literals or volatiles for the type;
 *             -          for efficiency they are not strsave'd !!!
 */

hashel *_SC_install(void *key, void *obj, char *type, HASHTAB *self,
                    int mark, int lookup)
   {hashel *hp;
    long hv;

/* sanity checks */
    if ((self == NULL) || (key == NULL))
       return(NULL);

    SC_LOCKON(SC_hash_lock);

/* is it already in the hash table? */
    if (lookup)
       hp = SC_lookup(key, self);
    else
       hp = NULL;

/* if not found, then insert it */
    if (hp == NULL)
       {hp = CMAKE(hashel);
        if (hp != NULL)

/* setup the key */
           {if (STRING_KEY(self->hash))
               {hp->name = CSTRSAVE(key);
                if (hp->name == NULL)
                   CFREE(hp);}
            else
               hp->name = key;

	    if (hp != NULL)

/* add to the head of the bucket */
	       {hv = (*(self->hash))(hp->name, self->size);

	        hp->next = self->table[hv];
		self->table[hv] = hp;

		(self->nelements)++;};};};

/* update valid hash elements inserted or looked-up */
    if (hp != NULL)
       {hp->type = type;
        hp->def  = obj;

/* do any marking (reference counting) for the new obj */
        if (mark)
           hp->free = (SC_mark(obj, 1) != -1);
        else
           hp->free = FALSE;};

    SC_LOCKOFF(SC_hash_lock);

    return(hp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INSTALL - install an object in the hash table and mark it
 *            - WARNING: do NOT use literals or volatiles for the type;
 *            -          for efficiency they are not strsavef'd !!!
 */

hashel *SC_install(void *key, void *obj, char *type, HASHTAB *self)
   {hashel *hp;

    hp = _SC_install(key, obj, type, self, TRUE, TRUE);

    return(hp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INSTALL - install an object in the hash table and mark it
 *            - WARNING: do NOT use literals or volatiles for the type;
 *            -          for efficiency they are not strsavef'd !!!
 */

int SC_hash_insert(HASHTAB *self, char *type, void *key, void *obj)
   {int rv;
    hashel *hp;

    hp = _SC_install(key, obj, type, self, TRUE, TRUE);
    rv = (hp != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_INS - install an object pointed to by iter into the hash table 
 *             - returns -1 on failure (cannot insert from a table
 *             - into the very same hash table)
 *             - WARNING: do NOT use literals or volatiles for the type;
 *             -          for efficiency they are not strsavef'd !!!
 */

int SC_hash_ins(HASHTAB *self, hashiter *iter)
   {int status;

    status = 0;

    if (self == NULL || iter == NULL || self == iter->htab || iter->curr == NULL)
       status = -1;

    else  
       _SC_install(SC_hashiter_key(iter), SC_hashiter_value(iter), 
		   SC_hashiter_value_type(iter), self, TRUE, TRUE);

    return(status);}

/*--------------------------------------------------------------------------*/

/*                           DELETION                                       */

/*--------------------------------------------------------------------------*/

/* _SC_FREE_HASHEL - free the hashel HP */

static void _SC_free_hashel(hashel *hp, PFIntUn rel, int string)
   {

    if (rel != NULL)
       (*rel)(hp->def);

/* undo the MARK in SC_install */
    else if (hp->free == TRUE)
       CFREE(hp->def);

    if (string)
       CFREE(hp->name);

    CFREE(hp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_REMOVE - synonym for SC_hash_rem with args in proper order
 *                - remove the entry corresponding to the specified key 
 *                - return TRUE iff successfully removed 
 */

int SC_hash_remove(HASHTAB *self, void *key)
   {int rv;

    rv = SC_hash_rem(key, self);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPLICE_OUT_HASHEL - if THS matches KEY splice it out of TAB */

static int _SC_splice_out_hashel(HASHTAB *tab, void *key,
				 hashel **prv, hashel *ths, int string)
   {int ok;
    PFIntBin comp;

    comp = tab->comp;

    ok = (*comp)(key, ths->name);
    if (ok == 0)
       {*prv = ths->next;

	_SC_free_hashel(ths, NULL, string);

	tab->nelements--;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_REM - remove the entry corresponding to the specified key 
 *             - return TRUE iff successfully removed 
 */

int SC_hash_rem(void *key, HASHTAB *tab)
   {int sz, string, rv;
    long i;
    hashel *hp, *curr, **tb;
    PFKeyHash hash;

    rv = FALSE;

    sz   = tab->size;
    tb   = tab->table;
    hash = tab->hash;

    string = STRING_KEY(hash);

    i  = (*hash)(key, sz);
    hp = tb[i];

    if (hp != NULL)
       {SC_LOCKON(SC_hash_lock);

/* check for match in first hash element */
	if (_SC_splice_out_hashel(tab, key, &tb[i], hp, string) == 0)
	   rv = TRUE;

/* otherwise search for the matching hash element */
        else
           for (i = 0;
		(hp != NULL) && (hp->next != NULL) && (rv == FALSE);
		hp = hp->next, i++)
               {curr = hp->next;
		if (_SC_splice_out_hashel(tab, key, &hp->next, curr, string) == 0)
		   rv = TRUE;};
   
	SC_LOCKOFF(SC_hash_lock);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_CLR - remove all entries in the given hash table */

void SC_hash_clr(HASHTAB *tab)
   {int i, sz, string;
    hashel **tb, *hp, *next;

    sz = tab->size;
    tb = tab->table;

    string = STRING_KEY(tab->hash);

    for (i = 0; i < sz; i++)
        {for (hp = tb[i]; hp != NULL; hp = next)
             {next = hp->next;
	      _SC_free_hashel(hp, NULL, string);};
         tb[i] = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_CLEAN - release the storage after applying FNC to each element */

void SC_hash_clean(HASHTAB *tab, int (*fnc)(hashiter *iter))
   {hashiter *iter;

    if (tab != NULL && fnc != NULL && SC_safe_to_free(tab))
       {iter = SC_hashiter_alloc(tab);

        if (fnc != NULL)
           {do {(*fnc)(iter);}
            while (SC_hashiter_inc(iter) == 0);}
      
        SC_hashiter_free(iter);}

    SC_hash_free(tab);

    return;}    

/*--------------------------------------------------------------------------*/

/*                            LOOK-UP ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* SC_LOOKUP - lookup the hashel for the given KEY */

hashel *SC_lookup(void *key, HASHTAB *tab)
   {int sz;
    long hv;
    hashel *rv, *hp, **tb;
    PFKeyHash hash;
    PFIntBin comp;

    rv = NULL;

/* sanity checks */
    if ((key != NULL) && (tab != NULL))
       {SC_LOCKON(SC_hash_lock);

        hash = tab->hash;
	comp = tab->comp;

	sz = tab->size;
	tb = tab->table;
	hv = (*hash)(key, sz);

	for (hp = tb[hv]; hp != NULL; hp = hp->next)
	    {if ((*comp)(key, hp->name) == 0)
	        {rv = hp;
		 break;};};

	SC_LOCKOFF(SC_hash_lock);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DEF_LOOKUP - lookup the actual object for the given KEY */

void *SC_def_lookup(void *key, HASHTAB *tab)
   {hashel *hp;
    void *obj;
  
    obj = NULL;

    if (tab != NULL)
       {hp  = SC_lookup(key, tab);
	obj = (hp != NULL) ? hp->def : NULL;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_LOOKUP - lookup the actual object for the given KEY */

void *SC_hash_lookup(HASHTAB *tab, void *key)
   {void *rv;

    rv = SC_def_lookup(key, tab);
 
    return(rv);}
 
/*--------------------------------------------------------------------------*/

/*                             META DATA                                    */

/*--------------------------------------------------------------------------*/

/* SC_HASH_SIZE - return the number of elements in the hash table */

int SC_hash_size(HASHTAB *tab)
   {int size;

    if (tab == NULL)
       size = -1;
    else
       size = tab->nelements;

    return(size);}

/*--------------------------------------------------------------------------*/

/*                               SIZING                                     */

/*--------------------------------------------------------------------------*/

/* SC_HASH_BUCKET_COUNT - return the number of buckets in the hash table */

int SC_hash_bucket_count(HASHTAB *tab)
   {int buckets;

    if (tab == NULL)
       buckets = -1;
    else 
       buckets = tab->size;

    return(buckets);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_HASH_RESIZE - resize so that we use BUCKETS worth of buckets */

void SC_hash_resize(HASHTAB *tab, int buckets)
   {HASHTAB *temp;
    hashiter *iter;

    if ((tab != NULL) && (buckets > 0))

/* handle the case of an empty table */
       {if (tab->nelements == 0)
           CREMAKE(tab->table, hashel *, buckets);

        else 
           {temp = SC_hash_alloc();
    
/* make sure they use the same key type */
            if (strcmp(SC_hash_type(tab), SC_HASH_ADDR_KEY) == 0)
               {SC_hash_key_addr(temp);}
        
/* put all of the hashels into a temp hash table */ 
            iter = SC_hashiter_alloc(tab);
        
            do {SC_hash_ins(temp, iter);}
            while (SC_hashiter_inc(iter) == 0);
        
            SC_hashiter_free(iter);
        
/* clear the table */
            SC_hash_clr(tab);
         
/* resize the table */
            CREMAKE(tab->table, hashel *, buckets);
        
/* reinsert the hashel's from the temp table */
            iter = SC_hashiter_alloc(temp);
        
            do
               {SC_hash_ins(tab, iter);}
            while (SC_hashiter_inc(iter) == 0);
        
            SC_hashiter_free(iter);
        
            CFREE(temp);};}

    return;}
   
/*--------------------------------------------------------------------------*/

/*                Functions that should be made private                     */

/*--------------------------------------------------------------------------*/

/* SC_HASH_KEY_CLEAR - clear the key strategy for the hash table TAB
 *                   - really useful for reinitializing hash tables
 *                   - GOTCHA: changes only part of our internal structure
 *                   -      and does not alter any associated hashels
 *                   -      that may make sense only if key strategy is 
 *                   -      present (should be made into a reset function?)
 */

void SC_hash_key_clear(HASHTAB *tab)
   {

    if (tab != NULL)
       {CFREE(tab->key_type);

        tab->hash = NULL;
        tab->comp = NULL;};

    return;}

/*--------------------------------------------------------------------------*/

/*             Code below here is suggested for deprecation                 */

/*--------------------------------------------------------------------------*/

/* SC_HASH_CLR_TABLE - release the storage associated with a homogeneous
 *                   - hash table
 */

void SC_hash_clr_table(HASHTAB *tab, PFIntUn rel)
   {int i, n, string;
    hashel **tb, *hp, *next;

    if (SC_safe_to_free(tab))
       {n  = tab->size;
        tb = tab->table;

	string = STRING_KEY(tab->hash);

        for (i = 0; i < n; i++)
            {for (hp = tb[i]; hp != NULL; hp = next)
                 {next = hp->next;
		  _SC_free_hashel(hp, rel, string);};
             tb[i] = NULL;};};

    SC_rl_hash_table(tab);

    return;}    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_CLN_TABLE - release the storage associated with an inhomogeneous
 *                   - hash table
 */

void SC_hash_cln_table(HASHTAB *tab, PFIntUn rel)
   {int i, n, string;
    hashel **tb, *hp, *next;

    if (SC_safe_to_free(tab))
       {n  = tab->size;
        tb = tab->table;

	string = STRING_KEY(tab->hash);

        for (i = 0; i < n; i++)
            {for (hp = tb[i]; hp != NULL; hp = next)
                 {next = hp->next;
  
                  if (rel != NULL)
                     {(*rel)(hp);}
            
		  if (string)
		     {CFREE(hp->name);};

                  CFREE(hp);};

             tb[i] = NULL;};};

    SC_rl_hash_table(tab);

    return;}    

/*--------------------------------------------------------------------------*/

/*                             HASH ITER                                    */

/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/

/*                            CONSTRUCTORS                                  */

/*--------------------------------------------------------------------------*/

/* SC_HASHITER_ALLOC - allocate the storage associated with a hash iterator
 *                   - and init its state to the first hashel in the table
 */

hashiter *SC_hashiter_alloc(HASHTAB *htab)
   {hashiter *self; 

    if (htab != NULL) 
       {self = CMAKE(hashiter);

        self->htab       = htab;
        self->match_type = NULL;
        self->bucket     = 0;
        self->prev       = NULL;
        self->curr       = NULL;
        self->didDelete  = FALSE;
   
/* find the very first valid hashel in the table */
        SC_hashiter_reset(self);}

    else 
       self = NULL;

    return(self);}

/*--------------------------------------------------------------------------*/

/*                             DESTRUCTOR                                   */

/*--------------------------------------------------------------------------*/

/* SC_HASHITER_FREE - release the storage associated with a hash iterator */

void SC_hashiter_free(hashiter *self)
   {

    if ((self != NULL) && (self->match_type != NULL))
       {CFREE(self->match_type);}

    CFREE(self);

    return;}

/*--------------------------------------------------------------------------*/

/*                              SET KIND                                    */

/*--------------------------------------------------------------------------*/

/* SC_HASHITER_KIND - set the value type to iterate over */

void SC_hashiter_kind(hashiter *self, char *type)
   {

    if ((self != NULL) && (self->match_type != NULL))
       {CFREE(self->match_type);};

    if ((self != NULL) && (type != NULL))
       self->match_type = CSTRSAVE(type);

    return;}

/*--------------------------------------------------------------------------*/

/*                              DELETION                                    */

/*--------------------------------------------------------------------------*/

/* SC_HASHITER_DEL - release the storage of the element that we are currently 
 *                 - pointing at in the hash table
 *                 - increment the iterator to the next valid position
 *                 - return -1 on failure (cannot delete if curr is NULL) 
 */

int SC_hashiter_del(hashiter *self)
   {int status, freeKey, bucket;
    hashel *prev, *curr;

    if (self == NULL)
       return(-1);

    status  = 0;
    freeKey = STRING_KEY(self->htab->hash);

/* do not delete at invalid iterator positions */
    if ((self != NULL) && (self->curr != NULL))
       {SC_LOCKON(SC_hash_lock);

/* remember where we were */
        prev   = self->prev;
        curr   = self->curr;
        bucket = self->bucket;

/* first move up one position */
        SC_hashiter_inc(self);

/* next remember we deleted */
        self->didDelete = TRUE;

/* prev is still old prev due to imminent delete */
        self->prev = prev;

/* then delete at middle or end of linked list */
        if (prev != NULL)
           prev->next = curr->next;

/* then delete at start of linked list */
        else
           self->htab->table[bucket] = curr->next;

/* undo the SC_mark done during installation */
        if (curr->free == TRUE)
           {CFREE(curr->def);}

/* free key if we are using SC_HASH_NAME_KEYs */
        if (freeKey)
           {CFREE(curr->name);}

/* finally actually free the hashel itself */
        CFREE(curr);
        (self->htab->nelements)--;

        SC_LOCKOFF(SC_hash_lock);}

    else
       status = -1;

    return(status);}

/*--------------------------------------------------------------------------*/

/*                              MOVEMENT                                    */

/*--------------------------------------------------------------------------*/

/* SC_HASHITER_INC - increment the iterator to the next element 
 *                 - return -1 if we have reached the end
 */

int SC_hashiter_inc(hashiter *self)
   {int size, status, stop;
    hashel **tb;
    char *type;

    status = 0;

/* sanity check: make sure self->curr is valid */
    if ((self != NULL) && (self->curr != NULL))
       {stop = FALSE;
        type = self->match_type;
        size = self->htab->size;
        tb   = self->htab->table;

/* first attempt to move by one within the bucket */
        self->prev      = self->curr;
        self->curr      = self->curr->next;
        self->didDelete = FALSE;
 
/* stop if this is a valid element, otherwise continue on */ 

/* is this the end of the table? */
        while ((stop == FALSE) && (self->bucket < size))

/* is this the end of the bucket? */
            {while ((stop == FALSE) && (self->curr != NULL))
                 {if ((type == NULL) || (strcmp(type, self->curr->type) == 0))
                     stop = TRUE;
                  else
                     {self->prev = self->curr; 
                      self->curr = self->curr->next;};}

/* if we are not stopping, iterate to the next bucket */
             if (stop == FALSE)
                {self->bucket++;
                 if (self->bucket < size)
                    {self->prev = NULL;
                     self->curr = tb[self->bucket];};};}

        if (self->bucket == size)
           status = -1;}
    else 
       status = -1;
 
    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHITER_RESET - reset the iterator to the beginning of the table */

void SC_hashiter_reset(hashiter *self)
   {int size;
    hashel **tab, *hp;
    char *type;

    if (self != NULL)
       {self->bucket = 0;
        self->prev   = NULL;
        self->curr   = NULL;
        self->didDelete = FALSE;
    
        size = self->htab->size;
        tab  = self->htab->table;
        type = self->match_type;
    
/* find the first valid bucket */
        while (self->curr == NULL && self->bucket < size)
            {hp = tab[self->bucket]; 
    
             if (hp != NULL) 
                {self->curr = hp;}
             else
                {self->bucket++;};};
    
/* if the type match fails, increment to the next valid position */
        if ((type != NULL) && (self->curr != NULL) && 
            (strcmp(type, self->curr->type) != 0))
           {SC_hashiter_inc(self);};};

    return;}

/*--------------------------------------------------------------------------*/

/*                              META-DATA                                   */

/*--------------------------------------------------------------------------*/

/* SC_HASHITER_KEY - return the key at curr */

char *SC_hashiter_key(hashiter *self)
   {char *key;

    if ((self != NULL) && (self->curr != NULL))
       key = self->curr->name;
    else 
       key = NULL;

    return(key);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHITER_KEY_TYPE - return the type of the key at curr */

char *SC_hashiter_key_type(hashiter *self)
   {char *key_type;

    if ((self != NULL) && (self->htab != NULL))
       key_type = self->htab->key_type;
    else 
       key_type = NULL;

    return(key_type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHITER_VALUE - return the value at curr */

void *SC_hashiter_value(hashiter *self)
   {void *value;

    if ((self != NULL) && (self->curr != NULL))
       value = self->curr->def;
    else
       value = NULL;

    return(value);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASHITER_VALUE_TYPE - return the type of the value at curr */

char *SC_hashiter_value_type(hashiter *self)
   {char *value_type;

    if ((self != NULL) && (self->curr != NULL))
       value_type = self->curr->type;
    else 
       value_type = NULL;

    return(value_type);}

/*--------------------------------------------------------------------------*/

/*                           ITERATION                                      */

/*--------------------------------------------------------------------------*/

/* SC_HASHITER_APPLY - iterate operator FNC over the elements of TAB
 *                   - FNC's return value is treated as follows: 
 *                   -    Zero causes the iteration to continue
 *                   -    Non-zero causes the iteration to immediately 
 *                   -    return that value
 *                   - return -1 on error otherwise return FNC's last status
 */

int SC_hashiter_apply(hashiter *self, int (*fnc)(hashiter *i, void *a),
		      void *a)
   {int stop, status;

    if (self == NULL || fnc == NULL)
       status = -1;

    else if (self->curr == NULL)
       status = 0;

    else 
       {status = 0;
        stop   = 0;

/* apply the function while it continues to return zero */
        while ((status == 0) && (stop == 0))
           {status = (*fnc)(self, a);
           
/* only spend time incrementing if status is still zero */ 
            if (status == 0)

/* only increment if we did NOT delete */
               {if (self->didDelete)
                   {self->didDelete = FALSE; 
                    stop = (self->curr == NULL);}

                else
                   stop = (SC_hashiter_inc(self) != 0);};};};

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HASH_FOREACH - apply FNC to each element of TAB */

int SC_hash_foreach(HASHTAB *tab, int (*f)(hashiter *i, void *a), void *a)
   {int st;
    hashiter *it;

    it = SC_hashiter_alloc(tab);
    if (it != NULL)
       {st = SC_hashiter_apply(it, f, a);

	SC_hashiter_free(it);}

    else
       st = -1;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
