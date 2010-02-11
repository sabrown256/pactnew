/*
 * SCTYP.C - type management functions for PACT
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "score_int.h"

int
 SC_CHAR_I       = 1,
 SC_SHORT_I      = 2,
 SC_INTEGER_I    = 3,
 SC_LONG_I       = 4,
 SC_ENUM_I       = 5,
 SC_FLOAT_I      = 6,
 SC_DOUBLE_I     = 7,
 SC_REAL_I       = 8,
 SC_STRING_I     = 9,
 SC_POINTER_I    = 10,
 SC_CHAR_8_I     = 11,
 SC_VOID_I       = 12,
 SC_SHORT_P_I    = 13,
 SC_INTEGER_P_I  = 14,
 SC_LONG_P_I     = 15,
 SC_ENUM_P_I     = 16,
 SC_FLOAT_P_I    = 17,
 SC_REAL_P_I     = 18,
 SC_DOUBLE_P_I   = 19,
 SC_BIGINT_I     = 20,
 SC_BIGINT_P_I   = 21,
 SC_UNKNOWN_I    = 22,
 SC_PCONS_I      = 23,
 SC_PCONS_P_I    = 24,
#ifdef _LARGE_FILES
 SC_OFF_T_I      = 20,
 SC_OFF_T_P_I    = 21,
#else
 SC_OFF_T_I      = 4,
 SC_OFF_T_P_I    = 15,
#endif
 SC_STRUCT_I     = 25,
 SC_PROCESS_I    = 26,
 SC_FILE_I       = 27;

char
 *SC_CHAR_8_S    = "char_8",
 *SC_CHAR_S      = "char",
 *SC_SHORT_S     = "short",
 *SC_INTEGER_S   = "integer",
 *SC_LONG_S      = "long",
 *SC_ENUM_S      = "enum",
 *SC_BIGINT_S    = "BIGINT",
 *SC_FLOAT_S     = "float",
 *SC_REAL_S      = "double",
 *SC_DOUBLE_S    = "double",
 *SC_STRING_S    = "char *",
 *SC_POINTER_S   = "void *",
 *SC_PCONS_P_S   = "pcons *",
 *SC_VOID_S      = "void",
 *SC_SHORT_P_S   = "short *",
 *SC_INTEGER_P_S = "integer *",
 *SC_LONG_P_S    = "long *",
 *SC_ENUM_P_S    = "enum *",
 *SC_BIGINT_P_S  = "BIGINT *",
 *SC_FLOAT_P_S   = "float *",
 *SC_REAL_P_S    = "double *",
 *SC_DOUBLE_P_S  = "double *",
 *SC_PCONS_S     = "pcons",
 *SC_STRUCT_S    = "struct",
 *SC_UNKNOWN_S   = "unknown";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_TYPE - register a data type with the type manager
 *                  - the types are referenced via a linear array
 *                  - and a hash table
 *                  - the linear array gives fast lookups when using
 *                  - the integer id number
 *                  - the hash table gives fast lookups when using
 *                  - the type name
 */

int SC_register_type(char *name, int bpi, ...)
   {int n;
    SC_type t;
    SC_type *pt;

    if (_SC.typh == NULL)
       {_SC.typh = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);
	_SC.typa = SC_MAKE_ARRAY("SC_REGISTER_TYPE", SC_type, NULL);};

    n = SC_array_get_n(_SC.typa);

    SC_VA_START(bpi);

    t.id   = n;
    t.type = SC_strsavef(name, "PERM|char*:SC_REGISTER_TYPE:type");
    t.free = SC_VA_ARG(PFVoidAPV);
    t.bpi  = bpi;

    SC_VA_END;

/* enter the type in the linear array */
    SC_array_set(_SC.typa, -1, &t);

/* enter the type in the hash table */
    pt = SC_array_get(_SC.typa, n);
    SC_hasharr_install(_SC.typh, name, pt, "SC_TYPE", TRUE, TRUE);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_ID - given the type NAME return the type id
 *            - return -1 if type NAME is unknown
 */

int SC_type_id(char *name)
   {int n;
    SC_type *t;

    t = (SC_type *) SC_hasharr_def_lookup(_SC.typh, name);
    n = (t != NULL) ? t->id : -1;

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

    t    = SC_array_get(_SC.typa, id);
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

    t   = SC_array_get(_SC.typa, id);
    bpi = (t != NULL) ? t->bpi : -1;

    return(bpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_SIZE_A - free an instance X of type NAME */

int SC_type_size_a(char *name)
   {int bpi;
    SC_type *t;

    t   = (SC_type *) SC_hasharr_def_lookup(_SC.typh, name);
    bpi = (t != NULL) ? t->bpi : -1;

    return(bpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_FREE_I - free an instance X of type ID */

void SC_type_free_i(int id, void *x)
   {SC_type *t;

    t   = SC_array_get(_SC.typa, id);
    if (t != NULL)
       (*t->free)(x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_FREE_A - given the type NAME return the type free
 *                - return -1 if type NAME is unknown
 */

void SC_type_free_a(char *name, void *x)
   {SC_type *t;

    t   = (SC_type *) SC_hasharr_def_lookup(_SC.typh, name);
    if (t != NULL)
       (*t->free)(x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_BASE_TYPES - register the common C types
 *                    - and some types used often in PACT
 */

void SC_init_base_types(void)
   {int szptr;
    enum {A, B, C} x;

    szptr = sizeof(char *);

    SC_UNKNOWN_I = SC_register_type("unknown", 0,               NULL);

    SC_CHAR_I    = SC_register_type("char",    sizeof(char),    NULL);
    SC_SHORT_I   = SC_register_type("short",   sizeof(short),   NULL);
    SC_INTEGER_I = SC_register_type("int",     sizeof(int),     NULL);
    SC_LONG_I    = SC_register_type("long",    sizeof(long),    NULL);
    SC_ENUM_I    = SC_register_type("enum",    sizeof(x),       NULL);
    SC_FLOAT_I   = SC_register_type("float",   sizeof(float),   NULL);
    SC_DOUBLE_I  = SC_register_type("double",  sizeof(double),  NULL);
    SC_REAL_I    = SC_register_type("REAL",    sizeof(REAL),    NULL);
    SC_STRING_I  = SC_register_type("string",  szptr,           NULL);
    SC_VOID_I    = SC_register_type("void",    0,               NULL);
    SC_STRUCT_I  = SC_register_type("struct",  0,               NULL);
    SC_POINTER_I = SC_register_type("pointer", szptr,           NULL);
    SC_POINTER_I = SC_register_type("*",       szptr,           NULL);
    SC_BIGINT_I  = SC_register_type("BIGINT",  sizeof(BIGINT),  NULL);
    SC_PCONS_I   = SC_register_type("pcons",   sizeof(pcons),   NULL);
    SC_PROCESS_I = SC_register_type("PROCESS", sizeof(PROCESS), NULL);
    SC_FILE_I    = SC_register_type("FILE",    sizeof(FILE),    NULL);

/* fortran type */
    SC_CHAR_8_I  = SC_register_type("char", sizeof(char),       NULL);

/* some pointer types */
    SC_SHORT_P_I   = SC_register_type("short *",  szptr, NULL);
    SC_INTEGER_P_I = SC_register_type("int *",    szptr, NULL);
    SC_LONG_P_I    = SC_register_type("long *",   szptr, NULL);
    SC_ENUM_P_I    = SC_register_type("enum *",   szptr, NULL);
    SC_FLOAT_P_I   = SC_register_type("float *",  szptr, NULL);
    SC_DOUBLE_P_I  = SC_register_type("double *", szptr, NULL);
    SC_REAL_P_I    = SC_register_type("REAL *",   szptr, NULL);
    SC_BIGINT_P_I  = SC_register_type("BIGINT *", szptr, NULL);
    SC_PCONS_P_I   = SC_register_type("pcons *",  szptr, NULL);

    if (SC_HAVE_LARGE_FILES)
       {SC_OFF_T_I   = SC_BIGINT_I;
	SC_OFF_T_P_I = SC_BIGINT_P_I;}
    else
       {SC_OFF_T_I   = SC_LONG_I;
	SC_OFF_T_P_I = SC_LONG_P_I;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
