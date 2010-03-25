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
   {BIGINT sz;
    BIGINT curr;
    SC_array *stack;
    FILE *fp;};

/*--------------------------------------------------------------------------*/

/*                           SMALL FILE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _SC_BSEEK - small file method for fseek */

static int _SC_bseek(FILE *fp, long offs, int wh)
   {int ret;
    off_t addr;
    struct stat buf;

    ACCESS(fp);

    switch (wh)
       {case SEEK_SET :
	     addr = offs;
             break;
        case SEEK_CUR :
             addr = bid->curr + offs;
             break;
        case SEEK_END :
	     fstat(fileno(fp), &buf);
             addr = buf.st_size + offs;
             break;};

    addr = max(addr, 0);
    bid->curr = addr;

    ret = fseek(fp, offs, wh);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BTELL - small file method for ftell */

static long _SC_btell(FILE *fp)
   {long ret;

    ACCESS(fp);

#if 0
    ret = ftell(fp);
#else
    ret = bid->curr;
#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BREAD - small file method for fread */

static size_t _SC_bread(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = SC_fread_sigsafe(s, bpi, nitems, fp);

    bid->curr += (nr*bpi);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BWRITE_ATM - atomic worker for _SC_bwrite */

static size_t _SC_bwrite_atm(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t nw;

    nw = SC_fwrite_sigsafe(s, bpi, nitems, fp);

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BWRITE - small file method for fwrite */

static size_t _SC_bwrite(void *s, size_t bpi, size_t nitems, FILE *fp)
   {int ok, flags, rw, fd;
    BIGINT i, nb;
    size_t nw, nr;
    off_t addr;
    char msg[MAXLINE];
    char *ps, *t;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    if (SC_verify_writes == TRUE)

/* eliminate stdin, stdout, and stderr */
       {fd = fileno(fp);
	rw = (fd > 2);

/* eliminate read-only and write-only streams */
	flags = SC_fd_flags(fd);
	rw   &= ((flags & O_RDWR) != 0);

/* remember where we are and write it out */
	addr = _SC_btell((FILE *) bid);
	if (addr < 0)
	   SC_strerror(errno, msg, MAXLINE);

	nw = _SC_bwrite_atm(s, bpi, nitems, fp);

/* go back and read it in
 * fseek will fail unless the file mode is read-write
 */
	if ((addr >= 0) && (nw > 0) && (rw == TRUE))
	   {nb = bpi*nitems;
	    t  = FMAKE_N(char, nb, "char*:_SC_bWRITE:t");

	    fseek(fp, addr, SEEK_SET);
	    nr = _SC_bread(t, bpi, nitems, fp);

/* compare */
	    ok = TRUE;
	    if (nr != nw)
	       ok = FALSE;
	    else
	       {ps = (char *) s;
		for (i = 0; (ok == TRUE) && (i < nb); i++)
		    ok = (ps[i] == t[i]);};

/* if they disagree move back to the start and return 0 */
	    if (ok == FALSE)
	       {nw = 0;
		fseek(fp, addr, SEEK_SET);};

	    SFREE(t);};}

    else
       nw = _SC_bwrite_atm(s, bpi, nitems, fp);

    bid->curr += (nw*bpi);

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
    struct stat buf;

    ACCESS(fp);

    switch (wh)
       {case SEEK_SET :
	     addr = offs;
             break;
        case SEEK_CUR :
             addr = bid->curr + offs;
             break;
        case SEEK_END :
	     fstat(fileno(fp), &buf);
             addr = buf.st_size + offs;
             break;};

    addr = max(addr, 0);
    bid->curr = addr;

    ret = fseeko(fp, offs, wh);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLTELL - large file method for ftell */

static off_t _SC_bltell(FILE *fp)
   {off_t ret;

    ACCESS(fp);

#if 0
    ret = ftello(fp);
#else
    ret = bid->curr;
#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLREAD - large file method for fread */

static BIGUINT _SC_blread(void *s, size_t bpi, BIGUINT nitems, FILE *fp)
   {size_t nr;

    ACCESS(fp);

    nr = SC_fread_sigsafe(s, bpi, nitems, fp);

    bid->curr += (nr*bpi);

    return(nr);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BLWRITE - large file method for fwrite */

static BIGUINT _SC_blwrite(void *s, size_t bpi, BIGUINT nitems, FILE *fp)
   {int ok, flags, rw, fd;
    BIGINT i, nb;
    size_t nw, nr;
    off_t addr;
    char msg[MAXLINE];
    char *ps, *t;

    ACCESS(fp);

/* turn off SIGIO handler */
    SC_catch_io_interrupts(FALSE);

    if (SC_verify_writes == TRUE)

/* eliminate stdin, stdout, and stderr */
       {fd = fileno(fp);
	rw = (fd > 2);

/* eliminate read-only and write-only streams */
	flags = SC_fd_flags(fd);
	rw   &= ((flags & O_RDWR) != 0);

/* remember where we are and write it out */
	addr = _SC_bltell((FILE *) bid);
	if (addr < 0)
	   SC_strerror(errno, msg, MAXLINE);

	nw = _SC_bwrite_atm(s, bpi, nitems, fp);

/* go back and read it in
 * fseek will fail unless the file mode is read-write
 */
	if ((addr >= 0) && (nw > 0) && (rw == TRUE))
	   {nb = bpi*nitems;
	    t  = FMAKE_N(char, nb, "char*:_SC_bWRITE:t");

	    fseek(fp, addr, SEEK_SET);
	    nr = _SC_bread(t, bpi, nitems, fp);

/* compare */
	    ok = TRUE;
	    if (nr != nw)
	       ok = FALSE;
	    else
	       {ps = (char *) s;
		for (i = 0; (ok == TRUE) && (i < nb); i++)
		    ok = (ps[i] == t[i]);};

/* if they disagree move back to the start and return 0 */
	    if (ok == FALSE)
	       {nw = 0;
		fseek(fp, addr, SEEK_SET);};

	    SFREE(t);};}

    else
       nw = _SC_bwrite_atm(s, bpi, nitems, fp);

    bid->curr += (nw*bpi);

/* turn on SIGIO handler */
    SC_catch_io_interrupts(SC_io_interrupts_on);

    return(nw);}
 
/*--------------------------------------------------------------------------*/

/*                           STANDARD FILE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_BOUT - worker for _SC_bputs and _SC_bwrite */

static int _SC_bout(const char *s, bf_io_desc *bid)
   {int i, nb, nc, ns, nz;
    char *p;
    FILE *fp;

    fp = bid->fp;

    nb = 0;

    if ((fp != NULL) && (s != NULL))
       {p  = (char *) s;
	nz = 0;
	for (i = 0, nc = strlen(s); nc > 0; nc -= ns, i++)
	    {ns = SC_fwrite_sigsafe(p, 1, nc, fp);

/* count the number of consecutive zero writes */
	     if (ns <= 0)
	        nz++;
	     else
	        nz = 0;

/* if we are not getting anywhere bail out and
 * return minus the number of chars written to signify an error
 * this is to be consistent with fputs error return
 */
	     if (nz > 3)
	        {nb = -nb;
		 nb = min(nb, -1);
		 break;};

/* wait a tenth of a second if the whole thing did not get sent
 * presumably because the system is clogged
 */
	     if (ns < nc)
	        SC_sleep(100);

	     nb += ns;
	     p  += ns;};};

    bid->curr += nb;

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

    ret = 0;

    if (fp != NULL)
       {msg = SC_vdsnprintf(TRUE, fmt, a);
	ret = _SC_bout(msg, bid);
	SFREE(msg);};

    FLUSH_ON(fp, NULL);

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

    bid->curr++;

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BUNGETC - method for ungetc */

static int _SC_bungetc(int c, FILE *fp)
   {int rv;

    ACCESS(fp);

    rv = ungetc(c, fp);

    bid->curr--;

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
       bid->curr += strlen(r);

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
       ret = fflush(fp);

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
	    bid->sz    = -1;
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
	    bid->sz    = -1;
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

