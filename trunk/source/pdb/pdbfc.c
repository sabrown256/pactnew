/*
 * PDBFC.C - routines to access files in containers
 *
 * Routines and Usage:
 *
 *   PF_open     - open a PDBfile which encapsulates a memory buffer
 *               - into and out of which all IO goes
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define GET_CONTAINERF(_f)       ((containerf *) (_f))
#define GET_FILE(_c)             ((_c)->fc->handle->file)

typedef struct s_containerf containerf;
 
struct s_containerf
   {fcontainer *fc;
    int64_t begin;
    int64_t end;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCTELL - do an ftell on the pseudo file */

static BIGINT _PD_fctell(FILE *fp)
   {int64_t addr, ada;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

/* get the absolute container address */
    ada = ftell(fl);

/* compute the apparent file address */
    addr = ada - cf->begin;

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCSEEK - do an fseek on the pseudo file */

static int _PD_fcseek(FILE *fp, BIGINT addr, int offset)
   {int ret;
    int64_t ada;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

/* compute the absolute container address */
    switch (offset)
       {case SEEK_SET :
	     ada = addr + cf->begin;
	     break;
        case SEEK_CUR :
	     ada  = ftell(fl);
	     ada += addr;
	     break;
        case SEEK_END :
	     ada = cf->end + addr;
	     break;};

    ret = fseek(fl, ada, SEEK_SET);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCSETVBUF - setvbuf method for containerf */

static int _PD_fcsetvbuf(FILE *fp, char *bf, int type, size_t sz)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = setvbuf(fl, bf, type, sz);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCREAD - fread method for containerf */

static BIGUINT _PD_fcread(void *p, size_t sz, BIGUINT ni, FILE *fp)
   {BIGUINT rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

{int64_t ada;
ada = ftell(fl);

    rv = fread(p, sz, ni, fl);
};
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCWRITE - fwrite method for containerf */

static BIGUINT _PD_fcwrite(void *p, size_t sz, BIGUINT ni, FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = fwrite(p, sz, ni, fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCCLOSE - fclose method for containerf */

static int _PD_fcclose(FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = fclose(fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCPRINTF - fprintf method for containerf */

static int _PD_fcprintf(FILE *fp, char *fmt, va_list a)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = vfprintf(fl, fmt, a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCPUTS - fputs method for containerf */

static int _PD_fcputs(const char *s, FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = fputs(s, fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCGETC - getc method for containerf */

static int _PD_fcgetc(FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = getc(fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCUNGETC - ungetc method for containerf */

static int _PD_fcungetc(int c, FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = ungetc(c, fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCFLUSH - fflush method for containerf */

static int _PD_fcflush(FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = fflush(fl);

#ifdef MACOSX
/* on OSX the fflush sometimes fails for the last file in a container
 * with "Bad file descriptor" errno 9
 */
    rv = 0;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCGETS - fgets method for containerf */

static char *_PD_fcgets(char *s, int n, FILE *fp)
   {char *rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = fgets(s, n, fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCEOF - feof method for containerf */

static int _PD_fceof(FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = feof(fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_OPEN_GET_CONTAINERF - open the file in the container */

FILE *_PD_open_container_file(char *name, char *mode)
   {char cntr[MAXLINE];
    char *file;
    FILE *fp;
    file_io_desc *fid;
    containerf *cf;
    fcontainer *fc;
    fcent *fe;

    SC_strncpy(cntr, MAXLINE, name, -1);
    file = _SC_udl_container(cntr, TRUE);

    fp = NULL;

    cf = FMAKE(containerf, "_PD_OPEN_GET_CONTAINERF:cf");

    fc = SC_open_fcontainer(cntr, SC_UNKNOWN);
    fe = (fcent *) SC_hasharr_def_lookup(fc->handle->entries, file);
    if (fe == NULL)
       {SC_close_fcontainer(fc);
	SFREE(cf);}
    else
       {cf->fc    = fc;
	cf->begin = fe->address;
	cf->end   = fe->address + fe->size;

	fid          = SC_make_io_desc(cf);
	fid->fopen   = _PD_open_container_file;

	fid->lftell  = _PD_fctell;
	fid->lfseek  = _PD_fcseek;
	fid->lfread  = _PD_fcread;
	fid->lfwrite = _PD_fcwrite;

	fid->setvbuf = _PD_fcsetvbuf;
	fid->fclose  = _PD_fcclose;
	fid->fprintf = _PD_fcprintf;
	fid->fputs   = _PD_fcputs;
	fid->fgetc   = _PD_fcgetc;
	fid->ungetc  = _PD_fcungetc;
	fid->fflush  = _PD_fcflush;
	fid->fgets   = _PD_fcgets;
	fid->feof    = _PD_fceof;

	fp = (FILE *) fid;};

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

