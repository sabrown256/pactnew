/*
 * SCOPE_IO.H - support I/O services in SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_IO

#define PCK_SCOPE_IO

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

#define SC_DEFEAT_MPI_BUG  "\f\t\n"

#define SC_N_IO_OPER  21

/* optimal buffer size - 2 MB */
#define SC_OPT_BFSZ   (1 << 21)

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* IO Hooks */

/* general file I/O hooks */

#define io_open     (*_SC_ios.sfopen)
#define lio_open    (*_SC_ios.lfopen)

#define SC_lmf_insert    SC_mf_insert

#define SC_set_put_line(_f)  (_SC_ios.putln = (PFfprintf) (_f))
#define SC_get_put_line(_f)  (_f = _SC_ios.putln)
#define SC_put_line_cmp(_f)  (_SC_ios.putln == (PFfprintf) (_f))

#define SC_set_get_line(_f)  (_SC_ios.getln = (PFfgets) (_f))
#define SC_get_get_line(_f)  (_f = _SC_ios.getln)
#define SC_get_line_cmp(_f)  (_SC_ios.getln == (PFfgets) (_f))

#define SC_set_put_string(_f)  (_SC_ios.putstr = (PFfputs) (_f))
#define SC_get_put_string(_f)  (_f = _SC_ios.putstr)
#define SC_put_string_cmp(_f)  (_SC_ios.putstr == (PFfputs) (_f))

#undef STDOUT
#define STDOUT ((SC_gs.comm_rank == 0) ? stdout : NULL)

#undef PRINT
#define PRINT (*_SC_ios.putln)

#undef PUTS
#define PUTS (*_SC_ios.putstr)

#undef GETLN
#define GETLN (*_SC_ios.getln)

#define SC_HAVE_LARGE_FILES   (sizeof(int64_t) == sizeof(long long))


/* general file I/O hooks */

/* return TRUE if file is a standard I/O file desc */

#define IS_STD_IO(_f)                                                        \
    (((_f) == stdout) || ((_f) == stdin)  || ((_f) == stderr))

#define FILE_IO_DATA(_f)                                                     \
    (IS_STD_IO(_f) ?                                                         \
     _f :                                                                    \
     ((FILE *) (((file_io_desc *) (_f))->info)))

#define FILE_IO_INFO(_t, _f)                                                 \
    (((_f) == NULL) ? NULL : ((_t *) (((file_io_desc *) (_f))->info)))

#define FILE_IO_FNC(_f, _m)   (((file_io_desc *) (_f))->_m)

#define SAFE_FPUTS(_s, _f)   SC_fwrite_sigsafe((void *) _s, 1, strlen(_s), _f)

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

enum e_io_oper
   {IO_OPER_NONE, IO_OPER_FOPEN, IO_OPER_FCLOSE,
    IO_OPER_FTELL, IO_OPER_LFTELL, IO_OPER_FSEEK, IO_OPER_LFSEEK,
    IO_OPER_FREAD, IO_OPER_LFREAD, IO_OPER_FWRITE, IO_OPER_LFWRITE,
    IO_OPER_FPRINTF, IO_OPER_FPUTS, IO_OPER_FGETS, IO_OPER_FGETC,
    IO_OPER_UNGETC, IO_OPER_FFLUSH, IO_OPER_FEOF, IO_OPER_SETVBUF,
    IO_OPER_POINTER, IO_OPER_SEGSIZE};

typedef enum e_io_oper io_oper;

enum e_SC_io_kind
   {SC_IO_STD_NONE = -1, SC_IO_STD_IN, SC_IO_STD_OUT,
    SC_IO_STD_ERR, SC_IO_STD_BOND};

typedef enum e_SC_io_kind SC_io_kind;

enum e_SC_io_device
   {SC_IO_DEV_NONE,
    SC_IO_DEV_PIPE, SC_IO_DEV_SOCKET, SC_IO_DEV_PTY, 
    SC_IO_DEV_TERM, SC_IO_DEV_FILE, SC_IO_DEV_VAR, SC_IO_DEV_EXPR };

typedef enum e_SC_io_device SC_io_device;

enum e_SC_io_mode
   {SC_IO_MODE_NONE,
    SC_IO_MODE_RO, SC_IO_MODE_WO, SC_IO_MODE_WD, SC_IO_MODE_APPEND };

typedef enum e_SC_io_mode SC_io_mode;

enum e_SC_task_kind
   {TASK_NONE, TASK_COMPOUND, TASK_GROUP};

typedef enum e_SC_task_kind SC_task_kind;

typedef struct s_SC_iodes SC_iodes;
typedef struct s_SC_ttydes SC_ttydes;

FUNCTION_POINTER(FILE,  *(*PFPFILE));

struct s_SC_iodes
   {int fd;
    int fid;                /* file descriptor for redirect - 2>&1 type */
    int gid;                /* index of process group member for redirect */
    int flag;               /* file open mode flags */
    SC_io_kind knd;
    SC_io_device dev;
    SC_io_mode mode;
    char *file;             /* file name */
    char *raw;};

struct s_SC_ttydes
   {int nr;                  /* number of rows for TTY */
    int nc;                  /* number of columns for TTY */
    int pw;                  /* window pixel width */
    int ph;};                /* window pixel height */


/* file I/O function types */

typedef FILE *(*PFfopen)(char *name, char *mode);

typedef long (*PFftell)(FILE *stream);
typedef int (*PFfseek)(FILE *stream, long offs, int whence);

typedef size_t (*PFfread)(void *ptr, size_t sz, size_t ni, FILE *stream);
typedef size_t (*PFfwrite)(void *ptr, size_t sz, size_t ni, FILE *stream);

typedef int (*PFsetvbuf)(FILE *stream, char *bf, int type, size_t sz);
typedef int (*PFfclose)(FILE *stream);
typedef int (*PFfprintf)(FILE *stream, char *fmt, ...);
typedef int (*PFfputs)(const char *s, FILE *stream);
typedef int (*PFfgetc)(FILE *stream);
typedef int (*PFungetc)(int c, FILE *stream);
typedef int (*PFfflush)(FILE *stream);
typedef int (*PFfeof)(FILE *stream);

typedef char *(*PFfgets)(char *s, int n, FILE *stream);

typedef ssize_t (*PFread)(int fd, void *bf, size_t nc);
typedef ssize_t (*PFwrite)(int fd, void *bf, size_t nc);

typedef void (*PFIOErr)(int err, char *sys, char *usr);


/* large file I/O function types */

typedef u_int64_t (*PFSegSize)(void *fp, int64_t nsz);
typedef u_int64_t (*PFlread)(void *ptr, size_t sz, u_int64_t ni, FILE *stream);
typedef u_int64_t (*PFlwrite)(void *ptr, size_t sz, u_int64_t ni, FILE *stream);

typedef struct s_file_io_desc file_io_desc;

/* NOTE: to make life tractable use the convention that if the info
 * member points to a struct, the first member of the struct is
 * the pointer to the stream (file, socket, etc)
 * That way generic routines can tell if there is a non-NULL file
 * in any file_io_desc instance.
 */

struct s_file_io_desc
   {void *info;
    FILE *(*fopen)(char *name, char *mode);
    long (*ftell)(FILE *fp);
    int64_t (*lftell)(FILE *fp);
    int (*fseek)(FILE *fp, long offs, int whence);
    int (*lfseek)(FILE *fp, int64_t offs, int whence);
    size_t (*fread)(void *ptr, size_t sz, size_t ni, FILE *fp);
    u_int64_t (*lfread)(void *ptr, size_t sz, u_int64_t ni, FILE *fp);
    size_t (*fwrite)(void *ptr, size_t sz, size_t ni, FILE *fp);
    u_int64_t (*lfwrite)(void *ptr, size_t sz, u_int64_t ni, FILE *fp);
    int (*setvbuf)(FILE *fp, char *bf, int type, size_t sz);
    int (*fclose)(FILE *fp);
    int (*fprintf)(FILE *fp, char *fmt, va_list a);
    int (*fputs)(const char *s, FILE *fp);
    int (*fgetc)(FILE *fp);
    int (*ungetc)(int c, FILE *fp);
    int (*fflush)(FILE *fp);
    int (*feof)(FILE *fp);
    char *(*fgets)(char *s, int n, FILE *fp);
    char *(*pointer)(FILE *fp);
    u_int64_t (*segsize)(FILE *fp, int64_t n);
    int gather;
    int nhits[SC_N_IO_OPER];
    double nsec[SC_N_IO_OPER];};


#ifdef HAVE_MMAP

typedef struct s_SC_file_block SC_file_block;
typedef struct s_SC_mapped_file SC_mapped_file;
typedef void (*PFMMTrav)(SC_mapped_file *mf, char *s, void *a, int64_t nb);

struct s_SC_file_block
   {int64_t off;                  /* physical offset from the beginning of the file */
    int64_t start;                /* logical start of block */
    int64_t end;                  /* logical end of block */
    SC_mapped_file *file;
    SC_file_block *next;};

/* NOTES on the mapped file structure
 *   1) the mapped file can be used on 32 bit architectures to
 *      access 64 bit addressable files
 *      to do this two views are maintained
 *         a) a logical view for the application
 *         b) a physical view in which a 32 bit addressable stripe
 *            is mapped for piecewise access
 *      for files small enough to be accessed solely by 32 bit operations
 *      one may use the SC_mf_ functions
 *      for files larger than 32 bit operations can handle
 *      one may use the SC_lmf_ functions
 *
 *   2) a list of file blocks is maintained in the logical view
 *      which allows for rearranging the contents of the file
 *      (i.e. deleting sections of the file or adding sections from
 *      other mapped files)
 *      this facilitates manipulating large files for purposed such
 *      as editing
 *      operations on the file contents happen instantly
 *      the cost of actually moving data on disk is deferred to closing
 *      the file at which time a properly ordered stream of bytes is
 *      written to disk
 */

/* NOTE: see comment above concerning file_io_desc and the stream member
 *       here the buffer is the analog of the FILE *
 */
struct s_SC_mapped_file
   {void *scp;                    /* pointer to start of current stripe */

    char *name;
    int fd;
    int prot;
    int share;
    int perm;
    int extend;
    int page;                     /* pagesize */
                                  /* logical file description members */
    int64_t lcpos;                /* current file position (LC) */
    int64_t lclen;                /* total file length (LC) */
    int64_t lcposx;               /* truncate to this length (LC) */
    int64_t lcoff;                /* offset of the current stripe (LC) */

                                  /* physical stripe description members */
    size_t scsize;                /* size of the current stripe (SC) */
    size_t scsizex;               /* maximum/nominal stripe size (SC) */

    SC_file_block *map;

/* low level system call interfaces */
    int (*mopenf)(const char *file, int flag, ...);
    void *(*mmapf)(void *addr, size_t len, int prot,
		   int flags, int fd, int64_t offs);
    int64_t (*mwritef)(int fd, const void *buf, size_t nb, int64_t off);
    void (*setup)(SC_mapped_file *mf);
    
    file_io_desc fid;};

#endif


/* UDL support */

typedef struct s_SC_udl SC_udl;

struct s_SC_udl
   {FILE *stream;       /* actual data medium - usually file_io_desc */
    char *mode;         /* file mode - r, w, a */
    char *udl;          /* original UDL */
    char *protocol;
    char *server;
    char *path;         /* path to file or container */
    char *entry;        /* entry in container */
    char *target;       /* data_standard for output files */
    char *format;       /* PDB, HDF5, ... */
    char *buffer;       /* I/O buffer for setvbuf */
    int64_t address;      /* file address for remote acceses */
    pcons *info;};      /* other attributes */


/* file container support */

enum e_SC_file_type
   {SC_NO_FILE = 9,
    SC_ASCII,
    SC_BINARY,
    SC_PDB,
    SC_TAR,
    SC_AR,
    SC_OTHER,
    SC_UNKNOWN = 0};

typedef enum e_SC_file_type SC_file_type;

typedef struct s_fcdes fcdes;
typedef struct s_fcent fcent;
typedef struct s_tarhdr tarhdr;
typedef struct s_fcontainer fcontainer;

/* NOTE: see comment above concerning file_io_desc and the stream member */
struct s_fcdes
   {FILE *file;          /* the file pointer */
    int hdr_size;        /* size of container header */
    char *name;          /* name of container file */
    hasharr *entries;};  /* table of entries in file */

struct s_fcent
   {char *name;
    int64_t address;
    time_t date;
    int uid;
    int gid;
    size_t size;
    char perm[8];};

struct s_tarhdr
   {char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char link;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char fill[12];};     /* fill out to TAR_BLOCKSIZE */

struct s_fcontainer
   {SC_file_type type;
    char *name;
    fcdes *handle;};


typedef struct s_SC_scope_io SC_scope_io;

struct s_SC_scope_io
   {PFfgets getln;                           /* line input function pointer */
    PFfprintf putln;                              /* print function pointer */
    PFfputs putstr;                               /* print function pointer */
    FILE *(*sfopen)(char *name, char *mode);
    FILE *(*lfopen)(char *name, char *mode);
    void (*error)(int err, char *sys, char *usr);};


enum e_SC_file_cat
   {SC_LOCAL = 201,
    SC_REMOTE,
    SC_BUFFERED,
    SC_PARALLEL};

typedef enum e_SC_file_cat SC_file_cat;

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SC_scope_io
 _SC_ios;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* SCAR.C declarations */

extern fcdes
 *SC_scan_archive(char *arf);

extern int
 _SC_is_archive(FILE *fp);


/* SCBIO.C declarations */

extern FILE
 *SC_bopen(char *name, char *mode),
 *SC_lbopen(char *name, char *mode);

extern void
 SC_bf_set_hooks(void),
 SC_lbf_set_hooks(void);

extern long
 *SC_binfo(void);


/* SCFCNTNR.C declarations */

extern fcontainer
 *SC_open_fcontainer(char *name, SC_file_type type,
		     fcdes *(*meth)(char *name, SC_file_type *pt));

extern char
 **SC_fcontainer_list(fcontainer *cf, int full);

extern void
 SC_free_fcontainer(fcdes *fc),
 SC_list_fcontainer(FILE *f, fcontainer *cf, int full),
 SC_close_fcontainer(fcontainer *cf),
 SC_describe_fcontainer(FILE *f, fcontainer *cf, int full);


/* SCFIO.C declarations */

extern file_io_desc
 *SC_make_io_desc(void *fp);

extern FILE
 *SC_fwrap(FILE *fp),
 *SC_fopen(char *name, char *mode),
 *SC_lfwrap(FILE *fp),
 *SC_lfopen(char *name, char *mode),
 *SC_std_file(void *p);

extern void
 io_error(int err, char *fmt, ...),
 SC_get_io_info(FILE *fp, int **pnhits, double **pnsec),
 SC_setbuf(FILE *fp, char *bf);

extern int
 SC_collect_io_info(int wh),
 SC_gather_io_info(FILE *fp, int wh),
 SC_io_connect(int flag),
 SC_file_access(int log),
 SC_exit_all(void),
 io_seek(FILE *fp, long offs, int whence),
 io_setvbuf(FILE *fp, char *bf, int type, size_t sz),
 io_close(FILE *fp),
 io_printf(FILE *fp, char *fmt, ...),
 io_puts(const char *s, FILE *fp),
 io_getc(FILE *fp),
 io_ungetc(int c, FILE *fp),
 io_flush(FILE *fp),
 io_eof(FILE *fp),
 lio_seek(FILE *fp, int64_t offs, int whence),
 lio_setvbuf(FILE *fp, char *bf, int type, size_t sz),
 lio_close(FILE *fp),
 lio_printf(FILE *fp, char *fmt, ...),
 lio_puts(const char *s, FILE *fp),
 lio_getc(FILE *fp),
 lio_ungetc(int c, FILE *fp),
 lio_flush(FILE *fp),
 lio_eof(FILE *fp);

extern long
 io_tell(FILE *fp);

extern size_t
 io_read(void *ptr, size_t sz, size_t ni, FILE *fp),
 io_write(void *ptr, size_t sz, size_t ni, FILE *fp);

extern u_int64_t
 io_segsize(void *fp, int64_t n),
 lio_read(void *ptr, size_t sz, u_int64_t ni, FILE *fp),
 lio_write(void *ptr, size_t sz, u_int64_t ni, FILE *fp),
 lio_segsize(void *fp, int64_t n);

extern int64_t
 SC_set_buffer_size(int64_t sz),
 SC_get_buffer_size(void),
 SC_file_size(FILE *fp),
 SC_file_length(char *name);

extern int64_t
 lio_tell(FILE *fp),
 SC_filelen(FILE *fp);

extern char
 *SC_prompt(char *prompt, char *s, int n),
 *_SC_rl_fgets(char *s, int n),
 *SC_dgets(char *bf, int *pnc, FILE *fp),
 *SC_fgets(char *s, int n, FILE *fp),
 *io_gets(char *s, int n, FILE *fp),
 *io_pointer(void *fp),
 *lio_gets(char *s, int n, FILE *fp),
 *lio_pointer(void *fp);

extern ssize_t
 SC_read_sigsafe(int fd, void *bf, size_t n),
 SC_write_sigsafe(int fd, void *bf, size_t n);

extern size_t
 SC_fread_sigsafe(void *s, size_t bpi, size_t ni, FILE *fp),
 SC_fwrite_sigsafe(void *s, size_t bpi, size_t ni, FILE *fp);

extern PFIOErr
 SC_set_io_error_handler(PFIOErr hnd);


/* SCMMAP.C declarations */

extern FILE
 *SC_mf_bind(void *mf),
 *SC_mf_open(char *name, char *mode),
 *SC_lmf_bind(void *mf),
 *SC_lmf_open(char *name, char *mode);

extern void
 SC_mf_set_hooks(void),
 SC_lmf_set_hooks(void),
 SC_set_io_hooks(int whch);


#ifdef HAVE_MMAP

/* SCMMAPS.C declarations */

extern SC_mapped_file
 *SC_mf_make(char *name, int prot, int shar, int perm, int extend,
	     void (*setup)(SC_mapped_file *mf));

extern void
 SC_mf_set_prop(SC_mapped_file *mf, int fd, void *p, int64_t len),
 SC_mf_free(SC_mapped_file *mf),
 SC_mf_set_size(int64_t mnsz, int64_t extsz);

extern int64_t
 SC_mf_traverse(PFMMTrav fnc, void *a, int ext, int64_t nb, FILE *fp),
 SC_mf_read_striped(FILE *fp, int64_t off, char *type,
		    int ni, int stride, int nv, void **vrs),
 SC_mf_length(FILE *fp);

extern char
 *SC_mf_name(FILE *fp);

extern SC_file_block
 *SC_mf_insert(FILE *fp, SC_file_block *nbl, int64_t off),
 *SC_mf_delete(FILE *fp, int64_t off, int64_t nb);

extern FILE
 *SC_mf_copy(char *name, FILE *fp, int bckup);

#endif


/* SCTAR.C declarations */

extern fcdes
 *SC_scan_tarfile(char *tarf);

extern int
 _SC_is_tarfile(FILE *fp);


/* SCTERM.C declarations */

extern int
 SC_set_fd_attr(int fd, int i, int state),
 SC_set_io_attr(int fd, int attr, int state),
 SC_set_io_attrs(int fd, ...),
 SC_is_background_process(int pid),
 SC_set_term_state(void *t, int trmfd),
 SC_set_raw_state(int fd, int trap),
 SC_set_cooked_state(int fd, int trap),
 SC_get_term_size(int *pcr, int *pcc, int *ppr, int *ppc),
 SC_set_term_size(int fd, int nr, int nc, int pw, int ph),
 SC_io_suspend(int fl),
 SC_mpi_printf(FILE *fp, char *fmt, ...),
 SC_mpi_fputs(const char *s, FILE *fp),
 SC_mpi_ftn_snprintf(char *bf, int nc, char *fmt, ...),
 SC_mpi_io_suppress(int on),
 SC_mpi_suppress(int st);

extern void
 *SC_get_term_state(int fd, int size),
 SC_disconnect_tty(void);

extern void
 SC_print_term_state(FILE *fp, int fd);


/* SCUDL.C declarations */

extern int
 _SC_udlp(char *s);

extern char
 *_SC_udl_container(char *s, int trunc);

extern void
 _SC_rel_udl(SC_udl *pu);

extern SC_udl
 *_SC_parse_udl(char *s);


#ifdef __cplusplus
}
#endif

#endif

