/*
 * SCOPE_MPEG.H - contains all the following files:
 *
 *	#include "libpnmrw.h"
 *	#include "all.h"
 *	#include "ansi.h"
 *	#include "general.h"
 *	#include "byteorder.h"
 *	#include "dct.h"
 *	#include "mtypes.h"
 *	#include "bitio.h"
 *	#include "mheaders.h"
 *	#include "huff.h"
 *	#include "frame.h"
 *	#include "frames.h"
 *	#include "specifics.h"
 *	#include "prototypes.h"
 *	#include "search.h"
 *	#include "fsize.h"
 *	#include "mpeg.h"
 *	#include "param.h"
 *	#include "readframe.h"
 *	#include "rgbtoycc.h"
 *	#include "parallel.h"
 *	#include "postdct.h"
 *	#include "jpeg.h"
 *	#include "rate.h"
 *	#include "opts.h"
 *	#include "combine.h"
 */

#ifndef PCK_SCOPE_MPEG

#define PCK_SCOPE_MPEG

/* 
  defining NDEBUG prevents assert statements from being compiled
  which prevents gcc from generating calls to _eprintf.
*/

#define NDEBUG
#include "score.h"

#undef I                 /* C99 pure imaginary */
#ifndef PCC
#include <limits.h>
#endif
#ifdef MIPS
#include <sys/types.h>
#endif
#include <sys/stat.h>

/* There's got to be a better way.... */
#ifdef LINUX
#include <time.h>
#endif
#ifdef MIPS
#include <time.h>
#endif
#ifdef IRIX
#define FORCE_LITTLE_ENDIAN
#include <time.h>
#endif

extern FILE
 *_PG_fopen(char *name, char *mode);


/* pnmrw.h - header file for PBM/PGM/PPM read/write library
**
** Copyright (C) 1988, 1989, 1991 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#ifndef _PNMRW_H_
#define _PNMRW_H_

/* CONFIGURE: On some systems, malloc.h doesn't declare these, so we have
** to do it.  On other systems, for example HP/UX, it declares them
** incompatibly.  And some systems, for example Dynix, don't have a
** malloc.h at all.  A sad situation.  If you have compilation problems
** that point here, feel free to tweak or remove these declarations.
*/
/*
#ifndef __APPLE__
# include <malloc.h>
#endif
*/
/* End of configurable definitions. */


/* Definitions to make PBMPLUS work with either ANSI C or C Classic. */

#if __STDC__
#define ARGS(alist) alist
#else /*__STDC__*/
#define ARGS(alist) ()
#define const
#endif /*__STDC__*/


/* Types. */

typedef unsigned char bit;
#define PBM_WHITE 0
#define PBM_BLACK 1
#define PBM_FORMAT_TYPE(f) ((f) == PBM_FORMAT || (f) == RPBM_FORMAT ? PBM_TYPE : -1)

typedef unsigned char gray;
#define PGM_MAXMAXVAL 255
#define PGM_FORMAT_TYPE(f) ((f) == PGM_FORMAT || (f) == RPGM_FORMAT ? PGM_TYPE : PBM_FORMAT_TYPE(f))

typedef gray pixval;
#define PPM_MAXMAXVAL PGM_MAXMAXVAL
typedef struct
    {
    pixval r, g, b;
    } pixel;
#define PPM_GETR(p) ((p).r)
#define PPM_GETG(p) ((p).g)
#define PPM_GETB(p) ((p).b)
#define PPM_ASSIGN(p,red,grn,blu) do { (p).r = (red); (p).g = (grn); (p).b = (blu); } while ( 0 )
#define PPM_EQUAL(p,q) ( (p).r == (q).r && (p).g == (q).g && (p).b == (q).b )
#define PPM_FORMAT_TYPE(f) ((f) == PPM_FORMAT || (f) == RPPM_FORMAT ? PPM_TYPE : PGM_FORMAT_TYPE(f))

typedef pixel xel;
typedef pixval xelval;
#define PNM_MAXMAXVAL PPM_MAXMAXVAL
#define PNM_GET1(x) PPM_GETB(x)
#define PNM_ASSIGN1(x,v) PPM_ASSIGN(x,0,0,v)
#define PNM_EQUAL(x,y) PPM_EQUAL(x,y)
#define PNM_FORMAT_TYPE(f) PPM_FORMAT_TYPE(f)


/* Magic constants. */

#define PBM_MAGIC1 'P'
#define PBM_MAGIC2 '1'
#define RPBM_MAGIC2 '4'
#define PBM_FORMAT (PBM_MAGIC1 * 256 + PBM_MAGIC2)
#define RPBM_FORMAT (PBM_MAGIC1 * 256 + RPBM_MAGIC2)
#define PBM_TYPE PBM_FORMAT

#define PGM_MAGIC1 'P'
#define PGM_MAGIC2 '2'
#define RPGM_MAGIC2 '5'
#define PGM_FORMAT (PGM_MAGIC1 * 256 + PGM_MAGIC2)
#define RPGM_FORMAT (PGM_MAGIC1 * 256 + RPGM_MAGIC2)
#define PGM_TYPE PGM_FORMAT

#define PPM_MAGIC1 'P'
#define PPM_MAGIC2 '3'
#define RPPM_MAGIC2 '6'
#define PPM_FORMAT (PPM_MAGIC1 * 256 + PPM_MAGIC2)
#define RPPM_FORMAT (PPM_MAGIC1 * 256 + RPPM_MAGIC2)
#define PPM_TYPE PPM_FORMAT


/* Color scaling macro -- to make writing ppmtowhatever easier. */

#define PPM_DEPTH(newp,p,oldmaxval,newmaxval) \
    PPM_ASSIGN( (newp), \
	( (int) PPM_GETR(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval), \
	( (int) PPM_GETG(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval), \
	( (int) PPM_GETB(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval) )


/* Luminance macro. */

#define PPM_LUMIN(p) ( 0.299 * PPM_GETR(p) + 0.587 * PPM_GETG(p) + 0.114 * PPM_GETB(p) )


/* Declarations of pnmrw routines. */

void pnm_init2 ARGS(( char* pn ));

char** pm_allocarray ARGS(( int cols, int rows, int size ));
#define pnm_allocarray( cols, rows ) ((xel**) pm_allocarray( cols, rows, (int) sizeof(xel) ))
char* pm_allocrow ARGS(( int cols, int size ));
#define pnm_allocrow( cols ) ((xel*) pm_allocrow( cols, (int) sizeof(xel) ))
void pm_freearray ARGS(( char** its, int rows ));
#define pnm_freearray( xels, rows ) pm_freearray( (char**) xels, rows )
void pm_freerow ARGS(( char* itrow ));
#define pnm_freerow( xelrow ) pm_freerow( (char*) xelrow )

xel** pnm_readpnm ARGS(( FILE* file, int* colsP, int* rowsP, xelval* maxvalP, int* formatP ));
int pnm_readpnminit ARGS(( FILE* file, int* colsP, int* rowsP, xelval* maxvalP, int* formatP ));
int pnm_readpnmrow ARGS(( FILE* file, xel* xelrow, int cols, xelval maxval, int format ));

int pnm_writepnm ARGS(( FILE* file, xel** xels, int cols, int rows, xelval maxval, int format, int forceplain ));
int pnm_writepnminit ARGS(( FILE* file, int cols, int rows, xelval maxval, int format, int forceplain ));
int pnm_writepnmrow ARGS(( FILE* file, xel* xelrow, int cols, xelval maxval, int format, int forceplain ));

extern xelval pnm_pbmmaxval;
/* This is the maxval used when a PNM program reads a PBM file.  Normally
** it is 1; however, for some programs, a larger value gives better results
*/


/* File open/close that handles "-" as stdin and checks errors. */

FILE* pm_openr ARGS(( char* name ));
FILE* pm_openw ARGS(( char* name ));
int pm_closer ARGS(( FILE* f ));
int pm_closew ARGS(( FILE* f ));


/* Colormap stuff. */

typedef struct colorhist_item* colorhist_vector;
struct colorhist_item
    {
    pixel color;
    int value;
    };

typedef struct colorhist_list_item* colorhist_list;
struct colorhist_list_item
    {
    struct colorhist_item ch;
    colorhist_list next;
    };

typedef colorhist_list* colorhash_table;

colorhist_vector ppm_computecolorhist ARGS(( pixel** pixels, int cols, int rows, int maxcolors, int* colorsP ));
/* Returns a colorhist *colorsP long (with space allocated for maxcolors. */

void ppm_addtocolorhist ARGS(( colorhist_vector chv, int* colorsP, int maxcolors, pixel* colorP, int value, int position ));

void ppm_freecolorhist ARGS(( colorhist_vector chv ));

colorhash_table ppm_computecolorhash ARGS(( pixel** pixels, int cols, int rows, int maxcolors, int* colorsP ));

int
ppm_lookupcolor ARGS(( colorhash_table cht, pixel* colorP ));

colorhist_vector ppm_colorhashtocolorhist ARGS(( colorhash_table cht, int maxcolors ));
colorhash_table ppm_colorhisttocolorhash ARGS(( colorhist_vector chv, int colors ));

int ppm_addtocolorhash ARGS(( colorhash_table cht, pixel* colorP, int value ));
/* Returns -1 on failure. */

colorhash_table ppm_alloccolorhash ARGS(( void ));

void ppm_freecolorhash ARGS(( colorhash_table cht ));

/* Other function declarations */
void pnm_promoteformat(xel** xels, int cols, int rows, xelval maxval,
		       int format, xelval newmaxval, int newformat);
void pnm_promoteformatrow(xel* xelrow, int cols, xelval maxval, 
			  int format, xelval newmaxval, int newformat);

xel pnm_backgroundxel ARGS(( xel** xels, int cols, int rows, xelval maxval, int format ));
xel pnm_backgroundxelrow ARGS(( xel* xelrow, int cols, xelval maxval, int format ));
xel pnm_whitexel ARGS(( xelval maxval, int format ));
xel pnm_blackxel ARGS(( xelval maxval, int format ));
void pnm_invertxel ARGS(( xel* xP, xelval maxval, int format ));

#endif /*_PNMRW_H_*/
/*===========================================================================*
 * all.h								     *
 *									     *
 *	stuff included from ALL source files				     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.9  1995/06/05  21:11:06  smoot
 * added little_endian force for irizx
 *
 * Revision 1.8  1995/02/02  22:02:18  smoot
 * added ifdefs for compatability on stranger and stranger architectures...
 *
 * Revision 1.7  1995/02/02  07:26:45  eyhung
 * added parens to all.h to remove compiler warning
 *
 * Revision 1.6  1995/02/02  01:47:11  eyhung
 * added MAXINT
 *
 * Revision 1.5  1995/01/19  23:54:33  eyhung
 * Changed copyrights
 *
 * Revision 1.4  1994/11/14  22:52:04  smoot
 * Added linux #include for time.h
 *
 * Revision 1.3  1994/11/12  02:12:13  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.1  1993/07/09  00:17:23  keving
 * nothing
 *
 */


#ifndef ENCODE_ALL_INCLUDED
#define ENCODE_ALL_INCLUDED

#if 0
/*NOTE*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <limits.h>

/* There's got to be a better way.... */
#ifdef LINUX
#include <time.h>
#endif
#ifdef MIPS
#include <time.h>
#endif
#ifdef IRIX
#define FORCE_LITTLE_ENDIAN
#include <time.h>
#endif

#include "libpnmrw.h"
#include "ansi.h"
#include "general.h"
/*NOTE*/
#endif

/* some machines have #define index strchr; get rid of this nonsense */
#ifdef index
#undef index
#endif /* index */

#ifndef MAXINT
#define MAXINT 0x7fffffff
#endif

#endif /* ENCODE_ALL_INCLUDED */
/*===========================================================================*
 * ansi.h								     *
 *									     *
 *	macro for non-ansi compilers					     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.6  1995/08/15 23:43:13  smoot
 *  *** empty log message ***
 *
 *  Revision 1.5  1995/01/19 23:54:35  eyhung
 *  Changed copyrights
 *
 * Revision 1.4  1994/11/12  02:12:13  keving
 * nothing
 *
 * Revision 1.3  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.2  1993/07/09  00:17:23  keving
 * nothing
 *
 * Revision 1.1  1993/06/14  22:50:22  keving
 * nothing
 *
 */


#ifndef ANSI_INCLUDED
#define ANSI_INCLUDED

/*  
 *  _ANSI_ARGS_ macro stolen from Tcl6.5 by John Ousterhout
 */
#undef _ANSI_ARGS_
#undef CONST             /*NOTE: replaced "const" by "CONST" NOTE*/
/*#ifdef NON_ANSI_COMPILER*/
#ifdef PCC
#define _ANSI_ARGS_(x)       ()
#define CONST
#else
#define _ANSI_ARGS_(x)   x
#define CONST const
#ifdef __cplusplus
#define VARARGS (...)
#else
#define VARARGS ()
#endif
#endif


#endif /* ANSI_INCLUDED */
/*===========================================================================*
 * general.h								     *
 *									     *
 *	very general stuff						     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.7  1995/08/04 23:34:13  smoot
 *  jpeg5 changed the silly HAVE_BOOLEAN define....
 *
 *  Revision 1.6  1995/01/19 23:54:49  eyhung
 *  Changed copyrights
 *
 * Revision 1.5  1994/11/12  02:12:48  keving
 * nothing
 *
 * Revision 1.4  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.3  1993/07/09  00:17:23  keving
 * nothing
 *
 * Revision 1.2  1993/06/03  21:08:53  keving
 * nothing
 *
 * Revision 1.1  1993/02/22  22:39:19  keving
 * nothing
 *
 */


#ifndef GENERAL_INCLUDED
#define GENERAL_INCLUDED


/* prototypes for library procedures
 *
 * if your /usr/include headers do not have these, then pass -DMISSING_PROTOS
 * to your compiler
 *
 */ 
#ifdef MISSING_PROTOS
int fprintf();
int fread();
int fflush();
int fclose();

int sscanf();
int bzero();
int bcopy();
int system();
int time();
int perror();

int socket();
int bind();
int listen();
int accept();
int connect();
int close();
int read();
int write();

int pclose();

#endif


/*===========*
 * CONSTANTS *
 *===========*/

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define SPACE ' '
#define TAB '\t'
#define SEMICOLON ';'
#define NULL_CHAR '\0'
#define NEWLINE '\n'


/*==================*
 * TYPE DEFINITIONS *
 *==================*/

#if defined(HAVE_POSIX_SYS)
typedef int boolean;
#endif

/* this is for JPEG stuff */
#define BOOLEAN_DEFINED
#define HAVE_BOOLEAN

/* LONG_32 should only be defined iff
 *	    1) long's are 32 bits and
 *	    2) int's are not
 */

typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;

#ifdef LONG_32		
typedef unsigned long u_int32_t;
#else
typedef unsigned int u_int32_t;
#endif

/* _H_INTTYPES is defined by an AIX include which also typedefs
 * the types int8, int16, and int32 but not the unsigned versions
 */
#ifndef _H_INTTYPES

typedef char int8;
typedef short int16;

#ifdef LONG_32		
typedef long int32;
#else
typedef int int32;
#endif

#endif

/*========*
 * MACROS *
 *========*/

#undef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#undef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef abs
#define abs(a) ((a) >= 0 ? (a) : -(a))


#endif
/*===========================================================================*
 * byteorder.h								     *
 *									     *
 *	stuff to handle different byte order				     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.3  1995/01/19  23:54:39  eyhung
 * Changed copyrights
 *
 * Revision 1.3  1995/01/19  23:54:39  eyhung
 * Changed copyrights
 *
 * Revision 1.2  1994/11/12  02:12:15  keving
 * nothing
 *
 * Revision 1.1  1993/07/22  22:24:23  keving
 * nothing
 *
 */


#if 0
/*NOTE*/
#include "general.h"
/*NOTE*/
#endif

#ifdef FORCE_BIG_ENDIAN
    /* leave byte order as it is */
#define htonl(x) (x)
#define ntohl(x) (x)
#define htons(x) (x)
#define ntohs(x) (x)
#else
#ifdef FORCE_LITTLE_ENDIAN
    /* need to reverse byte order */
    /* note -- we assume here that htonl is called on a variable, not a
     * constant; thus, this is not for general use, but works with bitio.c
     */
#define htonl(x)    \
    ((((unsigned char *)(&x))[0] << 24) |	\
     (((unsigned char *)(&x))[1] << 16) |	\
     (((unsigned char *)(&x))[2] << 8) |	\
     (((unsigned char *)(&x))[3]))
#define ntohl(x)    htonl(x)
#define htons(x)    \
    ((((unsigned char *)(&x))[0] << 8) |	\
     ((unsigned char *)(&x))[1])
#define ntohs(x)    htons(x)
#else
    /* let in.h handle it, if possible */		   
#include <sys/types.h>

#if defined(UNIX)
#include <netinet/in.h>
#endif

#endif /* FORCE_LITTLE_ENDIAN */
#endif /* FORCE_BIG_ENDIAN */
/*===========================================================================*
 * dct.h								     *
 *									     *
 *	DCT procedures							     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#ifndef DCT_INCLUDED
#define DCT_INCLUDED


#if 0
/*NOTE*/
#include "ansi.h"
/*NOTE*/
#endif



#define DCTSIZE     8   /* you really don't want to change this */
#define DCTSIZE_SQ 64   /* you really don't want to change this */

#define DCTSIZE2    DCTSIZE*DCTSIZE
typedef short DCTELEM;
typedef DCTELEM DCTBLOCK[DCTSIZE2];
typedef DCTELEM DCTBLOCK_2D[DCTSIZE][DCTSIZE];


/*  
 *  from mfwddct.c:
 */
extern void mp_fwd_dct_block2 _ANSI_ARGS_((DCTBLOCK_2D src, DCTBLOCK_2D dest));

/* jrevdct.c */
extern void init_pre_idct _ANSI_ARGS_((void ));
extern void mpeg_jrevdct _ANSI_ARGS_((DCTBLOCK data ));


/* We assume that right shift corresponds to signed division by 2 with
 * rounding towards minus infinity.  This is correct for typical "arithmetic
 * shift" instructions that shift in copies of the sign bit.  But some
 * C compilers implement >> with an unsigned shift.  For these machines you
 * must define RIGHT_SHIFT_IS_UNSIGNED.
 * RIGHT_SHIFT provides a proper signed right shift of an int32 quantity.
 * It is only applied with constant shift counts.  SHIFT_TEMPS must be
 * included in the variables of any routine using RIGHT_SHIFT.
 */

#ifdef RIGHT_SHIFT_IS_UNSIGNED
#define SHIFT_TEMPS     int32 shift_temp;
#define RIGHT_SHIFT(x,shft)  \
        ((shift_temp = (x)) < 0 ? \
         (shift_temp >> (shft)) | ((~((int32) 0)) << (32-(shft))) : \
         (shift_temp >> (shft)))
#else
#define SHIFT_TEMPS
#define RIGHT_SHIFT(x,shft)     ((x) >> (shft))
#endif


#endif /* DCT_INCLUDED */
/*===========================================================================*
 * mtypes.h								     *
 *									     *
 *	MPEG data types							     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.12  1995/04/14  23:12:11  smoot
 * added ChromBlock for future color MV searches
 *
 * Revision 1.11  1995/01/19  23:55:05  eyhung
 * Changed copyrights
 *
 * Revision 1.10  1994/11/14  22:48:57  smoot
 * added defines for Specifics operation
 *
 * Revision 1.9  1994/11/12  02:12:52  keving
 * nothing
 *
 * Revision 1.8  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.7  1993/07/09  00:17:23  keving
 * nothing
 *
 * Revision 1.6  1993/06/03  21:08:53  keving
 * nothing
 *
 * Revision 1.5  1993/02/17  23:18:20  dwallach
 * checkin prior to keving's joining the project
 *
 * Revision 1.4  1993/01/18  10:20:02  dwallach
 * *** empty log message ***
 *
 * Revision 1.3  1993/01/18  10:17:29  dwallach
 * RCS headers installed, code indented uniformly
 *
 * Revision 1.3  1993/01/18  10:17:29  dwallach
 * RCS headers installed, code indented uniformly
 *
 */


#ifndef MTYPES_INCLUDED
#define MTYPES_INCLUDED


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "general.h"
#include "dct.h"
/*NOTE*/
#endif


/*===========*
 * CONSTANTS *
 *===========*/

#define TYPE_BOGUS	0   /* for the header of the circular list */
#define TYPE_VIRGIN	1

#define STATUS_EMPTY	0
#define STATUS_LOADED	1
#define STATUS_WRITTEN	2


/*==================*
 * TYPE DEFINITIONS *
 *==================*/

/*  
 *  your basic Block type
 */
typedef int16 Block[DCTSIZE][DCTSIZE];
typedef int16 FlatBlock[DCTSIZE_SQ];
typedef	    int32   LumBlock[2*DCTSIZE][2*DCTSIZE];
typedef	    int32   ChromBlock[DCTSIZE][DCTSIZE];

/*========*
 * MACROS *
 *========*/

#ifdef ABS
#undef ABS
#endif

#define ABS(x) (((x)<0)?-(x):(x))

#ifdef HEINOUS_DEBUG_MODE
#define DBG_PRINT(x) {printf x; fflush(stdout);}
#else
#define DBG_PRINT(x)
#endif

#define ERRCHK(bool, str) {if (!(bool)) {perror(str); exit(1);}}

/* For Specifics */
typedef struct detalmv_def {
  int typ,fx,fy,bx,by;
} BlockMV;
#define TYP_SKIP 0
#define TYP_FORW 1
#define TYP_BACK 2
#define TYP_BOTH 3


#endif /* MTYPES_INCLUDED */


/*===========================================================================*
 * bitio.h								     *
 *									     *
 *	bitwise input/output						     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.8  1995/01/19 23:54:37  eyhung
 *  Changed copyrights
 *
 * Revision 1.7  1994/11/12  02:12:14  keving
 * nothing
 *
 * Revision 1.6  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.5  1993/07/09  00:17:23  keving
 * nothing
 *
 * Revision 1.4  1993/06/03  21:08:53  keving
 * nothing
 *
 * Revision 1.3  1993/01/18  10:20:02  dwallach
 * *** empty log message ***
 *
 * Revision 1.2  1993/01/18  10:17:29  dwallach
 * RCS headers installed, code indented uniformly
 *
 * Revision 1.2  1993/01/18  10:17:29  dwallach
 * RCS headers installed, code indented uniformly
 *
 */


#ifndef BIT_IO_INCLUDED
#define BIT_IO_INCLUDED


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "general.h"
#include "ansi.h"
/*NOTE*/
#endif


/*===========*
 * CONSTANTS *
 *===========*/

#define WORDS_PER_BUCKET 128
#define MAXBITS_PER_BUCKET	(WORDS_PER_BUCKET * 32)
#define	MAX_BUCKETS	128
#define MAX_BITS	MAX_BUCKETS*MAXBITS_PER_BUCKET


/*=======================*
 * STRUCTURE DEFINITIONS *
 *=======================*/

typedef struct bitBucket {
    struct bitBucket *nextPtr;
    u_int32_t bits[WORDS_PER_BUCKET];
    int bitsleft, bitsleftcur, currword;
} ActualBucket;

typedef struct _BitBucket {
    long totalbits;
    long cumulativeBits;
    long bitsWritten;
    FILE    *filePtr;
    ActualBucket *firstPtr;
    ActualBucket *lastPtr;
} BitBucket;


/*========*
 * MACROS *
 *========*/

#define	SET_ITH_BIT(bits, i)	(bits |= (1 << (i)))
#define	GET_ITH_BIT(bits, i)	(bits & (1 << (i)))


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

void	    Bitio_Free _ANSI_ARGS_((BitBucket *bbPtr));
void	    Bitio_Write _ANSI_ARGS_((BitBucket *bbPtr, u_int32_t bits, int nbits));
void	    Bitio_BytePad _ANSI_ARGS_((BitBucket *bbPtr));
BitBucket  *Bitio_New _ANSI_ARGS_((FILE *filePtr));
void	    Bitio_Flush _ANSI_ARGS_((BitBucket *bbPtr));
void	    Bitio_WriteToSocket _ANSI_ARGS_((BitBucket *bbPtr, int socket));


#endif /* BIT_IO_INCLUDED */
/*===========================================================================*
 * mheaders.h								     *
 *									     *
 *	MPEG headers							     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.4  1995/03/27  19:29:24  smoot
 * changed to remove mb_quant
 *
 * Revision 1.3  1995/01/19  23:54:56  eyhung
 * Changed copyrights
 *
 * Revision 1.2  1994/11/12  02:12:51  keving
 * nothing
 *
 * Revision 1.1  1993/07/22  22:24:23  keving
 * nothing
 *
 *
 */


#ifndef MHEADERS_INCLUDED
#define MHEADERS_INCLUDED


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "general.h"
#include "ansi.h"
#include "bitio.h"
/*NOTE*/
#endif


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

void	SetGOPStartTime _ANSI_ARGS_((int index));
void	Mhead_GenSequenceHeader _ANSI_ARGS_((BitBucket *bbPtr,
            u_int32_t hsize, u_int32_t vsize,
            int32 pratio, int32 pict_rate,
            int32 bit_rate, int32 buf_size,
            int32 c_param_flag, int32 *iq_matrix,
            int32 *niq_matrix, u_int8_t *ext_data,
          /*int32 ext_data_size, u_int8_t *user_data, int32 user_data_size));*/
            int32 ext_data_size, char *user_data, int32 user_data_size));
void	Mhead_GenSequenceEnder _ANSI_ARGS_((BitBucket *bbPtr));
void	Mhead_GenGOPHeader _ANSI_ARGS_((BitBucket *bbPtr,
	   int32 drop_frame_flag,
           int32 tc_hrs, int32 tc_min,
           int32 tc_sec, int32 tc_pict,
           int32 closed_gop, int32 broken_link,
           u_int8_t *ext_data, int32 ext_data_size,
           u_int8_t *user_data, int32 user_data_size));
void	Mhead_GenPictureHeader _ANSI_ARGS_((BitBucket *bbPtr, int frameType,
					    int pictCount, int f_code));
void	Mhead_GenSliceHeader _ANSI_ARGS_((BitBucket *bbPtr, u_int32_t slicenum,
					  u_int32_t qscale, u_int8_t *extra_info,
					  u_int32_t extra_info_size));
void	Mhead_GenSliceEnder _ANSI_ARGS_((BitBucket *bbPtr));
void	Mhead_GenMBHeader _ANSI_ARGS_((BitBucket *bbPtr,
	  u_int32_t pict_code_type, u_int32_t addr_incr,
          u_int32_t q_scale,
          u_int32_t forw_f_code, u_int32_t back_f_code,
          u_int32_t horiz_forw_r, u_int32_t vert_forw_r,
          u_int32_t horiz_back_r, u_int32_t vert_back_r,
          int32 motion_forw, int32 m_horiz_forw,
          int32 m_vert_forw, int32 motion_back,
          int32 m_horiz_back, int32 m_vert_back,
          u_int32_t mb_pattern, u_int32_t mb_intra));


#endif /* MHEADERS_INCLUDED */
/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  THIS FILE IS MACHINE GENERATED!  DO NOT EDIT!
 */
#define HUFF_MAXRUN	32
#define HUFF_MAXLEVEL	41

extern int huff_maxlevel[];
extern u_int32_t *huff_table[];
extern int *huff_bits[];
/*===========================================================================*
 * frame.h								     *
 *									     *
 *	basic frames procedures						     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#ifndef FRAME_INCLUDED
#define FRAME_INCLUDED

/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "general.h"
#include "ansi.h"
#include "mtypes.h"
/*NOTE*/
#endif


/*===========*
 * CONSTANTS *
 *===========*/
#define TYPE_IFRAME	2
#define TYPE_PFRAME	3
#define TYPE_BFRAME	4


/*=======================*
 * STRUCTURE DEFINITIONS *
 *=======================*/

typedef struct mpegFrame {
    int type;
    char    inputFileName[256];
    int id;           /* the frame number -- starts at 0 */
    boolean inUse;	/* TRUE iff this frame is currently being used */
			/* FALSE means any data here can be thrashed */

    u_int8_t   **ppm_data;
    xel **rgb_data;         /* pnm format -- see pbmplus docs */
    xelval rgb_maxval;      /* largest value of any pixel index */
    int rgb_format;         /* more info from pnm */

    /*  
     *  now, the YCrCb data.  All pixel information is stored in unsigned
     *  8-bit pieces.  We separate y, cr, and cb because cr and cb are
     *  subsampled by a factor of 2.
     */
    u_int8_t **orig_y, **orig_cr, **orig_cb;

    /* now, the decoded data -- relevant only if
     *	    referenceFrame == DECODED_FRAME
     *
     */
    u_int8_t **decoded_y, **decoded_cr, **decoded_cb;

    /* reference data */
    u_int8_t **ref_y, **ref_cr, **ref_cb;

    /*  
     *  these are the Blocks which will ultimately compose MacroBlocks.
     *  A Block is in a format that mp_fwddct() can crunch.
     */
    Block **y_blocks, **cr_blocks, **cb_blocks;

    /*
     *  this is the half-pixel luminance data (for reference frames)
     */
    u_int8_t **halfX, **halfY, **halfBoth;

    boolean   halfComputed;        /* TRUE iff half-pixels already computed */

    struct mpegFrame *next;  /* points to the next B-frame to be encoded, if
		       * stdin is used as the input. 
		       */
} MpegFrame;


extern MpegFrame *Frame_New _ANSI_ARGS_((int id, int type, MpegFrame **frM));
extern void	  Frame_Init _ANSI_ARGS_((MpegFrame **frM));
extern void	  Frame_Free _ANSI_ARGS_((MpegFrame *frame));
extern void	  Frame_Exit _ANSI_ARGS_((MpegFrame **frM));
extern void	  Frame_AllocPPM _ANSI_ARGS_((MpegFrame * frame));
extern void	  Frame_AllocYCC _ANSI_ARGS_((MpegFrame * mf));
extern void	  Frame_AllocDecoded _ANSI_ARGS_((MpegFrame *frame,
						  boolean makeReference));
extern void	  Frame_AllocHalf _ANSI_ARGS_((MpegFrame *frame));
extern void	  Frame_AllocBlocks _ANSI_ARGS_((MpegFrame *mf));
extern void	  Frame_Resize _ANSI_ARGS_((MpegFrame *omf, MpegFrame *mf,
					    int insize_x, int insize_y,
					    int outsize_x, int outsize_y));


#endif /* FRAME_INCLUDED */
/*===========================================================================*
 * frames.h								     *
 *									     *
 *	stuff dealing with frames					     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.13  1995/08/15 23:43:04  smoot
 *  *** empty log message ***
 *
 * Revision 1.12  1995/04/14  23:13:18  smoot
 * Reorganized for better rate control.  Added overflow in DCT values
 * handling.
 *
 * Revision 1.11  1995/01/19  23:54:46  smoot
 * allow computediffdcts to un-assert parts of the pattern
 *
 * Revision 1.10  1995/01/16  07:43:10  eyhung
 * Added realQuiet
 *
 * Revision 1.9  1995/01/10  23:15:28  smoot
 * Fixed searchRange lack of def
 *
 * Revision 1.8  1994/11/15  00:55:36  smoot
 * added printMSE
 *
 * Revision 1.7  1994/11/14  22:51:02  smoot
 * added specifics flag.  Added BlockComputeSNR parameters
 *
 * Revision 1.6  1994/11/01  05:07:23  darryl
 *  with rate control changes added
 *
 * Revision 1.1  1994/09/27  01:02:55  darryl
 * Initial revision
 *
 * Revision 1.5  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.4  1993/07/09  00:17:23  keving
 * nothing
 *
 * Revision 1.3  1993/06/03  21:08:53  keving
 * nothing
 *
 * Revision 1.2  1993/03/02  19:00:27  keving
 * nothing
 *
 * Revision 1.1  1993/02/19  20:15:51  keving
 * nothing
 *
 */


#ifndef FRAMES_INCLUDED
#define FRAMES_INCLUDED

/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "ansi.h"
#include "mtypes.h"
#include "mheaders.h"
#include "frame.h"
/*NOTE*/
#endif


/*===========*
 * CONSTANTS *
 *===========*/

#define I_FRAME	1
#define P_FRAME 2
#define	B_FRAME	3

#define LUM_BLOCK   0
#define	CHROM_BLOCK 1
#define	CR_BLOCK    2
#define CB_BLOCK    3

#define	MOTION_FORWARD	    0
#define MOTION_BACKWARD	    1
#define MOTION_INTERPOLATE  2


#define USE_HALF    0
#define	USE_FULL    1

    /* motion vector stuff */
#define FORW_F_CODE fCode	    /* from picture header */
#define BACK_F_CODE fCode
#define FORW_F	(1 << (FORW_F_CODE - 1))
#define	BACK_F	(1 << (BACK_F_CODE - 1))
#define RANGE_NEG	(-(1 << (3 + FORW_F_CODE)))
#define RANGE_POS	((1 << (3 + FORW_F_CODE))-1)
#define MODULUS		(1 << (4 + FORW_F_CODE))

#define ORIGINAL_FRAME	0
#define DECODED_FRAME	1


/*=======================*
 * STRUCTURE DEFINITIONS *
 *=======================*/

typedef	struct FrameTableStruct {
    /* the following are all initted once and never changed */
    /* (they depend only on the pattern */
    char typ;
    struct FrameTableStruct *next;
    struct FrameTableStruct *prev;

    /* nextOutput is a pointer to next frame table entry to output */
    struct FrameTableStruct *nextOutput;

    boolean	freeNow;	/* TRUE iff no frames point back to this */

    int number;

    int	bFrameNumber;		/* actual frame number, if a b-frame */
    
} FrameTable;


/*==================*
 * TYPE DEFINITIONS *
 *==================*/

typedef struct dct_data_tye_struct {
  char useMotion;
  char pattern, mode;
  int fmotionX, fmotionY, bmotionX, bmotionY;
} dct_data_type;

void	EncodeYDC _ANSI_ARGS_((int32 dc_term, int32 *pred_term, BitBucket *bb));
void EncodeCDC _ANSI_ARGS_((int32 dc_term, int32 *pred_term, BitBucket *bb));


/*========*
 * MACROS *
 *========*/

#define FRAME_TYPE(num)	    framePattern[num % framePatternLen]

/* return ceiling(a/b) where a, b are ints, using temp value c */
#define int_ceil_div(a,b,c)     ((b*(c = a/b) < a) ? (c+1) : c)
#define int_floor_div(a,b,c)	((b*(c = a/b) > a) ? (c-1) : c)

/* assumes many things:
 * block indices are (y,x)
 * variables y_dc_pred, cr_dc_pred, and cb_dc_pred
 * flat block fb exists
 */
#define	GEN_I_BLOCK(frameType, frame, bb, mbAI, qscale)	{                   \
	boolean overflow, overflowChange=FALSE;                             \
        int overflowValue = 0;                                              \
        do {                                                                \
	  overflow =  Mpost_QuantZigBlock(dct[y][x], fb[0],                 \
			 qscale, TRUE)==MPOST_OVERFLOW;                     \
          overflow |= Mpost_QuantZigBlock(dct[y][x+1], fb[1],               \
		         qscale, TRUE)==MPOST_OVERFLOW;                     \
	  overflow |= Mpost_QuantZigBlock(dct[y+1][x], fb[2],               \
                         qscale, TRUE)==MPOST_OVERFLOW;                     \
	  overflow |= Mpost_QuantZigBlock(dct[y+1][x+1], fb[3],             \
                         qscale, TRUE)==MPOST_OVERFLOW;                     \
	  overflow |= Mpost_QuantZigBlock(dctb[y >> 1][x >> 1],             \
                         fb[4], qscale, TRUE)==MPOST_OVERFLOW;              \
	  overflow |= Mpost_QuantZigBlock(dctr[y >> 1][x >> 1],             \
			 fb[5], qscale, TRUE)==MPOST_OVERFLOW;              \
          if ((overflow) && (qscale!=31)) {                                 \
           overflowChange = TRUE; overflowValue++;                          \
	   qscale++;                                                        \
	   } else overflow = FALSE;                                         \
	} while (overflow);                                                 \
        Mhead_GenMBHeader(bb,						    \
		    frameType /* pict_code_type */, mbAI /* addr_incr */,   \
		    qscale /* q_scale */,	                            \
		    0 /* forw_f_code */, 0 /* back_f_code */,		    \
		    0 /* horiz_forw_r */, 0 /* vert_forw_r */,		    \
		    0 /* horiz_back_r */, 0 /* vert_back_r */,		    \
		    0 /* motion_forw */, 0 /* m_horiz_forw */,		    \
		    0 /* m_vert_forw */, 0 /* motion_back */,		    \
		    0 /* m_horiz_back */, 0 /* m_vert_back */,		    \
		    0 /* mb_pattern */, TRUE /* mb_intra */);		    \
									    \
	/* Y blocks */							    \
        EncodeYDC(fb[0][0], &y_dc_pred, bb);		               	    \
	Mpost_RLEHuffIBlock(fb[0], bb);				            \
	EncodeYDC(fb[1][0], &y_dc_pred, bb);			            \
        Mpost_RLEHuffIBlock(fb[1], bb);			                    \
	EncodeYDC(fb[2][0], &y_dc_pred, bb);			            \
	Mpost_RLEHuffIBlock(fb[2], bb);				            \
	EncodeYDC(fb[3][0], &y_dc_pred, bb);			            \
	Mpost_RLEHuffIBlock(fb[3], bb);				            \
									    \
	/* CB block */							    \
	EncodeCDC(fb[4][0], &cb_dc_pred, bb);				    \
	Mpost_RLEHuffIBlock(fb[4], bb);					    \
									    \
	/* CR block */							    \
	EncodeCDC(fb[5][0], &cr_dc_pred, bb);				    \
	Mpost_RLEHuffIBlock(fb[5], bb);					    \
	if (overflowChange) qscale -= overflowValue;                        \
    }

#define	BLOCK_TO_FRAME_COORD(bx1, bx2, x1, x2) {    \
	x1 = (bx1)*DCTSIZE;			    \
	x2 = (bx2)*DCTSIZE;			    \
    }

#define MOTION_TO_FRAME_COORD(bx1, bx2, mx1, mx2, x1, x2) { \
	x1 = (bx1)*DCTSIZE+(mx1);			    \
	x2 = (bx2)*DCTSIZE+(mx2);			    \
    }

#define COORD_IN_FRAME(fy,fx, type)					\
    ((type == LUM_BLOCK) ?						\
     ((fy >= 0) && (fx >= 0) && (fy < Fsize_y) && (fx < Fsize_x)) :	\
     ((fy >= 0) && (fx >= 0) && (fy < (Fsize_y>>1)) && (fx < (Fsize_x>>1))))

#define ENCODE_MOTION_VECTOR(x,y,xq, yq, xr, yr, f) {			\
	int	tempC;							\
									\
	if ( x < RANGE_NEG )	    tempX = x + MODULUS;		\
	else if ( x > RANGE_POS ) tempX = x - MODULUS;			\
	else				    tempX = x;			\
									\
	if ( y < RANGE_NEG )	    tempY = y + MODULUS;		\
	else if ( y > RANGE_POS ) tempY = y - MODULUS;			\
	else				    tempY = y;			\
									\
	if ( tempX >= 0 ) {						\
	    xq = int_ceil_div(tempX, f, tempC);				\
	    xr = f - 1 + tempX - xq*f;					\
	} else {							\
	    xq = int_floor_div(tempX, f, tempC);			\
	    xr = f - 1 - tempX + xq*f;					\
	}								\
									\
	if ( tempY >= 0 ) {						\
	    yq = int_ceil_div(tempY, f, tempC);				\
	    yr = f - 1 + tempY - yq*f;					\
	} else {							\
	    yq = int_floor_div(tempY, f, tempC);			\
	    yr = f - 1 - tempY + yq*f;					\
	}								\
    }


#define DoQuant(bit, src, dest)                                         \
  if (pattern & bit) {                                                  \
    switch (Mpost_QuantZigBlock(src, dest, QScale, FALSE)) {            \
    case MPOST_NON_ZERO:                                                \
      break;                                                            \
    case MPOST_ZERO:                                                    \
      pattern ^= bit;                                                   \
      break;                                                            \
    case MPOST_OVERFLOW:                                                \
      if (QScale != 31) {                                               \
	QScale++;                                                       \
	overflowChange = TRUE;                                          \
	overflowValue++;                                                \
	goto calc_blocks;                                               \
      }                                                                 \
      break;                                                            \
    }                                                                   \
  }

/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

void	ComputeBMotionLumBlock _ANSI_ARGS_((MpegFrame *prev, MpegFrame *next,
			       int by, int bx, int mode, int fmy, int fmx,
			       int bmy, int bmx, LumBlock motionBlock));
int	BMotionSearch _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prev, MpegFrame *next,
		      int by, int bx, int *fmy, int *fmx, int *bmy, int *bmx, int oldMode));


void	ComputeDiffDCTs _ANSI_ARGS_((MpegFrame *current, MpegFrame *prev, int by, int bx,
			int my, int mx, int *pattern));
int	ComputeDiffDCTBlock _ANSI_ARGS_((Block current, Block dest, Block motionBlock));
void	ComputeMotionBlock _ANSI_ARGS_((u_int8_t **prev, int by, int bx, int my, int mx,
			   Block motionBlock));
void	ComputeMotionLumBlock _ANSI_ARGS_((MpegFrame *prevFrame, int by,
					   int bx, int my, int mx,
					   LumBlock motionBlock));
int32	ComputeBlockMAD _ANSI_ARGS_((Block current, Block prev));

void	GenIFrame _ANSI_ARGS_((BitBucket *bb, MpegFrame *mf));
void	GenPFrame _ANSI_ARGS_((BitBucket *bb, MpegFrame *current, MpegFrame *prev));
void	GenBFrame _ANSI_ARGS_((BitBucket *bb, MpegFrame *curr, MpegFrame *prev, MpegFrame *next));
void    AllocDctBlocks _ANSI_ARGS_((void ));

float	ShowIFrameSummary _ANSI_ARGS_((int inputFrameBits, int32 totalBits, FILE *fpointer));
float	ShowPFrameSummary _ANSI_ARGS_((int inputFrameBits, int32 totalBits, FILE *fpointer));
float	ShowBFrameSummary _ANSI_ARGS_((int inputFrameBits, int32 totalBits, FILE *fpointer));


/* DIFFERENCE FUNCTIONS */

int32    LumBlockMAD _ANSI_ARGS_((LumBlock currentBlock, LumBlock motionBlock, int32 bestSoFar));
int32    LumBlockMSE _ANSI_ARGS_((LumBlock currentBlock, LumBlock motionBlock, int32 bestSoFar));
int32	LumMotionError _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prev,
				    int by, int bx, int my, int mx,
				    int32 bestSoFar));
int32	LumAddMotionError _ANSI_ARGS_((LumBlock currentBlock,
				       LumBlock blockSoFar, MpegFrame *prev,
				       int by, int bx, int my, int mx,
				       int32 bestSoFar));
int32	LumMotionErrorA _ANSI_ARGS_((LumBlock current, MpegFrame *prevFrame,
				     int by, int bx, int my, int mx,
				     int32 bestSoFar));
int32	LumMotionErrorB _ANSI_ARGS_((LumBlock current, MpegFrame *prevFrame,
				     int by, int bx, int my, int mx,
				     int32 bestSoFar));
int32	LumMotionErrorC _ANSI_ARGS_((LumBlock current, MpegFrame *prevFrame,
				     int by, int bx, int my, int mx,
				     int32 bestSoFar));
int32	LumMotionErrorD _ANSI_ARGS_((LumBlock current, MpegFrame *prevFrame,
				     int by, int bx, int my, int mx,
				     int32 bestSoFar));
int32	LumMotionErrorSubSampled _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prevFrame,
			  int by, int bx, int my, int mx,
			  int startY, int startX));
void	BlockComputeSNR _ANSI_ARGS_((MpegFrame *current,
				float *snr, float *psnr));
int32   time_elapsed _ANSI_ARGS_((void));
void    AllocDctBlocks _ANSI_ARGS_((void));

/*==================*
 * GLOBAL VARIABLES *
 *==================*/

extern int pixelFullSearch;
extern int searchRangeP,searchRangeB;
extern int qscaleI;
extern int gopSize;
extern int slicesPerFrame;
extern int blocksPerSlice;
extern int referenceFrame;
extern int specificsOn;
extern int quietTime;		/* shut up for at least quietTime seconds;
				 * negative means shut up forever
				 */
extern boolean realQuiet;	/* TRUE = no messages to stdout */

extern boolean frameSummary;	/* TRUE = frame summaries should be printed */
extern boolean	printSNR;
extern boolean	printMSE;
extern boolean	decodeRefFrames;    /* TRUE = should decode I and P frames */
extern int	fCodeI,fCodeP,fCodeB;
extern boolean	  forceEncodeLast;
extern int TIME_RATE;

#endif /* FRAMES_INCLUDED */
/*===========================================================================*
 *  specifics.h                                                              *
 *									     *
 *	reading the specifics file					     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.4  1995/04/14  23:11:39  smoot
 * made types smalled
 *
 * Revision 1.3  1995/01/19  23:55:23  eyhung
 * Changed copyrights
 *
 * Revision 1.2  1995/01/17  21:53:32  smoot
 * killed prototypes
 *
 * Revision 1.1  1994/11/15  00:40:35  smoot
 * Initial revision
 *
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "ansi.h"
/*NOTE*/
#endif


/*===========*
 * TYPES     *
 *===========*/

typedef struct bs_def {
  int num;
  boolean relative;
  char qscale;
  BlockMV *mv;  /* defined in mtypes.h */
  struct bs_def *next;
} Block_Specifics;

typedef struct detail_def {
  int num;
  char qscale;
  struct detail_def *next;
}  Slice_Specifics;

typedef struct fsl_def {
  int framenum; 
  int frametype;
  char qscale;
  Slice_Specifics *slc;
  Block_Specifics *bs;
  struct fsl_def *next;
} FrameSpecList;

/*===========*
 * CONSTANTS *
 *===========*/


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

/*===========================================================================*
 * prototypes.h								     *
 *									     *
 *	miscellaneous prototypes					     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.9  1995/03/30 01:46:58  smoot
 *  added SpecType
 *
 * Revision 1.8  1995/03/21  00:27:10  smoot
 * added pnm stuff
 *
 * Revision 1.7  1995/02/18  01:48:27  smoot
 * changed SpecLookup for version 2 specfiles
 *
 * Revision 1.6  1995/01/19  23:55:14  eyhung
 * Changed copyrights
 *
 * Revision 1.5  1995/01/17  21:53:10  smoot
 * Added Specs like
 * Make that: Added specifics prototypes
 *
 * Revision 1.4  1994/12/07  00:42:01  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.3  1994/11/12  02:12:56  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.1  1993/07/09  00:17:23  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "general.h"
#include "ansi.h"
#include "frame.h"
/*NOTE*/
#endif


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

int	GetBQScale _ANSI_ARGS_((void));
int	GetPQScale _ANSI_ARGS_((void));
void	ResetBFrameStats _ANSI_ARGS_((void));
void	ResetPFrameStats _ANSI_ARGS_((void));
void	SetSearchRange _ANSI_ARGS_((int pixelsP, int pixelsB));
void	ResetIFrameStats _ANSI_ARGS_((void));
void	SetPixelSearch _ANSI_ARGS_((char *searchType));
void	SetIQScale _ANSI_ARGS_((int qI));
void	SetPQScale _ANSI_ARGS_((int qP));
void	SetBQScale _ANSI_ARGS_((int qB));
float	EstimateSecondsPerIFrame _ANSI_ARGS_((void));
float	EstimateSecondsPerPFrame _ANSI_ARGS_((void));
float	EstimateSecondsPerBFrame _ANSI_ARGS_((void));
void	SetGOPSize _ANSI_ARGS_((int size));
void	SetStatFileName _ANSI_ARGS_((char *fileName));
void	SetSlicesPerFrame _ANSI_ARGS_((int number));
void	SetBlocksPerSlice _ANSI_ARGS_((void));


void DCTFrame _ANSI_ARGS_((MpegFrame * mf));

void PPMtoYCC _ANSI_ARGS_((MpegFrame * mf));

void	MotionSearchPreComputation _ANSI_ARGS_((MpegFrame *frame));
boolean	PMotionSearch _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prev,
				   int by, int bx, int *motionY, int *motionX));
void	ComputeHalfPixelData _ANSI_ARGS_((MpegFrame *frame));
void mp_validate_size _ANSI_ARGS_((int *x, int *y));

/* block.c */
void	BlockToData _ANSI_ARGS_((u_int8_t **data, Block block, int by, int bx));
void	AddMotionBlock _ANSI_ARGS_((Block block, u_int8_t **prev, int by, int bx,
		       int my, int mx));
void	AddBMotionBlock _ANSI_ARGS_((Block block, u_int8_t **prev, u_int8_t **next,
				     int by, int bx, int mode,
				     int fmy, int fmx, int bmy, int bmx));

void	BlockifyFrame _ANSI_ARGS_((MpegFrame *frame));



extern void	SetFCode _ANSI_ARGS_((void));


/* frametype.c */
int	FType_Type _ANSI_ARGS_((int frameNum));
int	FType_FutureRef _ANSI_ARGS_((int currFrameNum));
int	FType_PastRef _ANSI_ARGS_((int currFrameNum));
void    SetFramePattern _ANSI_ARGS_((char *pattern));
void    ComputeFrameTable _ANSI_ARGS_((void));

/* psearch.c */
void	ShowPMVHistogram _ANSI_ARGS_((FILE *fpointer));
void	ShowBBMVHistogram _ANSI_ARGS_((FILE *fpointer));
void	ShowBFMVHistogram _ANSI_ARGS_((FILE *fpointer));

/* specifics.c */
void	Specifics_Init _ANSI_ARGS_((void));
int     SpecLookup _ANSI_ARGS_((int fn, int typ, int num, 
				BlockMV **info, int start_qs));
int SpecTypeLookup _ANSI_ARGS_((int fn));

/* libpnm.c */
/*
void pnm_promoteformat _ANSI_ARGS_(( xel** xels, int cols, int rows, xelval maxval,
                                     int format, xelval newmaxval, int newformat ));
void pnm_promoteformatrow _ANSI_ARGS_(( xel* xelrow, int cols, xelval maxval, 
                                        int format, xelval newmaxval, int newformat ));
*/

/*===========================================================================*
 * search.h								     *
 *									     *
 *	stuff dealing with the motion search				     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.6  1995/08/15 23:43:36  smoot
 *  *** empty log message ***
 *
 *  Revision 1.5  1995/01/19 23:55:20  eyhung
 *  Changed copyrights
 *
 * Revision 1.4  1994/12/07  00:42:01  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.3  1994/11/12  02:12:58  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.1  1993/07/09  00:17:23  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "ansi.h"
/*NOTE*/
#endif


/*===========*
 * CONSTANTS *
 *===========*/

#define PSEARCH_SUBSAMPLE   0
#define PSEARCH_EXHAUSTIVE  1
#define	PSEARCH_LOGARITHMIC 2
#define	PSEARCH_TWOLEVEL    3

#define BSEARCH_EXHAUSTIVE  0
#define BSEARCH_CROSS2	    1
#define BSEARCH_SIMPLE	    2


/*========*
 * MACROS *
 *========*/

#define COMPUTE_MOTION_BOUNDARY(by,bx,stepSize,leftMY,leftMX,rightMY,rightMX)\
    leftMY = -2*DCTSIZE*by;	/* these are valid motion vectors */	     \
    leftMX = -2*DCTSIZE*bx;						     \
			    	/* these are invalid motion vectors */	     \
    rightMY = 2*(Fsize_y - (by+2)*DCTSIZE + 1) - 1;			     \
    rightMX = 2*(Fsize_x - (bx+2)*DCTSIZE + 1) - 1;			     \
									     \
    if ( stepSize == 2 ) {						     \
	rightMY++;							     \
	rightMX++;							     \
    }
    
#define VALID_MOTION(y,x)	\
    (((y) >= leftMY) && ((y) < rightMY) &&   \
     ((x) >= leftMX) && ((x) < rightMX) )


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

void	SetPSearchAlg _ANSI_ARGS_((char *alg));
void	SetBSearchAlg _ANSI_ARGS_((char *alg));
char	*BSearchName _ANSI_ARGS_((void));
char	*PSearchName _ANSI_ARGS_((void));
int32	PLogarithmicSearch _ANSI_ARGS_((LumBlock currentBlock,
					MpegFrame *prev,
					int by, int bx,
					int *motionY, int *motionX, int searchRange));
int32	PSubSampleSearch _ANSI_ARGS_((LumBlock currentBlock,
					      MpegFrame *prev, int by, int bx,
					      int *motionY, int *motionX, int searchRange));
int32	PLocalSearch _ANSI_ARGS_((LumBlock currentBlock,
					  MpegFrame *prev, int by, int bx,
					  int *motionY, int *motionX,
					  int32 bestSoFar, int searchRange));
int32	PTwoLevelSearch _ANSI_ARGS_((LumBlock currentBlock,
				     MpegFrame *prev, int by, int bx,
				     int *motionY, int *motionX,
				     int32 bestSoFar, int searchRange));
boolean PMotionSearch _ANSI_ARGS_((LumBlock currentBlock,
				   MpegFrame *prev,
				   int by, int bx,
				   int *motionY, int *motionX));


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

extern int psearchAlg;

/*===========================================================================*
 * fsize.h								     *
 *									     *
 *	procedures to deal with frame size				     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#ifndef FSIZE_INCLUDED
#define FSIZE_INCLUDED

/*==================*
 * GLOBAL VARIABLES *
 *==================*/
extern int Fsize_x;
extern int Fsize_y;


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

extern void Fsize_Reset _ANSI_ARGS_((void));
extern void Fsize_Validate _ANSI_ARGS_((int *x, int *y));
extern void Fsize_Note _ANSI_ARGS_((int id, int width, int height));


#endif /* FSIZE_INCLUDED */
/*===========================================================================*
 * mpeg.h								     *
 *									     *
 *	no comment							     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.2  1995/01/19  23:54:58  eyhung
 * Changed copyrights
 *
 * Revision 1.1  1994/11/12  02:12:51  keving
 * nothing
 *
 * Revision 1.1  1994/10/07  04:24:40  darryl
 * Initial revision
 *
 * Revision 1.4  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.3  1993/07/09  00:17:23  keving
 * nothing
 *
 * Revision 1.2  1993/06/03  21:08:53  keving
 * nothing
 *
 * Revision 1.1  1993/02/17  23:18:20  dwallach
 * Initial revision
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "ansi.h"
#include "mtypes.h"
#include "frame.h"
/*NOTE*/
#endif


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

int32 GenMPEGStream _ANSI_ARGS_((int whichGOP, int frameStart, int frameEnd,
				 int32 qtable[], int32 niqtable[],
				 int numFrames, FILE *ofp,
				 char *outputFileName));
extern void	PrintStartStats _ANSI_ARGS_((int firstFrame, int lastFrame));
extern void	IncrementTCTime _ANSI_ARGS_((void));
void	SetReferenceFrameType _ANSI_ARGS_((char *type));
boolean	NonLocalRefFrame _ANSI_ARGS_((int id));
extern void ReadDecodedRefFrame _ANSI_ARGS_((MpegFrame *frame,
					     int frameNumber));
extern void	WriteDecodedFrame _ANSI_ARGS_((MpegFrame *frame));
extern void	SetBitRateFileName _ANSI_ARGS_((char *fileName));
extern void	SetFrameRate _ANSI_ARGS_((void));


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

extern int32	  tc_hrs, tc_min, tc_sec, tc_pict, tc_extra;
extern int	  totalFramesSent;
extern int	  gopSize;
extern char	 *framePattern;
extern int	  framePatternLen;
extern int32 qtable[];
extern int32 niqtable[];
extern int32 *customQtable;
extern int32 *customNIQtable;
extern int  aspectRatio;
extern int  frameRate;
extern int     frameRateRounded;
extern boolean    frameRateInteger;
#if 0
/*NOTE*/
extern MpegFrame *frameMemory[3];
extern int32	  tc_hrs, tc_min, tc_sec, tc_pict, tc_extra;
extern int	  totalFramesSent;
extern int	  gopSize;
extern char	 *framePattern;
extern int	  framePatternLen;
extern int32 qtable[];
extern int32 niqtable[];
extern int32 *customQtable;
extern int32 *customNIQtable;
extern int  aspectRatio;
extern int  frameRate;
extern int     frameRateRounded;
extern boolean    frameRateInteger;
/*NOTE*/
#endif

/*===========================================================================*
 * param.h								     *
 *									     *
 *	reading the parameter file					     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.8  1995/05/02  01:51:08  eyhung
 * added VidRateNum for determining frame rate
 *
 * Revision 1.7  1995/01/19  23:55:10  eyhung
 * Changed copyrights
 *
 * Revision 1.6  1995/01/17  04:47:26  eyhung
 * added coding-on-the-fly variable
 *
 * Revision 1.5  1995/01/16  06:06:58  eyhung
 * added yuvConversion global variable
 *
 * Revision 1.4  1994/12/08  20:13:28  smoot
 * Killed linux MAXPATHLEN warning
 *
 * Revision 1.3  1994/11/12  02:12:54  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.1  1993/07/09  00:17:23  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "ansi.h"
/*NOTE*/
#endif


/*===========*
 * CONSTANTS *
 *===========*/

#define MAX_MACHINES	    256
#ifndef MAXPATHLEN
#define MAXPATHLEN  1024
#endif

#define	ENCODE_FRAMES	0
#define COMBINE_GOPS	1
#define COMBINE_FRAMES	2


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

boolean	ReadParamFile _ANSI_ARGS_((char *fileName, int function));
void	GetNthInputFileName _ANSI_ARGS_((char *fileName, int n));
extern void    JM2JPEG _ANSI_ARGS_((void));


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

extern int numInputFiles;
extern char	outputFileName[256];
extern int	whichGOP;
extern int numMachines;
extern char	machineName[MAX_MACHINES][256];
extern char	userName[MAX_MACHINES][256];
extern char	executable[MAX_MACHINES][1024];
extern char	remoteParamFile[MAX_MACHINES][1024];
extern boolean	remote[MAX_MACHINES];
extern boolean	childProcess;
extern char	currentPath[MAXPATHLEN];
extern char inputConversion[1024];
extern char yuvConversion[256];
extern int  yuvWidth, yuvHeight;
extern int  realWidth, realHeight;
extern char ioConversion[1024];
extern char slaveConversion[1024];
extern FILE *bitRateFile;
extern boolean showBitRatePerFrame;
extern boolean computeMVHist;
extern boolean stdinUsed;
extern const double VidRateNum[9];
/*===========================================================================*
 * readframe.h								     *
 *									     *
 *	stuff dealing with reading frames				     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.6  1995/01/19 23:55:17  eyhung
 *  Changed copyrights
 *
 * Revision 1.5  1995/01/14  00:05:54  smoot
 * *** empty log message ***
 *
 * Revision 1.4  1995/01/13  23:44:54  smoot
 * added B&W (Y files)
 *
 * Revision 1.3  1994/11/12  02:12:57  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.1  1993/07/09  00:17:23  keving
 * nothing
 *
 */


/*===========*
 * CONSTANTS *
 *===========*/

#define	PPM_FILE_TYPE	    0
#define YUV_FILE_TYPE	    2
#define ANY_FILE_TYPE	    3
#define BASE_FILE_TYPE	    4
#define PNM_FILE_TYPE	    5
#define SUB4_FILE_TYPE	    6
#define JPEG_FILE_TYPE	    7
#define JMOVIE_FILE_TYPE    8
#define Y_FILE_TYPE	    9
#define SIGNED_YUV_FILE_TYPE 10

/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

extern void	ReadFrame _ANSI_ARGS_((MpegFrame *frame, char *fileName,
				       char *conversion, boolean addPath));
extern void	SetFileType _ANSI_ARGS_((char *conversion));
extern void	SetFileFormat _ANSI_ARGS_((char *format));
extern FILE	*ReadIOConvert _ANSI_ARGS_((char *fileName));
extern void	SetResize _ANSI_ARGS_((boolean set));


extern int	baseFormat;
/*===========================================================================*
 * rgbtoycc.h								     *
 *									     *
 *	Procedures to convert from RGB space to YUV space		     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */



#if 0
/*NOTE*/
#include "frame.h"
/*NOTE*/
#endif


extern void PNMtoYUV _ANSI_ARGS_((MpegFrame * frame));
extern void PPMtoYUV _ANSI_ARGS_((MpegFrame * frame));
/*===========================================================================*
 * parallel.h								     *
 *									     *
 *	parallel encoding						     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.5  1995/08/15 23:43:26  smoot
 *  *** empty log message ***
 *
 *  Revision 1.4  1995/01/19 23:55:08  eyhung
 *  Changed copyrights
 *
 * Revision 1.3  1994/11/12  02:12:53  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.1  1993/07/09  00:17:23  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "ansi.h"
#include "bitio.h"
#include "frame.h"
/*NOTE*/
#endif


/*===============================*
 * EXTERNAL PROCEDURE prototypes *
 *===============================*/

extern void StartMasterServer _ANSI_ARGS_((int numInputFiles,
					     char *paramFile,
					     char *outputFileName));
extern boolean	NotifyMasterDone _ANSI_ARGS_((char *hostName, int portNum,
					      int machineNumber,
					      int seconds,
					      int *frameStart, int *frameEnd));
extern void	StartIOServer _ANSI_ARGS_((int numInputFiles,
					   char *hostName, int portNum));
extern void	StartCombineServer _ANSI_ARGS_((int numInputFiles,
					       char *outputFileName,
					       char *hostName, int portNum));
extern void	StartDecodeServer _ANSI_ARGS_((int numInputFiles,
					       char *outputFileName,
					       char *hostName, int portNum));
extern void	WaitForOutputFile _ANSI_ARGS_((int number));
extern void	GetRemoteFrame _ANSI_ARGS_((MpegFrame *frame, int frameNumber));
extern void	SendRemoteFrame _ANSI_ARGS_((int frameNumber, BitBucket *bb));
extern void	NoteFrameDone _ANSI_ARGS_((int frameStart, int frameEnd));
extern void	SetIOConvert _ANSI_ARGS_((boolean separate));
void	SetRemoteShell _ANSI_ARGS_((char *shell));
extern void	NotifyDecodeServerReady _ANSI_ARGS_((int id));
extern void	WaitForDecodedFrame _ANSI_ARGS_((int id));
extern void	SendDecodedFrame _ANSI_ARGS_((MpegFrame *frame));
extern void	GetRemoteDecodedRefFrame _ANSI_ARGS_((MpegFrame *frame,
						      int frameNumber));
extern void	SetParallelPerfect _ANSI_ARGS_((boolean val));


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

extern int parallelTestFrames;
extern int parallelTimeChunks;

extern char *IOhostName;
extern int ioPortNumber;
extern int combinePortNumber;
extern int decodePortNumber;

extern boolean	ioServer;
extern boolean	niceProcesses;
extern boolean	forceIalign;
extern int    machineNumber;
extern boolean remoteIO;
extern boolean	separateConversion;
/*===========================================================================*
 * postdct.h								     *
 *									     *
 *	MPEG post-DCT processing					     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */



int Mpost_QuantZigBlock _ANSI_ARGS_((Block in, FlatBlock out, int qscale,
					 int iblock));
void	Mpost_UnQuantZigBlock _ANSI_ARGS_((FlatBlock in, Block out,
					   int qscale, boolean iblock));
void	Mpost_RLEHuffIBlock _ANSI_ARGS_((FlatBlock in, BitBucket *out));
void	Mpost_RLEHuffPBlock _ANSI_ARGS_((FlatBlock in, BitBucket *out));

#define MPOST_ZERO 0
#define MPOST_NON_ZERO 1
#define MPOST_OVERFLOW (-1)
/*===========================================================================*
 * jpeg.h								     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 * Revision 1.2  1995/01/19  23:54:53  eyhung
 * Changed copyrights
 *
 * Revision 1.1  1994/11/12  02:12:49  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:24:23  keving
 * nothing
 *
 * Revision 1.1  1993/07/09  00:17:23  keving
 * nothing
 *
 */

#if 0
/*NOTE*/
#include "ansi.h"
/*NOTE*/
#endif


extern void     JMovie2JPEG _ANSI_ARGS_((char *infilename, char *obase,
                                        int start, int end));
extern void ReadJPEG _ANSI_ARGS_((MpegFrame * mf, FILE *fp));
/*===========================================================================*
 * rate.h								     *
 *									     *
 *	Procedures concerned with rate control
 *									     *
 * EXPORTED PROCEDURES:							     *
 *  getRateMode()
 *  setBitRate()
 *  getBitRate()
 *  setBufferSize()
 *  getBufferSize()
 *	initRateControl()
 *	targetRateControl()
 * 	updateRateControl()
 *	MB_RateOut()
 *									     *
 *===========================================================================*/

/* 	COPYRIGHT INFO HERE	*/

#define VARIABLE_RATE 0
#define FIXED_RATE 1


/*==================*
 * Exported VARIABLES *
 *==================*/


extern int rc_bitsThisMB;
extern int rc_numBlocks;
extern int rc_totalQuant;
extern int rc_quantOverride;


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * initRateControl
 *
 *	initialize the allocation parameters.
 *===========================================================================*/
extern int initRateControl _ANSI_ARGS_((void));


/*===========================================================================*
 *
 * targetRateControl
 *
 *      Determine the target allocation for given picture type.
 *
 * RETURNS:     target size in bits
 *===========================================================================*/
extern void  targetRateControl _ANSI_ARGS_((MpegFrame   *frame));


/*===========================================================================*
 *
 * MB_RateOut
 *
 *      Prints out sampling of MB rate control data.  Every "nth" block
 *	stats are printed, with "n" controled by global RC_MB_SAMPLE_RATE
 *
 * RETURNS:     nothing
 *===========================================================================*/
extern void MB_RateOut _ANSI_ARGS_((int type));


/*===========================================================================*
 *
 * updateRateControl
 *
 *      Update the statistics kept, after end of frame
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   many global variables
 *===========================================================================*/
extern void updateRateControl _ANSI_ARGS_((int type));


/*===========================================================================*
 *
 * needQScaleChange(current Q scale, 4 luminance blocks)
 *
 *
 * RETURNS:     new Qscale
 *===========================================================================*/
extern int needQScaleChange _ANSI_ARGS_((int oldQScale,  Block blk0, Block blk1, Block blk2, Block blk3));

/*===========================================================================*
 *
 * incNumBlocks()
 *
 *
 * RETURNS:   nothing
 *===========================================================================*/
extern void incNumBlocks _ANSI_ARGS_((int num));


/*===========================================================================*
 *
 * incMacroBlockBits()
 *
 *  Increments the number of Macro Block bits and the total of Frame
 *  bits by the number passed.
 *
 * RETURNS:   nothing
 *===========================================================================*/
extern void incMacroBlockBits _ANSI_ARGS_((int num));


/*===========================================================================*
 *
 * SetRateControl ()
 *
 *      Checks the string parsed from the parameter file.  Verifies
 *  number and sets global values.
 *
 * RETURNS:     nothing
 *===========================================================================*/
extern void SetRateControl _ANSI_ARGS_((char *charPtr));


/*===========================================================================*
 *
 * setBufferSize ()
 *
 *      Checks the string parsed from the parameter file.  Verifies
 *  number and sets global values.
 *
 * RETURNS:     nothing
 *===========================================================================*/
extern void setBufferSize _ANSI_ARGS_((char *charPtr));


/*===========================================================================*
 *
 * getBufferSize ()
 *
 *      returns the buffer size read from the parameter file.  Size is
 *  in bits- not in units of 16k as written to the sequence header.
 *
 * RETURNS:     int (or -1 if invalid)
 *===========================================================================*/
extern int getBufferSize _ANSI_ARGS_((void));


/*===========================================================================*
 *
 * setBitRate ()
 *
 *      Checks the string parsed from the parameter file.  Verifies
 *  number and sets global values.
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   global variables
 *===========================================================================*/
extern void setBitRate _ANSI_ARGS_((char *charPtr));


/*===========================================================================*
 *
 * getBitRate ()
 *
 *      Returns the bit rate read from the parameter file.  This is the
 *  real rate in bits per second, not in 400 bit units as is written to
 *  the sequence header.
 *
 * RETURNS:     int (-1 if Variable mode operation)
 *===========================================================================*/
extern int getBitRate _ANSI_ARGS_((void));


/*===========================================================================*
 *
 * getRateMode ()
 *
 *      Returns the rate mode- interpreted waa either Fixed or Variable
 *
 * RETURNS:     integer
 *===========================================================================*/
extern int getRateMode _ANSI_ARGS_((void));


/*===========================================================================*
 *
 * incQuantOverride()
 *
 *  counter of override of quantization
 *
 * RETURNS:   nothing
 *===========================================================================*/
extern void incQuantOverride  _ANSI_ARGS_((int num));

/*
 * opts.h - set optional parameters
 */

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*  
 *  Revision 1.3  1995/08/15 23:43:43  smoot
 *  *** empty log message ***
 *
 * Revision 1.2  1995/05/02  22:00:51  smoot
 * added TUNEing stuff
 *
 * Revision 1.1  1995/04/14  23:12:53  smoot
 * Initial revision
 *
 */

#if 0
/*NOTE*/
#include "general.h"
#include "ansi.h"
#include "mtypes.h"
/*NOTE*/
#endif

/*
 TUNE b [limit] lower limit on how different a block must be to be DCT coded
 TUNE c [file [color-diff]] Collect statistics on Quantization
 TUNE d [RateScale DistortionScale] Do a DCT in the P search, not just DIFF
 TUNE k [breakpt end [slope]] Squash small lum values
 TUNE l Figure out Laplacian distrib and use them to dequantize and do snr calc
 TUNE n Dont consider DC differenece in DCT searches
 TUNE q Do MSE for distortion measure, not MAD
 TUNE s [Max] | [LumMax ChromMax] Squash small differences in successive frames
 TUNE u disallow skip blocks in B frames
 TUNE w filename [c]  Write I block distortion numbers to file [with bit-rates]
 TUNE z Zaps Intra blocks in P/B frames.

 [ Note k and s make -snr numbers a lie, by playing with input ]
 [ Note d n and q are contradictory (can only use one)         ]
 [ Note c will not work on parallel encodings                  ]
*/

extern boolean tuneingOn;

/* Smash to no-change a motion block DCT with MAD less than: */
/* DETAL b value               */
extern int block_bound;

/* Collect info on quantization */
extern boolean collect_quant;
extern int collect_quant_detailed;
extern FILE   *collect_quant_fp;

/* Nuke dim areas */
extern int kill_dim, kill_dim_break, kill_dim_end;
extern float kill_dim_slope;


/* Stuff to control MV search comparisons */
#define DEFAULT_SEARCH 0
#define LOCAL_DCT  1 /* Do DCT in search (SLOW!!!!) */
#define NO_DC_SEARCH  2  /* Dont consider DC component in motion searches */
#define DO_Mean_Squared_Distortion  3 /* Do Squared distortion, not ABS */

/* Parameters for special searches */
/* LOCAL_DCT */
extern float LocalDCTRateScale, LocalDCTDistortScale;

/* Search Type Variable */
extern int SearchCompareMode;

/* squash small differences */
extern boolean squash_small_differences;
extern int SquashMaxLum, SquashMaxChr;

/* Disallows Intra blocks in P/B code */
extern boolean IntraPBAllowed;

/* Write out distortion numbers */
extern boolean WriteDistortionNumbers;
extern int collect_distortion_detailed;
extern FILE *distortion_fp;
extern FILE *fp_table_rate[31], *fp_table_dist[31];

/* Laplacian Distrib */
extern boolean DoLaplace;
extern double **L1, **L2, **Lambdas;
extern int LaplaceNum, LaplaceCnum;

/* Turn on/off skipping in B frames */
extern boolean BSkipBlocks;

/* Procedures Prototypes */
int	GetIQScale _ANSI_ARGS_((void));
int	GetPQScale _ANSI_ARGS_((void));
int	GetBQScale _ANSI_ARGS_((void));
void	Tune_Init _ANSI_ARGS_((void));
char	*SkipSpacesTabs _ANSI_ARGS_((char *start));
int     CalcRLEHuffLength _ANSI_ARGS_((FlatBlock in));
void    ParseTuneParam _ANSI_ARGS_((char *charPtr));
int     mse _ANSI_ARGS_((Block blk1, Block blk2));




/*===========================================================================*
 * combine.h								     *
 *									     *
 *	procedures to combine frames or GOPs				     *
 *									     *
 *===========================================================================*/

/*
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


void	GOPStoMPEG _ANSI_ARGS_((int numGOPS, char *outputFileName,
				FILE *fpointer));
void	FramesToMPEG _ANSI_ARGS_((int numFrames,
				  char *outputFileName, FILE *fpointer,
				  boolean parallel));

#endif


