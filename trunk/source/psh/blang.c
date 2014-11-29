/*
 * BLANG.C - generate language bindings and documentation
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"

#define NO_DEFAULT_VALUE "----"

#define N_MODES  7

#define NF 3

enum e_cast_kind
   {CAST_NONE = 0, CAST_LENGTH, CAST_TYPE};

typedef enum e_cast_kind cast_kind;

enum e_doc_kind
   {DK_HTML = 0, DK_MAN};

typedef enum e_doc_kind doc_kind;

enum e_type_kind
   {TK_PRIMITIVE = 0, TK_ENUM, TK_STRUCT};

typedef enum e_type_kind type_kind;

enum e_fparam
   {FP_ANY = 0,
    FP_VARARG,
    FP_FNC,            /* pointer to function */
    FP_INDIRECT,       /* pointer to undefined type */
    FP_ARRAY,          /* pointer to defined type */
    FP_SCALAR};

typedef enum e_fparam fparam;

enum e_fdir
   {FD_NONE = -1, FD_IN, FD_OUT, FD_IN_OUT};

typedef enum e_fdir fdir;

typedef struct s_str_list str_list;
typedef struct s_statedes statedes;
typedef struct s_bindes bindes;
typedef struct s_typdes typdes;
typedef struct s_mbrdes mbrdes;
typedef struct s_der_list der_list;
typedef struct s_idecl idecl;
typedef struct s_fdecl fdecl;
typedef struct s_farg farg;

struct s_mbrdes
   {int is_fnc_ptr;
    cast_kind cast;
    char type[BFSML];
    char name[BFSML];
    char dim[BFSML];
    char cast_mbr[BFSML];};

struct s_typdes
   {char *c;
    char *f;
    char *gen;
    char *defv;};

struct s_str_list
   {int n;
    int nx;
    char **arr;};

struct s_idecl
   {char decl[BFLRG];    /* local variable declaration */
    char defa[BFLRG];    /* default value assignment */
    char argc[BFLRG];    /* argument to C function call */ 
    char argi[BFLRG];    /* argument to call to get value from arg list */ 
    char argn[BFLRG];};  /* arg names - for Python keyword list */

struct s_farg
   {int nv;                          /* number of default values specified */
    int arr;                         /* TRUE for array default values */
    pboolean cls;                    /* TRUE for class */
    fparam knd;
    fdir dir;                        /* data flow direction */
    char *arg;                       /* original C argument specification */
    char *qualifier;                 /* ARG qualifiers on argument */
    char type[BFLRG];                /* argument type */
    char name[BFLRG];                /* argument name */
    char **val;                      /* default value specifications */
    idecl interp;};

struct s_fdecl
   {int proc;                        /* TRUE iff processed */
    int na;                          /* number of arguments */
    int nr;                          /* number of return quantities */
    int nc;                          /* number of char * args */
    int ntf;                         /* number of (type,var) prototype pairs */
    int iref;                        /* index of reference binding desc - C */
    int voidf;                       /* TRUE for functions returning nothing */
    int voida;                       /* TRUE for functions with no args */
    int error;                       /* TRUE if declaration is incorrect */
    char **args;                     /* all args */
    char **cargs;                    /* char * args */
    char **tfproto;                  /* (type,var) prototype pairs */
    char **bindings;                 /* list of language bindings specified */
    bindes *ref;                     /* reference binding descriptor - C */
    farg proto;                      /* farg representation of declaration */
    farg *al;};

struct s_der_list
   {char *def;
    char **members;};

struct s_statedes
   {int nbi;                         /* number of bindings */
    int ndcl;                        /* number of declarations */
    int nen;                         /* number of enums */
    int nst;                         /* number of structs */
    int no[N_MODES];                 /* no generate flags */
    char *pck;                       /* name of package */
    char **sbi;
    char **cdc;
    char **cdv;
    char **cpr;
    char **fpr;
    char **fwr;
    fdecl *dcl;
    der_list *enums;
    der_list *structs;
    char path[BFLRG];                /* path for output files */
    str_list defv;};

struct s_bindes
   {int *iparam;
    statedes *st;
    FILE *fp[NF];
    str_list types;
    int (*cl)(statedes *st, bindes *bd, int c, char **v);
    void (*init)(statedes *st, bindes *bd);
    int (*bind)(bindes *bd);
    void (*doc)(FILE *fp, fdecl *dcl, doc_kind dk);
    void (*fin)(bindes *bd);};

static int
 nbd = 0;

static char
 *tykind[3] = {"primitive", "enum", "struct"};

static bindes
 gbd[N_MODES];

static char
 *lookup_type(char ***val, int *pit, char *ty, bindes *bo),
 **mc_proto_list(fdecl *dcl);

static void
 fc_type(fdecl *dcl, char *wty, int nc, farg *al, int afl, bindes *bo),
 cs_type(char *a, int nc, farg *arg, int drf);

extern int
 MODE_C;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BERR - report error for blang */

static void berr(char *fmt, ...)
   {char s[BFLRG];
    static int first = TRUE;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (first == TRUE)
       {first = FALSE;
	printf("\n");};

    printf("Error: %s\n", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_DERIVED - add S and SA to the derived type list LST */

static der_list *push_derived(der_list *lst, int *pnl, char *s, char **sa)
   {int ni, nb;
    static int nx = 10;

    ni = *pnl;
    if ((lst == NULL) || (ni == 0))
       lst = MAKE_N(der_list, nx);
	
    if ((ni % nx == 0) && (ni > 0))
       {nb = ni/nx;
	REMAKE(lst, der_list, (nb+1)*nx);};

    lst[ni].def     = STRSAVE(s);
    lst[ni].members = sa;

    ni++;

    *pnl = ni;

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STR_LST_ADD - add S to list SL */

int str_lst_add(str_list *sl, char *s)
   {int n, nx;
    char **sa;

    n  = sl->n;
    nx = sl->nx;
    sa = sl->arr;

    if (sa == NULL)
       {nx = 100;
        sa = MAKE_N(char *, nx);
	if (sa != NULL)
	   memset(sa, 0, nx*sizeof(char *));
        sl->nx  = nx;
        sl->arr = sa;};

    if (sa != NULL)
       {if (n >= nx - 10)
	   {nx += 100;
	    REMAKE(sa, char *, nx);
	    if (sa != NULL)
	       memset(sa+n, 0, (nx - n)*sizeof(char *));
	    sl->nx  = nx;
	    sl->arr = sa;};

	if (sa != NULL)
	   {if (s == NULL)
	       sa[n] = NULL;
	    else
	       sa[n] = STRSAVE(s);
            n++;};

        sl->n = n;};

    return(n);}

/*--------------------------------------------------------------------------*/

/*                           TYPE MAP ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* IS_FIXED_POINT - return TRUE if T is a fixed point type */

pboolean is_fixed_point(char *t)
   {pboolean rv;

    rv =  ((strcmp(t, "short") == 0)     ||
	   (strcmp(t, "int") == 0)       ||
	   (strcmp(t, "pboolean") == 0)  ||
	   (strcmp(t, "long") == 0)      ||
	   (strcmp(t, "long long") == 0) ||
	   (strcmp(t, "int16_t") == 0)   ||
	   (strcmp(t, "int32_t") == 0)   ||
	   (strcmp(t, "int64_t") == 0));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_REAL - return TRUE if T is a real floating point type */

pboolean is_real(char *t)
   {pboolean rv;

    rv = ((strcmp(t, "float") == 0) ||
	  (strcmp(t, "double") == 0) ||
	  (strcmp(t, "long double") == 0));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_COMPLEX - return TRUE if T is a complex floating point type */

pboolean is_complex(char *t)
   {pboolean rv;

    rv = ((strcmp(t, "float _Complex") == 0) ||
	  (strcmp(t, "double _Complex") == 0) ||
	  (strcmp(t, "long double _Complex") == 0));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_BOOL - return TRUE if T is a boolean type */

pboolean is_bool(char *t)
   {pboolean rv;

    rv = (strcmp(t, "bool") == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_CHAR - return TRUE if T is a char type */

pboolean is_char(char *t)
   {pboolean rv;

    rv = (strcmp(t, "char") == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_STRING - return TRUE if T is a char * type */

pboolean is_string(char *t)
   {pboolean rv;

    rv = (strcmp(t, "char *") == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_PTR - return TRUE if TYPE is a pointer */

static pboolean is_ptr(char *type)
   {pboolean rv;

    rv = (strchr(type, '*') != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_FUNC_PTR - return TRUE if TYPE is a function pointer */

static pboolean is_func_ptr(char *type, int wh)
   {pboolean rv;

    rv  = B_F;

    if ((rv == B_F) && (wh & 1))
       rv |= (strstr(type, "(*") != NULL);

    if ((rv == B_F) && (wh & 2))
       rv |= (strncmp(type, "PF", 2) == 0);

    if ((rv == B_F) && (wh & 4))
       rv |= (strstr(type, "PG_event_handler") != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_TYPE - add a type to the map */

static void add_type(int iref, char *cty, char *fty, char *gty, char *defv)
   {int i, j;

    i = iref;

/* add types for C mode */
    str_lst_add(&gbd[i].st->defv, defv);
    str_lst_add(&gbd[i].types, cty);
    i++;

/* add types for 2 Fortran modes - modules and wrappers */
    for (j = 0; j < 2; j++)
        str_lst_add(&gbd[i++].types, fty);

/* add types for remaining modes */
    for (; i < nbd; i++)
        str_lst_add(&gbd[i].types, gty);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_TYPE - add a type to the map */

static void add_type_(int iref, typdes *td)
   {int i, j;
    char *cty, *fty, *gty, *defv;

    cty  = td->c;
    fty  = td->f;
    gty  = td->gen;
    defv = td->defv;

    i = iref;

/* add types for C mode */
    str_lst_add(&gbd[i].st->defv, defv);
    str_lst_add(&gbd[i].types, cty);
    i++;

/* add types for 2 Fortran modes - modules and wrappers */
    for (j = 0; j < 2; j++)
        str_lst_add(&gbd[i++].types, fty);

/* add types for remaining modes */
    for (; i < nbd; i++)
        str_lst_add(&gbd[i].types, gty);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOOKUP_TYPE - lookup and return a type from the type lists */

static char *lookup_type(char ***val, int *pit, char *ty, bindes *bo)
   {int i, l;
    char *rv, *dv, **lst, **ta;
    bindes *bi;

    rv = NULL;

    bi = gbd + MODE_C;

    if (bo == NULL)
       bo = bi;

    l = -1;

    ta = bi->types.arr;
    for (i = 0; ta[i] != NULL; i++)
        {if (strcmp(ty, ta[i]) == 0)
	    {l = i;
	     break;};};

    dv = NO_DEFAULT_VALUE;
    if (l != -1)
       {dv = gbd[0].st->defv.arr[l];
	rv = bo->types.arr[l];}

    else if ((is_func_ptr(ty, 3) == B_T) || (is_ptr(ty) == B_T))
       dv = "NULL";

    if (val != NULL)
       {lst = NULL;
	if (dv != NULL)
	   lst = lst_push(lst, dv);
	*val = lst;};

    if (pit != NULL)
       *pit = l;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

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

    add_type(iref, "pcons",         "C_PTR-A",      "SC_PCONS_I",         "NULL");
    add_type(iref, "pcons *",       "C_PTR-A",      "SC_PCONS_P_I",       "NULL");
/*
    add_type(iref, "FILE *",        "C_PTR-A",      "SC_FILE_I",          "NULL");
 */
    add_type(iref, "PROCESS *",     "C_PTR-A",      "SC_PROCESS_I",       "NULL");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_DERIVED_TYPES - add derived types */

static void add_derived_types(int iref, char **sbi)
   {int ib;
    char s[BFLRG];
    char *sb, **ta;
    typdes td;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {if (strncmp(sb, "derived ", 8) == 0)
		{nstrncpy(s, BFLRG, sb, -1);
		 ta   = tokenize(s, " \t", 0);
		 td.c    = ta[1];
		 td.f    = ta[2];
		 td.gen  = ta[3];
		 td.defv = ta[4];}
	     else if (strncmp(sb, "enum ", 5) == 0)
	        {nstrncpy(s, BFLRG, sb, -1);
		 ta   = tokenize(s, " \t", 0);
		 td.c    = ta[1];
		 td.f    = "integer";
		 td.gen  = tykind[TK_ENUM];
		 td.defv = ta[2];}
	     else if (strncmp(sb, "struct ", 7) == 0)
	        {nstrncpy(s, BFLRG, sb, -1);
		 ta   = tokenize(s, " \t", 0);
		 td.c    = ta[1];
		 td.f    = tykind[TK_STRUCT];
		 td.gen  = tykind[TK_STRUCT];
		 td.defv = NULL;}
	     else
	        ta = NULL;
	
	     if (ta != NULL)
	        {add_type_(iref, &td);
		 FREE(ta[0]);
		 FREE(ta);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEREF - dereference a pointer S
 *       - place result in D
 *       - return TRUE iff a pointer was dereferenced
 */

static int deref(char *d, int nc, char *s)
   {int rv;

    rv = FALSE;

    nstrncpy(d, nc, s, -1);
    if (LAST_CHAR(d) == '*')
       {LAST_CHAR(d) = '\0';
	memmove(d, trim(d, BOTH, " \t"), nc);
        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IDEREF - dereference S with SC_FOO_P_I to SC_FOO_I
 *        - in place
 *        - return TRUE iff a pointer was dereferenced
 */

static int ideref(char *s)
   {int rv;
    char *p;

    rv = FALSE;

    p = strstr(s, "_P_I");
    if (p != NULL)
       {memmove(p, p+2, 3);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           INFRASTRUCTURE ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* GET_DEF_VALUE - get a proper default value VL appropriate to type TY
 *               - from the value specification SP
 *               - proper means legal in C syntax
 *               - the space VL is NC long
 */

static void get_def_value(char *lvl, int nc, char *sp, char *ty)
   {char lty[BFLRG];
    char **dv;

    if (sp == NULL)
       lvl[0] = '\0';
    else
       nstrncpy(lvl, nc, sp, -1);

/* if no value specified */
    dv = NULL;
    if ((IS_NULL(lvl) == TRUE) || (strcmp(lvl, NO_DEFAULT_VALUE) == 0))
       {if ((is_ptr(ty) == B_T) || (is_func_ptr(ty, 3) == B_T))
	   nstrncpy(lvl, nc, "NULL", -1);
	else
	   lookup_type(&dv, NULL, ty, NULL);}

/* dereference a pointer type and get its default value */
    else if (lvl[0] == '*')
       {if (is_ptr(ty) == B_T)
	   {deref(lty, BFLRG, ty);
	    lookup_type(&dv, NULL, lty, NULL);}
	else
	   lookup_type(&dv, NULL, ty, NULL);};

    if (dv != NULL)
       {nstrncpy(lvl, nc, dv[0], -1);
	lst_free(dv);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_CLASS_ARG - return the argument of DCL that specifies a class
 *               - return NULL if there is no class arg
 */

static farg *get_class_arg(fdecl *dcl)
   {int i, na;
    farg *al, *rv;

    na = dcl->na;
    al = dcl->al;
    rv = NULL;

    for (i = 0; (i < na) && (rv == NULL); i++)
        {if (al[i].cls == B_T)
	    rv = al + i;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_VAR_ARG - return TRUE iff the function described by PR
 *            - has a variable arg list
 */

static int is_var_arg(char *pr)
   {int rv;

    rv = (strstr(pr, "...") != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_ARG - return the next argument from the list ARGS */

static char *get_arg(char *args)
   {int c, np, ok;
    char *pa;

    ok = FALSE;
    np = 0;
    pa = args;

    for (c = *pa++; (c != '\0') && (ok == FALSE); c = *pa++)
        {switch (c)
	    {case ',' :
	          ok = (np == 0);
	          break;
	     case '(' :
	     case '[' :
	          np++;
	          break;
	     case ')' :
	     case ']' :
	          ok = (np == 0);
	          np--;
	          break;
	     default :
	          break;};};

    if ((c != '\0') && (pa > args))
       pa[-2] = '\0';

    return(args);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLIT_ARGS - split an arg list into individual args */

static char **split_args(char *args)
   {char *arg, *pa, **al;

    al = NULL;
    for (pa = args; IS_NULL(pa) == FALSE; )
        {arg = get_arg(pa);
	 pa += strlen(arg) + 1;
	 al  = lst_push(al, trim(arg, FRONT, " \t"));};

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_NO_DEFAULT - handle intepreter declarations in case
 *               - no defaults specified
 */

static void id_no_default(idecl *ip, char *ty, char *lty, char *nm, char *val)
   {char lvl[BFLRG];

    get_def_value(lvl, BFLRG, val, ty);

    snprintf(ip->decl, BFLRG, "%s _l%s;\n", ty, nm);
    snprintf(ip->defa, BFLRG, "    _l%-8s = %s;\n", nm, lvl);
    snprintf(ip->argi, BFLRG, "            %s, &_l%s,\n", lty, nm);
    snprintf(ip->argn, BFLRG, "\"%s\", ", nm);
    snprintf(ip->argc, BFLRG, "_l%s", nm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_FD_OUT - handle intepreter declarations in case
 *           - of output arguments
 */

static void id_fd_out(idecl *ip, char *ty, char *nm, int nvl, char **vls)
   {int drf;
    char dty[BFLRG], vl[BFLRG];

    drf = deref(dty, BFLRG, ty);

    if (drf == TRUE)
       snprintf(ip->argc, BFLRG, "_l%s", nm);
    else
       snprintf(ip->argc, BFLRG, "&_l%s", nm);

    if (nvl == 1)
       {if (drf == TRUE)
	   {if (strcmp(vls[0], "NULL") == 0)
	       {snprintf(ip->decl, BFLRG, "%s _l%s;\n", ty, nm);
		snprintf(ip->defa, BFLRG, "    _l%-8s = NULL;\n", nm);}
	    else if (vls[0][0] == '*')
	       {get_def_value(vl, BFLRG, vls[0], dty);
		snprintf(ip->decl, BFLRG, "%s _l%s;\n", dty, nm);
		snprintf(ip->defa, BFLRG, "    _l%-8s = %s;\n", nm, vl);
		snprintf(ip->argc, BFLRG, "&_l%s", nm);}
	    else
	       {snprintf(ip->decl, BFLRG, "%s _l%s;\n", dty, nm);
		snprintf(ip->defa, BFLRG, "    _l%-8s = %s;\n", nm, vls[0]);
		snprintf(ip->argc, BFLRG, "&_l%s", nm);};}
	else
	   snprintf(ip->decl, BFLRG, "%s _l%s;\n", dty, nm);}
    else
       snprintf(ip->decl, BFLRG, "%s _l%s[%d];\n", dty, nm, nvl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_FD_IN_OUT - handle intepreter declarations in case
 *              - of input/output arguments
 */

static void id_fd_in_out(idecl *ip, char *ty, char *lty,
			 char *nm, int nvl, char **vls)
   {int l;
    char dty[BFLRG], lvl[BFLRG];

    deref(dty, BFLRG, ty);
    ideref(lty);

    if (nvl == 1)
       {get_def_value(lvl, BFLRG, NULL, dty);
	snprintf(ip->decl, BFLRG, "%s _l%s;\n", dty, nm);
	snprintf(ip->defa, BFLRG, "    _l%-8s = %s;\n", nm, lvl);
	snprintf(ip->argi, BFLRG, "            %s, &_l%s,\n", lty, nm);
	snprintf(ip->argn, BFLRG, "\"%s\", ", nm);
	snprintf(ip->argc, BFLRG, "&_l%s", nm);}

    else
       {snprintf(ip->decl, BFLRG, "%s _l%s[%d];\n", dty, nm, nvl);
	for (l = 0; l < nvl; l++)
	    {get_def_value(lvl, BFLRG, vls[l], dty);
	     vstrcat(ip->defa, BFLRG, "    _l%s[%d] = %s;\n", nm, l, lvl);
	     vstrcat(ip->argi, BFLRG, "            %s, &_l%s[%d],\n",
		     lty, nm, l);
	     vstrcat(ip->argn, BFLRG, "\"%s%d\", ", nm, l);};

	snprintf(ip->argc, BFLRG, "_l%s", nm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_FD_IN - handle intepreter declarations in case
 *          - of input arguments
 */

static void id_fd_in(idecl *ip, char *ty, char *lty,
		     char *nm, int arr, int nvl, char **vls)
   {int l, drf, strp, sarr;
    char dty[BFLRG], lvl[BFLRG];

    drf = deref(dty, BFLRG, ty);
    ideref(lty);

    strp = (strcmp(lty, "SC_STRING_I") == 0);
    sarr = ((arr == TRUE) && (strp == TRUE) && (strcmp(ty, "char *") != 0));
    if ((nvl == 1) && (sarr == FALSE))
       {get_def_value(lvl, BFLRG, vls[0], dty);

	snprintf(ip->defa, BFLRG, "    _l%-8s = %s;\n", nm, lvl);
	snprintf(ip->argi, BFLRG, "            %s, &_l%s,\n", lty, nm);
	snprintf(ip->argn, BFLRG, "\"%s\", ", nm);

/* GOTCHA: this clause is certainly wrong for
 *    char *s ARG("a b c",in)
 * on the other hand it seems right for
 *    int *b ARG([1],in)
 * difference looks like "a b c" is formally a scalar string
 * while [1] is formally an array of one element
 */
	if ((drf == TRUE) && (strp == FALSE) &&
	    ((is_ptr(ty) == FALSE) || (strcmp(vls[0], "NULL") != 0)))
	   {snprintf(ip->decl, BFLRG, "%s _l%s;\n", dty, nm);
	    snprintf(ip->argc, BFLRG, "&_l%s", nm);}
	else
	   {snprintf(ip->decl, BFLRG, "%s _l%s;\n", ty, nm);
	    snprintf(ip->argc, BFLRG, "_l%s", nm);};}

/* multiple default values means a local array */
    else
       {snprintf(ip->decl, BFLRG, "%s _l%s[%d];\n", dty, nm, nvl);

	for (l = 0; l < nvl; l++)
	    {get_def_value(lvl, BFLRG, vls[l], ty);
	     vstrcat(ip->defa, BFLRG, "    _l%s[%d] = %s;\n", nm, l, lvl);
	     vstrcat(ip->argi, BFLRG, "            %s, &_l%s[%d],\n",
		     lty, nm, l);
	     vstrcat(ip->argn, BFLRG, "\"%s%d\", ", nm, l);};

	snprintf(ip->argc, BFLRG, "_l%s", nm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_INTERP_DEF - setup correct expressions for interpreter
 *                    - handling of defaults
 */

static void process_interp_def(farg *al)
   {int nvl, arr;
    fdir dir;
    char s[BFLRG], lty[BFLRG], lvl[BFLRG];
    char oexp[BFLRG], nexp[BFLRG];
    char *ty, *nm, *arg, *val, **vls;
    idecl *ip;
    static int count = 0;
	
    ip  = &al->interp;
    ty  = al->type;
    nm  = al->name;
    dir = al->dir;
    arg = al->arg;
    arr = al->arr;
    vls = al->val;
    nvl = al->nv;

    ip->decl[0] = '\0';
    ip->defa[0] = '\0';
    ip->argi[0] = '\0';
    ip->argn[0] = '\0';
    ip->argc[0] = '\0';

    val = (nvl > 0) ? vls[0] : NO_DEFAULT_VALUE;

    cs_type(lty, BFLRG, al, FALSE);

/* function pointer */
    if (is_func_ptr(arg, 1) == B_T)
       {get_def_value(lvl, BFLRG, val, ty);
	snprintf(oexp, BFLRG, "(*%s)", nm);
	snprintf(nexp, BFLRG, "(*_l%s)", nm);
	nstrncpy(s, BFLRG, arg, -1);
	snprintf(ip->decl, BFLRG, "%s;\n", subst(s, oexp, nexp, 1));
	snprintf(ip->defa, BFLRG, "    _l%-8s = %s;\n", nm, lvl);
	snprintf(ip->argi, BFLRG, "            %s, &_l%s,\n", lty, nm);
	snprintf(ip->argn, BFLRG, "\"%s\", ", nm);
	snprintf(ip->argc, BFLRG, "_l%s", nm);}

    else if (nvl == 0)
       id_no_default(ip, ty, lty, nm, val);

/* single default values means a local scalar for a C pointer type */
    else
       {switch (dir)
	   {case FD_OUT :
	         id_fd_out(ip, ty, nm, nvl, vls);
		 break;

	    case FD_IN_OUT :
	         id_fd_in_out(ip, ty, lty, nm, nvl, vls);
		 break;

	    case FD_IN :
	         id_fd_in(ip, ty, lty, nm, arr, nvl, vls);
		 break;

	    default :
	         id_no_default(ip, ty, lty, nm, val);
		 break;};};

    count++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_QUALIFIERS - process argument qualifiers
 *                    - return TRUE if successful
 */

static int process_qualifiers(farg *al, char *qual)
   {int rv, arr, ptr;
    char t[BFLRG];
    char *val, *dir, **lst, **ta;

    rv = TRUE;

    if (IS_NULL(qual) == FALSE)
       {nstrncpy(t, BFLRG, qual+4, -1);
	LAST_CHAR(t) = '\0';

	ta = split_args(t);
	if (ta != NULL)
	   {ptr = is_ptr(al->type);

/* get default value(s)
 *    <sspec> := "*" | <val>
 *    <spec>  := "" | <sspec>
 *    <list>  := <sspec> | <list> ',' <sspec>
 *    <specs> := <spec> | '[' <list> ']'
 *    ""       - no value specification
 *    "*"      - any single value
 *    <val>    - single value <val> 
 * NOTE: to make parsing easier things like [,] are not allowed
 * hence the <spec> <sspec> distinction
 */
	    val = ta[0];
	    lst = NULL;
	    arr = FALSE;
	    if ((IS_NULL(val) == TRUE) || (val[0] == '*'))
	       lookup_type(&lst, NULL, al->type, NULL);
	    else
	       {arr = (strchr(val, '[') != NULL);
		lst = tokenize(val, "[,]", 0);};

	    al->arr = arr;
	    al->val = lst;
	    al->nv  = lst_length(lst);

	    if ((al->nv > 1) && (ptr == FALSE))
	       {berr("multiple values illegal with scalar '%s %s'",
		     al->arg, al->qualifier);
		al->nv = 0;
		rv = FALSE;};

/* get argument IN/OUT direction */
	    al->dir = FD_IN;
	    dir = ta[1];
	    if (dir != NULL)
	       {if (strcmp(dir, "in") == 0)
		   al->dir = FD_IN;
		else if (strcmp(dir, "out") == 0)
		   al->dir = FD_OUT;
		else if (strcmp(dir, "io") == 0)
		   al->dir = FD_IN_OUT;};

	    if (((al->dir == FD_OUT) || (al->dir == FD_IN_OUT)) &&
		 (ptr == FALSE))
	       {berr("scalar arguments cannot be IO or OUT - '%s %s'",
		     al->arg, al->qualifier);
		al->nv = 0;
		rv = FALSE;};

/* get kind adjustment */
	    al->cls = ((IS_NULL(ta[2]) == B_F) && (strcmp(ta[2], "cls") == 0));

	    free_strings(ta);}
	else
	   {lookup_type(&al->val, NULL, al->type, NULL);
	    al->dir = FD_NONE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLIT_DECL - split a declaration S of the form
 *            -    <type> ['*']* <name> [ARG([spec,]* | [spec])]
 *            - into proper type and name
 *            - return TRUE iff successful
 */

static int split_decl(farg *al, char *s, int isarg)
   {int nc, rv;
    char t[BFLRG];
    char *p, *pn, *qual;

    nstrncpy(t, BFLRG, s, -1);

/* look for ARG qualifiers */
    qual = strstr(t, "ARG(");
    if (qual != NULL)
       qual[-1] = '\0';

    al->arg       = STRSAVE(t);
    al->qualifier = (qual == NULL) ? qual : STRSAVE(qual);
    al->nv        = 0;
    al->knd       = FP_ANY;
    al->dir       = FD_NONE;
    al->val       = NULL;

    p = trim(strtok(t, "),"), BOTH, " \t");

    if (strcmp(p, "void") == 0)
       pn = p + 4;
    else if (is_var_arg(p) == TRUE)
       pn = p + 3;
    else
       {nc = strlen(p);
	for (pn = p + nc - 1; pn > p; pn--)
	    {if (strchr(" *\t", *pn) != NULL)
	        break;};
	if ((pn > p) && (pn[-1] == '('))
	   pn--;
	else
	   pn++;};

/* handle function pointer case which would be like '(*f' here */
    if (*pn == '(')
       {nstrncpy(al->name, BFLRG, pn+2, -1);
	*pn = '\0';
	nstrncpy(al->type, BFLRG, "PFInt", -1);

	rv = TRUE;}

    else if (is_func_ptr(p, 2) == B_T)
       {nstrncpy(al->name, BFLRG, pn, -1);
	pn[-1] = '\0';
	nstrncpy(al->type, BFLRG, p, -1);

	rv = TRUE;}

/* handle other args */
    else
       {nstrncpy(al->name, BFLRG, pn, -1);
	*pn = '\0';
	nstrncpy(al->type, BFLRG, trim(p, BACK, " \t"), -1);

	rv = FALSE;};

    rv = process_qualifiers(al, qual);

/* setup correct default values for interpreters */
    if ((rv == TRUE) && (isarg == TRUE))
       process_interp_def(al);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROC_ARGS - process the arg list in the declaration DCL */

static farg *proc_args(fdecl *dcl)
   {int i, na, nr, ok, err;
    char **args;
    farg *al;
    bindes *bd;

    na   = dcl->na;
    args = dcl->args;
    bd   = dcl->ref;

    al = MAKE_N(farg, na);
    if (al != NULL)
       {memset(al, 0, na*sizeof(farg));

	err = FALSE;
	nr  = 0;
	for (i = 0; i < na; i++)
	    {ok = split_decl(al+i, args[i], TRUE);
	     if (ok == FALSE)
	        {FREE(al);
		 free_strings(dcl->bindings);
		 dcl->bindings = NULL;
		 dcl->na       = 0;
		 err           = TRUE;
		 break;}
	     else
	        {fc_type(dcl, NULL, 0, al+i, TRUE, bd);
		 nr += al[i].nv;};};

	dcl->error = err;
	dcl->al    = al;
	dcl->nr    = nr;
	dcl->voida = ((dcl->na == 1) && (IS_NULL(dcl->al[0].name) == TRUE));

/* fill in the (type,var) prototype token pairs */
	mc_proto_list(dcl);};

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_FUNC - return true if function named in prototype PR matches F */

static int find_func(char *pr, char *f)
   {int rv;
    char t[BFLRG];
    char *p;

    snprintf(t, BFLRG, "%s(", f);
    p  = strstr(pr, t);
    rv = ((p != NULL) && (strchr(" *\t", p[-1]) != NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_PROTO - find the prototype for F in CPR
 *            - return it iff successful
 */

static int find_proto(fdecl *dcl, char **cpr, char *f)
   {int ip, na, nt, ok;
    char pf[BFLRG];
    char *p, *pa, *sp, *pro, *cfn;
    char **ty, **args;

    ok  = FALSE;
    pro = NULL;

    for (ip = 0; cpr[ip] != NULL; ip++)
        {sp = cpr[ip];
	 if (blank_line(sp) == FALSE)
	    {if (find_func(sp, f) == TRUE)
	        {pro = sp;
		 break;};};};

    if ((strcmp(f, "derived") == 0) ||
	(strcmp(f, tykind[TK_ENUM]) == 0) ||
	(strcmp(f, tykind[TK_STRUCT]) == 0))
       dcl = NULL;

    else if (pro == NULL)
       {berr("no binding for '%s'", f);
	dcl = NULL;}

    else
       {if (dcl == NULL)
	   {dcl = MAKE(fdecl);
	    memset(dcl, 0, sizeof(fdecl));};

	if (dcl != NULL)
	   {dcl->proc  = FALSE;
	    dcl->na    = 0;
	    dcl->args  = NULL;
	    dcl->nc    = 0;
	    dcl->cargs = NULL;

/* break up the prototype into the type/function name and args */
	    nstrncpy(pf, BFLRG, pro, -1);
	    p = strchr(pf, '(');
            if (p != NULL)
	       *p++ = '\0';
	    pa = p;
	    if (pa != NULL)
	       LAST_CHAR(pa) = '\0';

/* get the return type */
	    ok = split_decl(&dcl->proto, pf, FALSE);
	
	    ty = tokenize(pf, " \t", 0);
	    if (ty != NULL)
	       {nt = lst_length(ty);

		FREE(dcl->proto.arg);
		dcl->proto.arg = STRSAVE(pro);

/* get the function name */
		cfn = ty[nt-1];
		ASSERT(cfn != NULL);

/* get the args */
		args = split_args(pa);
		if (args != NULL)
		   for (na = 0; IS_NULL(args[na]) == FALSE; na++);
		else
		   na = 0;

		dcl->na    = na;
		dcl->args  = args;
		dcl->voida = TRUE;
		dcl->voidf = (strcmp(dcl->proto.type, "void") == 0);

		proc_args(dcl);

		free_strings(ty);};};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSEP - print C comment separator */

static void csep(FILE *fp)
   {

    fprintf(fp, "/*--------------------------------------------------------------------------*/\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HSEP - print HTML comment separator */

static void hsep(FILE *fp)
   {

    fprintf(fp, "<!-- -------------------------------------------------------------------- -->\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_MEMBER - parse a member description, MBR, into type, TY,
 *              - name, NM, and dimensions, DM
 *              - each of these is NC long if non-NULL
 */

void parse_member(mbrdes *md, char *mbr)
   {int im, ns, hd;
    char s[BFLRG], ind[BFSML];
    char **sa, *pn, *pt;

    memset(md, 0, sizeof(mbrdes));

    hd = (strchr(mbr, '[') != NULL);
    im = 0;

    nstrncpy(s, BFLRG, mbr, -1);
    if (strstr(s, "(*") != NULL)
       {md->is_fnc_ptr = TRUE;
	sa = tokenize(s, "()[]", 0);
	ns = 0;
	pt = sa[im++];
	pn = sa[im++] + 1;}
    else if (strncmp(s, "PF", 2) == 0)
       {md->is_fnc_ptr = TRUE;
	sa = tokenize(s, " \t", 0);
	ns = 0;
	pt = sa[im++];
	pn = sa[im++];}
    else if (strchr(s, '*') != NULL)
       {sa = tokenize(s, " \t[]", 0);
	pt = sa[im++];
	pn = sa[im++];
	ns = strspn(pn, "*");
	pn += ns;}
    else
       {sa = tokenize(s, " \t[]", 0);
	ns = 0;
	pt = sa[im++];
	pn = sa[im++];};

/* type */
    if (ns > 0)
       {memset(ind, '*', ns);
	ind[ns] = '\0';
	snprintf(md->type, BFSML, "%s %s", pt, ind);}
    else
       nstrncpy(md->type, BFSML, pt, -1);

/* name */
    nstrncpy(md->name, BFSML, pn, -1);

/* dimensions */
    if (hd == TRUE)
       nstrncpy(md->dim, BFSML, sa[im++], -1);

/* cast */
    if ((IS_NULL(sa[im]) == FALSE) && (strncmp(sa[im], "MBR(", 4) == 0))
       {char **ta;

	ta = tokenize(sa[im], " \t(,)", 0);
        md->cast = ((strcmp(ta[1], "type") == 0) ? CAST_TYPE : CAST_LENGTH);
	nstrncpy(md->cast_mbr, BFSML, trim(sa[3], BOTH, "()"), -1);
	lst_free(ta);};

    lst_free(sa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EMIT_ENUM_DEFS - emit enum specifications via FEMIT */

void emit_enum_defs(bindes *bd,
		    void (*femit)(FILE **fp, char *dv, char **ta,
				  char *pck, int ni))
   {int ie, ne;
    char *pck, *sb, **ta;
    der_list *el;
    FILE **fp;
    statedes *st;

    st  = bd->st;
    fp  = bd->fp;
    pck = st->pck;
    ne  = st->nen;
    el  = st->enums;

    femit(fp, "begin", NULL, pck, ne);

    for (ie = 0; ie < ne; ie++)
        {sb = el[ie].def;
	 ta = el[ie].members;
	 femit(fp, sb, ta, pck, -1);};
	    
    femit(fp, "end", NULL, pck, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EMIT_STRUCT_DEFS - emit struct specifications via FEMIT */

void emit_struct_defs(bindes *bd,
		      void (*femit)(FILE **fp, char *dv, char **ta,
				    char *pck, int ni))
   {int is, ns;
    char *pck, *sb, **ta;
    der_list *sl;
    FILE **fp;
    statedes *st;

    st  = bd->st;
    fp  = bd->fp;
    pck = st->pck;
    ns  = st->nst;
    sl  = st->structs;

    femit(fp, "begin", NULL, pck, ns);

    for (is = 0; is < ns; is++)
        {sb = sl[is].def;
	 ta = sl[is].members;
	 femit(fp, sb, ta, pck, -1);};
	    
    femit(fp, "end", NULL, pck, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EMIT_LOCAL_VAR_INIT - emit initializations of local variables
 *                     - mainly to quell compiler warnings
 */

static void emit_local_var_init(FILE *fc, fdecl *dcl)
   {int i, na;
    farg *al;

    fprintf(fc, "/* local variable initializations */\n");

    na = dcl->na;
    al = dcl->al;
        
/* initialize local variable to quell compiler warnings */
    for (i = 0; i < na; i++)
        {if ((IS_NULL(al[i].name) == FALSE) && (al[i].cls == B_F))
	    fputs(al[i].interp.defa, fc);};

    fprintf(fc, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAP_NAME - map the C function name CF to form target language name D
 *          - unless the specified language binging name LF is non-NULL
 *          - or not "yes"
 *          - in which case return LF in D
 */

static char *map_name(char *d, int nc, char *cf, char *lf,
		      char *sfx, int cs, int us, pboolean cls)
   {int i, n, ok;
    static char *pre[] = { "SC_", "PM_", "PD_", "PG_", "PA_" };

    n = sizeof(pre)/sizeof(char *);
    ok = FALSE;
    for (i = 0; (i < n) && (ok == FALSE); i++)
        {if (strncmp(cf, pre[i], 3) == 0)
	    ok = TRUE;};

    if ((lf != NULL) && (strcmp(lf, "none") != 0) && (strcmp(lf, "yes") != 0))
       {if ((sfx != NULL) && (sfx[0] == 'i'))
	   snprintf(d, nc, "%s_%s", lf, sfx);
	else
	   nstrncpy(d, nc, lf, -1);}

    else if ((cls == B_T) && (ok == TRUE))
       snprintf(d, nc, "%s", cf+3);

    else
       {if (sfx != NULL)
	   snprintf(d, nc, "%s_%s", cf, sfx);
	else
	   snprintf(d, nc, "%s", cf);};

    switch (cs)
       {case -1 :
	     downcase(d);
	     break;
	case 1 :
	     upcase(d);
	     break;};

/* map underscore to dash */
    if (us == TRUE)
       memmove(d, subst(d, "_", "-", -1), nc);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_BIND - generate declarations for all bindings from CPR and SBI
 *           - return TRUE iff successful
 */

static void find_bind(statedes *st, int iref)
   {int i, ib, id, nb, nc, ne, ns, nbi;
    char t[BFVLG], ps[BFLRG];
    char *sb, *lng, *te, *p;
    char **cpr, **cdv, **sbi, **sa, **ta;
    fdecl *dcl;
    der_list *el, *sl;

    nbi = st->nbi;
    sbi = st->sbi;
    cpr = st->cpr;
    cdv = st->cdv;

/* parse out prototypes */
    nb  = 0;
    dcl = MAKE_N(fdecl, nbi);
    if (dcl != NULL)
       {for (ib = 0; ib < nbi; ib++)
	    {sb = sbi[ib];
	     if (blank_line(sb) == FALSE)
	        {nstrncpy(t, BFVLG, sb, -1);
		 sa = tokenize(t, " \t", 0);

		 if ((sa != NULL) &&
		     (strcmp(sa[0], "derived") != 0) &&
		     (strcmp(sa[0], tykind[TK_STRUCT]) != 0) &&
		     (strcmp(sa[0], tykind[TK_ENUM]) != 0))
		    {ta = NULL;
		     for (i = 1; sa[i] != NULL; i++)
		         {lng = sa[i];
			  p   = strchr(lng, '(');
			  if (p != NULL)
			     {*p++ = '\0';
			      ta   = lst_push(ta, lng);
			      p    = trim(p, BACK, ")");
			      if (IS_NULL(p) == TRUE)
				 ta = lst_push(ta, "yes");
			      else
				 ta = lst_push(ta, p);}
			  else
			     ta = lst_push(ta, lng);};

		     dcl[nb].bindings = ta;
		     dcl[nb].iref     = iref;
		     dcl[nb].ref      = gbd + iref;

		     nb += find_proto(dcl+nb, cpr, sa[0]);};

		 free_strings(sa);};};};

    st->dcl  = dcl;
    st->ndcl = nb;

/* parse out enums */
    ne = 0;
    el = NULL;
    if (cdv != NULL)
       {for (ib = 0; ib < nbi; ib++)
	    {nstrncpy(ps, BFLRG, sbi[ib], -1);
	     if (strncmp(ps, tykind[TK_ENUM], 4) == 0)
	        {te = strtok(ps, " ");
		 te = strtok(NULL, " ");
		 for (id = 0; cdv[id] != NULL; id++)
		     {sb = cdv[id];
		      if (blank_line(sb) == FALSE)
			 {nstrncpy(t, BFVLG, sb, -1);
			  ta = tokenize(t, " \t", 0);
			  if (strcmp(ta[1]+2, te) == 0)
			     {el = push_derived(el, &ne, sb, ta);
			      break;}
			  else
			     free_strings(ta);};};};};};

    st->enums = el;
    st->nen   = ne;

/* parse out structs */
    ns = 0;
    sl = NULL;
    if (cdv != NULL)
       {for (ib = 0; ib < nbi; ib++)
	    {nstrncpy(ps, BFLRG, sbi[ib], -1);
	     if (strncmp(ps, tykind[TK_STRUCT], 6) == 0)
	        {te = strtok(ps, " \t");
		 te = strtok(NULL, " \t");
		 nc = strlen(te);
		 for (id = 0; cdv[id] != NULL; id++)
		     {sb = cdv[id];
		      if (blank_line(sb) == FALSE)
			 {nstrncpy(t, BFVLG, sb, -1);
			  ta = tokenize(t, "{;}", 0);
			  if (strncmp(ta[0]+9, te, nc) == 0)
			     {sl = push_derived(sl, &ns, sb, ta);
			      break;}
			  else
			     free_strings(ta);};};};};};

    st->structs = sl;
    st->nst     = ns;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HAS_BINDING - return the LANG binding specification */

static char *has_binding(fdecl *dcl, char *lang)
   {int ib;
    char *b, *spec;
    char **bindings;

    spec = NULL;

    bindings = dcl->bindings;
    if (bindings != NULL)
       {for (ib = 0; bindings[ib] != NULL; ib += 2)
	    {b = bindings[ib];
	     if (strcmp(b, lang) == 0)
	        {spec = bindings[ib+1];
		 break;};};};

    return(spec);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_BINDER - initialize the data members of BD */

static void setup_binder(statedes *st, int iref, char *pck, char **sbi)
   {

    if (st != NULL)
       {st->pck = pck;

	st->sbi = sbi;
	st->nbi = lst_length(sbi);

/* parse out and construct all the declarations */
	find_bind(st, iref);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CF_CALL_LIST - render the arg list of DCL into A for a
 *              - compiled function call (usually C)
 */

static void cf_call_list(char *a, int nc, fdecl *dcl, int local)
   {int i, na, voida;
    char *nm;
    farg *al;
    idecl *ip;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    {nm = al[i].name;
	     ip = &al[i].interp;

/* pass function pointers straight thru - no local variable */
	     if ((al[i].knd == FP_FNC) && (local == FALSE))
	        vstrcat(a, BFLRG, "%s, ", nm);

/* scalar return values need references */
	     else if (local == TRUE)
	        vstrcat(a, BFLRG, "%s, ", ip->argc);

/* all other argument go thru a local variable as declared */
	     else
	        vstrcat(a, BFLRG, "_l%s, ", nm);};

	a[strlen(a) - 2] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IF_CALL_LIST - render the arg list of DCL into A for an
 *              - interpreter function call
 */

static void if_call_list(char *a, int nc, fdecl *dcl, char *dlm)
   {int i, na, voida;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if (voida == FALSE)
       {if (dlm == NULL)
	   {for (i = 0; i < na; i++)
	        {if (al[i].cls == B_F)
		    vstrcat(a, BFLRG, "%s ", al[i].name);};

	    a[strlen(a) - 1] = '\0';}

	else
	   {for (i = 0; i < na; i++)
	        {if (al[i].cls == B_F)
		    vstrcat(a, BFLRG, "%s%s ", al[i].name, dlm);};

	    a[strlen(a) - 2] = '\0';};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_PROTO - make a C prototype without any ARG qualifiers */

static void c_proto(char *pr, int nc, fdecl *dcl)
   {int i, na, voida;
    char *arg;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    snprintf(pr, nc, "%s %s(", dcl->proto.type, dcl->proto.name);
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    {arg = al[i].arg;
	     vstrcat(pr, nc, "%s, ", arg);};
	pr[strlen(pr)-2] = '\0';};

    nstrcat(pr, nc, ")");
    nstrncpy(pr, nc, subst(pr, "* ", "*", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/

/*                        LANGUAGE BINDING ROUTINES                         */

/*--------------------------------------------------------------------------*/

#include "blang-c.c"
#include "blang-f.c"
#include "blang-scm.c"
#include "blang-py.c"
#include "blang-doc.c"
#include "blang-ba.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANG - control the generation of language binding
 *       - return TRUE iff successful
 */

static int blang(statedes *st, char *pck, char *fbi, int iref)
   {int ib, rv;
    char **sbi;
    bindes *pb;

    init_types(iref);

    rv = FALSE;

    if (IS_NULL(fbi) == FALSE)
       {sbi = file_text(FALSE, fbi);

	if (sbi == NULL)
	   printf("No bindings found for '%s'\n", pck);
            
	else
	   {add_derived_types(iref, sbi);

	    setup_binder(st, iref, pck, sbi);

/* initialize the binding constructors */
	    for (pb = gbd, ib = 0; ib < nbd; ib++, pb++)
	        {if (pb->init != NULL)
		    pb->init(st, pb);};

/* make all the language bindings */
	    if (sbi != NULL)
	       {for (pb = gbd, ib = 0; ib < nbd; ib++, pb++)
		    {if (pb->bind != NULL)
			pb->bind(pb);};};

/* cleanup */
	    for (pb = gbd, ib = 0; ib < nbd; ib++, pb++)
	        {if (pb->fin != NULL)
		    pb->fin(pb);};

	    rv = TRUE;};

	free_strings(sbi);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, ib, iref, rv, err;
    char pck[BFLRG], msg[BFLRG];
    char *fbi;
    bindes *pb;
    statedes st = {0, 0, 0, 0,
                   {FALSE, FALSE, FALSE, FALSE},
		   NULL,
		   NULL, NULL, NULL, NULL, NULL, NULL,
		   NULL, NULL, NULL, };

    nbd = 0;

    for (i = 0; i < N_MODES; i++)
        st.no[i] = TRUE;

/* register the language bindings */
    iref = register_c(TRUE, &st);
    register_doc(st.no[0], &st);
    register_fortran(st.no[1], &st);
    register_scheme(st.no[2], &st);
    register_python(st.no[3], &st);
    register_basis(st.no[4], &st);

    nstrncpy(st.path, BFLRG, ".", -1);

    rv  = 1;
    fbi = "";

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-b") == 0)
	    fbi = v[++i];

	 else if (strcmp(v[i], "-h") == 0)
            {printf("Usage: blang -b <bindings> [-h] [-p <dir>]\n");
             printf("   b    file containing binding specifications\n");
             printf("   h    this help message\n");
             printf("   p    directory for generated files\n");
             printf("\n");

/* let each binding process its own help */
             err = 0;
	     for (pb = gbd, ib = 0; ib < nbd; ib++, pb++)
	         {if (pb->cl != NULL)
		     err += pb->cl(&st, pb, c, v);};

             return(err);}

	 else if (strcmp(v[i], "-p") == 0)
            nstrncpy(st.path, BFLRG, v[++i], -1);};

/* let each binding process its own command line args */
    err = 0;
    for (pb = gbd, ib = 0; ib < nbd; ib++, pb++)
        {if (pb->cl != NULL)
	    err += pb->cl(&st, pb, c, v);};

    if (err == 0)
       {snprintf(pck, BFLRG, "%s", path_base(path_tail(fbi)));
	snprintf(msg, BFLRG, "%s bindings", pck);

	printf("      %s ", fill_string(msg, 25));

	rv = blang(&st, pck, fbi, iref);
	rv = (rv != TRUE);

	printf("done\n");

	log_safe("dump", 0, NULL, NULL);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

