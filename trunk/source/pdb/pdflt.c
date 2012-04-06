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

/* _PD_FILT_BLOCK_OUT - do the output filter for the Nth block of EP
 *                    - this should be like conversion routines
 *                    - return the TRUE iff successful
 */ 
 
char *_PD_filt_block_out(PDBfile *file, char *bf, long bpi, int64_t ni)
   {fltbdes *fl, *flt;
            
/* run the filter chain on the block */
    if ((bf != NULL) && (bpi != 0) && (ni != 0))
       {flt = file->filter_block;
	for (fl = flt; (fl != NULL) && (bf != NULL); fl = fl->next)
            bf = fl->out(fl, file, bf, bpi, ni);};
                    
    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILT_BLOCK_IN - do the input filter for the Nth block of EP
 *                   - this should be like conversion routines
 *                   - return the TRUE iff successful
 */ 
 
char *_PD_filt_block_in(PDBfile *file, char *bf, long bpi, int64_t ni)
   {fltbdes *fl, *flt;
            
/* run the filter chain on the block */
    if ((bf != NULL) && (bpi != 0) && (ni != 0))
       {flt = file->filter_block;

/* GOTCHA: have to run the filters in the opposite order as on write */
	for (fl = flt; (fl != NULL) && (bf != NULL); fl = fl->next)
            bf = fl->in(fl, file, bf, bpi, ni);};
                    
    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FILT_REGISTER_BLOCK - register variable block filter */

int PD_filt_register_block(PDBfile *file, PFifltbdes fi, PFifltbdes fo,
			   void *data)
    {fltbdes *fl;

     fl = CMAKE(fltbdes);
     if (fl != NULL)
        {fl->in   = fi;
	 fl->out  = fo;
	 fl->data = data;
	 fl->next = file->filter_block;

         file->filter_block = fl;};
   
     return(TRUE);}
     
/*--------------------------------------------------------------------------*/

/*                         WHOLE FILE FILTER ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* _PD_FILT_FILE_OUT - do the output side of the whole file filter */

int _PD_filt_file_out(PDBfile *file)
   {int ok;
    fltwdes *fl, *flt;
    FILE *fp;

    ok = FALSE;

    if (file != NULL)
       {fp = file->stream; 

/* do not try to restore virtual internal or non-existant files */
	if ((file->virtual_internal == FALSE) &&
	    (IS_PDBFILE(file) == TRUE) &&
	    (fp != NULL))

/* the file had better be flushed before applying filters */
	   {PD_flush(file);

	    ok  = TRUE;
            flt = file->filter_file;
            for (fl = flt; (fl != NULL) && (ok == TRUE); fl = fl->next)
                ok &= fl->out(fl, file);};};
    
    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILT_FILE_IN - do the input side of the whole file filter */

int _PD_filt_file_in(PDBfile *file ARG(,,cls))
   {int ok;
    fltwdes *fl, *flt;
    FILE *fp;

    ok = FALSE;

    if (file != NULL)
       {fp = file->stream; 

/* do not try to restore virtual internal or non-existant files */
	if ((file->virtual_internal == FALSE) &&
	    (IS_PDBFILE(file) == TRUE) &&
	    (fp != NULL))

/* the file had better be flushed before applying filters */
	   {PD_flush(file);

	    ok  = TRUE;
            flt = file->filter_file;
            for (fl = flt; (fl != NULL) && (ok == TRUE); fl = fl->next)
                ok &= fl->in(fl, file);};};
    
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

/* PD_FILT_REGISTER_FILE - register whole file filter */

int PD_filt_register_file(PDBfile *file, PFifltwdes fi, PFifltwdes fo,
			  void *data)
    {fltwdes *fl;

     fl = CMAKE(fltwdes);
     if (fl != NULL)
        {fl->in   = fi;
	 fl->out  = fo;
	 fl->data = data;
	 fl->next = file->filter_file;

         file->filter_file = fl;};
   
     return(TRUE);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
