/*
 * SCBIO.C - I/O hooks suitable for buffered I/O
 *         - since many modern C implementations make setvbuf
 *         - a hint not a mandate
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

/* #define DEBUG */
/* #define USE_C_BUFFERED_IO */
#define SC_N_BIO_OPER  6

#define ACCESS(_fp)                                                          \
    bio_desc *bid;                                                           \
    bid = (bio_desc *) (_fp);                                                \
    fp  = bid->fp

#define CONTAINS(_a, _x, _b)	(((_a) <= (_x)) && ((_x) < (_b)))

enum e_bio_kind
   {BIO_READ, BIO_WRITE, BIO_EMPTY};

enum e_bio_oper
   {BIO_OPER_NONE, BIO_OPER_STAT, BIO_OPER_SEEK,
    BIO_OPER_READ, BIO_OPER_WRITE, BIO_OPER_SETVBUF};

typedef enum e_bio_kind bio_kind;
typedef enum e_bio_oper bio_oper;

typedef struct s_bio_desc bio_desc;
typedef struct s_bio_frame bio_frame;

/* NOTE: valid bytes of the buffer are BF[I] where
 *       ADDR <= I < ADDR+SZ
 */

struct s_bio_frame
   {int64_t sz;                         /* size of the frame */
    int64_t nb;                         /* number of working bytes in frame */
    int64_t addr;                       /* starting disk address of frame */
    int flushed;
    bio_kind rw;                       /* has read/write bytes */
    unsigned char *bf;};               /* frame buffer */

struct s_bio_desc
   {int64_t sz;                         /* file size */
    int64_t curr;                       /* current file address */
    int64_t bfsz;                       /* buffer size */
    SC_array *stack;
    int fd;
    FILE *fp;
    long nhr;                          /* number of fread level calls */
    long nhw;                          /* number of fwrite level calls */
    long nbfmx;
    long nhits[SC_N_BIO_OPER];};

static int
 _SC_bio_debug = 0;

static long
 bio_stats[5] = { 0L, 0L, 0L, 0L, 0L };

static int
 _SC_bfr_remove(bio_desc *bid, int i, bio_frame *fr, int fl, int orig);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BIO_DEBUG - set buffering debugging level
 *              - return the old value
 *              - if LVL < 0 do not change the current value
 *              - bit 1 activates read checks
 *              - bit 2 activates write checks
 */

int SC_bio_debug(int lvl)
   {int ol;

    ol = _SC_bio_debug;
    if (lvl >= 0)
       _SC_bio_debug = lvl;

    return(ol);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPBFSTACK - debugging print the list of buffer frames for BID */

void dpbfstack(bio_desc *bid)
   {int i, n;
    long as, ae, nb;
    bio_frame *fa;

    n = SC_array_get_n(bid->stack);
    for (i = 0; i < n; i++)
        {fa = *(bio_frame **) SC_array_get(bid->stack, i);
	 
	 as = fa->addr;
	 ae = as + fa->sz;
	 nb = fa->nb;

	 if (i == 0)
	    printf("  Buffer        Start      Stop  # bytes\n");

	 printf("%8d  %10ld %10ld %8ld\n", i+1, as, ae, nb);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_AUDIT - debugging aid */

static INLINE void _SC_bio_audit(bio_desc *bid, char *tag)
   {

#ifdef DEBUG
    printf(">>> %s to %ld (%ld)\n",
	   tag,
	   (long) lseek(bid->fd, 0, SEEK_CUR),
	   (long) bid->curr);

    bid->nhits[BIO_OPER_SEEK]++;

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_SET_ADDR - update the address and size of BID */

static INLINE int64_t _SC_bio_set_addr(bio_desc *bid, int64_t addr, int chsz,
				     char *tag)
   {

    addr = max(addr, 0);

    bid->curr = addr;
    if ((chsz == TRUE) && (addr > bid->sz))
       bid->sz = addr;

    _SC_bio_audit(bid, tag);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_FREE - free bio_frame */

static void _SC_bfr_free(bio_frame *fr)
   {

    if (fr != NULL)
       {CFREE(fr->bf);
	CFREE(fr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_FREE_ELEM - free buffering */

static int _SC_bfr_free_elem(void *a)
   {bio_frame *fr;

    fr = *(bio_frame **) a;
    _SC_bfr_free(fr);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_FREE - free buffering */

static void _SC_bio_free(bio_desc *bid, int total)
   {

    if (bid->stack != NULL)

/* free the frames and array - no flush */
       {if (total == TRUE)
	   SC_free_array(bid->stack, _SC_bfr_free_elem);

/* free the frames only - flush */
	else
	   {int i, n;
	    bio_frame *fr;

	    n = SC_array_get_n(bid->stack);
	    for (i = 0; i < n; i++)
	        {fr = *(bio_frame **) SC_array_get(bid->stack, i);
		 n  = _SC_bfr_remove(bid, i, fr, TRUE, FALSE);
		 i--;};};};

    return;}

/*--------------------------------------------------------------------------*/

#ifndef USE_C_BUFFERED_IO

static int
 _SC_bio_seek(bio_desc *bid, int64_t offs, int wh);

/*--------------------------------------------------------------------------*/

/* _SC_BFR_MARK - mark a bio_frame FR */

static void _SC_bfr_mark(bio_frame *fr, bio_kind rw, int fl)
   {

    if (fr != NULL)
       {fr->rw      = rw;
	fr->flushed = fl;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_INIT - initialize a buffered io frame */

static void _SC_bfr_init(bio_frame *fr, int64_t addr, size_t bfsz)
   {

    if (fr != NULL)
       {if (fr->sz != bfsz)
	   CFREE(fr->bf);

	if (fr->bf == NULL)
	   fr->bf = CMAKE_N(unsigned char, bfsz);

        memset(fr->bf, 0, bfsz);

	fr->sz   = bfsz;
        fr->nb   = 0;
	fr->addr = addr;

	_SC_bfr_mark(fr, BIO_EMPTY, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_ALLOC - initialize a buffered io frame */

static bio_frame *_SC_bfr_alloc(int64_t addr, size_t bfsz)
   {bio_frame *fr;

    fr = CMAKE(bio_frame);

    SC_MEM_INIT(bio_frame, fr);
    
    _SC_bfr_init(fr, addr, bfsz);

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_BUFFER - initialize buffering */

static void _SC_bio_buffer(bio_desc *bid, size_t bfsz)
   {

    if (bfsz < 1)
       {_SC_bio_free(bid, FALSE);
	SC_free_array(bid->stack, NULL);}

    else
       {bid->bfsz = bfsz;
	if (bid->stack == NULL)
	   bid->stack = CMAKE_ARRAY(bio_frame *, NULL, 0);};

    bid->nhits[BIO_OPER_SETVBUF]++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_INFILL - copy the appropriate parts of FS into FD
 *                - return the number of bytes copied
 */

static int64_t _SC_bfr_infill(bio_frame *fd, bio_frame *fs)
   {int64_t fdi[3], fsi[3], fsr, fdr;
    int64_t nb, ov[2], no[2];
    unsigned char *ps, *pd;

    fsr = fs->nb;
    fdr = fd->nb;

/* decide overlap based on file address */
    fdi[0] = fd->addr + fdr;
    fdi[2] = fd->addr + fd->sz;

    fsi[0] = fs->addr;
    fsi[2] = fs->addr + fs->sz;

/* the overlap is the greater of the starts and the lesser of the ends */
    ov[0] = max(fsi[0], fdi[0]);
    ov[1] = min(fsi[2], fdi[2]);

    no[0] = ov[0] - fs->addr;
    no[1] = ov[0] - fd->addr;

    nb = ov[1] - ov[0];
    if (nb <= 0)
       nb = 0;
    else if (nb > 0)
       {fsi[1] = no[0] + nb;
	fsi[1] = max(fsi[1], fsr);
	fdi[1] = no[1] + nb;
	fdi[1] = max(fdi[1], fdr);

/* do copy based on memory address */
	ps = fs->bf + no[0];
	pd = fd->bf + no[1];
	memcpy(pd, ps, nb);

	fs->nb = fsi[1];
	fd->nb = fdi[1];};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_OUTFILL - copy the appropriate parts of FS into FD
 *                 - return the number of bytes copied
 */

static int64_t _SC_bfr_outfill(bio_frame *fd, bio_frame *fs)
   {int64_t fdi[3], fsi[3], fdw, fsw;
    int64_t nb, ov[2], no[2];
    unsigned char *ps, *pd;

    fsw = fs->nb;
    fdw = fd->nb;

/* decide overlap based on file address */
    fdi[0] = fd->addr;
    fdi[2] = fd->addr + fd->sz;

    fsi[0] = fs->addr + fsw;
    fsi[2] = fs->addr + fs->sz;

/* the overlap is the greater of the starts and the lesser of the ends */
    ov[0] = max(fsi[0], fdi[0]);
    ov[1] = min(fsi[2], fdi[2]);

    no[0] = ov[0] - fs->addr;
    no[1] = ov[0] - fd->addr;

    nb = ov[1] - ov[0];
    if (nb <= 0)
       nb = 0;
    else
       {fsi[1] = no[0] + nb;
	fsi[1] = max(fsi[1], fsw);
	fdi[1] = no[1] + nb;
	fdi[1] = max(fdi[1], fdw);

/* do copy based on memory address */
	ps = fs->bf + no[0];
	pd = fd->bf + no[1];
	memcpy(pd, ps, nb);

	fs->nb = fsi[1];
	fd->nb = fdi[1];

       _SC_bfr_mark(fd, BIO_WRITE, FALSE);};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_REMOVE - free FR and remove it from BID stack
 *                - return the new stack depth
 */

static int _SC_bfr_remove(bio_desc *bid, int i, bio_frame *fr, int fl, int orig)
   {int n, ok, nw;
    int64_t fi[2], ad, nb;
	
    if ((fr->rw == BIO_WRITE) && (fl == TRUE))
       {ad    = 0;
	nb    = fr->nb;
	fi[0] = fr->addr;
	fi[1] = fi[0] + nb;

	if (orig == TRUE)
	   ad = bid->curr;

	ok = _SC_bio_seek(bid, fi[0], SEEK_SET);
	SC_ASSERT(ok >= 0);

/* GOTCHA: does this go past end of file? */
	nw = SC_write_sigsafe(bid->fd, fr->bf, nb);
	SC_ASSERT(nw == nb);

	bid->nhits[BIO_OPER_WRITE]++;

	if (orig == FALSE)
	   {ad = fi[1];
	    _SC_bio_set_addr(bid, ad, TRUE, "flush");}
	else
	   _SC_bio_seek(bid, ad, SEEK_SET);};

    _SC_bfr_free(fr);

    n = SC_array_remove(bid->stack, i);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_CONTAINS - return a stack frame which contains ADDR */

static bio_frame *_SC_bfr_contains(bio_desc *bid, int64_t addr)
   {int i, n;
    int64_t fi[2];
    bio_frame *fr, *rv;

    rv = NULL;
    if (bid->stack != NULL)
       {n = SC_array_get_n(bid->stack);
        for (i = 0; i < n; i++)
	    {fr = *(bio_frame **) SC_array_get(bid->stack, i);
             fi[0] = fr->addr;
	     fi[1] = fi[0] + fr->nb;

	     if (CONTAINS(fi[0], addr, fi[1]))
	        {rv = fr;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_NEXT - return the leftmost stack frame which overlaps FR */

static bio_frame *_SC_bfr_next(bio_desc *bid, bio_frame *fr)
   {int i, n, imn;
    int64_t fi[2], ri[2], lmn;
    bio_frame *fa, *frn, *rv;

    rv = NULL;
    if (bid->stack != NULL)
       {ri[0] = fr->addr;
	ri[1] = ri[0] + fr->sz;

        lmn = 1LL << (8*sizeof(int64_t) - 2);
	imn = -1;
	frn = NULL;

	n = SC_array_get_n(bid->stack);
        for (i = 0; i < n; i++)
	    {fa = *(bio_frame **) SC_array_get(bid->stack, i);
             fi[0] = fa->addr;
	     fi[1] = fi[0] + fa->nb;

/* if a stack frame is completely covered by the request or
 * if the front of a stack frame is covered by the back of the request
 * throw it away
 * NOTE: do not use CONTAIN here because the interval FI is based on
 * fa->nb not fa->sz
 * NOTE: the ri[0] < fi[0] test is there rather than ri[0] <= fi[0]
 * in case the entire file is in the buffer FA.  This way the file
 * will not have to be reread in toto
 */
	     if ((ri[0] < fi[0]) && (fi[0] <= ri[1]))
	        {n = _SC_bfr_remove(bid, i, fa, TRUE, TRUE);
		 i--;}

	     else if ((fi[0] < ri[1]) && (ri[0] <= fi[1]) && (fi[0] < lmn))
	        {frn = fa;
		 lmn = fi[0];
	         imn = i;};};

	if (frn != NULL)
	   {rv = frn;
	    n  = SC_array_remove(bid->stack, imn);};};

    return(rv);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SC_BFR_REMOVE_OVERLAP - eliminate any buffers which overlap FR */

static void _SC_bfr_remove_overlap(bio_desc *bid, bio_frame *fr, bio_kind rw)
   {int i, n, fl;
    int64_t ri[2], fi[2];
    bio_frame *fa;

    if ((bid->stack != NULL) && (fr != NULL))
       {ri[0] = fr->addr;
	ri[1] = ri[0] + fr->sz;

	n = SC_array_get_n(bid->stack);
	for (i = 0; i < n; i++)
	    {fa = *(bio_frame **) SC_array_get(bid->stack, i);

	     fi[0] = fa->addr;
	     fi[1] = fi[0] + fa->sz;

	     if ((fi[0] <= ri[1]) && (ri[0] <= fi[1]))
	        {fl = (rw == BIO_READ);
		 n  = _SC_bfr_remove(bid, i, fa, fl, TRUE);
		 i--;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_PUSH - push a read buffer onto the BID stack */

static void _SC_bfr_push(bio_desc *bid, bio_frame *fr, int rm)
   {

    if ((bid->stack != NULL) && (fr != NULL))
       {if (rm == TRUE)
	   _SC_bfr_remove_overlap(bid, fr, fr->rw);
	SC_array_push(bid->stack, &fr);
	bid->nbfmx = max(bid->nbfmx, bid->stack->n);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_READ_SETUP - make a read buffer for BID */

static bio_frame *_SC_bfr_read_setup(bio_desc *bid, bio_frame *fr)
   {int ok;
    int64_t ad, sz;
    bio_kind rw;

    ok = (fr == NULL);

/* allocate the frame */
    if (ok == TRUE)
       {sz = bid->bfsz;
	ad = bid->curr;
	fr = _SC_bfr_alloc(ad, sz);}
    else
       {sz = fr->sz;
	ad = fr->addr + sz;};

    rw = fr->rw;

/* initialize it */
    _SC_bfr_init(fr, ad, sz);

/* eliminate any buffers which overlap the new interval */
    if (ok == FALSE)
       _SC_bfr_remove_overlap(bid, fr, rw);

/* fill it */
    fr->nb = SC_read_sigsafe(bid->fd, fr->bf, sz);

/* complete the setup */
    _SC_bfr_mark(fr, BIO_READ, FALSE);
    bid->nhits[BIO_OPER_READ]++;

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_READ_OPT - check for read optimization scenario
 *                  - where file is small enough to fit in a buffer
 *                  - of size BSZ
 */

static void _SC_bio_read_opt(bio_desc *bid, char *mode, int64_t bsz)
   {


/* GOTCHA: turning this on only has the effect of adding and fstat
 * and an ioctl to the set of operations reported by strace
 * the reference is reading all variables in a PDB file with a
 * buffer size greater than the file size
 */

#if 0
    int fd, rv;
    int64_t sz;
    struct stat bf;
    bio_frame *fr;

    if (bsz > 0)
       {fd = bid->fd;
	if (((strcmp(mode, "r") == 0) || (strcmp(mode, "rb") == 0)) &&
	    (isatty(fd) == FALSE))
	   {rv = fstat(fd, &bf);
	    if (rv == 0)
	       {sz = bf.st_size;

/* if the file size is greater than zero and less than the requested size */
		if (CONTAINS(0, sz-1, bsz))

/* initialize buffering in the descriptor */
		   {_SC_bio_buffer(bid, bsz);

/* read in the whole file */
		    fr = _SC_bfr_read_setup(bid, NULL);

/* push the frame onto the stack */
		    SC_array_push(bid->stack, &fr);
		    bid->nbfmx = max(bid->nbfmx, bid->stack->n);};};

	    bid->nhits[BIO_OPER_STAT]++;};};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_SEEK - seek to location in file */

static int _SC_bio_seek(bio_desc *bid, int64_t offs, int wh)
   {int ret;
    int64_t addr;

    ret  = 0;
    addr = 0;

    switch (wh)
       {case SEEK_SET :
	     addr = offs;
             break;
        case SEEK_CUR :
             addr = bid->curr + offs;
             break;
        case SEEK_END :
             addr = bid->sz + offs;
             break;};

    addr = max(addr, 0);

#ifdef USE_C_BUFFERED_IO

# ifdef AIX
    ret = fseek(bid->fp, offs, wh);
# else
    if (addr != bid->curr)
       ret = fseeko(bid->fp, offs, wh);
# endif

    if (ret < 0)
       io_error(errno, "fseek to %lld failed", (long long) offs);

#else

    if (addr != bid->curr)
       {int sk;
	bio_frame *fr;

	fr = _SC_bfr_contains(bid, addr);

	sk = TRUE;
	if (fr != NULL)
	   sk &= ((fr->rw != BIO_READ) || (fr->flushed == TRUE));

	if (sk == TRUE)
	   {addr = lseek(bid->fd, offs, wh);
	    if (addr < 0)
	       io_error(errno, "lseek to %lld on %d failed",
			(long long) offs, bid->fd);
	    bid->nhits[BIO_OPER_SEEK]++;};};

#endif

    _SC_bio_set_addr(bid, addr, FALSE, "seek");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_TELL - small file method for ftell */

static int64_t _SC_bio_tell(bio_desc *bid)
   {long ret;

    ret = bid->curr;

    _SC_bio_audit(bid, "tell");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_VERIFY_FILE - verify the NB bytes in FD starting at AD match BF */

static int _SC_verify_file(int fd, int64_t ad, int64_t nb, unsigned char *bf)
   {int ok;
    int64_t i, rad, st;
    unsigned char *cbf;

    rad = lseek(fd, 0, SEEK_CUR);
    if (rad < 0)
       io_error(errno, "lseek to 0 on %d failed", fd);

    cbf = CMAKE_N(unsigned char, nb);
    st  = lseek(fd, ad, SEEK_SET);
    if (st < 0)
       io_error(errno, "lseek to %lld on %d failed",
		(long long) ad, fd);

/* read the file contents */
    st = SC_read_sigsafe(fd, cbf, nb);
    if (st < 0)
       ok = -1;

/* compare direct read with buffered results */
    else
       {ok = TRUE;
	for (i = 0; (i < nb) && (ok == TRUE); i++)
	    ok = (cbf[i] == bf[i]);};

    st = lseek(fd, rad, SEEK_SET);
    if (st < 0)
       io_error(errno, "lseek to %lld on %d failed",
		(long long) rad, fd);

    CFREE(cbf);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHECK_READ - debugging diagnostic to verify buffered reads */

static int _SC_check_read(bio_desc *bid, bio_frame *rq)
   {int fd, ok;
    int64_t oad, nb;
    unsigned char *rbf;

    ok = TRUE;

    if (_SC_bio_debug & 1)
       {fd  = bid->fd;
	oad = rq->addr;
	nb  = rq->sz;
	rbf = rq->bf;

	ok = _SC_verify_file(fd, oad, nb, rbf);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHECK_WRITE - debugging diagnostic to verify buffered writes */

static int _SC_check_write(bio_desc *bid, bio_frame *rq, bio_frame *fr)
   {int ok;
    int64_t cad, rad, nbr, bad, nbb;
    static int count = 1;

    ok = TRUE;

    if (_SC_bio_debug & 2)
       {cad = bid->curr;

	rad = rq->addr;
	nbr = rq->sz;

	if (fr == NULL)
	   {bad = 0;
	    nbb = 0;}
	else
	   {bad = fr->addr;
	    nbb = fr->nb;};

	if (count == 1)
	   {printf("                                      Request                              Buffer\n");
	    printf("   Write  Kind     Addr        Start      Stop  # bytes           Start       Stop  # bytes\n");};

	if (rq->rw == BIO_WRITE)
	   printf("%8d  %s %10ld   %10ld %10ld %8ld      %10ld %10ld %8ld\n",
		  count++,
		  "w",
		  (long) cad,
		  (long) rad, (long) (rad+nbr), (long) nbr,
		  (long) bad, (long) (bad+nbb), (long) nbb);
	else
	   printf("%8d  %s %10ld   %10ld %10ld %8ld      %10ld %10ld %8ld\n",
		  count++,
		  "r",
		  (long) cad,
		  (long) -1L, (long) -1L, (long) nbr,
		  (long) bad, (long) (bad+nbb), (long) nbb);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHECK_BUF - check for buffer overlaps
 *               - return TRUE iff the file is ok
 */

static int _SC_check_buf(bio_desc *bid)
   {int i, j, n, ok;
    int64_t ri[2], fi[2];
    bio_frame *fa;

    ok = TRUE;

/* check for buffer overlaps */
    if (bid->stack != NULL)
       {n = SC_array_get_n(bid->stack);
        for (i = 0; (i < n) && (ok == TRUE); i++)
	    {fa = *(bio_frame **) SC_array_get(bid->stack, i);

	     ri[0] = fa->addr;
	     ri[1] = ri[0] + fa->sz;

	     for (j = i+1; (j < n) && (ok == TRUE); j++)
	         {fa = *(bio_frame **) SC_array_get(bid->stack, j);

		  fi[0] = fa->addr;
		  fi[1] = fi[0] + fa->sz;

		  ok &= ((ri[1] < fi[0]) || (fi[1] < ri[0]));};};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHECK_FILE - verify the file
 *               - especially wrt buffer overlaps
 *               - return TRUE iff the file is ok
 */

int SC_check_file(FILE *fp)
   {int ok;
    file_io_desc *fid;

    fid = (file_io_desc *) fp;
    fp  = fid->info;

    ACCESS(fp);

    ok = _SC_check_buf(bid);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_IN - buffer BF in from BID */

static int64_t _SC_bio_in(void *bf, int64_t bpi, int64_t ni, bio_desc *bid)
   {int64_t na, nr, nb, nba;

    nb = bpi*ni;
    nr = 0;
    na = 0;

    bid->nhr++;

#ifdef USE_C_BUFFERED_IO

    nr = SC_fread_sigsafe(bf, bpi, ni, bid->fp);
    nr *= bpi;

#else

   {int i, ok;
    int64_t nbc, ad;
    int64_t bsz, olc, nlc;
    bio_frame *fr, rq;

    nba = bid->sz - bid->curr;

    if (bid->stack == NULL)
       {nr = SC_read_sigsafe(bid->fd, bf, nb);
	bid->nhits[BIO_OPER_READ]++;}

/* do not try to read if the file pointer is
 * positioned past the current end of file - only writes
 * are permitted there
 */
    else if (nba <= 0)
       return(0);

    else

/* encapsulate the requested write as a frame so that all subsequent
 * operations can be posed in terms of frames
 */
       {rq.sz   = min(nb, nba);
	rq.nb   = 0;
	rq.addr = bid->curr;
	rq.bf   = bf;
	_SC_bfr_mark(&rq, BIO_READ, FALSE);

/* get the frame to use for staging the request from disk
 * at the end if the buffer is not completely full it
 * goes back onto the stack
 */
	fr = _SC_bfr_next(bid, &rq);
	if (fr == NULL)
	   fr = _SC_bfr_read_setup(bid, NULL);

#if 1
	for (i = 0, ok = TRUE; ok == TRUE; i++)
	    {nbc = _SC_bfr_infill(&rq, fr);
	     nr += nbc;
	     if (rq.nb != rq.sz)
	        {bsz = fr->sz;
		 olc = fr->addr;
		 nlc = olc + bsz;

/* NOTE: although an io_seek may have been done before getting
 * to the io_read, it may not have moved the file pointer
 * because it only knew that the address landed in a buffer
 * it could not know whether the buffer had enough bytes to
 * fulfill the request
 * consequently, if the first pass through the loop does not
 * fulfill the request, we must do a seek before reading bytes
 * from the file
 */
		 if ((olc != bid->curr) || (i == 0))
		    {na += (bid->curr - nlc);
		     ad  = _SC_bio_seek(bid, nlc, SEEK_SET);
		     SC_ASSERT(ad >= 0);};

		 fr = _SC_bfr_read_setup(bid, fr);

/* signify that something happened and we cannot exit the loop */
		 nbc = -1;};
	     ok = (nbc != 0);};
#else
	do {nbc = _SC_bfr_infill(&rq, fr);
	    nr += nbc;
	    if (rq.nb != rq.sz)
	       {bsz = fr->sz;
		olc = fr->addr;
		nlc = olc + bsz;
		if (olc != bid->curr)
		   {na += (bid->curr - nlc);
		    ad  = _SC_bio_seek(bid, nlc, SEEK_SET);
		    SC_ASSERT(ad >= 0);};

		fr = _SC_bfr_read_setup(bid, fr);

/* signify that something happened and we cannot exit the loop */
	        nbc = -1;};}
	while (nbc != 0);
#endif

	_SC_bfr_push(bid, fr, TRUE);

/* check the result and return -1 on error */
	ok = _SC_check_read(bid, &rq);
        if (ok == FALSE)
	   return(-1);};};
#endif

    _SC_bio_set_addr(bid, bid->curr + na + nr, TRUE, "read");

    nr /= bpi;

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_OUT - buffer BF out to BID */

static int64_t _SC_bio_out(void *bf, int64_t bpi, int64_t ni, bio_desc *bid)
   {int64_t na, nb, nw;

    nb = bpi*ni;
    nw = 0;
    na = 0;

    bid->nhw++;

#ifdef USE_C_BUFFERED_IO

    nw = SC_fwrite_sigsafe(bf, bpi, ni, bid->fp);
    nw *= bpi;

#else

   {int64_t nbc, nbw, ad;
    bio_frame *fr, rq;

    fr = NULL;

    if (bid->stack == NULL)
       {nw = SC_write_sigsafe(bid->fd, bf, nb);
	bid->nhits[BIO_OPER_WRITE]++;}

    else

/* encapsulate the requested write as a frame so that all subsequent
 * operations can be posed in terms of frames
 */
       {rq.sz   = nb;
	rq.nb   = 0;
	rq.addr = bid->curr;
	rq.bf   = bf;
	_SC_bfr_mark(&rq, BIO_WRITE, FALSE);

/* get the frame to use for staging the request to disk
 * at the end if the buffer is not completely full it
 * goes back onto the stack
 */
	fr = _SC_bfr_next(bid, &rq);
	if (fr == NULL)
	   fr = _SC_bfr_alloc(bid->curr, bid->bfsz);

	do {nbc = _SC_bfr_outfill(fr, &rq);
	    nw += nbc;
	    if (fr->nb == fr->sz)
	       {if (fr->addr != bid->curr)
		   {na += (bid->curr - fr->addr);
		    ad  = _SC_bio_seek(bid, fr->addr, SEEK_SET);
		    SC_ASSERT(ad >= 0);};

		nbw = SC_write_sigsafe(bid->fd, fr->bf, fr->nb);
		_SC_bfr_init(fr, bid->curr + nbw, bid->bfsz);
		bid->nhits[BIO_OPER_WRITE]++;};}
	while (nbc != 0);

        if (fr->nb != 0)
	   _SC_bfr_push(bid, fr, FALSE);
	else
	   _SC_bfr_free(fr);};

    _SC_check_write(bid, &rq, fr);};

#endif

    _SC_bio_set_addr(bid, bid->curr + na + nw, TRUE, "write");

    nw /= bpi;

    return(nw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_FLUSH - flush the buffers */

static int _SC_bio_flush(bio_desc *bid)
   {int ret;

    ret = FALSE;

#ifdef USE_C_BUFFERED_IO

    ret = fflush(bid->fp);

#else

   {int64_t ad;

/* NOTE: flush should not change file position so save and restore */
    ad = bid->curr;

   _SC_bio_free(bid, FALSE);

    bid->curr = ad;}

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                           SMALL FILE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _SC_BSEEK - small file method for fseek */

static int _SC_bseek(FILE *fp, long offs, int wh)
   {int ret;

    ACCESS(fp);

    ret = _SC_bio_seek(bid, offs, wh);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BTELL - small file method for ftell */

static long _SC_btell(FILE *fp)
   {long ret;

    ACCESS(fp);

    ret = _SC_bio_tell(bid);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BREAD - small file method for fread */

static size_t _SC_bread(void *s, size_t bpi, size_t ni, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = _SC_bio_in(s, bpi, ni, bid);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BWRITE - small file method for fwrite */

static size_t _SC_bwrite(void *s, size_t bpi, size_t ni, FILE *fp)
   {size_t nw;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    nw = _SC_bio_out(s, bpi, ni, bid);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_gs.io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/

/*                           LARGE FILE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _SC_BLSEEK - large file method for fseek */

static int _SC_blseek(FILE *fp, int64_t offs, int wh)
   {int ret;

    ACCESS(fp);

    ret = _SC_bio_seek(bid, offs, wh);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLTELL - large file method for ftell */

static int64_t _SC_bltell(FILE *fp)
   {int64_t ret;

    ACCESS(fp);

    ret = _SC_bio_tell(bid);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLREAD - large file method for fread */

static uint64_t _SC_blread(void *s, size_t bpi, uint64_t ni, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = _SC_bio_in(s, bpi, ni, bid);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLWRITE - large file method for fwrite */

static uint64_t _SC_blwrite(void *s, size_t bpi, uint64_t ni, FILE *fp)
   {size_t nw;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    nw = _SC_bio_out(s, bpi, ni, bid);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_gs.io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/

/*                           STANDARD FILE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_BOUT - worker for _SC_bputs and _SC_bprintf */

static int _SC_bout(const char *s, bio_desc *bid)
   {int nb, nc;
    FILE *fp;

    fp = bid->fp;
    nb = 0;

    if ((fp != NULL) && (s != NULL))
       {nc = strlen(s);
	nb = _SC_bio_out((void *) s, 1, nc, bid);};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BPUTS - method for fputs */

static int _SC_bputs(const char *s, FILE *fp)
   {int nb;

    ACCESS(fp);

    nb = _SC_bout(s, bid);

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BPRINTF - method for fprintf */

static int _SC_bprintf(FILE *fp, char *fmt, va_list a)
   {int ret;
    char *msg;

    ACCESS(fp);

    msg = SC_vdsnprintf(TRUE, fmt, a);
    ret = _SC_bout(msg, bid);
    CFREE(msg);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BSETVBUF - method for setvbuf */

static int _SC_bsetvbuf(FILE *fp, char *buf, int type, size_t size)
   {int ret;

    ACCESS(fp);

#ifdef USE_C_BUFFERED_IO

    ret = setvbuf(fp, buf, type, size);

#else

    if (size < 1)
       ret = setvbuf(fp, buf, type, size);
    else
       ret = 0;

    _SC_bio_buffer(bid, size);

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BGETC - method for fgetc */

static int _SC_bgetc(FILE *fp)
   {int rv;

    ACCESS(fp);

#ifdef USE_C_BUFFERED_IO

    rv = fgetc(fp);

    _SC_bio_set_addr(bid, bid->curr + 1, TRUE, "getc");

#else

   {int nb;
    char s[2];

    if (bid->curr >= bid->sz)
       rv = EOF;
    else
       {nb = _SC_bio_in(s, 1, 1, bid);
	SC_ASSERT(nb >= 0);

        rv = s[0];};};

#endif

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BUNGETC - method for ungetc */

static int _SC_bungetc(int c, FILE *fp)
   {int rv;

    ACCESS(fp);

#ifdef USE_C_BUFFERED_IO

    rv = ungetc(c, fp);

#else

    rv = c;

#endif

    if (rv != EOF)
       _SC_bio_seek(bid, -1, SEEK_CUR);
/*       _SC_bio_set_addr(bid, bid->curr - 1, TRUE, "ungetc"); */

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BEOF - method for feof */

static int _SC_beof(FILE *fp)
   {int rv;

    ACCESS(fp);

#ifdef USE_C_BUFFERED_IO

    rv = feof(fp);

#else

    rv = (bid->curr >= bid->sz);

#endif

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BGETS - method for fgets which terminates on \r or \n */

static char *_SC_bgets(char *s, int n, FILE *fp)
   {int i, nbr, nb, nc;
    int64_t pos;
    char *r;

    ACCESS(fp);

    if (fp == stdin)
       r = _SC_rl_fgets(s, n);

    else
       {r = NULL;

/* reserve the last character for a '\0' terminator */
        nc = n - 1;
	if (nc > 0)
	   {pos = _SC_bio_tell(bid);
	    nbr = _SC_bio_in(s, 1, nc, bid);
	    if (nbr < nc)
	       s[nbr] = '\0';

/* check for newlines */
	    if (nbr > 0)
	       {for (i = 0; (i < nc) && (s[i] != '\n') && (s[i] != '\r'); i++);
		nb = i + 1;
		if (nb < nc)
		   s[nb] = '\0';

		nb = min(nb, nbr);
		_SC_bio_seek(bid, pos + nb, SEEK_SET);};
       
	    r = (nbr > 0) ? s : NULL;

/* always set last character to '\0' to avoid a buffer overrun */
	    s[nc] = '\0';};};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BCLOSE - method for fclose */

static int _SC_bclose(FILE *fp)
   {int ret;

    ACCESS(fp);

    ret = FALSE;

    if (fp != NULL)
       {ret = _SC_bio_flush(bid);
	ret = fclose(fp);};

    bio_stats[0] += bid->nhr;
    bio_stats[1] += bid->nhits[BIO_OPER_READ];
    bio_stats[2] += bid->nhw;
    bio_stats[3] += bid->nhits[BIO_OPER_WRITE];
    bio_stats[4] = max(bio_stats[4], bid->nbfmx);

    _SC_bio_free(bid, TRUE);

    CFREE(bid);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFLUSH - method for fflush */

static int _SC_bflush(FILE *fp)
   {int ret, st;

    ACCESS(fp);

    ret = _SC_bio_flush(bid);

    st = fsync(bid->fd);

    SC_ASSERT(st != 0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BINFO - return buffering statistics for FP */

long *SC_binfo(void)
   {

    return(bio_stats);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BOPEN - small file method for fopen */

FILE *SC_bopen(char *name, char *mode)
   {int st;
    FILE *ret, *fp;
    bio_desc *bid;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {bid        = CMAKE(bio_desc);
            bid->sz    = SC_file_size(fp);
	    bid->curr  = 0;
	    bid->bfsz  = 0;
	    bid->stack = NULL;
	    bid->fd    = fileno(fp);
	    bid->fp    = fp;
	    bid->nhr   = 0;
	    bid->nhw   = 0;
	    bid->nbfmx = 0;

#ifndef USE_C_BUFFERED_IO

/* NOTE: this should not be necessary but various implementations
 * do not reliably leave the system level file position at 0
 */
	    st = lseek(bid->fd, 0, 0);
	    if (st < 0)
	       io_error(errno, "lseek to 0 on %d failed", bid->fd);

	    bid->nhits[BIO_OPER_SEEK]++;

#endif

	    fid          = SC_make_io_desc(bid);
	    fid->fopen   = SC_bopen;
	    fid->ftell   = _SC_btell;
	    fid->fseek   = _SC_bseek;
	    fid->fread   = _SC_bread;
	    fid->fwrite  = _SC_bwrite;
	    fid->setvbuf = _SC_bsetvbuf;
	    fid->fclose  = _SC_bclose;
	    fid->fprintf = _SC_bprintf;
	    fid->fputs   = _SC_bputs;
	    fid->fgetc   = _SC_bgetc;
	    fid->ungetc  = _SC_bungetc;
	    fid->fflush  = _SC_bflush;
	    fid->feof    = _SC_beof;
	    fid->fgets   = _SC_bgets;

	    _SC_bio_read_opt(bid, mode, _SC.buffer_size);

	    ret = (FILE *) fid;}
	else
	   ret = NULL;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LBOPEN - large file method for fopen */

FILE *SC_lbopen(char *name, char *mode)
   {int st;
    FILE *ret, *fp;
    bio_desc *bid;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {bid        = CMAKE(bio_desc);
            bid->sz    = SC_file_size(fp);
	    bid->curr  = 0;
	    bid->bfsz  = 0;
	    bid->stack = NULL;
	    bid->fd    = fileno(fp);
	    bid->fp    = fp;
	    bid->nhr   = 0;
	    bid->nhw   = 0;
	    bid->nbfmx = 0;
            memset(bid->nhits, 0, sizeof(bid->nhits));

#ifndef USE_C_BUFFERED_IO

/* NOTE: this should not be necessary but various implementations
 * do not reliably leave the system level file position at 0
 */
	    st = lseek(bid->fd, 0, 0);
	    if (st < 0)
	       io_error(errno, "lseek to 0 on %d failed", bid->fd);
	    bid->nhits[BIO_OPER_SEEK]++;
#endif

	    fid          = SC_make_io_desc(bid);
	    fid->fopen   = SC_lbopen;
	    fid->lftell  = _SC_bltell;
	    fid->lfseek  = _SC_blseek;
	    fid->lfread  = _SC_blread;
	    fid->lfwrite = _SC_blwrite;
	    fid->setvbuf = _SC_bsetvbuf;
	    fid->fclose  = _SC_bclose;
	    fid->fprintf = _SC_bprintf;
	    fid->fputs   = _SC_bputs;
	    fid->fgetc   = _SC_bgetc;
	    fid->ungetc  = _SC_bungetc;
	    fid->fflush  = _SC_bflush;
	    fid->feof    = _SC_beof;
	    fid->fgets   = _SC_bgets;

	    _SC_bio_read_opt(bid, mode, _SC.buffer_size);

	    ret = (FILE *) fid;}
	else
	   ret = NULL;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BF_SET_HOOKS - setup the buffered io hooks
 *                 - this is for insane systems where C std library
 *                 - buffering does not work
 *                 - IBM BG/P is especially bad
 *                 - modern Linux is iffy
 */

void SC_bf_set_hooks(void)
   {

    _SC_ios.sfopen  = SC_bopen;
    _SC_ios.lfopen = SC_lbopen;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LBF_SET_HOOKS - setup the buffered io hooks
 *                  - this is for insane systems where C std library
 *                  - buffering does not work
 *                  - IBM BG/P is especially bad
 *                  - modern Linux is iffy
 */

void SC_lbf_set_hooks(void)
   {

    _SC_ios.sfopen  = SC_bopen;
    _SC_ios.lfopen = SC_lbopen;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

