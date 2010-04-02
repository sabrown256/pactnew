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

#include "scope_proc.h"

/* #define DEBUG */
#define USE_C_BUFFERED_IO

#define ACCESS(_fp)                                                          \
    bio_desc *bid;                                                           \
    bid = (bio_desc *) (_fp);                                                \
    fp  = bid->fp

typedef enum e_bio_oper bio_oper;

enum e_bio_oper
   {BIO_READ, BIO_WRITE, BIO_EMPTY};

typedef struct s_bio_desc bio_desc;
typedef struct s_bio_frame bio_frame;

struct s_bio_frame
   {BIGINT sz;                         /* size of the frame */
    BIGINT nb;                         /* number of bytes in frame */
    BIGINT addr;                       /* starting disk address of frame */
    int flushed;
    bio_oper rw;                       /* has read/write bytes */
    unsigned char *bf;};               /* frame buffer */

struct s_bio_desc
   {BIGINT sz;                         /* file size */
    BIGINT curr;                       /* current file address */
    BIGINT bfsz;                       /* buffer size */
    SC_array *stack;
    int fd;
    FILE *fp;};

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
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_SET_ADDR - update the address and size of BID */

static INLINE off_t _SC_bio_set_addr(bio_desc *bid, off_t addr, int chsz,
				     char *tag)
   {

    addr = max(addr, 0);

    bid->curr = addr;
    if ((chsz == TRUE) && (addr > bid->sz))
       bid->sz = addr;

    _SC_bio_audit(bid, tag);

    return(addr);}

/*--------------------------------------------------------------------------*/

#ifndef USE_C_BUFFERED_IO

static int
 _SC_bio_seek(bio_desc *bid, off_t offs, int wh);

/*--------------------------------------------------------------------------*/

/* _SC_BFR_INIT - initialize a buffered io frame */

static void _SC_bfr_init(bio_frame *fr, off_t addr, size_t bfsz)
   {

    if (fr != NULL)
       {if (fr->sz != bfsz)
	   SFREE(fr->bf);

	if (fr->bf == NULL)
	   fr->bf = FMAKE_N(unsigned char, bfsz, "_SC_BFR_INIT:bf");

        memset(fr->bf, 0, bfsz);

	fr->sz      = bfsz;
        fr->nb      = 0;
	fr->addr    = addr;
	fr->rw      = BIO_EMPTY;
	fr->flushed = TRUE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_ALLOC - initialize a buffered io frame */

static bio_frame *_SC_bfr_alloc(off_t addr, size_t bfsz)
   {bio_frame *fr;

    fr = FMAKE(bio_frame, "_SC_BFR_ALLOC:fr");
    memset(fr, 0, sizeof(bio_frame));
    
    _SC_bfr_init(fr, addr, bfsz);

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_BUFFER - initialize buffering */

static void _SC_bio_buffer(bio_desc *bid, size_t bfsz)
   {

    bid->bfsz  = bfsz;
    bid->stack = SC_MAKE_ARRAY("_SC_BIO_BUFFER", bio_frame *, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_INFILL - copy the appropriate parts of FS into FD
 *                - return the number of bytes copied
 */

static BIGINT _SC_bfr_infill(bio_frame *fd, bio_frame *fs)
   {off_t fds, fde, fss, fse;
    BIGINT nb, os, oe;
    unsigned char *ps, *pd;

/* decide overlap based on file address */
    fds = fd->addr + fd->nb;
    fde = fd->addr + fd->sz;

    fss = fs->addr + fs->nb;
    fse = fs->addr + fs->sz;

/* the overlap is the greater of the starts and the lesser of the ends */
    os = max(fss, fds);
    oe = min(fse, fde);

/* make the offsets relative */
    os -= fs->addr;
    oe -= fs->addr;
    nb  = oe - os;
	
/* do copy based on memory address */
    ps = fs->bf + os;
    pd = fd->bf + fd->nb;
    memcpy(pd, ps, nb);

    fd->nb += nb;

    if (nb > 0)
       {fd->flushed = FALSE;
	fd->rw      = BIO_READ;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_OUTFILL - copy the appropriate parts of FS into FD
 *                 - return the number of bytes copied
 */

static BIGINT _SC_bfr_outfill(bio_frame *fd, bio_frame *fs)
   {off_t fds, fde, fss, fse;
    BIGINT nb, os, oe;
    unsigned char *ps, *pd;

/* decide overlap based on file address */
    fds = fd->addr + fd->nb;
    fde = fd->addr + fd->sz;

    fss = fs->addr + fs->nb;
    fse = fs->addr + fs->sz;

/* the overlap is the greater of the starts and the lesser of the ends */
    os = max(fss, fds);
    oe = min(fse, fde);

/* make the offsets relative */
    os -= fs->addr;
    oe -= fs->addr;
    nb  = oe - os;
	
/* do copy based on memory address */
    ps = fs->bf + os;
    pd = fd->bf + fd->nb;
    memcpy(pd, ps, nb);

    fd->nb += nb;

    if (nb > 0)
       {fd->flushed = FALSE;
	fd->rw      = BIO_WRITE;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_NEXT - return the leftmost stack frame which overlaps FR */

static bio_frame *_SC_bfr_next(bio_desc *bid, bio_frame *fr)
   {int i, n, imn;
    off_t fs, fe, rs, re, lmn;
    bio_frame *fa, *frn, *rv;

    rv = NULL;
    if (bid->stack != NULL)
       {rs = fr->addr;
	re = fr->addr + fr->sz;

        lmn = 1LL << (8*sizeof(off_t) - 2);
	imn = -1;
	frn = NULL;

	n = SC_array_get_n(bid->stack);
        for (i = 0; i < n; i++)
	    {fa = *(bio_frame **) SC_array_get(bid->stack, i);
             fs = fa->addr;
	     fe = fs + fa->nb;
	     if ((fs < re) && (rs <= fe) && (fs < lmn))
	        {frn = fa;
		 lmn = fs;
	         imn = i;};};

	if (frn != NULL)
	   {rv = frn;
	    n  = SC_array_remove(bid->stack, imn);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFR_FLUSH - flush the frame FR */

static int _SC_bfr_flush(bio_desc *bid, bio_frame *fr)
   {int ret, ok, nw;
    off_t fs, fe, nb;

    ret = FALSE;

    nb = fr->nb;
    fs = fr->addr;
    fe = fs + nb;

    ok = _SC_bio_seek(bid, fs, SEEK_SET);

/* GOTCHA: does this go past end of file? */
    nw = SC_write_sigsafe(bid->fd, fr->bf, nb);

    _SC_bio_set_addr(bid, fe, TRUE, "flush");

    return(ret);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SC_BFR_FREE - free bio_frame */

static void _SC_bfr_free(bio_frame *fr)
   {

    if (fr != NULL)
       {SFREE(fr->bf);
	SFREE(fr);};

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

static void _SC_bio_free(bio_desc *bid)
   {

    if (bid->stack != NULL)
       SC_free_array(bid->stack, _SC_bfr_free_elem);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_SEEK - seek to location in file */

static int _SC_bio_seek(bio_desc *bid, off_t offs, int wh)
   {int ret;
    off_t addr;

    ret = 0;

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

#ifdef USE_C_BUFFERED_IO

# ifdef AIX
    ret = fseek(fp, offs, wh);
# else
    if (addr != bid->curr)
       ret = fseeko(bid->fp, offs, wh);
# endif

#else

    addr = lseek(bid->fd, offs, wh);

#endif

    _SC_bio_set_addr(bid, addr, FALSE, "seek");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_TELL - small file method for ftell */

static off_t _SC_bio_tell(bio_desc *bid)
   {long ret;

    ret = bid->curr;

    _SC_bio_audit(bid, "tell");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_IN - buffer BF in from BID */

static BIGINT _SC_bio_in(void *bf, BIGINT bpi, BIGINT ni, bio_desc *bid)
   {off_t nr, nb;

    nb = bpi*ni;
    nr = 0;

#ifdef USE_C_BUFFERED_IO

    nr = SC_fread_sigsafe(bf, bpi, ni, bid->fp);
    nr *= bpi;

#else

   {off_t nbc, nbr;
    bio_frame *fr, rq;

    if (bid->stack == NULL)
       nr = SC_read_sigsafe(bid->fd, bf, nb);
    else

/* encapsulate the requested write as a frame so that all subsequent
 * operations can be posed in terms of frames
 */
       {rq.sz      = nb;
	rq.nb      = 0;
	rq.addr    = bid->curr;
	rq.bf      = bf;
	rq.flushed = FALSE;

/* get the frame to use for staging the request from disk
 * at the end if the buffer is not completely full it
 * goes back onto the stack
 */
	fr = _SC_bfr_next(bid, &rq);
	if (fr == NULL)
	   {fr = _SC_bfr_alloc(bid->curr, bid->bfsz);
	    nbr = SC_read_sigsafe(bid->fd, fr->bf, fr->sz);
	    _SC_bfr_init(fr, bid->curr, fr->sz);};

	do {nbc = _SC_bfr_infill(&rq, fr);
	    nr += nbc;
	    if (rq.nb != rq.sz)
	       {nbr = SC_read_sigsafe(bid->fd, fr->bf, fr->sz);
		_SC_bfr_init(fr, bid->curr + nr, fr->sz);};}
	while (nbc != 0);

	SC_array_push(bid->stack, &fr);};};
#endif

    _SC_bio_set_addr(bid, bid->curr + nr, TRUE, "read");

    nr /= bpi;

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIO_OUT - buffer BF out to BID */

static BIGINT _SC_bio_out(void *bf, BIGINT bpi, BIGINT ni, bio_desc *bid)
   {off_t nw, nb;

    nb = bpi*ni;
    nw = 0;

#ifdef USE_C_BUFFERED_IO

    nw = SC_fwrite_sigsafe(bf, bpi, ni, bid->fp);
    nw *= bpi;

#else

   {off_t nbc, nbw;
    bio_frame *fr, rq;

    if (bid->stack == NULL)
       nw = SC_write_sigsafe(bid->fd, bf, nb);

    else

/* encapsulate the requested write as a frame so that all subsequent
 * operations can be posed in terms of frames
 */
       {rq.sz      = nb;
	rq.nb      = 0;
	rq.addr    = bid->curr;
	rq.bf      = bf;
	rq.flushed = FALSE;

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
	       {nbw = SC_write_sigsafe(bid->fd, fr->bf, fr->nb);
		_SC_bfr_init(fr, bid->curr + nw, bid->bfsz);};}
	while (nbc != 0);

        if (fr->nb != 0)
	   SC_array_push(bid->stack, &fr);
	else
	   _SC_bfr_free(fr);};};

#endif

    _SC_bio_set_addr(bid, bid->curr + nw, TRUE, "write");

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

   {int i, n;
    bio_frame *fr;

    if (bid->stack != NULL)
       {n = SC_array_get_n(bid->stack);
        for (i = 0; i < n; i++)
	    {fr  = *(bio_frame **) SC_array_get(bid->stack, i);
	     if (fr->rw == BIO_WRITE)
	        ret = _SC_bfr_flush(bid, fr);};};};

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

static size_t _SC_bread(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = _SC_bio_in(s, bpi, nitems, bid);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BWRITE - small file method for fwrite */

static size_t _SC_bwrite(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nw;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    nw = _SC_bio_out(s, bpi, nitems, bid);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/

/*                           LARGE FILE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _SC_BLSEEK - large file method for fseek */

static int _SC_blseek(FILE *fp, off_t offs, int wh)
   {int ret;

    ACCESS(fp);

    ret = _SC_bio_seek(bid, offs, wh);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLTELL - large file method for ftell */

static off_t _SC_bltell(FILE *fp)
   {off_t ret;

    ACCESS(fp);

    ret = _SC_bio_tell(bid);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLREAD - large file method for fread */

static BIGUINT _SC_blread(void *s, size_t bpi, BIGUINT nitems, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = _SC_bio_in(s, bpi, nitems, bid);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLWRITE - large file method for fwrite */

static BIGUINT _SC_blwrite(void *s, size_t bpi, BIGUINT nitems, FILE *fp)
   {size_t nw;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    nw = _SC_bio_out(s, bpi, nitems, bid);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

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
    SFREE(msg);

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

    nb = _SC_bio_in(s, 1, 1, bid);
    rv = s[0];};

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

    _SC_bio_set_addr(bid, bid->curr - 1, TRUE, "ungetc");

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
   {int i, nbr, nb;
    off_t pos;
    char *r;

    ACCESS(fp);

    if (fp == stdin)
       r = fgets(s, n, stdin);

    else
       {pos = _SC_bio_tell(bid);
	nbr = _SC_bio_in(s, 1, n, bid);
	if (nbr < n)
	   s[nbr] = '\0';

/* check for newlines */
	if (nbr > 0)
	   {for (i = 0; (i < n) && (s[i] != '\n') && (s[i] != '\r'); i++);
	    nb = i + 1;
	    if (nb < n)
	       s[nb] = '\0';

	    nb = min(nb, nbr);
	    _SC_bio_seek(bid, pos + nb, SEEK_SET);};
       
	r = (nbr > 0) ? s : NULL;};

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

    _SC_bio_free(bid);

    SFREE(bid);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFLUSH - method for fflush */

static int _SC_bflush(FILE *fp)
   {int ret;

    ACCESS(fp);

    ret = _SC_bio_flush(bid);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BOPEN - small file method for fopen */

FILE *SC_bopen(char *name, char *mode)
   {FILE *ret, *fp;
    bio_desc *bid;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {bid        = FMAKE(bio_desc, "SC_BOPEN:bid");
            bid->sz    = SC_file_size(fp);
	    bid->curr  = 0;
	    bid->bfsz  = 0;
	    bid->stack = NULL;
	    bid->fd    = fileno(fp);
	    bid->fp    = fp;

	    fid          = FMAKE(file_io_desc, "SC_BOPEN:fid");
	    fid->info    = bid;
	    fid->fopen   = SC_bopen;
	    fid->ftell   = _SC_btell;
	    fid->lftell  = NULL;
	    fid->fseek   = _SC_bseek;
	    fid->lfseek  = NULL;
	    fid->fread   = _SC_bread;
	    fid->lfread  = NULL;
	    fid->fwrite  = _SC_bwrite;
	    fid->lfwrite = NULL;
	    fid->setvbuf = _SC_bsetvbuf;
	    fid->fclose  = _SC_bclose;
	    fid->fprintf = _SC_bprintf;
	    fid->fputs   = _SC_bputs;
	    fid->fgetc   = _SC_bgetc;
	    fid->ungetc  = _SC_bungetc;
	    fid->fflush  = _SC_bflush;
	    fid->feof    = _SC_beof;
	    fid->fgets   = _SC_bgets;
	    fid->pointer = NULL;
	    fid->segsize = NULL;

	    ret = (FILE *) fid;}
	else
	   ret = NULL;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LBOPEN - large file method for fopen */

FILE *SC_lbopen(char *name, char *mode)
   {FILE *ret, *fp;
    bio_desc *bid;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {bid        = FMAKE(bio_desc, "SC_LBOPEN:bid");
            bid->sz    = SC_file_size(fp);
	    bid->curr  = 0;
	    bid->bfsz  = 0;
	    bid->stack = NULL;
	    bid->fd    = fileno(fp);
	    bid->fp    = fp;

	    fid          = FMAKE(file_io_desc, "SC_LBOPEN:fid");
	    fid->info    = bid;
	    fid->fopen   = SC_lbopen;
	    fid->ftell   = NULL;
	    fid->lftell  = _SC_bltell;
	    fid->fseek   = NULL;
	    fid->lfseek  = _SC_blseek;
	    fid->fread   = NULL;
	    fid->lfread  = _SC_blread;
	    fid->fwrite  = NULL;
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
	    fid->pointer = NULL;
	    fid->segsize = NULL;

	    ret = (FILE *) fid;}
	else
	   ret = NULL;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

