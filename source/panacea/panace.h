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

#include "ppc.h"
#include "pgs.h"
    
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

#define RAD       PA_radian
#define STER      PA_steradian
#define MOLE      PA_mole
#define Q         PA_electric_charge
#define CM        PA_cm
#define SEC       PA_sec
#define G         PA_gram
#define EV        PA_eV
#define K         PA_kelvin
#define ERG       PA_erg
#define CC        PA_cc

/*--------------------------------------------------------------------------*/

/*
 * #bind derived PA_scope_kind integer SC_ENUM_I SC_ENUM_I RUNTIME
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
 * #bind derived PA_class_kind integer SC_ENUM_I SC_ENUM_I REQU
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
 * #bind derived PA_persist_kind integer SC_ENUM_I SC_ENUM_I REL
 */

enum e_PA_persist_kind
   {REL     = -10,
    KEEP    = -11,
    CACHE_F = -12,
    CACHE_R = -13};

typedef enum e_PA_persist_kind PA_persist_kind;

#define PERSIST   'c'


/* CENTER and centerings are alread defined */

/*
 * #bind derived PA_allocation_kind integer SC_ENUM_I SC_ENUM_I REL
 */

enum e_PA_allocation_kind
   {STATIC  = -100,
    DYNAMIC = -101};

typedef enum e_PA_allocation_kind PA_allocation_kind;

#define ALLOCATION 'e'

/*
 * #bind derived PA_unit_conversion integer SC_ENUM_I SC_ENUM_I INT_CGS
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

#define PA_ERR    (*PA_error_hook)
#define PA_WARN   (*PA_warn_hook)

enum e_PA_medium
   {PA_FILE     = -1,
    PA_DATABASE = -2};

typedef enum e_PA_medium PA_medium;

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* PA_GET_FUNCTION - return a pointer to the named function */

#define PA_GET_FUNCTION(type, name)                                          \
    SC_HASHARR_LOOKUP_FUNCTION(PA_symbol_tab, type, name)
    
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
       {t_data[pr->str_index] += (val)*pr->conv;                             \
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
   t_data[pr->str_index] = (val)*pr->conv

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
 *                     - the PA_variable_tab
 */

#define PA_INQUIRE_VARIABLE(x)                                               \
    ((PA_variable *) SC_hasharr_def_lookup(PA_variable_tab, (x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INQUIRE_PACKAGE - return a PA_package pointer from
 *                    - the PA_variable_tab
 */

#define PA_INQUIRE_PACKAGE(x)                                                \
    ((PA_package *) SC_hasharr_def_lookup(PA_variable_tab, (x)))

/*--------------------------------------------------------------------------*/

#define PA_SET_MAX_NAME_SPACE(f)   PA_name_spaces = f
#define PA_GET_MAX_NAME_SPACE(f)   f = PA_name_spaces

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
typedef PM_set *(*PFBuildDom)(char *base_name, C_array *arr, double t);
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
typedef void (*PFErrHand)(int, char *, ...);
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


/* PA_SET_SPEC is PD_DEFSTR'd in PA_DEF_STR in PANACEA.C
 * so any changes here must be reflected there
 */

struct s_PA_set_spec
   {char *var_name;
    char *function;
    char *text;
    int n_values;
    double *values;
    struct s_PA_set_spec *next;};

/* PA_PLOT_REQUEST is PD_DEFSTR'd in PA_DEF_STR in PANACEA.C
 * so any changes here must be reflected there
 */

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
    struct s_PA_plot_request *next;};


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
    char *db_file;               /* name of text file defining the database */
    pcons *db_list;               /* list of the database names from a file */
    pcons *alist;                        /* list of package level functions */
    struct s_PA_package *next;};               /* point to the next package */


/* added a pointer to the number of elements so that variables could be
 * installed with zero length and when read from restart dumps, source
 * files, or the generator deck the size can be dynamically set
 * this also allows for error checking by comparing desc->number with
 * *size when both are non-zero
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


struct s_PA_src_variable
   {char *name;                             /* variable name (with indexes) */
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

/* IV_SPECIFICATION is PD_DEFSTR'd in PA_DEF_STR in PANACEA.C
 * so any changes here must be reflected there
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
    struct s_PA_iv_specification *next;};

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
    struct s_PA_unit_spec *next;};

/* SET_INDEX - aid to selecting elements out of PM_sets
 *           - this is PD_defstr'd in PANACEA.C changes must be reflected there!!!
 */

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
   {LAST_TAG             = 0,
    PA_INFO_TYPE         = -1,
    PA_INFO_N_DIMS       = -2,
    PA_INFO_DIMS         = -3,
    PA_INFO_SCOPE        = -4,
    PA_INFO_CLASS        = -5,
    PA_INFO_CENTER       = -6,
    PA_INFO_PERSISTENCE  = -7,
    PA_INFO_ALLOCATION   = -8,
    PA_INFO_FILE_NAME    = -9,
    PA_INFO_INIT_VAL     = -10,
    PA_INFO_INIT_FNC     = -11,
    PA_INFO_CONV         = -12,
    PA_INFO_UNIT         = -13,
    PA_INFO_KEY          = -14,
    PA_INFO_ATTRIBUTE    = -15,
    PA_INFO_UNITS        = -16,
    PA_INFO_DATA_PTR     = -17,
    PA_INFO_UNIT_NUMER   = -18,
    PA_INFO_UNIT_DENOM   = -19,
    PA_INFO_APPL_ATTR    = -20,
    PA_INFO_DEFAULT      = -21,
    PA_INFO_SHARE        = -22,
    PA_INFO_ATT_NAME     = -23,
    PA_INFO_DIM_NAME     = -24,
    PA_INFO_UNITS_NAME   = -25,
    PA_INFO_DOMAIN_NAME  = -26,
    PA_INFO_MAP_DOMAIN   = -27,
    PA_INFO_BUILD_DOMAIN = -28};

typedef enum e_PA_info_tag PA_info_tag;

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern PFErrHand
 PA_error_hook,
 PA_warn_hook;

extern PA_iv_specification
 *iv_spec_lst;

extern PA_package
 *Packages;

extern PA_variable
 *_PA_default_variable;

extern PA_plot_request
 *plot_reqs;

extern PROCESS
 *PA_pp;

extern hasharr
 *PA_alias_tab,
 *PA_commands,
 *PA_symbol_tab,
 *PA_variable_tab,
 *PA_var_att_tab,
 *PA_var_def_tab,
 *PA_var_dim_tab,
 *PA_var_domain_tab,
 *PA_var_unit_tab,
 *PA_cpp_name_tab,
 *PA_cpp_value_tab;

extern PDBfile
 *PA_cache_file,
 **_PA_state_files,
 *PA_vif,
 *PA_pva_file,
 *PA_pp_file;

extern FILE
 *PA_edit_file;

extern PA_src_variable
 **SV_List;

extern char
 errbuf[],                      /* global buffer for certain error messages */
 PA_err[],
 *PA_input_prompt,
 **global_name,
 _PA_base_name[],
 *_PA_rsname,
 *PA_strtok_p,
 *PA_token_delimiters,
 *PAN_COMMAND,
 *PAN_CPP_NODE,
 *PAN_EDIT,
 *PAN_PLOT_REQ,
 *PAN_PACKAGE,
 *PAN_SOURCE,
 *PAN_VARIABLE,
 *PAN_ATTRIBUTE,
 *PAN_DIMENSION,
 *PAN_DOMAIN,
 *PAN_EDIT_REQUEST,
 *PAN_EDIT_OUT,
 *PAN_UNIT,
 *PA_SET_INDEX_S,
 *PA_SET_INDEX_P_S;

extern char
 *PA_INFO_TYPE_S,
 *PA_INFO_N_DIMS_S,
 *PA_INFO_DIMS_S,
 *PA_INFO_SCOPE_S,
 *PA_INFO_CLASS_S,
 *PA_INFO_CENTER_S,
 *PA_INFO_PERSISTENCE_S,
 *PA_INFO_ALLOCATION_S,
 *PA_INFO_FILE_NAME_S,
 *PA_INFO_INIT_VAL_S,
 *PA_INFO_INIT_FNC_S,
 *PA_INFO_CONV_S,
 *PA_INFO_UNIT_S,
 *PA_INFO_KEY_S,
 *PA_INFO_ATTRIBUTE_S,
 *PA_INFO_UNITS_S,
 *PA_INFO_DATA_PTR_S,
 *PA_INFO_UNIT_NUMER_S,
 *PA_INFO_UNIT_DENOM_S,
 *PA_INFO_APPL_ATTR_S,
 *PA_INFO_DEFAULT_S,
 *PA_INFO_SHARE_S,
 *PA_INFO_ATT_NAME_S,
 *PA_INFO_DIM_NAME_S,
 *PA_INFO_UNITS_NAME_S,
 *PA_INFO_DOMAIN_NAME_S,
 *PA_INFO_MAP_DOMAIN_S,
 *PA_INFO_BUILD_DOMAIN_S,
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

extern int
 *global_swtch,
 N_graphs,
 N_plots,
 N_Packages,
 N_Switches,
 N_Parameters,
 N_Names,
 N_Units,
 N_Variables,
 N_time_plots,
 N_unique_variables,
 PA_name_spaces,
 PA_current_pp_cycle,
 _PA_ul_print_flag,
 _PA_n_state_files,
 _PA_halt_fl,
 *PA_DUL,
 *PA_DON,
 PA_radian,
 PA_steradian,
 PA_mole,
 PA_electric_charge,
 PA_cm,
 PA_sec,
 PA_gram,
 PA_eV,
 PA_kelvin,
 PA_erg,
 PA_cc,
 STAND_ALONE;                                           /* stand alone flag */

extern double
 *convrsn,
 *global_param,
 *t_data,
 *unit;

/* CODE UNITS VARIABLES AND CONSTANTS */

extern double
 alpha,              /* fine structure constant e^2/HbarC  - 7.297353080e-3 */
 c,                              /* speed of light (cm/sec) - 2.99792458e10 */
 Coulomb,                /* Coulomb in fundamental charges - 6.241506363e18 */
 e,                              /* electron charge in esu - 4.80320680e-10 */
 eV_erg,                                      /* eV to erg - 1.60217733e-12 */
 Gn,       /* Newtonian gravitational constant (cm^3/g-sec^2) - 6.673231e-8 */
 Hbar,                                  /* Hbar in erg-sec - 1.05457267e-27 */
 HbarC,                                  /* Hbar*C in eV-cm - 1.97327054e-5 */
 kBoltz,                     /* Boltzman constant in (erg/K) - 1.380658e-16 */
 K_eV,                                        /* Kelvin to eV 8.6173856e-05 */
 M_a,                            /* atomic mass unit in g - 1.660540210e-24 */
 M_e,                               /* electron mass in g - 9.109389754e-28 */
 M_e_eV,                              /* electron mass in eV - 5.10999065e5 */
 N0,                                   /* Avagadro's number - 6.02213665e23 */
 Ryd,                           /* (M_e*c^2*alpha^2)/2 in eV - 13.605698140 */

 icm_g,                                /* inverse cm to g  - 3.51767578e-38 */
 g_icm,                                 /* g to inverse cm  - 2.84278615e37 */

 icm_eV,                                       /* inverse cm to eV  - HbarC */
 eV_icm,                                 /* eV toinverse cm  - 5.06772882e4 */

 icm_erg,                            /* inverse cm to erg  - 3.16152932e-17 */
 erg_icm;                             /* erg to inverse cm  - 3.16302617e16 */

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
 *PA_load_data(char *s),
 *PA_get_access(void **vr, char *sn, long offs, long ne, int track),
 PA_rel_access(void **vp, char *s, long offs, long ne),
 PA_equivalence(void *vr, char *s),
 PA_set_equiv(char *s, void *vl);

extern PA_variable
 *PA_inquire_variable(char *x);


/* PADSYS.C declarations */

extern PA_package
 *PA_current_package(void),
 *PA_gen_package(char *name, PFPkgGencmd cmd, PFPkgDfstrc dfs,
                 PFPkgDefun dfu, PFPkgDefvar dfr, PFPkgDefcnt cnt,
                 PFPkgIntrn inr, char *fname);

extern void
 PA_run_time_package(char *name, PFPkgDfstrc dfs, PFPkgDefun dfu,
                     PFPkgDefvar dfr, PFPkgDefcnt cnt, PFPkgInizer izr,
                     PFPkgMain mn, PFPkgPpsor psr, PFPkgFinzer fzr,
                     char *fname),
 PA_def_package(char *name, PFPkgGencmd gcmd, PFPkgDfstrc dfs,
                PFPkgDefun dfu, PFPkgDefvar dfr, PFPkgDefcnt cnt,
                PFPkgIntrn inr, PFPkgInizer izr, PFPkgMain mn,
                PFPkgPpsor psr, PFPkgFinzer fzr, PFPkgPpcmd pcmd,
                char *fname),
 PA_control_set(char *s);


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
 *PA_set_data(char *name, C_array *arr, PM_centering *pcent),
 *PA_fill_component(double *data, int len, int *pist, int ne);

extern PM_mapping
 *PA_build_mapping(PA_plot_request *pr, PM_set *(*build_ran)(PA_plot_request *pr, char *name), double t);

extern PA_set_spec
 *PA_non_time_domain(PA_plot_request *pr);

extern C_array
 *PA_get_domain_info(PA_plot_request *pr, char *dname, int nc);


/* PAFI.C declarations */

extern int
 PA_connect_ptrs_p(char *name, char **pvn, void **pp, int *pn);

extern void
 PA_read_def(PA_package *pck, char *name),
 PA_install_function(char *s, PFVoid fnc),
 PA_install_identifier(char *s, void *vr),
 PA_add_hook(char *name, void *fnc);


/* PAGNRD.C declarations */

extern hasharr
 *PA_inst_com(void);

extern int
 PA_function_form(char *t, PA_set_spec *spec);

extern void
 PA_inst_pck_gen_cmmnds(void),
 PA_intern_pck_db(void),
 PA_def_alias(char *name, char *type, void *pv),
 PA_inst_c(char *cname, void *cvar, int ctype, int cnum,
           PFVoid cproc, PFPanHand chand),
 PA_defh(void),
 PA_specifyh(void),
 PA_sh(void),
 PA_proc_iv_spec(PA_iv_specification *lst),
 PA_clear(void),
 PA_packh(void),
 PA_pshand(PA_command *cp),
 PA_nploth(void),
 PA_time_plot(char *rname, void *vr),
 PA_wrrstrth(void),
 PA_name_files(char *base_name, char **ped, char **prs,
	       char **ppp, char **pgf),
 PA_read_file(char *str, int sfl),
 PA_readh(char *str),
 PA_get_commands(FILE *fp, PFVString errfnc),
 PA_done(void);

extern double
 PA_get_num_field(char *s, char *t, int optp),
 PA_alias_value(char *s);

extern char
 *PA_get_next_line(void),
 *PA_get_field(char *s, char *t, int optp);


/* PAHAND.C declarations */

extern void
 PA_zargs(PA_command *cp),
 PA_sargs(PA_command *cp),
 PA_strarg(PA_command *cp);


/* PAMM.C declarations */

extern void
 PA_mk_control(PA_package *pck, char *s, int na, int np, int ns),
 PA_mark_space(PA_package *pck),
 PA_accm_space(PA_package *pck),
 PA_mark_time(PA_package *pck),
 PA_accm_time(PA_package *pck);

extern PA_iv_specification
 *PA_mk_spec(char *id, int type, char *fn, int n, int interp,
	     pcons *lst, PA_iv_specification *nxt);


/* PANACEA.C declarations */

extern void
 PA_simulate(double tc, int nc, int nz, double ti, double tf,
	     double dtf_init, double dtf_min, double dtf_max, double dtf_inc,
	     char *rsname, char *edname, char *ppname, char *gfname),
 PA_init_system(double t, double dt, int nc,
		char *edname, char *ppname, char *gfname),
 PA_run_packages(double t, double dt, int cycle),
 PA_fin_system(int nz, int nc, int silent),
 PA_terminate(char *edname, char *ppname, char *gfname, int cycle);

extern double
 PA_advance_t(double dtmn, double dtn, double dtmx);


/* PANTH.C declarations */

extern int
 PA_th_write(PDBfile *strm, char *name, char *type, int inst, int nr, void *vr),
 PA_th_wr_iattr(PDBfile *strm, char *vr, int inst, char *attr, void *avl),
 PA_th_transpose(char *name, int ncpf),
 PA_th_trans_family(char *name, int ord, int ncpf),
 PA_th_family_list(char *name, int c, char ***pfiles),
 PA_th_trans_name(int n, char **names, int ord, int ncpf),
 PA_th_name_list(int n, char **names, char ***pthfiles),
 PA_th_trans_link(int n, char **names, int ord, int ncpf),
 PA_th_link_list(int n, char **names, char ***pthfiles),
 PA_th_trans_files(char *name, int ncpf, int nthf, char **thfiles, int ord, int flag),
 PA_merge_family(char *base, char *family, int ncpf),
 PA_merge_files(char *base, int n, char **files, int ncpf),
 PA_th_wr_member(PDBfile *strm, char *name, char *member, char *type,
		 int inst, void *vr);

extern defstr
 *PA_th_def_rec(PDBfile *file, char *name, char *type, int nmemb,
	      char **members, char **labels);

extern PDBfile
 *PA_th_open(char *name, char *mode, long size, char *prev),
 *PA_th_family(PDBfile *file);


/* PAPP.C declarations */

extern void
 PA_transpose_pp(char *ppname, int ntp, int nuv);


/* PASHAR.C declarations */

extern int
 PA_set_default_offset(int d),
 PA_get_default_offset(void);

extern void
 PA_def_var(char *vname, char *vtype, void *viv,
	    PFVarInit vif, ...),
 PA_inst_var(char *vname, char *vtype, void *viv,
	     PFVarInit vif, ...),
 PA_inst_scalar(char *vname, char *vtype, void *vaddr, void *viv,
		PFVarInit vif, ...),
 PA_init_scalar(char *s),
 PA_error_handler(int test, char *fmt, ...),
 PA_warning_handler(int test, char *fmt, ...),
 PA_error(char *msg);

extern PDBfile
 *PA_open(char *name, char *mode, int flag);


/* PASHAS.C declarations */

extern void
 PA_trap_signals(int nsig, ...),
 PA_signal_handler(int sig),
 PA_interrupt_handler(int sig),
 PA_file_mon(char *edname, char *ppname, char *gfname),
 PA_rd_restart(char *rsname, int convs),
 PA_wr_restart(char *rsname),
 PA_close_pp(void),
 PA_init_strings(void),
 PA_def_str(PDBfile *pdrs),
 PA_change_dim(int *pdm, int val),
 PA_change_size(char *name, int flag),
 *PA_intern(void *vr, char *name);

extern int
 PA_sizeof(char *s);


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
 *PA_get_source(char *s, int start_flag);

extern PA_iv_specification 
 *PA_get_iv_source(char *name),
 *PA_find_iv_source(char *name, int off);

extern double
 PA_intr_spec(PA_iv_specification *sp, double t, double val, long off);

extern void
 PA_interp_src(void *v, PA_src_variable *svp, int ni, int nf,
	       double t, double dt),
 PA_source_variables(double t, double dt);


/* PASTR.C declarations */

extern void
 *PA_mk_instance(char *name, char *type, long n),
 *PA_get_member(char *name, char *member);

extern void
 PA_rl_instance(char *name),
 PA_set_member(char *name, void *data, char *member);

extern syment
 *PA_int_instance(char *name, char *type, long n, void *data);


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
 PA_def_var_domain(char *name, ...);

extern PA_variable
 *PA_def_variable(char *name, ...),
 *PA_def_var_default(char *name, ...);

extern PA_dimens
 *PA_def_var_dimension(char *name, ...);

extern pcons
 *PA_var_to_alist(PA_variable *pp),
 *PA_def_var_attribute(char *name, ...),
 *PA_def_var_units(char *name, ...);

extern hasharr
 *PA_install_table(char *s, void *vr, char *type, hasharr *tab);


/* PAINFO.C declarations */

extern int 
 PA_info_check(int info, int value);

extern char
 *PA_info_name(int info),
 *PA_info_type(int info),
 *PA_info_value(int info, int value);

extern void
 PA_info_init(void),
 PA_info_print(void);


/* PACPP.C */

extern int
 PA_cpp_symbol_itype(char *name),
 convert_type_s_i(char *type_name),
 PA_cpp_name_itype(char *name);

extern char
 *PA_cpp_value_to_name(char *group, ...),
 *convert_type_i_s(int type);

extern void
 PA_cpp_init(void),
 PA_cpp_add_group(char *name, int itype, char *type),
 PA_cpp_add_name(char *name, char *group, ...),
 PA_cpp_default(void);

extern void
 *PA_cpp_name_to_value(char *name);

#ifdef __cplusplus
}
#endif

#endif

