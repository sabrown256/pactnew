/*
 * SCOPE_MEM.H - header defining private MEM scope
 *             - routines and variables defining the API are in SCORE.H
 *             - routines and variables internal to the SCORE memory manager
 *             - are defined here
 *             - NOTE: this is the model for all of PACT from now on
 *             - these internal scope headers will all have names of the
 *             - form scope_<pkg>.h and will use the SC_DECLARE_VAR and
 *             - SC_DECLARE_VARU macros to define/declare variables.  These
 *             - macros facillitate specifying variables only once in the
 *             - entire source.  The presence of the SC_DEFINE constant
 *             - controls whether the inclusion results in variable extern
 *             - declaration or variable definition and initialization. One
 *             - file of using the scope MUST include the scope with:
 *             -
 *             -   #define SC_DEFINE
 *             -   #include "scope_mem.h"
 *             -
 *             - this results in the variables being defined.
 *             - all other files using the scope MUST include the scope with:
 *             -
 *             -   #include "scope_mem.h"
 *             -
 *             - this results in the variables being declared in the
 *             - compilation unit.
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_MEM
# define PCK_SCOPE_MEM

#ifndef MM_CONFIG

# include "cpyright.h"
# include "score.h"

# if defined(LINUX) || defined(CYGWIN)
#  include <malloc.h>
# endif

#endif

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/* fundamental platform memory constants */

#define BITS_PER_BYTE       8

#if defined(SGI) || defined(OSF) || defined(MACOSX)
# define SC_MEM_ALIGN_SIZE   sizeof(double);
#else
# define SC_MEM_ALIGN_SIZE   sizeof(long double);
#endif

#ifdef USE_FULL_MM
#define _SC_DEF_MM_METHOD_                                                  \
    {_SC_nalloc_wl, _SC_alloc_wl, _SC_realloc_wl,                           \
     _SC_free_wl, _SC_arrlen_wl,                                            \
     _SC_alloc_nl, _SC_realloc_nl, _SC_free_nl}
#else
#define _SC_DEF_MM_METHOD_                                                  \
    {_SC_nalloc_ws, _SC_alloc_ws, _SC_realloc_ws,                           \
     _SC_free_ws, _SC_arrlen_ws,                                            \
     _SC_alloc_ns, _SC_realloc_ns, _SC_free_ns}
#endif

/* memory manager constants */

#define SC_MEM_ID         0xCAFEFEED
#define UNCOLLECT         SHRT_MAX

/* used in ref_count member of mem_inf */
#define SC_MEM_MFA        ((short) 0x0FE1)
/* used in type member of mem_inf */
#define	SC_MEM_MFB        ((short) 0x0DC3)

#define SC_BLOCK_ID_MASK  0xEFFFFF00
#define SC_FF_NAME_MASK   0x10000000
#define HEAPS_UNIT_DELTA  1000L
#define BLOCKS_UNIT_DELTA 10L

#define EXTRA_WORD_SIZE   sizeof(long)
#define NBITS             (BITS_PER_BYTE*EXTRA_WORD_SIZE - 4)

#define _SC_ALLOC   (*_SC.alloc)
#define _SC_REALLOC (*_SC.realloc)
#define _SC_FREE    (*_SC.free)

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define SC_mm_lock   _SC_ms.mm_lock
#define SC_mc_lock   _SC_ms.mc_lock

#define SC_SET_BLOCK_ID(_d, _v)   ((_d)->desc.info.idt = _v)
#define SC_GET_BLOCK_ID(_d)       ((_d)->desc.info.idt)

#define SCORE_BLOCK_P(_d)                                                   \
    (SC_GET_BLOCK_ID(_d) == SC_MEM_ID)

#define FREE_SCORE_BLOCK_P(_d)                                              \
    (((_d)->desc.info.ref_count == SC_MEM_MFA) &&                           \
     ((_d)->desc.info.type == SC_MEM_MFB))

#define FF_NAME(_d)                                                         \
    ((SC_FF_NAME_MASK & SC_GET_BLOCK_ID(_d)) != 0)

#define _SC_ALLOC_N     SC_gs.mm.alloc_n
#define _SC_REALLOC_N   SC_gs.mm.realloc_n
#define _SC_FREE_N      SC_gs.mm.free_n

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

enum e_SC_mem_attribute
   {SC_MEM_ATTR_PERMANENT = 1,    /* memory block is UNCOLLECT'able */
    SC_MEM_ATTR_NO_ACCOUNT,       /* memory block not included in totals */
    SC_MEM_ATTR_ZERO_SPACE,       /* set zero_space behavior */
    SC_MEM_ATTR_TYPE,             /* data type index */
    SC_MEM_ATTR_FUNC,             /* function containing call */
    SC_MEM_ATTR_FILE,             /* file containing function */
    SC_MEM_ATTR_LINE};            /* line number in file */

typedef enum e_SC_mem_attribute SC_mem_attribute;

enum e_SC_mem_tag
   {SC_MEM_ALLOC = 1,
    SC_MEM_REALLOC,
    SC_MEM_FREE};

typedef enum e_SC_mem_tag SC_mem_tag;

enum e_mem_kind
   {MEM_BLOCK_ACTIVE,
    MEM_BLOCK_FREE,
    MEM_BLOCK_REG};

typedef enum e_mem_kind mem_kind;

typedef union u_mem_header mem_header;
typedef union u_mem_infu mem_infu;

typedef struct s_mem_inf mem_inf;
typedef struct s_mem_descriptor mem_descriptor;
typedef struct s_major_block_des major_block_des;
typedef struct s_SC_heap_des SC_heap_des;
typedef struct s_SC_mem_opt SC_mem_opt;
typedef struct s_SC_mem_hst SC_mem_hst;
typedef struct s_SC_scope_mem SC_scope_mem;
typedef struct s_SC_memfncs SC_memfncs;

typedef void *(*PFMalloc)(size_t size);
typedef void (*PFFree)(void *p);
typedef void *(*PFRealloc)(void *ptr, size_t size);

struct s_SC_memfncs
   {int sys;
    PFMalloc malloc;
    PFFree free;
    PFRealloc realloc;};

/* define the mem_inf so that a union with an SC_srcloc
 * will give the tightest memory packing of the overall
 * mem_descriptor - saves 20% with GLIBC
 * NOTE: must be careful since several compilers do not
 * support signed bit fields (e.g. PGCC 11.1)
 */

struct s_mem_inf
   {const char *pfile;
    const char *pfunc;
    unsigned int line;
    unsigned int initialized : 1;
    int type : 15;
    short ref_count;
    long idt;};

union u_mem_infu
   {mem_inf info;
    SC_srcloc where;};

/* use the mem_header struct to force alignment to that of a double
 * this solves all alignment problems (especially for RISC chips)
 */

struct s_mem_descriptor
   {mem_header *prev;
    mem_header *next;
    SC_heap_des *heap;
    mem_infu desc;
    long length;};

union u_mem_header
   {mem_descriptor block;
    double align[N_DOUBLES_MD];};

struct s_major_block_des
   {char *block;            /* pointer to major block space */
    long index;             /* bin index of the major block */
    long nunits;            /* number of blocks (units) in the major block */
    long size;};            /* byte size of unit */

struct s_SC_heap_des
   {int init;
    int tid;
    int zero_space;         /* per thread version of _SC.zero_space */
    mem_descriptor **free_list;
    mem_header *latest_block;
    major_block_des *major_block_list;
    long n_major_blocks;
    long nx_major_blocks;
    long n_mem_blocks;
    long nx_mem_blocks;
    unsigned long hdr_size;
    unsigned long hdr_size_max;
    u_int64_t sp_alloc;
    u_int64_t sp_free;
    u_int64_t sp_diff;
    u_int64_t sp_max;
    long ih;                /* current history index */
    long nh;                /* number of history entries */
    SC_mem_hst *ring;};     /* ring of history entries */

struct s_SC_mem_opt
   {int perm;             /* if 1 make block UNCOLLECT */
    int na;               /* if 1 do not add block to allocated count */
    int zsp;              /* zero space flag */
    int typ;              /* set the type field to this type index */
    SC_srcloc where;};   /* info supplied free by C99 */

struct s_SC_mem_hst
   {int action;
    int ncall;
    long nb;
    double time;
    char *name;
    mem_descriptor *space;};

struct s_SC_scope_mem
   {int trap_sig;
    void *trap_ptr;
    int mem_align_expt;
    int mem_align_size;
    int mem_align_pad;
    long block_size;
    long n_bins;
    long *bins;
    SC_thread_lock mm_lock;
    SC_thread_lock mc_lock;};


typedef int (*PFMemMap)(SC_heap_des *ph, mem_descriptor *md,
			mem_kind wh, void *a, long i, long j);

#ifdef __cplusplus
extern "C" {
#endif

# ifndef MM_CONFIG

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SC_scope_mem
 _SC_ms;

extern SC_memfncs
  _SC_mf;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/* SCMEMC.C declarations */

extern SC_heap_des
 *_SC_tid_mm(void);

extern void
 _SC_mem_stats_acc(SC_heap_des *ph, int64_t a, int64_t f),
 _SC_assign_block(SC_heap_des *ph, mem_header *space,
		  long nb, char *func, char *file, int line),
 _SC_deassign_block(SC_heap_des *ph, mem_descriptor *desc,
		    void *addr),
 _SC_unassign_block(SC_heap_des *ph, mem_header *space),
 _SC_init_heap(SC_heap_des *ph, int id),
 _SC_mem_exhausted(int cnd),
 *SC_alloc_n(long ni, long bpi, ...),
 *SC_realloc_n(void *p, long ni, long bpi, ...);

extern int
 SC_free_n(void *p, ...),
 SC_is_score_space(void *p, mem_header **psp, mem_descriptor **pds),
 SC_is_free_space(void *p, mem_header **psp, mem_descriptor **pds),
 SC_is_active_space(void *p, mem_header **psp, mem_descriptor **pds),
 SC_mem_over_mark(int n),
 SC_mem_trim(size_t pad);


/* SCMEMCL.C declarations */

extern SC_mem_fnc
 SC_use_full_mm(void);

extern void
 *_SC_alloc_nl(long ni, long bpi, void *a),
 *_SC_realloc_nl(void *p, long ni, long bpi, void *a),
 *_SC_nalloc_wl(long ni, long bpi, int memfl, int zsp,
	        const char *fnc, const char *file, int line),
 *_SC_alloc_wl(long ni, long bpi, char *name, int memfl, int zsp),
 *_SC_realloc_wl(void *p, long ni, long bpi, int memfl, int zsp);

extern int
 _SC_free_nl(void *p, void *a),
 _SC_free_wl(void *p, int zsp);

extern int64_t
 _SC_arrlen_wl(void *p);


/* SCMEMCS.C declarations */

extern SC_mem_fnc
 SC_use_reduced_mm(void);

extern void
 *_SC_alloc_ns(long ni, long bpi, void *a),
 *_SC_realloc_ns(void *p, long ni, long bpi, void *a),
 *_SC_nalloc_ws(long ni, long bpi, int memfl, int zsp,
	        const char *fnc, const char *file, int line),
 *_SC_alloc_ws(long ni, long bpi, char *name, int memfl, int zsp),
 *_SC_realloc_ws(void *p, long ni, long bpi, int memfl, int zsp);

extern int
 _SC_free_ns(void *p, void *a),
 _SC_free_ws(void *p, int zsp);

extern int64_t
 _SC_arrlen_ws(void *p);


/* SCMEMDA.C declarations */

extern char
 *_SC_block_name(mem_descriptor *desc);

extern void
 _SC_print_block_info(FILE *fp, SC_heap_des *ph, void *ptr, int flag);


/* SCMEMDB.C declarations */

extern int
 _SC_n_blocks(SC_heap_des *ph, int flag);


/* SCMEMDC.C declarations */

extern void
 SC_mem_map_f(int typ, PFMemMap f, void *a);


/* SCMEMG.C declarations */

extern void
 *_SC_alloc_guarded(size_t nb),
 *_SC_realloc_guarded(void *p, size_t nb),
 _SC_free_guarded(void *p);


/* SCMEMT.C declarations */

extern SC_heap_des
 *_SC_get_heap(int id);

# endif

#ifdef __cplusplus
}
#endif

#endif
