/*
 * PACPP.C - some routines to help the code know about pre-processor names
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/*
 *   Names names must be unique. Each name belongs to a group.
 *   Each group must have unique values.
 *
 *   For example
 *
 *   #define FOO_FIRST   1
 *   #define FOO_SECOND  2
 *   #define FOO_THIRD   3
 *
 *   #define BAR_FIRST   1
 *   #define BAR_SECOND  2
 *   #define BAR_THIRD   3
 *
 *   #define BLAT_FIRST   one
 *   #define BLAT_SECOND  two
 *   #define BLAT_THIRD   three
 *
 * Then the usage would be:
 *      PA_cpp_init();
 *      PA_cpp_add_group("FOO", SC_INT_I, NULL);
 *      PA_cpp_add_group("BAR", SC_INT_I, NULL);
 *      PA_cpp_add_group("BLAT", SC_STRING_I, NULL);
 *
 * Then define some names:
 *      PA_cpp_add_name("FOO_FIRST",  "FOO", 1)
 *      PA_cpp_add_name("FOO_SECOND", "FOO", 2)
 *      PA_cpp_add_name("FOO_THIRD",  "FOO", 3)
 *
 *      PA_cpp_add_name("BAR_FIRST",  "BAR", 1)
 *      PA_cpp_add_name("BAR_SECOND", "BAR", 2)
 *      PA_cpp_add_name("BAR_THIRD",  "BAR", 3)
 *
 *      blat_tmp = CSTRSAVE("one");
 *      PA_cpp_add_name("BLAT_FIRST",  "BLAT", blat_tmp)
 *      blat_tmp = CSTRSAVE("two");
 *      PA_cpp_add_name("BLAT_SECOND", "BLAT", blat_tmp)
 *      blat_tmp = CSTRSAVE("three");
 *      PA_cpp_add_name("BLAT_THIRD",  "BLAT", balt_tmp)
 *
 *      PA_cpp_name_to_value
 */

struct s_PA_cpp_node
   {char *name;
    char *group;
    int itype;
    char *type;
    void *data;};

typedef struct s_PA_cpp_node PA_cpp_node;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_INIT - initialize hash table and strings used by the cpp package */

void PA_cpp_init(void)
   {

    PA_gs.cpp_name_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

    PA_gs.cpp_value_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

    PA_gs.cpp_node = CSTRDUP("PA_cpp_node", 3);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_ADD_GROUP - add a group heading
 *    name  - name of group
 *    itype - integer type of group
 *    type  - ascii name of type, can be NULL if itype is a known type.
 *            (i.e. not a user defined type)
 */

void PA_cpp_add_group(char *name, int itype, char *type)
   {PA_cpp_node *node;

    node        = CMAKE(PA_cpp_node);
    node->name  = name;
    node->group = name;
    node->itype = itype;
    node->type  = type;
    node->data  = NULL;

    SC_hasharr_install(PA_gs.cpp_value_tab, name, node, PA_gs.cpp_node, 3, -1);
  
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_CPP_DATA - return a pointer to the data for name
 *              - allocate memory if requested
 *
 *   alloc   - if true return a pointer to the data.  Will allocate memory
 *             for non-pointer types.
 *   name0   - returned name, must be long enough to hold the name.
 *   group   - pointer to group name.
 *   itype   - integer type index.
 *   input   - the input from the user, will be interpretered based on itype.
 *             if itype is a pointer type, space will not be allocated
 *             so the pointer must not be changed.
 */

static void *_PA_cpp_data(int alloc, char *name0, char *group,
                          int itype, va_list *input)
   {char bf[MAXLINE], v[MAX_PRSZ];
    void *data;

    data = NULL;

    SC_VA_SAVE(input);

    SC_VA_ARG_ID(itype, v, 0);
    SC_ntos(bf, MAXLINE, itype, v, 0, 1);

    sprintf(name0, "%s-%s", group, bf);

    if (alloc == TRUE)
       {if (SC_is_type_ptr(itype) == TRUE)
	   data = *(void **) v;
	else
	   data = SC_convert_id(itype, NULL, 0, 1, itype, v, 0, 1, 1, FALSE);};
  
    SC_VA_RESTORE(input);

    if (alloc)
       SC_mark(data, 1);

    return(data);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_ADD_NAME - add a preprocessor name and value to the table */

void PA_cpp_add_name(char *name, char *group, ...)
   {char name0[MAXLINE];
    PA_cpp_node *g_node, *node;

/* find the type for this group */
    g_node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_gs.cpp_value_tab, group);
    PA_ERR((g_node == NULL),
	   "NO SUCH CPP GROUP %s", group);

    node = CMAKE(PA_cpp_node);

    node->name  = name;
    node->group = group;
    node->itype = g_node->itype;
    node->type  = g_node->type;

/* get a pointer to the data and name of cross reference */
    SC_VA_START(group);
    node->data = _PA_cpp_data(1, name0, group, g_node->itype, &SC_VA_VAR);
    SC_VA_END;

/* install into hash table */
    SC_hasharr_install(PA_gs.cpp_name_tab, name, node, PA_gs.cpp_node, 3, -1);

/* build cross reference from value to name */
    SC_hasharr_install(PA_gs.cpp_value_tab, name0, node, PA_gs.cpp_node, 3, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_NAME_TO_VALUE - return a pointer to the names value */

void *PA_cpp_name_to_value(char *name)
   {void *data;
    PA_cpp_node *node;

    node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_gs.cpp_name_tab, name);

    if (node != NULL)
       data = node->data;
    else
       data = NULL;

    return(data);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_NAME_ITYPE - return the names type as an integer
 *                   - using SC_TYPE_I defines
 */

int PA_cpp_name_itype(char *name)
   {int itype;
    PA_cpp_node *node;

    node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_gs.cpp_name_tab, name);

    if (node != NULL)
       itype = node->itype;
    else
       itype = SC_UNKNOWN_I;

    return(itype);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_VALUE_TO_NAME - return the name of a name given the group it
 *                      - belongs to and a value
 */

char *PA_cpp_value_to_name(char *group, ...)
   {char *cpp_name = NULL;
    char name0[MAXLINE];
    PA_cpp_node *g_node, *cpp_node;

/* find the type for this group */
    g_node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_gs.cpp_value_tab, group);

    if (g_node != NULL)
       {SC_VA_START(group);
	_PA_cpp_data(0, name0, group, g_node->itype, &SC_VA_VAR);
	SC_VA_END;

/* look up value */
	cpp_node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_gs.cpp_value_tab, name0);

	if (cpp_node != NULL)
	   cpp_name = cpp_node->name;};

    return(cpp_name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

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
    PA_cpp_add_name(SC_PCONS_S,     PA_gs.cpp_type_SC, SC_PCONS_I);
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
    PA_cpp_add_name("SC_PCONS_I",     PA_gs.cpp_type, SC_PCONS_I);
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
    PA_cpp_add_name("SC_PCONS_S",     PA_gs.cpp_type_S, SC_PCONS_S);
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

/* CONVERT_TYPE_S_I - convert a string type identifier to an integer
 *                  - type identifier
 */

int convert_type_s_i(char *type_name)
   {int type, *ptype;

    ptype = (int *) PA_cpp_name_to_value(type_name);

    if (ptype != NULL)
       type = *ptype;
    else
       type = SC_UNKNOWN_I;

    return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONVERT_TYPE_I_S - convert a integer type identifier to a string
 *                  - type identifier
 */

char *convert_type_i_s(int type)
   {char *type_name;

    type_name = PA_cpp_value_to_name(PA_gs.cpp_type, type);
    if (type_name == NULL)
       type_name = SC_UNKNOWN_S;

    return(type_name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
