/*
 * PANACEA_INT.H - internal header in support of PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */
 
#ifndef PCK_PANACEA_INT

#include "cpyright.h"

#define PCK_PANACEA_INT

#include "pgs_int.h"
#include "pdb_int.h"
#include "scope_dp.h"
#include "panace.h"

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define GET_INFO (*_PA.get_info)

/* _PA_SELECT - build a list of unique variables to save
 *            - for post processing
 */

#define _PA_SELECT(crv, nsn, ntn, nm, nmint, axs, axsint, idx, zn, iu, nu)   \
   tmp = pr->axs;                                                            \
   for (match = FALSE, iu = 0; iu < nu; iu++)                                \
       if (strcmp(tmp, nm[iu]) == 0)                                         \
          {match = TRUE;                                                     \
           crv.idx = iu;                                                     \
           break;};                                                          \
   if (!match)                                                               \
      {nm[iu] = tmp;                                                         \
       if (pr->axsint == NULL)                                               \
          nmint[iu] = pr->axs;                                               \
       else                                                                  \
          nmint[iu] = pr->axsint;                                            \
       nsn = crv.npts;                                                       \
       ntn = pr->zn;                                                         \
       crv.idx = iu;                                                         \
       nu++;}

/*--------------------------------------------------------------------------*/

/*                   TYPEDEF AND STRUCT DEFINITIONS                         */

/*--------------------------------------------------------------------------*/
 
typedef int (*PFFIXME)(PA_variable *pp, int req, void *vr);
typedef struct s_th_record th_record;
typedef struct s_PA_scope_private PA_scope_private;

struct s_th_record
   {int n_members;
    char **members;
    char **labels;
    char *type;
    char *entry_name;};

/* PA_SCOPE_PRIVATE - static variables which must potentially be per thread */

struct s_PA_scope_private
   {int tsid;
    int nthreads;
    int default_offset;
    int isrc;
    int imap;
    int input_flag;
    int ndom;
    int max_state_files;

    int64_t edstr;
    int64_t ppstr;

/* zero initial value variables */
    char *delim;
    char *dict;
    char *dimtab;
    char *alist;
    char *lkey;
    char *heterogeneous;
    char *rsname;          /* communicate the restart name between
                              readh and wrrstrth */

    int *ncrv;
    FILE *input_stream;
    SC_array *inf;        /* array of input files */
    dimdes *ndpt;
    th_record *thd;
    PDBfile **uf;
    PA_unit_spec *units;

    pcons *ivlst;

    PA_package *last_gen;
    PA_package *last_run;

    PFFIXME get_info;
    PFBuildDom build_domain;
    PFBuildMap build_mapping;

    char type[MAXLINE];
    char pp_title[MAXLINE];
    char pp_bf[MAXLINE];
    char pp_ubf[MAXLINE];
    char input_line[MAXLINE];
    char input_bf[MAXLINE];

/* PADEF.C */
    int def_error;

/* PADUMP.C */
    int max_domains;
    int n_domains;

/* PAFI.C */
    hasharr *ddtab;

/* PAGNRD.C */
    int ivnum;

/* PAPP.C */
    int first_cycle;
    int last_cycle;
    int n_curve;

    PDBfile *pduf;

/* variables formerly in panace.h */
    int ul_print_flag;
    int n_state_files;
    int halt_fl;

    char base_name[MAXLINE];
    PDBfile **state_files;
    PA_variable *default_variable;};

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern PA_scope_private
 _PA;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* PACCESS.C declarations */

extern void
 *_PA_init_space(PA_variable *pp, long psz),
 *_PA_alloc(defstr *dp, char *type, inti ni, void *pval);


/* PADSYS.C declarations */

extern void
 _PA_init(void),
 _PA_internal_init(void);


/* PADUMP.C declarations */

extern int
 _PA_get_data(double *d, char *vr, inti ni, long offset, long stride);

extern void
 _PA_init_pp(char *ppname, char *gfname),
 _PA_scan_pp(void),
 _PA_allocate_mapping_space(PA_package *pck),
 _PA_dump_package_mappings(PA_package *pck, double t, double dt, int cycle);

extern PA_plot_request
 *_PA_splice_out_request(PA_plot_request *pr, PA_plot_request *lst);


/* PAGNRD.C declarations */

extern PA_plot_request
 *_PA_mk_plot_request(PA_set_spec *range, PA_set_spec *domain, char *text,
		      PA_plot_request *req);

extern PA_set_spec
 *_PA_proc_set_spec(char *s, PA_set_spec *lst);


/* PAMM.C declarations */

extern PA_variable
 *_PA_mk_variable(char *vname, PA_dimens *vdims, void *iv,
		  PFVarInit ifn,
		  int *vattr, char *vtype,
		  double conv_fact, double unit_fact,
		  pcons *nu, pcons *du, pcons *alist);

extern PA_dimens
 *_PA_mk_dimens(int *mini, int *maxi, int method);

extern dimdes
 *_PA_mk_sym_dims(PA_dimens *vdims);

extern PA_src_variable
 *_PA_mk_src_variable(char *name, int vindx, int n, double *pt, PDBfile *fp);

extern void
 _PA_rl_variable(PA_variable *pp),
 _PA_rl_set_spec(PA_set_spec *ip),
 _PA_rl_request(PA_plot_request *rq),
 _PA_rl_spec(PA_iv_specification *lst),
 _PA_rl_domain_map(PA_set_index *dmap);

extern long
 _PA_list_to_dims(PA_dimens *vd, dimdes *sd),
 _PA_dims_to_list(dimdes *sd, PA_dimens *vd),
 _PA_consistent_size(PA_variable *pp, int flag),
 _PA_comp_size(PA_dimens *vdims);


/* PANACEA.C declarations */

extern void
 _PA_init_files(char *edname, char *ppname, char *gfname);


/* PAPP.C declarations */

extern void
 _PA_proc_time(char *ufname);


/* PASHAR.C declarations */

extern void
 *_PA_pdb_read(PDBfile *file, char *name, syment **psp, long *indx);

extern void
 _PA_install_var(char *vname, PA_variable *pp),
 _PA_rd_variable(PDBfile *pdrs, PA_variable *pp, int conv_flag, int scope),
 _PA_wrrstrt(char *rsname, int conv_flag),
 _PA_rdrstrt(char *fname, int conv_flag);


/* PASRC.C declartions */

extern void
 _PA_init_sources(double t, double dt),
 _PA_init_queue(PA_src_variable *svp, double t, double dt),
 _PA_step_queue(PA_src_variable *svp, double t);


/* PADEF.C declarations */

extern PA_variable
 *_PA_process_def_var(char *vname, va_list *list);

extern PA_dimens
 *_PA_process_dimension(va_list *list);

extern pcons
 *_PA_process_att(int tag, va_list *list, pcons *alist),
 *_PA_process_units(va_list *list, pcons *alist);


#ifdef __cplusplus
}
#endif

#endif

