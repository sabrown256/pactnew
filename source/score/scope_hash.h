/*
 * SCOPE_HASH.H - header for PACT hash routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_SCOPE_HASH

#define PCK_SCOPE_HASH

#include "scope_array.h"

#define HAVE_HASHTAB

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                                 DEFINES                                  */

/*--------------------------------------------------------------------------*/

#define DOC        1              /* indicate the presence of documentation */
#define NODOC      0              /* indicate no documentation */

#define SC_HA_NAME_KEY       "by-name"
#define SC_HA_ADDR_KEY       "by-address"

/*--------------------------------------------------------------------------*/

/*                                TYPEDEFS                                  */

/*--------------------------------------------------------------------------*/

typedef long (*PFKeyHash)(void *s, int size);         /* hash */

/* new hasharr structures */

typedef struct s_haelem haelem;
typedef struct s_hasharr hasharr;

struct s_haelem                 
   {long iht;                /* index of hash bucket */
    long iar;                /* index into array */
    char *name;              /* string on which the hash function operates */
    char *type;              /* type of def member */
    void *def;               /* actual object stored */
    int free;                /* whether def member was marked (SC_mark?) */
    haelem *next;};          /* next haelem in linked list */

#define PD_DEFINE_HAELEM(_f)                                       \
   {PD_defstr(_f, "haelem",                                        \
	      "long iht",                                          \
	      "long iar",                                          \
	      "char *name",                                        \
	      "char *type",                                        \
	      "char *def",                                         \
	      "int free",                                          \
	      "haelem *next",                                      \
	      LAST);                                               \
    PD_cast(_f, "haelem", "def", "type");}

/*
 * #bind struct hasharr
 *
 * MEMFL bits:
 *     1  permanent memory
 *     2  accountable memory
 *     4  do not set thread locks on access
 */

struct s_hasharr
   {int size;                /* number of buckets */
    int docp;                /* TRUE iff doc available in hash elements */
    int memfl;               /* memory control flags */
    long ne;                 /* number of elements */
    PFKeyHash hash;          /* hash function */
    PFIntBin comp;           /* key equality comparison function */
    char *key_type;          /* type of the key used: name or address */
    haelem **table;          /* size worth of buckets of haelem pointers */
    SC_array *a;};

#define PD_DEFINE_HASHARRAY(_f)                                    \
    PD_defstr(_f, "hasharr",                                       \
	      "int size",                                          \
	      "int docp",                                          \
	      "int memfl",                                         \
	      "long ne",                                           \
	      "function hash",                                     \
	      "function comp",                                     \
	      "char *key_type",                                    \
	      "haelem **table",                                    \
	      "SC_array *a",                                       \
	      LAST)

/*--------------------------------------------------------------------------*/

/*                           FUNCTION DECLARATIONS                          */

/*--------------------------------------------------------------------------*/

/* SCTAB.C declarations */

extern hasharr
 *SC_make_hasharr(int sz, int docflag, char *lm, int flags);

extern haelem
 *SC_hasharr_install(hasharr *ha, void *key, void *obj, char *type,
		     int64_t flags, int lookup),
 *SC_hasharr_lookup(hasharr *ha, void *key);

extern void
 SC_free_hasharr(hasharr *ha, int (*f)(haelem *hp, void *a), void *a),
 *SC_hasharr_def_lookup(hasharr *ha, void *key),
 *SC_hasharr_get(hasharr *ha, long n);

extern int
 SC_haelem_data(haelem *hp, char **pname, char **ptype, void **po, int svr),
 SC_hasharr_remove(hasharr *ha, void *key),
 SC_hasharr_free_n(void *d, void *a),
 SC_hasharr_foreach(hasharr *ha, int (*f)(haelem *hp, void *a), void *a),
 SC_hasharr_next(hasharr *ha, long *pi, char **pname, char **ptype, void **po),
 SC_hasharr_clear(hasharr *ha, int (*f)(haelem *hp, void *a), void *a),
 SC_hasharr_key(hasharr *ha, PFKeyHash hash, PFIntBin comp),
 SC_hasharr_rekey(hasharr *ha, char *method),
 SC_hasharr_sort(hasharr *ha, PFIntBin pred);

extern long
 SC_hasharr_get_n(hasharr *ha),
 SC_hasharr_prune(hasharr *ha,
		  int (*pred)(hasharr *ha, haelem *hp, void *a),
		  void *a);

extern char
 **SC_hasharr_dump(hasharr *ha, char *patt, char *type, int sort);


/* for FORTRAN hasharr API */

extern int
 SC_free_hash_dump(void),
 SC_set_hash_dump(hasharr *ha, int (*fun)(haelem **v, int n));

extern char
 *SC_get_entry(int n);

/*--------------------------------------------------------------------------*/

#ifdef HAVE_HASHTAB

/*--------------------------------------------------------------------------*/

/* old Hash table structures, functions, and variables - deprecated 01/2010 */

#define SC_HASH_NAME_KEY  "by-name"
#define SC_HASH_ADDR_KEY  "by-address"

/*--------------------------------------------------------------------------*/

/*                                TYPEDEFS                                  */

/*--------------------------------------------------------------------------*/

typedef void (*PFSort)(void **list, int n);

typedef struct s_hashel hashel;
typedef struct s_HASHTAB HASHTAB;
typedef struct s_hashiter hashiter;

struct s_hashel
   {char *name;
    char *type;
    void *def;
    int free;
    hashel *next;};

struct s_HASHTAB
   {int size;
    int nelements;
    int docp;
    PFKeyHash hash;
    PFIntBin comp;
    char *key_type;
    hashel **table;};

struct s_hashiter
   {HASHTAB *htab;
    char *match_type;
    int bucket;
    int didDelete;
    hashel *prev;
    hashel *curr;};

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern hashiter
 *SC_hashiter_alloc(HASHTAB *htab); 

extern void
 SC_hashiter_free(hashiter *self),
 SC_hashiter_kind(hashiter *self, char *type),
 SC_hashiter_reset(hashiter *self),
 *SC_hashiter_value(hashiter *self);

extern int
 SC_hashiter_inc(hashiter *self),
 SC_hashiter_del(hashiter *self),
 SC_hashiter_apply(hashiter *self, 
		   int (*fnc)(hashiter *i, void *arg), void *arg),
 SC_hash_foreach(HASHTAB *tab, int (*fnc)(hashiter *i, void *arg),
		 void *arg);

extern char
 *SC_hashiter_key(hashiter *self),
 *SC_hashiter_key_type(hashiter *self),
 *SC_hashiter_value_type(hashiter *self);

extern int
 SC_hash_key_type(HASHTAB *self, char *name, PFKeyHash hash, PFIntBin comp),
 SC_hash_key_addr(HASHTAB *self),
 SC_hash_key_name(HASHTAB *self),
 SC_hash_insert(HASHTAB *self, char *type, void *key, void *obj),
 SC_hash_ins(HASHTAB *self, hashiter *iter),
 SC_hash_remove(HASHTAB * self, void *key),
 SC_hash_rem(void *key, HASHTAB *self),
 SC_hash_size(HASHTAB *self),
 SC_hash_bucket_count(HASHTAB *self);

extern HASHTAB
 *SC_hash_alloc(void),
 *SC_make_addr_table(int sz, int docflag),
 *SC_make_hash_table(int sz, int docflag);

extern void
 SC_hash_free(HASHTAB *self),
 SC_rl_hash_table(HASHTAB *self),
 *SC_hash_lookup(HASHTAB* self, void *key),
 *SC_def_lookup(void *key, HASHTAB *self),
 SC_hash_clr(HASHTAB *self),
 SC_hash_clean(HASHTAB *self, int (*fnc)(hashiter *iter)),
 SC_hash_resize(HASHTAB *self, int buckets),
 **SC_hash_to_list(HASHTAB *table);

extern char
 *SC_hash_type(HASHTAB *self),
 **SC_hash_keys(HASHTAB *self),
 **SC_hash_specific_keys(HASHTAB *self, char *patt, int sort),
 **SC_dump_hash(HASHTAB *self, char *patt, int sort),
 **SC_hash_dump(HASHTAB *self, char *patt);

extern hashel
 *SC_install(void *key, void *obj, char *type, HASHTAB *self),
 *SC_lookup(void *key, HASHTAB *self);

extern int 
 SC_hash(char *s, int size);

extern void
 SC_hash_key_clear(HASHTAB *self);

extern void
 SC_hash_clr_table(HASHTAB *self, PFIntUn rel),
 SC_hash_cln_table(HASHTAB *self, PFIntUn rel);

extern char
 **_SC_dump_hash(HASHTAB *self, char *patt, char *type, int sort);

extern hashel
 *_SC_install(void *key, void *obj, char *type, HASHTAB *self, int mark, int lookup);

#endif

#ifdef __cplusplus
}
#endif

#endif

