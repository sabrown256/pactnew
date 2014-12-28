/*
 * GSGROT.C - grotrian plotting routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GROTRIAN_PLOT - draw the grotrian plot */

static void _PG_grotrian_plot(PG_device *dev, PG_graph *g)
   {int *maxes, *nt;
    int n_tr, n_s, i, u, l, ndpt, nrpt, clr;
    double tn[2], vw[2];
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    double p[PG_SPACEDM];
    double wc[PG_BOXSZ];
    double **d, **r, **lvl, extr[4];
    double **lx, **lu, *dx;
    char format[20], **labels;
    PM_set *domain, *range;

    if ((dev == NULL) || (g == NULL))
       return;

    domain = g->f->domain;
    ndpt   = domain->n_elements;
    r      = (double **) domain->elements;
    d      = PM_convert_vectors(2, ndpt, r, domain->element_type);

    PM_array_real(domain->element_type, domain->extrema, 4, extr);
    wc[0] = extr[0] - 0.75;
    wc[1] = extr[1] + 0.75;
    wc[2] = extr[2];
    wc[3] = extr[3];

    maxes = domain->max_index;
    n_s   = maxes[0];

    range = g->f->range;
    nrpt  = range->n_elements;
    r     = (double **) range->elements;
    lvl   = PM_convert_vectors(2, nrpt, r+1, range->element_type);

    PM_array_real(range->element_type, range->extrema, 2, extr);

    maxes = range->max_index;
    n_tr  = maxes[0];

    PG_get_attrs_set(domain,
		     "GROTRIAN-LABELS", G_POINTER_I, &labels, NULL,
		     "LINE-COLOR",      G_INT_I, &clr, dev->WHITE,
		     NULL);

/* draw the vertical axis */
    SC_strncpy(format, 20, "%10.2g", -1);
    PG_fset_line_width(dev, 0.0);

    xl[0] = wc[0];
    xl[1] = wc[2];
    xr[0] = wc[0];
    xr[1] = wc[3];
    tn[0] = 0.0;
    tn[1] = 1.0;
    vw[0] = wc[2];
    vw[1] = wc[3];

    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0, format,
		   AXIS_TICK_LEFT, AXIS_TICK_LEFT, FALSE,
		   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | AXIS_TICK_LABEL, 0);

/* draw the horizontal labels */
    wc[2] -= 0.05*(wc[3] - wc[2]);
    for (x1[0] = wc[0] + 0.7; x1[0] < wc[1]; x1[0] += 1.0)
        {p[0] = x1[0];
	 p[1] = wc[2];
	 PG_write_n(dev, 2, WORLDC, p, "%ld", (int) (x1[0] + 0.5));};

/* draw the states */
    PG_fset_line_width(dev, 1.0);
    for (i = 0; i < n_s; i++)
        {x1[0] = d[0][i] - 0.25;    
         x1[1] = d[1][i];
         x2[0] = x1[0] + 0.5;    
         x2[1] = d[1][i];
         PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};

/* label the states */
    if (labels != NULL)

/* set the labelling PostScript font */
       {if (POSTSCRIPT_DEVICE(dev))
           io_printf(dev->file, "Flabel\n");

        for (i = 0; i < n_s; i++)
            {x1[0] = d[0][i] + 0.27;
             x1[1] = d[1][i];
             PG_write_n(dev, 2, WORLDC, x1, "%s", labels[i]);};

/* restore the main PostScript font */
       if (POSTSCRIPT_DEVICE(dev))
          io_printf(dev->file, "Fplot\n");};

    lx = PM_make_vectors(2, n_tr);
    lu = PM_make_vectors(2, n_tr);

    nt = CMAKE_N(int, ndpt);
    for (i = 0; i < ndpt; i++)
        nt[i] = 0;

    for (i = 0; i < n_tr; i++)
        {u = lvl[0][i];
         l = lvl[1][i];
         nt[u]++;
         nt[l]++;};

    dx = CMAKE_N(double, ndpt);
    for (i = 0; i < ndpt; i++)
        dx[i] = 0.5/((double) nt[i] + 1);

/* draw the transitions */
    for (i = 0; i < n_tr; i++)
        {u  = lvl[0][i];
         l  = lvl[1][i];
         lx[0][i] = d[0][u] - dx[u]*nt[u] + 0.25;
         lx[1][i] = d[1][u];
         lu[0][i] = d[0][l] - dx[l]*nt[l] + 0.25 - lx[0][i];
         lu[1][i] = d[1][l] - lx[1][i];
         nt[u]--;
         nt[l]--;};

    PG_set_vec_attr(dev, VEC_LINETHICK, 0.1,
                         VEC_COLOR, clr,
                         VEC_SCALE, 1.0,
                         VEC_FIXHEAD, TRUE,
                         VEC_ANGLE, 15.0,
                         VEC_HEADSIZE, 0.01, 0);
    PG_draw_vector_n(dev, 2, WORLDC, n_tr, lx, lu);

/* we will want to draw the palette at some point */
/*    PG_draw_palette(dev, 0.808, 0.808, 0.175, 0.825, zmin, zmax, 0.1); */

    PG_update_vs(dev);

    PM_free_vectors(2, d);
    PM_free_vectors(2, lvl);
    PM_free_vectors(2, lx);
    PM_free_vectors(2, lu);

    CFREE(nt);
    CFREE(dx);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_GROTRIAN_PLOT - main grotrian plot control routine */

void PG_grotrian_plot(PG_device *dev, PG_graph *data, ...)
   {double wc[PG_BOXSZ], ndc[PG_BOXSZ];
    double dextr[4], rextr[2];
    PG_dev_attributes *attr;
    PG_graph *g;
    PM_set *domain, *range;

    if ((dev == NULL) || (data == NULL))
       return;

    PG_box_init(3, wc, HUGE_REAL, -HUGE_REAL);

/* find the extrema for this frame */
    for (g = data; g != NULL; g = g->next)
        {domain = g->f->domain;
	 PM_array_real(domain->element_type, domain->extrema, 4, dextr);

         range = g->f->range;
	 PM_array_real(range->element_type, range->extrema, 2, rextr);

         wc[0] = min(wc[0], dextr[0]);
         wc[1] = max(wc[1], dextr[1]);
         wc[2] = min(wc[2], dextr[2]);
         wc[3] = max(wc[3], dextr[3]);
         wc[4] = min(wc[4], rextr[0]);
         wc[5] = max(wc[5], rextr[1]);};

    wc[0] -= 0.75;
    wc[1] += 0.75;

    if (wc[0] >= wc[1])
       {wc[0] = 0.0;
        wc[1] = 1.0;};

    if (wc[2] >= wc[3])
       {wc[2] = 0.0;
        wc[3] = 1.0;};

    PG_get_viewspace(dev, NORMC, ndc);

    switch (_PG_gattrs.hl_clear_mode)
       {case CLEAR_SCREEN :
             PG_clear_window(dev);
	     break;
        case CLEAR_VIEWPORT :
             PG_clear_viewport(dev);
	     break;
        case CLEAR_FRAME :
             PG_clear_frame(dev);
	     break;};

/* set the plot limits and set the range and domain controls */
    if (dev->autorange == TRUE)
       {ndc[3] = wc[3];
        ndc[2] = wc[2];};

    if (dev->autodomain == TRUE)
       {ndc[1] = wc[1];
        ndc[0] = wc[0];};

    PG_set_viewspace(dev, 2, WORLDC, ndc);

    PG_get_viewspace(dev, NORMC, ndc);
    PG_center_label(dev, 0.5*(1.0 + ndc[3]), data->f->name);

/* save user's values for various attributes */
    attr = PG_get_attributes(dev);

    SC_VA_START(data);

/* plot all of the current functions */
    for (g = data; g != NULL; g = g->next)
        _PG_grotrian_plot(dev, g);

    SC_VA_END;

    if (dev->finished)
       {PG_finish_plot(dev);}
    else
       {PG_update_vs(dev);};

/* reset user's values for various attributes */
    PG_set_attributes(dev, attr);
    CFREE(attr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
