/*
 * PDB_INT.H - header file supporting PDBLIB internals
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PDB_INT

#include "cpyright.h"

#define PCK_PDB_INT

#include "pml_int.h"
#include "pdb.h"
#include "scope_mpi.h"

/*--------------------------------------------------------------------------*/

/*                                 MACROS                                   */

/*--------------------------------------------------------------------------*/

#define N_CASTS_INCR 30

#define PD_CKSUM_LEN     33

#define CRAY_BYTES_WORD   8

#define PD_TYPE_PLACEHOLDER     "typename#placeholder"

#define _PD_entry_get_number(_e, _n)                                         \
   (((_e) != NULL) ? _PD_block_get_number((_e)->blocks, _n) : 0)

#define _PD_entry_set_number(_e, _n, _v)                                     \
   if ((_e) != NULL) _PD_block_set_number((_e)->blocks, _n, _v)

#define _PD_entry_get_address(_e, _n)                                        \
   (((_e) != NULL) ? _PD_block_get_address((_e)->blocks, _n) : 0)

#define _PD_entry_set_address(_e, _n, _v)                                    \
   if ((_e) != NULL) _PD_block_set_address((_e)->blocks, _n, _v)

#define _PD_sys_read(_f, _e, _t, _p)                                         \
   (((_f)->sys->read != NULL) ?                                              \
    (*(_f)->sys->read)(_f, _e, _t, _p) :                                     \
    _PD_rd_syment(_f, _e, _t, _p))

#define _PD_sys_write(_f, _p, _n, _ti, _to)                                  \
   (((_f)->sys->write != NULL) ?                                             \
    (*(_f)->sys->write)(_f, _p, _n, _ti, _to) :                              \
    _PD_wr_syment(_f, _p, _n, _ti, _to))

#define _PD_SETUP_PSEUDO_FILE(fp)   (*PD_par_fnc.setup_pseudo_file)(fp)
#define _PD_SETUP_MP_FILE(f, comm)  (*PD_par_fnc.setup_mp_file)(f, comm)
#define _PD_EXTEND_FILE(f, nb)      (*PD_par_fnc.extend_file)(f, nb)
#define _PD_FLUSH_FILE(f)           (*PD_par_fnc.flush_file)(f)
#define _PD_GET_FILE_SIZE(fp)       (*PD_par_fnc.get_file_size)(fp)
#define _PD_GET_FILE_STREAM(f)      (*PD_par_fnc.get_file_stream)(f)
#define _PD_GET_FILE_PTR(fp)        (*PD_par_fnc.get_file_ptr)(fp)
#define _PD_MARK_AS_FLUSHED(f, w)   (*PD_par_fnc.mark_as_flushed)(f, w)
#define _PD_SERIAL_FLUSH(fp, tid)   (*PD_par_fnc.serial_flush)(fp, tid)
#define _PD_ADD_FILE(f, addr)       (*PD_par_fnc.add_file)(f, addr)
#define _PD_REMOVE_FILE(f)          (*PD_par_fnc.remove_file)(f)
#define _PD_SET_ADDRESS(f, addr)    (*PD_par_fnc.set_address)(f, addr)
#define _PD_GETSPACE(f, nb, rf, cf) (*PD_par_fnc.getspace)(f, nb, rf, cf)
#define _PD_SET_EOD(f, addr, n)     (*PD_par_fnc.set_eod)(f, addr, n)
#define _PD_IS_DP_INIT              (*PD_par_fnc.is_dp_init)()
#define _PD_IS_SMP_INIT             (*PD_par_fnc.is_smp_init)()
#define _PD_IS_NULL_FP(fp)          (*PD_par_fnc.is_null_fp)(fp)
#define _PD_IS_SEQUENTIAL           (*PD_par_fnc.is_sequential)()
#define _PD_IS_MASTER(f)            (*PD_par_fnc.is_master)(f)
#define _PD_NEXT_ADDRESS(f, t, n, v, sf, tf, cf) (*PD_par_fnc.next_address)(f, t, n, v, sf, tf, cf)

#define GET_PFILE(_f)       ((PD_Pfile *) (_f))
#define _PD_TEXT_OUT(_f)    ((_f)->std->fx[2].order == TEXT_ORDER)

#define IS_PDBFILE(_f)                                                       \
    (((_f)->type == NULL) || (strcmp((_f)->type, "PDBfile") == 0))

#define PD_REGISTER(_t, _f, _h, _m, _o, _c, _w, _r)                          \
    _PD_register(_t, _f, _h,                                                 \
		 (PFBinCreate) _m, (PFBinOpen) _o, (PFBinClose) _c,          \
		 (PFBinWrite) _w, (PFBinRead) _r)

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

typedef int (*PFBinType)(char *type);
typedef PDBfile *(*PFBinCreate)(tr_layer *tr, SC_udl *pu, char *name, void *a);
typedef PDBfile *(*PFBinOpen)(tr_layer *tr, SC_udl *pu, char *name,
			      char *mode, void *a);
typedef int (*PFBinClose)(PDBfile *file);
typedef syment *(*PFBinWrite)(PDBfile *file, char *path,
			      char *inty, char *outty,
			      void *vr, dimdes *dims,
			      int appnd, int *pnew);
typedef int (*PFBinRead)(PDBfile *file, char *path, char *ty,
			 syment *ep, void *vr, int nd, long *ind);

typedef struct s_PD_state PD_state;
typedef struct s_PD_address PD_address;
typedef struct s_PD_Pfile PD_Pfile;
typedef struct s_PD_printdes PD_printdes;

struct s_PD_printdes
   {int nn;
    int mjr;
    int def_off;
    long offset;
    char *prefix;
    char *before;
    char *after;
    char *nodename;
    dimdes *dims;
    FILE *fp;};

struct s_PD_address
   {int indx;                  /* indx for /&ptrs - possible future use */
    BIGINT addr;                /* disk address of start of data or itag */
    BIGINT reta;                /* disk address of end of data */
    syment *entry;             /* symbol table entry */
    char *ptr;                 /* memory address of data */
    int written;};             /* flag--1 if data written, 0 if not */

 
/* NOTE: see comment in scope_io.h concerning file_io_desc
 * and the stream member
 */

struct s_PD_Pfile
   {SC_address *f_addr;        /* current address (file) (per thread) */
    SC_address *bf;            /* buffer (per thread) */
    void *stream;              /* the stream pointer */
    long *bf_len;              /* length of buffer (per thread) */
    SC_communicator comm;
    int mp_id;                 /* file id received from MP master process */
    int id;};                  /* id (for smp file manager) */

struct s_PD_state
   {

/* initializes to non-zero values */

/* PDSER.C */
    int ita;

/* PDPTR.C */
    long ninc;

/* PDPARMP.C */
    int mp_rank;
    int mp_size;
    int mp_initialized;
    int mp_master_proc;
    int mp_collective;

/* PDCOMM.C */
    SC_communicator comm_null;
    SC_communicator comm_world;
    SC_communicator comm_self;

/* initializes to 0 bytes */

/* PDPAR.C */
    int nfilesx;

/* PDPARMP.C */
    int nterminated;                    /* num processes who sent terminate msg */
    SC_communicator mp_comm;

/* PDSZOF.C */
    long szl;

/* PDBDIR.C */
    int dir_num;

    BIGINT maxfsize;

/* PDFMT.C */
    char id_token[MAXLINE];

   };

enum e_PD_block_type
   {PD_BLOCK_UNINIT     = -2,
    PD_BLOCK_UNVERIFIED = -1,
    PD_BLOCK_INVALID    =  1,
    PD_BLOCK_VALID      =  2,
    PD_BLOCK_CORRUPT    =  3};

typedef enum e_PD_block_type PD_block_type;

enum e_PD_data_location
   {LOC_OTHER,
    LOC_HERE,
    LOC_BLOCK};

typedef enum e_PD_data_location PD_data_location;


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            PDBLIB VARIABLES                              */

/*--------------------------------------------------------------------------*/

extern PD_state
 _PD;

extern char
 *SYMENT_P_S,
 *SYMENT_S;

extern SC_communicator
 PD_COMM_NULL,
 PD_COMM_WORLD,
 PD_COMM_SELF;

/*--------------------------------------------------------------------------*/

/*                            PDBLIB FUNCTIONS                              */

/*--------------------------------------------------------------------------*/


/* PDB.C declarations */

extern syment
 *_PD_defent(PDBfile *file, char *name, char *outtype,
	     long number, dimdes *dims);

extern void
 _PD_sign_extend(char *out, long nitems,
		 int nbo, int nbti, int *ord),
 _PD_ones_complement(char *out, long nitems, int nbo, int *order),
 _PD_cast_insert(hasharr* chart, char* type, char* memb, char* cast);

extern int
 _PD_read(PDBfile *file, char *fullpath, char *type, syment *ep,
	  void *vr, int nd, long *ind);


/* PDBFC.C declarations */

extern FILE
 *_PD_open_container_file(char *name, char *mode);


/* PDBLK.C declarations */

extern SC_array
 *_PD_block_make(long n),
 *_PD_block_init(syment *ep, int fl),
 *_PD_block_copy_seg(SC_array *bl, long imn, long imx);

extern PD_block_type
 _PD_block_get_valid(SC_array *bl, long n);

extern void
 _PD_block_free(SC_array *bl),
 _PD_block_rel(syment *ep),
 _PD_block_copy(syment *nsym, syment *osym),
 _PD_block_switch(syment *ep, SC_array *bln),
 _PD_block_truncate(syment *ep, long ni),
 _PD_block_get_desc(BIGINT *paddr, long *pni, SC_array *bl, long n),
 _PD_block_set_desc(BIGINT addr, long ni, SC_array *bl, long n);

extern int
 _PD_block_get_csum(SC_array *bl, long n, unsigned char *dig),
 _PD_block_set_csum(SC_array *bl, long n, unsigned char *dig),
 _PD_block_set_valid(SC_array *bl, long n, PD_block_type vl),
 _PD_block_add(PDBfile *file, syment *ep, dimdes *dims, BIGINT addr),
 _PD_block_define(PDBfile *file),
 _PD_block_csum_write(PDBfile *file, syment *ep, char *name),
 _PD_block_csum_read(PDBfile *file);

extern long
 _PD_effective_addr(BIGINT *paddr, long *pnitems,
		    int bpi, SC_array *da),
 _PD_block_get_number(SC_array *bl, long n),
 _PD_block_set_number(SC_array *bl, long n, long ni),
 _PD_n_blocks(syment *ep),
 _PD_block_find(PDBfile *file, syment *ep, BIGINT addr);

extern BIGINT
 _PD_block_get_address(SC_array *bl, long n),
 _PD_block_set_address(SC_array *bl, long n, BIGINT addr);


/* PDBMM.C declarations */

extern PDBfile
 *_PD_mk_pdb(SC_udl *pu, char *name, char *md, int reg,
	     sys_layer *sys, tr_layer *tr);

extern data_standard
 *_PD_copy_standard(data_standard *std);

extern data_alignment
 *_PD_copy_alignment(data_alignment *align);

extern data_standard
 *_PD_mk_standard(PDBfile *file);

extern data_alignment
 *_PD_mk_alignment(char *vals);

extern syment
 *_PD_mk_syment(char *type, long numb, BIGINT addr, symindir *indr, dimdes *dims);

extern defstr
 *_PD_defstr_copy(defstr *dp),
 *_PD_mk_defstr(hasharr *chrt, char *type, PD_type_kind kind,
		memdes *lst, multides *tuple,
		long sz, int align, PD_byte_order ord, int conv,
		int *ordr, long *formt, int unsgned, int onescmp);

extern multides
 *_PD_make_tuple(char *type, int ni, int *ord),
 *_PD_copy_tuple(multides *tuple);

extern memdes
 *_PD_mk_descriptor(char *member, int defoff);

extern dimdes
 *_PD_mk_dimensions(long mini, long leng);

extern void
 _PD_clr_table(hasharr *tab, int (*rel)(haelem *p, void *a)),
 _PD_free_tuple(multides *tuple),
 _PD_rl_defstr(defstr *dp),
 _PD_rl_pdb(PDBfile *file),
 _PD_rl_standard(data_standard *std),
 _PD_rl_alignment(data_alignment *align),
 _PD_rl_syment(syment *ep),
 _PD_rl_descriptor(memdes *desc),
 _PD_rl_dimensions(dimdes *dims);

extern int
 _PD_ha_rl_syment(haelem *hp, void *a),
 _PD_ha_rl_defstr(haelem *hp, void *a);


/* PDCOMM.C declarations */

extern void
 _PD_par_set_comm(SC_communicator comm),
 _PD_set_null_comm(int tid),
 _PD_set_world_comm(int tid);

extern int
 _PD_initialize_comm(int v),
 _PD_is_comm_null(SC_communicator comm),
 _PD_is_comm_self(SC_communicator comm);


/* PDCONV.C declarations */

extern int
 _PD_require_conv(defstr *dpf, defstr *dph);

extern void
 _PD_convert_ascii(char *out, char *in, long nitems, int bpci, int offs),
 _PD_iconvert(char **out, char **in, long nitems,
	      long nbi, PD_byte_order ordi,
	      long nbo, PD_byte_order ordo,
	      int onescmp, int usg),
 _PD_fconvert(char **out, char **in, long nitems, int boffs,
	      long *infor, int *inord, long *outfor, int *outord,
	      PD_byte_order l_order, int l_bytes, int onescmp);


/* PDCSUM.C declarations */

extern void
 _PD_md5_checksum(PDBfile* file, unsigned char digest[PD_CKSUM_LEN]);

extern int
 _PD_csum_close(PDBfile *file),
 _PD_csum_var_read(PDBfile *file, char *fullpath, char *type,
		   syment *ep, void *vr),
 _PD_csum_var_write(PDBfile *file, char *name, syment *ef),
 _PD_csum_block_read(PDBfile *file, char *name, syment *ep, long n),
 _PD_csum_block_write(PDBfile *file, syment *ep, long n),
 _PD_csum_reserve(PDBfile *file),
 _PD_csum_file_write(PDBfile *file);

extern BIGINT
 _PD_locate_checksum(PDBfile* file);


/* PDBDIR.C declarations */

extern char
 **_PD_ls_extr(PDBfile *file, char *path, char *type, long size,
	       int *num, int all, char *flags),
 *_PD_fixname(PDBfile *file, char *inname);


/* PDBX.C declarations */

extern int
 _PD_contains_indirections(hasharr *tab, char *type);

extern void
 _PD_convert_attrtab(PDBfile *file),
 _PD_rl_attribute(attribute *attr);


/* PDCONV.C declarations */

extern int
 _PD_prim_typep(char *memb, hasharr *chrt, PD_major_op error);


/* PDFIA.C declarations */


extern FIXNUM
 _PD_write_aux(PDBfile *file, char *name, char *typi, char *typo,
	       void *space, FIXNUM nd, FIXNUM *dims);


/* PDFMT.C declarations */

extern void
 _PD_check_casts_list(hasharr *chrt, char **lst, long n),
 _PD_check_casts_hash(hasharr *chrt, char **lst, long n),
 _PD_check_casts(PDBfile *file);

extern char
 *_PD_header_token(int which),
 *_PD_rfgets(char *s, int n, FILE *fp),
 *_PD_get_tbuffer(void),
 *_PD_get_token(char *bf, char *s, int n, int ch);

extern int
 _PD_put_text(int reset, int ns, char *s),
 _PD_put_string(int reset, char *fmt, ...),
 _PD_cp_tbuffer(char **buf),
 _PD_identify_file(PDBfile *file),
 _PD_format_version(PDBfile *file, int vers),
 _PD_write_attrtab(PDBfile *file),
 _PD_read_attrtab(PDBfile *file);


/* PDFMTA.C declarations */

extern int
 _PD_set_format_i(PDBfile *file);


/* PDFMTB.C declarations */

extern int
 _PD_set_format_ii(PDBfile *file);


/* PDFMTC.C declarations */

extern int
 _PD_set_format_iii(PDBfile *file);


/* PDLOW.C declarations */

extern SC_udl
 *_PD_pio_open(char *name, char *mode);

extern void
 _PD_set_io_buffer(SC_udl *pu),
 _PD_set_standard(PDBfile *file, data_standard *std, data_alignment *algn),
 _PD_init_consts(void),
 _PD_init_chrt(PDBfile *file, int ftk),
 _PD_setup_chart(hasharr *chart, data_standard *fstd,
		 data_standard *hstd, data_alignment *falign,
		 data_alignment *halign, int flag, int ftk),
 _PD_def_real(char *type, PDBfile *file),
 _PD_d_install(PDBfile *file, char *name, defstr *def, int host),
 _PD_e_install(PDBfile *file, char *name, syment *entr, int lookup);

extern int
 _PD_compare_std(data_standard *a, data_standard *b,
		 data_alignment *c, data_alignment *d),
 _PD_safe_flush(PDBfile *file),
 _PD_remove_type(PDBfile *file, char *name),
 _PD_rev_chrt(hasharr *ch),
 _PD_items_per_tuple(defstr *dp),
 _PD_set_current_address(PDBfile *file, BIGINT addr, int wh, PD_major_op tag),
 _PD_init_s(void),
 _PD_close(PDBfile *file);

extern BIGINT
 _PD_eod(PDBfile *file),
 _PD_get_next_address(PDBfile *file, char *type, long number, void *vr,
		      int seekf, int tellf, int colf),
 _PD_get_current_address(PDBfile *file, PD_major_op tag);

extern defstr
 *_PD_defstr(PDBfile *file, int host, char *name, PD_type_kind kind,
	     memdes *desc, multides *tuple,
	     long sz, int align, PD_byte_order ord,
	     int conv, int *ordr, long *formt, int unsgned, int onescmp),
 *_PD_defstr_inst(PDBfile* file, char *name, PD_type_kind kind,
		  memdes *desc, PD_byte_order ord, int *ordr,
		  long *formt, int flag),
 *_PD_type_container(PDBfile *file, defstr *dp);

extern PDBfile
 *_PD_create(tr_layer *tr, SC_udl *pu, char *name, void *a),
 *_PD_open(tr_layer *tr, SC_udl *pu, char *name, char *mode, void *a);

extern FILE
 *_PD_data_source(SC_udl *pu);

extern char
 *PD_set_text_delimiter(PDBfile *file, char *d);


/* PDMEMB.C declarations */

extern dimdes
 *_PD_ex_dims(char *memb, int defoff, int *pde);

extern defstr
 *_PD_lookup_type(char *s, hasharr *tab);

extern long
 _PD_comp_num(dimdes *dims),
 _PD_comp_nind(int nd, long *ind, int str),
 _PD_str_size(memdes *str, hasharr *tab),
 _PD_lookup_size(char *s, hasharr *tab);

extern BIGINT
 _PD_member_location(char *s, hasharr *tab, defstr *dp, memdes **pdesc);

extern int
 _PD_align(long n, char *type, int is_indirect, hasharr *tab, int *palign),
 _PD_adj_dimensions(PDBfile *file, char *name, syment *ep);

extern char
 *_PD_member_type(char *s),
 *_PD_member_base_type(char *s),
 *_PD_member_name(char *s),
 *_PD_var_name(char *s),
 *_PD_hyper_type(char *name, char *type),
 *_PD_var_namef(PDBfile *file, char *name, char *bf);


/* PDPARMP.C declarations */

extern int
 _PD_set_collective(int flag),
 _PD_init_d(void);


/* PDPATH.C declarations */

extern long
 _PD_hyper_number(PDBfile *file, char *indxpr, long numb, dimdes *dims, long *poff),
 _PD_parse_index_expr(char *expr, dimdes *dim,
		      long *pstart, long *pstop, long *pstep),
 _PD_num_indirects(char *type, hasharr *tab),
 _PD_member_items(char *s);

extern BIGINT
 _PD_skip_over(PDBfile *file, long skip, int noind);

extern char
 *_PD_tr_syment_v(PDBfile *file, syment *ep, char *s, memdes **pdesc);


/* PDPRNT.C declarations */

extern int
 _PD_print_leaf(PD_printdes *prnt, PDBfile *file, char *vr, long nitems,
		char *type, int irecursion, int n, long *ind);

extern void
 _PD_set_user_defaults(void),
 _PD_set_user_formats(void),
 _PD_set_format_defaults(void),
 _PD_set_digits(PDBfile *file),
 _PD_digits_tol(PDBfile *file_a, PDBfile *file_b);


/* PDPTR.C declarations */

extern int
 _PD_ptr_index(void *p),
 _PD_ptr_reset(PDBfile *file, char *vr),
 _PD_ptr_entry_itag(PDBfile *file, int n, PD_itag *pi),
 _PD_ptr_wr_itags(PDBfile *file, void *vr, long nitems, char *type),
 _PD_ptr_rd_itags(PDBfile *file, char **vr, PD_itag *pi);

extern long
 _PD_ptr_wr_lookup(PDBfile *file, void *vr, int *ploc, int write),
 _PD_ptr_get_index(PDBfile *file, char *bf),
 _PD_ptr_read(PDBfile *file, BIGINT addr, int force),
 _PD_ptr_fix(PDBfile *file, long n);

extern void
 _PD_ptr_save_ap(PDBfile *file, SC_array **poa, char **pob, char *base),
 _PD_ptr_restore_ap(PDBfile *file, SC_array *oa, char *ob),
 _PD_ptr_init_apl(PDBfile *file),
 _PD_ptr_free_apl(PDBfile *file),
 _PD_index_ptr(char *p, int i),
 _PD_ptr_rd_install_addr(PDBfile *file, BIGINT addr, int loc),
 _PD_ptr_wr_syment(PDBfile *file, long n, char *type, long nitems, BIGINT addr),
 _PD_ptr_open_setup(PDBfile *file);

extern syment
 *_PD_ptr_get_entry(PDBfile *file, long i);


/* PDRDWR.C declarations */

extern char
 *_PD_expand_hyper_name(PDBfile *file, char *name);

extern int
 _PD_hyper_write(PDBfile *file, char *name, syment *ep, void *vr,
		 char *intype),
 _PD_hyper_read(PDBfile *file, char *name, char *type,
		syment *ep, void *vr),
 _PD_indexed_read_as(PDBfile *file, char *fullpath, char *type,
		     void *vr, int nd, long *ind, syment *ep),
 _PD_rd_bits(PDBfile *file, char *name, char *type,
	     long nitems, int sgned, int nbits, int padsz, int fpp, long offs,
	     long *pan, char **pdata),
 _PD_valid_dims(dimdes *dimscheck, dimdes *vardims);

extern long
 _PD_number_refd(void *vr, char *type, hasharr *tab),
 _PD_wr_syment(PDBfile *file, char *vr, long nitems, char *intype,
	       char *outtype),
 _PD_rd_syment(PDBfile *file, syment *ep, char *outtype, void *vr);

extern BIGINT
 _PD_annotate_text(PDBfile *file, syment *ep, char *name,
		   BIGINT addr, void *vr);

extern dimdes
 *_PD_hyper_dims(PDBfile *file, char *name, dimdes *dims);


/* PDSER.C declarations */

extern void
 _PD_rl_pfile(PD_Pfile *pf);

extern int
 _PD_init_state(int smpflag);

extern PD_smp_state
 *_PD_get_state(int id);


/* PDPAR.C declarations */

extern int
 _PD_init_t(void);


/* PDSHAR.C declarations */

extern int
 _PD_register(char *type, char *fmt, PFBinType hook,
	      PFBinCreate creat, PFBinOpen open, PFBinClose close,
	      PFBinWrite write, PFBinRead read),
 _PD_pdbfilep(char *type);

extern tr_layer
 *_PD_lookup(char *type);

extern void
 _PD_install_funcs(void),
 _PD_conv_in(PDBfile *file, void *out, void *in, char *type, long nitems);

extern PDBfile
 *_PD_open_bin(char *name, char *mode, void *a);

extern long
 _PD_write_bin(PDBfile *file),
 _PD_read_bin(PDBfile *file);

extern int
 _PD_close_bin(PDBfile *file),
 _PD_spokep(char *type);

extern char
 *_PD_file_type(PDBfile *file);


#ifdef __cplusplus
}
#endif

#endif

