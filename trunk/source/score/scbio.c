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
/* #define NEWWAY */

#define ACCESS(_fp)                                                          \
    bf_io_desc *bid;                                                         \
    bid = (bf_io_desc *) (_fp);                                              \
    fp  = bid->fp

typedef struct s_bf_io_desc bf_io_desc;
typedef struct s_bf_io_frame bf_io_frame;

struct s_bf_io_frame
   {BIGINT nb;                         /* size of the frame */
    BIGINT addr;                       /* starting disk address of frame */
    unsigned char *bf;};               /* frame buffer */

struct s_bf_io_desc
   {BIGINT sz;                         /* file size */
    BIGINT curr;                       /* current file address */
    SC_array *stack;
    FILE *fp;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BSET_ADDR - update the address and size of BID */

static INLINE off_t _SC_bset_addr(bf_io_desc *bid, off_t addr, int chsz,
				  char *tag)
   {

    addr = max(addr, 0);

    bid->curr = addr;
    if ((chsz == TRUE) && (addr > bid->sz))
       bid->sz = addr;

#ifdef DEBUG
 printf(">>> set address %s to %ld (%ld)\n",
	tag, (long) ftello(bid->fp), (long) bid->curr);
#endif

    return(addr);}

/*--------------------------------------------------------------------------*/

/*                           SMALL FILE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _SC_BSEEK - small file method for fseek */

static int _SC_bseek(FILE *fp, long offs, int wh)
   {int ret;
    off_t addr;

    ACCESS(fp);

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

    ret = fseek(fp, offs, wh);

    _SC_bset_addr(bid, addr, FALSE, "seek");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BTELL - small file method for ftell */

static long _SC_btell(FILE *fp)
   {long ret;

    ACCESS(fp);

#ifdef NEWWAY
    ret = bid->curr;
#else
    ret = ftell(fp);
#endif

#ifdef DEBUG
 printf(">>> small tell %ld (%ld)\n", (long) ftello(fp), (long) bid->curr);
#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BREAD - small file method for fread */

static size_t _SC_bread(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = SC_fread_sigsafe(s, bpi, nitems, fp);

    _SC_bset_addr(bid, bid->curr + nr*bpi, TRUE, "read");

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BWRITE - small file method for fwrite */

static size_t _SC_bwrite(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nw;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    nw = SC_fwrite_sigsafe(s, bpi, nitems, fp);

    _SC_bset_addr(bid, bid->curr + nw*bpi, TRUE, "write");

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/

/*                           LARGE FILE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _SC_BLSEEK - large file method for fseek */

static int _SC_blseek(FILE *fp, off_t offs, int wh)
   {int ret;
    off_t addr;

    ACCESS(fp);

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

    ret = fseeko(fp, offs, wh);

    _SC_bset_addr(bid, addr, FALSE, "seek");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLTELL - large file method for ftell */

static off_t _SC_bltell(FILE *fp)
   {off_t ret;

    ACCESS(fp);

#ifdef NEWWAY
    ret = bid->curr;
#else
    ret = ftello(fp);
#endif

#ifdef DEBUG
 printf(">>> large tell %ld (%ld)\n", (long) ftello(fp), (long) bid->curr);
#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLREAD - large file method for fread */

static BIGUINT _SC_blread(void *s, size_t bpi, BIGUINT nitems, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = SC_fread_sigsafe(s, bpi, nitems, fp);

    _SC_bset_addr(bid, bid->curr + nr*bpi, TRUE, "read");

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLWRITE - large file method for fwrite */

static BIGUINT _SC_blwrite(void *s, size_t bpi, BIGUINT nitems, FILE *fp)
   {size_t nw;    off_t ad;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    nw = SC_fwrite_sigsafe(s, bpi, nitems, fp);
    fflush(fp);   ad = ftello(fp);

    _SC_bset_addr(bid, bid->curr + nw*bpi, TRUE, "write");

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/

/*                           STANDARD FILE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_BOUT - worker for _SC_bputs and _SC_bwrite */

static int _SC_bout(const char *s, bf_io_desc *bid)
   {int nb, nc;
    FILE *fp;

    fp = bid->fp;
    nb = 0;

    if ((fp != NULL) && (s != NULL))
       {nc = strlen(s);
	nb = _SC_blwrite((void *) s, 1, nc, (FILE *) bid);};

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

    ret = setvbuf(fp, buf, type, size);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BGETC - method for fgetc */

static int _SC_bgetc(FILE *fp)
   {int rv;

    ACCESS(fp);

    rv = fgetc(fp);

    _SC_bset_addr(bid, bid->curr + 1, TRUE, "getc");

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BUNGETC - method for ungetc */

static int _SC_bungetc(int c, FILE *fp)
   {int rv;

    ACCESS(fp);

    rv = ungetc(c, fp);

    _SC_bset_addr(bid, bid->curr - 1, TRUE, "ungetc");

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BEOF - method for feof */

static int _SC_beof(FILE *fp)
   {int rv;

    ACCESS(fp);

    rv = feof(fp);

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
       {pos = _SC_btell((FILE *) bid);
	nbr = fread(s, 1, n, fp);
	if (nbr < n)
	   s[nbr] = '\0';

/* check for newlines */
	if (nbr > 0)
	   {for (i = 0; (i < n) && (s[i] != '\n') && (s[i] != '\r'); i++);
	    nb = i + 1;
	    if (nb < n)
	       s[nb] = '\0';

	    nb = min(nb, nbr);
	    fseek(fp, pos + nb, SEEK_SET);};
       
	r = (nbr > 0) ? s : NULL;};

    if (r != NULL)
       _SC_bset_addr(bid, bid->curr + strlen(r), TRUE, "gets");

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BCLOSE - method for fclose */

static int _SC_bclose(FILE *fp)
   {int ret;

    ACCESS(fp);

    ret = FALSE;

    if (fp != NULL)
       ret = fclose(fp);

    SFREE(bid);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BFLUSH - method for fflush */

static int _SC_bflush(FILE *fp)
   {int ret;

    ACCESS(fp);

    ret = FALSE;

    if (fp != NULL)
       {ret = fflush(fp);
	_SC_bset_addr(bid, ftell(fp), TRUE, "flush");};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BOPEN - small file method for fopen */

FILE *SC_bopen(char *name, char *mode)
   {FILE *ret, *fp;
    bf_io_desc *bid;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {bid        = FMAKE(bf_io_desc, "SC_BOPEN:bid");
            bid->sz    = SC_file_size(fp);
	    bid->curr  = 0;
	    bid->stack = NULL;
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
    bf_io_desc *bid;
    file_io_desc *fid;

    ret = NULL;

    if (name != NULL)
       {fp = fopen(name, mode);
	if (fp != NULL)
	   {bid        = FMAKE(bf_io_desc, "SC_LBOPEN:bid");
            bid->sz    = SC_file_size(fp);
	    bid->curr  = 0;
	    bid->stack = NULL;
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

