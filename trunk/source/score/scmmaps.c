/*
 * SCMMAPS.C - support routines for using memory mapped I/O
 *           - GOALS:
 *           -   1) fast I/O based on MMAP
 *           -   2) high performance/throughput
 *           -         map user specifiable segment sizes at a time
 *           -         to control throughput - not whole file
 *           -         permits very high speed streaming to and from disk
 *           -   3) wrap to fit standard C library I/O set
 *           -   4) support 64 bit files (> 2 GB) on 32 bit OSes
 *           -   5) user tunable
 *           -         _SC_mmf.initial_length (variable)
 *           -         _SC_mmf.max_extend     (variable)
 *           -         SC_mf_set_size         (function)
 *           -         SC_mf_segment_size     (function)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score.h"

#ifdef HAVE_MMAP

#include "scope_proc.h"
#include "scope_mmap.h"
#include <sys/mman.h>

SC_scope_mmap
 _SC_mmf = { 1048576,        /* 1 << 20 ~ 1 MB */
             134217728 };    /* 1 << 27 ~ 100 MB */

/*--------------------------------------------------------------------------*/

/*                           FILE BLOCK ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _SC_MAKE_FILE_BLOCK - initialize a block segment of a mapped file */

static SC_file_block *_SC_make_file_block(SC_mapped_file *file, int64_t off,
					  int64_t start, int64_t end,
					  SC_file_block *nxt)
   {SC_file_block *bl;

    bl = CMAKE(SC_file_block);

    bl->off   = off;
    bl->start = start;
    bl->end   = end;
    bl->file  = file;
    bl->next  = nxt;

    return(bl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_FILE_BLOCK - release a list of block segments of a mapped file */

static void _SC_free_file_block(SC_file_block *bl)
   {SC_file_block *pb, *nxt;

    if (SC_safe_to_free(bl))
       {for (pb = bl; pb != NULL; pb = nxt)
	    {nxt = pb->next;
	     CFREE(pb);};}

    else
       {CFREE(bl);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ADD_BLOCK - add a block to the end of the list */

static SC_file_block *_SC_add_block(SC_file_block *lst, SC_file_block *bl)
   {SC_file_block *pb;

    if (lst == NULL)
       lst = bl;

    else
       {for (pb = lst; pb->next != NULL; pb = pb->next);
	pb->next = bl;};

    bl->next = NULL;

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPLIT_BLOCK - split the given block at the specified point
 *                 - inserting a new block into the list
 */

static SC_file_block *_SC_split_block(SC_file_block *bl, int p)
   {int64_t as, ae, ao;
    int64_t bs, be, bo;
    int64_t cs, ce, co;
    SC_file_block *nbl;

    if (bl != NULL)
       {bs = bl->start;
        be = bl->end;
	bo = bl->off;

	as = bs;
	ae = p;
	ao = bo;

	cs = p;
	ce = be;
	co = bo + (ae - as);

/* the new block is on the right */
	nbl = _SC_make_file_block(bl->file, co, cs, ce, bl->next);

/* the old block is on the left */
        bl->start = as;
	bl->end   = ae;
	bl->off   = ao;
	bl->next  = nbl;};

    return(bl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIND_AND_SPLIT - given the list BL find the block containing A and
 *                    - split it iff A is properly contained
 */

static SC_file_block *_SC_find_and_split(SC_file_block *bl, int64_t a)
   {int64_t sb, se;
    SC_file_block *pb;

    for (pb = bl; pb != NULL; pb = pb->next)
        {sb = pb->start;
	 se = pb->end;
	 if ((sb < a) && (a < se))
	    {_SC_split_block(pb, a);
	     break;};};

    return(pb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RESET_BLOCKS - reset the starts and ends of the given block list */

static void _SC_reset_blocks(SC_file_block *bl)
   {int64_t oe, nb, ne;
    SC_file_block *pb, *nxt;

/* shift the start and ends of the remaining blocks */
     if (bl != NULL)
        {bl->end  -= bl->start;
	 bl->start = 0;

	 for (pb = bl; pb->next != NULL; pb = nxt)
	     {oe = pb->end;

	      nxt = pb->next;

	      nb = nxt->start;
	      ne = nxt->end;

	      nxt->start = oe;
	      nxt->end   = oe + (ne - nb);};};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPMFBLOCK - diagnostic print of the block list */

void dpmfblock(SC_mapped_file *mf)
   {int i;
    int64_t ln, bo, db, bs, be;
    SC_file_block *bl, *pb;

    bl = mf->map;
   
    io_printf(stdout, "               Physical                    Logical\n");
    io_printf(stdout, " #        Offset       Length        Start          End\n");

    ln = 0L;
    for (i = 1, pb = bl; pb != NULL; pb = pb->next, i++)
        {bo = pb->off;
	 bs = pb->start;
	 be = pb->end;
	 db = be - bs;

	 io_printf(stdout, "%3d %12ld %12ld %12ld %12ld\n",
		   i, (long) bo, (long) db, (long) bs, (long) be);

	 ln += db;};


    io_printf(stdout, "File length = %12ld\n", (long) ln);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_MAKE - initialize a memory mapped file structure */

SC_mapped_file *SC_mf_make(char *name, int prot, int shar, int perm,
			   int extend, void (*setup)(SC_mapped_file *mf))
   {SC_mapped_file *mf;

    mf = CMAKE(SC_mapped_file);

    mf->name    = CSTRSAVE(name);
    mf->fd      = -1;
    mf->prot    = prot;
    mf->share   = shar;
    mf->perm    = perm;
    mf->extend  = extend;
    mf->page    = getpagesize();
    mf->lcpos   = 0L;
    mf->lclen   = 0L;
    mf->lcposx  = 0L;
    mf->lcoff   = 0L;
    mf->scsize  = 0L;
    mf->scsizex = 0L;
    mf->scp     = NULL;
    mf->map     = NULL;

    if (setup == NULL)
       setup = _SC_mf_setup;

    mf->setup = setup;

    (*setup)(mf);

    return(mf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_SET_PROP - set various file members before working with the file */

void SC_mf_set_prop(SC_mapped_file *mf, int fd, void *p, int64_t len)
   {int ns, pgsz;
    int64_t ssz, szmx;
    SC_file_block *bl;

    bl = _SC_make_file_block(mf, 0L, 0L, len, NULL);

    pgsz = mf->page;
    szmx = _SC_mmf.max_extend;
    ns   = (szmx + pgsz - 1)/pgsz;
    szmx = ns*pgsz;

    ssz = min(len, szmx);
    ssz = max(ssz, pgsz);

    mf->fd      = fd;
    mf->lclen   = len;
    mf->scsize  = ssz;
    mf->scsizex = szmx;
    mf->scp     = p;
    mf->map     = bl;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_EXT_FILE - physically extend the file FD to length LEN */

static void *_SC_mf_ext_file(SC_mapped_file *mf, int fd,
			     int64_t len, int64_t off, int64_t sz)
   {int err, prot, shar;
    char c;
    void *p;

    prot = mf->prot;
    shar = mf->share;

    p   = NULL;
    c   = '\0';
    err = (*mf->mwritef)(fd, &c, 1, len);
    if (err == 1)
       {p = (*mf->mmapf)(0, sz, prot, shar, fd, off);
	if (p == (void *) -1)
	   {perror("_SC_MF_EXT_FILE");
	    p = NULL;};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_CREATE_FILE - create a file with mmap */

static SC_mapped_file *_SC_mf_create_file(char *name, int64_t len, int extend,
					  void (*setup)(SC_mapped_file *mf))
   {int fd, action, prot, shar;
    int64_t nbp;
    void *p;
    SC_mapped_file *mf;

    action = O_RDWR | O_CREAT;
    prot   = PROT_READ | PROT_WRITE;
    shar   = MAP_SHARED;
	
    mf = SC_mf_make(name, prot, shar, action, extend, setup);

    p  = NULL;
    fd = (*mf->mopenf)(name, action, 0666);
    if (fd >= 0)
       {nbp = mf->page;
	len = (len + nbp - 1) & ~(nbp - 1);
	p   = _SC_mf_ext_file(mf, fd, len, 0, len);
	if (p != NULL)
	   SC_mf_set_prop(mf, fd, p, len);};

    return(mf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_MAP_FILE - map a file into memory */

static SC_mapped_file *_SC_mf_map_file(char *name, int action, int extend,
				       void (*setup)(SC_mapped_file *mf))
   {int fd, prot, shar, create;
    int rw, wo, cr;
    int64_t len, off;
    size_t ssz;
    void *p;
    struct stat s;
    SC_mapped_file *mf;

    rw = O_RDWR;
    wo = O_WRONLY;
    cr = O_CREAT;

    create = (action & cr);

    if (create)
       mf = _SC_mf_create_file(name, _SC_mmf.initial_length, extend, setup);

    else
       {shar = MAP_SHARED;
	prot = PROT_READ;
	if (action & rw)
	   prot = (PROT_READ | PROT_WRITE);
	if (action & wo)
	   prot = PROT_WRITE;

	mf = SC_mf_make(name, prot, shar, action, extend, setup);

	fd = (*mf->mopenf)(name, action, 0666);
	if (fd >= 0)
	   {p = NULL;
	    if (fstat(fd, &s) == 0)
	       {len = s.st_size;
		ssz = min(len, _SC_mmf.max_extend);
		off = 0L;

		p = (*mf->mmapf)(0, ssz, prot, shar, fd, off);
		if (p == (void *) -1)
		   {perror("_SC_MF_MAP_FILE");
		    p = NULL;};};

	    if (p != NULL)
	       SC_mf_set_prop(mf, fd, p, len);};};

    return(mf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_OPEN - open a memory mapped file ala fopen */

FILE *_SC_mf_open(char *name, char *mode, void (*setup)(SC_mapped_file *mf))
   {FILE *fp;

#ifdef HAVE_MMAP

    int action, extend, exists;
    int rw, ro, wo, cr;
    SC_mapped_file *mf;
    
/* possibilities are: r, r+, w, w+, a, a+
 * with optional b, e.g. rb+ or r+b
 *
 *       r      Open  text  file  for reading.  The stream is posi-
 *              tioned at the beginning of the file.
 *
 *       r+     Open for reading and writing.  The stream is  posi-
 *              tioned at the beginning of the file.
 *
 *       w      Truncate  file  to  zero length or create text file
 *              for writing.   The  stream  is  positioned  at  the
 *              beginning of the file.
 *
 *       w+     Open  for reading and writing.  The file is created
 *              if it does not exist, otherwise  it  is  truncated.
 *              The  stream  is  positioned at the beginning of the
 *              file.
 *
 *       a      Open for writing.  The file is created if  it  does
 *              not  exist.  The stream is positioned at the end of
 *              the file.
 *
 *       a+     Open for reading and writing.  The file is  created
 *              if  it does not exist.  The stream is positioned at
 *              the end of the file.
 */

    if ((name == NULL) || (mode == NULL))
       return(NULL);

    rw = O_RDWR;
    ro = O_RDONLY;
    wo = O_WRONLY;
    cr = O_CREAT;

    extend = (strchr(mode, '+') != NULL);

    exists = SC_isfile(name);

    mf = NULL;

    switch (*mode)
       {case 'r':
	     if (exists)
                {action = (extend) ? rw : ro;
		 mf     = _SC_mf_map_file(name, action, extend, setup);};
             break;

        case 'w':
	     if (exists)
	        SC_remove(name);

	     action  = (extend) ? rw : wo;
	     action |= cr;
	     mf      = _SC_mf_map_file(name, action, TRUE, setup);
             break;

        case 'a':
	     action = (extend) ? rw : wo;
	     if (!exists)
	        action |= cr;

	     mf = _SC_mf_map_file(name, action, TRUE, setup);
	     (*mf->fid.fseek)((FILE *) mf, 0L, SEEK_END);
             break;};

    fp = (FILE *) mf;

#else

    fp = SC_fopen(name, mode);

#endif

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_FREE - release a memory mapped file structure */

void SC_mf_free(SC_mapped_file *mf)
   {

    _SC_free_file_block(mf->map);

    CFREE(mf->name);
    CFREE(mf);

    return;}

/*--------------------------------------------------------------------------*/

/*                            INTERNAL ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _SC_MF_PTR - return the pointer to the mapped file data */

static char *_SC_mf_ptr(void *fp)
   {int64_t ln, off;
    char *base, *data;
    SC_mapped_file *mf;
    file_io_desc *fid;

    mf  = (SC_mapped_file *) fp;
    fid = &mf->fid;

/* this is the logical address */
    ln = (*fid->lftell)(fp);

    off  = mf->lcoff;
    ln  -= off;
    base = (char *) mf->scp;

    data = base + ln;

    return(data);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_REMAP - if the address AD is outside of the current
 *              - segment then remap to a new segment containing AD
 */

static void _SC_mf_remap(SC_mapped_file *mf, int64_t ad, int seta)
   {int rv;
    int64_t sts, stn, len;
    int64_t nbc, nbn, nbp;
    unsigned char *p;

    p   = mf->scp;
    nbn = mf->scsizex;
    nbp = mf->page;
    len = mf->lclen;
    sts = mf->lcoff;
    nbc = mf->scsize;

    stn  = ad/nbn;
    stn *= nbn;
    if (stn != sts)
       {if (len < stn)
	   rv = FALSE;

        else if (len < stn+nbn)
	   {nbn = len - stn;
	    nbn = (nbn + nbp - 1) & ~(nbp - 1);};

/* unmap the old segment */
	if (p != NULL)
	   {rv = munmap(p, nbc);
	    SC_ASSERT(rv == 0);};

	p = NULL;

/* map the next segment */
	p = (*mf->mmapf)(0, nbn, mf->prot, mf->share, mf->fd, stn);
	if (p == (void *) -1)
	   {perror("_SC_MF_REMAP");
	    p   = NULL;
	    ad  = len;
	    stn = len;
	    nbn = 0L;};

	mf->scp    = p;
	mf->lcoff  = stn;
	mf->scsize = nbn;};

    if (seta == TRUE)
       mf->lcpos = ad;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_UNMAP - unmap the file only */

static void _SC_mf_unmap(SC_mapped_file *mf)
   {int action, err, st;
    int64_t len, sz;
    char *p;
    FILE *fp;
    file_io_desc *fid;

    fp  = (FILE *) mf;
    fid = &mf->fid;

    action = mf->perm;
    len    = mf->lcposx;
    sz     = mf->scsize;
    p      = mf->scp;

    (*fid->fflush)(fp);

    munmap(p, sz);
    mf->scp = NULL;

    if (action & O_CREAT)
       {st = truncate(mf->name, len);
	SC_ASSERT(st == 0);};

    err = close(mf->fd);
    SC_ASSERT(err == 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_EXTEND - extend a file with mmap */

static void _SC_mf_extend(SC_mapped_file *mf, int64_t nb)
   {int fd, extend;
    int64_t pos, ne, ns, np, ln, sz, nsz, pgsz;
    char *name, *p;
    FILE *fp;
    SC_file_block *pb;

    fp = (FILE *) mf;

    pos = (*mf->fid.lftell)(fp);
    ne  = pos + nb;
    ln  = mf->lclen;

    extend = mf->extend;

    if ((ne > ln) && extend)
       {name = mf->name;
	fd   = mf->fd;
	p    = mf->scp;
	sz   = mf->scsizex;
	pgsz = mf->page;

	ns = ne - ln;
	ns = max(ns, _SC_mmf.max_extend);
	np = (ns + pgsz - 1)/pgsz;
	ns = ln + np*pgsz;

	nsz = mf->scsize;
	nsz = max(nsz, ns);
	nsz = min(nsz, sz);

/* unmap and close the file */
	_SC_mf_unmap(mf);

/* reopen and resize the file */
	fd = (*mf->mopenf)(name, O_RDWR, 0666);
	p  = _SC_mf_ext_file(mf, fd, ns, mf->lcoff, nsz);

	mf->scp    = p;
	mf->lclen  = ns;
	mf->fd     = fd;
	mf->scsize = nsz;

	for (pb = mf->map; pb->next != NULL; pb = pb->next);
	pb->end = ns;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_SEEK_ABS - seek to the specified absolute logical position N
 *                 - in the file MF
 *                 - return the new position if successful
 *                 - and -1 on failure
 */

static int64_t _SC_mf_seek_abs(SC_mapped_file *mf, int64_t n)
   {int rv;
    int64_t sts, stn, len;
    int64_t nbc;

/* find out where we need to be in logical terms */
    len = mf->lclen;
    sts = mf->lcoff;
    nbc = mf->scsize;

    rv = TRUE;

    if (n == len)
       stn = n;

/* seeking postion outside the current stripe */
    else if ((n < sts) || (sts+nbc <= n))
       {_SC_mf_remap(mf, n, TRUE);

	stn = mf->lcoff;
	n   = mf->lcpos;}

    else
       stn = sts;

    mf->lcoff  = stn;
    mf->lcpos  = n;
    mf->lcposx = max(mf->lcposx, n);

    rv = (rv) ? n : -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_TELL_OFFSET - return the offset from the beginning
 *                    - of the currently mapped segment
 *                    - of the specified physical address AD
 *                    - remap if necessary
 */

static char *_SC_mf_tell_offset(SC_mapped_file *mf, int64_t lp)
   {int64_t mo, sz, na;
    char *p;

    mo = mf->lcoff;
    sz = mf->scsize;

    if ((lp < mo) || (mo + sz <= lp))
       {na  = lp/sz;
	na *= sz;
	_SC_mf_remap(mf, na, FALSE);};

    p  = (char *) mf->scp;
    p += (lp - mf->lcoff);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_TELL_PHYSICAL - return a pointer to the physical location
 *                       - of the current logical file position
 *                       - which means the offset from beginning of
 *                       - the currently mapped segment
 *                       - this looks through the block list
 *                       - which defines the mapping from the physical
 *                       - view to the logical view
 *    
 */

static char *_SC_mf_tell_physical(SC_mapped_file *mf)
   {int64_t ll, lp, so, sb, se;
    SC_file_block *pb;
    SC_mapped_file *md;
    FILE *fp;
    char *p;

    fp = (FILE *) mf;
    p  = NULL;

/* this is the logical address */
    ll = (*mf->fid.lftell)(fp);

/* now find the physical address by looking through the block list */
    if (mf->map == NULL)
       p = _SC_mf_tell_offset(mf, ll);

    else
       {lp = 0L;
	for (pb = mf->map; pb != NULL; pb = pb->next)
	    {sb = pb->start;
	     se = pb->end;
	     if ((sb <= ll) && (ll < se))
	        {so = pb->off;
		 md = pb->file;
		 lp = ll - sb + so;
		 p  = _SC_mf_tell_offset(md, lp);
		 break;};};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_WRT - do the memory copies for a write operation */

static void _SC_mf_wrt(SC_mapped_file *mf, char *dst, void *a, int64_t ni)
   {long nb;
    int64_t pos, nba, len, off, sz, awr;
    char *src;
    FILE *fp;
    SC_io_buffer *bf;

    fp  = (FILE *) mf;
    bf  = (SC_io_buffer *) a;
    pos = bf->position;
    src = ((char **) bf->vrs)[0] + pos;

    bf->position += ni;

    nba = mf->lcpos;
    len = mf->lclen;
    off = mf->lcoff;
    sz  = mf->scsize;

/* in case we are close to the end of the file */
    if (len < nba + ni)
       {nb = (long) (len - nba - 1);

	if ((dst != NULL) && (nb > 0))
           memcpy(dst, src, nb);
	(*mf->fid.lfseek)(fp, nb, SEEK_CUR);}

    else
       {awr = nba - off;

/* copy the data in chunks of the stripe size
 * modulo the beginning and end cases
 */
	for ( ; ni > 0; ni -= nb)
	    {nb  = (long) (sz - awr);
	     nb  = min(nb, ni);
	     awr = 0;

	     memcpy(dst, src, nb);

	     (*mf->fid.lfseek)(fp, nb, SEEK_CUR);

	     dst  = _SC_mf_tell_physical(mf);
	     src += nb;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_RD - do the memory copies for a read operation */

static void _SC_mf_rd(SC_mapped_file *mf, char *src, void *a, int64_t ni)
   {long nb;
    int64_t pos, nba, len, off, sz, ard;
    char *dst;
    FILE *fp;
    SC_io_buffer *bf;
    file_io_desc *fid;

    fp  = (FILE *) mf;
    bf  = (SC_io_buffer *) a;
    pos = bf->position;
    dst = ((char **) bf->vrs)[0] + pos;

    bf->position += ni;

    nba = mf->lcpos;
    len = mf->lclen;
    off = mf->lcoff;
    sz  = mf->scsize;
    fid = &mf->fid;

/* in case we are close to the end of the file */
    if (len < nba + ni)
       {nb = (long) (len - nba);

	if ((src != NULL) && (nb > 0))
	   memcpy(dst, src, nb);
	(*fid->lfseek)(fp, nb, SEEK_CUR);}

    else
       {ard = nba - off;

/* copy the data in chunks of the stripe size
 * modulo the beginning and end cases
 */
	for ( ; ni > 0; ni -= nb)
	    {nb  = (long) (sz - ard);
	     nb  = min(nb, ni);
	     ard = 0;

	     memcpy(dst, src, nb);

	     (*fid->lfseek)(fp, nb, SEEK_CUR);

	     src  = _SC_mf_ptr(mf);
	     dst += nb;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_LENGTH - return the length of the file according
 *               - to the block list
 */

static int64_t _SC_mf_length(SC_mapped_file *mf)
   {int64_t ln;
    SC_file_block *bl, *pb;

    ln = 0L;
    if (mf != NULL)
       {bl = mf->map;
   
	for (pb = bl; pb != NULL; pb = pb->next)
	    ln += (pb->end - pb->start);};

    return(ln);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_LENGTH - return the length of the file according
 *              - to the block list
 */

int64_t SC_mf_length(FILE *fp)
   {int64_t ln;
    SC_mapped_file *mf;

    mf = FILE_IO_INFO(SC_mapped_file, fp);

    ln = _SC_mf_length(mf);

    return(ln);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_TRAVERSE - traverse a memory mapped file and
 *                 - apply the function FNC at the appropriate
 *                 - positions in the file
 *                 - if EXT is TRUE attempts to extend the file may be made
 *                 - return the number of bytes operated on
 */

static int64_t _SC_mf_traverse(PFMMTrav fnc, void *a, int ext, int64_t nb,
			      SC_mapped_file *mf)
   {int64_t nbo, ln;
    int64_t ab, ae, so, sb, se, nbs;
    char *s;
    SC_file_block *pb;
    FILE *fp;
    SC_mapped_file *md;
    file_io_desc *fid;

    nbo = 0;
    if (mf != NULL)
       {fid = &mf->fid;
	fp  = (FILE *) mf;

	ab = (*fid->lftell)(fp);
	ae = ab + nb;

	if (ext)
	   _SC_mf_extend(mf, nb);

	for (pb = mf->map; (pb != NULL) && (ab < ae); pb = pb->next)
	    {so = pb->off;
	     sb = pb->start;
	     se = pb->end;
	     md = pb->file;
	 
	     if ((sb <= ab) && (ab <= se))
	        {(*fid->lfseek)((FILE *) md, so + ab - sb, SEEK_SET);
		 s = _SC_mf_ptr(md);

/* if the end of the requested data access is within the current block */
		 if ((sb <= ae) && (ae <= se))
		    {nbs = nb - nbo;
		     ab  = ae;}

/* if this is the last block and it does not cover to the end
 * of the file extend the block accordingly
 */
		 else if ((pb->next == NULL) && (pb->end != mf->lclen))
		    {nbs = nb - nbo;
		     ab  = ae;
		     pb->end = ae;}

/* operate on the segment of the requested data that is within this block */
		 else
		    {nbs = se - ab;
		     ab  = se;};
		 
		 (*fnc)(md, s, a, nbs);

		 nbo += nbs;};};

	if (!ext)
	   {ln = _SC_mf_length(mf);
	    ae = min(ae, ln);};

	(*fid->lfseek)(fp, ae, SEEK_SET);};

    return(nbo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_TRAVERSE - traverse a memory mapped file and
 *                - apply the function FNC at the appropriate
 *                - positions in the file
 *                - if EXT is TRUE attempts to extend the file may be made
 *                - return the number of bytes operated on
 */

int64_t SC_mf_traverse(PFMMTrav fnc, void *a, int ext, int64_t nb, FILE *fp)
   {int64_t nbo;
    SC_mapped_file *mf;

    mf  = FILE_IO_INFO(SC_mapped_file, fp);
    nbo = _SC_mf_traverse(fnc, a, ext, nb, mf);

    return(nbo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_RD_STRIPE - worker level routine to read the data from
 *                  - a now defined location
 *                  - return the number of items read
 */

static void _SC_mf_rd_stripe(SC_mapped_file *mf, char *s, void *a, int64_t nb)
   {int i, iv, nv, nir, ni, nra;
    int nbr, bpi, bpu, stride;
    long l;
    unsigned char *pcs, *pcd;
    unsigned short *pss, *psd;
    unsigned long *pls, *pld;
    double *pds, *pdd;
    void **vrs;
    SC_io_buffer *bf;

    bf  = (SC_io_buffer *) a;
    nbr = bf->position;
    bpi = bf->item_size;
    bpu = bf->stride;
    nv  = bf->nv;
    vrs = bf->vrs;

    stride = bpu/bpi;
    nra    = nbr/bpu;
    ni     = nb/bpu;

    nir = ni;
     
    switch (bpi)
       {case 1 :
	       pcs = (unsigned char *) s;
	       for (i = 0; i < ni; i++)
		   {l = i*stride;
		    for (iv = 0; iv < nv; iv++)
		        {pcd    = ((unsigned char **) vrs)[iv] + nra;
			 pcd[i] = pcs[l + iv];};};
	       break;

	case 2 :
	       pss = (unsigned short *) s;
	       for (i = 0; i < ni; i++)
		   {l = i*stride;
		    for (iv = 0; iv < nv; iv++)
		        {psd    = ((unsigned short **) vrs)[iv] + nra;
			 psd[i] = pss[l + iv];};};
	       break;

	case 4 :
	       pls = (unsigned long *) s;
	       for (i = 0; i < ni; i++)
		   {l = i*stride;
		    for (iv = 0; iv < nv; iv++)
		        {pld    = ((unsigned long **) vrs)[iv] + nra;
			 pld[i] = pls[l + iv];};};
	       break;

	case 8 :
	       pds = (double *) s;
	       for (i = 0; i < ni; i++)
		   {l = i*stride;
		    for (iv = 0; iv < nv; iv++)
		        {pdd    = ((double **) vrs)[iv] + nra;
			 pdd[i] = pds[l + iv];};};
	       break;

        default :
               nir = -1;};

    bf->position += nir*bpu;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_CORE_READ - read from a memory mapped file
 *                  - ala the read system call
 */

u_int64_t _SC_mf_core_read(void *bf, size_t sz, u_int64_t ni, FILE *fp)
   {u_int64_t nir;

#ifdef HAVE_MMAP

    int64_t nb, nbr;
    SC_io_buffer iobf;
    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;
    nb = sz*ni;

    iobf.mf        = mf;
    iobf.position  = 0;
    iobf.item_size = 1;
    iobf.stride    = 1;
    iobf.nv        = 1;
    iobf.vrs       = &bf;

    nbr = _SC_mf_traverse(_SC_mf_rd, &iobf, FALSE, nb, mf);
    nir = nbr/sz;

#else

    nir = io_read(bf, sz, ni, (FILE *) fp);

#endif

    return(nir);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_CORE_WRITE - write to a memory mapped file
 *                   - ala the write system call
 */

u_int64_t _SC_mf_core_write(void *bf, size_t sz, u_int64_t ni, FILE *fp)
   {u_int64_t niw;

#ifdef HAVE_MMAP

    int64_t nb, nbw;
    SC_io_buffer iobf;
    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;
    nb = sz*ni;

    iobf.mf        = mf;
    iobf.position  = 0;
    iobf.item_size = 1;
    iobf.stride    = 1;
    iobf.nv        = 1;
    iobf.vrs       = &bf;

    nbw = _SC_mf_traverse(_SC_mf_wrt, &iobf, TRUE, nb, mf);
    niw = nbw/sz;

#else

    niw = io_write(bf, sz, ni, (FILE *) fp);

#endif

    return(niw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_CORE_SEEK - position the current location
 *                  - ala the fseek system call
 */

int _SC_mf_core_seek(FILE *fp, int64_t offset, int whence)
   {int ret;

#ifdef HAVE_MMAP

    int err;
    int64_t naddr, ln, pos;
    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;

/* NOTE: must use actual file length not the logical length
 * derived from the map!!!
 */
    ln    = mf->lclen;
    pos   = mf->lcpos;
    naddr = 0;

    switch (whence)
       {case SEEK_SET :
	     naddr = offset;
	     break;

        case SEEK_CUR :
	     naddr = pos + offset;
	     break;

        case SEEK_END :
	     naddr = ln + offset;
	     break;};

    if (naddr > ln)
       _SC_mf_extend(mf, naddr);

    if (naddr != mf->lcpos)
       {err = _SC_mf_seek_abs(mf, naddr);
	ret = (err == -1) ? -1 : 0;}
    else
       ret = 0;

#else

    ret = io_seek((FILE *) fp, offset, whence);

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_CORE_TELL - return the current logical location
 *                  - ala the ftell C library call
 *                  - NOTE: u_int64_t and int64_t are the same size
 *                  - when using large files so use standard int64_t
 */

int64_t _SC_mf_core_tell(FILE *fp)
   {int64_t pos;

#ifdef HAVE_MMAP

    SC_mapped_file *mf;

    mf  = (SC_mapped_file *) fp;
    pos = mf->lcpos;

#else

    pos = io_tell((FILE *) fp);

#endif

    return(pos);}

/*--------------------------------------------------------------------------*/

/*                               API ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* SC_MF_SET_SIZE - set the default byte size for creating mapped files */

void SC_mf_set_size(int64_t mnsz, int64_t extsz)
   {

    _SC_mmf.initial_length = mnsz;
    _SC_mmf.max_extend     = extsz;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_SEGMENT_SIZE - set the memory mapped segment size
 *                     - take NSZ as an initial guess and round it
 *                     - up to the nearest page size
 *                     - return the actual segment size used
 */

u_int64_t _SC_mf_segment_size(FILE *fp, int64_t nsz)
   {int64_t len, nbp;
    u_int64_t rv;
    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;

    len = mf->lclen;
    nbp = mf->page;

    nsz = min(nsz, len);

/* make the size be a multiple of the page size */
    nsz = (nsz + nbp - 1) & ~(nbp - 1);

    mf->scsize  = nsz;
    mf->scsizex = nsz;

    rv = (u_int64_t) nsz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_NAME - return the name of the mapped file */

char *SC_mf_name(FILE *fp)
   {char *fname;
    SC_mapped_file *mf;
    file_io_desc *fid;

    fid = (file_io_desc *) fp;
    mf  = (SC_mapped_file *) fid->info;

    fname = mf->name;

    return(fname);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_INSERT - insert a block NB of bytes into the logical map
 *              - of the specified file MF at OFF
 *              - return the new block
 */

SC_file_block *SC_mf_insert(FILE *fp, SC_file_block *nbl, int64_t off)
   {int64_t sb;
    SC_file_block *pb, *bl, *bls, *nxt;
    SC_mapped_file *mf;
    file_io_desc *fid;

    fid = (file_io_desc *) fp;
    mf  = (SC_mapped_file *) fid->info;

    bl = mf->map;

    _SC_find_and_split(bl, off);

/* each block is now either totally left or totally right
 * of the new block(s)
 */
    bls = NULL;
    for (pb = bl; pb != NULL; pb = nxt)
        {nxt = pb->next;

	 sb = pb->start;
	 if (sb == off)
	    bls = _SC_add_block(bls, nbl);

	 bls = _SC_add_block(bls, pb);};

    _SC_reset_blocks(bls);

    mf->map = bls;

    return(nbl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_DELETE - delete a block NB of bytes from the logical map
 *              - of the specified file MF at OFF
 *              - return the deleted block
 */

SC_file_block *SC_mf_delete(FILE *fp, int64_t off, int64_t nb)
   {int64_t sb, se, ab, ae;
    SC_file_block *pb, *bl, *bld, *bls, *nxt;
    SC_mapped_file *mf;
    file_io_desc *fid;

    fid = (file_io_desc *) fp;
    mf  = (SC_mapped_file *) fid->info;
    bl  = mf->map;

/* compute the region to delete */
    ab = off;
    ae = off + nb;

/* split the blocks containing the endpoints */
    _SC_find_and_split(bl, ab);
    _SC_find_and_split(bl, ae);

/* each block is now either totally in or totally out
 * so add it to bls or bld respectively
 */
    bld = NULL;
    bls = NULL;
    for (pb = bl; pb != NULL; pb = nxt)
        {nxt = pb->next;

	 sb = pb->start;
	 se = pb->end;
	 if ((ab <= sb) && (se <= ae))
	    bld = _SC_add_block(bld, pb);
	 else
	    bls = _SC_add_block(bls, pb);};

    _SC_reset_blocks(bls);
    _SC_reset_blocks(bld);

    mf->map = bls;

    return(bld);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_READ_STRIPED - read from a memory mapped file
 *                    - in a striped fashion (i.e. with a stride)
 *                    - return the total number of items read
 *                    -   OFF     offset of the first stripe in bytes
 *                    -   NI      number of stripes to scan
 *                    -   STRIDE  number of items of sizeof(type) in a stripe
 *                    -   NV      number of things to read from each stripe
 */

int64_t SC_mf_read_striped(FILE *fp, int64_t off, char *type,
			  int ni, int stride, int nv, void **vrs)
   {int bpu, bpi, nb, nbr;
    int64_t rv;
    SC_io_buffer iobf;
    SC_mapped_file *mf;
    file_io_desc *fid;

    rv = 0;

    mf = FILE_IO_INFO(SC_mapped_file, fp);
    if (mf != NULL)
       {fid = &mf->fid;
	if (fid != NULL)
	   {(*fid->lfseek)(fp, off, SEEK_SET);

	    bpi = SIZEOF(type);
	    bpu = stride*bpi;

	    iobf.mf        = mf;
	    iobf.position  = 0;
	    iobf.item_size = bpi;
	    iobf.stride    = bpu;
	    iobf.nv        = nv;
	    iobf.vrs       = vrs;

	    nb  = bpu*ni;
	    nbr = _SC_mf_traverse(_SC_mf_rd_stripe, &iobf, FALSE, nb, mf);

	    rv = nbr/bpu;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_DATA - return the pointer to the mapped file data */

char *_SC_mf_data(FILE *fp)
   {char *data;
    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;

    data = _SC_mf_tell_physical(mf);

    return(data);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_GETS - do an fgets on the mapped file
 *             - needs error checking
 */

char *_SC_mf_gets(char *s, int n, FILE *fp)
   {int nc;
    char *r;

#ifdef HAVE_MMAP

    int i, nbr, pos;
    SC_mapped_file *mf;
    file_io_desc *fid;

    r = NULL;

/* reserve the last character for a '\0' terminator */
    nc = n - 1;
    if (nc > 0)
       {mf  = (SC_mapped_file *) fp;
	fid = &mf->fid;

	if (fid->lftell != NULL)
	   pos = (*fid->lftell)(fp);
	else
	   pos = (*fid->ftell)(fp);

	if (fid->lfread != NULL)
	   nbr = (*fid->lfread)(s, 1, nc, fp);
	else
	   nbr = (*fid->fread)(s, 1, nc, fp);

/* check for newlines */
	if (nbr > 0)
	   {for (i = 0; (s[i] != '\n') && (i < nc); i++);
	    if (i < nc)
	       s[i+1] = '\0';

	    if (fid->lfseek != NULL)
	       (*fid->lfseek)(fp, pos + i + 1, SEEK_SET);
	    else
	       (*fid->fseek)(fp, pos + i + 1, SEEK_SET);};
       
	r = (nbr > 0) ? s : NULL;

/* always set last character to '\0' to avoid a buffer overrun */
	s[nc] = '\0';};

#else

    r = io_gets(s, n, fp);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_GETC - do an fgetc on the mapped file
 *             - needs error checking
 */

int _SC_mf_getc(FILE *fp)
   {int c;

#ifdef HAVE_MMAP

    int ln;
    char *base;
    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;

    ln   = mf->lcpos++;
    base = (char *) mf->scp;

    c = base[ln];

#else

    c = io_getc(fp);

#endif

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_UNGETC - do an fungetc on the mapped file
 *               - needs error checking 
 */

int _SC_mf_ungetc(int c, FILE *fp)
   {int i;

#ifdef HAVE_MMAP

    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;

    mf->lcpos--;

    i = c;

#else

    i = io_ungetc(c, fp);

#endif

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_PRINTF - do an fprintf on the mapped file */

int _SC_mf_printf(FILE *fp, char *fmt, va_list a)
   {int nbw;
    char s[MAXLINE];

    nbw = 0;

    if (fp != NULL)
       {nbw = (*SC_vsnprintf_hook)(s, MAXLINE, fmt, a);

#ifdef HAVE_MMAP
	{SC_mapped_file *mf;
	 file_io_desc *fid;

	 mf  = (SC_mapped_file *) fp;
	 fid = &mf->fid;
	 if (fid->fwrite != NULL)
	    nbw = (*fid->fwrite)(s, 1, nbw, fp);
	 else
	    nbw = (*fid->lfwrite)(s, 1, nbw, fp);};

#else
    
	nbw = io_write(s, 1, nbw, fp);

#endif

	};

    return(nbw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_PUTS - do an fputs on the mapped file */

int _SC_mf_puts(const char *s, FILE *fp)
   {int ret;

#ifdef HAVE_MMAP

    int64_t nbw, ln;
    SC_mapped_file *mf;
    file_io_desc *fid;

    mf  = (SC_mapped_file *) fp;
    fid = &mf->fid;

    ln  = strlen(s);
    if (fid->fwrite != NULL)
       nbw = (*fid->fwrite)((void *) s, 1, ln, fp);
    else
       nbw = (*fid->lfwrite)((void *) s, 1, ln, fp);
    ret = (nbw != ln) ? EOF : nbw;

#else

    ret = io_puts(s, fp);

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_SETVBUF - stub for setvbuf hook
 *                - this doesn't have meaning for mapped files
 */

int _SC_mf_setvbuf(FILE *fp, char *bf, int type, size_t sz)
   {int ret;

#ifdef HAVE_MMAP

    ret = 0;

#else
    
    ret = io_setvbuf(fp, bf, type, sz);

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_FLUSH - stub for flush hook */

int _SC_mf_flush(FILE *fp)
   {int ret;

#ifdef HAVE_MMAP

    int nbp;
    int64_t len;
    char *p;
    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;

    ret = 0;
    len = mf->lcposx;
    p   = mf->scp;
    nbp = mf->page;
    len = (len + nbp - 1) & ~(nbp - 1);
    len = min(len, mf->scsize);

    ret = msync(p, len, MS_SYNC);
    if (ret != 0)
       {int err;
	char msg[MAXLINE];

	err = errno;
	SC_strerror(err, msg, MAXLINE);
    
/* we seem to get lots of ENOMEM errors from x86_64 which also
 * appear safe to ignore
 */
	if (err == ENOMEM)
	   ret = 0;
	else
	   ret = EOF;};

#else
    
    ret = io_flush(fp);

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_EOF - return TRUE if the file is positioned at the end */

int _SC_mf_eof(FILE *fp)
   {int rv;
    int64_t se;
    SC_mapped_file *mf;
    SC_file_block *pb;

    mf = (SC_mapped_file *) fp;
    se = 0;

    for (pb = mf->map; pb != NULL; pb = pb->next)
        se = pb->end;

    rv = ((mf->scp == NULL) || (mf->lcpos >= se));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_CLOSE - close and unmap a mapped file */

int _SC_mf_close(FILE *fp)
   {int ret, err;

#ifdef HAVE_MMAP

    SC_mapped_file *mf;

    mf = (SC_mapped_file *) fp;

    _SC_mf_unmap(mf);

    err = close(mf->fd);
    SC_ASSERT(err == 0);

    SC_mf_free(mf);

    ret = 0;

#else

    ret = io_close(fp);

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_COPY - copy the contents of the mapped file MF to a new
 *            - mapped file and return a pointer to the new
 *            - mapped file
 */

FILE *SC_mf_copy(char *name, FILE *fp, int bckup)
   {int st;
    int64_t ln, so, sb, se, nb, pos;
    char *pi, *nnm, *onm;
    SC_mapped_file *nmf, *md, *mf;
    SC_file_block *pb;
    FILE *nfp;
    void (*setup)(SC_mapped_file *mf);
    file_io_desc *fid;

    fid = (file_io_desc *) fp;
    mf  = (SC_mapped_file *) fid->info;

    setup = mf->setup;
    pos   = (*fid->ftell)(fp);

    nnm = SC_dsnprintf(FALSE, ".sc-mf-XXXXXX");
    st = mkstemp(nnm);
    SC_ASSERT(st == 0);

    ln  = _SC_mf_length(mf);
    nmf = _SC_mf_create_file(nnm, ln, mf->extend, setup);
    nfp = (FILE *) nmf;
    
/* copy the contents */
    for (pb = mf->map; pb != NULL; pb = pb->next)
        {so = pb->off;
	 sb = pb->start;
	 se = pb->end;
	 md = pb->file;

	 nb = se - sb;

	 (*fid->fseek)((FILE *) md, so, SEEK_SET);
	 pi = _SC_mf_ptr(md);

	 (*fid->fwrite)(pi, 1, nb, nfp);};

    pos = min(pos, ln);

/* set the offset to be the same as it was */
    (*fid->fseek)(nfp, pos, SEEK_SET);

/* do appropriate file renaming */
    if (strcmp(name, mf->name) == 0)
       {if (bckup)
	   {onm = SC_dsnprintf(FALSE, "%s~", name);
	    unlink(onm);
	    rename(name, onm);}
	else
	   unlink(name);};

    rename(nnm, name);

    CFREE(nmf->name);
    nmf->name = CSTRSAVE(name);

/* close the original file */
    (*fid->fclose)(fp);

/* replace the info of FID with the new mapped file */
    fid->info = nmf;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

