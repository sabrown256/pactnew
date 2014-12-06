/*
 * LIBTYP.C - routines supporting PACT type management
 *
 * NOTES:
 *    fundamental types - mostly primitive
 *                        (quaternion and FILE are exceptional)
 *    type foo or foo_t implies variables:
 *        SC_FOO_I, SC_FOO_S, SC_FOO_P_I, SC_FOO_P_S
 *    whack the "_t" for foo_t
 *    pointer versions have min/max  -LLONG_MAX  LLONG_MAX
 *   
 *    KIND_CHAR/KIND_INT have
 *      signed       unsigned    variations
 *      int8_t       u_int8_t
 *      signed int   unsigned int
 *   
 *    pointer types have default value of NULL
 *   
 *    f90 type names are uniquely different
 *    other languages follow C naming by and large
 *    f90 pointer types have "-A" convention
 *       char   -> character
 *       char * -> character-A
 *
 * Generated from this:
 *
 *    1) typinf ti[] = DEF_TYPINF;     for template.c
 *    2) DEF_GENERATED_TYPES;          for init_types in blang.c
 *    3) DEF_STANDARD_TYPES;           for SC_init_base_types in sctyp.c
 *    4) #define SC_xxx_I   et. al.    for scope_typeh.h
 *    5) _SC_DEF_TYP_                  for scctl.c
 *    6) DEF_PANACEA_TYPES;            for PA_cpp_default in pacpp.c
 *
 * libtyp.c has routines to parse and manage the master type tables
 * for use by blang and template
 *
 * blang generates (3), (4), (5), and (6)
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBTYP

# define LIBTYP

enum e_type_kind
   {TK_PRIMITIVE = 0, TK_ENUM, TK_STRUCT, TK_UNION};

typedef enum e_type_kind type_kind;

enum e_type_group
   {KIND_CHAR = 0,
    KIND_BOOL,
    KIND_INT,
    KIND_FLOAT,
    KIND_COMPLEX,
    KIND_QUATERNION,
    KIND_POINTER,
    KIND_STRUCT,
    KIND_UNION,
    KIND_OTHER};

typedef enum e_type_group type_group;

typedef struct s_typdes typdes;
typedef struct s_typalias typalias;

struct s_typdes
   {char *type;     /* C type name quoted to capture white space,
                     * e.g. "long long" */
    char *name;     /* abbreviate name to use in generated function names */
    bool ptr;       /* generate "foo *" for "foo" iff TRUE */
    type_kind knd;  /* enumerated type kind: primitive, enum, ... */
    type_group g;   /* enumerated type group: bool, int, complex, ... */
    char *size;     /* size or method for determining size of a
                     * single instance */
    char *mn;       /* minimum value of type */
    char *mx;       /* maximum value of type */
    char *defv;     /* default value of type */
    char *f90;};    /* F90 type name for the C type */

static char
 *tykind[4] = {"primitive", "enum", "struct", "union"};

static typdes
 *stl = NULL,
 *atl = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_TYPE - push TD onto the type list LT */

typdes *push_type(typdes *lt, typdes *td)
   {int nb;
    static int ni = 0;
    static int nx = 10;

    if (lt == NULL)
       {ni = 0;
        lt = MAKE_N(typdes, nx);};
	
    if ((ni % nx == 0) && (ni > 0))
       {nb = ni/nx;
	REMAKE(lt, typdes, (nb+1)*nx);};

    if (td != NULL)
       lt[ni] = *td;
    else
       memset(lt+ni, 0, sizeof(typdes));

    ni++;

    return(lt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_TYPE_PTR - push a pointer type derived from TD onto LT */

typdes *push_type_ptr(typdes *lt, typdes *td)
   {char t[BFLRG];
    typdes tp;

    tp = *td;

    snprintf(t, BFLRG, "%s *", tp.type);
    tp.type = STRSAVE(t);

    tp.name = NULL;

    tp.knd = TK_PRIMITIVE;
    tp.g   = KIND_POINTER;
    tp.size = "sizeof";
    tp.ptr  = B_F;
    tp.mn   = NULL;
    tp.mx   = NULL;
    tp.defv = NULL;

    snprintf(t, BFLRG, "%s-A", tp.f90);
    tp.f90 = STRSAVE(t);

    lt = push_type(lt, &tp);

    return(lt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_STANDARD_TYPES - parse the master type table, type-table */

void parse_standard_types(char **sa)
   {int i, in;
    char *s, **ta;
    typdes td;

    in = FALSE;

    for (i = 0; sa[i] != NULL; i++)
        {s = trim(sa[i], BOTH, " \t\n");
	 if (blank_line(s))
	    continue;
	 else if (strcmp(s, "standard = {") == 0)
	    in = TRUE;
	 else if (strcmp(s, "}") == 0)
	    {in = FALSE;
	     break;}
	 else if (in == TRUE)
	    {ta = tokenize(s, " \t\n", 0);
             td.type = trim(ta[0], BOTH, "\" ");
             td.name = ta[1];

             td.knd = TK_PRIMITIVE;
	     if (strcmp(ta[2], "CHAR") == 0)
	        td.g = KIND_CHAR;
	     else if (strcmp(ta[2], "BOOL") == 0)
	        td.g = KIND_BOOL;
	     else if (strcmp(ta[2], "INT") == 0)
	        td.g = KIND_INT;
	     else if (strcmp(ta[2], "FLOAT") == 0)
	        td.g = KIND_FLOAT;
	     else if (strcmp(ta[2], "COMPLEX") == 0)
	        td.g = KIND_COMPLEX;
	     else if (strcmp(ta[2], "QUATERNION") == 0)
	        td.g = KIND_QUATERNION;
	     else if (strcmp(ta[2], "POINTER") == 0)
	        td.g = KIND_POINTER;
	     else if (strcmp(ta[2], "STRUCT") == 0)
	        {td.g   = KIND_STRUCT;
		 td.knd = TK_STRUCT;}
	     else if (strcmp(ta[2], "UNION") == 0)
	        {td.g   = KIND_UNION;
		 td.knd = TK_UNION;}
	     else if (strcmp(ta[2], "OTHER") == 0)
	        td.g = KIND_OTHER;

	     if (strcmp(ta[3], "sizeof") == 0)
	        td.size = NULL;
	     else
	        td.size = ta[3];

             td.ptr = ((strcmp(ta[4], "TRUE") == 0) ||
		       (strcmp(ta[4], "B_T") == 0));

             td.mn   = ta[5];
             td.mx   = ta[6];
             td.defv = ta[7];
             td.f90  = ta[8];

	     stl = push_type(stl, &td);

	     if (td.ptr == B_T)
                stl = push_type_ptr(stl, &td);

	     FREE(ta);};};

    stl = push_type(stl, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_ALIAS_TYPES - parse the master type table, type-table */

void parse_alias_types(char **sa)
   {int i, in;
    char *s, **ta;
    typdes td;

    in = FALSE;

    memset(&td, 0, sizeof(typdes));

    for (i = 0; sa[i] != NULL; i++)
        {s = trim(sa[i], BOTH, " \t\n");
	 if (blank_line(s))
	    continue;
	 else if (strcmp(s, "standard = {") == 0)
	    in = TRUE;
	 else if (strcmp(s, "}") == 0)
	    {in = FALSE;
	     break;}
	 else if (in == TRUE)
	    {ta = tokenize(s, " \t\n", 0);
             td.type = ta[0];                   /* new */
             td.name = ta[1];                   /* old */
             td.ptr = ((strcmp(ta[4], "TRUE") == 0) ||
		       (strcmp(ta[4], "B_T") == 0));

	     atl = push_type(atl, &td);

	     if (td.ptr == B_T)
                atl = push_type_ptr(atl, &td);

	     FREE(ta);};};

    atl = push_type(atl, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_TYPE_TABLE - parse the master type table TYTAB */

void parse_type_table(char *tytab)
   {char **sa;

    sa = file_text(FALSE, tytab);

    parse_standard_types(sa);
    parse_alias_types(sa);

    lst_free(sa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0
/* raw info */

/* fundamental types - mostly primitive (quaternion and FILE are exceptional)
 * type foo or foo_t implies variables:
 *     SC_FOO_I, SC_FOO_S, SC_FOO_P_I, SC_FOO_P_S
 * whack the "_t" for foo_t
 * pointer versions have min/max  -LLONG_MAX  LLONG_MAX
 *
 * KIND_CHAR/KIND_INT have
 *   signed       unsigned    variations
 *   int8_t       u_int8_t
 *   signed int   unsigned int
 *
 * pointer types have default value of NULL
 *
 * f90 type names are uniquely different
 * other languages follow C naming by and large
 * f90 pointer types have "-A" convention
 *    char   -> character
 *    char * -> character-A
 */

/*  type                   name   kind            size     ptr   min        max         def    f90 */

    "unknown"              unk    KIND_OTHER      0        B_F
    "bit"                  bit    KIND_OTHER      0        B_F
    "struct"               strct  KIND_STRUCT     0        B_F

    "void"                 any    KIND_OTHER      0        B_T
    "bool"                 bool   KIND_BOOL       sizeof   B_T   BOOL_MIN   BOOL_MAX    B_F    logical

    "char"                 chr    KIND_CHAR       sizeof   B_T   SCHAR_MIN  SCHAR_MAX   '\0'   character
    "wchar"                wchr   KIND_CHAR       sizeof   B_T   WCHAR_MIN  WCHAR_MAX

    "int8"                 int8   KIND_INT        sizeof   B_T   INT8_MIN   INT8_MAX    0
    "short"                shrt   KIND_INT        sizeof   B_T   SHRT_MIN   SHRT_MAX    0      integer(2)
    "int"                  int    KIND_INT        sizeof   B_T   INT_MIN    INT_MAX     0      integer
    "long"                 lng    KIND_INT        sizeof   B_T   LONG_MIN   LONG_MAX    0L     integer(8)
    "long long"            ll     KIND_INT        sizeof   B_T   LLONG_MIN  LLONG_MAX   0LL    integer(8)

    "float"                flt    KIND_FLOAT      sizeof   B_T   -FLT_MAX   FLT_MAX     0.0    real(4)
    "double"               dbl    KIND_FLOAT      sizeof   B_T   -DBL_MAX   DBL_MAX     0.0    real(8)
    "long double"          ldbl   KIND_FLOAT      sizeof   B_T   -LDBL_MAX  LDBL_MAX    0.0    real(16)

    "float _Complex"       fcx    KIND_COMPLEX    sizeof   B_T   -FLT_MAX   FLT_MAX     0.0    complex(4)
    "double _Complex"      dcx    KIND_COMPLEX    sizeof   B_T   -DBL_MAX   DBL_MAX     0.0    complex(8)
    "long double _Complex" ldcx   KIND_COMPLEX    sizeof   B_T   -LDBL_MAX  LDBL_MAX    0.0    complex(16)

    "quaternion"           qut    KIND_QUATERNION sizeof   B_T   -DBL_MAX   DBL_MAX     Q_ZERO
    "FILE"                 file   KIND_STRUCT     sizeof   B_T

/* aliases */
/* impy the same variables */
    enum                 int                   B_F
    pboolean             int                   B_T
    size_t               unsigned long         B_T
    ssize_t              long                  B_T
    integer              int                   B_T
    int16_t              short                 B_T
    int32_t              int                   B_T
    int64_t              long long             B_T
    REAL                 double                B_T
    float32_t            float                 B_T
    float64_t            double                B_T
    float128_t           long double           B_T
    complex32_t          float _Complex        B_T
    complex64_t          double _Complex       B_T
    complex128_t         long double _Complex  B_T

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEMPLATE.C variables */

typedef struct s_typinf typinf;

struct s_typinf
   {int id;
    char *name;
    char *type;
    char *stype;
    char *utype;
    char *comp;
    char *promo;
    char *mn;
    char *mx;};

static typinf
 ti[] = { {0,  NULL,   NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	  {1,  NULL,   NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	  {2,  "bool", "bool", "bool", "bool", "bool", "int", "BOOL_MIN", "BOOL_MAX"},
	  {3,  "char", "char", "signed char", "unsigned char", "char", "int", "SCHAR_MIN", "SCHAR_MAX"},
	  {4,  "wchr", "wchar_t", "wchar_t", "wchar_t", "wchar_t", "int", "WCHAR_MIN", "WCHAR_MAX"},

	  {5,  "int8", "int8_t", "int8_t", "u_int8_t", "int8_t", "int", "INT8_MIN", "INT8_MAX"},
	  {6,  "shrt", "short", "signed short", "unsigned short", "short", "int", "SHRT_MIN", "SHRT_MAX"},
	  {7,  "int",  "int", "signed int", "unsigned int", "int", "int", "INT_MIN", "INT_MAX"},
	  {8,  "lng",  "long", "signed long", "unsigned long", "long", "long", "LONG_MIN", "LONG_MAX"},
	  {9,  "ll",   "long long", "signed long long", "unsigned long long", "long long", "long long", "LLONG_MIN", "LLONG_MAX"},

	  {10, "flt",  "float", "float", "float", "float", "double", "-FLT_MAX", "FLT_MAX"},
	  {11, "dbl",  "double", "double", "double", "double", "double", "-DBL_MAX", "DBL_MAX"},
	  {12, "ldbl", "long double", "long double", "long double", "long double", "long double", "-LDBL_MAX", "LDBL_MAX"},

	  {13, "fcx",  "float _Complex", "float _Complex", "float _Complex", "float", "float _Complex", "-FLT_MAX", "FLT_MAX"},
	  {14, "dcx",  "double _Complex", "double _Complex", "double _Complex", "double", "double _Complex", "-DBL_MAX", "DBL_MAX"},
	  {15, "ldcx", "long double _Complex", "long double _Complex", "long double _Complex", "long double", "long double _Complex", "-LDBL_MAX", "LDBL_MAX"},

	  {16, "qut",  "quaternion", "quaternion", "quaternion", "double", "quaternion", "-DBL_MAX", "DBL_MAX"},
	  {17, "ptr",  "void *", "void *", "void *", "void *", "void *", "-LLONG_MAX", "LLONG_MAX"},
	  {18, NULL,   NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	  {19, "str",  "char *", "char *", "char *", "char *", "char *", "-LLONG_MAX", "LLONG_MAX"} };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCTYP.C variables */

/* 0 */
       SC_UNKNOWN_I               = SC_type_register(SC_UNKNOWN_S,             KIND_OTHER,      B_F, 0,                            0);

       SC_BIT_I                   = SC_type_register(SC_BIT_S,                 KIND_OTHER,      B_F, 0,                            0);
       SC_BOOL_I                  = SC_type_register(SC_BOOL_S,                KIND_BOOL,       B_T, sizeof(bool),                 0);
       SC_CHAR_I                  = SC_type_register(SC_CHAR_S,                KIND_CHAR,       B_T, sizeof(char),                 0);
       SC_WCHAR_I                 = SC_type_register(SC_WCHAR_S,               KIND_CHAR,       B_T, sizeof(wchar_t),              0);
       SC_INT8_I                  = SC_type_register(SC_INT8_S,                KIND_INT,        B_T, sizeof(int8_t),               0);
       SC_SHORT_I                 = SC_type_register(SC_SHORT_S,               KIND_INT,        B_T, sizeof(short),                0);
       SC_INT_I                   = SC_type_register(SC_INT_S,                 KIND_INT,        B_T, sizeof(int),                  0);
       SC_LONG_I                  = SC_type_register(SC_LONG_S,                KIND_INT,        B_T, sizeof(long),                 0);
       SC_LONG_LONG_I             = SC_type_register(SC_LONG_LONG_S,           KIND_INT,        B_T, sizeof(long long),            0);
/* 10 */
       SC_FLOAT_I                 = SC_type_register(SC_FLOAT_S,               KIND_FLOAT,      B_T, sizeof(float),                0);
       SC_DOUBLE_I                = SC_type_register(SC_DOUBLE_S,              KIND_FLOAT,      B_T, sizeof(double),               0);
       SC_LONG_DOUBLE_I           = SC_type_register(SC_LONG_DOUBLE_S,         KIND_FLOAT,      B_T, sizeof(long double),          0);
       SC_FLOAT_COMPLEX_I         = SC_type_register(SC_FLOAT_COMPLEX_S,       KIND_COMPLEX,    B_T, sizeof(float _Complex),       0);
       SC_DOUBLE_COMPLEX_I        = SC_type_register(SC_DOUBLE_COMPLEX_S,      KIND_COMPLEX,    B_T, sizeof(double _Complex),      0);
       SC_LONG_DOUBLE_COMPLEX_I   = SC_type_register(SC_LONG_DOUBLE_COMPLEX_S, KIND_COMPLEX,    B_T, sizeof(long double _Complex), 0);
       SC_QUATERNION_I            = SC_type_register(SC_QUATERNION_S,          KIND_QUATERNION, B_T, 4*sizeof(double),             0);

/* these must shadow SC_BIT_I thru SC_QUATERNION_I
 * so that SC_xxx_P_I = SC_xxx_I - SC_BIT_I + SC_POINTER_I
 * this make SC_type_ptr_id work
 */
       SC_POINTER_I               = SC_type_register(SC_POINTER_S,               KIND_POINTER, B_F, szptr, 0);
       SC_BOOL_P_I                = SC_type_register(SC_BOOL_P_S,                KIND_POINTER, B_F, szptr, 0);
       SC_STRING_I                = SC_type_register(SC_STRING_S,                KIND_POINTER, B_F, szptr, 0);
/* 20 */
       SC_WCHAR_P_I               = SC_type_register(SC_WCHAR_P_S,               KIND_POINTER, B_F, szptr, 0);
       SC_INT8_P_I                = SC_type_register(SC_INT8_P_S,                KIND_POINTER, B_F, szptr, 0);
       SC_SHORT_P_I               = SC_type_register(SC_SHORT_P_S,               KIND_POINTER, B_F, szptr, 0);
       SC_INT_P_I                 = SC_type_register(SC_INT_P_S,                 KIND_POINTER, B_F, szptr, 0);
       SC_LONG_P_I                = SC_type_register(SC_LONG_P_S,                KIND_POINTER, B_F, szptr, 0);
       SC_LONG_LONG_P_I           = SC_type_register(SC_LONG_LONG_P_S,           KIND_POINTER, B_F, szptr, 0);
       SC_FLOAT_P_I               = SC_type_register(SC_FLOAT_P_S,               KIND_POINTER, B_F, szptr, 0);
       SC_DOUBLE_P_I              = SC_type_register(SC_DOUBLE_P_S,              KIND_POINTER, B_F, szptr, 0);
       SC_LONG_DOUBLE_P_I         = SC_type_register(SC_LONG_DOUBLE_P_S,         KIND_POINTER, B_F, szptr, 0);
       SC_FLOAT_COMPLEX_P_I       = SC_type_register(SC_FLOAT_COMPLEX_P_S,       KIND_POINTER, B_F, szptr, 0);
/* 30 */
       SC_DOUBLE_COMPLEX_P_I      = SC_type_register(SC_DOUBLE_COMPLEX_P_S,      KIND_POINTER, B_F, szptr, 0);
       SC_LONG_DOUBLE_COMPLEX_P_I = SC_type_register(SC_LONG_DOUBLE_COMPLEX_P_S, KIND_POINTER, B_F, szptr, 0);
       SC_QUATERNION_P_I          = SC_type_register(SC_QUATERNION_P_S,          KIND_POINTER, B_F, szptr, 0);

       SC_VOID_I                  = SC_type_register(SC_VOID_S,     KIND_OTHER,   B_T, 0,               0);
       SC_STRUCT_I                = SC_type_register(SC_STRUCT_S,   KIND_STRUCT,  B_F, 0,               0);
       SC_FILE_I                  = SC_type_register(SC_FILE_S,     KIND_STRUCT,  B_F, sizeof(FILE),    0);
       SC_PCONS_P_I               = SC_type_register(SC_PCONS_P_S,  KIND_POINTER, B_F, szptr,           0);
/* 36 */

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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCOPE_TYPEH.H variables */

#define SC_UNKNOWN_I                 SC_gs.ltyp[0].i
#define SC_UNKNOWN_S                 SC_gs.ltyp[0].s
#define SC_BIT_I                     SC_gs.ltyp[1].i
#define SC_BIT_S                     SC_gs.ltyp[1].s

#define SC_BOOL_I                    SC_gs.ltyp[2].i
#define SC_BOOL_S                    SC_gs.ltyp[2].s
#define SC_BOOL_P_I                  SC_gs.ltyp[2].p_i
#define SC_BOOL_P_S                  SC_gs.ltyp[2].p_s

#define SC_CHAR_I                    SC_gs.ltyp[3].i
#define SC_CHAR_S                    SC_gs.ltyp[3].s
#define SC_STRING_I                  SC_gs.ltyp[3].p_i
#define SC_STRING_S                  SC_gs.ltyp[3].p_s
#define SC_WCHAR_I                   SC_gs.ltyp[4].i
#define SC_WCHAR_S                   SC_gs.ltyp[4].s
#define SC_WCHAR_P_I                 SC_gs.ltyp[4].p_i
#define SC_WCHAR_P_S                 SC_gs.ltyp[4].p_s

#define SC_SHORT_I                   SC_gs.ltyp[5].i
#define SC_SHORT_S                   SC_gs.ltyp[5].s
#define SC_SHORT_P_I                 SC_gs.ltyp[5].p_i
#define SC_SHORT_P_S                 SC_gs.ltyp[5].p_s
#define SC_INT_I                     SC_gs.ltyp[6].i
#define SC_INT_S                     SC_gs.ltyp[6].s
#define SC_INT_P_I                   SC_gs.ltyp[6].p_i
#define SC_INT_P_S                   SC_gs.ltyp[6].p_s
#define SC_LONG_I                    SC_gs.ltyp[7].i
#define SC_LONG_S                    SC_gs.ltyp[7].s
#define SC_LONG_P_I                  SC_gs.ltyp[7].p_i
#define SC_LONG_P_S                  SC_gs.ltyp[7].p_s
#define SC_LONG_LONG_I               SC_gs.ltyp[8].i
#define SC_LONG_LONG_S               SC_gs.ltyp[8].s
#define SC_LONG_LONG_P_I             SC_gs.ltyp[8].p_i
#define SC_LONG_LONG_P_S             SC_gs.ltyp[8].p_s

#define SC_INT8_I                    SC_gs.ltyp[9].i
#define SC_INT8_S                    SC_gs.ltyp[9].s
#define SC_INT8_P_I                  SC_gs.ltyp[9].p_i
#define SC_INT8_P_S                  SC_gs.ltyp[9].p_s
#define SC_INT16_I                   SC_gs.ltyp[10].i
#define SC_INT16_S                   SC_gs.ltyp[10].s
#define SC_INT16_P_I                 SC_gs.ltyp[10].p_i
#define SC_INT16_P_S                 SC_gs.ltyp[10].p_s
#define SC_INT32_I                   SC_gs.ltyp[11].i
#define SC_INT32_S                   SC_gs.ltyp[11].s
#define SC_INT32_P_I                 SC_gs.ltyp[11].p_i
#define SC_INT32_P_S                 SC_gs.ltyp[11].p_s
#define SC_INT64_I                   SC_gs.ltyp[12].i
#define SC_INT64_S                   SC_gs.ltyp[12].s
#define SC_INT64_P_I                 SC_gs.ltyp[12].p_i
#define SC_INT64_P_S                 SC_gs.ltyp[12].p_s

#define SC_FLOAT_I                   SC_gs.ltyp[13].i
#define SC_FLOAT_S                   SC_gs.ltyp[13].s
#define SC_FLOAT_P_I                 SC_gs.ltyp[13].p_i
#define SC_FLOAT_P_S                 SC_gs.ltyp[13].p_s
#define SC_DOUBLE_I                  SC_gs.ltyp[14].i
#define SC_DOUBLE_S                  SC_gs.ltyp[14].s
#define SC_DOUBLE_P_I                SC_gs.ltyp[14].p_i
#define SC_DOUBLE_P_S                SC_gs.ltyp[14].p_s
#define SC_LONG_DOUBLE_I             SC_gs.ltyp[15].i
#define SC_LONG_DOUBLE_S             SC_gs.ltyp[15].s
#define SC_LONG_DOUBLE_P_I           SC_gs.ltyp[15].p_i
#define SC_LONG_DOUBLE_P_S           SC_gs.ltyp[15].p_s

#define SC_FLOAT32_I                 SC_gs.ltyp[16].i
#define SC_FLOAT32_S                 SC_gs.ltyp[16].s
#define SC_FLOAT32_P_I               SC_gs.ltyp[16].p_i
#define SC_FLOAT32_P_S               SC_gs.ltyp[16].p_s
#define SC_FLOAT64_I                 SC_gs.ltyp[17].i
#define SC_FLOAT64_S                 SC_gs.ltyp[17].s
#define SC_FLOAT64_P_I               SC_gs.ltyp[17].p_i
#define SC_FLOAT64_P_S               SC_gs.ltyp[17].p_s
#define SC_FLOAT128_I                SC_gs.ltyp[18].i
#define SC_FLOAT128_S                SC_gs.ltyp[18].s
#define SC_FLOAT128_P_I              SC_gs.ltyp[18].p_i
#define SC_FLOAT128_P_S              SC_gs.ltyp[18].p_s

#define SC_FLOAT_COMPLEX_I           SC_gs.ltyp[19].i
#define SC_FLOAT_COMPLEX_S           SC_gs.ltyp[19].s
#define SC_FLOAT_COMPLEX_P_I         SC_gs.ltyp[19].p_i
#define SC_FLOAT_COMPLEX_P_S         SC_gs.ltyp[19].p_s
#define SC_DOUBLE_COMPLEX_I          SC_gs.ltyp[20].i
#define SC_DOUBLE_COMPLEX_S          SC_gs.ltyp[20].s
#define SC_DOUBLE_COMPLEX_P_I        SC_gs.ltyp[20].p_i
#define SC_DOUBLE_COMPLEX_P_S        SC_gs.ltyp[20].p_s
#define SC_LONG_DOUBLE_COMPLEX_I     SC_gs.ltyp[21].i
#define SC_LONG_DOUBLE_COMPLEX_S     SC_gs.ltyp[21].s
#define SC_LONG_DOUBLE_COMPLEX_P_I   SC_gs.ltyp[21].p_i
#define SC_LONG_DOUBLE_COMPLEX_P_S   SC_gs.ltyp[21].p_s

#define SC_COMPLEX32_I               SC_gs.ltyp[22].i
#define SC_COMPLEX32_S               SC_gs.ltyp[22].s
#define SC_COMPLEX32_P_I             SC_gs.ltyp[22].p_i
#define SC_COMPLEX32_P_S             SC_gs.ltyp[22].p_s
#define SC_COMPLEX64_I               SC_gs.ltyp[23].i
#define SC_COMPLEX64_S               SC_gs.ltyp[23].s
#define SC_COMPLEX64_P_I             SC_gs.ltyp[23].p_i
#define SC_COMPLEX64_P_S             SC_gs.ltyp[23].p_s
#define SC_COMPLEX128_I              SC_gs.ltyp[24].i
#define SC_COMPLEX128_S              SC_gs.ltyp[24].s
#define SC_COMPLEX128_P_I            SC_gs.ltyp[24].p_i
#define SC_COMPLEX128_P_S            SC_gs.ltyp[24].p_s

#define SC_QUATERNION_I              SC_gs.ltyp[25].i
#define SC_QUATERNION_S              SC_gs.ltyp[25].s
#define SC_QUATERNION_P_I            SC_gs.ltyp[25].p_i
#define SC_QUATERNION_P_S            SC_gs.ltyp[25].p_s

#define SC_VOID_I                    SC_gs.ltyp[26].i
#define SC_VOID_S                    SC_gs.ltyp[26].s
#define SC_POINTER_I                 SC_gs.ltyp[26].p_i
#define SC_POINTER_S                 SC_gs.ltyp[26].p_s

#define SC_ENUM_I                    SC_gs.ltyp[27].i
#define SC_ENUM_S                    SC_gs.ltyp[27].s
#define SC_STRUCT_I                  SC_gs.ltyp[28].i
#define SC_STRUCT_S                  SC_gs.ltyp[28].s

#define SC_FILE_I                    SC_gs.ltyp[29].i
#define SC_FILE_S                    SC_gs.ltyp[29].s
#define SC_INTEGER_I                 SC_gs.ltyp[30].i
#define SC_INTEGER_S                 SC_gs.ltyp[30].s

#define SC_REAL_I                    SC_gs.ltyp[31].i
#define SC_REAL_S                    SC_gs.ltyp[31].s
#define SC_REAL_P_I                  SC_gs.ltyp[31].p_i
#define SC_REAL_P_S                  SC_gs.ltyp[31].p_s

#define SC_TYP_N                     32




#define SC_PCONS_P_I                 SC_gs.lityp[0].i
#define SC_PCONS_P_S                 SC_gs.lityp[0].s
#define SC_ITYP_N                    1

/* these must have the same sequence/values as the dynamic values
 * assigned in SC_init_base_types
 * changes here MUST be reflected there
 */

#define _SC_DEF_TYP_                                                         \
 { { 0, "unknown", },                                                        \
   { 1, "bit", },                                                            \
   { 2, "bool",                  18, "bool *" },                             \
   { 3, "char",                  19, "char *" },                             \
   { 4, "wchar",                 20, "wchar *" },                            \
   { 6, "short",                 22, "short *" },                            \
   { 7, "int",                   23, "int *" },                              \
   { 8, "long",                  24, "long *" },                             \
   { 9, "long_long",             25, "long_long *" },                        \
   { 5, "int8_t",                21, "int8_t *" },                           \
   { 6, "int16_t",               22, "int16_t *" },                          \
   { 7, "int32_t",               23, "int32_t *" },                          \
   { 9, "int64_t",               25, "int64_t *" },                          \
   { 10, "float",                26, "float *" },                            \
   { 11, "double",               27, "double *" },                           \
   { 12, "long_double",          28, "long_double *" },                      \
   { 10, "float32_t",            26, "float32_t *" },                        \
   { 11, "float64_t",            27, "float64_t *" },                        \
   { 12, "float128_t",           28, "float128_t *" },                       \
   { 13, "float_complex",        29, "float_complex *" },                    \
   { 14, "double_complex",       30, "double_complex *" },                   \
   { 15, "long_double_complex",  31, "long_double_complex *" },              \
   { 13, "complex32_t",          29, "complex32_t *" },                      \
   { 14, "complex64_t",          30, "complex64_t *" },                      \
   { 15, "complex128_t",         31, "complex128_t *" },                     \
   { 16, "quaternion",           32, "quaternion *" },                       \
   { 33, "void",                 17, "void *" },                             \
   { 7,  "enum", },                                                          \
   { 34, "struct", },                                                        \
   { 35, "FILE", },                                                          \
   { 7, "integer", },                                                        \
   { 11, "double", } }

#define _SC_DEF_ITYP_                                                        \
 { { 39, "pcons *",  } }

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANG.C variables */


/* INIT_TYPES - initialize the type map */

static void init_types(int iref)
   {

    add_type(iref, "void",        "",            "",                 "NULL");
    add_type(iref, "bool",        "logical",     "SC_BOOL_I",        "FALSE");
    add_type(iref, "char",        "character",   "SC_CHAR_I",        "'\\0'");
    add_type(iref, "pboolean",    "integer",     "SC_INT_I",         "B_F");

/* fixed point types */
    add_type(iref, "short",       "integer(2)",  "SC_SHORT_I",       "0");
    add_type(iref, "int",         "integer",     "SC_INT_I",         "0");
    add_type(iref, "long",        "integer(8)",  "SC_LONG_I",        "0L");
    add_type(iref, "long long",   "integer(8)",  "SC_LONG_LONG_I",   "0LL");

    add_type(iref, "size_t",      "integer(8)",  "SC_LONG_I",        "0L");
    add_type(iref, "ssize_t",     "integer(8)",  "SC_LONG_I",        "0L");

/* fixed width fixed point types */
    add_type(iref, "int16_t",     "integer(2)",  "SC_INT16_I",       "0");
    add_type(iref, "int32_t",     "integer(4)",  "SC_INT32_I",       "0");
    add_type(iref, "int64_t",     "integer(8)",  "SC_INT64_I",       "0L");

/* floating point types */
    add_type(iref, "float",       "real(4)",     "SC_FLOAT_I",       "0.0");
    add_type(iref, "double",      "real(8)",     "SC_DOUBLE_I",      "0.0");
    add_type(iref, "long double", "real(16)",    "SC_LONG_DOUBLE_I", "0.0");

/* complex types */
    add_type(iref, "float _Complex",       "complex(4)",
	     "SC_FLOAT_COMPLEX_I", "0.0");
    add_type(iref, "double _Complex",      "complex(8)",
	     "SC_DOUBLE_COMPLEX_I", "0.0");
    add_type(iref, "long double _Complex", "complex(16)",
	     "SC_LONG_DOUBLE_COMPLEX_I", "0.0");

/* GOTCHA: there is a general issue with pointers and Fortran here
 * doing add_type on "void *" causes Fortran wrapper declarations
 * to be generated with "void *" in the arg list
 * if on the other hand we do not do an add_type on "void *" then
 * blang will generate Fortran wrapper declarations with "void **"
 * in the arg list
 * in some contexts we would rather have "void **" to accord with
 * the extra reference added by Fortran which is call by reference
 * by default
 * the same applies to all of these pointers and we have been
 * bitten by FILE and void in the tests
 * with "void *" defined pd_write_f works for real*8 a(10)
 * but fails for type(C_PTR) b
 */
    add_type(iref, "void *",        "C_PTR-A",      "SC_POINTER_I",       "NULL");
    add_type(iref, "bool *",        "logical-A",    "SC_BOOL_P_I",        "NULL");
    add_type(iref, "char *",        "character-A",  "SC_STRING_I",        "NULL");

    add_type(iref, "short *",       "integer(2)-A", "SC_SHORT_P_I",       "NULL");
    add_type(iref, "int *",         "integer-A",    "SC_INT_P_I",         "NULL");
    add_type(iref, "long *",        "integer(8)-A", "SC_LONG_P_I",        "NULL");
    add_type(iref, "long long *",   "integer(8)-A", "SC_LONG_LONG_P_I",   "NULL");

    add_type(iref, "float *",       "real(4)-A",    "SC_FLOAT_P_I",       "NULL");
    add_type(iref, "double *",      "real(8)-A",    "SC_DOUBLE_P_I",      "NULL");
    add_type(iref, "long double *", "real(16)-A",   "SC_LONG_DOUBLE_P_I", "NULL");

/* complex types */
    add_type(iref, "float _Complex *",       "complex(4)-A",
	     "SC_FLOAT_COMPLEX_P_I", "NULL");
    add_type(iref, "double _Complex *",      "complex(8)-A",
	     "SC_DOUBLE_COMPLEX_P_I", "NULL");
    add_type(iref, "long double _Complex *", "complex(16)-A",
	     "SC_LONG_DOUBLE_COMPLEX_P_I", "NULL");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PACPP.C variables */

/* PA_CPP_DEFAULT - install panacea cpp names into tables
 *                - must be called after PA_defun (defines units) and
 *                - PA_init_strings (defines units)
 */

void PA_cpp_default(void)
   {

    PA_gs.cpp_info        = CSTRDUP("INFO", 3);
    PA_gs.cpp_allocation  = CSTRDUP("ALLOCATION", 3);
    PA_gs.cpp_scope       = CSTRDUP("SCOPE", 3);
    PA_gs.cpp_class       = CSTRDUP("CLASS", 3);
    PA_gs.cpp_center      = CSTRDUP("CENTER", 3);
    PA_gs.cpp_persistence = CSTRDUP("PERSISTENCE", 3);
    PA_gs.cpp_units       = CSTRDUP("UNITS", 3);
    PA_gs.cpp_type        = CSTRDUP("TYPE", 3);
    PA_gs.cpp_type_SC     = CSTRDUP("TYPE_SC", 3);
    PA_gs.cpp_type_S      = CSTRDUP("TYPE_S", 3);

    PA_INFO_TYPE_S         = CSTRDUP("PA_INFO_TYPE", 3);
    PA_INFO_N_DIMS_S       = CSTRDUP("PA_INFO_N_DIMS", 3);
    PA_INFO_DIMS_S         = CSTRDUP("PA_INFO_DIMS", 3);
    PA_INFO_SCOPE_S        = CSTRDUP("PA_INFO_SCOPE", 3);
    PA_INFO_CLASS_S        = CSTRDUP("PA_INFO_CLASS", 3);
    PA_INFO_CENTER_S       = CSTRDUP("PA_INFO_CENTER", 3);
    PA_INFO_PERSISTENCE_S  = CSTRDUP("PA_INFO_PERSISTENCE", 3);
    PA_INFO_ALLOCATION_S   = CSTRDUP("PA_INFO_ALLOCATION", 3);
    PA_INFO_FILE_NAME_S    = CSTRDUP("PA_INFO_FILE_NAME", 3);
    PA_INFO_INIT_VAL_S     = CSTRDUP("PA_INFO_INIT_VAL", 3);
    PA_INFO_INIT_FNC_S     = CSTRDUP("PA_INFO_INIT_FNC", 3);
    PA_INFO_CONV_S         = CSTRDUP("PA_INFO_CONV", 3);
    PA_INFO_UNIT_S         = CSTRDUP("PA_INFO_UNIT", 3);
    PA_INFO_KEY_S          = CSTRDUP("PA_INFO_KEY", 3);
    PA_INFO_ATTRIBUTE_S    = CSTRDUP("PA_INFO_ATTRIBUTE", 3);
    PA_INFO_UNITS_S        = CSTRDUP("PA_INFO_UNITS", 3);
    PA_INFO_DATA_PTR_S     = CSTRDUP("PA_INFO_DATA_PTR", 3);
    PA_INFO_UNIT_NUMER_S   = CSTRDUP("PA_INFO_UNIT_NUMER", 3);
    PA_INFO_UNIT_DENOM_S   = CSTRDUP("PA_INFO_UNIT_DENOM", 3);
    PA_INFO_APPL_ATTR_S    = CSTRDUP("PA_INFO_APPL_ATTR", 3);
    PA_INFO_DEFAULT_S      = CSTRDUP("PA_INFO_DEFAULT", 3);
    PA_INFO_SHARE_S        = CSTRDUP("PA_INFO_SHARE", 3);
    PA_INFO_ATT_NAME_S     = CSTRDUP("PA_INFO_ATT_NAME", 3);
    PA_INFO_DIM_NAME_S     = CSTRDUP("PA_INFO_DIM_NAME", 3);
    PA_INFO_UNITS_NAME_S   = CSTRDUP("PA_INFO_UNITS_NAME", 3);
    PA_INFO_DOMAIN_NAME_S  = CSTRDUP("PA_INFO_DOMAIN_NAME", 3);
    PA_INFO_MAP_DOMAIN_S   = CSTRDUP("PA_INFO_MAP_DOMAIN", 3);
    PA_INFO_BUILD_DOMAIN_S = CSTRDUP("PA_INFO_BUILD_DOMAIN", 3);

    PA_cpp_add_group(PA_gs.cpp_info,        SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_allocation,  SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_scope,       SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_class,       SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_center,      SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_persistence, SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_units,       SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_type,        SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_type_SC,     SC_INT_I, NULL);
    PA_cpp_add_group(PA_gs.cpp_type_S,      SC_STRING_I,  NULL);

/* install types in several ways so that the following will work:
 *   #define SC_DOUBLE_I   6
 *   char SC_DOUBLE_S = "double";
 *
 *   itype = *(int *) PA_cpp_name_to_value("SC_DOUBLE_I");  returns  7
 *   itype = *(int *) PA_cpp_name_to_value("double");       returns 7
 *   type  = (char *) PA_cpp_name_to_value("SC_DOUBLE_S");  returns "double"
 */

/* install the actual types */
    PA_cpp_add_name(SC_CHAR_S,      PA_gs.cpp_type_SC, SC_CHAR_I);
    PA_cpp_add_name(SC_SHORT_S,     PA_gs.cpp_type_SC, SC_SHORT_I);
    PA_cpp_add_name(SC_INT_S,       PA_gs.cpp_type_SC, SC_INT_I);
    PA_cpp_add_name(SC_LONG_S,      PA_gs.cpp_type_SC, SC_LONG_I);
    PA_cpp_add_name(SC_FLOAT_S,     PA_gs.cpp_type_SC, SC_FLOAT_I);
    PA_cpp_add_name(SC_DOUBLE_S,    PA_gs.cpp_type_SC, SC_DOUBLE_I);
    PA_cpp_add_name(SC_STRING_S,    PA_gs.cpp_type_SC, SC_STRING_I);
    PA_cpp_add_name(SC_POINTER_S,   PA_gs.cpp_type_SC, SC_POINTER_I);
    PA_cpp_add_name(SC_PCONS_P_S,   PA_gs.cpp_type_SC, SC_PCONS_P_I);
    PA_cpp_add_name(SC_VOID_S,      PA_gs.cpp_type_SC, SC_VOID_I);
    PA_cpp_add_name(SC_SHORT_P_S,   PA_gs.cpp_type_SC, SC_SHORT_I);
    PA_cpp_add_name(SC_INT_P_S,     PA_gs.cpp_type_SC, SC_INT_P_I);
    PA_cpp_add_name(SC_LONG_P_S,    PA_gs.cpp_type_SC, SC_LONG_P_I);
    PA_cpp_add_name(SC_FLOAT_P_S,   PA_gs.cpp_type_SC, SC_FLOAT_P_I);
    PA_cpp_add_name(SC_DOUBLE_P_S,  PA_gs.cpp_type_SC, SC_DOUBLE_P_I);
    PA_cpp_add_name(SC_STRUCT_S,    PA_gs.cpp_type_SC, SC_STRUCT_I);
    PA_cpp_add_name(SC_UNKNOWN_S,   PA_gs.cpp_type_SC, SC_UNKNOWN_I);

    PA_cpp_add_name("SC_CHAR_I",      PA_gs.cpp_type, SC_CHAR_I);
    PA_cpp_add_name("SC_SHORT_I",     PA_gs.cpp_type, SC_SHORT_I);
    PA_cpp_add_name("SC_INT_I",       PA_gs.cpp_type, SC_INT_I);
    PA_cpp_add_name("SC_LONG_I",      PA_gs.cpp_type, SC_LONG_I);
    PA_cpp_add_name("SC_FLOAT_I",     PA_gs.cpp_type, SC_FLOAT_I);
    PA_cpp_add_name("SC_DOUBLE_I",    PA_gs.cpp_type, SC_DOUBLE_I);
    PA_cpp_add_name("SC_REAL_I",      PA_gs.cpp_type, SC_REAL_I);
    PA_cpp_add_name("SC_STRING_I",    PA_gs.cpp_type, SC_STRING_I);
    PA_cpp_add_name("SC_POINTER_I",   PA_gs.cpp_type, SC_POINTER_I);
    PA_cpp_add_name("SC_PCONS_P_I",   PA_gs.cpp_type, SC_PCONS_P_I);
    PA_cpp_add_name("SC_VOID_I",      PA_gs.cpp_type, SC_VOID_I);
    PA_cpp_add_name("SC_SHORT_P_I",   PA_gs.cpp_type, SC_SHORT_P_I);
    PA_cpp_add_name("SC_INT_P_I",     PA_gs.cpp_type, SC_INT_P_I);
    PA_cpp_add_name("SC_LONG_P_I",    PA_gs.cpp_type, SC_LONG_P_I);
    PA_cpp_add_name("SC_FLOAT_P_I",   PA_gs.cpp_type, SC_FLOAT_P_I);
    PA_cpp_add_name("SC_REAL_P_I",    PA_gs.cpp_type, SC_REAL_P_I);
    PA_cpp_add_name("SC_DOUBLE_P_I",  PA_gs.cpp_type, SC_DOUBLE_P_I);
    PA_cpp_add_name("SC_STRUCT_I",    PA_gs.cpp_type, SC_STRUCT_I);
    PA_cpp_add_name("SC_UNKNOWN_I",   PA_gs.cpp_type, SC_UNKNOWN_I);

/* install the types as strings */
    PA_cpp_add_name("SC_CHAR_S",      PA_gs.cpp_type_S, SC_CHAR_S);
    PA_cpp_add_name("SC_SHORT_S",     PA_gs.cpp_type_S, SC_SHORT_S);
    PA_cpp_add_name("SC_INT_S",       PA_gs.cpp_type_S, SC_INT_S);
    PA_cpp_add_name("SC_LONG_S",      PA_gs.cpp_type_S, SC_LONG_S);
    PA_cpp_add_name("SC_FLOAT_S",     PA_gs.cpp_type_S, SC_FLOAT_S);
    PA_cpp_add_name("SC_DOUBLE_S",    PA_gs.cpp_type_S, SC_DOUBLE_S);
    PA_cpp_add_name("SC_REAL_S",      PA_gs.cpp_type_S, SC_REAL_S);
    PA_cpp_add_name("SC_STRING_S",    PA_gs.cpp_type_S, SC_STRING_S);
    PA_cpp_add_name("SC_POINTER_S",   PA_gs.cpp_type_S, SC_POINTER_S);
    PA_cpp_add_name("SC_PCONS_P_S",   PA_gs.cpp_type_S, SC_PCONS_P_S);
    PA_cpp_add_name("SC_VOID_S",      PA_gs.cpp_type_S, SC_VOID_S);
    PA_cpp_add_name("SC_SHORT_P_S",   PA_gs.cpp_type_S, SC_SHORT_S);
    PA_cpp_add_name("SC_INT_P_S",     PA_gs.cpp_type_S, SC_INT_P_S);
    PA_cpp_add_name("SC_LONG_P_S",    PA_gs.cpp_type_S, SC_LONG_P_S);
    PA_cpp_add_name("SC_FLOAT_P_S",   PA_gs.cpp_type_S, SC_FLOAT_P_S);
    PA_cpp_add_name("SC_REAL_P_S",    PA_gs.cpp_type_S, SC_REAL_P_S);
    PA_cpp_add_name("SC_DOUBLE_P_S",  PA_gs.cpp_type_S, SC_DOUBLE_P_S);
    PA_cpp_add_name("SC_STRUCT_S",    PA_gs.cpp_type_S, SC_STRUCT_S);
    PA_cpp_add_name("SC_UNKNOWN_S",   PA_gs.cpp_type_S, SC_UNKNOWN_S);

    PA_cpp_add_name("PA_INFO_TYPE",         PA_gs.cpp_info, PA_INFO_TYPE);
    PA_cpp_add_name("PA_INFO_N_DIMS",       PA_gs.cpp_info, PA_INFO_N_DIMS);
    PA_cpp_add_name("PA_INFO_DIMS",         PA_gs.cpp_info, PA_INFO_DIMS);
    PA_cpp_add_name("PA_INFO_SCOPE",        PA_gs.cpp_info, PA_INFO_SCOPE);
    PA_cpp_add_name("PA_INFO_CLASS",        PA_gs.cpp_info, PA_INFO_CLASS);
    PA_cpp_add_name("PA_INFO_CENTER",       PA_gs.cpp_info, PA_INFO_CENTER);
    PA_cpp_add_name("PA_INFO_PERSISTENCE",  PA_gs.cpp_info, PA_INFO_PERSISTENCE);
    PA_cpp_add_name("PA_INFO_ALLOCATION",   PA_gs.cpp_info, PA_INFO_ALLOCATION);
    PA_cpp_add_name("PA_INFO_FILE_NAME",    PA_gs.cpp_info, PA_INFO_FILE_NAME);
    PA_cpp_add_name("PA_INFO_INIT_VAL",     PA_gs.cpp_info, PA_INFO_INIT_VAL);
    PA_cpp_add_name("PA_INFO_INIT_FNC",     PA_gs.cpp_info, PA_INFO_INIT_FNC);
    PA_cpp_add_name("PA_INFO_CONV",         PA_gs.cpp_info, PA_INFO_CONV);
    PA_cpp_add_name("PA_INFO_UNIT",         PA_gs.cpp_info, PA_INFO_UNIT);
    PA_cpp_add_name("PA_INFO_KEY",          PA_gs.cpp_info, PA_INFO_KEY);
    PA_cpp_add_name("PA_INFO_ATTRIBUTE",    PA_gs.cpp_info, PA_INFO_ATTRIBUTE);
    PA_cpp_add_name("PA_INFO_UNITS",        PA_gs.cpp_info, PA_INFO_UNITS);
    PA_cpp_add_name("PA_INFO_DATA_PTR",     PA_gs.cpp_info, PA_INFO_DATA_PTR);
    PA_cpp_add_name("PA_INFO_UNIT_NUMER",   PA_gs.cpp_info, PA_INFO_UNIT_NUMER);
    PA_cpp_add_name("PA_INFO_UNIT_DENOM",   PA_gs.cpp_info, PA_INFO_UNIT_DENOM);
    PA_cpp_add_name("PA_INFO_APPL_ATTR",    PA_gs.cpp_info, PA_INFO_APPL_ATTR);
    PA_cpp_add_name("PA_INFO_DEFAULT",      PA_gs.cpp_info, PA_INFO_DEFAULT);
    PA_cpp_add_name("PA_INFO_SHARE",        PA_gs.cpp_info, PA_INFO_SHARE);
    PA_cpp_add_name("PA_INFO_ATT_NAME",     PA_gs.cpp_info, PA_INFO_ATT_NAME);
    PA_cpp_add_name("PA_INFO_DIM_NAME",     PA_gs.cpp_info, PA_INFO_DIM_NAME);
    PA_cpp_add_name("PA_INFO_UNITS_NAME",   PA_gs.cpp_info, PA_INFO_UNITS_NAME);
    PA_cpp_add_name("PA_INFO_DOMAIN_NAME",  PA_gs.cpp_info, PA_INFO_DOMAIN_NAME);
    PA_cpp_add_name("PA_INFO_MAP_DOMAIN",   PA_gs.cpp_info, PA_INFO_MAP_DOMAIN);
    PA_cpp_add_name("PA_INFO_BUILD_DOMAIN", PA_gs.cpp_info, PA_INFO_BUILD_DOMAIN);

    PA_cpp_add_name("DEFN",    PA_gs.cpp_scope, DEFN);
    PA_cpp_add_name("RESTART", PA_gs.cpp_scope, RESTART);
    PA_cpp_add_name("DMND",    PA_gs.cpp_scope, DMND);
    PA_cpp_add_name("RUNTIME", PA_gs.cpp_scope, RUNTIME);
    PA_cpp_add_name("EDIT",    PA_gs.cpp_scope, EDIT);
    PA_cpp_add_name("SCRATCH", PA_gs.cpp_scope, SCRATCH);

    PA_cpp_add_name("REQU",    PA_gs.cpp_class, REQU);
    PA_cpp_add_name("OPTL",    PA_gs.cpp_class, OPTL);
    PA_cpp_add_name("DMND",    PA_gs.cpp_class, DMND);
    PA_cpp_add_name("PSEUDO",  PA_gs.cpp_class, PSEUDO);

    PA_cpp_add_name("U_CENT",  PA_gs.cpp_center, U_CENT);
    PA_cpp_add_name("N_CENT",  PA_gs.cpp_center, N_CENT);
    PA_cpp_add_name("E_CENT",  PA_gs.cpp_center, E_CENT);
    PA_cpp_add_name("F_CENT",  PA_gs.cpp_center, F_CENT);
    PA_cpp_add_name("Z_CENT",  PA_gs.cpp_center, Z_CENT);

    PA_cpp_add_name("REL",     PA_gs.cpp_persistence, REL);
    PA_cpp_add_name("KEEP",    PA_gs.cpp_persistence, KEEP);
    PA_cpp_add_name("CACHE_F", PA_gs.cpp_persistence, CACHE_F);
    PA_cpp_add_name("CACHE_R", PA_gs.cpp_persistence, CACHE_R);

    PA_cpp_add_name("STATIC",  PA_gs.cpp_allocation, STATIC);
    PA_cpp_add_name("DYNAMIC", PA_gs.cpp_allocation, DYNAMIC);

/* the units macros are actually references to variables,
 * If the variable were to change values, (which it shouldn't)
 * These names would be wrong.
 */
    PA_cpp_add_name("RAD",     PA_gs.cpp_units, RAD);   /* PA_gs.radian          */
    PA_cpp_add_name("STER",    PA_gs.cpp_units, STER);  /* PA_gs.steradian       */
    PA_cpp_add_name("MOLE",    PA_gs.cpp_units, MOLE);  /* PA_gs.mole            */
    PA_cpp_add_name("Q",       PA_gs.cpp_units, Q);     /* PA_gs.electric_charge */
    PA_cpp_add_name("CM",      PA_gs.cpp_units, CM);    /* PA_gs.cm              */
    PA_cpp_add_name("SEC",     PA_gs.cpp_units, SEC);   /* PA_gs.sec             */
    PA_cpp_add_name("G",       PA_gs.cpp_units, G);     /* PA_gs.gram            */
    PA_cpp_add_name("EV",      PA_gs.cpp_units, EV);    /* PA_gs.eV              */
    PA_cpp_add_name("K",       PA_gs.cpp_units, K);     /* PA_gs.kelvin          */
    PA_cpp_add_name("ERG",     PA_gs.cpp_units, ERG);   /* PA_gs.erg             */
    PA_cpp_add_name("CC",      PA_gs.cpp_units, CC);    /* PA_gs.cc              */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

#endif

