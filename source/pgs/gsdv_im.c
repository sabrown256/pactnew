/*
 * GSDV_IM.C - distributed PGS graphics routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

/* to use the IMAGE device issue a call
 *
 *   PG_register_device("IMAGE", PG_setup_image_device);
 *
 */

#include "cpyright.h"
#include "pgs_int.h"
#include "ppc_int.h"
#include "scope_proc.h"

#define PG_DPMT_SETUP    105
#define PG_DPMT_IMAGE     35

/*--------------------------------------------------------------------------*/

/*                         PARALLEL GRAPHICS SUPPORT                        */

/*--------------------------------------------------------------------------*/

/* PG_GET_NUMBER_PROCESSORS - handle the number of processors in the
 *                          - presence of parallel_graphics attribute
 */

int PG_get_number_processors(void)
   {int np;

    if (_PG_gattrs.parallel_graphics == TRUE)
       np = PC_get_number_processors();

    else
       np = 1;

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_PROCESSOR_NUMBER - handle the processor number in the
 *                          - presence of parallel_graphics attribute
 */

int PG_get_processor_number(void)
   {int ip;

    if (_PG_gattrs.parallel_graphics == TRUE)
       ip = PC_get_processor_number();

    else
       ip = 0;

    return(ip);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_INIT_DEV_PARALLEL - initialize parallel rendering info for a device */

PG_par_rend_info *_PG_init_dev_parallel(PG_device *dd, int dp)
   {int ip, np;
    int *map;
    PG_par_rend_info *pri;
    
    pri = NULL;
    if (_PG.pp_me != NULL)
       {np = PG_get_number_processors();
        ip = PG_get_processor_number();

        map = (ip == dp) ?
              CMAKE_N(int, np) :
              NULL;

        pri = CMAKE(PG_par_rend_info);
        pri->dd        = dd;
        pri->pp        = _PG.pp_me;
        pri->ip        = dp;
        pri->have_data = TRUE;
        pri->map       = map;
        pri->alist     = NULL;
        pri->label     = NULL;
        pri->render    = PLOT_NONE;
        pri->polar     = FALSE;};

    return(pri);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_INIT_PARALLEL - initialize parallel graphics */

int PG_init_parallel(char **argv, PROCESS *pp)
   {int ret;
    PDBfile *vif;

    if (pp == NULL)
       {PC_init_communications(NULL);

        _PG.pp_me = PC_open_member(argv, NULL);

        if (_PG.pp_me != NULL)
           {vif = _PG.pp_me->vif;
            PD_def_mapping(vif);}

        ret = TRUE;}

    else
       {_PG.pp_me = pp;

        ret = FALSE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FIN_PARALLEL - terminate parallel graphics */

int PG_fin_parallel(int trm)
   {

    if (trm)
       PC_close_member(_PG.pp_me);

    return(trm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PARALLEL_SETUP - reset the picture for parallel graphics */

void PG_parallel_setup(PG_device *dev, PG_picture_desc *pd)
   {int i, j, ia, ib, ip, ne, nf, np, nr;
    int ndf, ndr, dp, dfl;
    int *map;
    double wc[PG_BOXSZ], pc[PG_BOXSZ], ndc[PG_BOXSZ];
    double dx, dy, dvx, dvy, rx, ry;
    double *re, *dc, *adc, *pdc;
    PG_rendering render;
    PG_par_rend_info *pri;
    PROCESS *pp;
    static int sp[] = {SC_MATCH_NODE, 1, 0,
                       SC_MATCH_TAG, PG_DPMT_SETUP,
                       SC_BLOCK_STATE, FALSE,
                       SC_BUFFER_SIZE, 0,
                       0};

    np = PG_get_number_processors();
    if (np > 1)
       {ip = PG_get_processor_number();

        pri    = dev->pri;
        pp     = pri->pp;
        render = pri->render;
        dp     = pri->ip;
        map    = pri->map;

/* get the data to be shared */
        nr  = dev->range_n_extrema;
        dfl = (nr > 0);

        pri->have_data = dfl;

/* GOTCHA: arbitrarily limiting to 3 dimensional range elements */
        ndr = 6;
        ndf = 10;

        ne = ndf + ndr;
        dc = CMAKE_N(double, ne);

        if (dfl)
           {PG_get_viewspace(dev, WORLDC, dc);
            PG_get_viewspace(dev, NORMC, dc+4);

            re  = dev->range_extrema;
            pdc = dc + ndf;
            for (i = 0; i < nr; i++)
                *pdc++ = re[i];}

        else
           dc[0] = -HUGE;

        sp[6] = FALSE;
        sp[8] = ne*sizeof(double);

/* post the receives */
        if (ip == dp)
           {adc = CMAKE_N(double, np*ne);
	    if (adc == NULL)
	       return;

            pdc = adc + dp*ne;
            for (i = 0; i < ne; i++)
                pdc[i] = dc[i];

            for (i = 0; i < np; i++)
                {if (i != dp)
                    {sp[2] = i;
                     PC_in(adc+i*ne, SC_DOUBLE_S, ne, pp, sp);};};}

/* post the sends */
        else
           {sp[2] = dp;
            PC_out(dc, SC_DOUBLE_S, ne, pp, sp);};

/* wait for the data to move */
        PC_wait(pp);

/* find the global extrema */
        if (ip == dp)
           {for (i = 0; i < np; i++)
                {pdc    = adc + i*ne;
                 map[i] = (pdc[0] != -HUGE);};

/* if the node doing the final output has no data get data from
 * a node that has some
 */
            if (map[dp] == FALSE)
               {for (i = 0; i < np; i++)
                    {if (map[i] == TRUE)
                        {pdc = adc + i*ne;
                         for (j = 0; j < ne; j++)
                             dc[j] = pdc[j];
                         break;};};};
               
/* find global limits by checking everybody who has data */
            for (i = 0; i < np; i++)
                {if ((map[i] == TRUE) && (i != dp))
                    {pdc = adc + i*ne;

                     nf = ne >> 1;
                     for (j = 0; j < nf; j++)
                         {ia = 2*j;
                          ib = ia + 1;

                          dc[ia] = min(dc[ia], pdc[ia]);
                          dc[ib] = max(dc[ib], pdc[ib]);};};};

            CFREE(adc);

	    dc[8] = PG_window_width(dev);
	    dc[9] = PG_window_height(dev);

/* broadcast the extrema back out */
            for (i = 0; i < np; i++)
                {if (i != dp)
                    {sp[2] = i;
                     PC_out(dc, SC_DOUBLE_S, ne, pp, sp);};};}

        else
           {sp[2] = dp;
            PC_in(dc, SC_DOUBLE_S, ne, pp, sp);};
        
        PC_wait(pp);

/* compute what this node's piece of the global picture is */
        if (dfl)
           {PG_get_viewspace(dev, WORLDC, wc);
            PG_get_viewspace(dev, NORMC, ndc);

/* re-open the device to get the correct pixel width and height */
            dev->re_open = TRUE;
            dev->open_screen(dev, 0.0, 0.0, dc[8], dc[9]);
            dev->re_open = FALSE;

            dx  = dc[1] - dc[0];
            dy  = dc[3] - dc[2];
            dvx = dc[5] - dc[4];
            dvy = dc[7] - dc[6];

            rx = dvx/dx;
            ry = dvy/dy;

/* reset the device */
            ndc[0] += (wc[0] - dc[0])*rx;
            ndc[1] -= (dc[1] - wc[1])*rx;
            if ((render == PLOT_CONTOUR) ||
                (render == PLOT_FILL_POLY) ||
                (render == PLOT_IMAGE) ||
                (render == PLOT_WIRE_MESH) ||
                (render == PLOT_SURFACE) ||
                (render == PLOT_VECTOR) ||
                (render == PLOT_MESH))
               {ndc[2] += (wc[2] - dc[2])*ry;
                ndc[3] -= (dc[3] - wc[3])*ry;}
            else
	       {wc[2] = dc[2];
		wc[3] = dc[3];};

	    PG_set_viewspace(dev, 3, WORLDC, wc);

	    PG_trans_box(dev, 2, NORMC, ndc, PIXELC, pc);

	    pc[1] += 1.0;
            if (render != PLOT_CURVE)
		pc[3] += 1.0;

	    PG_trans_box(dev, 2, PIXELC, pc, NORMC, ndc);

            PG_set_viewspace(dev, 2, NORMC, ndc);

            pdc = dc + ndf;
            for (i = 0; i < nr; i++)
                re[i] = *pdc++;};

        CFREE(dc);};

    return;}

/*--------------------------------------------------------------------------*/

/*                            IMAGE COMMUNICATIONS                          */

/*--------------------------------------------------------------------------*/

/* _PG_TRANSMIT_IMAGES - transmit images between nodes */

static PG_image *_PG_transmit_images(PG_device *dev, PG_image *im)
   {int i, ip, np, dp, dfl;
    int *map;
    PROCESS *pp;
    PG_par_rend_info *pri;
    PG_image *pim;
    static int sp[] = {SC_MATCH_NODE, 1, 0,
                       SC_MATCH_TAG, PG_DPMT_IMAGE,
                       SC_BLOCK_STATE, FALSE,
                       SC_BUFFER_SIZE, 0,
                       0};

    pim = NULL;

    if (_PG_gattrs.parallel_simulate == TRUE)
       {ip = PG_get_processor_number();
        np = PG_get_number_processors();
        dp = dev->pri->ip;

/* post the receives */
        if (ip == dp)
           {pim = CMAKE_N(PG_image, np);

            for (i = 0; i < np; i++)
                pim[i] = *im;

            SC_mark(im->label, np);
            SC_mark(im->element_type, np);
            SC_mark(im->buffer, np);};}

    else
       {ip  = PG_get_processor_number();
        np  = PG_get_number_processors();
        pri = dev->pri;
        pp  = pri->pp;
        dp  = pri->ip;
        map = pri->map;
        dfl = pri->have_data;

        sp[6] = FALSE;

/* post the receives */
        if (ip == dp)
           {pim = CMAKE_N(PG_image, np);

	    if (SC_zero_on_alloc_n(-1) == FALSE)
	       SC_MEM_INIT_N(PG_image, pim, np);

            for (i = 0; i < np; i++)
                {if ((i != dp) && (map[i] == TRUE))
                    {sp[2] = i;
                     PC_in(pim+i, "PG_image", 1, pp, sp);};};

            if (dfl)
               pim[dp] = *im;

            SC_mark(im->label, np);
            SC_mark(im->element_type, np);
            SC_mark(im->buffer, np);}

/* post the sends */
        else if (dfl)
           {pim         = NULL;
            im->palette = NULL;
            sp[2] = dp;
            PC_out(im, "PG_image", 1, pp, sp);};

        PC_wait(pp);};

    return(pim);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DISPLAY_IMAGE - receive and assemble an image from other nodes
 *                   - once it is assembled draw it
 */

static void _PG_display_image(PG_device *dev, PG_image *pim)
   {int i, k, l, np, bpp, nk, nl, w, h, render;
    int *map;
    double dbx[PG_BOXSZ],  rbx[PG_BOXSZ], x[PG_BOXSZ];
    char *cpn, *labl, type[MAXLINE];
    PG_image *im, *nim, *tim;
    PG_par_rend_info *pri;
    PG_device *dd;
    extern void PG_render_parallel(PG_device *dd, PG_image *nim, int np,
                                   PG_image *pim);

    PG_get_device_image(dev, im);

    np = PG_get_number_processors();

    pri = dev->pri;
    if (pri != NULL)
       {render = pri->render;
        map    = pri->map;
        dd     = pri->dd;
        if (dd != NULL)
           {labl = pri->label;
            bpp  = im->bits_pixel;
        
            SC_strncpy(type, MAXLINE, im->element_type, -1);
            PD_dereference(type);

/* get the single image specs */
	    PG_box_init(2, dbx, HUGE, -HUGE);
	    PG_box_init(1, rbx, HUGE, -HUGE);
            for (i = 0; i < np; i++)
                {if (map[i])
                    {tim = pim + i;
                     x[0]  = tim->xmin;
                     x[1]  = tim->xmax;
                     x[2]  = tim->ymin;
                     x[3]  = tim->ymax;
                     x[4]  = tim->zmin;
                     x[5]  = tim->zmax;
                     dbx[0] = min(dbx[0], x[0]);
                     dbx[1] = max(dbx[1], x[1]);
                     dbx[2] = min(dbx[2], x[2]);
                     dbx[3] = max(dbx[3], x[3]);
                     rbx[0] = min(rbx[0], x[4]);
                     rbx[1] = max(rbx[1], x[5]);};};

	    nk = 0;
	    nl = 0;
            for (i = 0; i < np; i++)
                {if (map[i])
                    {tim = pim + i;
                     if (render == PLOT_CURVE)
                        {x[0] = tim->xmin;
                         x[1] = tim->xmax;
                         nk = (dbx[1] - dbx[0])/(x[1] - x[0]) + 0.5;
                         nl = 1.0 + 0.5;}
                     else
                        {x[0] = tim->xmin;
                         x[1] = tim->xmax;
                         x[2] = tim->ymin;
                         x[3] = tim->ymax;
                         nk = (dbx[1] - dbx[0])/(x[1] - x[0]) + 0.5;
                         nl = (dbx[3] - dbx[2])/(x[3] - x[2]) + 0.5;};
                     break;};};

            k   = 0;
            w = 0;
            for (i = 0; i < np; i++)
                {if (map[i])
                    {tim = pim + i;
                     w += tim->kmax;
                     k++;
                     if (k >= nk)
                        break;};};
            w -= (nk == np) ? (2*np - 1) : 1;

            l   = 0;
            h = 0;
            for (i = 0; i < np; i++)
                {if (map[i])
                    {tim = pim + i;
                     h += tim->lmax;
                     l++;
                     if (l >= nl)
                        break;};};
            if (render != PLOT_CURVE)
               h -= (nl == np) ? (2*np - 1) : 1;

            cpn = dev->current_palette->name;

/* make the single image */
            nim = PG_make_image_n(labl, type, NULL,
				  2, WORLDC, dbx, rbx, w, h, bpp,
				  PG_fset_palette(dd, cpn));
	    if (nim != NULL)
	       {memset(nim->buffer, dev->background_color_white, nim->size);

/* assemble the images into a single image for plotting */
		for (i = 0; i < np; i++)
		    {if (map[i])
		        {tim = pim + i;
			 PG_copy_image(nim, tim, 0);};};

/* prevent rescaling the image data by NULLing the type */
		CFREE(nim->element_type);

		PG_set_viewspace(dev, 2, WORLDC, dbx);
		PG_render_parallel(dd, nim, np, pim);};

            CFREE(pri->label);};};

/* cleanup */
    for (i = 0; i < np; i++)
        {CFREE(pim[i].buffer);
         pim[i].buffer = NULL;

         CFREE(pim[i].element_type);
         pim[i].element_type = NULL;

         CFREE(pim[i].label);
         pim[i].label = NULL;};

    CFREE(pim);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_IM_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                    - and get the device updated with the image
 *                    - after this function nothing more can be added to
 *                    - the image
 */
 
static void _PG_IM_finish_plot(PG_device *dev)
   {int ip;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    double *rextr;
    PG_image *im, *pim;

    PG_get_device_image(dev, im);
    if (im != NULL)
       {PG_rl_image(im);
        PG_put_device_image(dev, NULL);};

    PG_box_init(2, dbx, 0.0, 0.0);
    PG_box_init(1, rbx, 0.0, 1.0);

    rextr = dev->range_extrema;
    if (rextr != NULL)
       {rbx[0] = rextr[0];
        rbx[1] = rextr[1];};

    im = PG_extract_image(dev, dbx, rbx);

    PG_put_device_image(dev, im);

/* send the images around */
    pim = _PG_transmit_images(dev, im);

/* put them all together */
    ip = PG_get_processor_number();
    if (ip == dev->pri->ip)
       _PG_display_image(dev, pim);

    PG_rl_image(im);
    PG_put_device_image(dev, NULL);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_IM_OPEN - initialize an IMAGE device */
 
static PG_device *_PG_IM_open(PG_device *dev, double xf, double yf,
                              double dxf, double dyf)
   {int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    int n_colors, re_open;
    int dx[PG_SPACEDM];
    double intensity;
    PG_par_rend_info *pri;
    PG_font_family *ff;
    frame *fr;
    PG_RAST_device *mdv;
    PG_dev_geometry *g;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    re_open = dev->re_open;

    if (re_open == FALSE)
       {pri = dev->pri;
        if (pri->dd != NULL)
           PG_open_device(pri->dd, xf, yf, dxf, dyf);

        PG_setup_markers();

        dev->type_index       = IMAGE_DEVICE;
        dev->quadrant         = QUAD_ONE;
        dev->hard_copy_device = FALSE;
        dev->print_labels     = FALSE;};

/* get the window shape in NDC */
    if (dxf < 2.0)
       dxf = 400.0;

    if (dyf < 2.0)
       dyf = 400.0;

    _PG_rst_set_dev_prop(dev, (int) dxf, (int) dyf, N_RAST_COLOR);

    if (re_open == TRUE)
       {GET_RAST_DEVICE(dev, mdv);
        PG_free_raster_device(mdv);};

    fr  = NULL;
    mdv = PG_make_raster_device((int) dxf, (int) dyf, dev->title,
				fr, FALSE, NULL);

    SET_RAST_DEVICE(dev, mdv);

    PG_query_screen_n(dev, dx, &n_colors);

/* set device pixel coordinate limits */
    g->cpc[0] = SHRT_MIN + dx[0];
    g->cpc[1] = SHRT_MAX - dx[0];
    g->cpc[2] = SHRT_MIN + dx[1];
    g->cpc[3] = SHRT_MAX - dx[1];
    g->cpc[4] = SHRT_MIN;
    g->cpc[5] = SHRT_MAX;

    g->hwin[0] = 0.0;
    g->hwin[1] = dx[0];
    g->hwin[2] = 0.0;
    g->hwin[3] = dx[0];

    SET_PC_FROM_HWIN(g);

    dev->window_x[0] = g->hwin[0];
    dev->window_x[2] = g->hwin[2];

    if (re_open == FALSE)
       {Color_Map(dev, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

        dev->ncolor = N_RAST_COLOR;
        dev->absolute_n_color = N_RAST_COLOR;

/* compute the view port */
	_PG_default_viewspace(dev, FALSE);

	PG_init_viewspace(dev, TRUE);
 
/* initialize fonts */
        ff = PG_make_font_family(dev, "helvetica", NULL, 4,
                                 "Helvetica",
                                 "Helvetica-Oblique",
                                 "Helvetica-Bold",
                                 "Helvetica-BoldOblique");

        ff = PG_make_font_family(dev, "times", ff, 4,
                                 "Times-Roman",
                                 "Times-Italic",
                                 "Times-Bold",
                                 "Times-BoldItalic");

        ff = PG_make_font_family(dev, "courier", ff, 4,
                                 "Courier",
                                 "Courier-Oblique",
                                 "Courier-Bold",
                                 "Courier-BoldOblique");

        dev->font_family = ff;

        PG_fset_font(dev, "helvetica", "medium", 12);

/* put in the default palettes */
        intensity  = dev->max_intensity*MAXPIX;
        Lightest   = 0;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = intensity;

        PG_setup_standard_palettes(dev, 64,
                                   Light, Dark,
                                   Light_Gray, Dark_Gray,
                                   Lightest, Darkest);

	SC_set_get_line(io_gets);
	SC_set_put_line(io_printf);
	SC_set_put_string(io_puts);};

    PG_clear_window(dev);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_IM_CLOSE_DEVICE - close a device */
 
static void _PG_IM_close_device(PG_device *dev)
   {PG_image *im;

    PG_get_device_image(dev, im);
    if (im != NULL)
       {PG_rl_image(im);
        PG_put_device_image(dev, NULL);};

/* clean up the device */
    PG_rl_device(dev);

    return;}

/*--------------------------------------------------------------------------*/

/*                              PGS INTERFACE ROUTINES                      */

/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_IMAGE_DEVICE - do the device dependent device initialization
 *                       - for PG_make_device
 */

int PG_setup_image_device(PG_device *d)
   {

    PG_setup_raster_device(d);

    d->type_index   = IMAGE_DEVICE;
    d->close_device = _PG_IM_close_device;
    d->finish_plot  = _PG_IM_finish_plot;
    d->open_screen  = _PG_IM_open;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
