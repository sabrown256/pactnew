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
#define SC_FTN_NAME_MASK  0x10000000
#define HEAPS_UNIT_DELTA  1000L
#define BLOCKS_UNIT_DELTA 10L

#define EXTRA_WORD_SIZE   sizeof(long)
#define NBITS             (BITS_PER_BYTE*EXTRA_WORD_SIZE - 4)

#define _SC_ALLOC   (*_SC_alloc_hook)
#define _SC_REALLOC (*_SC_realloc_hook)
#define _SC_FREE    (*_SC_free_hook)

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define SC_BIN_INDEX(_n)  _SC_bin_index(_n)
#define SC_BIN_SIZE(_n)   (((_n) >= _SC_n_bins) ? -1 : _SC_mm_bins[_n])
#define SC_BIN_UNITS(_n)  (((_n) < _SC_block_size) ? _SC_block_size/(_n) : 1)

#define SC_BIN_THRESHOLD()   _SC_mm_bins[_SC_n_bins-1]

#define ASSIGN_ID(_d)                                                       \
    (_d)->id = SC_MEM_ID

#define SCORE_BLOCK_P(_d)                                                   \
    (((_d)->id) == SC_MEM_ID)

#define SET_HEAP(_d, _p)                                                    \
    (_d)->heap = _p

#define GET_HEAP(_d)                                                        \
    ((_d)->heap)

#define FREE_SCORE_BLOCK_P(_d)                                              \
    (((_d)->ref_count == SC_MEM_MFA) && ((_d)->type == SC_MEM_MFB))

#define FTN_NAME(desc)                                                      \
    ((SC_FTN_NAME_MASK & (desc)->id) == 0)

#define BLOCK_LENGTH(desc)                                                  \
    (desc)->length

#define REF_COUNT(desc)                                                     \
    ((desc)->ref_count)

#define BLOCK_TYPE(desc)                                                    \
    ((desc)->type)

#define BLOCK_NAME(desc)                                                    \
    ((desc)->name)

#define SAVE_LINKS(desc)                                                    \
   {prev = desc->prev;                                                      \
    next = desc->next;                                                      \
    if (space == SC_LATEST_BLOCK(ph))                                       \
       SC_LATEST_BLOCK(ph) = next;}

/*--------------------------------------------------------------------------*/

#define SC_HEAP_TID(x)           (x)->tid
#define SC_HEAP_INIT(x)          (x)->init
#define SC_FREE_LIST(x)          (x)->free_list
#define SC_MAJOR_BLOCK_LIST(x)   (x)->major_block_list
#define SC_N_MAJOR_BLOCKS(x)     (x)->n_major_blocks
#define SC_NX_MAJOR_BLOCKS(x)    (x)->nx_major_blocks
#define SC_MAX_MEM_BLOCKS(x)     (x)->max_mem_blocks
#define SC_N_MEM_BLOCKS(x)       (x)->n_mem_blocks
#define SC_HDR_SIZE_MAX(x)       (x)->size_max
#define SC_HDR_SIZE(x)           (x)->size
#define SC_LATEST_BLOCK(x)       (x)->latest_block
#define SC_MEM_TRACE_PTR(x)      (x)->mem_trace_ptr
#define SC_SP_ALLOC(x)           (x)->sp_alloc
#define SC_SP_FREE(x)            (x)->sp_free
#define SC_SP_DIFF(x)            (x)->sp_diff
#define SC_SP_MAX(x)             (x)->sp_max

#define SC_HEAP_READY(x)         ((x) != NULL)

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef union u_mem_header mem_header;
typedef struct s_mem_descriptor mem_descriptor;
typedef struct s_major_block_des major_block_des;
typedef struct s_SC_heap_des SC_heap_des;
typedef struct s_SC_mem_opt SC_mem_opt;
typedef struct s_SC_mem_hst SC_mem_hst;

/* use the mem_header struct to force alignment to that of a double
 * this solves all alignment problems (especially for RISC chips)
 */

struct s_mem_descriptor
   {mem_header *prev;
    mem_header *next;
    char *name;
    SC_heap_des *heap;
    long length;
    long id;
    char initialized;
    short ref_count;
    short type;};

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
    mem_descriptor **free_list;
    mem_header *latest_block;
    major_block_des *major_block_list;
    long n_major_blocks;
    long nx_major_blocks;
    long max_mem_blocks;
    long n_mem_blocks;
    unsigned long size_max;
    unsigned long size;
    uint64_t sp_alloc;
    uint64_t sp_free;
    uint64_t sp_diff;
    uint64_t sp_max;
    long ih;                /* current history index */
    long nh;                /* number of history entries */
    SC_mem_hst *ring;       /* ring of history entries */
    void (*history)(int act, mem_descriptor *space);};

struct s_SC_mem_opt
   {int na;               /* if 1 do not add block to allocated count */
    int zsp;              /* zero space flag */
    int typ;};            /* set the type field to this type index */

struct s_SC_mem_hst
   {int action;
    int ncall;
    long nb;
    double time;
    char *name;
    mem_descriptor *space;};

enum e_SC_mem_tag
   {SC_MEM_ALLOC = 1,
    SC_MEM_REALLOC,
    SC_MEM_FREE};

typedef enum e_SC_mem_tag SC_mem_tag;

# ifndef MM_CONFIG

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

SC_DECLARE_VAR(long, _SC_block_size, 0L);
SC_DECLARE_VAR(long, _SC_n_bins, 0L);
SC_DECLARE_VAR(long, *_SC_mm_bins, NULL);

SC_DECLARE_VAR(int, _SC_mem_align_expt, 0);
SC_DECLARE_VAR(int, _SC_mem_align_size, 0);
SC_DECLARE_VAR(int, _SC_mem_align_pad, 0);
SC_DECLARE_VAR(int, _SC_zero_space, 0);
SC_DECLARE_VAR(int, SC_mm_debug, FALSE);
SC_DECLARE_VAR(int, _SC_trap_sig, -1);

SC_DECLARE_VAR(void, (*_SC_free_hook)(void *p), free);
SC_DECLARE_VAR(PFMalloc,  _SC_alloc_hook,   (PFMalloc) malloc);
SC_DECLARE_VAR(PFRealloc, _SC_realloc_hook, (PFRealloc) realloc);

SC_DECLARE_VAR(void,
	       (*_SC_mem_hst_hook)(int act, mem_descriptor *space),
	       NULL);


SC_DECLARE_VAR(void, *_SC_trap_ptr, NULL);

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
 _SC_init_heap(SC_heap_des *ph, int id);

extern int
 SC_is_score_space(void *p, mem_header **psp, mem_descriptor **pds);

extern long
 _SC_bin_index(long n);


/* SCMEMD.C declarations */

extern int
 _SC_name_ok(char *name, int flag);

extern void
 _SC_print_block_info(FILE *fp, SC_heap_des *ph, void *ptr, int flag);


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
