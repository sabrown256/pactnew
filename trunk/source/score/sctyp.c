/*
 * SCTYP.C - type management functions for PACT
 *         - situations that would be nice to manage:
 *         -   type to type conversion (SC_convert and SC_convert_id)
 *         -   printing (see pdprnt.c)
 *         -   variable arg (see gsattrs.c)
 *         -   type to object and object to type (see shmm.c)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "score_int.h"
#include <sctypeg.h>

/* these must have the same sequence/values as the dynamic values
 * assigned in SC_init_base_types
 * changes here MUST be reflected there
 */

int
 SC_UNKNOWN_I               = 0,

 SC_BIT_I                   = 1,
 SC_BOOL_I                  = 2,
 SC_CHAR_I                  = 3,
 SC_SHORT_I                 = 4,
 SC_INT_I                   = 5,
 SC_LONG_I                  = 6,
 SC_LONG_LONG_I             = 7,
 SC_FLOAT_I                 = 8,
 SC_DOUBLE_I                = 9,
 SC_LONG_DOUBLE_I           = 10,
 SC_FLOAT_COMPLEX_I         = 11,
 SC_DOUBLE_COMPLEX_I        = 12,
 SC_LONG_DOUBLE_COMPLEX_I   = 13,
 SC_QUATERNION_I            = 14,

/* these must shadow SC_BIT_I thru SC_QUATERNION_I
 * so that SC_xxx_P_I = SC_xxx_I - SC_BIT_I + SC_POINTER_I
 */
 SC_POINTER_I               = 15,
 SC_BOOL_P_I                = 16,
 SC_STRING_I                = 17,
 SC_SHORT_P_I               = 18,
 SC_INT_P_I                 = 19,
 SC_LONG_P_I                = 20,
 SC_LONG_LONG_P_I           = 21,
 SC_FLOAT_P_I               = 22,
 SC_DOUBLE_P_I              = 23,
 SC_LONG_DOUBLE_P_I         = 24,
 SC_FLOAT_COMPLEX_P_I       = 25,
 SC_DOUBLE_COMPLEX_P_I      = 26,
 SC_LONG_DOUBLE_COMPLEX_P_I = 27,
 SC_QUATERNION_P_I          = 28,

 SC_VOID_I                  = 29,
 SC_CHAR_8_I                = 30,
 SC_STRUCT_I                = 31,
 SC_PCONS_I                 = 32,
 SC_PROCESS_I               = 33,
 SC_FILE_I                  = 34,
 SC_PCONS_P_I               = 35,

/* aliases */
 SC_ENUM_I                  = 5,
 SC_INTEGER_I               = 5,
 SC_REAL_I                  = 9,
 SC_REAL_P_I                = 23;

char
 *SC_UNKNOWN_S               = "unknown",
 *SC_BIT_S                   = "bit",
 *SC_BOOL_S                  = "bool",
 *SC_CHAR_S                  = "char",
 *SC_SHORT_S                 = "short",
 *SC_INT_S                   = "int",
 *SC_LONG_S                  = "long",
 *SC_LONG_LONG_S             = "long_long",
 *SC_FLOAT_S                 = "float",
 *SC_DOUBLE_S                = "double",
 *SC_LONG_DOUBLE_S           = "long_double",
 *SC_FLOAT_COMPLEX_S         = "float_complex",
 *SC_DOUBLE_COMPLEX_S        = "double_complex",
 *SC_LONG_DOUBLE_COMPLEX_S   = "long_double_complex",
 *SC_QUATERNION_S            = "quaternion",

/* these must shadow SC_BIT_S thru SC_QUATERNION_S */
 *SC_POINTER_S               = "void *",
 *SC_BOOL_P_S                = "bool *",
 *SC_STRING_S                = "char *",
 *SC_SHORT_P_S               = "short *",
 *SC_INT_P_S                 = "int *",
 *SC_LONG_P_S                = "long *",
 *SC_LONG_LONG_P_S           = "long_long *",
 *SC_FLOAT_P_S               = "float *",
 *SC_DOUBLE_P_S              = "double *",
 *SC_LONG_DOUBLE_P_S         = "long_double *",
 *SC_FLOAT_COMPLEX_P_S       = "float_complex *",
 *SC_DOUBLE_COMPLEX_P_S      = "double_complex *",
 *SC_LONG_DOUBLE_COMPLEX_P_S = "long_double_complex *",
 *SC_QUATERNION_P_S          = "quaternion *",

 *SC_VOID_S                  = "void",
 *SC_CHAR_8_S                = "char_8",
 *SC_STRUCT_S                = "struct",
 *SC_PCONS_S                 = "pcons",
 *SC_PROCESS_S               = "PROCESS",
 *SC_FILE_S                  = "FILE",
 *SC_PCONS_P_S               = "pcons *",

/* aliases */
 *SC_ENUM_S                  = "enum",
 *SC_INTEGER_S               = "integer",
 *SC_REAL_S                  = "double",
 *SC_REAL_P_S                = "double *";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TYPE_ID - return type ID */

static SC_type *_SC_get_type_id(int id)
   {haelem *hp;
    hasharr *ha;
    SC_type *t;

    SC_init_base_types();

    t  = NULL;
    ha = (hasharr *) _SC.types.typ;

    hp = *(haelem **) SC_hasharr_get(ha, id);
    if (hp != NULL)
       t = (SC_type *) hp->def;

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TYPE_NAME - given the type NAME */

static SC_type *_SC_get_type_name(char *name)
   {hasharr *ha;
    SC_type *t;

    SC_init_base_types();

    ha = (hasharr *) _SC.types.typ;
    t  = (SC_type *) SC_hasharr_def_lookup(ha, name);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_TYPE - register a data type with the type manager */

int SC_register_type(char *name, int bpi, ...)
   {int n;
    hasharr *ha;
    SC_type *t;

    ha = (hasharr *) _SC.types.typ;
    if (ha == NULL)
       {ha = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);
	_SC.types.typ = ha;};

    SC_VA_START(bpi);

    n = SC_hasharr_get_n(ha);

    t = FMAKE(SC_type, "PERM|SC_REGISTER_TYPE:t");
    t->id   = n;
    t->type = SC_strsavef(name, "PERM|char*:SC_REGISTER_TYPE:type");
    t->free = SC_VA_ARG(PFVoidAPV);
    t->bpi  = bpi;

    SC_VA_END;

    SC_hasharr_install(ha, name, t, "SC_TYPE", TRUE, TRUE);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_ALIAS - register an alias for a known data type */

int SC_type_alias(char *name, int id)
   {hasharr *ha;
    SC_type *t, *ot;

    ha = (hasharr *) _SC.types.typ;

    if (ha == NULL)
       {ha = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);
	_SC.types.typ = ha;};

    ot = _SC_get_type_id(id);

    t  = FMAKE(SC_type, "PERM|SC_REGISTER_TYPE:t");
    *t = *ot;

    t->type = SC_strsavef(name, "PERM|char*:SC_REGISTER_TYPE:type");

    SC_hasharr_install(ha, name, t, "SC_TYPE", TRUE, TRUE);

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

/* SC_IS_TYPE_FIX - return TRUE if ID is a fixed point type */

int SC_is_type_fix(int id)
   {int rv;

    rv = ((SC_SHORT_I <= id) && (id <= SC_LONG_LONG_I));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_FP - return TRUE if ID is a floating point type */

int SC_is_type_fp(int id)
   {int rv;

    rv = ((SC_FLOAT_I <= id) && (id <= SC_LONG_DOUBLE_I));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_CX - return TRUE if ID is a complex floating point type */

int SC_is_type_cx(int id)
   {int rv;

    rv = ((SC_FLOAT_COMPLEX_I <= id) && (id <= SC_LONG_DOUBLE_COMPLEX_I));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_NUM - return TRUE if ID is a numeric type */

int SC_is_type_num(int id)
   {int rv;

    rv = ((SC_BIT_I < id) && (id < SC_POINTER_I));

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

/* SC_INIT_BASE_TYPES - register the common C types
 *                    - and some types used often in PACT
 *                    - NOTE: this must be kept consistent with
 *                    - both scope_type.h and pdprnt.c
 */

void SC_init_base_types(void)
   {int szptr;

    ONCE_SAFE(FALSE, NULL)
       szptr = sizeof(char *);

/* these must have the same sequence/values as the static values
 * in the definition at the top of the file
 * changes here MUST be reflected there
 */

       SC_UNKNOWN_I               = SC_register_type(SC_UNKNOWN_S,             0,                            NULL);

       SC_BIT_I                   = SC_register_type(SC_BIT_S,                 0,                 NULL);
       SC_BOOL_I                  = SC_register_type(SC_BOOL_S,                sizeof(bool),                 NULL);
       SC_CHAR_I                  = SC_register_type(SC_CHAR_S,                sizeof(char),                 NULL);
       SC_SHORT_I                 = SC_register_type(SC_SHORT_S,               sizeof(short),                NULL);
       SC_INT_I                   = SC_register_type(SC_INT_S,                 sizeof(int),                  NULL);
       SC_LONG_I                  = SC_register_type(SC_LONG_S,                sizeof(long),                 NULL);
       SC_LONG_LONG_I             = SC_register_type(SC_LONG_LONG_S,           sizeof(long long),            NULL);
       SC_FLOAT_I                 = SC_register_type(SC_FLOAT_S,               sizeof(float),                NULL);
       SC_DOUBLE_I                = SC_register_type(SC_DOUBLE_S,              sizeof(double),               NULL);
       SC_LONG_DOUBLE_I           = SC_register_type(SC_LONG_DOUBLE_S,         sizeof(long double),          NULL);
       SC_FLOAT_COMPLEX_I         = SC_register_type(SC_FLOAT_COMPLEX_S,       sizeof(float _Complex),       NULL);
       SC_DOUBLE_COMPLEX_I        = SC_register_type(SC_DOUBLE_COMPLEX_S,      sizeof(double _Complex),      NULL);
       SC_LONG_DOUBLE_COMPLEX_I   = SC_register_type(SC_LONG_DOUBLE_COMPLEX_S, sizeof(long double _Complex), NULL);
       SC_QUATERNION_I            = SC_register_type(SC_QUATERNION_S,          4*sizeof(double),             NULL);

/* these must shadow SC_BIT_I thru SC_QUATERNION_I
 * so that SC_xxx_P_I = SC_xxx_I - SC_BIT_I + SC_POINTER_I
 */
       SC_POINTER_I               = SC_register_type(SC_POINTER_S,               szptr, NULL);
       SC_BOOL_P_I                = SC_register_type(SC_BOOL_P_S,                szptr, NULL);
       SC_STRING_I                = SC_register_type(SC_STRING_S,                szptr, NULL);
       SC_SHORT_P_I               = SC_register_type(SC_SHORT_P_S,               szptr, NULL);
       SC_INT_P_I                 = SC_register_type(SC_INT_P_S,                 szptr, NULL);
       SC_LONG_P_I                = SC_register_type(SC_LONG_P_S,                szptr, NULL);
       SC_LONG_LONG_P_I           = SC_register_type(SC_LONG_LONG_P_S,           szptr, NULL);
       SC_FLOAT_P_I               = SC_register_type(SC_FLOAT_P_S,               szptr, NULL);
       SC_DOUBLE_P_I              = SC_register_type(SC_DOUBLE_P_S,              szptr, NULL);
       SC_LONG_DOUBLE_P_I         = SC_register_type(SC_LONG_DOUBLE_P_S,         szptr, NULL);
       SC_FLOAT_COMPLEX_P_I       = SC_register_type(SC_FLOAT_COMPLEX_P_S,       szptr, NULL);
       SC_DOUBLE_COMPLEX_P_I      = SC_register_type(SC_DOUBLE_COMPLEX_P_S,      szptr, NULL);
       SC_LONG_DOUBLE_COMPLEX_P_I = SC_register_type(SC_LONG_DOUBLE_COMPLEX_P_S, szptr, NULL);
       SC_QUATERNION_P_I          = SC_register_type(SC_QUATERNION_P_S,          szptr, NULL);

       SC_VOID_I                  = SC_register_type(SC_VOID_S,     0,               NULL);
       SC_CHAR_8_I                = SC_register_type(SC_CHAR_8_S,   sizeof(char),                 NULL);
       SC_STRUCT_I                = SC_register_type(SC_STRUCT_S,   0,               NULL);
       SC_PCONS_I                 = SC_register_type(SC_PCONS_S,    sizeof(pcons),   NULL);
       SC_PROCESS_I               = SC_register_type(SC_PROCESS_S,  sizeof(PROCESS), NULL);
       SC_FILE_I                  = SC_register_type(SC_FILE_S,     sizeof(FILE),    NULL);

       SC_PCONS_P_I               = SC_register_type(SC_PCONS_P_S,  szptr, NULL);

/* aliases */
    SC_ENUM_I    = SC_type_alias(SC_ENUM_S, SC_INT_I);
    SC_INTEGER_I = SC_type_alias(SC_INTEGER_S, SC_INT_I);
    SC_REAL_I    = SC_type_alias(SC_DOUBLE_S, SC_DOUBLE_I);
    SC_REAL_P_I  = SC_type_alias(SC_DOUBLE_P_S, SC_DOUBLE_P_I);

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

/* SC_CONVERT_ID - convert data from one binary format to another
 *               - if destination pointer is NULL, space is allocated
 *               - if types are the same do nothing but return -1
 *               - arguments:
 *               -     DID  - type of destination data
 *               -     D    - pointer to converted data destination
 *               -     OD   - offset from destination pointer
 *               -     SID  - type of source data
 *               -     S    - pointer to data to be converted
 *               -     OS   - offset from source pointer
 *               -     SS    - stride through source
 *               -     N    - number of data items
 *               -     FLAG - free source data flag
 *               - the offsets permit conversions like
 *               -     d[n] = s[m]
 *               - return the destination pointer
 */

void *SC_convert_id(int did, void *d, long od,
		    int sid, void *s, long os, long ss,
		    long n, int flag)
   {int rv, bpi;
    long nc;

/* allocate the space if need be */
    if (d == NULL)
       {bpi = SC_type_size_i(did);
	d   = FMAKE_N(char, n*bpi, "SC_CONVERT_ID:d");};

    if (_SC_convf[did][sid] != NULL)
       nc = _SC_convf[did][sid](d, od, s, os, ss, n);
    
    rv = (nc == n);

    if (flag && (rv == TRUE))
       SFREE(s);

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
 *            -     STYPE - type of source data
 *            -     S     - pointer to data to be converted
 *            -     OS    - offset from source pointer
 *            -     SS    - stride through source
 *            -     N     - number of data items
 *            -     FLAG  - free source data flag
 *            - return the destination pointer
 */

void *SC_convert(char *dtype, void *d, long od, char *stype,
		 void *s, long os, long ss,
		 long n, int flag)
   {int sid, did;

    sid = SC_type_id(stype, FALSE);
    did = SC_type_id(dtype, FALSE);

    d = SC_convert_id(did, d, od, sid, s, os, ss, n, flag);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
    
/* SC_VA_GET_ARG - set the Nth element of D which is of type ID
 *               - from the variable arg list A
 */

int SC_va_get_arg(va_list a, int id, void *d, long n)
    {int ok;
    
     ok = FALSE;

     if (_SC_argf[id] != NULL)
        ok = _SC_argf[id](a, d, n);
    
     return(ok);}

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

     if (_SC_strf[id] != NULL)
        rv = _SC_strf[id](t, nc, s, n, mode);
    
     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_USER_DEFAULTS - default user specified formats to NULL */

void _SC_set_user_defaults(void)
   {int i;
    char **ufmts, **ufmta;

    ufmts = _SC.types.user_formats;
    ufmta = _SC.types.user_formata;

    for (i = 0; i < N_PRIMITIVES; i++)
        {if (ufmts[i] != NULL)
	    {SFREE(ufmts[i]);};

	 if (ufmta[i] != NULL)
	    {SFREE(ufmta[i]);};};

    if (_SC.types.suppress_member != NULL)
       {SFREE(_SC.types.suppress_member);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_USER_FORMATS - replace edit formats with user specified formats  */

void _SC_set_user_formats(void)
   {int i;
    char **fmts, **fmta;
    char **ufmts, **ufmta;

    fmts  = _SC.types.formats;
    fmta  = _SC.types.formata;
    ufmts = _SC.types.user_formats;
    ufmta = _SC.types.user_formata;

    for (i = 0; i < N_PRIMITIVES; i++)
        {if (ufmts[i] != NULL)
	    {SFREE(fmts[i]);
	     fmts[i] = SC_strsavef(ufmts[i],
				   "char*:_SC_SET_USER_FORMATS:fmts(i)");};

	 if (ufmta[i] != NULL)
	    {SFREE(fmta[i]);
	     fmta[i] = SC_strsavef(ufmta[i],
				   "char*:_SC_SET_USER_FORMATS:fmta(i)");};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_FORMAT_DEFAULTS - set the defaults for the edit formats 
 *                         -   1 = bit
 *                         -   2 = bool
 *                         -   3 = char
 *                         -   4 = short
 *                         -   5 = int
 *                         -   6 = long
 *                         -   7 = long long
 *                         -   8 = float
 *                         -   9 = double
 *                         -  10 = long double
 *                         -  11 = float complex
 *                         -  12 = double complex
 *                         -  13 = long double complex
 */

void _SC_set_format_defaults(void)
   {int i;
    int *fix_pre;
    precisionfp *fp_pre;
    char tmp[MAXLINE], *t;
    char **fmts, **fmta;
    
    fix_pre = _SC.types.fix_precision;
    fp_pre  = _SC.types.fp_precision;
    fmts    = _SC.types.formats;
    fmta    = _SC.types.formata;

/* fmts is used for scalars */

/* fixed point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {if (fmts[i+SC_SHORT_I] != NULL)
	    SFREE(fmts[i+SC_SHORT_I]);

	 snprintf(tmp, MAXLINE, "%%%dd", fix_pre[i]);

	 t = SC_strsavef(tmp, "char*:_SC_SET_FORMAT_DEFAULTS:format1(fix)");
	 fmts[i+SC_SHORT_I] = t;};

/* real floating point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {if (fmts[i+SC_FLOAT_I] != NULL)
	    SFREE(fmts[i+SC_FLOAT_I]);

	 snprintf(tmp, MAXLINE, "%%# .%de", fp_pre[i].digits);

	 t = SC_strsavef(tmp, "char*:_SC_SET_FORMAT_DEFAULTS:format1(fp)");
	 fmts[i+SC_FLOAT_I] = t;};

/* complex floating point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {if (fmts[i+SC_FLOAT_COMPLEX_I] != NULL)
	    SFREE(fmts[i+SC_FLOAT_COMPLEX_I]);

	 snprintf(tmp, MAXLINE, "%%# .%de + %%# .%de*I",
		  fp_pre[i].digits, fp_pre[i].digits);

	 t = SC_strsavef(tmp, "char*:_SC_SET_FORMAT_DEFAULTS:format1(fp)");
	 fmts[i+SC_FLOAT_COMPLEX_I] = t;};

/* other primitive types */
    if (fmts[SC_CHAR_I] != NULL)
       SFREE(fmts[SC_CHAR_I]);

    t = SC_strsavef("%c", "char*:_SC_SET_FORMAT_DEFAULTS:format1(char)");
    fmts[SC_CHAR_I] = t;

    if (fmts[SC_BIT_I] != NULL)
       SFREE(fmts[SC_BIT_I]);

    t = SC_strsavef("%x", "char*:_SC_SET_FORMAT_DEFAULTS:format1(bit)");
    fmts[SC_BIT_I] = t;

    if (fmts[SC_BOOL_I] != NULL)
       SFREE(fmts[SC_BOOL_I]);

    t = SC_strsavef("%s", "char*:_SC_SET_FORMAT_DEFAULTS:format1(bool)");
    fmts[SC_BOOL_I] = t;

/* fmta is used for arrays */
    for (i = 0; i < N_PRIMITIVES; i++)
        {if (fmta[i] != NULL)
            SFREE(fmta[i]);

         t = SC_strsavef(fmts[i],
			 "char*:_SC_SET_FORMAT_DEFAULTS:formats2");
         fmta[i] = t;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
