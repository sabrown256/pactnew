/*
 * SCOPE_TYPEH.H - support type handling in SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_TYPEH

#define PCK_SCOPE_TYPEH

#include "cpyright.h"

# define REAL double
# define HUGE_REAL 1.0e100
FUNCTION_POINTER(double, *(*PFPREAL));

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_SC_type SC_type;

/* SC_type
 *    - make a struct to encapsulate basic type information
 *    - to facilitate working with types as first class objects
 *    - also a place for further type related development
 *    - this is complementary to the PDB type system in that
 *    - in PDB the emphasis is in data conversion in file I/O
 *    - while here it is about working with data types in
 *    - programming
 */

struct s_SC_type
   {int id;                    /* integer type id */
    char *type;                /* type name */
    int bpi;                   /* bytes per item */
    void (*free)(void *x);};   /* function to free instance */

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern int
 SC_UNKNOWN_I,
 SC_BIT_I,
 SC_BOOL_I,
 SC_CHAR_I,
 SC_SHORT_I,
 SC_INT_I,
 SC_LONG_I,
 SC_LONG_LONG_I,
 SC_FLOAT_I,
 SC_DOUBLE_I,
 SC_LONG_DOUBLE_I,
 SC_FLOAT_COMPLEX_I,
 SC_DOUBLE_COMPLEX_I,
 SC_LONG_DOUBLE_COMPLEX_I,
 SC_QUATERNION_I,
 SC_STRING_I,
 SC_POINTER_I,
 SC_BOOL_P_I,
 SC_SHORT_P_I,
 SC_INT_P_I,
 SC_LONG_P_I,
 SC_LONG_LONG_P_I,
 SC_FLOAT_P_I,
 SC_DOUBLE_P_I,
 SC_LONG_DOUBLE_P_I,
 SC_FLOAT_COMPLEX_P_I,
 SC_DOUBLE_COMPLEX_P_I,
 SC_LONG_DOUBLE_COMPLEX_P_I,
 SC_QUATERNION_P_I,
 SC_VOID_I,
 SC_CHAR_8_I,
 SC_ENUM_I,
 SC_STRUCT_I,
 SC_PCONS_I,
 SC_PROCESS_I,
 SC_FILE_I,
 SC_PCONS_P_I,
 SC_INTEGER_I,
 SC_REAL_I,
 SC_REAL_P_I;

extern char
 *SC_UNKNOWN_S,
 *SC_BIT_S,
 *SC_BOOL_S,
 *SC_CHAR_S,
 *SC_SHORT_S,
 *SC_INT_S,
 *SC_LONG_S,
 *SC_LONG_LONG_S,
 *SC_FLOAT_S,
 *SC_DOUBLE_S,
 *SC_LONG_DOUBLE_S,
 *SC_FLOAT_COMPLEX_S,
 *SC_DOUBLE_COMPLEX_S,
 *SC_LONG_DOUBLE_COMPLEX_S,
 *SC_QUATERNION_S,
 *SC_STRING_S,
 *SC_POINTER_S,
 *SC_BOOL_P_S,
 *SC_SHORT_P_S,
 *SC_INT_P_S,
 *SC_LONG_P_S,
 *SC_LONG_LONG_P_S,
 *SC_FLOAT_P_S,
 *SC_DOUBLE_P_S,
 *SC_LONG_DOUBLE_P_S,
 *SC_FLOAT_COMPLEX_P_S,
 *SC_DOUBLE_COMPLEX_P_S,
 *SC_LONG_DOUBLE_COMPLEX_P_S,
 *SC_QUATERNION_P_S,
 *SC_VOID_S,
 *SC_CHAR_8_S,
 *SC_ENUM_S,
 *SC_STRUCT_S,
 *SC_PCONS_S,
 *SC_PROCESS_S,
 *SC_FILE_S,
 *SC_PCONS_P_S,
 *SC_INTEGER_S,
 *SC_REAL_S,
 *SC_REAL_P_S;

extern int
 (*SC_sizeof_hook)(char *s);                     /* string driven size hook */

extern int
 (*SC_convert_hook)(char *dtype, void **pd,
                    char *stype, void *s,
                    int n, int flag);     /* data type conversion in memory */

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* SCFNCA.C declarations */

extern char
 *SC_dereference(char *s);

extern int
 SC_fix_lmt(int nb, BIGINT *pmn, BIGINT *pmx, BIGINT *pumx),
 SC_convert_id(int did, void **pd, int sid, void *s, int n, int flag),
 SC_convert(char *dtype, void **pd, char *stype, void *s, int n, int flag),
 SC_unpack_bits(char *out, char *in, int ityp, int nbits,
		int padsz, int fpp, long nitems, long offs),
 SC_sizeof(char *s);

extern long
 SC_extract_field(char *in, int offs, int nbi, int nby, int *ord);


/* SCTYP.C declarations */

extern int
 SC_register_type(char *name, int bpi, ...),
 SC_type_alias(char *name, int id),
 SC_type_id(char *name, int unsg),
 SC_fix_type_id(char *name, int unsg),
 SC_fp_type_id(char *name),
 SC_cx_type_id(char *name),
 SC_type_size_i(int id),
 SC_type_size_a(char *name),
 SC_convert(char *dtype, void **d, char *stype, void *s, int n, int flag);

extern size_t
 SC_copy_primitive(void *d, void *s, long n, int id);

extern char
 *SC_type_name(int id);

extern void
 SC_type_free_i(int id, void *x),
 SC_type_free_a(char *name, void *x),
 SC_init_base_types(void);

#ifdef __cplusplus
}
#endif

#endif

