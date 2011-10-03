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
 *             - this results in the variables being declared in the compilation
 *             - unit.
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

/* memory manager constants */

#define SC_MEM_MFA        ((short) 0xF0E1)
#define	SC_MEM_MFB        ((short) 0xD2C3)
#define SC_MEM_ID         0xCAFEFEED
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

#define SCORE_BLOCK_P(_d)                                                   \
    (((_d)->id) == SC_MEM_ID)

#define FREE_SCORE_BLOCK_P(_d)                                              \
    (((_d)->ref_count == SC_MEM_MFA) && ((_d)->type == SC_MEM_MFB))

#define FF_NAME(desc)                                                       \
    ((SC_FF_NAME_MASK & (desc)->id) != 0)

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

typedef struct s_mem_descriptor mem_descriptor;
typedef struct s_major_block_des major_block_des;
typedef struct s_SC_heap_des SC_heap_des;
typedef struct s_SC_mem_opt SC_mem_opt;
typedef struct s_SC_mem_hst SC_mem_hst;
typedef struct s_SC_mem_state SC_mem_state;
typedef struct s_SC_memfncs SC_memfncs;

typedef void *(*PFMalloc)(size_t size);
typedef void (*PFFree)(void *p);
typedef void *(*PFRealloc)(void *ptr, size_t size);

struct s_SC_memfncs
   {int sys;
    PFMalloc malloc;
    PFFree free;
    PFRealloc realloc;};

/* use the mem_header struct to force alignment to that of a double
 * this solves all alignment problems (especially for RISC chips)
 */

struct s_mem_descriptor
   {mem_header *prev;
    mem_header *next;
    SC_heap_des *heap;
    long length;
    long id;
    char initialized;
    short ref_count;
    short type;
    SC_srcloc where;};

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
    uint64_t sp_alloc;
    uint64_t sp_free;
    uint64_t sp_diff;
    uint64_t sp_max;
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

struct s_SC_mem_state
   {int trap_sig;
    void *trap_ptr;
    int mem_align_expt;
    int mem_align_size;
    int mem_align_pad;
    long block_size;
    long n_bins;
    long *bins;};

typedef int (*PFMemMap)(SC_heap_des *ph, mem_descriptor *md,
			mem_kind wh, void *a, long i, long j);

# ifndef MM_CONFIG

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

#ifdef SC_DEFINE

SC_mem_state
 _SC_ms = {-1, NULL, 0, 0, 0, 0L, 0L, NULL};

SC_memfncs
  _SC_mf = { FALSE, NULL, NULL, NULL };

#else

extern SC_mem_state
 _SC_ms;

extern SC_memfncs
  _SC_mf;

#endif

extern SC_thread_lock
 SC_mm_lock,
 SC_mc_lock;

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
 *SC_alloc_n(long ni, long bpi, ...),
 *SC_realloc_n(void *p, long ni, long bpi, ...);

extern int
 SC_free_n(void *p, ...),
 SC_is_score_space(void *p, mem_header **psp, mem_descriptor **pds),
 SC_is_free_space(void *p, mem_header **psp, mem_descriptor **pds),
 SC_is_active_space(void *p, mem_header **psp, mem_descriptor **pds),
 SC_mem_over_mark(int n);


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

#endif
