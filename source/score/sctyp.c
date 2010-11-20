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

char
 *SC_print_formats[N_PRIMITIVES],
 *SC_print_formata[N_PRIMITIVES];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TYPE_ID - return type ID */

static SC_type *_SC_get_type_id(int id)
   {haelem *hp;
    SC_type *t;

    SC_init_base_types();

    t = NULL;

    hp = *(haelem **) SC_hasharr_get(_SC.typ, id);
    if (hp != NULL)
       t = (SC_type *) hp->def;

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TYPE_NAME - given the type NAME */

static SC_type *_SC_get_type_name(char *name)
   {SC_type *t;

    SC_init_base_types();

    t = (SC_type *) SC_hasharr_def_lookup(_SC.typ, name);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_TYPE - register a data type with the type manager */

int SC_register_type(char *name, int bpi, ...)
   {int n;
    SC_type *t;

    if (_SC.typ == NULL)
       _SC.typ = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);

    SC_VA_START(bpi);

    n = SC_hasharr_get_n(_SC.typ);

    t = FMAKE(SC_type, "PERM|SC_REGISTER_TYPE:t");
    t->id   = n;
    t->type = SC_strsavef(name, "PERM|char*:SC_REGISTER_TYPE:type");
    t->free = SC_VA_ARG(PFVoidAPV);
    t->bpi  = bpi;

    SC_VA_END;

    SC_hasharr_install(_SC.typ, name, t, "SC_TYPE", TRUE, TRUE);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_ALIAS - register an alias for a known data type */

int SC_type_alias(char *name, int id)
   {SC_type *t, *ot;

    if (_SC.typ == NULL)
       _SC.typ = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);

    ot = _SC_get_type_id(id);

    t  = FMAKE(SC_type, "PERM|SC_REGISTER_TYPE:t");
    *t = *ot;

    t->type = SC_strsavef(name, "PERM|char*:SC_REGISTER_TYPE:type");

    SC_hasharr_install(_SC.typ, name, t, "SC_TYPE", TRUE, TRUE);

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
 *               -     did  - type of destination data
 *               -     d    - pointer to converted data destination
 *               -     sid  - type of source data
 *               -     s    - pointer to data to be converted
 *               -     n    - number of data items
 *               -     flag - free source data flag
 */

int SC_convert_id(int did, void **pd, int sid, void *s, int n, int flag)
   {int rv, bpi;
    long nc;
    void *d;

/* allocate the space if need be */
    d = *pd;
    if (d == NULL)
       {bpi = SC_type_size_i(did);
	d   = FMAKE_N(char, n*bpi, "SC_CONVERT_ID:d");
	*pd = (void *) d;};

    if (_SC_convf[did][sid] != NULL)
       nc = _SC_convf[did][sid](d, s, n);
    
    rv = (nc == n);

    if (flag && (rv == TRUE))
       SFREE(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONVERT - convert data from one binary format to another
 *            - if destination pointer is NULL, space is allocated
 *            - if types are the same do nothing but return -1
 *            - arguments:
 *            -     dtype - type of destination data
 *            -     d     - pointer to converted data destination
 *            -     stype - type of source data
 *            -     s     - pointer to data to be converted
 *            -     n     - number of data items
 *            -     flag  - free source data flag
 */

int SC_convert(char *dtype, void **pd, char *stype, void *s, int n, int flag)
   {int rv, sid, did;

    sid = SC_type_id(stype, FALSE);
    did = SC_type_id(dtype, FALSE);

    rv = SC_convert_id(did, pd, sid, s, n, flag);

    return(rv);}

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
