/*
 * PANACE.H - header in support of PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */
 
#ifndef PCK_PANACEA

#include "cpyright.h"

#define PCK_PANACEA

#include "pdb.h"
#include "scope_dp.h"
#include "pgs.h"
#include "panacea_gen.h"
    
/*--------------------------------------------------------------------------*/

/*                           CONSTANT DEFINES                               */

/*--------------------------------------------------------------------------*/

#undef UNREFL
#define UNREFL     1
#undef REFL
#define REFL      -1

#undef SEC

#define DIMENSION   NULL
#define DONE        -3
#define PER         -2
#define UNITS       -1

#define N_ATTRIBUTES 10

#define RAD       PA_gs.radian
#define STER      PA_gs.steradian
#define MOLE      PA_gs.mole
#define Q         PA_gs.electric_charge
#define CM        PA_gs.cm
#define SEC       PA_gs.sec
#define G         PA_gs.gram
#define EV        PA_gs.eV
#define K         PA_gs.kelvin
#define ERG       PA_gs.erg
#define CC        PA_gs.cc

#define PA_INFO_TYPE_S          PA_gs.inames[PA_INFO_TYPE]
#define PA_INFO_N_DIMS_S        PA_gs.inames[PA_INFO_N_DIMS]
#define PA_INFO_DIMS_S          PA_gs.inames[PA_INFO_DIMS]
#define PA_INFO_SCOPE_S         PA_gs.inames[PA_INFO_SCOPE]
#define PA_INFO_CLASS_S         PA_gs.inames[PA_INFO_CLASS]
#define PA_INFO_CENTER_S        PA_gs.inames[PA_INFO_CENTER]
#define PA_INFO_PERSISTENCE_S   PA_gs.inames[PA_INFO_PERSISTENCE]
#define PA_INFO_ALLOCATION_S    PA_gs.inames[PA_INFO_ALLOCATION]
#define PA_INFO_FILE_NAME_S     PA_gs.inames[PA_INFO_FILE_NAME]
#define PA_INFO_INIT_VAL_S      PA_gs.inames[PA_INFO_INIT_VAL]
#define PA_INFO_INIT_FNC_S      PA_gs.inames[PA_INFO_INIT_FNC]
#define PA_INFO_CONV_S          PA_gs.inames[PA_INFO_CONV]
#define PA_INFO_UNIT_S          PA_gs.inames[PA_INFO_UNIT]
#define PA_INFO_KEY_S           PA_gs.inames[PA_INFO_KEY]
#define PA_INFO_ATTRIBUTE_S     PA_gs.inames[PA_INFO_ATTRIBUTE]
#define PA_INFO_UNITS_S         PA_gs.inames[PA_INFO_UNITS]
#define PA_INFO_DATA_PTR_S      PA_gs.inames[PA_INFO_DATA_PTR]
#define PA_INFO_UNIT_NUMER_S    PA_gs.inames[PA_INFO_UNIT_NUMER]
#define PA_INFO_UNIT_DENOM_S    PA_gs.inames[PA_INFO_UNIT_DENOM]
#define PA_INFO_APPL_ATTR_S     PA_gs.inames[PA_INFO_APPL_ATTR]
#define PA_INFO_DEFAULT_S       PA_gs.inames[PA_INFO_DEFAULT]
#define PA_INFO_SHARE_S         PA_gs.inames[PA_INFO_SHARE]
#define PA_INFO_ATT_NAME_S      PA_gs.inames[PA_INFO_ATT_NAME]
#define PA_INFO_DIM_NAME_S      PA_gs.inames[PA_INFO_DIM_NAME]
#define PA_INFO_UNITS_NAME_S    PA_gs.inames[PA_INFO_UNITS_NAME]
#define PA_INFO_DOMAIN_NAME_S   PA_gs.inames[PA_INFO_DOMAIN_NAME]
#define PA_INFO_MAP_DOMAIN_S    PA_gs.inames[PA_INFO_MAP_DOMAIN]
#define PA_INFO_BUILD_DOMAIN_S  PA_gs.inames[PA_INFO_BUILD_DOMAIN]

/*--------------------------------------------------------------------------*/

/*
 * #bind enum PA_scope_kind RUNTIME
 */

enum e_PA_scope_kind
   {DEFN    = -1,
    RESTART = -2,
    DMND    = -3,
    RUNTIME = -4,
    EDIT    = -5,
    SCRATCH = -6};
typedef enum e_PA_scope_kind PA_scope_kind;

#define SCOPE     'a'

/*
 * #bind enum PA_class_kind REQU
 */

#undef REQU
#undef OPTL

enum e_PA_class_kind
   {REQU   = 1,
    OPTL   = 2,
    PSEUDO = 3};

typedef enum e_PA_class_kind PA_class_kind;

#define CLASS     'b'

/*
 * #bind enum PA_persist_kind REL
 */

enum e_PA_persist_kind
   {REL     = -10,
    KEEP    = -11,
    CACHE_F = -12,
    CACHE_R = -13};

typedef enum e_PA_persist_kind PA_persist_kind;

#define PERSIST   'c'


/* CENTER - centering is already defined */

#define CENTER   'd'

/*
 * #bind enum PA_allocation_kind REL
 */

enum e_PA_allocation_kind
   {STATIC  = -100,
    DYNAMIC = -101};

typedef enum e_PA_allocation_kind PA_allocation_kind;

#define ALLOCATION 'e'

/*
 * #bind enum PA_unit_conversion INT_CGS
 */

#undef NONE

enum e_PA_unit_conversion
   {INT_CGS = -20,
    INT_EXT = -21,
    EXT_CGS = -22,
    EXT_INT = -23,
    CGS_INT = -24,
    CGS_EXT = -25,
    NONE    = -26};

typedef enum e_PA_unit_conversion PA_unit_conversion;

#define ATTRIBUTE 'f'

/*--------------------------------------------------------------------------*/

#define NO_VALUE_I 0x80000000

#define PA_FORCE  -32765
#define PA_DELAY  -32766

#define PA_UPPER_LOWER    0xFFFF
#define PA_OFFSET_NUMBER  0xFFFE

#define PA_scale_array   PM_array_scale
#define PA_array_set     PM_array_set
#define PA_copy_array    PM_array_copy
#define PA_index_min     PM_index_min
#define PA_find_index    PM_find_index
#define PA_max_value     PM_max_value

#define PA_flush         PD_flush
#define PA_family        PD_family

#define PA_advance_name  SC_advance_name
#define PA_assoc         SC_assoc

#define PA_add_alist(a, n, v)     SC_add_alist(a, n, NULL, v)

#define PA_complete_premap PA_get_range_info

#define PA_ERR    (*PA_gs.error_hook)
#define PA_WARN   (*PA_gs.warn_hook)

enum e_PA_medium
   {PA_FILE     = -1,
    PA_DATABASE = -2};

typedef enum e_PA_medium PA_medium;

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* PA_GET_FUNCTION - return a pointer to the named function */

#define PA_GET_FUNCTION(type, name)                                          \
    SC_HASHARR_LOOKUP_FUNCTION(PA_gs.symbol_tab, type, name)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_PCK_FUNCTION - return a pointer to the given package function */

#define PA_GET_PCK_FUNCTION(pck, type, name)                                 \
    SC_ASSOC_FUNCTION(PA_PACKAGE_ATTR(pck), type, name)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_VAR_FUNCTION - return a pointer to the given variable function */

#define PA_GET_VAR_FUNCTION(pp, type, name)                                  \
    SC_ASSOC_FUNCTION(PA_VARIABLE_ATTR(pp), type, name)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTERN_V - intern a given scalar value in the variable data base */

#define PA_INTERN_V(var, name)                                               \
    PA_intern((void *) &var, name)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTERN - intern a given array in the variable data base */

#define PA_INTERN(var, name)                                                 \
    PA_intern((void *) var, name)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTERN_PTR - intern the given array in the variable data base
 *               - this is necessary to ensure that a pointer to the type
 *               - associated with the variable is handed off to PDBLib
 *               - e.g.  char **s -> .., "char **", &s, ...
 */

#define PA_INTERN_PTR(var, name, type)                                       \
    var = (type *) *PA_intern((void *) &var, name)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CONNECT - connect a given pointer to the named data */

#define PA_CONNECT(var, name, track)                                         \
    PA_get_access((void **) &var, name, -1L, 0L, track)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_LOAD_VARIABLE - load/init the variable data into its memory space
 *                  - intended for STATIC variables
 */

#define PA_LOAD_VARIABLE(name) PA_load_data(name)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DISCONNECT - disconnect a given pointer from the named data */

#define PA_DISCONNECT(name, ptr)                                             \
    PA_rel_access((void **) &ptr, name, -1L, 0L)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ACCESS - return a connection to a sub-space of an array */

#define PA_ACCESS(var, name, type, offs, ne, track)                          \
   (type) PA_get_access((void **) &var, name, offs, ne, track)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_RELEASE - release the access to an array sub-space */

#define PA_RELEASE(name, ptr, offs, ne)                                      \
    PA_rel_access(&(void *) ptr, name, offs, ne)
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_VAR_DIM - return the number of elements
 *            - implied by the dimension range
 */

#define PA_VAR_DIM(dm)                                                       \
    ((((dm)->method) == PA_UPPER_LOWER) ?                                    \
     ((long) (*((dm)->index_max) - *((dm)->index_min) + 1)) :                \
     ((long) *((dm)->index_max)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_LST_VAL - access the first integer element of the list */

#define PA_LST_VAL(lst)  (*((int *) (lst)->car))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PR_RANGE_SIZE - reset the size of the plot request range */

#define PA_PR_RANGE_SIZE(pr, n)                                              \
    {pr->size = n;                                                           \
     pr->data->n_elements = n;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_STASH_SP_SCALAR - save a scalar value for a spatial pseudo variable
 *                    - plot request
 */

#define PA_STASH_SP_SCALAR(name, indx, val)                                  \
    if (strcmp(vr, name) == 0)                                               \
       {data[indx] += (val)*pr->conv;                                        \
        continue;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_STASH_TV_SCALAR - save a scalar value for a time pseudo variable
 *                    - plot request
 */

#define PA_STASH_TV_SCALAR(name, val)                                        \
    if (strcmp(vr, name) == 0)                                               \
       {PA_gs.t_data[pr->str_index] += (val)*pr->conv;                       \
        continue;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_MESH_RANGE_SEARCH - loop the range specification of the plot request
 *                      - to setup an invokation of PA_STASH_SP_SCALAR
 */

#define PA_MESH_RANGE_SEARCH(pr, off, fn)                                    \
   {int nde;                                                                 \
    char *vr;                                                                \
    double **elem, *data;                                                    \
    PA_set_spec *rp;                                                         \
    PM_set *set;                                                             \
    set = pr->data;                                                          \
    PA_ERR((set == NULL), "BAD PLOT REQUEST %s - %s", pr->text, fn);         \
    elem = (double **) set->elements;                                        \
    nde  = 0;                                                                \
    for (rp = pr->range; rp != NULL; rp = rp->next)                          \
        {vr   = rp->var_name;                                                \
         data = elem[nde++] + off

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TIME_RANGE_SEARCH - loop the range specification of the plot request
 *                      - to setup an invokation of PA_STASH_TV_SCALAR
 */

#define PA_TIME_RANGE_SEARCH(pr, fn)                                         \
   {char *vr;                                                                \
    PA_set_spec *rp;                                                         \
    for (rp = pr->range; rp != NULL; rp = rp->next)                          \
        {vr = rp->var_name

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_END_MESH_SEARCH - gracefully terminate the loop started by
 *                    - PA_MESH_RANGE_SEARCH
 *                    - this is mainly for lexical clarity
 */

#define PA_END_MESH_SEARCH(pr, expr) }; expr;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_END_TIME_SEARCH - gracefully terminate the loop started by
 *                    - PA_TIME_RANGE_SEARCH
 *                    - this is purely for lexical clarity
 */

#define PA_END_TIME_SEARCH(pr) };}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_STORE_TV - save away the given time value data */

#define PA_STORE_TV(pr, val)                                                 \
   PA_gs.t_data[pr->str_index] = (val)*pr->conv

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_ALIAS - handle alias definitions a little bit more nicely */

#define PA_DEF_ALIAS(n, t, ts, v)                                            \
   {t x;                                                                     \
    x = (t) v;                                                               \
    PA_def_alias(n, ts, &x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INQUIRE_VARIABLE - return a PA_variable pointer from
 *                     - the PA_gs.variable_tab
 */

#define PA_INQUIRE_VARIABLE(x)                                               \
    ((PA_variable *) SC_hasharr_def_lookup(PA_gs.variable_tab, (x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INQUIRE_PACKAGE - return a PA_package pointer from
 *                    - the PA_gs.variable_tab
 */

#define PA_INQUIRE_PACKAGE(x)                                                \
    ((PA_package *) SC_hasharr_def_lookup(PA_gs.variable_tab, (x)))

/*--------------------------------------------------------------------------*/

#define PA_SET_MAX_NAME_SPACE(f)   PA_gs.name_spaces = f
#define PA_GET_MAX_NAME_SPACE(f)   f = PA_gs.name_spaces

#define PA_VARIABLE_NAME(x)             ((x)->name)
#define PA_VARIABLE_FILE(x)             ((x)->file)
#define PA_VARIABLE_FILE_CONVERS(x)     ((x)->file_conv)
#define PA_VARIABLE_CACHE_FILE_NAME(x)  ((x)->cache_file)

#define PA_VARIABLE_DATA(x)         ((x)->data)
#define PA_VARIABLE_INIT_VAL(x)     ((x)->init_val)
#define PA_VARIABLE_INIT_FUNC(x)    ((x)->init)
#define PA_VARIABLE_SIZE(x)         ((x)->size)

#define PA_VARIABLE_INT_UNIT(x)     ((x)->unit)
#define PA_VARIABLE_EXT_UNIT(x)     ((x)->conv)

#define PA_VARIABLE_DESC(x)         ((x)->desc)
#define PA_VARIABLE_TYPE(x)         ((x)->type)
#define PA_VARIABLE_TYPE_S(x)       ((x)->desc->type)
#define PA_VARIABLE_DIMS(x)         ((x)->dims)
#define PA_VARIABLE_N_DIMS(x)       ((x)->n_dimensions)

#define PA_VARIABLE_ACCESS_LIST(x)     ((x)->access_list)
#define PA_VARIABLE_REFERENCE_LIST(x)  ((x)->reference_list)

#define PA_VARIABLE_SCOPE(x)        ((x)->scope)
#define PA_VARIABLE_CLASS(x)        ((x)->option)
#define PA_VARIABLE_PERSISTENCE(x)  ((x)->persistence)
#define PA_VARIABLE_CENTERING(x)    ((x)->centering)
#define PA_VARIABLE_ALLOCATION(x)   ((x)->allocation)

#define PA_VARIABLE_PACKAGE(x)      ((x)->pck)
#define PA_VARIABLE_UNIT_NUMER(x)   ((x)->numer_list)
#define PA_VARIABLE_UNIT_DENOM(x)   ((x)->denom_list)
#define PA_VARIABLE_FILE_NAME(x)    ((x)->file_name)
#define PA_VARIABLE_ATTR(x)         ((x)->alist)

#define PA_PACKAGE_NAME(x)          ((x)->name)
#define PA_PACKAGE_SPACE(x)         ((x)->space)
#define PA_PACKAGE_TIME(x)          ((x)->time)
#define PA_PACKAGE_DT(x)            ((x)->dt)
#define PA_PACKAGE_DT_ZONE(x)       ((x)->dt_zone)
#define PA_PACKAGE_PLOTS(x)         ((x)->pseudo_pr)
#define PA_PACKAGE_ATTR(x)          ((x)->alist)
#define PA_PACKAGE_SWITCHES(x)      ((x)->iswtch)
#define PA_PACKAGE_PARAMS(x)        ((x)->rparam)
#define PA_PACKAGE_NAMES(x)         ((x)->ascii)
#define PA_PACKAGE_ON(x)            ((x)->on)
#define PA_PACKAGE_PROB_DT(x)       ((x)->p_dt)
#define PA_PACKAGE_PROB_T(x)        ((x)->p_t)

#define SWTCH  (PA_get_thread(-1)->iswtch)
#define PARAM  (PA_get_thread(-1)->rparam)
#define NAME   (PA_get_thread(-1)->aname)

/*--------------------------------------------------------------------------*/

/*                   TYPEDEF AND STRUCT DEFINITIONS                         */

/*--------------------------------------------------------------------------*/

typedef struct s_PA_dimens PA_dimens;
typedef struct s_PA_command PA_command;
typedef struct s_PA_package PA_package;
typedef struct s_PA_variable PA_variable;
typedef struct s_PA_plot_request PA_plot_request;
typedef struct s_PA_set_spec PA_set_spec;
typedef struct s_PA_src_variable PA_src_variable;
typedef struct s_PA_thread_state PA_thread_state;
typedef struct s_PA_iv_specification PA_iv_specification;
typedef struct s_PA_unit_spec PA_unit_spec;
typedef struct s_PA_set_index PA_set_index;
typedef struct s_PA_tab_head PA_tab_head;

typedef int (*PFPkgGencmd)(void);
typedef int (*PFPkgDfstrc)(PDBfile *pdrs);
typedef int (*PFPkgIntrn)(void);
typedef int (*PFPkgDefun)(PA_package *pck);
typedef int (*PFPkgDefvar)(PA_package *pck);
typedef int (*PFPkgDefcnt)(PA_package *pck);
typedef int (*PFPkgInizer)(PA_package *pck);
typedef int (*PFPkgMain)(PA_package *pck);
typedef int (*PFPkgFinzer)(PA_package *pck);
typedef int (*PFPkgPpcmd)(void);
typedef void (*PFPanHand)(PA_command *);
typedef PM_mapping *(*PFPkgPpsor)(PA_plot_request *pr, double t);

typedef void (*PFVarInit)(void *p, long sz, char *s);
typedef pcons *(*PFUserAttr)(int tag, va_list *a, pcons *alist2);
typedef int (*PFTimePlotP)(PA_plot_request *pr);
typedef void (*PFPreMap)(PA_plot_request *pr);
typedef PM_set *(*PFBuildDom)(const char *base, C_array *arr, double t);
typedef PM_mapping *(*PFBuildMap)(PA_plot_request *pr, double t);
typedef int (*PFNeedPVA)(void);
typedef int (*PFRegID)(char *s);
typedef void (*PFBaseName)(char *s);
typedef void (*PFGenErr)(char *token);
typedef void (*PFRegSetup)(void);
typedef void (*PFWriteState)(PDBfile *pdrs);
typedef void (*PFReadState)(PDBfile *pdrs);
typedef void (*PFDefTyp)(PDBfile *pdrs);
typedef int (*PFChangeDim)(char *pname, void *newsp, void *oldsp, long *, long *ndm, long *odm);
typedef void (*PFErrHand)(int, const char *, ...);
typedef void (*PFVString)(char *s);


struct s_PA_dimens
   {int *index_min;
    int *index_max;
    int method;
    PA_dimens *next;};

struct s_PA_command
   {char type;
    int *vr;
    int num;
    char *name;
    PFVoid proc;
    PFPanHand handler;};

struct s_PA_set_spec
   {char *var_name;
    char *function;
    char *text;
    int n_values;
    double *values;
    PA_set_spec *next;};

struct s_PA_plot_request
   {PA_set_spec *range;
    char *range_name;
    PA_set_spec *domain;
    char *base_domain_name;
    C_array *domain_map;         /* this is easy to work with at run time */
    char *text;
    int time_plot;                   /* flag to identify time/cycle plots */
    int mesh_plot;                    /* flag to identify full mesh plots */
    int status;              /* flag for PSEUDO vars for proper treatment */
    long size;              /* size of space for PSEUDO variables (range) */
    long offset;        /* offset into space for PSEUDO variables (range) */
    long stride;     /* stride through space for PSEUDO variables (range) */
    int str_index;          /* index into time data stripe for time plots */
    double conv;
    PM_centering centering;
    int allocate_data;                   /* permit set allocation if TRUE */
    PM_set *data;           /* storage space for PSEUDO variables (range) */
    int data_index;                   /* index into the source data array */ 
    int data_type;
    PA_plot_request *next;};


/* PANACEA package
 *
 * #bind struct PA_package
 */

struct s_PA_package
   {char *name;                                             /* package name */
    int on;                                      /* on/off flag for package */
    double p_t;                                             /* problem time */
    double p_dt;                                       /* problem time step */
    int p_cycle;                                           /* problem cycle */
    double time;                           /* cpu time spent in the package */
    double space;        /* bytes of dynamic memory required by the package */
    double dt;                           /* package vote for next time step */
    int dt_zone;                  /* zone controlling the package time step */
    PFPkgGencmd gencmd;    /* function which defines the generator commands */
    PFPkgDfstrc dfstrc;        /* function to define structures for rs-dump */
    PFPkgIntrn intrn;     /* function which interns variables at generation */
    PFPkgDefun defun;                   /* function which defines new units */
    PFPkgDefvar defvar;     /* function which defines the package variables */
    PFPkgDefcnt defcnt;      /* function which defines the package controls */
    PFPkgInizer inizer;           /* function which initializes the package */
    PFPkgMain main;                            /* main entry in the package */
    PFPkgPpsor ppsor;         /* function to handle package specific output */
    PFPkgFinzer finzer;            /* function which closes out the package */
    PFPkgPpcmd ppcmd; /* function which defines the post-processor commands */
    int n_swtch;                                      /* number of switches */
    int *iswtch;                     /* package control switches (integers) */
    int n_param;                                    /* number of parameters */
    double *rparam;                   /* package control parameters (reals) */
    int n_ascii;                                       /* number of strings */
    char **ascii;                   /* package control strings (characters) */
    PA_plot_request *pseudo_pr;        /* list of edits of pseudo variables */
    const char *db_file;         /* name of text file defining the database */
    pcons *db_list;               /* list of the database names from a file */
    pcons *alist;                        /* list of package level functions */
    PA_package *next;};                        /* point to the next package */


/* added a pointer to the number of elements so that variables could be
 * installed with zero length and when read from restart dumps, source
 * files, or the generator deck the size can be dynamically set
 * this also allows for error checking by comparing desc->number with
 * *size when both are non-zero
 */

/* PANACEA variable
 *
 * #bind struct PA_variable
 */

struct s_PA_variable
   {char *name;                  /* name by which the variable is installed */
    PA_package *pck;                /* the package which owns this variable */
    void *data;                               /* pointer to the actual data */
    defstr *type;
    long size;         /* pointer to the number of elements in the variable */
    int n_dimensions;                 /* the dimensionality of the variable */
    PA_dimens *dims;           /* pointer to the dimensions of the variable */
    int scope;                        /* DEFN, RESTART, DMND, RUNTIME, EDIT */
    int option;                                       /* REQU, OPTL, PSEUDO */
    int persistence;                         /* REL, KEEP, CACHE_F, CACHE_R */
    PM_centering centering;                /* FALSE for node, TRUE for zone */
    int allocation;                                    /* STATIC or DYNAMIC */
    double conv;                         /* external unit conversion factor */
    double unit;                         /* internal unit conversion factor */
    pcons *numer_list;                    /* list of units in the numerator */
    pcons *denom_list;                  /* list of units in the denominator */
    syment *desc;        /* information about the variable (see PDBLib Doc) */
    char *file_name;
    int file_conv;
    void *init_val;
    PFVarInit init;
    pcons *alist;
    pcons *access_list;
    pcons *reference_list;
    PDBfile *file;
    char *cache_file;};


/* PANACEA source variable
 *
 * #bind struct PA_src_variable
 */

struct s_PA_src_variable
   {const char *name;                       /* variable name (with indexes) */
    PA_variable *pp;                        /* associated database variable */
    int var_index;    /* an index into times identifying the last read time */
    int n_times;             /* the number of times for which there is data */
    double conv_fact;       /* conversion factor from CGS to INTERNAL units */
    double *times;                      /* the times at which there is data */
    double *queue_times;              /* the times of the data in the queue */
    double **queue;                  /* a queue buffer for some of the data */
    int index;        /* an index into times identifying the last read time */
    int size;                        /* the size of the arrays in the queue */
    int interpolate;       /* flag to indicate the need to interpolate data */
    PDBfile *file;};                 /* the source file containing the data */

struct s_PA_thread_state
   {double mark_space;
    double mark_time;
    int *iswtch;            
    double *rparam;
    char **aname;
    PA_package *current;

/* PACCESS.C vars */
   int ic;                          /* cache file index */
   char cache_fname[MAXLINE];};     /* cache file name */

/* PA_IV_SPECIFICATION
 *
 * #bind struct PA_iv_specification
 */

struct s_PA_iv_specification
   {int type;
    char *name;                                /* name of the specification */
    char *file;                   /* file from which to get the data if any */
    int num;                               /* number of data points in data */
    int index;            /* an index into data - used at runtime if needed */
    int interpolate;       /* flag to indicate the need to interpolate data */
    pcons *spec;                    /* list of data read at generation time */
    double *data;                      /* runtime storage of data from spec */
    PA_iv_specification *next;};

/* UNIT_SPEC - specify information for building units out of other
 *           - existing units
 */

struct s_PA_unit_spec
   {int index;
    double factor;
    int fundamental;
    int n_num;
    int *numerator;
    int n_den;
    int *denominator;
    PA_unit_spec *next;};

struct s_PA_set_index
   {double val;
    int index;
    int imax;
    double conv;
    char *name;};

struct s_PA_tab_head
   {char *name;
    char *type;
    pcons *entries;
    long n_entries;
    long n_fields;};

enum e_PA_info_tag
   {LAST_TAG = 0,
    PA_INFO_TYPE,
    PA_INFO_N_DIMS,
    PA_INFO_DIMS,
    PA_INFO_SCOPE,
    PA_INFO_CLASS,
    PA_INFO_CENTER,
    PA_INFO_PERSISTENCE,
    PA_INFO_ALLOCATION,
    PA_INFO_FILE_NAME,
    PA_INFO_INIT_VAL,
    PA_INFO_INIT_FNC,
    PA_INFO_CONV,
    PA_INFO_UNIT,
    PA_INFO_KEY,
    PA_INFO_ATTRIBUTE,
    PA_INFO_UNITS,
    PA_INFO_DATA_PTR,
    PA_INFO_UNIT_NUMER,
    PA_INFO_UNIT_DENOM,
    PA_INFO_APPL_ATTR,
    PA_INFO_DEFAULT,
    PA_INFO_SHARE,
    PA_INFO_ATT_NAME,
    PA_INFO_DIM_NAME,
    PA_INFO_UNITS_NAME,
    PA_INFO_DOMAIN_NAME,
    PA_INFO_MAP_DOMAIN,
    PA_INFO_BUILD_DOMAIN,
    PA_INFO_N_ENTRIES};

typedef enum e_PA_info_tag PA_info_tag;

typedef struct s_PA_scope_public PA_scope_public;

struct s_PA_scope_public
   {

/* variables with non-zero value initialization */
    int current_pp_cycle;

    char *token_delimiters;

    PFErrHand error_hook;
    PFErrHand warn_hook;

/* variables with zero value initialization */
    int n_graphs;
    int n_plots;
    int n_packages;
    int n_switches;
    int n_parameters;
    int n_names;
    int n_units;
    int n_variables;
    int n_time_plots;
    int n_unique_variables;
    int name_spaces;
    int *dul;
    int *don;
    int radian;
    int steradian;
    int mole;
    int electric_charge;
    int cm;
    int sec;
    int gram;
    int eV;
    int kelvin;
    int erg;
    int cc;
    int stand_alone;                             /* stand alone flag */
    int *global_swtch;
    
    double *convrsns;
    double *global_param;
    double *t_data;
    double *units;

    char errbuf[MAXLINE];       /* global buffer for certain error messages */
    char err[MAXLINE];
    char *input_prompt;
    char **global_name;
    char *strtok_p;
    char *command;
    char *cpp_node;
    char *edit;
    char *plot_req;
    char *package;
    char *source;
    char *variable;
    char *attribute;
    char *dimension;
    char *domain;
    char *edit_REQUEST;
    char *edit_OUT;
    char *unit;
    char *inames[PA_INFO_N_ENTRIES];
    char *cpp_info;
    char *cpp_allocation;
    char *cpp_scope;
    char *cpp_class;
    char *cpp_center;
    char *cpp_persistence;
    char *cpp_units;
    char *cpp_type;
    char *cpp_type_SC;
    char *cpp_type_S;

    hasharr *alias_tab;
    hasharr *command_tab;
    hasharr *symbol_tab;
    hasharr *variable_tab;
    hasharr *var_att_tab;
    hasharr *var_def_tab;
    hasharr *var_dim_tab;
    hasharr *var_domain_tab;
    hasharr *var_unit_tab;
    hasharr *cpp_name_tab;
    hasharr *cpp_value_tab;

    PDBfile *cache_file;
    PDBfile *vif;
    PDBfile *pva_file;
    PDBfile *pp_file;

    PA_iv_specification *iv_spec_lst;
    PA_package *packages;
    PA_plot_request *plot_reqs;
    PA_src_variable **sv_list;

    PROCESS *pp;
    FILE *edit_file;};


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern PA_scope_public
 PA_gs;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* PAPKG.C declarations */

extern void
 PA_gencmd(void),
 PA_defunc(void),
 PA_defcnt(void);


/* PACCESS.C declarations */

extern void
 *PA_load_data(const char *s),
 *PA_get_access(void **vr, const char *sn, long offs, long ne, int track),
 PA_rel_access(void **vp, const char *s, long offs, long ne),
 PA_equivalence(void *vr, const char *s),
 PA_set_equiv(const char *s, void *vl),
 PA_init_scalar(const char *s);

extern PA_variable
 *PA_inquire_variable(const char *x);


/* PADSYS.C declarations */

extern PA_package
 *PA_current_package(void),
 *PA_gen_package(const char *name, PFPkgGencmd cmd, PFPkgDfstrc dfs,
                 PFPkgDefun dfu, PFPkgDefvar dfr, PFPkgDefcnt cnt,
                 PFPkgIntrn inr, const char *fname);

extern void
 PA_run_time_package(const char *name, PFPkgDfstrc dfs, PFPkgDefun dfu,
                     PFPkgDefvar dfr, PFPkgDefcnt cnt, PFPkgInizer izr,
                     PFPkgMain mn, PFPkgPpsor psr, PFPkgFinzer fzr,
                     const char *fname),
 PA_def_package(const char *name, PFPkgGencmd gcmd, PFPkgDfstrc dfs,
                PFPkgDefun dfu, PFPkgDefvar dfr, PFPkgDefcnt cnt,
                PFPkgIntrn inr, PFPkgInizer izr, PFPkgMain mn,
                PFPkgPpsor psr, PFPkgFinzer fzr, PFPkgPpcmd pcmd,
                const char *fname),
 PA_control_set(const char *s);


/* PADUMP.C declarations */

extern int
 PA_dump_time(PA_set_spec *pi, double tc, double dtc, int cycle);

extern void
 PA_dump_pp(double t, double dt, int cycle),
 PA_get_range_info(PA_plot_request *pr),
 PA_put_set(PDBfile *file, PM_set *s),
 PA_put_picture(PG_device *dev, PDBfile *file, PM_mapping *f,
		PG_rendering ptyp, int bndp, int cbnd, int sbnd,
		double wbnd, int mshp, int cmsh, int smsh, double wmsh),
 PA_put_mapping(PG_device *dev, PDBfile *file, PM_mapping *f,
		PG_rendering ptyp);

extern double
 *PA_set_data(const char *name, C_array *arr, PM_centering *pcent),
 *PA_fill_component(double *data, int len, int *pist, int ne);

extern PM_mapping
 *PA_build_mapping(PA_plot_request *pr,
		   PM_set *(*build_ran)(PA_plot_request *pr,
					const char *name),
		   double t);

extern PA_set_spec
 *PA_non_time_domain(PA_plot_request *pr);

extern C_array
 *PA_get_domain_info(PA_plot_request *pr, char *dname, int nc);


/* PAFI.C declarations */

extern int
 PA_connect_ptrs_p(const char *name, char **pvn, void **pp, int *pn);

extern void
 PA_read_def(PA_package *pck, const char *name),
 PA_install_function(const char *s, PFVoid fnc),
 PA_install_identifier(const char *s, void *vr),
 PA_add_hook(const char *name, void *fnc);


/* PAGNRD.C declarations */

extern hasharr
 *PA_inst_com(void);

extern int
 PA_function_form(const char *t, PA_set_spec *spec);

extern void
 PA_inst_pck_gen_cmmnds(void),
 PA_intern_pck_db(void),
 PA_def_alias(const char *name, const char *type, void *pv),
 PA_inst_c(const char *cname, void *cvar, int ctype, int cnum,
           PFVoid cproc, PFPanHand chand),
 PA_defh(void),
 PA_specifyh(void),
 PA_sh(void),
 PA_proc_iv_spec(PA_iv_specification *lst),
 PA_clear(void),
 PA_packh(void),
 PA_pshand(PA_command *cp),
 PA_nploth(void),
 PA_time_plot(const char *rname, void *vr),
 PA_wrrstrth(void),
 PA_name_files(const char *base_name, char **ped, char **prs,
	       char **ppp, char **pgf),
 PA_read_file(const char *str, int sfl),
 PA_readh(const char *str),
 PA_get_commands(FILE *fp, PFVString errfnc),
 PA_done(void);

extern double
 PA_get_num_field(const char *s, const char *t, int optp),
 PA_alias_value(const char *s);

extern char
 *PA_get_next_line(void),
 *PA_get_field(const char *s, const char *t, int optp);


/* PAHAND.C declarations */

extern void
 PA_zargs(PA_command *cp),
 PA_sargs(PA_command *cp),
 PA_strarg(PA_command *cp);


/* PAMM.C declarations */

extern void
 PA_mk_control(PA_package *pck, const char *s, int na, int np, int ns),
 PA_mark_space(PA_package *pck),
 PA_accm_space(PA_package *pck),
 PA_mark_time(PA_package *pck),
 PA_accm_time(PA_package *pck);

extern PA_iv_specification
 *PA_mk_spec(const char *id, int type, const char *fn, int n, int interp,
	     pcons *lst, PA_iv_specification *nxt);


/* PANACEA.C declarations */

extern void
 PA_simulate(double tc, int nc, int nz, double ti, double tf,
	     double dtf_init, double dtf_min, double dtf_max, double dtf_inc,
	     const char *rsname, char *edname,
	     char *ppname, char *gfname),
 PA_init_system(double t, double dt, int nc,
		const char *edname, const char *ppname, const char *gfname),
 PA_run_packages(double t, double dt, int cycle),
 PA_fin_system(int nz, int nc, int silent),
 PA_terminate(const char *edname, const char *ppname,
	      const char *gfname, int cycle);

extern double
 PA_advance_t(double dtmn, double dtn, double dtmx);


/* PANTH.C declarations */

extern int
 PA_th_write(PDBfile *strm, const char *name, const char *type,
	     int inst, int nr, void *vr),
 PA_th_wr_iattr(PDBfile *strm, const char *vr, int inst,
		const char *attr, void *avl),
 PA_th_transpose(const char *name, int ncpf),
 PA_th_trans_family(const char *name, int ord, int ncpf),
 PA_th_family_list(const char *name, int c, char ***pfiles),
 PA_th_trans_name(int n, char **names, int ord, int ncpf),
 PA_th_name_list(int n, char **names, char ***pthfiles),
 PA_th_trans_link(int n, char **names, int ord, int ncpf),
 PA_th_link_list(int n, char **names, char ***pthfiles),
 PA_th_trans_files(const char *name, int ncpf, int nthf,
		   char **thfiles, int ord, int flag),
 PA_merge_family(const char *base, const char *family, int ncpf),
 PA_merge_files(const char *base, int n, char **files, int ncpf),
 PA_th_wr_member(PDBfile *strm, const char *name, const char *member,
		 const char *type, int inst, void *vr);

extern defstr
 *PA_th_def_rec(PDBfile *file, const char *name, const char *type,
		int nmemb, char **members, char **labels);

extern PDBfile
 *PA_th_open(const char *name, const char *mode, long size,
	     const char *prev),
 *PA_th_family(PDBfile *file);


/* PAPP.C declarations */

extern void
 PA_transpose_pp(const char *ppname, int ntp, int nuv);


/* PASHAR.C declarations */

extern int
 PA_set_default_offset(int d),
 PA_get_default_offset(void);

extern void
 PA_def_var(const char *vname, const char *vtype, void *viv,
	    PFVarInit vif, ...),
 PA_inst_var(const char *vname, const char *vtype, void *viv,
	     PFVarInit vif, ...),
 PA_inst_scalar(const char *vname, const char *vtype,
		void *vaddr, void *viv, PFVarInit vif, ...),
 PA_error_handler(int test, const char *fmt, ...),
 PA_warning_handler(int test, const char *fmt, ...),
 PA_error(const char *msg);

extern PDBfile
 *PA_open(const char *name, const char *mode, int flag);


/* PASHAS.C declarations */

extern void
 PA_trap_signals(int nsig, ...),
 PA_signal_handler(int sig),
 PA_interrupt_handler(int sig),
 PA_file_mon(char *edname, char *ppname, char *gfname),
 PA_rd_restart(const char *rsname, int convs),
 PA_wr_restart(char *rsname),
 PA_close_pp(void),
 PA_init_strings(void),
 PA_change_dim(int *pdm, int val),
 PA_change_size(const char *name, int flag),
 *PA_intern(void *vr, const char *name);

extern int
 PA_def_str(PDBfile *pdrs),
 PA_sizeof(const char *s);


/* PASMP.C declarations */

extern PA_thread_state
 *PA_get_thread(int id);

extern void
 PA_init_threads(int nt, PFTid tid),
 PA_do_threads(void *(*fnc)(void), void **ret),
 PA_set_n_threads(int n);

extern int
 PA_current_thread(void),
 PA_get_n_threads(void);


/* PASRC.C declartions */

extern PA_src_variable
 *PA_get_source(const char *s, int start_flag);

extern PA_iv_specification 
 *PA_get_iv_source(const char *name),
 *PA_find_iv_source(const char *name, int off);

extern double
 PA_intr_spec(PA_iv_specification *sp, double t, double val, long off);

extern void
 PA_interp_src(void *v, PA_src_variable *svp, int ni, int nf,
	       double t, double dt),
 PA_source_variables(double t, double dt);


/* PASTR.C declarations */

extern void
 *PA_mk_instance(const char *name, const char *type, long n),
 *PA_get_member(const char *name, const char *member);

extern void
 PA_rl_instance(const char *name),
 PA_set_member(const char *name, void *data, const char *member);

extern syment
 *PA_int_instance(const char *name, const char *type, long n, void *data);


/* PAVARS.C declarations */

extern void
 PA_sub_select(PA_variable *pp, C_array *arr, long *pitems,
	       long *poffs, long *pstr),
 PA_general_select(PA_variable *pp, C_array *arr,
		   unsigned long *pitems, unsigned long *pdims,
		   unsigned long *poffs, unsigned long *pstr,
		   unsigned long *pmax),
 PA_definitions(void),
 PA_variables(int flag),
 PA_proc_units(void),
 PA_def_units(int flag),
 PA_set_conversions(int flag),
 PA_physical_constants_int(void),
 PA_physical_constants_ext(void),
 PA_physical_constants_cgs(void);

extern int
 PA_def_unit(double fac, ...);


/* PADEF.C declarations */

extern void
 PA_def_var_init(void),
 PA_def_var_domain(const char *name, ...);

extern PA_variable
 *PA_def_variable(const char *name, ...),
 *PA_def_var_default(const char *name, ...);

extern PA_dimens
 *PA_def_var_dimension(const char *name, ...);

extern pcons
 *PA_var_to_alist(PA_variable *pp),
 *PA_def_var_attribute(const char *name, ...),
 *PA_def_var_units(const char *name, ...);

extern hasharr
 *PA_install_table(const char *s, void *vr, const char *type, hasharr *tab);


/* PACPP.C */

extern int
 PA_cpp_symbol_itype(const char *name),
 convert_type_s_i(const char *type_name),
 PA_cpp_name_itype(const char *name);

extern char
 *PA_cpp_value_to_name(const char *group, ...),
 *convert_type_i_s(int type);

extern void
 PA_cpp_init(void),
 PA_cpp_add_group(const char *name, int itype, const char *type),
 PA_cpp_add_name(const char *name, const char *group, ...),
 PA_cpp_default(void);

extern void
 *PA_cpp_name_to_value(const char *name);

#ifdef __cplusplus
}
#endif

#endif

