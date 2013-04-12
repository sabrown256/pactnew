/*
 * SCLMMAP.C - routines for using memory mapped I/O
 *           - these provide support for 64 bit files on
 *           - 32 bit OSes
 *           - not needed with 64 bit OS
 *           - tested mainly on 32 bit LINUX
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

#ifdef HAVE_ALT_LARGE_FILE

# ifndef _LARGEFILE64_SOURCE
#  define _LARGEFILE64_SOURCE
# endif

# include <sys/ioctl.h>
# include <sys/mman.h>

extern int
 open64(const char *file, int flag, ...);

#if !defined(SOLARIS) && !defined(LINUX)
extern void
 *mmap64(void *addr, size_t len, int prot, int flags, int fd, int64_t offs);
#endif

extern ssize_t
 pwrite64(int fd, const void *buf, size_t nb, int64_t off);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LMF_WRITE - stub for write hook
 *               - this wraps the LMF version to be call compatible
 */

static u_int64_t _SC_lmf_write(void *bf, size_t sz, u_int64_t ni, FILE *stream)
   {u_int64_t rv;

    rv = _SC_mf_core_write(bf, sz, ni, stream);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LMF_READ - stub for read hook
 *              - this wraps the LMF version to be call compatible
 */

static u_int64_t _SC_lmf_read(void *bf, size_t sz, u_int64_t ni, FILE *stream)
   {u_int64_t rv;

    rv = _SC_mf_core_read(bf, sz, ni, stream);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LMF_MWRITE - mwrite method for memory mapped files */

static int64_t _SC_lmf_mwrite(int fd, const void *buf, size_t nb, int64_t off)
   {int64_t rv;

    rv = pwrite64(fd, buf, nb, off);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_AUX_FLUSH - stub for flush hook
 *               - this doesn't have meaning for mapped files
 *               - in some special cases
 */

static int _SC_aux_flush(FILE *stream)
   {int ret;

    ret = 0;
    if (stream == stdout)
       ret = SC_fflush_safe(stream);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LMF_BIND - bind a memory mapped file into a file_io_desc */

FILE *SC_lmf_bind(void *mf)
   {FILE *ret;
    file_io_desc *fid;

    ret = NULL;
    if (mf != NULL)
       {fid          = SC_make_io_desc(mf);
	fid->fopen   = SC_lmf_open;

	fid->lftell  = _SC_mf_core_tell;
	fid->lfseek  = _SC_mf_core_seek;
	fid->lfread  = _SC_lmf_read;
	fid->lfwrite = _SC_lmf_write;

	fid->setvbuf = _SC_mf_setvbuf;
	fid->fclose  = _SC_mf_close;
	fid->fprintf = _SC_mf_printf;
	fid->fputs   = _SC_mf_puts;
	fid->fgetc   = _SC_mf_getc;
	fid->ungetc  = _SC_mf_ungetc;
	fid->fflush  = _SC_aux_flush;
	fid->feof    = _SC_mf_eof;
	fid->fgets   = _SC_mf_gets;
	fid->pointer = _SC_mf_data;
	fid->segsize = _SC_mf_segment_size;

	ret = (FILE *) fid;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LMF_OPEN - open a memory mapped file ala fopen */

FILE *SC_lmf_open(char *name, char *mode)
   {FILE *fp, *ret;

    fp  = _SC_mf_open(name, mode, _SC_lmf_setup);
    ret = SC_lmf_bind(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LMF_MMAP - mmap method for memory mapped files */

static void *_SC_lmf_mmap(void *addr, size_t len, int prot,
			  int flags, int fd, int64_t offs)
   {void *rv;

    rv = mmap64(addr, len, prot, flags, fd, offs);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LMF_SETUP - setup the methods for memory mapped file MF */

void _SC_lmf_setup(SC_mapped_file *mf)
   {file_io_desc *fid;

    mf->mopenf  = open64;
    mf->mmapf   = _SC_lmf_mmap;
    mf->mwritef = _SC_lmf_mwrite;

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

/* SC_LMF_SET_HOOKS - setup the io hooks for memory mapped files */

void SC_lmf_set_hooks(void)
   {

    _SC_ios.lfopen = SC_lmf_open;

    return;}

/*--------------------------------------------------------------------------*/

/*                              I/O HOOK CONTROL                            */

/*--------------------------------------------------------------------------*/

/* SC_SET_IO_HOOKS - set both sets of I/O hooks appropriately
 *                 - to use the memory mapped functions
 *                 - if WHCH is 1 then use the large model (> 2 GB) versions
 *                 - otherwise use what may be the small model (< 2 GB)
 *                 - versions on 32 bit machines
 *                 - this sacrifices some functionality in favor of ease
 *                 - of use so use with caution
 */

void SC_set_io_hooks(int whch)
   {

    if (whch == 1)
       {SC_lmf_set_hooks();

	_SC_ios.sfopen = _SC_ios.lfopen;}

    else
       {SC_mf_set_hooks();

	_SC_ios.lfopen = _SC_ios.sfopen;};

    return;}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* SC_LMF_SET_HOOKS - setup the memory mapped set of
 *                  - large file io hooks
 */

void SC_lmf_set_hooks(void)
   {

    _SC_ios.lfopen = SC_mf_open;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_IO_HOOKS - set both sets of I/O hooks appropriately
 *                 - to use the memory mapped functions
 *                 - if WHCH is 1 then use the large model (> 2 GB) versions
 *                 - otherwise use what may be the small model (< 2 GB)
 *                 - versions on 32 bit machines
 *                 - this sacrifices some functionality in favor of ease
 *                 - of use so use with caution
 */

void SC_set_io_hooks(int whch)
   {

    SC_mf_set_hooks();

    _SC_ios.lfopen = _SC_ios.sfopen;

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#endif
