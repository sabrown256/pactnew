/*
 * PDB.H - header file supporting PDBLIB routines
 *       -
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PDB

#include "cpyright.h"

#define PCK_PDB                       /* announce that this package is here */

#include "pml.h"

/* PDBLib History:
 * Version 4 contains bug fixes pertaining to cast information in PDB files
 * and how it is read in (the bug was that the DATE returned by SC_date
 * had a newline at the end which buggered _PD_rd_extras) - 3/19/91
 *
 * Version 5 contains the EXTRA defining row major or column major
 * ordering for multidimensional arrays - 3/22/91
 *
 * Version 6 contains the primitive defintions in the EXTRA table
 * done in a way which is compatible with old files (pre Version 6)
 * this allows applications to define new primitive data types - 2/25/92
 *
 * Version 7 has the new interface with the functions from the old
 * interface deprecated.  The new functions are: PD_write_alt,
 * PD_write_as_alt, PD_defent, PD_defent_alt, and PD_part_read.
 * The old functions are: PD_r_write, PD_v_write, PD_l_write, PD_r_part_read,
 * and PD_v_part_read. This new interface is aimed at general partial
 * reads and writes via a hyper slab approach. 5/29/92
 *
 * Version 8 has the deprecated functions removed and has the discontiguous
 * data block stuff added.  ITAG flags have a third possible value of 2
 * This signals a discontiguous data block.  That is a pointered variable
 * may be written in discontiguous blocks of data with each succeeding
 * block pointing to the previous one.  Syment was also modified to use
 * an array of address, number structs describing the discontiguous blocks.
 * Using PD_entry_xxx macros helps with backward compatibility. 5/17/93
 *
 * Version 9 has file directories. The new functions are: PD_ls, PD_mkdir,
 * PD_ln, PD_cd, PD_pwd. 6/4/93
 *
 * Version 10 has the uncomputable additional struct alignment handled
 * properly. 7/23/93
 *
 * Version 11 has support for families of files with links to previous files
 * and maximum size per file in the family.  PD_family will spawn the next
 * file in the family. 7/27/93
 *
 * Version 12 has conversions done on a per type rather than a per file basis
 * and it read/writes the structure chart, symbol table, and extras table
 * as single blocks of data for improved performance (especially in the
 * remote file access case). 1/24/94
 *
 * Version 13 uses a LALR(1) grammar and parser for expressions which access
 * parts of entries. This is both more robust and complete than previous
 * versions. 1/6/94
 *
 * Version 14 recognizes and converts bitstreams.  This allows PDB to open
 * and read data in files written on old machines with byte sizes other
 * than 8 bits and to support the use of bit fields in data files which
 * PDB may also have to access. 1/8/94
 *
 * Version 15 adds symbol table entry for dynamically allocated spaces
 * (pointees).  This is a step toward the next generation pointer
 * handling. 10/15/97
 *
 * Version 16 supports the new hash table structure 01/15/02
 *
 * Version 17 made changes to the data structures to increase performance
 *
 * Version 18 abstract out methods to support different format versions
 * and introduce format version III
 *
 * Version 19 write attribute table in its own directory and with its
 * own set of pointers in order to function properly with format version III
 *
 * Version 20 write checksums in hex form not raw character form
 *
 * Version 21 factor out db_layer, sys_layer, and fs_layer in order to
 * access other databases such as SQL, XML/HTML where it isn't feasible
 * to have the PDB engine read and write the actual data but PDB can
 * run the API's to do so
 *  db_layer  - manages the symbol table, structure charts and the
 *              PDB schema, abstractions, and metadata
 *  sys_layer - accesses databases via native API's
 *  fs_layer  - file system level I/O
 *            - this is the io_hooks abstraction in SCORE 
 *
 * Version 22 introduce the UDL concept to access more kinds of data
 * from servers using protocols such as http, ftp, and mysql
 * from containers such as TAR and AR
 * to be able to add various attributes to control I/O such as
 * writing files to the TEXT_STD. 11/09/2009  

 * Version 23 introduce type kinds to permit definition and
 * proper conversion of multi-byte character types
 * This is important for SQL and TEXT_STD applications. 12/06/2009
 *
 * Version 24 convert to use of SC_array and hasharr instead of
 * SC_dynamic_array and HASHTAB. 02/03/2010
 *
 * Version 25 add support for C99 types: long double, boolean,
 * float complex, double complex, long double complex. 10/25/2010
 *
 */

#define PDB_SYSTEM_VERSION  25

#define BITS_DEFAULT 8     /* default bits per byte */
#define NSTD         6     /* number of standards currently in the system 
                            * should be same as last standard */

#define PD_N_PRIMITIVE_FIX  4   /* short, int, long, and long long */
#define PD_N_PRIMITIVE_FP   3   /* float, double, and long double */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PN_sizeof(type, tab)  _PD_lookup_size(type, tab)

#define PD_get_file_type(file)                                               \
   (((file)->type == NULL) ? PDBFILE_S : (file)->type)

#define PD_get_mode(file)              ((file)->mode)
#define PD_set_mode(file, v)           (file)->mode = (v)
#define PD_get_offset(file)            ((file)->default_offset)
#define PD_set_offset(file, v)         (file)->default_offset = (v)
#define PD_get_track_pointers(file)    ((file)->track_pointers)
#define PD_set_track_pointers(file, v) (file)->track_pointers = (v)

/* #define PD_file_maximum_size(file) file->maximum_size           */
/* replaced by PD_get_max_file_size and PD_set_max_file_size below */

#define PD_get_max_file_size(file)     ((file)->maximum_size)
#define PD_set_max_file_size(file, v)  (file)->maximum_size = (v)

#define PD_set_format_version(_n)    PD_default_format_version = _n
#define PD_get_format_version(_n)    _n = PD_default_format_version

#define PD_entry_type(ep)            (ep)->type
#define PD_entry_dimensions(ep)      (ep)->dimensions
#define PD_entry_number(ep)          (ep)->number
#define PD_entry_indirects(ep)       (ep)->indirects

#define PD_get_major_order(file)       (((file) != NULL) ? (file)->major_order : NO_MAJOR_ORDER)
#define PD_set_major_order(file, type) if ((file) != NULL) (file)->major_order = (type)
#define PD_get_file_name(file)         (((file) != NULL) ? (file)->name : NULL)
#define PD_has_directories(file)                                             \
    ((file != NULL) && (file->current_prefix != NULL))

#define PD_inquire_table_type(table, name)                                   \
 (defstr *) SC_hasharr_def_lookup(table, name)

#define PD_hyper_refp(name, commnd)                                          \
   {char c, s[MAXLINE];                                                      \
    strcpy(s, name);                                                         \
    c = SC_LAST_CHAR(s);                                                     \
    if ((c == ')') || (c == ']'))                                            \
       (commnd);}

#define PD_hyper_ref(s, name)                                                \
   {char c;                                                                  \
    strcpy(s, name);                                                         \
    c = SC_LAST_CHAR(s);                                                     \
    if ((c == ')') || (c == ']'))                                            \
       {SC_LAST_CHAR(s) = '\0';                                              \
        SC_lasttok(s, "[]()");};}

#define PD_CAST_TYPE(t, d, vg, vl, err_fnc, err_msg, err_arg)                \
    {if (d->cast_offs < 0L)                                                  \
        t = d->type;                                                         \
     else                                                                    \
        {t = DEREF(vl + d->cast_offs);                                       \
         if (t == NULL)                                                      \
            {if (DEREF(vg) != NULL)                                          \
                {err_fnc(err_msg, err_arg);}                                 \
             else                                                            \
                t = d->type;};};}

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

/* temporary until hasharr rollover complete */
typedef int (*PFDefDel)(void *a);

typedef struct BF_FILE_s BF_FILE;
typedef struct s_fixdes fixdes;
typedef struct s_fpdes fpdes;
typedef struct s_data_alignment data_alignment;
typedef struct s_data_standard data_standard;
typedef struct s_PD_smp_state PD_smp_state;
typedef struct s_PDBfile PDBfile;
typedef struct s_multides multides;
typedef struct s_dimdes dimdes;
typedef struct s_memdes memdes;
typedef struct s_symindir symindir;
typedef struct s_syment syment;
typedef struct s_defstr defstr;
typedef struct s_sys_layer sys_layer;
typedef struct s_tr_layer tr_layer;
typedef struct s_PD_itag PD_itag;
typedef struct s_dimind dimind;
typedef struct s_attribute attribute;
typedef struct s_attribute_value attribute_value;
typedef struct s_precisionfp precisionfp;
typedef struct s_PD_image PD_image;
typedef struct s_PD_pfm_fnc PD_pfm_fnc;
typedef memdes *(*PFPDBwrite)(PDBfile *file, char *vr, defstr *defp);
typedef memdes *(*PFPDBread)(memdes *members);

enum e_PD_major_op
   {PD_READ = 0,
    PD_WRITE,
    PD_APPEND,
    PD_OPEN,
    PD_CREATE,
    PD_CLOSE,
    PD_TRACE,
    PD_PRINT,
    PD_GENERIC,
    PD_UNINIT};

typedef enum e_PD_major_op PD_major_op;

enum e_PD_byte_order
   {NO_ORDER = 0,             /* opaque type - no order */
    NORMAL_ORDER,             /* network or big-endian order */
    REVERSE_ORDER,            /* little-endian */
    TEXT_ORDER,               /* not binary but text */
    SPEC_ORDER};              /* explicitly specified byte order */

typedef enum e_PD_byte_order PD_byte_order;

enum e_PD_type_kind
   {CHAR_KIND = 0,
    INT_KIND,
    FLOAT_KIND,
    NON_CONVERT_KIND,
    STRUCT_KIND};

typedef enum e_PD_type_kind PD_type_kind;

enum e_PD_major_order
   {NO_MAJOR_ORDER = 0,
    ROW_MAJOR_ORDER = 101,
    COLUMN_MAJOR_ORDER};

typedef enum e_PD_major_order PD_major_order;

enum e_PD_curve_io
   {NOR_X_Y = 0,
    X_AND_Y,
    X_ONLY,
    Y_ONLY};

typedef enum e_PD_curve_io PD_curve_io;

enum e_PD_symbol_order
   {PF_ALPHABETIC = 1,
    PF_DISK_ORDER};

typedef enum e_PD_symbol_order PD_symbol_order;

enum e_PD_checksum_mode
   {PD_MD5_OFF = 0,
    PD_MD5_FILE,
    PD_MD5_RW,
    PD_MD5_FULL};

typedef enum e_PD_checksum_mode PD_checksum_mode;

/* NOTE: see comment in scope_io.h concerning file_io_desc and the stream member
 *       here the buffer is the analog of the FILE *
 */
struct BF_FILE_s
   {SC_address bf;
    SC_address addr;
    long length;};

struct s_data_alignment
   {int char_alignment;
    int ptr_alignment;
    int bool_alignment;
    int fx[PD_N_PRIMITIVE_FIX];              /* fixed point types */
    int fp[PD_N_PRIMITIVE_FP];               /* floating point types */
    int struct_alignment;};

struct s_precisionfp
   {int digits;
    long double tolerance;};

struct s_fixdes
   {int bpi;
    PD_byte_order order;};

#define PD_DEFINE_FIXDES(_f)               \
    PD_defstr(_f, "fixdes",                \
              "int bpi",                   \
              "int order",                 \
              LAST)
struct s_fpdes
   {int bpi;
    long *format;
    int *order;};

#define PD_DEFINE_FPDES(_f)                \
    PD_defstr(_f, "fpdes",                 \
              "int bpi",                   \
              "long *format",              \
              "int *order",                \
              LAST)

struct s_data_standard
   {int bits_byte;
    int ptr_bytes;
    int bool_bytes;
    fixdes fx[PD_N_PRIMITIVE_FIX];
    fpdes  fp[PD_N_PRIMITIVE_FP];
    PDBfile *file;};

#if 0
typedef struct s_PD_disk_block PD_disk_block;  /* move up with other typedefs */

struct s_PD_disk_block
   {BIGINT addr;                       /* address of the disk block */
    BIGINT size;                       /* size in bytes */
    struct s_PD_disk_block *prev;
    struct s_PD_disk_block *next;};
#endif

struct s_tr_layer
   {char *type;                       /* file type - e.g. PDBfile*/
    char *fmt;                        /* format name - e.g. pdb */
    int (*gettype)(char *type);
    PDBfile *(*create)(tr_layer *tr, SC_udl *pu, char *name, void *a);
    PDBfile *(*open)(tr_layer *tr, SC_udl *pu, char *name, char *mode, void *a);
    int (*close)(PDBfile *file);
    int (*read)(PDBfile *file, char *path, char *ty,
		syment *ep, void *vr, int nd, long *ind);
    syment *(*write)(PDBfile *file, char *path,
		     char *inty, char *outty,
		     void *vr, dimdes *dims,
		     int appnd, int *pnew);};

struct s_sys_layer
   {long (*read)(PDBfile *file, syment *ep,
		 char *outtype, void *vr);
    long (*write)(PDBfile *file, char *vr, long nitems,
		  char *intype, char *outtype);};

struct s_PDBfile
   {FILE *stream;                     /* actual stream to the file */
    char *name;                       /* filename such as: "temp.pdb" */
    char *type;                       /* used by files masquerading as PDB */
    SC_udl *udl;
    hasharr *symtab;                  /* info about variables in a file */
    hasharr *chart;                   /* info about datatypes in a file */
    hasharr *host_chart;              /* info about datatypes on localhost */
    hasharr *attrtab;
    char *previous_file;              /* for file family bookkeeping */
    char *file_mode;                  /* record the fopen type mode */
    PD_major_op mode;                 /* file mode */
    int default_offset;
    data_standard *std;               /* file data standards */
    data_standard *host_std;          /* host machine data standards */
    data_alignment *align;            /* file alignment info */
    data_alignment *host_align;       /* host machine alignment info */
    int radix;                        /* integer radix in TEXT mode */
    char *delim;                      /* field delimiter in TEXT mode */
    char *current_prefix;             /* dir prefix such as: "/" */
    char *ptr_base;                   /* base name for pointer entries */
    int flushed;                      /* TRUE or FALSE */
    int virtual_internal;
    int format_version;
    int system_version;
    PD_major_order major_order;       /* COLUMN_ or ROW_MAJOR_ORDER */
    int track_pointers;
    int use_itags;
    char *date;
    SC_array *ap;                    /* address/pointer list */
    long n_dyn_spaces;
#if 0
    PD_disk_block *free_list;
#endif
    int mpi_file;
    int mpi_mode;                        /* serial (1) parallel (0) */
    BIGINT maximum_size;                  /* for file family bookkeeping */
    BIGINT headaddr;
    BIGINT symtaddr;
    BIGINT chrtaddr;
    PD_checksum_mode use_cksum;          /* session use of checksums */
    PD_checksum_mode file_cksum;         /* file use of checksums */
    int fix_denorm;

    sys_layer *sys;
    tr_layer *tr;

    void *meta;                          /* container for metadata in the tr layer */
    
/* the db_layer methods */
    int (*create)(PDBfile *file, int mst);
    int (*open)(PDBfile *file);
    int (*flush)(PDBfile *file);

    BIGINT (*wr_symt)(PDBfile *file);
    int (*parse_symt)(PDBfile *file, char *bf, int flag);

    int (*wr_meta)(PDBfile *file, FILE *out, int fh);
    int (*wr_prim_types)(FILE *fp, hasharr *tab);
    int (*rd_prim_types)(PDBfile *file, char *bf);

    int (*wr_itag)(PDBfile *file, long n, long nitems, char *type,
		   BIGINT addr, int flag);
    int (*rd_itag)(PDBfile *file, char *p, PD_itag *pi);

    int (*wr_fmt)(PDBfile *file);};

/* tuple descriptor - describe a simple compounding of primitive types
 * enables C99 complex primitives for example
 * item ordering gives flexibility to possibly support some C++ mendacity
 */
struct s_multides
   {char *type;
    int ni;
    int *order;};

#define PD_DEFINE_MULTIDES(_f)      \
    PD_defstr(_f, "multides",       \
              "char *type",         \
              "int ni",             \
              "int *order",         \
              LAST)

/* dimension descriptor - describe an array dimension range */
struct s_dimdes
   {long index_min;
    long index_max;
    long number;
    struct s_dimdes *next;};

#define PD_DEFINE_DIMDES(_f)        \
    PD_defstr(_f, "dimdes",         \
              "long index_min",     \
              "long index_max",     \
              "long number",        \
              "dimdes *next",       \
              LAST)

/* member descriptor - describe a member efficiently */
struct s_memdes
   {char *member;
    BIGINT member_offs;
    char *cast_memb;
    long cast_offs;
    int is_indirect;
    char *type;
    char *base_type;
    char *name;
    dimdes *dimensions;
    long number;
    struct s_memdes *next;};

FUNCTION_POINTER(memdes, *(*PFPmemdes));

#define PD_DEFINE_MEMDES(_f)              \
    PD_defstr(_f, "memdes",               \
              "char *member",             \
              "long_long member_offs",    \
              "char *cast_memb",          \
              "long cast_offs",           \
              "int is_indirect",          \
              "char *type",               \
              "char *base_type",          \
              "char *name",               \
              "dimdes *dimensions",       \
              "long number",              \
              "memdes *next",             \
              LAST) 

/* symbol table entry indirects */
struct s_symindir
   {BIGINT addr;
    long n_ind_type;
    long arr_offs;};

#define PD_DEFINE_SYMINDIR(_f)            \
  PD_defstr(_f, "symindir",               \
	    "long_long addr",             \
	    "long n_ind_type",            \
	    "long arr_offs",              \
            LAST)

/* symbol table entry */
struct s_syment
   {char *type;
    dimdes *dimensions;
    long number;
    long foreign;             /* means nothing to PDB only spokes if needed */
    symindir indirects;
    SC_array *blocks;};

#define PD_DEFINE_SYMENT(_f)               \
    PD_defstr(_f, "syment",                \
              "char *type",                \
              "dimdes *dimensions",        \
              "long number",               \
              "long foreign",              \
              "symindir indirects",        \
              "SC_array *blocks",          \
              LAST)

/* structure chart entry */
struct s_defstr
   {char *type;
    PD_type_kind kind;
    long size_bits;
    long size;
    int alignment;   /* type begins on byte boundary divisible by alignment */
    int n_indirects;                              /* number of indirections */
    int is_indirect;                     /* TRUE iff type is a pointer type */
    int convert;
    int onescmp;      /* TRUE iff ones complement arithmetic - old machines */
    int unsgned;                  /* TRUE iff the integral type is unsigned */
    fixdes fix;
    fpdes fp;
    memdes *members;
    multides *tuple;};

#define PD_DEFINE_DEFSTR(_f)       \
    PD_defstr(_f, "defstr",        \
              "char *type",        \
              "int kind",          \
              "long size_bits",    \
              "long size",         \
              "int alignment",     \
              "int n_indirects",   \
              "int is_indirect",   \
              "int convert",       \
              "int onescmp",       \
              "int unsgned",       \
              "fixdes fix",        \
              "fpdes fp",          \
              "memdes *member",    \
              "multides *tuple",   \
              LAST)

/* indirect reference tag */
struct s_PD_itag
   {long nitems;                              /* number of items pointed to */
    char *type;                                  /* type of item pointed to */
    BIGINT addr;                          /* address of the itag owning data */
    int flag;                            /* TRUE if this itag owns the data */
    int length;};          /* byte length of the itag - not written to file */

/* hyper-index support structure */
struct s_dimind
   {long stride;
    long start;
    long stop;
    long step;};

struct s_attribute
   {char *name;
    char *type;
    void **data;
    long size;
    long indx;};

struct s_attribute_value
   {attribute *attr;
    long indx;
    struct s_attribute_value *next;};

/* PD_IMAGE is a faithful shadow of the PG_image which is not defined yet
 * any changes to PG_image must be reflected here
 */

struct s_PD_image
   {int version_id;
    char *label;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    char *element_type;
    unsigned char *buffer;
    int kmax;
    int lmax;
    long size;
    int bits_pixel;
    char *palette;};

struct s_PD_smp_state
   {int append_flag;                  /* PDB state */
    FILE *ofp;
    FILE *cfp;

    char err[MAXLINE];                /* PDPAR state */
    size_t buffer_size;
    data_standard *int_std;
    data_standard *req_std;
    data_alignment *int_align;
    data_alignment *req_align;
    PDBfile *vif;

    PFPDBwrite   wr_hook;
    PFPDBread    rd_hook;

    SC_communicator communicator;     /* PDPARMP state */

    long n_casts;                     /* PDLOW state */
    int has_dirs;
    char **cast_lst;
    char local[MAX_BFSZ];
    char *tbf;
    char *tbuffer;
    long ncx;
    long nc;
    char *spl;

    char **outlist;                   /* PDFIA state */
    int n_entries;

    long *rw_call_stack;              /* PDRDWR state */
    long  rw_call_ptr;
    long  rw_call_ptr_x;
    long  rw_lval_ptr;
    long  rw_lval_ptr_x;
    long  rw_str_ptr;
    long  rw_str_ptr_x;
    long  rw_list_t;
    long  rw_list_d;
    SC_address *rw_lval_stack;
    char **rw_str_stack;

    long *sz_call_stack;              /* PDSZOF state */
    long  sz_call_ptr;
    long  sz_call_ptr_x;
    long  sz_lval_ptr;
    long  sz_lval_ptr_x;
    long  sz_str_ptr;
    long  sz_str_ptr_x;
    long  sz_list_t;
    long  sz_list_d;
    SC_address *sz_lval_stack;
    char **sz_str_stack;

    char cwd[MAXLINE];               /* PDBDIR state */
    char outname[MAXLINE];

    char tmbf[MAXLINE];              /* PDMEMB state */

    PFfopen        pn_open_save;     /* PDBNET state */
    hasharr        *host_chart;
    data_standard  *host_std;
    data_alignment *host_align;

    void        *frames;              /* PDPATH state */
    int         have_colon;
    int         frame_n;
    int         frame_nx;
    PDBfile     *file_s;
    char        outtype[MAXLINE];
    char        msg[MAXLINE];
    char        text[MAXLINE];
    long        num_val;

    JMP_BUF read_err;
    JMP_BUF write_err;
    JMP_BUF print_err;
    JMP_BUF open_err;
    JMP_BUF trace_err;
    JMP_BUF close_err;
    JMP_BUF create_err;};

struct s_PD_pfm_fnc
   {PFfopen open_hook;
    FILE *(*setup_pseudo_file)(BF_FILE *fp);
    FILE *(*get_file_stream)(PDBfile *file);
    BF_FILE *(*get_file_ptr)(FILE *file);
    int (*flush_file)(PDBfile *file);
    int (*extend_file)(PDBfile *file, long nb);
    int (*serial_flush)(FILE *fp, int _t_index);
    int (*set_eod)(PDBfile *file, BIGINT addr, long nb);
    int (*is_dp_init)(void);
    int (*is_smp_init)(void);
    int (*is_null_fp)(void *fp);
    int (*is_sequential)(void);
    int (*is_master)(PDBfile *file);
    BIGINT (*get_file_size)(PDBfile *fp);
    BIGINT (*getspace)(PDBfile *file, size_t nbytes, int rflag, int colf);
    BIGINT (*next_address)(PDBfile *file, char *type, long number,
			  void *vr, int seekf, int tellf, int colf);
    void (*setup_mp_file)(PDBfile *file, SC_communicator comm);
    void (*init)(void);
    void (*add_file)(PDBfile *file, BIGINT start_addr);
    void (*remove_file)(FILE *file);
    void (*mark_as_flushed)(PDBfile *file, int wh);
    void (*set_address)(PDBfile *file, BIGINT addr);};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            PDBLIB VARIABLES                              */

/*--------------------------------------------------------------------------*/

extern PD_pfm_fnc
 PD_par_fnc;

extern int
 _PD_nthreads;

extern long
 PD_print_controls[];

extern int
 PD_fix_precision[];

extern precisionfp
 PD_fp_precision[];

extern int
 FORMAT_FIELDS,
 PD_default_format_version,
 PD_buffer_size,
 PD_DIM,
 PD_tolerance;

extern data_standard
 *INT_STANDARD,
 *REQ_STANDARD,
 *PD_std_standards[];

extern data_alignment
 *INT_ALIGNMENT,
 *REQ_ALIGNMENT,
 *PD_std_alignments[];

extern PDBfile
 *PD_vif;

extern char
 *PDBFILE_S,
 *PD_ALIGNMENT_S,
 *PD_DEFSTR_S,
 *PD_STANDARD_S,
 PD_err[],
 *PD_no_print_member,
 *PD_print_formats1[],
 *PD_print_formats2[],
 *PD_user_formats1[],
 *PD_user_formats2[];

extern JMP_BUF
 _PD_read_err,
 _PD_write_err,
 _PD_print_err,
 _PD_open_err,
 _PD_trace_err,
 _PD_close_err,
 _PD_create_err;

extern PFPDBwrite
 pdb_wr_hook;

extern PFPDBread
 pdb_rd_hook;

extern data_standard
 TEXT_STD,
 PPC32_STD,
 PPC64_STD,
 I386_STD,
 I586L_STD,
 I586O_STD,
 X86_64_STD,
 X86_64A_STD,
 M68X_STD,
 VAX_STD,
 CRAY_STD;

extern data_alignment
 TEXT_ALIGNMENT,
 BYTE_ALIGNMENT,
 WORD2_ALIGNMENT,
 WORD4_ALIGNMENT,
 WORD8_ALIGNMENT,
 GNU4_I686_ALIGNMENT,
 OSX_10_5_ALIGNMENT,
 SPARC_ALIGNMENT,
 XLC32_PPC64_ALIGNMENT,
 CYGWIN_I686_ALIGNMENT,
 GNU3_PPC64_ALIGNMENT,
 GNU4_PPC64_ALIGNMENT,
 XLC64_PPC64_ALIGNMENT,
 GNU4_X86_64_ALIGNMENT,
 PGI_X86_64_ALIGNMENT;

/*--------------------------------------------------------------------------*/

/*                            PDBLIB FUNCTIONS                              */

/*--------------------------------------------------------------------------*/


/* PDB.C declarations */

extern PDBfile
 *PD_open(char *name, char *mode),
 *PD_family(PDBfile *file, int flag),
 *PD_create(char *name),
 *PD_open_vif(char *name);

extern syment
 *PD_defent(PDBfile *file, char *name, char *outtype),
 *PD_defent_alt(PDBfile *file, char *name, char *outtype,
		int nd, long *ind),
 *_PD_write(PDBfile *file, char *name, char *intype, char *outtype,
	    void *vr, dimdes *dims, int appnd, int *pnew);

extern int
 PD_close(PDBfile *file),
 PD_flush(PDBfile *file),
 PD_remove_entry(PDBfile *file, char *name),
 PD_append(PDBfile *file, char *name, void *vr),
 PD_append_as(PDBfile *file, char *name, char *intype, void *vr),
 PD_append_alt(PDBfile *file, char *name, void *vr,
	       int nd, long *ind),
 PD_append_as_alt(PDBfile *file, char *name,
		  char *intype, void *vr,
		  int nd, long *ind),
 PD_write(PDBfile *file, char *name, char *type, void *vr),
 PD_write_as(PDBfile *file, char *name,
	     char *intype, char *outtype, void *vr),
 PD_write_alt(PDBfile *file, char *name, char *type, void *vr,
	      int nd, long *ind),
 PD_write_as_alt(PDBfile *file, char *name,
		 char *intype, char *outtype, void *vr,
		 int nd, long *ind),
 PD_read(PDBfile *file, char *name, void *vr),
 PD_read_as(PDBfile *file, char *name, char *type, void *vr),
 PD_read_as_dwim(PDBfile *file, char *name, char *outtype,
		 long nix, void *space),
 PD_read_alt(PDBfile *file, char *name, void *vr, long *ind),
 PD_read_as_alt(PDBfile *file, char *name, char *type, void *vr,
		long *ind),
 PD_cast(PDBfile *file, char *type, char *memb, char *contr),
 PD_free(PDBfile *file, char *type, void *var),
 PD_fix_denorm(data_standard* std, char *type, BIGINT ni, void *vr),
 PD_autofix_denorm(PDBfile *file, int flag);
 
extern void
 PD_set_io_hooks(int which),
 PD_error(char *s, PD_major_op n);


/* PDBAIB.C declarations */

extern defstr
 *PD_typedef(PDBfile *file, char *oname, char *tname),
 *PD_defncv(PDBfile *file, char *name, long bytespitem, int align),
 *PD_defchr(PDBfile *file, char *name, long bpi, int align,
	    PD_byte_order ord, int unsgned),
 *PD_defix(PDBfile *file, char *name, long bytespitem, int align,
	   PD_byte_order flg),
 *PD_defixnum(PDBfile *file, char *name, long bytespitem, int align,
	      PD_byte_order ord, int unsgned, int onescmp),
 *PD_defloat(PDBfile *file, char *name, long bytespitem, int align,
	     int *ordr, long expb, long mantb, long sbs, long sbe,
	     long sbm, long hmb, long bias),
 *PD_defstr(PDBfile *file, char *name, ...),
 *PD_defstr_alt(PDBfile *file, char *name, int nmemb, char **members);

extern int
 PD_verify_writes(int st),
 PD_change_primitive(PDBfile *file, int ityp, int nb, int algn,
		     long *fpfmt, int *fpord);

extern void
 PD_contents(PDBfile *file, FILE *out, int fh, int vers),
 PD_fp_toler(PDBfile *file, long double *fptol);


/* PDBMM.C declarations */

extern syment
 *PD_copy_syment(syment *osym);

extern memdes
 *PD_copy_members(memdes *desc);

extern dimdes
 *PD_copy_dims(dimdes *odims);

extern void
 *PD_alloc_entry(PDBfile *file, char *name);

extern void
 _PD_rl_syment_d(syment *ep),
 *_PD_alloc_entry(PDBfile *file, char *type, long nitems);


/* PDBDIR.C declarations */

extern char
 **PD_ls(PDBfile *file, char *path, char *type, int *num),
 **PD_ls_alt(PDBfile *file, char *path, char *type, int *num,
             char *flags),
 *PD_pwd(PDBfile *file);

extern int
 PD_cd(PDBfile *file, char *dirname),
 PD_isdir(PDBfile *file, char *dirname),
 PD_mkdir(PDBfile *file, char *dirname),
 PD_ln(PDBfile *file, char *oldname, char *newname),
 PD_def_dir(PDBfile *file);


/* PDBMP.C declarations */

extern PDBfile
 *PD_mp_open(char *name, char *mode, SC_communicator comm),
 *PD_mp_create(char *name, SC_communicator *comm);

extern int
 PD_mp_set_collective(int flag),
 PD_mp_set_serial(PDBfile *file, int flag);


/* PDBNET.C declarations */

extern void
 PN_conv_in(void *out, void *in, char *type, long nitems,
	    hasharr *in_chart),
 PN_conv_out(void *out, void *in, char *type, long nitems,
          hasharr *out_chart);

extern int
 PN_close(PDBfile *file),
 PN_write(PDBfile *file, char *type, long nitems, void *vr),
 PN_read(PDBfile *file, char *type, long nitems, void *vr);

extern PDBfile
 *PN_open(PDBfile *fm, char *bf);

extern hasharr
 *PN_target(data_standard *data, data_alignment *align);

extern defstr
 *PN_defstr(hasharr *fchart, char *name,
	    data_alignment *align, int defoff, ...);


/* PDBX.C declarations */

extern void
 *PD_get_attribute(PDBfile *file, char *vr, char *at);

extern int
 PD_def_pdb_types(PDBfile *file),
 PD_def_attr_str(PDBfile *file),
 PD_def_attribute(PDBfile *file, char *at, char *type),
 PD_set_attribute(PDBfile *file, char *vr, char *at, void *vl),
 PD_rem_attribute(PDBfile *file, char *at),
 PD_def_hash_types(PDBfile *file, int flag),
 PD_read_pdb_curve(PDBfile *fp, char *name, double **pxp, double **pyp,
		   int *pn, char *label,
		   double *pxmn, double *pxmx, double *pymn, double *pymx,
		   PD_curve_io flag),
 PD_wrt_pdb_curve(PDBfile *fp, char *labl, int n,
		  double *px, double *py, int icurve),
 PD_wrt_pdb_curve_y(PDBfile *fp, char *labl, int n, int ix,
		    double *py, int icurve),
 PD_put_mapping(PDBfile *file, PM_mapping *f, int mapping),
 PD_put_image(PDBfile *file, void *f, int image),
 PD_put_set(PDBfile *file, PM_set *s),
 PD_mesh_struct(PDBfile *file),
 PD_def_mapping(PDBfile *fp),
 _PD_disk_addr_sort(haelem **v, int n);

extern attribute
 *PD_mk_attribute(char *at, char *type),
 *PD_inquire_attribute(PDBfile *file, char *name, char *path);

extern attribute_value
 *PD_inquire_attribute_value(PDBfile *file, char *name, char *path);

extern void
 PD_rel_image(PD_image *im),
 PD_rel_attr_table(PDBfile *file);

extern syment
 *PD_write_table(PDBfile *file, char *name, hasharr *tab),
 *PD_replace(PDBfile *file, char *name, char *type, void *vr, dimdes *dims);

extern char
 *PD_process_set_name(char *dname);

extern PD_image
 *PD_make_image(char *name, char *type, void *data, int kmax, int lmax,
		int bpp, double xmin, double xmax, double ymin, double ymax,
		double zmin, double zmax);


/* PDCONV.C declarations */

extern void
 PD_byte_reverse(char *out, long nb, long nitems);

extern int
 PD_convert(char **out, char **in, char *typi, char *typo,
	    long nitems, data_standard *stdi, data_standard *stdo,
	    data_standard *hstd, hasharr *chi, hasharr *cho,
	    int boffs, PD_major_op error);


/* PDGS.C declarations */

extern int
 PD_gather_as(PDBfile *file, char *name, char *type,
	      void *vr, long *sind, int ndst, long *dind),
 PD_gather(PDBfile *file, char *name, void *vr, long *sind,
	   int ndst, long *dind),
 PD_scatter_as(PDBfile *file, char *name, char *intype, char *outtype,
	       void *vr, int nsrc, long *sind, int ndst, long *dind),
 PD_scatter(PDBfile *file, char *name, char *type, void *vr,
	    int nsrc, long *sind, int ndst, long *dind);


/* PDCSUM.C declarations */

extern int
 PD_verify(PDBfile *file),
 PD_activate_cksum(PDBfile *file, PD_checksum_mode flag);


/* PDFIA.C declarations */

extern FIXNUM
 _PD_read_aux(PDBfile *file, char *name, char *type, void *vr,
	      FIXNUM *ind);


/* PDFMT.C declarations */

extern int
 PD_isfile(char *fname);


/* PDLOW.C declarations */

extern defstr
 *PD_inquire_type(PDBfile *file, char *name),
 *PD_inquire_host_type(PDBfile *file, char *name);

extern char
 *PD_get_error(void);

extern int
 PD_get_entry_info(syment *ep, char **ptyp, long *pni, int *pnd, long **pdim);

extern BIGINT
 PD_get_buffer_size(void),
 PD_set_buffer_size(BIGINT v);

extern BIGINT
 PD_entry_set_address(syment *ep, BIGINT a),
 PD_entry_address(syment *ep),
 PD_get_file_length(PDBfile *file);

extern void
 PD_free_entry_info(char *typ, long *pdim),
 PD_rel_entry_info(syment *ep, char *typ, long *pdim),
 PD_typedef_primitive_types(PDBfile *file);


/* PDMEMB.C declarations */

extern char
 *PD_index_to_expr(char *bf, long indx, dimdes *pd, int major_order, int def_off);

extern haelem
 *PD_inquire_symbol(PDBfile *file, char *name, int flag, char *fullname, hasharr *tab);

extern syment
 *PD_inquire_entry(PDBfile *file, char *name, int flag, char *fullname),
 *PD_query_entry(PDBfile *file, char *name, char *fullname),
 *PD_effective_entry(PDBfile *file, char *name, int flag, char *fullname),
 *_PD_effective_ep(PDBfile *file, char *name, int flag, char *fullname);


/* PDPARMP.C declarations */

extern void
 PD_term_mpi(void);

extern int
 PD_init_mpi(int masterproc, int nthreads, PFTid tid);


/* PDPATH.C declarations */

extern long
 PD_hyper_number(PDBfile *file, char *name, syment *ep);


/* PDPRNT.C declarations */

extern int
 PD_print_entry(PDBfile *file, char *name, void *vr, syment *ep),
 PD_write_entry(FILE *f0,
		PDBfile *file, char *name, void *vr, syment *ep,
		int n, long *ind);

extern void
 PD_print_extras(PDBfile *file),
 PD_print_syment(syment *ep),
 PD_print_defstr(defstr *dp),
 PD_write_extras(FILE *f0, PDBfile *file),
 PD_write_syment(FILE *f0, syment *ep),
 PD_write_defstr(FILE *f0, defstr *dp);


/* PDRDWR.C declarations */

extern char
 *PD_dereference(char *s);

extern int
 PD_read_bits(PDBfile *file, char *name, char *type,
	      long nitems, int sgned, int nbits, int padsz, int fpp, long offs,
	      long *pan, char **pdata),
 _PD_indirection(char *s),
 PD_reset_ptr_list(PDBfile *file);


/* PDSER.C declarations */

extern int
 PD_init_threads(int nthreads, PFTid tid),
 PD_init_threads_arg(int c, char **v, char *key, PFTid tid);


/* PDSZOF.C declarations */

extern long
 PD_sizeof(PDBfile *file, char *type, long n, void *vri);

extern int
 PN_relocate(PDBfile *file, char *type, long n);


/* PDTGT.C declarations */

extern int
 PD_target_n_platforms(void),
 PD_target_platform(char *tgt),
 PD_target_platform_n(int np),
 PD_target(data_standard *data, data_alignment *align);

extern char
 *PD_target_platform_name(int np);


#ifdef __cplusplus
}
#endif

#endif

