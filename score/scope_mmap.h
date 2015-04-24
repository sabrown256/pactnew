/*
 * SCOPE_MMAP.H - define MMAP scope
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_MMAP

# define PCK_SCOPE_MMAP

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define CURRENT_MAP(_m)                                                      \
    ((char *) ((SC_mapped_file *) (_m))->p + ((SC_mapped_file *) (_m))->lpos)

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef void *(*PFMMap)(void *addr, size_t len,
			int prot, int flags, int fd, int64_t offs);

typedef struct s_SC_io_buffer SC_io_buffer;
typedef struct s_SC_scope_mmap SC_scope_mmap;

struct s_SC_io_buffer
   {SC_mapped_file *mf;
    int position;
    int item_size;
    int stride;
    int nv;
    void **vrs;};

struct s_SC_scope_mmap
   {int64_t initial_length;
    int64_t max_extend;};

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SC_scope_mmap
 _SC_mmf;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SCMMAPS.C declarations */

extern FILE
 *_SC_mf_open(const char *name, const char *mode,
	      void (*setup)(SC_mapped_file *mf));

extern char
 *_SC_mf_gets(char *s, int n, FILE *fp),
 *_SC_mf_data(FILE *fp);

extern int
 _SC_mf_getc(FILE *fp),
 _SC_mf_ungetc(int c, FILE *fp),
 _SC_mf_puts(const char *s, FILE *fp),
 _SC_mf_printf(FILE *fp, const char *fmt, va_list a),
 _SC_mf_flush(FILE *fp),
 _SC_mf_setvbuf(FILE *fp, char *bf, int type, size_t sz),
 _SC_mf_eof(FILE *fp),
 _SC_mf_close(FILE *fp),
 _SC_mf_core_seek(FILE *fp, int64_t offset, int whence);

extern int64_t
 _SC_mf_core_tell(FILE *fp);

extern u_int64_t
 _SC_mf_core_read(void *bf, size_t sz, u_int64_t ni, FILE *fp),
 _SC_mf_core_write(const void *bf, size_t sz, u_int64_t ni, FILE *fp),
 _SC_mf_segment_size(FILE *fp, int64_t nsz);


/* SCMMAP.C declarations */

void
 _SC_mf_setup(SC_mapped_file *mf);


/* SCLMMAP.C declarations */

void
 _SC_lmf_setup(SC_mapped_file *mf);

#endif

