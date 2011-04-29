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

char
 *PA_CPP_INFO,
 *PA_CPP_ALLOCATION,
 *PA_CPP_SCOPE,
 *PA_CPP_CLASS,
 *PA_CPP_CENTER,
 *PA_CPP_PERSISTENCE,
 *PA_CPP_UNITS,
 *PA_CPP_TYPE,
 *PA_CPP_TYPE_SC,
 *PA_CPP_TYPE_S;

char
 *PAN_CPP_NODE = NULL;

hasharr
 *PA_cpp_name_tab  = NULL,
 *PA_cpp_value_tab = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_INIT - initialize hash table and strings used by the cpp package */

void PA_cpp_init(void)
   {

    PA_cpp_name_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);

    PA_cpp_value_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);

    PAN_CPP_NODE = CSTRSAVE("PA_cpp_node");
    SC_permanent(PAN_CPP_NODE);

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

    SC_hasharr_install(PA_cpp_value_tab, name, node, PAN_CPP_NODE, TRUE, TRUE);
  
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
    g_node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_cpp_value_tab, group);
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
    SC_hasharr_install(PA_cpp_name_tab, name, node, PAN_CPP_NODE, TRUE, TRUE);

/* build cross reference from value to name */
    SC_hasharr_install(PA_cpp_value_tab, name0, node, PAN_CPP_NODE, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CPP_NAME_TO_VALUE - return a pointer to the names value */

void *PA_cpp_name_to_value(char *name)
   {void *data;
    PA_cpp_node *node;

    node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_cpp_name_tab, name);

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

    node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_cpp_name_tab, name);

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
    g_node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_cpp_value_tab, group);

    if (g_node != NULL)
       {SC_VA_START(group);
	_PA_cpp_data(0, name0, group, g_node->itype, &SC_VA_VAR);
	SC_VA_END;

/* look up value */
	cpp_node = (PA_cpp_node *) SC_hasharr_def_lookup(PA_cpp_value_tab, name0);

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

    PA_CPP_INFO        = CSTRSAVE("INFO");
    PA_CPP_ALLOCATION  = CSTRSAVE("ALLOCATION");
    PA_CPP_SCOPE       = CSTRSAVE("SCOPE");
    PA_CPP_CLASS       = CSTRSAVE("CLASS");
    PA_CPP_CENTER      = CSTRSAVE("CENTER");
    PA_CPP_PERSISTENCE = CSTRSAVE("PERSISTENCE");
    PA_CPP_UNITS       = CSTRSAVE("UNITS");
    PA_CPP_TYPE        = CSTRSAVE("TYPE");
    PA_CPP_TYPE_SC     = CSTRSAVE("TYPE_SC");
    PA_CPP_TYPE_S      = CSTRSAVE("TYPE_S");

/* do not delete any of these strings */
    SC_permanent(PA_CPP_INFO);
    SC_permanent(PA_CPP_ALLOCATION);
    SC_permanent(PA_CPP_SCOPE);
    SC_permanent(PA_CPP_CLASS);
    SC_permanent(PA_CPP_CENTER);
    SC_permanent(PA_CPP_PERSISTENCE);
    SC_permanent(PA_CPP_UNITS);
    SC_permanent(PA_CPP_TYPE);
    SC_permanent(PA_CPP_TYPE_SC);
    SC_permanent(PA_CPP_TYPE_S);

    PA_INFO_TYPE_S         = CSTRSAVE("PA_INFO_TYPE");
    PA_INFO_N_DIMS_S       = CSTRSAVE("PA_INFO_N_DIMS");
    PA_INFO_DIMS_S         = CSTRSAVE("PA_INFO_DIMS");
    PA_INFO_SCOPE_S        = CSTRSAVE("PA_INFO_SCOPE");
    PA_INFO_CLASS_S        = CSTRSAVE("PA_INFO_CLASS");
    PA_INFO_CENTER_S       = CSTRSAVE("PA_INFO_CENTER");
    PA_INFO_PERSISTENCE_S  = CSTRSAVE("PA_INFO_PERSISTENCE");
    PA_INFO_ALLOCATION_S   = CSTRSAVE("PA_INFO_ALLOCATION");
    PA_INFO_FILE_NAME_S    = CSTRSAVE("PA_INFO_FILE_NAME");
    PA_INFO_INIT_VAL_S     = CSTRSAVE("PA_INFO_INIT_VAL");
    PA_INFO_INIT_FNC_S     = CSTRSAVE("PA_INFO_INIT_FNC");
    PA_INFO_CONV_S         = CSTRSAVE("PA_INFO_CONV");
    PA_INFO_UNIT_S         = CSTRSAVE("PA_INFO_UNIT");
    PA_INFO_KEY_S          = CSTRSAVE("PA_INFO_KEY");
    PA_INFO_ATTRIBUTE_S    = CSTRSAVE("PA_INFO_ATTRIBUTE");
    PA_INFO_UNITS_S        = CSTRSAVE("PA_INFO_UNITS");
    PA_INFO_DATA_PTR_S     = CSTRSAVE("PA_INFO_DATA_PTR");
    PA_INFO_UNIT_NUMER_S   = CSTRSAVE("PA_INFO_UNIT_NUMER");
    PA_INFO_UNIT_DENOM_S   = CSTRSAVE("PA_INFO_UNIT_DENOM");
    PA_INFO_APPL_ATTR_S    = CSTRSAVE("PA_INFO_APPL_ATTR");
    PA_INFO_DEFAULT_S      = CSTRSAVE("PA_INFO_DEFAULT");
    PA_INFO_SHARE_S        = CSTRSAVE("PA_INFO_SHARE");
    PA_INFO_ATT_NAME_S     = CSTRSAVE("PA_INFO_ATT_NAME");
    PA_INFO_DIM_NAME_S     = CSTRSAVE("PA_INFO_DIM_NAME");
    PA_INFO_UNITS_NAME_S   = CSTRSAVE("PA_INFO_UNITS_NAME");
    PA_INFO_DOMAIN_NAME_S  = CSTRSAVE("PA_INFO_DOMAIN_NAME");
    PA_INFO_MAP_DOMAIN_S   = CSTRSAVE("PA_INFO_MAP_DOMAIN");
    PA_INFO_BUILD_DOMAIN_S = CSTRSAVE("PA_INFO_BUILD_DOMAIN");

/* do not delete any of these strings */
    SC_permanent(PA_INFO_TYPE_S);
    SC_permanent(PA_INFO_N_DIMS_S);
    SC_permanent(PA_INFO_DIMS_S);
    SC_permanent(PA_INFO_SCOPE_S);
    SC_permanent(PA_INFO_CLASS_S);
    SC_permanent(PA_INFO_CENTER_S);
    SC_permanent(PA_INFO_PERSISTENCE_S);
    SC_permanent(PA_INFO_ALLOCATION_S);
    SC_permanent(PA_INFO_FILE_NAME_S);
    SC_permanent(PA_INFO_INIT_VAL_S);
    SC_permanent(PA_INFO_INIT_FNC_S);
    SC_permanent(PA_INFO_CONV_S);
    SC_permanent(PA_INFO_UNIT_S);
    SC_permanent(PA_INFO_KEY_S);
    SC_permanent(PA_INFO_ATTRIBUTE_S);
    SC_permanent(PA_INFO_UNITS_S);
    SC_permanent(PA_INFO_DATA_PTR_S);
    SC_permanent(PA_INFO_UNIT_NUMER_S);
    SC_permanent(PA_INFO_UNIT_DENOM_S);
    SC_permanent(PA_INFO_APPL_ATTR_S);
    SC_permanent(PA_INFO_DEFAULT_S);
    SC_permanent(PA_INFO_SHARE_S);
    SC_permanent(PA_INFO_ATT_NAME_S);
    SC_permanent(PA_INFO_DIM_NAME_S);
    SC_permanent(PA_INFO_UNITS_NAME_S);
    SC_permanent(PA_INFO_DOMAIN_NAME_S);
    SC_permanent(PA_INFO_MAP_DOMAIN_S);
    SC_permanent(PA_INFO_BUILD_DOMAIN_S);

    PA_cpp_add_group(PA_CPP_INFO,        SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_ALLOCATION,  SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_SCOPE,       SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_CLASS,       SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_CENTER,      SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_PERSISTENCE, SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_UNITS,       SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_TYPE,        SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_TYPE_SC,     SC_INT_I, NULL);
    PA_cpp_add_group(PA_CPP_TYPE_S,      SC_STRING_I,  NULL);

/* install types in several ways so that the following will work:
 *   #define SC_DOUBLE_I   6
 *   char SC_DOUBLE_S = "double";
 *
 *   itype = *(int *) PA_cpp_name_to_value("SC_DOUBLE_I");  returns  7
 *   itype = *(int *) PA_cpp_name_to_value("double");       returns 7
 *   type  = (char *) PA_cpp_name_to_value("SC_DOUBLE_S");  returns "double"
 */

/* install the actual types */
    PA_cpp_add_name(SC_CHAR_S,      PA_CPP_TYPE_SC, SC_CHAR_I);
    PA_cpp_add_name(SC_SHORT_S,     PA_CPP_TYPE_SC, SC_SHORT_I);
    PA_cpp_add_name(SC_INT_S,       PA_CPP_TYPE_SC, SC_INT_I);
    PA_cpp_add_name(SC_LONG_S,      PA_CPP_TYPE_SC, SC_LONG_I);
    PA_cpp_add_name(SC_FLOAT_S,     PA_CPP_TYPE_SC, SC_FLOAT_I);
    PA_cpp_add_name(SC_DOUBLE_S,    PA_CPP_TYPE_SC, SC_DOUBLE_I);
    PA_cpp_add_name(SC_STRING_S,    PA_CPP_TYPE_SC, SC_STRING_I);
    PA_cpp_add_name(SC_POINTER_S,   PA_CPP_TYPE_SC, SC_POINTER_I);
    PA_cpp_add_name(SC_PCONS_P_S,   PA_CPP_TYPE_SC, SC_PCONS_P_I);
    PA_cpp_add_name(SC_VOID_S,      PA_CPP_TYPE_SC, SC_VOID_I);
    PA_cpp_add_name(SC_SHORT_P_S,   PA_CPP_TYPE_SC, SC_SHORT_I);
    PA_cpp_add_name(SC_INT_P_S,     PA_CPP_TYPE_SC, SC_INT_P_I);
    PA_cpp_add_name(SC_LONG_P_S,    PA_CPP_TYPE_SC, SC_LONG_P_I);
    PA_cpp_add_name(SC_FLOAT_P_S,   PA_CPP_TYPE_SC, SC_FLOAT_P_I);
    PA_cpp_add_name(SC_DOUBLE_P_S,  PA_CPP_TYPE_SC, SC_DOUBLE_P_I);
    PA_cpp_add_name(SC_PCONS_S,     PA_CPP_TYPE_SC, SC_PCONS_I);
    PA_cpp_add_name(SC_STRUCT_S,    PA_CPP_TYPE_SC, SC_STRUCT_I);
    PA_cpp_add_name(SC_UNKNOWN_S,   PA_CPP_TYPE_SC, SC_UNKNOWN_I);

    PA_cpp_add_name("SC_CHAR_I",      PA_CPP_TYPE, SC_CHAR_I);
    PA_cpp_add_name("SC_SHORT_I",     PA_CPP_TYPE, SC_SHORT_I);
    PA_cpp_add_name("SC_INT_I",       PA_CPP_TYPE, SC_INT_I);
    PA_cpp_add_name("SC_LONG_I",      PA_CPP_TYPE, SC_LONG_I);
    PA_cpp_add_name("SC_FLOAT_I",     PA_CPP_TYPE, SC_FLOAT_I);
    PA_cpp_add_name("SC_DOUBLE_I",    PA_CPP_TYPE, SC_DOUBLE_I);
    PA_cpp_add_name("SC_REAL_I",      PA_CPP_TYPE, SC_REAL_I);
    PA_cpp_add_name("SC_STRING_I",    PA_CPP_TYPE, SC_STRING_I);
    PA_cpp_add_name("SC_POINTER_I",   PA_CPP_TYPE, SC_POINTER_I);
    PA_cpp_add_name("SC_PCONS_P_I",   PA_CPP_TYPE, SC_PCONS_P_I);
    PA_cpp_add_name("SC_VOID_I",      PA_CPP_TYPE, SC_VOID_I);
    PA_cpp_add_name("SC_SHORT_P_I",   PA_CPP_TYPE, SC_SHORT_P_I);
    PA_cpp_add_name("SC_INT_P_I",     PA_CPP_TYPE, SC_INT_P_I);
    PA_cpp_add_name("SC_LONG_P_I",    PA_CPP_TYPE, SC_LONG_P_I);
    PA_cpp_add_name("SC_FLOAT_P_I",   PA_CPP_TYPE, SC_FLOAT_P_I);
    PA_cpp_add_name("SC_REAL_P_I",    PA_CPP_TYPE, SC_REAL_P_I);
    PA_cpp_add_name("SC_DOUBLE_P_I",  PA_CPP_TYPE, SC_DOUBLE_P_I);
    PA_cpp_add_name("SC_PCONS_I",     PA_CPP_TYPE, SC_PCONS_I);
    PA_cpp_add_name("SC_STRUCT_I",    PA_CPP_TYPE, SC_STRUCT_I);
    PA_cpp_add_name("SC_UNKNOWN_I",   PA_CPP_TYPE, SC_UNKNOWN_I);

/* install the types as strings */
    PA_cpp_add_name("SC_CHAR_S",      PA_CPP_TYPE_S, SC_CHAR_S);
    PA_cpp_add_name("SC_SHORT_S",     PA_CPP_TYPE_S, SC_SHORT_S);
    PA_cpp_add_name("SC_INT_S",       PA_CPP_TYPE_S, SC_INT_S);
    PA_cpp_add_name("SC_LONG_S",      PA_CPP_TYPE_S, SC_LONG_S);
    PA_cpp_add_name("SC_FLOAT_S",     PA_CPP_TYPE_S, SC_FLOAT_S);
    PA_cpp_add_name("SC_DOUBLE_S",    PA_CPP_TYPE_S, SC_DOUBLE_S);
    PA_cpp_add_name("SC_REAL_S",      PA_CPP_TYPE_S, SC_REAL_S);
    PA_cpp_add_name("SC_STRING_S",    PA_CPP_TYPE_S, SC_STRING_S);
    PA_cpp_add_name("SC_POINTER_S",   PA_CPP_TYPE_S, SC_POINTER_S);
    PA_cpp_add_name("SC_PCONS_P_S",   PA_CPP_TYPE_S, SC_PCONS_P_S);
    PA_cpp_add_name("SC_VOID_S",      PA_CPP_TYPE_S, SC_VOID_S);
    PA_cpp_add_name("SC_SHORT_P_S",   PA_CPP_TYPE_S, SC_SHORT_S);
    PA_cpp_add_name("SC_INT_P_S",     PA_CPP_TYPE_S, SC_INT_P_S);
    PA_cpp_add_name("SC_LONG_P_S",    PA_CPP_TYPE_S, SC_LONG_P_S);
    PA_cpp_add_name("SC_FLOAT_P_S",   PA_CPP_TYPE_S, SC_FLOAT_P_S);
    PA_cpp_add_name("SC_REAL_P_S",    PA_CPP_TYPE_S, SC_REAL_P_S);
    PA_cpp_add_name("SC_DOUBLE_P_S",  PA_CPP_TYPE_S, SC_DOUBLE_P_S);
    PA_cpp_add_name("SC_PCONS_S",     PA_CPP_TYPE_S, SC_PCONS_S);
    PA_cpp_add_name("SC_STRUCT_S",    PA_CPP_TYPE_S, SC_STRUCT_S);
    PA_cpp_add_name("SC_UNKNOWN_S",   PA_CPP_TYPE_S, SC_UNKNOWN_S);

    PA_cpp_add_name("PA_INFO_TYPE",         PA_CPP_INFO, PA_INFO_TYPE);
    PA_cpp_add_name("PA_INFO_N_DIMS",       PA_CPP_INFO, PA_INFO_N_DIMS);
    PA_cpp_add_name("PA_INFO_DIMS",         PA_CPP_INFO, PA_INFO_DIMS);
    PA_cpp_add_name("PA_INFO_SCOPE",        PA_CPP_INFO, PA_INFO_SCOPE);
    PA_cpp_add_name("PA_INFO_CLASS",        PA_CPP_INFO, PA_INFO_CLASS);
    PA_cpp_add_name("PA_INFO_CENTER",       PA_CPP_INFO, PA_INFO_CENTER);
    PA_cpp_add_name("PA_INFO_PERSISTENCE",  PA_CPP_INFO, PA_INFO_PERSISTENCE);
    PA_cpp_add_name("PA_INFO_ALLOCATION",   PA_CPP_INFO, PA_INFO_ALLOCATION);
    PA_cpp_add_name("PA_INFO_FILE_NAME",    PA_CPP_INFO, PA_INFO_FILE_NAME);
    PA_cpp_add_name("PA_INFO_INIT_VAL",     PA_CPP_INFO, PA_INFO_INIT_VAL);
    PA_cpp_add_name("PA_INFO_INIT_FNC",     PA_CPP_INFO, PA_INFO_INIT_FNC);
    PA_cpp_add_name("PA_INFO_CONV",         PA_CPP_INFO, PA_INFO_CONV);
    PA_cpp_add_name("PA_INFO_UNIT",         PA_CPP_INFO, PA_INFO_UNIT);
    PA_cpp_add_name("PA_INFO_KEY",          PA_CPP_INFO, PA_INFO_KEY);
    PA_cpp_add_name("PA_INFO_ATTRIBUTE",    PA_CPP_INFO, PA_INFO_ATTRIBUTE);
    PA_cpp_add_name("PA_INFO_UNITS",        PA_CPP_INFO, PA_INFO_UNITS);
    PA_cpp_add_name("PA_INFO_DATA_PTR",     PA_CPP_INFO, PA_INFO_DATA_PTR);
    PA_cpp_add_name("PA_INFO_UNIT_NUMER",   PA_CPP_INFO, PA_INFO_UNIT_NUMER);
    PA_cpp_add_name("PA_INFO_UNIT_DENOM",   PA_CPP_INFO, PA_INFO_UNIT_DENOM);
    PA_cpp_add_name("PA_INFO_APPL_ATTR",    PA_CPP_INFO, PA_INFO_APPL_ATTR);
    PA_cpp_add_name("PA_INFO_DEFAULT",      PA_CPP_INFO, PA_INFO_DEFAULT);
    PA_cpp_add_name("PA_INFO_SHARE",        PA_CPP_INFO, PA_INFO_SHARE);
    PA_cpp_add_name("PA_INFO_ATT_NAME",     PA_CPP_INFO, PA_INFO_ATT_NAME);
    PA_cpp_add_name("PA_INFO_DIM_NAME",     PA_CPP_INFO, PA_INFO_DIM_NAME);
    PA_cpp_add_name("PA_INFO_UNITS_NAME",   PA_CPP_INFO, PA_INFO_UNITS_NAME);
    PA_cpp_add_name("PA_INFO_DOMAIN_NAME",  PA_CPP_INFO, PA_INFO_DOMAIN_NAME);
    PA_cpp_add_name("PA_INFO_MAP_DOMAIN",   PA_CPP_INFO, PA_INFO_MAP_DOMAIN);
    PA_cpp_add_name("PA_INFO_BUILD_DOMAIN", PA_CPP_INFO, PA_INFO_BUILD_DOMAIN);

    PA_cpp_add_name("DEFN",    PA_CPP_SCOPE, DEFN);
    PA_cpp_add_name("RESTART", PA_CPP_SCOPE, RESTART);
    PA_cpp_add_name("DMND",    PA_CPP_SCOPE, DMND);
    PA_cpp_add_name("RUNTIME", PA_CPP_SCOPE, RUNTIME);
    PA_cpp_add_name("EDIT",    PA_CPP_SCOPE, EDIT);
    PA_cpp_add_name("SCRATCH", PA_CPP_SCOPE, SCRATCH);

    PA_cpp_add_name("REQU",    PA_CPP_CLASS, REQU);
    PA_cpp_add_name("OPTL",    PA_CPP_CLASS, OPTL);
    PA_cpp_add_name("DMND",    PA_CPP_CLASS, DMND);
    PA_cpp_add_name("PSEUDO",  PA_CPP_CLASS, PSEUDO);

    PA_cpp_add_name("U_CENT",  PA_CPP_CENTER, U_CENT);
    PA_cpp_add_name("N_CENT",  PA_CPP_CENTER, N_CENT);
    PA_cpp_add_name("E_CENT",  PA_CPP_CENTER, E_CENT);
    PA_cpp_add_name("F_CENT",  PA_CPP_CENTER, F_CENT);
    PA_cpp_add_name("Z_CENT",  PA_CPP_CENTER, Z_CENT);

    PA_cpp_add_name("REL",     PA_CPP_PERSISTENCE, REL);
    PA_cpp_add_name("KEEP",    PA_CPP_PERSISTENCE, KEEP);
    PA_cpp_add_name("CACHE_F", PA_CPP_PERSISTENCE, CACHE_F);
    PA_cpp_add_name("CACHE_R", PA_CPP_PERSISTENCE, CACHE_R);

    PA_cpp_add_name("STATIC",  PA_CPP_ALLOCATION, STATIC);
    PA_cpp_add_name("DYNAMIC", PA_CPP_ALLOCATION, DYNAMIC);

/* the units macros are actually references to variables,
 * If the variable were to change values, (which it shouldn't)
 * These names would be wrong.
 */
    PA_cpp_add_name("RAD",     PA_CPP_UNITS, RAD);   /* PA_radian          */
    PA_cpp_add_name("STER",    PA_CPP_UNITS, STER);  /* PA_steradian       */
    PA_cpp_add_name("MOLE",    PA_CPP_UNITS, MOLE);  /* PA_mole            */
    PA_cpp_add_name("Q",       PA_CPP_UNITS, Q);     /* PA_electric_charge */
    PA_cpp_add_name("CM",      PA_CPP_UNITS, CM);    /* PA_cm              */
    PA_cpp_add_name("SEC",     PA_CPP_UNITS, SEC);   /* PA_sec             */
    PA_cpp_add_name("G",       PA_CPP_UNITS, G);     /* PA_gram            */
    PA_cpp_add_name("EV",      PA_CPP_UNITS, EV);    /* PA_eV              */
    PA_cpp_add_name("K",       PA_CPP_UNITS, K);     /* PA_kelvin          */
    PA_cpp_add_name("ERG",     PA_CPP_UNITS, ERG);   /* PA_erg             */
    PA_cpp_add_name("CC",      PA_CPP_UNITS, CC);    /* PA_cc              */

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

    type_name = PA_cpp_value_to_name(PA_CPP_TYPE, type);
    if (type_name == NULL)
       type_name = SC_UNKNOWN_S;

    return(type_name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/