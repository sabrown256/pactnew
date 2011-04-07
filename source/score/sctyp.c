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

/* to add a new primitive type you need to check at least the
 * following places:
 *   in score/scoerce.c check:
 *      variables: names, types, mn, and mx
 *      add any special conversion coding
 *      add any special printing coding
 *   in score/sctyp.c check:
 *      add the type definition and register it
 *          for example:  SC_INT8_I, SC_INT8_P_I, SC_INT8_S, SC_INT8_P_S
 *   in score/scope_typeh.h check:
 *      add the type declaration
 *          for example:  SC_INT8_I, SC_INT8_P_I, SC_INT8_S, SC_INT8_P_S
 *      check the N_PRIMITIVE_ and N_TYPE count macros
 *   in score/varg.c check:
 *      variables: types and promo
 *      add any special va_arg coding
 *   in pdb/detect.c check:
 *      add the type definition
 *      add any coding to compute the types characterization
 *   in pdb/pdconv.c check:
 *      add the type to the initializers for data_standard and data_alignment
 *      add any special conversion coding
 *   in pdb/pdlow.c check:
 *      verify the registration of the PDB representation of the type
 *   elsewhere check:
 *      look for changes to the minimum type of a family
 *         for example:  id - SC_SHORT_I  ->  id - SC_INT8_I
 */

/* these must have the same sequence/values as the dynamic values
 * assigned in SC_init_base_types
 * changes here MUST be reflected there
 */

int
 SC_UNKNOWN_I               = 0,

 SC_BIT_I                   = 1,
 SC_BOOL_I                  = 2,
 SC_CHAR_I                  = 3,
 SC_WCHAR_I                 = 4,
 SC_INT8_I                  = 5,
 SC_SHORT_I                 = 6,
 SC_INT_I                   = 7,
 SC_LONG_I                  = 8,
 SC_LONG_LONG_I             = 9,
 SC_FLOAT_I                 = 10,
 SC_DOUBLE_I                = 11,
 SC_LONG_DOUBLE_I           = 12,
 SC_FLOAT_COMPLEX_I         = 13,
 SC_DOUBLE_COMPLEX_I        = 14,
 SC_LONG_DOUBLE_COMPLEX_I   = 15,
 SC_QUATERNION_I            = 16,

 SC_INT16_I                 = 6, 
 SC_INT32_I                 = 7,
 SC_INT64_I                 = 9,
 SC_FLOAT32_I               = 10,
 SC_FLOAT64_I               = 11,
 SC_FLOAT128_I              = 12,
 SC_COMPLEX32_I             = 13,
 SC_COMPLEX64_I             = 14,
 SC_COMPLEX128_I            = 15,

/* these must shadow SC_BIT_I thru SC_QUATERNION_I
 * so that SC_xxx_P_I = SC_xxx_I - SC_BIT_I + SC_POINTER_I
 */
 SC_POINTER_I               = 17,
 SC_BOOL_P_I                = 18,
 SC_STRING_I                = 19,
 SC_WCHAR_P_I               = 20,
 SC_INT8_P_I                = 21,
 SC_SHORT_P_I               = 22,
 SC_INT_P_I                 = 23,
 SC_LONG_P_I                = 24,
 SC_LONG_LONG_P_I           = 25,
 SC_FLOAT_P_I               = 26,
 SC_DOUBLE_P_I              = 27,
 SC_LONG_DOUBLE_P_I         = 28,
 SC_FLOAT_COMPLEX_P_I       = 29,
 SC_DOUBLE_COMPLEX_P_I      = 30,
 SC_LONG_DOUBLE_COMPLEX_P_I = 31,
 SC_QUATERNION_P_I          = 32,

 SC_INT16_P_I               = 22, 
 SC_INT32_P_I               = 23,
 SC_INT64_P_I               = 25,
 SC_FLOAT32_P_I             = 26,
 SC_FLOAT64_P_I             = 27,
 SC_FLOAT128_P_I            = 28,
 SC_COMPLEX32_P_I           = 29,
 SC_COMPLEX64_P_I           = 30,
 SC_COMPLEX128_P_I          = 31,

 SC_VOID_I                  = 33,
 SC_CHAR_8_I                = 34,
 SC_STRUCT_I                = 35,
 SC_PCONS_I                 = 36,
 SC_PROCESS_I               = 37,
 SC_FILE_I                  = 38,
 SC_PCONS_P_I               = 39,

/* aliases */
 SC_ENUM_I                  = 7,
 SC_INTEGER_I               = 7,
 SC_REAL_I                  = 11,
 SC_REAL_P_I                = 27;

char
 *SC_UNKNOWN_S               = "unknown",
 *SC_BIT_S                   = "bit",
 *SC_BOOL_S                  = "bool",
 *SC_CHAR_S                  = "char",
 *SC_WCHAR_S                 = "wchar",
 *SC_INT8_S                  = "int8_t",
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

 *SC_INT16_S                 = "int16_t", 
 *SC_INT32_S                 = "int32_t",
 *SC_INT64_S                 = "int64_t",
 *SC_FLOAT32_S               = "float32_t",
 *SC_FLOAT64_S               = "float64_t",
 *SC_FLOAT128_S              = "float128_t",
 *SC_COMPLEX32_S             = "complex32_t",
 *SC_COMPLEX64_S             = "complex64_t",
 *SC_COMPLEX128_S            = "complex128_t",

/* these must shadow SC_BIT_S thru SC_QUATERNION_S */
 *SC_POINTER_S               = "void *",
 *SC_BOOL_P_S                = "bool *",
 *SC_STRING_S                = "char *",
 *SC_WCHAR_P_S               = "wchar *",
 *SC_INT8_P_S                = "int8_t *",
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

 *SC_INT16_P_S               = "int16_t *", 
 *SC_INT32_P_S               = "int32_t *",
 *SC_INT64_P_S               = "int64_t *",
 *SC_FLOAT32_P_S             = "float32_t *",
 *SC_FLOAT64_P_S             = "float64_t *",
 *SC_FLOAT128_P_S            = "float128_t *",
 *SC_COMPLEX32_P_S           = "complex32_t *",
 *SC_COMPLEX64_P_S           = "complex64_t *",
 *SC_COMPLEX128_P_S          = "float128_t *",

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

static SC_type *_SC_get_type_name(char *name)
   {hasharr *ha;
    SC_type *t;

    SC_init_base_types();

    ha = (hasharr *) _SC.types.typ;
    t  = (SC_type *) SC_hasharr_def_lookup(ha, name);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_REGISTER - register a data type with the type manager */

int SC_type_register(char *name, SC_kind kind, int bpi, ...)
   {int id, ok;
    uint64_t al, fr;
    void *pf;
    SC_type_method attr;
    hasharr *ha;
    SC_type *t;

/* remember current memory stats so that we can make any
 * type manager memory unaccountable
 * this memory doesn't leak per se
 * and the accounting is very difficult
 */
    SC_mem_statb(&al, &fr, NULL, NULL);

    ha = (hasharr *) _SC.types.typ;
    if (ha == NULL)
       {ha = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);
	_SC.types.typ = ha;
	SC_init_base_types();};

    SC_VA_START(bpi);

    t = (SC_type *) SC_hasharr_def_lookup(ha, name);
    if (t != NULL)
       id = t->id;
    else
       {id = SC_hasharr_get_n(ha);

	t = SC_permanent(CMAKE(SC_type));
	t->id   = id;
	t->type = SC_permanent(CSTRSAVE(name));
	t->kind = kind;
	t->bpi  = bpi;

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

	SC_hasharr_install(ha, name, t, "SC_TYPE", TRUE, TRUE);};

    SC_VA_END;

/* restore the initial memory stats */
    SC_mem_statb_set(al, fr);

    return(id);}

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

    t  = SC_permanent(CMAKE(SC_type));
    *t = *ot;

    t->type = SC_permanent(CSTRSAVE(name));

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

/* SC_TYPE_PTR_ID - given the ID of a primitive type
 *                - return the ID of the pointer to the type
 *                - return -1 if type NAME is unknown
 */

int SC_type_ptr_id(int id)
   {int pid;

    pid = id - SC_BIT_I + SC_POINTER_I;

    return(pid);}

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
    rv = ((t != NULL) && (t->kind == KIND_CHAR));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_FIX - return TRUE if ID is a fixed point type */

int SC_is_type_fix(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->kind == KIND_INT));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_FP - return TRUE if ID is a floating point type */

int SC_is_type_fp(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->kind == KIND_FLOAT));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_CX - return TRUE if ID is a complex floating point type */

int SC_is_type_cx(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->kind == KIND_COMPLEX));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_QUT - return TRUE if ID is a quaternion type */

int SC_is_type_qut(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->kind == KIND_QUATERNION));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_NUM - return TRUE if ID is a numeric type */

int SC_is_type_num(int id)
   {int rv;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) &&
	  ((t->kind == KIND_BOOL)    ||
	   (t->kind == KIND_CHAR)    ||
	   (t->kind == KIND_INT)     ||
	   (t->kind == KIND_FLOAT)   ||
	   (t->kind == KIND_COMPLEX) ||
	   (t->kind == KIND_QUATERNION)));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_TYPE_PTR - return TRUE if ID is a pointer type */

int SC_is_type_ptr(int id)
   {int rv;
    char *typn;
    SC_type *t;

    t  = _SC_get_type_id(id);
    rv = ((t != NULL) && (t->kind == KIND_POINTER));
    if (rv == FALSE)
       {typn = SC_type_name(id);
	rv   = ((typn != NULL) && (strchr(typn, '*') != NULL));};

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
    rv = ((t != NULL) && (t->kind == KIND_STRUCT));

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

/* SC_FIN_TYPE_MANAGER - free up the space of the type manager
 *                     - this had better be pretty near the last
 *                     - PACT action taken in an application
 */

void SC_fin_type_manager(void)
   {int i;
    char **fmts, **fmta;
    hasharr *ha;

    ha   = (hasharr *) _SC.types.typ;
    fmts = _SC.types.formats;
    fmta = _SC.types.formata;

    for (i = 0; i < N_TYPES; i++)
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
   {int szptr;

    ONCE_SAFE(FALSE, NULL)
       szptr = sizeof(char *);

/* these must have the same sequence/values as the static values
 * in the definition at the top of the file
 * changes here MUST be reflected there
 */

       SC_UNKNOWN_I               = SC_type_register(SC_UNKNOWN_S,             KIND_OTHER,      0,                            0);

       SC_BIT_I                   = SC_type_register(SC_BIT_S,                 KIND_OTHER,      0,                            0);
       SC_BOOL_I                  = SC_type_register(SC_BOOL_S,                KIND_BOOL,       sizeof(bool),                 0);
       SC_CHAR_I                  = SC_type_register(SC_CHAR_S,                KIND_CHAR,       sizeof(char),                 0);
       SC_WCHAR_I                 = SC_type_register(SC_WCHAR_S,               KIND_CHAR,       sizeof(wchar_t),              0);
       SC_INT8_I                  = SC_type_register(SC_INT8_S,                KIND_INT,        sizeof(int8_t),               0);
       SC_SHORT_I                 = SC_type_register(SC_SHORT_S,               KIND_INT,        sizeof(short),                0);
       SC_INT_I                   = SC_type_register(SC_INT_S,                 KIND_INT,        sizeof(int),                  0);
       SC_LONG_I                  = SC_type_register(SC_LONG_S,                KIND_INT,        sizeof(long),                 0);
       SC_LONG_LONG_I             = SC_type_register(SC_LONG_LONG_S,           KIND_INT,        sizeof(long long),            0);
       SC_FLOAT_I                 = SC_type_register(SC_FLOAT_S,               KIND_FLOAT,      sizeof(float),                0);
       SC_DOUBLE_I                = SC_type_register(SC_DOUBLE_S,              KIND_FLOAT,      sizeof(double),               0);
       SC_LONG_DOUBLE_I           = SC_type_register(SC_LONG_DOUBLE_S,         KIND_FLOAT,      sizeof(long double),          0);
       SC_FLOAT_COMPLEX_I         = SC_type_register(SC_FLOAT_COMPLEX_S,       KIND_COMPLEX,    sizeof(float _Complex),       0);
       SC_DOUBLE_COMPLEX_I        = SC_type_register(SC_DOUBLE_COMPLEX_S,      KIND_COMPLEX,    sizeof(double _Complex),      0);
       SC_LONG_DOUBLE_COMPLEX_I   = SC_type_register(SC_LONG_DOUBLE_COMPLEX_S, KIND_COMPLEX,    sizeof(long double _Complex), 0);
       SC_QUATERNION_I            = SC_type_register(SC_QUATERNION_S,          KIND_QUATERNION, 4*sizeof(double),             0);

/* these must shadow SC_BIT_I thru SC_QUATERNION_I
 * so that SC_xxx_P_I = SC_xxx_I - SC_BIT_I + SC_POINTER_I
 */
       SC_POINTER_I               = SC_type_register(SC_POINTER_S,               KIND_POINTER, szptr, 0);
       SC_BOOL_P_I                = SC_type_register(SC_BOOL_P_S,                KIND_POINTER, szptr, 0);
       SC_STRING_I                = SC_type_register(SC_STRING_S,                KIND_POINTER, szptr, 0);
       SC_WCHAR_P_I               = SC_type_register(SC_WCHAR_P_S,               KIND_POINTER, szptr, 0);
       SC_INT8_P_I                = SC_type_register(SC_INT8_P_S,                KIND_POINTER, szptr, 0);
       SC_SHORT_P_I               = SC_type_register(SC_SHORT_P_S,               KIND_POINTER, szptr, 0);
       SC_INT_P_I                 = SC_type_register(SC_INT_P_S,                 KIND_POINTER, szptr, 0);
       SC_LONG_P_I                = SC_type_register(SC_LONG_P_S,                KIND_POINTER, szptr, 0);
       SC_LONG_LONG_P_I           = SC_type_register(SC_LONG_LONG_P_S,           KIND_POINTER, szptr, 0);
       SC_FLOAT_P_I               = SC_type_register(SC_FLOAT_P_S,               KIND_POINTER, szptr, 0);
       SC_DOUBLE_P_I              = SC_type_register(SC_DOUBLE_P_S,              KIND_POINTER, szptr, 0);
       SC_LONG_DOUBLE_P_I         = SC_type_register(SC_LONG_DOUBLE_P_S,         KIND_POINTER, szptr, 0);
       SC_FLOAT_COMPLEX_P_I       = SC_type_register(SC_FLOAT_COMPLEX_P_S,       KIND_POINTER, szptr, 0);
       SC_DOUBLE_COMPLEX_P_I      = SC_type_register(SC_DOUBLE_COMPLEX_P_S,      KIND_POINTER, szptr, 0);
       SC_LONG_DOUBLE_COMPLEX_P_I = SC_type_register(SC_LONG_DOUBLE_COMPLEX_P_S, KIND_POINTER, szptr, 0);
       SC_QUATERNION_P_I          = SC_type_register(SC_QUATERNION_P_S,          KIND_POINTER, szptr, 0);

       SC_VOID_I                  = SC_type_register(SC_VOID_S,     KIND_OTHER,   0,               0);
       SC_CHAR_8_I                = SC_type_register(SC_CHAR_8_S,   KIND_CHAR,    sizeof(char),    0);
       SC_STRUCT_I                = SC_type_register(SC_STRUCT_S,   KIND_STRUCT,  0,               0);
       SC_PCONS_I                 = SC_type_register(SC_PCONS_S,    KIND_STRUCT,  sizeof(pcons),   0);
       SC_PROCESS_I               = SC_type_register(SC_PROCESS_S,  KIND_STRUCT,  sizeof(PROCESS), 0);
       SC_FILE_I                  = SC_type_register(SC_FILE_S,     KIND_STRUCT,  sizeof(FILE),    0);

       SC_PCONS_P_I               = SC_type_register(SC_PCONS_P_S,  KIND_POINTER, szptr,           0);

/* aliases */
       SC_ENUM_I         = SC_type_alias(SC_ENUM_S,    SC_INT_I);

       SC_INTEGER_I      = SC_type_alias(SC_INTEGER_S, SC_INT_I);
       SC_INT16_I        = SC_type_alias(SC_INT16_S,   SC_SHORT_I);
       SC_INT32_I        = SC_type_alias(SC_INT32_S,   SC_INT_I);
       SC_INT64_I        = SC_type_alias(SC_INT64_S,   SC_LONG_LONG_I);

       SC_REAL_I         = SC_type_alias(SC_REAL_S,    SC_DOUBLE_I);
       SC_FLOAT32_I      = SC_type_alias(SC_FLOAT32_S,  SC_FLOAT_I);
       SC_FLOAT64_I      = SC_type_alias(SC_FLOAT64_S,  SC_DOUBLE_I);
       SC_FLOAT128_I     = SC_type_alias(SC_FLOAT128_S, SC_LONG_DOUBLE_I);

       SC_COMPLEX32_I    = SC_type_alias(SC_COMPLEX32_S,  SC_FLOAT_COMPLEX_I);
       SC_COMPLEX64_I    = SC_type_alias(SC_COMPLEX64_S,  SC_DOUBLE_COMPLEX_I);
       SC_COMPLEX128_I   = SC_type_alias(SC_COMPLEX128_S, SC_LONG_DOUBLE_COMPLEX_I);

       SC_INT16_P_I      = SC_type_alias(SC_INT16_P_S,   SC_SHORT_P_I);
       SC_INT32_P_I      = SC_type_alias(SC_INT32_P_S,   SC_INT_P_I);
       SC_INT64_P_I      = SC_type_alias(SC_INT64_P_S,   SC_LONG_LONG_P_I);

       SC_REAL_P_I       = SC_type_alias(SC_REAL_P_S,  SC_DOUBLE_P_I);
       SC_FLOAT32_P_I    = SC_type_alias(SC_FLOAT32_P_S,  SC_FLOAT_P_I);
       SC_FLOAT64_P_I    = SC_type_alias(SC_FLOAT64_P_S,  SC_DOUBLE_P_I);
       SC_FLOAT128_P_I   = SC_type_alias(SC_FLOAT128_P_S, SC_LONG_DOUBLE_P_I);

       SC_COMPLEX32_P_I  = SC_type_alias(SC_COMPLEX32_P_S,  SC_FLOAT_COMPLEX_P_I);
       SC_COMPLEX64_P_I  = SC_type_alias(SC_COMPLEX64_P_S,  SC_DOUBLE_COMPLEX_P_I);
       SC_COMPLEX128_P_I = SC_type_alias(SC_COMPLEX128_P_S, SC_LONG_DOUBLE_COMPLEX_P_I);

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
       {case KIND_CHAR :
             for (i = 0; i < N_PRIMITIVE_CHAR; i++)
		 {id = i + SC_CHAR_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CHAR)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_INT :
             for (i = 0; i < N_PRIMITIVE_FIX; i++)
		 {id = i + SC_INT8_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FIX)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_FLOAT :
             for (i = 0; i < N_PRIMITIVE_FP; i++)
		 {id = i + SC_FLOAT_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FP)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_COMPLEX :
             for (i = 0; i < N_PRIMITIVE_CPX; i++)
		 {id = i + SC_FLOAT_COMPLEX_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb <= t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CPX)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_QUATERNION :
             for (i = 0; i < N_PRIMITIVE_QUT; i++)
		 {id = i + SC_QUATERNION_I;
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
       {case KIND_CHAR :
             for (i = 0; i < N_PRIMITIVE_CHAR; i++)
		 {id = i + SC_CHAR_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CHAR)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_INT :
             for (i = 0; i < N_PRIMITIVE_FIX; i++)
		 {id = i + SC_INT8_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FIX)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_FLOAT :
             for (i = 0; i < N_PRIMITIVE_FP; i++)
		 {id = i + SC_FLOAT_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_FP)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_COMPLEX :
             for (i = 0; i < N_PRIMITIVE_CPX; i++)
		 {id = i + SC_FLOAT_COMPLEX_I;
		  hp = *(haelem **) SC_hasharr_get(ha, id);
		  if (hp != NULL)
		     {t = (SC_type *) hp->def;
		      if (nb == t->bpi)
			 break;};};
	     if (i >= N_PRIMITIVE_CPX)
	        id = SC_UNKNOWN_I;
             break;

        case KIND_QUATERNION :
             for (i = 0; i < N_PRIMITIVE_QUT; i++)
		 {id = i + SC_QUATERNION_I;
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
 *               -     SD   - stride through destination
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

void *SC_convert_id(int did, void *d, long od, long ds,
		    int sid, void *s, long os, long ss,
		    long n, int flag)
   {int rv, bpi;
    long nc;

/* allocate the space if need be */
    if (d == NULL)
       {bpi = SC_type_size_i(did);
	d   = CMAKE_N(char, n*bpi);};

    if (_SC_convf[did][sid] != NULL)
       nc = _SC_convf[did][sid](d, od, ds, s, os, ss, n);
    
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

#if 0
    if (id > N_TYPES)
       {id -= (SC_POINTER_I - SC_BIT_I);
	s   = *(void **) s;};
#endif

    if (id < N_TYPES)
       {if (_SC_strf[id] != NULL)
	   rv = _SC_strf[id](t, nc, s, n, mode);};
    
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
	    {CFREE(ufmts[i]);};

	 if (ufmta[i] != NULL)
	    {CFREE(ufmta[i]);};};

    if (_SC.types.suppress_member != NULL)
       {CFREE(_SC.types.suppress_member);};

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
	    {CFREE(fmts[i]);
	     fmts[i] = CSTRSAVE(ufmts[i]);};

	 if (ufmta[i] != NULL)
	    {CFREE(fmta[i]);
	     fmta[i] = CSTRSAVE(ufmta[i]);};};

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
   {int i, id;
    int *fix_pre;
    precisionfp *fp_pre;
    char tmp[MAXLINE], *t;
    char **fmts, **fmta;
    
    fix_pre = _SC.types.fix_precision;
    fp_pre  = _SC.types.fp_precision;
    fmts    = _SC.types.formats;
    fmta    = _SC.types.formata;

/* fmts is used for scalars */

    if (fmts[SC_BIT_I] != NULL)
       CFREE(fmts[SC_BIT_I]);

    t = SC_permanent(CSTRSAVE("%x"));
    fmts[SC_BIT_I] = t;

    if (fmts[SC_BOOL_I] != NULL)
       CFREE(fmts[SC_BOOL_I]);

    t = SC_permanent(CSTRSAVE("%s"));
    fmts[SC_BOOL_I] = t;

/* character types (proper) */
    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        {id = i + SC_CHAR_I;
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_CHAR_I)
	    snprintf(tmp, MAXLINE, "%%c");
	 else if (id == SC_WCHAR_I)
	    snprintf(tmp, MAXLINE, "%%Lc");

	 t = SC_permanent(CSTRSAVE(tmp));
	 fmts[id] = t;};

/* fixed point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {id = i + SC_INT8_I;
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_LONG_LONG_I)
	    snprintf(tmp, MAXLINE, "%%%dlld", fix_pre[i]);
	 else if (id == SC_LONG_I)
	    snprintf(tmp, MAXLINE, "%%%dld", fix_pre[i]);
	 else
	    snprintf(tmp, MAXLINE, "%%%dd", fix_pre[i]);

	 t = SC_permanent(CSTRSAVE(tmp));
	 fmts[id] = t;};

/* real floating point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {id = i + SC_FLOAT_I;
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_LONG_DOUBLE_I)
	    snprintf(tmp, MAXLINE, "%%# .%dle", fp_pre[i].digits);
	 else
	    snprintf(tmp, MAXLINE, "%%# .%de", fp_pre[i].digits);

	 t = SC_permanent(CSTRSAVE(tmp));
	 fmts[id] = t;};

/* complex floating point types (proper) */
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {id = i + SC_FLOAT_COMPLEX_I;
	 if (fmts[id] != NULL)
	    CFREE(fmts[id]);

	 if (id == SC_LONG_DOUBLE_COMPLEX_I)
	    snprintf(tmp, MAXLINE, "%%# .%dle + %%# .%dle*I",
		     fp_pre[i].digits, fp_pre[i].digits);
	 else
	    snprintf(tmp, MAXLINE, "%%# .%de + %%# .%de*I",
		     fp_pre[i].digits, fp_pre[i].digits);

	 t = SC_permanent(CSTRSAVE(tmp));
	 fmts[id] = t;};

/* other primitive types */
    if (fmts[SC_STRING_I] != NULL)
       CFREE(fmts[SC_STRING_I]);

    t = SC_permanent(CSTRSAVE("%s"));
    fmts[SC_STRING_I] = t;

/* fmta is used for arrays */
    for (i = 0; i < N_TYPES; i++)
        {if (fmta[i] != NULL)
            CFREE(fmta[i]);

         t = SC_permanent(CSTRSAVE(fmts[i]));
         fmta[i] = t;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
