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
 *    exceptions for implied variables:
 *        SC_CHAR_P_x   -> SC_STRING_x
 *        SC_VOID_P_x   -> SC_POINTER_x
 *
 *    KIND_CHAR/KIND_INT have
 *      signed       unsigned    variations
 *      int8_t       u_int8_t
 *      signed int   unsigned int
 *   
 *    PDB type names should match results of fix_type_designator
 *
 *    pointer types have default value of NULL
 *   
 *    f90 type names are uniquely different
 *    other languages follow C naming by and large
 *    f90 pointer types have "-A" convention
 *       char   -> character
 *       char * -> character-A
 *
 * libtyp.c has routines to parse and manage the master type tables
 * for use by blang, template, and other part of PACT
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBTYP

# define LIBTYP

#define GET_TYPE(_t)                                                         \
    ((strcmp(_t, "-") == 0) ? NULL : trim(_t, BOTH, " \t\""))

#define IS_PRIMITIVE_TYPE(_t)                                                \
    ((_t != NULL) &&                                                         \
     (_t->knd == TK_PRIMITIVE) &&                                            \
     (_t->alias == NULL) &&                                                  \
     ((_t->g != KIND_POINTER) || (_t->id == std_pointer_id)))

#ifdef __cplusplus
# define DEF_FUNCTION_PTR(_t, _n) typedef _t (*PF##_n)(...)
#else
# define DEF_FUNCTION_PTR(_t, _n) typedef _t (*PF##_n)(void)
#endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

enum e_type_kind
   {TK_PRIMITIVE = 0, TK_ENUM, TK_STRUCT, TK_UNION, TK_META};

typedef enum e_type_kind type_kind;

enum e_type_group
   {KIND_CHAR = 0,
    KIND_BOOL,
    KIND_INT,
    KIND_FLOAT,
    KIND_COMPLEX,
    KIND_QUATERNION,
    KIND_POINTER,
    KIND_ENUM,
    KIND_STRUCT,
    KIND_UNION,
    KIND_OTHER};

typedef enum e_type_group type_group;

typedef struct s_typdes typdes;

struct s_typdes
   {int id;         /* integer type id */
    int bpi;        /* bytes per item */
    char *type;     /* C type name quoted to capture white space,
                     * e.g. "long long" */
    char *stype;    /* signed type version */
    char *utype;    /* unsigned type version */
    char *ftype;    /* name to use in PDB files and similar */
    char *fncp;     /* abbreviation to use in generated function names */
    char *alias;    /* name of other type iff an alias, NULL otherwise */
    bool ptr;       /* generate "foo *" for "foo" iff TRUE */
    type_kind knd;  /* enumerated type kind: primitive, enum, ... */
    type_group g;   /* enumerated type group: bool, int, complex, ... */
    char *mn;       /* minimum value of type */
    char *mx;       /* maximum value of type */
    char *defv;     /* default value of type */
    char *f90;      /* F90 type name for the C type */
    char *promo;    /* type promoted to in calls */
    char *comp;     /* unit type for composite types */
    char *typ_i;    /* type index variable name, SC_INT_I */
    char *typ_s;    /* type name variable name, SC_INT_S */
    void *a;        /* application supplied info */
    void (*init)(void *x);     /* function to initialize instance */
    void (*free)(void *x);};   /* function to free instance */

typedef struct s_quaternion quaternion;

struct s_quaternion
   {double s;
    double i;
    double j;
    double k;};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

int
 std_pointer_id = -1,   /* id number of 'void *' the one distinct pointer */
 type_counts[13];

char
 *tykind[] = {"primitive", "enum", "struct", "union", "meta"},
 *tyknde[] = { "TK_PRIMITIVE",
	       "TK_ENUM", "TK_STRUCT", "TK_UNION",
	       "TK_META" },
 *tygrpe[] = { "KIND_CHAR", "KIND_BOOL", "KIND_INT",
	       "KIND_FLOAT", "KIND_COMPLEX", "KIND_QUATERNION",
	       "KIND_POINTER",
	       "KIND_ENUM", "KIND_STRUCT", "KIND_UNION",
	       "KIND_OTHER" };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEREF - dereference a pointer S
 *       - place result in D
 *       - return TRUE iff a pointer was dereferenced
 */

int deref(char *d, int nc, char *s)
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

int ideref(char *s)
   {int rv;
    char *p;

    rv = FALSE;

    p = strstr(s, "_P_I");
    if (p != NULL)
       {memmove(p, p+2, 3);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_TYPE_DESIGNATOR - make a properly named type designator
 *                     - from raw string T
 *                     - FL it a bit array controlling:
 *                     -   1  on requests upper case and off lower case
 *                     -   2  on removes _t from conventional C types
 */

char *fix_type_designator(int fl, char *fmt, ...)
   {int nc;
    char t[BFLRG];
    char *p, *rv;

    VA_START(fmt);
    VSNPRINTF(t, BFLRG, fmt);
    VA_END;

    if (fl & 1)
       upcase(t);
    else
       downcase(t);
    rv = subst(t, " ", "_", -1);
    rv = subst(rv, "__", "_", -1);
    
/* fix hated _t types
 * e.g. SC_INT32_T_I -> SC_INT32_I
 * or   SC_INT32_T_P_I -> SC_INT32_P_I
 */
    if (fl & 2)
       {nc = strlen(rv);
	p  = rv + nc - 2;
	if ((strcmp(p, "_I") == 0) || (strcmp(p, "_S") == 0))
	   {p -= 2;
	    if (strncmp(p, "_P", 2) == 0)
	       p -= 2;
	    if (strncmp(p, "_T", 2) == 0)
	       memmove(p, p+2, strlen(p)-1);};};

    rv = STRSAVE(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_TYPE_ALIAS - make a properly named type alias
 *                - from raw string T
 */

static char *fix_type_alias(char *fmt, ...)
   {int nc;
    char t[BFLRG];
    char *p, *rv;

    VA_START(fmt);
    VSNPRINTF(t, BFLRG, fmt);
    VA_END;

    downcase(t);
    rv = subst(t, " ", "_", -1);
    rv = subst(rv, "__", "_", -1);
    
/* fix hated _t types
 * e.g. int32_t -> int32
 */
    nc = strlen(rv);
    p  = rv + nc - 2;
    if (strcmp(p, "_t") == 0)
       *p = '\0';

    rv = STRSAVE(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_SIGNED - return the correct signed/unsigned type name of T
 *            - the following apply:
 *            -   '-'     the signed/unsigned name is the same as type
 *            -   '+'     the signed/unsigned name is the same as
 *            -           type prefixed with either signed or unsigned
 *            -           as appropriate
 *            -   <name>  is used as the literal signed/unsigned type name
 */

char *fix_signed(char *ust, char *usp, char *t)
   {char s[BFSML];
    char *rv;

    if (ust == NULL)
       rv = NULL;
    else if (ust[0] == '-')
       rv = STRSAVE(t);
    else if (ust[0] == '+')
       {snprintf(s, BFSML, "%s %s", usp, t);
	rv = STRSAVE(s);}
    else
       {ust = GET_TYPE(ust);
	rv  = STRSAVE(ust);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_COMPONENT - fixup and return the unit type for composite types */

char *fix_component(char *tc, char *t)
   {char *rv;

    rv = GET_TYPE(tc);

    if (IS_NULL(rv) == TRUE)
       rv = STRSAVE(t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_CAMELCASE - make up a camel case version of the type T */

char *fix_camelcase(char *t)
   {int n;
    char s[BFSML], r[BFSML];
    char **sa, *rv;

    rv = subst(t, "_t", "", -1);
    sa = tokenize(rv, " _\t", 0);

/* capitolize each token for a camel case version of type name */
    for (n = 0; sa[n] != NULL; n++)
        sa[n][0] = toupper(sa[n][0]);

    if (sa[n-1][0] == '*')
       snprintf(s, BFSML, "%s%s",
                subst(sa[n-1], "*", "P", -1),
                concatenate(r, BFSML, sa, 0, n-1, ""));
    else
       snprintf(s, BFSML, "%s",
		concatenate(r, BFSML, sa, 0, -1, ""));

    free_strings(sa);

    rv = STRSAVE(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_TABLE - return a pointer to the type table */

typdes *type_table(typdes *td)
   {int nb;
    static int ni = 0;
    static int nx = 20;
    static typdes *tl = NULL;

/* if we are not adding anything just return the existing table */
    if (td != NULL)
       {if (tl == NULL)
	   {ni = 0;
	    tl = MAKE_N(typdes, nx);
	    memset(type_counts, 0, sizeof(type_counts));};
	
	if (((ni + 1) % nx == 0) && (ni > 0))
	   {nb = (ni + 1)/nx;
	    REMAKE(tl, typdes, (nb+1)*nx);};

	td->id   = ni;
	tl[ni++] = *td;

/* count types */
        if (td->alias == NULL)
	   {type_counts[td->g]++;
	    type_counts[11] += (IS_PRIMITIVE_TYPE(td) == B_T);};
        type_counts[12]  = ni;

/* always leave the list NULL terminated */
	memset(tl+ni, 0, sizeof(typdes));};

    return(tl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOOKUP_TYPE_INFO - lookup and return a type from the type lists */

typdes *lookup_type_info(char *ty)
   {int i;
    typdes *tl, *rv;

    rv = NULL;
    tl = type_table(NULL);

    for (i = 0; tl[i].type != NULL; i++)
        {if (strcmp(ty, tl[i].type) == 0)
	    {rv = tl + i;
	     break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PUSH_TYPE_PTR - add a pointer type derived from TD to the type table */

static void _push_type_ptr(typdes *td, int alias)
   {char ti[BFSML], ts[BFSML];
    typdes tp;

    tp = *td;

    snprintf(ti, BFSML, "%s *", tp.type);
    tp.type  = STRSAVE(ti);
    tp.stype = tp.type;
    tp.utype = tp.type;

    if (alias == TRUE)
       {snprintf(ti, BFSML, "%s *", tp.fncp);
	tp.fncp = STRSAVE(ti);}
    else
       tp.fncp = STRSAVE("ptr");
    tp.bpi  = sizeof(char *);
    tp.knd  = TK_PRIMITIVE;
    tp.g    = KIND_POINTER;
    tp.ptr  = B_F;
    tp.mn   = NULL;
    tp.mx   = NULL;
    tp.defv = "NULL";

    if (td->f90 != NULL)
       {if (strcmp(td->f90, "void") == 0)
	   nstrncpy(ti, BFSML, "C_PTR-A", -1);
	else
	   snprintf(ti, BFSML, "%s-A", tp.f90);}
    else
#if 1
       nstrncpy(ti, BFSML, "C_PTR-A", -1);
#else
       nstrncpy(ti, BFSML, "character-A", -1);
#endif

    tp.f90 = STRSAVE(ti);

    if (strcmp(td->type, "char") == 0)
       {nstrncpy(ti, BFSML, "SC_STRING_I", -1);
	nstrncpy(ts, BFSML, "SC_STRING_S", -1);}
    else if (strcmp(td->type, "void") == 0)
       {nstrncpy(ti, BFSML, "SC_POINTER_I", -1);
	nstrncpy(ts, BFSML, "SC_POINTER_S", -1);}
    else
       {snprintf(ti, BFSML, "SC_%s_P_I", td->type);
	snprintf(ts, BFSML, "SC_%s_P_S", td->type);};

    tp.typ_i = fix_type_designator(3, ti);
    tp.typ_s = fix_type_designator(3, ts);

/* NOTE: we have to do this before adding it to the type table
 * so that it will count as a primitive
 */
    if (td->knd == TK_META)
       std_pointer_id = tp.id + 1;

    type_table(&tp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PARSE_STANDARD_TYPES - parse the master type table, type-table */

static int _parse_standard_types(char **sa)
   {int i, in, nt;
    char *s, **ta;
    typdes td;

    in = FALSE;

    for (i = 0, nt = 0; sa[i] != NULL; i++)
        {s = trim(sa[i], BOTH, " \t\n");
	 if (blank_line(s))
	    continue;
	 else if (strcmp(s, "standard = {") == 0)
	    in = TRUE;
	 else if ((strcmp(s, "}") == 0) && (in == TRUE))
	    {in = FALSE;
	     break;}
	 else if (in == TRUE)
	    {ta = tokenize(s, " \t\n", 0);

	     td.id    = -1;
	     td.bpi   = -1;
             td.type  = GET_TYPE(ta[0]);
             td.fncp  = GET_TYPE(ta[1]);
	     td.alias = NULL;

/* work out the type_group */
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
	     else if (strcmp(ta[2], "ENUM") == 0)
	        {td.g   = KIND_ENUM;
		 td.knd = TK_ENUM;}
	     else if (strcmp(ta[2], "STRUCT") == 0)
	        {td.g   = KIND_STRUCT;
		 td.knd = TK_STRUCT;}
	     else if (strcmp(ta[2], "UNION") == 0)
	        {td.g   = KIND_UNION;
		 td.knd = TK_UNION;}
	     else if (strcmp(ta[2], "OTHER") == 0)
	        {td.g   = KIND_OTHER;
		 td.knd = TK_META;}

             td.ptr   = ((strcmp(ta[3], "TRUE") == 0) ||
		         (strcmp(ta[3], "B_T") == 0));

             td.mn    = GET_TYPE(ta[4]);
             td.mx    = GET_TYPE(ta[5]);
             td.defv  = GET_TYPE(ta[6]);
             td.f90   = GET_TYPE(ta[7]);
	     td.promo = GET_TYPE(ta[8]);

	     td.comp  = fix_component(ta[9], td.type);

	     td.stype = fix_signed(ta[10], "signed", td.type);
	     td.utype = fix_signed(ta[11], "unsigned", td.type);
	     td.ftype = fix_type_designator(0, td.type);

	     td.typ_i = fix_type_designator(3, "SC_%s_I", td.type);
	     td.typ_s = fix_type_designator(3, "SC_%s_S", td.type);

	     td.a    = NULL;
	     td.init = NULL;
	     td.free = NULL;

	     type_table(&td);
	     nt++;

	     if (td.ptr == B_T)
                {_push_type_ptr(&td, FALSE);
		 nt++;};

	     FREE(ta);};};

    return(nt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PARSE_ALIAS_TYPES - parse the master type table, type-table */

static int _parse_alias_types(char **sa)
   {int i, in, nt;
    char *s, *p, **ta;
    typdes td, *to;

    in = FALSE;

    memset(&td, 0, sizeof(typdes));

    for (i = 0, nt = 0; sa[i] != NULL; i++)
        {s = trim(sa[i], BOTH, " \t\n");
	 if (blank_line(s))
	    continue;
	 else if (strcmp(s, "alias = {") == 0)
	    in = TRUE;
	 else if ((strcmp(s, "}") == 0) && (in == TRUE))
	    {in = FALSE;
	     break;}
	 else if (in == TRUE)
	    {ta = tokenize(s, " \t\n", 0);
	     p  = trim(ta[1], BOTH, " \"");
	     to = lookup_type_info(p);
	     if (to != NULL)
	        {td = *to;

		 td.type  = trim(ta[0], BOTH, " \"");              /* new */
		 td.stype = td.type;
		 td.utype = td.type;
		 td.ftype = fix_type_designator(0, td.type);
		 td.fncp  = fix_type_alias(p);                     /* old */
		 td.alias = to->type;

		 td.ptr = ((strcmp(ta[2], "TRUE") == 0) ||
			   (strcmp(ta[2], "B_T") == 0));

		 td.typ_i = fix_type_designator(3, "SC_%s_I", td.type);
		 td.typ_s = fix_type_designator(3, "SC_%s_S", td.type);

		 type_table(&td);
		 nt++;

		 if (td.ptr == B_T)
		    {_push_type_ptr(&td, TRUE);
		     nt++;};

		 FREE(ta);};};};

    return(nt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_TYPE_TABLE - parse the master type table TYTAB */

void parse_type_table(char *tytab)
   {int ne;
    char **sa;

    if (type_table(NULL) == NULL)
       {sa = file_text(FALSE, tytab);

	ne  = 0;
	ne += _parse_standard_types(sa);
	ne += _parse_alias_types(sa);

	lst_free(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INSTALL_DERIVED_TYPE - construct and install an entry
 *                      - for a derived type
 */

void install_derived_type(char *name, type_kind knd, char *defv)
   {char t[BFSML];
    typdes td;

    if (knd == TK_ENUM)
       {td.f90  = "integer";
	td.g    = KIND_ENUM;
	td.defv = defv;}
    else if (knd == TK_STRUCT)
       {td.f90  = tykind[knd];
	td.g    = KIND_STRUCT;
	td.defv = NULL;}
    else if (knd == TK_UNION)
       {td.f90  = tykind[knd];
	td.g    = KIND_UNION;
	td.defv = NULL;};
	
    td.type  = name;
    td.stype = td.type;
    td.utype = td.type;
    td.ftype = fix_type_designator(0, td.type);
    td.fncp  = NULL;
    td.knd   = knd;
    td.ptr   = B_F;
    td.mn    = NULL;
    td.mx    = NULL;

    snprintf(t, BFSML, "G_%s_I", td.type);
    upcase(t);
    td.typ_i = STRSAVE(subst(t, " ", "_", -1));

    snprintf(t, BFSML, "G_%s_S", td.type);
    upcase(t);
    td.typ_s = STRSAVE(subst(t, " ", "_", -1));

    td.promo = NULL;
    td.comp  = NULL;

    type_table(&td);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FOREACH_TYPE - parse the master type table TYTAB */

int foreach_type(int (*f)(typdes *td, void *a), void *a)
   {int i, rv;
    typdes *tl;

    rv = TRUE;
    tl = type_table(NULL);

    for (i = 0; tl[i].type != NULL; i++)
        rv &= f(tl+i, a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef UNDEFINED
#undef GET_TYPE

# endif
#endif

