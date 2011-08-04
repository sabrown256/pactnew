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

#define SC_BIN_INDEX(_n)  _SC_bin_index(_n)
#define SC_BIN_SIZE(_n)   (((_n) >= _SC_ms.n_bins) ? -1 : _SC_ms.bins[_n])
#define SC_BIN_UNITS(_n)  (((_n) < _SC_ms.block_size) ? _SC_ms.block_size/(_n) : 1)

#define SC_BIN_THRESHOLD()   _SC_ms.bins[_SC_ms.n_bins-1]

#define SCORE_BLOCK_P(_d)                                                   \
    (((_d)->id) == SC_MEM_ID)

#define FREE_SCORE_BLOCK_P(_d)                                              \
    (((_d)->ref_count == SC_MEM_MFA) && ((_d)->type == SC_MEM_MFB))

#define FF_NAME(desc)                                                      \
    ((SC_FF_NAME_MASK & (desc)->id) != 0)

#define SAVE_LINKS(desc)                                                    \
   {prev = desc->prev;                                                      \
    next = desc->next;                                                      \
    if (space == ph->latest_block)                                          \
       ph->latest_block = next;}

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


typedef union u_mem_header mem_header;
typedef struct s_mem_descriptor mem_descriptor;
typedef struct s_major_block_des major_block_des;
typedef struct s_SC_heap_des SC_heap_des;
typedef struct s_SC_mem_opt SC_mem_opt;
typedef struct s_SC_mem_hst SC_mem_hst;
typedef struct s_SC_mem_state SC_mem_state;

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
    SC_csrcloc where;};

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
    SC_csrcloc where;};   /* info supplied free by C99 */

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

# ifndef MM_CONFIG

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

#ifdef SC_DEFINE

SC_mem_state
 _SC_ms = {-1, NULL, 0, 0, 0, 0L, 0L, NULL};

#else

extern SC_mem_state
 _SC_ms;

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
 *_SC_alloc_n(long ni, long bpi, void *a),
 *SC_alloc_n(long ni, long bpi, ...),
 *_SC_realloc_n(void *p, long nitems, long bpi, void *a),
 *SC_realloc_n(void *p, long nitems, long bpi, ...);

extern void
 _SC_init_heap(SC_heap_des *ph, int id);

extern int
 _SC_free_n(void *p, void *a),
 SC_free_n(void *p, ...),
 SC_is_score_space(void *p, mem_header **psp, mem_descriptor **pds);

extern long
 _SC_bin_index(long n);


/* SCMEMDA.C declarations */

extern void
 *_SC_nalloc_w(long nitems, long bpi, int memfl, int zsp,
	       const char *fnc, const char *file, int line),
 *_SC_alloc_w(long nitems, long bpi, char *name, int memfl, int zsp),
 *_SC_realloc_w(void *p, long nitems, long bpi, int memfl, int zsp);

extern int
 _SC_free_w(void *p, int zsp);

extern char
 *_SC_block_name(mem_descriptor *desc);

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
