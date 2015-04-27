/*
 * GSCLR.C - color handling routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

#define PG_map_to_color_table(d, pal)                                        \
    if ((d) != NULL)                                                         \
       {if ((d)->map_to_color_table != NULL)                                 \
           (*(d)->map_to_color_table)(d, pal);}

#define PG_match_rgb_colors(d, pal)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->match_rgb_colors != NULL)                                   \
           (*(d)->match_rgb_colors)(d, pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE void _PG_shade_hue(RGB_color_map *cm, PG_palette *pal,
				 int j, unsigned long *pv, int off,
				 int r, int g, int b, int n, int ns,
				 double fl, double fu)
    {int i;
     double a;

     a = (fu - fl)/n;

     for (i = 1; (i <= n) && (j < ns); i++, j++)
         {cm[j].red   = pal->max_red_intensity*r*(a*i + fl);
          cm[j].green = pal->max_green_intensity*g*(a*i + fl);
          cm[j].blue  = pal->max_blue_intensity*b*(a*i + fl);

          if (pv != NULL)
             pv[j] = j + off + 2;};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_INSTALL_COLOR - install RGB values as color J */

static INLINE int _PG_install_color(RGB_color_map *true_cm, int j,
				    unsigned long *pv, int off,
				    double mxr, double mxg, double mxb,
				    double fr, double fg, double fb)
    {

     true_cm[j].red   = mxr*fr;
     true_cm[j].green = mxg*fg;
     true_cm[j].blue  = mxb*fb;

     if (pv != NULL)
        pv[j] = j + off + 2;

     j++;

     return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MK_PALETTE - allocate and initialize a new palette */

PG_palette *PG_mk_palette(PG_device *dev, const char *name, int nclr)
   {int npc, ndc, nclim, i;
    unsigned long *pv;
    double mxr, mxg, mxb;
    PG_palette *npal;
    RGB_color_map *pcm;

    mxr = dev->max_red_intensity;
    mxg = dev->max_green_intensity;
    mxb = dev->max_blue_intensity;

/* the 256 max is to make it doable at all on a system with
 * 24 bits of color - nobody wants to wait a year for X to
 * allocate 16 million color
 */
    nclim = 256;
    nclim = 4*_PG_gattrs.max_color_factor;

    npc = min(nclr, nclim);
    ndc = min(dev->absolute_n_color, nclim);

    npal = CMAKE(PG_palette);

    if (strcmp(dev->name, "IMAGE") == 0)
       pv = CMAKE_N(unsigned long, npc+2);
    else
       pv = NULL;

    npal->max_pal_dims        = 0;
    npal->pal_dims            = NULL;
    npal->pixel_value         = pv;
    npal->true_colormap       = CMAKE_N(RGB_color_map, npc + 2);
    npal->pseudo_colormap     = CMAKE_N(RGB_color_map, npc + 2);
    npal->max_red_intensity   = mxr;
    npal->max_green_intensity = mxg;
    npal->max_blue_intensity  = mxb;
    npal->n_pal_colors        = npc;
    npal->n_dev_colors        = ndc;
    npal->name                = CSTRSAVE(name);
    npal->next                = NULL;

    pcm  = npal->true_colormap;

    for (i = 0; i < (npc + 2); i++)
        {pcm[i].red   = 0;
         pcm[i].green = 0;
         pcm[i].blue  = 0;};

    pcm  = npal->pseudo_colormap;

    for (i = 0; i < (npc + 2); i++)
        {pcm[i].red   = 0;
         pcm[i].green = 0;
         pcm[i].blue  = 0;};

/* load black and white into the palette */

    pcm  = npal->true_colormap;

    pcm[0].red   = 0;
    pcm[0].green = 0;
    pcm[0].blue  = 0;
    pcm[1].red   = MAXPIX*mxr;
    pcm[1].green = MAXPIX*mxg;
    pcm[1].blue  = MAXPIX*mxb;

    return(npal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RL_PALETTE - release a palette */

void PG_rl_palette(PG_palette *pal)
   {int i, **pd;

    CFREE(pal->name);
    CFREE(pal->pixel_value);
    CFREE(pal->true_colormap);
    CFREE(pal->pseudo_colormap);
    pal->next = NULL;

    if (pal->pal_dims != NULL)
       {pd = pal->pal_dims;
        for (i = 0; i < pal->max_pal_dims; i++)
            CFREE(pd[i]);
        CFREE(pd);}

    CFREE(pal);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ATTACH_PALETTE_DIMS - attach a set of shapes for multiple
 *                         - multi-dimensional palettes
 */

static PG_palette *_PG_attach_palette_dims(PG_palette *pal, int nc,
                                           int nd, int *sh)
   {int i, j, ncd, **pd, *pp;

    pd = CMAKE_N(int *, nd);
    for (i = 0; i < nd; i++)
        {if (sh == NULL)
            ncd = POW((double) nc, 1.0/((double) (i + 1)));

         pp = CMAKE_N(int, i+1);
         for (j = 0; j <= i; j++)
             pp[j] = (sh == NULL) ? ncd : sh[j];

         pd[i] = pp;};

    pal->max_pal_dims = nd;
    pal->pal_dims     = pd;

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TRANS_PALETTE - fill a part of a palette with colors transforming
 *                   - from the first RGB value to the second
 */

static void _PG_trans_palette(RGB_color_map *cm, int nc,
                              double r1, double g1, double b1,
                              double r2, double g2, double b2, double nx)
   {int i;
    double f, x, x1, x2, c;
   
/* control the color distribution */
    if (nx == 0.0)
       {f = 1.0/(nc - 1.0);
        for (i = 0; i < nc; i++)
            {x2 = i*f;
             x1 = 1.0 - x2;

             cm[i].red   = r1*x1 + r2*x2;
             cm[i].green = g1*x1 + g2*x2;
             cm[i].blue  = b1*x1 + b2*x2;};}

    else if (nx < 0.0)
       {nx = -nx;
        f  = 1.0/(nc - 1.0);
        for (i = 0; i < nc; i++)
            {x  = i*f;
	     x2 = PM_pow(x, nx);
             x1 = 1.0 - x2;

             cm[i].red   = r1*x1 + r2*x2;
             cm[i].green = g1*x1 + g2*x2;
             cm[i].blue  = b1*x1 + b2*x2;};}

    else
       {f = 1.0/(nc - 1.0);
        c = 1.0/(1.0 - PM_pow(0.5, nx));
        for (i = 0; i < nc; i++)
            {x  = 1.0/(1.0 + i*f);
             x2 = c*(1.0 - POW(x, nx));
             x1 = 1.0 - x2;

             cm[i].red   = r1*x1 + r2*x2;
             cm[i].green = g1*x1 + g2*x2;
             cm[i].blue  = b1*x1 + b2*x2;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WB_PALETTE - compute a white to black palette */

static PG_palette *_PG_wb_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "wb", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    _PG_trans_palette(true_cm, nc, mxr, mxg, mxb, 0.0, 0.0, 0.0, 1.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RGB_PALETTE - compute a blue -> green -> red palette */

static PG_palette *_PG_rgb_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    int s1, s2, s3, s4;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "rgb", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    s1 = nc/4.0;
    s2 = (nc - s1)/3.0;
    s3 = (nc - s1 - s2)/2.0;
    s4 = (nc - s1 - s2 - s3);

/* blue -> cyan */
    _PG_trans_palette(true_cm,
                      s1, 0.0, 0.0, mxb, 0.0, mxg, mxb, 2.0);

/* cyan -> green */
    _PG_trans_palette(true_cm + s1 - 1,
                      s2, 0.0, mxg, mxb, 0.0, mxg, 0.0, -2.0);

/* green -> yellow */
    _PG_trans_palette(true_cm + s1 + s2 - 1,
                      s3, 0.0, mxg, 0.0, mxr, mxg, 0.0, 2.0);

/* yellow -> red */
    _PG_trans_palette(true_cm + s1 + s2 + s3 - 1,
                      s4+1, mxr, mxg, 0.0, mxr, 0.0, 0.0, -2.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CYM_PALETTE - compute a cyan -> yellow -> magenta palette */

static PG_palette *_PG_cym_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    int s1, s2, s3, s4;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "cym", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    s1 = nc/4.0;
    s2 = (nc - s1)/3.0;
    s3 = (nc - s1 - s2)/2.0;
    s4 = (nc - s1 - s2 - s3);

/* cyan -> green */
    _PG_trans_palette(true_cm,
                      s1, 0.0, mxg, mxb, 0.0, mxg, 0.0, -2.0);

/* green -> yellow */
    _PG_trans_palette(true_cm + s1 - 1,
                      s2, 0.0, mxg, 0.0, mxr, mxg, 0.0, 2.0);

/* yellow -> red */
    _PG_trans_palette(true_cm + s1 + s2 - 1,
                      s3, mxr, mxg, 0.0, mxr, 0.0, 0.0, -2.0);

/* red -> magenta */
    _PG_trans_palette(true_cm + s1 + s2 + s3 - 1,
                      s4+1, mxr, 0.0, 0.0, mxr, 0.0, mxb, 2.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_HC_PALETTE - compute a blue -> white -> red palette */

static PG_palette *_PG_hc_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    int s1, s2, s3;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "hc", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    s1 = nc/4.0;
    s2 = (nc - s1)/3.0;
    s3 = (nc - s1 - s2)/2.0;

/* dark blue -> blue */
    _PG_trans_palette(true_cm,
                      s1, 0.0, 0.0, 0.5*mxb, 0.0, 0.0, mxb, 1.0);

/* blue -> white */
    _PG_trans_palette(true_cm + s1 - 1,
                      s2, 0.0, 0.0, mxb, mxr, mxg, mxb, 1.0);

/* white -> red */
    _PG_trans_palette(true_cm + s1 + s2 - 1,
                      s3, mxr, mxg, mxb, mxr, 0.0, 0.0, 1.0);

/* red -> dark red */
    _PG_trans_palette(true_cm + s1 + s2 + s3 - 1,
                      s2+1, mxr, 0.0, 0.0, 0.5*mxr, 0.0, 0.0, 1.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BGY_PALETTE - compute a blue -> cyan -> green -> yellow palette */

static PG_palette *_PG_bgy_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    int s1, s2, s3;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "bgy", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    s1 = nc/3.0;
    s2 = (nc - s1)/2.0;
    s3 = (nc - s1 - s2);

/* blue -> cyan */
    _PG_trans_palette(true_cm,
                      s1, 0.0, 0.0, mxb, 0.0, mxg, mxb, 2.0);

/* cyan -> green */
    _PG_trans_palette(true_cm + s1 - 1,
                      s2, 0.0, mxg, mxb, 0.0, mxg, 0.0, -2.0);

/* green -> yellow */
    _PG_trans_palette(true_cm + s1 + s2 - 1,
                      s3+1, 0.0, mxg, 0.0, mxr, mxg, 0.0, 2.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TRI_PALETTE - compute a blue -> black -> white -> red palette */

static PG_palette *_PG_tri_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    int s1, s2, s3;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "tri", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    s1 = nc/3.0;
    s2 = 0.66666*(nc - s1);
    s3 = (nc - s1 - s2);

/* blue -> black */
    _PG_trans_palette(true_cm,
                      s1, 0.0, 0.0, mxb, 0.0, 0.0, 0.0, -2.0);

/* black -> white */
    _PG_trans_palette(true_cm + s1 - 1,
                      s2, 0.0, 0.0, 0.0, mxr, mxg, mxb, 1.0);

/* white -> red */
    _PG_trans_palette(true_cm + s1 + s2 - 1,
                      s3+1, mxr, mxg, mxb, mxr, 0.0, 0.0, 0.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_IRON_PALETTE - compute a black -> red -> yellow -> white palette */

static PG_palette *_PG_iron_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    int s1, s2, s3;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "iron", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    s1 = 0.5*nc;
    s2 = 0.75*(nc - s1);
    s3 = (nc - s1 - s2);

/* black -> red */
    _PG_trans_palette(true_cm,
                      s1, 0.0, 0.0, 0.0, mxr, 0.0, 0.0, 2.0);

/* red -> yellow */
    _PG_trans_palette(true_cm + s1 - 1,
                      s2, mxr, 0.0, 0.0, mxr, mxg, 0.0, 2.0);

/* yellow -> white */
    _PG_trans_palette(true_cm + s1 + s2 - 1,
                      s3+1, mxr, mxg, 0.0, mxr, mxg, mxb, 2.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_THRESH_PALETTE - compute a blue -> cyan  yellow -> red palette */

static PG_palette *_PG_thresh_palette(PG_device *dev, PG_palette *pal, int nc)
   {int ns, reg;
    int s1, s2;
    double mxr, mxg, mxb;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "thresh", nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    s1 = 0.5*nc;
    s2 = nc - s1;

/* blue -> cyan */
    _PG_trans_palette(true_cm,
                      s1, 0.0, 0.0, mxb, 0.0, mxg, mxb, 2.0);

/* yellow -> red */
    _PG_trans_palette(true_cm + s1 - 1,
                      s2+1, mxr, mxg, 0.0, mxr, 0.0, 0.0, -2.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RAND_PALETTE - compute a palette with random colors */

static PG_palette *_PG_rand_palette(PG_device *dev, PG_palette *pal)
   {int i, j, k, ic, nc, reg;
    int *ip; 
    PG_palette *root;
    RGB_color_map *true_cm, *root_cm;

    root    = dev->color_table;
    root_cm = root->true_colormap;
    nc      = root->n_pal_colors;

    ip = CMAKE_N(int, nc);
    for (i = 0; i < nc; i++)
        ip[i] = i;

    for (i = 0, ic = nc-1; i < nc; i++, ic--)
        {j = 0.5*(1.0 + PM_random(-1))*ic;

         k      = ip[j];
         ip[j]  = ip[ic];
         ip[ic] = k;};

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "rand", nc);

    true_cm = pal->true_colormap + 2;

    for (i = 0; i < nc; i++)
        true_cm[i] = root_cm[ip[i]];

    CFREE(ip);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MONO_PALETTES - create the mono chromatic palettes */

static void _PG_mono_palettes(PG_device *dev, int nct)
   {int l, ns, nc;
    int s1, s2;
    double mxr, mxg, mxb, rf, gf, bf, rw, gw, bw, *pv;
    PG_palette *pal;
    RGB_color_map *true_cm;
    static char *colors[] = {"magentas", "blues", "cyans",
                             "greens", "yellows", "reds"};
    static double val[] = {1.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 1.0, 1.0,
                           0.0, 1.0, 0.0,   1.0, 1.0, 0.0,   1.0, 0.0, 0.0};

    nc = nct/6.0;
    ns  = MAXPIX;
    mxr = ns*dev->max_red_intensity;
    mxg = ns*dev->max_green_intensity;
    mxb = ns*dev->max_blue_intensity;

    pv = val;
    for (l = 0; l < 6; l++)
        {pal = PG_mk_palette(dev, colors[l], nc);

         true_cm = pal->true_colormap + 2;

         s1 = 0.715*nc;
         s2 = nc - s1;

         rf = *pv++;
         gf = *pv++;
         bf = *pv++;

         rw = max(rf, 0.85);
         gw = max(gf, 0.85);
         bw = max(bf, 0.85);

/* low value -> high value */
         _PG_trans_palette(true_cm, s1,
                           0.5*rf*mxr, 0.5*gf*mxg, 0.5*bf*mxb,
                           rf*mxr, gf*mxg, bf*mxb,
                           1.0);

/* high saturation -> low saturation */
         _PG_trans_palette(true_cm + s1 - 1, s2+1,
                           rf*mxr, gf*mxg, bf*mxb,
                           rw*mxr, gw*mxg, bw*mxb,
                           1.0);

         PG_register_palette(dev, pal, FALSE);};

    return;}

/*--------------------------------------------------------------------------*/

/*                           PRINCIPAL COLOR SELECTORS                      */

/*--------------------------------------------------------------------------*/

/* _PG_BW_PALETTE - compute a black and white palette
 *                - always add ABSOLUTE black and white in the palette
 *                - as the first two colors
 */

static PG_palette *_PG_bw_palette(PG_device *dev, PG_palette *pal,
                                  int off, int nc)
   {int i, j, ns, reg;
    unsigned long *pv;
    double mxr, mxg, mxb,  scale;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "bw", nc);

    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    true_cm += (2 + off);
    if (pv != NULL)
       pv += (2 + off);

/* NOTE: there is no sense in repeating full black and full white */
    scale = 1.0/(nc + 1.0);
    for (i = 0; i < nc; i++)
        {j = i + 1;
         true_cm[i].red   = mxr*j*scale;
         true_cm[i].green = mxg*j*scale;
         true_cm[i].blue  = mxb*j*scale;

         if (pv != NULL)
            pv[i] = i + off + 2;};

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RAINBOW_PALETTE - set up a chromatic color palette
 *                     - always add ABSOLUTE black and white in the palette
 *                     - as the first two colors
 */

static PG_palette *_PG_rainbow_palette(PG_device *dev, PG_palette *pal,
                                       int off, int nc)
   {int i, j, l, ns, nb, nv, nl, nr;
    int reg;
    unsigned long *pv;
    RGB_color_map *true_cm;
    double a, r, g, b, rx, gx, bx, mxr, mxg, mxb, fl, fu, fs;
    double *pcv;
    static double cv[] = {1.0, 0.0, 1.0,  0.0, 0.0, 1.0,   0.0, 1.0, 1.0,
                          0.0, 1.0, 0.0,  1.0, 1.0, 0.0,   1.0, 0.0, 0.0};

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "rainbow", nc);

    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    true_cm += (2 + off);
    if (pv != NULL)
       pv += (2 + off);

/* number of colors in hue
 * nv and nl coefficients based on 14 colors divided to 10 and 4
 */
    nb = nc/6.0;
    fl = 1.0/1.4;
    nv = fl*((double) nb);
    nl = 0.4*((double) nv);
    nr = nc - 6.0*(nv + nl);

/* set a rainbow spectrum: magenta, blue, cyan, green, yellow, red */
    fl = 0.5;
    fu = 1.0;
    fs = 0.15;
    pcv = cv;
    for (l = 0, j = 0; l < 6; l++)
        {a = (fu - fl)/nv;
         r = (*pcv++)*mxr;
         g = (*pcv++)*mxg;
         b = (*pcv++)*mxb;

	 rx = 0;
	 gx = 0;
	 bx = 0;

         for (i = 1; (i <= nv) && (j < ns); i++, j++)
             {rx = r*(a*i + fl);
              gx = g*(a*i + fl);
              bx = b*(a*i + fl);
              true_cm[j].red   = rx;
              true_cm[j].green = gx;
              true_cm[j].blue  = bx;

              if (pv != NULL)
                 pv[j] = j + off + 2;};

         a = ns*(1.0 - fs)/nl;
         for (i = 1; (i <= nl) && (j < ns); i++, j++)
             {rx += a;
              gx += a;
              bx += a;
              rx  = min(rx, ns);
              gx  = min(gx, ns);
              bx  = min(bx, ns);
              true_cm[j].red   = rx;
              true_cm[j].green = gx;
              true_cm[j].blue  = bx;

              if (pv != NULL)
                 pv[j] = j + off + 2;};};

/* fill the rest of the color map with gray scale */
    if (nr > 0)
       _PG_shade_hue(true_cm, pal, j, pv, off,
		     ns, ns, ns, nr, ns, 0.5, 1.0);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_2DRAINBOW_PALETTE - set up a chromatic color palette
 *                       - always add ABSOLUTE black and white in the palette
 *                       - as the first two colors
 */

static PG_palette *_PG_2drainbow_palette(PG_device *dev, PG_palette *pal,
                                         int off, int nc)
   {int i, j, l, ns, nb, nv, nl, nr, shape[2];
    int reg;
    unsigned long *pv;
    RGB_color_map *true_cm;
    double a, r, g, b, rx, gx, bx, mxr, mxg, mxb, fl, fu, fs;
    double *pcv;
    static double cv[] = {1.0, 0.0, 1.0,  0.0, 0.0, 1.0,   0.0, 1.0, 1.0,
                          0.0, 1.0, 0.0,  1.0, 1.0, 0.0,   1.0, 0.0, 0.0};

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "2drainbow", nc);

    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    true_cm += (2 + off);
    if (pv != NULL)
       pv += (2 + off);

/* number of colors in hue
 * nv and nl coefficients based on 14 colors divided to 10 and 4
 */
    nb = nc/6.0;
    fl = 1.0/1.4;
    nv = fl*((double) nb);
    nl = 0.4*((double) nv);
    nr = nc - 6.0*(nv + nl);

/* set a rainbow spectrum: magenta, blue, cyan, green, yellow, red */
    fl = 0.5;
    fu = 1.0;
    fs = 0.15;
    pcv = cv;
    for (l = 0, j = 0; l < 6; l++)
        {a = (fu - fl)/nv;
         r = (*pcv++)*mxr;
         g = (*pcv++)*mxg;
         b = (*pcv++)*mxb;

	 rx = 0;
	 gx = 0;
	 bx = 0;

         for (i = 1; (i <= nv) && (j < ns); i++, j++)
             {rx = r*(a*i + fl);
              gx = g*(a*i + fl);
              bx = b*(a*i + fl);
              true_cm[j].red   = rx;
              true_cm[j].green = gx;
              true_cm[j].blue  = bx;

              if (pv != NULL)
                 pv[i] = i + off + 2;};

         a = ns*(1.0 - fs)/nl;
         for (i = 1; (i <= nl) && (j < ns); i++, j++)
             {rx += a;
              gx += a;
              bx += a;
              rx  = min(rx, ns);
              gx  = min(gx, ns);
              bx  = min(bx, ns);
              true_cm[j].red   = rx;
              true_cm[j].green = gx;
              true_cm[j].blue  = bx;

              if (pv != NULL)
                 pv[i] = i + off + 2;};};

/* fill the rest of the color map with gray scale */
    if (nr > 0)
       _PG_shade_hue(true_cm, pal, j, pv, off,
		     ns, ns, ns, nr, ns, 0.5, 1.0);

    shape[0] = 14;
    shape[1] = 6;

    _PG_attach_palette_dims(pal, nc, 2, shape);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_2DCTAB1_PALETTE - set up a 2d color table (as specified by 
 *                     - Pete Keller)
 *                     - always add ABSOLUTE black and white in the palette
 *                     - as the first two colors
 */

static PG_palette *_PG_2dctab1_palette(PG_device *dev, PG_palette *pal,
                                       int off)
   {int i, j, k, nc, shape[2];
    double r0, g0, b0, dxr, dxg, dxb, saturated;
    int reg;
    RGB_color_map *true_cm;

    nc = 192;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "2dctab1", nc);

    true_cm = pal->true_colormap;

    true_cm += (2 + off);
    
    saturated = MAXPIX;

    r0 = 0.0;
    g0 = saturated;
    b0 = saturated;

/*
    dxr = saturated/23.0;
    dxg = -dxr;
    dxb = -saturated/7.0;

    for (i = 0, k = 0; i < 8; i++)
        for (j = 0; j < 24; j++, k++)
            {true_cm[k].red   = r0 + j * dxr;
             true_cm[k].green = g0 + j * dxg;
             true_cm[k].blue  = b0 + i * dxb;}
*/

    dxr = saturated/11.0;
    dxg = -dxr;
    dxb = -saturated/7.0;

    for (i = 0, k = 0; i < 8; i++)
        {for (j = 0; j < 12; j++, k++)
            {true_cm[k].red   = r0 + j * dxr;
             true_cm[k].green = g0;
             true_cm[k].blue  = b0 + i * dxb;}
         for (j = 0; j < 12; j++, k++)
            {true_cm[k].red   = saturated;
             true_cm[k].green = g0 + j * dxg;
             true_cm[k].blue  = b0 + i * dxb;};} 

    shape[0] = 24;
    shape[1] =  8;

    _PG_attach_palette_dims(pal, nc, 2, shape);

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SPECTRUM_PALETTE - a different color palette
 *                      - always add ABSOLUTE black and white in the palette
 *                      - as the first two colors
 */

static PG_palette *_PG_spectrum_palette(PG_device *dev, PG_palette *pal,
                                        int off, int nc)
   {int i, j, reg;
    int ns, s1, s2, s3, s4, s5, s6, ss;
    unsigned long *pv;
    double mxr, mxg, mxb, f, x;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "spectrum", nc);

    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    true_cm += (off + 2);
    if (pv != NULL)
       pv += (2 + off);

    s1 = 0.20*nc;
    s4 = 0.20*nc;
    s5 = 0.20*nc;
    s6 = 0.20*nc;
    ss = nc - (s1 + s4 + s5 + s6);
    s2 = 0.5*ss;
    s3 = ss - s2;

/* blue -> cyan */
    f = 1.0/((double) s1);
    for (i = 0, j = 0; i < s1; i++, j++)
        {x = 1.0/(1.0 + i*f);
         true_cm[j].blue  = mxb;
         true_cm[j].green = mxg*1.33333*(1.0 - x*x);
         true_cm[j].red   = 0;

         if (pv != NULL)
            pv[j] = j + off + 2;};

/* cyan -> green */
    f = 1.0/((double) s2);
    for (i = 0; i < s2; i++, j++)
        {x = 1.0/(1.0 + (s2 - i)*f);
         true_cm[j].blue  = mxb*1.33333*(1.0 - x*x);
         true_cm[j].green = mxg;
         true_cm[j].red   = 0;

         if (pv != NULL)
            pv[j] = j + off + 2;};

/* green -> yellow */
    f = 1.0/((double) s3);
    for (i = 0; i < s3; i++, j++)
        {x = 1.0/(1.0 + i*f);
         true_cm[j].blue  = 0;
         true_cm[j].green = mxg;
         true_cm[j].red   = mxr*1.33333*(1.0 - x*x);

         if (pv != NULL)
            pv[j] = j + off + 2;};

/* yellow -> red */
    f = 1.0/((double) s4);
    for (i = 0; i < s4; i++, j++)
        {x = 1.0/(1.0 + (s4 - i)*f);
         true_cm[j].blue  = 0;
         true_cm[j].green = mxg*1.33333*(1.0 - x*x);
         true_cm[j].red   = mxr;

         if (pv != NULL)
            pv[j] = j + off + 2;};

/* red -> magenta */
    f = 1.0/((double) s5);
    for (i = 0; i < s5; i++, j++)
        {x = 1.0/(1.0 + i*f);
         true_cm[j].blue  = mxb*1.33333*(1.0 - x*x);
         true_cm[j].green = 0;
         true_cm[j].red   = mxr;

         if (pv != NULL)
            pv[j] = j + off + 2;};

/* magenta -> blue */
    f = 1.0/((double) s6);
    for (i = 0; i < s6; i++, j++)
        {x = 1.0/(1.0 + (s6 - i)*f);
         true_cm[j].blue  = mxb;
         true_cm[j].green = 0;
         true_cm[j].red   = mxr*1.33333*(1.0 - x*x);

         if (pv != NULL)
            pv[j] = j + off + 2;};

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIT_CONTRAST_COLOR - return the value of the Ith color at X
 *                        - for the contrast palette
 */

double _PG_fit_contrast_color(double x, int i)
   {double c;
    static double coeff[18][7] = {{-0.111384,  0.0010881, 5.29941,  -7.95243,  -0.660816, 11.4918,  -7.21765},
                                  {-0.183919,  0.917355,  1.46455,  -2.08342,  -0.559403,  3.64277, -2.19793},
                                  { 1.0,       1.0,       1.0,       1.0,       1.0,       1.0,      1.0},

                                  {0.973133,   0.716276, -7.10909,  33.9296,  -78.3328,   77.4848, -27.1113},
                                  {1.04129,   -0.656724,  1.49683,  11.2711,  -49.8407,   60.6168, -23.3788},
                                  {0.788826,   3.15097, -37.1244,   97.8121, -109.985,    54.0,     -8.64},

                                  { 1.19825, -15.5503,  118.001,  -369.484,   578.855,  -448.748,  135.946},
                                  {-0.1875,    3.98646, -24.6406,   50.3867,  -10.139,   -46.4227,  28.0194},
                                  { 0.885724, -8.29119,  62.8982, -182.894,   261.387,  -184.068,   51.0846},

                                  {0.0,        0.0,       0.0,       0.0,       0.0,       0.0,      0.0},
                                  {1.73125,  -14.3875,   97.6174, -318.443,   512.438,  -396.734,  118.329},
                                  {1.73125,  -14.3875,   97.6174, -318.443,   512.438,  -396.734,  118.329},

                                  {0.705649,  -3.53468,  22.2842,  -53.6505,   77.7098,  -65.402,   22.0933},
                                  {0.174466,  -4.91226,  45.5935, -187.462,   366.955,  -324.986,  105.633},
                                  {0.421731, -11.8017,  107.577,  -430.112,   820.49,   -722.444,  236.083},

                                  {0.0,        0.0,       0.0,       0.0,       0.0,       0.0,      0.0},
                                  {3.0,      -38.1491,  261.234,  -863.494,  1448.57,  -1187.6,    376.437},
                                  {-1.71111,  33.6163, -236.6,     786.343, -1329.17,   1099.91,  -351.834}};

    c = PM_horner(x, coeff[i], 0, 6);
    c = min(c, 0.95);
    c = max(c, 0.05);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CLR_CONTRAST_PALETTE - a color contrast palette
 *                          - always add ABSOLUTE black and white
 *                          - as the first two colors
 */

static PG_palette *_PG_clr_contrast_palette(PG_device *dev, PG_palette *pal,
                                            int off, int nc)
   {int i, j, reg, ns, nb, nbb;
    unsigned long *pv;
    double mxr, mxg, mxb, in, x, r, g, b;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "contrast", nc);

    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

    ns  = MAXPIX;
    mxr = ns*pal->max_red_intensity;
    mxg = ns*pal->max_green_intensity;
    mxb = ns*pal->max_blue_intensity;

    true_cm += (off + 2);
    if (pv != NULL)
       pv += (2 + off);

    j   = 0;
    nbb = nc / 6;

/* blue */
    nb = nbb;
    in = 1.0/(nb - 1.0);
    for (i = 0; i < nb; i++)
        {x = i*in;
         r = _PG_fit_contrast_color(x, 0);
         g = _PG_fit_contrast_color(x, 1);
         b = _PG_fit_contrast_color(x, 2);
         j = _PG_install_color(true_cm, j, pv, off, mxr, mxg, mxb, r, g, b);};

/* yellow */
    nb = nbb + 1;
    in = 1.0/(nb - 1.0);
    for (i = 0; i < nb; i++)
        {x = i*in;
         r = _PG_fit_contrast_color(x, 3);
         g = _PG_fit_contrast_color(x, 4);
         b = _PG_fit_contrast_color(x, 5);
         j = _PG_install_color(true_cm, j, pv, off, mxr, mxg, mxb, r, g, b);};

/* magenta */
    nb = nbb + 1;
    in = 1.0/(nb - 1.0);
    for (i = 0; i < nb; i++)
        {x = i*in;
         r = _PG_fit_contrast_color(x, 6);
         g = _PG_fit_contrast_color(x, 7);
         b = _PG_fit_contrast_color(x, 8);
         j = _PG_install_color(true_cm, j, pv, off, mxr, mxg, mxb, r, g, b);};

/* cyan */
    nb = nbb - 2;
    in = 1.0/(nb - 1.0);
    for (i = 0; i < nb; i++)
        {x = i*in;
         r = _PG_fit_contrast_color(x, 9);
         g = _PG_fit_contrast_color(x, 10);
         b = _PG_fit_contrast_color(x, 11);
         j = _PG_install_color(true_cm, j, pv, off, mxr, mxg, mxb, r, g, b);};

/* red */
    nb = nbb;
    in = 1.0/(nb - 1.0);
    for (i = 0; i < nb; i++)
        {x = i*in;
         r = _PG_fit_contrast_color(x, 12);
         g = _PG_fit_contrast_color(x, 13);
         b = _PG_fit_contrast_color(x, 14);
         j = _PG_install_color(true_cm, j, pv, off, mxr, mxg, mxb, r, g, b);};

/* green */
    nb = nbb + (nc - nbb*6);
    in = 1.0/(nb - 1.0);
    for (i = 0; i < nb; i++)
        {x = i*in;
         r = _PG_fit_contrast_color(x, 15);
         g = _PG_fit_contrast_color(x, 16);
         b = _PG_fit_contrast_color(x, 17);
         j = _PG_install_color(true_cm, j, pv, off, mxr, mxg, mxb, r, g, b);};

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_STANDARD_PALETTE - set up a standard color palette for dev */

static PG_palette *_PG_standard_palette(PG_device *dev, PG_palette *pal,
                                        int off, int nc, int *gs)
   {int i, reg;
    unsigned long *pv;
    RGB_color_map *true_cm;

    reg = (pal == NULL);
    if (reg)
       pal = PG_mk_palette(dev, "standard", nc);

    true_cm = pal->true_colormap + off;

/* compute the color maps */
    true_cm[dev->RED].red            = pal->max_red_intensity*gs[0];
    true_cm[dev->RED].green          = pal->max_green_intensity*gs[1];
    true_cm[dev->RED].blue           = pal->max_blue_intensity*gs[1];
    
    true_cm[dev->YELLOW].red         = pal->max_red_intensity*gs[2];
    true_cm[dev->YELLOW].green       = pal->max_green_intensity*gs[2];
    true_cm[dev->YELLOW].blue        = pal->max_blue_intensity*gs[1];
    
    true_cm[dev->GREEN].red          = pal->max_red_intensity*gs[1];
    true_cm[dev->GREEN].green        = pal->max_green_intensity*gs[0];
    true_cm[dev->GREEN].blue         = pal->max_blue_intensity*gs[1];
    
    true_cm[dev->CYAN].red           = pal->max_red_intensity*gs[1];
    true_cm[dev->CYAN].green         = pal->max_green_intensity*gs[0];
    true_cm[dev->CYAN].blue          = pal->max_blue_intensity*gs[0];
    
    true_cm[dev->BLUE].red           = pal->max_red_intensity*gs[1];
    true_cm[dev->BLUE].green         = pal->max_green_intensity*gs[1];
    true_cm[dev->BLUE].blue          = pal->max_blue_intensity*gs[0];
    
    true_cm[dev->MAGENTA].red        = pal->max_red_intensity*gs[0];
    true_cm[dev->MAGENTA].green      = pal->max_green_intensity*gs[1];
    true_cm[dev->MAGENTA].blue       = pal->max_blue_intensity*gs[0];
    
    true_cm[dev->DARK_RED].red       = pal->max_red_intensity*gs[2];
    true_cm[dev->DARK_RED].green     = pal->max_green_intensity*gs[1];
    true_cm[dev->DARK_RED].blue      = pal->max_blue_intensity*gs[1];
    
    true_cm[dev->BROWN].red          = pal->max_red_intensity*gs[2];
    true_cm[dev->BROWN].green        = pal->max_green_intensity*gs[3];
    true_cm[dev->BROWN].blue         = pal->max_blue_intensity*gs[1];
    
    true_cm[dev->DARK_GREEN].red     = pal->max_red_intensity*gs[1];
    true_cm[dev->DARK_GREEN].green   = pal->max_green_intensity*gs[2];
    true_cm[dev->DARK_GREEN].blue    = pal->max_blue_intensity*gs[1];
    
    true_cm[dev->DARK_CYAN].red      = pal->max_red_intensity*gs[1];
    true_cm[dev->DARK_CYAN].green    = pal->max_green_intensity*gs[2];
    true_cm[dev->DARK_CYAN].blue     = pal->max_blue_intensity*gs[2];
    
    true_cm[dev->DARK_BLUE].red      = pal->max_red_intensity*gs[1];
    true_cm[dev->DARK_BLUE].green    = pal->max_green_intensity*gs[1];
    true_cm[dev->DARK_BLUE].blue     = pal->max_blue_intensity*gs[2];
    
    true_cm[dev->DARK_MAGENTA].red   = pal->max_red_intensity*gs[2];
    true_cm[dev->DARK_MAGENTA].green = pal->max_green_intensity*gs[1];
    true_cm[dev->DARK_MAGENTA].blue  = pal->max_blue_intensity*gs[3];

    true_cm[dev->GRAY].red           = pal->max_red_intensity*gs[2];
    true_cm[dev->GRAY].green         = pal->max_green_intensity*gs[2];
    true_cm[dev->GRAY].blue          = pal->max_blue_intensity*gs[2];
    
    true_cm[dev->DARK_GRAY].red      = pal->max_red_intensity*gs[3];
    true_cm[dev->DARK_GRAY].green    = pal->max_green_intensity*gs[3];
    true_cm[dev->DARK_GRAY].blue     = pal->max_blue_intensity*gs[3];
    
/* these MUST be assigned last in case all dev->RED, etc have the same
 * value!!!
 */
    if (off == 0)
       {true_cm[dev->BLACK].red   = pal->max_red_intensity*gs[5];
        true_cm[dev->BLACK].green = pal->max_green_intensity*gs[5];
        true_cm[dev->BLACK].blue  = pal->max_blue_intensity*gs[5];

        true_cm[dev->WHITE].red   = pal->max_red_intensity*gs[4];
        true_cm[dev->WHITE].green = pal->max_green_intensity*gs[4];
        true_cm[dev->WHITE].blue  = pal->max_blue_intensity*gs[4];}

    pv = pal->pixel_value;
    if (pv != NULL)
       {for (i = 0; i < 16; i++)
            pv[i] = i;};

    dev->line_color = dev->WHITE;
    dev->text_color = dev->WHITE;

    if (reg)
       PG_register_palette(dev, pal, FALSE);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REGISTER_PALETTE - enter the given palette into the list of palettes
 *                     - for the given device
 *                     - match the colors of the palette to those from the
 *                     - device color table
 */

void PG_register_palette(PG_device *dev, PG_palette *pal, int map)
   {

    if ((dev != NULL) && (pal != NULL))

/* splice the palette into the ring */
       {if (dev->palettes == NULL)
	   {pal->next     = pal;
	    dev->palettes = pal;}

        else
	   {pal->next           = dev->palettes->next;
	    dev->palettes->next = pal;};

/* match the palettes to the color table */
	PG_match_rgb_colors(dev, pal);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RGB_INDEX - replace the contents of the given RGB color cell with
 *              - the nearest RGB values from the device color table
 *              - return the index from the device color table
 */

int PG_rgb_index(PG_device *dev, RGB_color_map *cm)
   {int i, ic, npc;
    double r, g, b, dc, dcmin, dr, dg, db;
    PG_palette *root;
    RGB_color_map *ct;

    r = cm->red;
    g = cm->green;
    b = cm->blue;

    root = dev->color_table;
    npc  = root->n_pal_colors;
    ct   = root->true_colormap;

    dcmin = HUGE;
    ic    = 0;
    for (i = 0; i < npc; i++)
        {dr = r - ct[i].red;
         dg = g - ct[i].green;
         db = b - ct[i].blue;
         dc = dr*dr + dg*dg + db*db;
         if (dc < dcmin)
            {ic = i;
             dcmin = dc;};};

    *cm = ct[ic];

    return(ic);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MATCH_RGB_COLORS - map the RGB references in the palette to
 *                      - valid colors from the root color table generically
 */

void _PG_match_rgb_colors(PG_device *dev, PG_palette *pal)
   {int i, npc, ic;
    unsigned long *pv;
    RGB_color_map *true_cm;

    npc     = pal->n_pal_colors;
    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

/* add two for the black and white colors at the top */
    npc += 2;
    for (i = 0; i < npc; i++)
        {ic = PG_rgb_index(dev, true_cm++);
         if (pv != NULL)
            pv[i] = ic;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_STANDARD_PALETTES - setup the standard set of palettes for
 *                            - a PGS device
 *                            - NC is the number of colors in the spectrum
 *                            - from blue to blue thru green and red
 *                            - it sets the scale for the number of colors
 *                            - in the other palettes
 */

void PG_setup_standard_palettes(PG_device *dev, int nc, int *gs)
   {int nbw, nsp, nrb, ntc;
    PG_palette *root_pal;

    if (dev != NULL)

/* build up the device color table */
       {nbw = 0.5*nc;
	nsp = nc;
	nrb = 1.3125*nc;
	ntc = nbw + nsp + nrb + 14;
	root_pal = PG_mk_palette(dev, "Root", ntc);

	_PG_bw_palette(dev, root_pal, 0, nbw);
	_PG_spectrum_palette(dev, root_pal, nbw, nsp);
	_PG_rainbow_palette(dev, root_pal, nbw+nsp, nrb);
	_PG_standard_palette(dev, root_pal, nbw+nsp+nrb, 14, gs);

	PG_map_to_color_table(dev, root_pal);

	dev->color_table = root_pal;

/* make and register the standard palettes */
	dev->current_palette = _PG_standard_palette(dev, NULL, 0, 14, gs);

	_PG_spectrum_palette(dev, (PG_palette *) NULL, 0, nsp);
	_PG_clr_contrast_palette(dev, (PG_palette *) NULL, 0, nsp);
	_PG_rainbow_palette(dev, (PG_palette *) NULL, 0, nrb);
	_PG_2drainbow_palette(dev, (PG_palette *) NULL, 0, nrb);
	_PG_2dctab1_palette(dev, (PG_palette *) NULL, 0);
	_PG_bw_palette(dev, (PG_palette *) NULL, 0, nbw);
	_PG_wb_palette(dev, (PG_palette *) NULL, nbw);
	_PG_rgb_palette(dev, (PG_palette *) NULL, nsp);
	_PG_cym_palette(dev, (PG_palette *) NULL, nsp);
	_PG_bgy_palette(dev, (PG_palette *) NULL, nsp);
	_PG_tri_palette(dev, (PG_palette *) NULL, nbw);
	_PG_iron_palette(dev, (PG_palette *) NULL, nbw);
	_PG_hc_palette(dev, (PG_palette *) NULL, nbw);
	_PG_thresh_palette(dev, (PG_palette *) NULL, nbw);
	_PG_rand_palette(dev, (PG_palette *) NULL);
	_PG_mono_palettes(dev, nrb);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SELECT_COLOR - given an array of N values return the color index
 *                 - from the N dimensional palette
 */

int PG_select_color(PG_device *dev, int n, double *a, double *extr)
   {int i, color, prod, *shape;
    int **pd;
    double da, vl, av, an, ax, *xt, cf;
    PG_palette *pal;

    pal = dev->current_palette;
    pd  = pal->pal_dims;
    if ((pd == NULL) || (pal->max_pal_dims < n-1))
       {xt = extr;
        vl = 0.0;
        cf = ((double) pal->n_pal_colors - 1.0)/((double) n);
        for (i = 0; i < n; i++)
            {av = a[i];
             an = xt[0];
             ax = xt[1];

             if (_PG_gattrs.color_map_to_extrema == TRUE)
                {if (av < an)
                    av = an;
                 else if (av > ax)
                    av = ax;}

             if ((an <= av) && (av <= ax))
                {da  = (av - an)/(ax - an + SMALL);
                 vl += da;}
             else if (av < an)
                return(0);
             else if (ax < av)
                return(1);

             xt += 2;};

        color = vl*cf + 2.0 + PGS_TOL;}
       
    else
       {shape = pal->pal_dims[n-1];
        color = 2;
        xt    = extr;
        prod  = 1;
        for (i = 0; i < n; i++)
            {av = a[i];
             an = xt[0];
             ax = xt[1];

             if (_PG_gattrs.color_map_to_extrema == TRUE)
                {if (av < an)
                    av = an;
                 else if (av > ax)
                    av = ax;}

             if ((an <= av) && (av <= ax))
                {da     = (shape[i] - 1)/(fabs(ax - an) + SMALL);
                 color += (int)(fabs(av - an)*da + 0.49999) * prod;
                 prod  *= shape[i];}
             else
                return(dev->WHITE);

             xt += 2;};};

    return(color);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_PALETTE - return the palette of DEV specified by NAME
 *                 - if NAME is NULL return current palette
 *
 * #bind PG_fget_palette fortran() scheme() python()
 */

PG_palette *PG_fget_palette(PG_device *dev ARG(,,cls), const char *name)
   {PG_palette *pal;

    if (name == NULL)
       pal = dev->current_palette;

    else
       {pal = dev->palettes;
        while (TRUE)
           {if (strcmp(name, pal->name) == 0)
               break;

            pal = pal->next;
            if (pal == dev->palettes)
               {pal = NULL;
                break;};};};

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_PALETTE - set the color palette of the given device
 *                 - return the named palette if successful
 *                 - return NULL otherwise
 *
 * #bind PG_fset_palette fortran() scheme() python()
 */

PG_palette *PG_fset_palette(PG_device *dev ARG(,,cls), const char *name)
   {PG_palette *pal;

    pal = dev->palettes;
    if ((name != NULL) && (pal != NULL))
       {while (TRUE)
           {if (strcmp(name, pal->name) == 0)
               {dev->current_palette = pal;
                break;};

            pal = pal->next;
            if (pal == dev->palettes)
               {pal = NULL;
                break;};};};

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RGB_HSV - convert an RGB color-map to an HSV colormap */

static void PG_rgb_hsv(RGB_color_map *cm, int nc, int flg)
   {int i;
    double r, g, b;
    double h, s, v;
    double d, rc, gc, bc, norm, mlt;

    mlt = flg ? MAXPIX : 1.0;

    norm = 1.0/MAXPIX;
    for (i = 0; i < nc; i++)
        {r = norm*cm[i].red;
         g = norm*cm[i].green;
         b = norm*cm[i].blue;

         v = max(r, g);
         v = max(v, b);

         s = min(r, g);
         s = min(s, b);

         d = v - s;
         s = (v != 0) ? d/v : 0;

	 h = 0.0;
         if (s != 0.0)
            {d  = 1.0/d;
             rc = (v - r)*d;
             gc = (v - g)*d;
             bc = (v - b)*d;
             if (r == v)
                h = 2.0 + bc - gc;
             else if (g == v)
                h = 4.0 + rc - bc;
             else if (b == v)
                h = 6.0 + gc - rc;};

         h /= 7.0;

         cm[i].red   = mlt*h;
         cm[i].green = mlt*s;
         cm[i].blue  = mlt*v;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HSV_RGB - convert an HSV color-map to an RGB colormap */

static void PG_hsv_rgb(RGB_color_map *cm, int nc, int flg)
   {int i, j;
    double h, s, v;
    double f, p, q, t, norm, mlt;

    mlt  = flg ? 1.0 : MAXPIX;
    norm = flg ? 1.0/MAXPIX : 1.0;
    for (i = 0; i < nc; i++)
        {h = cm[i].red;
         s = norm*cm[i].green;
         v = mlt*cm[i].blue;

         h *= 7.0*norm;
         if (s == 0.0)
            {v = (h < 0.999999) ? v : 0.0;

             cm[i].red   = v;
             cm[i].green = v;
             cm[i].blue  = v;}

         else
            {j = floor(h);
             f = h - j;
             p = v*(1.0 - s);
             q = v*(1.0 - s*f);
             t = v*(1.0 - s*(1.0 - f));
             switch (j)

/* magenta */
                {case 1 :
                      cm[i].red   = v;
                      cm[i].green = p;
                      cm[i].blue  = q;
                      break;

/* red */
                 case 2 :
                      cm[i].red   = v;
                      cm[i].green = t;
                      cm[i].blue  = p;
                      break;

/* yellow */
                 case 3 :
                      cm[i].red   = q;
                      cm[i].green = v;
                      cm[i].blue  = p;
                      break;

/* green */
                 case 4 :
                      cm[i].red   = p;
                      cm[i].green = v;
                      cm[i].blue  = t;
                      break;

/* cyan */
                 case 5 :
                      cm[i].red   = p;
                      cm[i].green = q;
                      cm[i].blue  = v;
                      break;

/* blue */
                 case 6 :
                      cm[i].red   = t;
                      cm[i].green = p;
                      cm[i].blue  = v;
                      break;};};};

    return;}

/*--------------------------------------------------------------------------*/

/*                        PALETTE DISPLAY ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* PG_SHOW_COLORMAP - display the colormap for the given device */

void PG_show_colormap(const char *type, int wbck)
   {int k, l, n_pal_colors, color, ns, mapped;
    double dx, dy;
    double wc[PG_BOXSZ], x[5], y[5];
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double x3[PG_SPACEDM], x4[PG_SPACEDM];
    PG_device *dev;

    dev = PG_make_device(type, "COLOR", "Device Colormap");
    if (dev != NULL)
       {PG_white_background(dev, wbck);
	PG_open_device(dev, 0.05, 0.1, 0.4, 0.4);

	PG_set_viewspace(dev, 2, NORMC, NULL);
	PG_set_viewspace(dev, 2, WORLDC, NULL);

	PG_make_device_current(dev);
        
	mapped = TRUE;
	n_pal_colors = dev->current_palette->n_dev_colors;

	ns = ceil(sqrt((double) n_pal_colors));

	PG_get_viewspace(dev, WORLDC, wc);
	PG_draw_box_n(dev, 2, WORLDC, wc);

	dx = (wc[1] - wc[0])/ns;
	dy = (wc[3] - wc[2])/ns;

	color = 0;
	y[0]  = wc[2];
	for (k = 0; (k < ns) && (color < n_pal_colors); k++)
	    {x[0] = wc[0];
	     for (l = 0; (l < ns) && (color < n_pal_colors); l++)
	         {x[1] = x[0] + dx;
		  y[1] = y[0];
		  x[2] = x[1];
		  y[2] = y[1] + dy;
		  x[3] = x[2] - dx;
		  y[3] = y[2];
		  x[4] = x[0];
		  y[4] = y[0];

		  PG_fill_polygon(dev, color++, mapped, x, y, 5);

		  x[0] += dx;};

	     y[0] += dy;};

	PG_fset_line_color(dev, dev->WHITE, TRUE);

	ns++;
	x1[0] = wc[0];
	x1[1] = wc[2];
	x2[0] = wc[0]; 
	x2[1] = wc[3];
	x3[0] = wc[1];
	x3[1] = wc[2];
	x4[0] = wc[1];
	x4[1] = wc[3];
	PG_draw_multiple_line(dev, ns, x1, x2, x3, x4, NULL);

	x1[0] = wc[0];
	x1[1] = wc[2];
	x2[0] = wc[1]; 
	x2[1] = wc[2];
	x3[0] = wc[0];
	x3[1] = wc[3];
	x4[0] = wc[1];
	x4[1] = wc[3];
	PG_draw_multiple_line(dev, ns, x1, x2, x3, x4, NULL);

	PG_update_vs(dev);

	SC_pause();

	PG_close_device(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DUMP_COLORMAP - dump the colormap for the given device */

void PG_dump_colormap(const char *type, const char *file)
   {int k, ntc, nc, nbw, nsp;
    double r, g, b;
    PG_palette *root_pal;
    RGB_color_map *cm;
    PG_device *dev;
    FILE *fp;

    fp = _PG_fopen(file, "w");

    dev = PG_make_device("PS", "COLOR", "dump-tmp");
    if (dev != NULL)
       {PG_open_device(dev, 0.0, 1.0, 1.0, 1.0);

	root_pal = dev->color_table;
	cm       = root_pal->true_colormap;
	ntc      = root_pal->n_pal_colors;
	nc       = (int) (((double) ntc - 14.0)/2.8125);

/* build up the device color table */
	nbw = 0.5*nc;
	nsp = nc;

	PRINT(fp, "            Red     Green    Blue\n");
	for (k = 0; k < ntc; k++)
	    {r = cm[k].red/MAXPIX;
	     g = cm[k].green/MAXPIX;
	     b = cm[k].blue/MAXPIX;
	     if (k == 2)
	        PRINT(fp, "Black and White\n");
	     if (k == nbw+2)
	        PRINT(fp, "Spectrum\n");
	     if (k == nsp+nbw+2)
	        PRINT(fp, "Rainbow\n");
	     PRINT(fp, "          %5.4f   %5.4f   %5.4f\n", r, g, b);};

	PG_close_device(dev);};

    io_close(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PAL_EV_HAND - handle the show palette buttons */

static void _PG_pal_ev_hand(void *d, PG_event *ev)
   {

    if (ev != NULL)
       {PG_interface_object *iob;
        PG_text_box *b;

        _PG_gcont.input_bf[1] = '\0';

        iob  = (PG_interface_object *) d;
        if (strcmp(iob->type, PG_BUTTON_OBJECT_S) == 0)
           _PG_gcont.input_bf[0] = iob->action_name[0];

        else if (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0)
           {b = (PG_text_box *) iob->obj;
            _PG_gcont.input_bf[0] = b->text_buffer[0][0];};

        LONGJMP(_PG.io_avail, ERR_FREE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PAL_MOUSE_EV_HAND - handle mouse events for palette selection */

static void _PG_pal_mouse_ev_hand(PG_device *dev, PG_event *ev)
   {

#ifdef HAVE_WINDOW_DEVICE

    int mod;
    int iev[PG_SPACEDM];
    double x[PG_SPACEDM];
    PG_mouse_button btn;

    PG_mouse_event_info(dev, ev, iev, &btn, &mod);

    x[0] = iev[0];
    x[1] = iev[1];
    PG_trans_point(dev, 2, PIXELC, x, WORLDC, x);

    _PG_gcont.input_bf[0] = 10.0*x[1] - 0.5;

#else

    _PG_gcont.input_bf[0] = 0;

#endif

    LONGJMP(_PG.io_avail, ERR_FREE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SHOW_PALETTES - display the palettes for the given device
 *
 * #bind PG_show_palettes fortran() scheme(pg-show-palettes) python()
 */

void PG_show_palettes(PG_device *sdev ARG(,,cls), const char *type, int wbck)
   {int i, j, k, np, page;
    double y, dy, z, old, wd;
    double obx[PG_BOXSZ], rbx[PG_BOXSZ], p[PG_SPACEDM];
    char bf[MAXLINE];
    PG_device *dev;
    PG_palette *std_pal, *pal, **pals;
    SC_array *pa;

    if (strcmp(type, "WINDOW") != 0)
       {dev = PG_make_device(type, "COLOR", "palettes");
        PG_white_background(dev, wbck);
        PG_open_device(dev, 0.01, 0.01, 0.98, 0.98);
        PG_set_viewspace(dev, 2, NORMC, NULL);
        PG_set_viewspace(dev, 2, WORLDC, NULL);}

    else
       {dev = PG_make_device(type, "COLOR", "Color Palettes");
        PG_white_background(dev, wbck);
        PG_open_device(dev, 0.05, 0.1, 0.4, 0.4);

        PG_set_viewspace(dev, 2, NORMC, NULL);
        PG_set_viewspace(dev, 2, WORLDC, NULL);

        PG_set_mouse_down_event_handler(dev, _PG_pal_mouse_ev_hand);

	obx[0] = 0.1;
	obx[1] = 0.24;
	obx[2] = 0.93;
	obx[3] = 0.97;
        PG_add_button(dev, obx, "Forward", _PG_pal_ev_hand);

	obx[0] = 0.43;
	obx[1] = 0.57;
	obx[2] = 0.93;
	obx[3] = 0.97;
        PG_add_button(dev, obx, "Backward", _PG_pal_ev_hand);

	obx[0] = 0.76;
	obx[1] = 0.9;
	obx[2] = 0.93;
	obx[3] = 0.97;
        PG_add_button(dev, obx, "Close", _PG_pal_ev_hand);};

    if (dev == NULL)
       return;

    if (sdev == NULL)
       sdev = dev;

    PG_make_device_current(dev);

    old = _PG_gattrs.axis_label_y_standoff;
    _PG_gattrs.axis_label_y_standoff = 0.5;

/* count the palettes and make an array of them */
    pa = CMAKE_ARRAY(PG_palette *, NULL, 0);
    for (pal = sdev->palettes; TRUE; )
        {SC_array_push(pa, &pal);
         pal = pal->next;
         if (pal == sdev->palettes)
            break;};

    np   = SC_array_get_n(pa);
    pals = SC_array_done(pa);

/* draw the palettes */
    dy = 0.1;
    wd = _PG_gattrs.palette_width;

    std_pal = dev->current_palette;
    for (page = 0; page < np; )
        {PG_clear_window(dev);
         if (!HARDCOPY_DEVICE(dev))
            {dev->current_palette = std_pal;
             PG_fset_font(dev, "helvetica", "medium", 12);
             PG_draw_interface_objects(dev);};

         for (i = 0, y = dy; i < 8; i++, y += dy)
             {j = i + page;
              if (j >= np)
                 break;

              pal = pals[j];
              z   = pal->n_pal_colors;

              dev->current_palette = std_pal;
              PG_fset_line_color(dev, dev->WHITE, TRUE);
              PG_fset_font(dev, "helvetica", "medium", 12);

	      p[0] = 0.05;
	      p[1] = y;
              PG_write_n(dev, 2, WORLDC, p, pal->name);

              dev->current_palette = pal;
              PG_fset_font(dev, "helvetica", "medium", 8);

              obx[0] = 0.2;
	      obx[1] = 0.9;
	      obx[2] = y;
	      obx[3] = y;
	      rbx[0] = 0.0;
	      rbx[1] = z;
              PG_draw_palette_n(dev, obx, rbx, wd, TRUE);};

         PG_finish_plot(dev);

         if (!HARDCOPY_DEVICE(dev))
            {if (GETLN(bf, MAXLINE, stdin) == NULL)
                break;

             k = bf[0];
             switch (k)
                {default :
                      if ((0 <= k) && (k < 8) && ((page + k) < np))
                         sdev->current_palette = pals[page + k];
                      else
                         break;

                 case 'c' :
                 case 'C' :
                      CFREE(pals);
                      PG_close_device(dev);
                      _PG_gattrs.axis_label_y_standoff = old;
                      return;

                 case 'f' :
                 case 'F' :
                      page += 8;
                      page = (page < np) ? page : page - 8;
                      break;

                 case 'b' :
                 case 'B' :
                      page -= 8;
                      page = (page < 0) ? page + 8 : page;
                      break;};}

         else
            page += 8;};

    CFREE(pals);
    PG_close_device(dev);
    _PG_gattrs.axis_label_y_standoff = old;

    return;}

/*--------------------------------------------------------------------------*/

/*                      PALETTE READ/WRITE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* PG_RD_PALETTE - read a palette from a file and install it in the
 *               - specified device
 */

PG_palette *PG_rd_palette(PG_device *dev, const char *fname)
   {int i, ns, nc, ncx, ndims, dims[2];
    double mxr, mxg, mxb, r, g, b;
    char bf[MAXLINE], *name, *s, *nd;
    RGB_color_map *true_cm;
    PG_palette *pal;
    FILE *fp;

    fp = _PG_fopen(fname, "r");
    if (fp == NULL)
       return(NULL);

    if (io_gets(bf, MAXLINE, fp) == NULL)
       return(NULL);
    
    ndims = 0;
    name  = SC_strtok(bf, " \t", s);
    nc    = SC_stoi(SC_strtok(NULL, " \t\r\n", s));
    nd    = SC_strtok(NULL," \t\r\n", s);
    if (nd != NULL)
       {ndims   = SC_stoi(nd);
        dims[0] = SC_stoi(SC_strtok(NULL, " \t\r\n", s));
        dims[1] = SC_stoi(SC_strtok(NULL, " \t\r\n", s));} 
    
    ncx  = 1 << 24;
    nc   = min(nc, ncx);
    pal  = PG_mk_palette(dev, name, nc);

    true_cm = pal->true_colormap + 2;

    ns  = MAXPIX;
    mxr = ns*dev->max_red_intensity;
    mxg = ns*dev->max_green_intensity;
    mxb = ns*dev->max_blue_intensity;

    for (i = 0; i < nc; i++)
        {if (io_gets(bf, MAXLINE, fp) == NULL)
            {PG_rl_palette(pal);
             pal = NULL;
             break;};

         r = SC_stof(SC_strtok(bf, " \t\r\n", s));
         g = SC_stof(SC_strtok(NULL, " \t\r\n", s));
         b = SC_stof(SC_strtok(NULL, " \t\r\n", s));

         true_cm[i].red   = r*mxr;
         true_cm[i].green = g*mxg;
         true_cm[i].blue  = b*mxb;};

    io_close(fp);
    if (pal != NULL)
       {if ((0 < ndims) && (ndims <= 2))
           _PG_attach_palette_dims(pal, nc, ndims, dims);
        PG_register_palette(dev, pal, FALSE);}

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WR_PALETTE - write a palette to an ASCII file */

int PG_wr_palette(PG_device *dev, PG_palette *pal, const char *fname)
   {int i, ns, nc, ndims, *dims;
    double mxr, mxg, mxb, r, g, b;
    RGB_color_map *true_cm;
    FILE *fp;

    fp = _PG_fopen(fname, "w");
    if (fp == NULL)
       return(FALSE);

    nc      = pal->n_pal_colors;
    true_cm = pal->true_colormap + 2;
    ndims   = pal->max_pal_dims;

    ns  = MAXPIX;
    mxr = 1.0/(ns*dev->max_red_intensity);
    mxg = 1.0/(ns*dev->max_green_intensity);
    mxb = 1.0/(ns*dev->max_blue_intensity);

    if ((pal->pal_dims == NULL) || (ndims != 2))
       io_printf(fp, "%s %d\n", pal->name, nc);
    else
       {dims = pal->pal_dims[ndims-1];
        io_printf(fp, "%s %d %d %d %d\n", pal->name,
                  nc, ndims, dims[0], dims[1]);}

    for (i = 0; i < nc; i++)
        {r = mxr*true_cm[i].red;
         g = mxg*true_cm[i].green;
         b = mxb*true_cm[i].blue;

         io_printf(fp, "%.4f  %.4f  %.4f\n", r, g, b);}

    io_close(fp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                      PALETTE GENERATION ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* PG_MAKE_PALETTE - make a new 1-dimensional palette for the given
 *                   device graphically
 */

PG_palette *PG_make_palette(PG_device *tdev, const char *name,
			    int nclr, int wbck)
   {PG_palette *pal;

    pal = PG_make_ndim_palette(tdev, name, 1, &nclr, wbck);

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ADD_SELECTED_COLOR - book keeping for one additional color
 *                        - return the index of the newly selected color
 */

static int _PG_add_selected_color(PG_device *dev, double *h,
                                  RGB_color_map *cm, int nc, int *ncl,
				  int ipc, int npc,
                                  double *bx, double *dxw, double *dx)
   {int i, ic;
    double s, dh, ds, dv, dc, dcmin, x0, dx0, dybox, thrs;
    double x[5], y[5];

    thrs = 1.0/7.0 - 1.0e-6;

/* if a selection from the arrays of available colors has been made */
    h[0] = (h[0] - bx[0])/dxw[0];
    if (h[0] > thrs)
       {s = 1.0;
        h[1] = 2.0*(h[1] - bx[2])/dxw[1];
        if (h[1] > 1.0)
           {s = 2.0 - h[1];
            h[1] = 1.0;};}

    else
      {h[0] = 0.0;
       h[1] = (h[1] - bx[2])/(dxw[1] - dx[1] - 0.005);
       s    = 0.0;};

    dcmin = HUGE;
    ic    = 0;
    for (i = 0; i < nc; i++)
        {dh = h[0] - cm[i].red;
         ds = s - cm[i].green;
         dv = h[1] - cm[i].blue;
         dc = dh*dh + ds*ds + dv*dv;
         if (dc < dcmin)
            {ic = i;
             dcmin = dc;};};

    dx0   = 0.45*dxw[0]/((double) ncl[0]);
    x0    = bx[0] + 0.45*(dxw[0] - ncl[0]*dx0);
    dybox = (ncl[1] > 4) ? (.98 - (bx[3] + dx[1]))/ncl[1] : dx[1];

    x[0] = x0 + (ipc % ncl[0]) *dx0;
    y[0] = bx[3] + 0.03*dxw[1] + (ipc/ncl[0]) * dybox;
    x[1] = x[0] + dx0;
    y[1] = y[0];
    x[2] = x[1];
    y[2] = y[1] + dybox;
    x[3] = x[2] - dx0;
    y[3] = y[2];
    x[4] = x[0];
    y[4] = y[0];

    PG_fill_polygon(dev, ic, TRUE, x, y, 5);

    return(ic);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SELECT_COLORS - pick colors with the mouse */

static void _PG_select_colors(PG_device *dev, PG_palette *npal,
                              int nc, int *ncl, RGB_color_map *cm)
   {int btn, mod, ipc, npc, ic;
    int reset, row, col;
    int ix[PG_SPACEDM];
    double dx[PG_SPACEDM], dxw[PG_SPACEDM], dxb[PG_SPACEDM];
    double h[PG_SPACEDM], bx[PG_BOXSZ], nbx[PG_BOXSZ];
    RGB_color_map *pcm;

    npc = npal->n_pal_colors;
    pcm = npal->true_colormap + 2;
    ipc = 0;

    bx[0] = 0.1;
    bx[1] = 0.9;
    bx[2] = 0.1;
    bx[3] = 0.85;

    dxw[0] = (bx[1] - bx[0]);
    dxw[1] = (bx[3] - bx[2]);
    dx[0]  = 0.03*dxw[0];
    dx[1]  = 0.03*dxw[1];
    dxb[0] = 0.45*dxw[0]/((double) ncl[0]);
    dxb[1] = (ncl[1] > 4) ? (.98 - (bx[3] + dx[1]))/ncl[1] : dx[1];
    nbx[0] = bx[0] + 0.45*(dxw[0] - ncl[0]*dxb[0]);
    nbx[2] = bx[3] + 0.03*dxw[1];
    nbx[1] = nbx[0] + ncl[0]*dxb[0];
    nbx[3] = nbx[2] + ncl[1]*dxb[1];

    reset = TRUE;
    while (TRUE)
       {if ((ipc < 0) || (ipc >= npc))
           ipc = 0;

        PG_query_pointer(dev, ix, &btn, &mod);
        if (btn && reset)
           {switch (btn)
               {case MOUSE_LEFT :
		     h[0] = ix[0];
		     h[1] = ix[1];
		     PG_trans_point(dev, 2, PIXELC, h, WORLDC, h);

                     if (PG_box_contains(2, bx, h) == TRUE)
                        {ic = _PG_add_selected_color(dev, h, cm, nc,
                                                     ncl, ipc, npc,
                                                     bx, dxw, dx);
                         pcm[ipc++] = cm[ic];
                         reset = FALSE;}

/* if a cell in the new palette has been indicated */
                     else if (PG_box_contains(2, nbx, h) == TRUE)
                        {row = (int) ((h[1] - nbx[2])/dxb[1]);
                         col = (int) ((h[0] - nbx[0])/dxb[0]);
                         ipc = row * ncl[0] + col;}
                     break;

                case MOUSE_MIDDLE :
                     break;

                case MOUSE_RIGHT :
                     PG_hsv_rgb(pcm, npc, FALSE);
                     return;};}
        else
           reset = !btn;};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_NDIM_PALETTE - make a new n-dimensional palette for the given
 *                        device graphically 
 */

PG_palette *PG_make_ndim_palette(PG_device *tdev, const char *name,
                                 int ndims, int *dims, int wbck)
   {int i, k, l, nc, color, nclr;
    int ncl[PG_SPACEDM];
    double h, s, v, thrs, x0, dx0, dybox;
    double tn[2], vw[2];
    double wc[PG_BOXSZ], bx[PG_BOXSZ], x[5], y[5];
    double dx[PG_SPACEDM], dxw[PG_SPACEDM];
    double xl[PG_SPACEDM], xr[PG_SPACEDM], p[PG_SPACEDM];
    RGB_color_map *cm, *pcm, *scm;
    PG_device *dev;
    PG_palette *root, *npal;

    memset(x, 0, sizeof(x));
    memset(y, 0, sizeof(y));

    if (name == NULL)
       name = CSTRSAVE("new");

/* dimensionality greater than 2 not currently supported */
    if (ndims > 2)
       ndims = 2;

    if (ndims < 1)
       {ndims = 1;
        nclr  = 16;
        ncl[0] = 16;
        ncl[1] = 1;}
    else
       {nclr = dims[0];
        for (i = 1; i < ndims; i++)
            nclr *= dims[i];
        ncl[0] = dims[0];
        ncl[1] = (ndims > 1) ? dims[1] : 1;}
    
    npal = NULL;
    dev  = PG_make_device("WINDOW", "COLOR", "Color Selection");
    if (dev != NULL)
       {PG_white_background(dev, wbck);
	PG_open_device(dev, 0.05, 0.1, 0.4, 0.6);

	PG_set_viewspace(dev, 2, NORMC, NULL);
	PG_set_viewspace(dev, 2, WORLDC, NULL);

/* copy the device color table and convert to HSV */
	root = dev->color_table;
	scm  = root->true_colormap;
	nc   = root->n_pal_colors + 2;

	cm  = CMAKE_N(RGB_color_map, nc);
	pcm = cm;
	for (l = 0; l < nc; l++)
	    *pcm++ = *scm++;

	PG_rgb_hsv(cm, nc, FALSE);

/* draw the available colors in HSV space */
	PG_make_device_current(dev);

	PG_get_viewspace(dev, WORLDC, wc);
	PG_draw_box_n(dev, 2, WORLDC, wc);

	wc[0] = 0.1;
	wc[1] = 0.9;
	wc[2] = 0.1;
	wc[3] = 0.85;

	dxw[0] = (wc[1] - wc[0]);
	dxw[1] = (wc[3] - wc[2]);
	dx[0]  = 0.03*dxw[0];
	dx[1]  = 0.03*dxw[1];

	thrs = 1.0/7.0 - 1.0e-6;

/* draw the axes and such */
	PG_fset_line_color(dev, dev->WHITE, TRUE);
	PG_draw_box_n(dev, 2, WORLDC, wc);

/* saturation axis */
	xl[0] = wc[0];
	xl[1] = wc[2] + 0.5*dxw[1];
	xr[0] = wc[0];
	xr[1] = wc[3];
	tn[0] = 0.0;
	tn[1] = -1.0;
	vw[0] = 1.0;
	vw[1] = 0.5;
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0, "%3.1f",
		       AXIS_TICK_LEFT, AXIS_TICK_ENDS,
		       FALSE, AXIS_TICK_MAJOR | AXIS_TICK_MINOR, 0);

	p[0] = wc[0] - 0.1*dxw[0];
	p[1] = wc[3] - 0.5*dxw[1] - 0.02*dxw[1];
	PG_write_n(dev, 2, WORLDC, p, "1.0");

	p[0] = wc[0] - 0.1*dxw[0];
	p[1] = wc[3] - 0.02*dxw[1];
	PG_write_n(dev, 2, WORLDC, p, "0.5");

	p[0] = wc[0] - 0.08*dxw[0];
	p[1] = wc[2] + 0.73*dxw[1];
	PG_write_n(dev, 2, WORLDC, p, "S");

/* value axis */
	xl[0] = wc[0];
	xl[1] = wc[2];
	xr[0] = wc[0];
	xr[1] = wc[2] + 0.5*dxw[1];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = 0.5;
	vw[1] = 1.0;
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0, "%3.1f",
		       AXIS_TICK_LEFT, AXIS_TICK_ENDS,
		       FALSE, AXIS_TICK_MAJOR | AXIS_TICK_MINOR, 0);

	p[0] = wc[0] - 0.1*dxw[0];
	p[1] = wc[2] + 0.5*dxw[1] - 0.02*dxw[1];
	PG_write_n(dev, 2, WORLDC, p, "1.0");

	p[0] = wc[0] - 0.1*dxw[0];
	p[1] = wc[2] - 0.02*dxw[1];
	PG_write_n(dev, 2, WORLDC, p, "0.5");

	p[0] = wc[0] - 0.08*dxw[0];
	p[1] = wc[2] + 0.23*dxw[1];
	PG_write_n(dev, 2, WORLDC, p, "V");

/* hue axis */
	xl[0] = wc[0];
	xl[1] = wc[2];
	xr[0] = wc[1];
	xr[1] = wc[2];
	tn[0] = 0.03;
	tn[1] = 0.97;
	vw[0] = 0.0;
	vw[1] = 7.0;
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0, "%3.1f",
		       AXIS_TICK_RIGHT, AXIS_TICK_RIGHT,
		       FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | AXIS_TICK_LABEL,
		       0);

	p[0] = wc[0] + 0.49*dxw[0];
	p[1] = wc[2] - 0.06*dxw[1];
	PG_write_n(dev, 2, WORLDC, p, "H");

/* draw the boxes for the new colors */
	dx0   = 0.45*dxw[0]/((double) ncl[0]);
	x0    = wc[0] + 0.45*(dxw[0] - ncl[0]*dx0);
	dybox = (ncl[1] > 4) ? (.98 - (wc[3] + dx[1]))/ncl[1] : dx[1];
	for (k = 0; k < ncl[1]; k++)
	    for (l = 0; l < ncl[0]; l++)
	        {bx[0] = x0 + l*dx0;
		 bx[1] = x[0] + dx0;
		 bx[2] = wc[3] + 0.03*dxw[1] + k*dybox;
		 bx[3] = y[0] + dybox;
		 PG_draw_box_n(dev, 2, WORLDC, bx);}

/* draw the available colors */
	dev->current_palette = root;

	color = 0;
	for (l = 0; l < nc; l++)
	    {h = cm[color].red;
	     s = cm[color].green;
	     v = cm[color].blue;

	     x[0] = wc[0] + 0.935*dxw[0]*(h + 0.02);

	     if (h > thrs)
	        {if (s < 0.99)
		    y[0] = wc[3] - 0.5*dxw[1]*s;
		 else
		    y[0] = wc[2] + dxw[1]*v - 0.39;}
	     else
	        y[0] = wc[2] + (dxw[1] - dx[1] - 0.005)*v;

	     x[1] = x[0] + dx[0];
	     y[1] = y[0];
	     x[2] = x[1];
	     y[2] = y[1] + dx[1];
	     x[3] = x[2] - dx[0];
	     y[3] = y[2];
	     x[4] = x[0];
	     y[4] = y[0];

	     color++;
	     PG_fill_polygon(dev, l, TRUE, x, y, 5);};

	PG_update_vs(dev);

/* make the new palette */
	npal = PG_mk_palette(dev, name, nclr);
	if (npal != NULL)
	   {_PG_select_colors(dev, npal, nc, ncl, cm);

	    if (tdev != NULL)
	       {if (ndims > 1)
		   _PG_attach_palette_dims(npal, nclr, ndims, ncl);
		PG_register_palette(tdev, npal, FALSE);}
	    else
	       PG_rl_palette(npal);};

	CFREE(cm);

	PG_close_device(dev);};

    return(npal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RGB_COLOR - return RGB color components of LC in CLR */

void PG_rgb_color(RGB_color_map *clr, int lc, int rgb,
		  PG_palette *pal)
   {int nc;
    unsigned long *pv;
    double scale;
    RGB_color_map *true_cm;

    nc      = pal->n_pal_colors;
    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

    clr->red   = 0.0;
    clr->green = 0.0;
    clr->blue  = 0.0;

    if ((0 <= lc) && (lc <= nc+1))
       {if (rgb == TRUE)
	   {scale = 255.0/((double) MAXPIX);

	    *clr        = true_cm[lc];
	    clr->red   *= scale;
	    clr->green *= scale;
	    clr->blue  *= scale;}

        else
	   {scale = ((double) nc)/((double) MAXPIX);

	    clr->red = (pv != NULL) ? pv[lc] : (unsigned long) lc;};};

    return;}

/*--------------------------------------------------------------------------*/

/*                           DRAWING COLOR ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* PG_GET_COLOR_COMPONENTS - get the RGB color components of C */

RGB_color_map PG_get_color_components(PG_device *dev, int c)
    {double r, g, b, mi, sf;
     RGB_color_map rv;

     mi = dev->max_intensity*MAXPIX;
     sf = mi/255.0;

     r = (c & 0x00ff0000) >> 16;
     g = (c & 0x0000ff00) >> 8;
     b = (c & 0x000000ff);

     rv.red   = r*sf;
     rv.green = g*sf;
     rv.blue  = b*sf;

     return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_TRANS_COLOR - shift the color to some more appropriate value
 *                 - the background is logically BLACK
 *                 - regardless of the screen color chosen
 */
 
int _PG_trans_color(PG_device *dev, int color)
   {int c, nc, clr;
    PG_palette *pal;
    RGB_color_map cm;

    if (dev == NULL)
       clr = color;

    else if (color == dev->BLACK)
       clr = dev->WHITE;

    else
       {if (PG_is_true_color(color) == TRUE)
	   {cm  = PG_get_color_components(dev, color);
	    clr = PG_rgb_index(dev, &cm);}

	else
	   {pal = dev->current_palette;
	    if (pal != NULL)
	       {nc = min(pal->n_pal_colors, dev->ncolor);
		c = (color - 1) % (nc + 1) + 1;}
	    else
	       c = (color - 1) % (dev->ncolor + 1) + 1;

	    if ((c == dev->BLACK) || (c < 0) || (dev->ncolor <= c))
	       clr = dev->WHITE;
	    else
	       clr = c;};};

    return(clr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_COLOR_MAP - setup the color members of DEV
 *              - MONO is TRUE only for black and white
 *              - FIX is TRUE only when the multi-color layout
 *              - is independent of dev->background_color_white
 *              - Note: about BLACK and WHITE
 *              -   dev->WHITE is the logical background color
 *              -   dev->BLACK is the opposite of dev->WHITE
 *              -   BLACK is what the eye sees as black
 *              -   WHITE is what the eye sees as white
 *              - the confusion comes in that for some devices
 *              - the pixel whose value is 0 shows as BLACK (X window)
 *              - whereas for other devices the pixel whose value
 *              - is 0 shows as WHITE (PS)
 */

void PG_color_map(PG_device *dev, int mono, int fix)
   {

    if (dev->background_color_white == TRUE)
       {if (mono == TRUE)
	   {dev->BLACK        = WHITE;
	    dev->WHITE        = BLACK;
	    dev->GRAY         = BLACK;
	    dev->DARK_GRAY    = BLACK;
	    dev->BLUE         = BLACK;
	    dev->GREEN        = BLACK;
	    dev->CYAN         = BLACK;
	    dev->RED          = BLACK;
	    dev->MAGENTA      = BLACK;
	    dev->BROWN        = BLACK;
	    dev->DARK_BLUE    = BLACK;
	    dev->DARK_GREEN   = BLACK;
	    dev->DARK_CYAN    = BLACK;
	    dev->DARK_RED     = BLACK;
	    dev->YELLOW       = BLACK;
	    dev->DARK_MAGENTA = BLACK;
	    dev->ncolor       = 2;}
	else
	   {dev->BLACK        = BLACK;
	    dev->WHITE        = WHITE;
	    dev->GRAY         = GRAY;
	    dev->DARK_GRAY    = DARK_GRAY;
	    dev->BLUE         = BLUE;
	    dev->GREEN        = GREEN;
	    dev->CYAN         = CYAN;
	    dev->RED          = RED;
	    dev->MAGENTA      = MAGENTA;
	    dev->BROWN        = BROWN;
	    dev->DARK_BLUE    = DARK_BLUE;
	    dev->DARK_GREEN   = DARK_GREEN;
	    dev->DARK_CYAN    = DARK_CYAN;
	    dev->DARK_RED     = DARK_RED;
	    dev->YELLOW       = YELLOW;
	    dev->DARK_MAGENTA = DARK_MAGENTA;
	    dev->ncolor       = N_COLORS;};}
    else
       {if (mono == TRUE)
	   {dev->BLACK        = BLACK;
	    dev->WHITE        = WHITE;
	    dev->GRAY         = WHITE;
	    dev->DARK_GRAY    = WHITE;
	    dev->BLUE         = WHITE;
	    dev->GREEN        = WHITE;
	    dev->CYAN         = WHITE;
	    dev->RED          = WHITE;
	    dev->MAGENTA      = WHITE;
	    dev->BROWN        = WHITE;
	    dev->DARK_BLUE    = WHITE;
	    dev->DARK_GREEN   = WHITE;
	    dev->DARK_CYAN    = WHITE;
	    dev->DARK_RED     = WHITE;
	    dev->YELLOW       = WHITE;
	    dev->DARK_MAGENTA = WHITE;
	    dev->ncolor       = 2;}

	else if (fix == FALSE)
	   {dev->BLACK        = WHITE;
	    dev->WHITE        = BLACK;
	    dev->GRAY         = GRAY;
	    dev->DARK_GRAY    = DARK_GRAY;
	    dev->BLUE         = BLUE;
	    dev->GREEN        = GREEN;
	    dev->CYAN         = CYAN;
	    dev->RED          = RED;
	    dev->MAGENTA      = MAGENTA;
	    dev->BROWN        = BROWN;
	    dev->DARK_BLUE    = DARK_BLUE;
	    dev->DARK_GREEN   = DARK_GREEN;
	    dev->DARK_CYAN    = DARK_CYAN;
	    dev->DARK_RED     = DARK_RED;
	    dev->YELLOW       = YELLOW;
	    dev->DARK_MAGENTA = DARK_MAGENTA;
	    dev->ncolor       = N_COLORS;}

/* CGM and IM */
	else if (fix == TRUE)
           {dev->BLACK        = BLACK;
	    dev->WHITE        = WHITE;
	    dev->GRAY         = GRAY;
	    dev->DARK_GRAY    = DARK_GRAY;
	    dev->BLUE         = BLUE;
	    dev->GREEN        = GREEN;
	    dev->CYAN         = CYAN;
	    dev->RED          = RED;
	    dev->MAGENTA      = MAGENTA;
	    dev->BROWN        = BROWN;
	    dev->DARK_BLUE    = DARK_BLUE;
	    dev->DARK_GREEN   = DARK_GREEN;
	    dev->DARK_CYAN    = DARK_CYAN;
	    dev->DARK_RED     = DARK_RED;
	    dev->YELLOW       = YELLOW;
	    dev->DARK_MAGENTA = DARK_MAGENTA;
	    dev->ncolor       = N_COLORS;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GRAY_MAP - setup the gray scale map GS for DEV based on
 *             - the maximum intensity IMX
 */

void PG_gray_map(PG_device *dev, int ng, int *gs, double imx)
   {

    gs[0] = imx;
    gs[1] = 0;
    gs[2] = 0.8*imx;
    gs[3] = 0.5*imx;
        
    if (dev->background_color_white == TRUE)
       {gs[4] = 0;
        gs[5] = imx;}
    else
       {gs[4] = imx;
        gs[5] = 0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

