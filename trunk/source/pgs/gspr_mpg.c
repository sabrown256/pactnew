/*
 * GSPR_MPG.C - PGS MPEG device primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "pgs_int.h"
#include "scope_mpeg.h"

typedef struct s_MpegDevInfo MpegDevInfo;
struct s_MpegDevInfo
   {PG_device *dev;
    boolean firstFrameDone;
    int FrameId;
    int Fsize_x;
    int Fsize_y;
    int framesOutput;
    BitBucket *bbPtr;	        /* bbPtr->filePtr is FILE* */
    int currentGOP;
    MpegFrame *futureRefFrame;
    MpegFrame *frameMemory[3];  /* always 3 for our case */
    struct s_MpegDevInfo *next;};

MpegDevInfo *listMpegDevices = NULL;
static int numMpegDevices = 0;

extern boolean remoteIO; /* from parallel.h */
extern int Fsize_x, Fsize_y; /* from fsize.h */

/*===========*
 * CONSTANTS *
 *===========*/

#define	FPS_30	0x5   /* from MPEG standard sect. 2.4.3.2 */
#define ASPECT_1    0x1	/* aspect ratio, from MPEG standard sect. 2.4.3.2 */


/*====================*
 * STATIC VARIABLES A *
 *====================*/

static int frameStart = -1;
static int frameEnd   = INT_MAX;
static int firstFrame = 0;
static int lastFrame  = INT_MAX;

static MpegFrame *futureRefFrame = NULL;
static MpegFrame *pastRefFrame   = NULL;

static boolean firstFrameDone = FALSE;
static int FrameId = -1;


/*==================*
 * STATIC VARIABLES *
 *==================*/

/*static int32 diffTime; - not used */
static int   framesOutput;
static int   realStart;
static int   realEnd;
static int   currentGOP;
static int   timeMask;
static int   numI, numP, numB;


/*==================*
 * GLOBAL VARIABLES *	
 *==================*/

/* important -- don't initialize anything here */
/* must be re-initted anyway in GenMPEGStream */

extern time_t  IOtime;
extern boolean resizeFrame;
extern int     outputWidth, outputHeight;
extern char    bitRateFileName[256];
extern time_t  timeStart, timeEnd;
extern FILE    *statFile;

/*static int framesRead; - not used */

extern char userDataFileName[]; /*NOTE: deleted "unsigned" NOTE*/
extern int  mult_seq_headers;

extern int32 bit_rate, buf_size;


/*-----   Default_Init   -----*/
extern char currentGOPPath[MAXPATHLEN];
extern char currentFramePath[MAXPATHLEN];

int baseFormat; /* probably not used at all */

int whichGOP = -1; /* fixed value */
int quietTime = 0; /* fixed value */
boolean realQuiet = TRUE; /* fixed value */
boolean frameSummary = TRUE; /* TRUE = frame summaries should be printed */
boolean showBitRatePerFrame = FALSE;
boolean pureDCT = FALSE;
boolean computeMVHist = FALSE;

boolean childProcess = FALSE;
boolean ioServer = FALSE;
/*-----   Default_Init   -----*/


/*===============================*
 * INTERNAL PROCEDURE prototypes *
 *===============================*/

static void
 ShowRemainingTime(void),
 ProcessRefFrame(MpegDevInfo *MpegDev, int lastFrame,
		 char *outputFileName),
 CopyImage2Frame(PG_RAST_device *mdv, MpegFrame *mf),
 ConvertRGB2YIV(PG_RAST_device *mdv);


/*=====================*
 * EXPORTED PROCEDURES *
 *=====================*/

extern void
 SetRemoteShell(char *shell),
 SetFCode(void),
 ResetIFrameStats(void),
 Fsize_Reset(void),
 SetBlocksPerSlice(void),
 Fsize_Note(int id, int width, int height),
 SendRemoteFrame(int frameNumber, BitBucket *bb),
 IncrementTCTime(void),
 SetGOPSize(int size),
 SetReferenceFrameType(char *type);

extern boolean
 NonLocalRefFrame(int id);


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_INIT - set defaults for some global variables 
 *              - that were used to set in ReadParamFile
 *              - in mpeg_encode.
 *              - this function is called by init_MPEGjob,
 *              - when MPEG-device is to be created.
 */

boolean Default_Init(void)
   {

    /* should set defaults */
    numInputFiles = 0;
    numMachines = 0;
    snprintf(currentPath, MAXPATHLEN, ".");
    snprintf(currentGOPPath, MAXPATHLEN, ".");
    snprintf(currentFramePath, MAXPATHLEN, ".");
#ifdef HPUX
    SetRemoteShell("remsh");
#else
    SetRemoteShell("rsh");
#endif


    /* ASPECT_RATIO */
    /* aspectRatio = ratio;           (default: 1) */

    /* BQSCALE, IQSCALE & PQSCALE */
    /* qscaleI = value;               (default: 0)
     */
    qscaleI = 1;   /* our default value */

    /* BASE_FILE_FORMAT: JPEG_SX_FILE_I_TYPE for now */
    /* baseFormat = file_type;        (default: PPM_FILE_TYPE = 0) */
    /* baseFormat = JPEG_SX_FILE_I_TYPE; - probably not used at all */

    /* BIT_RATE */
    /* RateControlMode = FIXED_RATE;  (default: VARIABLE_RATE)
     * bit_rate = rate;               (default: -1)
     */

    /* BUFFER_SIZE: DEFAULT_BUFFER_SIZE = 327680 */
    /* buffer_size = size;            (default: DEFAULT_BUFFER_SIZE) */

    /* CDL_FILE & SPECIFICS_FILE */
    /* specificsFile = filename;      (default: "")
     * specificsOn = TRUE;            (default: FALSE)
     */

    /* CDL_DEFINES & SPECIFICS_DEFINES */
    /* specificsDefines = string;     (default: "") */

    /* FRAME_INPUT_DIR */
    /* if stdin or STDIN is used after the option,
     *    stdinUsed = TRUE;           (default: FALSE)
     *    numInputFiles = MAXINT;     (default: 0)
     *    numInputFileEntries = 1;    (default: 0)
     * else
     *    currentFramePath = dirname; (default: ".")
     */

    /* FORCE_I_ALIGN, FORCE_ENCODE_LAST_FRAME */
    /* forceIalign = TRUE;            (default: FALSE)
     * forceEncodeLast = TRUE;        (default: FALSE)
     */

    /* FRAME_RATE */
    /* frameRate = 5;                 (default)
     * frameRateRounded = 30;         (default)
     * frameRateInteger = TRUE;       (default)
     */

    /* GOP_SIZE */
    /* gopSize = size;                (default: 100) */

    /* GOP_INPUT_DIR */
    /* if stdin or STDIN is used after the option,
     *    stdinUsed = TRUE;           (default: FALSE)
     *    numInputFiles = MAXINT;     (default: 0)
     *    numInputFileEntries = 1;    (default: 0)
     * else
     *    currentGOPPath = dirname;   (default: ".")
     */

    /* GAMMA */
    /* GammaCorrection = TRUE;        (default: FALSE)
     * GammaValue = value;            (default: 0.0)
     */

    /* INPUT_DIR */
    /* if stdin or STDIN is used after the option,
     *    stdinUsed = TRUE;           (default: FALSE)
     *    numInputFiles = MAXINT;     (default: 0)
     *    numInputFileEntries = 1;    (default: 0)
     * else
     *    currentPath = dirname;      (default: ".")
     */

    /* INPUT_CONVERT */
    /* inputConversion = string;      (default: "") */
    strncpy(inputConversion, "*", 1024);     /* our default */

    /* IO_SERVER_CONVERT */
    /* ioConversion = string;         (default: "") */

    /* IQTABLE */
    /* qtable[] = {seq of int};       (default: seq of various int)
     * customQtable = qtable;         (default: NULL)
     */

    /* NIQTABLE */
    /* niqtable[] = {seq of int};     (default: seq of 16)
     * customNIQtable = niqtable;     (default: NULL)
     */

    /* OUTPUT */
    /* outputFileName = filename;     (default: "") */
    strncpy(outputFileName, "plots.mpg", 256); /* our default */

    /* PATTERN */
    /* framePattern = string;         (default: NULL)
     * framePatternLen = len(string); (default: 0)
     */
    framePattern = (char *)malloc(sizeof(char)*2);
    if (framePattern != NULL)
       strncpy(framePattern, "i", 2);     /* our default */
    framePatternLen = 1;              /* our default */

    /* PIXEL */
    /* pixelFullSearch = FALSE;       (default) */

    /* PARALLEL_TEST_FRAMES, PARALLEL_TIME_CHUNKS, PARALLEL_CHUNK_TAPER */
    /* all the above options set parallelPerfect to FALSE...
     * parallelTestFrames = val;      (default: 10)
     * parallelTimeChunks = val;      (default: 60)
     * parallelTimeChunks = -1;       (default: 60)
     * parallelPerfect = FALSE;       (default)
     */

    /* PARALLEL_PERFECT */
    /* parallelPerfect = TRUE;        (default: FALSE) */

    /* PARALLEL */
    /* ignore this for now - reads param file */

    /* RANGE */
    /* searchRangeB = computed value; (default: 0)
     * searchRangeP = computed value; (default: 0)
     */
    searchRangeB = 20;                /* our default - may not need it */
    searchRangeP = 20;                /* our default - may not need it */

    /* REFERENCE_FRAME: ORIGINAL_FRAME (0) or DECODED_FRAME (1) */
    /* referenceFrame = whatframe;    (default: ORIGINAL_FRAME) */

    /* RSH */
    /* nonparallel case: do nothing
     * parallel case: rsh = string    (default: "")
     */

    /* RESIZE - never seen RESIZE in all param files in examples dir */
    /* outputWidth = val1;            (default: 0)
     * outputHeight = val2;           (default: 0)
     */

    /* SLICES_PER_FRAME */
    /* slicesPerFrame = number        (default: 0) */
    slicesPerFrame = 1;               /* our default */

    /* SLAVE_CONVERT - used in parallel case */
    /* slaveConversion = string;     (default: "") */

    /* SPECIFICS_FILE */
    /* specificsFile = filename;     (default: "") */

    /* SPECIFICS_DEFINES */
    /* specificsDefines = string;    (default: "") */

    /* SEQUENCE_SIZE */
    /* mult_seq_headers = val;       (default: 0) */

    /* SIZE or YUV_SIZE or Y_SIZE - not used for JPEG format */
    /* yuvWidth   = val;             (default: 0)
     * yuvHeight  = val;             (default: 0)
     * realWidth  = yuvWidth;
     * realHeight = yuvHeight;
     */

    /* TUNE - not used in all param files in examples dir.
              Complicated as this invokes ParseTuneParam. */
    /* tuneingOn = TRUE;             (default: FALSE)
     * block_bound = val if 'B'      (default: 128)
     * etc. depending on what comes after TUNE...
     * Forget this for now...
     */

    /* USER_DATA */
    /* userDataFileName = filename;  (default: "") */

    /* YUV_FORMAT */
    /* yuvConversion = string;       (default: "") */
    /*
    **-----  replaces info fro ReadParamFile and parameter file  -----
    */

#if 0
/*NOTE*/
    /* do nothing for nonparallel case */
    SetIOConvert(FALSE /*optionSeen[OPTION_IO_CONVERT]*/);

    /* never seen RESIZE option in all param files */
    SetResize(FALSE /*optionSeen[OPTION_RESIZE]*/);
/*NOTE*/
#endif

    /* this will set fCodeI, fCodeB & fCodeP */
    SetFCode();

    return(TRUE);}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MPEGJOB - this function is called by _PG_MP_open
 *              - initializes all the necessary tasks when
 *              - MPEG-device is to be opened.
 */

boolean init_MPEGjob(PG_device *dev)
   {int i, bitstreamMode;
    BitBucket *bbPtr;
    MpegDevInfo *MpegDev;

    realStart  = 0;        /* no change in value here */
    realEnd    = INT_MAX;  /* no change in value here */
    firstFrame = 0;        /* no change in value here */
    lastFrame  = INT_MAX;  /* no change in value here */
    frameStart = -1;       /* no change in value here */
    frameEnd   = 0;        /* not used but keep for future */

    firstFrameDone = FALSE;
    FrameId = -1;

    ResetIFrameStats();  /* sets numBlocks, numBits, etc. to 0 */
    Fsize_Reset();       /* sets Fsize_x & Fsize_y to 0 */

    framesOutput = 0;

    Default_Init();

    if ( frameStart == -1 ) {
	PG_RAST_device *mdv;
	GET_RAST_DEVICE(dev, mdv);
        bbPtr = Bitio_New(mdv->fp);
    } else {
        bbPtr = NULL;
    }

    tc_hrs = 0; tc_min = 0; tc_sec = 0; tc_pict = 0; tc_extra = 0;
    for ( i = 0; i < firstFrame; i++ ) {
        IncrementTCTime();
    }

    totalFramesSent = firstFrame;
    currentGOP = gopSize;        /* so first I-frame generates GOP Header */

    /* Rate Control Initialization */
    bitstreamMode = getRateMode();
    if (bitstreamMode == FIXED_RATE) {
       initRateControl();
      /*
        SetFrameRate();
        */
         }
    
#ifdef BLEAH
fprintf(stdout, "firstFrame, lastFrame = %d, %d;  real = %d, %d\n",
        firstFrame, lastFrame, realStart, realEnd);
SC_fflush_safe(stdout);
#endif

    pastRefFrame = NULL;
    futureRefFrame = NULL;

/* allocate MpegDevInfo item */
    MpegDev = CMAKE(MpegDevInfo);

/* allocates sizeof(MpegFrame) memory to frameMemory - frame.c */
    Frame_Init(MpegDev->frameMemory);

/* save info for this particular MPEG device */
    MpegDev->dev            = dev;
    MpegDev->firstFrameDone = firstFrameDone;
    MpegDev->FrameId        = FrameId;
    MpegDev->Fsize_x        = Fsize_x;
    MpegDev->Fsize_y        = Fsize_y;
    MpegDev->framesOutput   = framesOutput;
    MpegDev->bbPtr          = bbPtr;
    MpegDev->currentGOP     = currentGOP;
    MpegDev->futureRefFrame = futureRefFrame;
    MpegDev->next           = listMpegDevices;

    listMpegDevices         = MpegDev;
    numMpegDevices++;

    return(TRUE);}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static MpegDevInfo *find_MpegDev(PG_device *dev)
   {MpegDevInfo *ptr;

    for (ptr = listMpegDevices; ptr != NULL; ptr = ptr->next)
       if (ptr->dev == dev)
	  break;

    return(ptr);}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static MpegDevInfo *del_MpegDev(PG_device *dev)
   {MpegDevInfo *ptr, *pptr;

    pptr = NULL;
    for (ptr = listMpegDevices; ptr != NULL; ptr = ptr->next)
       {if (ptr->dev == dev)
	   {if (pptr == NULL)
	       listMpegDevices = ptr->next;
	    else
	       pptr->next = ptr->next;
	    break;}
	pptr = ptr;}

    return(ptr);}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_MPEGJOB - this function is called by _PG_MP_close_device 
 *               - do all the necessary tasks before closing
 *               - MPEG-device.
 */

void close_MPEGjob(PG_device *dev)
   {MpegFrame *mpgfr;
    BitBucket *bbPtr;
    MpegDevInfo *MpegDev;

    if (!(MpegDev = del_MpegDev(dev)))
       {printf("MPEG device could not be found\n");
        exit(-1);}

    mpgfr = MpegDev->futureRefFrame;
    bbPtr = MpegDev->bbPtr;

    if (mpgfr != NULL)
       {Frame_Free(mpgfr);}

    /* SEQUENCE END CODE */
    if ((whichGOP == -1) && (frameStart == -1))
       {Mhead_GenSequenceEnder(bbPtr);}

    if (frameStart == -1)
       {Bitio_Flush(bbPtr);
        bbPtr = NULL;}

    MpegDev->dev            = NULL;
    MpegDev->futureRefFrame = NULL;
    MpegDev->bbPtr          = NULL;
    numMpegDevices--;

    Frame_Exit(MpegDev->frameMemory);
    CFREE(MpegDev);}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SAVE_IMAGE2FILE - this function is called by _PG_MP_finish_plot 
 *                 - writes RASTER image to MPEG file
 */

void save_image2file(PG_device *dev)
   {int bitstreamMode;
    BitBucket *bbPtr;
    int frameType;
    int frid;
    PG_RAST_device *mdv;
    MpegDevInfo *MpegDev;

    if (!(MpegDev = find_MpegDev(dev)))
       {printf("MPEG device could not be found\n");
        exit(-1);}

    futureRefFrame = MpegDev->futureRefFrame;
    bbPtr          = MpegDev->bbPtr;
    frid           = MpegDev->FrameId;
    Fsize_x        = MpegDev->Fsize_x;
    Fsize_y        = MpegDev->Fsize_y;

    /* frame type is I-Frame */
    frameType = 'i';
    frid++;

    pastRefFrame = futureRefFrame;
    futureRefFrame = Frame_New(frid, frameType, MpegDev->frameMemory);
    if (!futureRefFrame)
       {PRINT(stderr, "No New Frame Available\n");
	exit(1);}

/* copy mdv->frame to frame */
    GET_RAST_DEVICE(dev, mdv);
    CopyImage2Frame(mdv, futureRefFrame);

/* the above coding between 2 NOTEs is replaced by the following */
    if (! MpegDev->firstFrameDone)

       {char *userData = (char *)NULL;
        int userDataSize = 0;

        SetBlocksPerSlice();

        if ((whichGOP == -1) && (frameStart == -1))
	   {DBG_PRINT(("Generating sequence header\n"));
            bitstreamMode = getRateMode();
            if (bitstreamMode == FIXED_RATE)
	       {bit_rate = getBitRate();
	        buf_size = getBufferSize();}
            else
	       {bit_rate = -1;
	        buf_size = -1;}

            Mhead_GenSequenceHeader(bbPtr, Fsize_x, Fsize_y,
                                /* pratio */ aspectRatio,
                                /* pict_rate */ frameRate,
				/* bit_rate */ bit_rate,
                                /* buf_size */ buf_size,
			        /* c_param_flag */ 1,
                                /* iq_matrix */ qtable, 
			        /* niq_matrix */ niqtable,
                                /* ext_data */ NULL, 
			        /* ext_data_size */ 0,
                                /* user_data */ userData, 
			        /* user_data_size */ userDataSize);}

	MpegDev->firstFrameDone = TRUE;}

    MpegDev->futureRefFrame = futureRefFrame;
    ProcessRefFrame(MpegDev, lastFrame, mdv->out_fname);

    MpegDev->FrameId        = frid;
    MpegDev->Fsize_x        = Fsize_x;
    MpegDev->Fsize_y        = Fsize_y;
    MpegDev->framesOutput   = framesOutput;
    MpegDev->bbPtr          = bbPtr;
    MpegDev->currentGOP     = currentGOP;
    MpegDev->futureRefFrame = futureRefFrame;}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONVERTRGB2YIV - this function is called by CopyImage2Frame
 *                - converts RGB to YIV format.
 */

static void ConvertRGB2YIV(PG_RAST_device *mdv)
   {int i, j, l, w, h, size;
    int i1, i2, r, g, b;
    unsigned char *rpx, *gpx, *bpx, *Y, *I, *V;
    frame *fr;

    fr = mdv->raster;

    w = fr->width;
    h = fr->height;
    size = w*h;

    Y = CMAKE_N(unsigned char, size);
    I = CMAKE_N(unsigned char, size);
    V = CMAKE_N(unsigned char, size);
    memset(Y, 0, size);
    memset(I, 0, size);
    memset(V, 0, size);

    GET_RGB(fr, rpx, gpx, bpx);

    l = 0;    
    for (i = 0; i < h; i++)
        for (j = 0; j < w/2; j++)
	    {r = rpx[l];
	     g = gpx[l];
	     b = bpx[l];
	     l++;

	     i1 = (w/2)*(i/2) + j;
	     i2 = w*i + 2*j;

	     I[i1] += ((512*r - 429*g - 83*b)/4096 + 32);
	     Y[i2]  = ((306*r + 601*g + 117*b)/1024);
	     V[i1] += ((-173*r - 339*g + 512*b)/4096 + 32);

	     r = rpx[l];
	     g = gpx[l];
	     b = bpx[l];
	     l++;

	     i2++;

	     I[i1] += ((512*r - 429*g - 83*b)/4096 + 32);
	     Y[i2]  = ((306*r + 601*g + 117*b)/1024);
	     V[i1] += ((-173*r - 339*g + 512*b)/4096 + 32);};

    PUT_RGB(fr, Y, I, V);

    CFREE(rpx);
    CFREE(gpx);
    CFREE(bpx);

    return;}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPYIMAGE2FRAME - this function is called by save_image2file
 *                 - moves RASTER image to MPEG frame buffer
 */

static void CopyImage2Frame(PG_RAST_device *mdv, MpegFrame *mf)
   {unsigned char **origR, **origG, **origB;
    unsigned char *r, *g, *b;
    int image_height;        /* Number of rows in image */
    int image_width;	     /* Number of cols in image */
    int half_height;
    long i, j;
    frame *fr;

    ConvertRGB2YIV(mdv);

    fr = mdv->raster;
    image_width  = mdv->width;
    image_height = mdv->height;
    Fsize_Note(mf->id, image_width, image_height);

/* allocate memory for the raw YCbCr data to occupy */
    Frame_AllocYCC(mf);	 /* allocate space for mpeg frame */

    origR = mf->orig_y;
    origG = mf->orig_cr;
    origB = mf->orig_cb;

    GET_RGB(fr, r, g, b);

    for (j = 0; j < Fsize_y; j++)
        for (i = 0; i < Fsize_x; i++)
            origR[j][i] = r[i+j*image_height];

    half_height = image_height>>1;
    for (j = 0; j < (Fsize_y>>1); j++)
        for (i = 0; i < (Fsize_x>>1); i++)
            {origG[j][i] = g[i+j*half_height];
	     origB[j][i] = b[i+j*half_height];}

    return;}


/*--------------------------------------------------------------------------*/

/*                            INTERNAL PROCEDURES                           */

/*--------------------------------------------------------------------------*/

/* ProcessRefFrame - process an I or P frame -- encode it,
 *                 - and process any B frames that we can now
 */

static void ProcessRefFrame(MpegDevInfo *MpegDev, int lfr, char *ofnm)
   {MpegFrame *mfr;
    BitBucket *bb;

    char fileName[1024];
    FILE *fpointer = NULL;
    boolean separateFiles;

    mfr          = MpegDev->futureRefFrame;
    bb           = MpegDev->bbPtr;
    currentGOP   = MpegDev->currentGOP;
    framesOutput = MpegDev->framesOutput;

    separateFiles = (bb == NULL);

    if (separateFiles)
       {if (remoteIO)
	   {bb = Bitio_New(NULL);}
        else
	   {snprintf(fileName, 1024, "%s.frame.%d", ofnm, mfr->id);
            if ((fpointer = _PG_fopen(fileName, "wb")) == NULL)
	       {fprintf(stderr, 
			"ERROR:  Could not open output file(1):  %s\n",
		        fileName);
	        exit(1);}

            bb = Bitio_New(fpointer);};}

    /* nothing to do */
    if (mfr->id < realStart)
       return;

    /* first, output this frame */
    if (mfr->type == TYPE_IFRAME)
       {

#ifdef BLEAH
        fprintf(stdout, "I-frame %d, currentGOP = %d\n",
	        mfr->id, currentGOP);
        SC_fflush_safe(stdout);
#endif

        /* only start a new GOP with I */
        /* don't start GOP if only doing frames */
        if ((! separateFiles) && (currentGOP >= gopSize))
	   {int closed;
            static int num_gop = 0;

            /* first, check to see if closed GOP */
            if (totalFramesSent == mfr->id || pastRefFrame == NULL)
	       closed = 1;
            else
	       closed = 0;

            /* new GOP */
            if (num_gop != 0 && 
		mult_seq_headers && num_gop % mult_seq_headers == 0)
	       {if (! realQuiet)
		   {fprintf(stdout, 
			    "Creating new Sequence before GOP %d\n", 
			    num_gop);
	            SC_fflush_safe(stdout);}

	        Mhead_GenSequenceHeader(bb, Fsize_x, Fsize_y,
			/*pratio*/         aspectRatio,
			/*pict_rate*/      frameRate, 
			/*bit_rate*/       bit_rate,
			/*buf_size*/       buf_size,  
			/*c_param_flag*/   1,
			/*iq_matrix*/      customQtable, 
			/*niq_matrix*/     customNIQtable,
			/*ext_data*/       NULL,  
			/*ext_data_size*/  0,
			/*user_data*/      NULL, 
			/*user_data_size*/ 0);}

	    if (! realQuiet)
	       {fprintf(stdout, 
			"Creating new GOP (closed = %c) before frame %d\n",
			"FT"[closed], mfr->id);
		SC_fflush_safe(stdout);}

	    num_gop++;
	    Mhead_GenGOPHeader(bb,
			       0,    /* drop_frame_flag */
			       tc_hrs, tc_min, tc_sec, tc_pict,
			       closed,
			       0,    /* broken_link */
			       NULL, /* ext_data */
			       0,    /* ext_data_size */
			       NULL, /* user_data */
			       0     /* user_data_size */);
	    currentGOP -= gopSize;
	    if (pastRefFrame == NULL)
	       SetGOPStartTime(0);
	    else
	       SetGOPStartTime(pastRefFrame->id+1);}

	if ((mfr->id >= realStart) && (mfr->id <= realEnd))
	   {GenIFrame(bb, mfr);

	    framesOutput++;

	    if (separateFiles)
	       {if (remoteIO)
		   {SendRemoteFrame(mfr->id, bb);}
	        else
		   {Bitio_Flush(bb);
		    io_close(fpointer);};};}

	numI--;
	timeMask &= 0x6;

	currentGOP++;
	IncrementTCTime();}
    else
       {if ((mfr->id >= realStart) && (mfr->id <= realEnd))
	   {GenPFrame(bb, mfr, pastRefFrame);

	    framesOutput++;
	
	    if (separateFiles)
	       {if (remoteIO)
		   {SendRemoteFrame(mfr->id, bb);}
	        else
		   {Bitio_Flush(bb);
		    io_close(fpointer);};};}

	numP--;
	timeMask &= 0x5;
	ShowRemainingTime();

	currentGOP++;
	IncrementTCTime();}

    /* now free previous frame, if there was one */
    if (pastRefFrame != NULL)
       Frame_Free(pastRefFrame);

    /* note, we may still not free last frame if lfr is incorrect
     * (if the last frames are B frames, they aren't output!)
     */

    return;}


/* ShowRemainingTime - print out an estimate of the time 
 *                   - left to encode
 */

static void ShowRemainingTime()
   {static int lastTime = 0;
    float  total;
    time_t nowTime;
    float  secondsPerFrame;

    if (childProcess)
       return /* nothing */;

    if (numI + numP + numB == 0) /* no time left */
       return /* nothing */;

    if (timeMask != 0) /* haven't encoded all types yet */
       return /* nothing */;

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

    if ((quietTime >= 0) && (! realQuiet) && (! stdinUsed) &&
	((lastTime < (int)total) || ((lastTime-(int)total) >= quietTime) ||
	 (lastTime == 0) || (quietTime == 0)))
       {if (total > 270.0)
	   fprintf(stdout, "ESTIMATED TIME OF COMPLETION:  %d minutes\n",
		   ((int)total+30)/60);
	else
	   fprintf(stdout, "ESTIMATED TIME OF COMPLETION:  %d seconds\n",
		   (int)total);

	lastTime = (int)total;}

   return;}
