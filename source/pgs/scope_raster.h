/*
 * SCOPE_RASTER.H
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_SCOPE_RASTER

#define PCK_SCOPE_RASTER

/*--------------------------------------------------------------------------*/

/*                            MACRO DEFINITIONS                             */

/*--------------------------------------------------------------------------*/

#define N_RAST_COLOR 256

#define GET_RST_DEVICE(_d)      ((PG_RAST_device *) (_d)->raster)
#define GET_RAST_DEVICE(_d, _r) ((_r) = (PG_RAST_device *) (_d)->raster)
#define SET_RAST_DEVICE(_d, _r) ((_d)->raster = (FILE *) (_r))

#define GET_RGB(fr, _r, _g, _b)                                              \
   {_r = fr->r;                                                              \
    _g = fr->g;                                                              \
    _b = fr->b;}

#define PUT_RGB(fr, _r, _g, _b)                                              \
   {fr->r = _r;                                                              \
    fr->g = _g;                                                              \
    fr->b = _b;}

#define SET_RASTER_PIXEL(_f, _c, _x, _y, _z)                                 \
   {(_f)->n_on++;                                                            \
    (_f)->zb[m]  = _z;                                                       \
    (_f)->r[m]   = (_c)->red;                                                \
    (_f)->g[m]   = (_c)->green;                                              \
    (_f)->b[m]   = (_c)->blue;                                               \
    (_f)->box[0] = min((_f)->box[0], _x);                                    \
    (_f)->box[1] = max((_f)->box[1], _x);                                    \
    (_f)->box[2] = min((_f)->box[2], _y);                                    \
    (_f)->box[3] = max((_f)->box[3], _y);                                    \
    (_f)->box[4] = min((_f)->box[4], _z);                                    \
    (_f)->box[5] = max((_f)->box[5], _z);}


#define MPG_DEVICE(dev) (dev->type_index == MPEG_DEVICE)
#define JPG_DEVICE(dev) (dev->type_index == JPEG_DEVICE)

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                       STRUCTS AND TYPEDEFS                               */

/*--------------------------------------------------------------------------*/

typedef struct s_frame frame;
typedef struct s_PG_RAST_device PG_RAST_device;
typedef struct s_PG_CS_info PG_CS_info;

struct s_frame
   {int width;
    int height;
    int n_on;
    double box[PG_BOXSZ];  /* box containing active pixels */
    RGB_color_map bc;      /* background color pixel value */
    unsigned char *r;      /* R or Y */
    unsigned char *g;      /* G or Cr */
    unsigned char *b;      /* B or Cb */
    double *zb;};

struct s_PG_RAST_device
   {int nf;           /* number of files in family */
    int width;        /* width of video data */
    int height;       /* height of video data */
    int rgb_mode;     /* do RGB or lookup color */

    double text_scale;

    char *out_fname;  /* raster data file/device name */
    FILE *fp;
    frame *raster;};

struct s_PG_CS_info
   {double old_wc[4];
    double old_nc[4];
    double new_nc[4];};

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* GSDLR.C declarations */

extern void
 _PG_rst_draw_line_2d(PG_device *dev, int *i1, int *i2),
 _PG_rst_draw_line_3d(PG_device *dev, int *i1, int *i2);


/* GSPR_MP.C declarations */

extern int
 _PG_do_intra(PG_RAST_device *dev);


/* GSDV_RST.C declarations */

extern int
 PG_setup_mpeg_device(PG_device *dev),
 PG_setup_jpeg_device(PG_device *dev),
 PG_setup_raster_device(PG_device *dev);

extern double
 *_PG_frame_z_buffer(frame *fr);

extern void
 PG_free_raster_device(PG_RAST_device *mdv),
 PG_clear_raster_device(PG_device *dev),
 PG_free_frame(frame *fr),
 _PG_clear_raster_region(PG_device *dev, int nd, PG_coord_sys cs,
			 double *bx, int pad),
 _PG_code_vl(PG_RAST_device *dev, short *block, int type, int dctnum),
 _PG_align(PG_RAST_device *dev),
 _PG_write_code(PG_RAST_device *dev, unsigned int data, int nb),
 _PG_rst_set_dev_prop(PG_device *dev, int dx, int dy, int nc),
 _PG_copy_frame(frame *fr1, frame *fr2);

extern PG_RAST_device
 *PG_make_raster_device(int w, int h, char *name,
			frame *infr, int rgb, FILE *fp),
 *_PG_get_raster(PG_device *dev, int resz);


/* GSPR_RST.C declarations */

extern void
 _PG_rst_draw_disjoint_polyline_3(PG_device *dev, PG_coord_sys cs, long n,
				  double **r, int clip, int cpy),
 _PG_rst_shade_poly(PG_device *dev, int nd, int n, double **r);

#ifdef __cplusplus
}
#endif

#endif
