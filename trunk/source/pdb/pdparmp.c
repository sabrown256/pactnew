/*
 * PDPARMP.C - routines to support MP parallel I/O
 *
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "scope_mpi.h"

#ifndef HAVE_MPI

# if defined(PTHREAD_POSIX) && defined(HAVE_THREAD_SAFE_MPI)
#  define USE_THREAD_SAFE_MPI
# endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_D - initialize the library for DP if there is no MPI support */

int _PD_init_d(void)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* DEBUG */
/* #define DEBUG */

#ifdef DEBUG
# define DBG(msg)             PRINT(stderr, "%d: %s\n", _PD.mp_rank, msg);
# define DBG1(msg, arg)       fprintf(stderr, msg, arg);
# define DBG2(msg, rank, arg) fprintf(stderr, msg, rank, arg);
#else
# define DBG(msg)
# define DBG1(msg, arg)       
# define DBG2(msg, rank, arg)
#endif

/* Parallel File Manager (PFM) defaults */
#define PFM_N_FILES     8    /* init number of parallel files to manage */
#define PFM_N_INC       4    /* num to grow by when increasing list size */
#define PFM_TERM_MASTER 99   /* magic number to return if shutting down PFM */

/* TRUE if file is a valid MPI-IO file handle */
#define IS_MPI_IO(_f)     (_PD_is_comm_null((_f)->comm) == FALSE)

/* get current disk address for given thread ID */
#define DISK(t, x)  ((x)->f_addr[t].diskaddr)


/* MPI tags for requests */

enum e_PD_MP_request_tag
   {PD_MP_NULL_REQ,  /* NULL request */
    PD_MP_GETSPACE,  /* Get address for space in the parallel file */
    PD_MP_FLUSH,     /* Request flush of file */
    PD_MP_CLOSE,     /* Request closing of file */
    PD_MP_SHUTDOWN,  /* Notify PFM to shutdown */
    PD_MP_SYNC};     /* Sync symtab */

typedef enum e_PD_MP_request_tag PD_MP_request_tag;

typedef struct s_pflist pflist;
typedef struct s_pfelement pfelement;
typedef struct s_PD_MP_ADD_req PD_MP_ADD_req;
typedef struct s_PD_MP_GETSPACE_req PD_MP_GETSPACE_req;
typedef struct s_PD_MP_FLUSH_req PD_MP_FLUSH_req;
typedef struct s_PD_MP_request PD_MP_request;

struct s_pfelement
   {int available;   /* is this element available for use */
    int64_t addr;      /* current disk address */
    char *name;      /* filename */
    int nflushed;    /* num of processes who have flushed this pfile */
    int nprocs;      /* num of processes total */
    PDBfile *file;}; /* underlying PDBfile */

struct s_pflist
   {int len;                  /* size of elem array */
    pfelement *elem;};        /* array of pfile elements to manage */
 
struct s_PD_MP_ADD_req
   {char *fname;
    int64_t start_addr;};

struct s_PD_MP_GETSPACE_req
   {int id;
    long nbytes;};

struct s_PD_MP_FLUSH_req
   {int id;
    char *buf;};

struct s_PD_MP_request
   {int type;          /* type (MPI tag) of request received */
    int sender;        /* rank of process who sent request */
    int len;           /* length of the buffer */
    char *buf;};       /* buffer to receive request msg into */

PDBfile
 *PD_pvif = NULL;

static pflist
 *_PD_pflst = NULL;   /* parallel file manager's list */

SC_DETACHED_THREAD(_PD_thread_attr);

SC_THREAD_LOCK(pflist_lock_d);
SC_THREAD_LOCK(PD_io_lock_d);

static void
 _PD_pfm_service_req(int id, int request),
 _PD_pfm_remove_file_d(FILE *fp),
 _PD_pfm_init_d(void);

/*--------------------------------------------------------------------------*/

/*                   PARALLEL OPERATION PREDICATES                          */

/*--------------------------------------------------------------------------*/

/* _PD_IS_DP_INIT_D - return TRUE iff the DP initializations
 *                  - have been done
 */

static int _PD_is_dp_init_d(void)
   {int rv;

    rv = (_PD.mp_rank >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_SMP_INIT_D - return TRUE iff the SMP initializations
 *                   - have been done
 */

static int _PD_is_smp_init_d(void)
   {int rv;

    rv = (_PD_nthreads >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_SEQUENTIAL_D - return TRUE iff this is a serial session */

static int _PD_is_sequential_d(void)
   {int rv;

    rv = (_PD_nthreads == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_NULL_FP_D - check if file ptr is null */

static int _PD_is_null_fp_d(void *fp)
   {int rv;
    PD_Pfile *pf;

    rv = TRUE;

    if (fp != NULL)
       {pf = FILE_IO_INFO(PD_Pfile, fp);
	rv = (pf->stream == NULL);};

    return(rv);}


/*--------------------------------------------------------------------------*/

/*                       MPI PARALLEL C I/O INTERFACE                       */

/*--------------------------------------------------------------------------*/

/* _PD_MPTELL - do an ftell on the parallel file */

static int64_t _PD_mptell(FILE *stream)
   {long addr;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    addr = -1;

    if (pf != NULL)
       addr = DISK(tid, pf);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPSEEK - do an fseek on the parallel file */

static int _PD_mpseek(FILE *stream, int64_t offset, int whence)
   {int rv, fail;
    PD_Pfile *pf;
    MPI_Offset rmoffset;
    MPI_File *fh;
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
	         fh = (MPI_File *) pf->stream;
                 if (IS_MPI_IO(pf))
                    {SC_LOCKON(PD_io_lock_d);
                     MPI_File_get_size(*fh, &rmoffset);
                     rmoffset += offset;
                     DISK(tid, pf) = rmoffset;
                     SC_LOCKOFF(PD_io_lock_d);}
                 else
                    {SC_LOCKON(PD_io_lock_d);
                     rv = lio_seek(pf->stream, offset, SEEK_END);
                     DISK(tid, pf) = lio_tell(pf->stream);
                     SC_LOCKOFF(PD_io_lock_d);}
                 break;
    
            default :
                 rv = fail;
                 break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPSETVBUF - set the parallel file buffer */

static int _PD_mpsetvbuf(FILE *stream, char *bf, int type, size_t size)
   {int rv;
    PD_Pfile *pf;

    pf = GET_PFILE(stream);

    rv = 0;

    if (pf != NULL)
       {if (pf->stream != NULL)
	   {if (IS_MPI_IO(pf) == FALSE)
	       rv = lio_setvbuf(pf->stream, bf, type, size);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPCLOSE - close the parallel file
 *             - this needs to flush, then send a flush (& close?) message
 *             - to server, wait for it to complete, then close
 */

static int _PD_mpclose(FILE *stream)
   {int rv, fail;
    PD_Pfile *pf;

    DBG("+ _PD_mpclose");

    pf = GET_PFILE(stream);

    fail = EOF;
    rv   = 0;

    if (pf == NULL)
       rv = fail;

    else
       {if (pf->stream != NULL)
	   {if (IS_MPI_IO(pf))
	       rv = MPI_File_close(pf->stream);
	    else
               rv = lio_close(pf->stream);};

        _PD_pfm_remove_file_d(stream);};
        
    DBG("- _PD_mpclose");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPFLUSH - do an fflush on the parallel file */

static int _PD_mpflush(FILE *stream)
   {int rv, fail;
    PD_Pfile *pf;
    MPI_File *fh;

    DBG("+ _PD_mpflush");

    pf = GET_PFILE(stream);

    fail = EOF;
    rv   = fail;

    if (pf == NULL)
       rv = fail;

    else if (IS_MPI_IO(pf))
       {SC_LOCKON(PD_io_lock_d);
        fh  = (MPI_File *) pf->stream;
        rv = MPI_File_sync(*fh);
        SC_LOCKOFF(PD_io_lock_d);}

    else
       {SC_LOCKON(PD_io_lock_d);
	rv = lio_flush(pf->stream);
        SC_LOCKOFF(PD_io_lock_d);}

    DBG("- _PD_mpflush");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPREAD - do an fread on the parallel file */

static uint64_t _PD_mpread(void *s, size_t nbi, uint64_t ni, FILE *stream)
   {int nread, fstatus;
    size_t nbr, fail;
    PD_Pfile *pf;
    MPI_Offset offs;
    MPI_File *fh;
    MPI_Status status;
    FILE *strm;
    SC_THREAD_ID(tid);

    DBG("+ _PD_mpread");

    pf = GET_PFILE(stream);

    fail = 0;

    if (pf == NULL)
       nread = fail;

    else
       {if (IS_MPI_IO(pf))
           {SC_LOCKON(PD_io_lock_d);

            fh   = (MPI_File *) pf->stream;
            offs = DISK(tid, pf);
            MPI_File_read_at(*fh, offs, s, nbi*ni, MPI_BYTE, &status);

/* number read (nread) may overflow since this is an int */
            MPI_Get_count(&status, MPI_BYTE, &nread);    
            nbr = ni;

            SC_LOCKOFF(PD_io_lock_d);}

        else
           {SC_LOCKON(PD_io_lock_d);

            offs    = DISK(tid, pf);
            strm    = pf->stream;
            fstatus = lio_seek(strm, (long) offs, SEEK_SET);
 
            if (fstatus == -1)
               nbr = fail;
            else
               nbr = lio_read(s, nbi, ni, strm);
                
            SC_LOCKOFF(PD_io_lock_d);}

        DISK(tid, pf) += nbi*ni;}

    DBG("- _PD_mpread");

    return(nbr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPWRITE - do an fwrite on the parallel file */

static uint64_t _PD_mpwrite(void *s, size_t nbi, uint64_t ni, FILE *stream)
   {int nwritten, fstatus;
    long nbw;
    size_t fail;
    PD_Pfile *pf;
    MPI_Offset offs;
    MPI_File *fh;
    MPI_Status status;
    SC_THREAD_ID(tid);

    DBG("+ _PD_mpwrite");

    pf = GET_PFILE(stream);

    fail = 0;

    if ((pf == NULL) || (nbi*ni == 0))
       nwritten = fail;

    else
       {if (IS_MPI_IO(pf))
           {SC_LOCKON(PD_io_lock_d);

            fh   = (MPI_File *) pf->stream;
            offs = DISK(tid, pf);
            MPI_File_write_at(*fh, offs, s, nbi*ni, MPI_BYTE, &status);
            MPI_Get_count(&status, MPI_BYTE, &nwritten);
            nbw = ni;

            SC_LOCKOFF(PD_io_lock_d);}

        else
           {SC_LOCKON(PD_io_lock_d);

            fstatus = lio_seek(pf->stream, DISK(tid, pf), SEEK_SET);
 
            if (fstatus == -1)
               nbw = fail;
            else
               nbw = lio_write(s, nbi, ni, pf->stream);
                
            SC_LOCKOFF(PD_io_lock_d);}

        DISK(tid, pf) += nbi*ni;}

    DBG("- _PD_mpwrite");

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPWRITEC - do an write on the parallel file
 *              - this used the collective MPI-IO call
 */

static uint64_t _PD_mpwritec(void *s, size_t nbi, uint64_t ni, FILE *stream)
   {int nwritten, fstatus;
    long nbw;
    size_t fail;
    PD_Pfile *pf;
    MPI_Offset offs;
    MPI_File *fh;
    MPI_Status status;
    SC_THREAD_ID(tid);

    DBG("+ _PD_mpwritec");

    pf = GET_PFILE(stream);

    fail = 0;

    if ((pf == NULL) || (nbi*ni == 0))
       nwritten = fail;

    else
       {if (IS_MPI_IO(pf))
           {SC_LOCKON(PD_io_lock_d);

            fh   = (MPI_File *) pf->stream;
            offs = DISK(tid, pf);
            MPI_File_seek(*fh, offs, MPI_SEEK_SET);
            MPI_File_write_all(*fh, s, nbi*ni, MPI_BYTE, &status);
            MPI_Get_count(&status, MPI_BYTE, &nwritten);
            nbw = ni;

            SC_LOCKOFF(PD_io_lock_d);}
        else
           {SC_LOCKON(PD_io_lock_d);

            fstatus = lio_seek(pf->stream, DISK(tid, pf), SEEK_SET);
 
            if (fstatus == -1)
               nbw = fail;
            else
               nbw = lio_write(s, nbi, ni, pf->stream);
                
            SC_LOCKOFF(PD_io_lock_d);}

        DISK(tid, pf) += nbi*ni;}

    DBG("- _PD_mpwritec");

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPPRINTF - do an fprintf on the parallel file */

static int _PD_mpprintf(FILE *stream, char *fmt, va_list a)
   {int rv;
    inti ni, nw;
    char *bf;

    nw = 0;
    
    if (stream != NULL)
       {bf = SC_vdsnprintf(FALSE, fmt, a);
        ni = strlen(bf);
        nw = _PD_mpwrite(bf, (size_t) 1, ni, stream);}

    rv = nw;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPPUTS - do an fputs on the parallel file */

static int _PD_mpputs(const char *s, FILE *stream)
   {int nwritten, fstatus, fail;
    inti ni;
    PD_Pfile *pf;
    MPI_Offset offs;
    MPI_File *fh;
    MPI_Status status;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = 0;

    if (pf == NULL)
       nwritten = fail;

    else
       {if (IS_MPI_IO(pf))
           {SC_LOCKON(PD_io_lock_d);

            ni   = strlen(s);
            fh   = (MPI_File *) pf->stream;
            offs = DISK(tid, pf);
            MPI_File_write_at(*fh, offs, (void *) s, ni, MPI_BYTE, &status);
            MPI_Get_count(&status, MPI_BYTE, &nwritten);

            SC_LOCKOFF(PD_io_lock_d);}
        else
           {SC_LOCKON(PD_io_lock_d);

            fstatus = lio_seek(pf->stream, DISK(tid, pf), SEEK_SET);
   
            if (fstatus == -1)
               nwritten = fail;
            else
               nwritten = lio_puts(s, pf->stream);
                
            SC_LOCKOFF(PD_io_lock_d);};

        DISK(tid, pf) += nwritten;}

    return(nwritten);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPGETS - do an fgets on the parallel file */

static char *_PD_mpgets(char *s, int nc, FILE *stream)
   {int nread, nbr, fstatus;
    char *ps, *fail;
    MPI_Offset offs;
    MPI_File *fh;
    MPI_Status status;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = NULL;
    ps   = NULL;

    if (pf == NULL)
       ps = fail;

    else
       {if (IS_MPI_IO(pf))
           {SC_LOCKON(PD_io_lock_d);
            fh   = (MPI_File *) pf->stream;
            offs = DISK(tid, pf);
            MPI_File_read_at(*fh, offs, s, nc, MPI_BYTE, &status);
            MPI_Get_count(&status, MPI_BYTE, &nread);    

            if (nread > 0)
               {ps = s;
                nbr = nread;
                while ((--nbr > 0) && (*s++ != '\n'));
                *s = '\0';}
            else
                ps = fail; 

            SC_LOCKOFF(PD_io_lock_d);}
        else
           {SC_LOCKON(PD_io_lock_d);

            fstatus = lio_seek(pf->stream, DISK(tid, pf), SEEK_SET);
  
            if (fstatus == -1)
               ps = fail;
            else
               ps = lio_gets(s, nc, pf->stream);
        
            SC_LOCKOFF(PD_io_lock_d);};

        if (ps != NULL)
           {DISK(tid, pf) += strlen(ps);};}

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPGETC - do an fgetc on the parallel file */

static int _PD_mpgetc(FILE *stream)
   {int c, fail, fstatus, nread;
    unsigned char in;
    MPI_Offset offs;
    MPI_File *fh;
    MPI_Status status;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = EOF;

    if (pf == NULL)
       c = fail;

    else
       {if (IS_MPI_IO(pf))
           {SC_LOCKON(PD_io_lock_d);

            fh = (MPI_File *) pf->stream;
            offs = DISK(tid, pf);
            MPI_File_read_at(*fh, offs, &in, 1, MPI_BYTE, &status);
            MPI_Get_count(&status, MPI_BYTE, &nread);

            if (nread == 1)
               c = (int)in;
            else
               c = fail;   

            SC_LOCKOFF(PD_io_lock_d);}
        else
           {SC_LOCKON(PD_io_lock_d);
            fstatus = lio_seek(pf->stream, DISK(tid, pf), SEEK_SET);
 
            if (fstatus == -1)
	       c = fail;
            else 
               {c = lio_getc(pf->stream);
                nread = 1;};

            SC_LOCKOFF(PD_io_lock_d);};

        if (c != fail)
           DISK(tid, pf) += nread;};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPUNGETC - do an fungetc on the parallel file */

static int _PD_mpungetc(int c, FILE *stream)
   {int fstatus, fail;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    pf = GET_PFILE(stream);

    fail = 0;

    if (pf == NULL)
       c = fail;

    else
       {if (!(IS_MPI_IO(pf)))
           {SC_LOCKON(PD_io_lock_d);
            fstatus = lio_seek(pf->stream, DISK(tid, pf), SEEK_SET);
 
            if (fstatus == -1)
               c = fail;
            else
               c = lio_ungetc(c, pf->stream);
                
            SC_LOCKOFF(PD_io_lock_d);};

        DISK(tid, pf) -= 1;};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MPOPEN - set up the file for mp parallel i/o
 *            - Since MPI_File_open requires a communicator it will be
 *            - accessed through an extern variable to avoid changing
 *            - the standard PFfopen signature
 */

static FILE *_PD_mpopen(char *name, char *mode)
   {int i, mpi_mode, err;
    PD_Pfile *pf;
    PD_smp_state *pa;
    SC_address *fa;
    FILE *fp, *fpstream;
    MPI_File *fh;
    SC_communicator file_comm, comm;
    MPI_Group comm_group;
    MPI_Comm mcomm, gcomm;
    file_io_desc *fid;

    pa = _PD_get_state(-1);

    comm = pa->communicator;

/* initialize file_comm to something - thanks to ZeroFault for the USTKR */
    file_comm = comm;

    fpstream = NULL;
    fh       = NULL;

/* do not open anything if name is NULL */
    if (name != NULL)

/* open an MPI file if comm is not NULL */      
       {if (_PD_is_comm_null(comm) == FALSE)
           {switch (*mode)
               {case 'w' :
                     if (!strcmp(mode, "w+b"))
                        mpi_mode = MPI_MODE_RDWR | MPI_MODE_CREATE;
                     else
                        mpi_mode = MPI_MODE_WRONLY | MPI_MODE_CREATE;
                     break;
                case 'a' :
                     mpi_mode = MPI_MODE_RDWR;
                     break;
                case 'r' :
    	        default  :
                     mpi_mode = MPI_MODE_RDONLY;
                     break;};
    
/* set up a communicator for use on this file so that
 * pdblib can maintain a separate context from the app
 */
            gcomm = (MPI_Comm) comm;
            MPI_Comm_group(gcomm, &comm_group);
            MPI_Comm_create(MPI_COMM_WORLD, comm_group, &mcomm);
    
/* if an error occurs MPI_File_open will call a default error
 * handler rather than returning.  Need to provide our version.
 */
            fh  = CMAKE(MPI_File);
            err = MPI_File_open(mcomm, name, mpi_mode, MPI_INFO_NULL, fh);
            if (err != 0)
               {CFREE(fh);};
    
            file_comm = (SC_communicator) mcomm;}

/* otherwise open a stdc file */      
        else
           {
#ifdef SC_C_STD_BUFFERED_IO
            fpstream = SC_lfopen(name, mode);
#else
	    fpstream = SC_lbopen(name, mode);
#endif
            file_comm = comm;};};
    
    fa = CMAKE_N(SC_address, _PD_nthreads+1);
    for (i = 0; i <= _PD_nthreads; i++)
        fa[i].diskaddr = 0;

/* no buffered I/O for now */
    pf = CMAKE(PD_Pfile);
    pf->stream = (_PD_is_comm_null(comm) == TRUE) ? (FILE *) fpstream : (FILE *) fh;
    pf->f_addr = fa;
    pf->bf     = NULL; 
    pf->comm   = file_comm;
    pf->bf_len = 0L;

    if (_PD_pflst == NULL)
       _PD_pfm_init_d();

    fid          = SC_make_io_desc(pf);
    fid->fopen   = _PD_mpopen;

    fid->lftell  = _PD_mptell;
    fid->lfseek  = _PD_mpseek;
    fid->lfread  = _PD_mpread;
    fid->lfwrite = (_PD.mp_collective == TRUE) ? _PD_mpwritec : _PD_mpwrite;

    fid->setvbuf = _PD_mpsetvbuf;
    fid->fclose  = _PD_mpclose;
    fid->fprintf = _PD_mpprintf;
    fid->fputs   = _PD_mpputs;
    fid->fgetc   = _PD_mpgetc;
    fid->ungetc  = _PD_mpungetc;
    fid->fflush  = _PD_mpflush;
    fid->fgets   = _PD_mpgets;

    fp = (FILE *) fid;
 
    return(fp);}

/*--------------------------------------------------------------------------*/

/*                  PARALLEL FILE MANAGEMENT ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* _PD_PFM_IS_MASTER_D - return TRUE iff it is OK to update the EOD point
 *                     - in FILE
 */

static int _PD_pfm_is_master_d(PDBfile *file)
   {int rv;

    rv = ((_PD.mp_rank == _PD.mp_master_proc) || (file->mpi_file == FALSE));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_ADD_D - add a file to the list of managed files
 *               - check to see if file is already on the list
 *               - return the ID 
 */

static int _PD_pfm_add_d(PDBfile *file, int64_t start_addr)
   {int i, ins;
    PD_Pfile *pf;
    pfelement *fe;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);

    SC_LOCKON(pflist_lock_d);

/* check if file is already registered */
    for (fe = _PD_pflst->elem, ins = 0; ins < _PD_pflst->len; fe++, ins++)
        if ((fe->name != NULL) && (!strcmp(fe->name, file->name))) 
           {SC_LOCKOFF(pflist_lock_d);
            return(ins);}

/* if file is not already registered - register it if enough space */
    if (ins == _PD_pflst->len)
       for (fe = _PD_pflst->elem, ins = 0; ins < _PD_pflst->len; fe++, ins++)
           if (fe->available) 
              break;

/* allocate more entries in _PD_pflst and register the file */
    if (ins == _PD_pflst->len)
       {_PD_pflst->len += PFM_N_INC;
        NREMAKE(_PD_pflst->elem, pfelement, _PD_pflst->len);

        fe = &_PD_pflst->elem[ins];
        for (i = ins; i < _PD_pflst->len; fe++, i++)
	    {fe->available = TRUE;
	     fe->addr      = 0;
             fe->nflushed  = 0;
             fe->file      = NULL;
             fe->name      = NULL;};}
                    
    _PD_pflst->elem[ins].available = FALSE;
    _PD_pflst->elem[ins].file      = file;
    _PD_pflst->elem[ins].addr      = start_addr;
    _PD_pflst->elem[ins].name      = CSTRSAVE(file->name);
    pf->id = ins;

    SC_LOCKOFF(pflist_lock_d);

    return(pf->id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_ADD_FILE_D - add a file to the list of managed files 
 *                    - IGNORES start_addr argument (why?)
 */

static void _PD_pfm_add_file_d(PDBfile *file, int64_t start_addr)
   {PD_Pfile *pf;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);

    pf->mp_id = _PD_pfm_add_d(file, (int64_t) 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_REMOVE_FILE_D - remove the given file from
 *                       - the list of managed files
 */

static void _PD_pfm_remove_file_d(FILE *fp)
   {int id;
    PD_Pfile *pf;

    pf = GET_PFILE(fp);
    id = pf->id;

    SC_LOCKON(pflist_lock_d);

    _PD_pflst->elem[id].available = TRUE; 
    _PD_pflst->elem[id].addr      = 0;
    _PD_pflst->elem[id].nflushed  = 0;
    _PD_pflst->elem[id].file      = NULL;
    CFREE(_PD_pflst->elem[id].name);

    SC_LOCKOFF(pflist_lock_d);
 
    _PD_rl_pfile(pf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_INIT_D - initialize the file manager for parallel settings */

static void _PD_pfm_init_d(void)
   {int i;
    pfelement *pf;

    SC_LOCKON(pflist_lock_d);

    if (_PD_pflst == NULL)
       {pf = CPMAKE_N(pfelement, PFM_N_FILES, 3);

        _PD_pflst       = CPMAKE(pflist, 3);
	_PD_pflst->elem = pf;
        _PD_pflst->len  = PFM_N_FILES;

        for (pf = _PD_pflst->elem, i = 0; i < _PD_pflst->len; i++, pf++)
	    {pf->available = TRUE;
	     pf->addr      = 0;
	     pf->nflushed  = 0;
	     pf->file      = NULL;
	     pf->name      = NULL;};};

    SC_LOCKOFF(pflist_lock_d);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_BUFFER_MP_REQ - write a request message into a buffer */

static long _PD_pfm_buffer_mp_req(char *type, void *request, char **buffer)
   {long nb;
    PDBfile *outf;

    DBG("+ _PD_pfm_buffer_mp_req");

/* determine size necessary for buffer */
    nb = PD_sizeof(PD_pvif, type, 1, request);

/* allocate the buffer */
    *buffer = CMAKE_N(char, nb);

/* write to the buffer */
    outf = PN_open(PD_pvif, *buffer);
    PN_write(outf, type, 1, request);
    PN_close(outf);

    DBG("- _PD_pfm_buffer_mp_req");

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_GETSPACE_AUX - return the next disk address available for writing
 *                      - and increment it to reserve space in threaded mode
 */

static int64_t _PD_pfm_getspace_aux(int id, size_t nbytes, int mpi_flag)
   {long nbuf;
    int64_t rv;
    char *mpbuf;
    PD_MP_GETSPACE_req space_req;
    MPI_Comm comm;
    MPI_Status stat;
    
    DBG("+ _PD_pfm_getspace_aux");

    if ((_PD.mp_rank == _PD.mp_master_proc) || !mpi_flag)
       {
#ifndef USE_THREAD_SAFE_MPI
        if (mpi_flag)
           _PD_pfm_service_req(id, PD_MP_GETSPACE);
#endif
        SC_LOCKON(pflist_lock_d);

        rv = _PD_pflst->elem[id].addr;

        if (nbytes > 0)
            _PD_pflst->elem[id].addr += nbytes;

        SC_LOCKOFF(pflist_lock_d);}

    else
       {space_req.id     = id;
        space_req.nbytes = nbytes;

        nbuf = _PD_pfm_buffer_mp_req("PD_MP_GETSPACE_req", &space_req, &mpbuf); 
	comm = (MPI_Comm) _PD.mp_comm;

        MPI_Send(mpbuf, nbuf, MPI_BYTE, _PD.mp_master_proc,
                 PD_MP_GETSPACE, comm);
        MPI_Recv(&rv, sizeof(int64_t), 
                 MPI_BYTE, _PD.mp_master_proc,
                 PD_MP_GETSPACE, comm, &stat);

        CFREE(mpbuf);}

    DBG("- _PD_pfm_getspace_aux");

    return(rv);} 

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_GETSPACE_COL - return the next disk address available for writing
 *                      - and increment it to reserve space in threaded mode
 *                      - this is the collective getspace call to get a
 *                      - a reference to the same space across all participating
 *                      - processes
 */

static int64_t _PD_pfm_getspace_col(PDBfile *file, size_t nbytes)
   {int64_t rv;
    PD_Pfile *pf;
    MPI_Comm comm;
    SC_THREAD_ID(tid);
    
    DBG("+ _PD_pfm_getspace_col");

    pf = FILE_IO_INFO(PD_Pfile, file->stream);

    if (_PD_pfm_is_master_d(file))
       rv = _PD_pfm_getspace_aux(pf->id, nbytes, FALSE);

    if (file->mpi_file)
       {comm = (MPI_Comm) _PD.mp_comm;
	MPI_Bcast(&rv, sizeof(int64_t), MPI_BYTE, _PD.mp_master_proc, comm);};
    
    DISK(tid, pf) = rv;

    DBG("- _PD_pfm_getspace_col");

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_GETSPACE_D - return the next disk address available for writing
 *                    - and increment it to reserve space in threaded mode
 */

static int64_t _PD_pfm_getspace_d(PDBfile *file, size_t nbytes,
				int rflag, int colf)
   {int id;
    int64_t rv;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);

    DBG("+ _PD_pfm_getspace_d");

    if (colf)
       {rv = _PD_pfm_getspace_col(file, nbytes);}

    else
       {pf = FILE_IO_INFO(PD_Pfile, file->stream);
	id = pf->id;
	rv = _PD_pfm_getspace_aux(id, nbytes, rflag);

	DISK(tid, pf) = rv;};

    DBG("- _PD_pfm_getspace_d");

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                    PFM REQUEST HANDLING API                              */

/*--------------------------------------------------------------------------*/

/* _PD_MP_GET_MESSAGE - get an mpi message, return message in buffer
 *                      return sending process in sender
 *                      return value is tag
 *
 */

static int _PD_pfm_get_msg(char **buf, int *length, int *sender)
   {MPI_Status stat;
    MPI_Comm comm;

    DBG("+ _PD_pfm_get_msg");

    comm = (MPI_Comm) _PD.mp_comm;

    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &stat);
    MPI_Get_count(&stat, MPI_BYTE, length);

    *buf = CMAKE_N(char, *length);

    MPI_Recv(*buf, *length, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG,
             comm, &stat);

    *sender = stat.MPI_SOURCE;

    DBG("- _PD_pfm_get_msg");

    return(stat.MPI_TAG);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_REMOTE_CLOSE - do what's necessary to close the remote file */

static int _PD_pfm_remote_close(char *buf)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_REMOTE_SHUTDOWN - do what's necessary to shutdown
 *                         - the remote file
 */

static int _PD_pfm_remote_shutdown(void)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_REMOTE_GETSPACE - process a PD_MP_GETSPACE message contained
 *                         - in the input buffer
 */

static int64_t _PD_pfm_remote_getspace(char *buf)
   {int64_t rv;
    PDBfile *inf;
    PD_MP_GETSPACE_req req;

    DBG("+ _PD_pfm_remote_getspace");

    inf = PN_open(PD_pvif, buf);
    PN_read(inf, "PD_MP_GETSPACE_req", 1, &req);
    PN_close(inf);

    rv = _PD_pfm_getspace_aux(req.id, (size_t)req.nbytes, FALSE);

    DBG("- _PD_pfm_remote_getspace");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_REMOTE_FLUSH - do a flush on another CPU */

static int _PD_pfm_remote_flush(char *buf)
   {int rv;
    PDBfile *inf, *file;
    FILE *fp;
    PD_MP_FLUSH_req req;

    DBG("+ _PD_pfm_remote_flush");

    inf = PN_open(PD_pvif, buf);
    PN_read(inf, "PD_MP_FLUSH_req", 1, &req);
    PN_close(inf);

    file = _PD_pflst->elem[req.id].file;

    rv = (*file->parse_symt)(file, req.buf, TRUE, "*", NULL);
    SC_ASSERT(rv == TRUE);

/* if everyone has checked in actually flush the file to disk */
    if (++_PD_pflst->elem[req.id].nflushed == (_PD_pflst->elem[req.id].nprocs - 1))
       {rv = (*file->flush)(file);
        _PD_pflst->elem[req.id].nflushed = 0;

        fp = file->stream;
        if (lio_flush(fp))
           PD_error("FFLUSH FAILED _PD_pfm_remote_flush", PD_WRITE);}

    CFREE(req.buf);
    
    DBG("- _PD_pfm_remote_flush");

    return(req.id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_COMPLETE_REQUEST - actually complete service requests
 *                          - for disk resources 
 */

static int _PD_pfm_complete_request(PD_MP_request req, MPI_Comm comm)
   {int rv, id, done, shutdown;
    int64_t space;
    MPI_Request request;

    done     = 0;
    shutdown = 1;
    rv       = 0;

    DBG("+ _PD_pfm_complete_request");

    switch (req.type)
       {case PD_MP_GETSPACE :
             DBG("of type PD_MP_GETSPACE");
             space = _PD_pfm_remote_getspace(req.buf);
             MPI_Send(&space, sizeof(int64_t), MPI_BYTE, req.sender, PD_MP_GETSPACE, comm);
             break; 
             
        case PD_MP_FLUSH :

/* add symbol table entries from remote processes until everyone
 * has checked in, then it does a flush to the file
 */
             DBG("of type PD_MP_FLUSH");
             id = _PD_pfm_remote_flush(req.buf);
             MPI_Send(&id, 1, MPI_INT, req.sender, PD_MP_FLUSH, comm);
             break;

        case PD_MP_CLOSE :
             DBG("of type PD_MP_CLOSE");
             rv = _PD_pfm_remote_close(req.buf);
             MPI_Send(&done, 1, MPI_INT, req.sender, PD_MP_CLOSE, comm);
             break;

        case PD_MP_SHUTDOWN :

/* clean up and shut down the master process */
             DBG("of type PD_MP_SHUTDOWN");
             rv = _PD_pfm_remote_shutdown();
             MPI_Isend(&shutdown, 1, MPI_INT, req.sender, PD_MP_SHUTDOWN, comm, &request);

             if (++_PD.nterminated == (_PD.mp_size - 1))
                rv = PFM_TERM_MASTER;
             break;

        default :
             DBG("received unrecognized message");
             break;}

    if (req.buf != NULL)
       {CFREE(req.buf);
        req.buf = NULL;}

    DBG("- _PD_pfm_complete_request");

    return(rv);}
       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SERVICE_REQ - service requests for disk resources in a
 *                     - non-threaded environment
 */

static void _PD_pfm_service_req(int id, int request)
   {int n_expected, n_received, i;
    MPI_Comm comm;
    PD_MP_request req, *pend_req;

    pend_req   = CMAKE_N(PD_MP_request, _PD.mp_size);
    n_expected = _PD.mp_size - 1;
    n_received = 0;

    comm = (MPI_Comm) _PD.mp_comm;

    DBG2("%d: + _PD_pfm_service_req for req type %d\n", _PD.mp_rank, request);

/* receive all requests before servicing them */
    while (n_received < n_expected)
       {req.buf  = NULL;
        req.type = _PD_pfm_get_msg(&req.buf, &req.len, &req.sender);
            
        if (req.type != request)
           {DBG2("%d: unexpected msg %d\n", _PD.mp_rank, req.type);
            DBG2("%d: from process %d (error)\n", _PD.mp_rank, req.sender);}
        else
           {DBG2("%d: msg from %d\n", _PD.mp_rank, req.sender);
            n_received++;
            pend_req[req.sender] = req;};};

/* service all requests we received */
    for (i = 1 ; i < _PD.mp_size ; i++ )
        {if (pend_req[i].type == request)
            {DBG2("%d: servicing msg from %d\n", _PD.mp_rank, pend_req[i].sender);
             _PD_pfm_complete_request(pend_req[i], comm);};}

    DBG2("%d: - _PD_pfm_service_req for req type %d\n", _PD.mp_rank, request);

    CFREE(pend_req);

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _PD_PFM_MASTER_THREAD - control thread for mpi file management
 *                       - in threaded environment
 */

static void *_PD_pfm_master_thread(void *x)
   {int rv;
    PD_MP_request req;
    MPI_Comm comm;

    DBG("+ _PD_pfm_master_thread");
    comm = (MPI_Comm) _PD.mp_comm;

/* go into message handling loop */
    while (1)
       {req.type = _PD_pfm_get_msg(&req.buf, &req.len, &req.sender);

        DBG2("%d: _PD_pfm_master_thread got msg from %d\n",
	     _PD.mp_rank, req.sender);

        rv = _PD_pfm_complete_request(req, comm);

        if (rv == PFM_TERM_MASTER)
           {DBG("- _PD_pfm_master_thread");
            return(NULL);};};}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                       PARALLEL FILE MANAGER API                          */

/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETUP_FILE_D - convert/wrap a BF_FILE FB, to a 
 *                      - FILE and return it
 */

static FILE *_PD_pfm_setup_file_d(BF_FILE *fb)
   {FILE *rf;
    PD_Pfile *pf;

    pf         = (PD_Pfile *) _PD_mpopen(NULL, NULL);
    pf->stream = fb;
    rf         = (FILE *) pf;

    return(rf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETUP_MP_FILE_D - setup a PDBfile to be MP */

static void _PD_pfm_setup_mp_file_d(PDBfile *file, SC_communicator comm)
   {int nprocs;
    PD_Pfile *pf;
    MPI_Comm mcomm, fcomm;

    DBG("+ _PD_pfm_setup_mp_file_d");

    pf    = FILE_IO_INFO(PD_Pfile, file->stream);
    mcomm = (MPI_Comm) _PD.mp_comm;
    fcomm = (MPI_Comm) pf->comm;

    if (_PD_is_comm_self(comm) == FALSE)
       {file->mpi_file = TRUE;
        MPI_Bcast(&pf->mp_id, 1, MPI_INT, _PD.mp_master_proc, mcomm);
        MPI_Comm_size(fcomm, &nprocs);}
    else
       nprocs = 1;
       
    _PD_pflst->elem[pf->mp_id].nprocs = nprocs;

    DBG("- _PD_pfm_setup_mp_file_d");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_NEXT_ADDRESS_D - reserve the appropriate amount of space in FILE
 *                    - this a worker for _PD_get_next_address
 */

static int64_t _PD_next_address_d(PDBfile *file, char *type, long number,
				void *vr, int seekf, int tellf, int colf)
   {int64_t addr;

    DBG("+ _PD_next_address_d");

    addr = _PD_next_address_t(file, type, number, vr, seekf, tellf, colf);

    DBG("- _PD_next_address_d");

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_STREAM_D - get the FILE from the FILE */

static FILE *_PD_get_file_stream_d(PDBfile *file)
   {FILE *fp;
    PD_Pfile *pf;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    fp = (pf != NULL) ? (FILE *) pf->stream : NULL;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_PTR_D - get the BF_FILE from the FILE */

static BF_FILE *_PD_get_file_ptr_d(FILE *file)
   {BF_FILE *fb;
    PD_Pfile *pf;

    pf = GET_PFILE(file);
    fb = (pf != NULL) ? (BF_FILE *) pf->stream : NULL;

    return(fb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FILE_SIZE_D - return the file size */

static int64_t _PD_get_file_size_d(PDBfile *file)
   {int status;
    int64_t rv;
    MPI_Offset sz;
    PD_Pfile *pf;
    MPI_File *fp;

    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    fp = (pf != NULL) ? (MPI_File *) pf->stream : NULL;

    status = MPI_File_get_size(*fp, &sz);
    SC_ASSERT(status == 0);

/* is there a portable way to check the return value (status)? */

    rv = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_EXTEND_FILE_D - extend the file by the given number of bytes */

static int _PD_pfm_extend_file_d(PDBfile *file, long nb)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_EOD_D - reset the EOD point in the file */

static int _PD_set_eod_d(PDBfile *file, int64_t addr, long nb)
   {

    file->chrtaddr = addr;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_MARK_AS_FLUSHED_D - remember that the file is flushed */

static void _PD_pfm_mark_as_flushed_d(PDBfile *file, int wh)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SERIAL_FLUSH_D - do a serial flush */

static int _PD_pfm_serial_flush_d(FILE *fp, int tid)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MP_FLUSH - handle flushing the tables to the remote server and
 *              - wait until everyone has flushed before continuing
 */

static int _PD_mp_flush(PDBfile *file)
   {int nbuf, id, len, i, rv;
    char *buf, *mpbuf, *symtab_buf;
    PD_Pfile *pf;
    PD_MP_FLUSH_req flush_req;
    MPI_Comm mcomm, fcomm;
    MPI_Status stat;
    FILE *fp;

    DBG("+ _PD_mp_flush");

/* have everybody flush before sending out the tables */
    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    fp = file->stream;

    lio_flush(fp);

/* we can be doing a flush unrelated to a write or close
 * and this could go nowhere - e.g. tpddp
    if (lio_flush(fp) != 0)
       PD_error("FLUSH FAILED - _PD_MP_FLUSH", PD_WRITE);
*/
    mcomm = (MPI_Comm) _PD.mp_comm;
    fcomm = (MPI_Comm) pf->comm;
    id    = pf->mp_id;

    if (_PD.mp_rank != _PD.mp_master_proc)
       {flush_req.id = id;

        file->symtaddr = (*file->wr_symt)(file);
        _PD_cp_tbuffer(&buf);

        flush_req.buf = buf;
        nbuf = _PD_pfm_buffer_mp_req("PD_MP_FLUSH_req", &flush_req, &mpbuf);

        MPI_Send(mpbuf, nbuf, MPI_BYTE, _PD.mp_master_proc,
		 PD_MP_FLUSH, mcomm);

        CFREE(buf);
        CFREE(mpbuf);

/* have everybody except master process flush again after writing
 * the tables master process will flush when the tables are flushed
 * this will ensure that tables are flushed before proceeding
 */
        if (lio_flush(fp) != 0)
           PD_error("FLUSH FAILED - _PD_MP_FLUSH", PD_WRITE);

        MPI_Recv(&rv, 1, MPI_INT, _PD.mp_master_proc, PD_MP_FLUSH,
		 mcomm, &stat);

/* sync the in-memory symbol table amongst all processes */
        MPI_Recv(&len, 1, MPI_INT, _PD.mp_master_proc, PD_MP_SYNC,
		 mcomm, &stat);
        symtab_buf = CMAKE_N(char, len); 

        MPI_Bcast(symtab_buf, len, MPI_BYTE, _PD.mp_master_proc, mcomm);
        (*file->parse_symt)(file, symtab_buf, TRUE, "*", NULL);

        CFREE(symtab_buf);}

    else
       {
#ifndef USE_THREAD_SAFE_MPI
        _PD_pfm_service_req(id, PD_MP_FLUSH);
#endif

/* sync the in-memory symbol table amongst all processes */
        SC_LOCKON(pflist_lock_d);
        file = _PD_pflst->elem[id].file;

	(*file->wr_symt)(file);

        len = _PD_cp_tbuffer(&symtab_buf);
        SC_LOCKOFF(pflist_lock_d);
   
        for (i = 1 ; i < _PD.mp_size ; i++)
            {MPI_Send(&len, 1, MPI_INT, i, PD_MP_SYNC, mcomm);}

        MPI_Bcast(symtab_buf, len, MPI_BYTE, _PD.mp_master_proc, mcomm);
    
        CFREE(symtab_buf);};

/* wait until everybody has flushed */
    MPI_Barrier(fcomm);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_FLUSH_FILE_D - flush the given file */

static int _PD_pfm_flush_file_d(PDBfile *file)
   {int rv;

    if ((!file->mpi_file) || (_PD.mp_size < 2))
       rv = (*file->flush)(file);
    else
       rv = _PD_mp_flush(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PFM_SETADDR_D - set the next available address for writing to ADDR */

static void _PD_pfm_setaddr_d(PDBfile *file, int64_t addr)
   {int id;
    PD_Pfile *pf;
    SC_THREAD_ID(tid);
    
    pf = FILE_IO_INFO(PD_Pfile, file->stream);
    id = pf->id;

    SC_LOCKON(pflist_lock_d);
    _PD_pflst->elem[id].addr = addr;
    SC_LOCKOFF(pflist_lock_d);

    DISK(tid, pf) = addr;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void _PD_init_pvif(char *name)
   {

    PD_pvif = PD_open_vif(name);

/* define the necessary structures */

/* PDBLib types */
    PD_def_pdb_types(PD_pvif);

    PD_defstr(PD_pvif, "PD_MP_ADD_req",
              "char *fname",
	      "long_long start_addr",
              LAST);

    PD_defstr(PD_pvif, "PD_MP_GETSPACE_req",
              "int id",
              "long nbytes",
              LAST);

    PD_defstr(PD_pvif, "PD_MP_DEFENT_req",
              "int id",
              "syment *ep",
              LAST);

    PD_defstr(PD_pvif, "PD_MP_FLUSH_req",
              "int id",
              "char *buf",
              LAST);

    PD_defstr(PD_pvif, "PD_MP_DEFSTR_req",
              "int id",
              "defstr *dp",
              LAST);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_COLLECTIVE - set parallel io hooks for collective or
 *                    - non-collective ops
 */

int _PD_set_collective(int flag)
   {

    _PD.mp_collective = flag;

    return(flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_D - initialize the library for DP */

int _PD_init_d(void)
   {

    PD_par_fnc.open_hook         = _PD_mpopen;

    PD_par_fnc.init              = _PD_pfm_init_d;
    PD_par_fnc.add_file          = _PD_pfm_add_file_d;
    PD_par_fnc.remove_file       = _PD_pfm_remove_file_d;
    PD_par_fnc.getspace          = _PD_pfm_getspace_d;

    PD_par_fnc.is_dp_init        = _PD_is_dp_init_d;
    PD_par_fnc.is_smp_init       = _PD_is_smp_init_d;
    PD_par_fnc.is_sequential     = _PD_is_sequential_d;
    PD_par_fnc.is_null_fp        = _PD_is_null_fp_d;
    PD_par_fnc.is_master         = _PD_pfm_is_master_d;

    PD_par_fnc.setup_pseudo_file = _PD_pfm_setup_file_d;
    PD_par_fnc.setup_mp_file     = _PD_pfm_setup_mp_file_d;
    PD_par_fnc.get_file_size     = _PD_get_file_size_d;
    PD_par_fnc.get_file_stream   = _PD_get_file_stream_d;
    PD_par_fnc.get_file_ptr      = _PD_get_file_ptr_d;
    PD_par_fnc.next_address      = _PD_next_address_d;
    PD_par_fnc.extend_file       = _PD_pfm_extend_file_d;
    PD_par_fnc.flush_file        = _PD_pfm_flush_file_d;
    PD_par_fnc.serial_flush      = _PD_pfm_serial_flush_d;
    PD_par_fnc.set_eod           = _PD_set_eod_d;
    PD_par_fnc.mark_as_flushed   = _PD_pfm_mark_as_flushed_d;
    PD_par_fnc.set_address       = _PD_pfm_setaddr_d;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMM_CONNECT_MPI - connect up the MPI communicators
 *                      - this has to be done in the file which includes
 *                      - mpi.h rather than in pdcomm.c where these
 *                      - variables are defined because OpenMPI uses
 *                      - pointers instead of constants which in turn
 *                      - would results in unresolved externals for
 *                      - non-MPI applications
 */

static void _PD_comm_connect_mpi(void)
   {

    PD_COMM_NULL  = (SC_communicator) MPI_COMM_NULL;
    PD_COMM_WORLD = (SC_communicator) MPI_COMM_WORLD;
    PD_COMM_SELF  = (SC_communicator) MPI_COMM_SELF;

    _PD_initialize_comm(TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INIT_MPI - initialize the library for distributed parallel i/o
 *             -
 *             - if called by masterproc
 *             -    spawn master control process which will:
 *             -          initialize disk space manager
 *             -          set up symbol table manager
 *             -          go into a message loop to process requests
 *             -             from other processes
 *             - Have to initialize threads here because we need to
 *             - add 1 for the master process
 *
 * #bind PD_init_mpi fortran() scheme() python()
 */

int PD_init_mpi(int masterproc, int nthreads, PFTid tid)
   {MPI_Comm world, mcomm;
    MPI_Group world_group;

    if (!_PD.mp_initialized)
       {_PD.mp_initialized = TRUE;

	_PD_comm_connect_mpi();

/* get number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &_PD.mp_size);

/* get my rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &_PD.mp_rank);
	_PD.mp_master_proc = masterproc;

	SC_set_processor_info(_PD.mp_size, _PD.mp_rank);

/* set up separate library communication context */
	if ((_PD.mp_master_proc >= 0) && (_PD.mp_size > 1))
	   {world = MPI_COMM_WORLD;
            mcomm = (MPI_Comm) _PD.mp_comm;

	    MPI_Comm_group(world, &world_group);
	    MPI_Comm_create(world, world_group, &mcomm);

            _PD.mp_comm = (SC_communicator) mcomm;};

/* initialize threads */
	nthreads = max(1L, nthreads);

#ifdef USE_THREAD_SAFE_MPI
	if (_PD.mp_rank == masterproc)
	   nthreads += 1;
#endif
          
	PD_init_threads(nthreads, tid);

/* initialize the io hooks */
	_PD_init_state(TRUE); /* REDUNDANT (PD_init_threads does this) */
	_PD_init_d();

/* initialize the PFM */
        _PD_pfm_init_d();

	if (_PD.mp_size > 1)
	   _PD_init_pvif("_PD_pbuffer");

#ifdef USE_THREAD_SAFE_MPI
	if ((_PD.mp_rank == masterproc) && (_PD.mp_size > 1))
	   {int pret;

	    pret = SC_THREAD_CREATE(_PD_mp_master_thread, _PD_thread_attr,
				    _PD_pfm_master_thread, NULL);
	    if (pret != 0)
	       {printf("THREAD CREATE failed, pret = %d\n", pret);
		return(FALSE);}
	    else
	       DBG("pthread_create succeeded");}
#endif
       };

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TERM_MPI - shutdown the library--terminate the master thread
 *
 * #bind PD_term_mpi fortran() scheme() python()
 */

void PD_term_mpi(void)
   {

#ifdef USE_THREAD_SAFE_MPI 
    {int done, rv;
     MPI_Status stat;
     MPI_Comm mcomm;

     mcomm = (MPI_Comm) _PD.mp_comm;

     if (_PD.mp_rank != _PD.mp_master_proc)
        {done = TRUE;

	 MPI_Send(&done, 1, MPI_INT, _PD.mp_master_proc,
		  PD_MP_SHUTDOWN, mcomm);
	 MPI_Recv(&rv, 1, MPI_INT, _PD.mp_master_proc,
		  MPI_ANY_TAG, mcomm, &stat);}
   
/* wait until everyone finished before returning */
     MPI_Barrier(mcomm);};
#endif

/* close the virtual internal file */
    if (_PD.mp_size > 1)
       PD_close(PD_pvif);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

