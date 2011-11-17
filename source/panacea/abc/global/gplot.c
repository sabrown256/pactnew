/*
 * GPLOT.C - plotting routines in support of the LR global package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "gloin.h"

int
 mesh_acolor,
 mesh_bcolor,
 graphics_init = FALSE,
 mesh_grid,
 mesh_kmax,
 mesh_kmin,
 mesh_lmax,
 mesh_lmin,
 mesh_mcolor,
 mesh_plot_log,
 mesh_plot_phys,
 mesh_scatter,
 mesh_tcolor,
 mesh_vcolor,
 window_init = FALSE;

double
 mesh_botspace,
 mesh_gxmax,
 mesh_gxmin,
 mesh_gymax,
 mesh_gymin,
 mesh_leftspace,
 mesh_rightspace,
 mesh_topspace,
 mesh_xmax,
 mesh_xmin,
 mesh_ymax,
 mesh_ymin,
 xmn,
 xmx,
 ymn,
 ymx;

PG_device
 *mesh_display,
 *mesh_PS_display;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PLOTW - make a new plot */

int plotw(dev, new, name)
   PG_device *dev;
   int new;
   char *name;
   {

#ifndef GEN_ONLY
    int i, ni, nde;
    int *rvp;
    char s[MAXLINE], *token;
    double *v, conv;
    PA_variable *pp;
    static double **elem = NULL;

    if (new)
       elem = NULL;

    rvp = PG_ptr_attr_glb("restore-viewport");

    *rvp = TRUE;
    if (name != NULL)
       {window_init = TRUE;

        strcpy(s, name);
        elem  = CMAKE_N(double *, 10);
        token = SC_firsttok(s, " \t,;");
        nde   = 0;
        for (; token != NULL; token = SC_firsttok(s, " \t,;"))
            {pp = PA_INQUIRE_VARIABLE(token);
             PA_ERR((pp == NULL),
                    "BAD VARIABLE %s - PLOTW", token);

             v = CMAKE_N(double, PA_VARIABLE_SIZE(pp));
	     PM_array_copy(v, PA_VARIABLE_DATA(pp), PA_VARIABLE_SIZE(pp));

             v  = LR_map_centers(v, PA_VARIABLE_CENTERING(pp));
	     ni = SC_MEM_GET_N(double, v);

             conv = PA_VARIABLE_INT_UNIT(pp)/PA_VARIABLE_EXT_UNIT(pp);
             PM_array_scale(v, ni, conv);
             elem[nde++] = v;};
        CREMAKE(elem, double *, nde);};

    if (elem != NULL)
       {rvp = FALSE;
        LR_int_plot(dev, name, N_CENT, nde, elem);};

    if (!window_init)
       {window_init = TRUE;

        mesh_kmax = kmax;
        mesh_kmin = 1;
        mesh_lmax = lmax;
        mesh_lmin = 1;
        meshlmt();};

    if (mesh_plots)
       {if (mesh_plots_r)
           meshlmt();

        if (elem == NULL)
           newframew(dev, new);

        if (mesh_mesh)
           meshw(dev, UNREFL);
        if (mesh_bndry)
           matsw(dev, UNREFL);
        if (mesh_vectr)
           velw(dev, UNREFL);

/* are there any reflected plots? */
        if (mesh_plots_r)
           {for (i = 0; i < N_nodes; i++)
                ry[i] = -ry[i];

/* reflected mesh plot */
            if (mesh_mesh_r)
               meshw(dev, REFL);

/* reflected boundary plot */
            if (mesh_bndry_r)
               matsw(dev, REFL);};};

    PG_set_attrs_glb(TRUE,
		     "restore-viewport", rvp,
		     0);

#endif

    PG_update_vs(dev);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NEWFRAMEW - make a new frame, i.e. draw it */

int newframew(PG_device *dev, int new)
   {double dt;
    char label[MAXLINE];
    mesh_quality *mq;
    static int first = TRUE;
    static double lxmn = HUGE, lymn = HUGE, lxmx = -HUGE, lymx = -HUGE;

    if ((lxmn != xmn) || (lymn != ymn) || (lxmx != xmx) || (lymx != ymx))
       {first = TRUE;
        lxmn = xmn;
        lymn = ymn;
        lxmx = xmx;
        lymx = ymx;};

    if (new || first)
       {first = FALSE;
        PG_clear_screen(dev);
        PG_set_viewport(dev, 0.0, 1.0, 0.0, 1.0);
        PG_set_window(dev, 0.0, 1.0, 0.0, 1.0);
        PG_set_line_color(dev, mesh_tcolor);
        PG_set_text_color(dev, mesh_tcolor);

/* print some useful problem information around the mesh plot */
        PG_set_clipping(dev, FALSE);
        dt = unit[SEC]*PARAM[4]*(PARAM[3] - PARAM[2]);
        PG_write_abs(dev, 0.20, 0.92, "%s", NAME[8]);
        PG_write_abs(dev, 0.55, 0.92, "Time = %9.2e", unit[SEC]*PARAM[1]);
        PG_write_abs(dev, 0.20, 0.88, "Cycle = %d", SWTCH[3]);
        PG_write_abs(dev, 0.55, 0.88, "Dt = %9.2e", dt);

        mq = compute_mesh_quality();
        PG_write_abs(dev, 0.20, 0.84,
                     "Skew = %5.2f  J = %5.2f  Ortho = %5.2f",
                     mq->skew, mq->jac, mq->orth);
        CFREE(mq);

        if (mesh_plot_phys)
           snprintf(label, MAXLINE, "Physical: (%f, %f) (%f, %f)",
		    xmn, ymn, xmx, ymx);
        else if (mesh_plot_log)
           snprintf(label, MAXLINE, "Logical: (%d, %d) (%d, %d)",
		    mesh_kmin, mesh_lmin, mesh_kmax, mesh_lmax);
        PG_center_label(dev, 0.07, label);
        snprintf(label, MAXLINE, "kmax = %d lmax = %d",
		 kmax, lmax);
        PG_center_label(dev, 0.04, label);

/* set up the world coordinate system */
        PG_set_viewport(dev, 0.2, 0.8, 0.2, 0.8);
        PG_set_window(dev, xmn, xmx, ymn, ymx);

/* draw an axis set */
        PG_axis(dev, CARTESIAN_2D);}

    else
       PG_clear_viewport(dev);

    PG_set_clipping(dev, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MATSW -  draw material boundaries */

int matsw(dev, refl)
   PG_device *dev;
   int refl;
   {int k1, k2, l1, l2;
    PM_part *parta;
    PM_side *base;

    PG_set_line_color(dev, mesh_bcolor);
    PG_set_line_width(dev, 1.5);

    for (parta = parts; parta != NULL; parta = parta->next)
        {base = parta->leg;
         while (TRUE)
            {k1 = base->k;
             l1 = base->l;
             k2 = base->next->k;
             l2 = base->next->l;
             if ((k1 < mesh_kmin) || (mesh_kmax < k2) ||
                 (l1 < mesh_lmin) || (mesh_lmax < l2))
                {base = base->next;
                 if (base == parta->leg)
                    break;
                 else
                    continue;};

             plot_leg(dev, base, refl);

             base = base->next;
             if (base == parta->leg)
                break;};};

    PG_set_line_color(dev, mesh_tcolor);
    PG_set_line_width(dev, 0.0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PLOT_LEG - plot the boundary segments for the given side */

int plot_leg(dev, base, refl)
   PG_device *dev;
   PM_side *base;
   int refl;
   {int i, ie, j, n, dk, dl;
    int sdk, sdl, dkdl, dldk, dke, dle, m, m1;

    dk = base->dk;
    dl = base->dl;
    m  = PM_reg_base(parts, base);

/* if there is a matching side that is logically above and to the right
 * of this side duplicate its node if its material is different
 */
    if (base->match != NULL)
       m1 = PM_reg_base(parts, base->match);
    else
       m1 = 0;

    if (m == m1)
       return(FALSE);

/* if this a logically vertical side */
    if (dk == 0)
       {sdl = abs(dl)/dl;
        dl = sdl*dl;
        for (j = 0; j < dl; j++)
            {i  = NODE_OF(base->k, base->l + sdl*j);
             ie = NODE_OF(base->k, base->l + sdl*(j + 1));
             PG_draw_line(dev, rx[i], refl*ry[i], rx[ie], refl*ry[ie]);};
        return(TRUE);}

/* if this a logically horizontal side */
    else if (dl == 0)
       {sdk = abs(dk)/dk;
        dk = sdk*dk;
        for (j = 0; j < dk; j++)
            {i  = NODE_OF(base->k + sdk*j, base->l);
             ie = NODE_OF(base->k + sdk*(j + 1), base->l);
             PG_draw_line(dev, rx[i], refl*ry[i], rx[ie], refl*ry[ie]);};
        return(TRUE);}

/* if this side has an angle of 45 degrees with k axis */
    else if (dk == dl)
       {sdl = abs(dl)/dl;
        dl = sdl*dl;
        for (j = 0; j < dl; j++)
            {i  = NODE_OF(base->k + sdl*j, base->l + sdl*j);
             ie = NODE_OF(base->k + sdl*(j + 1), base->l + sdl*(j + 1));
             PG_draw_line(dev, rx[i], refl*ry[i], rx[ie], refl*ry[ie]);};
        return(TRUE);}

    sdk = abs(dk)/dk;
    dk  = sdk*dk;
    sdl = abs(dl)/dl;
    dl  = sdl*dl;

/* if this side has an angle of less than 45 degrees with k axis */
    if (dk > dl)
       {dkdl = ((dk+1) % (dl+1));
        dke  = 0;
        for (j = 0; j < dl; j++)
            {for (n = 0; n < dkdl; n++)
                 {i  = NODE_OF(base->k + sdk*(j*dkdl + n + dke), base->l + sdl*j);
                  ie = i + sdk;
                  if (!SAME(rx[i], ry[i], rx[ie], ry[ie]))
                     {ie += sdk*kbnd;
                      PG_draw_line(dev, rx[i], refl*ry[i], rx[ie], refl*ry[ie]);};};
             if (dk > dkdl*dl)
                {i = NODE_OF(base->k + sdk*(j*dkdl + n), base->l + sdl*j);
                 PG_draw_line(dev, rx[ie], refl*ry[ie], rx[i], refl*ry[i]);
                 dke++;
                 dk--;};};}

/* if the side makes an angle of greater the 45 degrees with the k axis */
    else if (dl > dk)
       {dldk = ((dl+1) % (dk+1));
        dle  = 0;
        for (j = 0; j < dk; j++)
            {for (n = 0; n < dldk; n++)
                 {i  = NODE_OF(base->k + sdk*j, base->l + sdl*(j*dldk + n + dle));
                  ie = i + sdl*kbnd;
                  if (!SAME(rx[i], ry[i], rx[ie], ry[ie]))
                     {ie += sdl;
                      PG_draw_line(dev, rx[i], refl*ry[i], rx[ie], refl*ry[ie]);};};
             if (dl > dldk*dk)
                {i = NODE_OF(base->k + sdk*(j*dldk + n), base->l + sdl*j);
                 PG_draw_line(dev, rx[ie], refl*ry[ie], rx[i], refl*ry[i]);
                 dle++;
                 dl--;};};};

     return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MESHW - draw the mesh */

int meshw(dev, refl)
   PG_device *dev;
   int refl;
   {int k, l, i1, i2;

    PG_set_line_color(dev, mesh_mcolor);
    PG_set_line_width(dev, 0.0);

    for (l = mesh_lmin; l <= mesh_lmax; l++)
        for (k = mesh_kmin; k < mesh_kmax; k++)
            {i1 = NODE_OF(k, l);
             i2 = i1 + 1;
             PG_draw_line(dev, rx[i1], refl*ry[i1], rx[i2], refl*ry[i2]);};

                
    for (k = mesh_kmin; k <= mesh_kmax; k++)
        for (l = mesh_lmin; l < mesh_lmax; l++)
            {i1 = NODE_OF(k, l);
             i2 = i1 + kbnd;
             PG_draw_line(dev, rx[i1], refl*ry[i1], rx[i2], refl*ry[i2]);};

    PG_set_line_color(dev, mesh_tcolor);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VELMAXW - find the maximum speed on the visible
 *           - portion of the mesh
 */

double velmaxw()
    {return(0.0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VELW - plot the velocities on the mesh */

int velw(dev, refl)
   PG_device *dev;
   int refl;
   {return(refl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_FRAME - initialize global graphics info */

int make_frame()
   {

    mesh_kmax = kmax;
    mesh_kmin = 1;
    mesh_lmax = lmax;
    mesh_lmin = 1;

    mesh_init_graphics_state();
    meshlmt();

    if (!graphics_init)
       {graphics_init = TRUE;

        if (global_name[7] == NULL)
           global_name[7] = CSTRSAVE("COLOR");

        mesh_display = PG_make_device("WINDOW", global_name[7],
                                      global_name[8]);

        PG_open_device(mesh_display, 0.4, 0.1, 0.5, 0.5);
        mesh_set_graphics_state(mesh_display);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GLOBAL_END_GRAPHICS - shutdown the graphics system */

void global_end_graphics(err)
   int err;
   {if (mesh_display != NULL)
       PG_close_device(mesh_display);

    if (mesh_PS_display != NULL)
       PG_close_device(mesh_PS_display);

    err = TRUE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MESH_INIT_GRAPHICS_STATE - initialize the graphics state of the mesh */

int mesh_init_graphics_state()
   {mesh_scatter    = OFF;
    mesh_grid       = OFF;

    mesh_leftspace  = 0.01;
    mesh_rightspace = 0.01;
    mesh_topspace   = 0.01;
    mesh_botspace   = 0.01;

    mesh_gxmin      = 0.0;
    mesh_gymin      = 0.0;
    mesh_gxmax      = 1.0;
    mesh_gymax      = 1.0;

    mesh_xmin       = 0.0;
    mesh_ymin       = 0.0;
    mesh_xmax       = 1.0;
    mesh_ymax       = 1.0;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MESH_SET_GRAPHICS_STATE - map the desired graphics state to the
 *                         - specified device
 */

int mesh_set_graphics_state(d)
   PG_device *d;
   {PG_dev_geometry *g;

    g = &d->g;

    d->scatter = mesh_scatter;
    d->grid    = mesh_grid;

    g->pad[0] = mesh_leftspace;
    g->pad[1] = mesh_rightspace;
    g->pad[2] = mesh_botspace;
    g->pad[3] = mesh_topspace;

    g->bnd[0] = mesh_gxmin;
    g->bnd[1] = mesh_gxmax;
    g->bnd[2] = mesh_gymin;
    g->bnd[3] = mesh_gymax;

    g->wc[0]  = mesh_xmin;
    g->wc[1]  = mesh_xmax;
    g->wc[2]  = mesh_ymin;
    g->wc[3]  = mesh_ymax;

/* set some local graphics state variables from the device */
    mesh_tcolor = d->WHITE;
    mesh_acolor = d->WHITE;
    mesh_mcolor = d->GREEN;
    mesh_bcolor = d->RED;
    mesh_vcolor = d->RED;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TURN - toggle the specified switch in the frame */

void turn()
   {char *token, *sval;
    int val, *pvr;

/* make hash table for TURN command */
    if (swtab == NULL)
       {mesh_plots    = FALSE;
        mesh_mesh     = FALSE;
        mesh_bndry    = FALSE;
        mesh_vectr    = FALSE;
        mesh_plots_r  = FALSE;
        mesh_mesh_r   = FALSE;
        mesh_bndry_r  = FALSE;
        mesh_vectr_r  = FALSE;
        graphics_init = FALSE;
        window_init   = FALSE;

        swtab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
        SC_hasharr_install(swtab, "mesh",       &mesh_mesh,     SC_INTEGER_S, 3, -1);
        SC_hasharr_install(swtab, "boundary",   &mesh_bndry,    SC_INTEGER_S, 3, -1);
        SC_hasharr_install(swtab, "vector",     &mesh_vectr,    SC_INTEGER_S, 3, -1);
/*
        SC_hasharr_install(swtab, "physical",   &plot_physical, SC_INTEGER_S, 3, -1);
        SC_hasharr_install(swtab, "logical",    &plot_log,      SC_INTEGER_S, 3, -1);
*/
        SC_hasharr_install(swtab, "mesh-r",     &mesh_mesh_r,   SC_INTEGER_S, 3, -1);
        SC_hasharr_install(swtab, "boundary-r", &mesh_bndry_r,  SC_INTEGER_S, 3, -1);
        SC_hasharr_install(swtab, "vector-r",   &mesh_vectr_r,  SC_INTEGER_S, 3, -1);};

    token = PA_get_field("TYPE", "TURN", REQU);
    sval  = PA_get_field("VALUE", "TURN", REQU);

    if (strcmp(sval, "on") == 0)
       val = ON;
    else
       val = OFF;

#ifndef GEN_ONLY

    pvr = (int *) SC_hasharr_def_lookup(swtab, token);
    if (pvr != NULL)
       *pvr = val;

    val     = mesh_plots_r;
    mesh_plots_r = mesh_mesh_r || mesh_bndry_r || mesh_vectr_r;
    if (val != mesh_plots_r)
       meshlmt();

    mesh_plots = mesh_mesh || mesh_bndry || mesh_vectr || mesh_plots_r;

    if (Autoplot)
       plot(FALSE, NULL);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VIEWH - handle the view command */

void viewh()
   {char *s;

    s = SC_strtok(NULL, "\n", PA_strtok_p);
    plot(TRUE, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PLOT - make a new plot */

void plot(new, name)
   int new;
   char *name;
   {static int tv_flag = FALSE;

    mesh_plot_phys = TRUE;

#ifndef GEN_ONLY
    if (!graphics_init)
       if (!tv_flag)
          {make_frame();
           tv_flag = TRUE;};
#endif

    plotw(mesh_display, new, name);

    PG_finish_plot(mesh_display);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINTH - handle the print command */

void printh()
   {char *s;

    s = SC_strtok(NULL, "\n", PA_strtok_p);
    printscr(TRUE, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINTSCR - dump the screen to the printer */

void printscr(new, name)
   int new;
   char *name;
   {PG_device *dev;
    char bf[MAXLINE];

    if (mesh_PS_display == NULL)
       {snprintf(bf, MAXLINE, "%s %s", global_name[8], global_name[9]);
        mesh_PS_display = PG_make_device("PS", "MONOCHROME", bf);
        PA_ERR(mesh_PS_display == NULL,
               "CAN'T OPEN POST-SCRIPT DEVICE - PRINTSCR");
        mesh_set_graphics_state(mesh_PS_display);
        PG_open_device(mesh_PS_display, 0.0, 0.0, 0.0, 0.0);};

/* set the device, plot, and reset the device */
    dev          = mesh_display;
    mesh_display = mesh_PS_display;
    plot(new, name);
    mesh_display = dev;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MESHLMT - compute mesh limits xmx, xmn, ymx, and ymn */

int meshlmt()
   {int k, l;
    int nofirst, i;
    double dx, dy;

    nofirst = 1;
    for (l = mesh_lmin; l <= mesh_lmax; l++)
        for (k = mesh_kmin; k <= mesh_kmax; k++)
	    {i = NODE_OF(k, l);
	     if (nodet[i] > 0.0)
	        {if (nofirst)
		    {nofirst = 0;
		     xmn = rx[i];
		     xmx = rx[i];
		     ymn = ry[i];
		     ymx = ry[i];}
		 else
		    {if (rx[i] < xmn)
		        xmn = rx[i];
		     if (rx[i] > xmx)
		        xmx = rx[i];
		     if (ry[i] < ymn)
		        ymn = ry[i];
		     if (ry[i] > ymx)
		        ymx = ry[i];};};};

    if (mesh_plots_r)
       ymn = -ymx;

    dy = ymx - ymn;
    dx = xmx - xmn;

    if (dy > dx)
       xmx = xmn + dy;
    else
       ymx = ymn + dx;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
