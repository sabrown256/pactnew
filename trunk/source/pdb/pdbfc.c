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

static int64_t _PD_fctell(FILE *fp)
   {int64_t addr, ada;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

/* get the absolute container address */
    ada = ftell(fl);
    if (ada < 0)
       io_error(errno, "ftell failed");

/* compute the apparent file address */
    addr = ada - cf->begin;

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCSEEK - do an fseek on the pseudo file */

static int _PD_fcseek(FILE *fp, int64_t addr, int offset)
   {int ret;
    int64_t ada;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    ada = -1;

/* compute the absolute container address */
    switch (offset)
       {case SEEK_SET :
	     ada = addr + cf->begin;
	     break;
        case SEEK_CUR :
	     ada  = ftell(fl);
	     if (ada < 0)
	        io_error(errno, "ftell failed");
	     ada += addr;
	     break;
        case SEEK_END :
	     ada = cf->end + addr;
	     break;};

    ret = fseek(fl, ada, SEEK_SET);
    if (ret < 0)
       io_error(errno, "fseek to %s failed", SC_itos(NULL, 0, ada, NULL));

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

static u_int64_t _PD_fcread(void *p, size_t sz, u_int64_t ni, FILE *fp)
   {u_int64_t rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = fread(p, sz, ni, fl);
    if (ferror(fl))
       {io_error(errno, "fread of %s bytes failed",
		 SC_itos(NULL, 0, sz*ni, NULL));
	clearerr(fl);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCWRITE - fwrite method for containerf */

static u_int64_t _PD_fcwrite(void *p, size_t sz, u_int64_t ni, FILE *fp)
   {int rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);

    rv = fwrite(p, sz, ni, fl);
    if (rv < 0)
       io_error(errno, "fwrite of %s bytes failed",
		 SC_itos(NULL, 0, sz*ni, NULL));

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

#if defined(MACOSX) || defined(FREEBSD)
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
   {int nc;
    char *rv;
    containerf *cf;
    FILE *fl;

    cf = GET_CONTAINERF(fp);
    fl = GET_FILE(cf);
    rv = NULL;

/* reserve the last character for a '\0' terminator */
    nc = n - 1;
    if (nc > 0)
       {rv = fgets(s, nc, fl);
	s[nc] = '\0';};

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

/* _PD_OPEN_CONTAINER_FILE - open the file in the container */

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

    cf = CMAKE(containerf);

    fc = SC_open_fcontainer(name, SC_UNKNOWN, NULL);
    if (fc == NULL)
       fe = NULL;
    else
       fe = (fcent *) SC_hasharr_def_lookup(fc->handle->entries, file);

    if (fe == NULL)
       {SC_close_fcontainer(fc);
	CFREE(cf);}
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

/* PD_OPEN_CONTAINED - open a PDB file contained in file NAME
 *                   - in the address range (SAD, EAD)
 */

PDBfile *PD_open_contained(char *name, int64_t sad, int64_t ead)
   {char t[3][MAXLINE];
    PDBfile *fp;

    SC_itos(t[0], MAXLINE, sad, NULL);
    SC_itos(t[0], MAXLINE, ead, NULL);
    snprintf(t[2], MAXLINE, "%s~%s:%s", name, t[0], t[1]);

    fp = PD_open(t[2], "r");

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

