/*
 * PDPAR.C - routines to support parallel i/o
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#ifdef HAVE_THREADS

#define N_PFM_FILES  8;
#define N_PFM_INC    4;

typedef struct pfmanager_s pfmanager;

struct pfmanager_s
   {int available;
    int64_t addr;};

#define DISK(t, x)  ((x)->f_addr[t].diskaddr)

SC_THREAD_LOCK(pfmanager_lock_t);
SC_THREAD_LOCK(PD_io_lock_t);

static pfmanager
 *_PD_pfman = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MAKE_PFILE - instantiate a PD_Pfile */

static PD_Pfile *_PD_make_pfile(FILE *fp)
   {int i;
    SC_address *fa;
    PD_Pfile *pf;

    fa = CMAKE_N(SC_address, _PD_nthreads+1);
    for (i = 0; i <= _PD_nthreads; i++)
        fa[i].diskaddr = 0;

/* no buffered i/o for now */
    pf = CMAKE(PD_Pfile);

    pf->f_addr = fa;
    pf->bf     = NULL; 
    pf->stream = fp;
    pf->bf_len = 0L;
    pf->comm   = 0;
    pf->mp_id  = 0;
    pf->id     = 0;

    return(pf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FREE_PFILE - free a PD_Pfile instance */

static void _PD_free_pfile(PD_Pfile *pf)
   {

    CFREE(pf->f_addr);
    CFREE(pf);

    return;}

/*--------------------------------------------------------------------------*/

/*                  PARALLEL FILE MANAGER INTERFACE                         */

/*--------------------------------------------------------------------------*/

/* _PD_PFM_INIT_T - initialize the file manager for parallel settings */

static void _PD_pfm_init_t(void)
   {int i;
    pfmanager *pm;

    SC_LOCKON(pfmanager_lock_t);

    if (_PD_pfman == NULL)
       {_PD.nfilesx = N_PFM_FILES;

	pm = SC_alloc_n(_PD.nfilesx, sizeof(pfmanager),
			SC_MEM_ATTR_PERMANENT,  TRUE,
			SC_MEM_ATTR_NO_ACCOUNT, TRUE,
			SC_MEM_ATTR_FUNC, __func__,
			SC_MEM_ATTR_FILE, __FILE__,
			SC_MEM_ATTR_LINE, __LINE__,
			0);

	_PD_pfman = pm;

        for(i = 0; i < _PD.nfilesx; i++, pm++)
           {pm->available = TRUE;
            pm->addr      = 0;};};

     SC_LOCKOFF(pfmanager_lock_t);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_ADD_FILE_T - add a file to the list of managed files */

static void _PD_pfm_add_file_t(PDBfile *file, int64_t start_addr)
   {int i, ins;
    pfmanager *pm;
    PD_Pfile *pf;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    if (pf != NULL)
       {SC_LOCKON(pfmanager_lock_t);

	for (pm = _PD_pfman, ins = 0; ins < _PD.nfilesx; pm++, ins++)
	    if (pm->available) 
	       break;

	if (ins == _PD.nfilesx)
	   {_PD.nfilesx += N_PFM_INC;
	    NREMAKE(_PD_pfman, pfmanager, _PD.nfilesx);

	    pm = &_PD_pfman[ins];
	    for (i = ins; i < _PD.nfilesx; pm++, i++)
	        {pm->available = TRUE;
		 pm->addr      = 0;};}
                    
	_PD_pfman[ins].available = FALSE;
	_PD_pfman[ins].addr      = start_addr;

	pf->id = ins;

	SC_LOCKOFF(pfmanager_lock_t);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_REMOVE_FILE_T - remove the given file from
 *                       - the list of managed files
 */

static void _PD_pfm_remove_file_t(FILE *fp)
   {int id;
    PD_Pfile *pf;

    pf = GET_PFILE(fp);
    id = pf->id;

    SC_LOCKON(pfmanager_lock_t);

    _PD_pfman[id].available = TRUE;
    _PD_pfman[id].addr      = 0;

    SC_LOCKOFF(pfmanager_lock_t);
 
    _PD_rl_pfile(pf);

    return;}

/*--------------------------------------------------------------------------*/

/*                      PARALLEL FILE C I/O INTERFACE                       */

/*--------------------------------------------------------------------------*/

/* _PD_PSETVBUF - set the parallel file buffer */

static int _PD_psetvbuf(FILE *stream, char *bf, int type, size_t size)
   {int rv;
    PD_Pfile *pf;

    pf = GET_PFILE(stream);

    if (pf == NULL)
       rv = FALSE;

    else
       rv = lio_setvbuf(pf->stream, bf, type, size);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PCLOSE - close the parallel file */

static int _PD_pclose(FILE *stream)
   {int rv, fail;
    PD_Pfile *pf;

    pf = GET_PFILE(stream);

    fail = EOF;
    rv   = 0;

    if (pf == NULL)
       rv = fail;

    else
       {if (pf->stream != NULL)
           rv = lio_close(pf->stream);

        _PD_pfm_remove_file_t(stream);

        _PD_free_pfile(pf);};
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFLUSH - do an fflush on the parallel file */

static int _PD_pflush(FILE *stream)
   {int rv, fail;
    PD_Pfile *pf;

    pf = GET_PFILE(stream);

    fail = EOF;
    rv   = fail;

    if (pf == NULL)
       rv = fail;

/* when buffered i/o is introduced this will be changed */
    else
       {SC_LOCKON(PD_io_lock_t);
        rv = lio_flush(pf->stream);
        SC_LOCKOFF(PD_io_lock_t);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTELL - do an ftell on the parallel file */

static int64_t _PD_ptell(FILE *stream)
   {int64_t addr;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    addr = -1;

    if (pf != NULL)
       addr = DISK(tid, pf);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PSEEK - do an fseek on the parallel file */

static int _PD_pseek(FILE *stream, int64_t offset, int whence)
   {int rv, fail;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = -1;
    rv   = 0;

    if (pf == NULL)
       rv = fail;

    else
       {switch (whence)
           {case SEEK_SET :
                 DISK(tid, pf) = offset;
                 break;

            case SEEK_CUR :
                 DISK(tid, pf) += offset;
                 break;

            case SEEK_END :
                 SC_LOCKON(PD_io_lock_t);
                 rv = lio_seek(pf->stream, offset, SEEK_END);
                 DISK(tid, pf) = lio_tell(pf->stream);
                 SC_LOCKOFF(PD_io_lock_t);
                 break;

            default :
                 rv = fail;
                 break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PREAD - do an fread on the parallel file */

static uint64_t _PD_pread(void *s, size_t nbi, uint64_t ni, FILE *stream)
   {int status;
    size_t nbw, fail;
    int64_t offs;
    FILE *strm;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = 0; 

    if (pf == NULL)
       nbw = fail;

    else
       {SC_LOCKON(PD_io_lock_t);

        offs = DISK(tid, pf);
        strm = pf->stream;

        status = lio_seek(strm, offs, SEEK_SET);

        if (status == -1)
           nbw = fail;
        else
           {nbw = lio_read(s, nbi, ni, strm);
            DISK(tid, pf) += nbi*ni;};

        SC_LOCKOFF(PD_io_lock_t);};

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PWRITE - do an fwrite on the parallel file */

static uint64_t _PD_pwrite(void *s, size_t nbi, uint64_t ni, FILE *stream)
   {int rv;
    long nbw;
    int64_t ad;
    size_t fail;
    FILE *fs;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = 0;

    if ((pf == NULL) || (nbi*ni == 0))
       nbw = fail;

    else
       {SC_LOCKON(PD_io_lock_t);

        fs = pf->stream;
        ad = DISK(tid, pf);
        rv = lio_seek(fs, ad, SEEK_SET);

        if (rv != 0)
           nbw = fail;
        else
           {nbw = lio_write(s, nbi, ni, fs);
            DISK(tid, pf) += nbi*ni;};

        SC_LOCKOFF(PD_io_lock_t);};

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PPRINTF - do an fprintf on the parallel file */

static int _PD_pprintf(FILE *stream, char *fmt, va_list a)
   {int rv;
    size_t ni, nw;
    char *bf;

    nw = 0;

    if (stream != NULL)
       {bf = SC_vdsnprintf(FALSE, fmt, a);
        ni = strlen(bf);
	nw = _PD_pwrite(bf, (size_t) 1, ni, stream);};

    rv = nw;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PPUTS - do an fputs on the parallel file */

static int _PD_pputs(const char *s, FILE *stream)
   {int status, fail;
    long nbw;
    int64_t ad;
    FILE *fs;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = 0;

    if (pf == NULL)
       nbw = fail;

    else
       {SC_LOCKON(PD_io_lock_t);

        fs     = pf->stream;
        ad     = DISK(tid, pf);
        status = lio_seek(fs, ad, SEEK_SET);
 
        if (status == -1)
           nbw = fail;
        else 
           {nbw = lio_puts(s, pf->stream);
            DISK(tid, pf) += nbw;};

        SC_LOCKOFF(PD_io_lock_t);};

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PGETS - do an fgets on the parallel file */

static char *_PD_pgets(char *s, int nc, FILE *stream)
   {int status;
    int64_t ad;
    char *ps, *fail;
    FILE *fs;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = NULL;
    ps   = fail;

    if (pf == NULL)
       ps = fail;

    else
       {SC_LOCKON(PD_io_lock_t);

        fs     = pf->stream;
        ad     = DISK(tid, pf);
        status = lio_seek(fs, ad, SEEK_SET);

        if (status == -1) 
           ps = fail;
        else
           {ps = lio_gets(s, nc, fs);
          
            if (ps != fail)
               DISK(tid, pf) += strlen(ps);};

        SC_LOCKOFF(PD_io_lock_t);};

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PGETC - do an fgetc on the parallel file */

static int _PD_pgetc(FILE *stream)
   {int c, status, fail;
    int64_t ad;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = EOF;

    if (pf == NULL)
       c = fail;

    else
       {SC_LOCKON(PD_io_lock_t);

        ad     = DISK(tid, pf);
        status = lio_seek(pf->stream, ad, SEEK_SET);

        if (status == -1)
           c = fail;
        else
           {c = lio_getc(pf->stream);
            DISK(tid, pf) += 1;};

        SC_LOCKOFF(PD_io_lock_t);};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PUNGETC - do an fungetc on the parallel file */

static int _PD_pungetc(int c, FILE *stream)
   {int status, fail;
    int64_t ad;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = 0;

    if (pf == NULL)
       c = fail;

    else
       {SC_LOCKON(PD_io_lock_t);

        ad     = DISK(tid, pf);
        status = lio_seek(pf->stream, ad, SEEK_SET);

        if (status == -1)
           c = fail;
        else
           {c = lio_ungetc(c, pf->stream);
            DISK(tid, pf) -= 1;};

        SC_LOCKOFF(PD_io_lock_t);};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_POPEN - set up the file for smp parallel i/o */

static FILE *_PD_popen(char *name, char *mode)
   {FILE *fp, *fail;
    PD_Pfile *pf;
    file_io_desc *fid;

    fail = NULL;

    if (name == NULL)
       fp = fail;
    else
       {
#ifdef SC_C_STD_BUFFERED_IO
	fp = SC_lfopen(name, mode);
#else
	fp = SC_lbopen(name, mode);
#endif

        if (fp == fail)
           return(fail);};

    pf = _PD_make_pfile(fp);

    if (_PD_pfman == NULL)
       _PD_pfm_init_t();

    fid          = SC_make_io_desc(pf);
    fid->fopen   = _PD_popen;

    fid->lftell  = _PD_ptell;
    fid->lfseek  = _PD_pseek;
    fid->lfread  = _PD_pread;
    fid->lfwrite = _PD_pwrite;

    fid->setvbuf = _PD_psetvbuf;
    fid->fclose  = _PD_pclose;
    fid->fprintf = _PD_pprintf;
    fid->fputs   = _PD_pputs;
    fid->fgetc   = _PD_pgetc;
    fid->ungetc  = _PD_pungetc;
    fid->fflush  = _PD_pflush;
    fid->fgets   = _PD_pgets;

    fp = (FILE *) fid;

    return(fp);}

/*--------------------------------------------------------------------------*/

/*                      PARALLEL FILE MANAGER ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETUP_FILE_T - convert/wrap a BF_FILE, FB, to a 
 *                      - FILE and return it
 */

static FILE *_PD_pfm_setup_file_t(BF_FILE *fb)
   {FILE *rf;
    PD_Pfile *pf;
    file_io_desc *fid;

    rf = NULL;

    fid = (file_io_desc *) _PD_popen(NULL, NULL);
    if (fid != NULL)
       {pf         = GET_PFILE(fid->info);
	pf->stream = fb;
	rf         = (FILE *) pf;

	CFREE(fid);};

    return(rf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETUP_MP_FILE_T - setup a PDBfile to be MP */

static void _PD_pfm_setup_mp_file_t(PDBfile *file, SC_communicator comm)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_GETSPACE_T - return the next disk address available for writing
 *                    - and increment it to reserve space in threaded mode
 */

static int64_t _PD_pfm_getspace_t(PDBfile *file, size_t nbytes,
				int rflag, int colf)
   {int id;
    int64_t rv;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    rv = 0;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    if (pf != NULL)
       {id = pf->id;

	SC_LOCKON(pfmanager_lock_t);

	rv = _PD_pfman[id].addr;
	if (nbytes > 0)
	   _PD_pfman[id].addr += nbytes;

	SC_LOCKOFF(pfmanager_lock_t);

	DISK(tid, pf) = rv;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETADDR_T - set the next available address for writing to ADDR */

static void _PD_pfm_setaddr_t(PDBfile *file, int64_t addr)
   {int id;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);
    
    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    if (pf != NULL)
       {id = pf->id;

	SC_LOCKON(pfmanager_lock_t);
	_PD_pfman[id].addr = addr;
	SC_LOCKOFF(pfmanager_lock_t);

	DISK(tid, pf) = addr;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_EXTEND_FILE_T - extend the file by the given number of bytes */

static int _PD_pfm_extend_file_t(PDBfile *file, long nb)
   {int64_t addr;
    char bf[1];
    FILE *fp;

    if (nb > 0)
       {fp   = file->stream;
	addr = file->chrtaddr + nb;

/* expand the file or the space will not be there */
	bf[0] = ' ';
	nb    = lio_write(bf, (size_t) 1, (size_t) 1, fp);
	if (nb != 1L)
	   {PD_error("CAN'T SET FILE SIZE - _PD_PFM_EXTEND_FILE_T",
		     PD_GENERIC);
	    return(FALSE);};

	file->chrtaddr = addr;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_NEXT_ADDRESS_T - reserve the appropriate amount of space in FILE
 *                    - this a worker for _PD_get_next_address
 */

static int64_t _PD_next_address_t(PDBfile *file, char *type, long number,
				void *vr, int seekf, int tellf, int colf)
   {int flag, ipt;
    size_t nb, bpi;
    int64_t addr;
    defstr *dpf;

    flag = ((file->mpi_mode == TRUE)  ||
            (!file->mpi_file)) ? FALSE : TRUE;

    dpf = _PD_lookup_type(type, file->chart);
    ipt = _PD_items_per_tuple(dpf);

    if (vr == NULL)
       {bpi  = _PD_lookup_size(type, file->chart);
	nb   = number*ipt*bpi;
	addr = _PD_GETSPACE(file, nb, flag, colf);

	if (seekf)
	   _PD_set_current_address(file, addr+nb, SEEK_SET, PD_GENERIC);}

    else
       {nb   = PD_sizeof(file, type, number, vr);
	nb  *= ipt;
	addr = _PD_GETSPACE(file, nb, flag, colf);};

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_STREAM_T - get the FILE from the FILE */

static FILE *_PD_get_file_stream_t(PDBfile *file)
   {FILE *fp;
    PD_Pfile *pf;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    fp = (pf != NULL) ? (FILE *) pf->stream : NULL;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_PTR_T - get the BF_FILE from the FILE */

static BF_FILE *_PD_get_file_ptr_t(FILE *file)
   {BF_FILE *fb;
    PD_Pfile *pf;

    pf = GET_PFILE(file);
    fb = (pf != NULL) ? (BF_FILE *) pf->stream : NULL;

    return(fb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_SIZE_T - return the file size */

static int64_t _PD_get_file_size_t(PDBfile *file)
   {int64_t ln;
    FILE *fp;
    PD_Pfile *pf;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    fp = (pf != NULL) ? (FILE *) pf->stream : NULL;

    ln = SC_filelen(fp);

    return(ln);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_MARK_AS_FLUSHED_T - remember that the file is flushed */

static void _PD_pfm_mark_as_flushed_t(PDBfile *file, int wh)
   {

    file->flushed = wh;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SERIAL_FLUSH_T - do a serial flush */

static int _PD_pfm_serial_flush_t(FILE *fp, int tid)
   {int rv;

    rv = lio_flush(fp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_FLUSH_FILE_T - flush the given file */

static int _PD_pfm_flush_file_t(PDBfile *file)
   {int rv;

    if (file->flushed == TRUE)
       rv = TRUE;
    else
       rv = (*file->flush)(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_IS_MASTER_T - return TRUE iff it is OK to update the EOD point
 *                     - in FILE
 */

static int _PD_pfm_is_master_t(PDBfile *file)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_EOD_T - reset the EOD point in the file */

static int _PD_set_eod_t(PDBfile *file, int64_t addr, long nb)
   {

    file->chrtaddr = addr;
/*    lio_seek(file->stream, addr+nb, SEEK_SET); */

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_DP_INIT_T - return TRUE iff the DP initializations
 *                  - have been done
 */

static int _PD_is_dp_init_t(void)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_SMP_INIT_T - return TRUE iff the SMP initializations
 *                   - have been done
 */

static int _PD_is_smp_init_t(void)
   {int rv;

    rv = (_PD_nthreads >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_SEQUENTIAL_T - return TRUE iff this is a serial session */

static int _PD_is_sequential_t(void)
   {int rv;

    rv = (_PD_nthreads == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_NULL_FP_T - check if file ptr is NULL */

static int _PD_is_null_fp_t(void *fp)
   {int rv;
    PD_Pfile *pf;

    rv = TRUE;
    if (fp != NULL)
       {pf = FILE_IO_INFO(PD_Pfile, fp);
	rv = (pf->stream == NULL);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_T - initialize the library for SMP */

int _PD_init_t(void)
   {

    PD_par_fnc.open_hook         = _PD_popen;

    PD_par_fnc.init              = _PD_pfm_init_t;
    PD_par_fnc.add_file          = _PD_pfm_add_file_t;
    PD_par_fnc.remove_file       = _PD_pfm_remove_file_t;
    PD_par_fnc.getspace          = _PD_pfm_getspace_t;
    PD_par_fnc.is_dp_init        = _PD_is_dp_init_t;
    PD_par_fnc.is_smp_init       = _PD_is_smp_init_t;
    PD_par_fnc.is_sequential     = _PD_is_sequential_t;
    PD_par_fnc.is_null_fp        = _PD_is_null_fp_t;
    PD_par_fnc.is_master         = _PD_pfm_is_master_t;

    PD_par_fnc.get_file_size     = _PD_get_file_size_t;
    PD_par_fnc.get_file_stream   = _PD_get_file_stream_t;
    PD_par_fnc.get_file_ptr      = _PD_get_file_ptr_t;
    PD_par_fnc.next_address      = _PD_next_address_t;

    PD_par_fnc.setup_pseudo_file = _PD_pfm_setup_file_t;
    PD_par_fnc.setup_mp_file     = _PD_pfm_setup_mp_file_t;
    PD_par_fnc.extend_file       = _PD_pfm_extend_file_t;
    PD_par_fnc.flush_file        = _PD_pfm_flush_file_t;
    PD_par_fnc.serial_flush      = _PD_pfm_serial_flush_t;
    PD_par_fnc.set_eod           = _PD_set_eod_t;
    PD_par_fnc.mark_as_flushed   = _PD_pfm_mark_as_flushed_t;
    PD_par_fnc.set_address       = _PD_pfm_setaddr_t;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* _PD_INIT_T - initialize the library for DP */

int _PD_init_t(void)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
