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
#include "pdb_old.h"
#include "scope_dl.h"

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
 * Version 26 revert and deprecate Alignment extra.
 * It matches versions 24 and earlier for forward compatibility
 * with old codes and is ignored by new versions of PDBLib. 01/21/2011
 *
 * Version 27 support <file>~<start>:<end> container syntax. 08/15/2011
 *
 * Version 28 add path to name of pointer entries in the symbol table
 * A pointer path name is <base><n>#<path>
 * where <base> is the file->ptr_base string
 * If <path> is a directory it must end in a '/', e.g. /a/b/c/
 * otherwise it is names a variable.  In the first case the pointer
 * is associated with some variable in the directory.  In the
 * second it is associated with a specific variable.  11/10/2011
 *
 * Version 29 associate full path name of variable with each
 * of its pointers rather than just the directory in which the
 * variable resides as was done in version 28.  This solves a
 * problem associated with copying PDB files in which the current
 * directory is always '/' so no real variable info was associated
 * with the pointers in the copy.
 * This is forward compatible with older versions except that the
 * delay symbol modes with not work optimally and that feature
 * does not go back before version 28.  02/08/2012
 *
 * Version 30 adds the ability to have the size of pointer members
 * determined by other members similarly to the way types can be
 * controlled by PD_cast.  04/11/2012
 *
 */

#define PDB_SYSTEM_VERSION  30

#define BITS_DEFAULT 8     /* default bits per byte */
#define NSTD         6     /* number of standards currently in the system 
                            * should be same as last standard */

#define PD_TYPI(_n)     _n ## _I
#define PD_TYPPI(_n)    _n ## _P_I
#define PD_TYPN(_n)     PD_gs.tnames[PD_TYPI(_n) - PD_TYPI(PDBfile)]
#define PD_TYPPN(_n)    PD_gs.tpnames[PD_TYPPI(_n) - PD_TYPPI(PDBfile)]

#define PDBFILE_S       PD_gs.tnames[0]
#define PD_DEFSTR_S     PD_gs.tnames[1]
#define PD_ALIGNMENT_S  PD_gs.tnames[2]
#define PD_STANDARD_S   PD_gs.tnames[3]
#define SYMENT_S        PD_gs.tnames[4]
#define SYMENT_P_S      PD_gs.tnames[5]
#define PD_N_TYPES      6

#define TEXT_STD        PD_gs.standards[PD_TEXT_STD]
#define I386_STD        PD_gs.standards[PD_I386_STD]
#define I586L_STD       PD_gs.standards[PD_I586L_STD]
#define I586O_STD       PD_gs.standards[PD_I586O_STD]
#define PPC32_STD       PD_gs.standards[PD_PPC32_STD]
#define X86_64_STD      PD_gs.standards[PD_X86_64_STD]
#define X86_64A_STD     PD_gs.standards[PD_X86_64A_STD]
#define PPC64_STD       PD_gs.standards[PD_PPC64_STD]
#define M68X_STD        PD_gs.standards[PD_M68X_STD]
#define VAX_STD         PD_gs.standards[PD_VAX_STD]
#define CRAY_STD        PD_gs.standards[PD_CRAY_STD]
#define PD_N_STANDARDS  11

#define TEXT_ALIGNMENT         PD_gs.alignments[PD_TEXT_ALGN]
#define BYTE_ALIGNMENT         PD_gs.alignments[PD_BYTE_ALGN]
#define WORD2_ALIGNMENT        PD_gs.alignments[PD_WORD2_ALGN]
#define WORD4_ALIGNMENT        PD_gs.alignments[PD_WORD4_ALGN]
#define WORD8_ALIGNMENT        PD_gs.alignments[PD_WORD8_ALGN]
#define GNU4_I686_ALIGNMENT    PD_gs.alignments[PD_GNU4_I686_ALGN]
#define OSX_10_5_ALIGNMENT     PD_gs.alignments[PD_OSX_10_5_ALGN]
#define SPARC_ALIGNMENT        PD_gs.alignments[PD_SPARC_ALGN]
#define XLC32_PPC64_ALIGNMENT  PD_gs.alignments[PD_XLC32_PPC64_ALGN]
#define CYGWIN_I686_ALIGNMENT  PD_gs.alignments[PD_CYGWIN_I686_ALGN]
#define GNU3_PPC64_ALIGNMENT   PD_gs.alignments[PD_GNU3_PPC64_ALGN]
#define GNU4_PPC64_ALIGNMENT   PD_gs.alignments[PD_GNU4_PPC64_ALGN]
#define XLC64_PPC64_ALIGNMENT  PD_gs.alignments[PD_XLC64_PPC64_ALGN]
#define GNU4_X86_64_ALIGNMENT  PD_gs.alignments[PD_GNU4_X86_64_ALGN]
#define PGI_X86_64_ALIGNMENT   PD_gs.alignments[PD_PGI_X86_64_ALGN]
#define PD_N_ALIGNMENTS        15

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PN_sizeof(type, tab)  _PD_lookup_size(type, tab)

#define PD_get_file_type(file)                                               \
   (((file)->type == NULL) ? PDBFILE_S : (file)->type)

/* deprecated in favor PD_set_fmt_version and PD_get_fmt_version */
#define PD_set_format_version        PD_set_fmt_version
#define PD_get_format_version(_n)    (_n = PD_get_fmt_version())

#define PD_has_directories(file)                                             \
    ((file != NULL) && (file->current_prefix != NULL))

#define PD_inquire_table_type(table, name)                                   \
 (defstr *) SC_hasharr_def_lookup(table, name)

#define PD_hyper_refp(name, commnd)                                          \
   {char c, s[MAXLINE];                                                      \
    SC_strncpy(s, MAXLINE, name, -1);                                        \
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

typedef struct BF_FILE_s BF_FILE;
typedef struct s_chardes chardes;
typedef struct s_fixdes fixdes;
typedef struct s_fpdes fpdes;
typedef struct s_data_alignment data_alignment;
typedef struct s_data_standard data_standard;
typedef struct s_io_request io_request;
typedef struct s_PD_smp_state PD_smp_state;
typedef struct s_PDBfile PDBfile;
typedef struct s_multides multides;
typedef struct s_dimdes dimdes;
typedef struct s_memdes memdes;
typedef struct s_symindir symindir;
typedef struct s_syment syment;
typedef struct s_defstr defstr;
typedef struct s_fltdes fltdes;
typedef struct s_fltdat fltdat;
typedef struct s_sys_layer sys_layer;
typedef struct s_tr_layer tr_layer;
typedef struct s_PD_itag PD_itag;
typedef struct s_dimind dimind;
typedef struct s_attribute attribute;
typedef struct s_attribute_value attribute_value;
typedef struct s_PD_address PD_address;
typedef struct s_PD_cksumdes PD_cksumdes;
typedef struct s_PD_image PD_image;
typedef struct s_PD_pfm_fnc PD_pfm_fnc;
typedef struct s_PD_scope_public PD_scope_public;

typedef int (*PFifltdes)(PDBfile *file, fltdes *fl, fltdat *inf);
typedef memdes *(*PFPDBwrite)(PDBfile *file, char *vr, defstr *defp);
typedef memdes *(*PFPDBread)(memdes *members);

typedef int (*PFSymDelay)(PDBfile *file, int ad, char *name, char *type,
			  char *acc, char *rej);


/*
 * #bind enum PD_data_std_i PD_NO_STD
 */

enum e_PD_data_std_i
   {PD_NO_STD      = -1,
    PD_TEXT_STD    =  0,
    PD_I386_STD    =  1,
    PD_I586L_STD   =  2,
    PD_I586O_STD   =  3,
    PD_PPC32_STD   =  4,
    PD_X86_64_STD  =  5,
    PD_X86_64A_STD =  6,
    PD_PPC64_STD   =  7,
    PD_M68X_STD    =  8,
    PD_VAX_STD     =  9,
    PD_CRAY_STD    = 10};

typedef enum e_PD_data_std_i PD_data_std_i;

/*
 * #bind enum PD_data_algn_i PD_NO_ALGN
 */

enum e_PD_data_algn_i
   {PD_NO_ALGN          = -1,
    PD_TEXT_ALGN        =  0,
    PD_BYTE_ALGN        =  1,
    PD_WORD2_ALGN       =  2,
    PD_WORD4_ALGN       =  3,
    PD_WORD8_ALGN       =  4,
    PD_GNU4_I686_ALGN   =  5,
    PD_OSX_10_5_ALGN    =  6,
    PD_SPARC_ALGN       =  7,
    PD_XLC32_PPC64_ALGN =  8,
    PD_CYGWIN_I686_ALGN =  9,
    PD_GNU3_PPC64_ALGN  = 10,
    PD_GNU4_PPC64_ALGN  = 11,
    PD_XLC64_PPC64_ALGN = 12,
    PD_GNU4_X86_64_ALGN = 13,
    PD_PGI_X86_64_ALGN  = 14};

typedef enum e_PD_data_algn_i PD_data_algn_i;

/*
 * #bind enum PD_major_op PD_GENERIC
 */

enum e_PD_major_op
   {PD_OPEN = 0,
    PD_CREATE,
    PD_CLOSE,
    PD_READ,
    PD_WRITE,
    PD_PRINT,
    PD_TRACE,
    PD_APPEND,
    PD_GENERIC,
    PD_UNINIT};

typedef enum e_PD_major_op PD_major_op;

/*
 * #bind enum PD_byte_order NO_ORDER
 */

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

enum e_PD_text_kind
   {UTF_8 = 0,
    UTF_16,
    UTF_32,
    UTF_NONE};

typedef enum e_PD_text_kind PD_text_kind;

/*
 * #bind enum PD_major_order NO_MAJOR_ORDER
 */

enum e_PD_major_order
   {NO_MAJOR_ORDER = 0,
    ROW_MAJOR_ORDER = 101,
    COLUMN_MAJOR_ORDER};

typedef enum e_PD_major_order PD_major_order;

/*
 * #bind enum PD_curve_io NOR_X_Y
 */

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

/*
 * #bind enum PD_checksum_mode PD_MD5_OFF
 */

#ifdef NON_INTEGER_ENUM

#define PD_MD5_OFF     0
#define PD_MD5_FILE    1
#define PD_MD5_RW      2
#define PD_MD5_FULL    3

typedef int PD_checksum_mode;

#else

enum e_PD_checksum_mode
   {PD_MD5_OFF = 0,
    PD_MD5_FILE,
    PD_MD5_RW,
    PD_MD5_FULL};

typedef enum e_PD_checksum_mode PD_checksum_mode;

#endif

/*
 * #bind enum PD_character_standard PD_UTF_8
 */

enum e_PD_character_standard
   {PD_ITA2_UPPER    = -5,
    PD_ITA2_LOWER    =  5,
    PD_ASCII_6_UPPER = -6,
    PD_ASCII_6_LOWER =  6,
    PD_ASCII_7       =  7,
    PD_UTF_8         =  8,
    PD_EBCDIC        =  157};

typedef enum e_PD_character_standard PD_character_standard;

enum e_PD_data_location
   {LOC_OTHER,
    LOC_HERE,
    LOC_BLOCK};

typedef enum e_PD_data_location PD_data_location;

/*
 * #bind enum PD_delay_mode PD_DELAY_NONE
 */

enum e_PD_delay_mode
   {PD_DELAY_NONE,
    PD_DELAY_PTRS,
    PD_DELAY_ALL};

typedef enum e_PD_delay_mode PD_delay_mode;


/* NOTE: see comment in scope_io.h concerning file_io_desc and the stream member
 *       here the buffer is the analog of the FILE *
 */

struct BF_FILE_s
   {SC_address bf;
    SC_address addr;
    long length;};

struct s_data_alignment
   {int ptr_alignment;
    int bool_alignment;
    int chr[N_PRIMITIVE_CHAR];            /* character types */
    int fx[N_PRIMITIVE_FIX];              /* fixed point types */
    int fp[N_PRIMITIVE_FP];               /* floating point types */
    int struct_alignment;};
 
struct s_chardes
  {int bpi;
   PD_text_kind utf;};

#define PD_DEFINE_CHARDES(_f)              \
    PD_defstr(_f, "chardes",               \
              "int bpi",                   \
              "enum utf",                  \
              LAST)

struct s_fixdes
   {int bpi;
    PD_byte_order order;};

#define PD_DEFINE_FIXDES(_f)               \
    PD_defstr(_f, "fixdes",                \
              "int bpi",                   \
              "enum order",                \
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

struct s_io_request
   {PD_major_op oper;       /* requested operation */
    char *base_name;        /* base path name - no member or index expr */
    char *base_type;        /* base variable type - pointers ok */
    defstr *ftype;          /* file type of base */
    defstr *htype;};        /* host type of base */

struct s_data_standard
   {int bits_byte;
    int ptr_bytes;
    int bool_bytes;
    chardes chr[N_PRIMITIVE_CHAR];
    fixdes fx[N_PRIMITIVE_FIX];
    fpdes  fp[N_PRIMITIVE_FP];
    PDBfile *file;};

struct s_tr_layer
   {char *type;                       /* file type - e.g. PDBfile*/
    char *fmt;                        /* format name - e.g. pdb */
    int (*gettype)(char *type);
    PDBfile *(*create)(tr_layer *tr, SC_udl *pu,
		       char *name, char *mode, void *a);
    PDBfile *(*open)(tr_layer *tr, SC_udl *pu,
		     char *name, char *mode, void *a);
    int64_t (*close)(PDBfile *file);
    int (*read)(PDBfile *file, char *path, char *ty,
		syment *ep, void *vr, int nd, long *ind);
    syment *(*write)(PDBfile *file, char *path,
		     char *inty, char *outty,
		     void *vr, dimdes *dims,
		     int appnd, int *pnew);};

struct s_sys_layer
   {int64_t (*read)(PDBfile *file, syment *ep,
		    char *outtype, void *vr);
    int64_t (*write)(PDBfile *file, char *name, char *vr, int64_t ni,
		     char *intype, char *outtype);};

struct s_PD_address
   {int indx;                  /* indx for /&ptrs */
    PD_data_location loc;      /* location of data wrt itag */
    int written;               /* 1 if data written, 0 if not */
    int64_t addr;              /* disk address of start of data or itag */
    int64_t reta;              /* disk address of end of data */
    char *ptr;                 /* memory address of data */
    syment *entry;};           /* symbol table entry */

/*
 * #bind struct PD_cksumdes
 */

struct s_PD_cksumdes
   {int verified;              /* is the file currently verified */
    PD_checksum_mode use;      /* session use of checksums */
    PD_checksum_mode file;};   /* file use of checksums */

/*
 * #bind struct PDBfile
 */

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
    SC_array *ap;                     /* address/pointer list */
    long n_dyn_spaces;
    int mpi_file;
    int mpi_mode;                     /* serial (1) parallel (0) */
    int64_t maximum_size;             /* for file family bookkeeping */
    int64_t headaddr;
    int64_t symtaddr;
    int64_t chrtaddr;
    int64_t bfsz;                     /* buffer size for chart, symtab, ... */
    PD_cksumdes cksum;                /* checksum state */
    int fix_denorm;
    PD_delay_mode delay_sym;

    sys_layer *sys;
    tr_layer *tr;

    void *meta;                       /* container for metadata in the tr layer */
    io_request req;
    fltdes *block_chain;
    fltdes *file_chain;
    
    int (*symatch)(PDBfile *file, int ad, char *name, char *type,
		   char *acc, char *rej);

/* the db_layer methods */
    int (*create)(PDBfile *file, char *mode, int mst);
    int (*open)(PDBfile *file, char *mode);
    int (*flush)(PDBfile *file);

    int64_t (*wr_symt)(PDBfile *file);
    int (*rd_symt)(PDBfile *file, char *acc, char *rej);
    int (*parse_symt)(PDBfile *file, char *bf, int flag, char *acc, char *rej);

    int (*wr_meta)(PDBfile *file, FILE *out, int fh);
    int (*wr_prim_types)(FILE *fp, hasharr *tab);
    int (*rd_prim_types)(PDBfile *file, char *bf);

    int (*wr_itag)(char *s, int nc, char *type, int64_t ni,
		   int64_t addr, PD_data_location loc);
    char **(*rd_itag)(char *bf);

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
    dimdes *next;};

#define PD_DEFINE_DIMDES(_f)        \
    PD_defstr(_f, "dimdes",         \
              "long index_min",     \
              "long index_max",     \
              "long number",        \
              "dimdes *next",       \
              LAST)

/* member descriptor - describe a member efficiently
 *
 * #bind struct memdes
 */

struct s_memdes
   {char *member;
    int64_t member_offs;
    char *cast_memb;
    long cast_offs;
    char **size_memb;
    int64_t *size_offs;
    int is_indirect;
    char *type;
    char *base_type;
    char *name;
    dimdes *dimensions;
    long number;
    memdes *next;};

FUNCTION_POINTER(memdes, *(*PFPmemdes));

#define PD_DEFINE_MEMDES(_f)              \
    PD_defstr(_f, "memdes",               \
              "char *member",             \
              "int64_t member_offs",      \
              "char *cast_memb",          \
              "long cast_offs",           \
              "char **size_memb",         \
              "int64_t *size_offs",       \
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
   {int64_t addr;
    long n_ind_type;
    long arr_offs;};

#define PD_DEFINE_SYMINDIR(_f)            \
  PD_defstr(_f, "symindir",               \
	    "int64_t addr",               \
	    "long n_ind_type",            \
	    "long arr_offs",              \
            LAST)

/* symbol table entry
 *
 * #bind struct syment
 */

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

/* structure chart entry
 *
 * #bind struct defstr
 */

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
              "enum kind",         \
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
   {int64_t nitems;                           /* number of items pointed to */
    char *type;                                  /* type of item pointed to */
    int64_t addr;                        /* address of the itag owning data */
    PD_data_location flag;                      /* which itag owns the data */
    int length;};          /* byte length of the itag - not written to file */

/* hyper-index support structure */
struct s_dimind
   {long stride;
    long start;
    long stop;
    long step;};

/* FLTDAT - contain info needed to do filtering */

struct s_fltdat
   {char *type;               /* data type */
    long bpi[2];              /* bytes per item in and out */
    int64_t ni[2];            /* number of items in and out */
    int64_t nb[2];            /* buffer size in and out */
    unsigned char *bf[2];};   /* buffers in and out */

/* FLTDES - generic filter descriptor */

struct s_fltdes
   {char *name;
    void *data;
    PFifltdes in;
    PFifltdes out;
    fltdes *next;
    fltdes *prev;};

struct s_attribute
   {char *name;
    char *type;
    void **data;
    long size;
    long indx;};

struct s_attribute_value
   {attribute *attr;
    long indx;
    attribute_value *next;};

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

struct s_PD_pfm_fnc
   {PFfopen open_hook;
    FILE *(*setup_pseudo_file)(BF_FILE *fp);
    FILE *(*get_file_stream)(PDBfile *file);
    BF_FILE *(*get_file_ptr)(FILE *file);
    int (*flush_file)(PDBfile *file);
    int (*extend_file)(PDBfile *file, long nb);
    int (*serial_flush)(FILE *fp, int _t_index);
    int (*set_eod)(PDBfile *file, int64_t addr, long nb);
    int (*is_dp_init)(void);
    int (*is_smp_init)(void);
    int (*is_null_fp)(void *fp);
    int (*is_sequential)(void);
    int (*is_master)(PDBfile *file);
    int64_t (*get_file_size)(PDBfile *fp);
    int64_t (*getspace)(PDBfile *file, size_t nbytes, int rflag, int colf);
    int64_t (*next_address)(PDBfile *file, char *type, long number,
			  void *vr, int seekf, int tellf, int colf);
    void (*setup_mp_file)(PDBfile *file, SC_communicator comm);
    void (*init)(void);
    void (*add_file)(PDBfile *file, int64_t start_addr);
    void (*remove_file)(FILE *file);
    void (*mark_as_flushed)(PDBfile *file, int wh);
    void (*set_address)(PDBfile *file, int64_t addr);};

struct s_PD_scope_public
   {int nthreads;
    int format_fields;
    int default_format_version;
    int buffer_size;
    int host_order;

    long print_ctrl[10];

    char err[MAXLINE];
    char *tnames[PD_N_TYPES];

    PDBfile *vif;
    PFPDBwrite write;                    /* semi-obsolete read/write hooks */
    PFPDBread read;

    data_standard *int_standard;
    data_standard *req_standard;
    data_standard standards[PD_N_STANDARDS];

    data_alignment *int_alignment;
    data_alignment *req_alignment;
    data_alignment alignments[PD_N_ALIGNMENTS];

/* NOTE: these next two members are deprecated - do not use */
    data_standard *std_standards[PD_N_STANDARDS + 1];
    data_alignment *std_alignments[PD_N_ALIGNMENTS + 1];

    PD_pfm_fnc par;};

/* per thread private state */

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

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                           VARIABLE DECLARATIONS                          */

/*--------------------------------------------------------------------------*/

extern PD_scope_public
 PD_gs;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

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

extern int64_t
 PD_close_n(PDBfile *file),
 PD_flush(PDBfile *file);

extern int
 PD_close(PDBfile *file),
 PD_read(PDBfile *file, char *name, void *vr),
 PD_read_as(PDBfile *file, char *name, char *type, void *vr),
 PD_read_as_dwim(PDBfile *file, char *name, char *outtype,
		 long nix, void *space),
 PD_read_alt(PDBfile *file, char *name, void *vr, long *ind),
 PD_read_as_alt(PDBfile *file, char *name, char *type, void *vr,
		long *ind),
 PD_autofix_denorm(PDBfile *file, int flag);
 
extern pboolean
 PD_copy_type(PDBfile *sf, PDBfile *df, char *type),
 PD_write(PDBfile *file, char *name, char *type, void *vr),
 PD_write_as(PDBfile *file, char *name,
	     char *intype, char *outtype, void *vr),
 PD_write_alt(PDBfile *file, char *name, char *type, void *vr,
	      int nd, long *ind),
 PD_write_as_alt(PDBfile *file, char *name,
		 char *intype, char *outtype, void *vr,
		 int nd, long *ind),
 PD_append(PDBfile *file, char *name, void *vr),
 PD_append_as(PDBfile *file, char *name, char *intype, void *vr),
 PD_append_alt(PDBfile *file, char *name, void *vr,
	       int nd, long *ind),
 PD_append_as_alt(PDBfile *file, char *name,
		  char *intype, void *vr,
		  int nd, long *ind),
 PD_free(PDBfile *file, char *type, void *var),
 PD_remove_entry(PDBfile *file, char *name),
 PD_fix_denorm(data_standard* std, char *type, int64_t ni, void *vr);

extern void
 PD_set_io_hooks(int which),
 PD_error(char *s, PD_major_op n);


/* PDBAIB.C declarations */

extern defstr
 *PD_typedef(PDBfile *file, char *oname, char *tname),
 *PD_defncv(PDBfile *file, char *name, long bpi, int align),
 *PD_defchr(PDBfile *file, char *name, long bpi, int align,
	    PD_byte_order ord, int unsgned),
 *PD_defix(PDBfile *file, char *name, long bpi, int align,
	   PD_byte_order flg),
 *PD_defixnum(PDBfile *file, char *name, long bpi, int align,
	      PD_byte_order ord, int unsgned, int onescmp),
 *PD_defloat(PDBfile *file, char *name, long bpi, int align,
	     int *ordr, long expb, long mantb, long sbs, long sbe,
	     long sbm, long hmb, long bias),
 *PD_defstr(PDBfile *file, char *name, ...),
 *PD_defstr_alt(PDBfile *file, char *name, int nmemb, char **members),
 *PD_defstr_s(PDBfile *file, char *name, char *members);

extern int
 PD_verify_writes(int st),
 PD_change_primitive(PDBfile *file, int ityp, int nb, int algn,
		     long *fpfmt, int *fpord);

extern void
 PD_contents(PDBfile *file, FILE *out, int fh, int vers),
 PD_fp_toler(PDBfile *file, long double *fptol);


/* PDCAST.C declarations */

extern int
 PD_cast(PDBfile *file, char *type, char *memb, char *contr),
 PD_size_from(PDBfile *file, char *type, char *memb, char *contr);


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
 PD_finish(void);


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


/* PDBFC.C declarations */

extern PDBfile
 *PD_open_contained(char *name, int64_t sad, int64_t ead);


/* PDBMP.C declarations */

extern PDBfile
 *PD_mp_open(char *name, char *mode, SC_communicator comm),
 *PD_mp_create(char *name, SC_communicator *comm);

extern int
 PD_mp_set_collective(int flag),
 PD_mp_set_serial(PDBfile *file, int flag);


/* PDBNET.C declarations */

extern void
 PN_conv_in(void *out, void *in, char *type, int64_t ni,
	    hasharr *in_chart),
 PN_conv_out(void *out, void *in, char *type, int64_t ni,
          hasharr *out_chart);

extern int
 PN_close(PDBfile *file),
 PN_write(PDBfile *file, char *type, int64_t ni, void *vr),
 PN_read(PDBfile *file, char *type, int64_t ni, void *vr);

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

extern char
 *PD_process_set_name(char *dname);

extern PD_image
 *PD_make_image(char *name, char *type, void *data, int kmax, int lmax,
		int bpp, double xmin, double xmax, double ymin, double ymax,
		double zmin, double zmax);


/* PDCONV.C declarations */

extern void
 PD_byte_reverse(char *out, int32_t nb, int64_t ni);

extern int
 PD_convert(char **out, char **in, char *typi, char *typo,
	    int64_t ni, data_standard *stdi, data_standard *stdo,
	    data_standard *hstd, hasharr *chi, hasharr *cho,
	    int boffs, PD_major_op error),
 PD_n_bit_char_std(PD_character_standard cstd);

extern char
 *PD_convert_ascii(char *out, int nc, PD_character_standard cstd,
		   char *in, int64_t nb),
 *PD_conv_from_ascii(char *out, int nc, PD_character_standard cstd,
		     char *in, int64_t nb);


/* PDGS.C declarations */

extern int64_t
 PD_gather_as(PDBfile *file, char *name, char *type,
	      void *vr, long *sind, int ndst, long *dind),
 PD_gather(PDBfile *file, char *name, void *vr, long *sind,
	   int ndst, long *dind);

extern int
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


/* PDFLT.C declarations */

extern fltdes
 *PD_filt_make_chain(fltdes *fl, char *name,
		     PFifltdes fi, PFifltdes fo, void *data);

extern int
 PD_filt_register_block(PDBfile *file, fltdes *fl),
 PD_filt_register_file(fltdes *fl);

extern void
 PD_filt_free_chain(fltdes *fl);


/* PDFMT.C declarations */

extern int
 PD_isfile(char *fname);


/* PDLOW.C declarations */

extern dimdes
 *PD_entry_dimensions(syment *ep);

extern defstr
 *PD_inquire_type(PDBfile *file, char *name),
 *PD_inquire_host_type(PDBfile *file, char *name);

extern char
 *PD_get_file_name(PDBfile *file),
 *PD_entry_type(syment *ep),
 *PD_get_error(void);

extern PD_major_op
 PD_get_mode(PDBfile *file),
 PD_set_mode(PDBfile *file, PD_major_op v);

extern PD_major_order
 PD_get_major_order(PDBfile *file),
 PD_set_major_order(PDBfile *file, PD_major_order v);

extern int64_t
 PD_get_buffer_size(void),
 PD_set_buffer_size(int64_t v),
 PD_entry_set_address(syment *ep, int64_t a),
 PD_entry_address(syment *ep),
 PD_get_file_length(PDBfile *file),
 PD_get_max_file_size(PDBfile *file),
 PD_set_max_file_size(PDBfile *file, int64_t v);

extern long
 PD_get_pointer_size(void),
 PD_set_pointer_size(long n);

extern int
 PD_get_system_version(PDBfile *file),
 PD_get_entry_info(syment *ep, char **ptyp, long *pni, int *pnd, long **pdim),
 PD_get_offset(PDBfile *file),
 PD_set_offset(PDBfile *file, int v),
 PD_get_track_pointers(PDBfile *file),
 PD_set_track_pointers(PDBfile *file, int v),
 PD_get_fmt_version(void),
 PD_set_fmt_version(int v),
 PD_entry_number(syment *ep),
 PD_entry_n_dimensions(syment *ep),
 PD_type_size(defstr *dp),
 PD_type_alignment(defstr *dp),
 PD_type_n_indirects(defstr *dp);

extern void
 PD_free_entry_info(char *typ, long *pdim),
 PD_rel_entry_info(syment *ep, char *typ, long *pdim),
 PD_typedef_primitive_types(PDBfile *file);


/* PDMAP.C declarations */

extern int
 PD_wrt_curve_alt(PDBfile *file, char *labl,
		  int n, double *x, double *y, int *sic),
 PD_wrt_curve_y_alt(PDBfile *file, char *labl,
		    int n, int ix, double *y, int *sic),
 PD_wrt_set(PDBfile *file, char *dname, int *adp, double *adm),
 PD_wrt_map(PDBfile *file,
	    char *dname, int *adp, double *adm,
	    char *rname, int *arp, double *arm,
	    int *sim),
 PD_wrt_map_ran(PDBfile *file, char *dname,
		char *rname, int *arp, double *arm,
		pcons *info, int *sim);


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


/* PDPTR.C declarations */

extern int
 PD_reset_ptr_list(PDBfile *file);


/* PDRDWR.C declarations */

extern long
 PD_hyper_number(PDBfile *file, char *name, syment *ep);

extern char
 *PD_dereference(char *s);

extern int
 PD_read_bits(PDBfile *file, char *name, char *type,
	      int64_t ni, int sgned, int nbits, int padsz, int fpp,
	      int64_t offs, long *pan, char **pdata),
 _PD_indirection(char *s);


/* PDSER.C declarations */

extern int
 PD_init_threads(int nthreads, PFTid tid),
 PD_init_threads_arg(int c, char **v, char *key, PFTid tid);


/* PDSPOKE.C declarations */

extern int
 PD_register_xml(void),
 PD_register_sql(void),
 PD_register_nacf(void),
 PD_register_llf(void),
 PD_register_hdf5(void),
 PD_register_pdb(void);


/* PDSYMT.C declarations */

extern PFSymDelay
 PD_set_delay_method(PFSymDelay *mth);


/* PDSZOF.C declarations */

extern long
 PD_sizeof(PDBfile *file, char *type, int64_t n, void *vri);

extern int
 PN_relocate(PDBfile *file, char *type, long n);


/* PDTGT.C declarations */

extern int
 PD_target_n_platforms(void),
 PD_target_platform(char *tgt),
 PD_target_platform_n(int np),
 PD_target(data_standard *data, data_alignment *align),
 PD_target_i(PD_data_std_i is, PD_data_algn_i ia);

extern char
 *PD_target_platform_name(int np);


#ifdef __cplusplus
}
#endif

#endif

