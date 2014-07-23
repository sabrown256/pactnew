/*
 * PDCSUM.C - checksum support routines for PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define PD_MD5_DIR       "/&md5"
#define PD_MD5_SUFFIX    "-md5"
#define PD_MD5_DIMS      "[16]"

#define PD_CKSUM_SIG     "MD5"
#define PD_CKSUM_SIG_LEN 3

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_COMPARE - compare checksums DA and DB */

static int _PD_cksum_compare(unsigned char *da, unsigned char *db)
   {int i, rv;

    rv = TRUE;

    for (i = 0; i < PD_CKSUM_LEN; i++) 
        {if (da[i] != db[i])
	    {rv = FALSE;
	     break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_CLOSE - handle checksum related chores on close
 *                 - return TRUE if successful and FALSE otherwise
 */

int _PD_cksum_close(PDBfile *file)
   {int ok, ret;

    ret = TRUE;

/* if someone turned off file checksums during their run, in a file that
 * already has file checksums in it, make sure we turn checksums back on
 * before we flush/close the file, so that a valid checksum is recomputed
 * and added to the file
 */
    if ((((file->cksum.use & PD_MD5_FILE) == 0) &&
	 ((file->cksum.file & PD_MD5_FILE) != 0)) &&
	(file->virtual_internal == FALSE))
       {ok = PD_verify(file);
        if (ok == FALSE)
	   file->cksum.use |= PD_MD5_FILE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_VAR_READ - handle checksum chores on variable read
 *                    - return the number of item successfully read
 */ 
 
int _PD_cksum_var_read(PDBfile *file, char *name, char *type,
		      syment *ep, void *vr)
   {int rv, ok;
    inti ni, len;
    intb bpi;
    unsigned char rdig[PD_CKSUM_LEN], cdig[PD_CKSUM_LEN];
    char *path;
            
    rv = TRUE;

/* if requested, check MD5 checksum for this var */
    if ((file->cksum.use & PD_MD5_RW) && (strstr(name, PD_MD5_DIR) == NULL)) 
       {len  = strlen(PD_MD5_DIR) + strlen(name) + strlen(PD_MD5_SUFFIX) + 2;
        path = CMAKE_N(char, len);

/* read checksum as /&md5/<varname> */
        snprintf(path, len, "%s%s", PD_MD5_DIR, name);
        
        if (PD_query_entry(file, path, NULL) == NULL)

/* if preferred form is not there check archaic form - /&md5/<varname>-md5 */
	   {snprintf(path, len, "%s%s%s", PD_MD5_DIR, name, PD_MD5_SUFFIX);
        
/* make sure &md5 var has been created */
	    if (PD_query_entry(file, path, NULL) == NULL)
	       PD_error("NO MD5 CHECKSUM AVAILABLE FOR THIS VARIABLE - _PD_CKSUM_VAR_READ",
			PD_READ);}
        else
           {ni  = _PD_comp_num(ep->dimensions);
            bpi = _PD_lookup_size(type, file->host_chart);
        
/* do checksum in local memory on vr just read */
            memset(cdig, 0, PD_CKSUM_LEN);
            PM_md5_checksum_array(vr, ni, bpi, cdig);
        
/* read previous checksum */
            PD_read(file, path, rdig);
        
            CFREE(path);
        
/* return -1 and set PD_error if MD5 mismatch */
	    ok = _PD_cksum_compare(cdig, rdig);
	    if (ok == FALSE)
	       {rv = -1;

/* NOTE: this must be treated as generic because of the PD_read above */
		PD_error("MD5 CHECKSUM OF VARIABLE FAILED - _PD_CKSUM_VAR_READ",
			 PD_GENERIC);};};};
                    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_VAR_WRITE - handle checksum chores on variable write
 *                     - the effective entry EF will have the info
 *                     - for blocks of the entry NAME to be checksummed
 */

int _PD_cksum_var_write(PDBfile *file, char *name, syment *ef)
   {int rv;
    inti i, nb, n, ni;
    int64_t addr;
    char bf[MAXLINE];
    char *s;
    syment *ep;
    SC_array *bl;
    PD_block_type iv;

    rv = TRUE;

/* if requested, add an MD5 checksum for this var */
    if ((ef != NULL) && (file->cksum.use & PD_MD5_RW))
       {if (strpbrk(name, ".[(") != NULL)
	   {SC_strncpy(bf, MAXLINE, name, -1);
	    name = SC_strtok(bf, ".[(", s);};

	ep = PD_inquire_entry(file, name, FALSE, NULL);

/* if the entry and the effective entry are not the same then 
 * the blocks of the entry indicated by the effective entry have
 * been touched and must have checksums recomputed
 */
	if (ep != ef)
	   {nb = _PD_n_blocks(ef);
	    bl = ef->blocks;
	    for (i = 0; i < nb; i++)
	        {_PD_block_get_desc(&addr, &ni, bl, i);
		 iv = _PD_block_get_valid(bl, i);
		 n  = _PD_block_find(file, ep, addr);

/* if the effective block has been marked invalid then recompute
 * the appropriate block of the entry
 */
		 if (iv == PD_BLOCK_INVALID)
		    _PD_cksum_block_write(file, ep, n);};};};
       
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_BLOCK_READ - find the checksum for the Nth block of EP
 *                      - and return it in CDIG
 *                      - we must get all of the data from the file
 *                      - because you might not have all of the
 *                      - data and it is hard to deal with the problems
 *                      - of type converted data
 *                      - return the TRUE iff successful
 */ 
 
int _PD_cksum_block_read(PDBfile *file, char *name, syment *ep, long n)
   {int rv, st;
    inti i, mn, mx, ni, nb;
    intb bpi;
    int64_t start, stop, addr;
    unsigned char cdig[PD_CKSUM_LEN], rdig[PD_CKSUM_LEN];
    char bf[MAXLINE];
    char *lname, *type;
    syment *epo;
    SC_array *bl, *blo;
    PD_block_type vl;
            
    rv = TRUE;

/* if requested, check MD5 checksum for this block */
    if ((ep != NULL) && (file->cksum.use & PD_MD5_RW))
       {bl = ep->blocks;
	nb = SC_array_get_n(bl);
	if (n < 0)
	   {mn = 0;
	    mx = nb;}
	else
	   {mn = n;
	    mx = n+1;};

	type = PD_entry_type(ep);
	bpi  = _PD_lookup_size(type, file->chart);

	lname = _PD_var_namef(NULL, name, bf, MAXLINE);
	epo   = PD_inquire_entry(file, lname, FALSE, NULL);
	if (epo != NULL)
	   {blo   = epo->blocks;

	    for (i = mn; i < mx; i++)
	        {_PD_block_get_desc(&addr, &ni, bl, i);
		 n  = _PD_block_find(file, epo, addr);
		 vl = _PD_block_get_valid(blo, n);
        
		 if ((vl == PD_BLOCK_INVALID) || (vl == PD_BLOCK_UNVERIFIED))

/* do checksum of block on disk */
		    {memset(cdig, 0, PD_CKSUM_LEN);
		     start = addr;
		     stop  = start + ni*bpi - 1;
		     PM_md5_checksum_file(file->stream, start, stop, cdig);

		     st  = _PD_block_get_cksum(blo, n, rdig);
		     st &= _PD_cksum_compare(cdig, rdig);
		     if (st == TRUE)
		        _PD_block_set_valid(blo, n, PD_BLOCK_VALID);
		     else
		        {_PD_block_set_valid(blo, n, PD_BLOCK_CORRUPT);
			 rv = FALSE;};}

		 else if (vl == PD_BLOCK_CORRUPT)
		    {rv = FALSE;
		     break;};};};};
                    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_BLOCK_WRITE - find the checksum for the Nth block of EP
 *                       - we must get all of the data from the file
 *                       - because you might not have all of the
 *                       - data and it is hard to deal with the problems
 *                       - of type converted data
 *                       - return the TRUE iff successful
 */ 
 
int _PD_cksum_block_write(PDBfile *file, syment *ep, long n)
   {int rv;
    inti ni;
    intb bpi;
    int64_t start, stop, addr;
    char *type;
    unsigned char cdig[PD_CKSUM_LEN];
    SC_array *bl;
            
    rv = TRUE;

/* if requested, check MD5 checksum for this block */
    if ((ep != NULL) && (file->cksum.use & PD_MD5_RW))
       {bl   = ep->blocks;
	type = PD_entry_type(ep);
	_PD_block_get_desc(&addr, &ni, bl, n);

	bpi   = _PD_lookup_size(type, file->chart);
	start = addr;
	stop  = start + ni*bpi - 1;
        
/* do checksum of block on disk */
	memset(cdig, 0, PD_CKSUM_LEN);
	PM_md5_checksum_file(file->stream, start, stop, cdig);

	rv = _PD_block_set_cksum(bl, n, cdig);

        file->cksum.file |= PD_MD5_RW;};
                    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_RESERVE - reserve space for checksum
 *                   - this is called during a PD_flush
 */

int _PD_cksum_reserve(PDBfile *file)
   {int rv;
       
/* allocate space for the MD5 checksum (128 bits => 16 unsigned chars)
 * NOTE: this MUST be the last extra in the file !!!
 */
    if (file->cksum.use & PD_MD5_FILE) 
       rv = _PD_put_string(1, "%s:00000000000000000000000000000000\n",
			   PD_CKSUM_SIG);
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CKSUM_FILE_WRITE - write the whole file checksum in the
 *                      - reserved space
 */

int _PD_cksum_file_write(PDBfile *file)
   {int rv;
    int64_t addr;
    unsigned char dig[PD_CKSUM_LEN];
    FILE *fp;

    fp = file->stream;

/* now calculate the checksum if we have that feature turned on
 * WARNING: no more writes to the file other than the checksum
 * from here on out
 */
    if (file->cksum.use & PD_MD5_FILE)
       {_PD_md5_checksum(file, dig);

	addr = _PD_locate_checksum(file);
        if (addr != -1)
           {if (lio_write(dig, 1, PD_CKSUM_LEN-1, fp) <= 0) 
	       PD_error("WRITE OF CHECKSUM FAILED - _PD_CKSUM_FILE_WRITE",
			PD_WRITE);

	    if (file->format_version > 2)
	       lio_printf(fp, "\n");}

        else
           PD_error("FSEEK FAILED DURING CHECKSUM CALC - _PD_CKSUM_FILE_WRITE",
		    PD_WRITE);};

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_VERIFY - return TRUE if there is a checksum and it matches a freshly
 *           - computed checksum
 *           - return FALSE if there is a checksum and it does not match
 *           - a freshly computed checksum
 *           - return -1 if there is no checksum in the extras table to 
 *           - verify with 
 *
 * #bind PD_verify fortran() scheme() python()
 */

int PD_verify(PDBfile *file ARG(,,cls))
   {int i, ok;
    int64_t prev, addr;
    unsigned char cso[PD_CKSUM_LEN], csn[PD_CKSUM_LEN];
    char bf[PD_CKSUM_LEN];
    char *p;
    FILE *fp;

    ok = -1;

/* sanity check */ 
    if (file != NULL)
       {fp = file->stream; 

/* do not try to verify virtual internal or non-existant files */
	if ((file->virtual_internal == FALSE) &&
	    (IS_PDBFILE(file) == TRUE) &&
	    (fp != NULL))

/* the file had better be flushed before doing checksum */
	   {PD_flush(file);

            prev = _PD_get_current_address(file, PD_GENERIC);
	    addr = _PD_locate_checksum(file);
	    if (addr != -1)
	       {if (file->system_version < 20)
		   {lio_read(bf, 1, 16, fp);
		    p = (char *) cso;
		    for (i = 0; i < 16; i++, p += 2)
		        snprintf(p, PD_CKSUM_LEN-2*i, "%02x", bf[i]);}
	    
		else
		   lio_read(cso, 1, PD_CKSUM_LEN-1, fp);

		cso[PD_CKSUM_LEN-1] = '\0';

		_PD_set_current_address(file, prev, SEEK_SET, PD_GENERIC);
        
		_PD_md5_checksum(file, (unsigned char *) csn);
  
		ok = _PD_cksum_compare(cso, csn);

		file->cksum.verified = ok;
		file->cksum.file    |= PD_MD5_FILE;};};};
    
    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ACTIVATE_CKSUM - Turn on file level checksums if flag is PD_MD5_FILE.
 *                   - Turn on variable level checksums if flag is PD_MD5_RW
 *                   - during PD_write and PD_read calls.
 *                   - Turn off all checksums if flag is PD_MD5_OFF.
 *                   - Return the old flag value.
 *
 * #bind PD_activate_cksum fortran() scheme() python()
 */

int PD_activate_cksum(PDBfile *file ARG(,,cls), PD_checksum_mode flag) 
    {int ov;

     ov              = file->cksum.use;
     file->cksum.use = flag;
   
     return(ov);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LOCATE_CHECKSUM - position the file at the beginning
 *                     - of the checksum data in the file
 */

int64_t _PD_locate_checksum(PDBfile* file)
   {int nb;
    int64_t addr, offs;
    char sgn[PD_CKSUM_SIG_LEN+1], bf[MAXLINE+1];
    char *pcs;
    FILE *fp;
    
    addr = -1L;

    if (file != NULL)
       {memset(sgn, 0, PD_CKSUM_SIG_LEN+1);
	memset(bf,  0, MAXLINE+1);

	fp = file->stream;
    
	SC_strncpy(sgn, PD_CKSUM_SIG_LEN+1,
		   PD_CKSUM_SIG, (size_t) PD_CKSUM_SIG_LEN);

/* read the checksum extra's signature (from where it *should* be) */
	_PD_set_current_address(file, -MAXLINE, SEEK_END, PD_GENERIC);

	nb = lio_read(bf, 1, MAXLINE, fp);
	SC_ASSERT(nb > 0);

	pcs = SC_strstr(bf, sgn);

/* this is a PDBfile hence do *NOT* checksum the entire file
 * do not put the 16 byte checksum at the end in the checksum calculation
 * that means avoiding the last 16 + 3 bytes (checksum + 3 newlines)
 */
	if ((pcs != NULL) && (strncmp(pcs, sgn, PD_CKSUM_SIG_LEN) == 0))
	   {offs = MAXLINE - (pcs - bf);
	    _PD_set_current_address(file, -offs, SEEK_END, PD_GENERIC);
	    lio_read(bf, 1, MAXLINE, fp);

	    offs = MAXLINE - (pcs - bf + PD_CKSUM_SIG_LEN + 1);
	    _PD_set_current_address(file, -offs, SEEK_END, PD_GENERIC);
	    addr = _PD_get_current_address(file, PD_GENERIC);};};
    
    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MD5_CHECKSUM - calculate an MD5 checksum for the given PDBfile
 *                  - return the MD5 checksum in DIGEST
 */

void _PD_md5_checksum(PDBfile* file, unsigned char digest[PD_CKSUM_LEN]) 
   {int64_t start, stop, here;
    FILE *fr;
    
    if (file != NULL)
       {memset(digest, 0, PD_CKSUM_LEN);

	fr = _PD_GET_FILE_STREAM(file);
    
	here = _PD_get_current_address(file, PD_GENERIC);

/* NOTE: the -1 is an historical artifact which is essential for
 * backward compatibility
 */
	stop = _PD_locate_checksum(file) - 1;
	if (stop > 0)
	   {start = 0;
	    PM_md5_checksum_file(fr, start, stop, digest);};

	_PD_set_current_address(file, here, SEEK_SET, PD_GENERIC);};
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
