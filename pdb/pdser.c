/*
 * PDSER.C - serial routines and parallel/serial API
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "scope_mpi.h"

/*--------------------------------------------------------------------------*/

/*                          SERIAL PFM ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PD_PFM_INIT_S - initialize the file manager for parallel settings */

static void _PD_pfm_init_s(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_ADD_FILE_S - add a file to the list of managed files */

static void _PD_pfm_add_file_s(PDBfile *file, int64_t start_addr)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_REMOVE_FILE_S - remove the given file from
 *                       - the list of managed files
 */

static void _PD_pfm_remove_file_s(FILE *file)
   {
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_GETSPACE_S - return the next disk address available for writing
 *                    - and increment it to reserve space
 */

static int64_t _PD_pfm_getspace_s(PDBfile *file, size_t nbytes,
				int rflag, int colf)
   {int64_t addr;
    int64_t old, new;

    old = file->chrtaddr;
    new = _PD_get_current_address(file, PD_GENERIC);

    addr = max(old, new);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_EXTEND_FILE_S - extend the file by the given number of bytes */

static int _PD_pfm_extend_file_s(PDBfile *file, long nb)
   {FILE *fp;
    int64_t addr;
    char bf[1];

    if (nb > 0)
       {fp   = file->stream;
	addr = file->chrtaddr + nb;

/* expand the file or the space will not be there */
	if (_PD_set_current_address(file, addr, SEEK_SET, PD_GENERIC) != 0)
	   {PD_error("FSEEK FAILED - _PD_PFM_EXTEND_FILE_S", PD_GENERIC);
	    return(FALSE);};

	bf[0] = ' ';
	nb    = lio_write(bf, (size_t) 1, (size_t) 1, fp);
	if (nb != 1L)
	   {PD_error("CAN'T SET FILE SIZE - _PD_PFM_EXTEND_FILE_S",
		     PD_GENERIC);
	    return(FALSE);};

	file->chrtaddr = addr;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_IS_MASTER_S - return TRUE iff it is OK to update the EOD point
 *                     - in FILE
 */

static int _PD_pfm_is_master_s(PDBfile *file)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_EOD_S - reset the EOD point in the file */

static int _PD_set_eod_s(PDBfile *file, int64_t addr, long nb)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_NEXT_ADDRESS_S - reserve the appropriate amount of space in FILE
 *                    - this a worker for _PD_get_next_address
 */

static int64_t _PD_next_address_s(PDBfile *file, const char *type,
				  long number, const void *vr,
				  int seekf, int tellf, int colf)
   {int64_t addr;
    FILE *fp;

    fp = file->stream;

    if (tellf)
       addr = lio_tell(fp);
    else
       addr = file->chrtaddr;

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_STREAM_S - get the FILE from the FILE */

static FILE *_PD_get_file_stream_s(PDBfile *file)
   {FILE *fp;

    fp = file->stream;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_PTR_S - get the BF_FILE from the FILE */

static BF_FILE *_PD_get_file_ptr_s(FILE *file)
   {BF_FILE *fb;

    fb = (BF_FILE *) file;

    return(fb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_SIZE_S - return the file size */

static int64_t _PD_get_file_size_s(PDBfile *file)
   {int64_t rv;
    FILE *fp;

    fp = (file != NULL) ? file->stream : NULL;

    rv = SC_filelen(fp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_MARK_AS_FLUSHED_S - remember that the file is flushed */

static void _PD_pfm_mark_as_flushed_s(PDBfile *file, int wh)
   {

    file->flushed = wh;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SERIAL_FLUSH_S - do a serial file level flush */

static int _PD_pfm_serial_flush_s(FILE *fp, int tid)
   {int ret;

    ret = lio_flush(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_FLUSH_FILE_S - do PDB level flush on the given file */

static int _PD_pfm_flush_file_s(PDBfile *file)
   {int ret;

    if (file->flushed)
       ret = TRUE;
    else
       ret = (*file->flush)(file);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETUP_FILE_S - convert/wrap a BF_FILE, FB, to a 
 *                      - FILE and return it
 */

static FILE *_PD_pfm_setup_file_s(BF_FILE *fb)
   {FILE *rf;

    rf = (FILE *) fb;

    return(rf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETUP_MP_FILE_S - setup a PDBfile to be MP */

static void _PD_pfm_setup_mp_file_s(PDBfile *file, SC_communicator comm)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETADDR_S - set the next available address for writing to ADDR */

static void _PD_pfm_setaddr_s(PDBfile *file, int64_t addr)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_DP_INIT_S - return TRUE iff the DP initializations
 *                  - have been done
 */

static int _PD_is_dp_init_s(void)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_SMP_INIT_S - return TRUE iff the SMP initializations
 *                   - have been done
 */

static int _PD_is_smp_init_s(void)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_SEQUENTIAL_S - return TRUE iff this is a serial session */

static int _PD_is_sequential_s(void)
   {int ret;

    ret = TRUE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_NULL_FP_S - check if file ptr is null */

static int _PD_is_null_fp_s(void *fp)
   {int rv;

    rv = (fp == NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_S - initialize the library for serial */

int _PD_init_s(void)
   {

    PD_gs.par.open_hook         = _SC_ios.lfopen;

    PD_gs.par.init              = _PD_pfm_init_s;
    PD_gs.par.add_file          = _PD_pfm_add_file_s;
    PD_gs.par.remove_file       = _PD_pfm_remove_file_s;
    PD_gs.par.getspace          = _PD_pfm_getspace_s;
    PD_gs.par.is_dp_init        = _PD_is_dp_init_s;
    PD_gs.par.is_smp_init       = _PD_is_smp_init_s;
    PD_gs.par.is_sequential     = _PD_is_sequential_s;
    PD_gs.par.is_null_fp        = _PD_is_null_fp_s;
    PD_gs.par.is_master         = _PD_pfm_is_master_s;

    PD_gs.par.get_file_size     = _PD_get_file_size_s;
    PD_gs.par.get_file_stream   = _PD_get_file_stream_s;
    PD_gs.par.get_file_ptr      = _PD_get_file_ptr_s;
    PD_gs.par.next_address      = _PD_next_address_s;

    PD_gs.par.setup_pseudo_file = _PD_pfm_setup_file_s;
    PD_gs.par.setup_mp_file     = _PD_pfm_setup_mp_file_s;
    PD_gs.par.extend_file       = _PD_pfm_extend_file_s;
    PD_gs.par.flush_file        = _PD_pfm_flush_file_s;
    PD_gs.par.serial_flush      = _PD_pfm_serial_flush_s;
    PD_gs.par.set_eod           = _PD_set_eod_s;
    PD_gs.par.mark_as_flushed   = _PD_pfm_mark_as_flushed_s;
    PD_gs.par.set_address       = _PD_pfm_setaddr_s;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_THREAD - init state PA for thread ID */

static void _PD_init_thread(PD_smp_state *pa, int id)
   {

/* PDB state */
    pa->append_flag  = FALSE;
    pa->ofp          = NULL;
    pa->cfp          = NULL;

/* PDPARMP state */
    _PD_set_world_comm(id);

/* PDLOW state */
    pa->n_casts  = 0L;
    pa->has_dirs = FALSE;
    pa->tbf      = NULL;
    pa->tbuffer  = NULL;
    pa->ncx      = 0L;
    pa->nc       = 0L;
    pa->spl      = NULL;

/* PDFIA state */
    pa->outlist   = NULL;
    pa->n_entries = 0;

/* PDRDWR state */
    pa->rw_call_stack = NULL;
    pa->rw_call_ptr   = 0L;
    pa->rw_call_ptr_x = 0L;
    pa->rw_lval_stack = NULL;
    pa->rw_lval_ptr   = 0L;
    pa->rw_lval_ptr_x = 0L;
    pa->rw_str_stack  = NULL;
    pa->rw_str_ptr    = 0L;
    pa->rw_str_ptr_x  = 0L;
    pa->rw_list_t     = 0L;
    pa->rw_list_d     = 0L;

/* PDSZOF state */
    pa->sz_call_stack = NULL;
    pa->sz_call_ptr   = 0L;
    pa->sz_call_ptr_x = 0L;
    pa->sz_lval_stack = NULL;
    pa->sz_lval_ptr   = 0L;
    pa->sz_lval_ptr_x = 0L;
    pa->sz_str_stack  = NULL;
    pa->sz_str_ptr    = 0L;
    pa->sz_str_ptr_x  = 0L;
    pa->sz_list_t     = 0L;
    pa->sz_list_d     = 0L;

/* PDBNET state */
    pa->pn_open_save  = NULL;
    pa->host_chart    = NULL;
    pa->host_std      = NULL;
    pa->host_align    = NULL;

/* PDPATH state */
    pa->frames = NULL;

    pa->buffer_size = PD_gs.buffer_size;
    pa->int_std     = PD_gs.int_standard;
    pa->req_std     = PD_gs.req_standard;
    pa->int_align   = PD_gs.int_alignment;
    pa->req_align   = PD_gs.req_alignment;
    pa->vif         = PD_gs.vif;
    pa->wr_hook     = PD_gs.write;
    pa->rd_hook     = PD_gs.read;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_STATE - return the PD state for thread ID
 *               - use current thread if ID == -1
 */

PD_smp_state *_PD_get_state(int id)
   {intb bpi;
    PD_smp_state *pa;

    if (_PD.ita < 0)
       {bpi     = sizeof(PD_smp_state);
	_PD.ita = SC_register_thread_data("pdb-state", "PD_smp_state",
					  1, bpi,
					  (PFTinit) _PD_init_thread);};

    if (id < 0)
       id = SC_current_thread();

    pa = (PD_smp_state *) SC_get_thread_element(id, _PD.ita);

    return(pa);}

/*--------------------------------------------------------------------------*/

/*                      PARALEL/SERIAL API ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PD_RL_PFILE - release a PD_Pfile
 *              - in the MP world I think the PD_close will end up
 *              - freeing the files resources on the master process
 */

void _PD_rl_pfile(PD_Pfile *pf)
   {

    CFREE(pf->f_addr);

/* GOTCHA: double free
    CFREE(pf->stream);
 */

    CFREE(pf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_STATE - initialize the state variables needed for potential
 *                - parallel execution which would otherwise
 *                - be file static variables
 */

int _PD_init_state(int smpflag)
   {int ok;

    ONCE_SAFE(FALSE, NULL)
       ok = FALSE;
       if ((ok == FALSE) && smpflag)
          ok = _PD_init_t();

       if (ok == FALSE)
          ok = _PD_init_s();
    END_SAFE;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INIT_THREADS - initialize the library for pthreads
 *                 - must be called by only one thread.
 *
 * #bind PD_init_threads fortran() python()
 */

int PD_init_threads(int nthreads, PFTid tid)
   {int ret;

    ret = TRUE;

/* check whether SCORE level has been initialized */
    if (SC_n_threads == -1)
       {SC_configure_mm(128L, 2000000L, 65536L, 1.2);
	PD_gs.nthreads = max(0L, nthreads);
	SC_init_threads(PD_gs.nthreads, tid);};

    ONCE_SAFE(TRUE, NULL)

      PD_gs.nthreads = SC_n_threads;

       ret = _PD_init_state(TRUE);

    END_SAFE;

    return(ret);} 

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INIT_THREADS_ARG - initialize the number of threads by scanning
 *                     - for a designated key in a pattern like
 *                     -  .... <key> <# threads> ...
 *
 * #bind PD_init_threads_arg fortran() python()
 */

int PD_init_threads_arg(int c, char **v, char *key, PFTid tid)
   {int i, nt;

    nt = 0;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], key) == 0)
	    {nt = SC_stoi(v[++i]);
	     break;};};

/* make Klocworks happy */
    nt = min(nt, SC_OPT_BFSZ);

    PD_init_threads(nt, tid);

    return(nt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_NEXT_ADDRESS_PF - reserve the appropriate amount of space in FILE
 *                     - this a worker for _PD_get_next_address
 */

int64_t _PD_next_address_pf(PDBfile *file, const char *type, long number,
			    const void *vr, int seekf, int tellf, int colf)
   {int flag, ipt, sk;
    inti nb;
    intb bpi;
    int64_t addr;
    defstr *dpf;

    addr = -1;

    flag = ((file->mpi_mode == TRUE)  ||
            (!file->mpi_file)) ? FALSE : TRUE;

    dpf = _PD_type_lookup(file, PD_CHART_FILE, type);
    if (dpf != NULL)
       {ipt = _PD_items_per_tuple(dpf);

	if (dpf->n_indirects == 0)
	   {bpi = dpf->size;
	    nb  = number*ipt*bpi;
	    sk  = TRUE;}

	else if (vr == NULL)
	   {bpi = _PD_lookup_size(type, file->chart);
	    nb  = number*bpi;
	    sk  = TRUE;}

	else
	   {nb  = PD_sizeof(file, type, number, vr);
	    nb *= ipt;
	    sk  = FALSE;};

	addr = _PD_GETSPACE(file, nb, flag, colf);

	if ((seekf) && (sk == TRUE))
	   _PD_set_current_address(file, addr+nb, SEEK_SET, PD_GENERIC);};

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
