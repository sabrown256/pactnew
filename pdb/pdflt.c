/*
 * PDFLT.C - generic PDB filter infrastructure
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FILT_SWITCH - move the J part of INF to the I part */

void PD_filt_switch(fltdat *inf, int i, int j)
   {unsigned char *bfa, *bfb;

    bfa = inf->bf[i];
    bfb = inf->bf[j];

    inf->nb[i]  = inf->nb[j];
    inf->ni[i]  = inf->ni[j];
    inf->bpi[i] = inf->bpi[j];

    inf->nb[j]  = 0;
    inf->ni[j]  = 0;
    inf->bpi[j] = 0;

    if (bfa != bfb)
       {CFREE(bfa);
	inf->bf[i] = bfb;
        inf->bf[j] = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILT_BLOCK_OUT - do the output filter for the Nth block of EP
 *                    - this should be like conversion routines
 *                    - return the TRUE iff successful
 */ 
 
int _PD_filt_block_out(PDBfile *file, unsigned char *bf,
		       const char *type, long bpi, int64_t ni)
   {int ok;
    inti niw, nie;
    fltdes *fl, *flt;
    fltdat inf;
    FILE *fp;

    ok = FALSE;

/* run the filter chain on the block */
    if ((bf != NULL) && (bpi != 0) && (ni != 0))
       {fp  = file->stream;
	flt = file->block_chain;

	memset(&inf, 0, sizeof(inf));

	inf.type   = type;
	inf.nb[1]  = bpi*ni;
	inf.bf[1]  = bf;
	inf.bpi[1] = bpi;
	inf.ni[1]  = ni;

	PD_filt_switch(&inf, 0, 1);

/* find the end of the chain */
	for (fl = flt; (fl != NULL) && (fl->next != NULL); fl = fl->next);

/* run the filter chain in reverse order as defined */
	ok = TRUE;
	for (; (fl != NULL) && (ok == TRUE); fl = fl->prev)
            {ok &= fl->out(file, fl, &inf);

/* we may not free BF so make sure it is
 * put out of the way before switching
 */
	     if (inf.bf[0] == bf)
	        inf.bf[0] = NULL;

	     PD_filt_switch(&inf, 0, 1);};
                    
        niw = lio_write(inf.bf[0], inf.bpi[0], inf.ni[0], fp);
        nie = inf.ni[0];
	ok  = (niw == nie);

	if (flt != NULL)
	   CFREE(inf.bf[0]);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILT_BLOCK_IN - do the input filter for the Nth block of EP
 *                   - this should be like conversion routines
 *                   - return the TRUE iff successful
 */ 
 
int _PD_filt_block_in(PDBfile *file, unsigned char *bf,
		      const char *type, intb bpi, inti ni)
   {int ok;
    inti nir, nie, nbi;
    unsigned char *lbf;
    fltdes *fl, *flt;
    fltdat inf;
    FILE *fp;

    ok = FALSE;

/* run the filter chain on the block */
    if ((bf != NULL) && (bpi != 0) && (ni != 0))
       {fp  = file->stream;
	flt = file->block_chain;
	nbi = bpi*ni;

	memset(&inf, 0, sizeof(inf));

	if (flt != NULL)
	   lbf = CMAKE_N(unsigned char, nbi);
	else
	   lbf = bf;

	inf.type   = type;
	inf.nb[1]  = nbi;
	inf.bf[1]  = lbf;
	inf.bpi[1] = bpi;
	inf.ni[1]  = ni;

	PD_filt_switch(&inf, 0, 1);

	nir = lio_read(inf.bf[0], inf.bpi[0], inf.ni[0], fp);
	nie = inf.ni[0];

/* run the filter chain in order as defined */
	ok = (nir == nie);
	for (fl = flt; (fl != NULL) && (ok == TRUE); fl = fl->next)

/* make sure that the final version ends up in BF */
	    {if (fl->next == NULL)
	        {inf.bf[1]  = bf;
		 inf.bpi[1] = bpi;
		 inf.ni[1]  = ni;};

	     ok &= fl->in(file, fl, &inf);
	     PD_filt_switch(&inf, 0, 1);};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FILT_REGISTER_BLOCK - register variable block filter */

int PD_filt_register_block(PDBfile *file, fltdes *fl)
    {

     if (file != NULL)
        file->block_chain = fl;
   
     return(TRUE);}
     
/*--------------------------------------------------------------------------*/

/*                         WHOLE FILE FILTER ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* _PD_FILT_APPLY_FILE - apply filter F to the whole file FILE from
 *                     - START to END
 */

static int _PD_filt_apply_file(PDBfile *file, fltdes *fl, PFifltdes f,
			       int64_t start, int64_t end)
   {int rv;
    int64_t nb, ni;
    int64_t addr;
    char nm[MAXLINE];
    fltdat inf;
    FILE *fi, *fo;

    rv = TRUE;

    if (f != NULL)
       {ni = end - start;

	nb = PD_get_buffer_size();
	if (nb <= 0)
	   nb = SC_OPT_BFSZ;

/* initialize the filter info such that the
 * output side is the same as the input side
 * so a do nothing filter works
 */
	memset(&inf, 0, sizeof(inf));

	inf.type   = "char";
	inf.nb[0]  = min(nb, ni);
	inf.bf[0]  = CMAKE_N(unsigned char, nb);
	inf.bpi[0] = 1;
	inf.ni[0]  = inf.nb[0];

	snprintf(nm, MAXLINE, "%s.+", file->name);

	fo   = lio_open(nm, "w+");
	fi   = file->stream;
	addr = lio_tell(fi);

	lio_seek(fi, start, SEEK_SET);

	for (; (ni > 0); ni -= inf.ni[0])
	    {inf.ni[0] = lio_read(inf.bf[0], inf.bpi[0], inf.nb[0], fi);
	     
	     rv &= f(file, fl, &inf);

	     if ((rv == TRUE) && (inf.bf[1] != NULL) &&
		 (inf.bpi[1] > 0) && (inf.ni[1] > 0))
	        lio_write(inf.bf[1], inf.bpi[1], inf.ni[1], fo);
	     else
	        {rv = FALSE;
		 break;};};

	lio_close(fo);

	if (inf.bf[1] != inf.bf[0])
	   CFREE(inf.bf[1]);
	CFREE(inf.bf[0]);

	_PD_replace_file(file, nm, addr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILT_FILE_OUT - do the output side of the whole file filter
 *                   - the file has been flushed and will be closed next
 *                   - return TRUE if there is a chain and all the
 *                   - filters succeeded
 *                   - return FALSE if there is a chain and one of the
 *                   - filters failed
 *                   - return -1 if there is no chain
 */

int _PD_filt_file_out(PDBfile *file)
   {int ok;
    int64_t len;
    fltdes *fl, *flt;
    FILE *fp;

    ok = -1;

    if (file != NULL)
       {fp = file->stream; 

/* do not try to restore virtual internal or non-existant files */
	if ((file->virtual_internal == FALSE) &&
	    (IS_PDBFILE(file) == TRUE) &&
	    (fp != NULL))
           {flt = file->file_chain;

/* find the end of the chain */
            for (fl = flt; (fl != NULL) && (fl->next != NULL); fl = fl->next);

/* run the filter chain in reverse order as defined */
	    ok = TRUE;
            for (; (fl != NULL) && (ok == TRUE); fl = fl->prev)
	        {len = PD_get_file_length(file);
		 ok &= _PD_filt_apply_file(file, fl, fl->out, 0, len);};};};
    
    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILT_FILE_IN - do the input side of the whole file filter
 *                  - return TRUE if there is a chain and all the
 *                  - filters succeeded
 *                  - return FALSE if there is a chain and one of the
 *                  - filters failed
 *                  - return -1 if there is no chain
 */

int _PD_filt_file_in(PDBfile *file ARG(,,cls))
   {int ok;
    int64_t len;
    fltdes *fl, *flt;
    FILE *fp;

    ok = -1;

    if (file != NULL)
       {fp = file->stream; 

/* do not try to restore virtual internal or non-existant files */
	if ((file->virtual_internal == FALSE) &&
	    (IS_PDBFILE(file) == TRUE) &&
	    (fp != NULL))
	   {flt = file->file_chain;

/* run the filter chain in order as defined */
            ok = TRUE;
            for (fl = flt; (fl != NULL) && (ok == TRUE); fl = fl->next)
	        {len = PD_get_file_length(file);
		 ok &= _PD_filt_apply_file(file, fl, fl->in, 0, len);};};};
    
    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILT_CLOSE - handle filter related chores on close
 *                - return TRUE if successful, FALSE on failure,
 *                - and -1 if there is no filter work to do
 */

int _PD_filt_close(PDBfile *file)
   {int ok;

    ok = -1;

    if (file->virtual_internal == FALSE)
       ok = _PD_filt_file_out(file);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FILT_MAKE_CHAIN - make a filter chain */

fltdes *PD_filt_make_chain(fltdes *flp, const char *name,
			   PFifltdes fi, PFifltdes fo, void *data)
   {fltdes *fl;

    fl = CMAKE(fltdes);
    if (fl != NULL)
       {fl->name = CSTRSAVE(name);
	fl->in   = fi;
	fl->out  = fo;
	fl->data = data;
	fl->next = flp;

	if (flp != NULL)
	   flp->prev = fl;};
   
    return(fl);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FILT_FREE_CHAIN - free a filter chain */

void PD_filt_free_chain(fltdes *fl)
   {fltdes *nxt;

    for (; fl != NULL; fl = nxt)
        {nxt = fl->next;
         CFREE(fl->name);
	 CFREE(fl);};
   
    return;}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FILT_REGISTER_FILE - register whole file filter chain FL with FILE */

int PD_filt_register_file(fltdes *fl)
   {

    _PD.file_chain = fl;
   
    return(TRUE);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
