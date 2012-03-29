/*
 * SCMMAP.C - routines for using memory mapped I/O
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_WRITE - write to a memory mapped file ala the write system call */

static size_t _SC_mf_write(void *bf, size_t sz, size_t ni, FILE *stream)
   {size_t niw;

    niw = _SC_mf_core_write(bf, sz, ni, stream);

    return(niw);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_READ - read from a memory mapped file ala the read system call */

static size_t _SC_mf_read(void *bf, size_t sz, size_t ni, FILE *stream)
   {size_t nir;

    nir = _SC_mf_core_read(bf, sz, ni, stream);

    return(nir);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_SEEK - seek in a memory mapped file ala the lseek system call */

static int _SC_mf_seek(FILE *stream, long offset, int whence)
   {int rv;

    rv = _SC_mf_core_seek(stream, offset, whence);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_TELL - tell in a memory mapped file ala the ftell library call */

static long _SC_mf_tell(FILE *stream)
   {long rv;

    rv = _SC_mf_core_tell(stream);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_MWRITE - mwrite method for memory mapped files */

static int64_t _SC_mf_mwrite(int fd, const void *buf, size_t nb, int64_t off)
   {int64_t rv;

    lseek(fd, off, SEEK_SET);

    rv = SC_write_sigsafe(fd, (void *) buf, nb);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_BIND - bind a memory mapped file into a file_io_desc */

FILE *SC_mf_bind(void *mf)
   {FILE *ret;
    file_io_desc *fid;

    ret = NULL;
    if (mf != NULL)
       {fid          = SC_make_io_desc(mf);
	fid->fopen   = SC_mf_open;

	fid->ftell   = _SC_mf_tell;
	fid->fseek   = _SC_mf_seek;
	fid->fread   = _SC_mf_read;
	fid->fwrite  = _SC_mf_write;

	fid->setvbuf = _SC_mf_setvbuf;
	fid->fclose  = _SC_mf_close;
	fid->fprintf = _SC_mf_printf;
	fid->fputs   = _SC_mf_puts;
	fid->fgetc   = _SC_mf_getc;
	fid->ungetc  = _SC_mf_ungetc;
	fid->fflush  = _SC_mf_flush;
	fid->feof    = _SC_mf_eof;
	fid->fgets   = _SC_mf_gets;
	fid->pointer = _SC_mf_data;

	ret = (FILE *) fid;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_OPEN - open a memory mapped file ala fopen */

FILE *SC_mf_open(char *name, char *mode)
   {FILE *fp, *ret;

    fp  = _SC_mf_open(name, mode, _SC_mf_setup);
    ret = SC_mf_bind(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_MMAP - mmap method for memory mapped files */

static void *_SC_mf_mmap(void *addr, size_t len, int prot,
			 int flags, int fd, int64_t offs)
   {void *rv;

    rv = mmap(addr, len, prot, flags, fd, offs);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MF_SETUP - setup the methods for memory mapped file MF */

void _SC_mf_setup(SC_mapped_file *mf)
   {file_io_desc *fid;

    mf->mopenf  = open;
    mf->mmapf   = _SC_mf_mmap;
    mf->mwritef = _SC_mf_mwrite;

    fid = &mf->fid;

    fid->ftell    = NULL;
    fid->lftell   = _SC_mf_core_tell;
    fid->fseek    = NULL;
    fid->lfseek   = _SC_mf_core_seek;
    fid->fread    = NULL;
    fid->lfread   = _SC_mf_core_read;
    fid->fwrite   = NULL;
    fid->lfwrite  = _SC_mf_core_write;

    fid->fputs    = _SC_mf_puts;
    fid->fprintf  = _SC_mf_printf;
    fid->setvbuf  = _SC_mf_setvbuf;
    fid->fclose   = _SC_mf_close;
    fid->fgetc    = _SC_mf_getc;
    fid->ungetc   = _SC_mf_ungetc;
    fid->fflush   = _SC_mf_flush;
    fid->fgets    = _SC_mf_gets;
    fid->feof     = _SC_mf_eof;

    fid->pointer  = _SC_mf_data;
    fid->segsize  = _SC_mf_segment_size;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MF_SET_HOOKS - setup the io hooks for memory mapped files */

void SC_mf_set_hooks(void)
   {

    _SC_ios.sfopen = SC_mf_open;

    return;}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* SC_MF_SET_HOOKS - stup setup of io hooks for memory mapped files */

void SC_mf_set_hooks(void)
   {

    SC_io_connect(SC_LOCAL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
