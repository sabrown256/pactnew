/*
 * SCTYP.C - type management functions for PACT
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "score_int.h"

/*--------------------------------------------------------------------------*/

#define SC_TRANS_SPACE(_d, _dtyp, _pd, _n)                                   \
    _dtyp *_d;                                                               \
    _d = (_dtyp *) *_pd;                                                     \
    if (_d == NULL)                                                          \
       {_d   = FMAKE_N(_dtyp, _n, "SC_TRANS_SPACE:_d");                      \
        *_pd = (void *) _d;};                                                \

/*--------------------------------------------------------------------------*/

#define SC_TRANS_DATA(_dtyp, _pd, _styp, _s, _n)                             \
    {SC_TRANS_SPACE(_d, _dtyp, _pd, _n);                                     \
     if (_d != NULL)                                                         \
        {if (strcmp(_styp, SC_DOUBLE_S) == 0)                                \
            {SC_TRANS_TYPE(_d, double, _s, _n);}                             \
         else if (strcmp(_styp, SC_FLOAT_S) == 0)                            \
            {SC_TRANS_TYPE(_d, float, _s, _n);}                              \
	 else if ((strcmp(_styp, SC_INT_S) == 0) ||                          \
		  (strcmp(_styp, SC_INTEGER_S) == 0))                        \
            {SC_TRANS_TYPE(_d, int, _s, _n);}                                \
         else if (strcmp(_styp, SC_LONG_LONG_S) == 0)                        \
            {SC_TRANS_TYPE(_d, long long, _s, _n);}                          \
         else if (strcmp(_styp, SC_LONG_S) == 0)                             \
            {SC_TRANS_TYPE(_d, long, _s, _n);}                               \
         else if (strcmp(_styp, SC_SHORT_S) == 0)                            \
            {SC_TRANS_TYPE(_d, short, _s, _n);}                              \
         else if (strcmp(_styp, SC_CHAR_S) == 0)                             \
            {SC_TRANS_TYPE(_d, char, _s, _n);};                              \
     ret = TRUE;};}

/*--------------------------------------------------------------------------*/

#define SC_TRANS_TYPE(_d, _styp, _s, _n)                                     \
    {_styp *_ps;                                                             \
     _ps = (_styp *) _s;                                                     \
     for (i = 0; i < _n; i++, *_d++ = *_ps++);}

/*--------------------------------------------------------------------------*/

#define SC_TRANS_TYPE_L(_d, _styp, _s, _n, _mn, _mx)                         \
    {_styp *_ps;                                                             \
     _styp _sm, _smn, _smx;                                                  \
     _smn = (_styp) _mn;                                                     \
     _smx = (_styp) _mx;                                                     \
     _ps  = (_styp *) _s;                                                    \
     for (i = 0; i < _n; i++)                                                \
         {_sm = *_ps++;                                                      \
          _sm = min(_sm, _smx);                                              \
          _sm = max(_sm, _smn);                                              \
          *_d++ = _sm;};}

/*--------------------------------------------------------------------------*/

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
 SC_STRING_I                = 14,
 SC_POINTER_I               = 15,
 SC_BOOL_P_I                = 16,
 SC_SHORT_P_I               = 17,
 SC_INT_P_I                 = 18,
 SC_LONG_P_I                = 19,
 SC_LONG_LONG_P_I           = 20,
 SC_FLOAT_P_I               = 21,
 SC_DOUBLE_P_I              = 22,
 SC_LONG_DOUBLE_P_I         = 23,
 SC_FLOAT_COMPLEX_P_I       = 24,
 SC_DOUBLE_COMPLEX_P_I      = 25,
 SC_LONG_DOUBLE_COMPLEX_P_I = 26,
 SC_VOID_I                  = 27,
 SC_CHAR_8_I                = 28,
 SC_ENUM_I                  = 29,
 SC_STRUCT_I                = 30,
 SC_PCONS_I                 = 31,
 SC_PROCESS_I               = 32,
 SC_FILE_I                  = 33,
 SC_PCONS_P_I               = 34,

 SC_INTEGER_I               = 5,
 SC_REAL_I                  = 9,
 SC_REAL_P_I                = 22;

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
 *SC_STRING_S                = "char *",
 *SC_POINTER_S               = "void *",
 *SC_BOOL_P_S                = "bool *",
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
 *SC_VOID_S                  = "void",
 *SC_CHAR_8_S                = "char_8",
 *SC_ENUM_S                  = "enum",
 *SC_STRUCT_S                = "struct",
 *SC_PCONS_S                 = "pcons",
 *SC_PROCESS_S               = "PROCESS",
 *SC_FILE_S                  = "FILE",
 *SC_PCONS_P_S               = "pcons *",

 *SC_INTEGER_S               = "integer",
 *SC_REAL_S                  = "double",
 *SC_REAL_P_S                = "double *";

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

int SC_type_id(char *name)
   {int n;
    SC_type *t;

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

    if ((n < SC_SHORT_I) || (SC_LONG_LONG_I < n))
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

    if ((n < SC_FLOAT_I) || (SC_LONG_DOUBLE_I < n))
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

    if ((n < SC_FLOAT_COMPLEX_I) || (SC_LONG_DOUBLE_COMPLEX_I < n))
       n = -1;

    return(n);}

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

/*       SC_STRING_I                = SC_register_type("string",                   szptr, NULL); */
       SC_STRING_I                = SC_register_type(SC_STRING_S,                szptr, NULL);
       SC_POINTER_I               = SC_register_type(SC_POINTER_S,               szptr, NULL);
       SC_BOOL_P_I                = SC_register_type(SC_BOOL_P_S,                szptr, NULL);
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

       SC_VOID_I                  = SC_register_type(SC_VOID_S,     0,               NULL);
       SC_CHAR_8_I                = SC_register_type(SC_CHAR_8_S,   sizeof(char),                 NULL);
       SC_ENUM_I                  = SC_register_type(SC_ENUM_S,     0,               NULL);
       SC_STRUCT_I                = SC_register_type(SC_STRUCT_S,   0,               NULL);
       SC_PCONS_I                 = SC_register_type(SC_PCONS_S,    sizeof(pcons),   NULL);
       SC_PROCESS_I               = SC_register_type(SC_PROCESS_S,  sizeof(PROCESS), NULL);
       SC_FILE_I                  = SC_register_type(SC_FILE_S,     sizeof(FILE),    NULL);

       SC_PCONS_P_I               = SC_register_type(SC_PCONS_P_S,  szptr, NULL);

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

int SC_convert(char *dtype, void **pd, char *stype, void *s,
	       int n, int flag)
   {int i, ret;

    if (_SC.bmx == 0)
       SC_fix_lmt(sizeof(long long), &_SC.bmn, &_SC.bmx, NULL);

    ret = FALSE;

#ifdef SC_FAST_TRUNC

/* fast but with simple truncation - FPE's are possible */

/* floating point types */
    if (strcmp(dtype, SC_FLOAT_S) == 0)
       {SC_TRANS_DATA(float, pd, stype, s, n);}

    else if (strcmp(dtype, SC_DOUBLE_S) == 0)
       {SC_TRANS_DATA(double, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_DOUBLE_S) == 0)
       {SC_TRANS_DATA(long double, pd, stype, s, n);}


/* fixed point types */
    else if (strcmp(dtype, SC_SHORT_S) == 0)
       {SC_TRANS_DATA(short, pd, stype, s, n);}

    else if ((strcmp(dtype, SC_INT_S) == 0) ||
	     (strcmp(dtype, SC_INTEGER_S) == 0))
       {SC_TRANS_DATA(int, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_S) == 0)
       {SC_TRANS_DATA(long, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_LONG_S) == 0)
       {SC_TRANS_DATA(long long, pd, stype, s, n);}

    else if (strcmp(dtype, SC_CHAR_S) == 0)
       {SC_TRANS_DATA(char, pd, stype, s, n);};

#else

/* slower with clipping - FPE's are NOT possible */

    if (strcmp(dtype, SC_FLOAT_S) == 0)
       {SC_TRANS_SPACE(_d, float, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE(_d, float, s, n);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, FLT_MIN, FLT_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, FLT_MIN, FLT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long long, s, n);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
            ret = TRUE;};}

    else if (strcmp(dtype, SC_DOUBLE_S) == 0)
       {SC_TRANS_DATA(double, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_DOUBLE_S) == 0)
       {SC_TRANS_SPACE(_d, long double, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE(_d, float, s, n);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, FLT_MIN, FLT_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, FLT_MIN, FLT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long long, s, n);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
            ret = TRUE;};}

    else if (strcmp(dtype, SC_SHORT_S) == 0)
       {SC_TRANS_SPACE(_d, short, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, SHRT_MIN, SHRT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE_L(_d, int, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, SHRT_MIN, SHRT_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if ((strcmp(dtype, SC_INT_S) == 0) ||
	     (strcmp(dtype, SC_INTEGER_S) == 0))
       {SC_TRANS_SPACE(_d, int, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, INT_MIN, INT_MAX);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, INT_MIN, INT_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, INT_MIN, INT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long, s, n, INT_MIN, INT_MAX);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, INT_MIN, INT_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if (strcmp(dtype, SC_LONG_S) == 0)
       {SC_TRANS_SPACE(_d, long, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, LONG_MIN, LONG_MAX);}
            else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, LONG_MIN, LONG_MAX);}
            else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, LONG_MIN, LONG_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, LONG_MIN, LONG_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if (strcmp(dtype, SC_LONG_LONG_S) == 0)
       {SC_TRANS_SPACE(_d, long long, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE(_d, float, s, n);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, _SC.bmn, _SC.bmx);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, _SC.bmn, _SC.bmx);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long long, s, n);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if (strcmp(dtype, SC_CHAR_S) == 0)
       {SC_TRANS_SPACE(_d, char, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, CHAR_MIN, CHAR_MAX);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, CHAR_MIN, CHAR_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, CHAR_MIN, CHAR_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE_L(_d, short, s, n, CHAR_MIN, CHAR_MAX);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE_L(_d, int, s, n, CHAR_MIN, CHAR_MAX);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long, s, n, CHAR_MIN, CHAR_MAX);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, CHAR_MIN, CHAR_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};

        ret = TRUE;};}

#endif

    if (flag && (ret == TRUE))
       SFREE(s);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
