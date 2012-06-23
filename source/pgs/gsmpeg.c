/*
 * GSMPEG.C - PGS MPEG_ENCODE routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_mpeg.h"     /* includes system headers */

#if defined(UNIX)

#undef CMAKE
#undef CMAKE_N
#undef CREMAKE
#undef CFREE

#define CMAKE(_t)              ((_t *) malloc(sizeof(_t)))
#define CMAKE_N(_t, _n)        ((_t *) malloc((_n)*sizeof(_t)))
#define CREMAKE(_p, _t, _n)    (_p = (_t *) realloc(_p, (_n)*sizeof(_t)))
#define CFREE(_p)                                                            \
   {free(_p);                                                                \
    _p = NULL;}


/* BFRAME.C */
#define NO_MOTION 0
#define MOTION 1
#define SKIP 2  /* used in useMotion in dct_data */

/* BITIO.C */

/* BLOCK.C */
#undef ABS
#define ABS(x)	((x < 0) ? (-x) : x)

#define TRUNCATE_UINT8(x)	((x < 0) ? 0 : ((x > 255) ? 255 : x))

/* BSEARCH.C */

/* COMBINE.C */
char	currentGOPPath[MAXPATHLEN];
char	currentFramePath[MAXPATHLEN];

/* FRAME.C */
#define  B_FRAME_RUN  16
MpegFrame      *frameMemory[B_FRAME_RUN+2];

/* FRAMETYPE.C */
boolean	    forceEncodeLast = FALSE;

/* FSIZE.C */
int Fsize_x = 0;
int Fsize_y = 0;

/* HUFF.C */
int huff_maxlevel[32] = { 41, 19, 6, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };

u_int32_t huff_table0[41] = { 0x0, 0x6, 0x8, 0xa, 0xc, 0x4c, 0x42, 0x14, 0x3a, 0x30, 0x26, 0x20, 0x34, 0x32, 0x30, 0x2e, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e, 0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x30, 0x2e, 0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20 };
int huff_bits0[41] = { 0, 3, 5, 6, 8, 9, 9, 11, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16 };

u_int32_t huff_table1[19] = { 0x0, 0x6, 0xc, 0x4a, 0x18, 0x36, 0x2c, 0x2a, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x26, 0x24, 0x22, 0x20 };
int huff_bits1[19] = { 0, 4, 7, 9, 11, 13, 14, 14, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17 };

u_int32_t huff_table2[6] = { 0x0, 0xa, 0x8, 0x16, 0x28, 0x28 };
int huff_bits2[6] = { 0, 5, 8, 11, 13, 14 };

u_int32_t huff_table3[5] = { 0x0, 0xe, 0x48, 0x38, 0x26 };
int huff_bits3[5] = { 0, 6, 9, 13, 14 };

u_int32_t huff_table4[4] = { 0x0, 0xc, 0x1e, 0x24 };
int huff_bits4[4] = { 0, 6, 11, 13 };

u_int32_t huff_table5[4] = { 0x0, 0xe, 0x12, 0x24 };
int huff_bits5[4] = { 0, 7, 11, 14 };

u_int32_t huff_table6[4] = { 0x0, 0xa, 0x3c, 0x28 };
int huff_bits6[4] = { 0, 7, 13, 17 };

u_int32_t huff_table7[3] = { 0x0, 0x8, 0x2a };
int huff_bits7[3] = { 0, 7, 13 };

u_int32_t huff_table8[3] = { 0x0, 0xe, 0x22 };
int huff_bits8[3] = { 0, 8, 13 };

u_int32_t huff_table9[3] = { 0x0, 0xa, 0x22 };
int huff_bits9[3] = { 0, 8, 14 };

u_int32_t huff_table10[3] = { 0x0, 0x4e, 0x20 };
int huff_bits10[3] = { 0, 9, 14 };

u_int32_t huff_table11[3] = { 0x0, 0x46, 0x34 };
int huff_bits11[3] = { 0, 9, 17 };

u_int32_t huff_table12[3] = { 0x0, 0x44, 0x32 };
int huff_bits12[3] = { 0, 9, 17 };

u_int32_t huff_table13[3] = { 0x0, 0x40, 0x30 };
int huff_bits13[3] = { 0, 9, 17 };

u_int32_t huff_table14[3] = { 0x0, 0x1c, 0x2e };
int huff_bits14[3] = { 0, 11, 17 };

u_int32_t huff_table15[3] = { 0x0, 0x1a, 0x2c };
int huff_bits15[3] = { 0, 11, 17 };

u_int32_t huff_table16[3] = { 0x0, 0x10, 0x2a };
int huff_bits16[3] = { 0, 11, 17 };

u_int32_t huff_table17[2] = { 0x0, 0x3e };
int huff_bits17[2] = { 0, 13 };

u_int32_t huff_table18[2] = { 0x0, 0x34 };
int huff_bits18[2] = { 0, 13 };

u_int32_t huff_table19[2] = { 0x0, 0x32 };
int huff_bits19[2] = { 0, 13 };

u_int32_t huff_table20[2] = { 0x0, 0x2e };
int huff_bits20[2] = { 0, 13 };

u_int32_t huff_table21[2] = { 0x0, 0x2c };
int huff_bits21[2] = { 0, 13 };

u_int32_t huff_table22[2] = { 0x0, 0x3e };
int huff_bits22[2] = { 0, 14 };

u_int32_t huff_table23[2] = { 0x0, 0x3c };
int huff_bits23[2] = { 0, 14 };

u_int32_t huff_table24[2] = { 0x0, 0x3a };
int huff_bits24[2] = { 0, 14 };

u_int32_t huff_table25[2] = { 0x0, 0x38 };
int huff_bits25[2] = { 0, 14 };

u_int32_t huff_table26[2] = { 0x0, 0x36 };
int huff_bits26[2] = { 0, 14 };

u_int32_t huff_table27[2] = { 0x0, 0x3e };
int huff_bits27[2] = { 0, 17 };

u_int32_t huff_table28[2] = { 0x0, 0x3c };
int huff_bits28[2] = { 0, 17 };

u_int32_t huff_table29[2] = { 0x0, 0x3a };
int huff_bits29[2] = { 0, 17 };

u_int32_t huff_table30[2] = { 0x0, 0x38 };
int huff_bits30[2] = { 0, 17 };

u_int32_t huff_table31[2] = { 0x0, 0x36 };
int huff_bits31[2] = { 0, 17 };

u_int32_t *huff_table[32] = { huff_table0, huff_table1, huff_table2, huff_table3, huff_table4, huff_table5, huff_table6, huff_table7, huff_table8, huff_table9, huff_table10, huff_table11, huff_table12, huff_table13, huff_table14, huff_table15, huff_table16, huff_table17, huff_table18, huff_table19, huff_table20, huff_table21, huff_table22, huff_table23, huff_table24, huff_table25, huff_table26, huff_table27, huff_table28, huff_table29, huff_table30, huff_table31 };
int *huff_bits[32] = { huff_bits0, huff_bits1, huff_bits2, huff_bits3, huff_bits4, huff_bits5, huff_bits6, huff_bits7, huff_bits8, huff_bits9, huff_bits10, huff_bits11, huff_bits12, huff_bits13, huff_bits14, huff_bits15, huff_bits16, huff_bits17, huff_bits18, huff_bits19, huff_bits20, huff_bits21, huff_bits22, huff_bits23, huff_bits24, huff_bits25, huff_bits26, huff_bits27, huff_bits28, huff_bits29, huff_bits30, huff_bits31 };

/* IFRAME.C */
int	qscaleI;
int	slicesPerFrame;
int	blocksPerSlice;
int	fCodeI, fCodeP, fCodeB;
boolean	printSNR = FALSE;
boolean	printMSE = FALSE;
boolean	decodeRefFrames = FALSE;
Block **dct=NULL, **dctr=NULL, **dctb=NULL;
dct_data_type   **dct_data; /* used in p/bframe.c */
int  TIME_RATE;

/* JREVDCT.C */

/* LIBPNMRW.C */

/* MFWDDCT.C */

/* MHEADERS.C */

/* MPEG.C */
#undef VERSION
#define VERSION "1.5b"

#define	FPS_30	0x5   /* from MPEG standard sect. 2.4.3.2 */
#define ASPECT_1    0x1	/* aspect ratio, from MPEG standard sect. 2.4.3.2 */

int	    gopSize = 100;  /* default */
int32	    tc_hrs, tc_min, tc_sec, tc_pict, tc_extra;
int	    totalFramesSent;
int	    yuvWidth, yuvHeight;
int	    realWidth, realHeight;
char	    currentPath[MAXPATHLEN];
char	    statFileName[256];
char	    bitRateFileName[256];
time_t	    timeStart, timeEnd;
FILE	   *statFile;
FILE	   *bitRateFile = NULL;
char	   *framePattern;
int	    framePatternLen;
int	    referenceFrame;
static int  framesRead;
MpegFrame  *pastRefFrame;
MpegFrame  *futureRefFrame;
int	    frameRate = FPS_30;
int	    frameRateRounded = 30;
boolean	    frameRateInteger = TRUE;
int	    aspectRatio = ASPECT_1;

int32 bit_rate, buf_size;

/* NOJPEG.C */

/* NOPARALLEL.C */
int parallelTestFrames = 10;
int parallelTimeChunks = 60;
char *IOhostName;
int ioPortNumber;
int combinePortNumber;
int decodePortNumber;
boolean	niceProcesses = FALSE;
boolean	forceIalign = FALSE;
int	    machineNumber = -1;
boolean	remoteIO = FALSE;
boolean	separateConversion;
time_t	IOtime = 0;

/* OPTS.C */
boolean tuneingOn = FALSE;
int block_bound = 128;
boolean collect_quant = FALSE;
int collect_quant_detailed = 0;
FILE *collect_quant_fp;
int kill_dim = FALSE;
int kill_dim_break, kill_dim_end;
float kill_dim_slope;
int SearchCompareMode = DEFAULT_SEARCH;
boolean squash_small_differences = FALSE;
int SquashMaxLum, SquashMaxChr;
float LocalDCTRateScale = 1.0, LocalDCTDistortScale = 1.0;
boolean IntraPBAllowed = TRUE;
boolean WriteDistortionNumbers = FALSE;
int collect_distortion_detailed = 0;
FILE *distortion_fp;
FILE *fp_table_rate[31], *fp_table_dist[31];
boolean DoLaplace = FALSE;
double **L1, **L2, **Lambdas;
int LaplaceNum, LaplaceCnum;
boolean BSkipBlocks = TRUE;
boolean BC_on=FALSE;
FILE *BC_file;

/* PARAM.C */
#define INPUT_ENTRY_BLOCK_SIZE   128

#define FIRST_OPTION           0
#define OPTION_GOP             0
#define OPTION_PATTERN         1
#define OPTION_PIXEL           2
#define OPTION_PQSCALE         3
#define OPTION_OUTPUT          4
#define OPTION_RANGE           5
#define OPTION_PSEARCH_ALG     6
#define OPTION_IQSCALE         7
#define OPTION_INPUT_DIR       8
#define OPTION_INPUT_CONVERT   9
#define OPTION_INPUT          10
#define OPTION_BQSCALE        11
#define OPTION_BASE_FORMAT    12
#define OPTION_SPF            13
#define OPTION_BSEARCH_ALG    14
#define OPTION_REF_FRAME      15
#define LAST_OPTION           15

/* put any non-required options after LAST_OPTION */
#define OPTION_RESIZE	      16
#define OPTION_IO_CONVERT     17
#define OPTION_SLAVE_CONVERT  18
#define OPTION_IQTABLE	      19
#define OPTION_NIQTABLE	      20
#define OPTION_FRAME_RATE     21
#define OPTION_ASPECT_RATIO   22
#define OPTION_YUV_SIZE	      23
#define OPTION_SPECIFICS      24
#define OPTION_DEFS_SPECIFICS 25
#define OPTION_BUFFER_SIZE    26
#define OPTION_BIT_RATE       27
#define OPTION_USER_DATA      28
#define OPTION_YUV_FORMAT     29
#define OPTION_GAMMA          30
#define OPTION_PARALLEL       31

#define NUM_OPTIONS           31

char	outputFileName[256];
int	outputWidth, outputHeight;
int numInputFiles = 0;
char inputConversion[1024];
char ioConversion[1024];
char slaveConversion[1024];
char yuvConversion[256];
char specificsFile[256],specificsDefines[1024]="";
boolean GammaCorrection=FALSE;
float   GammaValue;
char userDataFileName[256]={0};
boolean specificsOn = FALSE;
boolean optionSeen[NUM_OPTIONS+1];
int numMachines;
char	machineName[MAX_MACHINES][256];
char	userName[MAX_MACHINES][256];
char	executable[MAX_MACHINES][1024];
char	remoteParamFile[MAX_MACHINES][1024];
boolean	remote[MAX_MACHINES];
boolean stdinUsed = FALSE;
int mult_seq_headers = 0;  /* 0 for none, N for header/N GOPs */

void Mpost_UnQuantZigBlockLaplace 
  _ANSI_ARGS_((FlatBlock in, Block out, int qscale, boolean iblock));

/* PFRAME.C */

/* POSTDCT.C */
int ZAG[] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

int32 qtable[] = {
    8, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83
};

int32 niqtable[] = {
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16
};

int32	*customQtable = NULL;
int32	*customNIQtable = NULL;

/* PSEARCH.C */
int **pmvHistogram = NULL;	/* histogram of P-frame motion vectors */
int **bbmvHistogram = NULL;	/* histogram of B-frame motion vectors */
int **bfmvHistogram = NULL;	/* histogram of B-frame motion vectors */
int pixelFullSearch;
int searchRangeP,searchRangeB;
int psearchAlg;

/* RATE.C */

/* READFRAME.C */
boolean resizeFrame;
char *CurrFile;

/* RGBTOYCC.C */

/* SPECIFICS.C */
#define CPP_LOC "/lib/cpp"

FrameSpecList *fsl;

/* SUBSAMPLE.C */


/*--------------------------------------------------------------------------*/

/*                           ALL THE MERGED FILES                           */

/*--------------------------------------------------------------------------*/

/*===========================================================================*
 * bframe.c								     *
 *									     *
 *	Procedures concerned with the B-frame encoding			     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	GenBFrame							     *
 *	ResetBFrameStats						     *
 *	ShowBFrameSummary						     *
 *	EstimateSecondsPerBFrame					     *
 *	ComputeBMotionLumBlock						     *
 *	SetBQScale							     *
 *	GetBQScale							     *
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
 *  Revision 1.21  1995/10/27 00:04:35  smoot
 *  error in checking b skip blocks cr vs cb
 *
 *  Revision 1.20  1995/08/14 22:28:11  smoot
 *  renamed index to idx
 *  added option to not skip in B frames
 *
 *  Revision 1.19  1995/08/07 21:52:11  smoot
 *  added Color to skip routine
 *  fixed full/half bug in intial loop
 *  added comments
 *  removed buggy "extra skips" code
 *
 *  Revision 1.18  1995/06/21 22:22:24  smoot
 *  generalized time checking, fixed bug in specifics filesm
 *  and added TUNEing stuff
 *
 * Revision 1.17  1995/04/14  23:08:02  smoot
 * reorganized to ease rate control experimentation
 *
 * Revision 1.16  1995/02/24  23:49:10  smoot
 * added Spec version 2
 *
 * Revision 1.15  1995/01/30  19:45:45  smoot
 * Fixed a cr/cb screwup
 *
 * Revision 1.14  1995/01/23  02:46:43  darryl
 * initialized variable
 *
 * Revision 1.13  1995/01/19  23:07:12  eyhung
 * Changed copyrights
 *
 * Revision 1.12  1995/01/16  07:44:11  eyhung
 * Added realQuiet
 *
 * Revision 1.11  1994/12/07  00:40:36  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.10  1994/11/24  00:35:47  smoot
 * fixed bug (divide by 0) in B fram statsitics
 *
 * Revision 1.9  1994/11/14  22:26:48  smoot
 * Merged specifics and rate control.
 *
 * Revision 1.8  1994/11/01  05:01:16  darryl
 *  with rate control changes added
 *
 * Revision 2.0  1994/10/24  02:38:51  darryl
 * will be adding the experiment code
 *
 * Revision 1.1  1994/09/27  00:16:04  darryl
 * Initial revision
 *
 * Revision 1.7  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.6  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.5  1993/07/30  19:24:04  keving
 * nothing
 *
 * Revision 1.4  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.3  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.2  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.1  1993/02/19  19:14:28  keving
 * nothing
 *
 */


/*==================*
 * STATIC VARIABLES *
 *==================*/

static int numBIBlocks = 0;
static int numBBBlocks = 0;
static int numBSkipped = 0;
static int numBIBits = 0;
static int numBBBits = 0;
static int numFrames = 0;
static int numFrameBits = 0;
static int32 totalTime = 0;
static int qscaleB;
static float    totalSNR = 0.0;
static float	totalPSNR = 0.0;

static int numBFOBlocks = 0;    /* forward only */
static int numBBABlocks = 0;    /* backward only */
static int numBINBlocks = 0;    /* interpolate */
static int numBFOBits = 0;
static int numBBABits = 0;
static int numBINBits = 0;


/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static boolean	MotionSufficient _ANSI_ARGS_((MpegFrame *curr, LumBlock currBlock, MpegFrame *prev, MpegFrame *next,
			 int by, int bx, int mode, int fmy, int fmx,
			 int bmy, int bmx));
static void	ComputeBMotionBlock _ANSI_ARGS_((MpegFrame *prev, MpegFrame *next,
			       int by, int bx, int mode, int fmy, int fmx,
			       int bmy, int bmx, Block motionBlock, int type));
static void	ComputeBDiffDCTs _ANSI_ARGS_((MpegFrame *current, MpegFrame *prev, MpegFrame *next,
			 int by, int bx, int mode, int fmy, int fmx, 
			 int bmy, int bmx, int *pattern));
static boolean	DoBIntraCode _ANSI_ARGS_((MpegFrame *current, MpegFrame *prev, MpegFrame *next,
		     int by, int bx, int mode, int fmy, int fmx, int bmy,
		     int bmx));

static int ComputeBlockColorDiff _ANSI_ARGS_((Block current, Block motionBlock));


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * GenBFrame
 *
 *	generate a B-frame from previous and next frames, adding the result
 *	to the given bit bucket
 *
 * RETURNS:	frame appended to bb
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

void GenBFrame(BitBucket *bb, MpegFrame *curr, MpegFrame *prev,  MpegFrame *next)
   {int x, y;
    int	fMotionX = 0, fMotionY = 0;
    int bMotionX = 0, bMotionY = 0;
    int	oldFMotionX = 0, oldFMotionY = 0;
    int oldBMotionX = 0, oldBMotionY = 0;
    int	oldMode = MOTION_FORWARD;
    int	mode = MOTION_FORWARD;
    int	offsetX, offsetY;
    int	tempX, tempY;
    int	fMotionXrem = 0, fMotionXquot = 0;
    int	fMotionYrem = 0, fMotionYquot = 0;
    int	bMotionXrem = 0, bMotionXquot = 0;
    int	bMotionYrem = 0, bMotionYquot = 0;
    int	pattern;
    int	numIBlocks = 0, numBBlocks = 0;
    int numSkipped = 0, totalBits;
    int	numIBits = 0,   numBBits = 0;
    int	    totalFrameBits;
    int lastX, lastY;
    int lastBlockX, lastBlockY;
    int32    startTime, endTime;
    int32 y_dc_pred, cr_dc_pred, cb_dc_pred;
    register int ix, iy;
    int         fy, fx;
    int	mbAddrInc = 1;
    int	mbAddress;
    int	    slicePos;
    int	    idx;
    int     QScale;
    int     bitstreamMode, newQScale;
    int     rc_blockStart=0;
    int overflowValue = 0;
    boolean	lastIntra = TRUE;
    boolean    motionForward, motionBackward;
    boolean	make_skip_block;
    boolean overflowChange=FALSE;
    float   snr[3], psnr[3];
    FlatBlock fba[6], fb[6];
    BlockMV *info;
    Block     dec[6];
    LumBlock currentBlock;
    extern int **bfmvHistogram;
    extern int **bbmvHistogram;

    if (prev == NULL)
       return;

    if (collect_quant) {io_printf(collect_quant_fp, "# B\n");}
    if (dct == NULL) AllocDctBlocks();
    numFrames++;
    totalFrameBits = bb->cumulativeBits;
    startTime = time_elapsed();

    /*   Rate Control */
    bitstreamMode = getRateMode();
    if (bitstreamMode == FIXED_RATE) {
      targetRateControl(curr);
    }
 
    QScale = GetBQScale();
    Mhead_GenPictureHeader(bb, B_FRAME, curr->id, fCodeB);
    /* Check for Qscale change */
    if (specificsOn) {
      newQScale = SpecLookup(curr->id, 0, 0 /* junk */, &info, QScale);
      if (newQScale != -1) {
	QScale = newQScale;
      }
      /* check for slice */
      newQScale = SpecLookup(curr->id, 1, 1, &info, QScale);
      if (newQScale != -1) {
	QScale = newQScale;
      }
    }

    Mhead_GenSliceHeader(bb, 1, QScale, NULL, 0);

    Frame_AllocBlocks(curr);
    BlockifyFrame(curr);

    if ( printSNR ) {
	Frame_AllocDecoded(curr, FALSE);
    }

    /* for I-blocks */
    y_dc_pred = cr_dc_pred = cb_dc_pred = 128;

    totalBits = bb->cumulativeBits;

    if ( ! pixelFullSearch ) {
	if ( ! prev->halfComputed && (prev != NULL)) {
	    ComputeHalfPixelData(prev);
	}

	if ( ! next->halfComputed ) {
	    ComputeHalfPixelData(next);
	}
    }

    lastBlockX = Fsize_x>>3;
    lastBlockY = Fsize_y>>3;
    lastX = lastBlockX-2;
    lastY = lastBlockY-2;
    mbAddress = 0;

    /* find motion vectors and do dcts */
    /* In this first loop, all MVs are in half-pixel scope, (if FULL is set
       then they will be multiples of 2).  This is not true in the second loop. */
    for (y = 0;  y < lastBlockY;  y += 2) {
      for (x = 0;  x < lastBlockX;  x += 2) {
	slicePos = (mbAddress % blocksPerSlice);

	/* compute currentBlock */
	BLOCK_TO_FRAME_COORD(y, x, fy, fx);
	for ( iy = 0; iy < 16; iy++ ) {
	  for ( ix = 0; ix < 16; ix++ ) {
	    currentBlock[iy][ix] = (int16)curr->orig_y[fy+iy][fx+ix];
	  }
	}
	    
	if (slicePos == 0) {
	  oldFMotionX = 0;	oldFMotionY = 0;
	  oldBMotionX = 0;	oldBMotionY = 0;
	  oldMode = MOTION_FORWARD;
	  lastIntra = TRUE;
	}

	/* STEP 1:  Select Forward, Backward, or Interpolated motion vectors */
	/* see if old motion is good enough */
	/* but force last block to be non-skipped */
	/* can only skip if:
	 *     1)  not the last block in frame
	 *     2)  not the last block in slice
	 *     3)  not the first block in slice
	 *     4)  previous block was not intra-coded
	 */
	if ( ((y < lastY) || (x < lastX)) &&
	    (slicePos+1 != blocksPerSlice) &&
	    (slicePos != 0) &&
	    (! lastIntra) &&
	    (BSkipBlocks) ) {
	  make_skip_block = MotionSufficient(curr, currentBlock, prev, next, y, x, oldMode,
					     oldFMotionY, oldFMotionX,
					     oldBMotionY, oldBMotionX);
	} else {
	  make_skip_block = FALSE;
	}

	if ( make_skip_block ) {
	skip_it:
	  /* skipped macro block */
	  dct_data[y][x].useMotion = SKIP;
	} else {
	  if (specificsOn) {
	    (void) SpecLookup(curr->id, 2, mbAddress, &info, QScale);
	    if (info == (BlockMV*)NULL) goto gosearch;
	    else {
	      switch (info->typ) {
	      case TYP_SKIP:
		goto skip_it;
	      case TYP_FORW:
		fMotionX = info->fx;
		fMotionY = info->fy;
		mode = MOTION_FORWARD;
		break;
	      case TYP_BACK:
		bMotionX = info->bx;
		bMotionY = info->by;
		mode = MOTION_BACKWARD;
		break;
	      case TYP_BOTH:
		fMotionX = info->fx;
		fMotionY = info->fy;
		bMotionX = info->bx;
		bMotionY = info->by;
		mode = MOTION_INTERPOLATE;
		break;
	      default:
		io_printf(stderr,"Unreachable code in GenBFrame!\n");
		goto gosearch;
	      }
	      goto skipsearch;
	    }}
	gosearch:		/* do bsearch */
	  mode = BMotionSearch(currentBlock, prev, next, y, x, &fMotionY,
			       &fMotionX, &bMotionY, &bMotionX, mode);
	skipsearch:	      
	      
	  /* STEP 2:  INTRA OR NON-INTRA CODING */
	  if ( IntraPBAllowed && DoBIntraCode(curr, prev, next, y, x, mode, fMotionY,
			    fMotionX, bMotionY, bMotionX) ) {
	    /* output I-block inside a B-frame */
	    numIBlocks++;
	    oldFMotionX = 0;	oldFMotionY = 0;
	    oldBMotionX = 0;	oldBMotionY = 0;
	    lastIntra = TRUE;
	    dct_data[y][x].useMotion = NO_MOTION;
	    oldMode = MOTION_FORWARD;
	    /* calculate forward dct's */
	    if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "li\n");
	    mp_fwd_dct_block2(curr->y_blocks[y][x], dct[y][x]);
	    mp_fwd_dct_block2(curr->y_blocks[y][x+1], dct[y][x+1]);
	    mp_fwd_dct_block2(curr->y_blocks[y+1][x], dct[y+1][x]);
	    mp_fwd_dct_block2(curr->y_blocks[y+1][x+1], dct[y+1][x+1]);
	    if (collect_quant && (collect_quant_detailed & 1)) {io_printf(collect_quant_fp, "ci\n");}
	    mp_fwd_dct_block2(curr->cb_blocks[y>>1][x>>1], dctb[y>>1][x>>1]);
	    mp_fwd_dct_block2(curr->cr_blocks[y>>1][x>>1], dctr[y>>1][x>>1]);

	  } else { /* dct P/Bi/B block */

	    pattern = 63;
	    lastIntra = FALSE;
	    numBBlocks++;
	    dct_data[y][x].mode = mode;
	    oldMode = mode;
	    dct_data[y][x].fmotionX = fMotionX;
	    dct_data[y][x].fmotionY = fMotionY;
	    dct_data[y][x].bmotionX = bMotionX;
	    dct_data[y][x].bmotionY = bMotionY;
	    switch (mode) {
	    case MOTION_FORWARD:
	      numBFOBlocks++;
	      oldFMotionX = fMotionX;		oldFMotionY = fMotionY;
	      break;
	    case MOTION_BACKWARD:
	      numBBABlocks++;
	      oldBMotionX = bMotionX;		oldBMotionY = bMotionY;
	      break;
	    case MOTION_INTERPOLATE:
	      numBINBlocks++;
	      oldFMotionX = fMotionX;		oldFMotionY = fMotionY;
	      oldBMotionX = bMotionX;		oldBMotionY = bMotionY;
	      break;
	    default:
	      io_printf(stderr, "PROGRAMMER ERROR:  Illegal mode: %d\n", mode);
	      exit(1);
	    }
	    
	    ComputeBDiffDCTs(curr, prev, next, y, x, mode, fMotionY,
			     fMotionX, bMotionY, bMotionX, &pattern);
	    
	    dct_data[y][x].pattern = pattern;
	    dct_data[y][x].useMotion = MOTION;
	    if ( computeMVHist ) {
	      assert(fMotionX+searchRangeB+1 >= 0);
	      assert(fMotionY+searchRangeB+1 >= 0);
	      assert(fMotionX+searchRangeB+1 <= 2*searchRangeB+2);
	      assert(fMotionY+searchRangeB+1 <= 2*searchRangeB+2);
	      assert(bMotionX+searchRangeB+1 >= 0);
	      assert(bMotionY+searchRangeB+1 >= 0);
	      assert(bMotionX+searchRangeB+1 <= 2*searchRangeB+2);
	      assert(bMotionY+searchRangeB+1 <= 2*searchRangeB+2);

	      bfmvHistogram[fMotionX+searchRangeB+1][fMotionY+searchRangeB+1]++;
	      bbmvHistogram[bMotionX+searchRangeB+1][bMotionY+searchRangeB+1]++;
	    }
	  } /* motion-block */
	} /* not skipped */
	mbAddress++;
      }}

    /* reset everything */
    oldFMotionX = 0;	oldFMotionY = 0;
    oldBMotionX = 0;	oldBMotionY = 0;
    oldMode = MOTION_FORWARD;
    lastIntra = TRUE;
    y_dc_pred = cr_dc_pred = cb_dc_pred = 128;
    mbAddress = 0;

    /* Now generate the frame */
    for (y = 0; y < lastBlockY; y += 2) {
      for (x = 0; x < lastBlockX; x += 2) {
	slicePos = (mbAddress % blocksPerSlice);

	if ( (slicePos == 0) && (mbAddress != 0) ) {
	  if (specificsOn) {
	    /* Make sure no slice Qscale change */
	    newQScale = SpecLookup(curr->id,1,mbAddress/blocksPerSlice, &info, QScale);
	    if (newQScale != -1) QScale = newQScale;
	  }
	  Mhead_GenSliceEnder(bb);
	  Mhead_GenSliceHeader(bb, 1+(y>>1), QScale, NULL, 0);

	  /* reset everything */
	  oldFMotionX = 0;	oldFMotionY = 0;
	  oldBMotionX = 0;	oldBMotionY = 0;
	  oldMode = MOTION_FORWARD;
	  lastIntra = TRUE;
	  y_dc_pred = cr_dc_pred = cb_dc_pred = 128;

	  mbAddrInc = 1+(x>>1);
	}

	/*  Determine if new Qscale needed for Rate Control purposes */
	if (bitstreamMode == FIXED_RATE) {
	  rc_blockStart =  bb->cumulativeBits;
	  newQScale = needQScaleChange(QScale,
				       curr->y_blocks[y][x],
				       curr->y_blocks[y][x+1],
				       curr->y_blocks[y+1][x],
				       curr->y_blocks[y+1][x+1]);
	  if (newQScale > 0) {
	    QScale = newQScale;
	  }
	}
 
	if (specificsOn) {
	  newQScale = SpecLookup(curr->id, 2, mbAddress, &info, QScale);
	  if (newQScale != -1) {
	    QScale = newQScale;
	  }}

	if (dct_data[y][x].useMotion == NO_MOTION) {

	  GEN_I_BLOCK(B_FRAME, curr, bb, mbAddrInc, QScale);
	  mbAddrInc = 1;
	  numIBits += (bb->cumulativeBits-totalBits);
	  totalBits = bb->cumulativeBits;
	      
	  /* reset because intra-coded */
	  oldFMotionX = 0;		oldFMotionY = 0;
	  oldBMotionX = 0;		oldBMotionY = 0;
	  oldMode = MOTION_FORWARD;
	  lastIntra = TRUE;
	      
	  if ( printSNR ) {
	    /* need to decode block we just encoded */
	    /* and reverse the DCT transform */
	    for ( idx = 0; idx < 6; idx++ ) {
	      Mpost_UnQuantZigBlock(fb[idx], dec[idx], QScale, TRUE);
	      mpeg_jrevdct((int16 *)dec[idx]);
	    }

	    /* now, unblockify */
	    BlockToData(curr->decoded_y, dec[0], y, x);
	    BlockToData(curr->decoded_y, dec[1], y, x+1);
	    BlockToData(curr->decoded_y, dec[2], y+1, x);
	    BlockToData(curr->decoded_y, dec[3], y+1, x+1);
	    BlockToData(curr->decoded_cb, dec[4], y>>1, x>>1);
	    BlockToData(curr->decoded_cr, dec[5], y>>1, x>>1);
	  }
	} else if (dct_data[y][x].useMotion == SKIP) {
	/*skip_block: - depends on BUGGY_CODE macro and not used*/
	  numSkipped++;
	  mbAddrInc++;
	      
	  /* decode skipped block */
	  if ( printSNR ) {
	    int	fmy, fmx, bmy, bmx;
		
	    for ( idx = 0; idx < 6; idx++ ) {
	      memset((char *)dec[idx], 0, sizeof(Block)); 
	    }
	    
	    if ( pixelFullSearch ) {
	      fmy = 2*oldFMotionY;
	      fmx = 2*oldFMotionX;
	      bmy = 2*oldBMotionY;
	      bmx = 2*oldBMotionX;
	    } else {
	      fmy = oldFMotionY;
	      fmx = oldFMotionX;
	      bmy = oldBMotionY;
	      bmx = oldBMotionX;
	    }
	    
	    /* now add the motion block */
	    AddBMotionBlock(dec[0], prev->decoded_y,
			    next->decoded_y, y, x, mode,
			    fmy, fmx, bmy, bmx);
	    AddBMotionBlock(dec[1], prev->decoded_y,
			    next->decoded_y, y, x+1, mode,
			    fmy, fmx, bmy, bmx);
	    AddBMotionBlock(dec[2], prev->decoded_y,
			    next->decoded_y, y+1, x, mode,
			    fmy, fmx, bmy, bmx);
	    AddBMotionBlock(dec[3], prev->decoded_y,
			    next->decoded_y, y+1, x+1, mode,
			    fmy, fmx, bmy, bmx);
	    AddBMotionBlock(dec[4], prev->decoded_cb,
			    next->decoded_cb, y>>1, x>>1, mode,
			    fmy/2, fmx/2,
			    bmy/2, bmx/2);
	    AddBMotionBlock(dec[5], prev->decoded_cr,
			    next->decoded_cr, y>>1, x>>1, mode,
			    fmy/2, fmx/2,
			    bmy/2, bmx/2);
	    
	    /* now, unblockify */
	    BlockToData(curr->decoded_y, dec[0], y, x);
	    BlockToData(curr->decoded_y, dec[1], y, x+1);
	    BlockToData(curr->decoded_y, dec[2], y+1, x);
	    BlockToData(curr->decoded_y, dec[3], y+1, x+1);
	    BlockToData(curr->decoded_cb, dec[4], y>>1, x>>1);
	    BlockToData(curr->decoded_cr, dec[5], y>>1, x>>1);
	  }
	} else   /* B block */ {
	  int fCode = fCodeB;	

	  pattern = dct_data[y][x].pattern;
	  fMotionX = dct_data[y][x].fmotionX;
	  fMotionY = dct_data[y][x].fmotionY;
	  bMotionX = dct_data[y][x].bmotionX;
	  bMotionY = dct_data[y][x].bmotionY;

	  if ( pixelFullSearch ) {
	    fMotionX /= 2;	    fMotionY /= 2;
	    bMotionX /= 2;	    bMotionY /= 2;
	  }
	      
	  /* create flat blocks and update pattern if necessary */
	calc_blocks:
	/* Note DoQuant references QScale, overflowChange, overflowValue,
           pattern, and the calc_blocks label                 */
	  DoQuant(0x20, dct[y][x], fba[0]);
	  DoQuant(0x10, dct[y][x+1], fba[1]);
	  DoQuant(0x08, dct[y+1][x], fba[2]);
	  DoQuant(0x04, dct[y+1][x+1], fba[3]);
	  DoQuant(0x02, dctb[y>>1][x>>1], fba[4]);
	  DoQuant(0x01, dctr[y>>1][x>>1], fba[5]);

	  motionForward  = (dct_data[y][x].mode != MOTION_BACKWARD);
	  motionBackward = (dct_data[y][x].mode != MOTION_FORWARD);

#ifdef BUGGY_CODE
	  /*
	  send us mail if you can tell me why this code
          doesnt work.  Generates some bad vectors.
          I suspect 'cuz oldMode/motions aren't being set right,
          but am unsure.
	  */
	  /* Check to see if we should have skipped */
	  if ((pattern == 0) &&
	      ((y < lastY) || (x < lastX)) &&
	      (slicePos+1 != blocksPerSlice) &&
	      (slicePos != 0) &&
	      (!lastIntra) &&
	      ( (!motionForward) || 
	       (motionForward && 
		fMotionX == oldFMotionX && fMotionY == oldFMotionY)) &&
	      ( (!motionBackward) || 
	       (motionBackward && 
		bMotionX == oldBMotionX && bMotionY == oldBMotionY))
	      ) {
	    /* Now *thats* an if statement! */
	    goto skip_block; 
	  }
#endif
	  /* Encode Vectors */
	  if ( motionForward ) {
	    /* transform the fMotion vector into the appropriate values */
	    offsetX = fMotionX - oldFMotionX;
	    offsetY = fMotionY - oldFMotionY;

	    ENCODE_MOTION_VECTOR(offsetX, offsetY, fMotionXquot,
				 fMotionYquot, fMotionXrem, fMotionYrem,
				 FORW_F);
	    oldFMotionX = fMotionX;		oldFMotionY = fMotionY;
	  }
	      
	  if ( motionBackward ) {
	    /* transform the bMotion vector into the appropriate values */
	    offsetX = bMotionX - oldBMotionX;
	    offsetY = bMotionY - oldBMotionY;
	    ENCODE_MOTION_VECTOR(offsetX, offsetY, bMotionXquot,
				 bMotionYquot, bMotionXrem, bMotionYrem,
				 BACK_F);
	    oldBMotionX = bMotionX;		oldBMotionY = bMotionY;
	  }
	      
	  oldMode = dct_data[y][x].mode;
	      
	  if ( printSNR ) { /* Need to decode */
	    if ( pixelFullSearch ) {
	      fMotionX *= 2;	fMotionY *= 2;
	      bMotionX *= 2;	bMotionY *= 2;
	    }
	    for ( idx = 0; idx < 6; idx++ ) {
	      if ( pattern & (1 << (5-idx)) ) {
		Mpost_UnQuantZigBlock(fba[idx], dec[idx], QScale, FALSE);
		mpeg_jrevdct((int16 *)dec[idx]);
	      } else {
		memset((char *)dec[idx], 0, sizeof(Block));
	      }
	    }

	    /* now add the motion block */
	    AddBMotionBlock(dec[0], prev->decoded_y,
			    next->decoded_y, y, x, mode,
			    fMotionY, fMotionX, bMotionY, bMotionX);
	    AddBMotionBlock(dec[1], prev->decoded_y,
			    next->decoded_y, y, x+1, mode,
			    fMotionY, fMotionX, bMotionY, bMotionX);
	    AddBMotionBlock(dec[2], prev->decoded_y,
			    next->decoded_y, y+1, x, mode,
			    fMotionY, fMotionX, bMotionY, bMotionX);
	    AddBMotionBlock(dec[3], prev->decoded_y,
			    next->decoded_y, y+1, x+1, mode,
			    fMotionY, fMotionX, bMotionY, bMotionX);
	    AddBMotionBlock(dec[4], prev->decoded_cb,
			    next->decoded_cb, y>>1, x>>1, mode,
			    fMotionY/2, fMotionX/2,
			    bMotionY/2, bMotionX/2);
	    AddBMotionBlock(dec[5], prev->decoded_cr,
			    next->decoded_cr, y>>1, x>>1, mode,
			    fMotionY/2, fMotionX/2,
			    bMotionY/2, bMotionX/2);

	    /* now, unblockify */
	    BlockToData(curr->decoded_y,  dec[0], y,    x);
	    BlockToData(curr->decoded_y,  dec[1], y,    x+1);
	    BlockToData(curr->decoded_y,  dec[2], y+1,  x);
	    BlockToData(curr->decoded_y,  dec[3], y+1,  x+1);
	    BlockToData(curr->decoded_cb, dec[4], y>>1, x>>1);
	    BlockToData(curr->decoded_cr, dec[5], y>>1, x>>1);
	  }

	  /* reset because non-intra-coded */
	  y_dc_pred = cr_dc_pred = cb_dc_pred = 128;
	  lastIntra = FALSE;
	  mode = dct_data[y][x].mode;

	  /*      DBG_PRINT(("MB Header(%d,%d)\n", x, y));  */
	  Mhead_GenMBHeader(bb, 3 /* pict_code_type */, mbAddrInc /* addr_incr */,
	    QScale /* q_scale */,
	    fCodeB /* forw_f_code */, fCodeB /* back_f_code */,
	    fMotionXrem /* horiz_forw_r */, fMotionYrem /* vert_forw_r */,
	    bMotionXrem /* horiz_back_r */, bMotionYrem /* vert_back_r */,
	    motionForward /* motion_forw */, fMotionXquot /* m_horiz_forw */,
	    fMotionYquot /* m_vert_forw */, motionBackward /* motion_back */,
	    bMotionXquot /* m_horiz_back */, bMotionYquot /* m_vert_back */,
	    pattern /* mb_pattern */, FALSE /* mb_intra */);
	  mbAddrInc = 1;
	      
	  /* now output the difference */
	  for ( tempX = 0; tempX < 6; tempX++ ) {
	    if ( GET_ITH_BIT(pattern, 5-tempX) ) {
	      Mpost_RLEHuffPBlock(fba[tempX], bb);
	    }
	  }
	      
	  
	  switch (mode) {
	  case MOTION_FORWARD:
	    numBFOBits += (bb->cumulativeBits-totalBits);
	    break;
	  case MOTION_BACKWARD:
	    numBBABits += (bb->cumulativeBits-totalBits);
	    break;
	  case MOTION_INTERPOLATE:
	    numBINBits += (bb->cumulativeBits-totalBits);
	    break;
	  default:
	    io_printf(stderr, "PROGRAMMER ERROR:  Illegal mode: %d\n",
		    mode);
	    exit(1);
	  }
	  
	  numBBits += (bb->cumulativeBits-totalBits);
	  totalBits = bb->cumulativeBits;
	
	  if (overflowChange) {
	    /* undo an overflow-caused Qscale change */
	    overflowChange = FALSE;
	    QScale -= overflowValue;
	    overflowValue = 0;
	  }
	} /* if I-block, skip, or B */

	mbAddress++;
	/*   Rate Control  */
	if (bitstreamMode == FIXED_RATE) {
	  incMacroBlockBits( bb->cumulativeBits - rc_blockStart);
	  rc_blockStart = bb->cumulativeBits;
	  MB_RateOut(TYPE_BFRAME);
	}
	
      }
    }

    if ( printSNR ) {
      BlockComputeSNR(curr,snr,psnr);
      totalSNR += snr[0];
      totalPSNR += psnr[0];
    }
    
    Mhead_GenSliceEnder(bb);
    /*   Rate Control  */
    if (bitstreamMode == FIXED_RATE) {
      updateRateControl(TYPE_BFRAME);
    }
    
    endTime = time_elapsed();
    totalTime += (endTime-startTime);
    
    if ( ( ! childProcess) && showBitRatePerFrame ) {
      /* ASSUMES 30 FRAMES PER SECOND */
      io_printf(bitRateFile, "%5d\t%8ld\n", curr->id,
	      30*(bb->cumulativeBits-totalFrameBits));
    }
    
    if ( (! childProcess) && frameSummary && !realQuiet) {
      io_printf(stdout, "FRAME %d (B):  I BLOCKS:  %d;  B BLOCKS:  %d   SKIPPED:  %d (%ld seconds)\n",
	      curr->id, numIBlocks, numBBlocks, numSkipped, (long)((endTime-startTime)/TIME_RATE));
      if ( printSNR )
	io_printf(stdout, "FRAME %d:  SNR:  %.1f\t%.1f\t%.1f\tPSNR:  %.1f\t%.1f\t%.1f\n",
		curr->id, snr[0], snr[1], snr[2],
		psnr[0], psnr[1], psnr[2]);
    }
    
    numFrameBits += (bb->cumulativeBits-totalFrameBits);
    numBIBlocks += numIBlocks;
    numBBBlocks += numBBlocks;
    numBSkipped += numSkipped;
    numBIBits += numIBits;
    numBBBits += numBBits;
  }


/*===========================================================================*
 *
 * SetBQScale
 *
 *	set the B-frame Q-scale
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    qscaleB
 *
 *===========================================================================*/
void
SetBQScale(int qB)
{
    qscaleB = qB;
}


/*===========================================================================*
 *
 * GetBQScale
 *
 *	get the B-frame Q-scale
 *
 * RETURNS:	the Q-scale
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
GetBQScale(void)
{
    return(qscaleB);
}


/*===========================================================================*
 *
 * ResetBFrameStats
 *
 *	reset the B-frame stats
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
ResetBFrameStats(void)
{
    numBIBlocks = 0;
    numBBBlocks = 0;
    numBSkipped = 0;
    numBIBits = 0;
    numBBBits = 0;
    numFrames = 0;
    numFrameBits = 0;
    totalTime = 0;
}


/*===========================================================================*
 *
 * ShowBFrameSummary
 *
 *	print out statistics on all B-frames
 *
 * RETURNS:	time taken for B-frames (in seconds)
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
float
ShowBFrameSummary(int inputFrameBits, int32 totalBits, FILE *fpointer)
{
    if ( numFrames == 0 ) {
	return(0.0);
    }

    io_printf(fpointer, "-------------------------\n");
    io_printf(fpointer, "*****B FRAME SUMMARY*****\n");
    io_printf(fpointer, "-------------------------\n");

    if ( numBIBlocks != 0 ) {
	io_printf(fpointer, "  I Blocks:  %5d     (%6d bits)     (%5d bpb)\n",
		numBIBlocks, numBIBits, numBIBits/numBIBlocks);
    } else {
	io_printf(fpointer, "  I Blocks:  %5d\n", 0);
    }

    if ( numBBBlocks != 0 ) {
	io_printf(fpointer, "  B Blocks:  %5d     (%6d bits)     (%5d bpb)\n",
		numBBBlocks, numBBBits, numBBBits/numBBBlocks);
	io_printf(fpointer, "  B types:   %5d     (%4d bpb) forw  %5d (%4d bpb) back   %5d (%4d bpb) bi\n",
		numBFOBlocks, (numBFOBlocks==0)?0:numBFOBits/numBFOBlocks,
		numBBABlocks, (numBBABlocks==0)?0:numBBABits/numBBABlocks,
		numBINBlocks, (numBINBlocks==0)?0:numBINBits/numBINBlocks);
    } else {
	io_printf(fpointer, "  B Blocks:  %5d\n", 0);
    }

    io_printf(fpointer, "  Skipped:   %5d\n", numBSkipped);

    io_printf(fpointer, "  Frames:    %5d     (%6d bits)     (%5d bpf)     (%2.1f%% of total)\n",
	    numFrames, numFrameBits, numFrameBits/numFrames,
	    100.0*(float)numFrameBits/(float)totalBits);	    
    io_printf(fpointer, "  Compression:  %3d:1     (%9.4f bpp)\n",
	    numFrames*inputFrameBits/numFrameBits,
	    24.0*(float)numFrameBits/(float)(numFrames*inputFrameBits));
    if ( printSNR )
	io_printf(fpointer, "  Avg Y SNR/PSNR:  %.1f     %.1f\n",
		totalSNR/(float)numFrames, totalPSNR/(float)numFrames);
    if ( totalTime == 0 ) {
	io_printf(fpointer, "  Seconds:  NONE\n");
    } else {
	io_printf(fpointer, "  Seconds:  %9ld     (%9.4f fps)  (%9ld pps)  (%9ld mps)\n",
		(long)(totalTime/TIME_RATE),
		(float)((float)(TIME_RATE*numFrames)/(float)totalTime),
		(long)((float)TIME_RATE*(float)numFrames*(float)inputFrameBits/(24.0*(float)totalTime)),
		(long)((float)TIME_RATE*(float)numFrames*(float)inputFrameBits/(256.0*24.0*(float)totalTime)));
    }

    return((float)totalTime/TIME_RATE);
}


/*===========================================================================*
 *
 * ComputeBMotionLumBlock
 *
 *	compute the luminance block resulting from motion compensation
 *
 * RETURNS:	motionBlock modified
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITION:	the motion vectors must be valid!
 *
 *===========================================================================*/
void
ComputeBMotionLumBlock(MpegFrame *prev, MpegFrame *next, int by, int bx,
                       int mode, int fmy, int fmx, int bmy, int bmx,
                       LumBlock motionBlock)
{
    LumBlock	prevBlock, nextBlock;
    register int	y, x;

    switch(mode) {
    case MOTION_FORWARD:
      ComputeMotionLumBlock(prev, by, bx, fmy, fmx, motionBlock);
      break;
    case MOTION_BACKWARD:
      ComputeMotionLumBlock(next, by, bx, bmy, bmx, motionBlock);
      break;
    case MOTION_INTERPOLATE:
      ComputeMotionLumBlock(prev, by, bx, fmy, fmx, prevBlock);
      ComputeMotionLumBlock(next, by, bx, bmy, bmx, nextBlock);
      
      for ( y = 0; y < 16; y++ ) {
	for ( x = 0; x < 16; x++ ) {
	  motionBlock[y][x] = (prevBlock[y][x]+nextBlock[y][x]+1)/2;
	}
      }
      break;
    default:
      io_printf(stderr, "Bad mode!\nProgrammer error!\n");
      break;
      
    }
}


/*===========================================================================*
 *
 * EstimateSecondsPerBFrame
 *
 *	estimate the seconds to compute a B-frame
 *
 * RETURNS:	the time, in seconds
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

float EstimateSecondsPerBFrame(void)
   {float rv;

    if (numFrames == 0)
       rv = 20.0;
    else
       rv = (float) totalTime/((float) TIME_RATE*(float) numFrames);

    return(rv);}

/*===========================================================================*
 *
 * ComputeBMotionBlock
 *
 *	compute the block resulting from motion compensation
 *
 * RETURNS:	motionBlock is modified
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITION:	the motion vectors must be valid!
 *
 *===========================================================================*/
static void
ComputeBMotionBlock(MpegFrame *prev, MpegFrame *next, int by, int bx, int mode,
                    int fmy, int fmx, int bmy, int bmx, Block motionBlock,
                    int type)
{
    Block	prevBlock, nextBlock;
    register int	y, x;

    switch(mode) {
	case MOTION_FORWARD:
	    if ( type == LUM_BLOCK ) {
		ComputeMotionBlock(prev->ref_y, by, bx, fmy, fmx, motionBlock);
	    } else if ( type == CB_BLOCK ) {
		ComputeMotionBlock(prev->ref_cb, by, bx, fmy, fmx, motionBlock);
	    } else if ( type == CR_BLOCK ) {
		ComputeMotionBlock(prev->ref_cr, by, bx, fmy, fmx, motionBlock);
	    }
	    break;
	case MOTION_BACKWARD:
	    if ( type == LUM_BLOCK ) {
		ComputeMotionBlock(next->ref_y, by, bx, bmy, bmx, motionBlock);
	    } else if ( type == CB_BLOCK ) {
		ComputeMotionBlock(next->ref_cb, by, bx, bmy, bmx, motionBlock);
	    } else if ( type == CR_BLOCK ) {
		ComputeMotionBlock(next->ref_cr, by, bx, bmy, bmx, motionBlock);
	    }
	    break;
	case MOTION_INTERPOLATE:
	    if ( type == LUM_BLOCK ) {
		ComputeMotionBlock(prev->ref_y, by, bx, fmy, fmx, prevBlock);
		ComputeMotionBlock(next->ref_y, by, bx, bmy, bmx, nextBlock);
	    } else if ( type == CB_BLOCK ) {
		ComputeMotionBlock(prev->ref_cb, by, bx, fmy, fmx, prevBlock);
		ComputeMotionBlock(next->ref_cb, by, bx, bmy, bmx, nextBlock);
	    } else if ( type == CR_BLOCK ) {
		ComputeMotionBlock(prev->ref_cr, by, bx, fmy, fmx, prevBlock);
		ComputeMotionBlock(next->ref_cr, by, bx, bmy, bmx, nextBlock);
	    }

	    for ( y = 0; y < 8; y++ ) {
		for ( x = 0; x < 8; x++ ) {
		    motionBlock[y][x] = (prevBlock[y][x]+nextBlock[y][x]+1)/2;
		}
	    }
	    break;
    }
}


/*===========================================================================*
 *
 * ComputeBDiffDCTs
 *
 *	compute the DCT of the error term
 *
 * RETURNS:	appropriate blocks of current will contain the DCTs
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITION:	the motion vectors must be valid!
 *
 *===========================================================================*/
static void
ComputeBDiffDCTs(MpegFrame *current, MpegFrame *prev, MpegFrame *next,
                 int by, int bx, int mode, int fmy, int fmx, int bmy, int bmx,
                 int *pattern)
{
    Block   motionBlock;
    if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "lb\n");
    if ( *pattern & 0x20 ) {
	ComputeBMotionBlock(prev, next, by, bx, mode, fmy, fmx,
			    bmy, bmx, motionBlock, LUM_BLOCK);
	if (! ComputeDiffDCTBlock(current->y_blocks[by][bx], dct[by][bx], motionBlock)) {
	  *pattern ^=  0x20;
	}
    }

    if ( *pattern & 0x10 ) {
	ComputeBMotionBlock(prev, next, by, bx+1, mode, fmy, fmx,
			    bmy, bmx, motionBlock, LUM_BLOCK);
	if (! ComputeDiffDCTBlock(current->y_blocks[by][bx+1], dct[by][bx+1], motionBlock)) {
	  *pattern ^=  0x10;
	}
    }

    if ( *pattern & 0x8 ) {
	ComputeBMotionBlock(prev, next, by+1, bx, mode, fmy, fmx,
			    bmy, bmx, motionBlock, LUM_BLOCK);
	if (! ComputeDiffDCTBlock(current->y_blocks[by+1][bx], dct[by+1][bx], motionBlock)) {
	  *pattern ^= 0x8;
	}
    }

    if ( *pattern & 0x4 ) {
	ComputeBMotionBlock(prev, next, by+1, bx+1, mode, fmy, fmx,
			    bmy, bmx, motionBlock, LUM_BLOCK);
	if (! ComputeDiffDCTBlock(current->y_blocks[by+1][bx+1], dct[by+1][bx+1], motionBlock)) {
	  *pattern ^= 0x4;
	}
    }
    if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "cbb\n");
    if ( *pattern & 0x2 ) {
	ComputeBMotionBlock(prev, next, by>>1, bx>>1, mode, fmy/2, fmx/2,
			    bmy/2, bmx/2, motionBlock, CB_BLOCK);
	if (! ComputeDiffDCTBlock(current->cb_blocks[by >> 1][bx >> 1], dctb[by >> 1][bx >> 1], motionBlock)) {
	  *pattern ^= 0x2;
	}
    }
    if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "crb\n");

    if ( *pattern & 0x1 ) {
	ComputeBMotionBlock(prev, next, by>>1, bx>>1, mode, fmy/2, fmx/2,
			    bmy/2, bmx/2, motionBlock, CR_BLOCK);
	if (! ComputeDiffDCTBlock(current->cr_blocks[by >> 1][bx >> 1], dctr[by >> 1][bx >> 1], motionBlock)) {
	  *pattern ^= 0x1;
	}
    }
}


/*===========================================================================*
 *
 *			    USER-MODIFIABLE
 *
 * DoBIntraCode
 *
 *	decides if this block should be coded as intra-block
 *
 * RETURNS:	TRUE if intra-coding should be used; FALSE otherwise
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITION:	the motion vectors must be valid!
 *
 *===========================================================================*/
static boolean
DoBIntraCode(MpegFrame *current, MpegFrame *prev, MpegFrame *next,
             int by, int bx, int mode, int fmy, int fmx, int bmy, int bmx)
{
    int	    x, y;
    int32 sum = 0, vard = 0, varc = 0, dif;
    int32 currPixel, prevPixel;
    LumBlock	motionBlock;
    int	    fy, fx;

    ComputeBMotionLumBlock(prev, next, by, bx, mode, fmy, fmx,
			   bmy, bmx, motionBlock);

    MOTION_TO_FRAME_COORD(by, bx, 0, 0, fy, fx);

    for ( y = 0; y < 16; y++ ) {
	for ( x = 0; x < 16; x++ ) {
	    currPixel = current->orig_y[fy+y][fx+x];
	    prevPixel = motionBlock[y][x];

	    sum += currPixel;
	    varc += currPixel*currPixel;

	    dif = currPixel - prevPixel;
	    vard += dif*dif;
	}
    }

    vard >>= 8;		/* divide by 256; assumes mean is close to zero */
    varc = (varc>>8) - (sum>>8)*(sum>>8);

    if ( vard <= 64 ) {
	return(FALSE);
    } else if ( vard < varc ) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

static int
ComputeBlockColorDiff(current, motionBlock)
    Block current, motionBlock;
{
  register int x, y, diff_total = 0, diff_tmp;
  
  for ( y = 0; y < 8; y++ ) {
    for ( x = 0; x < 8; x++ ) {
      diff_tmp = current[y][x] - motionBlock[y][x];
      diff_total += ABS(diff_tmp);
    }
  }
  return(diff_total);
}

/*===========================================================================*
 *
 *			    USER-MODIFIABLE
 *
 * MotionSufficient
 *
 *	decides if this motion vector is sufficient without DCT coding
 *
 * RETURNS:	TRUE if no DCT is needed; FALSE otherwise
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITION:	the motion vectors must be valid!
 *
 *===========================================================================*/
static boolean
MotionSufficient(MpegFrame *curr, LumBlock currBlock, MpegFrame *prev,
                 MpegFrame *next, int by, int bx, int mode,
                 int fmy, int fmx, int bmy, int bmx)
{
    LumBlock   mLumBlock;
    Block mColorBlock;
    int lumErr, colorErr;

    /* check bounds */
    if ( mode != MOTION_BACKWARD ) {
	if ( (by*DCTSIZE+(fmy-1)/2 < 0) || ((by+2)*DCTSIZE+(fmy+1)/2-1 >= Fsize_y) ) {
	    return(FALSE);
	}
	if ( (bx*DCTSIZE+(fmx-1)/2 < 0) || ((bx+2)*DCTSIZE+(fmx+1)/2-1 >= Fsize_x) ) {
	    return(FALSE);
	}
    }

    if ( mode != MOTION_FORWARD ) {
	if ( (by*DCTSIZE+(bmy-1)/2 < 0) || ((by+2)*DCTSIZE+(bmy+1)/2-1 >= Fsize_y) ) {
	    return(FALSE);
	}
	if ( (bx*DCTSIZE+(bmx-1)/2 < 0) || ((bx+2)*DCTSIZE+(bmx+1)/2-1 >= Fsize_x) ) {
	    return(FALSE);
	}
    }

    /* check Lum */
    ComputeBMotionLumBlock(prev, next, by, bx, mode, fmy, fmx,
			   bmy, bmx, mLumBlock);
    lumErr =  LumBlockMAD(currBlock, mLumBlock, 0x7fffffff);
    if (lumErr > 512) {
      return(FALSE);
    }

    /* check color */
    ComputeBMotionBlock(prev, next, by>>1, bx>>1, mode, fmy/2, fmx/2, 
			bmy/2, bmx/2, mColorBlock, CR_BLOCK);
    colorErr = ComputeBlockColorDiff(curr->cr_blocks[by >> 1][bx >> 1], mColorBlock);
    ComputeBMotionBlock(prev, next, by>>1, bx>>1, mode, fmy/2, fmx/2, 
			bmy/2, bmx/2, mColorBlock, CB_BLOCK);
    colorErr += ComputeBlockColorDiff(curr->cb_blocks[by >> 1][bx >> 1], mColorBlock);
    
    return((colorErr < 256)); /* lumErr checked above */
}


/*===========================================================================*
 * bitio.c								     *
 *									     *
 *	Procedures concerned with the bit-wise I/O			     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	Bitio_New							     *
 *	Bitio_Free							     *
 *	Bitio_Write							     *
 *	Bitio_Flush							     *
 *	Bitio_WriteToSocket						     *
 *	Bitio_BytePad							     *
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
 * Revision 1.13  1995/06/21  18:36:06  smoot
 * added a flush when done with file
 *
 * Revision 1.12  1995/01/19  23:07:15  eyhung
 * Changed copyrights
 *
 * Revision 1.11  1994/11/12  02:11:43  keving
 * nothing
 *
 * Revision 1.10  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.10  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.9  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.8  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.7  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.6  1993/02/17  23:21:41  dwallach
 * checkin prior to keving's joining the project
 *
 * Revision 1.5  1993/01/18  10:20:02  dwallach
 * *** empty log message ***
 *
 * Revision 1.4  1993/01/18  10:17:29  dwallach
 * RCS headers installed, code indented uniformly
 *
 * Revision 1.4  1993/01/18  10:17:29  dwallach
 * RCS headers installed, code indented uniformly
 *
 */

/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/
 
static void Dump _ANSI_ARGS_((BitBucket *bbPtr));


/*==================*
 * STATIC VARIABLES *
 *==================*/

static u_int32_t lower_mask[33] = {
    0,
    0x1, 0x3, 0x7, 0xf,
    0x1f, 0x3f, 0x7f, 0xff,
    0x1ff, 0x3ff, 0x7ff, 0xfff,
    0x1fff, 0x3fff, 0x7fff, 0xffff,
    0x1ffff, 0x3ffff, 0x7ffff, 0xfffff,
    0x1fffff, 0x3fffff, 0x7fffff, 0xffffff,
    0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
    0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * Bitio_New
 *
 *	Create a new bit bucket; filePtr is a pointer to the open file the
 *	bits should ultimately be written to.
 *
 * RETURNS:	pointer to the resulting bit bucket
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
BitBucket *
Bitio_New(FILE *filePtr)
{
    BitBucket *bbPtr;

    bbPtr = CMAKE(BitBucket);
    ERRCHK(bbPtr, "malloc");

    bbPtr->firstPtr = bbPtr->lastPtr = CMAKE(struct bitBucket);
    ERRCHK(bbPtr->firstPtr, "malloc");

    bbPtr->totalbits = 0;
    bbPtr->cumulativeBits = 0;
    bbPtr->bitsWritten = 0;
    bbPtr->filePtr = filePtr;

    bbPtr->firstPtr->nextPtr = NULL;
    bbPtr->firstPtr->bitsleft = MAXBITS_PER_BUCKET;
    bbPtr->firstPtr->bitsleftcur = 32;
    bbPtr->firstPtr->currword = 0;
    memset((char *)bbPtr->firstPtr->bits, 0, sizeof(u_int32_t) * WORDS_PER_BUCKET);

    return(bbPtr);
}


/*===========================================================================*
 *
 * Bitio_Free
 *
 *	Frees the memory associated with the given bit bucket
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Bitio_Free(BitBucket *bbPtr)
{
    struct bitBucket *tmpPtr, *nextPtr;

    for (tmpPtr = bbPtr->firstPtr; tmpPtr != NULL; tmpPtr = nextPtr) {
	nextPtr = tmpPtr->nextPtr;
	CFREE(tmpPtr);
    }
    CFREE(bbPtr);
}


/*===========================================================================*
 *
 * Bitio_Write
 *
 *	Writes 'nbits' bits from 'bits' into the given bit bucket
 *	'nbits' must be between 0 and 32
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    if the number of bits in the bit bucket surpasses
 *		    MAX_BITS, then that many bits are flushed to the
 *		    appropriate output file
 *
 *===========================================================================*/
void
Bitio_Write(BitBucket *bbPtr, u_int32_t bits, int nbits)
{
    register struct bitBucket *lastPtr, *newPtr;
    register int delta;

    assert(nbits <= 32 && nbits >= 0);

    /*
     * Clear top bits if not part of data, necessary due to down and
     * dirty calls of Bitio_Write with unecessary top bits set.
     */

    bits = bits & lower_mask[nbits];

    bbPtr->totalbits += nbits;
    bbPtr->cumulativeBits += nbits;
    lastPtr = bbPtr->lastPtr;

    delta = nbits - lastPtr->bitsleft;
    if (delta >= 0) {
	/*
         * there's not enough room in the current bucket, so we're
         * going to have to allocate another bucket
    	 */
	newPtr = lastPtr->nextPtr = CMAKE(struct bitBucket);
	ERRCHK(newPtr, "malloc");
	newPtr->nextPtr = NULL;
	newPtr->bitsleft = MAXBITS_PER_BUCKET;
	newPtr->bitsleftcur = 32;
	newPtr->currword = 0;
	memset((char *)newPtr->bits, 0, sizeof(u_int32_t) * WORDS_PER_BUCKET);
	bbPtr->lastPtr = newPtr;

	assert(lastPtr->currword == WORDS_PER_BUCKET - 1);
	lastPtr->bits[WORDS_PER_BUCKET - 1] |= (bits >> delta);
	lastPtr->bitsleft = 0;
	lastPtr->bitsleftcur = 0;
	/* lastPtr->currword++; */

	if (!delta) {
	    if ( bbPtr->totalbits > MAX_BITS ) {
		Dump(bbPtr);
	    }
	}

	assert(delta <= 32);
	newPtr->bits[0] = (bits & lower_mask[delta]) << (32 - delta);
	newPtr->bitsleft -= delta;
	newPtr->bitsleftcur -= delta;
    } else {
	/*
         * the current bucket will be sufficient
	 */
	delta = nbits - lastPtr->bitsleftcur;
	lastPtr->bitsleftcur -= nbits;
	lastPtr->bitsleft -= nbits;

	if (delta >= 0)
	{
	    /*
	     * these bits will span more than one word
	     */
	    lastPtr->bits[lastPtr->currword] |= (bits >> delta);
	    lastPtr->currword++;
	    lastPtr->bits[lastPtr->currword] = (bits & lower_mask[delta]) << (32 - delta);
	    lastPtr->bitsleftcur = 32 - delta;
	} else {
	    /*
	     * these bits will fit, whole
	     */
	    lastPtr->bits[lastPtr->currword] |= (bits << (-delta));
	}
    }

    if ( bbPtr->totalbits > MAX_BITS )	/* flush bits */
	Dump(bbPtr);
}


/*===========================================================================*
 *
 * Bitio_Flush
 *
 *	Flushes all of the remaining bits in the given bit bucket to the
 *	appropriate output file.  It will generate up to the nearest 8-bit
 *	unit of bits, which means that up to 7 extra 0 bits will be appended
 *	to the end of the file.
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    frees the bit bucket
 *
 *===========================================================================*/
void
Bitio_Flush(BitBucket *bbPtr)
{
    struct bitBucket *ptr, *tempPtr;
    u_int32_t buffer[WORDS_PER_BUCKET];
    u_int32_t  lastWord;
    int i, nitems;
    int	    bitsWritten = 0;
    int	    bitsLeft;
    int	    numWords;
    u_int8_t   charBuf[4];
    boolean    flushHere = FALSE;
    time_t  tempTimeStart, tempTimeEnd;

    time(&tempTimeStart);

    bitsLeft = bbPtr->totalbits;

    for (ptr = bbPtr->firstPtr; ptr; ptr = ptr->nextPtr) {
	if (ptr->bitsleftcur == 32 && ptr->currword == 0) {
	    continue;		/* empty */
	}

	if ( bitsLeft >= 32 ) {
	    if ( ((ptr->currword + 1) * 32) > bitsLeft ) {
		numWords = ptr->currword;
		flushHere = TRUE;
	    } else {
		numWords = ptr->currword+1;
	    }

	    for (i = 0; i < numWords; i++) {
		buffer[i] = htonl(ptr->bits[i]);
	    }

	    nitems = io_write(buffer, sizeof(u_int32_t), numWords, bbPtr->filePtr);
	    if (nitems != numWords) {
		io_printf(stderr, "Whoa!  Trouble writing %d bytes (got %d items)!  Game over, dude!\n",
			(int)(numWords), nitems);
		exit(1);
	    }

	    bitsWritten += (numWords * 32);
	    bitsLeft -= (numWords * 32);
	} else {
	    flushHere = TRUE;
	}

	if ( (bitsLeft < 32) && flushHere ) {
	    lastWord = ptr->bits[ptr->currword];

	    /* output the lastPtr word in big-endian order (network) */

	    /* now write out lastPtr bits */
	    while ( bitsLeft > 0 ) {
		charBuf[0] = (lastWord >> 24);
		charBuf[0] &= lower_mask[8];
		io_write(charBuf, 1, sizeof(u_int8_t), bbPtr->filePtr);
		lastWord = (lastWord << 8);
		bitsLeft -= 8;
		bitsWritten += 8;
	    }
	}
    }
    io_flush(bbPtr->filePtr);
    while ( bbPtr->firstPtr != ptr ) {
	tempPtr = bbPtr->firstPtr;
	bbPtr->firstPtr = tempPtr->nextPtr;
	CFREE(tempPtr);
    }

    CFREE(bbPtr);

    time(&tempTimeEnd);
    IOtime += (tempTimeEnd-tempTimeStart);
}


/*===========================================================================*
 *
 * Bitio_WriteToSocket
 *
 *	Writes all of the remaining bits in the given bit bucket to the
 *	given socket.  May pad the end of the socket stream with extra 0
 *	bits as does Bitio_Flush.
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    frees the bit bucket
 *
 *===========================================================================*/
void
Bitio_WriteToSocket(BitBucket *bbPtr, int socket)
{
    struct bitBucket *ptr, *tempPtr;
    u_int32_t buffer[WORDS_PER_BUCKET];
    u_int32_t  lastWord;
    int i, nitems;
    int	    bitsWritten = 0;
    int	    bitsLeft;
    int	    numWords;
    u_int8_t   charBuf[4];
    boolean    flushHere = FALSE;

    bitsLeft = bbPtr->totalbits;

    for (ptr = bbPtr->firstPtr; ptr; ptr = ptr->nextPtr) {
	if (ptr->bitsleftcur == 32 && ptr->currword == 0) {
	    continue;		/* empty */
	}

	if ( bitsLeft >= 32 ) {
	    if ( ((ptr->currword + 1) * 32) > bitsLeft ) {
		numWords = ptr->currword;
		flushHere = TRUE;
	    } else {
		numWords = ptr->currword+1;
	    }

	    for (i = 0; i < numWords; i++) {
		buffer[i] = htonl(ptr->bits[i]);
	    }

	    nitems = write(socket, buffer, numWords * sizeof(u_int32_t));
	    if (nitems != numWords*sizeof(u_int32_t)) {
		io_printf(stderr, "Whoa!  Trouble writing %d bytes (got %d bytes)!  Game over, dude!\n",
			(int)(numWords*sizeof(u_int32_t)), nitems);
		exit(1);
	    }

	    bitsWritten += (numWords * 32);
	    bitsLeft -= (numWords * 32);
	} else {
	    flushHere = TRUE;
	}

	if ( (bitsLeft < 32) && flushHere ) {
	    lastWord = ptr->bits[ptr->currword];

	    /* output the lastPtr word in big-endian order (network) */

	    /* now write out lastPtr bits */
	    while ( bitsLeft > 0 ) {
		charBuf[0] = (lastWord >> 24);
		charBuf[0] &= lower_mask[8];
		if ( write(socket, charBuf, 1) != 1 ) {
		    io_printf(stderr, "ERROR:  write of lastPtr bits\n");
		    exit(1);
		}
		lastWord = (lastWord << 8);
		bitsLeft -= 8;
		bitsWritten += 8;
	    }
	}
    }

    while ( bbPtr->firstPtr != ptr ) {
	tempPtr = bbPtr->firstPtr;
	bbPtr->firstPtr = tempPtr->nextPtr;
	CFREE(tempPtr);
    }

    CFREE(bbPtr);
}


/*===========================================================================*
 *
 * Bitio_BytePad
 *
 *	Pads the end of the bit bucket to the nearest byte with 0 bits
 *
 * RETURNS:	nothing
 *
 *===========================================================================*/
void
Bitio_BytePad(BitBucket *bbPtr)
{
    struct bitBucket *lastPtrPtr = bbPtr->lastPtr;

    if (lastPtrPtr->bitsleftcur % 8) {
	Bitio_Write(bbPtr, 0, lastPtrPtr->bitsleftcur % 8);
    }
}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * Dump
 *
 *	Writes out the first MAX_BITS bits of the bit bucket to the
 *	appropriate output file
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:  none
 *
 *===========================================================================*/
static void
Dump(BitBucket *bbPtr)
{
    struct bitBucket *ptr, *tempPtr;
    u_int32_t buffer[WORDS_PER_BUCKET];
    int i, nitems;
    int	    bitsWritten = 0;
    time_t  tempTimeStart, tempTimeEnd;

    time(&tempTimeStart);

    for (ptr = bbPtr->firstPtr; ptr && (bitsWritten < MAX_BITS);
	 ptr = ptr->nextPtr) {
	if (ptr->bitsleftcur == 32 && ptr->currword == 0) {
	    continue;		/* empty */
	}

	for (i = 0; i <= ptr->currword; i++) {
	    buffer[i] = htonl(ptr->bits[i]);
	}

	nitems = io_write((u_int8_t *)buffer, sizeof(u_int32_t),
			   (ptr->currword + 1), bbPtr->filePtr);
	if (nitems != (ptr->currword+1)) {
	    io_printf(stderr, "Error: Trouble writing %d bytes (got %d items)\n",
		    (int)((ptr->currword+1)), nitems);
	    assert(0);
	    exit(1);
	}

	bitsWritten += ((ptr->currword + 1) * 32);
    }

    while ( bbPtr->firstPtr != ptr ) {
	tempPtr = bbPtr->firstPtr;
	bbPtr->firstPtr = tempPtr->nextPtr;
	CFREE(tempPtr);
    }

    bbPtr->totalbits -= bitsWritten;
    bbPtr->bitsWritten += bitsWritten;

    time(&tempTimeEnd);
    IOtime += (tempTimeEnd-tempTimeStart);
}


/*===========================================================================*
 * block.c								     *
 *									     *
 *	Block routines							     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	ComputeDiffDCTBlock						     *
 *	ComputeDiffDCTs							     *
 *	ComputeMotionBlock						     *
 *	ComputeMotionLumBlock						     *
 *	LumBlockMAD							     *
 *	LumMotionError							     *
 *	LumMotionErrorSubSampled					     *
 *	LumAddMotionError						     *
 *	AddMotionBlock							     *
 *	BlockToData							     *
 *	BlockifyFrame							     *
 *									     *
 * NOTES:   MAD	=   Mean Absolute Difference				     *
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
 *  Revision 1.16  1995/08/07 21:43:29  smoot
 *  restructured lumdiff so it read better and used a switch instead of ifs
 *
 *  Revision 1.15  1995/06/21 22:21:16  smoot
 *  added TUNEing options
 *
 * Revision 1.14  1995/05/08  22:47:45  smoot
 * typechecking better
 *
 * Revision 1.13  1995/05/08  22:44:14  smoot
 * added prototypes (postdct.h)
 *
 * Revision 1.12  1995/05/02  21:44:07  smoot
 * added tuneing parameters
 *
 * Revision 1.11  1995/03/31  23:50:45  smoot
 * removed block bound (moved to opts.c)
 *
 * Revision 1.10  1995/03/29  20:12:39  smoot
 * added block_bound for TUNEing
 *
 * Revision 1.9  1995/02/01  21:43:55  smoot
 * cleanup
 *
 * Revision 1.8  1995/01/19  23:52:43  smoot
 * Made computeDiffDCTs able to rule out changes to the pattern (diff too small)
 *
 * Revision 1.7  1995/01/19  23:07:17  eyhung
 * Changed copyrights
 *
 * Revision 1.6  1994/11/12  02:11:44  keving
 * nothing
 *
 * Revision 1.5  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.5  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.4  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.3  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.2  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.1  1993/04/08  21:31:59  keving
 * nothing
 *
 */


/*===========================*
 * COMPUTE DCT OF DIFFERENCE *
 *===========================*/

/*===========================================================================*
 *
 * ComputeDiffDCTBlock
 *
 *	compute current-motionBlock, take the DCT, and put the difference
 *	back into current
 *
 * RETURNS:	current block modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
boolean
ComputeDiffDCTBlock(Block current, Block dest, Block motionBlock)
{
    register int x, y, diff = 0;

    for ( y = 0; y < 8; y++ ) {
	for ( x = 0; x < 8; x++ ) {
	  current[y][x] -= motionBlock[y][x];
	  diff += ABS(current[y][x]);
	}
    }
    /* Kill the block if change is too small     */
    /* (block_bound defaults to 128, see opts.c) */
    if (diff < block_bound)
       return(FALSE);

    mp_fwd_dct_block2(current, dest);

    return(TRUE);
}

/*===========================================================================*
 *
 * ComputeDiffDCTs
 *
 *	appropriate (according to pattern, the coded block pattern) blocks
 *	of 'current' are diff'ed and DCT'd.
 *
 * RETURNS:	current blocks modified
 *
 * SIDE EFFECTS:    Can remove too-small difference blocks from pattern
 *
 * PRECONDITIONS:	appropriate blocks of 'current' have not yet been
 *			modified
 *
 *===========================================================================*/
void
ComputeDiffDCTs(MpegFrame *current, MpegFrame *prev, int by, int bx,
                int my, int mx, int *pattern)
{
    Block   motionBlock;

    if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "lp\n");
    if ( *pattern & 0x20 ) {
	ComputeMotionBlock(prev->ref_y, by, bx, my, mx, motionBlock);
	if (!ComputeDiffDCTBlock(current->y_blocks[by][bx], dct[by][bx], motionBlock))
	  *pattern^=0x20;
    }

    if ( *pattern & 0x10 ) {
	ComputeMotionBlock(prev->ref_y, by, bx+1, my, mx, motionBlock);
	if (!ComputeDiffDCTBlock(current->y_blocks[by][bx+1], dct[by][bx+1], motionBlock))
	  *pattern^=0x10;
    }

    if ( *pattern & 0x8 ) {
	ComputeMotionBlock(prev->ref_y, by+1, bx, my, mx, motionBlock);
	if (!ComputeDiffDCTBlock(current->y_blocks[by+1][bx], dct[by+1][bx], motionBlock))
	  *pattern^=0x8;
    }

    if ( *pattern & 0x4 ) {
	ComputeMotionBlock(prev->ref_y, by+1, bx+1, my, mx, motionBlock);
	if (!ComputeDiffDCTBlock(current->y_blocks[by+1][bx+1], dct[by+1][bx+1], motionBlock))
	  *pattern^=0x4;
    }

    if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "cbp\n");
    if ( *pattern & 0x2 ) {
	ComputeMotionBlock(prev->ref_cb, by >> 1, bx >> 1, my/2, mx/2, motionBlock);
	if (!ComputeDiffDCTBlock(current->cb_blocks[by >> 1][bx >> 1], dctb[by >> 1][bx >> 1], motionBlock))
	  *pattern^=0x2;
    }

    if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "crp\n");
    if ( *pattern & 0x1 ) {
	ComputeMotionBlock(prev->ref_cr, by >> 1, bx >> 1, my/2, mx/2, motionBlock);
	if (!ComputeDiffDCTBlock(current->cr_blocks[by >> 1][bx >> 1], dctr[by >> 1][bx >> 1], motionBlock))
	  *pattern^=0x1;
    }
}


	/*======================*
	 * COMPUTE MOTION BLOCK *
	 *======================*/

/*===========================================================================*
 *
 * ComputeMotionBlock
 *
 *	compute the motion-compensated block
 *
 * RETURNS:	motionBlock
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:	motion vector MUST be valid
 *
 * NOTE:  could try to speed this up using halfX, halfY, halfBoth,
 *	  but then would have to compute for chrominance, and it's just
 *	  not worth the trouble (this procedure is not called relatively
 *	  often -- a constant number of times per macroblock)
 *
 *===========================================================================*/
void
ComputeMotionBlock(u_int8_t **prev, int by, int bx, int my, int mx,
                   Block motionBlock)
{
    register int   fy, fx;
    register int   y;
    register int16 *destPtr;
    register u_int8_t *srcPtr;
    register u_int8_t *srcPtr2;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, (my/2), (mx/2), fy, fx);

    if ( xHalf && yHalf ) {
	/* really should be fy+y-1 and fy+y so do (fy-1)+y = fy+y-1 and
	   (fy-1)+y+1 = fy+y
	 */
	if ( my < 0 ) {
	    fy--;
	}
	if ( mx < 0 ) {
	    fx--;
	}

	for ( y = 0; y < 8; y++ ) {
	    destPtr = motionBlock[y];
	    srcPtr = &(prev[fy+y][fx]);
	    srcPtr2 = &(prev[fy+y+1][fx]);

	    destPtr[0] = (srcPtr[0]+srcPtr[1]+srcPtr2[0]+srcPtr2[1]+2)>>2;
	    destPtr[1] = (srcPtr[1]+srcPtr[2]+srcPtr2[1]+srcPtr2[2]+2)>>2;
	    destPtr[2] = (srcPtr[2]+srcPtr[3]+srcPtr2[2]+srcPtr2[3]+2)>>2;
	    destPtr[3] = (srcPtr[3]+srcPtr[4]+srcPtr2[3]+srcPtr2[4]+2)>>2;
	    destPtr[4] = (srcPtr[4]+srcPtr[5]+srcPtr2[4]+srcPtr2[5]+2)>>2;
	    destPtr[5] = (srcPtr[5]+srcPtr[6]+srcPtr2[5]+srcPtr2[6]+2)>>2;
	    destPtr[6] = (srcPtr[6]+srcPtr[7]+srcPtr2[6]+srcPtr2[7]+2)>>2;
	    destPtr[7] = (srcPtr[7]+srcPtr[8]+srcPtr2[7]+srcPtr2[8]+2)>>2;
	}
    } else if ( xHalf ) {
	if ( mx < 0 ) {
	    fx--;
	}

	for ( y = 0; y < 8; y++ ) {
	    destPtr = motionBlock[y];
	    srcPtr = &(prev[fy+y][fx]);

	    destPtr[0] = (srcPtr[0]+srcPtr[1]+1)>>1;
	    destPtr[1] = (srcPtr[1]+srcPtr[2]+1)>>1;
	    destPtr[2] = (srcPtr[2]+srcPtr[3]+1)>>1;
	    destPtr[3] = (srcPtr[3]+srcPtr[4]+1)>>1;
	    destPtr[4] = (srcPtr[4]+srcPtr[5]+1)>>1;
	    destPtr[5] = (srcPtr[5]+srcPtr[6]+1)>>1;
	    destPtr[6] = (srcPtr[6]+srcPtr[7]+1)>>1;
	    destPtr[7] = (srcPtr[7]+srcPtr[8]+1)>>1;
	}
    } else if ( yHalf ) {
	if ( my < 0 ) {
	    fy--;
	}

	for ( y = 0; y < 8; y++ ) {
	    destPtr = motionBlock[y];
	    srcPtr = &(prev[fy+y][fx]);
	    srcPtr2 = &(prev[fy+y+1][fx]);

	    destPtr[0] = (srcPtr[0]+srcPtr2[0]+1)>>1;
	    destPtr[1] = (srcPtr[1]+srcPtr2[1]+1)>>1;
	    destPtr[2] = (srcPtr[2]+srcPtr2[2]+1)>>1;
	    destPtr[3] = (srcPtr[3]+srcPtr2[3]+1)>>1;
	    destPtr[4] = (srcPtr[4]+srcPtr2[4]+1)>>1;
	    destPtr[5] = (srcPtr[5]+srcPtr2[5]+1)>>1;
	    destPtr[6] = (srcPtr[6]+srcPtr2[6]+1)>>1;
	    destPtr[7] = (srcPtr[7]+srcPtr2[7]+1)>>1;
	}
    } else {
	for ( y = 0; y < 8; y++ ) {
	    destPtr = motionBlock[y];
	    srcPtr = &(prev[fy+y][fx]);

	    destPtr[0] = (u_int8_t) srcPtr[0];
	    destPtr[1] = (u_int8_t) srcPtr[1];
	    destPtr[2] = (u_int8_t) srcPtr[2];
	    destPtr[3] = (u_int8_t) srcPtr[3];
	    destPtr[4] = (u_int8_t) srcPtr[4];
	    destPtr[5] = (u_int8_t) srcPtr[5];
	    destPtr[6] = (u_int8_t) srcPtr[6];
	    destPtr[7] = (u_int8_t) srcPtr[7];
	}
    }
}


/*===========================================================================*
 *
 * ComputeMotionLumBlock
 *
 *	compute the motion-compensated luminance block
 *
 * RETURNS:	motionBlock
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:	motion vector MUST be valid
 *
 * NOTE:  see ComputeMotionBlock
 *
 *===========================================================================*/
void
ComputeMotionLumBlock(MpegFrame *prevFrame, int by, int bx, int my, int mx,
                      LumBlock motionBlock)
{
    register u_int8_t *across;
    register int32 *macross;
    register int y;
    u_int8_t **prev;
    int	    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf ) {
	if ( mx < 0 ) {
	    fx--;
	}

	if ( yHalf ) {
	    if ( my < 0 ) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf ) {
	if ( my < 0 ) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    for ( y = 0; y < 16; y++ ) {
	across = &(prev[fy+y][fx]);
	macross = motionBlock[y];

	macross[0] = across[0];
	macross[1] = across[1];
	macross[2] = across[2];
	macross[3] = across[3];
	macross[4] = across[4];
	macross[5] = across[5];
	macross[6] = across[6];
	macross[7] = across[7];
	macross[8] = across[8];
	macross[9] = across[9];
	macross[10] = across[10];
	macross[11] = across[11];
	macross[12] = across[12];
	macross[13]= across[13];
	macross[14] = across[14];
	macross[15] = across[15];
    }

    /* this is what's really happening, in slow motion:
     *
     *	for ( y = 0; y < 16; y++, py++ )
     *      for ( x = 0; x < 16; x++, px++ )
     *		motionBlock[y][x] = prev[fy+y][fx+x];
     *
     */
}


/*=======================*
 * BASIC ERROR FUNCTIONS *
 *=======================*/


/*===========================================================================*
 *
 * LumBlockMAD
 *
 *	return the MAD of two luminance blocks
 *
 * RETURNS:	the MAD, if less than bestSoFar, or
 *		some number bigger if not
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int32
LumBlockMAD(LumBlock currentBlock, LumBlock motionBlock, int32 bestSoFar)
{
    register int32   diff = 0;    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register int y, x;

    for ( y = 0; y < 16; y++ ) {
	for ( x = 0; x < 16; x++ ) {
	    localDiff = currentBlock[y][x] - motionBlock[y][x];
	    diff += ABS(localDiff);
	}

	if ( diff > bestSoFar ) {
	    return(diff);
	}
    }

    return((int32) diff);
}


/*===========================================================================*
 *
 * LumMotionError
 *
 *	return the MAD of the currentBlock and the motion-compensated block
 *      (without TUNEing)
 *
 * RETURNS:	the MAD, if less than bestSoFar, or
 *		some number bigger if not
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:  motion vector MUST be valid
 *
 * NOTES:  this is the procedure that is called the most, and should therefore
 *         be the most optimized!!!
 *
 *===========================================================================*/
int32
LumMotionError(LumBlock currentBlock, MpegFrame *prevFrame,
               int by, int bx, int my, int mx, int32 bestSoFar)
{
    register int32 adiff = 0,  diff = 0;    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register u_int8_t *across;
    register int32 *cacross;
    register int y;
    u_int8_t **prev;
    int	    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf ) {
	if ( mx < 0 ) {
	    fx--;
	}

	if ( yHalf ) {
	    if ( my < 0 ) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf ) {
	if ( my < 0 ) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    switch (SearchCompareMode) {
    case DEFAULT_SEARCH: /* Default. */
      /* this is what's happening:
       *	ComputeMotionLumBlock(prevFrame, by, bx, my, mx, lumMotionBlock);
       *	for ( y = 0; y < 16; y++ )
       *	    for ( x = 0; x < 16; x++ )
       *	    {
       *		localDiff = currentBlock[y][x] - lumMotionBlock[y][x];
       *		diff += ABS(localDiff);
       *	    }
       */
      for ( y = 0; y < 16; y++ ) {
	across = &(prev[fy+y][fx]);
	cacross = currentBlock[y];
	
	localDiff = across[0]-cacross[0];     diff += ABS(localDiff);
	localDiff = across[1]-cacross[1];     diff += ABS(localDiff);
	localDiff = across[2]-cacross[2];     diff += ABS(localDiff);
	localDiff = across[3]-cacross[3];     diff += ABS(localDiff);
	localDiff = across[4]-cacross[4];     diff += ABS(localDiff);
	localDiff = across[5]-cacross[5];     diff += ABS(localDiff);
	localDiff = across[6]-cacross[6];     diff += ABS(localDiff);
	localDiff = across[7]-cacross[7];     diff += ABS(localDiff);
	localDiff = across[8]-cacross[8];     diff += ABS(localDiff);
	localDiff = across[9]-cacross[9];     diff += ABS(localDiff);
	localDiff = across[10]-cacross[10];     diff += ABS(localDiff);
	localDiff = across[11]-cacross[11];     diff += ABS(localDiff);
	localDiff = across[12]-cacross[12];     diff += ABS(localDiff);
	localDiff = across[13]-cacross[13];     diff += ABS(localDiff);
	localDiff = across[14]-cacross[14];     diff += ABS(localDiff);
	localDiff = across[15]-cacross[15];     diff += ABS(localDiff);
	
	if ( diff > bestSoFar ) {
	  return(diff);
	}
      }
      break;
      
    case LOCAL_DCT: {
      Block     dctdiff[4], dctquant[4];
      FlatBlock quant;
      int x, i, tmp;
      int distortion=0, datarate=0;
      int pq = GetPQScale();
      
      for (y = 0;  y < 16;  y++) {
	across = &(prev[fy+y][fx]);
	cacross = currentBlock[y];
	for (x = 0;  x < 16;  x++) {
	  dctdiff[(x>7)+2*(y>7)][y%8][x%8] = cacross[x]-across[x];
	}}

      /* Calculate rate */
      for (i = 0;  i < 4;  i++) {
	mp_fwd_dct_block2(dctdiff[i], dctdiff[i]);
	if (Mpost_QuantZigBlock(dctdiff[i], quant, pq, FALSE) == MPOST_ZERO) {
	  /* no sense in continuing */
	  memset((char *)dctquant[i], 0, sizeof(Block));
	} else {
	  Mpost_UnQuantZigBlock(quant, dctquant[i], pq, FALSE);
	  mpeg_jrevdct((int16 *)dctquant[i]);
	  datarate += CalcRLEHuffLength(quant);
	}
      }
      
      /* Calculate distortion */
      for (y = 0;  y < 16;  y++) {
	across = &(prev[fy+y][fx]);
	cacross = currentBlock[y];
	for (x = 0;  x < 16;  x++) {
	  tmp = across[x] - cacross[x] + dctquant[(x>7)+2*(y>7)][y%8][x%8];
	  distortion += tmp*tmp;
	}}
      distortion /= 256;
      distortion *= LocalDCTDistortScale;
      datarate *= LocalDCTRateScale;
      diff = (int) sqrt(distortion*distortion + datarate*datarate);
      break;
    }

    case NO_DC_SEARCH: {
      extern int32 niqtable[];
      int pq = niqtable[0]*GetPQScale();
      
      for ( y = 0; y < 16; y++ ) {
	across = &(prev[fy+y][fx]);
	cacross = currentBlock[y];

	localDiff = across[0]-cacross[0];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[1]-cacross[1];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[2]-cacross[2];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[3]-cacross[3];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[4]-cacross[4];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[5]-cacross[5];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[6]-cacross[6];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[7]-cacross[7];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[8]-cacross[8];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[9]-cacross[9];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[10]-cacross[10];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[11]-cacross[11];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[12]-cacross[12];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[13]-cacross[13];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[14]-cacross[14];  diff += localDiff; adiff += ABS(localDiff);
	localDiff = across[15]-cacross[15];  diff += localDiff; adiff += ABS(localDiff);

      }

      diff /= 64*pq;  /* diff is now the DC difference (with QSCALE 1) */
      adiff -= 64*pq*ABS(diff);
      diff = adiff;
    }
      break;

    case DO_Mean_Squared_Distortion:
      for ( y = 0; y < 16; y++ ) {
	across = &(prev[fy+y][fx]);
	cacross = currentBlock[y];

	localDiff = across[0]-cacross[0];     diff += localDiff*localDiff;
	localDiff = across[1]-cacross[1];     diff += localDiff*localDiff;
	localDiff = across[2]-cacross[2];     diff += localDiff*localDiff;
	localDiff = across[3]-cacross[3];     diff += localDiff*localDiff;
	localDiff = across[4]-cacross[4];     diff += localDiff*localDiff;
	localDiff = across[5]-cacross[5];     diff += localDiff*localDiff;
	localDiff = across[6]-cacross[6];     diff += localDiff*localDiff;
	localDiff = across[7]-cacross[7];     diff += localDiff*localDiff;
	localDiff = across[8]-cacross[8];     diff += localDiff*localDiff;
	localDiff = across[9]-cacross[9];     diff += localDiff*localDiff;
	localDiff = across[10]-cacross[10];     diff += localDiff*localDiff;
	localDiff = across[11]-cacross[11];     diff += localDiff*localDiff;
	localDiff = across[12]-cacross[12];     diff += localDiff*localDiff;
	localDiff = across[13]-cacross[13];     diff += localDiff*localDiff;
	localDiff = across[14]-cacross[14];     diff += localDiff*localDiff;
	localDiff = across[15]-cacross[15];     diff += localDiff*localDiff;

	if ( diff > bestSoFar ) {
	  return(diff);
	}
      }
      break;
    } /* End of Switch */

    return(diff);
}


/*===========================================================================*
 *
 * LumAddMotionError
 *
 *	return the MAD of the currentBlock and the average of the blockSoFar
 *	and the motion-compensated block (this is used for B-frame searches)
 *
 * RETURNS:	the MAD, if less than bestSoFar, or
 *		some number bigger if not
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:  motion vector MUST be valid
 *
 *===========================================================================*/
int32
LumAddMotionError(LumBlock currentBlock, LumBlock blockSoFar,
                  MpegFrame *prevFrame,
                  int by, int bx, int my, int mx, int32 bestSoFar)
{
    register int32   diff = 0;    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register u_int8_t *across;
    register int32 *bacross;
    register int32 *cacross;
    register int y;
    u_int8_t **prev;
    int	    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf ) {
	if ( mx < 0 ) {
	    fx--;
	}

	if ( yHalf ) {
	    if ( my < 0 ) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf ) {
	if ( my < 0 ) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

/* do we add 1 before dividing by two?  Yes -- see MPEG-1 doc page 46 */

#define ADD_ADD_DIFF(d,l,a,b,c,i)       \
    l = ((a[i]+b[i]+1)>>1)-c[i];        \
    d += ABS(l)

    for ( y = 0; y < 16; y++ ) {
	across = &(prev[fy+y][fx]);
	bacross = blockSoFar[y];
	cacross = currentBlock[y];

	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,0);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,1);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,2);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,3);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,4);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,5);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,6);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,7);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,8);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,9);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,10);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,11);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,12);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,13);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,14);
	ADD_ADD_DIFF(diff,localDiff,across,bacross,cacross,15);

	if ( diff > bestSoFar ) {
	    return(diff);
	}
    }

    /* this is what's happening:
     *
     *	ComputeMotionLumBlock(prevFrame, by, bx, my, mx, lumMotionBlock);
     *
     *	for ( y = 0; y < 16; y++ )
     *	    for ( x = 0; x < 16; x++ )
     *	    {
     *		localDiff = currentBlock[y][x] - lumMotionBlock[y][x];
     *		diff += ABS(localDiff);
     *	    }
     *
     */

    return(diff);
}


/*===========================================================================*
 *
 * AddMotionBlock
 *
 *	adds the motion-compensated block to the given block
 *
 * RETURNS:	block modified
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:  motion vector MUST be valid
 *
 *===========================================================================*/
void
AddMotionBlock(Block block, u_int8_t **prev, int by, int bx, int my, int mx)
{
    int	    fy, fx;
    int	    x, y;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, (my/2), (mx/2), fy, fx);

    if ( xHalf && yHalf ) {
	/* really should be fy+y-1 and fy+y so do (fy-1)+y = fy+y-1 and
	   (fy-1)+y+1 = fy+y
	 */
	if ( my < 0 ) {
	    fy--;
	}
	if ( mx < 0 ) {
	    fx--;
	}

	for ( y = 0; y < 8; y++ ) {
	    for ( x = 0; x < 8; x++ ) {
		block[y][x] += (prev[fy+y][fx+x]+prev[fy+y][fx+x+1]+
				    prev[fy+y+1][fx+x]+prev[fy+y+1][fx+x+1]+2)>>2;
	    }
	}
    } else if ( xHalf ) {
	if ( mx < 0 ) {
	    fx--;
	}

	for ( y = 0; y < 8; y++ ) {
	    for ( x = 0; x < 8; x++ ) {
		block[y][x] += (prev[fy+y][fx+x]+prev[fy+y][fx+x+1]+1)>>1;
	    }
	}
    } else if ( yHalf ) {
	if ( my < 0 ) {
	    fy--;
	}

	for ( y = 0; y < 8; y++ ) {
	    for ( x = 0; x < 8; x++ ) {
		block[y][x] += (prev[fy+y][fx+x]+prev[fy+y+1][fx+x]+1)>>1;
	    }
	}
    } else {
	for ( y = 0; y < 8; y++ ) {
	    for ( x = 0; x < 8; x++ ) {
		block[y][x] += (int16)prev[fy+y][fx+x];
	    }
	}
    }
}


/*===========================================================================*
 *
 * AddBMotionBlock
 *
 *	adds the motion-compensated B-frame block to the given block
 *
 * RETURNS:	block modified
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:  motion vectors MUST be valid
 *
 *===========================================================================*/
void
AddBMotionBlock(Block block, u_int8_t **prev, u_int8_t **next, 
                int by, int bx, int mode, int fmy, int fmx, int bmy, int bmx)
{
    int	    x, y;
    Block   prevBlock, nextBlock;

    if ( mode == MOTION_FORWARD ) {
	AddMotionBlock(block, prev, by, bx, fmy, fmx);
    } else if ( mode == MOTION_BACKWARD ) {
	AddMotionBlock(block, next, by, bx, bmy, bmx);
    } else {
	ComputeMotionBlock(prev, by, bx, fmy, fmx, prevBlock);
	ComputeMotionBlock(next, by, bx, bmy, bmx, nextBlock);

	for ( y = 0; y < 8; y++ ) {
	    for ( x = 0; x < 8; x++ ) {
		block[y][x] += (prevBlock[y][x]+nextBlock[y][x]+1)/2;
	    }
	}
    }
}


/*===========================================================================*
 *
 * BlockToData
 *
 *	copies the given block into the appropriate data area
 *
 * RETURNS:	data modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
BlockToData(u_int8_t **data, Block block, int by, int bx)
{
    register int x, y;
    register int fy, fx;
    register int16    blockItem;

    BLOCK_TO_FRAME_COORD(by, bx, fy, fx);

    for ( y = 0; y < 8; y++ ) {
	for ( x = 0; x < 8; x++ ) {
	    blockItem = block[y][x];
	    data[fy+y][fx+x] = TRUNCATE_UINT8(blockItem);
	}
    }
}


/*===========================================================================*
 *
 * BlockifyFrame
 *
 *	copies data into appropriate blocks
 *
 * RETURNS:	mf modified
 *
 * SIDE EFFECTS:    none
 *
 * NOTES:  probably shouldn't be in this file
 *
 *===========================================================================*/
void
BlockifyFrame(framePtr)
    MpegFrame *framePtr;
{
    register int dctx, dcty;
    register int x, y;
    register int bx, by;
    register int fy, fx;
    register int16  *destPtr;
    register u_int8_t  *srcPtr;
    register int16  *destPtr2;
    register u_int8_t  *srcPtr2;
    Block   *blockPtr;
    Block   *blockPtr2;

    dctx = Fsize_x / DCTSIZE;
    dcty = Fsize_y / DCTSIZE;

    /*
     * copy y data into y_blocks
     */
    for (by = 0; by < dcty; by++) {
	fy = by*DCTSIZE;
	for (bx = 0; bx < dctx; bx++) {
	    fx = bx*DCTSIZE;
	    blockPtr = (Block *) &(framePtr->y_blocks[by][bx][0][0]);
	    for (y = 0; y < DCTSIZE; y++) {
		destPtr = &((*blockPtr)[y][0]);
		srcPtr = &(framePtr->orig_y[fy+y][fx]);
		for (x = 0; x < DCTSIZE; x++) {
		    destPtr[x] = srcPtr[x];
		}
	    }
	}
    }

    /*
     * copy cr/cb data into cr/cb_blocks
     */
    for (by = 0; by < (dcty >> 1); by++) {
	fy = by*DCTSIZE;
	for (bx = 0; bx < (dctx >> 1); bx++) {
	    fx = bx*DCTSIZE;
	    blockPtr = (Block *) &(framePtr->cr_blocks[by][bx][0][0]);
	    blockPtr2 = (Block *) &(framePtr->cb_blocks[by][bx][0][0]);
	    for (y = 0; y < DCTSIZE; y++) {
		destPtr = &((*blockPtr)[y][0]);
		srcPtr = &(framePtr->orig_cr[fy+y][fx]);
		destPtr2 = &((*blockPtr2)[y][0]);
		srcPtr2 = &(framePtr->orig_cb[fy+y][fx]);
		for (x = 0; x < DCTSIZE; x++) {
		    destPtr[x] = srcPtr[x];
		    destPtr2[x] = srcPtr2[x];
		}
	    }
	}
    }
}


/*===========================================================================*
 *									     *
 * UNUSED PROCEDURES							     *
 *									     *
 *	The following procedures are all unused by the encoder		     *
 *									     *
 *	They are listed here for your convenience.  You might want to use    *
 *	them if you experiment with different search techniques		     *
 *									     *
 *===========================================================================*/

#ifdef UNUSED_PROCEDURES

/* this procedure calculates the subsampled motion block (obviously)
 *
 * for speed, this procedure is probably not called anywhere (it is
 * incorporated directly into LumDiffA, LumDiffB, etc.
 *
 * but leave it here anyway for clarity
 *
 * (startY, startX) = (0,0) for A....(0,1) for B...(1,0) for C...(1,1) for D
 *  
 */
void
ComputeSubSampledMotionLumBlock(prevFrame, by, bx, my, mx, motionBlock,
				startY, startX)
    MpegFrame *prevFrame;
    int by;
    int bx;
    int my;
    int mx;
    LumBlock motionBlock;
    int startY;
    int startX;
{
    register u_int8_t *across;
    register int32 *macross;
    register int32 *lastx;
    register int y;
    u_int8_t **prev;
    int    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf ) {
	if ( mx < 0 ) {
	    fx--;
	}

	if ( yHalf ) {
	    if ( my < 0 ) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf ) {
	if ( my < 0 ) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    for ( y = startY; y < 16; y += 2 ) {
	across = &(prev[fy+y][fx+startX]);
	macross = &(motionBlock[y][startX]);
	lastx = &(motionBlock[y][16]);
	while ( macross < lastx ) {
	    (*macross) = (*across);
	    across += 2;
	    macross += 2;
	}
    }

    /* this is what's really going on in slow motion:
     *
     *	for ( y = startY; y < 16; y += 2 )
     *	    for ( x = startX; x < 16; x += 2 )
     *		motionBlock[y][x] = prev[fy+y][fx+x];
     *
     */
}


/*===========================================================================*
 *
 * LumMotionErrorSubSampled
 *
 *	return the MAD of the currentBlock and the motion-compensated block,
 *	subsampled 4:1 with given starting coordinates (startY, startX)
 *
 * RETURNS:	the MAD
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:  motion vector MUST be valid
 *
 * NOTES:  this procedure is never called.  Instead, see subsample.c.  This
 *         procedure is provided only for possible use in extensions
 *
 *===========================================================================*/
int32
LumMotionErrorSubSampled(currentBlock, prevFrame, by, bx, my, mx, startY,
			 startX)
    LumBlock currentBlock;
    MpegFrame *prevFrame;
    int by;
    int bx;
    int my;
    int mx;
    int startY;
    int startX;
{
    register int32    diff = 0;	    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register int32 *cacross;
    register u_int8_t *macross;
    register int32 *lastx;
    register int y;
    u_int8_t **prev;
    int    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf ) {
	if ( mx < 0 ) {
	    fx--;
	}

	if ( yHalf ) {
	    if ( my < 0 ) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf ) {
	if ( my < 0 ) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    for ( y = startY; y < 16; y += 2 ) {
	macross = &(prev[fy+y][fx+startX]);
	cacross = &(currentBlock[y][startX]);
	lastx = &(currentBlock[y][16]);
	while ( cacross < lastx ) {
	    localDiff = (*cacross)-(*macross);
	    diff += ABS(localDiff);
	    macross += 2;
	    cacross += 2;
	}
    }

    /* this is what's really happening:
     *
     *	ComputeSubSampledMotionLumBlock(prevFrame, by, bx, my, mx,
     *					lumMotionBlock, startY, startX);
     *
     *	for ( y = startY; y < 16; y += 2 )
     *	    for ( x = startX; x < 16; x += 2 )
     *	    {
     *	     	localDiff = currentBlock[y][x] - lumMotionBlock[y][x];
     *		diff += ABS(localDiff);
     *	    }
     *
     */

    return((int32) diff);
}


#endif /* UNUSED_PROCEDURES */
/*===========================================================================*
 * bsearch.c								     *
 *									     *
 *	Procedures concerned with the B-frame motion search		     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	SetBSearchAlg							     *
 *	BMotionSearch							     *
 *	BSearchName							     *
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
 *  Revision 1.10  1995/08/07 21:49:01  smoot
 *  fixed bug in initial-B-frame searches
 *
 *  Revision 1.9  1995/06/26 21:36:07  smoot
 *  added new ordering constraints
 *  (B frames which are backward P's at the start of a sequence)
 *
 *  Revision 1.8  1995/03/27 19:17:43  smoot
 *  killed useless type error messge (int32 defiend as int)
 *
 * Revision 1.7  1995/01/19  23:07:20  eyhung
 * Changed copyrights
 *
 * Revision 1.6  1994/12/07  00:40:36  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.5  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.4  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.3  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.2  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.1  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.1  1993/03/02  18:27:05  keving
 * nothing
 *
 */


/*==================*
 * STATIC VARIABLES *
 *==================*/

static int	bsearchAlg;


/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static int32	FindBestMatch _ANSI_ARGS_((LumBlock block, LumBlock currentBlock, MpegFrame *prev,
		      int by, int bx, int *motionY, int *motionX, int32 bestSoFar, int searchRange));
static int BMotionSearchSimple _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prev,
			MpegFrame *next, int by, int bx, int *fmy, int *fmx,
			int *bmy, int *bmx, int oldMode));
static int BMotionSearchCross2 _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prev,
			MpegFrame *next, int by, int bx, int *fmy, int *fmx,
			int *bmy, int *bmx, int oldMode));
static int BMotionSearchExhaust _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prev,
			MpegFrame *next, int by, int bx, int *fmy, int *fmx,
			int *bmy, int *bmx, int oldMode));
static void BMotionSearchNoInterp _ANSI_ARGS_((LumBlock currentBlock, MpegFrame *prev,
				  MpegFrame *next, int by, int bx,
				  int *fmy, int *fmx, int32 *forwardErr,
				  int *bmy, int *bmx, int32 *backErr,
					       boolean backNeeded));
static int32	FindBestMatchExhaust _ANSI_ARGS_((LumBlock block, LumBlock currentBlock, MpegFrame *prev,
		      int by, int bx, int *motionY, int *motionX,
		      int32 bestSoFar, int searchRange));
static int32	FindBestMatchTwoLevel _ANSI_ARGS_((LumBlock block, LumBlock currentBlock, MpegFrame *prev,
		      int by, int bx, int *motionY, int *motionX,
		      int32 bestSoFar, int searchRange));
static int32	FindBestMatchLogarithmic _ANSI_ARGS_((LumBlock block, LumBlock currentBlock, MpegFrame *prev,
		      int by, int bx, int *motionY, int *motionX,
		      int32 bestSoFar, int searchRange));
static int32	FindBestMatchSubSample _ANSI_ARGS_((LumBlock block, LumBlock currentBlock, MpegFrame *prev,
		      int by, int bx, int *motionY, int *motionX,
		      int32 bestSoFar, int searchRange));


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================*
 * INITIALIZATION PROCEDURES *
 *===========================*/


/*===========================================================================*
 *
 * SetBSearchAlg
 *
 *	set the B-search algorithm
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    bsearchAlg modified
 *
 *===========================================================================*/
void
SetBSearchAlg(alg)
    char *alg;
{
    if ( strcmp(alg, "SIMPLE") == 0 ) {
	bsearchAlg = BSEARCH_SIMPLE;
    } else if ( strcmp(alg, "CROSS2") == 0 ) {
	bsearchAlg = BSEARCH_CROSS2;
    } else if ( strcmp(alg, "EXHAUSTIVE") == 0 ) {
	bsearchAlg = BSEARCH_EXHAUSTIVE;
    } else {
	io_printf(stderr, "ERROR:  Illegal bsearch alg:  %s\n", alg);
	exit(1);
    }
}


/*===========================================================================*
 *
 * BSearchName
 *
 *	return the text of the B-search algorithm
 *
 * RETURNS:	a pointer to the string
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
char *
BSearchName()
{
    switch(bsearchAlg) {
	case BSEARCH_SIMPLE:
	    return("SIMPLE");
	case BSEARCH_CROSS2:
	    return("CROSS2");
	case BSEARCH_EXHAUSTIVE:
	    return("EXHAUSTIVE");
	default:
	    return("BAD_VALUE"); /* this fucntion is not used by MPEG */
	    /*break;*/
    }
}


/*===========================================================================*
 *
 * BMotionSearch
 *
 *	search for the best B-frame motion vectors
 *
 * RETURNS:	MOTION_FORWARD	    forward motion should be used
 *		MOTION_BACKWARD     backward motion should be used
 *		MOTION_INTERPOLATE  both should be used and interpolated
 *
 * OUTPUTS:	*fmx, *fmy  =	TWICE the forward motion vector
 *		*bmx, *bmy  =	TWICE the backward motion vector
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:	The relevant block in 'current' is valid (it has not
 *			been dct'd).  Thus, the data in 'current' can be
 *			accesed through y_blocks, cr_blocks, and cb_blocks.
 *			This is not the case for the blocks in 'prev' and
 *			'next.'  Therefore, references into 'prev' and 'next'
 *			should be done
 *			through the struct items ref_y, ref_cr, ref_cb
 *
 * POSTCONDITIONS:	current, prev, next should be unchanged.
 *			Some computation could be saved by requiring
 *			the dct'd difference to be put into current's block
 *			elements here, depending on the search technique.
 *			However, it was decided that it mucks up the code
 *			organization a little, and the saving in computation
 *			would be relatively little (if any).
 *
 * NOTES:	the search procedure MAY return (0,0) motion vectors
 *
 *===========================================================================*/
int
BMotionSearch(currentBlock, prev, next, by, bx, fmy, fmx, bmy, bmx, oldMode)
    LumBlock currentBlock;
    MpegFrame *prev;
    MpegFrame *next;
    int by;
    int bx;
    int *fmy;
    int *fmx;
    int *bmy;
    int *bmx;
    int oldMode;
{
  /* If we are an initial B frame, no possibility of forward motion */
  if (prev == (MpegFrame *) NULL) {
    PMotionSearch(currentBlock, next, by, bx, bmy, bmx);
    return(MOTION_BACKWARD);
  }
  
  /* otherwise simply call the appropriate algorithm, based on user preference */
  
    switch(bsearchAlg) {
	case BSEARCH_SIMPLE:
	    return(BMotionSearchSimple(currentBlock, prev, next, by, bx, fmy,
				       fmx, bmy, bmx, oldMode));
	    /*break;*/
	case BSEARCH_CROSS2:
	    return(BMotionSearchCross2(currentBlock, prev, next, by, bx, fmy,
				       fmx, bmy, bmx, oldMode));
	    /*break;*/
	case BSEARCH_EXHAUSTIVE:
	    return(BMotionSearchExhaust(currentBlock, prev, next, by, bx, fmy,
				       fmx, bmy, bmx, oldMode));
	    /*break;*/
	default:
	    io_printf(stderr, "Illegal B-frame motion search algorithm:  %d\n",
		    bsearchAlg);
	    return(-1); /* this fucntion is not used by MPEG */
    }
}


/*===========================================================================*
 *
 * BMotionSearchSimple
 *
 *	does a simple search for B-frame motion vectors
 *	see BMotionSearch for generic description
 *
 * DESCRIPTION:
 *	1)  find best backward and forward vectors
 *	2)  compute interpolated error using those two vectors
 *	3)  return the best of the three choices
 *
 *===========================================================================*/
static int
BMotionSearchSimple(currentBlock, prev, next, by, bx, fmy, fmx, bmy, bmx,
		    oldMode)
    LumBlock currentBlock;
    MpegFrame *prev;
    MpegFrame *next;
    int by;
    int bx;
    int *fmy;
    int *fmx;
    int *bmy;
    int *bmx;
    int oldMode;
{
    int32	forwardErr, backErr, interpErr;
    LumBlock	interpBlock;
    int32	bestSoFar;

    			    /* STEP 1 */
    BMotionSearchNoInterp(currentBlock, prev, next, by, bx, fmy, fmx,
			  &forwardErr, bmy, bmx, &backErr, TRUE);
			  
    			    /* STEP 2 */

    ComputeBMotionLumBlock(prev, next, by, bx, MOTION_INTERPOLATE,
			   *fmy, *fmx, *bmy, *bmx, interpBlock);
    bestSoFar = min(backErr, forwardErr);
    interpErr = LumBlockMAD(currentBlock, interpBlock, bestSoFar);

			    /* STEP 3 */

    if ( interpErr <= forwardErr ) {
	if ( interpErr <= backErr ) {
	    return(MOTION_INTERPOLATE);
	}
	else
	    return(MOTION_BACKWARD);
    } else if ( forwardErr <= backErr ) {
	return(MOTION_FORWARD);
    } else {
	return(MOTION_BACKWARD);
    }
}


/*===========================================================================*
 *
 * BMotionSearchCross2
 *
 *	does a cross-2 search for B-frame motion vectors
 *	see BMotionSearch for generic description
 *
 * DESCRIPTION:
 *	1)  find best backward and forward vectors
 *	2)  find best matching interpolating vectors
 *	3)  return the best of the 4 choices
 *
 *===========================================================================*/
static int
BMotionSearchCross2(currentBlock, prev, next, by, bx, fmy, fmx, bmy, bmx,
		    oldMode)
    LumBlock currentBlock;
    MpegFrame *prev;
    MpegFrame *next;
    int by;
    int bx;
    int *fmy;
    int *fmx;
    int *bmy;
    int *bmx;
    int oldMode;
{
    LumBlock	forwardBlock, backBlock;
    int32	forwardErr, backErr, interpErr;
    int		newfmy, newfmx, newbmy, newbmx;
    int32	interpErr2;
    int32	bestErr;

			    /* STEP 1 */

    BMotionSearchNoInterp(currentBlock, prev, next, by, bx, fmy, fmx,
			  &forwardErr, bmy, bmx, &backErr, TRUE);

    bestErr = min(forwardErr, backErr);

			    /* STEP 2 */
    ComputeBMotionLumBlock(prev, next, by, bx, MOTION_FORWARD,
			   *fmy, *fmx, 0, 0, forwardBlock);
    ComputeBMotionLumBlock(prev, next, by, bx, MOTION_BACKWARD,
			   0, 0, *bmy, *bmx, backBlock);

    /* try a cross-search; total of 4 local searches */    
    newbmy = *bmy;	newbmx = *bmx;
    newfmy = *fmy;	newfmx = *fmx;

    interpErr = FindBestMatch(forwardBlock, currentBlock, next, by, bx,
			      &newbmy, &newbmx, bestErr, searchRangeB);
    bestErr = min(bestErr, interpErr);
    interpErr2 = FindBestMatch(backBlock, currentBlock, prev, by, bx,
			       &newfmy, &newfmx, bestErr, searchRangeB);

			    /* STEP 3 */

    if ( interpErr <= interpErr2 ) {
	newfmy = *fmy;
	newfmx = *fmx;
    }
    else
    {
	newbmy = *bmy;
	newbmx = *bmx;
	interpErr = interpErr2;
    }

    if ( interpErr <= forwardErr ) {
	if ( interpErr <= backErr ) {
	    *fmy = newfmy;
	    *fmx = newfmx;
	    *bmy = newbmy;
	    *bmx = newbmx;

	    return(MOTION_INTERPOLATE);
	}
	else
	    return(MOTION_BACKWARD);
    } else if ( forwardErr <= backErr ) {
	return(MOTION_FORWARD);
    } else {
	return(MOTION_BACKWARD);
    }
}


/*===========================================================================*
 *
 * BMotionSearchExhaust
 *
 *	does an exhaustive search for B-frame motion vectors
 *	see BMotionSearch for generic description
 *
 * DESCRIPTION:
 *	1)  find best backward and forward vectors
 *	2)  use exhaustive search to find best interpolating vectors
 *	3)  return the best of the 3 choices
 *
 *===========================================================================*/
static int
BMotionSearchExhaust(currentBlock, prev, next, by, bx, fmy, fmx, bmy, bmx,
		    oldMode)
    LumBlock currentBlock;
    MpegFrame *prev;
    MpegFrame *next;
    int by;
    int bx;
    int *fmy;
    int *fmx;
    int *bmy;
    int *bmx;
    int oldMode;
{
    register int mx, my;
    int32 diff, bestDiff;
    int	    stepSize;
    LumBlock	forwardBlock;
    int32	forwardErr, backErr;
    int		newbmy, newbmx;
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;
    boolean result;

			    /* STEP 1 */

    BMotionSearchNoInterp(currentBlock, prev, next, by, bx, fmy, fmx,
			  &forwardErr, bmy, bmx, &backErr, FALSE);

    if ( forwardErr <= backErr ) {
        bestDiff = forwardErr;
	result = MOTION_FORWARD;
    }
    else
    {
        bestDiff = backErr;
	result = MOTION_BACKWARD;
    }

			    /* STEP 2 */

    stepSize = (pixelFullSearch ? 2 : 1);

    COMPUTE_MOTION_BOUNDARY(by,bx,stepSize,leftMY,leftMX,rightMY,rightMX);

    if ( searchRangeB < rightMY ) {
	rightMY = searchRangeB;
    }
    if ( searchRangeB < rightMX ) {
	rightMX = searchRangeB;
    }

    for ( my = -searchRangeB; my < rightMY; my += stepSize ) {
	if ( my < leftMY ) {
	    continue;
	}

	for ( mx = -searchRangeB; mx < rightMX; mx += stepSize ) {
	    if ( mx < leftMX ) {
		continue;
	    }

	    ComputeBMotionLumBlock(prev, next, by, bx, MOTION_FORWARD,
			   my, mx, 0, 0, forwardBlock);

	    newbmy = my;	newbmx = mx;

	    diff = FindBestMatch(forwardBlock, currentBlock, next, by, bx,
				 &newbmy, &newbmx, bestDiff, searchRangeB);

	    if ( diff < bestDiff ) {
		*fmy = my;
		*fmx = mx;
		*bmy = newbmy;
		*bmx = newbmx;
		bestDiff = diff;
		result = MOTION_INTERPOLATE;
	    }
	}
    }

    return(result);
}


/*===========================================================================*
 *
 * FindBestMatch
 *
 *	given a motion-compensated block in one direction, tries to find
 *	the best motion vector in the opposite direction to match it
 *
 * RETURNS:	the best vector (*motionY, *motionX), and the corresponding
 *		error is returned if it is better than bestSoFar.  If not,
 *		then a number greater than bestSoFar is returned and
 *		(*motionY, *motionX) has no meaning.
 *
 * SIDE EFFECTS:  none
 *
 *===========================================================================*/
static int32
FindBestMatch(block, currentBlock, prev, by, bx, motionY, motionX, bestSoFar, searchRange)
    LumBlock block;
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int32 bestSoFar;
    int searchRange;
{
    int32	result;

    switch(psearchAlg) {
	case PSEARCH_SUBSAMPLE:
	    result = FindBestMatchSubSample(block, currentBlock, prev, by, bx,
					    motionY, motionX, bestSoFar, searchRange);
	    break;
	case PSEARCH_EXHAUSTIVE:
	    result = FindBestMatchExhaust(block, currentBlock, prev, by, bx,
					  motionY, motionX, bestSoFar, searchRange);
	    break;
	case PSEARCH_LOGARITHMIC:
	    result = FindBestMatchLogarithmic(block, currentBlock, prev, by, bx,
					      motionY, motionX, bestSoFar, searchRange);
	    break;
	case PSEARCH_TWOLEVEL:
	    result = FindBestMatchTwoLevel(block, currentBlock, prev, by, bx,
					   motionY, motionX, bestSoFar, searchRange);
	    break;
	default:
	    io_printf(stderr, "ERROR:  Illegal P-search alg %d\n", psearchAlg);
	    exit(1);
    }

    return(result);
}


/*===========================================================================*
 *
 * FindBestMatchExhaust
 *
 *	tries to find matching motion vector
 *	see FindBestMatch for generic description
 *
 * DESCRIPTION:  uses an exhaustive search
 *
 *===========================================================================*/
static int32
FindBestMatchExhaust(block, currentBlock, prev, by, bx, motionY, motionX,
		     bestSoFar, searchRange)
    LumBlock block;
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int32 bestSoFar;
    int searchRange;
{
    register int mx, my;
    int32 diff, bestDiff;
    int	    stepSize;
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;
    int	    distance;
    int	    tempRightMY, tempRightMX;
    boolean changed = FALSE;

    stepSize = (pixelFullSearch ? 2 : 1);

    COMPUTE_MOTION_BOUNDARY(by,bx,stepSize,leftMY,leftMX,rightMY,rightMX);

    /* try old motion vector first */
    if ( VALID_MOTION(*motionY, *motionX) ) {
	bestDiff = LumAddMotionError(currentBlock, block, prev, by, bx,
				     *motionY, *motionX, bestSoFar);

	if ( bestSoFar < bestDiff ) {
	    bestDiff = bestSoFar;
	}
    }
    else
    {
	*motionY = 0;
	*motionX = 0;

	bestDiff = bestSoFar;
    }

/* maybe should try spiral pattern centered around  prev motion vector? */


    /* try a spiral pattern */    
    for ( distance = stepSize; distance <= searchRange; distance += stepSize ) {
	tempRightMY = rightMY;
	if ( distance < tempRightMY ) {
	    tempRightMY = distance;
	}
	tempRightMX = rightMX;
	if ( distance < tempRightMX ) {
	    tempRightMX = distance;
	}

	/* do top, bottom */
	for ( my = -distance; my < tempRightMY;
	      my += max(tempRightMY+distance-stepSize, stepSize) ) {
	    if ( my < leftMY ) {
		continue;
	    }

	    for ( mx = -distance; mx < tempRightMX; mx += stepSize ) {
		if ( mx < leftMX ) {
		    continue;
		}

		diff = LumAddMotionError(currentBlock, block, prev, by, bx,
				     my, mx, bestDiff);

		if ( diff < bestDiff ) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		}
	    }
	}

	/* do left, right */
	for ( mx = -distance; mx < tempRightMX; mx += max(tempRightMX+distance-stepSize, stepSize) ) {
	    if ( mx < leftMX ) {
		continue;
	    }

	    for ( my = -distance+stepSize; my < tempRightMY-stepSize; my += stepSize ) {
		if ( my < leftMY ) {
		    continue;
		}

		diff = LumAddMotionError(currentBlock, block, prev, by, bx,
				     my, mx, bestDiff);

		if ( diff < bestDiff ) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		    changed = TRUE;
		}
	    }
	}
    }

    if ( ! changed ) {
	bestDiff++;
    }

    return(bestDiff);
}


/*===========================================================================*
 *
 * FindBestMatchTwoLevel
 *
 *	tries to find matching motion vector
 *	see FindBestMatch for generic description
 *
 * DESCRIPTION:  uses an exhaustive full-pixel search, then looks at
 *		 neighboring half-pixels
 *
 *===========================================================================*/
static int32
FindBestMatchTwoLevel(block, currentBlock, prev, by, bx, motionY, motionX,
		      bestSoFar, searchRange)
    LumBlock block;
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int32 bestSoFar;
    int searchRange;
{
    register int mx, my;
    int32 diff, bestDiff;
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;
    int	    distance;
    int	    tempRightMY, tempRightMX;
    boolean changed = FALSE;
    int	    yOffset, xOffset;

    /* exhaustive full-pixel search first */

    COMPUTE_MOTION_BOUNDARY(by,bx,2,leftMY,leftMX,rightMY,rightMX);

    rightMY--;
    rightMX--;

    /* convert vector into full-pixel vector */
    if ( *motionY > 0 ) {
	if ( ((*motionY) % 2) == 1 ) {
	    (*motionY)--;
	}
    } else if ( ((-(*motionY)) % 2) == 1 ) {
	(*motionY)++;
    }

    if ( *motionX > 0 ) {
	if ( ((*motionX) % 2) == 1 ) {
	    (*motionX)--;
	}
    } else if ( ((-(*motionX)) % 2) == 1 ) {
	(*motionX)++;
    }

    /* try old motion vector first */
    if ( VALID_MOTION(*motionY, *motionX) ) {
	bestDiff = LumAddMotionError(currentBlock, block, prev, by, bx,
				     *motionY, *motionX, bestSoFar);

	if ( bestSoFar < bestDiff ) {
	    bestDiff = bestSoFar;
	}
    }
    else
    {
	*motionY = 0;
	*motionX = 0;

	bestDiff = bestSoFar;
    }

    rightMY++;
    rightMX++;

/* maybe should try spiral pattern centered around  prev motion vector? */


    /* try a spiral pattern */    
    for ( distance = 2; distance <= searchRange; distance += 2 ) {
	tempRightMY = rightMY;
	if ( distance < tempRightMY ) {
	    tempRightMY = distance;
	}
	tempRightMX = rightMX;
	if ( distance < tempRightMX ) {
	    tempRightMX = distance;
	}

	/* do top, bottom */
	for ( my = -distance; my < tempRightMY;
	      my += max(tempRightMY+distance-2, 2) ) {
	    if ( my < leftMY ) {
		continue;
	    }

	    for ( mx = -distance; mx < tempRightMX; mx += 2 ) {
		if ( mx < leftMX ) {
		    continue;
		}

		diff = LumAddMotionError(currentBlock, block, prev, by, bx,
				     my, mx, bestDiff);

		if ( diff < bestDiff ) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		}
	    }
	}

	/* do left, right */
	for ( mx = -distance; mx < tempRightMX; mx += max(tempRightMX+distance-2, 2) ) {
	    if ( mx < leftMX ) {
		continue;
	    }

	    for ( my = -distance+2; my < tempRightMY-2; my += 2 ) {
		if ( my < leftMY ) {
		    continue;
		}

		diff = LumAddMotionError(currentBlock, block, prev, by, bx,
				     my, mx, bestDiff);

		if ( diff < bestDiff ) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		    changed = TRUE;
		}
	    }
	}
    }

    /* now look at neighboring half-pixels */
    my = *motionY;
    mx = *motionX;

    rightMY--;
    rightMX--;

    for ( yOffset = -1; yOffset <= 1; yOffset++ ) {
	for ( xOffset = -1; xOffset <= 1; xOffset++ ) {
	    if ( (yOffset == 0) && (xOffset == 0) )
		continue;

	    if ( VALID_MOTION(my+yOffset, mx+xOffset) &&
		 ((diff = LumAddMotionError(currentBlock, block, prev, by, bx,
			 my+yOffset, mx+xOffset, bestDiff)) < bestDiff) ) {
		*motionY = my+yOffset;
		*motionX = mx+xOffset;
		bestDiff = diff;
		changed = TRUE;
	    }
	}
    }

    if ( ! changed ) {
	bestDiff++;
    }

    return(bestDiff);
}


/*===========================================================================*
 *
 * FindBestMatchLogarithmic
 *
 *	tries to find matching motion vector
 *	see FindBestMatch for generic description
 *
 * DESCRIPTION:  uses a logarithmic search
 *
 *===========================================================================*/
static int32
FindBestMatchLogarithmic(block, currentBlock, prev, by, bx, motionY, motionX,
		     bestSoFar, searchRange)
    LumBlock block;
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int32 bestSoFar;
    int searchRange;
{
    register int mx, my;
    int32 diff, bestDiff;
    int	    stepSize;
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;
    int	    tempRightMY, tempRightMX;
    int	    spacing;
    int	    centerX, centerY;
    int	    newCenterX, newCenterY;

    stepSize = (pixelFullSearch ? 2 : 1);

    COMPUTE_MOTION_BOUNDARY(by,bx,stepSize,leftMY,leftMX,rightMY,rightMX);

    bestDiff = 0x7fffffff;

    /* grid spacing */
    if ( stepSize == 2 ) {	/* make sure spacing is even */
	spacing = (searchRange+1)/2;
	if ( (spacing % 2) != 0 ) {
	    spacing++;
	}
    }
    else
	spacing = (searchRange+1)/2;
    centerX = 0;
    centerY = 0;

    while ( spacing >= stepSize ) {
	newCenterY = centerY;
	newCenterX = centerX;

	tempRightMY = rightMY;
	if ( centerY+spacing+1 < tempRightMY ) {
	    tempRightMY = centerY+spacing+1;
	}
	tempRightMX = rightMX;
	if ( centerX+spacing+1 < tempRightMX ) {
	    tempRightMX = centerX+spacing+1;
	}

	for ( my = centerY-spacing; my < tempRightMY; my += spacing ) {
	    if ( my < leftMY ) {
		continue;
	    }

	    for ( mx = centerX-spacing; mx < tempRightMX; mx += spacing ) {
		if ( mx < leftMX ) {
		    continue;
		}

		diff = LumAddMotionError(currentBlock, block, prev, by, bx,
				     my, mx, bestDiff);

		if ( diff < bestDiff ) {
		    newCenterY = my;
		    newCenterX = mx;

		    bestDiff = diff;
		}
	    }
	}

	centerY = newCenterY;
	centerX = newCenterX;

	if ( stepSize == 2 ) {	/* make sure spacing is even */
	    if ( spacing == 2 ) {
		spacing = 0;
	    }
	    else
	    {
		spacing = (spacing+1)/2;
		if ( (spacing % 2) != 0 ) {
		    spacing++;
		}
	    }
	}
	else
	{
	    if ( spacing == 1 ) {
		spacing = 0;
	    }
	    else
		spacing = (spacing+1)/2;
	}
    }

    /* check old motion -- see if it's better */
    if ( (*motionY >= leftMY) && (*motionY < rightMY) &&
	 (*motionX >= leftMX) && (*motionX < rightMX) ) {
	diff = LumAddMotionError(currentBlock, block, prev, by, bx, *motionY, *motionX, bestDiff);
    } else {
	diff = 0x7fffffff;
    }

    if ( bestDiff < diff ) {
	*motionY = centerY;
	*motionX = centerX;
    }
    else
	bestDiff = diff;

    return(bestDiff);
}


/*===========================================================================*
 *
 * FindBestMatchSubSample
 *
 *	tries to find matching motion vector
 *	see FindBestMatch for generic description
 *
 * DESCRIPTION:  should use subsampling method, but too lazy to write all
 *		 the code for it (so instead just calls FindBestMatchExhaust)
 *
 *===========================================================================*/

static int32 FindBestMatchSubSample(LumBlock block, LumBlock currentBlock,
				    MpegFrame *prev, int by, int bx,
				    int *motionY, int *motionX,
				    int32 bestSoFar, int searchRange)
   {int32 rv;

/* too lazy to write the code for this... */

    rv = FindBestMatchExhaust(block, currentBlock, prev,
			      by, bx, motionY, motionX, bestSoFar, searchRange);

    return(rv);}

/*===========================================================================*
 *
 * BMotionSearchNoInterp
 *
 *	finds the best backward and forward motion vectors
 *	if backNeeded == FALSE, then won't find best backward vector if it
 *	is worse than the best forward vector
 *
 * RETURNS:	(*fmy,*fmx) and associated error *forwardErr
 *		(*bmy,*bmx) and associated error *backErr
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
BMotionSearchNoInterp(currentBlock, prev, next, by, bx, fmy, fmx, forwardErr,
		      bmy, bmx, backErr, backNeeded)
    LumBlock currentBlock;
    MpegFrame *prev;
    MpegFrame *next;
    int by;
    int bx;
    int *fmy;
    int *fmx;
    int32 *forwardErr;
    int *bmy;
    int *bmx;
    int32 *backErr;
    boolean backNeeded;
{
    /* CALL SEARCH PROCEDURE */
    switch(psearchAlg) {
	case PSEARCH_SUBSAMPLE:
	    *forwardErr = PSubSampleSearch(currentBlock, prev, by, bx, 
					   fmy, fmx, searchRangeB);
	    *backErr = PSubSampleSearch(currentBlock, next, by, bx, 
					bmy, bmx, searchRangeB);
	    break;
	case PSEARCH_EXHAUSTIVE:
	    *forwardErr = PLocalSearch(currentBlock, prev, by, bx, fmy, fmx, 
				       0x7fffffff, searchRangeB);
	    if ( backNeeded ) {
		*backErr = PLocalSearch(currentBlock, next, by, bx, bmy, bmx, 
					0x7fffffff, searchRangeB);
	    } else {
		*backErr = PLocalSearch(currentBlock, next, by, bx, bmy, bmx, 
					*forwardErr, searchRangeB);
	    }
	    break;
	case PSEARCH_LOGARITHMIC:
	    *forwardErr = PLogarithmicSearch(currentBlock, prev, by, bx, 
					     fmy, fmx, searchRangeB);
	    *backErr = PLogarithmicSearch(currentBlock, next, by, bx, 
					  bmy, bmx, searchRangeB);
	    break;
	case PSEARCH_TWOLEVEL:
	    *forwardErr = PTwoLevelSearch(currentBlock, prev, by, bx, fmy, fmx, 
					  0x7fffffff, searchRangeB);
	    if ( backNeeded ) {
		*backErr = PTwoLevelSearch(currentBlock, next, by, bx, bmy, bmx, 
					   0x7fffffff, searchRangeB);
	    } else {
		*backErr = PTwoLevelSearch(currentBlock, next, by, bx, bmy, bmx, 
					   *forwardErr, searchRangeB);
	    }
	    break;
	default:
	    io_printf(stderr, "ERROR:  Illegal PSEARCH ALG:  %d\n", psearchAlg);
	    exit(1);	
	    break;
    }
}



/*===========================================================================*
 *									     *
 * UNUSED PROCEDURES							     *
 *									     *
 *	The following procedures are all unused by the encoder		     *
 *									     *
 *	They are listed here for your convenience.  You might want to use    *
 *	them if you experiment with different search techniques		     *
 *									     *
 *===========================================================================*/

#ifdef UNUSED_PROCEDURES

/*===========================================================================*
 *
 * ValidBMotion
 *
 *	decides if the given B-frame motion is valid
 *
 * RETURNS:	TRUE if the motion is valid, FALSE otherwise
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
boolean
ValidBMotion(by, bx, mode, fmy, fmx, bmy, bmx)
    int by;
    int bx;
    int mode;
    int fmy;
    int fmx;
    int bmy;
    int bmx;
{
    if ( mode != MOTION_BACKWARD ) {
	/* check forward motion for bounds */
	if ( (by*DCTSIZE+(fmy-1)/2 < 0) || ((by+2)*DCTSIZE+(fmy+1)/2-1 >= Fsize_y) ) {
	    return(FALSE);
	}
	if ( (bx*DCTSIZE+(fmx-1)/2 < 0) || ((bx+2)*DCTSIZE+(fmx+1)/2-1 >= Fsize_x) ) {
	    return(FALSE);
	}
    }

    if ( mode != MOTION_FORWARD ) {
	/* check backward motion for bounds */
	if ( (by*DCTSIZE+(bmy-1)/2 < 0) || ((by+2)*DCTSIZE+(bmy+1)/2-1 >= Fsize_y) ) {
	    return(FALSE);
	}
	if ( (bx*DCTSIZE+(bmx-1)/2 < 0) || ((bx+2)*DCTSIZE+(bmx+1)/2-1 >= Fsize_x) ) {
	    return(FALSE);
	}
    }

    return(TRUE);
}


#endif /* UNUSED_PROCEDURES */
/*===========================================================================*
 * combine.c								     *
 *									     *
 *	Procedures to combine frames or GOPS into an MPEG sequence	     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	GOPStoMPEG							     *
 *	FramesToMPEG							     *
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
 *  Revision 1.9  1995/08/07 21:42:38  smoot
 *  Sleeps when files do not exist.
 *  renamed index to idx
 *
 *  Revision 1.8  1995/06/21 22:20:45  smoot
 *  added a sleep for NFS to complete file writes
 *
 * Revision 1.7  1995/06/08  20:23:19  smoot
 * added "b"'s to fopen so PCs are happy
 *
 * Revision 1.6  1995/01/19  23:07:22  eyhung
 * Changed copyrights
 *
 * Revision 1.5  1995/01/16  07:53:55  eyhung
 * Added realQuiet
 *
 * Revision 1.4  1994/11/12  02:11:46  keving
 * nothing
 *
 * Revision 1.3  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.2  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.1  1993/07/22  22:23:43  keving
 * nothing
 *
 */


/* note, remove() might not have a prototype in the standard header files,
 * but it really should -- it's not my fault!
 */


static int	currentGOP;

#define READ_ATTEMPTS 5 /* number of times (seconds) to retry an input file */

/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static void	AppendFile _ANSI_ARGS_((FILE *outputFile, FILE *inputFile));


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * GOPStoMPEG
 *
 *	convert some number of GOP files into a single MPEG sequence file
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
GOPStoMPEG(numGOPS, outputFileName, outputFilePtr)
    int numGOPS;
    char *outputFileName;
    FILE *outputFilePtr;
{
    register int ind;
    BitBucket *bb;
    char    fileName[1024];
    char    inputFileName[1024];
    FILE *inputFile;
    int q;

    {
      /* Why is this reset called? */
      int x=Fsize_x, y=Fsize_y;
      Fsize_Reset();
      Fsize_Note(0, yuvWidth, yuvHeight);
      if (Fsize_x == 0 || Fsize_y == 0) {
	Fsize_Note(0, x, y);
      }}
    
    bb = Bitio_New(outputFilePtr);

    Mhead_GenSequenceHeader(bb, Fsize_x, Fsize_y, /* pratio */ aspectRatio,
	       /* pict_rate */ frameRate, /* bit_rate */ -1,
	       /* buf_size */ -1, /*c_param_flag */ 1,
	       /* iq_matrix */ customQtable, /* niq_matrix */ customNIQtable,
	       /* ext_data */ NULL, /* ext_data_size */ 0,
	       /* user_data */ NULL, /* user_data_size */ 0);

    /* it's byte-padded, so we can dump it now */
    Bitio_Flush(bb);

    if ( numGOPS > 0 ) {
	for ( ind = 0; ind < numGOPS; ind++ ) {
	    GetNthInputFileName(inputFileName, ind);
	    snprintf(fileName, 1024, "%s/%s", currentGOPPath, inputFileName);

	    for (q = 0;   q < READ_ATTEMPTS;  ++q ) {
	      if ( (inputFile = _PG_fopen(fileName, "rb")) != NULL ) break;
	      io_printf(stderr, "ERROR:  Couldn't read (GOPStoMPEG):  %s retry %d\n", 
		      fileName, q);
	      io_flush(stderr);
	      sleep(1);
	    }
	    if (q == READ_ATTEMPTS) {
	      io_printf(stderr, "Giving up (%d attepmts).\n", READ_ATTEMPTS);
	      exit(1);
	    }
	    
	    if (! realQuiet) {
	    	io_printf(stdout, "appending file:  %s\n", fileName);
	    }

	    AppendFile(outputFilePtr, inputFile);
	}
    } else {
	ind = 0;
	while ( TRUE ) {
	    snprintf(fileName, 1024, "%s.gop.%d", outputFileName, ind);

	    if ( (inputFile = _PG_fopen(fileName, "rb")) == NULL ) {
		break;
	    }

	    if (! realQuiet) {
	    	io_printf(stdout, "appending file:  %s\n", fileName);
	    }

	    AppendFile(outputFilePtr, inputFile);

	    ind++;
	}
    }

    bb = Bitio_New(outputFilePtr);

    /* SEQUENCE END CODE */
    Mhead_GenSequenceEnder(bb);

    Bitio_Flush(bb);

    io_close(outputFilePtr);
}


/*===========================================================================*
 *
 * FramestoMPEG
 *
 *	convert some number of frame files into a single MPEG sequence file
 *
 *	if parallel == TRUE, then when appending a file, blocks until that
 *	file is actually ready
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
FramesToMPEG(numFrames, outputFileName, outputFile, parallel)
    int numFrames;
    char *outputFileName;
    FILE *outputFile;
    boolean parallel;
{
    register int ind;
    BitBucket *bb;
    char    fileName[1024];
    char    inputFileName[1024];
    FILE *inputFile;
    int	pastRefNum = -1;
    int	futureRefNum = -1;
    int q;

    tc_hrs = 0;	tc_min = 0; tc_sec = 0; tc_pict = 0; tc_extra = 0;

    {
      /* Why is this reset called? */
      int x=Fsize_x, y=Fsize_y;
      Fsize_Reset();
      Fsize_Note(0, yuvWidth, yuvHeight);
      if (Fsize_x == 0 || Fsize_y == 0) {
	Fsize_Note(0, x, y);
      }}
    SetBlocksPerSlice();

    bb = Bitio_New(outputFile);
    Mhead_GenSequenceHeader(bb, Fsize_x, Fsize_y, /* pratio */ aspectRatio,
	       /* pict_rate */ frameRate, /* bit_rate */ -1,
	       /* buf_size */ -1, /*c_param_flag */ 1,
	       /* iq_matrix */ qtable, /* niq_matrix */ niqtable,
	       /* ext_data */ NULL, /* ext_data_size */ 0,
	       /* user_data */ NULL, /* user_data_size */ 0);
    /* it's byte-padded, so we can dump it now */
    Bitio_Flush(bb);

    /* need to do these in the right order!!! */
    /* also need to add GOP headers */

    currentGOP = gopSize;
    totalFramesSent = 0;

    if ( numFrames > 0 ) {
	for ( ind = 0; ind < numFrames; ind++ ) {
	    if ( FRAME_TYPE(ind) == 'b' ) {
		continue;
	    }

	    pastRefNum = futureRefNum;
	    futureRefNum = ind;

	    if ( (FRAME_TYPE(ind) == 'i') && (currentGOP >= gopSize) ) {
		int closed;

		/* first, check to see if closed GOP */
		if ( totalFramesSent == ind ) {
		    closed = 1;
		} else {
		    closed = 0;
		}

		if (! realQuiet) {
			io_printf(stdout, "Creating new GOP (closed = %d) after %d frames\n",
				closed, currentGOP);
		}

		/* new GOP */
		bb = Bitio_New(outputFile);
		Mhead_GenGOPHeader(bb, /* drop_frame_flag */ 0,
			   tc_hrs, tc_min, tc_sec, tc_pict,
			   closed, /* broken_link */ 0,
			   /* ext_data */ NULL, /* ext_data_size */ 0,
			   /* user_data */ NULL, /* user_data_size */ 0);
		Bitio_Flush(bb);
		SetGOPStartTime(ind);
		
		currentGOP -= gopSize;
	    }

	    if ( parallel ) {
		WaitForOutputFile(ind);
		snprintf(fileName, 1024, "%s.frame.%d", outputFileName, ind);
	    } else {
		GetNthInputFileName(inputFileName, ind);
		snprintf(fileName, 1024, "%s/%s", currentFramePath, inputFileName);
	    }

	    for (q = 0;   q < READ_ATTEMPTS;  ++q ) {
	      if ( (inputFile = _PG_fopen(fileName, "rb")) != NULL ) break;
	      io_printf(stderr, "ERROR:  Couldn't read 2:  %s retry %d\n", fileName, q);
	      io_flush(stderr);
	      sleep(1);
	    }
	    if (q == READ_ATTEMPTS) {
	      io_printf(stderr, "Giving up (%d attepmts).\n", READ_ATTEMPTS);
	      exit(1);
	    }
	    
	    AppendFile(outputFile, inputFile);
	    if ( parallel ) {
		remove(fileName);
	    }

	    currentGOP++;
	    IncrementTCTime();

	    /* now, output the B-frames */
	    if ( pastRefNum != -1 ) {
		register int bNum;

		for ( bNum = pastRefNum+1; bNum < futureRefNum; bNum++ ) {
		    if ( parallel ) {
			WaitForOutputFile(bNum);
			snprintf(fileName, 1024, "%s.frame.%d", outputFileName, bNum);
		    } else {
			GetNthInputFileName(inputFileName, bNum);
			snprintf(fileName, 1024, "%s/%s", currentFramePath, inputFileName);
		    }


		    for (q = 0;   q < READ_ATTEMPTS;  ++q ) {
		      if ( (inputFile = _PG_fopen(fileName, "rb")) != NULL ) break;
		      io_printf(stderr, "ERROR:  Couldn't read (bNum=%d):  %s retry %d\n", 
			      bNum, fileName, q);
		      io_flush(stderr);
		      sleep(1);
		    }
		    if (q == READ_ATTEMPTS) {
		      io_printf(stderr, "Giving up (%d attepmts).\n", READ_ATTEMPTS);
		      exit(1);
		    }
		    
		    AppendFile(outputFile, inputFile);
		    if ( parallel ) {
			remove(fileName);
		    }
			
		    currentGOP++;
		    IncrementTCTime();
		}
	    }
	}
    } else {
	if ( parallel ) {
	    io_printf(stderr, "ERROR:  PARALLEL COMBINE WITH 0 FRAMES\n");
	    io_printf(stderr, "(please send bug report!)\n");
	    exit(1);
	}

	ind = 0;
	while ( TRUE ) {
	    if ( FRAME_TYPE(ind) == 'b' ) {
		ind++;
		continue;
	    }

	    if ( (FRAME_TYPE(ind) == 'i') && (currentGOP >= gopSize) ) {
		int closed;

		/* first, check to see if closed GOP */
		if ( totalFramesSent == ind ) {
		    closed = 1;
		} else {
		    closed = 0;
		}

		if (! realQuiet) {
		io_printf(stdout, "Creating new GOP (closed = %d) before frame %d\n",
			closed, ind);
		}

		/* new GOP */
		bb = Bitio_New(outputFile);
		Mhead_GenGOPHeader(bb, /* drop_frame_flag */ 0,
			   tc_hrs, tc_min, tc_sec, tc_pict,
			   closed, /* broken_link */ 0,
			   /* ext_data */ NULL, /* ext_data_size */ 0,
			   /* user_data */ NULL, /* user_data_size */ 0);
		Bitio_Flush(bb);
		SetGOPStartTime(ind);

		currentGOP -= gopSize;
	    }

	    snprintf(fileName, 1024, "%s.frame.%d", outputFileName, ind);

	    if ( (inputFile = _PG_fopen(fileName, "rb")) == NULL ) {
		break;
	    }

	    AppendFile(outputFile, inputFile);
	    if ( parallel ) {
		remove(fileName);
	    }

	    currentGOP++;
	    IncrementTCTime();

	    /* now, output the B-frames */
	    if ( pastRefNum != -1 ) {
		register int bNum;

		for ( bNum = pastRefNum+1; bNum < futureRefNum; bNum++ ) {
		    snprintf(fileName, 1024, "%s.frame.%d", outputFileName, bNum);

		    for (q = 0;   q < READ_ATTEMPTS;  ++q ) {
		      if ( (inputFile = _PG_fopen(fileName, "rb")) != NULL ) break;
		      io_printf(stderr, "ERROR:  Couldn't read (FramestoMPEG):  %s retry %d\n", 
			      fileName, q);
		      io_flush(stderr);
		      sleep(1);
		    }
		    if (q == READ_ATTEMPTS) {
		      io_printf(stderr, "Giving up (%d attepmts).\n", READ_ATTEMPTS);
		      exit(1);
		    }

		    AppendFile(outputFile, inputFile);
		    if ( parallel ) {
			remove(fileName);
		    }

		    currentGOP++;
		    IncrementTCTime();
		}
	    }

	    ind++;
	}
    }

    if (! realQuiet) {
	io_printf(stdout, "Wrote %d frames\n", totalFramesSent);
	io_flush(stdout);
    }

    bb = Bitio_New(outputFile);

    /* SEQUENCE END CODE */
    Mhead_GenSequenceEnder(bb);

    Bitio_Flush(bb);

    CFREE(bb);

    io_close(outputFile);
}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * AppendFile
 *
 *	appends the output file with the contents of the given input file
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
AppendFile(outputFile, inputFile)
    FILE *outputFile;
    FILE *inputFile;
{
    u_int8_t   data[9999];
    int	    readItems;

    readItems = 9999;
    while ( readItems == 9999 ) {
	readItems = io_read(data, sizeof(u_int8_t), 9999, inputFile);
	if ( readItems > 0 ) {
	    io_write(data, sizeof(u_int8_t), readItems, outputFile);
	}
    }

    io_close(inputFile);
}


/*===========================================================================*
 * frame.c								     *
 *									     *
 *	basic frame procedures						     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	Frame_Init							     *
 *	Frame_Exit							     *
 *	Frame_New							     *
 *	Frame_Free							     *
 *	Frame_AllocPPM							     *
 *	Frame_AllocBlocks						     *
 *	Frame_AllocYCC							     *
 *	Frame_AllocDecoded						     *
 *	Frame_AllocHalf						             *
 *	Frame_Resize						             * 
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


/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static void FreeFrame _ANSI_ARGS_((MpegFrame * mf));
static MpegFrame *GetUnusedFrame _ANSI_ARGS_((MpegFrame **frM));
/*static void GetNumOfFrames _ANSI_ARGS_((int *numOfFrames));*/
static void ResetFrame _ANSI_ARGS_((int fnumber, int type, MpegFrame *frame));
static void Resize_Width _ANSI_ARGS_((MpegFrame *omfrw,MpegFrame *mfrw, int in_x,
       int in_y, int out_x));
static void Resize_Height _ANSI_ARGS_((MpegFrame *omfrh,MpegFrame *mfrh,
       int in_x,
       int in_y,  int out_y));
static void Resize_Array_Width _ANSI_ARGS_((u_int8_t **inarray,int in_x,
       int in_y,u_int8_t **outarray, int out_x));
static void Resize_Array_Height _ANSI_ARGS_((u_int8_t **inarray,int in_x,
       int in_y,u_int8_t **outarray, int out_y));


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===============================================================
 *
 * Frame_Resize                  by James Boucher
 *                Boston University Multimedia Communications Lab
 *  
 *     This function takes the mf input frame, read in READFrame(),
 * and resizes all the input component arrays to the output
 * dimensions specified in the parameter file as OUT_SIZE.
 * The new frame is returned with the omf pointer.  As well,
 * the values of Fsize_x and Fsize_y are adjusted.
 ***************************************************************/
void Frame_Resize(MpegFrame *omf, MpegFrame *mf,
		  int insize_x, int insize_y,
		  int outsize_x, int outsize_y)
   {MpegFrame *frameA;  /* intermediate frame */
  
    frameA = CMAKE(MpegFrame);
    if (frameA == NULL)
       return;
  
    if ((insize_x != outsize_x) && (insize_y != outsize_y))
       {Resize_Width(frameA,mf,insize_x,insize_y,outsize_x);
	Resize_Height(omf,frameA,outsize_x,insize_y,outsize_y);}

    else if ((insize_x == outsize_x) && (insize_y != outsize_y))
       Resize_Height(omf,mf,insize_x,insize_y,outsize_y);

    else if ((insize_x != outsize_x) && (insize_y == outsize_y))
       Resize_Width(omf,mf,insize_x,insize_y,outsize_x);

/* Free memory */
    CFREE(frameA);
    CFREE(mf);

    return;}

/*========================================================
 * Resize_Width
 *======================================================*/
static void  
  Resize_Width(omfrw,mfrw,in_x,in_y, out_x)
MpegFrame *omfrw,*mfrw;
int in_x,in_y, out_x;
{
  register int y;
  int i;
  
  omfrw->orig_y = NULL;
  Fsize_x = out_x;
  /* Allocate new frame memory */
  omfrw->orig_y = CMAKE_N(u_int8_t *, Fsize_y);
  ERRCHK(omfrw->orig_y, "malloc");
  for (y = 0; y < Fsize_y; y++) {
    omfrw->orig_y[y] = CMAKE_N(u_int8_t, out_x);
    ERRCHK(omfrw->orig_y[y], "malloc");
  }
  
  omfrw->orig_cr = CMAKE_N(u_int8_t *, Fsize_y/2);
  ERRCHK(omfrw->orig_cr, "malloc");
  for (y = 0; y < Fsize_y / 2; y++) {
    omfrw->orig_cr[y] = CMAKE_N(u_int8_t, out_x/2);
    ERRCHK(omfrw->orig_cr[y], "malloc");
  }
  
  omfrw->orig_cb = CMAKE_N(u_int8_t *, Fsize_y/2);
  ERRCHK(omfrw->orig_cb, "malloc");
  for (y = 0; y < Fsize_y / 2; y++) {
    omfrw->orig_cb[y] = CMAKE_N(u_int8_t, out_x/2);
    ERRCHK(omfrw->orig_cb[y], "malloc");
  }
  
  if ( referenceFrame == ORIGINAL_FRAME ) {
    omfrw->ref_y = omfrw->orig_y;
    omfrw->ref_cr = omfrw->orig_cr;
    omfrw->ref_cb = omfrw->orig_cb;
  }
  
  /* resize each component array separately */
  Resize_Array_Width(mfrw->orig_y,in_x,in_y,omfrw->orig_y,out_x);
  Resize_Array_Width(mfrw->orig_cr,(in_x/2),(in_y/2),omfrw->orig_cr,(out_x/2));
  Resize_Array_Width(mfrw->orig_cb,(in_x/2),(in_y/2),omfrw->orig_cb,(out_x/2));
  
  /* Free old frame memory */
  if (mfrw->orig_y) {
    for (i = 0; i < in_y; i++) {
      CFREE(mfrw->orig_y[i]);
    }
    CFREE(mfrw->orig_y);
    
    for (i = 0; i < in_y / 2; i++) {
      CFREE(mfrw->orig_cr[i]);
    }
    CFREE(mfrw->orig_cr);
    
    for (i = 0; i < in_y / 2; i++) {
      CFREE(mfrw->orig_cb[i]);
    }
    CFREE(mfrw->orig_cb);
  }
  
}

/*=======================================================
* Resize_Height
*
*   Resize Frame height up or down
*=======================================================*/
static  void
Resize_Height(omfrh,mfrh,in_x,in_y,out_y)
MpegFrame *omfrh,*mfrh;
int in_x,in_y, out_y;
{
register int y; 
int i;

Fsize_y = out_y;

/* Allocate new frame memory */
    omfrh->orig_y = CMAKE_N(u_int8_t *, out_y);
    ERRCHK(omfrh->orig_y, "malloc");
    for (y = 0; y < out_y; y++) {
	omfrh->orig_y[y] = CMAKE_N(u_int8_t, Fsize_x);
	ERRCHK(omfrh->orig_y[y], "malloc");
    }

    omfrh->orig_cr = CMAKE_N(u_int8_t *, out_y/2);
    ERRCHK(omfrh->orig_cr, "malloc");
    for (y = 0; y < out_y / 2; y++) {
	omfrh->orig_cr[y] = CMAKE_N(u_int8_t, Fsize_x/2);
	ERRCHK(omfrh->orig_cr[y], "malloc");
    }

    omfrh->orig_cb = CMAKE_N(u_int8_t *, out_y/2);
    ERRCHK(omfrh->orig_cb, "malloc");
    for (y = 0; y < out_y / 2; y++) {
	omfrh->orig_cb[y] = CMAKE_N(u_int8_t, Fsize_x/2);
	ERRCHK(omfrh->orig_cb[y], "malloc");
    }

    if ( referenceFrame == ORIGINAL_FRAME ) {
	omfrh->ref_y = omfrh->orig_y;
	omfrh->ref_cr = omfrh->orig_cr;
	omfrh->ref_cb = omfrh->orig_cb;
    }

/* resize component arrays separately */
Resize_Array_Height(mfrh->orig_y,in_x,in_y,omfrh->orig_y,out_y);
Resize_Array_Height(mfrh->orig_cr,(in_x/2),(in_y/2),omfrh->orig_cr,(out_y/2));
Resize_Array_Height(mfrh->orig_cb,(in_x/2),(in_y/2),omfrh->orig_cb,(out_y/2));

/* Free old frame memory */
    if (mfrh->orig_y) {
	for (i = 0; i < in_y; i++) {
	    CFREE(mfrh->orig_y[i]);
	}
	CFREE(mfrh->orig_y);

	for (i = 0; i < in_y / 2; i++) {
	    CFREE(mfrh->orig_cr[i]);
	}
	CFREE(mfrh->orig_cr);

	for (i = 0; i < in_y / 2; i++) {
	    CFREE(mfrh->orig_cb[i]);
	}
	CFREE(mfrh->orig_cb);
    }

}
/*====================================================
* Resize_Array_Width
*    
*   This function will resize any array width up
* or down in size.  The algorithm is based on the
* least common multiple approach more commonly
* used in audio frequency adjustments.
*=====================================================*/
static void 
Resize_Array_Width(inarray,in_x,in_y,outarray,out_x)
u_int8_t **inarray;
int in_x;
int in_y;
u_int8_t **outarray;
int out_x;
{
int i,j; 
int in_total;
int out_total;
u_int8_t *inptr;
u_int8_t *outptr;
/* double slope,diff; */

 for(i=0;i<in_y;i++){     /* For every row */
  inptr = &inarray[i][0];
  outptr = &outarray[i][0];
  in_total = 0;
  out_total = 0;
  for(j=0;j<out_x;j++){      /* For every output value */
    if(in_total == out_total){  
      *outptr = *inptr;
      outptr++;
      out_total=out_total+in_x;
      while(in_total < out_total){
	in_total = in_total + out_x;
	inptr++;
      }
      if(in_total > out_total){
	in_total = in_total - out_x;
	inptr--;
      }
    } else {  
      inptr++;
      inptr--;
/* Non-Interpolative solution */
    *outptr = *inptr;  

      outptr++;
      out_total=out_total+in_x;
      while(in_total < out_total){
	in_total = in_total + out_x;
	inptr++;
      }
      if(in_total > out_total){
	in_total = in_total - out_x;
	inptr--;
      }
    }  /* end if */
  }  /* end for each output value */

 }  /* end for each row */
}  /* end main */
/*==============================
* Resize_Array_Height
*
*    Resize any array height larger or smaller.
* Same as Resize_array_Width except pointer
* manipulation must change.
*===============================*/
static void 
Resize_Array_Height(inarray,in_x,in_y,outarray,out_y)
u_int8_t **inarray;
int in_x;
int in_y;
u_int8_t **outarray;
int out_y;
{
int i,j,k; 
int in_total;
int out_total;

 for(i=0;i<in_x;i++){    /* for each column */
  in_total = 0;
  out_total = 0;
  k = 0;
  for(j=0;j<out_y;j++){  /* for each output value */
    if(in_total == out_total){  
      outarray[j][i] = inarray[k][i];
      out_total=out_total+in_y;
      while(in_total < out_total){
	in_total = in_total + out_y;
	k++;
      }
      if(in_total > out_total){
	in_total = in_total - out_y;
	k--;
      }
    } else {  
 
#if 0
      u_int8_t pointA, pointB;

      pointA = inarray[k][i];
      if (k != (in_y -1))
         pointB = inarray[k+1][i];
      else
         pointB = pointA;
#endif

/* Non-Interpolative case */
    outarray[j][i] = inarray[k][i];
      out_total=out_total+in_y;
      while(in_total < out_total){
	in_total = in_total + out_y;
	k++;
      }
      if(in_total > out_total){
	in_total = in_total - out_y;
	k--;
      }
    } 
  }
 }

}



/*===========================================================================*
 *
 * Frame_Init
 *
 *	initializes the memory associated with all frames ever
 *	If the input is not coming in from stdin, only 3 frames are needed ;
 *      else, the program must create frames equal to the greatest distance
 *      between two reference frames to hold the B frames while it is parsing
 *      the input from stdin.
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    frameMemory
 *
 *===========================================================================*/

void Frame_Init(MpegFrame **frameMemory)
   {register int idx;

    for (idx = 0; idx < 3; idx++)
        {frameMemory[idx] = CMAKE(MpegFrame);

	 frameMemory[idx]->inUse     = FALSE;
	 frameMemory[idx]->ppm_data  = NULL;
	 frameMemory[idx]->rgb_data  = NULL;
	 frameMemory[idx]->orig_y    = NULL;   /* if NULL, then orig_cr, orig_cb invalid */
	 frameMemory[idx]->y_blocks  = NULL;   /* if NULL, then cr_blocks, cb_blocks invalid */
	 frameMemory[idx]->decoded_y = NULL;   /* if NULL, then blah blah */
	 frameMemory[idx]->halfX     = NULL;
	 frameMemory[idx]->next      = NULL;};

#ifdef BLEAH
    io_printf(stderr, "%d frames allocated.\n", numOfFrames);
#endif

    return;}

/*===========================================================================*
 *
 * Frame_Exit
 *
 *	frees the memory associated with frames
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    frameMemory
 *
 *===========================================================================*/

void Frame_Exit(MpegFrame **frameMemory)
   {register int idx;

    for (idx = 0; idx < 3; idx++)
	FreeFrame(frameMemory[idx]);

    return;}

/*===========================================================================*
 *
 * Frame_Free
 *
 *	frees the given frame -- allows it to be re-used
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

void Frame_Free(MpegFrame *frame)
   {

    frame->inUse = FALSE;

    return;}

/*===========================================================================*
 *
 * Frame_New
 *
 *	finds a frame that isn't currently being used and resets it
 *
 * RETURNS:	the frame
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

MpegFrame *Frame_New(int id, int type, MpegFrame **frameMemory)
   {MpegFrame *frame;

    frame = GetUnusedFrame(frameMemory);
    if (frame != NULL)
       ResetFrame(id, type, frame);

    return(frame);}


/*===========================================================================*
 *
 * Frame_AllocPPM
 *
 *	allocate memory for ppm data for the given frame, if required
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

void Frame_AllocPPM(MpegFrame *frame)
   {register int y;

/* already allocated */
    if (frame->ppm_data != NULL)
       return;

    frame->ppm_data = CMAKE_N(u_int8_t *, Fsize_y);
    ERRCHK(frame->ppm_data, "malloc");

    for (y = 0; y < Fsize_y; y++)
        {frame->ppm_data[y] = CMAKE_N(u_int8_t, 3*Fsize_x);
	 ERRCHK(frame->ppm_data[y], "malloc");};

    return;}

/*===========================================================================*
 *
 * Frame_AllocBlocks
 *
 *	allocate memory for blocks for the given frame, if required
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

void Frame_AllocBlocks(MpegFrame *frame)
   {int dctx, dcty;
    int i;

/* already allocated */
    if (frame->y_blocks != NULL)
       return;

    dctx = Fsize_x / DCTSIZE;
    dcty = Fsize_y / DCTSIZE;

    frame->y_blocks = CMAKE_N(Block *, dcty);
    ERRCHK(frame->y_blocks, "malloc");

    for (i = 0; i < dcty; i++)
        {frame->y_blocks[i] = CMAKE_N(Block, dctx);
	 ERRCHK(frame->y_blocks[i], "malloc");};

    frame->cr_blocks = CMAKE_N(Block *, (dcty >> 1));
    frame->cb_blocks = CMAKE_N(Block *, (dcty >> 1));
    ERRCHK(frame->cr_blocks, "malloc");
    ERRCHK(frame->cb_blocks, "malloc");

    for (i = 0; i < (dcty >> 1); i++)
        {frame->cr_blocks[i] = CMAKE_N(Block, (dctx >> 1));
	 frame->cb_blocks[i] = CMAKE_N(Block, (dctx >> 1));
	 ERRCHK(frame->cr_blocks[i], "malloc");
	 ERRCHK(frame->cb_blocks[i], "malloc");};

    return;}

/*===========================================================================*
 *
 * Frame_AllocYCC
 *
 *	allocate memory for YCC info for the given frame, if required
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

void Frame_AllocYCC(MpegFrame *frame)
   {register int y;

/* already allocated */
    if (frame->orig_y != NULL)
       return;

    DBG_PRINT(("ycc_calc:\n"));

/* first, allocate tons of memory */
    frame->orig_y = CMAKE_N(u_int8_t *, Fsize_y);
    ERRCHK(frame->orig_y, "malloc");

    for (y = 0; y < Fsize_y; y++)
        {frame->orig_y[y] = CMAKE_N(u_int8_t, Fsize_x);
	 ERRCHK(frame->orig_y[y], "malloc");};

    frame->orig_cr = CMAKE_N(u_int8_t *, (Fsize_y >> 1));
    ERRCHK(frame->orig_cr, "malloc");

    for (y = 0; y < (Fsize_y >> 1); y++)
        {frame->orig_cr[y] = CMAKE_N(u_int8_t, (Fsize_x >> 1));
	 ERRCHK(frame->orig_cr[y], "malloc");};

    frame->orig_cb = CMAKE_N(u_int8_t *, (Fsize_y >> 1));
    ERRCHK(frame->orig_cb, "malloc");

    for (y = 0; y < (Fsize_y >> 1); y++)
        {frame->orig_cb[y] = CMAKE_N(u_int8_t, (Fsize_x >> 1));
	 ERRCHK(frame->orig_cb[y], "malloc");};

    if (referenceFrame == ORIGINAL_FRAME)
       {frame->ref_y  = frame->orig_y;
	frame->ref_cr = frame->orig_cr;
	frame->ref_cb = frame->orig_cb;};

    return;}

/*===========================================================================*
 *
 * Frame_AllocHalf
 *
 *	allocate memory for half-pixel values for the given frame, if required
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

void Frame_AllocHalf(MpegFrame *frame)
   {register int y;

    if (frame->halfX != NULL)
       return;

    frame->halfX = CMAKE_N(u_int8_t *, Fsize_y);
    ERRCHK(frame->halfX, "malloc");

    frame->halfY = CMAKE_N(u_int8_t *, Fsize_y-1);
    ERRCHK(frame->halfY, "malloc");

    frame->halfBoth = CMAKE_N(u_int8_t *, Fsize_y - 1);
    ERRCHK(frame->halfBoth, "malloc");

    for (y = 0; y < Fsize_y; y++)
        {frame->halfX[y] = CMAKE_N(u_int8_t, Fsize_x - 1);
	 ERRCHK(frame->halfX[y], "malloc");};

    for (y = 0; y < Fsize_y-1; y++)
        {frame->halfY[y] = CMAKE_N(u_int8_t, Fsize_x);
	 ERRCHK(frame->halfY[y], "malloc");};

    for (y = 0; y < Fsize_y-1; y++)
        {frame->halfBoth[y] = CMAKE_N(u_int8_t, Fsize_x - 1);
	 ERRCHK(frame->halfBoth[y], "malloc");};

    return;}

/*===========================================================================*
 *
 * Frame_AllocDecoded
 *
 *	allocate memory for decoded frame for the given frame, if required
 *	if makeReference == TRUE, then makes it reference frame
 * 
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

void Frame_AllocDecoded(MpegFrame *frame, boolean makeReference)
   {register int y;

/* already allocated */
    if (frame->decoded_y != NULL)
       return;

/* allocate memory for decoded image
 * can probably reuse original image memory, but may decide to use
 * it for some reason, so do it this way at least for now -- more
 * flexible
 */
    frame->decoded_y = CMAKE_N(u_int8_t *, Fsize_y);
    ERRCHK(frame->decoded_y, "malloc");

    for (y = 0; y < Fsize_y; y++)
        {frame->decoded_y[y] = CMAKE_N(u_int8_t, Fsize_x);
	 ERRCHK(frame->decoded_y[y], "malloc");};

    frame->decoded_cr = CMAKE_N(u_int8_t *, (Fsize_y >> 1));
    ERRCHK(frame->decoded_cr, "malloc");

    for (y = 0; y < (Fsize_y >> 1); y++)
        {frame->decoded_cr[y] = CMAKE_N(u_int8_t, (Fsize_x >> 1));
	 ERRCHK(frame->decoded_cr[y], "malloc");};

    frame->decoded_cb = CMAKE_N(u_int8_t *, (Fsize_y >> 1));
    ERRCHK(frame->decoded_cb, "malloc");

    for (y = 0; y < (Fsize_y >> 1); y++)
        {frame->decoded_cb[y] = CMAKE_N(u_int8_t, (Fsize_x >> 1));
	 ERRCHK(frame->decoded_cb[y], "malloc");};

    if (makeReference)
       {frame->ref_y  = frame->decoded_y;
	frame->ref_cr = frame->decoded_cr;
	frame->ref_cb = frame->decoded_cb;};

    return;}

/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * GetUnusedFrame
 *
 *	return an unused frame
 *
 * RETURNS:	the frame
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

static MpegFrame *GetUnusedFrame(MpegFrame **frameMemory)
   {register int idx;

    for (idx = 0; idx < 3; idx++)
        {if (!frameMemory[idx]->inUse)
	    {frameMemory[idx]->inUse = TRUE;
	     return(frameMemory[idx]);};};

    io_printf(stderr, "ERROR:  No unused frames!!!\n");
    io_printf(stderr, "        If you are using stdin for input, it is likely that you have too many\n");
    io_printf(stderr, "        B-frames between two reference frames.  See the man page for help.\n");

    return(NULL);}

/*===========================================================================*
 *
 * ResetFrame
 *
 *	reset a frame to the given id and type
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

static void ResetFrame(int id, int type, MpegFrame *frame)
   {

    switch (type)
       {case 'i':
	     frame->type = TYPE_IFRAME;
	     break;
	case 'p':
	     frame->type = TYPE_PFRAME;
	     break;
	case 'b':
	     frame->type = TYPE_BFRAME;
	     break;
	default:
	     io_printf(stderr, "frame type %c: not supported\n", type);
	     exit(1);};

    frame->id           = id;
    frame->halfComputed = FALSE;
    frame->next         = NULL;

    return;}

/*===========================================================================*
 *
 * FreeFrame
 *
 *	frees the memory associated with the given frame
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

static void FreeFrame(MpegFrame *frame)
   {int i;

    if (frame == NULL)
       return;

    if (frame->ppm_data != NULL)

/* it may be a little bigger than Fsize_y, but that's fine for
 * our purposes, since we aren't going to free until we exit anyway,
 * so by the time we call this we won't care
 */
       {pnm_freearray(frame->ppm_data, Fsize_y);
	frame->ppm_data = NULL;};

    if (frame->rgb_data != NULL)
       pnm_freearray(frame->rgb_data, Fsize_y);

    if (frame->orig_y != NULL)
       {for (i = 0; i < Fsize_y; i++)
	    {CFREE(frame->orig_y[i]);};

	CFREE(frame->orig_y);

	for (i = 0; i < (Fsize_y >> 1); i++)
	    {CFREE(frame->orig_cr[i]);}

	CFREE(frame->orig_cr);

	for (i = 0; i < (Fsize_y >> 1); i++)
	    {CFREE(frame->orig_cb[i]);};

	CFREE(frame->orig_cb);};

    if (frame->decoded_y != NULL)
       {for (i = 0; i < Fsize_y; i++)
	    {CFREE(frame->decoded_y[i]);};

	CFREE(frame->decoded_y);

	for (i = 0; i < (Fsize_y >> 1); i++)
	    {CFREE(frame->decoded_cr[i]);};

	CFREE(frame->decoded_cr);

	for (i = 0; i < (Fsize_y >> 1); i++)
	    {CFREE(frame->decoded_cb[i]);};

	CFREE(frame->decoded_cb);};

    if (frame->y_blocks != NULL)
       {for (i = 0; i < Fsize_y / DCTSIZE; i++)
	    {CFREE(frame->y_blocks[i]);};

	CFREE(frame->y_blocks);

	for (i = 0; i < Fsize_y / (2 * DCTSIZE); i++)
	    {CFREE(frame->cr_blocks[i]);};

	CFREE(frame->cr_blocks);

	for (i = 0; i < Fsize_y / (2 * DCTSIZE); i++)
	    {CFREE(frame->cb_blocks[i]);};

	CFREE(frame->cb_blocks);};

    if (frame->halfX != NULL)
       {for (i = 0; i < Fsize_y; i++)
	    {CFREE(frame->halfX[i]);};

	CFREE(frame->halfX);

	for (i = 0; i < Fsize_y-1; i++)
	    {CFREE(frame->halfY[i]);};

	CFREE(frame->halfY);

	for (i = 0; i < Fsize_y-1; i++)
	    {CFREE(frame->halfBoth[i]);};

	CFREE(frame->halfBoth);};
        
    CFREE(frame);

    return;}

/*===========================================================================*
 * frametype.c								     *
 *									     *
 *	procedures to keep track of frame types (I, P, B)		     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	FType_Type						             *
 *	FType_FutureRef						             *
 *	FType_PastRef						             *
 *									     *
 * SYNOPSIS								     *
 *	FType_Type	returns the type of the given numbered frame	     *
 *	FType_FutureRef	returns the number of the future reference frame     *
 *	FType_PastRef	returns the number of the past reference frame	     *
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


static FrameTable *frameTable=NULL;
static boolean use_cache = FALSE;
static int firstI = 0;

/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * FType_Type
 *
 *	returns the type of the given numbered frame
 *
 * RETURNS:	the type
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
FType_Type(frameNum)
    int frameNum;
{
  if (use_cache)
     return((int)frameTable[frameNum].typ);
  
  if ( forceEncodeLast && (frameNum+1 == numInputFiles) ) {
    int result;
    
    result = framePattern[frameNum % framePatternLen];
    if ( result == 'b' )
       return('i');
    else return(result);
  } else {
    if (specificsOn) {
      static int lastI = -1;
      int newtype;
      
      if (lastI > frameNum) lastI = -1;
      newtype = SpecTypeLookup(frameNum);
      switch (newtype) {
      case 1:
	lastI = frameNum;
	return('i');
      case 2:
	return('p');
      case 3:
	return('b');
      default:
	if (lastI != -1)
	   return(framePattern[(frameNum-lastI+firstI) % framePatternLen]);
	else
	   return(framePattern[frameNum % framePatternLen]);
      }
    } else return(framePattern[frameNum % framePatternLen]);
  }
}


/*===========================================================================*
 *
 * FType_FutureRef
 *
 *	returns the number of the future reference frame
 *
 * RETURNS:	the number; -1 if none
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
FType_FutureRef(currFrameNum)
    int currFrameNum;
{
    int	    index;
    int	    futureIndex;
    int	    result;

    if (use_cache) {
      return(frameTable[currFrameNum].next->number);
    } else {
      index = currFrameNum % framePatternLen;
      futureIndex = frameTable[index].next->number;
      
      result = currFrameNum +
	(((futureIndex-index)+framePatternLen) % framePatternLen);
      
      if ( (result >= numInputFiles) && forceEncodeLast ) {
	return(numInputFiles-1);
      } else {
	return(result);
      }
    }
}


/*===========================================================================*
 *
 * FType_PastRef
 *
 *	returns the number of the past reference frame
 *
 * RETURNS:	the number
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
FType_PastRef(currFrameNum)
    int currFrameNum;
{
    int	    index;
    int	    pastIndex;

    if (use_cache) {
      return(frameTable[currFrameNum].prev->number);
    } else {
      index = currFrameNum % framePatternLen;
      pastIndex = frameTable[index].prev->number;
      
      return(currFrameNum -
	(((index-pastIndex)+framePatternLen) % framePatternLen));
    }
}


/*===========================================================================*
 *
 * SetFramePattern
 *
 *	set the IPB pattern; calls ComputeFrameTable to set up table
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    framePattern, framePatternLen, frameTable
 *
 *===========================================================================*/
#define SIMPLE_ASCII_UPPER(x)  (((x)>='a') ? ((x)-'a'+'A') : (x))
void
SetFramePattern(pattern)
    char *pattern;
{
    int index, len;
    char *buf;

    if ( ! pattern ) {
	io_printf(stderr, "pattern cannot be NULL\n");
	exit(1);
    }

    len = strlen(pattern);

    if ( SIMPLE_ASCII_UPPER(pattern[0]) != 'I' ) {
      for (index=0; index < len; index++) {

	if (SIMPLE_ASCII_UPPER(pattern[index]) == 'I') {
	  break;
	} else if (SIMPLE_ASCII_UPPER(pattern[index]) == 'P') {
	  io_printf(stderr, "first reference frame must be 'i'\n");
	  exit(1);
	}
      }
    }

    buf = CMAKE_N(char, len + 1);
    ERRCHK(buf, "malloc");

    firstI = -1;
    for ( index = 0; index < len; index++ ) {
      switch( SIMPLE_ASCII_UPPER(pattern[index]) ) {
      case 'I':	
	buf[index] = 'i';
	if (firstI == -1) firstI = index;
	break;
      case 'P':	
	buf[index] = 'p'; 
	break;
      case 'B':	
	buf[index] = 'b';
	break;
      default:
	io_printf(stderr, "Frame type '%c' not supported.\n", pattern[index]);
	exit(1);
      }
    }
    buf[len] = 0;

    if (firstI == -1) {
      io_printf(stderr, "Must have an I-frame in PATTERN\n");
      exit(1);
    }

    framePattern = buf;
    framePatternLen = len;

    /* Used to ComputeFrameTable(), but now must wait until param parsed. (STDIN or not)*/
}


/*===========================================================================*
 *
 * ComputeFrameTable
 *
 *	compute a table of I, P, B frames to help in determining dependencies
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    frameTable
 *
 *===========================================================================*/
void
ComputeFrameTable()
{
    register int index;
    FrameTable *lastIP, *firstB, *secondIP;
    FrameTable *ptr;
    char typ;
    int table_size;

    if (!stdinUsed) {
      table_size = numInputFiles;
    } else {
      table_size = framePatternLen;
    }

    frameTable = CMAKE_N(FrameTable, 1 + table_size);
    ERRCHK(frameTable, "malloc");

    lastIP = NULL;
    firstB = NULL;
    secondIP = NULL;
    for ( index = 0; index < table_size; index++ ) {
	frameTable[index].number = index;
	typ = FType_Type(index);
	frameTable[index].typ = typ;
	switch( typ ) {
	    case 'i':
		ptr = firstB;
		while ( ptr != NULL ) {
		    ptr->next = &(frameTable[index]);
		    ptr = ptr->nextOutput;
		}
		frameTable[index].nextOutput = firstB;
		frameTable[index].prev = lastIP;	/* for freeing */
		if ( lastIP != NULL ) {
		    lastIP->next = &(frameTable[index]);
		    if ( secondIP == NULL ) {
			secondIP = &(frameTable[index]);
		    }
		}
		lastIP = &(frameTable[index]);
		firstB = NULL;
		break;
	    case 'p':
		ptr = firstB;
		while ( ptr != NULL ) {
		    ptr->next = &(frameTable[index]);
		    ptr = ptr->nextOutput;
		}
		frameTable[index].nextOutput = firstB;
		frameTable[index].prev = lastIP;
		if ( lastIP != NULL ) {
		    lastIP->next = &(frameTable[index]);
		    if ( secondIP == NULL ) {
			secondIP = &(frameTable[index]);
		    }
		}
		lastIP = &(frameTable[index]);
		firstB = NULL;
		break;
	    case 'b':
		if ( (index+1 == framePatternLen) ||
		     (FType_Type(index+1) != 'b') ) {
		    frameTable[index].nextOutput = NULL;
		} else {
		    frameTable[index].nextOutput = &(frameTable[index+1]);
		}
		frameTable[index].prev = lastIP;
		if ( firstB == NULL ) {
		    firstB = &(frameTable[index]);
		}
		break;
	    default:
	        io_printf(stderr, "Programmer Error in ComputeFrameTable (%d)\n",
			framePattern[index]);
	        exit(1);
	        break;
	}
    }
    
    /* why? SRS */
    frameTable[table_size].number = framePatternLen;
    ptr = firstB;
    while ( ptr != NULL ) {
	ptr->next = &(frameTable[table_size]);
	ptr = ptr->nextOutput;
    }
    frameTable[table_size].nextOutput = firstB;
    frameTable[table_size].prev = lastIP;
    if ( secondIP == NULL )
	frameTable[table_size].next = &(frameTable[0]);
    else
	frameTable[table_size].next = secondIP;

    frameTable[0].prev = lastIP;
    if ( lastIP != NULL ) {
	lastIP->next = &(frameTable[table_size]);
    }

    if (!stdinUsed) {
      use_cache = TRUE;
    }
}

/*===========================================================================*
 * fsize.c								     *
 *									     *
 *	procedures to keep track of frame size				     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	Fsize_Reset							     *
 *	Fsize_Note							     *
 *	Fsize_Validate							     *
 *									     *
 * EXPORTED VARIABLES:							     *
 *	Fsize_x								     *
 *	Fsize_y								     *
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


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * Fsize_Reset
 *
 *	reset the frame size to 0
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    Fsize_x, Fsize_y
 *
 *===========================================================================*/
void
Fsize_Reset()
{
    Fsize_x = Fsize_y = 0;
}


/*===========================================================================*
 *
 * Fsize_Validate
 *
 *	make sure that the x, y values are 16-pixel aligned
 *
 * RETURNS:	modifies the x, y values to 16-pixel alignment
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Fsize_Validate(x, y)
    int *x;
    int *y;
{
    *x &= ~(DCTSIZE * 2 - 1);
    *y &= ~(DCTSIZE * 2 - 1);
}


/*===========================================================================*
 *
 * Fsize_Note
 *
 *	note the given frame size and modify the global values as appropriate
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    Fsize_x, Fsize_y
 *
 *===========================================================================*/
void
Fsize_Note(id, width, height)
    int id;
    int width;
    int height;
{
  static boolean warned16 = FALSE,
                 warnedSz = FALSE;
  int oFsize_x, oFsize_y;

  oFsize_x = Fsize_x;
  oFsize_y = Fsize_y;
  Fsize_x = width;
  Fsize_y = height;
  Fsize_Validate(&Fsize_x, &Fsize_y);
  
  if ((Fsize_x != width || Fsize_y != height) && !warned16) {
    warned16 = TRUE;
    io_printf(stderr, "WARNING: The input images are not a multiple of 16 in dimension\n\tthey will be trimmed.\n\n");
  }
  if ((((oFsize_x !=0) && (oFsize_x != Fsize_x)) ||
       ((oFsize_y !=0) && (oFsize_y != Fsize_y))) &&
      !warnedSz) {
    io_printf(stderr, "\nWARNING:: Your images seem to be of different sizes.\n");
    io_printf(stderr, "\tThis is likely to not work, and dump core.\n");
    io_printf(stderr, "\tBut we're feeling risky, we'll try it anyway!\n\n");
    warnedSz = TRUE;
  }

  if ((Fsize_x==0) || (Fsize_y==0)) {
    io_printf(stderr,"Frame %d:  size is zero!\n",id);
    /*      exit(1); */
  }
  
#ifdef BLEAH
    if (Fsize_x == 0) {
	Fsize_x = width;
	Fsize_y = height;
	Fsize_Validate(&Fsize_x, &Fsize_y);
    } else if (width < Fsize_x || height < Fsize_y) {
	io_printf(stderr, "Frame %d: wrong size: (%d,%d).  Should be greater or equal to: (%d,%d)\n",
		id, width, height, Fsize_x, Fsize_y);
	exit(1);
    }
#endif
}
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

/*===========================================================================*
 * iframe.c								     *
 *									     *
 *	Procedures concerned with the I-frame encoding			     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	GenIFrame							     *
 *	SetSlicesPerFrame						     *
 *	SetBlocksPerSlice						     *
 *	SetIQScale							     *
 *	GetIQScale							     *
 *	ResetIFrameStats						     *
 *	ShowIFrameSummary						     *
 *	EstimateSecondsPerIFrame					     *
 *	EncodeYDC							     *
 *	EncodeCDC							     *
 *      time_elapsed                                                         *
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
 *  Revision 1.23  1995/08/14 22:29:49  smoot
 *  changed inits in BlockComputeSNR so sgi compiler would be happy
 *
 *  Revision 1.22  1995/08/07 21:50:51  smoot
 *  spacing
 *  simplified some code
 *  added Laplace stuff
 *  minor error check bug in alloc
 *
 * Revision 1.21  1995/06/21  22:24:25  smoot
 * added CalcDistortion (TUNEing)
 * fixed timeing stuff for ANSI
 * fixed specifics bug
 *
 * Revision 1.20  1995/05/02  21:59:43  smoot
 * fixed BlockComputeSNR bugs
 *
 * Revision 1.19  1995/04/24  23:02:50  smoot
 * Fixed BlockComputeSNR for Linux and others
 *
 * Revision 1.18  1995/04/14  23:08:02  smoot
 * reorganized to ease rate control experimentation
 *
 * Revision 1.17  1995/02/24  23:49:38  smoot
 * added support for Specifics file version 2
 *
 * Revision 1.16  1995/01/30  20:02:34  smoot
 * cleanup, killed a couple warnings
 *
 * Revision 1.15  1995/01/30  19:49:17  smoot
 * cosmetic
 *
 * Revision 1.14  1995/01/23  02:49:34  darryl
 * initialized variable
 *
 * Revision 1.13  1995/01/19  23:08:30  eyhung
 * Changed copyrights
 *
 * Revision 1.12  1995/01/16  08:01:34  eyhung
 * Added realQuiet
 *
 * Revision 1.11  1994/12/07  00:40:36  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.10  1994/11/14  22:30:30  smoot
 * Merged specifics and rate control
 *
 * Revision 1.9  1994/11/01  05:00:48  darryl
 * with rate control changes added
 *
 * Revision 2.2  1994/10/31  00:06:07  darryl
 * version before, hopefully, final changes
 *
 * Revision 2.1  1994/10/24  22:03:01  darryl
 * put in preliminary experiments code
 *
 * Revision 2.0  1994/10/24  02:38:04  darryl
 * will be adding the experiment stuff.
 *
 * Revision 1.1  1994/09/27  00:15:24  darryl
 * Initial revision
 *
 * Revision 1.8  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.7  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.6  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.5  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.4  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.3  1993/03/04  22:24:06  keving
 * nothing
 *
 * Revision 1.2  1993/02/19  18:10:02  keving
 * nothing
 *
 * Revision 1.1  1993/02/18  22:56:39  keving
 * nothing
 *
 *
 */


/*==================*
 * STATIC VARIABLES *
 *==================*/

static  int	lastNumBits = 0;
static  int	lastIFrame = 0;
static int numBlocks = 0;
static int numBits;
static int numFrames2 = 0;
static int numFrameBits2 = 0;
static int32 totalTime2 = 0;
static float	totalSNR2 = 0.0;
static float	totalPSNR2 = 0.0;

static int lengths[256] = {
    0, 1, 2, 2, 3, 3, 3, 3,	    /* 0 - 7 */
    4, 4, 4, 4, 4, 4, 4, 4,	    /* 8 - 15 */
    5, 5, 5, 5, 5, 5, 5, 5,	    /* 16 - 31 */
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,	    /* 32 - 63 */
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7,	    /* 64 - 127 */
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8
};


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/
extern void	PrintItoIBitRate _ANSI_ARGS_((int numBits, int frameNum));

/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/
void AllocDctBlocks _ANSI_ARGS_((void ));
int SetFCodeHelper _ANSI_ARGS_((int sr));
void CalcDistortion _ANSI_ARGS_((MpegFrame *current, int y, int x));

int
  SetFCodeHelper(SR)
int SR;
{
    int	    range,fCode;

    if ( pixelFullSearch ) {
	range = SR;
    } else {
	range = SR*2;
    }

    if ( range < 256 ) {
	if ( range < 64 ) {
	    if ( range < 32 ) {
		fCode = 1;
	    } else {
		fCode = 2;
	    }
	} else {
	    if ( range < 128 ) {
		fCode = 3;
	    } else {
		fCode = 4;
	    }
	}
    } else {
	if ( range < 1024 ) {
	    if ( range < 512 ) {
		fCode = 5;
	    } else {
		fCode = 6;
	    }
	} else {
	    if ( range < 2048 ) {
		fCode = 7;
	    } else {
		io_printf(stderr, "ERROR:  INVALID SEARCH RANGE!!!\n");
		exit(1);
	    }
	}
      }
    return(fCode);
}

/*===========================================================================*
 *
 * SetFCode
 *
 *	set the forward_f_code and backward_f_code according to the search
 *	range.  Must be called AFTER pixelFullSearch and searchRange have
 *	been initialized.  Irrelevant for I-frames, but computation is
 *	negligible (done only once, as well)
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    fCodeI,fCodeP,fCodeB
 *
 *===========================================================================*/
void
SetFCode()
{
  fCodeI = SetFCodeHelper(1); /* GenIFrame ignores value */
  fCodeP = SetFCodeHelper(searchRangeP);
  fCodeB = SetFCodeHelper(searchRangeB);
}

/*===========================================================================*
 *
 * SetSlicesPerFrame
 *
 *	set the number of slices per frame
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    slicesPerFrame
 *
 *===========================================================================*/
void
SetSlicesPerFrame(number)
    int number;
{
    slicesPerFrame = number;
}


/*===========================================================================*
 *
 * SetBlocksPerSlice
 *
 *	set the number of blocks per slice, based on slicesPerFrame
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    blocksPerSlice
 *
 *===========================================================================*/
void
SetBlocksPerSlice()
{
    int	    totalBlocks;

    totalBlocks = (Fsize_y>>4)*(Fsize_x>>4);

    if ( slicesPerFrame > totalBlocks ) {
	blocksPerSlice = 1;
    } else {
	blocksPerSlice = totalBlocks/slicesPerFrame;
    }
}


/*===========================================================================*
 *
 * SetIQScale
 *
 *	set the I-frame Q-scale
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    qscaleI
 *
 *===========================================================================*/
void
SetIQScale(qI)
int qI;
{
    qscaleI = qI;
}

/*===========================================================================*
 *
 * GetIQScale
 *
 *	Get the I-frame Q-scale
 *
 * RETURNS:	the Iframe Qscale
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
GetIQScale()
{
    return(qscaleI);
}

/*===========================================================================*
 *
 * GenIFrame
 *
 *	generate an I-frame; appends result to bb
 *
 * RETURNS:	I-frame appended to bb
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
GenIFrame(bb, current)
     BitBucket *bb;
     MpegFrame *current;
{
    register int x, y;
    register int index;
    FlatBlock	 fb[6];
    Block 	 dec[6];
    int32 y_dc_pred, cr_dc_pred, cb_dc_pred;
    int          totalBits;
    int	         totalFrameBits;
    int32        startTime, endTime;
    float        snr[3], psnr[3];
    int	         mbAddress;
    int          QScale;
    BlockMV      *info; /* Not used in Iframes, but nice to pass in anyway */
    int          bitstreamMode, newQScale;
    int          rc_blockStart=0;

    if (dct==NULL) AllocDctBlocks();
    if (collect_quant) {io_printf(collect_quant_fp, "# I\n");}

    /* set-up for statistics */
    numFrames2++;
    totalFrameBits = bb->cumulativeBits;
    if ( ( ! childProcess) && showBitRatePerFrame ) {
      if ( lastNumBits == 0 ) {
	lastNumBits = bb->cumulativeBits;
	lastIFrame = current->id;
      } else {
	/* ASSUMES 30 FRAMES PER SECOND */
	
	if (! realQuiet) {
	  io_printf(stdout, "I-to-I (frames %5d to %5d) bitrate:  %8ld\n",
		  lastIFrame, current->id-1,
		  ((bb->cumulativeBits-lastNumBits)*30)/
		  (current->id-lastIFrame));
	}
	
	io_printf(bitRateFile, "I-to-I (frames %5d to %5d) bitrate:  %8ld\n",
		lastIFrame, current->id-1,
		((bb->cumulativeBits-lastNumBits)*30)/
		(current->id-lastIFrame));
	lastNumBits = bb->cumulativeBits;	    
	lastIFrame = current->id;
      }
    }
    
    startTime = time_elapsed();
    
    Frame_AllocBlocks(current);
    BlockifyFrame(current);

    DBG_PRINT(("Generating iframe\n"));
    QScale = GetIQScale();
    /*   Allocate bits for this frame for rate control purposes */
    bitstreamMode = getRateMode();
    if (bitstreamMode == FIXED_RATE) {
      targetRateControl(current);
    }

    Mhead_GenPictureHeader(bb, I_FRAME, current->id, fCodeI);
    /* Check for Qscale change */
    if (specificsOn) {
      newQScale = SpecLookup(current->id, 0, 0 /* junk */, &info, QScale);
      if (newQScale != -1) {
	QScale = newQScale;
      }
      /* check for slice */
      newQScale = SpecLookup(current->id, 1, 1, &info, QScale);
      if (newQScale != -1) {
	QScale = newQScale;
      }
    }
    Mhead_GenSliceHeader(bb, 1, QScale, NULL, 0);
    
    if ( referenceFrame == DECODED_FRAME ) {
      Frame_AllocDecoded(current, TRUE);
    } else if ( printSNR ) {
      Frame_AllocDecoded(current, FALSE);
    }
    
    y_dc_pred = cr_dc_pred = cb_dc_pred = 128;
    totalBits = bb->cumulativeBits;
    mbAddress = 0;

    /* DCT the macroblocks */
    for (y = 0;  y < (Fsize_y >> 3);  y += 2) {
      for (x = 0;  x < (Fsize_x >> 3);  x += 2) {
	if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "li\n");
	if (DoLaplace) {LaplaceCnum = 0;}
	mp_fwd_dct_block2(current->y_blocks[y][x], dct[y][x]);
	mp_fwd_dct_block2(current->y_blocks[y][x+1], dct[y][x+1]);
	mp_fwd_dct_block2(current->y_blocks[y+1][x], dct[y+1][x]);
	mp_fwd_dct_block2(current->y_blocks[y+1][x+1], dct[y+1][x+1]);
	if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "ci\n");
	if (DoLaplace) {LaplaceCnum = 1;}
	mp_fwd_dct_block2(current->cb_blocks[y>>1][x>>1], dctb[y>>1][x>>1]);
	if (DoLaplace) {LaplaceCnum = 2;}
	mp_fwd_dct_block2(current->cr_blocks[y>>1][x>>1], dctr[y>>1][x>>1]);
      }}
	
    if (DoLaplace) {
      extern void CalcLambdas(void);
      CalcLambdas();
    }

    for (y = 0;  y < (Fsize_y >> 3);  y += 2) {
      for (x = 0;  x < (Fsize_x >> 3);  x += 2) {
	/* Check for Qscale change */
	if (specificsOn) {
	  newQScale = SpecLookup(current->id, 2, mbAddress, &info, QScale);
	  if (newQScale != -1) {
	    QScale = newQScale;
	  }
	}
	
	/*  Determine if new Qscale needed for Rate Control purposes  */
	if (bitstreamMode == FIXED_RATE) {
	  rc_blockStart = bb->cumulativeBits;
	  newQScale = needQScaleChange(qscaleI,
				       current->y_blocks[y][x],
				       current->y_blocks[y][x+1],
				       current->y_blocks[y+1][x],
				       current->y_blocks[y+1][x+1]);
	  if (newQScale > 0) {
	    QScale = newQScale;
	  }
	}

	if ( (mbAddress % blocksPerSlice == 0) && (mbAddress != 0) ) {
	  /* create a new slice */
	  if (specificsOn) {
	    /* Make sure no slice Qscale change */
	    newQScale = SpecLookup(current->id,1,mbAddress/blocksPerSlice, &info, QScale);
	    if (newQScale != -1) QScale = newQScale;
	  }
	  Mhead_GenSliceEnder(bb);
	  Mhead_GenSliceHeader(bb, 1+(y>>1), QScale, NULL, 0);
	  y_dc_pred = cr_dc_pred = cb_dc_pred = 128;
	  
	  GEN_I_BLOCK(I_FRAME, current, bb, 1+(x>>1), QScale);
	} else {
	  GEN_I_BLOCK(I_FRAME, current, bb, 1, QScale);
	}

	if (WriteDistortionNumbers) {
	  CalcDistortion(current, y, x);
	}
	
	if ( decodeRefFrames ) {
	  /* now, reverse the DCT transform */
	  LaplaceCnum = 0;
	  for ( index = 0; index < 6; index++ ) {
	    if (!DoLaplace) {
	      Mpost_UnQuantZigBlock(fb[index], dec[index], QScale, TRUE);
	    } else {
	      if (index == 4) {LaplaceCnum = 1;}
	      if (index == 5) {LaplaceCnum = 2;}
	      Mpost_UnQuantZigBlockLaplace(fb[index], dec[index], QScale, TRUE);
	    }
	    mpeg_jrevdct((int16 *)dec[index]);		
	    }
	  
	  /* now, unblockify */
	  BlockToData(current->decoded_y, dec[0], y, x);
	  BlockToData(current->decoded_y, dec[1], y, x+1);
	  BlockToData(current->decoded_y, dec[2], y+1, x);
	  BlockToData(current->decoded_y, dec[3], y+1, x+1);
	  BlockToData(current->decoded_cb, dec[4], y>>1, x>>1);
	  BlockToData(current->decoded_cr, dec[5], y>>1, x>>1);
	}
	
	numBlocks++;
	mbAddress++;
	/*   Rate Control */
	if (bitstreamMode == FIXED_RATE) {
	  incMacroBlockBits(bb->cumulativeBits - rc_blockStart);
	  rc_blockStart = bb->cumulativeBits;
	  MB_RateOut(TYPE_IFRAME);
	}
      }
    }
    
    if ( printSNR ) {
      BlockComputeSNR(current,snr,psnr);
      totalSNR2 += snr[0];
      totalPSNR2 += psnr[0];
    }
    
    if ( (referenceFrame == DECODED_FRAME) && NonLocalRefFrame(current->id) ) {
      if ( remoteIO ) {
	SendDecodedFrame(current);
      } else {
	WriteDecodedFrame(current);
      }
      
      /* now, tell decode server it is ready */
      NotifyDecodeServerReady(current->id);
    }
    
    numBits += (bb->cumulativeBits-totalBits);
    
    DBG_PRINT(("End of frame\n"));
    
    Mhead_GenSliceEnder(bb);
    /*   Rate Control  */
    if (bitstreamMode == FIXED_RATE) {
      updateRateControl(TYPE_IFRAME);
    }
    
    endTime = time_elapsed();
    totalTime2 += (endTime-startTime);
    
    numFrameBits2 += (bb->cumulativeBits-totalFrameBits);
    
    if ( ( ! childProcess) && showBitRatePerFrame ) {
      /* ASSUMES 30 FRAMES PER SECOND */
      io_printf(bitRateFile, "%5d\t%8ld\n", current->id,
	      30*(bb->cumulativeBits-totalFrameBits));
    }
    
    if ( (! childProcess) && frameSummary && (! realQuiet) ) {
      
      /* ASSUMES 30 FRAMES PER SECOND */
      io_printf(stdout, "FRAME %d (I):  %ld seconds  (%ld bits/s output)\n", 
	      current->id, (long)((endTime-startTime)/TIME_RATE),
	      30*(bb->cumulativeBits-totalFrameBits));
      if ( printSNR ) {
	io_printf(stdout, "FRAME %d:  SNR:  %.1f\t%.1f\t%.1f\tPSNR:  %.1f\t%.1f\t%.1f\n",
		current->id, snr[0], snr[1], snr[2],
		psnr[0], psnr[1], psnr[2]);
      }
    }
}


/*===========================================================================*
 *
 * ResetIFrameStats
 *
 *	reset the I-frame statistics
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
ResetIFrameStats()
{
    numBlocks = 0;
    numBits = 0;
    numFrames2 = 0;
    numFrameBits2 = 0;
    totalTime2 = 0;
}


/*===========================================================================*
 *
 * ShowIFrameSummary
 *
 *	prints out statistics on all I-frames
 *
 * RETURNS:	time taken for I-frames (in seconds)
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
float
ShowIFrameSummary(inputFrameBits, totalBits, fpointer)
    int inputFrameBits;
    int32 totalBits;
    FILE *fpointer;
{
    if ( numFrames2 == 0 ) {
	return(0.0);
    }

    io_printf(fpointer, "-------------------------\n");
    io_printf(fpointer, "*****I FRAME SUMMARY*****\n");
    io_printf(fpointer, "-------------------------\n");

    io_printf(fpointer, "  Blocks:    %5d     (%6d bits)     (%5d bpb)\n",
	    numBlocks, numBits, numBits/numBlocks);
    io_printf(fpointer, "  Frames:    %5d     (%6d bits)     (%5d bpf)     (%2.1f%% of total)\n",
	    numFrames2, numFrameBits2, numFrameBits2/numFrames2,
	    100.0*(float)numFrameBits2/(float)totalBits);
    io_printf(fpointer, "  Compression:  %3d:1     (%9.4f bpp)\n",
	    numFrames2*inputFrameBits/numFrameBits2,
	    24.0*(float)numFrameBits2/(float)(numFrames2*inputFrameBits));
    if ( printSNR )
	io_printf(fpointer, "  Avg Y SNR/PSNR:  %.1f     %.1f\n",
		totalSNR2/(float)numFrames2, totalPSNR2/(float)numFrames2);
    if ( totalTime2 == 0 ) {
	io_printf(fpointer, "  Seconds:  NONE\n");
    } else {
	io_printf(fpointer, "  Seconds:  %9ld     (%9.4f fps)  (%9ld pps)  (%9ld mps)\n",
		(long)(totalTime2/TIME_RATE),
		(float)((float)(TIME_RATE*numFrames2)/(float)totalTime2),
		(long)((float)TIME_RATE*(float)numFrames2*(float)inputFrameBits/(24.0*(float)totalTime2)),
		(long)((float)TIME_RATE*(float)numFrames2*(float)inputFrameBits/(256.0*24.0*(float)totalTime2)));
    }

    return((float)totalTime2/(float)TIME_RATE);
}


/*===========================================================================*
 *
 * EstimateSecondsPerIFrame
 *
 *	estimates the number of seconds required per I-frame
 *
 * RETURNS:	seconds (floating point value)
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
float
EstimateSecondsPerIFrame()
{
    return((float)totalTime2/((float)TIME_RATE*(float)numFrames2));
}


/*===========================================================================*
 *
 * EncodeYDC
 *
 *	Encode the DC portion of a DCT of a luminance block
 *
 * RETURNS:	result appended to bb
 *
 * SIDE EFFECTS:    updates pred_term
 *
 *===========================================================================*/
void
EncodeYDC(dc_term, pred_term, bb)
    int32 dc_term;
    int32 *pred_term;
    BitBucket *bb;
{
    /* see Table B.5a -- MPEG-I doc */
    static int codes[9] = {
	0x4, 0x0, 0x1, 0x5, 0x6, 0xe, 0x1e, 0x3e, 0x7e
    };
    static int codeLengths[9] = {
	3,   2,   2,   3,   3,   4,   5,    6,    7
    };
    int ydiff, ydiff_abs;
    int	length;

    ydiff = (dc_term - (*pred_term));
    if (ydiff > 255) {
#ifdef BLEAH 
      io_printf(stdout, "TRUNCATED\n");
#endif
	ydiff = 255;
    } else if (ydiff < -255) {
#ifdef BLEAH 
      io_printf(stdout, "TRUNCATED\n");
#endif
	ydiff = -255;
    }

    ydiff_abs = ABS(ydiff);
    length = lengths[ydiff_abs];
    Bitio_Write(bb, codes[length], codeLengths[length]);
    if ( length != 0 ) {
	if ( ydiff > 0 ) {
	    Bitio_Write(bb, ydiff_abs, length);
	} else {
	    Bitio_Write(bb, ~ydiff_abs, length);
	}
    }

    (*pred_term) += ydiff;
}


/*===========================================================================*
 *
 * EncodeCDC
 *
 *	Encode the DC portion of a DCT of a chrominance block
 *
 * RETURNS:	result appended to bb
 *
 * SIDE EFFECTS:    updates pred_term
 *
 *===========================================================================*/
void
EncodeCDC(dc_term, pred_term, bb)
    int32 dc_term;
    int32 *pred_term;
    BitBucket *bb;
{
    /* see Table B.5b -- MPEG-I doc */
    static int codes[9] = {
	0x0, 0x1, 0x2, 0x6, 0xe, 0x1e, 0x3e, 0x7e, 0xfe
    };
    static int codeLengths[9] = {
	2,   2,   2,   3,   4,   5,    6,    7,    8
    };
    int cdiff, cdiff_abs;
    int	length;

    cdiff = (dc_term - (*pred_term));
    if (cdiff > 255) {
#ifdef BLEAH
io_printf(stdout, "TRUNCATED\n");	
#endif
	cdiff = 255;
    } else if (cdiff < -255) {
#ifdef BLEAH
io_printf(stdout, "TRUNCATED\n");	
#endif
	cdiff = -255;
    }

    cdiff_abs = ABS(cdiff);
    length = lengths[cdiff_abs];
    Bitio_Write(bb, codes[length], codeLengths[length]);
    if ( length != 0 ) {
	if ( cdiff > 0 ) {
	    Bitio_Write(bb, cdiff_abs, length);
	} else {
	    Bitio_Write(bb, ~cdiff_abs, length);
	}
    }

    (*pred_term) += cdiff;
}


void
BlockComputeSNR(current, snr, psnr)
     MpegFrame *current;
     float snr[];
     float psnr[];
{
  register int32	tempInt;
  register int y, x;
  int32	varDiff[3];
  double	ratio[3];
  double	total[3];
  register u_int8_t **origY=current->orig_y, **origCr=current->orig_cr, 
  **origCb=current->orig_cb;
  register u_int8_t **newY=current->decoded_y, **newCr=current->decoded_cr, 
  **newCb=current->decoded_cb;
  static int32       **SignalY,  **NoiseY;
  static int32       **SignalCb, **NoiseCb;
  static int32       **SignalCr, **NoiseCr;
  static short   ySize[3], xSize[3];
  static boolean needs_init=TRUE;
  
  /* Init */
  if (needs_init) {
    int ysz = (Fsize_y>>3);
    int xsz = (Fsize_x>>3);
    
    needs_init = FALSE;
    ySize[0]=Fsize_y;     xSize[0]=Fsize_x;
    ySize[1]=Fsize_y>>1;  xSize[1]=Fsize_x>>1;
    ySize[2]=Fsize_y>>1;  xSize[2]=Fsize_x>>1;
    SignalY  = CMAKE_N(int32 *, ysz);
    NoiseY   = CMAKE_N(int32 *, ysz);
    SignalCb = CMAKE_N(int32 *, ysz);
    NoiseCb  = CMAKE_N(int32 *, ysz);
    SignalCr = CMAKE_N(int32 *, ysz);
    NoiseCr  = CMAKE_N(int32 *, ysz);
    if (SignalY == NULL || NoiseY == NULL || SignalCr == NULL || 
	NoiseCb == NULL || SignalCb == NULL || NoiseCr == NULL) {
      io_printf(stderr, "Out of memory in BlockComputeSNR\n");
      exit(-1);
    }
    for (y = 0; y < ySize[0]>>3; y++) {
      SignalY[y]  = (int32 *) calloc(xsz,4);
      SignalCr[y]  = (int32 *) calloc(xsz,4);
      SignalCb[y]  = (int32 *) calloc(xsz,4);
      NoiseY[y]  = (int32 *) calloc(xsz,4);
      NoiseCr[y]  = (int32 *) calloc(xsz,4);
      NoiseCb[y]  = (int32 *) calloc(xsz,4);
    }
  } else {
    for (y = 0; y < ySize[0]>>3; y++) {
      memset((char *) &NoiseY[y][0], 0, (xSize[0]>>3) * 4);
      memset((char *) &SignalY[y][0], 0, (xSize[0]>>3) * 4);
      memset((char *) &NoiseCb[y][0], 0, (xSize[0]>>3) * 4);
      memset((char *) &NoiseCr[y][0], 0, (xSize[0]>>3) * 4);
      memset((char *) &SignalCb[y][0], 0, (xSize[0]>>3) * 4);
      memset((char *) &SignalCr[y][0], 0, (xSize[0]>>3) * 4);
    }
  }
  
  for (y=0; y<3; y++) {
    varDiff[y] = ratio[y] = total[y] = 0.0;
  }

  /* find all the signal and noise */
  for (y = 0; y < ySize[0]; y++) {
    for (x = 0; x < xSize[0]; x++) {
      tempInt = (origY[y][x] - newY[y][x]);
      NoiseY[y>>4][x>>4] += tempInt*tempInt;
      total[0] += (double)abs(tempInt);
      tempInt = origY[y][x];
      SignalY[y>>4][x>>4] += tempInt*tempInt;
    }}
  for (y = 0; y < ySize[1]; y++) {
    for (x = 0; x < xSize[1]; x ++) {
      tempInt = (origCb[y][x] - newCb[y][x]);
      NoiseCb[y>>3][x>>3] += tempInt*tempInt;
      total[1] += (double)abs(tempInt);
      tempInt = origCb[y][x];
      SignalCb[y>>3][x>>3] += tempInt*tempInt;
      tempInt = (origCr[y][x]-newCr[y][x]);
      NoiseCr[y>>3][x>>3] += tempInt*tempInt;
      total[2] += (double)abs(tempInt);
      tempInt = origCr[y][x];
      SignalCr[y>>3][x>>3] += tempInt*tempInt;
    }}
  
  /* Now sum up that noise */
  for(y=0; y<Fsize_y>>4; y++){
    for(x=0; x<Fsize_x>>4; x++){
      varDiff[0] += NoiseY[y][x];
      varDiff[1] += NoiseCb[y][x];
      varDiff[2] += NoiseCr[y][x];
      if (printMSE) printf("%4d ",(int)(NoiseY[y][x]/256.0));
    }
    if (printMSE) puts("");
  }
  
  /* Now look at those ratios! */
  for(y=0; y<Fsize_y>>4; y++){
    for(x=0; x<Fsize_x>>4; x++){
      ratio[0] += (double)SignalY[y][x]/(double)varDiff[0];
      ratio[1] += (double)SignalCb[y][x]/(double)varDiff[1];
      ratio[2] += (double)SignalCr[y][x]/(double)varDiff[2];
    }}
  
  for (x=0; x<3; x++) {
    snr[x] = 10.0*log10(ratio[x]);
    psnr[x] = 20.0*log10(255.0/sqrt((double)varDiff[x]/(double)(ySize[x]*xSize[x])));

    if (! realQuiet) {
      io_printf(stdout, "Mean error[%1d]:  %f\n", x, total[x]/(double)(xSize[x]*ySize[x]));
    }

  }
}

void
WriteDecodedFrame(frame)
    MpegFrame *frame;
{
    FILE    *fpointer;
    char    fileName[256];
    int	width, height;
    register int y;

    /* need to save decoded frame to disk because it might be accessed
       by another process */

    width = Fsize_x;
    height = Fsize_y;

    snprintf(fileName, 256, "%s.decoded.%d", outputFileName, frame->id);

    if (!realQuiet) {
	io_printf(stdout, "Outputting to %s\n", fileName);
	io_flush(stdout);
    }

    fpointer = _PG_fopen(fileName, "wb");

	for ( y = 0; y < height; y++ ) {
	    io_write(frame->decoded_y[y], 1, width, fpointer);
	}

	for (y = 0; y < (height >> 1); y++) {			/* U */
	    io_write(frame->decoded_cb[y], 1, width >> 1, fpointer);
	}

	for (y = 0; y < (height >> 1); y++) {			/* V */
	    io_write(frame->decoded_cr[y], 1, width >> 1, fpointer);
	}
    io_flush(fpointer);
    io_close(fpointer);
}


void
PrintItoIBitRate(int numBits, int frameNum)
{
    if ( ( ! childProcess) && showBitRatePerFrame ) {
	/* ASSUMES 30 FRAMES PER SECOND */

	if (! realQuiet) {
	io_printf(stdout, "I-to-I (frames %5d to %5d) bitrate:  %8d\n",
		lastIFrame, frameNum-1,
		((numBits-lastNumBits)*30)/
		(frameNum-lastIFrame));
        }

	io_printf(bitRateFile, "I-to-I (frames %5d to %5d) bitrate:  %8d\n",
		lastIFrame, frameNum-1,
		((numBits-lastNumBits)*30)/
		(frameNum-lastIFrame));
    }
}


/*===========================================================================*
 *
 * AllocDctBlocks
 *
 *	allocate memory for dct blocks
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    creates dct, dctr, dctb
 *
 *===========================================================================*/
void
AllocDctBlocks()
{
    int dctx, dcty;
    int i;

    dctx = Fsize_x / DCTSIZE;
    dcty = Fsize_y / DCTSIZE;

    dct = CMAKE_N(Block *, dcty);
    ERRCHK(dct, "malloc");
    for (i = 0; i < dcty; i++) {
	dct[i] = CMAKE_N(Block, dctx);
	ERRCHK(dct[i], "malloc");
    }

    dct_data = CMAKE_N(dct_data_type *, dcty);
    ERRCHK(dct_data, "malloc");
    for (i = 0; i < dcty; i++) {
	dct_data[i] = CMAKE_N(dct_data_type, dctx);
	ERRCHK(dct[i], "malloc");
    }

    dctr = CMAKE_N(Block *, (dcty >> 1));
    dctb = CMAKE_N(Block *, (dcty >> 1));
    ERRCHK(dctr, "malloc");
    ERRCHK(dctb, "malloc");
    for (i = 0; i < (dcty >> 1); i++) {
	dctr[i] = CMAKE_N(Block, (dctx >> 1));
	dctb[i] = CMAKE_N(Block, (dctx >> 1));
	ERRCHK(dctr[i], "malloc");
	ERRCHK(dctb[i], "malloc");
    }
}


/*======================================================================*
 *
 * time_elapsed
 *
 *     Handle different time systems on different machines
 *
 *  RETURNS number of seconds process time used
 *
 *======================================================================*/
int32 time_elapsed()
{int32 tm;

#ifdef CLOCKS_PER_SEC

/* ANSI C */
  TIME_RATE = CLOCKS_PER_SEC;
  tm = (int32) clock();

#else

  struct tms   timeBuffer;

  TIME_RATE = 60;
  times(&timeBuffer);

  tm = timeBuffer.tms_utime + timeBuffer.tms_stime;

#endif

  return(tm);}


void
CalcDistortion(current, y, x)
MpegFrame *current;
int y,x;
{

  int qscale, distort=0;
  Block decblk;
  FlatBlock fblk;
  int datarate = 0;
  
  for (qscale = 1; qscale < 32; qscale ++) {
    distort = 0;
    datarate = 0;
    Mpost_QuantZigBlock(dct[y][x], fblk, qscale, TRUE);
    Mpost_UnQuantZigBlock(fblk, decblk, qscale, TRUE);
    if (collect_distortion_detailed) datarate += CalcRLEHuffLength(fblk);
    mpeg_jrevdct((int16 *)decblk);
    distort += mse(current->y_blocks[y][x], decblk);

    Mpost_QuantZigBlock(dct[y][x+1], fblk, qscale, TRUE);
    Mpost_UnQuantZigBlock(fblk, decblk, qscale, TRUE);
    if (collect_distortion_detailed) datarate += CalcRLEHuffLength(fblk);
    mpeg_jrevdct((int16 *)decblk);
    distort += mse(current->y_blocks[y][x+1], decblk);

    Mpost_QuantZigBlock(dct[y+1][x], fblk, qscale, TRUE);
    Mpost_UnQuantZigBlock(fblk, decblk, qscale, TRUE);
    if (collect_distortion_detailed) datarate += CalcRLEHuffLength(fblk);
    mpeg_jrevdct((int16 *)decblk);
    distort += mse(current->y_blocks[y+1][x], decblk);

    Mpost_QuantZigBlock(dct[y+1][x+1], fblk, qscale, TRUE);
    Mpost_UnQuantZigBlock(fblk, decblk, qscale, TRUE);
    if (collect_distortion_detailed) datarate += CalcRLEHuffLength(fblk);
    mpeg_jrevdct((int16 *)decblk);
    distort += mse(current->y_blocks[y+1][x+1], decblk);

    Mpost_QuantZigBlock(dctb[y >> 1][x >> 1], fblk, qscale, TRUE);
    Mpost_UnQuantZigBlock(fblk, decblk, qscale, TRUE);
    if (collect_distortion_detailed) datarate += CalcRLEHuffLength(fblk);
    mpeg_jrevdct((int16 *)decblk);
    distort += mse(current->cb_blocks[y>>1][x>>1], decblk);

    Mpost_QuantZigBlock(dctr[y >> 1][x >> 1], fblk, qscale, TRUE);
    Mpost_UnQuantZigBlock(fblk, decblk, qscale, TRUE);
    if (collect_distortion_detailed) datarate += CalcRLEHuffLength(fblk);
    mpeg_jrevdct((int16 *)decblk);
    distort += mse(current->cr_blocks[y >> 1][x >> 1], decblk);

    if (!collect_distortion_detailed) {
      io_printf(distortion_fp, "\t%d\n", distort);
    } else if (collect_distortion_detailed == 1) {
      io_printf(distortion_fp, "\t%d\t%d\n", distort, datarate);
    } else {
      io_printf(fp_table_rate[qscale-1], "%d\n", datarate);
      io_printf(fp_table_dist[qscale-1], "%d\n", distort);
    }
  }
}




/*
 * jrevdct.c
 *
 * Copyright (C) 1991, 1992, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the basic inverse-DCT transformation subroutine.
 *
 * This implementation is based on an algorithm described in
 *   C. Loeffler, A. Ligtenberg and G. Moschytz, "Practical Fast 1-D DCT
 *   Algorithms with 11 Multiplications", Proc. Int'l. Conf. on Acoustics,
 *   Speech, and Signal Processing 1989 (ICASSP '89), pp. 988-991.
 * The primary algorithm described there uses 11 multiplies and 29 adds.
 * We use their alternate method with 12 multiplies and 32 adds.
 * The advantage of this method is that no data path contains more than one
 * multiplication; this allows a very simple and accurate implementation in
 * scaled fixed-point arithmetic, with a minimal number of shifts.
 * 
 * I've made lots of modifications to attempt to take advantage of the
 * sparse nature of the DCT matrices we're getting.  Although the logic
 * is cumbersome, it's straightforward and the resulting code is much
 * faster.
 *
 * A better way to do this would be to pass in the DCT block as a sparse
 * matrix, perhaps with the difference cases encoded.
 */


#define CONST_BITS 13

/*
 * This routine is specialized to the case DCTSIZE = 8.
 */

#if DCTSIZE != 8
  Sorry, this code only copes with 8x8 DCTs. /* deliberate syntax err */
#endif


/*
 * A 2-D IDCT can be done by 1-D IDCT on each row followed by 1-D IDCT
 * on each column.  Direct algorithms are also available, but they are
 * much more complex and seem not to be any faster when reduced to code.
 *
 * The poop on this scaling stuff is as follows:
 *
 * Each 1-D IDCT step produces outputs which are a factor of sqrt(N)
 * larger than the true IDCT outputs.  The final outputs are therefore
 * a factor of N larger than desired; since N=8 this can be cured by
 * a simple right shift at the end of the algorithm.  The advantage of
 * this arrangement is that we save two multiplications per 1-D IDCT,
 * because the y0 and y4 inputs need not be divided by sqrt(N).
 *
 * We have to do addition and subtraction of the integer inputs, which
 * is no problem, and multiplication by fractional constants, which is
 * a problem to do in integer arithmetic.  We multiply all the constants
 * by CONST_SCALE and convert them to integer constants (thus retaining
 * CONST_BITS bits of precision in the constants).  After doing a
 * multiplication we have to divide the product by CONST_SCALE, with proper
 * rounding, to produce the correct output.  This division can be done
 * cheaply as a right shift of CONST_BITS bits.  We postpone shifting
 * as long as possible so that partial sums can be added together with
 * full fractional precision.
 *
 * The outputs of the first pass are scaled up by PASS1_BITS bits so that
 * they are represented to better-than-integral precision.  These outputs
 * require BITS_IN_JSAMPLE + PASS1_BITS + 3 bits; this fits in a 16-bit word
 * with the recommended scaling.  (To scale up 12-bit sample data further, an
 * intermediate int32 array would be needed.)
 *
 * To avoid overflow of the 32-bit intermediate results in pass 2, we must
 * have BITS_IN_JSAMPLE + CONST_BITS + PASS1_BITS <= 26.  Error analysis
 * shows that the values given below are the most effective.
 */

#ifdef EIGHT_BIT_SAMPLES
#define PASS1_BITS  2
#else
#define PASS1_BITS  1		/* lose a little precision to avoid overflow */
#endif

#define ONE	((int32) 1)

#define CONST_SCALE (ONE << CONST_BITS)

/* Convert a positive real constant to an integer scaled by CONST_SCALE.
 * IMPORTANT: if your compiler doesn't do this arithmetic at compile time,
 * you will pay a significant penalty in run time.  In that case, figure
 * the correct integer constant values and insert them by hand.
 */

/* Descale and correctly round an int32 value that's scaled by N bits.
 * We assume RIGHT_SHIFT rounds towards minus infinity, so adding
 * the fudge factor is correct for either sign of X.
 */

#define DESCALE(x,n)  RIGHT_SHIFT((x) + (ONE << ((n)-1)), n)

/* Multiply an int32 variable by an int32 constant to yield an int32 result.
 * For 8-bit samples with the recommended scaling, all the variable
 * and constant values involved are no more than 16 bits wide, so a
 * 16x16->32 bit multiply can be used instead of a full 32x32 multiply;
 * this provides a useful speedup on many machines.
 * There is no way to specify a 16x16->32 multiply in portable C, but
 * some C compilers will do the right thing if you provide the correct
 * combination of casts.
 * NB: for 12-bit samples, a full 32-bit multiplication will be needed.
 */

#ifdef EIGHT_BIT_SAMPLES
#ifdef SHORTxSHORT_32		/* may work if 'int' is 32 bits */
#define MULTIPLY(var,const)  (((INT16) (var)) * ((INT16) (const)))
#endif
#ifdef SHORTxLCONST_32		/* known to work with Microsoft C 6.0 */
#define MULTIPLY(var,const)  (((INT16) (var)) * ((int32) (const)))
#endif
#endif

#ifndef MULTIPLY		/* default definition */
#define MULTIPLY(var,const)  ((var) * (const))
#endif


/* 
  Unlike our decoder where we approximate the FIXes, we need to use exact
ones here or successive P-frames will drift too much with Reference frame coding 
*/
#define FIX_0_211164243 1730
#define FIX_0_275899380 2260
#define FIX_0_298631336 2446
#define FIX_0_390180644 3196
#define FIX_0_509795579 4176
#define FIX_0_541196100 4433
#define FIX_0_601344887 4926
#define FIX_0_765366865 6270
#define FIX_0_785694958 6436
#define FIX_0_899976223 7373
#define FIX_1_061594337 8697
#define FIX_1_111140466 9102
#define FIX_1_175875602 9633
#define FIX_1_306562965 10703
#define FIX_1_387039845 11363
#define FIX_1_451774981 11893
#define FIX_1_501321110 12299
#define FIX_1_662939225 13623
#define FIX_1_847759065 15137
#define FIX_1_961570560 16069
#define FIX_2_053119869 16819
#define FIX_2_172734803 17799
#define FIX_2_562915447 20995
#define FIX_3_072711026 25172

/*
  Switch on reverse_dct choices
*/
void reference_rev_dct _ANSI_ARGS_((int16 *block));
void mpeg_jrevdct_quick _ANSI_ARGS_((int16 *block));
void init_idctref _ANSI_ARGS_((void));

extern boolean pureDCT;

void
mpeg_jrevdct(data)
    DCTBLOCK data;
{
  if (pureDCT) reference_rev_dct(data);
  else mpeg_jrevdct_quick(data);
}

/*
 * Perform the inverse DCT on one block of coefficients.
 */

void
mpeg_jrevdct_quick(data)
    DCTBLOCK data;
{
  int32 tmp0, tmp1, tmp2, tmp3;
  int32 tmp10, tmp11, tmp12, tmp13;
  int32 z1, z2, z3, z4, z5;
  int32 d0, d1, d2, d3, d4, d5, d6, d7;
  register DCTELEM *dataptr;
  int rowctr;
  SHIFT_TEMPS
   
  /* Pass 1: process rows. */
  /* Note results are scaled up by sqrt(8) compared to a true IDCT; */
  /* furthermore, we scale the results by 2**PASS1_BITS. */

  dataptr = data;

  for (rowctr = DCTSIZE-1; rowctr >= 0; rowctr--) {
    /* Due to quantization, we will usually find that many of the input
     * coefficients are zero, especially the AC terms.  We can exploit this
     * by short-circuiting the IDCT calculation for any row in which all
     * the AC terms are zero.  In that case each output is equal to the
     * DC coefficient (with scale factor as needed).
     * With typical images and quantization tables, half or more of the
     * row DCT calculations can be simplified this way.
     */

    register int *idataptr = (int*)dataptr;
    d0 = dataptr[0];
    d1 = dataptr[1];
    if ((d1 == 0) && (idataptr[1] | idataptr[2] | idataptr[3]) == 0) {
      /* AC terms all zero */
      if (d0) {
	  /* Compute a 32 bit value to assign. */
	  DCTELEM dcval = (DCTELEM) (d0 << PASS1_BITS);
	  register int v = (dcval & 0xffff) | ((dcval << 16) & 0xffff0000);
	  
	  idataptr[0] = v;
	  idataptr[1] = v;
	  idataptr[2] = v;
	  idataptr[3] = v;
      }
      
      dataptr += DCTSIZE;	/* advance pointer to next row */
      continue;
    }
    d2 = dataptr[2];
    d3 = dataptr[3];
    d4 = dataptr[4];
    d5 = dataptr[5];
    d6 = dataptr[6];
    d7 = dataptr[7];

    /* Even part: reverse the even part of the forward DCT. */
    /* The rotator is sqrt(2)*c(-6). */
{
    if (d6) {
	if (d4) {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 != 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp0 = (d0 + d4) << CONST_BITS;
		    tmp1 = (d0 - d4) << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp1 + tmp2;
		    tmp12 = tmp1 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 != 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp0 = d4 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp2 - tmp0;
		    tmp12 = -(tmp0 + tmp2);
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 != 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp0 = (d0 + d4) << CONST_BITS;
		    tmp1 = (d0 - d4) << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp1 + tmp2;
		    tmp12 = tmp1 - tmp2;
		} else {
		    /* d0 == 0, d2 == 0, d4 != 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp0 = d4 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp2 - tmp0;
		    tmp12 = -(tmp0 + tmp2);
		}
	    }
	} else {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 == 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp0 = d0 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp0 + tmp2;
		    tmp12 = tmp0 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 == 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp10 = tmp3;
		    tmp13 = -tmp3;
		    tmp11 = tmp2;
		    tmp12 = -tmp2;
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 == 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp0 = d0 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp0 + tmp2;
		    tmp12 = tmp0 - tmp2;
		} else {
		    /* d0 == 0, d2 == 0, d4 == 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp10 = tmp3;
		    tmp13 = -tmp3;
		    tmp11 = tmp2;
		    tmp12 = -tmp2;
		}
	    }
	}
    } else {
	if (d4) {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 != 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp0 = (d0 + d4) << CONST_BITS;
		    tmp1 = (d0 - d4) << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp1 + tmp2;
		    tmp12 = tmp1 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 != 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp0 = d4 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp2 - tmp0;
		    tmp12 = -(tmp0 + tmp2);
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 != 0, d6 == 0 */
		    tmp10 = tmp13 = (d0 + d4) << CONST_BITS;
		    tmp11 = tmp12 = (d0 - d4) << CONST_BITS;
		} else {
		    /* d0 == 0, d2 == 0, d4 != 0, d6 == 0 */
		    tmp10 = tmp13 = d4 << CONST_BITS;
		    tmp11 = tmp12 = -tmp10;
		}
	    }
	} else {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 == 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp0 = d0 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp0 + tmp2;
		    tmp12 = tmp0 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 == 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp10 = tmp3;
		    tmp13 = -tmp3;
		    tmp11 = tmp2;
		    tmp12 = -tmp2;
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 == 0, d6 == 0 */
		    tmp10 = tmp13 = tmp11 = tmp12 = d0 << CONST_BITS;
		} else {
		    /* d0 == 0, d2 == 0, d4 == 0, d6 == 0 */
		    tmp10 = tmp13 = tmp11 = tmp12 = 0;
		}
	    }
	}
      }

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */

    if (d7) {
	if (d5) {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 != 0, d7 != 0 */
		    z1 = d7 + d1;
		    z2 = d5 + d3;
		    z3 = d7 + d3;
		    z4 = d5 + d1;
		    z5 = MULTIPLY(z3 + z4, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-z1, FIX_0_899976223);
		    z2 = MULTIPLY(-z2, FIX_2_562915447);
		    z3 = MULTIPLY(-z3, FIX_1_961570560);
		    z4 = MULTIPLY(-z4, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 != 0, d5 != 0, d7 != 0 */
		    z2 = d5 + d3;
		    z3 = d7 + d3;
		    z5 = MULTIPLY(z3 + d5, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    z1 = MULTIPLY(-d7, FIX_0_899976223);
		    z2 = MULTIPLY(-z2, FIX_2_562915447);
		    z3 = MULTIPLY(-z3, FIX_1_961570560);
		    z4 = MULTIPLY(-d5, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 = z1 + z4;
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 != 0, d7 != 0 */
		    z1 = d7 + d1;
		    z4 = d5 + d1;
		    z5 = MULTIPLY(d7 + z4, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-z1, FIX_0_899976223);
		    z2 = MULTIPLY(-d5, FIX_2_562915447);
		    z3 = MULTIPLY(-d7, FIX_1_961570560);
		    z4 = MULTIPLY(-z4, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 = z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 == 0, d5 != 0, d7 != 0 */
		    tmp0 = MULTIPLY(-d7, FIX_0_601344887); 
		    z1 = MULTIPLY(-d7, FIX_0_899976223);
		    z3 = MULTIPLY(-d7, FIX_1_961570560);
		    tmp1 = MULTIPLY(-d5, FIX_0_509795579);
		    z2 = MULTIPLY(-d5, FIX_2_562915447);
		    z4 = MULTIPLY(-d5, FIX_0_390180644);
		    z5 = MULTIPLY(d5 + d7, FIX_1_175875602);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z3;
		    tmp1 += z4;
		    tmp2 = z2 + z3;
		    tmp3 = z1 + z4;
		}
	    }
	} else {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 == 0, d7 != 0 */
		    z1 = d7 + d1;
		    z3 = d7 + d3;
		    z5 = MULTIPLY(z3 + d1, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-z1, FIX_0_899976223);
		    z2 = MULTIPLY(-d3, FIX_2_562915447);
		    z3 = MULTIPLY(-z3, FIX_1_961570560);
		    z4 = MULTIPLY(-d1, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 = z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 != 0, d5 == 0, d7 != 0 */
		    z3 = d7 + d3;
		    
		    tmp0 = MULTIPLY(-d7, FIX_0_601344887); 
		    z1 = MULTIPLY(-d7, FIX_0_899976223);
		    tmp2 = MULTIPLY(d3, FIX_0_509795579);
		    z2 = MULTIPLY(-d3, FIX_2_562915447);
		    z5 = MULTIPLY(z3, FIX_1_175875602);
		    z3 = MULTIPLY(-z3, FIX_0_785694958);
		    
		    tmp0 += z3;
		    tmp1 = z2 + z5;
		    tmp2 += z3;
		    tmp3 = z1 + z5;
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 == 0, d7 != 0 */
		    z1 = d7 + d1;
		    z5 = MULTIPLY(z1, FIX_1_175875602);

		    z1 = MULTIPLY(z1, FIX_0_275899380);
		    z3 = MULTIPLY(-d7, FIX_1_961570560);
		    tmp0 = MULTIPLY(-d7, FIX_1_662939225); 
		    z4 = MULTIPLY(-d1, FIX_0_390180644);
		    tmp3 = MULTIPLY(d1, FIX_1_111140466);

		    tmp0 += z1;
		    tmp1 = z4 + z5;
		    tmp2 = z3 + z5;
		    tmp3 += z1;
		} else {
		    /* d1 == 0, d3 == 0, d5 == 0, d7 != 0 */
		    tmp0 = MULTIPLY(-d7, FIX_1_387039845);
		    tmp1 = MULTIPLY(d7, FIX_1_175875602);
		    tmp2 = MULTIPLY(-d7, FIX_0_785694958);
		    tmp3 = MULTIPLY(d7, FIX_0_275899380);
		}
	    }
	}
    } else {
	if (d5) {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 != 0, d7 == 0 */
		    z2 = d5 + d3;
		    z4 = d5 + d1;
		    z5 = MULTIPLY(d3 + z4, FIX_1_175875602);
		    
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-d1, FIX_0_899976223);
		    z2 = MULTIPLY(-z2, FIX_2_562915447);
		    z3 = MULTIPLY(-d3, FIX_1_961570560);
		    z4 = MULTIPLY(-z4, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 = z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 != 0, d5 != 0, d7 == 0 */
		    z2 = d5 + d3;
		    
		    z5 = MULTIPLY(z2, FIX_1_175875602);
		    tmp1 = MULTIPLY(d5, FIX_1_662939225);
		    z4 = MULTIPLY(-d5, FIX_0_390180644);
		    z2 = MULTIPLY(-z2, FIX_1_387039845);
		    tmp2 = MULTIPLY(d3, FIX_1_111140466);
		    z3 = MULTIPLY(-d3, FIX_1_961570560);
		    
		    tmp0 = z3 + z5;
		    tmp1 += z2;
		    tmp2 += z2;
		    tmp3 = z4 + z5;
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 != 0, d7 == 0 */
		    z4 = d5 + d1;
		    
		    z5 = MULTIPLY(z4, FIX_1_175875602);
		    z1 = MULTIPLY(-d1, FIX_0_899976223);
		    tmp3 = MULTIPLY(d1, FIX_0_601344887);
		    tmp1 = MULTIPLY(-d5, FIX_0_509795579);
		    z2 = MULTIPLY(-d5, FIX_2_562915447);
		    z4 = MULTIPLY(z4, FIX_0_785694958);
		    
		    tmp0 = z1 + z5;
		    tmp1 += z4;
		    tmp2 = z2 + z5;
		    tmp3 += z4;
		} else {
		    /* d1 == 0, d3 == 0, d5 != 0, d7 == 0 */
		    tmp0 = MULTIPLY(d5, FIX_1_175875602);
		    tmp1 = MULTIPLY(d5, FIX_0_275899380);
		    tmp2 = MULTIPLY(-d5, FIX_1_387039845);
		    tmp3 = MULTIPLY(d5, FIX_0_785694958);
		}
	    }
	} else {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 == 0, d7 == 0 */
		    z5 = d1 + d3;
		    tmp3 = MULTIPLY(d1, FIX_0_211164243);
		    tmp2 = MULTIPLY(-d3, FIX_1_451774981);
		    z1 = MULTIPLY(d1, FIX_1_061594337);
		    z2 = MULTIPLY(-d3, FIX_2_172734803);
		    z4 = MULTIPLY(z5, FIX_0_785694958);
		    z5 = MULTIPLY(z5, FIX_1_175875602);
		    
		    tmp0 = z1 - z4;
		    tmp1 = z2 + z4;
		    tmp2 += z5;
		    tmp3 += z5;
		} else {
		    /* d1 == 0, d3 != 0, d5 == 0, d7 == 0 */
		    tmp0 = MULTIPLY(-d3, FIX_0_785694958);
		    tmp1 = MULTIPLY(-d3, FIX_1_387039845);
		    tmp2 = MULTIPLY(-d3, FIX_0_275899380);
		    tmp3 = MULTIPLY(d3, FIX_1_175875602);
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 == 0, d7 == 0 */
		    tmp0 = MULTIPLY(d1, FIX_0_275899380);
		    tmp1 = MULTIPLY(d1, FIX_0_785694958);
		    tmp2 = MULTIPLY(d1, FIX_1_175875602);
		    tmp3 = MULTIPLY(d1, FIX_1_387039845);
		} else {
		    /* d1 == 0, d3 == 0, d5 == 0, d7 == 0 */
		    tmp0 = tmp1 = tmp2 = tmp3 = 0;
		}
	    }
	}
    }
}
    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    dataptr[0] = (DCTELEM) DESCALE(tmp10 + tmp3, CONST_BITS-PASS1_BITS);
    dataptr[7] = (DCTELEM) DESCALE(tmp10 - tmp3, CONST_BITS-PASS1_BITS);
    dataptr[1] = (DCTELEM) DESCALE(tmp11 + tmp2, CONST_BITS-PASS1_BITS);
    dataptr[6] = (DCTELEM) DESCALE(tmp11 - tmp2, CONST_BITS-PASS1_BITS);
    dataptr[2] = (DCTELEM) DESCALE(tmp12 + tmp1, CONST_BITS-PASS1_BITS);
    dataptr[5] = (DCTELEM) DESCALE(tmp12 - tmp1, CONST_BITS-PASS1_BITS);
    dataptr[3] = (DCTELEM) DESCALE(tmp13 + tmp0, CONST_BITS-PASS1_BITS);
    dataptr[4] = (DCTELEM) DESCALE(tmp13 - tmp0, CONST_BITS-PASS1_BITS);

    dataptr += DCTSIZE;		/* advance pointer to next row */
  }

  /* Pass 2: process columns. */
  /* Note that we must descale the results by a factor of 8 == 2**3, */
  /* and also undo the PASS1_BITS scaling. */

  dataptr = data;
  for (rowctr = DCTSIZE-1; rowctr >= 0; rowctr--) {
    /* Columns of zeroes can be exploited in the same way as we did with rows.
     * However, the row calculation has created many nonzero AC terms, so the
     * simplification applies less often (typically 5% to 10% of the time).
     * On machines with very fast multiplication, it's possible that the
     * test takes more time than it's worth.  In that case this section
     * may be commented out.
     */

    d0 = dataptr[DCTSIZE*0];
    d1 = dataptr[DCTSIZE*1];
    d2 = dataptr[DCTSIZE*2];
    d3 = dataptr[DCTSIZE*3];
    d4 = dataptr[DCTSIZE*4];
    d5 = dataptr[DCTSIZE*5];
    d6 = dataptr[DCTSIZE*6];
    d7 = dataptr[DCTSIZE*7];

    /* Even part: reverse the even part of the forward DCT. */
    /* The rotator is sqrt(2)*c(-6). */
    if (d6) {
	if (d4) {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 != 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp0 = (d0 + d4) << CONST_BITS;
		    tmp1 = (d0 - d4) << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp1 + tmp2;
		    tmp12 = tmp1 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 != 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp0 = d4 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp2 - tmp0;
		    tmp12 = -(tmp0 + tmp2);
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 != 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp0 = (d0 + d4) << CONST_BITS;
		    tmp1 = (d0 - d4) << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp1 + tmp2;
		    tmp12 = tmp1 - tmp2;
		} else {
		    /* d0 == 0, d2 == 0, d4 != 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp0 = d4 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp2 - tmp0;
		    tmp12 = -(tmp0 + tmp2);
		}
	    }
	} else {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 == 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp0 = d0 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp0 + tmp2;
		    tmp12 = tmp0 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 == 0, d6 != 0 */
		    z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
		    tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
		    tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

		    tmp10 = tmp3;
		    tmp13 = -tmp3;
		    tmp11 = tmp2;
		    tmp12 = -tmp2;
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 == 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp0 = d0 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp0 + tmp2;
		    tmp12 = tmp0 - tmp2;
		} else {
		    /* d0 == 0, d2 == 0, d4 == 0, d6 != 0 */
		    tmp2 = MULTIPLY(-d6, FIX_1_306562965);
		    tmp3 = MULTIPLY(d6, FIX_0_541196100);

		    tmp10 = tmp3;
		    tmp13 = -tmp3;
		    tmp11 = tmp2;
		    tmp12 = -tmp2;
		}
	    }
	}
    } else {
	if (d4) {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 != 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp0 = (d0 + d4) << CONST_BITS;
		    tmp1 = (d0 - d4) << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp1 + tmp2;
		    tmp12 = tmp1 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 != 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp0 = d4 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp2 - tmp0;
		    tmp12 = -(tmp0 + tmp2);
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 != 0, d6 == 0 */
		    tmp10 = tmp13 = (d0 + d4) << CONST_BITS;
		    tmp11 = tmp12 = (d0 - d4) << CONST_BITS;
		} else {
		    /* d0 == 0, d2 == 0, d4 != 0, d6 == 0 */
		    tmp10 = tmp13 = d4 << CONST_BITS;
		    tmp11 = tmp12 = -tmp10;
		}
	    }
	} else {
	    if (d2) {
		if (d0) {
		    /* d0 != 0, d2 != 0, d4 == 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp0 = d0 << CONST_BITS;

		    tmp10 = tmp0 + tmp3;
		    tmp13 = tmp0 - tmp3;
		    tmp11 = tmp0 + tmp2;
		    tmp12 = tmp0 - tmp2;
		} else {
		    /* d0 == 0, d2 != 0, d4 == 0, d6 == 0 */
		    tmp2 = MULTIPLY(d2, FIX_0_541196100);
		    tmp3 = MULTIPLY(d2, FIX_1_306562965);

		    tmp10 = tmp3;
		    tmp13 = -tmp3;
		    tmp11 = tmp2;
		    tmp12 = -tmp2;
		}
	    } else {
		if (d0) {
		    /* d0 != 0, d2 == 0, d4 == 0, d6 == 0 */
		    tmp10 = tmp13 = tmp11 = tmp12 = d0 << CONST_BITS;
		} else {
		    /* d0 == 0, d2 == 0, d4 == 0, d6 == 0 */
		    tmp10 = tmp13 = tmp11 = tmp12 = 0;
		}
	    }
	}
    }

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */
    if (d7) {
	if (d5) {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 != 0, d7 != 0 */
		    z1 = d7 + d1;
		    z2 = d5 + d3;
		    z3 = d7 + d3;
		    z4 = d5 + d1;
		    z5 = MULTIPLY(z3 + z4, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-z1, FIX_0_899976223);
		    z2 = MULTIPLY(-z2, FIX_2_562915447);
		    z3 = MULTIPLY(-z3, FIX_1_961570560);
		    z4 = MULTIPLY(-z4, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 != 0, d5 != 0, d7 != 0 */
		    z1 = d7;
		    z2 = d5 + d3;
		    z3 = d7 + d3;
		    z5 = MULTIPLY(z3 + d5, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    z1 = MULTIPLY(-d7, FIX_0_899976223);
		    z2 = MULTIPLY(-z2, FIX_2_562915447);
		    z3 = MULTIPLY(-z3, FIX_1_961570560);
		    z4 = MULTIPLY(-d5, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 = z1 + z4;
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 != 0, d7 != 0 */
		    z1 = d7 + d1;
		    z2 = d5;
		    z3 = d7;
		    z4 = d5 + d1;
		    z5 = MULTIPLY(z3 + z4, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-z1, FIX_0_899976223);
		    z2 = MULTIPLY(-d5, FIX_2_562915447);
		    z3 = MULTIPLY(-d7, FIX_1_961570560);
		    z4 = MULTIPLY(-z4, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 = z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 == 0, d5 != 0, d7 != 0 */
		    tmp0 = MULTIPLY(-d7, FIX_0_601344887); 
		    z1 = MULTIPLY(-d7, FIX_0_899976223);
		    z3 = MULTIPLY(-d7, FIX_1_961570560);
		    tmp1 = MULTIPLY(-d5, FIX_0_509795579);
		    z2 = MULTIPLY(-d5, FIX_2_562915447);
		    z4 = MULTIPLY(-d5, FIX_0_390180644);
		    z5 = MULTIPLY(d5 + d7, FIX_1_175875602);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z3;
		    tmp1 += z4;
		    tmp2 = z2 + z3;
		    tmp3 = z1 + z4;
		}
	    }
	} else {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 == 0, d7 != 0 */
		    z1 = d7 + d1;
		    z3 = d7 + d3;
		    z5 = MULTIPLY(z3 + d1, FIX_1_175875602);
		    
		    tmp0 = MULTIPLY(d7, FIX_0_298631336); 
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-z1, FIX_0_899976223);
		    z2 = MULTIPLY(-d3, FIX_2_562915447);
		    z3 = MULTIPLY(-z3, FIX_1_961570560);
		    z4 = MULTIPLY(-d1, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 += z1 + z3;
		    tmp1 = z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 != 0, d5 == 0, d7 != 0 */
		    z3 = d7 + d3;
		    
		    tmp0 = MULTIPLY(-d7, FIX_0_601344887); 
		    z1 = MULTIPLY(-d7, FIX_0_899976223);
		    tmp2 = MULTIPLY(d3, FIX_0_509795579);
		    z2 = MULTIPLY(-d3, FIX_2_562915447);
		    z5 = MULTIPLY(z3, FIX_1_175875602);
		    z3 = MULTIPLY(-z3, FIX_0_785694958);
		    
		    tmp0 += z3;
		    tmp1 = z2 + z5;
		    tmp2 += z3;
		    tmp3 = z1 + z5;
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 == 0, d7 != 0 */
		    z1 = d7 + d1;
		    z5 = MULTIPLY(z1, FIX_1_175875602);

		    z1 = MULTIPLY(z1, FIX_0_275899380);
		    z3 = MULTIPLY(-d7, FIX_1_961570560);
		    tmp0 = MULTIPLY(-d7, FIX_1_662939225); 
		    z4 = MULTIPLY(-d1, FIX_0_390180644);
		    tmp3 = MULTIPLY(d1, FIX_1_111140466);

		    tmp0 += z1;
		    tmp1 = z4 + z5;
		    tmp2 = z3 + z5;
		    tmp3 += z1;
		} else {
		    /* d1 == 0, d3 == 0, d5 == 0, d7 != 0 */
		    tmp0 = MULTIPLY(-d7, FIX_1_387039845);
		    tmp1 = MULTIPLY(d7, FIX_1_175875602);
		    tmp2 = MULTIPLY(-d7, FIX_0_785694958);
		    tmp3 = MULTIPLY(d7, FIX_0_275899380);
		}
	    }
	}
    } else {
	if (d5) {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 != 0, d7 == 0 */
		    z2 = d5 + d3;
		    z4 = d5 + d1;
		    z5 = MULTIPLY(d3 + z4, FIX_1_175875602);
		    
		    tmp1 = MULTIPLY(d5, FIX_2_053119869);
		    tmp2 = MULTIPLY(d3, FIX_3_072711026);
		    tmp3 = MULTIPLY(d1, FIX_1_501321110);
		    z1 = MULTIPLY(-d1, FIX_0_899976223);
		    z2 = MULTIPLY(-z2, FIX_2_562915447);
		    z3 = MULTIPLY(-d3, FIX_1_961570560);
		    z4 = MULTIPLY(-z4, FIX_0_390180644);
		    
		    z3 += z5;
		    z4 += z5;
		    
		    tmp0 = z1 + z3;
		    tmp1 += z2 + z4;
		    tmp2 += z2 + z3;
		    tmp3 += z1 + z4;
		} else {
		    /* d1 == 0, d3 != 0, d5 != 0, d7 == 0 */
		    z2 = d5 + d3;
		    
		    z5 = MULTIPLY(z2, FIX_1_175875602);
		    tmp1 = MULTIPLY(d5, FIX_1_662939225);
		    z4 = MULTIPLY(-d5, FIX_0_390180644);
		    z2 = MULTIPLY(-z2, FIX_1_387039845);
		    tmp2 = MULTIPLY(d3, FIX_1_111140466);
		    z3 = MULTIPLY(-d3, FIX_1_961570560);
		    
		    tmp0 = z3 + z5;
		    tmp1 += z2;
		    tmp2 += z2;
		    tmp3 = z4 + z5;
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 != 0, d7 == 0 */
		    z4 = d5 + d1;
		    
		    z5 = MULTIPLY(z4, FIX_1_175875602);
		    z1 = MULTIPLY(-d1, FIX_0_899976223);
		    tmp3 = MULTIPLY(d1, FIX_0_601344887);
		    tmp1 = MULTIPLY(-d5, FIX_0_509795579);
		    z2 = MULTIPLY(-d5, FIX_2_562915447);
		    z4 = MULTIPLY(z4, FIX_0_785694958);
		    
		    tmp0 = z1 + z5;
		    tmp1 += z4;
		    tmp2 = z2 + z5;
		    tmp3 += z4;
		} else {
		    /* d1 == 0, d3 == 0, d5 != 0, d7 == 0 */
		    tmp0 = MULTIPLY(d5, FIX_1_175875602);
		    tmp1 = MULTIPLY(d5, FIX_0_275899380);
		    tmp2 = MULTIPLY(-d5, FIX_1_387039845);
		    tmp3 = MULTIPLY(d5, FIX_0_785694958);
		}
	    }
	} else {
	    if (d3) {
		if (d1) {
		    /* d1 != 0, d3 != 0, d5 == 0, d7 == 0 */
		    z5 = d1 + d3;
		    tmp3 = MULTIPLY(d1, FIX_0_211164243);
		    tmp2 = MULTIPLY(-d3, FIX_1_451774981);
		    z1 = MULTIPLY(d1, FIX_1_061594337);
		    z2 = MULTIPLY(-d3, FIX_2_172734803);
		    z4 = MULTIPLY(z5, FIX_0_785694958);
		    z5 = MULTIPLY(z5, FIX_1_175875602);
		    
		    tmp0 = z1 - z4;
		    tmp1 = z2 + z4;
		    tmp2 += z5;
		    tmp3 += z5;
		} else {
		    /* d1 == 0, d3 != 0, d5 == 0, d7 == 0 */
		    tmp0 = MULTIPLY(-d3, FIX_0_785694958);
		    tmp1 = MULTIPLY(-d3, FIX_1_387039845);
		    tmp2 = MULTIPLY(-d3, FIX_0_275899380);
		    tmp3 = MULTIPLY(d3, FIX_1_175875602);
		}
	    } else {
		if (d1) {
		    /* d1 != 0, d3 == 0, d5 == 0, d7 == 0 */
		    tmp0 = MULTIPLY(d1, FIX_0_275899380);
		    tmp1 = MULTIPLY(d1, FIX_0_785694958);
		    tmp2 = MULTIPLY(d1, FIX_1_175875602);
		    tmp3 = MULTIPLY(d1, FIX_1_387039845);
		} else {
		    /* d1 == 0, d3 == 0, d5 == 0, d7 == 0 */
		    tmp0 = tmp1 = tmp2 = tmp3 = 0;
		}
	    }
	}
    }

    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    dataptr[DCTSIZE*0] = (DCTELEM) DESCALE(tmp10 + tmp3,
					   CONST_BITS+PASS1_BITS+3);
    dataptr[DCTSIZE*7] = (DCTELEM) DESCALE(tmp10 - tmp3,
					   CONST_BITS+PASS1_BITS+3);
    dataptr[DCTSIZE*1] = (DCTELEM) DESCALE(tmp11 + tmp2,
					   CONST_BITS+PASS1_BITS+3);
    dataptr[DCTSIZE*6] = (DCTELEM) DESCALE(tmp11 - tmp2,
					   CONST_BITS+PASS1_BITS+3);
    dataptr[DCTSIZE*2] = (DCTELEM) DESCALE(tmp12 + tmp1,
					   CONST_BITS+PASS1_BITS+3);
    dataptr[DCTSIZE*5] = (DCTELEM) DESCALE(tmp12 - tmp1,
					   CONST_BITS+PASS1_BITS+3);
    dataptr[DCTSIZE*3] = (DCTELEM) DESCALE(tmp13 + tmp0,
					   CONST_BITS+PASS1_BITS+3);
    dataptr[DCTSIZE*4] = (DCTELEM) DESCALE(tmp13 - tmp0,
					   CONST_BITS+PASS1_BITS+3);
    
    dataptr++;			/* advance pointer to next column */
  }
}


/* here is the reference one, in case of problems with the normal one */

/* idctref.c, Inverse Discrete Fourier Transform, double precision          */

/* Copyright (C) 1994, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

/*  Perform IEEE 1180 reference (64-bit floating point, separable 8x1
 *  direct matrix multiply) Inverse Discrete Cosine Transform
*/


/* Here we use math.h to generate constants.  Compiler results may
   vary a little */

#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.14159265358979323846
#endif
#endif

/* cosine transform matrix for 8x1 IDCT */
static double itrans_coef[8][8];

/* initialize DCT coefficient matrix */

void init_idctref(void)
{
  int freq, time;
  double scale;

  for (freq=0; freq < 8; freq++)
  {
    scale = (freq == 0) ? sqrt(0.125) : 0.5;
    for (time=0; time<8; time++)
      itrans_coef[freq][time] = scale*cos((PI/8.0)*freq*(time + 0.5));
  }
}

/* perform IDCT matrix multiply for 8x8 coefficient block */

void reference_rev_dct(block)
int16 *block;
{
  int i, j, k, v;
  double partial_product;
  double tmp[64];

  for (i=0; i<8; i++)
    for (j=0; j<8; j++)
    {
      partial_product = 0.0;

      for (k=0; k<8; k++)
        partial_product+= itrans_coef[k][j]*block[8*i+k];

      tmp[8*i+j] = partial_product;
    }

  /* Transpose operation is integrated into address mapping by switching 
     loop order of i and j */

  for (j=0; j<8; j++)
    for (i=0; i<8; i++)
    {
      partial_product = 0.0;

      for (k=0; k<8; k++)
        partial_product+= itrans_coef[k][i]*tmp[8*k+j];

      v = floor(partial_product+0.5);
      block[8*i+j] = (v<-256) ? -256 : ((v>255) ? 255 : v);
    }
}
/* libpnmrw.c - PBM/PGM/PPM read/write library
 **
 ** Copyright (C) 1988, 1989, 1991, 1992 by Jef Poskanzer.
 **
 ** Permission to use, copy, modify, and distribute this software and its
 ** documentation for any purpose and without fee is hereby granted, provided
 ** that the above copyright notice appear in all copies and that both that
 ** copyright notice and this permission notice appear in supporting
 ** documentation.  This software is provided "as is" without express or
 ** implied warranty.
 */

#define pm_error(x) exit(1)


#if defined(SVR2) || defined(SVR3) || defined(SVR4)
#define SYSV
#endif
#if ! ( defined(BSD) || defined(SYSV) || defined(MSDOS) )
/* CONFIGURE: If your system is >= 4.2BSD, set the BSD option; if you're a
 ** System V site, set the SYSV option; and if you're IBM-compatible, set
** MSDOS.  If your compiler is ANSI C, you're probably better off setting
** SYSV - all it affects is string handling.
*/
#define BSD
/* #define SYSV */
/* #define MSDOS */
#endif

/* if don't have string.h, try strings.h */
#ifndef _AIX
#include <string.h>
#undef rindex
#define rindex(s,c) strrchr(s,c)
#endif


/* Definitions. */

#define pbm_allocarray( cols, rows ) ((bit**) pm_allocarray( cols, rows, (int) sizeof(bit) ))
#define pbm_allocrow( cols ) ((bit*) pm_allocrow( cols, (int) sizeof(bit) ))
#define pbm_freearray( bits, rows ) pm_freearray( (char**) bits, rows )
#define pbm_freerow( bitrow ) pm_freerow( (char*) bitrow )
#define pgm_allocarray( cols, rows ) ((gray**) pm_allocarray( cols, rows, (int) sizeof(gray) ))
#define pgm_allocrow( cols ) ((gray*) pm_allocrow( cols, (int) sizeof(gray) ))
#define pgm_freearray( grays, rows ) pm_freearray( (char**) grays, rows )
#define pgm_freerow( grayrow ) pm_freerow( (char*) grayrow )
#define ppm_allocarray( cols, rows ) ((pixel**) pm_allocarray( cols, rows, (int) sizeof(pixel) ))
#define ppm_allocrow( cols ) ((pixel*) pm_allocrow( cols, (int) sizeof(pixel) ))
#define ppm_freearray( pixels, rows ) pm_freearray( (char**) pixels, rows )
#define ppm_freerow( pixelrow ) pm_freerow( (char*) pixelrow )


/* Variables. */

static char* progname;


/* Variable-sized arrays. */

char*
pm_allocrow(int cols, int size)
{
  register char* itrow;

  itrow = CMAKE_N(char, cols*size);
  if ( itrow == (char*) 0 )
    {
      (void) io_printf(
		     stderr, "%s: out of memory allocating a row\n", progname );
      return(NULL);
    }
  return(itrow);
}

void pm_freerow(char* itrow) 
   {

    CFREE(itrow);

    return;}

char **pm_allocarray(int cols, int rows, int size)
   {int i;
    char** its;

    its = CMAKE_N(char*, rows);
    if (its == NULL)
       {io_printf(stderr, "%s: out of memory allocating an array\n", progname );
	return(NULL);};

    its[0] = CMAKE_N(char, rows*cols*size);
    if (its[0] == NULL)
       {io_printf(stderr, "%s: out of memory allocating an array\n", progname );
	CFREE(its);
	return(NULL);};

    for (i = 1; i < rows; ++i)
        its[i] = &(its[0][i * cols * size]);

    return(its);}

void pm_freearray(char** its, int rows)
   {

    CFREE(its[0]);
    CFREE(its);

    return;}

/* File open/close that handles "-" as stdin and checks errors. */

static void pm_perror(char* reason)
   {

    if ((reason != 0) && (reason[0] != '\0'))
       io_printf( stderr, "%s: %s - %d\n", progname, reason, errno);
    else
       io_printf( stderr, "%s: %d\n", progname, errno);

    return;}

FILE *pm_openr(char* name)
   {FILE* f;

    if (strcmp(name, "-") == 0)
       f = stdin;

    else
       {f = _PG_fopen(name, "rb");
	if (f == NULL)
	   {pm_perror(name);
	    return(NULL);};};

    return(f);}

FILE *pm_openw(char* name)
   {FILE* f;

    f = _PG_fopen(name, "wb");
    if (f == NULL)
       {pm_perror(name);
	return(NULL);};

    return(f);}

int
  pm_closer( f )
FILE* f;
{
  if ( ferror( f ) )
    {
      (void) io_printf(
		     stderr, "%s: a file read error occurred at some point\n",
		     progname );
      return(-1);
    }
  if ( f != stdin )
    if ( io_close( f ) != 0 )
      {
	pm_perror( "io_close" );
	return(-1);
      }
  return(0);
}

int
  pm_closew( f )
FILE* f;
{
  io_flush( f );
  if ( ferror( f ) )
    {
      (void) io_printf(
		     stderr, "%s: a file write error occurred at some point\n",
		     progname );
      return(-1);
    }
  if ( f != stdout )
    if ( io_close( f ) != 0 )
      {
	pm_perror( "io_close" );
	return(-1);
      }
  return(0);
}

static int
pbm_getc(FILE* file)
{
  register int ich;

  ich = getc( file );
  if ( ich == EOF )
    {
      (void) io_printf( stderr, "%s: EOF / read error\n", progname );
      return(EOF);
    }
    
  if ( ich == '#' )
    {
      do
	{
	  ich = getc( file );
	  if ( ich == EOF )
	    {
	      (void) io_printf( stderr, "%s: EOF / read error\n", progname );
	      return(EOF);
	    }
	}
      while ( ich != '\n' && ich != '\r' );
    }

  return(ich);
}

static bit
pbm_getbit(FILE* file)
{
  register int ich;

  do
    {
      ich = pbm_getc( file );
      if ( ich == EOF )
	return(-1);
    }
  while ( ich == ' ' || ich == '\t' || ich == '\n' || ich == '\r' );

  if ( ich != '0' && ich != '1' )
    {
      (void) io_printf(
		     stderr, "%s: junk in file where bits should be\n", progname );
      return(-1);
    }

  return(( ich == '1' ) ? 1 : 0);
}

static int
pbm_readmagicnumber(FILE* file)
{
  int ich1, ich2;

  ich1 = getc( file );
  if ( ich1 == EOF )
    {
      (void) io_printf(
		     stderr, "%s: EOF / read error reading magic number\n", progname );
      return(-1);
    }
  ich2 = getc( file );
  if ( ich2 == EOF )
    {
      (void) io_printf(
		     stderr, "%s: EOF / read error reading magic number\n", progname );
      return(-1);
    }
  return(ich1 * 256 + ich2);
}

static int
pbm_getint(FILE* file)
{
  register int ich;
  register int i;

  do
    {
      ich = pbm_getc( file );
      if ( ich == EOF )
	return(-1);
    }
  while ( ich == ' ' || ich == '\t' || ich == '\n' || ich == '\r' );

  if ( ich < '0' || ich > '9' )
    {
      (void) io_printf(
		     stderr, "%s: junk in file where an integer should be\n", progname );
      return(-1);
    }

  i = 0;
  do
    {
      i = i * 10 + ich - '0';
      ich = pbm_getc( file );
      if ( ich == EOF )
	return(-1);
    }
  while ( ich >= '0' && ich <= '9' );

  return(i);
}

static int
pbm_readpbminitrest(FILE* file, int* colsP, int* rowsP)
{
  /* Read size. */
  *colsP = pbm_getint( file );
  *rowsP = pbm_getint( file );
  if ( *colsP == -1 || *rowsP == -1 )
    return(-1);
  return(0);
}

static int
pbm_getrawbyte(FILE* file)
{
  register int iby;

  iby = getc( file );
  if ( iby == EOF )
    {
      (void) io_printf( stderr, "%s: EOF / read error\n", progname );
      return(-1);
    }
  return(iby);
}

static int
pbm_readpbmrow(FILE* file, bit* bitrow, int cols, int format)
{
  register int col, bitshift, b;
  register int item;
  register bit* bP;

  switch ( format )
    {
    case PBM_FORMAT:
      for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
	{
	  b = pbm_getbit( file );
	  if ( b == -1 )
	    return(-1);
	  *bP = b;
	}
      break;

    case RPBM_FORMAT:
      bitshift = -1;
      item     = 0;
      for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
	{
	  if ( bitshift == -1 )
	    {
	      item = pbm_getrawbyte( file );
	      if ( item == -1 )
		return(-1);
	      bitshift = 7;
	    }
	  *bP = ( item >> bitshift ) & 1;
	  --bitshift;
	}
      break;

    default:
      (void) io_printf( stderr, "%s: can't happen\n", progname );
      return(-1);
    }
  return(0);
}

static void
pbm_writepbminit(FILE* file, int cols, int rows, int forceplain)
{
  if ( ! forceplain )
    (void) io_printf(
		   file, "%c%c\n%d %d\n", PBM_MAGIC1, RPBM_MAGIC2, cols, rows );
  else
    (void) io_printf(
		   file, "%c%c\n%d %d\n", PBM_MAGIC1, PBM_MAGIC2, cols, rows );
}

static void
pbm_writepbmrowraw(FILE* file, bit* bitrow, int cols)
{
  register int col, bitshift;
  register unsigned char item;
  register bit* bP;

  bitshift = 7;
  item = 0;
  for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
    {
      if ( *bP )
	item += 1 << bitshift;
      --bitshift;
      if ( bitshift == -1 )
	{
	  (void) putc( item, file );
	  bitshift = 7;
	  item = 0;
	}
    }
  if ( bitshift != 7 )
    (void) putc( item, file );
}

static void
pbm_writepbmrowplain(FILE* file, bit* bitrow, int cols)
{
  register int col, charcount;
  register bit* bP;

  charcount = 0;
  for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
    {
      if ( charcount >= 70 )
	{
	  (void) putc( '\n', file );
	  charcount = 0;
	}
      putc( *bP ? '1' : '0', file );
      ++charcount;
    }
  (void) putc( '\n', file );
}

static void
pbm_writepbmrow(FILE* file, bit* bitrow, int cols, int forceplain)
{
  if ( ! forceplain )
    pbm_writepbmrowraw( file, bitrow, cols );
  else
    pbm_writepbmrowplain( file, bitrow, cols );
}

static int
pgm_readpgminitrest(FILE* file, int* colsP, int* rowsP, gray* maxvalP)
{
  int maxval;

  /* Read size. */
  *colsP = pbm_getint( file );
  *rowsP = pbm_getint( file );
  if ( *colsP == -1 || *rowsP == -1 )
    return(-1);

  /* Read maxval. */
  maxval = pbm_getint( file );
  if ( maxval == -1 )
    return(-1);
  if ( maxval > PGM_MAXMAXVAL )
    {
      (void) io_printf( stderr, "%s: maxval is too large\n", progname );
      return(-1);
    }
  *maxvalP = maxval;
  return(0);
}

#if __STDC__
static int
  pgm_readpgmrow( FILE* file, gray* grayrow, int cols, gray maxval, int format )
#else /*__STDC__*/
static int
  pgm_readpgmrow( file, grayrow, cols, maxval, format )
FILE* file;
gray* grayrow;
int cols;
gray maxval;
int format;
#endif /*__STDC__*/
{
  register int col, val;
  register gray* gP;
  /*
    bit* bitrow;
    register bit* bP;
    */

  switch ( format )
    {
    case PGM_FORMAT:
      for ( col = 0, gP = grayrow; col < cols; ++col, ++gP )
	{
	  val = pbm_getint( file );
	  if ( val == -1 )
	    return(-1);
	  *gP = val;
	}
      break;
	
    case RPGM_FORMAT:
      if ( io_read( grayrow, 1, cols, file ) != cols )
	{
	  (void) io_printf( stderr, "%s: EOF / read error\n", progname );
	  return(-1);
	}
      break;

    default:
      (void) io_printf( stderr, "%s: can't happen\n", progname );
      return(-1);
    }
  return(0);
}

#if __STDC__
static void
  pgm_writepgminit( FILE* file, int cols, int rows, gray maxval, int forceplain )
#else /*__STDC__*/
static void
  pgm_writepgminit( file, cols, rows, maxval, forceplain )
FILE* file;
int cols, rows;
gray maxval;
int forceplain;
#endif /*__STDC__*/
{
  if ( !forceplain )
    io_printf(
	    file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, RPGM_MAGIC2,
	    cols, rows, maxval );
  else
    io_printf(
	    file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, PGM_MAGIC2,
	    cols, rows, maxval );
}

static void
putus(unsigned short n, FILE* file)
{
  if ( n >= 10 )
    putus( n / 10, file );
  putc( n % 10 + '0', file );
}

static int
pgm_writepgmrowraw(FILE* file, gray* grayrow, int cols, gray maxval)
{
  if ( io_write( grayrow, 1, cols, file ) != cols )
    {
      (void) io_printf( stderr, "%s: write error\n", progname );
      return(-1);
    }
  return(0);
}

static int
pgm_writepgmrowplain(FILE* file, gray* grayrow, int cols, gray maxval)
{
  register int col, charcount;
  register gray* gP;

  charcount = 0;
  for ( col = 0, gP = grayrow; col < cols; ++col, ++gP )
    {
      if ( charcount >= 65 )
	{
	  (void) putc( '\n', file );
	  charcount = 0;
	}
      else if ( charcount > 0 )
	{
	  (void) putc( ' ', file );
	  ++charcount;
	}
      putus( (unsigned short) *gP, file );
      charcount += 3;
    }
  if ( charcount > 0 )
    (void) putc( '\n', file );
  return(0);
}

#if __STDC__
static int
  pgm_writepgmrow( FILE* file, gray* grayrow, int cols, gray maxval, int forceplain )
#else /*__STDC__*/
static int
  pgm_writepgmrow( file, grayrow, cols, maxval, forceplain )
FILE* file;
gray* grayrow;
int cols;
gray maxval;
int forceplain;
#endif /*__STDC__*/
{
  if ( !forceplain )
    return(pgm_writepgmrowraw( file, grayrow, cols, maxval ));
  else
    return(pgm_writepgmrowplain( file, grayrow, cols, maxval ));
}

static int
ppm_readppminitrest(FILE* file, int* colsP, int* rowsP, pixval* maxvalP)
{
  int maxval;

  /* Read size. */
  *colsP = pbm_getint( file );
  *rowsP = pbm_getint( file );
  if ( *colsP == -1 || *rowsP == -1 )
    return(-1);

  /* Read maxval. */
  maxval = pbm_getint( file );
  if ( maxval == -1 )
    return(-1);
  if ( maxval > PPM_MAXMAXVAL )
    {
      (void) io_printf( stderr, "%s: maxval is too large\n", progname );
      return(-1);
    }
  *maxvalP = maxval;
  return(0);
}

#if __STDC__
static int
  ppm_readppmrow( FILE* file, pixel* pixelrow, int cols, pixval maxval, int format )
#else /*__STDC__*/
static int
  ppm_readppmrow( file, pixelrow, cols, maxval, format )
FILE* file;
pixel* pixelrow;
int cols, format;
pixval maxval;
#endif /*__STDC__*/
{
  register int col;
  register pixel* pP;
  register int r, g, b;
  gray* grayrow;
  register gray* gP;
  /*
    bit* bitrow;
    register bit* bP;
    */

  switch ( format )
    {
    case PPM_FORMAT:
      for ( col = 0, pP = pixelrow; col < cols; ++col, ++pP )
	{
	  r = pbm_getint( file );
	  g = pbm_getint( file );
	  b = pbm_getint( file );
	  if ( r == -1 || g == -1 || b == -1 )
	    return(-1);
	  PPM_ASSIGN( *pP, r, g, b );
	}
      break;

    case RPPM_FORMAT:
      grayrow = pgm_allocrow( 3 * cols );
      if ( grayrow == (gray*) 0 )
	return(-1);
      if ( io_read( grayrow, 1, 3 * cols, file ) != 3 * cols )
	{
	  (void) io_printf( stderr, "%s: EOF / read error\n", progname );
	  return(-1);
	}
      for ( col = 0, gP = grayrow, pP = pixelrow; col < cols; ++col, ++pP )
	{
	  r = *gP++;
	  g = *gP++;
	  b = *gP++;
	  PPM_ASSIGN( *pP, r, g, b );
	}
      pgm_freerow( grayrow );
      break;

    default:
      (void) io_printf( stderr, "%s: can't happen\n", progname );
      return(-1);
    }
  return(0);
}

#if __STDC__
static void
  ppm_writeppminit( FILE* file, int cols, int rows, pixval maxval, int forceplain )
#else /*__STDC__*/
static void
  ppm_writeppminit( file, cols, rows, maxval, forceplain )
FILE* file;
int cols, rows;
pixval maxval;
int forceplain;
#endif /*__STDC__*/
{
  if ( !forceplain )
    io_printf(
	    file, "%c%c\n%d %d\n%d\n", PPM_MAGIC1, RPPM_MAGIC2,
	    cols, rows, maxval );
  else
    io_printf(
	    file, "%c%c\n%d %d\n%d\n", PPM_MAGIC1, PPM_MAGIC2,
	    cols, rows, maxval );
}

static int
ppm_writeppmrowraw(FILE* file, pixel* pixelrow, int cols, pixval maxval)
{
  register int col;
  register pixel* pP;
  gray* grayrow;
  register gray* gP;

  grayrow = pgm_allocrow( 3 * cols );
  if ( grayrow == (gray*) 0 )
    return(-1);
  for ( col = 0, pP = pixelrow, gP = grayrow; col < cols; ++col, ++pP )
    {
      *gP++ = PPM_GETR( *pP );
      *gP++ = PPM_GETG( *pP );
      *gP++ = PPM_GETB( *pP );
    }
  if ( io_write( grayrow, 1, 3 * cols, file ) != 3 * cols )
    {
      (void) io_printf( stderr, "%s: write error\n", progname );
      return(-1);
    }
  pgm_freerow( grayrow );
  return(0);
}

static int
ppm_writeppmrowplain(FILE* file, pixel* pixelrow, int cols, pixval maxval)
{
  register int col, charcount;
  register pixel* pP;
  register pixval val;

  charcount = 0;
  for ( col = 0, pP = pixelrow; col < cols; ++col, ++pP )
    {
      if ( charcount >= 65 )
	{
	  (void) putc( '\n', file );
	  charcount = 0;
	}
      else if ( charcount > 0 )
	{
	  (void) putc( ' ', file );
	  (void) putc( ' ', file );
	  charcount += 2;
	}
      val = PPM_GETR( *pP );
      putus( val, file );
      (void) putc( ' ', file );
      val = PPM_GETG( *pP );
      putus( val, file );
      (void) putc( ' ', file );
      val = PPM_GETB( *pP );
      putus( val, file );
      charcount += 11;
    }
  if ( charcount > 0 )
    (void) putc( '\n', file );
  return(0);
}

#if __STDC__
static int
  ppm_writeppmrow( FILE* file, pixel* pixelrow, int cols, pixval maxval, int forceplain )
#else /*__STDC__*/
static int
  ppm_writeppmrow( file, pixelrow, cols, maxval, forceplain )
FILE* file;
pixel* pixelrow;
int cols;
pixval maxval;
int forceplain;
#endif /*__STDC__*/
{
  if ( !forceplain )
    return(ppm_writeppmrowraw( file, pixelrow, cols, maxval ));
  else
    return(ppm_writeppmrowplain( file, pixelrow, cols, maxval ));
}

void
  pnm_init2( pn )
char* pn;
{
  /* Save program name. */
  progname = pn;
}

xelval pnm_pbmmaxval = 1;

int
  pnm_readpnminit( file, colsP, rowsP, maxvalP, formatP )
FILE* file;
int* colsP;
int* rowsP;
int* formatP;
xelval* maxvalP;
{
  gray gmaxval;

  /* Check magic number. */
  *formatP = pbm_readmagicnumber( file );
  if ( *formatP == -1 )
    return(-1);
  switch ( PNM_FORMAT_TYPE(*formatP) )
    {
    case PPM_TYPE:
      if ( ppm_readppminitrest( file, colsP, rowsP, (pixval*) maxvalP ) < 0 )
	return(-1);
      break;

    case PGM_TYPE:
      if ( pgm_readpgminitrest( file, colsP, rowsP, &gmaxval ) < 0 )
	return(-1);
      *maxvalP = (xelval) gmaxval;
      break;

    case PBM_TYPE:
      if ( pbm_readpbminitrest( file, colsP, rowsP ) < 0 )
	return(-1);
      *maxvalP = pnm_pbmmaxval;
      break;

    default:
      (void) io_printf(
		     stderr, "%s: bad magic number - not a ppm, pgm, or pbm file\n",
		     progname );
      return(-1);
    }
  return(0);
}

#if __STDC__
int
  pnm_readpnmrow( FILE* file, xel* xelrow, int cols, xelval maxval, int format )
#else /*__STDC__*/
int
  pnm_readpnmrow( file, xelrow, cols, maxval, format )
FILE* file;
xel* xelrow;
xelval maxval;
int cols, format;
#endif /*__STDC__*/
{
  register int col;
  register xel* xP;
  gray* grayrow;
  register gray* gP;
  bit* bitrow;
  register bit* bP;

  switch ( PNM_FORMAT_TYPE(format) )
    {
    case PPM_TYPE:
      if ( ppm_readppmrow( file, (pixel*) xelrow, cols, (pixval) maxval, format ) < 0 )
	return(-1);
      break;

    case PGM_TYPE:
      grayrow = pgm_allocrow( cols );
      if ( grayrow == (gray*) 0 )
	return(-1);
      if ( pgm_readpgmrow( file, grayrow, cols, (gray) maxval, format ) < 0 )
	return(-1);
      for ( col = 0, xP = xelrow, gP = grayrow; col < cols; ++col, ++xP, ++gP )
	PNM_ASSIGN1( *xP, *gP );
      pgm_freerow( grayrow );
      break;

    case PBM_TYPE:
      bitrow = pbm_allocrow( cols );
      if ( bitrow == (bit*) 0 )
	return(-1);
      if ( pbm_readpbmrow( file, bitrow, cols, format ) < 0 )
	{
	  pbm_freerow( bitrow );
	  return(-1);
	}
      for ( col = 0, xP = xelrow, bP = bitrow; col < cols; ++col, ++xP, ++bP )
	PNM_ASSIGN1( *xP, *bP == PBM_BLACK ? 0: pnm_pbmmaxval );
      pbm_freerow( bitrow );
      break;

    default:
      (void) io_printf( stderr, "%s: can't happen\n", progname );
      return(-1);
    }
  return(0);
}

xel**
  pnm_readpnm( file, colsP, rowsP, maxvalP, formatP )
FILE* file;
int* colsP;
int* rowsP;
int* formatP;
xelval* maxvalP;
{
  xel** xels;
  int row;

  if ( pnm_readpnminit( file, colsP, rowsP, maxvalP, formatP ) < 0 )
    return((xel**) 0);

  xels = pnm_allocarray( *colsP, *rowsP );
  if ( xels == (xel**) 0 )
    return((xel**) 0);

  for ( row = 0; row < *rowsP; ++row )
    if ( pnm_readpnmrow( file, xels[row], *colsP, *maxvalP, *formatP ) < 0 )
      {
	pnm_freearray( xels, *rowsP );
	return((xel**) 0);
      }

  return(xels);
}

#if __STDC__
int
  pnm_writepnminit( FILE* file, int cols, int rows, xelval maxval, int format, int forceplain )
#else /*__STDC__*/
int
  pnm_writepnminit( file, cols, rows, maxval, format, forceplain )
FILE* file;
int cols, rows, format;
xelval maxval;
int forceplain;
#endif /*__STDC__*/
{
  switch ( PNM_FORMAT_TYPE(format) )
    {
    case PPM_TYPE:
      ppm_writeppminit( file, cols, rows, (pixval) maxval, forceplain );
      break;

    case PGM_TYPE:
      pgm_writepgminit( file, cols, rows, (gray) maxval, forceplain );
      break;

    case PBM_TYPE:
      pbm_writepbminit( file, cols, rows, forceplain );
      break;

    default:
      (void) io_printf( stderr, "%s: can't happen\n", progname );
      return(-1);
    }
  return(0);
}

#if __STDC__
int
  pnm_writepnmrow( FILE* file, xel* xelrow, int cols, xelval maxval, int format, int forceplain )
#else /*__STDC__*/
int
  pnm_writepnmrow( file, xelrow, cols, maxval, format, forceplain )
FILE* file;
xel* xelrow;
int cols, format;
xelval maxval;
int forceplain;
#endif /*__STDC__*/
{
  register int col;
  register xel* xP;
  gray* grayrow;
  register gray* gP;
  bit* bitrow;
  register bit* bP;

  switch ( PNM_FORMAT_TYPE(format) )
    {
    case PPM_TYPE:
      if ( ppm_writeppmrow( file, (pixel*) xelrow, cols, (pixval) maxval, forceplain ) < 0 )
	return(-1);
      break;

    case PGM_TYPE:
      grayrow = pgm_allocrow( cols );
      if ( grayrow == (gray*) 0 )
	return(-1);
      for ( col = 0, gP = grayrow, xP = xelrow; col < cols; ++col, ++gP, ++xP )
	*gP = PNM_GET1( *xP );
      if ( pgm_writepgmrow( file, grayrow, cols, (gray) maxval, forceplain ) < 0 )
	{
	  pgm_freerow( grayrow );
	  return(-1);
	}
      pgm_freerow( grayrow );
      break;

    case PBM_TYPE:
      bitrow = pbm_allocrow( cols );
      if ( bitrow == (bit*) 0 )
	return(-1);
      for ( col = 0, bP = bitrow, xP = xelrow; col < cols; ++col, ++bP, ++xP )
	*bP = PNM_GET1( *xP ) == 0 ? PBM_BLACK : PBM_WHITE;
      pbm_writepbmrow( file, bitrow, cols, forceplain );
      pbm_freerow( bitrow );
      break;

    default:
      (void) io_printf( stderr, "%s: can't happen\n", progname );
      return(-1);
    }
  return(0);
}

#if __STDC__
int
  pnm_writepnm( FILE* file, xel** xels, int cols, int rows, xelval maxval, int format, int forceplain )
#else /*__STDC__*/
int
  pnm_writepnm( file, xels, cols, rows, maxval, format, forceplain )
FILE* file;
xel** xels;
xelval maxval;
int cols, rows, format;
int forceplain;
#endif /*__STDC__*/
{
  int row;

  if ( pnm_writepnminit( file, cols, rows, maxval, format, forceplain ) < 0 )
    return(-1);

  for ( row = 0; row < rows; ++row )
    if ( pnm_writepnmrow( file, xels[row], cols, maxval, format, forceplain ) < 0 )
      return(-1);
  return(0);
}


/* Colormap stuff. */

#define HASH_SIZE 20023

#define ppm_hashpixel(p) ( ( ( (long) PPM_GETR(p) * 33023 + (long) PPM_GETG(p) * 30013 + (long) PPM_GETB(p) * 27011 ) & 0x7fffffff ) % HASH_SIZE )

colorhist_vector
  ppm_computecolorhist( pixels, cols, rows, maxcolors, colorsP )
pixel** pixels;
int cols, rows, maxcolors;
int* colorsP;
{
  colorhash_table cht;
  colorhist_vector chv;

  cht = ppm_computecolorhash( pixels, cols, rows, maxcolors, colorsP );
  if ( cht == (colorhash_table) 0 )
    return((colorhist_vector) 0);
  chv = ppm_colorhashtocolorhist( cht, maxcolors );
  ppm_freecolorhash( cht );
  return(chv);
}

void
  ppm_addtocolorhist( chv, colorsP, maxcolors, colorP, value, position )
colorhist_vector chv;
pixel* colorP;
int* colorsP;
int maxcolors, value, position;
{
  int i, j;

  /* Search colorhist for the color. */
  for ( i = 0; i < *colorsP; ++i )
    if ( PPM_EQUAL( chv[i].color, *colorP ) )
      {
	/* Found it - move to new slot. */
	if ( position > i )
	  {
	    for ( j = i; j < position; ++j )
	      chv[j] = chv[j + 1];
	  }
	else if ( position < i )
	  {
	    for ( j = i; j > position; --j )
	      chv[j] = chv[j - 1];
	  }
	chv[position].color = *colorP;
	chv[position].value = value;
	return;
      }
  if ( *colorsP < maxcolors )
    {
      /* Didn't find it, but there's room to add it; so do so. */
      for ( i = *colorsP; i > position; --i )
	chv[i] = chv[i - 1];
      chv[position].color = *colorP;
      chv[position].value = value;
      ++(*colorsP);
    }
}

colorhash_table
  ppm_computecolorhash( pixels, cols, rows, maxcolors, colorsP )
pixel** pixels;
int cols, rows, maxcolors;
int* colorsP;
{
  colorhash_table cht;
  register pixel* pP;
  colorhist_list chl;
  int col, row, hash;

  cht = ppm_alloccolorhash( );
  if ( cht == (colorhash_table) 0 )
    return((colorhash_table) 0);
  *colorsP = 0;

  /* Go through the entire image, building a hash table of colors. */
  for ( row = 0; row < rows; ++row )
    for ( col = 0, pP = pixels[row]; col < cols; ++col, ++pP )
      {
	hash = ppm_hashpixel( *pP );
	for ( chl = cht[hash]; chl != (colorhist_list) 0; chl = chl->next )
	  if ( PPM_EQUAL( chl->ch.color, *pP ) )
	    break;
	if ( chl != (colorhist_list) 0 )
	  ++(chl->ch.value);
	else
	  {
	    if ( ++(*colorsP) > maxcolors )
	      {
		ppm_freecolorhash( cht );
		return((colorhash_table) 0);
	      }
	    chl = (colorhist_list) malloc( sizeof(struct colorhist_list_item) );
	    if ( chl == 0 )
	      {
		(void) io_printf(
			       stderr, "%s: out of memory computing hash table\n",
			       progname );
		ppm_freecolorhash( cht );
		return((colorhash_table) 0);
	      }
	    chl->ch.color = *pP;
	    chl->ch.value = 1;
	    chl->next = cht[hash];
	    cht[hash] = chl;
	  }
      }
    
  return(cht);
}

colorhash_table
  ppm_alloccolorhash( )
{
  colorhash_table cht;
  int i;

  cht = (colorhash_table) malloc( HASH_SIZE * sizeof(colorhist_list) );
  if ( cht == 0 )
    {
      (void) io_printf(
		     stderr, "%s: out of memory allocating hash table\n", progname );
      return((colorhash_table) 0);
    }

  for ( i = 0; i < HASH_SIZE; ++i )
    cht[i] = (colorhist_list) 0;

  return(cht);
}

int
  ppm_addtocolorhash( cht, colorP, value )
colorhash_table cht;
pixel* colorP;
int value;
{
  register int hash;
  register colorhist_list chl;

  chl = (colorhist_list) malloc( sizeof(struct colorhist_list_item) );
  if ( chl == 0 )
    return(-1);
  hash = ppm_hashpixel( *colorP );
  chl->ch.color = *colorP;
  chl->ch.value = value;
  chl->next = cht[hash];
  cht[hash] = chl;
  return(0);
}

colorhist_vector
  ppm_colorhashtocolorhist( cht, maxcolors )
colorhash_table cht;
int maxcolors;
{
  colorhist_vector chv;
  colorhist_list chl;
  int i, j;

  /* Now collate the hash table into a simple colorhist array. */
  chv = (colorhist_vector) malloc( maxcolors * sizeof(struct colorhist_item) );
  /* (Leave room for expansion by caller.) */
  if ( chv == (colorhist_vector) 0 )
    {
      (void) io_printf(
		     stderr, "%s: out of memory generating histogram\n", progname );
      return((colorhist_vector) 0);
    }

  /* Loop through the hash table. */
  j = 0;
  for ( i = 0; i < HASH_SIZE; ++i )
    for ( chl = cht[i]; chl != (colorhist_list) 0; chl = chl->next )
      {
	/* Add the new entry. */
	chv[j] = chl->ch;
	++j;
      }

  /* All done. */
  return(chv);
}

colorhash_table
  ppm_colorhisttocolorhash( chv, colors )
colorhist_vector chv;
int colors;
{
  colorhash_table cht;
  int i, hash;
  pixel color;
  colorhist_list chl;

  cht = ppm_alloccolorhash( );
  if ( cht == (colorhash_table) 0 )
    return((colorhash_table) 0);

  for ( i = 0; i < colors; ++i )
    {
      color = chv[i].color;
      hash = ppm_hashpixel( color );
      for ( chl = cht[hash]; chl != (colorhist_list) 0; chl = chl->next )
	if ( PPM_EQUAL( chl->ch.color, color ) )
	  {
	    (void) io_printf(
			   stderr, "%s: same color found twice - %d %d %d\n", progname,
			   PPM_GETR(color), PPM_GETG(color), PPM_GETB(color) );
	    ppm_freecolorhash( cht );
	    return((colorhash_table) 0);
	  }
      chl = (colorhist_list) malloc( sizeof(struct colorhist_list_item) );
      if ( chl == (colorhist_list) 0 )
	{
	  (void) io_printf( stderr, "%s: out of memory\n", progname );
	  ppm_freecolorhash( cht );
	  return((colorhash_table) 0);
	}
      chl->ch.color = color;
      chl->ch.value = i;
      chl->next = cht[hash];
      cht[hash] = chl;
    }

  return(cht);
}

int ppm_lookupcolor(colorhash_table cht, pixel *colorP)
   {int hash;
    colorhist_list chl;

    hash = ppm_hashpixel(*colorP);
    for (chl = cht[hash]; chl != (colorhist_list) 0; chl = chl->next)
        if (PPM_EQUAL(chl->ch.color, *colorP))
	   return(chl->ch.value);

    return(-1);}

void ppm_freecolorhist(colorhist_vector chv)
   {

    free((char *) chv);

    return;}

void ppm_freecolorhash(colorhash_table cht)
   {int i;
    colorhist_list chl, chlnext;

    for (i = 0; i < HASH_SIZE; ++i)
        for (chl = cht[i]; chl != (colorhist_list) 0; chl = chlnext)
	    {chlnext = chl->next;
	     free((char *) chl);};

    free((char *) cht);

    return;}


/* added from libpnm3.c: */

/* libpnm3.c - pnm utility library part 3
 **
 ** Copyright (C) 1989, 1991 by Jef Poskanzer.
 **
 ** Permission to use, copy, modify, and distribute this software and its
 ** documentation for any purpose and without fee is hereby granted, provided
 ** that the above copyright notice appear in all copies and that both that
 ** copyright notice and this permission notice appear in supporting
 ** documentation.  This software is provided "as is" without express or
 ** implied warranty.
 */

#if __STDC__
xel
  pnm_backgroundxel( xel** xels, int cols, int rows, xelval maxval, int format )
#else /*__STDC__*/
xel
  pnm_backgroundxel( xels, cols, rows, maxval, format )
xel** xels;
int cols, rows, format;
xelval maxval;
#endif /*__STDC__*/
{
  xel bgxel, ul, ur, ll, lr;

  /* Guess a good background value. */
  ul = xels[0][0];
  ur = xels[0][cols-1];
  ll = xels[rows-1][0];
  lr = xels[rows-1][cols-1];

  /* First check for three corners equal. */
  if ( PNM_EQUAL( ul, ur ) && PNM_EQUAL( ur, ll ) )
    bgxel = ul;
  else if ( PNM_EQUAL( ul, ur ) && PNM_EQUAL( ur, lr ) )
    bgxel = ul;
  else if ( PNM_EQUAL( ul, ll ) && PNM_EQUAL( ll, lr ) )
    bgxel = ul;
  else if ( PNM_EQUAL( ur, ll ) && PNM_EQUAL( ll, lr ) )
    bgxel = ur;
  /* Nope, check for two corners equal. */
  else if ( PNM_EQUAL( ul, ur ) || PNM_EQUAL( ul, ll ) ||
	   PNM_EQUAL( ul, lr ) )
    bgxel = ul;
  else if ( PNM_EQUAL( ur, ll ) || PNM_EQUAL( ur, lr ) )
    bgxel = ur;
  else if ( PNM_EQUAL( ll, lr ) )
    bgxel = ll;
  else
    {
      /* Nope, we have to average the four corners.  This breaks the
       ** rules of pnm, but oh well.  Let's try to do it portably. */
      switch ( PNM_FORMAT_TYPE(format) )
	{
	case PPM_TYPE:
	  PPM_ASSIGN( bgxel,
		     PPM_GETR(ul) + PPM_GETR(ur) + PPM_GETR(ll) + PPM_GETR(lr) / 4,
		     PPM_GETG(ul) + PPM_GETG(ur) + PPM_GETG(ll) + PPM_GETG(lr) / 4,
		     PPM_GETB(ul) + PPM_GETB(ur) + PPM_GETB(ll) + PPM_GETB(lr) / 4 );
	  break;

	case PGM_TYPE:
	  {
	    gray gul, gur, gll, glr;
	    gul = (gray) PNM_GET1( ul );
	    gur = (gray) PNM_GET1( ur );
	    gll = (gray) PNM_GET1( ll );
	    glr = (gray) PNM_GET1( lr );
	    PNM_ASSIGN1( bgxel, ( ( gul + gur + gll + glr ) / 4 ) );
	    break;
	  }

	case PBM_TYPE:
	  pm_error(
		   "pnm_backgroundxel: four bits no two of which equal each other??" );

	default:
	  pm_error( "can't happen" );
	}
    }

  return(bgxel);
}

#if __STDC__
xel
  pnm_backgroundxelrow( xel* xelrow, int cols, xelval maxval, int format )
#else /*__STDC__*/
xel
  pnm_backgroundxelrow( xelrow, cols, maxval, format )
xel* xelrow;
int cols, format;
xelval maxval;
#endif /*__STDC__*/
{
  xel bgxel, l, r;

  /* Guess a good background value. */
  l = xelrow[0];
  r = xelrow[cols-1];

  /* First check for both corners equal. */
  if ( PNM_EQUAL( l, r ) )
    bgxel = l;
  else
    {
      /* Nope, we have to average the two corners.  This breaks the
       ** rules of pnm, but oh well.  Let's try to do it portably. */
      switch ( PNM_FORMAT_TYPE(format) )
	{
	case PPM_TYPE:
	  PPM_ASSIGN( bgxel, PPM_GETR(l) + PPM_GETR(r) / 2,
		     PPM_GETG(l) + PPM_GETG(r) / 2, PPM_GETB(l) + PPM_GETB(r) / 2 );
	  break;

	case PGM_TYPE:
	  {
	    gray gl, gr;
	    gl = (gray) PNM_GET1( l );
	    gr = (gray) PNM_GET1( r );
	    PNM_ASSIGN1( bgxel, ( ( gl + gr ) / 2 ) );
	    break;
	  }

	case PBM_TYPE:
	  {
	    int col, blacks;

	    /* One black, one white.  Gotta count. */
	    for ( col = 0, blacks = 0; col < cols; ++col )
	      {
		if ( PNM_GET1( xelrow[col] ) == 0 )
		  ++blacks;
	      }
	    if ( blacks >= cols / 2 )
	      PNM_ASSIGN1( bgxel, 0 );
	    else
	      PNM_ASSIGN1( bgxel, pnm_pbmmaxval );
	    break;
	  }

	default:
	  pm_error( "can't happen" );
	}
    }

  return(bgxel);
}

#if __STDC__
xel
  pnm_whitexel( xelval maxval, int format )
#else /*__STDC__*/
xel
  pnm_whitexel( maxval, format )
xelval maxval;
int format;
#endif /*__STDC__*/
{
  xel x;

  switch ( PNM_FORMAT_TYPE(format) )
    {
    case PPM_TYPE:
      PPM_ASSIGN( x, maxval, maxval, maxval );
      break;

    case PGM_TYPE:
      PNM_ASSIGN1( x, maxval );
      break;

    case PBM_TYPE:
      PNM_ASSIGN1( x, pnm_pbmmaxval );
      break;

    default:
      pm_error( "can't happen" );
    }

  return(x);
}

#if __STDC__
xel
  pnm_blackxel( xelval maxval, int format )
#else /*__STDC__*/
xel
  pnm_blackxel( maxval, format )
xelval maxval;
int format;
#endif /*__STDC__*/
{
  xel x;

  switch ( PNM_FORMAT_TYPE(format) )
    {
    case PPM_TYPE:
      PPM_ASSIGN( x, 0, 0, 0 );
      break;

    case PGM_TYPE:
      PNM_ASSIGN1( x, (xelval) 0 );
      break;

    case PBM_TYPE:
      PNM_ASSIGN1( x, (xelval) 0 );
      break;

    default:
      pm_error( "can't happen" );
    }

  return(x);
}

#if __STDC__
void
  pnm_invertxel( xel* xP, xelval maxval, int format )
#else /*__STDC__*/
void
  pnm_invertxel( xP, maxval, format )
xel* xP;
xelval maxval;
int format;
#endif /*__STDC__*/
{
  switch ( PNM_FORMAT_TYPE(format) )
    {
    case PPM_TYPE:
      PPM_ASSIGN(
		 *xP, maxval - PPM_GETR( *xP ),
		 maxval - PPM_GETG( *xP ), maxval - PPM_GETB( *xP ) );
      break;

    case PGM_TYPE:
      PNM_ASSIGN1( *xP, (gray) maxval - (gray) PNM_GET1( *xP ) );
      break;

    case PBM_TYPE:
      PNM_ASSIGN1( *xP, ( PNM_GET1( *xP ) == 0 ) ? pnm_pbmmaxval : 0 );
      break;

    default:
      pm_error( "can't happen" );
    }
}

void
  pnm_promoteformat( xel** xels, int cols, int rows, xelval maxval, int format, xelval newmaxval, int newformat )
{
  int row;

  for ( row = 0; row < rows; ++row )
    pnm_promoteformatrow(
			 xels[row], cols, maxval, format, newmaxval, newformat );
}

void
  pnm_promoteformatrow( xel* xelrow, int cols, xelval maxval, int format, xelval newmaxval, int newformat )
{
  register int col;
  register xel* xP;

  if ( ( PNM_FORMAT_TYPE(format) == PPM_TYPE &&
	( PNM_FORMAT_TYPE(newformat) == PGM_TYPE ||
	 PNM_FORMAT_TYPE(newformat) == PBM_TYPE ) ) ||
      ( PNM_FORMAT_TYPE(format) == PGM_TYPE &&
       PNM_FORMAT_TYPE(newformat) == PBM_TYPE ) )
    pm_error( "pnm_promoteformatrow: can't promote downwards!" );

  /* Are we promoting to the same type? */
  if ( PNM_FORMAT_TYPE(format) == PNM_FORMAT_TYPE(newformat) )
    {
      if ( PNM_FORMAT_TYPE(format) == PBM_TYPE )
	return;
      if ( newmaxval < maxval )
	pm_error(
		 "pnm_promoteformatrow: can't decrease maxval - try using pnmdepth" );
      if ( newmaxval == maxval )
	return;
      /* Increase maxval. */
      switch ( PNM_FORMAT_TYPE(format) )
	{
	case PGM_TYPE:
	  for ( col = 0, xP = xelrow; col < cols; ++col, ++xP )
	    PNM_ASSIGN1(
			*xP, (int) PNM_GET1(*xP) * newmaxval / maxval );
	  break;

	case PPM_TYPE:
	  for ( col = 0, xP = xelrow; col < cols; ++col, ++xP )
	    PPM_DEPTH( *xP, *xP, maxval, newmaxval );
	  break;

	default:
	  pm_error( "shouldn't happen" );
	}
      return;
    }

  /* We must be promoting to a higher type. */
  switch ( PNM_FORMAT_TYPE(format) )
    {
    case PBM_TYPE:
      switch ( PNM_FORMAT_TYPE(newformat) )
	{
	case PGM_TYPE:
	  for ( col = 0, xP = xelrow; col < cols; ++col, ++xP )
	    if ( PNM_GET1(*xP) == 0 )
	      PNM_ASSIGN1( *xP, 0 );
	    else
	      PNM_ASSIGN1( *xP, newmaxval );
	  break;

	case PPM_TYPE:
	  for ( col = 0, xP = xelrow; col < cols; ++col, ++xP )
	    if ( PNM_GET1(*xP) == 0 )
	      PPM_ASSIGN( *xP, 0, 0, 0 );
	    else
	      PPM_ASSIGN( *xP, newmaxval, newmaxval, newmaxval );
	  break;

	default:
	  pm_error( "can't happen" );
	}
      break;

    case PGM_TYPE:
      switch ( PNM_FORMAT_TYPE(newformat) )
	{
	case PPM_TYPE:
	  if ( newmaxval < maxval )
	    pm_error(
		     "pnm_promoteformatrow: can't decrease maxval - try using pnmdepth" );
	  if ( newmaxval == maxval )
	    {
	      for ( col = 0, xP = xelrow; col < cols; ++col, ++xP )
		PPM_ASSIGN(
			   *xP, PNM_GET1(*xP), PNM_GET1(*xP), PNM_GET1(*xP) );
	    }
	  else
	    {			/* Increase maxval. */
	      for ( col = 0, xP = xelrow; col < cols; ++col, ++xP )
		PPM_ASSIGN(
			   *xP, (int) PNM_GET1(*xP) * newmaxval / maxval,
			   (int) PNM_GET1(*xP) * newmaxval / maxval,
			   (int) PNM_GET1(*xP) * newmaxval / maxval );
	    }
	  break;

	default:
	  pm_error( "can't happen" );
	}
      break;

    default:
      pm_error( "can't happen" );
    }
}



/*
 * mfwddct.c (derived from jfwddct.c, which carries the following info)
 *
 * Copyright (C) 1991, 1992, Thomas G. Lane. This file is part of the
 * Independent JPEG Group's software. For conditions of distribution and use,
 * see the accompanying README file.
 *
 * This file contains the basic DCT (Discrete Cosine Transform) transformation
 * subroutine.
 *
 * This implementation is based on Appendix A.2 of the book "Discrete Cosine
 * Transform---Algorithms, Advantages, Applications" by K.R. Rao and P. Yip
 * (Academic Press, Inc, London, 1990). It uses scaled fixed-point arithmetic
 * instead of floating point.
 */

#if 0
/*NOTE*/
#include "all.h"

#include "dct.h"
#include "mtypes.h"
#include "opts.h"
/*NOTE*/
#endif

/*
 * The poop on this scaling stuff is as follows:
 *
 * We have to do addition and subtraction of the integer inputs, which is no
 * problem, and multiplication by fractional constants, which is a problem to
 * do in integer arithmetic.  We multiply all the constants by DCT_SCALE and
 * convert them to integer constants (thus retaining LG2_DCT_SCALE bits of
 * precision in the constants).  After doing a multiplication we have to
 * divide the product by DCT_SCALE, with proper rounding, to produce the
 * correct output.  The division can be implemented cheaply as a right shift
 * of LG2_DCT_SCALE bits.  The DCT equations also specify an additional
 * division by 2 on the final outputs; this can be folded into the
 * right-shift by shifting one more bit (see UNFIXH).
 *
 * If you are planning to recode this in assembler, you might want to set
 * LG2_DCT_SCALE to 15.  This loses a bit of precision, but then all the
 * multiplications are between 16-bit quantities (given 8-bit JSAMPLEs!) so
 * you could use a signed 16x16=>32 bit multiply instruction instead of full
 * 32x32 multiply.  Unfortunately there's no way to describe such a multiply
 * portably in C, so we've gone for the extra bit of accuracy here.
 */

#define EIGHT_BIT_SAMPLES
#ifdef EIGHT_BIT_SAMPLES
#define LG2_DCT_SCALE 16
#else
#define LG2_DCT_SCALE 15	/* lose a little precision to avoid overflow */
#endif

#define ONE	((int32) 1)

#define DCT_SCALE (ONE << LG2_DCT_SCALE)

/* In some places we shift the inputs left by a couple more bits, */
/* so that they can be added to fractional results without too much */
/* loss of precision. */
#define LG2_OVERSCALE 2
#define OVERSCALE  (ONE << LG2_OVERSCALE)
#define OVERSHIFT(x)  ((x) <<= LG2_OVERSCALE)

/* Scale a fractional constant by DCT_SCALE */
#define FIX(x)	((int32) ((x) * DCT_SCALE + 0.5))

/* Scale a fractional constant by DCT_SCALE/OVERSCALE */
/* Such a constant can be multiplied with an overscaled input */
/* to produce something that's scaled by DCT_SCALE */
#define FIXO(x)  ((int32) ((x) * DCT_SCALE / OVERSCALE + 0.5))

/* Descale and correctly round a value that's scaled by DCT_SCALE */
#define UNFIX(x)   RIGHT_SHIFT((x) + (ONE << (LG2_DCT_SCALE-1)), LG2_DCT_SCALE)

/* Same with an additional division by 2, ie, correctly rounded UNFIX(x/2) */
#define UNFIXH(x)  RIGHT_SHIFT((x) + (ONE << LG2_DCT_SCALE), LG2_DCT_SCALE+1)

/* Take a value scaled by DCT_SCALE and round to integer scaled by OVERSCALE */
#define UNFIXO(x)  RIGHT_SHIFT((x) + (ONE << (LG2_DCT_SCALE-1-LG2_OVERSCALE)),\
			       LG2_DCT_SCALE-LG2_OVERSCALE)

/* Here are the constants we need */
/* SIN_i_j is sine of i*pi/j, scaled by DCT_SCALE */
/* COS_i_j is cosine of i*pi/j, scaled by DCT_SCALE */

#define SIN_1_4 FIX(0.707106781)
#define COS_1_4 SIN_1_4

#define SIN_1_8 FIX(0.382683432)
#define COS_1_8 FIX(0.923879533)
#define SIN_3_8 COS_1_8
#define COS_3_8 SIN_1_8

#define SIN_1_16 FIX(0.195090322)
#define COS_1_16 FIX(0.980785280)
#define SIN_7_16 COS_1_16
#define COS_7_16 SIN_1_16

#define SIN_3_16 FIX(0.555570233)
#define COS_3_16 FIX(0.831469612)
#define SIN_5_16 COS_3_16
#define COS_5_16 SIN_3_16

/* OSIN_i_j is sine of i*pi/j, scaled by DCT_SCALE/OVERSCALE */
/* OCOS_i_j is cosine of i*pi/j, scaled by DCT_SCALE/OVERSCALE */

#define OSIN_1_4 FIXO(0.707106781)
#define OCOS_1_4 OSIN_1_4

#define OSIN_1_8 FIXO(0.382683432)
#define OCOS_1_8 FIXO(0.923879533)
#define OSIN_3_8 OCOS_1_8
#define OCOS_3_8 OSIN_1_8

#define OSIN_1_16 FIXO(0.195090322)
#define OCOS_1_16 FIXO(0.980785280)
#define OSIN_7_16 OCOS_1_16
#define OCOS_7_16 OSIN_1_16

#define OSIN_3_16 FIXO(0.555570233)
#define OCOS_3_16 FIXO(0.831469612)
#define OSIN_5_16 OCOS_3_16
#define OCOS_5_16 OSIN_3_16

/* Prototypes */
void reference_fwd_dct _ANSI_ARGS_((Block block, Block dest));
void mp_fwd_dct_fast _ANSI_ARGS_((Block data2d, Block dest2d));
void init_fdct _ANSI_ARGS_((void));

/*
 * --------------------------------------------------------------
 *
 * mp_fwd_dct_block2 --
 *
 * Select the appropriate mp_fwd_dct routine
 *
 * Results: None
 *
 * Side effects: None
 *
 * --------------------------------------------------------------
 */
#if 0
/*NOTE*/
extern boolean pureDCT;
/*NOTE*/
#endif
void
mp_fwd_dct_block2(data, dest)
    Block data, dest;
{
  if (pureDCT) reference_fwd_dct(data, dest);
  else mp_fwd_dct_fast(data, dest);
}

/*
 * --------------------------------------------------------------
 *
 * mp_fwd_dct_fast --
 *
 * Perform the forward DCT on one block of samples.
 *
 * A 2-D DCT can be done by 1-D DCT on each row followed by 1-D DCT on each
 * column.
 *
 * Results: None
 *
 * Side effects: Overwrites the input data
 *
 * --------------------------------------------------------------
 */

void
mp_fwd_dct_fast(data2d, dest2d)
    Block data2d, dest2d;
{
    int16 *data = (int16 *) data2d;	/* this algorithm wants
					 * a 1-d array */
    int16 *dest = (int16 *) dest2d;
    int pass, rowctr;
    register int16 *inptr, *outptr;
    int16 workspace[DCTSIZE_SQ];
    SHIFT_TEMPS

#ifdef ndef
    {
	int y;

	printf("fwd_dct (beforehand):\n");
	for (y = 0; y < 8; y++)
	    printf("%4d %4d %4d %4d %4d %4d %4d %4d\n",
		   data2d[y][0], data2d[y][1],
		   data2d[y][2], data2d[y][3],
		   data2d[y][4], data2d[y][5],
		   data2d[y][6], data2d[y][7]);
    }
#endif

    /*
     * Each iteration of the inner loop performs one 8-point 1-D DCT. It
     * reads from a *row* of the input matrix and stores into a *column*
     * of the output matrix.  In the first pass, we read from the data[]
     * array and store into the local workspace[].  In the second pass,
     * we read from the workspace[] array and store into data[], thus
     * performing the equivalent of a columnar DCT pass with no variable
     * array indexing.
     */

    inptr = data;		/* initialize pointers for first pass */
    outptr = workspace;
    for (pass = 1; pass >= 0; pass--) {
	for (rowctr = DCTSIZE - 1; rowctr >= 0; rowctr--) {
	    /*
	     * many tmps have nonoverlapping lifetime -- flashy
	     * register colourers should be able to do this lot
	     * very well
	     */
	    int32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	    int32 tmp10, tmp11, tmp12, tmp13;
	    int32 tmp14, tmp15, tmp16, tmp17;
	    int32 tmp25, tmp26;
	    /* SHIFT_TEMPS */

	    /* temp0 through tmp7:  -512 to +512 */
	    /* if I-block, then -256 to +256 */
	    tmp0 = inptr[7] + inptr[0];
	    tmp1 = inptr[6] + inptr[1];
	    tmp2 = inptr[5] + inptr[2];
	    tmp3 = inptr[4] + inptr[3];
	    tmp4 = inptr[3] - inptr[4];
	    tmp5 = inptr[2] - inptr[5];
	    tmp6 = inptr[1] - inptr[6];
	    tmp7 = inptr[0] - inptr[7];

	    /* tmp10 through tmp13:  -1024 to +1024 */
	    /* if I-block, then -512 to +512 */
	    tmp10 = tmp3 + tmp0;
	    tmp11 = tmp2 + tmp1;
	    tmp12 = tmp1 - tmp2;
	    tmp13 = tmp0 - tmp3;

	    outptr[0] = (int16) UNFIXH((tmp10 + tmp11) * SIN_1_4);
	    outptr[DCTSIZE * 4] = (int16) UNFIXH((tmp10 - tmp11) * COS_1_4);

	    outptr[DCTSIZE * 2] = (int16) UNFIXH(tmp13 * COS_1_8 + tmp12 * SIN_1_8);
	    outptr[DCTSIZE * 6] = (int16) UNFIXH(tmp13 * SIN_1_8 - tmp12 * COS_1_8);

	    tmp16 = UNFIXO((tmp6 + tmp5) * SIN_1_4);
	    tmp15 = UNFIXO((tmp6 - tmp5) * COS_1_4);

	    OVERSHIFT(tmp4);
	    OVERSHIFT(tmp7);

	    /*
	     * tmp4, tmp7, tmp15, tmp16 are overscaled by
	     * OVERSCALE
	     */

	    tmp14 = tmp4 + tmp15;
	    tmp25 = tmp4 - tmp15;
	    tmp26 = tmp7 - tmp16;
	    tmp17 = tmp7 + tmp16;

	    outptr[DCTSIZE] = (int16) UNFIXH(tmp17 * OCOS_1_16 + tmp14 * OSIN_1_16);
	    outptr[DCTSIZE * 7] = (int16) UNFIXH(tmp17 * OCOS_7_16 - tmp14 * OSIN_7_16);
	    outptr[DCTSIZE * 5] = (int16) UNFIXH(tmp26 * OCOS_5_16 + tmp25 * OSIN_5_16);
	    outptr[DCTSIZE * 3] = (int16) UNFIXH(tmp26 * OCOS_3_16 - tmp25 * OSIN_3_16);

	    inptr += DCTSIZE;	/* advance inptr to next row */
	    outptr++;		/* advance outptr to next column */
	}
	/* end of pass; in case it was pass 1, set up for pass 2 */
	inptr = workspace;
	outptr = dest;
    }
#ifdef ndef
    {
	int y;

	printf("fwd_dct (afterward):\n");
	for (y = 0; y < 8; y++)
	    printf("%4d %4d %4d %4d %4d %4d %4d %4d\n",
		   dest2d[y][0], dest2d[y][1],
		   dest2d[y][2], dest2d[y][3],
		   dest2d[y][4], dest2d[y][5],
		   dest2d[y][6], dest2d[y][7]);
    }
#endif
}


/* Modifies from the MPEG2 verification coder */
/* fdctref.c, forward discrete cosine transform, double precision           */

/* Copyright (C) 1994, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.14159265358979323846
#endif
#endif

/* private data */
static double trans_coef[8][8]; /* transform coefficients */

void init_fdct(void)
{
  int i, j;
  double s;

  for (i=0; i<8; i++)
  {
    s = (i==0) ? sqrt(0.125) : 0.5;

    for (j=0; j<8; j++)
      trans_coef[i][j] = s * cos((PI/8.0)*i*(j+0.5));
  }
}

void reference_fwd_dct(block, dest)
Block block, dest;
{
  int i, j, k;
  double s;
  double tmp[64];

  if (DoLaplace) {
    LaplaceNum++;
  }

  for (i=0; i<8; i++)
    for (j=0; j<8; j++)
    {
      s = 0.0;

      for (k=0; k<8; k++)
        s += trans_coef[j][k] * block[i][k];

      tmp[8*i+j] = s;
    }

  for (i=0; i<8; i++)
    for (j=0; j<8; j++)
    {
      s = 0.0;

      for (k=0; k<8; k++)
        s += trans_coef[i][k] * tmp[8*k+j];

      if (collect_quant) {
	io_printf(collect_quant_fp, "%d %f\n", 8*i+j, s);
      } 
      if (DoLaplace) {
	L1[LaplaceCnum][i*8+j] += s*s;
	L2[LaplaceCnum][i*8+j] += s;
      }


      dest[i][j] = (int)floor(s+0.499999);
      /*
       * reason for adding 0.499999 instead of 0.5:
       * s is quite often x.5 (at least for i and/or j = 0 or 4)
       * and setting the rounding threshold exactly to 0.5 leads to an
       * extremely high arithmetic implementation dependency of the result;
       * s being between x.5 and x.500001 (which is now incorrectly rounded
       * downwards instead of upwards) is assumed to occur less often
       * (if at all)
       */
    }
}
/*===========================================================================*
 * mheaders.c								     *
 *									     *
 *	Procedures to generate MPEG headers				     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	Mhead_GenPictureHeader						     *
 *	Mhead_GenSequenceHeader						     *
 *	Mhead_GenSequenceEnder						     *
 *	Mhead_GenGOPHeader						     *
 *	Mhead_GenSliceHeader						     *
 *	Mhead_GenSliceEnder						     *
 *	Mhead_GenMBHeader						     *
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
 *  Revision 1.15  1995/08/07 21:45:19  smoot
 *  check for illegal MVs (shouldnt ever be called, but....)
 *  fix bug which made us not weite Iframe Qscale changes
 *  warns if writing a size=0 mpeg
 *
 *  Revision 1.14  1995/05/22 20:53:35  smoot
 *  corrected bit_rate value in constrained params flag
 *
 * Revision 1.13  1995/05/02  01:50:38  eyhung
 * made VidRateNum un-static
 *
 * Revision 1.12  1995/03/27  19:28:23  smoot
 * auto-determines Qscale changes (was mb_quant)
 *
 * Revision 1.11  1995/02/16  09:12:39  eyhung
 * fixed compile bug with HP7xx
 *
 * Revision 1.10  1995/01/25  22:53:50  smoot
 * Better buf_size checking, and actually check constrained params
 *
 * Revision 1.9  1995/01/19  23:08:47  eyhung
 * Changed copyrights
 *
 * Revision 1.8  1995/01/16  08:45:10  eyhung
 * BLEAH'ed hsize and vsize
 *
 * Revision 1.7  1994/12/09  22:27:17  smoot
 * Fixed buffer size in stream
 *
 * Revision 1.6  1994/11/12  02:11:54  keving
 * nothing
 *
 * Revision 1.5  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.4  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.3  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.2  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.1  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.6  1993/03/01  23:03:40  keving
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


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "bitio.h"
#include "frames.h"
#include "mheaders.h"
/*NOTE*/
#endif


/*==================*
 * STATIC VARIABLES *
 *==================*/

static int gopStartFrame = 0;
static int lastGOPStart = 0;
static int lastQSSet;

static u_int32_t mbAddrIncrTable[][2] = {
    {0x0, 0},
    {0x1, 1},
    {0x3, 3},
    {0x2, 3},
    {0x3, 4},
    {0x2, 4},
    {0x3, 5},
    {0x2, 5},
    {0x7, 7},
    {0x6, 7},
    {0xb, 8},
    {0xa, 8},
    {0x9, 8},
    {0x8, 8},
    {0x7, 8},
    {0x6, 8},
    {0x17, 10},
    {0x16, 10},
    {0x15, 10},
    {0x14, 10},
    {0x13, 10},
    {0x12, 10},
    {0x23, 11},
    {0x22, 11},
    {0x21, 11},
    {0x20, 11},
    {0x1f, 11},
    {0x1e, 11},
    {0x1d, 11},
    {0x1c, 11},
    {0x1b, 11},
    {0x1a, 11},
    {0x19, 11},
    {0x18, 11}};

static u_int32_t mbMotionVectorTable[][2] = {
    {0x19, 11},
    {0x1b, 11},
    {0x1d, 11},
    {0x1f, 11},
    {0x21, 11},
    {0x23, 11},
    {0x13, 10},
    {0x15, 10},
    {0x17, 10},
    {0x7, 8},
    {0x9, 8},
    {0xb, 8},
    {0x7, 7},
    {0x3, 5},
    {0x3, 4},
    {0x3, 3},
    {0x1, 1},
    {0x2, 3},
    {0x2, 4},
    {0x2, 5},
    {0x6, 7},
    {0xa, 8},
    {0x8, 8},
    {0x6, 8},
    {0x16, 10},
    {0x14, 10},
    {0x12, 10},
    {0x22, 11},
    {0x20, 11},
    {0x1e, 11},
    {0x1c, 11},
    {0x1a, 11},
    {0x18, 11}};

static u_int32_t mbPatTable[][2] = {
    {0x0, 0},
    {0xb, 5},
    {0x9, 5},
    {0xd, 6},
    {0xd, 4},
    {0x17, 7},
    {0x13, 7},
    {0x1f, 8},
    {0xc, 4},
    {0x16, 7},
    {0x12, 7},
    {0x1e, 8},
    {0x13, 5},
    {0x1b, 8},
    {0x17, 8},
    {0x13, 8},
    {0xb, 4},
    {0x15, 7},
    {0x11, 7},
    {0x1d, 8},
    {0x11, 5},
    {0x19, 8},
    {0x15, 8},
    {0x11, 8},
    {0xf, 6},
    {0xf, 8},
    {0xd, 8},
    {0x3, 9},
    {0xf, 5},
    {0xb, 8},
    {0x7, 8},
    {0x7, 9},
    {0xa, 4},
    {0x14, 7},
    {0x10, 7},
    {0x1c, 8},
    {0xe, 6},
    {0xe, 8},
    {0xc, 8},
    {0x2, 9},
    {0x10, 5},
    {0x18, 8},
    {0x14, 8},
    {0x10, 8},
    {0xe, 5},
    {0xa, 8},
    {0x6, 8},
    {0x6, 9},
    {0x12, 5},
    {0x1a, 8},
    {0x16, 8},
    {0x12, 8},
    {0xd, 5},
    {0x9, 8},
    {0x5, 8},
    {0x5, 9},
    {0xc, 5},
    {0x8, 8},
    {0x4, 8},
    {0x4, 9},
    {0x7, 3},
    {0xa, 5},	/* grrr... 61, 62, 63 added - Kevin */
    {0x8, 5},
    {0xc, 6}
};

/*===========*
 * CONSTANTS *
 *===========*/

#define SEQ_HEAD_CODE 0x000001b3
#define EXT_START_CODE 0x000001b5
#define USER_START_CODE 0x000001b2
#define GOP_START_CODE 0x000001b8
#define PICT_START_CODE 0x00000100
#define SLICE_BASE_CODE 0x00000100

#define SEQ_END_CODE	0x000001b7

/* not static anymore because information is used for computing frame rate 
 * and for statistics */
CONST double VidRateNum[9]={1.0, 23.976, 24.0, 25.0, 29.97, 30.0,
                             50.0 ,59.94, 60.0};


/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static void	GenMBAddrIncr _ANSI_ARGS_((BitBucket *bb, u_int32_t addr_incr));
static void	GenPictHead _ANSI_ARGS_((BitBucket *bb, u_int32_t temp_ref,
		    u_int32_t code_type, u_int32_t vbv_delay,
		    int32 full_pel_forw_flag, u_int32_t forw_f_code,
		    int32 full_pel_back_flag, u_int32_t back_f_code,
		    u_int8_t *extra_info, u_int32_t extra_info_size,
		    u_int8_t *ext_data, u_int32_t ext_data_size,
		    u_int8_t *user_data, u_int32_t user_data_size));
static void	GenMBType _ANSI_ARGS_((BitBucket *bb, u_int32_t pict_code_type,
		  u_int32_t mb_quant, u_int32_t motion_forw, u_int32_t motion_back,
		  u_int32_t mb_pattern, u_int32_t mb_intra));
static void	GenMotionCode _ANSI_ARGS_((BitBucket *bb, int32 vector));
static void	GenBlockPattern _ANSI_ARGS_((BitBucket *bb,
					     u_int32_t mb_pattern));


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * SetGOPStartTime
 *
 *	sets the start frame of the GOP; to be used with GenPictureHeader
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
SetGOPStartTime(index)
    int index;
{
    lastGOPStart = gopStartFrame;
    gopStartFrame = index;
}


/*===========================================================================*
 *
 * Mhead_GenPictureHeader
 *
 *	generate picture header with given frame type and picture count
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mhead_GenPictureHeader(bbPtr, frameType, pictCount, f_code)
    BitBucket *bbPtr;
    int frameType;
    int pictCount;
    int f_code;
{
    int	    temporalRef;

    if ( pictCount >= gopStartFrame ) {
	temporalRef = (pictCount-gopStartFrame);
    } else {
	temporalRef = (pictCount-lastGOPStart);
    }
    temporalRef = (temporalRef % 1024);
	
    DBG_PRINT(("Picture Header\n"));
    GenPictHead(bbPtr, temporalRef, frameType,
		0 /* vbv_delay */,
		pixelFullSearch /* full_pel_forw_flag */,
		f_code /* forw_f_code */,
		pixelFullSearch /* full_pel_back_flag */,
		f_code /* back_f_code */,
		NULL, 0, NULL, 0, NULL, 0);
}


/*===========================================================================*
 *
 * Mhead_GenSequenceHeader
 *
 *	generate sequence header with given attributes
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mhead_GenSequenceHeader(bbPtr, hsize, vsize, pratio, pict_rate, bit_rate,
			buf_size, c_param_flag, iq_matrix, niq_matrix,
			ext_data, ext_data_size, user_data, user_data_size)
    BitBucket *bbPtr;
    u_int32_t hsize;
    u_int32_t vsize;
    int32 pratio;
    int32 pict_rate;
    int32 bit_rate;
    int32 buf_size;
    int32 c_param_flag;
    int32 *iq_matrix;
    int32 *niq_matrix;
    u_int8_t *ext_data;
    int32 ext_data_size;
    char *user_data; /*NOTE: changed "unit8" to "char" NOTE*/
    int32 user_data_size;
{
    extern int ZAG[];
    int i;

    /* Write seq start code. */

    Bitio_Write(bbPtr, SEQ_HEAD_CODE, 32);

    /* Write horiz. and vert. sizes. */

#ifdef BLEAH
io_printf(stdout, "hsize, vsize = %d, %d\n", hsize, vsize);
#endif

    if (hsize==0 || vsize==0) {
      io_printf(stderr, "Writing zero size to stream!\n");
    }
    Bitio_Write(bbPtr, hsize, 12);
    Bitio_Write(bbPtr, vsize, 12);

    /* Write pixel aspect ratio, negative values default to 1. */

    if (pratio < 0) {
	io_printf(stderr, "PROGRAMMER ERROR:  pratio = %d\n", pratio);
	exit(1);
    }
    Bitio_Write(bbPtr, pratio, 4);

    /* Wrtie picture rate, negative values default to 30 fps. */

    if (pict_rate < 0) {
	io_printf(stderr, "PROGRAMMER ERROR:  pict_rate = %d\n", pict_rate);
	exit(1);
    }
    Bitio_Write(bbPtr, pict_rate, 4);

    /* Write bit rate, negative values default to variable. */

    if (bit_rate < 0) {
	bit_rate = -1;
    } else {
	bit_rate = bit_rate / 400;
    }

    Bitio_Write(bbPtr, bit_rate, 18);

    /* Marker bit. */
    Bitio_Write(bbPtr, 0x1, 1);

    /* Write VBV buffer size. Negative values default to zero. */
    if (buf_size < 0) {
	buf_size = 0;
    }

    buf_size = (buf_size + (16*1024 - 1)) / (16*1024);
    if (buf_size>=0x400) buf_size=0x3ff;
    Bitio_Write(bbPtr, buf_size, 10);

    /* Write constrained parameter flag. */
    {
      int num_mb = ((hsize+15)/16) * ((vsize+15)/16);
      /* At present we cheat on buffer size */
      c_param_flag = ((bit_rate <= 4640) &&
                    (bit_rate >0) &&
                    (buf_size <= 20) &&
                    (pict_rate >= 1) &&
                    (pict_rate <= 5) &&
                    (hsize <= 768) &&
                    (vsize <= 576) &&
                    (num_mb <= 396) &&
                    (num_mb*VidRateNum[pict_rate] <= 9900) &&
                    (fCodeP<=4) &&
                    (fCodeB<=4));
    }

    if (c_param_flag) {
	Bitio_Write(bbPtr, 0x01, 1);
    } else {
	Bitio_Write(bbPtr, 0x00, 1);
    }

    /* Write intra quant matrix if present. */

    if (iq_matrix != NULL) {
	Bitio_Write(bbPtr, 0x01, 1);
	for (i = 0; i < 64; i++) {
	    Bitio_Write(bbPtr, iq_matrix[ZAG[i]], 8);
	}
    } else {
	Bitio_Write(bbPtr, 0x00, 1);
    }

    /* Write non intra quant matrix if present. */

    if (niq_matrix != NULL) {
	Bitio_Write(bbPtr, 0x01, 1);
	for (i = 0; i < 64; i++) {
	    Bitio_Write(bbPtr, niq_matrix[ZAG[i]], 8);
	}
    } else {
	Bitio_Write(bbPtr, 0x00, 1);
    }

    /* next start code */
    Bitio_BytePad(bbPtr);


    /* Write ext data if present. */

    if (ext_data != NULL) {
	Bitio_Write(bbPtr, EXT_START_CODE, 32);

	for (i = 0; i < ext_data_size; i++) {
	    Bitio_Write(bbPtr, ext_data[i], 8);
	}
	Bitio_BytePad(bbPtr);
    }
    /* Write user data if present. */
    if ((user_data != NULL) && (user_data_size != 0)) {
	Bitio_Write(bbPtr, USER_START_CODE, 32);

	for (i = 0; i < user_data_size; i++) {
	    Bitio_Write(bbPtr, user_data[i], 8);
	}
	Bitio_BytePad(bbPtr);
    }
}


/*===========================================================================*
 *
 * Mhead_GenSequenceEnder
 *
 *	generate sequence ender
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mhead_GenSequenceEnder(bbPtr)
    BitBucket *bbPtr;
{
    Bitio_Write(bbPtr, SEQ_END_CODE, 32);
}


/*===========================================================================*
 *
 * Mhead_GenGOPHeader
 *
 *	generate GOP header with specified attributes
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mhead_GenGOPHeader(bbPtr, drop_frame_flag, tc_hrs, tc_min, tc_sec, tc_pict,
		   closed_gop, broken_link, ext_data, ext_data_size,
		   user_data, user_data_size)
    BitBucket *bbPtr;
    int32 drop_frame_flag;
    int32 tc_hrs;
    int32 tc_min;
    int32 tc_sec;
    int32 tc_pict;
    int32 closed_gop;
    int32 broken_link;
    u_int8_t *ext_data;
    int32 ext_data_size;
    u_int8_t *user_data;
    int32 user_data_size;
{
    int i;

    /* Write gop start code. */
    Bitio_Write(bbPtr, GOP_START_CODE, 32);

		/* Construct and write timecode. */

    /* Drop frame flag. */
    if (drop_frame_flag) {
	Bitio_Write(bbPtr, 0x01, 1);
    } else {
	Bitio_Write(bbPtr, 0x00, 1);
    }

    /* Time code hours. */
    Bitio_Write(bbPtr, tc_hrs, 5);

    /* Time code minutes. */
    Bitio_Write(bbPtr, tc_min, 6);

    /* Marker bit. */
    Bitio_Write(bbPtr, 0x01, 1);

    /* Time code seconds. */
    Bitio_Write(bbPtr, tc_sec, 6);

    /* Time code pictures. */
    Bitio_Write(bbPtr, tc_pict, 6);


    /* Closed gop flag. */
    if (closed_gop) {
	Bitio_Write(bbPtr, 0x01, 1);
    } else {
	Bitio_Write(bbPtr, 0x00, 1);
    }

    /* Broken link flag. */
    if (broken_link) {
	Bitio_Write(bbPtr, 0x01, 1);
    } else {
	Bitio_Write(bbPtr, 0x00, 1);
    }

    /* next start code */
    Bitio_BytePad(bbPtr);

    /* Write ext data if present. */

    if (ext_data != NULL) {
	Bitio_Write(bbPtr, EXT_START_CODE, 32);

	for (i = 0; i < ext_data_size; i++) {
	    Bitio_Write(bbPtr, ext_data[i], 8);
	}
	Bitio_BytePad(bbPtr);
    }
    /* Write user data if present. */
    if (user_data != NULL) {
	Bitio_Write(bbPtr, USER_START_CODE, 32);

	for (i = 0; i < user_data_size; i++) {
	    Bitio_Write(bbPtr, user_data[i], 8);
	}
	Bitio_BytePad(bbPtr);
    }
}


/*===========================================================================*
 *
 * Mhead_GenSliceHeader
 *
 *	generate slice header with specified attributes
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mhead_GenSliceHeader(bbPtr, verticalPos, qscale, extra_info, extra_info_size)
    BitBucket *bbPtr;
    u_int32_t verticalPos;
    u_int32_t qscale;
    u_int8_t *extra_info;
    u_int32_t extra_info_size;
{
    int i;

    /* Write slice start code. */
    Bitio_Write(bbPtr, (SLICE_BASE_CODE + verticalPos), 32);

    /* Quant. scale. */
    Bitio_Write(bbPtr, qscale, 5);
    lastQSSet = qscale;

    /* Extra bit slice info. */

    if (extra_info != NULL) {
	for (i = 0; i < extra_info_size; i++) {
	    Bitio_Write(bbPtr, 0x01, 1);
	    Bitio_Write(bbPtr, extra_info[i], 8);
	}
    }

    /* extra_bit_slice */
    Bitio_Write(bbPtr, 0x00, 1);
}


/*===========================================================================*
 *
 * Mhead_GenSliceEnder
 *
 *	generate slice ender
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mhead_GenSliceEnder(bbPtr)
    BitBucket *bbPtr;
{
    Bitio_BytePad(bbPtr);
}


/*===========================================================================*
 *
 * Mhead_GenMBHeader
 *
 *	generate macroblock header with given attributes
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mhead_GenMBHeader(bbPtr, pict_code_type, addr_incr, q_scale,
		  forw_f_code, back_f_code, horiz_forw_r, vert_forw_r,
		  horiz_back_r, vert_back_r, motion_forw, m_horiz_forw,
		  m_vert_forw, motion_back, m_horiz_back, m_vert_back,
		  mb_pattern, mb_intra)
    BitBucket *bbPtr;
    u_int32_t pict_code_type;
    u_int32_t addr_incr;
    u_int32_t q_scale;
    u_int32_t forw_f_code;
    u_int32_t back_f_code;
    u_int32_t horiz_forw_r;
    u_int32_t vert_forw_r;
    u_int32_t horiz_back_r;
    u_int32_t vert_back_r;
    int32 motion_forw;
    int32 m_horiz_forw;
    int32 m_vert_forw;
    int32 motion_back;
    int32 m_horiz_back;
    int32 m_vert_back;
    u_int32_t mb_pattern;
    u_int32_t mb_intra;
{
    u_int32_t mb_quant;

    /* MB escape sequences if necessary. */

#ifdef BLEAH
if ( addr_incr != 1 )
    io_printf(stdout, "Creating MB_INCR:  %d\n", addr_incr);
#endif

    while (addr_incr > 33) {
	Bitio_Write(bbPtr, 0x008, 11);
	addr_incr -= 33;
    }

    /* Generate addr incr code. */
    GenMBAddrIncr(bbPtr, addr_incr);

    /* Determine mb_quant  (true if change in q scale) */
    if ((q_scale != lastQSSet) && ((mb_pattern != 0) || (mb_intra == TRUE))) {
      mb_quant = TRUE;
      lastQSSet = q_scale;
    } else {
      mb_quant = FALSE;
    }

    /* Generate mb type code. */
    GenMBType(bbPtr, pict_code_type, mb_quant, motion_forw, motion_back, mb_pattern, mb_intra);

    /* MB quant. */
    if (mb_quant) {
	Bitio_Write(bbPtr, q_scale, 5);
    }
    /* Forward predictive vector stuff. */

    if (motion_forw) {
	int forw_f, forw_r_size;

	forw_r_size = forw_f_code - 1;
	forw_f = 1 << forw_r_size;	/* 1 > 0 */
	if ((m_horiz_forw > 16*forw_f-1) || (m_horiz_forw < -16*forw_f)) {
	  io_printf(stderr, "Illegal motion? %d %d\n", m_horiz_forw, 16*forw_f);
	}
	if ((m_vert_forw > 16*forw_f-1) || (m_vert_forw < -16*forw_f)) {
	  io_printf(stderr, "Illegal motion? %d %d\n", m_vert_forw, 16*forw_f);
	}
	GenMotionCode(bbPtr, m_horiz_forw);

	if ((forw_f != 1) && (m_horiz_forw != 0)) {
	    Bitio_Write(bbPtr, horiz_forw_r, forw_r_size);
	}
	GenMotionCode(bbPtr, m_vert_forw);

	if ((forw_f != 1) && (m_vert_forw != 0)) {
	    Bitio_Write(bbPtr, vert_forw_r, forw_r_size);
	}
    }
    /* Back predicted vector stuff. */

    if (motion_back) {
	int back_f, back_r_size;

	back_r_size = back_f_code - 1;
	back_f = 1 << back_r_size;	/* 1 > 0 */

	if ((m_horiz_back > 16*back_f-1) || (m_horiz_back < -16*back_f)) {
	  io_printf(stderr, "Illegal motion? %d %d\n", m_horiz_back, 16*back_f);
	}
	if ((m_vert_back > 16*back_f-1) || (m_vert_back < -16*back_f)) {
	  io_printf(stderr, "Illegal motion? %d %d\n", m_vert_back, 16*back_f);
	}

	GenMotionCode(bbPtr, m_horiz_back);

	if ((back_f != 1) && (m_horiz_back != 0)) {
	    Bitio_Write(bbPtr, horiz_back_r, back_r_size);
	}
	GenMotionCode(bbPtr, m_vert_back);

	if ((back_f != 1) && (m_vert_back != 0)) {
	    Bitio_Write(bbPtr, vert_back_r, back_r_size);
	}
    }
    /* MB pattern. */

    if (mb_pattern) {
	GenBlockPattern(bbPtr, mb_pattern);
    }
}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * GenMBType
 *
 *	generate macroblock type with given attributes
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
GenMBType(bbPtr, pict_code_type, mb_quant, motion_forw, motion_back,
	  mb_pattern, mb_intra)
    BitBucket *bbPtr;
    u_int32_t pict_code_type;
    u_int32_t mb_quant;
    u_int32_t motion_forw;
    u_int32_t motion_back;
    u_int32_t mb_pattern;
    u_int32_t mb_intra;
{
    int code;

    switch (pict_code_type) {
    case 1:
	if ((motion_forw != 0) || (motion_back != 0) || (mb_pattern != 0) || (mb_intra != 1)) {
	    perror("Illegal parameters for macroblock type.");
	    exit(-1);
	}
	if (mb_quant) {
	    Bitio_Write(bbPtr, 0x1, 2);
	} else {
	    Bitio_Write(bbPtr, 0x1, 1);
	}
	break;

    case 2:
	code = 0;
	if (mb_quant) {
	    code += 16;
	}
	if (motion_forw) {
	    code += 8;
	}
	if (motion_back) {
	    code += 4;
	}
	if (mb_pattern) {
	    code += 2;
	}
	if (mb_intra) {
	    code += 1;
	}

	switch (code) {
	case 1:
	    Bitio_Write(bbPtr, 0x3, 5);
	    break;
	case 2:
	    Bitio_Write(bbPtr, 0x1, 2);
	    break;
	case 8:
	    Bitio_Write(bbPtr, 0x1, 3);
	    break;
	case 10:
	    Bitio_Write(bbPtr, 0x1, 1);
	    break;
	case 17:
	    Bitio_Write(bbPtr, 0x1, 6);
	    break;
	case 18:
	    Bitio_Write(bbPtr, 0x1, 5);
	    break;
	case 26:
	    Bitio_Write(bbPtr, 0x2, 5);
	    break;
	default:
	    perror("Illegal parameters for macroblock type.");
	    exit(-1);
	    break;
	}
	break;

    case 3:
	code = 0;
	if (mb_quant) {
	    code += 16;
	}
	if (motion_forw) {
	    code += 8;
	}
	if (motion_back) {
	    code += 4;
	}
	if (mb_pattern) {
	    code += 2;
	}
	if (mb_intra) {
	    code += 1;
	}

	switch (code) {
	case 12:
	    Bitio_Write(bbPtr, 0x2, 2);
	    break;
	case 14:
	    Bitio_Write(bbPtr, 0x3, 2);
	    break;
	case 4:
	    Bitio_Write(bbPtr, 0x2, 3);
	    break;
	case 6:
	    Bitio_Write(bbPtr, 0x3, 3);
	    break;
	case 8:
	    Bitio_Write(bbPtr, 0x2, 4);
	    break;
	case 10:
	    Bitio_Write(bbPtr, 0x3, 4);
	    break;
	case 1:
	    Bitio_Write(bbPtr, 0x3, 5);
	    break;
	case 30:
	    Bitio_Write(bbPtr, 0x2, 5);
	    break;
	case 26:
	    Bitio_Write(bbPtr, 0x3, 6);
	    break;
	case 22:
	    Bitio_Write(bbPtr, 0x2, 6);
	    break;
	case 17:
	    Bitio_Write(bbPtr, 0x1, 6);
	    break;
	default:
	    perror("Illegal parameters for macroblock type.");
	    exit(-1);
	    break;
	}
	break;
    }
}


/*===========================================================================*
 *
 * GenMotionCode
 *
 *	generate motion vector output with given value
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
GenMotionCode(bbPtr, vector)
    BitBucket *bbPtr;
    int32 vector;
{
    u_int32_t code, num;

    if ((vector < -16) || (vector > 16)) {
	perror("Motion vector out of range.");
	io_printf(stderr, "Motion vector out of range:  vector = %d\n", vector);
	exit(-1);
    }
    code = mbMotionVectorTable[vector + 16][0];
    num = mbMotionVectorTable[vector + 16][1];

    Bitio_Write(bbPtr, code, num);
}


/*===========================================================================*
 *
 * GenBlockPattern
 *
 *	generate macroblock pattern output
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
GenBlockPattern(bbPtr, mb_pattern)
    BitBucket *bbPtr;
    u_int32_t mb_pattern;
{
    u_int32_t code, num;

    code = mbPatTable[mb_pattern][0];
    num = mbPatTable[mb_pattern][1];

    Bitio_Write(bbPtr, code, num);
}


/*===========================================================================*
 *
 * GenMBAddrIncr
 *
 *	generate macroblock address increment output
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
GenMBAddrIncr(bbPtr, addr_incr)
    BitBucket *bbPtr;
    u_int32_t addr_incr;
{
    u_int32_t code;
    u_int32_t num;

    code = mbAddrIncrTable[addr_incr][0];
    num = mbAddrIncrTable[addr_incr][1];

    Bitio_Write(bbPtr, code, num);
}


/*===========================================================================*
 *
 * GenPictHead
 *
 *	generate picture header with given attributes
 *	append result to the specified bitstream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
GenPictHead(bbPtr, temp_ref, code_type, vbv_delay, full_pel_forw_flag,
	    forw_f_code, full_pel_back_flag, back_f_code, extra_info,
	    extra_info_size, ext_data, ext_data_size, user_data,
	    user_data_size)
    BitBucket *bbPtr;
    u_int32_t temp_ref;
    u_int32_t code_type;
    u_int32_t vbv_delay;
    int32 full_pel_forw_flag;
    u_int32_t forw_f_code;
    int32 full_pel_back_flag;
    u_int32_t back_f_code;
    u_int8_t *extra_info;
    u_int32_t extra_info_size;
    u_int8_t *ext_data;
    u_int32_t ext_data_size;
    u_int8_t *user_data;
    u_int32_t user_data_size;
{
    int i;

    /* Write picture start code. */
    Bitio_Write(bbPtr, PICT_START_CODE, 32);

    /* Temp reference. */
    Bitio_Write(bbPtr, temp_ref, 10);

    /* Code_type. */
    if (code_type == 0) {
	code_type = 1;
    }
    Bitio_Write(bbPtr, code_type, 3);

    /* vbv_delay. */
    vbv_delay = 0xffff;		    /* see page 36 (section 2.4.3.4) */
    Bitio_Write(bbPtr, vbv_delay, 16);

    if ((code_type == 2) || (code_type == 3)) {

	/* Full pel forw flag. */

	if (full_pel_forw_flag) {
	    Bitio_Write(bbPtr, 0x01, 1);
	} else {
	    Bitio_Write(bbPtr, 0x00, 1);
	}

	/* Forw f code. */

	Bitio_Write(bbPtr, forw_f_code, 3);
    }
    if (code_type == 3) {

	/* Full pel back flag. */

	if (full_pel_back_flag) {
	    Bitio_Write(bbPtr, 0x01, 1);
	} else {
	    Bitio_Write(bbPtr, 0x00, 1);
	}

	/* Back f code. */

	Bitio_Write(bbPtr, back_f_code, 3);
    }
    /* Extra bit picture info. */

    if (extra_info != NULL) {
	for (i = 0; i < extra_info_size; i++) {
	    Bitio_Write(bbPtr, 0x01, 1);
	    Bitio_Write(bbPtr, extra_info[i], 8);
	}
    }
    Bitio_Write(bbPtr, 0x00, 1);

    /* next start code */
    Bitio_BytePad(bbPtr);

    /* Write ext data if present. */

    if (ext_data != NULL) {
	Bitio_Write(bbPtr, EXT_START_CODE, 32);

	for (i = 0; i < ext_data_size; i++) {
	    Bitio_Write(bbPtr, ext_data[i], 8);
	}
	Bitio_BytePad(bbPtr);
    }
    /* Write user data if present. */
    if (user_data != NULL) {
	Bitio_Write(bbPtr, USER_START_CODE, 32);

	for (i = 0; i < user_data_size; i++) {
	    Bitio_Write(bbPtr, user_data[i], 8);
	}
	Bitio_BytePad(bbPtr);
    }
}


#ifdef UNUSED_PROCEDURES

/* GenMBEnd only used for `D` pictures. Shouldn't really ever be called. */
/* - dwallach */
void
GenMBEnd(bbPtr)
    BitBucket *bbPtr;
{
    Bitio_Write(bbPtr, 0x01, 1);
}

#endif /* UNUSED_PROCEDURES */
/*===========================================================================*
 * mpeg.c								     *
 *									     *
 *	Procedures to generate the MPEG sequence			     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	GetMPEGStream							     *
 *	IncrementTCTime							     *
 *	SetStatFileName							     *
 *	SetGOPSize							     *
 *	PrintStartStats							     *
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
 *  Revision 1.24  1995/08/16 18:10:48  smoot
 *  *** empty log message ***
 *
 *  Revision 1.23  1995/08/07 21:48:08  smoot
 *  stdin bugs fixed
 *
 *  Revision 1.22  1995/06/26 21:49:19  smoot
 *  added new frame ordering (hacks)^H^H^H^H^H code ;-)
 *
 *  Revision 1.21  1995/06/21 18:30:41  smoot
 *  changed time structure to be ANSI
 *  changed file access to be binary (DOS!)
 *  added time to userdata
 *  Added a sleep to remote reads (NFS delay)
 *
 * Revision 1.20  1995/05/02  01:49:21  eyhung
 * prints out true output bit rate and slightly untabified
 *
 * Revision 1.19  1995/05/02  00:45:35  eyhung
 * endstats now contain correct output fbit rate at the specified frame rate
 *
 * Revision 1.18  1995/03/27  23:43:20  smoot
 * killed printing long as int (compiler warning)
 *
 * Revision 1.17  1995/03/27  19:18:54  smoot
 * fixed divide by zero for very quick encodings
 *
 * Revision 1.16  1995/02/02  22:03:37  smoot
 * added types for MIPS
 *
 * Revision 1.15  1995/02/02  07:26:58  eyhung
 * removed unused tempframe
 *
 * Revision 1.14  1995/02/01  05:01:35  eyhung
 * Completed infinite coding-on-the-fly
 *
 * Revision 1.13  1995/02/01  02:34:02  eyhung
 * Added full coding-on-the-fly
 *
 * Revision 1.12  1995/01/31  23:05:14  eyhung
 * Added some stdin stuff
 *
 * Revision 1.11  1995/01/20  00:01:16  eyhung
 * Added output file to PrintEndStats
 *
 * Revision 1.10  1995/01/19  23:08:51  eyhung
 * Changed copyrights
 *
 * Revision 1.9  1995/01/17  18:55:54  smoot
 * added right version number, and error if no frames selected
 *
 * Revision 1.8  1995/01/16  08:12:54  eyhung
 * added realQuiet
 *
 * Revision 1.7  1994/12/07  00:40:36  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.6  1994/11/28  21:46:45  smoot
 * Added version printing
 *
 * Revision 1.5  1994/11/19  01:33:05  smoot
 * put in userdata
 *
 * Revision 1.4  1994/11/14  22:36:22  smoot
 * Merged specifics and rate control
 *
 * Revision 1.2  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.1  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.6  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.5  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.4  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.3  1993/02/19  18:10:12  keving
 * nothing
 *
 * Revision 1.2  1993/02/17  23:18:20  dwallach
 * checkin prior to keving's joining the project
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "mtypes.h"
#include "frames.h"
#include "search.h"
#include "mpeg.h"
#include "prototypes.h"
#include "parallel.h"
#include "param.h"
#include "readframe.h"
#include "fsize.h"
#include "mheaders.h"
#include "rate.h"
#ifdef MIPS
#include <sys/types.h>
#endif
#include <sys/stat.h>

/*===========*
 *  VERSION  *
 *===========*/

#define VERSION "1.5b"


/*===========*
 * CONSTANTS *
 *===========*/

#define	FPS_30	0x5   /* from MPEG standard sect. 2.4.3.2 */
#define ASPECT_1    0x1	/* aspect ratio, from MPEG standard sect. 2.4.3.2 */
/*NOTE*/
#endif


/*==================*
 * STATIC VARIABLES *
 *==================*/

static int32   diffTime;
static int framesOutput;
static int	    realStart, realEnd;
static int	currentGOP2;
static int	    timeMask;
static int	    numI, numP, numB;


/*==================*
 * GLOBAL VARIABLES *	
 *==================*/

/* important -- don't initialize anything here */
/* must be re-initted anyway in GenMPEGStream */

#if 0
/*NOTE*/
extern int  IOtime;
extern boolean	resizeFrame;
extern int outputWidth, outputHeight;
int	    gopSize = 100;  /* default */
int32	    tc_hrs, tc_min, tc_sec, tc_pict, tc_extra;
int	    totalFramesSent;
int	    yuvWidth, yuvHeight;
int	    realWidth, realHeight;
char	    currentPath[MAXPATHLEN];
char	    statFileName[256];
char	    bitRateFileName[256];
time_t	    timeStart, timeEnd;
FILE	   *statFile;
FILE	   *bitRateFile = NULL;
char	   *framePattern;
int	    framePatternLen;
int	    referenceFrame;
static int  framesRead;
MpegFrame  *pastRefFrame;
MpegFrame  *futureRefFrame;
int	    frameRate = FPS_30;
int	    frameRateRounded = 30;
boolean	    frameRateInteger = TRUE;
int	    aspectRatio = ASPECT_1;
extern char userDataFileName[]; /*NOTE: deleted "unsigned" NOTE*/
extern int mult_seq_headers;

int32 bit_rate, buf_size;
/*NOTE*/
#endif

/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static void	ShowRemainingTime _ANSI_ARGS_((void));
static void	ComputeDHMSTime _ANSI_ARGS_((int32 someTime, char *timeText));
static void	ComputeGOPFrames _ANSI_ARGS_((int whichGOP, int *firstFrame,
					      int *lastFrame, int numFrames));
static void	PrintEndStats _ANSI_ARGS_((int inputFrameBits, long totalBits));
static void	ProcessRefFrame _ANSI_ARGS_((MpegFrame *frame,
					      BitBucket *bb, int lastFrame,
					      char *outputFileName,
					      MpegFrame **frameMemory));
static void	OpenBitRateFile _ANSI_ARGS_((void));
static void	CloseBitRateFile _ANSI_ARGS_((void));


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * SetReferenceFrameType
 *
 *	set the reference frame type to be original or decoded
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    referenceFrame
 *
 *===========================================================================*/

void SetReferenceFrameType(char *type)
   {

    if (strcmp(type, "ORIGINAL") == 0)
       referenceFrame = ORIGINAL_FRAME;

    else if (strcmp(type, "DECODED") == 0)
       referenceFrame = DECODED_FRAME;

    else
       {io_printf(stderr, "ERROR:  Illegal reference frame type: '%s'\n",
		  type);
	exit(1);};

    return;}

void SetBitRateFileName(char *fileName)
   {

    strcpy(bitRateFileName, fileName);

    return;}

/*===========================================================================*
 *
 * GenMPEGStream
 *
 *	generate an MPEG sequence stream (generally)
 *	if whichGOP == frameStart == -1 then does complete MPEG sequence
 *	if whichGOP != -1 then does numbered GOP only (without sequence
 *			       header)
 *	if frameStart != -1 then does numbered frames only (without any
 *				 sequence or GOP headers)		       
 *
 * RETURNS:	amount of time it took
 *
 * SIDE EFFECTS:    too numerous to mention
 *
 *===========================================================================*/

int32 GenMPEGStream(int whichGOP, int frameStart, int frameEnd,
		    int32 *qtable, int32 *niqtable, int numFrames,
		    FILE *ofp, char *outputFileName)
   {
    int i, firstFrame, lastFrame, inputFrameBits;
    int32 bitstreamMode;
    long numBits;
    char inputFileName[1024], frameType;
    boolean firstFrameDone;
    time_t tempTimeStart, tempTimeEnd;
    BitBucket *bb;
    MpegFrame *frame, *tempFrame;
/*
    MpegFrame **frameMemory;
*/
    extern void        PrintItoIBitRate _ANSI_ARGS_((int numBits, int frameNum));

    inputFrameBits = 0;
    firstFrameDone = FALSE;
    frame          = NULL;

    if ((whichGOP == -1) && (frameStart == -1) &&
	(!stdinUsed) && (FType_Type(numFrames-1) == 'b'))
       {io_printf(stderr, "\n");
        io_printf(stderr, "WARNING:  One or more B-frames at end will not be encoded.\n");
        io_printf(stderr, "          See FORCE_ENCODE_LAST_FRAME option in man page.\n");
        io_printf(stderr, "\n");};

    time(&timeStart);

    framesRead = 0;

    ResetIFrameStats();
    ResetPFrameStats();
    ResetBFrameStats();

    Fsize_Reset();

    framesOutput = 0;

    if (childProcess && separateConversion)
       SetFileType(slaveConversion);
    else
       SetFileType(inputConversion);

    if (whichGOP != -1)
       {ComputeGOPFrames(whichGOP, &firstFrame, &lastFrame, numFrames);

        realStart = firstFrame;
        realEnd   = lastFrame;

        if (FType_Type(firstFrame) == 'b')

/* can't find the previous frame interactively */
	   {if (stdinUsed)
	       {io_printf(stderr,
			  "ERROR: Cannot encode GOP from stdin when first frame is a B-frame.\n");
                exit(1);};

/* need to load in previous frame; call it an I frame */
            frame = Frame_New(firstFrame-1, 'i', frameMemory);

            time(&tempTimeStart);

            if ((referenceFrame == DECODED_FRAME) && childProcess)
	       {WaitForDecodedFrame(firstFrame);

                if (remoteIO)
		   GetRemoteDecodedRefFrame(frame, firstFrame-1);
                else
		   ReadDecodedRefFrame(frame, firstFrame-1);}

	    else
	       {if (remoteIO)
		   GetRemoteFrame(frame, firstFrame-1);
	        else
		   {GetNthInputFileName(inputFileName, firstFrame-1);

                    if (childProcess && separateConversion)
		       ReadFrame(frame, inputFileName, slaveConversion, TRUE);
		    else
		       ReadFrame(frame, inputFileName, inputConversion, TRUE);};};

            framesRead++;

            time(&tempTimeEnd);
            IOtime += (tempTimeEnd-tempTimeStart);};}

    else if (frameStart != -1)
       {if (frameEnd > numFrames-1)
	   {io_printf(stderr, "ERROR:  Specified last frame is out of bounds\n");
            exit(1);};

        realStart = frameStart;
        realEnd   = frameEnd;

        firstFrame = frameStart;
        lastFrame  = frameEnd;

/* if first frame is P or B, need to read in P or I frame before it */
        if (FType_Type(firstFrame) != 'i')

/* can't find the previous frame interactively */
	   {if (stdinUsed)
	       {io_printf(stderr,
			  "ERROR: Cannot encode frames from stdin when first frame is not an I-frame.\n");
                exit(1);};

            firstFrame = FType_PastRef(firstFrame);};

/* if last frame is B, need to read in P or I frame after it */
        if ((FType_Type(lastFrame) == 'b') && (lastFrame != numFrames-1))

/* can't find the next reference frame interactively */
	   {if (stdinUsed)
	       {io_printf(stderr,
			  "ERROR: Cannot encode frames from stdin when last frame is a B-frame.\n");
                exit(1);};

            lastFrame = FType_FutureRef(lastFrame);};

        if (lastFrame > numFrames-1)            /* can't go last frame! */
	   lastFrame = numFrames-1;}

    else
       {firstFrame = 0;
        lastFrame  = numFrames-1;

        realStart = 0;
        realEnd   = numFrames-1;

        if (numFrames == 0)
	   {io_printf(stderr, "ERROR:  No frames selected!\n");
            exit(1);};};

/* count number of I, P, and B frames */
    numI     = 0;
    numP     = 0;
    numB     = 0;
    timeMask = 0;
    if (stdinUsed)
       {numI = numP = numB = MAXINT/4;}

    else
       {for (i = firstFrame; i <= lastFrame; i++)
	    {frameType = FType_Type(i);
	     switch (frameType)
	        {case 'i' :
		      numI++;
		      timeMask |= 0x1;
		      break;
		 case 'p' :
		      numP++;
		      timeMask |= 0x2;
		      break;
		 case 'b' :
		      numB++;
		      timeMask |= 0x4;
		      break;};};};

    if (!childProcess)
       {if (showBitRatePerFrame)
	   OpenBitRateFile();
        PrintStartStats(realStart, realEnd);};

    if (frameStart == -1)
       bb = Bitio_New(ofp);
    else
       bb = NULL;

    tc_hrs   = 0;
    tc_min   = 0;
    tc_sec   = 0;
    tc_pict  = 0;
    tc_extra = 0;
    for (i = 0; i < firstFrame; i++)
        IncrementTCTime();

    totalFramesSent = firstFrame;
    currentGOP2     = gopSize;        /* so first I-frame generates GOP Header */

/* rate control initialization  */
    bitstreamMode = getRateMode();
    if (bitstreamMode == FIXED_RATE)
       {initRateControl();

/*        SetFrameRate();        */
       };
    
#ifdef BLEAH
    io_printf(stdout, "firstFrame, lastFrame = %d, %d;  real = %d, %d\n",
	      firstFrame, lastFrame, realStart, realEnd);
    io_flush(stdout);
#endif

    pastRefFrame   = NULL;
    futureRefFrame = NULL;
    for (i = firstFrame; i <= lastFrame; i++)
        {

/* break out of the near-infinite loop if input from stdin is done */
#if 0 
	 char eofcheck[1];

	 if (stdinUsed)
	    {if (scanf("%c", eofcheck) != EOF)
	        ungetc(eofcheck[0], stdin);
	     else
	        break;};
#else
/*
 * For some reason the above version of this stdin EOF check does not
 * work right with jpeg files, the ungetc() is not padding anything to
 * stdin, I have no idea why (perhaps because a char is passed instead
 * of an int?), and it drove me nuts, so I wrote my own, slightly
 * cleaner version, and this one seems to work.
 * Dave Scott (dhs), UofO, 7/19/95.
 */
	 if (stdinUsed)
	    {int eofcheck_;

	     eofcheck_ = fgetc(stdin);
	     if (eofcheck_ == EOF) 
	        break;
	     else 
	        ungetc(eofcheck_, stdin);};
#endif

	 frameType = FType_Type(i);
	
	 time(&tempTimeStart);

	 FreeFrame(frame);
		 
/* skip non-reference frames if non-interactive
 * read in non-reference frames if interactive
 */
	 if (frameType == 'b')
	    {if (stdinUsed)
	        {frame = Frame_New(i, frameType, frameMemory);
		 ReadFrame(frame, "stdin", inputConversion, TRUE);

		 framesRead++;

		 time(&tempTimeEnd);
		 IOtime += (tempTimeEnd-tempTimeStart);

/* add the B frame to the end of the queue of B-frames 
 * for later encoding
 */
		 if (futureRefFrame != NULL)
		    {tempFrame = futureRefFrame;
		     while (tempFrame->next != NULL)
		        tempFrame = tempFrame->next;
		     tempFrame->next = frame;}
		 else
		    io_printf(stderr,
			      "Yow, something wrong in neverland! (hit bad code in mpeg.c\n");};

	     continue;};

	 frame = Frame_New(i, frameType, frameMemory);
	 if (frame == NULL)
	    {CFREE(bb);
	     return(0);};

	 pastRefFrame   = futureRefFrame;
	 futureRefFrame = frame;

	 if ((referenceFrame == DECODED_FRAME) &&
             ((i < realStart) || (i > realEnd)))
	    {WaitForDecodedFrame(i);

	     if (remoteIO)
                GetRemoteDecodedRefFrame(frame, i);
	     else
                ReadDecodedRefFrame(frame, i);}

	 else
	    {if (remoteIO)
                GetRemoteFrame(frame, i);
	     else
	        {GetNthInputFileName(inputFileName, i);
		 if (childProcess && separateConversion)
                    ReadFrame(frame, inputFileName, slaveConversion, TRUE);
		 else
                    ReadFrame(frame, inputFileName, inputConversion, TRUE);};};

	 framesRead++;

	 time(&tempTimeEnd);
	 IOtime += (tempTimeEnd-tempTimeStart);

	 if (!firstFrameDone)
	    {int userDataSize;
	     char *userData;

	     userDataSize = 0;
	     userData     = NULL;

	     inputFrameBits = 24*Fsize_x*Fsize_y;
	     SetBlocksPerSlice();
          
	     if ((whichGOP == -1) && (frameStart == -1))
	        {DBG_PRINT(("Generating sequence header\n"));
		 bitstreamMode = getRateMode();
		 if (bitstreamMode == FIXED_RATE)
		    {bit_rate = getBitRate();
		     buf_size = getBufferSize();}

		 else
		    {bit_rate = -1;
		     buf_size = -1;};
	    
		 if (strlen(userDataFileName) != 0)
		    {struct stat statbuf;
		     FILE *fp;
	      
		     stat(userDataFileName, &statbuf);
		     userDataSize = statbuf.st_size;
		     userData     = CMAKE_N(char, userDataSize);

		     fp = _PG_fopen(userDataFileName,"rb");
		     if (fp == NULL)
		        {io_printf(stderr,"Could not open userdata file-%s.\n",
				   userDataFileName);
			 CFREE(userData);
			 userDataSize = 0;
			 goto write;};

		     if (io_read(userData, 1, userDataSize, fp) != userDataSize)
		        {io_printf(stderr,
				   "Could not read %d bytes from userdata file-%s.\n",
				   userDataSize, userDataFileName);
			 CFREE(userData);
			 userDataSize = 0;
			 goto write;};}

/* put in our UserData Header */
		 else
		    {time_t now;

		     time(&now);
		     userData = CMAKE_N(char, 100);
		     if (userData != NULL)
		        {snprintf(userData, 100,
				  "MPEG stream encoded by UCB Encoder (mpeg_encode) v%s on %s.",
				  VERSION, (char *) ctime(&now));
			 userDataSize = strlen(userData);}
		     else
		        userDataSize = 0;};

write:
		 Mhead_GenSequenceHeader(bb, Fsize_x, Fsize_y,
					 aspectRatio,
					 frameRate, bit_rate,
					 buf_size, 1,
					 qtable, niqtable,
					 NULL, 0,
					 userData, userDataSize);

		 CFREE(userData);};
          
	     firstFrameDone = TRUE;};
        
	 ProcessRefFrame(frame, bb, lastFrame, outputFileName, frameMemory);};

    if (frame != NULL)
       Frame_Free(frame);

/* SEQUENCE END CODE */
    if ((whichGOP == -1) && (frameStart == -1))
       Mhead_GenSequenceEnder(bb);

    if (frameStart == -1)

/* I think this is right, since (bb == NULL) if (frameStart != -1).
 * See above where "bb" is initialized
 */
       numBits = bb->cumulativeBits;

    else

/* what should the correct value be?  Most likely 1.  "numBits" is
 * used below, so we need to make sure it's properly initialized 
 * to somthing (anything)
 */
       numBits = 1;

    if (frameStart == -1)
       {Bitio_Flush(bb);
        bb = NULL;
        io_close(ofp);

        time(&timeEnd);
        diffTime = (int32) (timeEnd-timeStart);

        if (!childProcess)
	   PrintEndStats(inputFrameBits, numBits);}

    else
       {time(&timeEnd);
        diffTime = (int32) (timeEnd-timeStart);

        if (!childProcess)
	   PrintEndStats(inputFrameBits, 1);};

    if (FType_Type(realEnd) != 'i')
       PrintItoIBitRate(numBits, realEnd+1);

    if ((!childProcess) && showBitRatePerFrame)
       CloseBitRateFile();

#ifdef BLEAH
    if (childProcess)
       NoteFrameDone(frameStart, frameEnd);
#endif

    if (!realQuiet)
       {io_printf(stdout, "======FRAMES READ:  %d\n", framesRead);
        io_flush(stdout);};

    return(diffTime);}

/*===========================================================================*
 *
 * IncrementTCTime
 *
 *	increment the tc time by one second (and update min, hrs if necessary)
 *	also increments totalFramesSent
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    totalFramesSent, tc_pict, tc_sec, tc_min, tc_hrs, tc_extra
 *
 *===========================================================================*/
void
IncrementTCTime()
{
    /* if fps = an integer, then tc_extra = 0 and is ignored

       otherwise, it is the number of extra 1/1001 frames we've passed by

       so far; for example, if fps = 24000/1001, then 24 frames = 24024/24000
       seconds = 1 second + 24/24000 seconds = 1 + 1/1000 seconds; similary,
       if fps = 30000/1001, then 30 frames = 30030/30000 = 1 + 1/1000 seconds
       and if fps = 60000/1001, then 60 frames = 1 + 1/1000 seconds

       if fps = 24000/1001, then 1/1000 seconds = 24/1001 frames
       if fps = 30000/1001, then 1/1000 seconds = 30/1001 frames
       if fps = 60000/1001, then 1/1000 seconds = 60/1001 frames     
     */

    totalFramesSent++;
    tc_pict++;
    if ( tc_pict >= frameRateRounded ) {
	tc_pict = 0;
	tc_sec++;
	if ( tc_sec == 60 ) {
	    tc_sec = 0;
	    tc_min++;
	    if ( tc_min == 60 ) {
		tc_min = 0;
		tc_hrs++;
	    }
	}
	if ( ! frameRateInteger ) {
	    tc_extra += frameRateRounded;
	    if ( tc_extra >= 1001 ) {	/* a frame's worth */
		tc_pict++;
		tc_extra -= 1001;
	    }
	}
    }
}


/*===========================================================================*
 *
 * SetStatFileName
 *
 *	set the statistics file name
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    statFileName
 *
 *===========================================================================*/
void
SetStatFileName(fileName)
    char *fileName;
{
    strcpy(statFileName, fileName);
}


/*===========================================================================*
 *
 * SetGOPSize
 *
 *	set the GOP size (frames per GOP)
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    gopSize
 *
 *===========================================================================*/
void
SetGOPSize(size)
    int size;
{
    gopSize = size;
}


/*===========================================================================*
 *
 * PrintStartStats
 *
 *	print out the starting statistics (stuff from the param file)
 *	firstFrame, lastFrame represent the first, last frames to be
 *	encoded
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
PrintStartStats(firstFrame, lastFrame)
    int firstFrame;
    int lastFrame;
{
    FILE *fpointer;
    register int i;
    char    inputFileName[1024];

    if ( statFileName[0] == '\0' ) {
	statFile = NULL;
    } else {
	statFile = _PG_fopen(statFileName, "a");	/* open for appending */
	if ( statFile == NULL ) {
	    io_printf(stderr, "ERROR:  Could not open stat file:  %s\n", statFileName);
	    io_printf(stderr, "        Sending statistics to stdout only.\n");
	    io_printf(stderr, "\n\n");
	} else if (! realQuiet) {
	    io_printf(stdout, "Appending statistics to file:  %s\n", statFileName);
	    io_printf(stdout, "\n\n");
	}
    }
	
    for ( i = 0; i < 2; i++ ) {
	if ( ( i == 0 ) && (! realQuiet) ) {
	    fpointer = stdout;
	} else if ( statFile != NULL ) {
	    fpointer = statFile;
	} else {
	    continue;
	}

	io_printf(fpointer, "MPEG ENCODER STATS (%s)\n",VERSION);
	io_printf(fpointer, "------------------------\n");
	io_printf(fpointer, "TIME STARTED:  %s", (char *) ctime(&timeStart));
	if ( getenv("HOST") != NULL ) {
	    io_printf(fpointer, "MACHINE:  %s\n", getenv("HOST"));
	} else {
	    io_printf(fpointer, "MACHINE:  unknown\n");
	}

	if ( stdinUsed ) {
	    io_printf(fpointer, "INPUT:  stdin\n");
	  }

	
	if ( firstFrame == -1 ) {
	    io_printf(fpointer, "OUTPUT:  %s\n", outputFileName);
	} else if ( ! stdinUsed ) {
	    GetNthInputFileName(inputFileName, firstFrame);
	    io_printf(fpointer, "FIRST FILE:  %s/%s\n", currentPath, inputFileName);
	    GetNthInputFileName(inputFileName, lastFrame);
	    io_printf(fpointer, "LAST FILE:  %s/%s\n", currentPath,
		    inputFileName);
	}
	if ( resizeFrame )
	    io_printf(fpointer, "RESIZED TO:  %dx%d\n",
		    outputWidth, outputHeight);
	io_printf(fpointer, "PATTERN:  %s\n", framePattern);
	io_printf(fpointer, "GOP_SIZE:  %d\n", gopSize);
	io_printf(fpointer, "SLICES PER FRAME:  %d\n", slicesPerFrame);
	if (searchRangeP==searchRangeB)
	  io_printf(fpointer, "RANGE:  +/-%d\n", searchRangeP/2);
	else io_printf(fpointer, "RANGES:  +/-%d %d\n", 
		     searchRangeP/2,searchRangeB/2);
	io_printf(fpointer, "PIXEL SEARCH:  %s\n", pixelFullSearch ? "FULL" : "HALF");
	io_printf(fpointer, "PSEARCH:  %s\n", PSearchName());
	io_printf(fpointer, "BSEARCH:  %s\n", BSearchName());
	io_printf(fpointer, "QSCALE:  %d %d %d\n", qscaleI, 
		GetPQScale(), GetBQScale());
	if (specificsOn) 
	  io_printf(fpointer, "(Except as modified by Specifics file)\n");
	if ( referenceFrame == DECODED_FRAME ) {
	    io_printf(fpointer, "REFERENCE FRAME:  DECODED\n");
	} else if ( referenceFrame == ORIGINAL_FRAME ) {
	    io_printf(fpointer, "REFERENCE FRAME:  ORIGINAL\n");
	} else {
	    io_printf(stderr, "ERROR:  Illegal referenceFrame!!!\n");
	    exit(1);
	}
 	/*	For new Rate control parameters */
 	if (getRateMode() == FIXED_RATE) {
	  io_printf(fpointer, "PICTURE RATE:  %d\n", frameRateRounded);
	  if (getBitRate() != -1) {
	    io_printf(fpointer, "\nBIT RATE:  %d\n", getBitRate());
	  }
	  if (getBufferSize() != -1) {
	    io_printf(fpointer, "BUFFER SIZE:  %d\n", getBufferSize());
	  }
	}
      }
    if (! realQuiet) {
    	io_printf(stdout, "\n\n");
    }	
}


/*===========================================================================*
 *
 * NonLocalRefFrame
 *
 *	decides if this frame can be referenced from a non-local process
 *
 * RETURNS:	TRUE or FALSE
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
boolean
NonLocalRefFrame(id)
    int id;
{
    int	    lastIPid;
    int	    nextIPid;

    if ( ! childProcess ) {
	return(FALSE);
    }

    lastIPid = FType_PastRef(id);

    /* might be accessed by B-frame */
    if ( lastIPid+1 < realStart ) {
	return(TRUE);
    }

    /* if B-frame is out of range, then current frame can be ref'd by it */
    nextIPid = FType_FutureRef(id);

    /* might be accessed by B-frame */
    if ( nextIPid-1 > realEnd ) {
	return(TRUE);
    }

    /* might be accessed by P-frame */
    if ( (nextIPid > realEnd) && (FType_Type(nextIPid) == 'p') ) {
	return(TRUE);
    }

    return(FALSE);
}


 
/*===========================================================================*
 *
 * SetFrameRate
 *
 *	sets global frame rate variables.  value passed is MPEG frame rate code.
 *
 * RETURNS:	TRUE or FALSE
 *
 * SIDE EFFECTS:    frameRateRounded, frameRateInteger
 *
 *===========================================================================*/
void
SetFrameRate()
{
    switch(frameRate) {
	case 1:
	    frameRateRounded = 24;
	    frameRateInteger = FALSE;
	    break;
	case 2:
	    frameRateRounded = 24;
	    frameRateInteger = TRUE;
	    break;
	case 3:
	    frameRateRounded = 25;
	    frameRateInteger = TRUE;
	    break;
	case 4:
	    frameRateRounded = 30;
	    frameRateInteger = FALSE;
	    break;
	case 5:
	    frameRateRounded = 30;
	    frameRateInteger = TRUE;
	    break;
	case 6:
	    frameRateRounded = 50;
	    frameRateInteger = TRUE;
	    break;
	case 7:
	    frameRateRounded = 60;
	    frameRateInteger = FALSE;
	    break;
	case 8:
	    frameRateRounded = 60;
	    frameRateInteger = TRUE;
	    break;
    }
	printf("frame rate(%d) set to %d\n", frameRate, frameRateRounded);
}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * ComputeDHMSTime
 *
 *	turn some number of seconds (someTime) into a string which
 *	summarizes that time according to scale (days, hours, minutes, or
 *	seconds)
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
ComputeDHMSTime(someTime, timeText)
    int32 someTime;
    char *timeText;
{
    int	    days, hours, mins, secs;

    days = someTime / (24*60*60);
    someTime -= days*24*60*60;
    hours = someTime / (60*60);
    someTime -= hours*60*60;
    mins = someTime / 60;
    secs = someTime - mins*60;

    if ( days > 0 ) {
        sprintf(timeText, "Total time:  %d days and %d hours", days, hours);
    } else if ( hours > 0 ) {
        sprintf(timeText, "Total time:  %d hours and %d minutes", hours, mins);
    } else if ( mins > 0 ) {
        sprintf(timeText, "Total time:  %d minutes and %d seconds", mins, secs);
    } else {
	sprintf(timeText, "Total time:  %d seconds", secs);
    }
}


/*===========================================================================*
 *
 * ComputeGOPFrames
 *
 *	calculate the first, last frames of the numbered GOP
 *
 * RETURNS:	lastFrame, firstFrame changed
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
ComputeGOPFrames(whichGOP, firstFrame, lastFrame, numFrames)
    int whichGOP;
    int *firstFrame;
    int *lastFrame;
    int numFrames;
{
    int	    passedB;
    int	    currGOP;
    int	    gopNum, frameNum;

    /* calculate first, last frames of whichGOP GOP */

    *firstFrame = -1;
    *lastFrame = -1;
    gopNum = 0;
    frameNum = 0;
    passedB = 0;
    currGOP = 0;
    while ( *lastFrame == -1 ) {
	if ( frameNum >= numFrames ) {
	    io_printf(stderr, "ERROR:  There aren't that many GOPs!\n");
	    exit(1);
	}

#ifdef BLEAH
if (! realQuiet) {
io_printf(stdout, "GOP STARTS AT %d\n", frameNum-passedB);
}
#endif

	if ( gopNum == whichGOP ) {
	    *firstFrame = frameNum;
	}

	/* go past one gop */
	/* must go past at least one frame */
	do {
	    currGOP += (1 + passedB);

	    frameNum++;

	    passedB = 0;
	    while ( (frameNum < numFrames) && (FType_Type(frameNum) == 'b') ) {
		frameNum++;
		passedB++;
	    }
	} while ( (frameNum < numFrames) && 
		  ((FType_Type(frameNum) != 'i') || (currGOP < gopSize)) );

	currGOP -= gopSize;

	if ( gopNum == whichGOP ) {
	    *lastFrame = (frameNum-passedB-1);
	}

#ifdef BLEAH
if (! realQuiet) {
io_printf(stdout, "GOP ENDS at %d\n", frameNum-passedB-1);
}
#endif

	gopNum++;
    }
}


/*===========================================================================*
 *
 * PrintEndStats
 *
 *	print end statistics (summary, time information)
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
PrintEndStats(inputFrameBits, totalBits)
    int inputFrameBits;
    long totalBits;
{
    FILE *fpointer;
    register int i;
    char    timeText[256];
    float   totalCPU;

    if (! realQuiet) {
    	io_printf(stdout, "\n\n");
    }

    ComputeDHMSTime(diffTime, timeText);

    for ( i = 0; i < 2; i++ ) {
	if ( ( i == 0 ) && (! realQuiet) ) {
	    fpointer = stdout;
	} else if ( statFile != NULL ) {
	    fpointer = statFile;
	} else {
	    continue;
	}

	io_printf(fpointer, "TIME COMPLETED:  %s", (char *) ctime(&timeEnd));
	io_printf(fpointer, "%s\n\n", timeText);

	totalCPU = 0.0;
	totalCPU += ShowIFrameSummary(inputFrameBits, (int32) totalBits, fpointer);
	totalCPU += ShowPFrameSummary(inputFrameBits, (int32) totalBits, fpointer);
	totalCPU += ShowBFrameSummary(inputFrameBits, (int32) totalBits, fpointer);
	io_printf(fpointer, "---------------------------------------------\n");
	io_printf(fpointer, "Total Compression:  %3ld:1     (%9.4f bpp)\n",
		framesOutput*inputFrameBits/totalBits,(float)(
		24.0*(double)(totalBits)/(double)(framesOutput*inputFrameBits)));
	if (diffTime > 0) {
	  io_printf(fpointer, "Total Frames Per Second:  %f (%ld mps)\n",
		  (float)framesOutput/(float)diffTime,
		  (long)((float)framesOutput*(float)inputFrameBits/(256.0*24.0*(float)diffTime)));
	} else {
	  io_printf(fpointer, "Total Frames Per Second:  Infinite!\n");
	}
	if ( totalCPU == 0.0 ) {
	    io_printf(fpointer, "CPU Time:  NONE!\n");
	} else {
	    io_printf(fpointer, "CPU Time:  %f fps     (%ld mps)\n",
		    (float)framesOutput/totalCPU,
		     (long)((float)framesOutput*(float)inputFrameBits/(256.0*24.0*totalCPU)));
	}
	io_printf(fpointer, "Total Output Bit Rate (%d fps):  %d bits/sec\n",
		frameRateRounded, (int) (((long)frameRateRounded)*totalBits/framesOutput));
	io_printf(fpointer, "MPEG file created in :  %s\n", outputFileName);
	io_printf(fpointer, "\n\n");

	if ( computeMVHist ) {
	    ShowPMVHistogram(fpointer);
	    ShowBBMVHistogram(fpointer);
	    ShowBFMVHistogram(fpointer);
	}
    }

    if ( statFile != NULL ) {
        io_close(statFile);
    }
}


/*===========================================================================*
 *
 * ProcessRefFrame
 *
 *	process an I or P frame -- encode it, and process any B frames that
 *	we can now
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    stuff appended to bb
 *
 *===========================================================================*/

static void ProcessRefFrame(MpegFrame *frame, BitBucket *bb, int lastFrame,
			    char *outputFileName, MpegFrame **frameMemory)
   {int id;
    boolean separateFiles;
    char fileName[1024];
    char inputFileName[1024];
    FILE *fpointer = NULL;
    MpegFrame *bFrame = NULL;
    time_t tempTimeStart, tempTimeEnd;
  
    fpointer = NULL;
    bFrame   = NULL;

    separateFiles = (bb == NULL);
  
    if (separateFiles && (frame->id >= realStart) &&
	(frame->id <= realEnd))
       {if (remoteIO)
	   bb = Bitio_New(NULL);
        else
	   {snprintf(fileName, 1024, "%s.frame.%d", outputFileName, frame->id);
	    if ((fpointer = _PG_fopen(fileName, "wb")) == NULL)
	       {io_printf(stderr, "ERROR:  Could not open output file(1):  %s\n",
			  fileName);
		exit(1);};
      
	    bb = Bitio_New(fpointer);};};
  
/* nothing to do */
    if (frame->id < realStart)
       return;
  
/* first, output this frame */
    if (frame->type == TYPE_IFRAME)
       {
      
#ifdef BLEAH
	io_printf(stdout, "I-frame %d, currentGOP = %d\n",
		  frame->id, currentGOP2);
	io_flush(stdout);
#endif
      
/* only start a new GOP with I
 * don't start GOP if only doing frames
 */
	if ((!separateFiles) && (currentGOP2 >= gopSize))
	   {int closed;
	    static int num_gop = 0;
	
/* first, check to see if closed GOP */
	    if ((totalFramesSent == frame->id) || (pastRefFrame == NULL))
	       closed = 1;
	    else
	       closed = 0;
	
/* new GOP */
	    if ((num_gop != 0) && mult_seq_headers &&
		(num_gop % mult_seq_headers == 0))
	       {if (!realQuiet)
		   {io_printf(stdout,
			      "Creating new Sequence before GOP %d\n", num_gop);
		    io_flush(stdout);};
	  
		Mhead_GenSequenceHeader(bb, Fsize_x, Fsize_y,
					aspectRatio,
					frameRate, bit_rate,
					buf_size,  1,
					customQtable, customNIQtable,
					NULL, 0,
					NULL, 0);};
	
	    if (!realQuiet)
	       {io_printf(stdout,
			  "Creating new GOP (closed = %c) before frame %d\n",
			  "FT"[closed], frame->id);
		io_flush(stdout);};
	
	    num_gop++;
	    Mhead_GenGOPHeader(bb, 0,
			       tc_hrs, tc_min, tc_sec, tc_pict,
			       closed, 0,
			       NULL, 0,
			       NULL, 0);
	    currentGOP2 -= gopSize;
	    if (pastRefFrame == NULL)
	       SetGOPStartTime(0);
	    else
	       SetGOPStartTime(pastRefFrame->id+1);};
      
	if ((frame->id >= realStart) && (frame->id <= realEnd))
	   {GenIFrame(bb, frame);
	
	    framesOutput++;
	
	    if (separateFiles)
	       {if (remoteIO)
		   SendRemoteFrame(frame->id, bb);
	        else
		   {Bitio_Flush(bb);
		    io_close(fpointer);};};};

	numI--;
	timeMask &= 0x6;
      
	currentGOP2++;
	IncrementTCTime();}

    else
       {if ((frame->id >= realStart) && (frame->id <= realEnd))
	   {GenPFrame(bb, frame, pastRefFrame);
	
	    framesOutput++;
	
	    if (separateFiles)
	       {if (remoteIO)
		   SendRemoteFrame(frame->id, bb);
	        else
		   {Bitio_Flush(bb);
		    io_close(fpointer);};};};
    
	numP--;
	timeMask &= 0x5;
	ShowRemainingTime();
    
	currentGOP2++;
	IncrementTCTime();};
  
/* now, output B-frames */
    if (pastRefFrame != NULL)
       {for (id = pastRefFrame->id+1; id < futureRefFrame->id; id++)
	    {if (!((id >= realStart) && (id <= realEnd)))
	        continue;
	
	     if (!stdinUsed)
	        {bFrame = Frame_New(id, 'b', frameMemory);
		 if (bFrame == NULL)
		    {CFREE(bb);
		     return;};
	  
		 time(&tempTimeStart);
	  
/* read B frame, output it */
		 if (remoteIO)
		    GetRemoteFrame(bFrame, bFrame->id);
		 else
		    {GetNthInputFileName(inputFileName, id);
		     if (childProcess && separateConversion )
		        ReadFrame(bFrame, inputFileName, slaveConversion, TRUE);
		     else
		        ReadFrame(bFrame, inputFileName, inputConversion, TRUE);};
	  
		 time(&tempTimeEnd);
		 IOtime += (tempTimeEnd-tempTimeStart);
	  
		 framesRead++;}
	  
/* retrieve and remove B-frame from queue set up in GenMPEGStream */
	     else
	        {bFrame             = pastRefFrame->next;
		 pastRefFrame->next = bFrame->next;};
	
	     if (separateFiles)
	        {if (bb != NULL)
		    {CFREE(bb);};

		 if (remoteIO)
		    {if (bb != NULL)
		        free(bb);
		     bb = Bitio_New(NULL);}
	         else
		    {snprintf(fileName, 1024, "%s.frame.%d", outputFileName, 
			      bFrame->id);
		     if ( (fpointer = _PG_fopen(fileName, "wb")) == NULL )
		        {io_printf(stderr, "ERROR:  Could not open output file(2):  %s\n",
				   fileName);
			 exit(1);};
		     bb = Bitio_New(fpointer);};};
	
	     GenBFrame(bb, bFrame, pastRefFrame, futureRefFrame);
	     framesOutput++;
	
	     if (separateFiles)
	        {if (remoteIO)
		    SendRemoteFrame(bFrame->id, bb);
	         else
		    {Bitio_Flush(bb);
		     io_close(fpointer);};};
	
/* free this B frame right away */
	     Frame_Free(bFrame);
	
	     numB--;
	     timeMask &= 0x3;
	     ShowRemainingTime();
	
	     currentGOP2++;
	     IncrementTCTime();};}

    else

/* SRS replicated code */
       {for (id = 0; id < futureRefFrame->id; id++)
	    {if (!((id >= realStart) && (id <= realEnd)))
	        continue;

	     if (!stdinUsed)
	        {bFrame = Frame_New(id, 'b', frameMemory);
		 if (bFrame == NULL)
		    return;

		 time(&tempTimeStart);

/* read B frame, output it */
		 if (remoteIO)
		    GetRemoteFrame(bFrame, bFrame->id);
		 else
		    {GetNthInputFileName(inputFileName, id);
		     if (childProcess && separateConversion)
		        ReadFrame(bFrame, inputFileName, slaveConversion, TRUE);
		     else
		        ReadFrame(bFrame, inputFileName, inputConversion, TRUE);};

		 time(&tempTimeEnd);
		 IOtime += (tempTimeEnd-tempTimeStart);

		 framesRead++;}

/* retrieve and remove B-frame from queue set up in 
 * GenMPEGStream 
 */
	     else
	        {printf("Yow, I doubt this works!\n");
		 if (pastRefFrame != NULL)
		    {bFrame = pastRefFrame->next;
		     pastRefFrame->next = bFrame->next;};};

	     if (separateFiles)
	        {if (bb != NULL)
		    {CFREE(bb);};

		 if (remoteIO)
		    bb = Bitio_New(NULL);
	         else
		    {snprintf(fileName, 1024, "%s.frame.%d", outputFileName, 
			      (bFrame != NULL) ? bFrame->id : -1);
		     fpointer = _PG_fopen(fileName, "wb");
		     if (fpointer == NULL)
		        {io_printf(stderr,
				   "ERROR:  Could not open output file(2):  %s\n",
				   fileName);
			 exit(1);};

		     bb = Bitio_New(fpointer);};};

	     GenBFrame(bb, bFrame, (MpegFrame *)NULL, futureRefFrame);
	     framesOutput++;

	     if (separateFiles)
	        {if (remoteIO)
		    SendRemoteFrame(bFrame->id, bb);
	         else
		    {Bitio_Flush(bb);
		     io_close(fpointer);};};

/* free this B frame right away */
	     Frame_Free(bFrame);

	     numB--;
	     timeMask &= 0x3;
	     ShowRemainingTime();

	     currentGOP2++;
	     IncrementTCTime();};};
  
/* now free previous frame, if there was one */
    if (pastRefFrame != NULL)
       Frame_Free(pastRefFrame);
  
/* note, we may still not free last frame if lastFrame is incorrect
 * (if the last frames are B frames, they aren't output!)
 */

    return;}

/*===========================================================================*
 *
 * ShowRemainingTime
 *
 *	print out an estimate of the time left to encode
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
ShowRemainingTime()
{
    static int	lastTime = 0;
    float   total;
    time_t  nowTime;
    float   secondsPerFrame;

    if ( childProcess ) {
	return; /* nothing */
    }

    if ( numI + numP + numB == 0 ) {	/* no time left */
	return; /* nothing */
    }

    if ( timeMask != 0 ) {	    /* haven't encoded all types yet */
	return; /* nothing */
    }

    time(&nowTime);
    secondsPerFrame = (nowTime-timeStart)/(float)framesOutput;
    total = secondsPerFrame*(float)(numI+numP+numB);

#ifdef BLEAH
    float   timeI, timeP, timeB;

    timeI = EstimateSecondsPerIFrame();
    timeP = EstimateSecondsPerPFrame();
    timeB = EstimateSecondsPerBFrame();
    total = (float)numI*timeI + (float)numP*timeP + (float)numB*timeB;
#endif

    if ( (quietTime >= 0) && (! realQuiet) && (! stdinUsed) &&
	 ((lastTime < (int)total) || ((lastTime-(int)total) >= quietTime) ||
	  (lastTime == 0) || (quietTime == 0)) ) {
	if ( total > 270.0 ) {
	    io_printf(stdout, "ESTIMATED TIME OF COMPLETION:  %d minutes\n",
		    ((int)total+30)/60);
	} else {
	    io_printf(stdout, "ESTIMATED TIME OF COMPLETION:  %d seconds\n",
		    (int)total);
	}

	lastTime = (int)total;
    }
}


void
ReadDecodedRefFrame(frame, frameNumber)
    MpegFrame *frame;
    int frameNumber;
{
    FILE    *fpointer;
    char    fileName[256];
    int	width, height;
    register int y;

    width = Fsize_x;
    height = Fsize_y;

    snprintf(fileName, 256, "%s.decoded.%d", outputFileName, frameNumber);
    if (! realQuiet) {
	io_printf(stdout, "reading %s\n", fileName);
	io_flush(stdout);
    }

    if ((fpointer = _PG_fopen(fileName, "rb")) == NULL) {
      sleep(1);
      if ((fpointer = _PG_fopen(fileName, "rb")) == NULL) {
	io_printf(stderr, "Cannot open %s\n", fileName);
	exit(1);
      }}

    Frame_AllocDecoded(frame, TRUE);
    
    for ( y = 0; y < height; y++ ) {
      if (io_read(frame->decoded_y[y], 1, width, fpointer) != width) {
	io_printf(stderr, "Could not read enough bytes from %s\n", fileName);
      }
    }
    
    for (y = 0; y < (height >> 1); y++) {			/* U */
      if (io_read(frame->decoded_cb[y], 1, width >> 1, fpointer) != (width>>1)) {
	io_printf(stderr, "Could not read enough bytes from %s\n", fileName);
      }
    }
    
    for (y = 0; y < (height >> 1); y++) {			/* V */
      if (io_read(frame->decoded_cr[y], 1, width >> 1, fpointer) != (width>>1)) {
	io_printf(stderr, "Could not read enough bytes from %s\n", fileName);
      }
    }
    
    io_close(fpointer);
}


static void
OpenBitRateFile()
{
    bitRateFile = _PG_fopen(bitRateFileName, "w");
    if ( bitRateFile == NULL ) {
	io_printf(stderr, "ERROR:  Could not open bit rate file:  %s\n", bitRateFileName);
	io_printf(stderr, "\n\n");
	showBitRatePerFrame = FALSE;
    }
}


static void
CloseBitRateFile()
{
#ifdef BLEAH
    char command[256];
#endif

    io_close(bitRateFile);
#ifdef BLEAH
    snprintf(command, 256, "sort -n %s > /tmp/fubahr", bitRateFileName);
    system(command);
    snprintf(command, 256, "mv /tmp/fubahr %s", bitRateFileName);
    system(command);
#endif
}
/*===========================================================================*
 * nojpeg.c								     *
 *									     *
 *	procedures to deal with JPEG files				     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	JMovie2JPEG							     *
 *      ReadJPEG							     *
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
 * Revision 1.2  1995/01/19  23:08:55  eyhung
 * Changed copyrights
 *
 * Revision 1.1  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.2  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.1  1993/07/22  22:23:43  keving
 * nothing
 *
 */

/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "mtypes.h"
#include "frames.h"
#include "prototypes.h"
#include "param.h"
#include "readframe.h"
#include "fsize.h"
#include "rgbtoycc.h"
#include "jpeg.h"
/*NOTE*/
#endif



/*=======================================================================*
 *                                                                       *
 * JMovie2JPEG                                                           *
 *                                                                       *
 *      Splits up a Parallax J_Movie into a set of JFIF image files      *
 *                                                                       *
 * RETURNS:     nothing                                                  *
 *                                                                       *
 * SIDE EFFECTS:    none                                                 *
 *                                                                       *
 *   Contributed By James Boucher(jboucher@flash.bu.edu)                 *
 *               Boston University Multimedia Communications Lab         *
 * This code was adapted from the Berkeley Playone.c and Brian Smith's   *
 * JGetFrame code after being modified on 10-7-93 by Dinesh Venkatesh    *
 * of BU.                                                                *
 *       This code converts a version 2 Parallax J_Movie into a          *
 * set of JFIF compatible JPEG images. It works for all image            *
 * sizes and qualities.                                                  *
 ************************************************************************/
void
JMovie2JPEG(infilename,obase,start,end)
    char *infilename;       /* input filename string */
    char *obase;            /* output filename base string=>obase##.jpg */ 
    int start;              /* first frame to be extracted */
    int end;                /* last frame to be extracted */
{
    io_printf(stderr, "ERROR:  This has not been compiled with JPEG support\n");
    exit(1);
}




/*===========================================================================*
 *
 * ReadJPEG  contributed by James Arthur Boucher of Boston University's
 *                                Multimedia Communications Lab
 *
 *      read a JPEG file and copy data into frame original data arrays
 *
 * RETURNS:     mf modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
/*************************JPEG LIBRARY INTERFACE*********************/
/*
 * THE BIG PICTURE:
 *
 * The rough outline this JPEG decompression operation is:
 *
 *      allocate and initialize JPEG decompression object
 *      specify data source (eg, a file)
 *      jpeg_read_header();     // obtain image dimensions and other parameters
 *      set parameters for decompression
 *      jpeg_start_decompress();
 *      while (scan lines remain to be read)
 *              jpeg_read_scanlines(...);
 *      jpeg_finish_decompress();
 *      release JPEG decompression object
 *
 */
void
ReadJPEG(mf, fp)
    MpegFrame *mf;
    FILE *fp;
{
    io_printf(stderr, "ERROR:  This has not been compiled with JPEG support\n");
    exit(1);
}
/*===========================================================================*
 * noparallel.c								     *
 *									     *
 *	would be procedures to make encoder to run in parallel -- except     *
 *	this machine doesn't have sockets, so we can only run sequentially   *
 *	so this file has dummy procedures which lets it compile		     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	StartIOServer							     *
 *	StartCombineServer						     *
 *	SendRemoteFrame							     *
 *	GetRemoteFrame							     *
 *	StartMasterServer						     *
 *	NotifyMasterDone						     *
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
 * Revision 1.6  1995/05/12  00:00:06  smoot
 * sco defined check
 *
 * Revision 1.5  1995/01/19  23:08:58  eyhung
 * Changed copyrights
 *
 * Revision 1.4  1994/11/14  22:33:31  smoot
 * added ifdef for Linux
 *
 * Revision 1.3  1994/11/12  02:11:55  keving
 * nothing
 *
 * Revision 1.2  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.2  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.1  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.1  1993/06/30  20:06:09  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "mtypes.h"
#include "parallel.h"
#include "frame.h"
#include <time.h>


/*==================*
 * GLOBAL VARIABLES *
 *==================*/
int parallelTestFrames = 10;
int parallelTimeChunks = 60;
char *IOhostName;
int ioPortNumber;
int combinePortNumber;
int decodePortNumber;
boolean	niceProcesses = FALSE;
boolean	forceIalign = FALSE;
int	    machineNumber = -1;
boolean	remoteIO = FALSE;
boolean	separateConversion;
time_t	IOtime = 0;
/*NOTE*/
#endif


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

			/*=================*
			 * IO SERVER STUFF *
			 *=================*/


void
SetIOConvert(separate)
    boolean separate;
{
    /* do nothing -- this may be called during non-parallel execution */
}


void
SetParallelPerfect(val)
boolean val;
{
    /* do nothing -- this may be called during non-parallel execution */
}


void
SetRemoteShell(shell)
    char *shell;
{
    /* do nothing -- this may be called during non-parallel execution */
}


/* StartIOServer
 */ 
void
StartIOServer(numInputFiles, parallelHostName, portNum)
    int numInputFiles;
    char *parallelHostName;
    int portNum;
{
    io_printf(stdout, "ERROR:  (StartIOServer) This machine can NOT run parallel version\n");
    exit(1);
}


void
StartCombineServer(numInputFiles, outputFileName, parallelHostName, portNum)
    int numInputFiles;
    char *outputFileName;
    char *parallelHostName;
    int portNum;
{
    io_printf(stdout, "ERROR:  (StartCombineServer) This machine can NOT run parallel version\n");
    exit(1);
}


void
NoteFrameDone(frameStart, frameEnd)
    int frameStart;
    int frameEnd;
{
    io_printf(stdout, "ERROR:  (NoteFrameDone) This machine can NOT run parallel version\n");
    exit(1);
}

/* SendRemoteFrame
 */
void
SendRemoteFrame(frameNumber, bb)
    int frameNumber;
    BitBucket *bb;
{
    io_printf(stdout, "ERROR:  (SendRemoteFrame) This machine can NOT run parallel version\n");
    exit(1);
}


/* GetRemoteFrame
 */
void
GetRemoteFrame(frame, frameNumber)
    MpegFrame *frame;
    int frameNumber;
{
    io_printf(stdout, "ERROR:  (GetRemoteFrame) This machine can NOT run parallel version\n");
    exit(1);
}


void
WaitForOutputFile(number)
    int number;
{
    io_printf(stdout, "ERROR:  (WaitForOutputFile) This machine can NOT run parallel version\n");
    exit(1);
}


			/*=======================*
			 * PARALLEL SERVER STUFF *
			 *=======================*/


/* StartMasterServer
 */
void
StartMasterServer(numInputFiles, paramFile, outputFileName)
    int numInputFiles;
    char *paramFile;
    char *outputFileName;
{
    io_printf(stdout, "ERROR:  (StartMasterServer) This machine can NOT run parallel version\n");
    exit(1);
}


/* NotifyMasterDone
 */
boolean
NotifyMasterDone(hostName, portNum, machineNumber, seconds, frameStart,
		 frameEnd)
    char *hostName;
    int portNum;
    int machineNumber;
    int seconds;
    int *frameStart;
    int *frameEnd;
{
    io_printf(stdout, "ERROR:  (NotifyMasterDone) This machine can NOT run parallel version\n");
    return(FALSE); /* this function is not used by MPEG */
}


void
StartDecodeServer(numInputFiles, decodeFileName, parallelHostName, portNum)
    int numInputFiles;
    char *decodeFileName;
    char *parallelHostName;
    int portNum;
{
    io_printf(stdout, "ERROR:  (StartDecodeServer) This machine can NOT run parallel version\n");
    exit(1);
}


void
NotifyDecodeServerReady(id)
    int id;
{
    io_printf(stdout, "ERROR:  (NotifyDecodeServerReady) This machine can NOT run parallel version\n");
    exit(1);
}


void
WaitForDecodedFrame(id)
    int id;
{
    io_printf(stdout, "ERROR:  (WaitForDecodedFrame) This machine can NOT run parallel version\n");
    exit(1);
}

void
SendDecodedFrame(frame)
    MpegFrame *frame;
{
    io_printf(stdout, "ERROR:  (SendDecodedFrame) This machine can NOT run parallel version\n");
    exit(1);
}

void
GetRemoteDecodedRefFrame(frame, frameNumber)
    MpegFrame *frame;
    int frameNumber;
{
    io_printf(stdout, "ERROR:  (GetRemoteDecodedRefFrame) This machine can NOT run parallel version\n");
    exit(1);
}
/*===========================================================================*
 * opts.c								     *
 *									     *
 *      Special C code to handle TUNEing options                             *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *      Tune_Init                                                            *
 *      CollectQuantStats                                                    *
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

/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include <stdio.h>
#include <string.h>
#include "opts.h"
#include <malloc.h>
#include <math.h>

/*==============*
 * EXTERNALS    *
 *==============*/

extern char    outputFileName[];
extern boolean pureDCT;
extern int32   qtable[], niqtable[];
extern int     ZAG[];
extern boolean printSNR, decodeRefFrames;
/*NOTE*/
#endif

void init_idctref _ANSI_ARGS_((void));
void init_fdct _ANSI_ARGS_((void));


/*===================*
 * GLOBALS MADE HERE *
 *===================*/

#if 0
/*NOTE*/
boolean tuneingOn = FALSE;
int block_bound = 128;
boolean collect_quant = FALSE;
int collect_quant_detailed = 0;
FILE *collect_quant_fp;
int kill_dim = FALSE;
int kill_dim_break, kill_dim_end;
float kill_dim_slope;
int SearchCompareMode = DEFAULT_SEARCH;
boolean squash_small_differences = FALSE;
int SquashMaxLum, SquashMaxChr;
float LocalDCTRateScale = 1.0, LocalDCTDistortScale = 1.0;
boolean IntraPBAllowed = TRUE;
boolean WriteDistortionNumbers = FALSE;
int collect_distortion_detailed = 0;
FILE *distortion_fp;
FILE *fp_table_rate[31], *fp_table_dist[31];
boolean DoLaplace = FALSE;
double **L1, **L2, **Lambdas;
int LaplaceNum, LaplaceCnum;
boolean BSkipBlocks = TRUE;
boolean BC_on=FALSE;
FILE *BC_file;
/*NOTE*/
#endif

/*====================*
 * Internal Prototypes*
 *====================*/
void	SetupCollectQuantStats _ANSI_ARGS_((char *charPtr));
void    SetupSquashSmall _ANSI_ARGS_ ((char *charPtr));
void    SetupKillDimAreas _ANSI_ARGS_((char *charPtr));
void    SetupLocalDCT _ANSI_ARGS_((char *charPtr));
void    SetupWriteDistortions _ANSI_ARGS_((char *charPtr));
void    SetupLaplace _ANSI_ARGS_((void));
void    CalcLambdas  _ANSI_ARGS_((void));
/*void    Mpost_UnQuantZigBlockLaplace _ANSI_ARGS_((FlatBlock in, Block out, int qscale, boolean iblock));*/
void    SetupBlockCounter _ANSI_ARGS_((char *fname));

/* define this as it too much of a pain to find toupper on different arch'es */
#define ASCII_TOUPPER(c) ((c>='a') && (c<='z')) ? c-'a'+'A' : c

/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * Tune_Init
 *
 *     Do any setup needed before coding stream
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:  varies
 *
 *===========================================================================*/
void Tune_Init()
{
  int i;

  /* Just check for each, and do whats needed */
  if (collect_quant) {
    if (!pureDCT) {
      pureDCT = TRUE;
      init_idctref();
      init_fdct();
    }
    io_printf(collect_quant_fp, "# %s\n", outputFileName);
    io_printf(collect_quant_fp, "#");
    for (i=0; i<64; i++) 
      io_printf(collect_quant_fp, " %d", qtable[i]);
    io_printf(collect_quant_fp, "\n#");
    for (i=0; i<64; i++) 
      io_printf(collect_quant_fp, " %d", niqtable[i]);
    io_printf(collect_quant_fp, "\n# %d %d %d\n\n", 
	    GetIQScale(), GetPQScale(), GetBQScale());
    
  }

  if (DoLaplace) {
    if (!pureDCT) {
      pureDCT = TRUE;
      init_idctref();
      init_fdct();
    }
    decodeRefFrames = TRUE;
    printSNR = TRUE;
  }
    
}

/*===========================================================================*
 *
 * ParseTuneParam
 *
 *     Handle the strings following TUNE
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:  varies
 *
 *===========================================================================*/
void ParseTuneParam(charPtr)
char *charPtr;
{
  switch (ASCII_TOUPPER(*charPtr)) {
  case 'B': 
    if (1 != sscanf(charPtr+2, "%d", &block_bound)) {
      io_printf(stderr, "Invalid tuning parameter (b) in parameter file.\n");
    }
    break;
  case 'C':
    SetupCollectQuantStats(charPtr+2);
    break;
  case 'D':
    SetupLocalDCT(SkipSpacesTabs(charPtr+1));
    break;
  case 'E':
    SetupBlockCounter(SkipSpacesTabs(charPtr+1));
    break;
  case 'K':
    SetupKillDimAreas(SkipSpacesTabs(charPtr+1));
    break;
  case 'L':
    SetupLaplace();
    break;
  case 'N':
    SearchCompareMode = NO_DC_SEARCH;
    break;
  case 'Q':
    SearchCompareMode = DO_Mean_Squared_Distortion;
    break;
  case 'S':
    SetupSquashSmall(SkipSpacesTabs(charPtr+1));
    break;
  case 'W':
    SetupWriteDistortions(SkipSpacesTabs(charPtr+1));
    break;
  case 'U':
    BSkipBlocks = FALSE;
    break;
  case 'Z':
     IntraPBAllowed = FALSE;
    break;
  default:
    io_printf(stderr, "Unknown tuning (%s) in parameter file.\n",charPtr);
    break;
  }
}


/*===============*
 * Internals     *
 *===============*/

/*===========================================================================*
 *
 * SetupCollectQuantStats
 *
 *     Setup variables to collect statistics on quantization values
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    sets collect_quant and collect_quant_fp
 *
 *===========================================================================*/
void SetupCollectQuantStats(charPtr)
char *charPtr;
{
  char fname[256], *cp;

  cp = charPtr;
  while ( (*cp != ' ') && (*cp != '\t') && (*cp != '\n')) {
    cp++;
  }

  SC_strncpy(fname, 256, charPtr, cp-charPtr);

  collect_quant = TRUE;
  if ((collect_quant_fp = _PG_fopen(fname,"w")) == NULL) {
    io_printf(stderr, "Error opening %s for quant statistics\n", fname);
    io_printf(stderr, "Using stdout (ick!)\n");
    collect_quant_fp = stdout;
  }

  cp = SkipSpacesTabs(cp);
  if (*cp != '\n') {
    switch (*cp) {
    case 'c':
      collect_quant_detailed = 1;
      break;
    default:
      io_printf(stderr, "Unknown TUNE parameter setting format %s\n", cp);
    }}
}




/*===========================================================================*
 *
 * SetupKillDimAreas
 *
 *     Do a transform on small lum values
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    sets kill_dim, kill_dim_break, kill_dim_end
 *
 *===========================================================================*/
void SetupKillDimAreas(charPtr)
char *charPtr;
{
  int items_scanned;

  kill_dim = TRUE;
  items_scanned = sscanf(charPtr, "%d %d %f", 
			 &kill_dim_break, &kill_dim_end, &kill_dim_slope);
  if (items_scanned != 3) {
    kill_dim_slope = 0.25;
    items_scanned = sscanf(charPtr, "%d %d", 
			   &kill_dim_break, &kill_dim_end);
    if (items_scanned != 2) {
      /* Use defaults */
      kill_dim_break = 20;
      kill_dim_end   = 25;
    }
  }
  /* check values */
  if (kill_dim_break > kill_dim_end) {
    io_printf(stderr, "TUNE parameter k: break > end is illegal.\n");
    exit(-1);
  }
  if (kill_dim_slope < 0) {
    io_printf(stderr, "TUNE parameter k: slope < 0 is illegal.\n");
    exit(-1);
  }
}



/*===========================================================================*
 *
 * SetupSquashSmall
 *
 *     Setup encoder to squash small changes in Y or Cr/Cb values
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    sets squash_max_differences SquashMaxLum SquashMaxChr 
 *
 *===========================================================================*/
void SetupSquashSmall(charPtr)
char *charPtr;
{
  squash_small_differences = TRUE;

  if (sscanf(charPtr, "%d %d", &SquashMaxLum, &SquashMaxChr) == 1) {
    /* Only set one, do both */
    SquashMaxChr = SquashMaxLum;
  }
}


/*===========================================================================*
 *
 * SetupLocalDCT
 *
 *     Setup encoder to use DCT for rate-distortion estimat ein Psearches
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    sets SearchCompareMode and
 *                        can change LocalDCTRateScale, LocalDCTDistortScale
 *
 *===========================================================================*/
void SetupLocalDCT(charPtr)
char *charPtr;
{
  int num_scales=0;

  SearchCompareMode = LOCAL_DCT;

  /* Set scaling factors if present */
  num_scales = sscanf(charPtr, "%f %f", &LocalDCTRateScale, &LocalDCTDistortScale);
  if (num_scales == 1) {
    io_printf(stderr, "Invalid number of scaling factors for local DCT\n");
    io_printf(stderr, "Must specify Rate Scale and Distorion scale (both floats)\n");
    io_printf(stderr, "Continuing with 1.0 1.0\n");
    LocalDCTRateScale = 1.0;
    LocalDCTDistortScale = 1.0;
  }
}


/*===========================================================================*
 *
 * SetupLaplace
 *
 *     Setup encoder to find distrubution for I-frames, and use for -snr
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    sets DoLaplace, L1, L2, and Lambdas
 *
 *===========================================================================*/
void SetupLaplace()
{
  int i;

  DoLaplace = TRUE;
  LaplaceNum = 0;
  L1      = CMAKE_N(double *, 3);
  L2      = CMAKE_N(double *, 3);
  Lambdas = CMAKE_N(double *, 3);
  if (L1 == NULL || L2 == NULL || Lambdas == NULL) {
    io_printf(stderr,"Out of memory!!!\n");
    exit(1);
  }
  for (i = 0; i < 3; i++) {
    L1[i] = (double *)calloc(64, sizeof(double));
    L2[i] = (double *)calloc(64, sizeof(double));
    Lambdas[i] = CMAKE_N(double, 64);
    if (L1[i] == NULL || L2[i] == NULL || Lambdas[i] == NULL) {
      io_printf(stderr,"Out of memory!!!\n");
      exit(1);
    }
  }
}

void CalcLambdas(void)
{
  int i,j,n;
  double var;
  
  n = LaplaceNum;
  for (i = 0;   i < 3;  i++) {
    for (j = 0;  j < 64;  j++) {
      var = (n*L1[i][j] + L2[i][j]*L2[i][j]) / (n*(n-1));
      Lambdas[i][j] = sqrt(2.0) / sqrt(var);
    }
  }
}


/*===========================================================================*
 *
 * Mpost_UnQuantZigBlockLaplace
 *
 *	unquantize and zig-zag (decode) a single block, using the distrib to get vals
 *      Iblocks only now
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mpost_UnQuantZigBlockLaplace(in, out, qscale, iblock)
    FlatBlock in;
    Block out;
    int qscale;
    boolean iblock;
{
    register int index;
    int	    position;
    register int	    qentry;
    int	    level, coeff;
    double low, high;
    double mid,lam;

    /* qtable[0] must be 8 */
    out[0][0] = (int16)(in[0] * 8);
    
    for ( index = 1;  index < DCTSIZE_SQ;  index++ ) {
      position = ZAG[index];
      level = in[index];
      
      if (level == 0) {
	((int16 *)out)[position] = 0;
	continue;
      }
      qentry = qtable[position] * qscale;
      coeff = (level*qentry)/8;
      low = ((ABS(level)-.5)*qentry)/8;
      high = ((ABS(level)+.5)*qentry)/8;
      lam = Lambdas[LaplaceCnum][position];
      mid = (1.0/lam) * log(0.5*(exp(-lam*low)+exp(-lam*high)));
      mid = ABS(mid);
      if (mid - floor(mid) > .4999) {
	mid = ceil(mid);
      } else {
	mid = floor(mid);
      }
      if (level<0) {mid = -mid;}
/*printf("(%2.1lf-%2.1lf): old: %d vs %d\n",low,high,coeff,(int) mid);*/
      coeff = mid;
      if ( (coeff & 1) == 0 ) {
	if ( coeff < 0 ) {
	  coeff++;
	} else if ( coeff > 0 ) {
	  coeff--;
	}
      }
      ((int16 *)out)[position] = coeff;
    }
}

void
SetupWriteDistortions(charPtr)
char *charPtr;
{
  char fname[256], *cp;
  int i;

  WriteDistortionNumbers = TRUE;
  cp = charPtr;
  while ( (*cp != ' ') && (*cp != '\t') && (*cp != '\n')) {
    cp++;
  }

  SC_strncpy(fname, 256, charPtr, cp-charPtr);

  collect_quant = TRUE;
  if ((distortion_fp = _PG_fopen(fname,"w")) == NULL) {
    io_printf(stderr, "Error opening %s for quant statistics\n", fname);
    io_printf(stderr, "Using stdout (ick!)\n");
    distortion_fp = stdout;
  }

  cp = SkipSpacesTabs(cp);
  if (*cp != '\n') {
    switch (*cp) {
    case 'c':
      collect_distortion_detailed = TRUE;
      break;
    case 't': {
      char scratch[256];
      collect_distortion_detailed = 2;
      for (i = 1;  i < 32;  i++) {
	snprintf(scratch, 256, "%srate%d", fname, i);
	fp_table_rate[i-1] = _PG_fopen(scratch, "w");
	snprintf(scratch, 256, "%sdist%d", fname, i);
	fp_table_dist[i-1] = _PG_fopen(scratch, "w");
	}}
      break;
    default:
      io_printf(stderr, "Unknown TUNE parameter setting format %s\n", cp);
    }}
}  

int mse(blk1, blk2)
Block blk1, blk2;
{
  register int index, error, tmp;
  int16 *bp1, *bp2;

  bp1 = (int16 *)blk1;
  bp2 = (int16 *)blk2;
  error = 0;
  for ( index = 0;  index < DCTSIZE_SQ;  index++ ) {
    tmp = *bp1++ - *bp2++;
    error += tmp*tmp;
  }
  return(error);
}


void
  SetupBlockCounter(fname)
char *fname;
{
  if ((BC_file = _PG_fopen(fname,"w")) == NULL) {
    io_printf(stderr, "Could not open %s\nTUNE E ignored\n", fname);
  } else {
    BC_on = TRUE;
  }
}
/*===========================================================================*
 * param.c								     *
 *									     *
 *	Procedures to read in parameter file				     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	ReadParamFile							     *
 *	GetNthInputFileName						     *
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
 *  Revision 1.38  1995/08/14 22:29:13  smoot
 *  cleaned up subcommand for file name generation
 *  fixed bug where foo* didnt work as input string
 *
 *  Revision 1.37  1995/08/07 21:47:01  smoot
 *  allows spaces after lines in param files
 *  added SIZE == YUV_SIZE for parallel encoding (otherwise you'd have
 *  to parse PPM files in the master server... Ick.
 *  So we have it specified)
 *
 *  Revision 1.36  1995/06/26 21:52:52  smoot
 *  allow uppercase stdin
 *
 *  Revision 1.35  1995/06/21 18:35:01  smoot
 *  moved TUNE stuff to opts.c
 *  Added ability to do:
 *  INPUT
 *  file.ppm  [1-10]
 *  to do 10 copies
 *  added CDL = SPECIFICS
 *
 * Revision 1.34  1995/05/02  01:48:25  eyhung
 * deleted some smootisms. fixed framerate settings, added check for
 * invalid input range
 *
 * Revision 1.33  1995/05/02  01:20:10  smoot
 * *** empty log message ***
 *
 * Revision 1.31  1995/03/09  06:22:36  eyhung
 * more robust input checking (whitespace at end of input file spec)
 *
 * Revision 1.30  1995/02/02  01:06:18  eyhung
 * Added error checking for JMOVIE and stdin ; deleted smoot's "understand"
 * param file ideas.
 *
 * Revision 1.29  1995/02/01  21:09:59  eyhung
 * Finished infinite coding-on-the-fly
 *
 * Revision 1.28  1995/01/31  22:34:28  eyhung
 * Added stdin as a parameter to INPUT_DIR for interactive encoding
 *
 * Revision 1.27  1995/01/27  21:58:07  eyhung
 * Fixed a bug with reading JMOVIES in GetNthInputFileName
 *
 * Revision 1.26  1995/01/25  23:00:05  smoot
 * Checks out Qtable entries
 *
 * Revision 1.25  1995/01/23  02:09:51  darryl
 * removed "PICT_RATE" code
 *
 * Revision 1.24  1995/01/20  00:07:46  smoot
 * requires unistd.c now
 *
 * Revision 1.23  1995/01/20  00:05:33  smoot
 * Added gamma correction option
 *
 * Revision 1.22  1995/01/19  23:55:55  eyhung
 * Fixed up smoot's "style" and made YUV_FORMAT default to EYUV
 *
 * Revision 1.21  1995/01/19  23:09:03  eyhung
 * Changed copyrights
 *
 * Revision 1.20  1995/01/17  22:04:14  smoot
 * Added `commands` to file name listing
 *
 * Revision 1.19  1995/01/17  06:28:01  eyhung
 * StdinUsed added.
 *
 * Revision 1.18  1995/01/16  09:33:35  eyhung
 * Fixed stupid commenting error.
 *
 * Revision 1.17  1995/01/16  06:07:53  eyhung
 * Made it look a little nicer
 *
 * Revision 1.16  1995/01/13  23:57:25  smoot
 * added Y format
 *
 * Revision 1.15  1995/01/08  06:20:39  eyhung
 * *** empty log message ***
 *
 * Revision 1.14  1995/01/08  06:15:57  eyhung
 * *** empty log message ***
 *
 * Revision 1.13  1995/01/08  05:50:32  eyhung
 * Added YUV Format parameter
 *
 * Revision 1.12  1994/12/16  00:55:30  smoot
 * Fixed INPU_FILES bug
 *
 * Revision 1.11  1994/12/12  23:54:36  smoot
 * Fixed GOP-missing error message (GOP to GOP_SIZE)
 *
 * Revision 1.10  1994/12/07  00:40:36  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.9  1994/11/18  23:19:22  smoot
 * Added USER_DATA parameter
 *
 * Revision 1.8  1994/11/16  22:33:40  smoot
 * Put in ifdef for rsh in param.c
 *
 * Revision 1.7  1994/11/16  22:25:05  smoot
 * Corrected ASPECT_RATIO bug
 *
 * Revision 1.6  1994/11/14  22:39:26  smoot
 * merged specifics and rate control
 *
 * Revision 1.5  1994/11/01  05:01:41  darryl
 *  with rate control changes added
 *
 * Revision 1.1  1994/09/27  00:16:28  darryl
 * Initial revision
 *
 * Revision 1.4  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.3  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.2  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.1  1993/06/30  20:06:09  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "mtypes.h"
#include "mpeg.h"
#include "search.h"
#include "prototypes.h"
#include "parallel.h"
#include "param.h"
#include "readframe.h"
#include "fsize.h"
#include "frames.h"
#include "jpeg.h"
#include <string.h>
#include <ctype.h>
#include "rate.h"
#include "opts.h"


/*===========*
 * CONSTANTS *
 *===========*/

#define INPUT_ENTRY_BLOCK_SIZE   128

#define FIRST_OPTION           0
#define OPTION_GOP             0
#define OPTION_PATTERN         1
#define OPTION_PIXEL           2
#define OPTION_PQSCALE         3
#define OPTION_OUTPUT          4
#define OPTION_RANGE           5
#define OPTION_PSEARCH_ALG     6
#define OPTION_IQSCALE         7
#define OPTION_INPUT_DIR       8
#define OPTION_INPUT_CONVERT   9
#define OPTION_INPUT          10
#define OPTION_BQSCALE        11
#define OPTION_BASE_FORMAT    12
#define OPTION_SPF            13
#define OPTION_BSEARCH_ALG    14
#define OPTION_REF_FRAME      15
#define LAST_OPTION           15

/* put any non-required options after LAST_OPTION */
#define OPTION_RESIZE	      16
#define OPTION_IO_CONVERT     17
#define OPTION_SLAVE_CONVERT  18
#define OPTION_IQTABLE	      19
#define OPTION_NIQTABLE	      20
#define OPTION_FRAME_RATE     21
#define OPTION_ASPECT_RATIO   22
#define OPTION_YUV_SIZE	      23
#define OPTION_SPECIFICS      24
#define OPTION_DEFS_SPECIFICS 25
#define OPTION_BUFFER_SIZE    26
#define OPTION_BIT_RATE       27
#define OPTION_USER_DATA      28
#define OPTION_YUV_FORMAT     29
#define OPTION_GAMMA          30
#define OPTION_PARALLEL       31

#define NUM_OPTIONS           31
/*NOTE*/
#endif

/*=======================*
 * STRUCTURE DEFINITIONS *
 *=======================*/

typedef struct InputFileEntryStruct {
    char    left[256];
    char    right[256];
    boolean glob;	    /* if FALSE, left is complete name */
    int	    startID;
    int	    endID;
    int	    skip;
    int	    numPadding;	    /* -1 if there is none */
    int	    numFiles;
    boolean repeat;
} InputFileEntry;


/*==================*
 * STATIC VARIABLES *
 *==================*/

static InputFileEntry **inputFileEntries;
static int numInputFileEntries = 0;
static int  maxInputFileEntries;


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

#if 0
/*NOTE*/
extern char currentPath[MAXPATHLEN];
extern char currentGOPPath[MAXPATHLEN];
extern char currentFramePath[MAXPATHLEN];
char	outputFileName[256];
int	outputWidth, outputHeight;
int numInputFiles = 0;
char inputConversion[1024];
char ioConversion[1024];
char slaveConversion[1024];
char yuvConversion[256];
char specificsFile[256],specificsDefines[1024]="";
boolean GammaCorrection=FALSE;
float   GammaValue;
char userDataFileName[256]={0}; /*NOTE: deleted "unsigned" NOTE*/
boolean specificsOn = FALSE;
boolean optionSeen[NUM_OPTIONS+1];
int numMachines;
char	machineName[MAX_MACHINES][256];
char	userName[MAX_MACHINES][256];
char	executable[MAX_MACHINES][1024];
char	remoteParamFile[MAX_MACHINES][1024];
boolean	remote[MAX_MACHINES];
boolean stdinUsed = FALSE;
int mult_seq_headers = 0;  /* 0 for none, N for header/N GOPs */
/*NOTE*/
#endif

/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/
static void	ReadInputFileNames _ANSI_ARGS_((FILE *fpointer,
						char *endInput));
static void	ReadMachineNames _ANSI_ARGS_((FILE *fpointer));
static int	GetAspectRatio _ANSI_ARGS_((char *p));
static int	GetFrameRate _ANSI_ARGS_((char *p));

/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * ReadParamFile
 *
 *	read the parameter file
 *	function is ENCODE_FRAMES, COMBINE_GOPS, or COMBINE_FRAMES, and
 *	    will slightly modify the procedure's behavior as to what it
 *	    is looking for in the parameter file
 *
 * RETURNS:	TRUE if the parameter file was read correctly; FALSE if not
 *
 * SIDE EFFECTS:    sets parameters accordingly, as well as machine info for
 *		    parallel execution and input file names
 *
 *===========================================================================*/

boolean ReadParamFile(char *fileName, int function)
   {char input[256];
    char *charPtr;
    FILE *fpointer;
    boolean yuvUsed = FALSE;
    static char *optionText[LAST_OPTION+1] = { "GOP_SIZE", "PATTERN", "PIXEL", "PQSCALE",
					       "OUTPUT", "RANGE", "PSEARCH_ALG", "IQSCALE", "INPUT_DIR",
                                               "INPUT_CONVERT", "INPUT", "BQSCALE", "BASE_FILE_FORMAT",
                                               "SLICES_PER_FRAME", "BSEARCH_ALG", "REFERENCE_FRAME"};
    register int index;
    register int row, col;

    fpointer = _PG_fopen(fileName, "r");
    if (fpointer == NULL)
       {io_printf(stderr, "Error:  Cannot open parameter file:  %s\n", fileName);
	return(FALSE);};

/* should set defaults */
    numInputFiles = 0;
    numMachines   = 0;
    sprintf(currentPath, ".");
    sprintf(currentGOPPath, ".");
    sprintf(currentFramePath, ".");
#ifndef HPUX
    SetRemoteShell("rsh");
#else
    SetRemoteShell("remsh");
#endif

    switch (function)
       {case ENCODE_FRAMES:
	     for (index = FIRST_OPTION; index <= LAST_OPTION; index++)
	         optionSeen[index] = FALSE;

	     optionSeen[OPTION_IO_CONVERT]    = FALSE;
	     optionSeen[OPTION_SLAVE_CONVERT] = FALSE;
	     break;

	case COMBINE_GOPS:
	     for (index = FIRST_OPTION; index <= LAST_OPTION; index++)
	         optionSeen[index] = TRUE;

	     optionSeen[OPTION_OUTPUT] = FALSE;
	     break;

	case COMBINE_FRAMES:
	     for (index = FIRST_OPTION; index <= LAST_OPTION; index++)
	         optionSeen[index] = TRUE;

	     optionSeen[OPTION_GOP]    = FALSE;
	     optionSeen[OPTION_OUTPUT] = FALSE;
	     break;};

    for (index = LAST_OPTION+1; index <= NUM_OPTIONS; index++)
        optionSeen[index] = FALSE;

    while (io_gets(input, 256, fpointer) != NULL)

/* skip comments */
       {if (input[0] == '#')
	   continue;

	{int len = strlen(input);

	 len = max(len, 1);
	 if (input[len-1] == '\n')
	    {len--;
	     input[len] = '\0'; /* get rid of newline */

/* junk whitespace */
	     while ((len > 0) &&
		    ((input[len-1] == ' ') || (input[len-1] == '\t')))
	        input[--len] = '\0';};};

	if (strlen(SkipSpacesTabs(input)) == 0)
	   continue;

	switch (input[0])
	   {case 'A':
	         if (strncmp(input, "ASPECT_RATIO", 12) == 0)
		    {charPtr     = SkipSpacesTabs(&input[12]);
		     aspectRatio = GetAspectRatio(charPtr);
		     optionSeen[OPTION_ASPECT_RATIO] = TRUE;};
		 break;

	    case 'B':
		 if (strncmp(input, "BQSCALE", 7) == 0)
		    {charPtr = SkipSpacesTabs(&input[7]);
		     SetBQScale(atoi(charPtr));
		     optionSeen[OPTION_BQSCALE] = TRUE;}
		 else if (strncmp(input, "BASE_FILE_FORMAT", 16) == 0)
		    {charPtr = SkipSpacesTabs(&input[16]);
		     SetFileFormat(charPtr);
		     if ((strncmp(charPtr,"YUV",3) == 0) || 
			 (strncmp(charPtr,"SIGNED_YUV", 10) == 0) ||
			 (strncmp(charPtr,"UNSIGNED_YUV", 12) == 0) ||
			 (strcmp(charPtr,"Y") == 0))
		        yuvUsed = TRUE;

		     optionSeen[OPTION_BASE_FORMAT] = TRUE;}

		 else if (strncmp(input, "BSEARCH_ALG", 11) == 0)
		    {charPtr = SkipSpacesTabs(&input[11]);
		     SetBSearchAlg(charPtr);
		     optionSeen[OPTION_BSEARCH_ALG] = TRUE;}

		 else if (strncmp(input, "BIT_RATE", 8) == 0)
		    {charPtr = SkipSpacesTabs(&input[8]);
		     setBitRate(charPtr);
		     optionSeen[OPTION_BIT_RATE] = TRUE;}

		 else if (strncmp(input, "BUFFER_SIZE", 11) == 0)
		    {charPtr = SkipSpacesTabs(&input[11]);
		     setBufferSize(charPtr);
		     optionSeen[OPTION_BUFFER_SIZE] = TRUE;};
		 break;

	    case 'C':
		 if (strncmp(input, "CDL_FILE", 8) == 0)
		    {charPtr = SkipSpacesTabs(&input[8]);
		     strcpy(specificsFile, charPtr);
		     specificsOn = TRUE;
		     optionSeen[OPTION_SPECIFICS] = TRUE;}

		 else if (strncmp(input, "CDL_DEFINES", 11) == 0)
		    {charPtr = SkipSpacesTabs(&input[11]);
		     strcpy(specificsDefines, charPtr);
		     optionSeen[OPTION_DEFS_SPECIFICS] = TRUE;};
		 break;

	    case 'F':
		 if (strncmp(input, "FRAME_INPUT_DIR", 15) == 0)
		    {charPtr = SkipSpacesTabs(&input[15]);
		     if ((strncmp(charPtr, "stdin", 5) == 0) ||
			 (strncmp(charPtr, "STDIN", 5) == 0))
		        {stdinUsed           = TRUE;
			 numInputFiles       = MAXINT;
			 numInputFileEntries = 1;};

		     strcpy(currentFramePath, charPtr);}

		 else if (strncmp(input, "FRAME_INPUT", 11) == 0)
		    {if (function == COMBINE_FRAMES)
		        ReadInputFileNames(fpointer, "FRAME_END_INPUT");

		     else if (strncmp(input, "FORCE_I_ALIGN", 13) == 0)
		        forceIalign = TRUE;

		     else if (strncmp(input, "FORCE_ENCODE_LAST_FRAME", 23) == 0)
		        forceEncodeLast = TRUE;

		     else if (strncmp(input, "FRAME_RATE", 10) == 0)
		        {charPtr = SkipSpacesTabs(&input[10]);
			 frameRate = GetFrameRate(charPtr);
			 frameRate = min(frameRate, 8);
			 frameRate = max(frameRate, 0);
			 frameRateRounded = (int) VidRateNum[frameRate];
			 if ((frameRate % 3) == 1)
			    frameRateInteger = FALSE;

			 optionSeen[OPTION_FRAME_RATE] = TRUE;};};
		 break;

	    case 'G':
		 if (strncmp(input, "GOP_SIZE", 8) == 0)
		    {charPtr = SkipSpacesTabs(&input[8]);
		     SetGOPSize(atoi(charPtr));
		     optionSeen[OPTION_GOP] = TRUE;}

		 else if (strncmp(input, "GOP_INPUT_DIR", 13) == 0)
		    {charPtr = SkipSpacesTabs(&input[13]);
		     if ((strncmp(charPtr, "stdin", 5) == 0) ||
			 (strncmp(charPtr, "STDIN", 5) == 0))
		        {stdinUsed           = TRUE;
			 numInputFiles       = MAXINT;
			 numInputFileEntries = 1;};

		     strcpy(currentGOPPath, charPtr);}

		 else if (strncmp(input, "GOP_INPUT", 9) == 0)
		    {if (function == COMBINE_GOPS)
		        ReadInputFileNames(fpointer, "GOP_END_INPUT");}

		 else if (strncmp(input, "GAMMA", 5) == 0)
		    {charPtr = SkipSpacesTabs(&input[5]);
		     GammaCorrection = TRUE;
		     sscanf(charPtr,"%f",&GammaValue);
		     optionSeen[OPTION_GAMMA] = TRUE;};

		 break;

	   case 'I':
		 if (strncmp(input, "IQSCALE", 7) == 0)
		    {charPtr = SkipSpacesTabs(&input[7]);
		     SetIQScale(atoi(charPtr));
		     optionSeen[OPTION_IQSCALE] = TRUE;}
		 else if (strncmp(input, "INPUT_DIR", 9) == 0)
		    {charPtr = SkipSpacesTabs(&input[9]);
		     if ((strncmp(charPtr, "stdin", 5) == 0) ||
			 (strncmp(charPtr, "STDIN", 5) == 0))
		        {stdinUsed           = TRUE;
			 numInputFiles       = MAXINT;
			 numInputFileEntries = 1;};

		     strcpy(currentPath, charPtr);
		     optionSeen[OPTION_INPUT_DIR] = TRUE;}

		 else if (strncmp(input, "INPUT_CONVERT", 13) == 0)
		    {charPtr = SkipSpacesTabs(&input[13]);
		     strcpy(inputConversion, charPtr);
		     optionSeen[OPTION_INPUT_CONVERT] = TRUE;}

		 else if (strcmp(input, "INPUT") == 0)
		    {if (function == ENCODE_FRAMES)
		        {ReadInputFileNames(fpointer, "END_INPUT");
			 optionSeen[OPTION_INPUT] = TRUE;};}

		 else if (strncmp(input, "IO_SERVER_CONVERT", 17) == 0)
		    {charPtr = SkipSpacesTabs(&input[17]);
		     strcpy(ioConversion, charPtr);
		     optionSeen[OPTION_IO_CONVERT] = TRUE;}

		 else if (strncmp(input, "IQTABLE", 7) == 0)
		    {for (row = 0; row < 8; row++)
		         {io_gets(input, 256, fpointer);
			  charPtr = input;
			  if (8 != sscanf(charPtr,"%d %d %d %d %d %d %d %d",
					  &qtable[row*8+0],  &qtable[row*8+1],
					  &qtable[row*8+2],  &qtable[row*8+3],
					  &qtable[row*8+4],  &qtable[row*8+5],
					  &qtable[row*8+6],  &qtable[row*8+7]))
			     {io_printf(stderr, "Line %d of IQTABLE doesn't have 8 elements!\n", row);
			      exit(1);};

			  for (col = 0; col < 8; col++)
			      {if ((qtable[row*8+col] < 1) ||
				   (qtable[row*8+col] > 255))
				  {io_printf(stderr,
					     "Warning:  IQTable Element %1d,%1d (%d) corrected to 1-255.\n",
					     row+1, col+1, qtable[row*8+col]);
				   qtable[row*8+col] = (qtable[row*8+col] < 1) ? 1 : 255;};};};

		     if (qtable[0] != 8)
		        {io_printf(stderr, "Warning:  IQTable Element 1,1 reset to 8, since it must be 8.\n");
			 qtable[0] = 8;};

		     customQtable = qtable;
		     optionSeen[OPTION_IQTABLE] = TRUE;}

/* handle spaces after input */
		 else if (strncmp(input, "INPUT", 5) == 0)
		    {charPtr = SkipSpacesTabs(&input[5]);
		     if ((function == ENCODE_FRAMES) && (*charPtr == 0))
		        {ReadInputFileNames(fpointer, "END_INPUT");
			 optionSeen[OPTION_INPUT] = TRUE;};};

		 break;

	    case 'N':
		 if (strncmp(input, "NIQTABLE", 8) == 0)
		    {for (row = 0; row < 8; row++)
		         {io_gets(input, 256, fpointer);
			  charPtr = input;
			  if (8 != sscanf(charPtr,"%d %d %d %d %d %d %d %d",
					  &niqtable[row*8+0],  &niqtable[row*8+1],
					  &niqtable[row*8+2],  &niqtable[row*8+3],
					  &niqtable[row*8+4],  &niqtable[row*8+5],
					  &niqtable[row*8+6],  &niqtable[row*8+7]))
			     {io_printf(stderr,
					"Line %d of NIQTABLE doesn't have 8 elements!\n", row);
			      exit(1);};

			  for (col = 0; col < 8; col++)
			      {if ((niqtable[row*8+col] < 1) ||
				   (niqtable[row*8+col] > 255))
				  {io_printf(stderr,
					     "Warning:  NIQTable Element %1d,%1d (%d) corrected to 1-255.\n",
					     row+1, col+1, niqtable[row*8+col]);
				   niqtable[row*8+col]=(niqtable[row*8+col] < 1) ? 1 : 255;};};};

		     customNIQtable = niqtable;
		     optionSeen[OPTION_NIQTABLE] = TRUE;};
		 break;

	    case 'O':
		  if (strncmp(input, "OUTPUT", 6) == 0)
		     {charPtr = SkipSpacesTabs(&input[6]);
		      if (whichGOP == -1)
			 strcpy(outputFileName, charPtr);
		      else
			 sprintf(outputFileName, "%s.gop.%d",
				 charPtr, whichGOP);

		      optionSeen[OPTION_OUTPUT] = TRUE;};
		 break;

	    case 'P':
		  if (strncmp(input, "PATTERN", 7) == 0)
		     {charPtr = SkipSpacesTabs(&input[7]);
		      SetFramePattern(charPtr);
		      optionSeen[OPTION_PATTERN] = TRUE;}

		  else if (strncmp(input, "PIXEL", 5) == 0)
		     {charPtr = SkipSpacesTabs(&input[5]);
		      SetPixelSearch(charPtr);
		      optionSeen[OPTION_PIXEL] = TRUE;}

		  else if (strncmp(input, "PQSCALE", 7) == 0)
		     {charPtr = SkipSpacesTabs(&input[7]);
		      SetPQScale(atoi(charPtr));
		      optionSeen[OPTION_PQSCALE] = TRUE;}

		  else if (strncmp(input, "PSEARCH_ALG", 11) == 0)
		     {charPtr = SkipSpacesTabs(&input[11]);
		      SetPSearchAlg(charPtr);
		      optionSeen[OPTION_PSEARCH_ALG] = TRUE;}

		  else if (strncmp(input, "PARALLEL_TEST_FRAMES", 20) == 0)
		     {SetParallelPerfect(FALSE);
		      charPtr = SkipSpacesTabs(&input[20]);
		      parallelTestFrames = atoi(charPtr);}

		  else if (strncmp(input, "PARALLEL_TIME_CHUNKS", 20) == 0)
		     {SetParallelPerfect(FALSE);
		      charPtr = SkipSpacesTabs(&input[20]);
		      parallelTimeChunks = atoi(charPtr);}

		  else if (strncmp(input, "PARALLEL_CHUNK_TAPER", 20) == 0)
		     {SetParallelPerfect(FALSE);
		      parallelTimeChunks = -1;}

		  else if (strncmp(input, "PARALLEL_PERFECT", 16) == 0)
		     SetParallelPerfect(TRUE);

		  else if (strncmp(input, "PARALLEL", 8) == 0)
		     {ReadMachineNames(fpointer);
		      optionSeen[OPTION_PARALLEL] = TRUE;};
		 break;

	    case 'R':
		 if (strncmp(input, "RANGE", 5) == 0)
		    {int num_ranges, a, b;

		     num_ranges = 0;

		     charPtr = SkipSpacesTabs(&input[5]);
		     optionSeen[OPTION_RANGE] = TRUE;
		     num_ranges = sscanf(charPtr, "%d %d", &a, &b);
		     a = min(a, 1000000);
		     a = max(a, 0);
		     b = min(b, 1000000);
		     b = max(b, 0);
		     if (num_ranges == 2)
		        SetSearchRange(a, b);

		     else if (sscanf(charPtr, "%d [%d]", &a, &b) == 2)
		        {a = min(a, 1000000);
			 a = max(a, 0);
			 b = min(b, 1000000);
			 b = max(b, 0);
			 SetSearchRange(a, b);}

		     else
		        {a = min(a, 1000000);
			 a = max(a, 0);
			 SetSearchRange(a, a);};}

		 else if (strncmp(input, "REFERENCE_FRAME", 15) == 0)
		    {charPtr = SkipSpacesTabs(&input[15]);
		     SetReferenceFrameType(charPtr);
		     optionSeen[OPTION_REF_FRAME] = TRUE;}

		 else if (strncmp(input, "RSH", 3) == 0)
		    {charPtr = SkipSpacesTabs(&input[3]);
		     SetRemoteShell(charPtr);}

		 else if (strncmp(input, "RESIZE", 6) == 0)
		    {charPtr = SkipSpacesTabs(&input[6]);                    
		     sscanf(charPtr, "%dx%d", &outputWidth, &outputHeight);
		     outputWidth  &= ~(DCTSIZE * 2 - 1);
		     outputHeight &= ~(DCTSIZE * 2 - 1);
		     optionSeen[OPTION_RESIZE] = TRUE;};
		 break;

	    case 'S':
		 if (strncmp(input, "SLICES_PER_FRAME", 16) == 0)
		    {charPtr = SkipSpacesTabs(&input[16]);
		     SetSlicesPerFrame(atoi(charPtr));
		     optionSeen[OPTION_SPF] = TRUE;}

		 else if (strncmp(input, "SLAVE_CONVERT", 13) == 0)
		    {charPtr = SkipSpacesTabs(&input[13]);
		     strcpy(slaveConversion, charPtr);
		     optionSeen[OPTION_SLAVE_CONVERT] = TRUE;}

		 else if (strncmp(input, "SPECIFICS_FILE", 14) == 0)
		    {charPtr = SkipSpacesTabs(&input[14]);
		     strcpy(specificsFile, charPtr);
		     specificsOn = TRUE;
		     optionSeen[OPTION_SPECIFICS] = TRUE;}

		 else if (strncmp(input, "SPECIFICS_DEFINES", 16) == 0)
		    {charPtr = SkipSpacesTabs(&input[17]);
		     strcpy(specificsDefines, charPtr);
		     optionSeen[OPTION_DEFS_SPECIFICS] = TRUE;}

		 else if (strncmp(input, "SEQUENCE_SIZE", 13) == 0)
		    {charPtr = SkipSpacesTabs(&input[13]);
		     mult_seq_headers = atoi(charPtr);}

		 else if (strncmp(input, "SIZE", 4) == 0)
		    {charPtr = SkipSpacesTabs(&input[4]);
		     sscanf(charPtr, "%dx%d", &yuvWidth, &yuvHeight);
		     realWidth = yuvWidth;
		     realHeight = yuvHeight;
		     Fsize_Validate(&yuvWidth, &yuvHeight);
		     optionSeen[OPTION_YUV_SIZE] = TRUE;};
		 break;

	    case 'T':
		 if (strncmp(input, "TUNE", 4) == 0)
		    {tuneingOn = TRUE;
		     charPtr = SkipSpacesTabs(&input[4]);
		     ParseTuneParam(charPtr);};
		 break;

	    case 'U':
		 if (strncmp(input, "USER_DATA", 9) == 0)
		    {charPtr = SkipSpacesTabs(&input[9]);
		     strcpy(userDataFileName, charPtr);
		     optionSeen[OPTION_USER_DATA] = TRUE;};
		 break;

	    case 'Y':
		  if (strncmp(input, "YUV_SIZE", 8) == 0)
		     {charPtr = SkipSpacesTabs(&input[8]);
		      sscanf(charPtr, "%dx%d", &yuvWidth, &yuvHeight);
		      realWidth = yuvWidth;
		      realHeight = yuvHeight;
		      Fsize_Validate(&yuvWidth, &yuvHeight);
		      optionSeen[OPTION_YUV_SIZE] = TRUE;}

		  else if (strncmp(input, "Y_SIZE", 6) == 0)
		     {charPtr = SkipSpacesTabs(&input[6]);
		      sscanf(charPtr, "%dx%d", &yuvWidth, &yuvHeight);
		      realWidth = yuvWidth;
		      realHeight = yuvHeight;
		      Fsize_Validate(&yuvWidth, &yuvHeight);
		      optionSeen[OPTION_YUV_SIZE] = TRUE;}

		  else if (strncmp(input, "YUV_FORMAT", 10) == 0)
		     {charPtr = SkipSpacesTabs(&input[10]);
		      strcpy(yuvConversion, charPtr);
		      optionSeen[OPTION_YUV_FORMAT] = TRUE;};
		 break;};};

    io_close(fpointer);

    for (index = FIRST_OPTION; index <= LAST_OPTION; index++)
        {if (!optionSeen[index])

/* INPUT unnecessary when stdin is used */
	    {if ((index == OPTION_INPUT) && stdinUsed)
	        continue;

	     io_printf(stderr, "ERROR:  Missing option '%s'\n", optionText[index]);
	     exit(1);};};

/* error checking */
    if (yuvUsed)
       {if (!optionSeen[OPTION_YUV_SIZE])
	   {io_printf(stderr, "ERROR:  YUV format used but YUV_SIZE not given\n");
	    exit(1);};

	if (!optionSeen[OPTION_YUV_FORMAT])
	   {strcpy(yuvConversion, "EYUV");
	    io_printf(stderr,
		      "WARNING:  YUV format not specified; defaulting to Berkeley YUV (EYUV)\n\n");};};

    if (stdinUsed && optionSeen[OPTION_PARALLEL])
       {io_printf(stderr,
		  "ERROR:  stdin reading for parallel execution not enabled yet.\n");
	exit(1);};

    if (optionSeen[OPTION_PARALLEL] && !optionSeen[OPTION_YUV_SIZE])
       {io_printf(stderr, "ERROR:  Specify SIZE WxH for parallel encoding\n");
	exit(1);};

    if (optionSeen[OPTION_IO_CONVERT] != optionSeen[OPTION_SLAVE_CONVERT])
       {io_printf(stderr, "ERROR:  must have either both IO_SERVER_CONVERT and SLAVE_CONVERT\n");
	io_printf(stderr, "        or neither\n");
	exit(1);};

    if (optionSeen[OPTION_DEFS_SPECIFICS] && !optionSeen[OPTION_SPECIFICS])
       {io_printf(stderr,
		  "ERROR:  does not make sense to define Specifics file options, but no specifics file!\n");
	exit(1);};

    SetIOConvert(optionSeen[OPTION_IO_CONVERT]);

    SetResize(optionSeen[OPTION_RESIZE]);

    if (function == ENCODE_FRAMES)
       {SetFCode();

	if (psearchAlg == PSEARCH_TWOLEVEL)
	   SetPixelSearch("HALF");};

  return(TRUE);}


/*===========================================================================*
 *
 * GetNthInputFileName
 *
 *	finds the nth input file name
 *
 * RETURNS:	name is placed in already allocated fileName string
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
  GetNthInputFileName(fileName, n)
char *fileName;
int n;
{
  static int	lastN = 0, lastMapN = 0, lastSoFar = 0;
  int	    mapN;
  register int index;
  int	    soFar;
  int	    loop;
  int	    numPadding;
  char    numBuffer[33];

  if ( stdinUsed) {
    return;
  }

  /* assumes n is within bounds 0...numInputFiles-1 */

  if ( n >= lastN) {
    soFar = lastSoFar;
    index = lastMapN;
  } else {
    soFar = 0;
    index = 0;
  }

  while ( soFar + inputFileEntries[index]->numFiles <= n) {
    soFar +=  inputFileEntries[index]->numFiles;
    index++;
  }

  mapN = index;

  index = inputFileEntries[mapN]->startID +
    inputFileEntries[mapN]->skip*(n-soFar);

  numPadding = inputFileEntries[mapN]->numPadding;

  if ( numPadding != -1) {
    snprintf(numBuffer, 33, "%32d", index);
    for ( loop = 32-numPadding; loop < 32; loop++) {
      if ( numBuffer[loop] != ' ') {
        break;
      } else {
        numBuffer[loop] = '0';
      }
    }

    if (inputFileEntries[mapN]->repeat != TRUE) {
      sprintf(fileName, "%s%s%s",
	      inputFileEntries[mapN]->left,
	      &numBuffer[32-numPadding],
	      inputFileEntries[mapN]->right);
    } else {
      sprintf(fileName, "%s", inputFileEntries[mapN]->left);
    }
  } else {
    if (inputFileEntries[mapN]->repeat != TRUE) {
      sprintf(fileName, "%s%d%s",
	      inputFileEntries[mapN]->left,
	      index,
	      inputFileEntries[mapN]->right);
    } else {
      sprintf(fileName, "%s", inputFileEntries[mapN]->left);
    }
  }

  lastN = n;
  lastMapN = mapN;
  lastSoFar = soFar;
}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * ReadMachineNames
 *
 *	read a list of machine names for parallel execution
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    machine info updated
 *
 *===========================================================================*/

static void ReadMachineNames(FILE *fpointer)
   {char input[256];
    char *charPtr;

    while ((io_gets(input, 256, fpointer) != NULL) &&
	   (strncmp(input, "END_PARALLEL", 12) != 0))
       {if ((input[0] == '#') || (input[0] == '\n'))
	   continue;

	if (strncmp(input, "REMOTE", 6) == 0)
	   {charPtr = SkipSpacesTabs(&input[6]);
	    remote[numMachines] = TRUE;

	    sscanf(charPtr, "%255s %255s %255s %255s",
		   machineName[numMachines], userName[numMachines],
		   executable[numMachines], remoteParamFile[numMachines]);}

	else
	   {remote[numMachines] = FALSE;

	    sscanf(input, "%255s %255s %255s",
		   machineName[numMachines], userName[numMachines],
		   executable[numMachines]);};

	numMachines++;};

  return;}

/*===========================================================================*
 *
 * ReadInputFileNames
 *
 *	read a list of input file names
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    info stored for retrieval using GetNthInputFileName
 *
 *===========================================================================*/

static void ReadInputFileNames(FILE *fpointer, char *endInput)
   {int leftNum, rightNum, skipNum;
    int	numPadding, length;
    boolean padding;
    char input[256], left[256], right[256];
    char *globPtr, *charPtr;
    char leftNumText[256], rightNumText[256], skipNumText[256];
    char full_path[MAXPATHLEN + 256];
    FILE *jmovie;

    numPadding = 0;

    inputFileEntries    = CMAKE_N(InputFileEntry *, INPUT_ENTRY_BLOCK_SIZE);
    maxInputFileEntries = INPUT_ENTRY_BLOCK_SIZE;

    length = strlen(endInput);

/* read input files up until endInput */
    while ((io_gets(input, 256, fpointer) != NULL) &&
	   (strncmp(input, endInput, length) != 0))

/* if input is coming in via standard input, keep on looping till the 
 * endInput string is reached so that the program points to the right 
 * place on exit.
 */
       {if (stdinUsed)
	   continue;
            
/* ignore comments and newlines */
	if ((input[0] == '#') || (input[0] == '\n'))
	   continue;

/* recurse for commands */
	if (input[0] == '`')
	   {FILE *fp;
	    char cmd[300], *start, *end, tmp[300], cdcmd[110];

	    start = &input[1];
	    end = &SC_LAST_CHAR(input);

	    while (*end != '`')
	       end--;

	    end--;

	    if (optionSeen[OPTION_INPUT_DIR] == TRUE)
	       snprintf(cdcmd, 110, "cd %s;",currentPath);
	    else
	       strcpy(cdcmd,"");

	    SC_strncpy(tmp, 300, start, end-start+1);
	    snprintf(cmd, 300, "(%s %s)", cdcmd, tmp);

	    fp = (FILE *) popen(cmd,"r");
	    if (fp == NULL)
	       {io_printf(stderr, "Command failed! Could not open piped command:\n%s\n",
			  cmd);
		continue;};

	    ReadInputFileNames(fp,"HOPE-THIS_ISNT_A_FILENAME.xyz5555");
	    continue;};

/* get rid of trailing whitespace including newline */
	while (isspace((int) SC_LAST_CHAR(input)))
	   {SC_LAST_CHAR(input) = '\0';};

/* more space! */
	if (numInputFileEntries == maxInputFileEntries)
 	   {maxInputFileEntries += INPUT_ENTRY_BLOCK_SIZE;

	    CREMAKE(inputFileEntries, InputFileEntry *,  maxInputFileEntries);};

	if (inputFileEntries == NULL)
	   return;

	inputFileEntries[numInputFileEntries] = CMAKE(InputFileEntry);
	if (inputFileEntries[numInputFileEntries] == NULL)
	   return;

	if (SC_LAST_CHAR(input) == ']')
	   {inputFileEntries[numInputFileEntries]->glob   = TRUE;
	    inputFileEntries[numInputFileEntries]->repeat = FALSE;

/* star expand */
	    globPtr = input;
	    charPtr = left;

/* copy left of '*' */
	    while ((*globPtr != '\0') && (*globPtr != '*'))
	       {*charPtr = *globPtr;
		charPtr++;
		globPtr++;};

	    *charPtr = '\0';

	    if (*globPtr == '\0')
	       {io_printf(stderr,
			  "WARNING: expanding non-star regular expression\n");
		inputFileEntries[numInputFileEntries]->repeat = TRUE;
		globPtr = input;
		charPtr = left;

/* recopy left of whitespace */
		while ((*globPtr != '\0') && (*globPtr != '*') && 
		       (*globPtr != ' ')  && (*globPtr != '\t'))
		   {*charPtr = *globPtr;
		    charPtr++;
		    globPtr++;};

		*charPtr = '\0';
		*right = '\0';}

	    else
	       {globPtr++;
		charPtr = right;

/* copy right of '*' */
		while ((*globPtr != '\0') && (*globPtr != ' ') &&
		       (*globPtr != '\t'))
		   {*charPtr = *globPtr;
		    charPtr++;
		    globPtr++;};

		*charPtr = '\0';};
      
	    globPtr = SkipSpacesTabs(globPtr);

	    if ( *globPtr != '[' )
	       {io_printf(stderr,
			  "ERROR:  Invalid input file expansion expression (no '[')\n");
		exit(1);};

	    globPtr++;
	    charPtr = leftNumText;

/* copy left number */
	    while (isdigit((int) *globPtr))
	       {*charPtr = *globPtr;
		charPtr++;
		globPtr++;};

	    *charPtr = '\0';

	    if (*globPtr != '-' )
	       {io_printf(stderr,
			  "ERROR:  Invalid input file expansion expression (no '-')\n");
		exit(1);};

	    globPtr++;
	    charPtr = rightNumText;

/* copy right number */
	    while (isdigit((int) *globPtr))
	       {*charPtr = *globPtr;
		charPtr++;
		globPtr++;};

	    *charPtr = '\0';
	    if (atoi(rightNumText) < atoi(leftNumText))
	       {io_printf(stderr,
			  "ERROR:  Beginning of input range is higher than end.\n");
		exit(1);};

	    if (*globPtr != ']')
	       {if (*globPtr != '+')
		   {io_printf(stderr,
			      "ERROR:  Invalid input file expansion expression (no ']')\n");
		    exit(1);};

		globPtr++;
		charPtr = skipNumText;

/* copy skip number */
		while (isdigit((int) *globPtr))
		   {*charPtr = *globPtr;
		    charPtr++;
		    globPtr++;};

		*charPtr = '\0';

		if (*globPtr != ']')
		   {io_printf(stderr,
			      "ERROR:  Invalid input file expansion expression (no ']')\n");
		    exit(1);};

		skipNum = atoi(skipNumText);}

	    else
	       skipNum = 1;

	    leftNum = atoi(leftNumText);
	    rightNum = atoi(rightNumText);

	    if ((leftNumText[0] == '0') && (leftNumText[1] != '\0'))
	       {padding    = TRUE;
		numPadding = strlen(leftNumText);}
	    else
	       padding = FALSE;

	    inputFileEntries[numInputFileEntries]->startID  = leftNum;
	    inputFileEntries[numInputFileEntries]->endID    = rightNum;
	    inputFileEntries[numInputFileEntries]->skip     = skipNum;
	    inputFileEntries[numInputFileEntries]->numFiles = (rightNum-leftNum+1)/skipNum;

	    strcpy(inputFileEntries[numInputFileEntries]->left, left);
	    strcpy(inputFileEntries[numInputFileEntries]->right, right);

	    if (padding)
	       inputFileEntries[numInputFileEntries]->numPadding = numPadding;
	    else
	       inputFileEntries[numInputFileEntries]->numPadding = -1;}

	else
	   {strcpy(inputFileEntries[numInputFileEntries]->left, input);
	    if (baseFormat == JMOVIE_FILE_TYPE)
	       {inputFileEntries[numInputFileEntries]->glob = TRUE;
		full_path[0] = '\0';
		strcpy(full_path, currentPath);
    
		if (!stdinUsed)
		   {strcat(full_path, "/");
		    strcat(full_path, input);
		    jmovie = _PG_fopen(input, "rb"); 

		    if (jmovie == NULL)
		       {perror(input); 
			exit(1);};

		    fseek(jmovie, (8*sizeof(char)), 0);
		    fseek(jmovie, (2*sizeof(int)), 1);

		    if (io_read(&(inputFileEntries[numInputFileEntries]->numFiles),
				sizeof(int), 1, jmovie) != 1)
		       {perror("Error in reading number of frames in JMOVIE");
			exit(1);};

		    io_close(jmovie);};

		strcpy(inputFileEntries[numInputFileEntries]->right,".jpg");
		inputFileEntries[numInputFileEntries]->numPadding = -1;
		inputFileEntries[numInputFileEntries]->startID    = 1;
		inputFileEntries[numInputFileEntries]->endID      = (inputFileEntries[numInputFileEntries]->numFiles-1);
		inputFileEntries[numInputFileEntries]->skip       = 1;
		if (!realQuiet)
		   io_printf(stdout,
			     "Encoding all %d frames from JMOVIE.\n",
			     inputFileEntries[numInputFileEntries]->endID);}

	    else
	       {inputFileEntries[numInputFileEntries]->glob     = FALSE;
		inputFileEntries[numInputFileEntries]->numFiles = 1;

/* fixes a bug from version 1.3: */
		inputFileEntries[numInputFileEntries]->numPadding = 0;

/* fixes a bug from version 1.4 */
		strcpy(inputFileEntries[numInputFileEntries]->right,"\0");
		inputFileEntries[numInputFileEntries]->startID = 0;
		inputFileEntries[numInputFileEntries]->endID   = 0;
		inputFileEntries[numInputFileEntries]->skip    = 0;};};

	numInputFiles += inputFileEntries[numInputFileEntries]->numFiles;
	numInputFileEntries++;};

    return;}


/*===========================================================================*
 *
 * SkipSpacesTabs
 *
 *	skip all spaces and tabs
 *
 * RETURNS:	point to next character not a space or tab
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
char *
  SkipSpacesTabs(start)
char *start;
{
  while ( (*start == ' ') || (*start == '\t')) {
    start++;
  }

  return(start);
}


/*===========================================================================*
 *
 * GetFrameRate
 *
 * take a character string with the input frame rate 
 * and return the correct frame rate code for use in the Sequence header
 *
 * RETURNS: frame rate code as per MPEG-I spec
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

static int GetFrameRate(char *p)
  {int thouRate, rv;
   float rate;

   sscanf(p, "%f", &rate);
   thouRate = (int) (0.5 + 1000.0*rate);

   if (thouRate == 23976)
      rv = 1;
   else if (thouRate == 24000)
      rv = 2;
   else if (thouRate == 25000)
      rv = 3;
   else if (thouRate == 29970)
      rv = 4;
   else if (thouRate == 30000)
      rv = 5;
   else if (thouRate == 50000)
      rv = 6;
   else if (thouRate == 59940)
      rv = 7;
   else if (thouRate == 60000)
      rv = 8;
   else
      {io_printf(stderr,"INVALID FRAME RATE: %s frames/sec\n", p);
       rv = -1;}; /* this function is not used by MPEG */

   return(rv);}


/*===========================================================================*
 *
 * GetAspectRatio
 *
 * take a character string with the pixel aspect ratio
 * and returns the correct aspect ratio code for use in the Sequence header
 *
 * RETURNS: aspect ratio code as per MPEG-I spec
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static int
  GetAspectRatio(p)
char *p;
{
  float   ratio;
  int	    ttRatio;

  sscanf(p, "%f", &ratio);
  ttRatio = (int)(0.5+ratio*10000.0);

  if ( ttRatio == 10000 )	   return(1);
  else if ( ttRatio ==  6735 )    return(2);
  else if ( ttRatio ==  7031 )    return(3);
  else if ( ttRatio ==  7615 )    return(4);
  else if ( ttRatio ==  8055 )    return(5);
  else if ( ttRatio ==  8437 )    return(6);
  else if ( ttRatio ==  8935 )    return(7);
  else if ( ttRatio ==  9157 )    return(8);
  else if ( ttRatio ==  9815 )    return(9);
  else if ( ttRatio == 10255 )    return(10);
  else if ( ttRatio == 10695 )    return(11);
  else if ( ttRatio == 10950 )    return(12);
  else if ( ttRatio == 11575 )    return(13);
  else if ( ttRatio == 12015 )    return(14);
  else {
    io_printf(stderr,"INVALID ASPECT RATIO: %s frames/sec\n", p);
    return(-1); /* this function is not used by MPEG */
  }
}






/****************************************************************
 *  Jim Boucher's code
 *
 *
 ****************************************************************/
void
JM2JPEG()
{
  char full_path[MAXPATHLEN + 256];
  char inter_file[MAXPATHLEN +256]; 
  int ci;

  for(ci = 0; ci < numInputFileEntries; ci++) {
    inter_file[0] = '\0';
    full_path[0] = '\0';
    strcpy(full_path, currentPath);
    
    if (! stdinUsed) {
      strcat(full_path, "/");
      strcat(full_path, inputFileEntries[ci]->left);
      strcpy(inter_file,full_path);
    
      if (! realQuiet) {
        io_printf(stdout, "Extracting JPEG's in the JMOVIE from %s\n",full_path);
      }
    
      JMovie2JPEG(full_path,
                  inter_file,
                  inputFileEntries[ci]->startID, inputFileEntries[ci]->endID);
    } else {
      io_printf(stderr, "ERROR: JMovie format not supported with stdin yet.\n");
      exit(1);
    }
      
  }
}
/*===========================================================================*
 * pframe.c								     *
 *									     *
 *	Procedures concerned with generation of P-frames		     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	GenPFrame							     *
 *	ResetPFrameStats						     *
 *	ShowPFrameSummary						     *
 *	EstimateSecondsPerPFrame					     *
 *	ComputeHalfPixelData						     *
 *	SetPQScale							     *
 *	GetPQScale							     *
 *                                                                           *
 * NOTE:  when motion vectors are passed as arguments, they are passed as    *
 *        twice their value.  In other words, a motion vector of (3,4) will  *
 *        be passed as (6,8).  This allows half-pixel motion vectors to be   *
 *        passed as integers.  This is true throughout the program.          *
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
 *  Revision 1.22  1995/08/07 21:51:23  smoot
 *  fixed  LumMotionError call, simpler now with option type
 *
 *  Revision 1.21  1995/06/21 22:23:16  smoot
 *  fixed specifics file bug
 *  generalized timeing stuff
 *  binary writes
 *  TUNEing stuff
 *
 * Revision 1.20  1995/04/14  23:07:41  smoot
 * reorganized to ease rate control experimentation
 *
 * Revision 1.19  1995/02/24  23:49:27  smoot
 * added specifications file format 2
 *
 * Revision 1.18  1995/02/01  21:48:17  smoot
 * cleanup
 *
 * Revision 1.17  1995/01/23  06:30:01  darryl
 * fixed bug in "MMB Type "pattern" and Rate control
 *
 * Revision 1.15  1995/01/19  23:49:28  smoot
 * moved rev_dct, make pattern changable by ComputeDiffDCTs
 *
 * Revision 1.14  1995/01/19  23:09:07  eyhung
 * Changed copyrights
 *
 * Revision 1.13  1995/01/19  23:00:26  smoot
 * Fixed 1st/last MB in slice color bug in P-frames
 *
 * Revision 1.12  1995/01/17  22:10:27  smoot
 * Fixed B/P Qscale bug
 *
 * Revision 1.11  1995/01/16  08:14:41  eyhung
 * added realQuiet
 *
 * Revision 1.10  1994/12/07  00:40:36  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.9  1994/11/14  22:38:18  smoot
 * merged specifics and rate control
 *
 * Revision 1.8  1994/11/01  05:01:09  darryl
 *  with rate control changes added
 *
 * Revision 2.1  1994/10/31  00:05:39  darryl
 * version before, hopefully, final changes
 *
 * Revision 2.0  1994/10/24  02:38:26  darryl
 * will be adding the experiment code
 *
 * Revision 1.1  1994/09/27  00:15:44  darryl
 * Initial revision
 *
 * Revision 1.7  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.6  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.5  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.4  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.3  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.2  1993/03/02  23:03:42  keving
 * nothing
 *
 * Revision 1.1  1993/02/19  19:14:12  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include <assert.h>
#include <sys/param.h>
#include "all.h"
#include "mtypes.h"
#include "bitio.h"
#include "frames.h"
#include "prototypes.h"
#include "param.h"
#include "mheaders.h"
#include "fsize.h"
#include "postdct.h"
#include "mpeg.h"
#include "parallel.h"
#include "rate.h"
#include "opts.h"
/*NOTE*/
#endif

/*==================*
 * STATIC VARIABLES *
 *==================*/

static int32	zeroDiff;
static int      numPIBlocks = 0;
static int      numPPBlocks = 0;
static int      numPSkipped = 0;
static int      numPIBits = 0;
static int      numPPBits = 0;
static int      numFrames3 = 0;
static int      numFrameBits3 = 0;
static int32    totalTime3 = 0;
static int      qscaleP;
static float	totalSNR3 = 0.0;
static float	totalPSNR3 = 0.0;
extern Block    **dct, **dctr, **dctb;
extern dct_data_type   **dct_data;

/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static boolean	ZeroMotionBetter _ANSI_ARGS_((LumBlock currentBlock,
					      MpegFrame *prev, int by, int bx,
					      int my, int mx));

static boolean	DoIntraCode _ANSI_ARGS_((LumBlock currentBlock,
					 MpegFrame *prev, int by, int bx,
					 int motionY, int motionX));

static boolean	ZeroMotionSufficient _ANSI_ARGS_((LumBlock currentBlock,
						  MpegFrame *prev,
						  int by, int bx));
     
#ifdef BLEAH
static void	ComputeAndPrintPframeMAD _ANSI_ARGS_((LumBlock currentBlock,
						      MpegFrame *prev,
						      int by, int bx,
						      int my, int mx,
						      int numBlock));
#endif
     
    
/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * GenPFrame
 *
 *	generate a P-frame from previous frame, adding the result to the
 *	given bit bucket
 *
 * RETURNS:	frame appended to bb
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
  GenPFrame(bb, current, prev)
BitBucket *bb;
MpegFrame *current;
MpegFrame *prev;
{
  extern int **pmvHistogram;
  FlatBlock fba[6], fb[6];
  Block	dec[6];
  int32 y_dc_pred, cr_dc_pred, cb_dc_pred;
  int x, y;
  int	motionX = 0, motionY = 0;
  int	oldMotionX = 0, oldMotionY = 0;
  int	offsetX, offsetY;
  int	tempX, tempY;
  int	motionXrem, motionXquot;
  int	motionYrem, motionYquot;
  int	pattern;
  int	mbAddrInc = 1;
  boolean	useMotion;
  int numIBlocks = 0;
  int	numPBlocks = 0;
  int	numSkipped = 0;
  int	numIBits = 0;
  int numPBits = 0;
  int totalBits;
  int	totalFrameBits;
  int32    startTime, endTime;
  int	lastBlockX, lastBlockY;
  int	lastX, lastY;
  int	fy, fx;
  LumBlock currentBlock;
  register int ix, iy;
  int	mbAddress;
  int slicePos;
  register int index;
  float   snr[3], psnr[3];
  int QScale;
  BlockMV *info;
  int bitstreamMode, newQScale;
  int rc_blockStart = 0;
  boolean overflowChange = FALSE;
  int     overflowValue  = 0;


  if (collect_quant) {io_printf(collect_quant_fp, "# P\n");}
  if (dct==NULL) AllocDctBlocks();
  numFrames3++;
  totalFrameBits = bb->cumulativeBits;
  startTime = time_elapsed();

  DBG_PRINT(("Generating pframe\n"));

  QScale = GetPQScale();
  /*   bit allocation for rate control purposes */
  bitstreamMode = getRateMode();
  if (bitstreamMode == FIXED_RATE) {
    targetRateControl(current);
  }
 
  Mhead_GenPictureHeader(bb, P_FRAME, current->id, fCodeP);
  /* Check for Qscale change */  
  if (specificsOn) {
    /* Set a Qscale for this frame? */
    newQScale = SpecLookup(current->id, 0, 0 /* junk */, &info /*junk*/, QScale);
    if (newQScale != -1) {
      QScale = newQScale;
    }
    /* Set for slice? */
    newQScale = SpecLookup(current->id, 1, 1, &info /*junk*/, QScale);
    if (newQScale != -1) {
      QScale = newQScale;
    }
  }

  DBG_PRINT(("Slice Header\n"));
  Mhead_GenSliceHeader(bb, 1, QScale, NULL, 0);

  if ( referenceFrame == DECODED_FRAME) {
    Frame_AllocDecoded(current, TRUE);
  } else if ( printSNR) {
    Frame_AllocDecoded(current, FALSE);
  }

  /* don't do dct on blocks yet */
  Frame_AllocBlocks(current);
  BlockifyFrame(current);

  /* for I-blocks */
  y_dc_pred = cr_dc_pred = cb_dc_pred = 128;

  totalBits = bb->cumulativeBits;

  if ( (! pixelFullSearch) && (! prev->halfComputed)) {
    ComputeHalfPixelData(prev);
  }

  lastBlockX = Fsize_x>>3;
  lastBlockY = Fsize_y>>3;
  lastX = lastBlockX-2;
  lastY = lastBlockY-2;
  mbAddress = 0;

  /* First loop though finding motion/not and DCTing */
  for (y = 0; y < lastBlockY; y += 2) {
    for (x = 0; x < lastBlockX; x += 2) {
      /* compute currentBlock */
      BLOCK_TO_FRAME_COORD(y, x, fy, fx);
      for ( iy = 0; iy < 16; iy++) {
	for ( ix = 0; ix < 16; ix++) {
	  currentBlock[iy][ix] = (int16)current->orig_y[fy+iy][fx+ix];
	}
      }

      /* See if we have a cached answer */
      if (specificsOn) {
	(void) SpecLookup(current->id, 2, mbAddress, &info, QScale);
	if (info != (BlockMV*)NULL) {
	  if (info->typ == TYP_SKIP) {
	    motionX = motionY = 0;
	    useMotion = TRUE;
	    goto no_search;
	  } else {		/* assume P, since we're a P frame.... */
	    motionX = info->fx;
	    motionY = info->fy;
	    useMotion = TRUE;
	    goto no_search;
	  }}
	/* if unspecified, just look */
      }

      /* see if we should use motion vectors, and if so, what those
       * vectors should be
       */
      if ( ZeroMotionSufficient(currentBlock, prev, y, x)) {
	motionX = 0;
	motionY = 0;
	useMotion = TRUE;
      } else {
	useMotion = PMotionSearch(currentBlock, prev, y, x,
				  &motionY, &motionX);
	if ( useMotion) {
	  if ( ZeroMotionBetter(currentBlock, prev, y, x, motionY,
				motionX)) {
	    motionX = 0;
	    motionY = 0;
	  }
	  if (IntraPBAllowed) 
	    useMotion = (! DoIntraCode(currentBlock, prev, y, x,
				       motionY, motionX));
	}
      }

    no_search:

      dct_data[y][x].useMotion = useMotion;
      if ( ! useMotion) {
	/* output I-block inside a P-frame */
	numIBlocks++;

	/* calculate forward dct's */
	if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "li\n");
	mp_fwd_dct_block2(current->y_blocks[y][x], dct[y][x]);
	mp_fwd_dct_block2(current->y_blocks[y][x+1], dct[y][x+1]);
	mp_fwd_dct_block2(current->y_blocks[y+1][x], dct[y+1][x]);
	mp_fwd_dct_block2(current->y_blocks[y+1][x+1], dct[y+1][x+1]);
	if (collect_quant && (collect_quant_detailed & 1)) io_printf(collect_quant_fp, "ci\n");
	mp_fwd_dct_block2(current->cb_blocks[y>>1][x>>1], dctb[y>>1][x>>1]);
	mp_fwd_dct_block2(current->cr_blocks[y>>1][x>>1], dctr[y>>1][x>>1]);

      } else {
	/* USE MOTION VECTORS */
	numPBlocks++;

	pattern = 63;
	ComputeDiffDCTs(current, prev, y, x, motionY, motionX,
			&pattern);

	assert(motionX+searchRangeP+1 >= 0);
	assert(motionY+searchRangeP+1 >= 0);

#ifdef BLEAH
	if ( motionX+searchRangeP+1 > 2*searchRangeP+2 )
	  {
	    io_printf(stdout, "motionX = %d, searchRangeP = %d\n",
		    motionX, searchRangeP);
	  }
#endif

	if ( computeMVHist) {
	  assert(motionX+searchRangeP+1 <= 2*searchRangeP+2);
	  assert(motionY+searchRangeP+1 <= 2*searchRangeP+2);
	  pmvHistogram[motionX+searchRangeP+1][motionY+searchRangeP+1]++;
	}
	/* Save specs for next loops */
	dct_data[y][x].pattern = pattern;
	dct_data[y][x].fmotionX = motionX;
	dct_data[y][x].fmotionY = motionY;

      }
      mbAddress++;
    }}

  mbAddress = 0;
  for (y = 0; y < lastBlockY; y += 2) {
    for (x = 0; x < lastBlockX; x += 2) {
      slicePos = (mbAddress % blocksPerSlice);

      if ( (slicePos == 0) && (mbAddress != 0)) {
	if (specificsOn) {
	  /* Make sure no slice Qscale change */
	  newQScale = SpecLookup(current->id, 1, mbAddress/blocksPerSlice,
				 &info /*junk*/, QScale);
	  if (newQScale != -1) QScale = newQScale;
	}

	Mhead_GenSliceEnder(bb);
	Mhead_GenSliceHeader(bb, 1+(y>>1), QScale, NULL, 0);

	/* reset everything */
	oldMotionX = 0;		oldMotionY = 0;
	y_dc_pred = cr_dc_pred = cb_dc_pred = 128;

	mbAddrInc = 1+(x>>1);
      }
	    
      /*  Determine if new Qscale needed for Rate Control purposes  */
      if (bitstreamMode == FIXED_RATE) {
	rc_blockStart =  bb->cumulativeBits;
	newQScale = needQScaleChange(qscaleP,
				     current->y_blocks[y][x],
				     current->y_blocks[y][x+1],
				     current->y_blocks[y+1][x],
				     current->y_blocks[y+1][x+1]);
	if (newQScale > 0) {
	  QScale = newQScale;
	}
      }
	    
      /* Check for Qscale change */
      if (specificsOn) {
	newQScale = SpecLookup(current->id, 2, mbAddress, &info, QScale);
	if (newQScale != -1) {
	  QScale = newQScale;
	}
      }

      if (! dct_data[y][x].useMotion) {
	GEN_I_BLOCK(P_FRAME, current, bb, mbAddrInc, QScale);
	mbAddrInc = 1;

	numIBits += (bb->cumulativeBits-totalBits);
	totalBits = bb->cumulativeBits;

	/* reset because intra-coded */
	oldMotionX = 0;		oldMotionY = 0;

	if ( decodeRefFrames) {
	  /* need to decode block we just encoded */
	  Mpost_UnQuantZigBlock(fb[0], dec[0], QScale, TRUE);
	  Mpost_UnQuantZigBlock(fb[1], dec[1], QScale, TRUE);
	  Mpost_UnQuantZigBlock(fb[2], dec[2], QScale, TRUE);
	  Mpost_UnQuantZigBlock(fb[3], dec[3], QScale, TRUE);
	  Mpost_UnQuantZigBlock(fb[4], dec[4], QScale, TRUE);
	  Mpost_UnQuantZigBlock(fb[5], dec[5], QScale, TRUE);

	  /* now, reverse the DCT transform */
	  for ( index = 0; index < 6; index++) {
	    mpeg_jrevdct((int16 *)dec[index]);
	  }

	  /* now, unblockify */
	  BlockToData(current->decoded_y, dec[0], y, x);
	  BlockToData(current->decoded_y, dec[1], y, x+1);
	  BlockToData(current->decoded_y, dec[2], y+1, x);
	  BlockToData(current->decoded_y, dec[3], y+1, x+1);
	  BlockToData(current->decoded_cb, dec[4], y>>1, x>>1);
	  BlockToData(current->decoded_cr, dec[5], y>>1, x>>1);
	}
      } else {
	int fCode = fCodeP;

	/* reset because non-intra-coded */
	y_dc_pred = cr_dc_pred = cb_dc_pred = 128;

	pattern = dct_data[y][x].pattern;
	motionX = dct_data[y][x].fmotionX;
	motionY = dct_data[y][x].fmotionY;

#ifdef BLEAH
	ComputeAndPrintPframeMAD(currentBlock, prev, y, x, motionY, motionX, mbAddress);
#endif

	if ( pixelFullSearch) { /* should be even */
	  motionY /= 2;
	  motionX /= 2;
	}

	/* transform the motion vector into the appropriate values */
	offsetX = motionX - oldMotionX;
	offsetY = motionY - oldMotionY;
/*	if ((offsetX+(8*x)) >= (Fsize_x-8)) log(10.0); */
	ENCODE_MOTION_VECTOR(offsetX, offsetY, motionXquot,
			     motionYquot, motionXrem, motionYrem,
			     FORW_F);

#ifdef BLEAH
	if ( (motionX != 0) || (motionY != 0)) {
	  io_printf(stdout, "FRAME (y, x)  %d, %d (block %d)\n", y, x, mbAddress);
	  io_printf(stdout, "motionX = %d, motionY = %d\n", motionX, motionY);
	  io_printf(stdout, "    mxq, mxr = %d, %d    myq, myr = %d, %d\n",
		  motionXquot, motionXrem, motionYquot, motionYrem);
	}
#endif

	oldMotionX = motionX;
	oldMotionY = motionY;

	if ( pixelFullSearch) { /* reset for use with PMotionSearch */
	  motionY *= 2;
	  motionX *= 2;
	}
	calc_blocks:
	/* create flat blocks and update pattern if necessary */
	/* Note DoQuant references QScale, overflowChange, overflowValue,
           pattern, and the calc_blocks label                 */
	  DoQuant(0x20, dct[y][x], fba[0]);
	  DoQuant(0x10, dct[y][x+1], fba[1]);
	  DoQuant(0x08, dct[y+1][x], fba[2]);
	  DoQuant(0x04, dct[y+1][x+1], fba[3]);
	  DoQuant(0x02, dctb[y>>1][x>>1], fba[4]);
	  DoQuant(0x01, dctr[y>>1][x>>1], fba[5]);

	if ( decodeRefFrames) {
	  for ( index = 0; index < 6; index++) {
	    if ( pattern & (1 << (5-index))) {
	      Mpost_UnQuantZigBlock(fba[index], dec[index], QScale, FALSE);
	      mpeg_jrevdct((int16 *)dec[index]);
	    } else {
	      memset((char *)dec[index], 0, sizeof(Block));
	    }
	  }

	  /* now add the motion block */
	  AddMotionBlock(dec[0], prev->decoded_y, y, x, motionY, motionX);
	  AddMotionBlock(dec[1], prev->decoded_y, y, x+1, motionY, motionX);
	  AddMotionBlock(dec[2], prev->decoded_y, y+1, x, motionY, motionX);
	  AddMotionBlock(dec[3], prev->decoded_y, y+1, x+1, motionY, motionX);
	  AddMotionBlock(dec[4], prev->decoded_cb, y>>1, x>>1, motionY/2, motionX/2);
	  AddMotionBlock(dec[5], prev->decoded_cr, y>>1, x>>1, motionY/2, motionX/2);

	  /* now, unblockify */
	  BlockToData(current->decoded_y, dec[0], y, x);
	  BlockToData(current->decoded_y, dec[1], y, x+1);
	  BlockToData(current->decoded_y, dec[2], y+1, x);
	  BlockToData(current->decoded_y, dec[3], y+1, x+1);
	  BlockToData(current->decoded_cb, dec[4], y>>1, x>>1);
	  BlockToData(current->decoded_cr, dec[5], y>>1, x>>1);
	} 

	if ( (motionX == 0) && (motionY == 0)) {
	  if ( pattern == 0) {
	    /* can only skip if:
	     *     1)  not the last block in frame
	     *     2)  not the last block in slice
	     *     3)  not the first block in slice
	     */

	    if ( ((y < lastY) || (x < lastX)) &&
		(slicePos+1 != blocksPerSlice) &&
		(slicePos != 0)) {
	      mbAddrInc++;	/* skipped macroblock */
	      numSkipped++;
	      numPBlocks--;
	    } else {		/* first/last macroblock */
	      Mhead_GenMBHeader(bb, 2 /* pict_code_type */, mbAddrInc /* addr_incr */,
				QScale /* q_scale */,
				fCode /* forw_f_code */, 1 /* back_f_code */,
				motionXrem /* horiz_forw_r */, motionYrem /* vert_forw_r */,
				0 /* horiz_back_r */, 0 /* vert_back_r */,
				1 /* motion_forw */, motionXquot /* m_horiz_forw */,
				motionYquot /* m_vert_forw */, 0 /* motion_back */,
				0 /* m_horiz_back */, 0 /* m_vert_back */,
				0 /* mb_pattern */, 0 /* mb_intra */);
	      mbAddrInc = 1;
	    }
	  } else {
	    DBG_PRINT(("MB Header(%d,%d)\n", x, y));
	    Mhead_GenMBHeader(bb, 2 /* pict_code_type */, mbAddrInc /* addr_incr */,
			      QScale /* q_scale */,
			      fCode /* forw_f_code */, 1 /* back_f_code */,
			      0 /* horiz_forw_r */, 0 /* vert_forw_r */,
			      0 /* horiz_back_r */, 0 /* vert_back_r */,
			      0 /* motion_forw */, 0 /* m_horiz_forw */,
			      0 /* m_vert_forw */, 0 /* motion_back */,
			      0 /* m_horiz_back */, 0 /* m_vert_back */,
			      pattern /* mb_pattern */, 0 /* mb_intra */);
	    mbAddrInc = 1;
	  }
	} else {
	  /*      DBG_PRINT(("MB Header(%d,%d)\n", x, y));  */
		  
	  Mhead_GenMBHeader(bb, 2 /* pict_code_type */, mbAddrInc /* addr_incr */,
			    QScale /* q_scale */,
			    fCode /* forw_f_code */, 1 /* back_f_code */,
			    motionXrem /* horiz_forw_r */, motionYrem /* vert_forw_r */,
			    0	/* horiz_back_r */, 0 /* vert_back_r */,
			    1	/* motion_forw */, motionXquot /* m_horiz_forw */,
			    motionYquot /* m_vert_forw */, 0 /* motion_back */,
			    0	/* m_horiz_back */, 0 /* m_vert_back */,
			    pattern /* mb_pattern */, 0 /* mb_intra */);
	  mbAddrInc = 1;
	}

	/* now output the difference */
	for ( tempX = 0; tempX < 6; tempX++) {
	  if ( GET_ITH_BIT(pattern, 5-tempX)) {
	    Mpost_RLEHuffPBlock(fba[tempX], bb);
	  }
	}

	numPBits += (bb->cumulativeBits-totalBits);
	totalBits = bb->cumulativeBits;
      }

      if (overflowChange) {
	/* undo an overflow-caused Qscale change */
	overflowChange = FALSE;
	QScale -= overflowValue;
	overflowValue = 0;
      }

      mbAddress++;
      /*   Rate Control  */
      if (bitstreamMode == FIXED_RATE) {
	incMacroBlockBits( bb->cumulativeBits- rc_blockStart);
	rc_blockStart = bb->cumulativeBits;
	MB_RateOut(TYPE_PFRAME);
      }
    }
  }

  if ( printSNR) {
    BlockComputeSNR(current,snr,psnr);
    totalSNR3 += snr[0];
    totalPSNR3 += psnr[0];
  }

#ifdef BLEAHBLEAH
  {
    FILE *filePtr;

    filePtr = _PG_fopen("PFRAME.yuv", "wb");

    for ( y = 0; y < Fsize_y; y++ )
      {
	for ( x = 0; x < Fsize_x; x++ )
	  io_printf(filePtr, "%d ", current->decoded_y[y][x]);
	io_printf(filePtr, "\n");
      }

    io_close(filePtr);
  }
#endif

  Mhead_GenSliceEnder(bb);
  /*   Rate Control */
  if (bitstreamMode == FIXED_RATE) {
    updateRateControl(TYPE_PFRAME);
  }

  /* UPDATE STATISTICS */
  endTime = time_elapsed();
  totalTime3 += (endTime-startTime);

  if ( ( ! childProcess) && showBitRatePerFrame) {
    /* ASSUMES 30 FRAMES PER SECOND */
    io_printf(bitRateFile, "%5d\t%8ld\n", current->id,
	    30*(bb->cumulativeBits-totalFrameBits));
  }

  if ( (! childProcess) && frameSummary && (! realQuiet)) {
    io_printf(stdout, "FRAME %d (P):  I BLOCKS:  %d;  P BLOCKS:  %d   SKIPPED:  %d  (%ld seconds)\n",
	    current->id, numIBlocks, numPBlocks, numSkipped, (long)(endTime-startTime)/TIME_RATE);
    if ( printSNR) {
      io_printf(stdout, "FRAME %d:  SNR:  %.1f\t%.1f\t%.1f\tPSNR:  %.1f\t%.1f\t%.1f\n",
	      current->id, snr[0], snr[1], snr[2],
	      psnr[0], psnr[1], psnr[2]);
    }
  }

  numFrameBits3 += (bb->cumulativeBits-totalFrameBits);
  numPIBlocks += numIBlocks;
  numPPBlocks += numPBlocks;
  numPSkipped += numSkipped;
  numPIBits += numIBits;
  numPPBits += numPBits;

  if ( (referenceFrame == DECODED_FRAME) && NonLocalRefFrame(current->id)) {
    if ( remoteIO) {
      SendDecodedFrame(current);
    } else {
      WriteDecodedFrame(current);
    }

    NotifyDecodeServerReady(current->id);
  }
}


/*===========================================================================*
 *
 * ResetPFrameStats
 *
 *	reset the P-frame statistics
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
  ResetPFrameStats()
{
  numPIBlocks = 0;
  numPPBlocks = 0;
  numPSkipped = 0;
  numPIBits = 0;
  numPPBits = 0;
  numFrames3 = 0;
  numFrameBits3 = 0;
  totalTime3 = 0;
}


/*===========================================================================*
 *
 * SetPQScale
 *
 *	set the P-frame Q-scale
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    qscaleP
 *
 *===========================================================================*/
void
  SetPQScale(qP)
int qP;
{
  qscaleP = qP;
}


/*===========================================================================*
 *
 * GetPQScale
 *
 *	return the P-frame Q-scale
 *
 * RETURNS:	the P-frame Q-scale
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
  GetPQScale()
{
  return(qscaleP);
}


/*===========================================================================*
 *
 * ShowPFrameSummary
 *
 *	print a summary of information on encoding P-frames
 *
 * RETURNS:	time taken for P-frames (in seconds)
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
float
  ShowPFrameSummary(inputFrameBits, totalBits, fpointer)
int inputFrameBits;
int32 totalBits;
FILE *fpointer;
{
  if ( numFrames3 == 0) {
    return(0.0);
  }

  io_printf(fpointer, "-------------------------\n");
  io_printf(fpointer, "*****P FRAME SUMMARY*****\n");
  io_printf(fpointer, "-------------------------\n");

  if ( numPIBlocks != 0) {
    io_printf(fpointer, "  I Blocks:  %5d     (%6d bits)     (%5d bpb)\n",
	    numPIBlocks, numPIBits, numPIBits/numPIBlocks);
  } else {
    io_printf(fpointer, "  I Blocks:  %5d\n", 0);
  }

  if ( numPPBlocks != 0) {
    io_printf(fpointer, "  P Blocks:  %5d     (%6d bits)     (%5d bpb)\n",
	    numPPBlocks, numPPBits, numPPBits/numPPBlocks);
  } else {
    io_printf(fpointer, "  P Blocks:  %5d\n", 0);
  }

  io_printf(fpointer, "  Skipped:   %5d\n", numPSkipped);

  io_printf(fpointer, "  Frames:    %5d     (%6d bits)     (%5d bpf)     (%2.1f%% of total)\n",
	  numFrames3, numFrameBits3, numFrameBits3/numFrames3,
	  100.0*(float)numFrameBits3/(float)totalBits);
  io_printf(fpointer, "  Compression:  %3d:1     (%9.4f bpp)\n",
	  numFrames3*inputFrameBits/numFrameBits3,
	  24.0*(float)numFrameBits3/(float)(numFrames3*inputFrameBits));
  if ( printSNR )
    io_printf(fpointer, "  Avg Y SNR/PSNR:  %.1f     %.1f\n",
	    totalSNR3/(float)numFrames3, totalPSNR3/(float)numFrames3);
  if ( totalTime3 == 0) {
    io_printf(fpointer, "  Seconds:  NONE\n");
  } else {
    io_printf(fpointer, "  Seconds:  %9ld     (%9.4f fps)  (%9ld pps)  (%9ld mps)\n",
	    (long)(totalTime3/TIME_RATE),
	    (float)((float)(TIME_RATE*numFrames3)/(float)totalTime3),
	    (long)((float)TIME_RATE*(float)numFrames3*(float)inputFrameBits/(24.0*(float)totalTime3)),
	    (long)((float)TIME_RATE*(float)numFrames3*(float)inputFrameBits/(256.0*24.0*(float)totalTime3)));
  }

  return((float)totalTime3/(float)TIME_RATE);
}


/*===========================================================================*
 *
 * EstimateSecondsPerPFrame
 *
 *	compute an estimate of the number of seconds required per P-frame
 *
 * RETURNS:	the estimate, in seconds
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
float
  EstimateSecondsPerPFrame()
{
  if ( numFrames3 == 0) {
    return(10.0);
  } else {
    return((float)totalTime3/((float)TIME_RATE*(float)numFrames3));
  }
}


/*===========================================================================*
 *
 * ComputeHalfPixelData
 *
 *	compute all half-pixel data required for half-pixel motion vector
 *	search (luminance only)
 *
 * RETURNS:	frame->halfX, ->halfY, and ->halfBoth modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
  ComputeHalfPixelData(frame)
MpegFrame *frame;
{
  register int x, y;

  /* we add 1 before dividing by 2 because .5 is supposed to be rounded up
   * (see MPEG-1, page D-31)
   */

  if ( frame->halfX == NULL) {	/* need to allocate memory */
    Frame_AllocHalf(frame);
  }

  /* compute halfX */
  for ( y = 0; y < Fsize_y; y++) {
    for ( x = 0; x < Fsize_x-1; x++) {
      frame->halfX[y][x] = (frame->ref_y[y][x]+
			    frame->ref_y[y][x+1]+1)>>1;
    }
  }

  /* compute halfY */
  for ( y = 0; y < Fsize_y-1; y++) {
    for ( x = 0; x < Fsize_x; x++) {
      frame->halfY[y][x] = (frame->ref_y[y][x]+
			    frame->ref_y[y+1][x]+1)>>1;
    }
  }

  /* compute halfBoth */
  for ( y = 0; y < Fsize_y-1; y++) {
    for ( x = 0; x < Fsize_x-1; x++) {
      frame->halfBoth[y][x] = (frame->ref_y[y][x]+
			       frame->ref_y[y][x+1]+
			       frame->ref_y[y+1][x]+
			       frame->ref_y[y+1][x+1]+2)>>2;
    }
  }

  frame->halfComputed = TRUE;
}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 *			      USER-MODIFIABLE
 *
 * ZeroMotionBetter
 *
 *	decide if (0,0) motion is better than the given motion vector
 *
 * RETURNS:	TRUE if (0,0) is better, FALSE if (my,mx) is better
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:	The relevant block in 'current' is valid (it has not
 *			been dct'd).  'zeroDiff' has already been computed
 *			as the LumMotionError() with (0,0) motion
 *
 * NOTES:	This procedure follows the algorithm described on
 *		page D-48 of the MPEG-1 specification
 *
 *===========================================================================*/
static boolean
ZeroMotionBetter(currentBlock, prev, by, bx, my, mx)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int my;
    int mx;
{
    int	bestDiff;
    int CompareMode;

    /* Junk needed to adapt for TUNEing */ 
    CompareMode = SearchCompareMode;
    SearchCompareMode = DEFAULT_SEARCH;
    bestDiff = LumMotionError(currentBlock, prev, by, bx, my, mx, 0x7fffffff);
    SearchCompareMode = CompareMode;

    if ( zeroDiff < 256*3) {
	if ( 2*bestDiff >= zeroDiff) {
	    return(TRUE);
	}
    } else {
	if ( 11*bestDiff >= 10*zeroDiff) {
	    return(TRUE);
	}
    }

    return(FALSE);
}


/*===========================================================================*
 *
 *			      USER-MODIFIABLE
 *
 * DoIntraCode
 *
 *	decide if intra coding is necessary
 *
 * RETURNS:	TRUE if intra-block coding is better; FALSE if not
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:	The relevant block in 'current' is valid (it has not
 *			been dct'd).
 *
 * NOTES:	This procedure follows the algorithm described on
 *		page D-49 of the MPEG-1 specification
 *
 *===========================================================================*/
static boolean
DoIntraCode(currentBlock, prev, by, bx, motionY, motionX)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int motionY;
    int motionX;
{
    int	    x, y;
    int32 sum = 0, vard = 0, varc = 0, dif;
    int32 currPixel, prevPixel;
    LumBlock	motionBlock;

    ComputeMotionLumBlock(prev, by, bx, motionY, motionX, motionBlock);

    for ( y = 0; y < 16; y++) {
	for ( x = 0; x < 16; x++) {
	    currPixel = currentBlock[y][x];
	    prevPixel = motionBlock[y][x];

	    sum += currPixel;
	    varc += currPixel*currPixel;

	    dif = currPixel - prevPixel;
	    vard += dif*dif;
	}
    }

    vard >>= 8;		/* divide by 256; assumes mean is close to zero */
    varc = (varc>>8) - (sum>>8)*(sum>>8);

    if ( vard <= 64) {
	return(FALSE);
    } else if ( vard < varc) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}


/*===========================================================================*
 *
 *			      USER-MODIFIABLE
 *
 * ZeroMotionSufficient
 *
 *	decide if zero motion is sufficient without DCT correction
 *
 * RETURNS:	TRUE no DCT required; FALSE otherwise
 *
 * SIDE EFFECTS:    none
 *
 * PRECONDITIONS:	The relevant block in 'current' is raw YCC data
 *
 *===========================================================================*/
static boolean
ZeroMotionSufficient(currentBlock, prev, by, bx)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
{
    LumBlock	motionBlock;
    register int    fy, fx;
    register int    x, y;

    fy = by*DCTSIZE;
    fx = bx*DCTSIZE;
    for ( y = 0; y < 16; y++) {
	for ( x = 0; x < 16; x++) {
	    motionBlock[y][x] = prev->ref_y[fy+y][fx+x];
	}
    }

    zeroDiff = LumBlockMAD(currentBlock, motionBlock, 0x7fffffff);

    return((zeroDiff <= 256));
}
			     

#ifdef UNUSED_PROCEDURES
static void
ComputeAndPrintPframeMAD(currentBlock, prev, by, bx, my, mbx, numBlock)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int my;
    int mx;
    int numBlock;
{
    LumBlock	lumMotionBlock;
    int32   mad;

    ComputeMotionLumBlock(prev, by, bx, my, mx, lumMotionBlock);

    mad = LumBlockMAD(currentBlock, lumMotionBlock, 0x7fffffff);

    if (! realQuiet) {
	io_printf(stdout, "%d %d\n", numBlock, mad);
    }
}
#endif
/*===========================================================================*
 * postdct.c								     *
 *									     *
 *	Procedures concerned with MPEG post-DCT processing:		     *
 *	    quantization and RLE Huffman encoding			     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	Mpost_QuantZigBlock						     *
 *	Mpost_RLEHuffIBlock						     *
 *	Mpost_RLEHuffPBlock						     *
 *	Mpost_UnQuantZigBlock						     *
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
 * Revision 1.12  1995/06/21  18:26:39  smoot
 * added length estimator for P-blocks
 *
 * Revision 1.11  1995/04/23  23:22:59  eyhung
 * nothing changed
 *
 * Revision 1.10  1995/04/14  23:10:46  smoot
 * Added overflow detection to MPOST_DCT so it will adjust Qscales (above)
 *
 * Revision 1.9  1995/02/15  23:15:32  smoot
 * killed useless asserts
 *
 * Revision 1.8  1995/02/01  21:48:41  smoot
 * assure out is set properly, short circuit 0 revquant
 *
 * Revision 1.7  1995/01/30  19:56:37  smoot
 * Killed a <0 shift
 *
 * Revision 1.6  1995/01/25  23:07:33  smoot
 * Better DBG_PRINTs, multiply/divide instead of shifts
 *
 * Revision 1.5  1995/01/19  23:09:10  eyhung
 * Changed copyrights
 *
 * Revision 1.4  1995/01/16  08:17:08  eyhung
 * added realQuiet
 *
 * Revision 1.3  1994/11/12  02:11:58  keving
 * nothing
 *
 * Revision 1.2  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.2  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.1  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.11  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.10  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.9  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.8  1993/02/24  18:57:19  keving
 * nothing
 *
 * Revision 1.7  1993/02/23  22:58:36  keving
 * nothing
 *
 * Revision 1.6  1993/02/23  22:54:56  keving
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


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include <assert.h>
#include "all.h"
#include "mtypes.h"
#include "bitio.h"
#include "huff.h"
#include "postdct.h"
#include "opts.h"

/*==================*
 * STATIC VARIABLES *
 *==================*/

/* ZAG[i] is the natural-order position of the i'th element of zigzag order. */
int ZAG[] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

/*
 * possible optimization: reorder the qtable in the correct zigzag order, to
 * reduce the number of necessary lookups
 *
 * this table comes from the MPEG draft, p. D-16, Fig. 2-D.15.
 */
int32 qtable[] = {
    8, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83
};

int32 niqtable[] = {
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16
};

int32	*customQtable = NULL;
int32	*customNIQtable = NULL;

/*==================*
 * GLOBAL VARIABLES *
 *==================*/

extern boolean realQuiet;
/*NOTE*/
#endif

/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * Mpost_UnQuantZigBlock
 *
 *	unquantize and zig-zag (decode) a single block
 *	see section 2.4.4.1 of MPEG standard
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mpost_UnQuantZigBlock(in, out, qscale, iblock)
    FlatBlock in;
    Block out;
    int qscale;
    boolean iblock;
{
    register int index;
    int	    start;
    int	    position;
    register int	    qentry;
    int	    level, coeff;
    
    if ( iblock) {
	/* qtable[0] must be 8 */
	out[0][0] = (int16)(in[0] * 8);

	/* don't need to do anything fancy here, because we saved orig
	    value, not encoded dc value */
	start = 1;
    } else {
	start = 0;
    }

    for ( index = start;  index < DCTSIZE_SQ;  index++) {
	position = ZAG[index];
	level = in[index];

	if (level == 0) {
	  ((int16 *)out)[position] = 0;
	  continue;
	}


	if ( iblock) {
	    qentry = qtable[position] * qscale;
	    coeff = (level*qentry)/8;
	    if ( (coeff & 1) == 0) {
		if ( coeff < 0) {
		    coeff++;
		} else if ( coeff > 0) {
		    coeff--;
		}
	    }
	} else {
	    qentry = niqtable[position] * qscale;
	    if ( level == 0) {
		coeff = 0;
	    } else if ( level < 0) {
		coeff = (((2*level)-1)*qentry) / 16;
		if ( (coeff & 1) == 0) {
		    coeff++;
		}
	    } else {
		coeff = (((2*level)+1)*qentry) >> 4;
		if ( (coeff & 1) == 0) {
		    coeff--;
		}
	    }

	    if ( coeff > 2047) {
		coeff = 2047;
	    } else if ( coeff < -2048) {
		coeff = -2048;
	    }
        }

	((int16 *)out)[position] = coeff;
    }
}


/*===========================================================================*
 *
 * Mpost_QuantZigBlock
 *
 *	quantize and zigzags a block
 *
 * RETURNS:	MPOST_OVERFLOW if a generated value is outside |255|
 *              MPOST_ZERO     if all coeffs are zero
 *              MPOST_NON_ZERO otherwisw
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
Mpost_QuantZigBlock(in, out, qscale, iblock)
    Block in;
    FlatBlock out;
    register int qscale;
    int iblock;
{
  register int i;
  register int16 temp;
  register int qentry;
  register int position;
  boolean nonZero = FALSE;
  boolean overflow = FALSE;
  
  DBG_PRINT(("Mpost_QuantZigBlock...\n"));
  if (iblock) {
    /*
     * the DC coefficient is handled specially -- it's not
     * sensitive to qscale, but everything else is
     */
    temp = ((int16 *) in)[ZAG[0]];
    qentry = qtable[ZAG[0]];
    
    if (temp < 0) {
      temp = -temp;
      temp += (qentry >> 1);
      temp /= qentry;
      temp = -temp;
    } else {
      temp += (qentry >> 1);
      temp /= qentry;
    }
    if ( temp != 0) {
      nonZero = TRUE;
    }
    out[0] = temp;
    
    for (i = 1; i < DCTSIZE_SQ; i++) {
      position = ZAG[i];
      temp = ((int16 *) in)[position];
      qentry = qtable[position] * qscale;
      
      /* see 1993 MPEG doc, section D.6.3.4 */
      if (temp < 0) {
	temp = -temp;
	temp = (temp << 3);	/* temp > 0 */
	temp += (qentry >> 1);
	temp /= qentry;
	temp = -temp;
      } else {
	temp = (temp << 3);	/* temp > 0 */
	temp += (qentry >> 1);
	temp /= qentry;
      }
      
      if ( temp != 0) {
	nonZero = TRUE;
	out[i] = temp;
	if (temp < -255) {
	  temp = -255;
	  overflow = TRUE;
	} else if (temp > 255) {
	  temp = 255;
	  overflow = TRUE;
	}
      } else out[i]=0;
    }
  } else {
    for (i = 0; i < DCTSIZE_SQ; i++) {
      position = ZAG[i];
      temp = ((int16 *) in)[position];
      
      /* multiply by non-intra qtable */
      qentry = qscale * niqtable[position];
      
      /* see 1993 MPEG doc, D.6.4.5 */
      temp *= 8;
      temp /= qentry;	    /* truncation toward 0 -- correct */
      
      if ( temp != 0) {
	nonZero = TRUE;
	out[i] = temp;
	if (temp < -255) {
	  temp = -255;
	  overflow = TRUE;
	} else if (temp > 255) {
	  temp = 255;
	  overflow = TRUE;
	}
	
      } else out[i]=0;
    }
  }

 if (overflow) return(MPOST_OVERFLOW);
 if (nonZero)  return(MPOST_NON_ZERO);
 return(MPOST_ZERO);
}



/*===========================================================================*
 *
 * Mpost_RLEHuffIBlock
 *
 *	generate the huffman bits from an I-block
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mpost_RLEHuffIBlock(in, out)
    FlatBlock in;
    BitBucket *out;
{
    register int i;
    register int nzeros = 0;
    register int16 cur;
    register int16 acur;
    register u_int32_t code;
    register int nbits;

    /*
     * yes, Virginia, we start at 1.  The DC coefficient is handled
     * specially, elsewhere.  Not here.
     */
    for (i = 1; i < DCTSIZE_SQ; i++) {
	cur = in[i];
	acur = ABS(cur);
	if (cur) {
	    if ( (nzeros < HUFF_MAXRUN) && (acur < huff_maxlevel[nzeros])) {
	        /*
		 * encode using the Huffman tables
		 */

		DBG_PRINT(("rle_huff %02d.%02d: Run %02d, Level %4d\n", i,  ZAG[i], nzeros, cur));
		code = (huff_table[nzeros])[acur];
		nbits = (huff_bits[nzeros])[acur];

		if (cur < 0) {
		    code |= 1;	/* the sign bit */
		}
		Bitio_Write(out, code, nbits);
	    } else {
		/*
		 * encode using the escape code
		 */
		DBG_PRINT(("Escape\n"));
		Bitio_Write(out, 0x1, 6);	/* ESCAPE */
		DBG_PRINT(("Run Length\n"));
		Bitio_Write(out, nzeros, 6);	/* Run-Length */

		/*
	         * this shouldn't happen, but the other
	         * choice is to bomb out and dump core...
		 * Hmmm, seems to happen with small Qtable entries (1) -srs
	         */
		if (cur < -255) {
		    cur = -255;
		} else if (cur > 255) {
		    cur = 255;
		}

		DBG_PRINT(("Level\n"));
		if (acur < 128) {
		    Bitio_Write(out, cur, 8);
		} else {
		    if (cur < 0) {
			Bitio_Write(out, 0x8001 + cur + 255, 16);
		    } else {
			Bitio_Write(out, cur, 16);
		    }
		}
	    }
	    nzeros = 0;
	} else {
	    nzeros++;
	}
    }
    DBG_PRINT(("End of block\n"));
    Bitio_Write(out, 0x2, 2);	/* end of block marker */
}


/*===========================================================================*
 *
 * Mpost_RLEHuffPBlock
 *
 *	generate the huffman bits from an P-block
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
Mpost_RLEHuffPBlock(in, out)
    FlatBlock in;
    BitBucket *out;
{
    register int i;
    register int nzeros = 0;
    register int16 cur;
    register int16 acur;
    register u_int32_t code;
    register int nbits;
    boolean first_dct = TRUE;

    /*
     * yes, Virginia, we start at 0.
     */
    for (i = 0; i < DCTSIZE_SQ; i++) {
	cur = in[i];
	acur = ABS(cur);
	if (cur) {
	    if ((nzeros < HUFF_MAXRUN) && (acur < huff_maxlevel[nzeros])) {
	        /*
		 * encode using the Huffman tables
		 */

		DBG_PRINT(("rle_huff %02d.%02d: Run %02d, Level %4d\n", i, ZAG[i], nzeros, cur));
		if ( first_dct && (nzeros == 0) && (acur == 1)) {
		    /* actually, only needs = 0x2 */
		    code = (cur == 1) ? 0x2 : 0x3;
		    nbits = 2;
		} else {
		    code = (huff_table[nzeros])[acur];
		    nbits = (huff_bits[nzeros])[acur];
		  }

		assert(nbits);

		if (cur < 0) {
		    code |= 1;	/* the sign bit */
		}
		Bitio_Write(out, code, nbits);
		first_dct = FALSE;
	    } else {
		/*
		 * encode using the escape code
		 */
		DBG_PRINT(("Escape\n"));
		Bitio_Write(out, 0x1, 6);	/* ESCAPE */
		DBG_PRINT(("Run Length\n"));
		Bitio_Write(out, nzeros, 6);	/* Run-Length */

		/*
	         * this shouldn't happen, but the other
	         * choice is to bomb out and dump core...
		 * Hmmm, seems to happen with small Qtable entries (1) -srs
	         */
		if (cur < -255) {
		  cur = -255;
		} else if (cur > 255) {
		  cur = 255;
		}

		DBG_PRINT(("Level\n"));
		if (acur < 128) {
		    Bitio_Write(out, cur, 8);
		} else {
		    if (cur < 0) {
			Bitio_Write(out, 0x8001 + cur + 255, 16);
		    } else {
			Bitio_Write(out, cur, 16);
		    }
		}

		first_dct = FALSE;
	    }
	    nzeros = 0;
	} else {
	    nzeros++;
	}
    }

    /* actually, should REALLY return FALSE and not use this! */

    if ( first_dct) {	/* have to give a first_dct even if all 0's */
	io_printf(stderr, "HUFF called with all-zero coefficients\n");
	io_printf(stderr, "exiting...\n");
	exit(1);
    }

    DBG_PRINT(("End of block\n"));
    Bitio_Write(out, 0x2, 2);	/* end of block marker */
}


/*===========================================================================*
 *
 * CalcRLEHuffLength
 *
 *	count the huffman bits for an P-block
 *
 * RETURNS:	number of bits
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int
CalcRLEHuffLength(in)
    FlatBlock in;
{
  register int i;
  register int nzeros = 0;
  register int16 cur;
  register int16 acur;
  register int nbits;
  register int countbits=0;
  boolean first_dct = TRUE;
  
  for (i = 0; i < DCTSIZE_SQ; i++) {
    cur = in[i];
    acur = ABS(cur);
    if (cur) {
      if ((nzeros < HUFF_MAXRUN) && (acur < huff_maxlevel[nzeros])) {
	/*
	 * encode using the Huffman tables
	 */

	if ( first_dct && (nzeros == 0) && (acur == 1)) {
	  nbits = 2;
	} else {
	  nbits = (huff_bits[nzeros])[acur];
	}
	countbits += nbits;
	first_dct = FALSE;
      } else {
	countbits += 12;	/* ESCAPE + runlength */

	if (acur < 128) {
	  countbits += 8;
	} else {
	  countbits += 16;
	}

	first_dct = FALSE;
      }
      nzeros = 0;
    } else {
      nzeros++;
    }
  }
  
  countbits += 2; /* end of block marker */
  return(countbits);
}
/*===========================================================================*
 * psearch.c								     *
 *									     *
 *	Procedures concerned with the P-frame motion search		     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	SetPixelSearch							     *
 *	SetPSearchAlg							     *
 *	SetSearchRange							     *
 *	MotionSearchPreComputation					     *
 *	PMotionSearch							     *
 *	PSearchName							     *
 *	PSubSampleSearch						     *
 *	PLogarithmicSearch						     *
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
 * Revision 1.9  1995/01/19  23:09:12  eyhung
 * Changed copyrights
 *
 * Revision 1.9  1995/01/19  23:09:12  eyhung
 * Changed copyrights
 *
 * Revision 1.8  1994/12/07  00:40:36  smoot
 * Added seperate P and B search ranges
 *
 * Revision 1.7  1994/11/12  02:09:45  eyhung
 * full pixel bug
 * fixed on lines 512 and 563
 *
 * Revision 1.6  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.5  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.4  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.3  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.2  1993/06/03  21:08:08  keving
 * nothing
 *
 * Revision 1.1  1993/03/02  18:27:05  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "mtypes.h"
#include "frames.h"
#include "search.h"
#include "prototypes.h"
#include "fsize.h"
#include "param.h"


/*==================*
 * STATIC VARIABLES *
 *==================*/

/* none */


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

int **pmvHistogram = NULL;	/* histogram of P-frame motion vectors */
int **bbmvHistogram = NULL;	/* histogram of B-frame motion vectors */
int **bfmvHistogram = NULL;	/* histogram of B-frame motion vectors */
int pixelFullSearch;
int searchRangeP,searchRangeB;
int psearchAlg;
/*NOTE*/
#endif


/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * PMotionSearch
 *
 *	compute the best P-frame motion vector we can
 *	
 *
 * RETURNS:	TRUE	    =	motion vector valid
 *		FALSE	    =	motion vector invalid; should code I-block
 *
 * PRECONDITIONS:	The relevant block in 'current' is valid (it has not
 *			been dct'd).  Thus, the data in 'current' can be
 *			accesed through y_blocks, cr_blocks, and cb_blocks.
 *			This is not the case for the blocks in 'prev.'
 *			Therefore, references into 'prev' should be done
 *			through the struct items ref_y, ref_cr, ref_cb
 *
 * POSTCONDITIONS:	current, prev should be unchanged.
 *			Some computation could be saved by requiring
 *			the dct'd difference to be put into current's block
 *			elements here, depending on the search technique.
 *			However, it was decided that it mucks up the code
 *			organization a little, and the saving in computation
 *			would be relatively little (if any).
 *
 * NOTES:	the search procedure need not check the (0,0) motion vector
 *		the calling procedure has a preference toward (0,0) and it
 *		will check it itself
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
boolean
PMotionSearch(currentBlock, prev, by, bx, motionY, motionX)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
{
    /* CALL SEARCH PROCEDURE */

    switch(psearchAlg) {
	case PSEARCH_SUBSAMPLE:
	    PSubSampleSearch(currentBlock, prev, by, bx, motionY, motionX, searchRangeP);
	    break;
	case PSEARCH_EXHAUSTIVE:
	    PLocalSearch(currentBlock, prev, by, bx, motionY, motionX,
			 0x7fffffff, searchRangeP);
	    break;
	case PSEARCH_LOGARITHMIC:
	    PLogarithmicSearch(currentBlock, prev, by, bx, motionY, motionX, searchRangeP);
	    break;
	case PSEARCH_TWOLEVEL:
	    PTwoLevelSearch(currentBlock, prev, by, bx, motionY, motionX,
			    0x7fffffff, searchRangeP);
	    break;
	default:
	    io_printf(stderr, "ILLEGAL PSEARCH ALG:  %d\n", psearchAlg);
	    exit(1);
    }

    return(TRUE);
}


/*===========================================================================*
 *
 * SetPixelSearch
 *
 *	set the pixel search type (half or full)
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    pixelFullSearch
 *
 *===========================================================================*/
void
SetPixelSearch(searchType)
    char *searchType;
{
    if ( (strcmp(searchType, "FULL") == 0 ) || ( strcmp(searchType, "WHOLE") == 0 )) {
	pixelFullSearch = TRUE;
    } else if (strcmp(searchType, "HALF") == 0) {
	pixelFullSearch = FALSE;
    } else {
	io_printf(stderr, "ERROR:  Invalid pixel search type:  %s\n",
		searchType);
	exit(1);
    }
}


/*===========================================================================*
 *
 * SetPSearchAlg
 *
 *	set the P-search algorithm
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    psearchAlg
 *
 *===========================================================================*/
void
SetPSearchAlg(alg)
    char *alg;
{
    if (strcmp(alg, "EXHAUSTIVE") == 0) {
	psearchAlg = PSEARCH_EXHAUSTIVE;
    } else if (strcmp(alg, "SUBSAMPLE") == 0) {
	psearchAlg = PSEARCH_SUBSAMPLE;
    } else if (strcmp(alg, "LOGARITHMIC") == 0) {
	psearchAlg = PSEARCH_LOGARITHMIC;
    } else if (strcmp(alg, "TWOLEVEL") == 0) {
	psearchAlg = PSEARCH_TWOLEVEL;
    } else {
	io_printf(stderr, "ERROR:  Invalid psearch algorithm:  %s\n", alg);
	exit(1);
    }
}


/*===========================================================================*
 *
 * PSearchName
 *
 *	returns a string containing the name of the search algorithm
 *
 * RETURNS:	pointer to the string
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
char *
PSearchName()
{
    switch(psearchAlg) {
	case PSEARCH_EXHAUSTIVE:
	    return("EXHAUSTIVE");
	case PSEARCH_SUBSAMPLE:
	    return("SUBSAMPLE");
	case PSEARCH_LOGARITHMIC:
	    return("LOGARITHMIC");
	case PSEARCH_TWOLEVEL:
	    return("TWOLEVEL");
	default:
	    return("BAD_VALUE"); /* this function is not used by MPEG */
	    /*break;*/
    }
}


/*===========================================================================*
 *
 * SetSearchRange
 *
 *	sets the range of the search to the given number of pixels
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    searchRange*, fCode
 *
 *===========================================================================*/

void SetSearchRange(int pixelsP, int pixelsB)
   {register int index;
    int na, nl;

    searchRangeP = 2*pixelsP;	/* +/- 'pixels' pixels */
    searchRangeB = 2*pixelsB;
    searchRangeB = 2*pixelsB;

    if (computeMVHist)
       {int max_search;

#if 0
	max_search = (searchRangeP > searchRangeB) ? 
	             ((searchRangeP > searchRangeB) ? searchRangeP : searchRangeB) :
		     ((searchRangeB > searchRangeB) ? searchRangeB : searchRangeB);
#else
	max_search = (searchRangeP > searchRangeB) ? searchRangeP : searchRangeB;
#endif	

	nl = 2*max_search + 3;

	na = 2*searchRangeP + 3;
	pmvHistogram = CMAKE_N(int *, na);
	if (pmvHistogram != NULL)
	   {for (index = 0; index < nl; index++)
	        pmvHistogram[index] = (int *) calloc(na, sizeof(int));};

	na = 2*searchRangeB + 3;
	bbmvHistogram = CMAKE_N(int *, na);
	if (bbmvHistogram != NULL)
	   {for (index = 0; index < 2*nl; index++)
	        bbmvHistogram[index] = (int *) calloc(na, sizeof(int));};

	bfmvHistogram = CMAKE_N(int *, na);
	if (bfmvHistogram != NULL)
	   {for (index = 0; index < 2*nl; index++)
	        bfmvHistogram[index] = (int *) calloc(na, sizeof(int));};};

    return;}


/*===========================================================================*
 *
 *				USER-MODIFIABLE
 *
 * MotionSearchPreComputation
 *
 *	do whatever you want here; this is called once per frame, directly
 *	after reading
 *
 * RETURNS:	whatever
 *
 * SIDE EFFECTS:    whatever
 *
 *===========================================================================*/
void
MotionSearchPreComputation(frame)
    MpegFrame *frame;
{
    /* do nothing */
}


/*===========================================================================*
 *
 * PSubSampleSearch
 *
 *	uses the subsampling algorithm to compute the P-frame vector
 *
 * RETURNS:	motion vector
 *
 * SIDE EFFECTS:    none
 *
 * REFERENCE:  Liu and Zaccarin:  New Fast Algorithms for the Estimation
 *		of Block Motion Vectors, IEEE Transactions on Circuits
 *		and Systems for Video Technology, Vol. 3, No. 2, 1993.
 *
 *===========================================================================*/
int32
PSubSampleSearch(currentBlock, prev, by, bx, motionY, motionX, searchRange)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int searchRange;
{
    register int mx, my;
    int32 diff, bestBestDiff;
    int	    stepSize;
    register int x;
    int	    bestMY[4], bestMX[4], bestDiff[4];
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;

    stepSize = (pixelFullSearch ? 2 : 1);

    COMPUTE_MOTION_BOUNDARY(by,bx,stepSize,leftMY,leftMX,rightMY,rightMX);

    if ( searchRange < rightMY) {
	rightMY = searchRange;
    }

    if ( searchRange < rightMX) {
	rightMX = searchRange;
    }

    for ( x = 0; x < 4; x++) {
	bestMY[x] = 0;
	bestMX[x] = 0;
	bestDiff[x] = 0x7fffffff;
    }

    /* do A pattern */
    for ( my = -searchRange; my < rightMY; my += 2*stepSize) {
	if ( my < leftMY) {
	    continue;
	}

	for ( mx = -searchRange; mx < rightMX; mx += 2*stepSize) {
	    if ( mx < leftMX) {
		continue;
	    }

	    diff = LumMotionErrorA(currentBlock, prev, by, bx, my, mx, bestDiff[0]);

	    if ( diff < bestDiff[0]) {
		bestMY[0] = my;
		bestMX[0] = mx;
		bestDiff[0] = diff;
	    }
	}
    }

    /* do B pattern */
    for ( my = stepSize-searchRange; my < rightMY; my += 2*stepSize) {
	if ( my < leftMY) {
	    continue;
	}

	for ( mx = -searchRange; mx < rightMX; mx += 2*stepSize) {
	    if ( mx < leftMX) {
		continue;
	    }

	    diff = LumMotionErrorB(currentBlock, prev, by, bx, my, mx, bestDiff[1]);

	    if ( diff < bestDiff[1]) {
		bestMY[1] = my;
		bestMX[1] = mx;
		bestDiff[1] = diff;
	    }
	}
    }

    /* do C pattern */
    for ( my = stepSize-searchRange; my < rightMY; my += 2*stepSize) {
	if ( my < leftMY) {
	    continue;
	}

	for ( mx = stepSize-searchRange; mx < rightMX; mx += 2*stepSize) {
	    if ( mx < leftMX) {
		continue;
	    }

	    diff = LumMotionErrorC(currentBlock, prev, by, bx, my, mx, bestDiff[2]);

	    if ( diff < bestDiff[2]) {
		bestMY[2] = my;
		bestMX[2] = mx;
		bestDiff[2] = diff;
	    }
	}
    }

    /* do D pattern */
    for ( my = -searchRange; my < rightMY; my += 2*stepSize) {
	if ( my < leftMY) {
	    continue;
	}

	for ( mx = stepSize-searchRange; mx < rightMX; mx += 2*stepSize) {
	    if ( mx < leftMX) {
		continue;
	    }

	    diff = LumMotionErrorD(currentBlock, prev, by, bx, my, mx, bestDiff[3]);

	    if ( diff < bestDiff[3]) {
		bestMY[3] = my;
		bestMX[3] = mx;
		bestDiff[3] = diff;
	    }
	}
    }

    /* first check old motion */
    if ( (*motionY >= leftMY) && (*motionY < rightMY) &&
	 (*motionX >= leftMX) && (*motionX < rightMX)) {
	bestBestDiff = LumMotionError(currentBlock, prev, by, bx, *motionY, *motionX, 0x7fffffff);
    } else {
	bestBestDiff = 0x7fffffff;
    }

    /* look at Error of 4 different motion vectors */
    for ( x = 0; x < 4; x++) {
	bestDiff[x] = LumMotionError(currentBlock, prev, by, bx,
				 bestMY[x], bestMX[x], bestBestDiff);

	if ( bestDiff[x] < bestBestDiff) {
	    bestBestDiff = bestDiff[x];
	    *motionY = bestMY[x];
	    *motionX = bestMX[x];
	}
    }

    return(bestBestDiff);
}


/*===========================================================================*
 *
 * PLogarithmicSearch
 *
 *	uses logarithmic search to compute the P-frame vector
 *
 * RETURNS:	motion vector
 *
 * SIDE EFFECTS:    none
 *
 * REFERENCE:  MPEG-I specification, pages 32-33
 *
 *===========================================================================*/
int32
PLogarithmicSearch(currentBlock, prev, by, bx, motionY, motionX, searchRange)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int searchRange;
{
    register int mx, my;
    int32 diff, bestDiff;
    int	    stepSize;
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;
    int	    tempRightMY, tempRightMX;
    int	    spacing;
    int	    centerX, centerY;
    int	    newCenterX, newCenterY;

    stepSize = (pixelFullSearch ? 2 : 1);

    COMPUTE_MOTION_BOUNDARY(by,bx,stepSize,leftMY,leftMX,rightMY,rightMX);

    bestDiff = 0x7fffffff;

    /* grid spacing */
    if ( stepSize == 2) {	/* make sure spacing is even */
	spacing = (searchRange+1)/2;
	if ( (spacing % 2) != 0) {
	    spacing--;
	}
    } else {
	spacing = (searchRange+1)/2;
    }
    centerX = 0;
    centerY = 0;

    while ( spacing >= stepSize) {
	newCenterY = centerY;
	newCenterX = centerX;

	tempRightMY = rightMY;
	if ( centerY+spacing+1 < tempRightMY) {
	    tempRightMY = centerY+spacing+1;
	}
	tempRightMX = rightMX;
	if ( centerX+spacing+1 < tempRightMX) {
	    tempRightMX = centerX+spacing+1;
	}

	for ( my = centerY-spacing; my < tempRightMY; my += spacing) {
	    if ( my < leftMY) {
		continue;
	    }

	    for ( mx = centerX-spacing; mx < tempRightMX; mx += spacing) {
		if ( mx < leftMX) {
		    continue;
		}

		diff = LumMotionError(currentBlock, prev, by, bx, my, mx, bestDiff);

		if ( diff < bestDiff) {
		    newCenterY = my;
		    newCenterX = mx;

		    bestDiff = diff;
		}
	    }
	}

	centerY = newCenterY;
	centerX = newCenterX;

	if ( stepSize == 2) {	/* make sure spacing is even */
	    if ( spacing == 2) {
		spacing = 0;
	    } else {
		spacing = (spacing+1)/2;
		if ( (spacing % 2) != 0) {
		    spacing--;
		}
	    }
	} else {
	    if ( spacing == 1) {
		spacing = 0;
	    } else {
		spacing = (spacing+1)/2;
	    }
	}
    }

    /* check old motion -- see if it's better */
    if ( (*motionY >= leftMY) && (*motionY < rightMY) &&
	 (*motionX >= leftMX) && (*motionX < rightMX)) {
	diff = LumMotionError(currentBlock, prev, by, bx, *motionY, *motionX, bestDiff);
    } else {
	diff = 0x7fffffff;
    }

    if ( bestDiff < diff) {
	*motionY = centerY;
	*motionX = centerX;
    } else {
	bestDiff = diff;
    }

    return(bestDiff);
}


/*===========================================================================*
 *
 * PLocalSearch
 *
 *	uses local exhaustive search to compute the P-frame vector
 *
 * RETURNS:	motion vector
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int32
PLocalSearch(currentBlock, prev, by, bx, motionY, motionX, bestSoFar, searchRange)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int32 bestSoFar;
    int searchRange;
{
    register int mx, my;
    int32 diff, bestDiff;
    int	    stepSize;
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;
    int	    distance;
    int	    tempRightMY, tempRightMX;

    stepSize = (pixelFullSearch ? 2 : 1);

    COMPUTE_MOTION_BOUNDARY(by,bx,stepSize,leftMY,leftMX,rightMY,rightMX);

    /* try old motion vector first */
    if ( VALID_MOTION(*motionY, *motionX)) {
	bestDiff = LumMotionError(currentBlock, prev, by, bx, *motionY, *motionX, bestSoFar);

	if ( bestSoFar < bestDiff) {
	    bestDiff = bestSoFar;
	}
    } else {
	*motionY = 0;
	*motionX = 0;

	bestDiff = bestSoFar;
    }

    /* try a spiral pattern */    
    for ( distance = stepSize; distance <= searchRange;
	  distance += stepSize) {
	tempRightMY = rightMY;
	if ( distance < tempRightMY) {
	    tempRightMY = distance;
	}
	tempRightMX = rightMX;
	if ( distance < tempRightMX) {
	    tempRightMX = distance;
	}

	/* do top, bottom */
	for ( my = -distance; my < tempRightMY;
	      my += max(tempRightMY+distance-stepSize, stepSize)) {
	    if ( my < leftMY) {
		continue;
	    }

	    for ( mx = -distance; mx < tempRightMX; mx += stepSize) {
		if ( mx < leftMX) {
		    continue;
		}

		diff = LumMotionError(currentBlock, prev, by, bx, my, mx, bestDiff);

		if ( diff < bestDiff) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		}
	    }
	}

	/* do left, right */
	for ( mx = -distance; mx < tempRightMX;
	      mx += max(tempRightMX+distance-stepSize, stepSize)) {
	    if ( mx < leftMX) {
		continue;
	    }

	    for ( my = -distance+stepSize; my < tempRightMY-stepSize;
		  my += stepSize) {
		if ( my < leftMY) {
		    continue;
		}

		diff = LumMotionError(currentBlock, prev, by, bx, my, mx, bestDiff);

		if ( diff < bestDiff) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		}
	    }
	}
    }

    return(bestDiff);
}


/*===========================================================================*
 *
 * PTwoLevelSearch
 *
 *	uses two-level search to compute the P-frame vector
 *	first does exhaustive full-pixel search, then looks at neighboring
 *	half-pixel motion vectors
 *
 * RETURNS:	motion vector
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int32
PTwoLevelSearch(currentBlock, prev, by, bx, motionY, motionX, bestSoFar, searchRange)
    LumBlock currentBlock;
    MpegFrame *prev;
    int by;
    int bx;
    int *motionY;
    int *motionX;
    int32 bestSoFar;
    int searchRange;
{
    register int mx, my;
    register int   loopInc;
    int32 diff, bestDiff;
    int	    leftMY, leftMX;
    int	    rightMY, rightMX;
    int	    distance;
    int	    tempRightMY, tempRightMX;
    int	    xOffset, yOffset;

    /* exhaustive full-pixel search first */

    COMPUTE_MOTION_BOUNDARY(by,bx,2,leftMY,leftMX,rightMY,rightMX);

    rightMY--;
    rightMX--;

    /* convert vector into full-pixel vector */
    if ( *motionY > 0) {
	if ( ((*motionY) % 2) == 1) {
	    (*motionY)--;
	}
    } else if ( ((-(*motionY)) % 2) == 1) {
	(*motionY)++;
    }

    if ( *motionX > 0) {
	if ( ((*motionX) % 2) == 1) {
	    (*motionX)--;
	}
    } else if ( ((-(*motionX)) % 2) == 1) {
	(*motionX)++;
    }

    /* try old motion vector first */
    if ( VALID_MOTION(*motionY, *motionX)) {
	bestDiff = LumMotionError(currentBlock, prev, by, bx, *motionY, *motionX, bestSoFar);

	if ( bestSoFar < bestDiff) {
	    bestDiff = bestSoFar;
	}
    } else {
	*motionY = 0;
	*motionX = 0;

	bestDiff = bestSoFar;
    }

    rightMY++;
    rightMX++;

    /* try a spiral pattern */    
    for ( distance = 2; distance <= searchRange; distance += 2) {
	tempRightMY = rightMY;
	if ( distance < tempRightMY) {
	    tempRightMY = distance;
	}
	tempRightMX = rightMX;
	if ( distance < tempRightMX) {
	    tempRightMX = distance;
	}

	/* do top, bottom */
	loopInc = max(tempRightMY+distance-2, 2);
	for ( my = -distance; my < tempRightMY; my += loopInc) {
	    if ( my < leftMY) {
		continue;
	    }

	    for ( mx = -distance; mx < tempRightMX; mx += 2) {
		if ( mx < leftMX) {
		    continue;
		}

		diff = LumMotionError(currentBlock, prev, by, bx, my, mx, bestDiff);

		if ( diff < bestDiff) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		}
	    }
	}

	/* do left, right */
	loopInc = max(tempRightMX+distance-2, 2);
	for ( mx = -distance; mx < tempRightMX; mx += loopInc) {
	    if ( mx < leftMX) {
		continue;
	    }

	    for ( my = -distance+2; my < tempRightMY-2; my += 2) {
		if ( my < leftMY) {
		    continue;
		}

		diff = LumMotionError(currentBlock, prev, by, bx, my, mx, bestDiff);

		if ( diff < bestDiff) {
		    *motionY = my;
		    *motionX = mx;
		    bestDiff = diff;
		}
	    }
	}
    }

    /* now look at neighboring half-pixels */
    my = *motionY;
    mx = *motionX;

    rightMY--;
    rightMX--;

    for ( yOffset = -1; yOffset <= 1; yOffset++) {
	for ( xOffset = -1; xOffset <= 1; xOffset++) {
	    if ( (yOffset == 0) && (xOffset == 0) )
		continue;

	    if ( VALID_MOTION(my+yOffset, mx+xOffset) &&
		 ((diff = LumMotionError(currentBlock, prev, by, bx,
			 my+yOffset, mx+xOffset, bestDiff)) < bestDiff)) {
		*motionY = my+yOffset;
		*motionX = mx+xOffset;
		bestDiff = diff;
	    }
	}
    }

    return(bestDiff);
}


void ShowPMVHistogram(FILE *fpointer)
   {int rowTotal;
    register int x, y;
    int	*columnTotals;

    columnTotals = (int *) calloc(2*searchRangeP+3, sizeof(int));
    if (columnTotals == NULL)
       return;

#ifdef COMPLETE_DISPLAY
    io_printf(fpointer, "    ");
    for ( y = 0; y < 2*searchRange+3; y++) {
	io_printf(fpointer, "%3d ", y-searchRangeP-1);
    }
    io_printf(fpointer, "\n");
#endif

    for ( x = 0; x < 2*searchRangeP+3; x++) {
#ifdef COMPLETE_DISPLAY
	io_printf(fpointer, "%3d ", x-searchRangeP-1);
#endif
	rowTotal = 0;
	for ( y = 0; y < 2*searchRangeP+3; y++) {
	    io_printf(fpointer, "%3d ", pmvHistogram[x][y]);
	    rowTotal += pmvHistogram[x][y];
	    columnTotals[y] += pmvHistogram[x][y];
	}
#ifdef COMPLETE_DISPLAY
	io_printf(fpointer, "%4d\n", rowTotal);
#else
	io_printf(fpointer, "\n");
#endif
    }

#ifdef COMPLETE_DISPLAY
    io_printf(fpointer, "Tot ");
    for ( y = 0; y < 2*searchRangeP+3; y++) {
	io_printf(fpointer, "%3d ", columnTotals[y]);
    }
#endif
    io_printf(fpointer, "\n");

    free(columnTotals);

    return;}


void ShowBBMVHistogram(FILE *fpointer)
   {int rowTotal;
    register int x, y;
    int	*columnTotals;

    io_printf(fpointer, "B-frame Backwards:\n");

    columnTotals = (int *) calloc(2*searchRangeB+3, sizeof(int));
    if (columnTotals == NULL)
       return;

#ifdef COMPLETE_DISPLAY
    io_printf(fpointer, "    ");
    for ( y = 0; y < 2*searchRangeB+3; y++) {
	io_printf(fpointer, "%3d ", y-searchRangeB-1);
    }
    io_printf(fpointer, "\n");
#endif

    for ( x = 0; x < 2*searchRangeB+3; x++) {
#ifdef COMPLETE_DISPLAY
	io_printf(fpointer, "%3d ", x-searchRangeB-1);
#endif
	rowTotal = 0;
	for ( y = 0; y < 2*searchRangeB+3; y++) {
	    io_printf(fpointer, "%3d ", bbmvHistogram[x][y]);
	    rowTotal += bbmvHistogram[x][y];
	    columnTotals[y] += bbmvHistogram[x][y];
	}
#ifdef COMPLETE_DISPLAY
	io_printf(fpointer, "%4d\n", rowTotal);
#else
	io_printf(fpointer, "\n");
#endif
    }

#ifdef COMPLETE_DISPLAY
    io_printf(fpointer, "Tot ");
    for ( y = 0; y < 2*searchRangeB+3; y++) {
	io_printf(fpointer, "%3d ", columnTotals[y]);
    }
#endif
    io_printf(fpointer, "\n");

    free(columnTotals);

    return;}


void ShowBFMVHistogram(FILE *fpointer)
   {int rowTotal;
    register int x, y;
    int	*columnTotals;

    io_printf(fpointer, "B-frame Forwards:\n");

    columnTotals = (int *) calloc(2*searchRangeB+3, sizeof(int));
    if (columnTotals == NULL)
       return;

#ifdef COMPLETE_DISPLAY
    io_printf(fpointer, "    ");
    for ( y = 0; y < 2*searchRangeB+3; y++) {
	io_printf(fpointer, "%3d ", y-searchRangeB-1);
    }
    io_printf(fpointer, "\n");
#endif

    for ( x = 0; x < 2*searchRangeB+3; x++) {
#ifdef COMPLETE_DISPLAY
	io_printf(fpointer, "%3d ", x-searchRangeB-1);
#endif
	rowTotal = 0;
	for ( y = 0; y < 2*searchRangeB+3; y++) {
	    io_printf(fpointer, "%3d ", bfmvHistogram[x][y]);
	    rowTotal += bfmvHistogram[x][y];
	    columnTotals[y] += bfmvHistogram[x][y];
	}
#ifdef COMPLETE_DISPLAY
	io_printf(fpointer, "%4d\n", rowTotal);
#else
	io_printf(fpointer, "\n");
#endif
    }

#ifdef COMPLETE_DISPLAY
    io_printf(fpointer, "Tot ");
    for ( y = 0; y < 2*searchRangeB+3; y++) {
	io_printf(fpointer, "%3d ", columnTotals[y]);
    }
#endif
    io_printf(fpointer, "\n");

    free(columnTotals);

    return;}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/

    /* none */

/*============================================================================*
 * rate.c								      *
 *									      * 
 *	Procedures concerned with rate control                                *
 *									      *
 * EXPORTED PROCEDURES:							      *
 *      initRatecontrol()                                                     *
 *      targetRateControl()                                                   *
 *      updateRateControl()                                                   *
 *      MB_RateOut()                                                          *
 *      needQScaleChange()                                                    *
 *      incNumBlocks()                                                        *
 *      incQuant()                                                            *
 *	incMacroBlockBits()                                                   *
 *      setPictureRate()                                                      *
 *      setBitRate()                                                          *
 *      getBitRate()                                                          *
 *      setBufferSize()                                                       *
 *      getBufferSize()                                                       *
 *                                                                            *
 * NOTES:                                                                     *
 *	Naming conventions follow those of MPEG-2 draft algorithm (chap. 10)  *
 *============================================================================*/


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

/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include <sys/times.h>
#include "all.h"
#include "mtypes.h"
#include "bitio.h"
#include "frames.h"
#include "prototypes.h"
#include "param.h"
#include "mheaders.h"
#include "fsize.h"
#include "postdct.h"
#include "mpeg.h"
#include "parallel.h"
#include "dct.h"
#include "rate.h"
/*NOTE*/
#endif


/*==================*
 * GLOBAL VARIABLES *
 *==================*/

#define MAX_BIT_RATE 104857600		/* 18 digit number in units of 400 */
#define MAX_BUFFER_SIZE 16760832        /* 10 digit number in units of 16k */
#define DEFAULT_BUFFER_SIZE 327680      /* maximun for "constrained" bitstream */
#define DEFAULT_VBV_FULLNESS 3          /* wait till 1/3 full */
#define DEFAULT_PICT_RATE_CODE 5        /* code for 30 Frames/sec */
#define DEFAULT_PICT_RATE 30            /* 30 frames per second */
#define MAX_VBV_DELAY 32768             /* 16 digits */


/*	  Variables from Parameter File */

static int	RateControlMode = VARIABLE_RATE;
static int32 buffer_size = DEFAULT_BUFFER_SIZE;
static int32 bit_Rate = -1;


/*   Variables for the VBV buffer defined in MPEG specs */
static int32 VBV_delay =0;	    /* delay in units of 1/90000 seconds */
static int32 VBV_buffer = 0;	    /* fullness of the theoretical VBV buffer */
static int32 bufferFillRate = 0;    /* constant rate at which buffer filled */
static int32 frameDelayIncrement = 0;	/* number of "delay" units/Frame */

/*  Global complexity measure variables */
static int Xi, Xp, Xb;  /*  Global complexity measure  */

static int Si, Sp, Sb;  /*  Total # bits for last pict of type (Overhead?) */

static float Qi, Qp, Qb; /* avg quantizaton for last picture of type  */
     
/*  Target bit allocations for each type of picture*/
int Ti, Tp, Tb;

int current_Tx;	/* allocation for current frame */

/*  Count of number of pictures of each type remaining */
int GOP_X = 0;
int GOP_I = 0;
int GOP_P = 0;
int GOP_B = 0;

int _Nx = 0;
int _Ni = 0;
int _Np = 0;
int _Nb = 0;

/*   Counters used while encoding frames   */

int rc_numBlocks = 0;
int rc_totalQuant = 0;
int rc_bitsThisMB;
int rc_totalMBBits;
int rc_totalFrameBits;
int rc_totalOverheadBits = 0;


/*	Want to print out Macroblock info every Nth MB */
int RC_MB_SAMPLE_RATE = 0;

static float Ki = .7;
static float Kp = 1;
static float Kb = 1.4;
static int rc_R;
static int rc_G;

/*   Rate Control variables   */

/*   Virtual buffers for each frame type */
static int d0_i;   /* Initial fullnesses */
static int d0_p;
static int d0_b;

static int lastFrameVirtBuf;   /* fullness after last frame of this type */
static int currentVirtBuf;     /* fullness during current encoding*/

static int MB_cnt = -1;	       /* Number of MB's in picture */

static int rc_Q;               /* reference quantization parameter */

static int reactionParameter;  /*  Reaction parameter */

/*	Adaptive Quantization variables */
static int act_j;              /*  spatial activity measure */
static float N_act;            /*  Normalised spacial activity */
static int avg_act;	   /*  average activity value in last picture encoded */
static int total_act_j;	       /*  Sum of activity values in current frame */

static int var_sblk;	       /* sub-block activity */
static int P_mean;	       /* Mean value of pixels in 8x8 sub-block */

static int mquant;	       /* Raw Quantization value */
static int Qscale;	       /* Clipped, truncated quantization value */



/*  Output-related variables */
/* to log stuff use: #define RC_STATS_FILE */
#ifdef RC_STATS_FILE
static FILE *RC_FILE;
#endif

#if 0
static char *Frame_header1 = "  Fm         #     Bit      GOP                    V                ";
static char *Frame_header2 = "   #  type   MBs   Alloc    left  Ni Np Nb  N_act  buff   Q_rc Qscale";
static char *Frame_header3 = "----     -  ----  ------ -------  -- -- --  -----  ------ ----   ----";
static char *Frame_trailer1 = "                      avg          virt     %    GOP      %     VBV";
static char *Frame_trailer2 = "    Sx    Qx      Xx  act N_act  buffer alloc    left  left     buf  delay";
static char *Frame_trailer3 = "------ --.-- -------  --- --.-- -------   --- -------   --- ------- ------";

static char *MB_header1 = "MB#  #bits  Q mqt     Dj  Q_j   actj  N_act  totbits b/MB %alloc %done";
static char *MB_header2 = "---  ----- -- --- ------  ---  -----  --.--   ------ ----    ---   ---";
#endif

static char rc_buffer[101];

/*	EXTERNAL Variables  */
#if 0
/*NOTE*/
extern char *framePattern;
extern int framePatternLen;
/*NOTE*/
#endif


/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

int initGOPRateControl _ANSI_ARGS_((void));
     int determineMBCount _ANSI_ARGS_((void));
     void checkBufferFullness _ANSI_ARGS_((int count));
     void checkSpatialActivity _ANSI_ARGS_((Block blk0, Block blk1, Block blk2, Block blk3));
     void incNumBlocks _ANSI_ARGS_((int num));
     void calculateVBVDelay _ANSI_ARGS_((int num));
     void updateVBVBuffer _ANSI_ARGS_((int frameBits));
     int BlockExperiments  _ANSI_ARGS_((int16 *OrigBlock, int16 *NewBlock, int control));
     
     
     /*=====================*
      * EXPORTED PROCEDURES *
      *=====================*/
     
     /*===========================================================================*
      *
      * initRateControl
      *
      *	initialize the allocation parameters.
      *
      * RETURNS:	nothing
      *
      * SIDE EFFECTS:   many global variables 
      *
      * NOTES:  Get rid of the redundant pattern stuff!!
      *===========================================================================*/
     int
       initRateControl()
{
  int index;
  int result;
  
  DBG_PRINT(("\tInitializing Allocation Data\n"));
  
#ifdef RC_STATS_FILE
  RC_FILE = _PG_fopen("RC_STATS_FILE", "w");
  if ( RC_FILE  == NULL) {
    DBG_PRINT(("\tOpen of RC file failed, using stderr\n"));
    RC_FILE = stderr;
    io_printf(RC_FILE, "\tOpen of RC file failed, using stderr\n");
    io_flush(RC_FILE);
  }
#endif
  
  /*  Initialize Pattern info */
  GOP_X = framePatternLen;
  for ( index = 0; index < framePatternLen; index++) {
    switch( framePattern[index]) {
    case 'i':
      GOP_I++;
      break;
    case 'p':
      GOP_P++;
      break;
    case 'b':
      GOP_B++;
      break;
    default:
      printf("\n\tERROR rate.c - BAD PATTERN!\n");
      RateControlMode = VARIABLE_RATE;
      return(0);
    }
  }
  if (GOP_X != (GOP_I + GOP_P + GOP_B )) {
    printf("\n\tERROR rate.c - Pattern Length Mismatch\n");
    RateControlMode = VARIABLE_RATE;
    return(-1);
  }
  
  /* Initializing GOP bit allocation */	
  rc_R = 0;
  rc_G = (bit_Rate * GOP_X/frameRateRounded);
  
  /*   Initialize the "global complexity measures" */
  Xi = (160 * bit_Rate/115);
  Xp = (60 * bit_Rate/115);
  Xb = (42 * bit_Rate/115);
  
  /*   Initialize MB counters */
  rc_totalMBBits= rc_bitsThisMB= rc_totalFrameBits=rc_totalOverheadBits = 0;
  rc_numBlocks = rc_totalQuant = 0;
  
  /*   init virtual buffers  */
  reactionParameter = (2 * bit_Rate / frameRateRounded);
  d0_i = (10 * reactionParameter / 31);
  d0_p = (Kp * d0_i);
  d0_b = (Kb * d0_i);
  
  lastFrameVirtBuf = d0_i;	/*  start with I Frame */
  rc_Q = lastFrameVirtBuf  * 31 / reactionParameter;
  
  /*   init spatial activity measures */
  avg_act = 400;		/* Suggested initial value */
  N_act = 1;
  
  mquant = rc_Q * N_act;
  
  frameDelayIncrement = (90000 / frameRateRounded); /* num of "delay" units per frame */
  bufferFillRate = bit_Rate / frameRateRounded; /* VBV buf fills at constant rate */
  VBV_buffer = buffer_size;
  DBG_PRINT(("\tVBV- delay: %d, fill rate: %d, delay/Frame: %d units, buffer size: %d\n",
	     VBV_delay, bufferFillRate, frameDelayIncrement, buffer_size));
  
  result = initGOPRateControl();
  
  return(result);
}

/*===========================================================================*
 *
 * initGOPRateControl
 *
 *		(re)-initialize the RC for the a new Group of Pictures.
 *	New bit allocation, but carry over complexity measures.
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:   many global variables 
 *
 *===========================================================================*/
int
  initGOPRateControl()
{
  DBG_PRINT(("\tInitializing new GOP\n"));
  
  _Nx = GOP_X;
  _Ni = GOP_I;
  _Np = GOP_P;
  _Nb = GOP_B;
  
  rc_R += rc_G;
  
  DBG_PRINT(("\tbufsize: %d, bitrate: %d, pictrate: %d, GOP bits: %d\n",
	     buffer_size, bit_Rate, frameRateRounded, rc_R));
  DBG_PRINT(("\tXi: %d, Xp: %d, Xb: %d Nx: %d, Ni: %d, Np: %d, Nb: %d\n",
	     Xi, Xp, Xb, Nx,Ni,Np,Nb));
  DBG_PRINT(("\td0_i: %d, d0_p: %d, d0_b: %d, avg_act: %d, rc_Q: %d, mquant: %d\n",
	     d0_i, d0_p, d0_b, avg_act, rc_Q, mquant));
  return(1);
}


/*===========================================================================*
 *
 * targetRateControl
 *
 *      Determine the target allocation for given picture type, initiates
 *  variables for rate control process.
 *
 * RETURNS:     nothing.
 *
 * SIDE EFFECTS:   many global variables
 *
 *===========================================================================*/
void
  targetRateControl(frame)
MpegFrame   *frame;
{
  float temp1, minimumBits;
  float tempX, tempY, tempZ;
  int result;
  int frameType;
  
  minimumBits = (bit_Rate / (8 * frameRateRounded));
  
  /*   Check if new GOP */
  if (_Nx == 0) {
    initGOPRateControl();
  }
  
  if (MB_cnt < 0) {MB_cnt = determineMBCount();}
  
  switch (frame->type) {
  case TYPE_IFRAME:
    frameType = 'I';
    
    /*		temp1 = ( rc_R / ( 1+ ((_Np * Xp) / (Xi * Kp)) + ((Nb*Xb) / (Xi*Kb))))); */
    
    tempX = ( (_Np * Ki * Xp) / (Xi * Kp) );
    tempY = ( (_Nb * Ki * Xb) / (Xi*Kb) );
    tempZ = _Ni + tempX + tempY;
    temp1 = (rc_R / tempZ);
    result = (int) (temp1 > minimumBits ? temp1 :  minimumBits);
    current_Tx = Ti = result;
    lastFrameVirtBuf = d0_i;
    break;
    
  case TYPE_PFRAME:
    frameType = 'P';
    tempX =  ( (_Ni * Kp * Xi) / (Ki * Xp) );
    tempY =  ( (_Nb * Kp * Xb) / (Kb * Xp) );
    tempZ = _Np + tempX + tempY;
    temp1 = (rc_R/ tempZ);
    result = (int) (temp1 > minimumBits ? temp1 :  minimumBits);
    current_Tx = Tp = result;
    lastFrameVirtBuf = d0_p;
    break;
    
  case TYPE_BFRAME:
    frameType = 'B';
    tempX =  ( (_Ni * Kb * Xi) / (Ki * Xb) );
    tempY =  ( (_Np * Kb * Xp) / (Kp * Xb) );
    tempZ = _Nb + tempX + tempY;
    temp1 = (rc_R/ tempZ);
    result = (int) (temp1 > minimumBits ? temp1 :  minimumBits);
    current_Tx = Tb = result;
    lastFrameVirtBuf = d0_b;
    break;
    
  default:
    frameType = 'X';
  }
  
  N_act = 1;
  rc_Q = lastFrameVirtBuf  * 31 / reactionParameter;
  mquant = rc_Q * N_act;
  Qscale = (mquant > 31 ? 31 : mquant);
  Qscale = (Qscale < 1 ? 1 : Qscale);
  
  /*   Print headers for Frame info */
#if 0
  char *strPtr;
  strPtr = Frame_header1;
  DBG_PRINT(("%s\n",strPtr));
  strPtr = Frame_header2;
  DBG_PRINT(("%s\n",strPtr));
  strPtr = Frame_header3;
  DBG_PRINT(("%s\n",strPtr));
#endif
  
  /*   Print Frame info */
  snprintf(rc_buffer, 101, "%4d     %1c  %4d  %6d %7d  %2d %2d %2d   %2.2f  %6d %4d    %3d",
	   frame->id, frameType, MB_cnt,current_Tx, rc_R,
	   _Ni, _Np, _Nb,
	   N_act, lastFrameVirtBuf, rc_Q, Qscale);
  
#ifdef RC_STATS_FILE
  io_printf(RC_FILE,"%s\n", rc_buffer);
  io_flush(RC_FILE);
#endif
  DBG_PRINT(("%s\n",rc_buffer));
  
  /*  Print headers for Macroblock info */
  if (RC_MB_SAMPLE_RATE) {
#if 0
    strPtr = MB_header1;
    DBG_PRINT(("%s\n",strPtr));
    strPtr = MB_header2;
    DBG_PRINT(("%s\n",strPtr));
#endif
  } else {
    return;
  }
  
  return;
}



/*===========================================================================*
 *
 * updateRateControl
 *
 *      Update the statistics kept, after end of frame.  Resets
 *  various global variables
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   many global variables
 *
 *===========================================================================*/
void
  updateRateControl(type)
int type; 
{
  int totalBits, frameComplexity, pctAllocUsed, pctGOPUsed;
  float avgQuant;
  
  totalBits = rc_totalFrameBits;
  avgQuant = ((float) rc_totalQuant / (float) rc_numBlocks);
  frameComplexity = totalBits * avgQuant;
  pctAllocUsed = (totalBits *100 / current_Tx);
  rc_R -= totalBits;
  pctGOPUsed = (rc_R *100/ rc_G);
  
  avg_act = (total_act_j / MB_cnt);
  
  updateVBVBuffer(totalBits);
  
  switch (type) {
  case TYPE_IFRAME:
    Ti = current_Tx;
    d0_i = currentVirtBuf;
    _Ni--;
    Si = totalBits;
    Qi = avgQuant;
    Xi = frameComplexity;
    break;
  case TYPE_PFRAME:
    Tp = current_Tx;
    d0_p = currentVirtBuf;
    _Np--;
    Sp = totalBits;
    Qp = avgQuant;
    Xp = frameComplexity;
    break;
  case TYPE_BFRAME:
    Tb = current_Tx;
    d0_b = currentVirtBuf;
    _Nb--;
    Sb = totalBits;
    Qb = avgQuant;
    Xb = frameComplexity;
    break;
  }
  
  
  /*  Print Frame info */
#if 0
  char *strPtr;
  strPtr = Frame_trailer1;
  DBG_PRINT(("%s\n",strPtr));
  strPtr = Frame_trailer2;
  DBG_PRINT(("%s\n",strPtr));
  strPtr = Frame_trailer3;
  DBG_PRINT(("%s\n",strPtr));
#endif
  
  snprintf(rc_buffer, 101, "%6d  %2.2f  %6d  %3d  %2.2f %7d   %3d %7d   %3d  %6d %6d",
	  totalBits, avgQuant, frameComplexity, avg_act, N_act, currentVirtBuf, pctAllocUsed, rc_R, pctGOPUsed, VBV_buffer, VBV_delay);
#ifdef RC_STATS_FILE
  io_printf(RC_FILE,"%s\n", rc_buffer);
  io_flush(RC_FILE);
#endif
  DBG_PRINT(("%s\n",rc_buffer));
  
  _Nx--;
  rc_totalMBBits= rc_bitsThisMB= rc_totalFrameBits=rc_totalOverheadBits = 0;
  rc_numBlocks = rc_totalQuant = total_act_j = currentVirtBuf = 0;
  
  DBG_PRINT(("GOP now has %d bits remaining (%3d%%) for %d frames .. , Ni= %d, Np= %d, Nb= %d\n", rc_R, (rc_R*100/rc_G), (Ni+Np+Nb), Ni, Np, Nb));
  
}


/*===========================================================================*
 *
 * MB_RateOut
 *
 *      Prints out sampling of MB rate control data.  Every "nth" block
 *	stats are printed, with "n" controled by global RC_MB_SAMPLE_RATE
 *	(NB. "skipped" blocks do not go through this function and thus do not
 *		show up in the sample )
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   none
 *
 * NOTES:
 *
 *===========================================================================*/
void
  MB_RateOut(type)
int type;
{
  int totalBits;
  int pctUsed, pctDone;
  int bitsThisMB;
  int bitsPerMB;
  
  bitsThisMB = rc_bitsThisMB;
  totalBits = rc_totalFrameBits;
  bitsPerMB = (totalBits / rc_numBlocks); 
  pctDone = (rc_numBlocks * 100/ MB_cnt); 
  pctUsed = (totalBits *100/current_Tx);
  
  snprintf(rc_buffer, 101, "%3d  %5d %2d %3d %6d  %3d %6d   %2.2f   %6d %4d    %3d   %3d\n",
	  (rc_numBlocks - 1), bitsThisMB, Qscale, mquant, currentVirtBuf, 
	  rc_Q, act_j, N_act, totalBits, bitsPerMB, pctUsed, pctDone);
#ifdef RC_STATS_FILE
  io_printf(RC_FILE, "%s", rc_buffer);
  io_flush(RC_FILE);
#endif
  
  if ( (RC_MB_SAMPLE_RATE) && ((rc_numBlocks -1) % RC_MB_SAMPLE_RATE)) {
    DBG_PRINT(("%s\n", rc_buffer));
  } else {
    return;
  }
}



/*===========================================================================*
 *
 * incNumBlocks()
 *
 *
 * RETURNS:   nothing
 *
 * SIDE EFFECTS:  rc_numBlocks
 *
 * NOTES:
 *
 *===========================================================================*/
void incNumBlocks(num)
     int num;
{
  rc_numBlocks += num;
}


/*===========================================================================*
 *
 * incMacroBlockBits()
 *
 *	Increments the number of Macro Block bits and the total of Frame
 *  bits by the number passed.
 *
 * RETURNS:   nothing
 *
 * SIDE EFFECTS:  rc_totalMBBits
 *
 * NOTES:
 *
 *===========================================================================*/
void incMacroBlockBits(num)
     int num;
{
  rc_bitsThisMB = num;
  rc_totalMBBits += num;
  rc_totalFrameBits += num;
}


/*===========================================================================*
 *
 *   	needQScaleChange(current Q scale, 4 luminance blocks)
 *
 *
 * RETURNS:     new Qscale
 *
 * SIDE EFFECTS:   
 *
 *===========================================================================*/
int needQScaleChange(oldQScale, blk0, blk1, blk2, blk3)
     int oldQScale;
     Block blk0;
     Block blk1;
     Block blk2;
     Block blk3;
{
  
  /*   One more MacroBlock seen */
  rc_numBlocks++;		/* this notes each block num in MB */
  
  checkBufferFullness(oldQScale);
  
  checkSpatialActivity(blk0, blk1, blk2, blk3);
  
  mquant = rc_Q * N_act;
  Qscale = (mquant > 31 ? 31 : mquant);
  Qscale = (Qscale < 1 ? 1 : Qscale);
  rc_totalQuant += Qscale;
  
  if (oldQScale == Qscale)
    return(-1);
  else
    return(Qscale);
}


/*===========================================================================*
 *
 * determineMBCount() 
 *
 *      Determines number of Macro Blocks in frame from the frame sizes
 *	passed.
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   sets the count passed
 *
 *===========================================================================*/
int
  determineMBCount ()
{
  int y,x;
  
  x = (Fsize_x +15)/16;
  y = (Fsize_y +15)/16;
  return(x * y);
}



/*===========================================================================*
 *
 * void checkBufferFullness ()
 *
 *      Calculates the fullness of the virtual buffer for each
 *  frame type.  Called before encoding each macro block.  Along
 *  with the normalisec spatial activity measure (N_act), it
 *  determine the quantization factor for the next macroblock.
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   the "currentVirtBuf" variable
 *
 * NOTES:
 *
 *===========================================================================*/
void checkBufferFullness (oldQScale)
     int oldQScale;
{
  int temp;
  
  temp = lastFrameVirtBuf + rc_totalFrameBits;
  temp -=  (current_Tx * rc_numBlocks / MB_cnt);
  currentVirtBuf = temp;
  
  rc_Q = (currentVirtBuf * 31 / reactionParameter);
  return;
}


/*===========================================================================*
 *
 * void checkSpatialActivity()
 *
 *      Calcualtes the spatial activity for the four luminance blocks of the
 *	macroblock.  Along with the normalised reference quantization parameter 
 *  (rc_Q) , it determines the quantization factor for the next macroblock.
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   the Adaptive quantization variables- act_j, N_act.
 *
 * NOTES:
 *
 *===========================================================================*/
void checkSpatialActivity(blk0, blk1, blk2, blk3)
     Block blk0;
     Block blk1;
     Block blk2;
     Block blk3;
{
  int temp;
  int16 *blkArray[4]; 
  int16 *curBlock;
  int16 *blk_ptr;
  int var[4];
  int i, j;
  
  
  blkArray[0] = (int16 *) blk0;
  blkArray[1] = (int16 *) blk1;
  blkArray[2] = (int16 *) blk2;
  blkArray[3] = (int16 *) blk3;
  
  
  for (i =0; i < 4; i++) {	/* Compute the activity in each block */
    curBlock = blkArray[i];
    blk_ptr = curBlock;
    P_mean = 0;
    /*  Find the mean pixel value */
    for (j=0; j < DCTSIZE_SQ; j ++) {
      P_mean += *(blk_ptr++);
      /*			P_mean += curBlock[j]; 
				if (curBlock[j] != *(blk_ptr++)) {
				printf("\n\tARRAY ERROR: block %d\n", j);
				}
				*/
    }
    P_mean /= DCTSIZE_SQ;
    
    /*  Now find the variance  */
    curBlock = blkArray[i];
    blk_ptr = curBlock;
    var[i] = 0;
    for (j=0; j < DCTSIZE_SQ; j++) {
#ifdef notdef
      if (curBlock[j] != *(blk_ptr++)) {
	printf("\n\tARRAY ERROR: block %d\n", j);
      }
      temp = curBlock[j] - P_mean;
#endif      
      temp = *(blk_ptr++) - P_mean;
      var[i] += (temp * temp);
    }
    var[i] /= DCTSIZE_SQ;
  }
  
  /*  Choose the minimum variance from the 4 blocks and use as the activity */
  var_sblk  = var[0];
  for (i=1; i < 4; i++) {
    var_sblk = (var_sblk < var[i] ? var_sblk : var[i]);
  }
  
  
  act_j = 1 + var_sblk;
  total_act_j += act_j;
  temp = (2 * act_j + avg_act);
  N_act = ( (float) temp / (float) (act_j + 2*avg_act) );
  
  return;
}




/*============================================================================*
 *
 * getRateMode ()
 *
 *      Returns the rate mode- interpreted as either Fixed or Variable
 *
 * RETURNS:     integer
 *
 * SIDE EFFECTS:   none
 *
 *
 *==========================================================================*/
int getRateMode()
{
  return(RateControlMode);
}


/*===========================================================================*
 *
 * setBitRate ()
 *
 *      Checks the string parsed from the parameter file.  Verifies
 *  number and sets global values. MPEG standard specifies that bit rate
 *	be rounded up to nearest 400 bits/sec.
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   global variables
 *
 * NOTES:	Should this be in the 400-bit units used in sequence header?
 *
 *===========================================================================*/
void setBitRate (charPtr)
     char * charPtr;
{
  int rate, rnd;
  
  rate = atoi(charPtr);
  if (rate > 0) {
    RateControlMode = FIXED_RATE;
  } else {
    printf("Parameter File Error:  invalid BIT_RATE: \"%s\", defaults to Variable ratemode\n",
	   charPtr);
    RateControlMode = VARIABLE_RATE;
    bit_Rate = -1;
  }
  rnd = (rate % 400);
  rate += (rnd ? 400 -rnd : 0); /* round UP to nearest 400 bps */
  rate = (rate > MAX_BIT_RATE ? MAX_BIT_RATE : rate);
  bit_Rate = rate;
  DBG_PRINT(("Bit rate is: %d\n", bit_Rate));
} 



/*===========================================================================*
 *
 * getBitRate ()
 *
 *      Returns the bit rate read from the parameter file.  This is the
 *  real rate in bits per second, not in 400 bit units as is written to
 *  the sequence header.
 *
 * RETURNS:     int (-1 if Variable mode operation)
 *
 * SIDE EFFECTS:   none
 *
 *===========================================================================*/
int getBitRate ()
{
  return(bit_Rate);
}




/*===========================================================================*
 *
 * setBufferSize ()
 *
 *      Checks the string parsed from the parameter file.  Verifies
 *  number and sets global values.
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   buffer_size global variable.
 *
 * NOTES:	The global is in bits, NOT the 16kb units used in sequence header
 *
 *===========================================================================*/
void setBufferSize (charPtr)
     char * charPtr;
{
  int size;
  
  size = atoi(charPtr);
  size = (size > MAX_BUFFER_SIZE ? MAX_BUFFER_SIZE : size);
  if (size > 0) {
    size = (16*1024) * ((size + (16*1024 - 1)) / (16*1024));
    buffer_size = size;
  } else {
    buffer_size = DEFAULT_BUFFER_SIZE;
    printf("Parameter File Error:  invalid BUFFER_SIZE: \"%s\", defaults to : %d\n",
	   charPtr, buffer_size);
  }
  DBG_PRINT(("Buffer size is: %d\n", buffer_size));
}


/*===========================================================================*
 *
 * getBufferSize ()
 *
 *      returns the buffer size read from the parameter file.  Size is
 *  in bits- not in units of 16k as written to the sequence header.
 *
 * RETURNS:     int (or -1 if invalid)
 *
 * SIDE EFFECTS:   none
 *
 *===========================================================================*/
int getBufferSize ()
{
  return(buffer_size);
}


/*===========================================================================*
 *
 * updateVBVBuffer ()
 *
 *      Update the VBV buffer after each frame.  This theoretical 
 * buffer is being filled at constant rate, given by the bit rate.
 * It is emptied as each frame is grabbed by the decoder.  Exception 
 * is that the deocder will wait until the "delay" is over.
 *
 * RETURNS:     nothing
 *
 * SIDE EFFECTS:   VBV_buffer
 *
 * NOTES:	
 *
 *===========================================================================*/
void updateVBVBuffer (frameBits)
     int frameBits;
{
  if (VBV_delay) {
    VBV_delay -= frameDelayIncrement;
    if (VBV_delay < 0) {
      VBV_delay = 0;
    }
    
  } else {
    VBV_buffer -= frameBits;
  }
  VBV_buffer += bufferFillRate;
  if (VBV_buffer < 0) {
    io_printf(stderr, "\tWARNING - VBV buffer underflow (%d)\n", VBV_buffer);
  }
  if (VBV_buffer > buffer_size) {
    io_printf(stderr, "WARNING - VBV buffer overflow (%d > %d)\n",
	    VBV_buffer, buffer_size);
  }
}
/*===========================================================================*
 * readframe.c								     *
 *									     *
 *	procedures to read in frames					     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	ReadFrame							     *
 *	SetFileType							     *
 *	SetFileFormat							     *
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
 *  Revision 1.28  1995/08/24 17:15:15  smoot
 *  phillips misspelling fixed
 *
 *  Revision 1.27  1995/08/14 22:31:40  smoot
 *  reads training info from PPms now (needed for piping reads)
 *
 *  Revision 1.26  1995/08/07 21:48:36  smoot
 *  better error reporting, JPG == JPEG now
 *
 *  Revision 1.25  1995/06/12 20:30:12  smoot
 *  added popen for OS2
 *
 * Revision 1.24  1995/06/08  20:34:36  smoot
 * added "b"'s to fopen calls to make MSDOS happy
 *
 * Revision 1.23  1995/05/03  10:16:01  smoot
 * minor compile bug with static f
 *
 * Revision 1.22  1995/05/02  22:00:12  smoot
 * added TUNEing, setting near-black values to black
 *
 * Revision 1.21  1995/03/27  21:00:01  eyhung
 * fixed bug with some long jpeg names
 *
 * Revision 1.20  1995/02/02  01:05:54  eyhung
 * Fixed aAdded error checking for stdin
 *
 * Revision 1.19  1995/02/01  05:01:12  eyhung
 * Removed troubleshooting printf
 *
 * Revision 1.18  1995/01/31  21:08:16  eyhung
 * Improved YUV_FORMAT strings with better algorithm
 *
 * Revision 1.17  1995/01/27  23:34:09  eyhung
 * Removed temporary JPEG files created by JMOVIE input
 *
 * Revision 1.16  1995/01/27  21:57:43  eyhung
 * Added case for reading original JMOVIES
 *
 * Revision 1.14  1995/01/24  23:47:51  eyhung
 * Confusion with Abekas format fixed : all other YUV revisions are wrong
 *
 * Revision 1.13  1995/01/20  00:02:30  smoot
 * added gamma correction
 *
 * Revision 1.12  1995/01/19  23:09:21  eyhung
 * Changed copyrights
 *
 * Revision 1.11  1995/01/17  22:23:07  aswan
 * AbekasYUV chrominance implementation fixed
 *
 * Revision 1.10  1995/01/17  21:26:25  smoot
 * Tore our average on Abekus/Phillips reconstruct
 *
 * Revision 1.9  1995/01/17  08:22:34  eyhung
 * Debugging of ReadAYUV
 *
 * Revision 1.8  1995/01/16  13:18:24  eyhung
 * Interlaced YUV format (e.g. Abekas) support added (slightly buggy)
 *
 * Revision 1.7  1995/01/16  06:58:23  eyhung
 * Added skeleton of ReadAYUV (for Abekas YUV files)
 *
 * Revision 1.6  1995/01/13  23:22:23  smoot
 * Added ReadY, so we can make black&white movies (how artsy!)
 *
 * Revision 1.5  1994/12/16  00:20:40  smoot
 * Now errors out on too small an input file
 *
 * Revision 1.4  1994/11/12  02:11:59  keving
 * nothing
 *
 * Revision 1.3  1994/03/15  00:27:11  keving
 * nothing
 *
 * Revision 1.2  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.1  1993/07/22  22:23:43  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "mtypes.h"
#include "frames.h"
#include "prototypes.h"
#include "parallel.h"
#include "param.h"
#include "readframe.h"
#include "fsize.h"
#include "rgbtoycc.h"
#include "jpeg.h"
#include "opts.h"
/*NOTE*/
#endif

#define PPM_READ_STATE_MAGIC	0
#define PPM_READ_STATE_WIDTH	1
#define PPM_READ_STATE_HEIGHT	2
#define PPM_READ_STATE_MAXVAL	3
#define PPM_READ_STATE_DONE	4


/*==================*
 * STATIC VARIABLES *
 *==================*/

static int  fileType = BASE_FILE_TYPE;
struct YuvLine {
	u_int8_t	data[4096];
	u_int8_t	y[2048];
	int8	cr[1024];
	int8	cb[1024];
};


/*==================*
 * Portability      *
 *==================*/
#ifdef __OS2__
  #define popen _popen
#endif
   

/*==================*
 * Global VARIABLES *
 *==================*/
#if 0
/*NOTE*/
extern boolean GammaCorrection;
extern float GammaValue;
extern int outputWidth,outputHeight;
boolean resizeFrame;
char *CurrFile;
/*NOTE*/
#endif

/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static char *ScanNextString _ANSI_ARGS_((char *inputLine, char *string));
static void ReadPNM _ANSI_ARGS_((FILE * fp, MpegFrame * mf));
static boolean	ReadPPM _ANSI_ARGS_((MpegFrame *mf, FILE *fpointer));
static void ReadEYUV _ANSI_ARGS_((MpegFrame * mf, FILE *fpointer,
				 int width, int height));
static void ReadAYUV _ANSI_ARGS_((MpegFrame * mf, FILE *fpointer,
				 int width, int height));
static void SeparateLine _ANSI_ARGS_((FILE *fpointer, struct YuvLine *lineptr,
				     int width));
static void ReadY _ANSI_ARGS_((MpegFrame * mf, FILE *fpointer,
				 int width, int height));
static void ReadSub4 _ANSI_ARGS_((MpegFrame * mf, FILE *fpointer,
				  int width, int height));
static void DoGamma  _ANSI_ARGS_((MpegFrame *mf, int width, int height));

static void DoKillDim _ANSI_ARGS_((MpegFrame *mf, int w, int h));

#define safe_fread(ptr,sz,len,fileptr)                           \
    if ((safe_read_count=io_read(ptr,sz,len,fileptr))!=len) {      \
      io_printf(stderr,"Input file too small! (%s)\n",CurrFile);   \
      exit(1);}                                                  \

/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/



void	SetResize(set)
    boolean	set;
{
    resizeFrame = set;
}



/*===========================================================================*
 *
 * ReadFrame
 *
 *	reads the given frame, performing conversion as necessary
 *	if addPath = TRUE, then must add the current path before the
 *	file name
 *
 * RETURNS:	frame modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
ReadFrame(frame, fileName, conversion, addPath)
    MpegFrame *frame;
    char *fileName;
    char *conversion;
    boolean addPath;
{
    FILE    *ifp;
    char    command[1024];
    MpegFrame    tempFrame;
    MpegFrame    *framePtr;
    static char    fullFileName[1024];

#ifdef BLEAH
    static int32    readDiskTime = 0;
    int32    diskStartTime, diskEndTime;

time(&diskStartTime);
#endif

    if ( resizeFrame) {
      tempFrame.inUse = FALSE;
      tempFrame.ppm_data = NULL;
      tempFrame.rgb_data = NULL;
      tempFrame.orig_y = NULL;
      tempFrame.y_blocks = NULL;
      tempFrame.decoded_y = NULL;
      tempFrame.halfX = NULL;
      framePtr = &tempFrame;
    } else {
      framePtr = frame;
    }

    if ( addPath) {
      snprintf(fullFileName, 1024, "%s/%s", currentPath, fileName);
    } else {
      snprintf(fullFileName, 1024, "%s", fileName);
    }

    CurrFile = fullFileName;

#ifdef BLEAH
    if ( ! childProcess) {
    io_printf(stdout, "+++++READING Frame %d  (type %d):  %s\n", framePtr->id,
            framePtr->type, fullFileName);
    }
#endif

    if ( fileType == ANY_FILE_TYPE) {
    char *convertPtr, *commandPtr, *charPtr;

      if ( stdinUsed) {
        io_printf(stderr, "ERROR : cannot use stdin with INPUT_CONVERT.\n");
        exit(1);
      }

      /* replace every occurrence of '*' with fullFileName */
    convertPtr = conversion;
    commandPtr = command;
    while ( *convertPtr != '\0') {
      while ( (*convertPtr != '\0') && (*convertPtr != '*')) {
	*commandPtr = *convertPtr;
	commandPtr++;
	convertPtr++;
      }

      if ( *convertPtr == '*') {
	/* copy fullFileName */
	charPtr = fullFileName;
	while ( *charPtr != '\0') {
	  *commandPtr = *charPtr;
	  commandPtr++;
	  charPtr++;
	}

	convertPtr++;		/* go past '*' */
      }
    }
    *commandPtr = '\0';
    
    if ( (ifp = (FILE *) popen(command, "r")) == NULL) {
      io_printf(stderr, "ERROR:  Couldn't execute input conversion command:\n");
      io_printf(stderr, "\t%s\n", command);
      io_printf(stderr, "errno = %d\n", errno);
      if ( ioServer) {
	io_printf(stderr, "IO SERVER:  EXITING!!!\n");
      } else {
	io_printf(stderr, "SLAVE EXITING!!!\n");
      }
      exit(1);
    }
    } else if (stdinUsed) {
      ifp = stdin;
    } else if ( (ifp = _PG_fopen(fullFileName, "rb")) == NULL) {
      io_printf(stderr, "ERROR:  Couldn't open input file %s\n",
              fullFileName);
      exit(1);
    }
    
    switch(baseFormat) {
    case YUV_FILE_TYPE:
    case SIGNED_YUV_FILE_TYPE:

      /* Encoder YUV */
      if ((strncmp (yuvConversion, "EYUV", 4) == 0) ||
	  (strncmp (yuvConversion, "UCB", 3) == 0) ) 
        {
	  ReadEYUV(framePtr, ifp, realWidth, realHeight);
        }

      /* Abekas-type (interlaced) YUV */
      else {
	ReadAYUV(framePtr, ifp, realWidth, realHeight);
      }

      break;
    case Y_FILE_TYPE:
      ReadY(framePtr, ifp, realWidth, realHeight);
      break;
    case PPM_FILE_TYPE:
      if ( ! ReadPPM(framePtr, ifp)) {
        io_printf(stderr, "Error reading PPM input file!!! (%s)\n", CurrFile);
        exit(1);
      }
      PPMtoYUV(framePtr);
      break;
    case PNM_FILE_TYPE:
      ReadPNM(ifp, framePtr);
      PNMtoYUV(framePtr);
      break;
    case SUB4_FILE_TYPE:
      ReadSub4(framePtr, ifp, yuvWidth, yuvHeight);
      break;
    case JPEG_FILE_TYPE:
    case JMOVIE_FILE_TYPE:
      ReadJPEG(framePtr, ifp);
      break;
    default:
      break;
    }
    
    if (! stdinUsed) {
      if ( fileType == ANY_FILE_TYPE) {
	int errorcode;
	if ( (errorcode = pclose(ifp)) != 0) {
	  io_printf(stderr, "WARNING:  Pclose reported error (%d)\n", errorcode);
	}
      } else {
        io_close(ifp);
      }
    }
    
    if ( baseFormat == JMOVIE_FILE_TYPE) {
      remove(fullFileName);
    }
    
    if ( resizeFrame) {
      Frame_Resize(frame, &tempFrame, Fsize_x, Fsize_y, outputWidth, outputHeight);
    }
    
#ifdef BLEAH
    time(&diskEndTime);
    
    readDiskTime += (diskEndTime-diskStartTime);
    
    io_printf(stdout, "cumulative disk read time:  %d seconds\n", readDiskTime);
#endif
    
    if ( GammaCorrection) {
      DoGamma(frame, Fsize_x, Fsize_y);
    }
    
    if ( kill_dim) {
      DoKillDim(frame, Fsize_x, Fsize_y);
    }
    
    MotionSearchPreComputation(frame);
  }


/*===========================================================================*
 *
 * SetFileType
 *
 *	set the file type to be either a base type (no conversion), or
 *	any type (conversion required)
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    fileType
 *
 *===========================================================================*/
void
  SetFileType(conversion)
char *conversion;
{
  if (strcmp(conversion, "*") == 0) {
    fileType = BASE_FILE_TYPE;
  } else {
    fileType = ANY_FILE_TYPE;
  }
}


/*===========================================================================*
 *
 * SetFileFormat
 *
 *	set the file format (PPM, PNM, YUV, JPEG)
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    baseFormat
 *
 *===========================================================================*/
void
  SetFileFormat(format)
char *format;
{
  if (strcmp(format, "PPM") == 0) {
    baseFormat = PPM_FILE_TYPE;
  } else if (( strcmp(format, "YUV") == 0 ) || (strcmp(format, "UNSIGNED_YUV") == 0 )) {
    baseFormat = YUV_FILE_TYPE;
  } else if (strcmp(format, "SIGNED_YUV") == 0) {
    baseFormat = SIGNED_YUV_FILE_TYPE;
  } else if (strcmp(format, "Y") == 0) {
    baseFormat = Y_FILE_TYPE;
  } else if (strcmp(format, "PNM") == 0) {
    baseFormat = PNM_FILE_TYPE;
  } else if (( strcmp(format, "JPEG") == 0 ) || ( strcmp(format, "JPG") == 0 )) {
    baseFormat = JPEG_FILE_TYPE;
  } else if (strcmp(format, "JMOVIE") == 0) {
    baseFormat = JMOVIE_FILE_TYPE;
  } else if (strcmp(format, "SUB4") == 0) {
    baseFormat = SUB4_FILE_TYPE;
  } else {
    io_printf(stderr, "ERROR:  Invalid file format:  %s\n", format);
    exit(1);
  }
}


/*===========================================================================*
 *
 * ReadPNM
 *
 *	read a PNM file
 *
 * RETURNS:	mf modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

static void ReadPNM(FILE *fp, MpegFrame *mf)
   {int x, y;
    int format;
    xelval maxval;

    if (mf == NULL)
       return;

    if (mf->rgb_data)
       {pnm_freearray(mf->rgb_data, Fsize_y);};

    mf->rgb_data = pnm_readpnm(fp, &x, &y, &maxval, &format);
    ERRCHK(mf->rgb_data, "pnm_readpnm");

    if (format != PPM_FORMAT)
       {if (maxval < 255)
	   {pnm_promoteformat(mf->rgb_data, x, y, maxval, format, 255, PPM_FORMAT);
	    maxval = 255;}
        else
	   pnm_promoteformat(mf->rgb_data, x, y, maxval, format, maxval, PPM_FORMAT);};

    if (maxval < 255)
       {pnm_promoteformat(mf->rgb_data, x, y, maxval, format, 255, format);
	maxval = 255;};

/* if this is the first frame read, set the global frame size */
    Fsize_Note(mf->id, x, y);

    mf->rgb_maxval = maxval;
    mf->rgb_format = PPM_FORMAT;

  return;}

/*===========================================================================*
 *
 * ReadIOConvert
 *
 *	do conversion; return a pointer to the appropriate file
 *
 * RETURNS:	pointer to the appropriate file
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
FILE *
  ReadIOConvert(fileName)
char *fileName;
{
  FILE	*ifp;
  char	command[1024];
  char	fullFileName[1024];
  char *convertPtr, *commandPtr, *charPtr;

  snprintf(fullFileName, 1024, "%s/%s", currentPath, fileName);

#ifdef BLEAH
  if ( ! childProcess) {
    io_printf(stdout, "+++++READING (IO CONVERT) Frame %d  (type %d):  %s\n", frame->id,
	    frame->type, fullFileName); }
#endif

  if (strcmp(ioConversion, "*") == 0) {
    char buff[1024];
    ifp = _PG_fopen(fullFileName, "rb");
    snprintf(buff, 1024, "fopen \"%s\"",fullFileName);
    ERRCHK(ifp, buff);
    return(ifp);
  }

  /* replace every occurrence of '*' with fullFileName */
  convertPtr = ioConversion;
  commandPtr = command;
  while ( *convertPtr != '\0') {
    while ( (*convertPtr != '\0') && (*convertPtr != '*')) {
      *commandPtr = *convertPtr;
      commandPtr++;
      convertPtr++;
    }

    if ( *convertPtr == '*') {
      /* copy fullFileName */
      charPtr = fullFileName;
      while ( *charPtr != '\0') {
	*commandPtr = *charPtr;
	commandPtr++;
	charPtr++;
      }

      convertPtr++;		/* go past '*' */
    }
  }
  *commandPtr = '\0';

  if ( (ifp = (FILE *) popen(command, "r")) == NULL) {
    io_printf(stderr, "ERROR:  Couldn't execute input conversion command:\n");
    io_printf(stderr, "\t%s\n", command);
    io_printf(stderr, "errno = %d\n", errno);
    if ( ioServer) {
      io_printf(stderr, "IO SERVER:  EXITING!!!\n");
    } else {
      io_printf(stderr, "SLAVE EXITING!!!\n");
    }
    exit(1);
  }

  return(ifp);
}



/*===========================================================================*
 *
 * ReadPPM
 *
 *	read a PPM file
 *
 * RETURNS:	TRUE if successful; FALSE otherwise; mf modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/

static boolean ReadPPM(MpegFrame *mf, FILE *fpointer)
   {int height, width, maxVal, state, safe_read_count;
    u_int8_t junk[4096];
    char inputBuffer[71], string[71];
    char *inputLine;
    register int y;

    height = 0;
    width  = 0;
    maxVal = 255;

    state = PPM_READ_STATE_MAGIC;

    while (state != PPM_READ_STATE_DONE)
       {if (io_gets(inputBuffer, 71, fpointer) == NULL)
	   return(FALSE);
	
	inputLine = inputBuffer;
 
	if (inputLine[0] == '#')
	   continue;

	if (SC_LAST_CHAR(inputLine) != '\n')
	   return(FALSE);

	switch (state)
	   {case PPM_READ_STATE_MAGIC:
	         inputLine = ScanNextString(inputLine, string);
	         if (inputLine == NULL)
		    return(FALSE);

		 if (strcmp(string, "P6") != 0)
		    return(FALSE);

		 state = PPM_READ_STATE_WIDTH;
/* no break */
	    case PPM_READ_STATE_WIDTH:
		 inputLine = ScanNextString(inputLine, string);
		 if (inputLine == NULL)
		    {if (inputLine == inputBuffer)
		        return(FALSE);
		     else
		        break;};

		 width = atoi(string);

		 state = PPM_READ_STATE_HEIGHT;

/* no break */
	    case PPM_READ_STATE_HEIGHT:
		 inputLine = ScanNextString(inputLine, string);
		 if (inputLine == NULL)
		    {if (inputLine == inputBuffer)
		        return(FALSE);
		     else
		        break;};

		 height = atoi(string);

		 state = PPM_READ_STATE_MAXVAL;

/* no break */
	    case PPM_READ_STATE_MAXVAL:
		 inputLine = ScanNextString(inputLine, string);
		 if (inputLine == NULL)
		    {if (inputLine == inputBuffer)
		        return(FALSE);
		     else
		        break;};

		 maxVal = atoi(string);

		 state = PPM_READ_STATE_DONE;
		 break;};};

    width  = (width < 0) ? 0 : width;
    width  = (width > 10000) ? 10000 : width;
    height = min(height, 1000000);
    height = max(height, 0);
    maxVal = (maxVal < 0) ? 0 : maxVal;
    maxVal = (maxVal > 1000000) ? 1000000 : maxVal;

    Fsize_Note(mf->id, width, height);

    mf->rgb_maxval = maxVal;

    Frame_AllocPPM(mf);

    for (y = 0; y < Fsize_y; y++)
        {safe_fread(mf->ppm_data[y], sizeof(char), 3*Fsize_x, fpointer);

/* read the leftover stuff on the right side */
	 safe_fread(junk, sizeof(char), 3*(width-Fsize_x), fpointer);};

/* read the leftover stuff to prevent broken pipe */
    for (y = Fsize_y; y < height; ++y)
       safe_fread(junk, sizeof(char), 3*Fsize_x, fpointer);

    return(TRUE);}

/*===========================================================================*
 *
 * ReadEYUV
 *
 *	read a Encoder-YUV file (concatenated Y, U, and V)
 *
 * RETURNS:	mf modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
  ReadEYUV(mf, fpointer, width, height)
MpegFrame *mf;
FILE *fpointer;
int width;
int height;
{
  register int y;
  u_int8_t   junk[4096];
  int     safe_read_count;

  Fsize_Note(mf->id, width, height);

  Frame_AllocYCC(mf);

  for (y = 0; y < Fsize_y; y++) { /* Y */
    safe_fread(mf->orig_y[y], 1, Fsize_x, fpointer);

    /* read the leftover stuff on the right side */
    if ( width != Fsize_x) {
      safe_fread(junk, 1, width-Fsize_x, fpointer);
    }
  }

  /* read the leftover stuff on the bottom */
  for (y = Fsize_y; y < height; y++) {
    safe_fread(junk, 1, width, fpointer);
  }

  for (y = 0; y < (Fsize_y >> 1); y++) { /* U */
    safe_fread(mf->orig_cb[y], 1, Fsize_x >> 1, fpointer);

    /* read the leftover stuff on the right side */
    if ( width != Fsize_x) {
      safe_fread(junk, 1, (width-Fsize_x)>>1, fpointer);
    }
  }

  /* read the leftover stuff on the bottom */
  for (y = (Fsize_y >> 1); y < (height >> 1); y++) {
    safe_fread(junk, 1, width>>1, fpointer);
  }

  for (y = 0; y < (Fsize_y >> 1); y++) { /* V */
    safe_fread(mf->orig_cr[y], 1, Fsize_x >> 1, fpointer);

    /* read the leftover stuff on the right side */
    if ( width != Fsize_x) {
      safe_fread(junk, 1, (width-Fsize_x)>>1, fpointer);
    }
  }

  /* ignore leftover stuff on the bottom */
}

/*===========================================================================*
 *
 * ReadAYUV
 *
 *	read an Abekas-YUV file
 *
 * RETURNS:	mf modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
  ReadAYUV(mf, fpointer, width, height)
MpegFrame *mf;
FILE *fpointer;
int width;
int height;
{
  register int x, y;
  struct  YuvLine line1, line2;
  u_int8_t   junk[4096];
  u_int8_t    *cbptr, *crptr; /*NOTE: replaced "int8" by "unit8" NOTE*/
  int     safe_read_count;

  memset(&line1, 0, sizeof(line1));
  memset(&line2, 0, sizeof(line2));

  Fsize_Note(mf->id, width, height);

  Frame_AllocYCC(mf);

  for (y = 0;  y < Fsize_y;  y += 2) {
    SeparateLine(fpointer, &line1, width);
    SeparateLine(fpointer, &line2, width);

    /* Copy the Y values for each line to the frame */
    for (x = 0; x < Fsize_x; x++) {
      mf->orig_y[y][x]   = line1.y[x];
      mf->orig_y[y+1][x] = line2.y[x];
    }

    cbptr = &(mf->orig_cb[y>>1][0]);
    crptr = &(mf->orig_cr[y>>1][0]);

    /* One U and one V for each two pixels horizontal as well */
    /* Toss the second line of Cr/Cb info, averaging was worse,
       so just subsample */
    if (baseFormat != SIGNED_YUV_FILE_TYPE) {
      for (x = 0;  x < (Fsize_x >> 1);  x++) {
	cbptr[x] =  line1.cb[x];
	crptr[x] =  line1.cr[x];
      }
    } else {
      for (x = 0;  x < (Fsize_x >> 1);  x++) {
	cbptr[x] =  line1.cb[x]+128;
	crptr[x] =  line1.cr[x]+128;
      }
    }
  }

  /* read the leftover stuff on the bottom */
  for (y = Fsize_y; y < height; y++) {
    safe_fread(junk, 1, width<<1, fpointer);
  }

}

/*===========================================================================*
 *
 * SeparateLine
 *
 *	Separates one line of pixels into Y, U, and V components
 *
 * RETURNS:	lineptr modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
  SeparateLine(fpointer, lineptr, width)
FILE *fpointer;
struct YuvLine *lineptr;
int width;
{
  u_int8_t   junk[4096];
  int8    *crptr, *cbptr;
  u_int8_t   *yptr;
  int     num, length;
  int     safe_read_count;
  
  
  /* Sets the deinterlacing pattern */
  
  /* shorthand for UYVY */
  if (strncmp(yuvConversion, "ABEKAS", 6) == 0) {
    strcpy(yuvConversion, "UYVY");
    
    /* shorthand for YUYV */
  } else if ((strncmp(yuvConversion, "PHILLIPS", 8) == 0) || 
	     (strncmp(yuvConversion, "PHILIPS", 7) == 0)) {
    strcpy(yuvConversion, "YUYV");
  }
  
  length = strlen (yuvConversion);
  
  if ((length % 2) != 0) {
    io_printf(stderr, "ERROR : YUV_FORMAT must represent two pixels, hence must be even in length.\n");
    exit(1);
  }

  /* each line in 4:2:2 chroma format takes 2X bytes to represent X pixels.
   * each line in 4:4:4 chroma format takes 3X bytes to represent X pixels.
   * Therefore, half of the length of the YUV_FORMAT represents 1 pixel.
   */
  safe_fread(lineptr->data, 1, Fsize_x*(length>>1), fpointer);

  /* read the leftover stuff on the right side */
  if ( width != Fsize_x) {
    safe_fread(junk, 1, (width-Fsize_x)*(length>>1), fpointer);
  }

  crptr = &(lineptr->cr[0]);
  cbptr = &(lineptr->cb[0]);
  yptr = &(lineptr->y[0]);

  for (num = 0; num < (Fsize_x*(length>>1)); num++) {
    switch (yuvConversion[num % length]) {
    case 'U':
    case 'u':
      *(cbptr++) = (int8) (lineptr->data[num]);
      break;
    case 'V':
    case 'v':
      *(crptr++) = (int8) (lineptr->data[num]);
      break;
    case 'Y':
    case 'y':
      *(yptr++) = (lineptr->data[num]);
      break;
    default:
      io_printf(stderr, "ERROR: YUV_FORMAT must be one of the following:\n");
      io_printf(stderr, "       ABEKAS\n");
      io_printf(stderr, "       EYUV\n");
      io_printf(stderr, "       PHILIPS\n");
      io_printf(stderr, "       UCB\n");
      io_printf(stderr, "       or any even-length string consisting of the letters U, V, and Y.\n");
      exit(1);
    }
	
  }

}


/*===========================================================================*
 *
 * ReadY
 *
 *	read a Y file
 *
 * RETURNS:	mf modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
  ReadY(mf, fpointer, width, height)
MpegFrame *mf;
FILE *fpointer;
int width;
int height;
{
  register int y;
  u_int8_t   junk[4096];
  int     safe_read_count;

  Fsize_Note(mf->id, width, height);

  Frame_AllocYCC(mf);

  for (y = 0; y < Fsize_y; y++) { /* Y */
    safe_fread(mf->orig_y[y], 1, Fsize_x, fpointer);

    /* read the leftover stuff on the right side */
    if ( width != Fsize_x) {
      safe_fread(junk, 1, width-Fsize_x, fpointer);
    }
  }

  /* read the leftover stuff on the bottom */
  for (y = Fsize_y; y < height; y++) {
    safe_fread(junk, 1, width, fpointer);
  }
    
  for (y = 0 ; y < (Fsize_y >> 1); y++) {
    memset(mf->orig_cb[y], 128, (Fsize_x>>1));
    memset(mf->orig_cr[y], 128, (Fsize_x>>1));
  }
}


/*===========================================================================*
 *
 * ReadSub4
 *
 *	read a YUV file (subsampled even further by 4:1 ratio)
 *
 * RETURNS:	mf modified
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
static void
  ReadSub4(mf, fpointer, width, height)
MpegFrame *mf;
FILE *fpointer;
int width;
int height;
{
  register int y;
  register int x;
  u_int8_t   buffer[1024];
  int     safe_read_count;

  Fsize_Note(mf->id, width, height);

  Frame_AllocYCC(mf);

  for (y = 0; y < (height>>1); y++) { /* Y */
    safe_fread(buffer, 1, width>>1, fpointer);
    for ( x = 0; x < (width>>1); x++) {
      mf->orig_y[2*y][2*x] = buffer[x];
      mf->orig_y[2*y][2*x+1] = buffer[x];
      mf->orig_y[2*y+1][2*x] = buffer[x];
      mf->orig_y[2*y+1][2*x+1] = buffer[x];
    }
  }

  for (y = 0; y < (height >> 2); y++) {	/* U */
    safe_fread(buffer, 1, width>>2, fpointer);
    for ( x = 0; x < (width>>2); x++) {
      mf->orig_cb[2*y][2*x] = buffer[x];
      mf->orig_cb[2*y][2*x+1] = buffer[x];
      mf->orig_cb[2*y+1][2*x] = buffer[x];
      mf->orig_cb[2*y+1][2*x+1] = buffer[x];
    }
  }

  for (y = 0; y < (height >> 2); y++) {	/* V */
    safe_fread(buffer, 1, width>>2, fpointer);
    for ( x = 0; x < (width>>2); x++) {
      mf->orig_cr[2*y][2*x] = buffer[x];
      mf->orig_cr[2*y][2*x+1] = buffer[x];
      mf->orig_cr[2*y+1][2*x] = buffer[x];
      mf->orig_cr[2*y+1][2*x+1] = buffer[x];
    }
  }
}


/*=====================*
 * INTERNAL PROCEDURES *
 *=====================*/

/*===========================================================================*
 *
 * ScanNextString
 *
 *	read a string from a input line, ignoring whitespace
 *
 * RETURNS:	pointer to position in input line after string
 *              NULL if all whitespace
 *              puts string in 'string'
 *
 * SIDE EFFECTS:    file stream munched a bit
 *
 *===========================================================================*/
static char *
  ScanNextString(inputLine, string)
char *inputLine;
char *string;
{
  /* skip whitespace */
  while ( isspace((int) *inputLine) && (*inputLine != '\n')) {
    inputLine++;
  }

  if ( *inputLine == '\n') {
    return(NULL);
  }

  while ( (! isspace((int) *inputLine)) && (*inputLine != '\n')) {
    *string = *inputLine;
    string++;
    inputLine++;
  }

  *string = '\0';

  return(inputLine);
}

/*===========================================================================*
 *
 * DoGamma
 *
 *	Gamma Correct the Lum values
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    Raises Y values to power gamma.
 *
 *===========================================================================*/
static void
  DoGamma(mf, w, h)
MpegFrame *mf;
int w,h;
{
  static int GammaVal[256];
  static boolean init_done=FALSE;
  int i,j;

  if (!init_done) {
    for(i=0; i<256; i++) 
      GammaVal[i]=(unsigned char) (pow(((double) i)/255.0,GammaValue)*255.0+0.5);
    init_done=TRUE;
  }

  for (i=0; i< h; i++) {	/* For each line */
    for (j=0; j<w; j++) {	/* For each Y value */
      mf->orig_y[i][j] = GammaVal[mf->orig_y[i][j]];
    }}
}




/*===========================================================================*
 *
 * DoKillDim
 *
 *	Applies an input filter to small Y values.
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    Changes Y values:
 *
 *  Output    |                 /
 |                /
 |               /
 |              !
 |             /
 |            !
 |           /
 |          -
 |        /
 |      --
 |     /
 |   --
 | /
 ------------------------
 ^ kill_dim_break
 ^kill_dim_end
 kill_dim_slope gives the slope (y = kill_dim_slope * x +0)
 from 0 to kill_dim_break                      
 *
 *===========================================================================*/

static void
  DoKillDim(mf, w, h)
MpegFrame *mf;
int w,h;
{
  static boolean init_done=FALSE;
  static unsigned char mapper[256];
  register int i,j;
  double slope, intercept;

  slope = (kill_dim_end - kill_dim_break*kill_dim_slope)*1.0 /
    (kill_dim_end - kill_dim_break);
  intercept = kill_dim_end * (1.0-slope);

  if (!init_done) {
    for(i=0; i<256; i++) {
      if (i >= kill_dim_end) {
        mapper[i] = (char) i;
      } else if (i >= kill_dim_break) {
        mapper[i] = (char) (slope*i + intercept);
      } else {			/* i <= kill_dim_break */
        mapper[i] = (char) floor(i*kill_dim_slope + 0.49999);
      }
    }
    init_done = TRUE;
  }

  for (i=0;  i < h;  i++) {	/* For each line */
    for (j=0;   j < w;   j++) { /* For each Y value */
      mf->orig_y[i][j] = mapper[mf->orig_y[i][j]];
    }}
}
/*===========================================================================*
 * rgbtoycc.c								     *
 *									     *
 *	Procedures to convert from RGB space to YUV space		     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	PNMtoYUV							     *
 *	PPMtoYUV							     *
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
 *  Revision 1.5  1995/08/14 22:32:16  smoot
 *  added better error message
 *
 *  Revision 1.4  1995/01/19 23:09:23  eyhung
 *  Changed copyrights
 *
 * Revision 1.3  1994/11/12  02:12:00  keving
 * nothing
 *
 * Revision 1.2  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.2  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.1  1993/07/22  22:23:43  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "frame.h"
#include "fsize.h"
#include "rgbtoycc.h"
/*NOTE*/
#endif


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * PNMtoYUV
 *
 *	convert PNM data into YUV data
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
PNMtoYUV(frame)
    MpegFrame *frame;
{
    register int x, y;
    register u_int8_t *dy0, *dy1;
    register u_int8_t *dcr, *dcb;
    register xel *src0, *src1;
    register int ydivisor, cdivisor;
    static boolean  first = TRUE;
    static float  mult299[1024], mult587[1024], mult114[1024];
    static float  mult16874[1024], mult33126[1024], mult5[1024];
    static float mult41869[1024], mult08131[1024];

    if ( first) {
        register int index;
	register int maxValue;

	maxValue = frame->rgb_maxval;

        for ( index = 0; index <= maxValue; index++) {
	    mult299[index] = index*0.29900;
	    mult587[index] = index*0.58700;
	    mult114[index] = index*0.11400;
	    mult16874[index] = -0.16874*index;
	    mult33126[index] = -0.33126*index;
	    mult5[index] = index*0.50000;
	    mult41869[index] = -0.41869*index;
	    mult08131[index] = -0.08131*index;
	}
	
	first = FALSE;
    }

    Frame_AllocYCC(frame);

    /*
     * okay.  Now, convert everything into YCrCb space. (the specific
     * numbers come from the JPEG source, jccolor.c) Their comment is the following:
     *
     * YCbCr is defined per CCIR 601-1, except that Cb and Cr are
     * normalized to the range 0..MAXJSAMPLE rather than -0.5 .. 0.5.
     * The conversion equations to be implemented are therefore
     *	Y  =  0.29900 * R + 0.58700 * G + 0.11400 * B
     *	Cb = -0.16874 * R - 0.33126 * G + 0.50000 * B  + MAXJSAMPLE/2
     *	Cr =  0.50000 * R - 0.41869 * G - 0.08131 * B  + MAXJSAMPLE/2
     * (These numbers are derived from TIFF 6.0 section 21, dated 3-June-92.)
     *
     */

/* ydivisor should be a FLOAT, shouldn't it?!?! */

    ydivisor = (frame->rgb_maxval + 1) >> 8;	/* for normalizing values
						 * 0-255, divide by 256 */
    cdivisor = (ydivisor << 2);	    /* because we're averaging 4 pixels */

    for (y = 0; y < Fsize_y; y += 2) {
	for (x = 0, src0 = frame->rgb_data[y], src1 = frame->rgb_data[y + 1],
	     dy0 = frame->orig_y[y], dy1 = frame->orig_y[y + 1],
	     dcr = frame->orig_cr[y >> 1], dcb = frame->orig_cb[y >> 1];
	     x < Fsize_x;
	     x += 2, dy0 += 2, dy1 += 2, dcr++,
	     dcb++, src0 += 2, src1 += 2) {
#ifndef BYtheBOOK
	    *dy0 = (mult299[PPM_GETR(*src0)] +
		    mult587[PPM_GETG(*src0)] +
		    mult114[PPM_GETB(*src0)]) / ydivisor;

	    *dy1 = (mult299[PPM_GETR(*src1)] +
		    mult587[PPM_GETG(*src1)] +
		    mult114[PPM_GETB(*src1)]) / ydivisor;

	    dy0[1] = (mult299[PPM_GETR(src0[1])] +
		      mult587[PPM_GETG(src0[1])] +
		      mult114[PPM_GETB(src0[1])]) / ydivisor;

	    dy1[1] = (mult299[PPM_GETR(src1[1])] +
		      mult587[PPM_GETG(src1[1])] +
		      mult114[PPM_GETB(src1[1])]) / ydivisor;

	    *dcb = ((mult16874[PPM_GETR(*src0)] +
		     mult33126[PPM_GETG(*src0)] +
		     mult5[PPM_GETB(*src0)] +
		     mult16874[PPM_GETR(*src1)] +
		     mult33126[PPM_GETG(*src1)] +
		     mult5[PPM_GETB(*src1)] +
		     mult16874[PPM_GETR(src0[1])] +
		     mult33126[PPM_GETG(src0[1])] +
		     mult5[PPM_GETB(src0[1])] +
		     mult16874[PPM_GETR(src1[1])] +
		     mult33126[PPM_GETG(src1[1])] +
		     mult5[PPM_GETB(src1[1])]) / cdivisor) + 128;

	    *dcr = ((mult5[PPM_GETR(*src0)] +
		     mult41869[PPM_GETG(*src0)] +
		     mult08131[PPM_GETB(*src0)] +
		     mult5[PPM_GETR(*src1)] +
		     mult41869[PPM_GETG(*src1)] +
		     mult08131[PPM_GETB(*src1)] +
		     mult5[PPM_GETR(src0[1])] +
		     mult41869[PPM_GETG(src0[1])] +
		     mult08131[PPM_GETB(src0[1])] +
		     mult5[PPM_GETR(src1[1])] +
		     mult41869[PPM_GETG(src1[1])] +
		     mult08131[PPM_GETB(src1[1])]) / cdivisor) + 128;
#else
	    /* A by-the-book version of (from ITU-R BT.470-2 System B, G and SMPTE 170M )*/
	    {
	      double L, CBsum, CRsum;
	      L = .587048*PPM_GETG(*src0) + .298943*PPM_GETR(*src0) + .114009*PPM_GETB(*src0);
	      CRsum = 0.713604*(PPM_GETR(*src0)-L);
	      CBsum = 0.563884*(PPM_GETB(*src0)-L);
	      *dy0 = L;
	      L = .587048*PPM_GETG(*src1) + .298943*PPM_GETR(*src1) + .114009*PPM_GETB(*src1);
	      CRsum += 0.713604*(PPM_GETR(*src1)-L);
	      CBsum += 0.563884*(PPM_GETB(*src1)-L);
	      *dy1 = L;

	      L = .587048*PPM_GETG(src0[1]) + .298943*PPM_GETR(src0[1]) + .114009*PPM_GETB(src0[1]);
	      CRsum += 0.713604*(PPM_GETR(src0[1])-L);
	      CBsum += 0.563884*(PPM_GETB(src0[1])-L);
	      dy0[1] = L;

	      L = .587048*PPM_GETG(src1[1]) + .298943*PPM_GETR(src1[1]) + .114009*PPM_GETB(src1[1]);
	      CRsum += 0.713604*(PPM_GETR(src1[1])-L);
	      CBsum += 0.563884*(PPM_GETB(src1[1])-L);
	      dy1[1] = L;

	      *dcr = CRsum/4+128;
	      *dcb = CBsum/4+128;
	    }
#endif
	    /* if your floating point is faster than your loads, you
	     * might consider this:
	     */
#ifdef FLOAT_RGBYUV
	    *dy0 = (PPM_GETR(*src0) * 0.29900 +
		    PPM_GETG(*src0) * 0.58700 +
		    PPM_GETB(*src0) * 0.11400) / ydivisor;
	    *dy1 = (PPM_GETR(*src1) * 0.29900 +
		    PPM_GETG(*src1) * 0.58700 +
		    PPM_GETB(*src1) * 0.11400) / ydivisor;

	    dy0[1] = (PPM_GETR(src0[1]) * 0.29900 +
		      PPM_GETG(src0[1]) * 0.58700 +
		      PPM_GETB(src0[1]) * 0.11400) / ydivisor;

	    dy1[1] = (PPM_GETR(src1[1]) * 0.29900 +
		      PPM_GETG(src1[1]) * 0.58700 +
		      PPM_GETB(src1[1]) * 0.11400) / ydivisor;

	    *dcb = ((PPM_GETR(*src0) * -0.16874 +
		     PPM_GETG(*src0) * -0.33126 +
		     PPM_GETB(*src0) * 0.50000 +
		     PPM_GETR(*src1) * -0.16874 +
		     PPM_GETG(*src1) * -0. +
		     PPM_GETB(*src1) * 0.50000 +
		     PPM_GETR(src0[1]) * -0.16874 +
		     PPM_GETG(src0[1]) * -0.33126 +
		     PPM_GETB(src0[1]) * 0.50000 +
		     PPM_GETR(src1[1]) * -0.16874 +
		     PPM_GETG(src1[1]) * -0.33126 +
		     PPM_GETB(src1[1]) * 0.50000) / cdivisor) + 128;

	    *dcr = ((PPM_GETR(*src0) * 0.50000 +
		     PPM_GETG(*src0) * -0.41869 +
		     PPM_GETB(*src0) * -0.08131 +
		     PPM_GETR(*src1) * 0.50000 +
		     PPM_GETG(*src1) * -0.41869 +
		     PPM_GETB(*src1) * -0.08131 +
		     PPM_GETR(src0[1]) * 0.50000 +
		     PPM_GETG(src0[1]) * -0.41869 +
		     PPM_GETB(src0[1]) * -0.08131 +
		     PPM_GETR(src1[1]) * 0.50000 +
		     PPM_GETG(src1[1]) * -0.41869 +
		     PPM_GETB(src1[1]) * -0.08131) / cdivisor) + 128;
#endif

	    DBG_PRINT(("%3d,%3d: (%3d,%3d,%3d) --> (%3d,%3d,%3d)\n", x, y, PPM_GETR(*src0), PPM_GETG(*src0), PPM_GETB(*src0), *dy0, *dcb, *dcr));
	}
    }
}



/*===========================================================================*
 *
 * PPMtoYUV
 *
 *	convert PPM data into YUV data
 *	same as PNMtoYUV, except extracts data from ppm_data, and
 *	assumes that ydivisor = 1
 *
 * RETURNS:	nothing
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
void
PPMtoYUV(frame)
    MpegFrame *frame;
{
    register int x, y;
    register u_int8_t *dy0, *dy1;
    register u_int8_t *dcr, *dcb;
    register u_int8_t *src0, *src1;
    register int cdivisor;
    static boolean  first = TRUE;
    static float  mult299[1024], mult587[1024], mult114[1024];
    static float  mult16874[1024], mult33126[1024], mult5[1024];
    static float mult41869[1024], mult08131[1024];

    if ( first) {
        register int index;
	register int maxValue;

	maxValue = frame->rgb_maxval;

        for ( index = 0; index <= maxValue; index++) {
	    mult299[index] = index*0.29900;
	    mult587[index] = index*0.58700;
	    mult114[index] = index*0.11400;
	    mult16874[index] = -0.16874*index;
	    mult33126[index] = -0.33126*index;
	    mult5[index] = index*0.50000;
	    mult41869[index] = -0.41869*index;
	    mult08131[index] = -0.08131*index;
	}
	
	first = FALSE;
    }

    Frame_AllocYCC(frame);

    /* assume ydivisor = 1, so cdivisor = 4 */
    if ( frame->rgb_maxval != 255) {
	io_printf(stderr, "PPM max gray value != 255.  Exiting.\n\tTry PNM type, not PPM\n");
	exit(1);
    }

    cdivisor = 4;

    for (y = 0; y < Fsize_y; y += 2) {
	src0 = frame->ppm_data[y];
	src1 = frame->ppm_data[y + 1];
	dy0 = frame->orig_y[y];
	dy1 = frame->orig_y[y + 1];
	dcr = frame->orig_cr[y >> 1];
	dcb = frame->orig_cb[y >> 1];

	for ( x = 0; x < Fsize_x; x += 2, dy0 += 2, dy1 += 2, dcr++,
				   dcb++, src0 += 6, src1 += 6) {
	    *dy0 = (mult299[*src0] +
		    mult587[src0[1]] +
		    mult114[src0[2]]);

	    *dy1 = (mult299[*src1] +
		    mult587[src1[1]] +
		    mult114[src1[2]]);

	    dy0[1] = (mult299[src0[3]] +
		      mult587[src0[4]] +
		      mult114[src0[5]]);

	    dy1[1] = (mult299[src1[3]] +
		      mult587[src1[4]] +
		      mult114[src1[5]]);

	    *dcb = ((mult16874[*src0] +
		     mult33126[src0[1]] +
		     mult5[src0[2]] +
		     mult16874[*src1] +
		     mult33126[src1[1]] +
		     mult5[src1[2]] +
		     mult16874[src0[3]] +
		     mult33126[src0[4]] +
		     mult5[src0[5]] +
		     mult16874[src1[3]] +
		     mult33126[src1[4]] +
		     mult5[src1[5]]) / cdivisor) + 128;

	    *dcr = ((mult5[*src0] +
		     mult41869[src0[1]] +
		     mult08131[src0[2]] +
		     mult5[*src1] +
		     mult41869[src1[1]] +
		     mult08131[src1[2]] +
		     mult5[src0[3]] +
		     mult41869[src0[4]] +
		     mult08131[src0[5]] +
		     mult5[src1[3]] +
		     mult41869[src1[4]] +
		     mult08131[src1[5]]) / cdivisor) + 128;

	    DBG_PRINT(("%3d,%3d: (%3d,%3d,%3d) --> (%3d,%3d,%3d)\n", x, y, PPM_GETR(*src0), PPM_GETG(*src0), PPM_GETB(*src0), *dy0, *dcb, *dcr));
	}
    }
}

/*===========================================================================*
 * specifics.c								     *
 *									     *
 *	basic procedures to deal with the specifics file                     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	Specifics_Init							     *
 *      Spec_Lookup                                                          *
 *      SpecTypeLookup                                                       *
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


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "mtypes.h"
#include "frames.h"
#include "frame.h"
#include "fsize.h"
#include "dct.h"
#include "specifics.h"
#include <stdio.h>
#include <string.h>
#include "prototypes.h"

/*====================*
 * System Information *
 *====================*/

#define CPP_LOC "/lib/cpp"

/*==================*
 * GLOBAL VARIABLES *
 *==================*/

extern boolean specificsOn;
extern char specificsFile[];
extern char specificsDefines[];
FrameSpecList *fsl;
/*NOTE*/
#endif

/*=====================*
 * Internal procedures *
 *=====================*/

void Parse_Specifics_File _ANSI_ARGS_((FILE *fp));
void Parse_Specifics_File_v1 _ANSI_ARGS_((FILE *fp));
void Parse_Specifics_File_v2 _ANSI_ARGS_((FILE *fp));
FrameSpecList *MakeFslEntry _ANSI_ARGS_((void));
void AddSlc _ANSI_ARGS_((FrameSpecList *c,int snum, int qs));
Block_Specifics *AddBs _ANSI_ARGS_((FrameSpecList *c,int bnum, 
				    boolean rel, int qs));
FrameSpecList *MakeFslEntry _ANSI_ARGS_((void));
#define my_upper(c) (((c>='a') && (c<='z')) ? (c-'a'+'A') : c)
#define CvtType(x) ReallyCvt(my_upper(x))
#define ReallyCvt(x) (x=='I' ? 1 : (x=='P')?2: ((x=='B')?3:-1))
#define SkipToSpace(lp) while ((*lp != ' ') && (*lp != '\n') && (*lp != '\0')) lp++
#define EndString(lp)  ((*lp == '\n') || (*lp == '\0'))

/*=============================================================
 * SPEC FILE FORMAT (version 1):

Specs files are processed with the c preprecoessor, so use C style comments
and #defines if you wish.

frames, blocks, and slices are numbered from 0.
(sorry)

In order by frame number, slice number, block number
(if you skip slices it's fine).
Can have specifics for any frame, block, or slice.
Format:
version N
  Specify the version of the specifics file format (this is 1)
frame N T M
  Sets frame number N to type T and Qscale M
  (type T is I,B,P,other, other means unspec.  I recomend - )
slice M Q
  Sets slice M (in frame N as defined by a previous frame command)
  to qscale Q
block M Q
  Sets block M to qscale Q, in frame N previously specified.

Unspecified frame types are set via the last I frame set, which is forced
to act as the first I of the GOP.
FORCE_ENCODE_LAST_FRAME overrides specifics on the final frame type.
Note that Qscale changes in skipped blocks will be lost!

Version 2:
frames and slices are the same as above, but Q in blocks can be relative, i.e.
+N or -N.  Clipping to 1..31 is done but sequences like
block 1 2
block 2 -3
block 3 +3

has undefined results (as present block 3 would be Qscale 2).

In addition motion vectors can be specified:
block M Q skip
  Says to skip the block  (not really an MV, but....)
block M Q bi fx fy bx by
  Sets block M to quality Q.  It will be a bidirectional block.
  fx/fy is the forward (like a P frame) vector, bx/y is the back
block M Q forw fx fy
block M Q back bx by
  Single directional.

All vectors are specified in HALF PIXEL fixed point units, i.e.
3.5 pixels is 7
To specify a vector but not touch the q factor, set Q to 0

*=============================================================*/


/*=============*
 * Local State *
 *=============*/

static char version = -1;

/*================================================================
 *
 *   Specifics_Init
 *
 *   Cpp's and reads in the specifics file.  Creates fsl data structure.
 *
 *   Returns: nothing
 * 
 *   Modifies: fsl, file specificsFile".cpp"
 *
 *================================================================
 */
void Specifics_Init()
{
  char command[1100];
  FILE *specificsFP;
  
  snprintf(command, 1100, "/bin/rm -f %s.cpp", specificsFile);
  system(command);
  snprintf(command, 1100, "%s -P %s %s %s.cpp",
	  CPP_LOC, specificsDefines, specificsFile, specificsFile);
  system(command);
  SC_strcat(specificsFile, 256, ".cpp");
  if ((specificsFP = _PG_fopen(specificsFile, "r")) == NULL) {
    io_printf(stderr, "Error with specifics file, cannot open %s\n", specificsFile);
    exit(1);
  }
  printf("Specifics file: %s\n", specificsFile);
  Parse_Specifics_File(specificsFP);
  snprintf(command, 1100, "/bin/rm -f %s.cpp", specificsFile);
  system(command);

}




/*================================================================
 *
 *   Parse_Specifics_File
 *
 *   Read through the file passed in creating the fsl data structure
 *   There is a primary routine, and helpers for the specific versions.
 *
 *   Returns: Nothing
 *
 *   Modifies: fsl
 *
 *================================================================
 */
void Parse_Specifics_File(fp)
FILE *fp;
{
  char line[1024], *lp;
  int vers;

  while ((io_gets(line, 1023, fp)) != NULL) {
    lp = &line[0];
    while ((*lp == ' ') || (*lp == '\t')) lp++;
    if (( *lp == '#' ) || (*lp=='\n')) {
      continue;
    }

    switch (my_upper(*lp)) {
    case 'F': case 'S': case 'B':
      io_printf(stderr, "Must specify version at beginning of specifics file\n");
      exit(0);
      break;
    case 'V':
      lp += 7;
      if (1 != sscanf(lp, "%d", &vers)) {
	io_printf(stderr," Improper version line in specs file: %s\n", line);
      } else {
	switch (vers) {
	case 1:
	  version = vers;
	  Parse_Specifics_File_v1(fp);
	  break;
	case 2:
	  version = vers;
	  Parse_Specifics_File_v2(fp);
	  break;
	default:
	  io_printf(stderr, "Improper version line in specs file: %s\n", line);
	  io_printf(stderr, "\tSpecifics file will be IGNORED.\n");
	  specificsOn = FALSE;
	  return;
	  /*break;*/
	}}
      break;
    default:
      io_printf(stderr, "Specifics file: What? *%s*\n", line);
      break;
    }}
  
}

/* Version 1 */
void Parse_Specifics_File_v1(FILE *fp)
   {int fnum,snum, bnum, qs, newqs;
    char line[1024], *lp;
    char typ; 
    FrameSpecList *current, *new;

    fsl = MakeFslEntry();
    if (fsl == NULL)
       return;

    current = fsl;
    qs      = 0;

    while ((io_gets(line,1023, fp)) != NULL)
       {lp = &line[0];
	while ((*lp == ' ') || (*lp == '\t')) lp++;

	if ((*lp == '#') || (*lp == '\n'))
	   continue;

	switch (my_upper(*lp))
	   {case 'F':
	         lp += 6;
		 sscanf(lp, "%d %c %d", &fnum, &typ, &qs);
		 if (current->framenum != -1)
		    {new = MakeFslEntry();
		     if (new != NULL)
		        {current->next = new;
			 current = new;};};

		 if (current != NULL)
		    {current->framenum = fnum;
		     current->frametype = CvtType(typ);
		     if (qs <= 0) qs = -1;
		     current->qscale = qs;};
		 break;
	    case 'S':
		 lp += 6;
		 sscanf(lp, "%d %d", &snum, &newqs);
		 if (qs == newqs) break;
		 qs = newqs;
		 AddSlc(current, snum, qs);
		 break;
	    case 'B':
		 lp += 6;
		 sscanf(lp, "%d %d", &bnum, &newqs);
		 if (qs == newqs) break;
		 qs = newqs;
		 AddBs(current, bnum, FALSE, qs);
		 break;
	    case 'V':
		 io_printf(stderr, "Cannot specify version twice!  Taking first (%d)\n", version);
		 break;
	    default:
		 io_printf(stderr," What? *%s*\n", line);
		 break;};};

    return;}

/* Version 2 */
void Parse_Specifics_File_v2(FILE *fp)
   {int fnum, snum, bnum, qs, newqs;
    int num_scanned, fx=0, fy=0, sx=0, sy=0;
    char line[1024], *lp;
    FrameSpecList *current, *new;
    char typ;
    char kind[100];
    Block_Specifics *new_blk;
    boolean relative;

    fsl = MakeFslEntry();
    if (fsl == NULL)
       return;

    current = fsl;

    new = NULL;
    qs  = 0;

    while ((io_gets(line,1023,fp))!=NULL)
       {lp = &line[0];
	while ((*lp == ' ') || (*lp == '\t')) lp++;

	if ((*lp == '#') || (*lp == '\n'))
	   continue;

	switch (my_upper(*lp))
	   {case 'F':
	         lp += 6;
		 sscanf(lp, "%d %c %d", &fnum, &typ, &qs);
		 new = MakeFslEntry();
		 if (new != NULL)
		    {if (current->framenum != -1)
		        {current->next = new;
			 current = new;}
		     else
		        free(new);};
		 current->framenum = fnum;
		 current->frametype = CvtType(typ);
		 if (qs <= 0)
		    qs = -1;
		 current->qscale = qs;
		 break;

	    case 'S':
		 lp += 6;
		 sscanf(lp, "%d %d", &snum, &newqs);
		 if (qs == newqs)
		    break;
		 qs = newqs;
		 AddSlc(current, snum, qs);
		 break;

	    case 'B':
		 lp += 6;
		 num_scanned = 0;
		 bnum = atoi(lp);
		 SkipToSpace(lp);
		 while ((*lp != '-') && (*lp != '+') &&
			((*lp < '0') || (*lp > '9'))) lp++;
		 relative = ((*lp == '-') || (*lp == '+'));
		 newqs = atoi(lp);
		 SkipToSpace(lp);
		 if (EndString(lp))
		   num_scanned = 2;
		 else
		   num_scanned = 2 +sscanf(lp, "%100s %d %d %d %d",
					   kind, &fx, &fy, &sx, &sy); 

		 qs = newqs;
		 new_blk = AddBs(current, bnum, relative, qs);
		 if ((new_blk != NULL) && (num_scanned > 2))
		    {BlockMV *tmp;

		     tmp = CMAKE(BlockMV);
		     if (tmp != NULL)
		        {switch (num_scanned)
			    {case 7:
			          tmp->typ = TYP_BOTH;
				  tmp->fx = fx;
				  tmp->fy = fy;
				  tmp->bx = sx;
				  tmp->by = sy;
				  new_blk->mv = tmp;
				  break;
			     case 3:
				  tmp->typ = TYP_SKIP;
				  new_blk->mv = tmp;
				  break;
			     case 5:
				  if (my_upper(kind[0]) == 'B')
				     {tmp->typ = TYP_BACK;
				      tmp->bx = fx;
				      tmp->by = fy;}
				  else
				     {tmp->typ = TYP_FORW;
				      tmp->fx = fx;
				      tmp->fy = fy;};

				  new_blk->mv = tmp;
				  break;
			     default:
				  free(tmp);
				  io_printf(stderr,
					    "Bug in specifics file!  Skipping short/long entry: %s\n",line);
				  break;};};}
		 else if (new_blk != NULL)
		    new_blk->mv = (BlockMV *) NULL;
		 
		 break;

	    case 'V':
		 io_printf(stderr,
			   "Cannot specify version twice!  Taking first (%d).\n",
			   version);
		 break;
	    default:
		 printf("What? *%s*\n",line);
		 break;};};

    return;}

/*=================================================================
 *
 *     MakeFslEntry
 *
 *     Makes a single entry in for the fsl linked list (makes a frame)
 *
 *     Returns: the new entry
 *
 *     Modifies: nothing
 *
 *=================================================================
 */

FrameSpecList *MakeFslEntry(void)
   {FrameSpecList *fslp;

    fslp = CMAKE(FrameSpecList);
    if (fslp != NULL)
       {fslp->framenum = -1;
	fslp->slc = (Slice_Specifics *) NULL;
	fslp->bs = (Block_Specifics *) NULL;};

    return(fslp);}

/*================================================================
 *
 *   AddSlc
 *
 *   Adds a slice to framespeclist c with values snum and qs
 *
 *   Returns: nothing
 *
 *   Modifies: fsl's structure
 *
 *================================================================
 */

void AddSlc(FrameSpecList *c, int snum, int qs)
   {Slice_Specifics *new;
    static Slice_Specifics *last;

    new = CMAKE(Slice_Specifics);
    if (new != NULL)
       {new->num = snum;
	new->qscale = qs;
	new->next = (Slice_Specifics *)NULL;
	if (c->slc == (Slice_Specifics *)NULL)
	   {last = new;
	    c->slc = new;}
	else
	   {last->next = new;
	    last = new;};};

    return;}

/*================================================================
 *
 *   AddBs
 *
 *   Adds a sliceblock to framespeclist c with values bnum and qs
 *
 *   Returns: pointer to the new block spec
 *
 *   Modifies: fsl's structure
 *
 *================================================================
 */

Block_Specifics *AddBs(FrameSpecList *c, boolean rel, int bnum, int qs)
   {Block_Specifics *new;
    static Block_Specifics *last;

    new = CMAKE(Block_Specifics);
    if (new != NULL)
       {new->num = bnum;
	if (qs == 0)
	   rel = TRUE;
	new->relative = rel;
	new->qscale   = qs;
	new->next     = NULL;
	new->mv       = (BlockMV *) NULL;
	if (c->bs == NULL)
	   {last  = new;
	    c->bs = new;}
	else
	   {last->next = new;
	    last       = new;};};

    return(new);}

/*================================================================
 *
 *  SpecLookup
 *
 *  Find out if there is any changes to be made for the qscale
 *  at entry fn.num (which is of type typ).  Sets info to point to
 *  motion vector info (if any), else NULL.
 *
 *  Returns: new qscale or -1
 *
 *  Modifies: *info (well, internal cache can change)
 *
 *================================================================
 */

int SpecLookup(fn,typ,num,info,start_qs)
int fn,typ,num;
BlockMV **info;
int start_qs;
{
  static FrameSpecList *last = (FrameSpecList *) NULL;
  Slice_Specifics *sptr=(Slice_Specifics *) NULL;
  Block_Specifics *bptr=(Block_Specifics *) NULL;
  FrameSpecList *tmp;
  boolean found_it;
  static int leftovers = 0;  /* Used in case of forced movement into 1..31 range */
  
  *info = (BlockMV * )NULL;
  if (last == (FrameSpecList *) NULL){
    /* No cache, try to find number fn */
    tmp = fsl;
    found_it = FALSE;
    while (tmp != (FrameSpecList *) NULL) {
      if (tmp->framenum == fn) {
	found_it = TRUE;
	break;
      } else tmp = tmp->next;
    }
    if (!found_it) return(-1);
    last=tmp;
  } else {
    if (last->framenum != fn) { /* cache miss! */
      /* first check if it is next */
      if ((last->next != (FrameSpecList *) NULL) && 
	  (last->next->framenum == fn)) {
	last = last->next;
      } else {
	/* if not next, check from the start.
	   (this allows people to put frames out of order,even
	   though the spec doesnt allow it.) */
	tmp = fsl;
	found_it = FALSE;
	while (tmp != (FrameSpecList *) NULL) {
	  if (tmp->framenum==fn) {found_it = TRUE; break;}
	  tmp = tmp->next;
	}
	if (!found_it) return(-1);
	last = tmp;
      }
    }
  }
  /* neither of these should ever be true, unless there is a bug above */
  if (last == (FrameSpecList *) NULL) {
    io_printf(stderr, "PROGRAMMER ERROR: last is null!\n");
    return(-1);
  }
  if (last->framenum!=fn) {
    io_printf(stderr, "PROGRAMMER ERROR: last has wrong number!\n");
    return(-1); /* no data on it */
  }
  
  switch(typ) {
  case 0: /* Frame: num is ignored */
    leftovers = 0;
#ifdef BLEAH
    printf("QSchange frame %d to %d\n", fn, last->qscale);
#endif 
    return(last->qscale);
    /*break;*/

  case 1: /* Slice */
    leftovers = 0;
    /* So, any data on slices? */
    if (last->slc == (Slice_Specifics *) NULL) return(-1);
    for (sptr = last->slc; sptr != (Slice_Specifics *) NULL; sptr = sptr->next) {
      if (sptr->num == num) {
#ifdef BLEAH
	printf("QSchange Slice %d.%d to %d\n", fn, num, sptr->qscale);
#endif
	if (sptr->qscale == 0) return(-1);
	return(sptr->qscale);
      }
    }
    break;

  case 2:  /* block */
    /* So, any data on blocks? */
    if (last->bs == (Block_Specifics *) NULL) {
      return(-1);
    }
    for (bptr=last->bs; bptr != (Block_Specifics *) NULL; bptr=bptr->next) {
      if (bptr->num == num) {
	int new_one;
#ifdef BLEAH
	printf("QSchange Block %d.%d to %d\n", fn, num, bptr->qscale);
#endif
	*info = bptr->mv;
	if (bptr->relative) {
	  if (bptr->qscale == 0) {
	    /* Do nothing! */
	    new_one = start_qs;
	  } else {
	    new_one = start_qs + bptr->qscale + leftovers;
	    if (new_one < 1) {
	      leftovers = new_one - 1;
	      new_one = 1;
	    } else if (new_one > 31) {
	      leftovers = new_one - 31;
	      new_one = 31;
	    } else leftovers = 0;
	  }}
	else {
	  new_one = bptr->qscale;
	  leftovers = 0;
	}
	return(new_one);
      }
    }
    break;
  default:
    io_printf(stderr, "PROGRAMMER ERROR:  reached unreachable code in SpecLookup\n");
    break;
  }
  /* no luck */
  return(-1);
}
     
    
/*================================================================
 *
 *  SpecTypeLookup
 *
 *  Find out if there is any changes to be made for the type of frame
 *  at frame fn.
 *
 *  Returns: new type or -1 (unspecified)
 *
 *================================================================
 */
int SpecTypeLookup(fn)
int fn;
{
  FrameSpecList *tmp;

  /* try to find number fn */
  tmp = fsl;
  do {
    if (tmp->framenum == fn) break;
    else tmp = tmp->next;
  } while (tmp != (FrameSpecList *) NULL);
  if (tmp == (FrameSpecList *) NULL) {
#ifdef BLEAH
    printf("Frame %d type not specified\n", fn);
#endif
    return(-1);
  }
#ifdef BLEAH
  printf("Frame %d type set to %d\n", fn, tmp->frametype);
#endif
  return(tmp->frametype);
}
/*===========================================================================*
 * subsample.c								     *
 *									     *
 *	Procedures concerned with subsampling				     *
 *									     *
 * EXPORTED PROCEDURES:							     *
 *	LumMotionErrorA							     *
 *	LumMotionErrorB							     *
 *	LumMotionErrorC							     *
 *	LumMotionErrorD							     *
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
 * Revision 1.6  1995/01/19  23:09:28  eyhung
 * Changed copyrights
 *
 * Revision 1.5  1994/11/12  02:12:01  keving
 * nothing
 *
 * Revision 1.4  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.4  1993/12/22  19:19:01  keving
 * nothing
 *
 * Revision 1.3  1993/07/22  22:23:43  keving
 * nothing
 *
 * Revision 1.2  1993/06/30  20:06:09  keving
 * nothing
 *
 * Revision 1.1  1993/06/22  21:56:05  keving
 * nothing
 *
 */


/*==============*
 * HEADER FILES *
 *==============*/

#if 0
/*NOTE*/
#include "all.h"
#include "mtypes.h"
#include "frames.h"
#include "bitio.h"
#include "prototypes.h"
/*NOTE*/
#endif

#undef ABS
#define ABS(x)	((x < 0) ? (-x) : x)


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/


/*===========================================================================*
 *
 * LumMotionErrorA
 *
 *	compute the motion error for the A subsampling pattern
 *
 * RETURNS:	the error, or some number greater if it is worse
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int32
LumMotionErrorA(currentBlock, prevFrame, by, bx, my, mx, bestSoFar)
    LumBlock currentBlock;
    MpegFrame *prevFrame;
    int by;
    int bx;
    int my;
    int mx;
    int32 bestSoFar;
{
    register int32    diff = 0;	    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register u_int8_t *macross;
    register u_int8_t **prev;
    register int    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf) {
	if ( mx < 0) {
	    fx--;
	}

	if ( yHalf) {
	    if ( my < 0) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf) {
	if ( my < 0) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    macross = &(prev[fy][fx]);

    localDiff = macross[0]-currentBlock[0][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[0][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[0][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[0][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[0][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[0][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[0][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[0][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+2][fx]);

    localDiff = macross[0]-currentBlock[2][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[2][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[2][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[2][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[2][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[2][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[2][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[2][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+4][fx]);

    localDiff = macross[0]-currentBlock[4][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[4][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[4][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[4][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[4][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[4][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[4][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[4][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+6][fx]);

    localDiff = macross[0]-currentBlock[6][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[6][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[6][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[6][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[6][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[6][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[6][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[6][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+8][fx]);

    localDiff = macross[0]-currentBlock[8][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[8][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[8][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[8][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[8][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[8][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[8][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[8][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+10][fx]);

    localDiff = macross[0]-currentBlock[10][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[10][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[10][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[10][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[10][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[10][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[10][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[10][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+12][fx]);

    localDiff = macross[0]-currentBlock[12][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[12][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[12][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[12][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[12][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[12][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[12][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[12][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+14][fx]);

    localDiff = macross[0]-currentBlock[14][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[14][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[14][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[14][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[14][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[14][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[14][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[14][14];     diff += ABS(localDiff);

    return(diff);
}


/*===========================================================================*
 *
 * LumMotionErrorB
 *
 *	compute the motion error for the B subsampling pattern
 *
 * RETURNS:	the error, or some number greater if it is worse
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int32
LumMotionErrorB(currentBlock, prevFrame, by, bx, my, mx, bestSoFar)
    LumBlock currentBlock;
    MpegFrame *prevFrame;
    int by;
    int bx;
    int my;
    int mx;
    int32 bestSoFar;
{
    register int32    diff = 0;	    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register u_int8_t *macross;
    register u_int8_t **prev;
    register int    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf) {
	if ( mx < 0) {
	    fx--;
	}

	if ( yHalf) {
	    if ( my < 0) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf) {
	if ( my < 0) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    macross = &(prev[fy+0][fx]);

    localDiff = macross[1]-currentBlock[0][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[0][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[0][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[0][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[0][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[0][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[0][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[0][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+2][fx]);

    localDiff = macross[1]-currentBlock[2][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[2][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[2][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[2][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[2][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[2][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[2][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[2][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+4][fx]);

    localDiff = macross[1]-currentBlock[4][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[4][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[4][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[4][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[4][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[4][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[4][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[4][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+6][fx]);

    localDiff = macross[1]-currentBlock[6][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[6][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[6][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[6][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[6][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[6][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[6][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[6][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+8][fx]);

    localDiff = macross[1]-currentBlock[8][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[8][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[8][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[8][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[8][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[8][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[8][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[8][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+10][fx]);

    localDiff = macross[1]-currentBlock[10][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[10][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[10][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[10][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[10][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[10][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[10][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[10][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+12][fx]);

    localDiff = macross[1]-currentBlock[12][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[12][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[12][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[12][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[12][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[12][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[12][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[12][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+14][fx]);

    localDiff = macross[1]-currentBlock[14][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[14][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[14][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[14][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[14][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[14][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[14][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[14][15];     diff += ABS(localDiff);

    return(diff);
}


/*===========================================================================*
 *
 * LumMotionErrorC
 *
 *	compute the motion error for the C subsampling pattern
 *
 * RETURNS:	the error, or some number greater if it is worse
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int32
LumMotionErrorC(currentBlock, prevFrame, by, bx, my, mx, bestSoFar)
    LumBlock currentBlock;
    MpegFrame *prevFrame;
    int by;
    int bx;
    int my;
    int mx;
    int32 bestSoFar;
{
    register int32    diff = 0;	    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register u_int8_t *macross;
    register u_int8_t **prev;
    register int    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf) {
	if ( mx < 0) {
	    fx--;
	}

	if ( yHalf) {
	    if ( my < 0) {
		fy--;
	    }
	    
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf) {
	if ( my < 0) {
	    fy--;
	}

	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    macross = &(prev[fy+1][fx]);

    localDiff = macross[0]-currentBlock[1][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[1][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[1][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[1][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[1][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[1][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[1][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[1][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+3][fx]);

    localDiff = macross[0]-currentBlock[3][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[3][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[3][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[3][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[3][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[3][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[3][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[3][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+5][fx]);

    localDiff = macross[0]-currentBlock[5][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[5][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[5][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[5][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[5][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[5][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[5][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[5][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+7][fx]);

    localDiff = macross[0]-currentBlock[7][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[7][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[7][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[7][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[7][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[7][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[7][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[7][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+9][fx]);

    localDiff = macross[0]-currentBlock[9][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[9][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[9][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[9][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[9][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[9][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[9][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[9][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+11][fx]);

    localDiff = macross[0]-currentBlock[11][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[11][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[11][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[11][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[11][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[11][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[11][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[11][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+13][fx]);

    localDiff = macross[0]-currentBlock[13][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[13][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[13][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[13][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[13][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[13][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[13][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[13][14];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+15][fx]);

    localDiff = macross[0]-currentBlock[15][0];     diff += ABS(localDiff);
    localDiff = macross[2]-currentBlock[15][2];     diff += ABS(localDiff);
    localDiff = macross[4]-currentBlock[15][4];     diff += ABS(localDiff);
    localDiff = macross[6]-currentBlock[15][6];     diff += ABS(localDiff);
    localDiff = macross[8]-currentBlock[15][8];     diff += ABS(localDiff);
    localDiff = macross[10]-currentBlock[15][10];     diff += ABS(localDiff);
    localDiff = macross[12]-currentBlock[15][12];     diff += ABS(localDiff);
    localDiff = macross[14]-currentBlock[15][14];     diff += ABS(localDiff);

    return(diff);
}


/*===========================================================================*
 *
 * LumMotionErrorD
 *
 *	compute the motion error for the D subsampling pattern
 *
 * RETURNS:	the error, or some number greater if it is worse
 *
 * SIDE EFFECTS:    none
 *
 *===========================================================================*/
int32
LumMotionErrorD(currentBlock, prevFrame, by, bx, my, mx, bestSoFar)
    LumBlock currentBlock;
    MpegFrame *prevFrame;
    int by;
    int bx;
    int my;
    int mx;
    int32 bestSoFar;
{
    register int32    diff = 0;	    /* max value of diff is 255*256 = 65280 */
    register int32 localDiff;
    register u_int8_t *macross;
    register u_int8_t **prev;
    register int    fy, fx;
    boolean xHalf, yHalf;

    xHalf = (ABS(mx) % 2 == 1);
    yHalf = (ABS(my) % 2 == 1);

    MOTION_TO_FRAME_COORD(by, bx, my/2, mx/2, fy, fx);

    if ( xHalf) {
	if ( mx < 0) {
	    fx--;
	}

	if ( yHalf) {
	    if ( my < 0) {	   
		fy--;
	    }
	    prev = prevFrame->halfBoth;
	} else {
	    prev = prevFrame->halfX;
	}
    } else if ( yHalf) {
	if ( my < 0) {
	    fy--;
	}
	prev = prevFrame->halfY;
    } else {
	prev = prevFrame->ref_y;
    }

    macross = &(prev[fy+1][fx]);

    localDiff = macross[1]-currentBlock[1][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[1][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[1][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[1][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[1][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[1][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[1][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[1][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+3][fx]);

    localDiff = macross[1]-currentBlock[3][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[3][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[3][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[3][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[3][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[3][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[3][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[3][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+5][fx]);

    localDiff = macross[1]-currentBlock[5][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[5][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[5][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[5][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[5][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[5][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[5][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[5][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+7][fx]);

    localDiff = macross[1]-currentBlock[7][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[7][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[7][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[7][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[7][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[7][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[7][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[7][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+9][fx]);

    localDiff = macross[1]-currentBlock[9][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[9][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[9][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[9][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[9][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[9][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[9][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[9][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+11][fx]);

    localDiff = macross[1]-currentBlock[11][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[11][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[11][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[11][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[11][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[11][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[11][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[11][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+13][fx]);

    localDiff = macross[1]-currentBlock[13][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[13][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[13][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[13][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[13][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[13][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[13][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[13][15];     diff += ABS(localDiff);

    if ( diff > bestSoFar) {
	return(diff);
    }

    macross = &(prev[fy+15][fx]);

    localDiff = macross[1]-currentBlock[15][1];     diff += ABS(localDiff);
    localDiff = macross[3]-currentBlock[15][3];     diff += ABS(localDiff);
    localDiff = macross[5]-currentBlock[15][5];     diff += ABS(localDiff);
    localDiff = macross[7]-currentBlock[15][7];     diff += ABS(localDiff);
    localDiff = macross[9]-currentBlock[15][9];     diff += ABS(localDiff);
    localDiff = macross[11]-currentBlock[15][11];     diff += ABS(localDiff);
    localDiff = macross[13]-currentBlock[15][13];     diff += ABS(localDiff);
    localDiff = macross[15]-currentBlock[15][15];     diff += ABS(localDiff);

    return(diff);
}

#endif
