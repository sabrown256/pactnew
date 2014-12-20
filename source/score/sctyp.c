/*
 * SCTYP.C - type management functions for PACT
 *         - situations that would be nice to manage:
 *         -   type to type conversion (SC_convert and SC_convert_id)
 *         -   printing (see pdprnt.c)
 *         -   variable arg (see gsattrs.c)
 *         -   type to object and object to type (see shmm.c)
 *
 */

#include "score_int.h"
#include "scope_mem.h"

/* to add a new primitive type you need to check at least the
 * following places:
 *   add an entry in psh/types.db
 *   in pdb/pdconv.c check:
 *      add the type to the initializers for data_standard and data_alignment
 *      add any special conversion coding
 *   elsewhere check:
 *      look for changes to the minimum type of a family
 *         for example:  id - SC_SHORT_I  ->  id - SC_INT8_I
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TYPE_ID - return type ID */

SC_type *_SC_get_type_id(int id)
   {int nt;
    haelem *hp;
    hasharr *ha;
    SC_type *t;

    SC_init_base_types();

    t  = NULL;
    ha = (hasharr *) _SC.types.typ;
    nt = SC_hasharr_get_n(ha);
    if ((0 <= id) && (id < nt))
       {hp = *(haelem **) SC_hasharr_get(ha, id);
	if (hp != NULL)
	   t = (SC_type *) hp->def;};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TYPE_NAME - given the type NAME */

SC_type *_SC_get_type_name(char *name)
   {hasharr *ha;
    SC_type *t;

    SC_init_base_types();

    ha = (hasharr *) _SC.types.typ;
    t  = (SC_type *) SC_hasharr_def_lookup(ha, name);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TYPE_REGISTER - register a data type, NAME, with the type manager */

static int _SC_type_register(char *name, SC_type *t)
   {int id;
    hasharr *ha;
    haelem *hp;
    typdes *td;

    ha = (hasharr *) _SC.types.typ;
    if (ha == NULL)
       {ha = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 3);
	_SC.types.typ = ha;
	SC_init_base_types();};

    id = SC_hasharr_get_n(ha);

    hp = SC_hasharr_install(ha, name, t, "SC_TYPE", 3, -1);
    td = (typdes *) hp->def;
    if (t != td)
       id = td->id;
    else
       t->id = id;

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_REGISTER - register a data type, NAME, with the type manager
 *                  - if PTR is TRUE register a pointer type too, e.g.
 *                  -    "int" also gets "int *"
 */

int SC_type_register(char *name, SC_kind kind, bool ptr, int bpi, ...)
   {int id, ok;
    void *pf;
    SC_type_method attr;
    SC_type *t;

    SC_VA_START(bpi);

    t = _SC_get_type_name(name);
    if (t != NULL)
       id = t->id;
    else
       {t = CPMAKE(SC_type, 3);

	t->id   = -1;
	t->type = CSTRDUP(name, 3);
	t->knd  = TK_STRUCT;
	t->g    = kind;
	t->bpi  = bpi;
	t->a    = NULL;

	t->init = NULL;
	t->free = NULL;

        for (ok = TRUE; ok == TRUE; )
	    {attr = SC_VA_ARG(SC_type_method);
	     if (attr == SC_TYPE_NONE)
	        ok = FALSE;
	     else
	        {pf = SC_VA_ARG(void *);
		 switch (attr)
		    {case SC_TYPE_FREE :
		          t->free = (PFVoidAPV) pf;
			  break;
		     case SC_TYPE_INIT :
			  t->init = (PFVoidAPV) pf;
			  break;
	             case SC_TYPE_NONE :
		     default :
			  ok = FALSE;
			  break;};};};

	id = _SC_type_register(name, t);

#if 0
	if (ptr == TRUE)
	   {int pid;
	    char ps[BFSML];
	    SC_type *p;

	    snprintf(ps, BFSML, "%s *", name);

	    p = CPMAKE(SC_type, 3);

	    p->id   = -1;
	    p->type = CSTRDUP(ps, 3);
	    p->knd  = KIND_POINTER;
	    p->bpi  = sizeof(char *);
	    p->a    = NULL;
	    p->init = NULL;
	    p->free = NULL;

	    pid = _SC_type_register(ps, p);};
#endif
	};

    SC_VA_END;

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_ALIAS - register an alias for a known data type */

int SC_type_alias(char *name, int id)
   {hasharr *ha;
    SC_type *t, *ot;

    ha = (hasharr *) _SC.types.typ;

    if (ha == NULL)
       {ha = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 3);
	_SC.types.typ = ha;};

    ot = _SC_get_type_id(id);
    if (ot != NULL)
       {t = CPMAKE(SC_type, 3);
	if (t != NULL)
	   {*t = *ot;

	    t->type = CSTRDUP(name, 3);

	    SC_hasharr_install(ha, name, t, "SC_TYPE", 3, -1);};};

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_ID - given the type NAME return the type id
 *            - return -1 if type NAME is unknown
 */

int SC_type_id(char *name, int unsg)
   {int n;
    SC_type *t;

    if ((unsg == TRUE) && (strncmp(name, "u_", 2) == 0))
       name += 2;

    t = _SC_get_type_name(name);
    n = (t != NULL) ? t->id : -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_PTR_ID - given the ID of a primitive type
 *                - return the ID of the pointer to the type
 *                - return -1 if type NAME is unknown
 */

int SC_type_ptr_id(int id)
   {int pid;

    pid = id + 1;

    return(pid);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_PTR_NAME - given the ID of a primitive type
 *                  - return the NAME of the pointer to the type
 *                  - return NULL if type NAME is unknown
 */

char *SC_type_ptr_name(int id)
   {int pid;
    char *pnm;

    pid = id + 1;
    pnm = SC_type_name(pid);

    return(pnm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIX_TYPE_ID - given the fixed point type NAME return the type id
 *                - return -1 if type NAME is unknown or not fixed point
 */

int SC_fix_type_id(char *name, int unsg)
   {int n;
    SC_type *t;

    if ((unsg == TRUE) && (strncmp(name, "u_", 2) == 0))
       name += 2;

    t = _SC_get_type_name(name);
    n = (t != NULL) ? t->id : -1;

    if (SC_is_type_fix(n) == FALSE)
       n = -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FP_TYPE_ID - given the floating point type NAME return the type id
 *               - return -1 if type NAME is unknown or not floating point
 */

int SC_fp_type_id(char *name)
   {int n;
    SC_type *t;

    t = _SC_get_type_name(name);
    n = (t != NULL) ? t->id : -1;

    if (SC_is_type_fp(n) == FALSE)
       n = -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CX_TYPE_ID - given the complex type NAME return the type id
 *               - return -1 if type NAME is unknown or not complex
 */

int SC_cx_type_id(char *name)
   {int n;
    SC_type *t;

    t = _SC_get_type_name(name);
    n = (t != NULL) ? t->id : -1;

    if (SC_is_type_cx(n) == FALSE)
       n = -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_CHAR - return TRUE if ID is a character type */

int SC_is_type_char(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->g == KIND_CHAR));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_FIX - return TRUE if ID is a fixed point type */

int SC_is_type_fix(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->g == KIND_INT));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_FP - return TRUE if ID is a floating point type */

int SC_is_type_fp(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->g == KIND_FLOAT));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_CX - return TRUE if ID is a complex floating point type */

int SC_is_type_cx(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->g == KIND_COMPLEX));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_QUT - return TRUE if ID is a quaternion type */

int SC_is_type_qut(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->g == KIND_QUATERNION));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_NUM - return TRUE if ID is a numeric type */

int SC_is_type_num(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) &&
	  ((t->g == KIND_BOOL)    ||
	   (t->g == KIND_CHAR)    ||
	   (t->g == KIND_INT)     ||
	   (t->g == KIND_FLOAT)   ||
	   (t->g == KIND_COMPLEX) ||
	   (t->g == KIND_QUATERNION)));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_PTR - return TRUE if ID is a pointer type */

int SC_is_type_ptr(int id)
   {int rv;
    char *typn;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->g == KIND_POINTER));
    if (rv == FALSE)
       {typn = SC_type_name(id);
	rv   = ((typn != NULL) && (strchr(typn, '*') != NULL));};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_PTR_A - return TRUE if NAME is a pointer type */

int SC_is_type_ptr_a(char *name)
   {int rv;

    rv = (strchr(name, '*') != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_PRIM - return TRUE if ID is a primitive type */

int SC_is_type_prim(int id)
   {int rv;

    rv = ((SC_UNKNOWN_I < id) && (id < N_TYPES));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_STRUCT - return TRUE if ID is a derived type */

int SC_is_type_struct(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->g == KIND_STRUCT));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_NAME - given the type ID return the type name
 *              - the return value should never be freed
 *              - return NULL if type ID is unknown
 */

char *SC_type_name(int id)
   {char *name;
    SC_type *t;

    t    = _SC_get_type_id(id);
    name = (t != NULL) ? t->type : NULL;

    return(name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_INFO - given the type ID return the type info
 *              - the return value should never be freed
 *              - return NULL if type ID is unknown
 */

void *SC_type_info(int id)
   {void *info;
    SC_type *t;

    t    = _SC_get_type_id(id);
    info = (t != NULL) ? t->a : NULL;

    return(info);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_SIZE_I - given the type ID return the type size
 *                - return -1 if type ID is unknown
 */

int SC_type_size_i(int id)
   {int bpi;
    SC_type *t;

    t   = _SC_get_type_id(id);
    bpi = (t != NULL) ? t->bpi : -1;

    return(bpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_SIZE_A - given the type ID return the type size
 *                - return -1 if type ID is unknown
 */

int SC_type_size_a(char *name)
   {int bpi;
    SC_type *t;

    t   = _SC_get_type_name(name);
    bpi = (t != NULL) ? t->bpi : -1;

    return(bpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_FREE_I - free an instance X of type given by ID */

void SC_type_free_i(int id, void *x)
   {SC_type *t;

    t = _SC_get_type_id(id);
    if (t != NULL)
       (*t->free)(x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_FREE_A - free an instance X of type given by NAME */

void SC_type_free_a(char *name, void *x)
   {SC_type *t;

    t = _SC_get_type_name(name);
    if (t != NULL)
       (*t->free)(x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DEREFERENCE - THREADSAFE
 *                - starting at the end of the string work backwards to
 *                - the first non-blank character and if it is a '*'
 *                - insert '\0' in its place
 *                - return a pointer to the beginning of the string
 */

char *SC_dereference(char *s)
   {char *t;

/* starting at the end of the string, working backwards */
    t = s + (strlen(s) - 1);

/* skip over any whitespace */
    while ((t >= s) && (*t == ' '))
       {t--;};

/* remove any terminating '*' char */
    if ((t >= s) && (*t == '*'))
       {*t = '\0';
        t--;};

/* remove any trailing whitespace -- e.g. "char *" -> "char " -> "char" */
    while ((t >= s) && (strchr(" \t", *t) != NULL))
       {*t = '\0';
        t--;};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DEREF_ID - return the index of the base type of TYPE
 *             - if BASE is TRUE do the base type
 *             - if BASE is FALSE dereference one level of indirection only
 */

int SC_deref_id(char *name, int base)
   {int id;
    char bf[MAXLINE];

    SC_strncpy(bf, MAXLINE, name, -1);

    if (base == TRUE)
       SC_trim_right(bf, " *");
    else
       SC_dereference(bf);

    id = SC_type_id(bf, FALSE);

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIN_TYPE_MANAGER - free up the space of the type manager
 *                     - this had better be pretty near the last
 *                     - PACT action taken in an application
 */

void SC_fin_type_manager(void)
   {int i, nt;
    char **fmts, **fmta;
    hasharr *ha;

    ha   = (hasharr *) _SC.types.typ;
    nt   = _SC.types.nstandard;
    fmts = _SC.types.formats;
    fmta = _SC.types.formata;

    for (i = 0; i < nt; i++)
        {CFREE(fmts[i]);
	 CFREE(fmta[i]);};

    SC_free_hasharr(ha, NULL, NULL);

    _SC.types.typ = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_BASE_TYPES - register the common C types
 *                    - and some types used often in PACT
 *                    - NOTE: this must be kept consistent with
 *                    - both scope_type.h and pdprnt.c
 */

void SC_init_base_types(void)
   {

    ONCE_SAFE(FALSE, NULL)
       typdes *tl;

       for (tl = SC_gs.stl; tl->type != NULL; tl++)
	   _SC_type_register(tl->type, tl);
	   
       _SC_set_format_defaults();

    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_COPY_PRIMITIVE - copy N items of type ID from S to D */

size_t SC_copy_primitive(void *d, void *s, long n, int id)
   {size_t rv, bpi;

    bpi = SC_type_size_i(id);
    rv  = bpi*n;
    memcpy(d, s, rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIND_PRIMITIVE - find the primitive type corresponding to ID
 *                   - return NULL if none
 *                   - this looks thru aliases
 */

SC_type *SC_find_primitive(int id)
   {typdes *ta, *rv;

    for (ta = SC_gs.stl + id, rv = NULL; (rv == NULL) && (ta != NULL); )
	{if (ta->alias != NULL)
	    ta = _SC_get_type_name(ta->alias);
	 else if (ta->knd == TK_PRIMITIVE)
	    rv = ta;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_CONTAINER_SIZE - return the type id of a type of KIND which
 *                        - is at least NB bytes long
 */

int SC_type_container_size(SC_kind kind, int nb)
   {int i, id;
    hasharr *ha;
    haelem *hp;
    SC_type *t;

    ha = (hasharr *) _SC.types.typ;

    switch (kind)

/* character types (proper) */
       {case KIND_CHAR :
             for (i = 0; i < N_PRIMITIVE_CHAR; i++)
	         {id = SC_TYPE_CHAR_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CHAR)
	        id = SC_UNKNOWN_I;
             break;

/* fixed point types (proper) */
        case KIND_INT :
             for (i = 0; i < N_PRIMITIVE_FIX; i++)
	         {id = SC_TYPE_FIX_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FIX)
	        id = SC_UNKNOWN_I;
             break;

/* real floating point types (proper) */
        case KIND_FLOAT :
             for (i = 0; i < N_PRIMITIVE_FP; i++)
	         {id = SC_TYPE_FP_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FP)
	        id = SC_UNKNOWN_I;
             break;

/* complex floating point types (proper) */
        case KIND_COMPLEX :
             for (i = 0; i < N_PRIMITIVE_CPX; i++)
	         {id = SC_TYPE_CPX_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CPX)
	        id = SC_UNKNOWN_I;
             break;

/* quaternion point types (proper) */
        case KIND_QUATERNION :
             for (i = 0; i < N_PRIMITIVE_QUT; i++)
		 {id = SC_TYPE_QUT(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_QUT)
	        id = SC_UNKNOWN_I;
             break;

        default :
	     id = SC_UNKNOWN_I;
             break;}

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_MATCH_SIZE - return the type id of a type of KIND which
 *                    - matches byte length NB
 */

int SC_type_match_size(SC_kind kind, int nb)
   {int i, id;
    hasharr *ha;
    haelem *hp;
    SC_type *t;

    ha = (hasharr *) _SC.types.typ;

    switch (kind)

/* character types (proper) */
       {case KIND_CHAR :
             for (i = 0; i < N_PRIMITIVE_CHAR; i++)
	         {id = SC_TYPE_CHAR_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CHAR)
	        id = SC_UNKNOWN_I;
             break;

/* fixed point types (proper) */
        case KIND_INT :
             for (i = 0; i < N_PRIMITIVE_FIX; i++)
	         {id = SC_TYPE_FIX_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FIX)
	        id = SC_UNKNOWN_I;
             break;

/* real floating point types (proper) */
        case KIND_FLOAT :
             for (i = 0; i < N_PRIMITIVE_FP; i++)
	         {id = SC_TYPE_FP_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FP)
	        id = SC_UNKNOWN_I;
             break;

/* complex floating point types (proper) */
        case KIND_COMPLEX :
             for (i = 0; i < N_PRIMITIVE_CPX; i++)
	         {id = SC_TYPE_CPX_ID(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CPX)
	        id = SC_UNKNOWN_I;
             break;

/* quaternion point types (proper) */
        case KIND_QUATERNION :
             for (i = 0; i < N_PRIMITIVE_QUT; i++)
		 {id = SC_TYPE_QUT(i);
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_QUT)
	        id = SC_UNKNOWN_I;
             break;

        default :
	     id = SC_UNKNOWN_I;
             break;}

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONVERT_ID - convert data from one binary format to another
 *               - if destination pointer is NULL, space is allocated
 *               - if types are the same do nothing but return -1
 *               - arguments:
 *               -     DID  - type of destination data
 *               -     D    - pointer to converted data destination
 *               -     OD   - offset from destination pointer
 *               -     DS   - stride through destination
 *               -     SID  - type of source data
 *               -     S    - pointer to data to be converted
 *               -     OS   - offset from source pointer
 *               -     SS   - stride through source
 *               -     N    - number of data items
 *               -     FLAG - free source data flag
 *               - the offsets permit conversions like
 *               -     d[n] = s[m]
 *               - return the destination pointer
 */

#define SC_PRIMITIVE_ID(id) ((id - 3) >> 1)

void *SC_convert_id(int did, void *d, long od, long ds,
		    int sid, void *s, long os, long ss,
		    long n, int flag)
   {int rv, bpi, np;
    int pdid, psid;
    long nc;
    typdes *td, *ts;

/* allocate the space if need be */
    if (d == NULL)
       {bpi = SC_type_size_i(did);
	d   = CMAKE_N(char, n*bpi);};

    td = SC_find_primitive(did);
    ts = SC_find_primitive(sid);

    pdid = (td != NULL) ? td->id : -1;
    psid = (ts != NULL) ? ts->id : -1;

    pdid = SC_PRIMITIVE_ID(pdid);
    psid = SC_PRIMITIVE_ID(psid);

    np = _SC.types.nprimitive;
    if ((0 <= pdid) && (pdid < np) &&
	(0 <= psid) && (psid < np) &&
	(_SC_convf[pdid][psid] != NULL))
       nc = _SC_convf[pdid][psid](d, od, ds, s, os, ss, n);
    else
       nc = -1;

    rv = (nc == n);

    if (flag && (rv == TRUE))
       CFREE(s);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONVERT - convert data from one binary format to another
 *            - if destination pointer is NULL, space is allocated
 *            - if types are the same do nothing but return -1
 *            - arguments:
 *            -     DTYPE - type of destination data
 *            -     D     - pointer to converted data destination
 *            -     OD    - offset from destination pointer
 *            -     SD    - stride through destination
 *            -     STYPE - type of source data
 *            -     S     - pointer to data to be converted
 *            -     OS    - offset from source pointer
 *            -     SS    - stride through source
 *            -     N     - number of data items
 *            -     FLAG  - free source data flag
 *            - return the destination pointer
 */

void *SC_convert(char *dtype, void *d, long od, long sd,
		 char *stype, void *s, long os, long ss,
		 long n, int flag)
   {int sid, did;

    sid = SC_type_id(stype, FALSE);
    did = SC_type_id(dtype, FALSE);

    d = SC_convert_id(did, d, od, sd, sid, s, os, ss, n, flag);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
    
/* SC_NTOS - render the Nth element of S which is of type ID
 *         - as a string in T
 *         - if MODE is 1 render as scalar
 *         - otherwiser render as element of array
 *         - Note: one might want to get many array elements in a line
 *         - and hence print fewer digits
 */

char *SC_ntos(char *t, int nc, int id, void *s, long n, int mode)
   {char *rv;
    
    rv = NULL;

    if (id < _SC.types.nstandard)
       {if (_SC_strf[id] != NULL)
	   rv = _SC_strf[id](t, nc, s, n, mode);};
    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_USER_DEFAULTS - default user specified formats to NULL */

void _SC_set_user_defaults(void)
   {int i, nt;
    char **ufmts, **ufmta;

    nt    = _SC.types.nstandard;
    ufmts = _SC.types.user_formats;
    ufmta = _SC.types.user_formata;

    for (i = 0; i < nt; i++)
        {if (ufmts[i] != NULL)
	    CFREE(ufmts[i]);

	 if (ufmta[i] != NULL)
	    CFREE(ufmta[i]);};

    if (_SC.types.suppress_member != NULL)
       CFREE(_SC.types.suppress_member);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_USER_FORMATS - replace edit formats with user specified formats  */

void _SC_set_user_formats(void)
   {int i, nt;
    char **fmts, **fmta;
    char **ufmts, **ufmta;

    nt    = _SC.types.nstandard;
    fmts  = _SC.types.formats;
    fmta  = _SC.types.formata;
    ufmts = _SC.types.user_formats;
    ufmta = _SC.types.user_formata;

    for (i = 0; i < nt; i++)
        {if (ufmts[i] != NULL)
	    {CFREE(fmts[i]);
	     fmts[i] = CSTRSAVE(ufmts[i]);};

	 if (ufmta[i] != NULL)
	    {CFREE(fmta[i]);
	     fmta[i] = CSTRSAVE(ufmta[i]);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_FORMAT_DEFAULTS - set the defaults for the edit formats */

void _SC_set_format_defaults(void)
   {int i, id, nt;
    int *fix_pre;
    precisionfp *fp_pre;
    char tmp[MAXLINE], *t;
    char **fmts, **fmta;
    
    nt      = _SC.types.nstandard;
    fix_pre = _SC.types.fix_precision;
    fp_pre  = _SC.types.fp_precision;
    fmts    = _SC.types.formats;
    fmta    = _SC.types.formata;

/* fmts is used for scalars */

    if (fmts[SC_BIT_I] != NULL)
       CFREE(fmts[SC_BIT_I]);

    t = CSTRDUP("%x", 3);
    fmts[SC_BIT_I] = t;

    if (fmts[SC_BOOL_I] != NULL)
       CFREE(fmts[SC_BOOL_I]);

    t = CSTRDUP("%s", 3);
    fmts[SC_BOOL_I] = t;

/* character types (proper) */
    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        {id = SC_TYPE_CHAR_ID(i);
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_CHAR_I)
	    snprintf(tmp, MAXLINE, "%%c");
	 else if (id == SC_WCHAR_I)
	    snprintf(tmp, MAXLINE, "%%Lc");

	 t = CSTRDUP(tmp, 3);
	 fmts[id] = t;};

/* fixed point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {id = SC_TYPE_FIX_ID(i);
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_LONG_LONG_I)
	    snprintf(tmp, MAXLINE, "%%%dlld", fix_pre[i]);
	 else if (id == SC_LONG_I)
	    snprintf(tmp, MAXLINE, "%%%dld", fix_pre[i]);
	 else
	    snprintf(tmp, MAXLINE, "%%%dd", fix_pre[i]);

	 t = CSTRDUP(tmp, 3);
	 fmts[id] = t;};

/* real floating point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {id = SC_TYPE_FP_ID(i);
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_LONG_DOUBLE_I)
	    snprintf(tmp, MAXLINE, "%%# .%dle", fp_pre[i].digits);
	 else
	    snprintf(tmp, MAXLINE, "%%# .%de", fp_pre[i].digits);

	 t = CSTRDUP(tmp, 3);
	 fmts[id] = t;};

/* complex floating point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {id = SC_TYPE_CPX_ID(i);
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_LONG_DOUBLE_COMPLEX_I)
	    snprintf(tmp, MAXLINE, "%%# .%dle + %%# .%dle*I",
		     fp_pre[i].digits, fp_pre[i].digits);
	 else
	    snprintf(tmp, MAXLINE, "%%# .%de + %%# .%de*I",
		     fp_pre[i].digits, fp_pre[i].digits);

	 t = CSTRDUP(tmp, 3);
	 fmts[id] = t;};

/* other primitive types */
    if (fmts[SC_STRING_I] != NULL)
       CFREE(fmts[SC_STRING_I]);

    t = CSTRDUP("%s", 3);
    fmts[SC_STRING_I] = t;

/* fmta is used for arrays */
    for (i = 0; i < nt; i++)
        {if (fmta[i] != NULL)
            CFREE(fmta[i]);

         t = CSTRDUP(fmts[i], 3);
         fmta[i] = t;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
