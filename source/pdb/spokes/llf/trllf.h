/*
 * TRLLF.H - header for Lanl Link Format spoke
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_LLF

#include "cpyright.h"
 
#include "../../pdb_int.h"

#define PCK_LLF

#include "../../pdb_int.h"

/* For 64-bit integer type support */

#ifdef OSF
#include "sys/bitypes.h"
#else
#include "sys/types.h"
#endif

/* File Layout:
 *    File Directory Header  
 *    [File Directory Entry]+
 *    [Record]+
 *
 * Record Layout:
 *    MCD Header
 *    VLD Header
 *    [problem VLB]
 *    variable VLB
 *    [generic VLB]+
 *
 * NOTE: the logical structure of the record has
 *         MCD -> VLD -> [VLB]+
 *       because the addresses are kept the actual order
 *       in which they are written to the disk and their
 *       actual location on the disk should not matter
 */

/*--------------------------------------------------------------------------*/

/*                              DEFINED CONSTANTS                           */

/*--------------------------------------------------------------------------*/

#define NWDIR   512	   /* size of file directory */
#define NWMCDH  100	   /* size of mesh cell data header */


/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/


/* === File Directory === */

typedef struct s_fdir_header fdir_header;
typedef struct s_fdir_entry fdir_entry;

/* LL File Header (effectively) */ 
struct s_fdir_header
   {char name[8];
    int64_t n_word;
    int64_t n_record;
    int64_t address;};

#define PD_DEFINE_FDIR_HEADER \
    PD_defstr(file, "fdir_header", \
              "char name[8]", \
              "int64_t n_word", \
              "int64_t n_record", \
              "int64_t address", \
              LAST)

/* Appears immediately after fdir_header (byte 32 onward)
 * There is one entry per record dumped out
 */
struct s_fdir_entry
   {double time;
    int64_t address;};

#define PD_DEFINE_FDIR_ENTRY \
    PD_defstr(file, "fdir_entry", \
              "double time", \
              "int64_t address", \
              LAST)


/* === MCD - Mesh Cell Data === */

typedef struct s_mcd_header mcd_header;

/* MCD header, one per MCD at fdir_entry.address */
struct s_mcd_header
   {double time;           /* word 1 - dump problem time */
    int64_t words_block;       /* # of words in this block (100) */
    int64_t words_point;       /* # of packed words per mesh cell data point (NWPMC) */
    int64_t vars_cell;         /* # of variables per mesh cell (NVARPMC) */
    char consistency[8];   /* consistency flagword ("no" or "yes"); this is used
                            * to indicate whether or not the data in the      
                            * current dump is consistent with the previous    
                            * dump; for the first dump, flag = "no"; if for   
                            * the current dump, the "vl dir" blocks, the      
                            * dimension maxima, and the mesh variable packing 
                            * (and perhaps other info) are the same as the    
                            * previous dump, then the flag = "yes" */
    int64_t pad1[4];           /* unused (was 5 bytes, corrected to 4 bytes) */
    int64_t n_dimensions;      /* word 10 - # of dimensions */
    int64_t n_des_words;       /* # of descriptive words for each dimension (=4) */

    char dim1[8];          /* name of first (outermost loop) dimension
                            * note that this is required to be "kmax    " */
    int64_t dim1_max;          /* maximum value of first dimension (kmax) */
    int64_t dim1_first;        /* first value of first dimension (1) */
    int64_t dim1_last;         /* last value of first dimension (kmax) */

    char dim2[8];          /* name of second dimension; this must be "lmax    " */
    int64_t dim2_max;          /* maximum value of second dimension (lmax) */
    int64_t dim2_first;        /* first value of second dimension (1) */
    int64_t dim2_last;         /* last value of second dimension (lmax) */

    int64_t pad2[20];          /* unused */

    char code_name[8];     /* word 40 - code name */
    char code_version[8];  /* code version */
    char code_date[8];     /* code date "mm/dd/yy" */
    char code_time[8];     /* code time "hh.mm.ss" */
    char dump_date[8];     /* dump date "mm/dd/yy" */
    char dump_time[8];     /* dump time "hh.mm.ss" */

    int64_t pad3[4];           /* unused */

    char problem_name[32]; /* word 50 - problem name */

    int64_t pad4;              /* unused */
    int64_t cycle;             /* cycle number */
    int64_t pad5[44];          /* unused */
    int64_t vld_addr;};        /* word 100 - address of VLD */

#define PD_DEFINE_MCD_HEADER \
    PD_defstr(file, "mcd_header", \
              "double time", \
              "int64_t words_block", \
              "int64_t words_point", \
              "int64_t vars_cell", \
              "char consistency[8]", \
              "int64_t pad1[4]", \
              "int64_t n_dimensions", \
              "int64_t n_des_words", \
              "char dim1[8]", \
              "int64_t dim1_max", \
              "int64_t dim1_first", \
              "int64_t dim1_last", \
              "char dim2[8]", \
              "int64_t dim2_max", \
              "int64_t dim2_first", \
              "int64_t dim2_last", \
              "int64_t pad2[20]", \
              "char code_name[8]", \
              "char code_version[8]", \
              "char code_date[8]", \
              "char code_time[8]", \
              "char dump_date[8]", \
              "char dump_time[8]", \
              "int64_t pad3[4]", \
              "char problem_name[32]", \
              "int64_t pad4", \
              "int64_t cycle", \
              "int64_t pad5[44]", \
              "int64_t vld_addr", \
              LAST)


/* === VLD - variable length directory === */

typedef struct s_vld_header vld_header;
typedef struct s_vld_entry vld_entry;

/* Located in file at mcd_header.vld_addr */

struct s_vld_header
   {char name[8];	     /* directory name - "vl dir  " */
    int64_t header_size;     /* dir header size in words */
    int64_t dir_size;        /* dir size in words */
    int64_t n_vlb;           /* number of VLBs */
    int64_t n_words;};       /* total number of words in all VLBs including VLDIR */

#define PD_DEFINE_VLD_HEADER \
    PD_defstr(file, "vld_header", \
              "char name[8]", \
              "int64_t header_size", \
              "int64_t dir_size", \
              "int64_t n_vlb", \
              "int64_t n_words", \
              LAST)

/* Follows immediately after vld_header
 * One entry per VLB
 */

struct s_vld_entry
   {char name[8];	  /* VLB name */
    int64_t address;  /* word data address of VLB (relative to vld_addr?) */
    int64_t n_words;  /* VLB number of words */
    char type[8];};   /* VLB type: and the choices are what ??? */

#define PD_DEFINE_VLD_ENTRY \
    PD_defstr(file, "vld_entry", \
              "char name[8]", \
              "int64_t address", \
              "int64_t n_words", \
              "char type[8]", \
              LAST)

/* === VLB - Variable Length Block === */

/* VLB - the basic data unit
 *     - closest analog is syment + data
 *     - LAYOUT:
 *          header        (vlb_header)
 *          dims+         (vlb_dim - one or more)
 *          descriptor    (vlb_desc)
 *          labels*       (char[8] - zero or more)
 *          data          (data)
 */

typedef struct s_vlb_header vlb_header;
typedef struct s_vlb_dim vlb_dim;
typedef struct s_vlb_desc vlb_desc;
typedef struct s_vlb_prb_desc vlb_prb_desc;
typedef struct s_vlb_var_desc vlb_var_desc;


/* VLB Header */
struct s_vlb_header
   {char name[8];         /* array name */
    int64_t header_size;  /* VLB header size */
    int64_t n_words;      /* number of words in this VLB
                            = NWDVLBH+NWDPDIM*NDIMVLB+NWLABLS+NARRAYS*(2+NDWDPA+arraysize)
                            =   10   +   6   *NDIMVLB+   0   +   1   *(2+   2  +arraysize)
                           */
    char type[8];         /* datatype = 'flt pt', 'integer', 'ascii', 'mixed' */
    int64_t n_arrays;     /* number of arrays */
    int64_t n_dims;       /* number of dimensions in array */
    int64_t n_wpd_dims;   /* number of description words per dimension (always 6?) */
    int64_t n_wpd_array;  /* number of description words per array */
    char units[8];        /* units */
    char scale[8];};      /* scale factor */

#define PD_DEFINE_VLB_HEADER \
    PD_defstr(file, "vlb_header", \
              "char name[8]", \
              "int64_t header_size", \
              "int64_t n_words", \
              "char type[8]", \
              "int64_t n_arrays", \
              "int64_t n_dims", \
              "int64_t n_wpd_dims", \
              "int64_t n_wpd_array", \
              "char units[8]", \
              "char scale[8]", \
              LAST)

/* VLB dimensions */

struct s_vlb_dim
   {char codename[8]; /* originating code name */
    char stdname[8];  /* standard name (same as code name) */
    int64_t max;      /* max value */
    int64_t start;    /* first value */
    int64_t stop;     /* last value */
    char desc[8];};   /* unknown */

#define PD_DEFINE_VLB_DIM \
    PD_defstr(file, "vlb_dim", \
              "char codename[8]", \
              "char stdname[8]", \
              "int64_t max", \
              "int64_t start", \
              "int64_t stop", \
              "char desc[8]", \
              LAST)

/* VLB descriptor(s) */

struct s_vlb_desc
   {char codename[8]; /* originating code name */
    char stdname[8];  /* standard name */
    char units[8];    /* units */
    double scale;};   /* scale factor */

#define PD_DEFINE_VLB_DESC \
    PD_defstr(file, "vlb_desc", \
              "char codename[8]", \
              "char stdname[8]", \
              "char units[8]", \
              "double scale", \
              LAST)

struct s_vlb_prb_desc
   {char codename[8]; /* originating code name */
    char stdname[8];  /* standard name */
    char units[8];    /* units */
    double scale;     /* scale factor */
    double value;};   /* scalar value */

#define PD_DEFINE_VLB_PRB_DESC \
    PD_defstr(file, "vlb_prb_desc", \
              "char codename[8]", \
              "char stdname[8]", \
              "char units[8]", \
              "double scale", \
              "double value", \
              LAST)

struct s_vlb_var_desc
   {char codename[8]; /* originating code name */
    char stdname[8];  /* standard name */
    double value;     /* value */
    char units[8];    /* units */
    double scale;};   /* scale factor */

#define PD_DEFINE_VLB_VAR_DESC \
    PD_defstr(file, "vlb_var_desc", \
              "char codename[8]", \
              "char stdname[8]", \
              "double value", \
              "char units[8]", \
              "double scale", \
              LAST)

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

#endif

