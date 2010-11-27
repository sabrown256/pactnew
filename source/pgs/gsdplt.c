/*
 * GSDPLT.C - domain mesh plot routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

int
 _PG_bnd_color = 0,
 _PG_bnd_style = LINE_SOLID;

double
 _PG_bnd_width = 0.0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SAVE_SEG - copy points JN1 and JN2 of R into T
 *              - iff they are both contained in BOX
 *              - GOTCHA: this should clip the segment to the BOX
 *              - not reject it entirely
 */

static INLINE int _PG_save_seg(int jn1, int jn2, int nn,
			       double **r, double **t, double *box)
   {int i, l, ok;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];

    ok = TRUE;
    for (i = 0; i < PG_SPACEDM; i++)
        {l = 2*i;

	 x1[i] = r[i][jn1];
	 x2[i] = r[i][jn2];

         ok &= ((box[l] <= x1[i]) && (x1[i] <= box[l+1]));};

    if (ok == TRUE)
       {for (i = 0; i < PG_SPACEDM; i++)
	    {t[i][nn]   = x1[i];
	     t[i][nn+1] = x2[i];};

        nn += 2;};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_1D - draw a 1d domain mesh */

static void _PG_dom_1d(PG_device *dev, PM_set *dom, PM_set *ran)
   {int i, nn, npts;
    double xv, wc[PG_BOXSZ], x1[PG_SPACEDM], x2[PG_SPACEDM];
    double **r, *rx, *f;

    npts = dom->n_elements;
    r    = (double **) dom->elements;
    rx   = PM_array_real(dom->element_type, r[0], npts, NULL);

    if (ran != NULL)
       {npts = ran->n_elements;
	r    = (double **) ran->elements;
        f    = PM_array_real(ran->element_type, r[0], npts, NULL);}
    else
       f = NULL;

/* do work here */
    PG_get_viewspace(dev, WORLDC, wc);

    x1[1] = wc[2];
    x2[1] = wc[3];

    switch (_PG.cntrg)
       {case Z_CENT :
	     nn = npts - 2;

	     xv    = rx[0];
	     x1[0] = xv;
	     x2[0] = xv;
	     PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);

	     for (i = 1; i < nn; i++)
	         {xv    = 0.5*(rx[i] + rx[i+1]);
		  x1[0] = xv;
		  x2[0] = xv;
		  PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};

	     xv    = rx[npts-1];
	     x1[0] = xv;
	     x2[0] = xv;
	     PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);

	     break;

	case N_CENT :
	     for (i = 0; i < npts; i++)
	         {xv    = rx[i];
		  x1[0] = xv;
		  x2[0] = xv;
		  PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};
	     break;

       default :
	     break;};

    SFREE(rx);
    SFREE(f);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_LR_2D - draw a 2d logically rectangular domain mesh */

static void _PG_dom_lr_2d(PG_device *dev, double *rx, double *ry,
			  void *cnnct, pcons *alist)
   {int i, k, l, i1, i2, n, nmap, imp, imq;
    int kb, lb, km, lm, kmax, lmax, eflag, refl;
    int kmn, kmx, knc, kon, kcl, kst;
    int lmn, lmx, lnc, lon, lcl, lst;
    int lc, ls;
    int *maxes, *str;
    double kwd, lwd, lw;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    char *emap;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    n     = kmax*lmax;

    km   = kmax - 1;
    lm   = lmax - 1;
    km   = max(km, 1);
    lm   = max(lm, 1);
    nmap = km*lm;

    refl = 1;
    LR_MAPPING_INFO(alist, nmap);

    PM_CHECK_EMAP(alist, nmap, eflag, emap);

    PG_get_attrs_alist(alist,
		       "MESH-SUBSELECT", SC_POINTER_I, &str, NULL,
		       NULL);

    if (str == NULL)
       {kmn = 0;
	kmx = kmax - 1;
	knc = 1;
	kon = TRUE;
        kcl = dev->line_color;
        kst = dev->line_style;
        kwd = dev->line_width;

        lmn = 0;
	lmx = lmax - 1;
	lnc = 1;
	lon = TRUE;
        lcl = dev->line_color;
        lst = dev->line_style;
        lwd = dev->line_width;}

/* entries in str are: start, stop, step, on/off, clr, sty, 1000*wid */
    else
       {kmn = str[0];
	kmx = str[1];
	knc = str[2];
	kon = str[3];
        kcl = str[4];
        kst = str[5];
        kwd = str[6]/1000.0;

        lmn = str[7];
	lmx = str[8];
	lnc = str[9];
	lon = str[10];
        lcl = str[11];
        lst = str[12];
        lwd = str[13]/1000.0;};

    PG_get_line_color(dev, &lc);
    PG_get_line_style(dev, &ls);
    PG_get_line_width(dev, &lw);

/* draw the k lines */
    if (kon == TRUE)
       {PG_set_line_color(dev, kcl);
	PG_set_line_style(dev, kst);
	PG_set_line_width(dev, kwd);

	for (l = lmn; l <= lmx; l += lnc)
	    for (k = kmn; k < kmx; k++)
	        {lb  = (lmn == lmx) ? 0 : l - 1;
		 i   = l*kmax + k;
		 imp = (l == lmx) ? lb*km + k : l*km + k;
		 imq = (l == lmn) ? l*km + k : lb*km + k;
		 if ((emap != NULL) && (emap[imp] == 0) && (emap[imq] == 0))
		    continue;

		 i1 = i;
		 i2 = i + 1;

		 x1[0] = rx[i1];
		 x1[1] = refl*ry[i1];
		 x2[0] = rx[i2];
		 x2[1] = refl*ry[i2];
		 PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};};

/* draw the l lines */
    if (lon == TRUE)
       {PG_set_line_color(dev, lcl);
	PG_set_line_style(dev, lst);
	PG_set_line_width(dev, lwd);

	for (k = kmn; k <= kmx; k += knc)
	    for (l = lmn; l < lmx; l++)
	        {kb  = (kmn == kmx) ? 0 : k - 1;
		 i   = l*kmax + k;
		 imp = (k == kmx) ? l*km + kb : l*km + k;
		 imq = (k == kmn) ? l*km + k : l*km + kb;
		 if ((emap != NULL) && (emap[imp] == 0) && (emap[imq] == 0))
		    continue;

		 i1 = i + kmax;
		 i2 = i;

		 x1[0] = rx[i1];
		 x1[1] = refl*ry[i1];
		 x2[0] = rx[i2];
		 x2[1] = refl*ry[i2];
		 PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};};

    PG_set_line_color(dev, lc);
    PG_set_line_style(dev, ls);
    PG_set_line_width(dev, lw);

    if (eflag)
       SFREE(emap);

    if (str != NULL)
       SFREE(str);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_AC_2D - draw a 2d arbitrarily connected domain mesh */

static void _PG_dom_ac_2d(PG_device *dev, double *rx, double *ry,
			  void *cnnct, pcons *alist)
   {int nd,is, os, iz, oz, is1, is2, in1, in2;
    int ok_ids, mark, clr, mcnt;
    int *nc, nz, *np, nzp, nsp, ofs, ofz;
    long zt[2];
    long **cells, *zones, *sides;
    double ave[PG_SPACEDM];
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PM_mesh_topology *mt;

    mt = (PM_mesh_topology *) cnnct;

    nd    = mt->n_dimensions;
    nc    = mt->n_cells;
    np    = mt->n_bound_params;
    cells = mt->boundaries;
	
    switch (nd)
       {case 1 :
	     zt[0] = 0;
	     zt[1] = nc[1] - 1;
	     zones = zt;
	     nzp   = 1;
	     nz    = 0;
	     ofz   = FALSE;
	     break;

	default :
        case 2  :
	     zones = cells[2];
	     nzp   = np[2];
	     nz    = nc[2] - 1;
	     ofz   = (nzp < 2);
	     break;};

    sides = cells[1];
    nsp   = np[1];

/* NOTE: this routine may be drawing for the new contour plotter
 * consequently we may want data_ids
 * the data_id will be driven off the color index instead of the
 * level index since we only will have the former
 */
    ok_ids = dev->data_id;
    if (ok_ids)
       {PG_get_line_color(dev, &clr);
	mark = 'A' + clr - dev->BLUE + 1;
	mcnt = 0;};
  
    ofs = (nsp < 2);
    for (iz = 0; iz <= nz; iz++)
        {oz  = iz*nzp;
	 is1 = zones[oz];
	 is2 = zones[oz+1] - ofz;

	 for (is = is1; is <= is2; is++)
	     {os  = is*nsp;
	      in1 = sides[os];
	      in2 = (ofs && (is == is2)) ? sides[is1] : sides[os+1];

	      x1[0] = rx[in1];
	      x1[1] = ry[in1];
	      x2[0] = rx[in2];
	      x2[1] = ry[in2];
	      PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);

	      if (ok_ids)
		 {if (mcnt > 9)
		     {mcnt = 0;
		      ave[0] = 0.5*(rx[in1] + rx[in2]);
		      ave[1] = 0.5*(ry[in1] + ry[in2]);
		      PG_write_n(dev, 2, WORLDC, ave, "%c", mark);}
		  else
		     mcnt++;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_2D - draw a 2d domain mesh */

static void _PG_dom_2d(PG_device *dev, PM_set *dom, PM_set *ran)
   {int npts, color, style;
    double width;
    double **r, **d, *f;
    void *cnnct;
    pcons *alst;
    PM_set *p;

    if (dev == NULL)
       return;

    for (p = dom; p != NULL; p = p->next)
        {npts = p->n_elements;
	 alst = (pcons *) p->info;
	 r    = (double **) p->elements;

	 PG_get_attrs_set(p,
			  "LINE-COLOR", SC_INT_I,    &color, dev->BLUE,
			  "LINE-STYLE", SC_INT_I,    &style, LINE_SOLID,
			  "LINE-WIDTH", SC_DOUBLE_I, &width, 0.0,
			  NULL);

         if (style != LINE_NONE)
	    {PG_set_color_line(dev, color, TRUE);
	     PG_set_line_width(dev, width);
	     PG_set_line_style(dev, style);

	     d = PM_convert_vectors(2, npts, r, p->element_type);

	     if (ran != NULL)
	        {npts = ran->n_elements;
		 r    = (double **) ran->elements;
		 f    = PM_array_real(ran->element_type, r[0], npts, NULL);}
	     else
	        f = NULL;

	     if (p->topology == NULL)
	        {cnnct = (void *) p->max_index;
		 _PG_dom_lr_2d(dev, d[0], d[1], cnnct, alst);}

	     else if (strcmp(p->topology_type, PM_MESH_TOPOLOGY_P_S) == 0)
	        {cnnct = p->topology;
		 _PG_dom_ac_2d(dev, d[0], d[1], cnnct, alst);};

	     PM_free_vectors(2, d);
	     SFREE(f);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_LR_3D - draw a 3d logically rectangular domain mesh */

static void _PG_dom_lr_3d(PG_device *dev, double **r,
			  double *dextr, void *cnnct, pcons *alist)
   {int i, j, k, i1, i2, n, np, ns;
    int imax, jmax, kmax, eflag, stride;
    int imn, imx, inc, ion, icl, ist;
    int jmn, jmx, jnc, jon, jcl, jst;
    int kmn, kmx, knc, kon, kcl, kst;
    int lc, ls;
    int *maxes, *str;
    char *emap;
    double iwd, jwd, kwd, lw;
    double box[PG_BOXSZ];
    double **t;

    maxes = (int *) cnnct;
    imax  = maxes[0];
    jmax  = maxes[1];
    kmax  = maxes[2];
    n     = imax*jmax*kmax;

    np = 6*n - 2*(jmax*kmax + imax*kmax + imax*jmax);

    t = PM_make_vectors(3, np);

    PG_get_attrs_alist(alist,
		       "EXISTENCE", SC_POINTER_I, &emap, NULL,
		       "MESH-SUBSELECT", SC_POINTER_I, &str, NULL,
		       NULL);

    eflag = (emap == NULL);
    if (eflag)
       {emap = FMAKE_N(char, n, "_PG_DOM_LR_3D:emap");
	memset(emap, 1, n);}
    else
       PM_CHECK_EMAP(alist, n, eflag, emap);

    if (str == NULL)
       {imn = 0;
	imx = imax - 1;
	inc = 1;
	ion = TRUE;
        icl = dev->line_color;
        ist = dev->line_style;
        iwd = dev->line_width;

        jmn = 0;
	jmx = jmax - 1;
	jnc = 1;
	jon = TRUE;
        jcl = dev->line_color;
        jst = dev->line_style;
        jwd = dev->line_width;

        kmn = 0;
	kmx = kmax - 1;
	knc = 1;
	kon = TRUE;
        kcl = dev->line_color;
        kst = dev->line_style;
        kwd = dev->line_width;}

/* entries in str are: start, stop, step, on/off, clr, sty, 1000*wid */
    else
       {imn = str[0];
	imx = str[1];
	inc = str[2];
	ion = str[3];
        icl = str[4];
        ist = str[5];
        iwd = str[6]/1000.0;

        jmn = str[7];
	jmx = str[8];
	jnc = str[9];
	jon = str[10];
        jcl = str[11];
        jst = str[12];
        jwd = str[13]/1000.0;

        kmn = str[14];
	kmx = str[15];
	knc = str[16];
	kon = str[17];
        kcl = str[18];
        kst = str[19];
        kwd = str[20]/1000.0;};

    PG_ADJUST_LIMITS_3D(dextr, box);

    PG_get_line_color(dev, &lc);
    PG_get_line_style(dev, &ls);
    PG_get_line_width(dev, &lw);

/* lines in the i direction */
    stride = 1;
    if (ion == TRUE)
       {np = 0;
	for (k = kmn; k <= kmx; k += knc)
	    for (j = jmn; j <= jmx; j += jnc)
	        for (i = imn; i < imx; i += inc)
		    {i1 = (k*jmax + j)*imax + i;
		     i2 = i1 + stride;
		     np = _PG_save_seg(i1, i2, np, r, t, box);};

	PG_set_line_color(dev, icl);
	PG_set_line_style(dev, ist);
	PG_set_line_width(dev, iwd);

	ns = np >> 1;
	PG_draw_disjoint_polyline_n(dev, 3, WORLDC, ns, t, TRUE);};

/* lines in the j direction */
    stride *= imax;
    if (jon == TRUE)
       {np = 0;
	for (k = kmn; k <= kmx; k += knc)
	    for (i = imn; i <= imx; i += inc)
	        for (j = jmn; j < jmx; j += jnc)
		    {i1 = (k*jmax + j)*imax + i;
		     i2 = i1 + stride;
		     np = _PG_save_seg(i1, i2, np, r, t, box);};

	PG_set_line_color(dev, jcl);
	PG_set_line_style(dev, jst);
	PG_set_line_width(dev, jwd);

	ns = np >> 1;
	PG_draw_disjoint_polyline_n(dev, 3, WORLDC, ns, t, TRUE);};
                
/* lines in the k direction */
    stride *= jmax;
    if (kon == TRUE)
       {np = 0;
	for (j = jmn; j <= jmx; j += jnc)
	    for (i = imn; i <= imx; i += inc)
	        for (k = kmn; k < kmx; k += knc)
		    {i1 = (k*jmax + j)*imax + i;
		     i2 = i1 + stride;
		     np = _PG_save_seg(i1, i2, np, r, t, box);};

	PG_set_line_color(dev, kcl);
	PG_set_line_style(dev, kst);
	PG_set_line_width(dev, kwd);

	ns = np >> 1;
	PG_draw_disjoint_polyline_n(dev, 3, WORLDC, ns, t, TRUE);};

    PG_set_line_color(dev, lc);
    PG_set_line_style(dev, ls);
    PG_set_line_width(dev, lw);

    if (eflag)
       SFREE(emap);

    PM_free_vectors(3, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_AC_3D - draw a 3d arbitrarily connected domain mesh */

static void _PG_dom_ac_3d(PG_device *dev, double **r,
			  double *dextr, void *cnnct, pcons *alist)
   {int jz, jf, je, jf1, jf2, je1, je2, jn1, jn2;
    int oz, of, oe, nn, ns, nz, nf, ne, oad;
    int *nc, *np, nzp, nfp, nep, ofz, off, ofe;
    long **cells, *zones, *faces, *edges;
    double box[PG_BOXSZ];
    double **t;
    PM_mesh_topology *mt;

    mt = (PM_mesh_topology *) cnnct;

    cells = mt->boundaries;
    nc    = mt->n_cells;
    np    = mt->n_bound_params;

    ne = nc[1];

    t = PM_make_vectors(3, 2*ne);

    PG_ADJUST_LIMITS_3D(dextr, box);

    nn = 0;
    switch (mt->n_dimensions)
        {case 3 :
	      zones = cells[3];
	      faces = cells[2];
	      edges = cells[1];

	      nz  = nc[3];
	      nf  = nc[2];
	      nzp = np[3];
	      nfp = np[2];
	      nep = np[1];
	      ofz = (nzp < 2);
	      off = 2*(nfp < 2);
	      ofe = (nep < 2);

	      for (jz = 0; jz < nz; jz++)
		  {oz  = jz*nzp;
		   jf1 = zones[oz];
		   jf2 = zones[oz+1] - ofz;
		   for (jf = jf1; jf <= jf2; jf++)
		       {of  = jf*nfp;
			je1 = faces[of];
			je2 = faces[of+1] - off;
			for (je = je1; je <= je2; je++)
			    {oe  = je*nep;
			     jn1 = edges[oe];
			     jn2 = (ofe && (je == je2)) ? edges[je1] : edges[oe+1];
			     nn  = _PG_save_seg(jn1, jn2, nn, r, t, box);};};};
	      break;

	 case 2 :
	      faces = cells[2];
	      edges = cells[1];

	      nf  = nc[2];
	      nfp = np[2];
	      nep = np[1];
	      off = 2*(nfp < 2);
	      ofe = (nep < 2);

	      for (jf = 0; jf < nf; jf++)
		  {of  = jf*nfp;
		   je1 = faces[of];
		   je2 = faces[of+1] - off;
		   for (je = je1; je <= je2; je++)
		       {oe  = je*nep;
			jn1 = edges[oe];
			jn2 = (ofe && (je == je2)) ? edges[je1] : edges[oe+1];
			nn  = _PG_save_seg(jn1, jn2, nn, r, t, box);};};
	      break;

	 case 1 :
	      edges = cells[1];

	      nep = np[1];
	      ofe = (nep < 2);

	      for (je = 0; je < ne; je++)
		  {oe  = je*nep;
		   jn1 = edges[oe];
		   jn2 = edges[oe+1];
		   nn  = _PG_save_seg(jn1, jn2, nn, r, t, box);};
	      break;};

    ns = nn >> 1;

    oad = dev->autodomain;
    dev->autodomain = FALSE;

    PG_draw_disjoint_polyline_n(dev, 3, WORLDC, ns, t, dev->autodomain);
    dev->autodomain = oad;

    PM_free_vectors(3, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_AC_3D_SHADED - draw shaded faces of a 3d domain mesh */

static void _PG_dom_ac_3d_shaded(PG_device *dev, double **r,
				 void *cnnct, pcons *alist)
   {int i, id, in, nd, nt, color;
    double ctl, stl, cpl, spl;
    double thl, phl, thld, phld;
    double v, hrz;
    double ls[PG_SPACEDM], p[PG_SPACEDM], ext[2];
    double **s;
    char *pal;
    PG_node *node;
    PG_triangle *tri, *b;

    if (dev == NULL)
       return;

    nd = 3;

    tri = _PG_hls_remove(dev, PM_MESH_TOPOLOGY_S, r, NULL, cnnct, alist, &nt);
    if (tri == NULL)
       return;

    PG_get_light_angle(dev, TRUE, &thld, &phld);

/* check for applicable attributes */
    PG_get_attrs_alist(alist,
		       "THETA-LIGHT", SC_DOUBLE_I,  &thl, thld,
		       "PHI-LIGHT",   SC_DOUBLE_I,  &phl, phld,
		       "PALETTE",     SC_POINTER_I, &pal, _PG.palette,
		       NULL);

    thl *= DEG_RAD;
    phl *= DEG_RAD;

    PG_set_palette(dev, pal);

    ctl = cos(thl);
    cpl = cos(phl);
    stl = sin(thl);
    spl = sin(phl);

/* NOTE: we rotate the light source by phi from negative y axis! */
    ls[0] =  stl*spl;
    ls[1] = -stl*cpl;
    ls[2] = ctl;

    hrz = -0.2;

    s = PM_make_vectors(nd, 20);

/* color range is 0 to 1 - the cosine of the incident angle */
    ext[0] = 0.0;
    ext[1] = 1.0;

    for (i = 0; i < nt; i++)
        {b = tri + i;

/* get out the nodes and the normal for this triangle */
	 for (in = 0; in < 3; in++)
	     {node = &TRI_NODE(b, in);
	      for (id = 0; id < nd; id++)
		  s[id][in] = node->x[id];};

         for (id = 0; id < nd; id++)
	     {s[id][3] = s[id][0];
	      p[id]    = b->norm[id];};

/* compute the incident angle of the light source */
	 v = 0.0;
	 for (id = 0; id < nd; id++)
	     v += p[id]*ls[id];

/* if below the horizon defined by hrz draw it in black */
	 if (v < hrz)
            color = dev->WHITE;

/* else if above the horizon pick the color from the palette */
         else
	    {v     = ABS(v);
	     color = PG_select_color(dev, 1, &v, ext);};

	 PG_fill_polygon_n(dev, color, TRUE, nd, WORLDC, 4, s);

	 if (dev->draw_fill_bound)
	    {PG_set_line_color(dev, dev->WHITE);
	     PG_draw_polyline_n(dev, 2, WORLDC, 4, s, TRUE);};};

    PM_free_vectors(nd, s);

    SFREE(tri);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_0D_3D - do a 3d scatter plot */

static void _PG_dom_0d_3d(PG_device *dev, long npts,
			  double **r, double *dextr,
			  pcons *alst)
   {int i, id, n, mrk;
    double u[PG_SPACEDM];
    double wc[PG_BOXSZ];
    double **t;
    PG_dev_geometry *g;

    g = &dev->g;

    PG_get_attrs_alist(alst,
		       "MARKER-INDEX", SC_INT_I,    &mrk,  0,
		       "MARKER-SCALE", SC_DOUBLE_I, &dev->marker_scale, dev->marker_scale,
		       NULL);

    t = PM_copy_vectors(3, npts, r);

/* get rid of points outside the domain limits */
    PG_box_copy(3, wc, dextr);
    for (i = 0, n = 0; i < npts; i++)
        {for (id = 0; id < 3; id++)
	     u[id] = r[id][i];

         if (PG_box_contains(3, wc, u) == TRUE)
	    {for (id = 0; id < 3; id++)
                 t[id][n] = t[id][i];
             n++;};};

    PG_draw_markers_n(dev, 3, WORLDC, n, t, mrk);

    PM_free_vectors(3, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LABEL_NODES_3 - label the nodes in a mesh plot */

static void PG_label_nodes_3(PG_device *dev, double **x,
			     long n, void *f, char *type)
   {int i, id;
    double box[PG_BOXSZ], p[PG_SPACEDM];
    double **r;
    char lbl[MAXLINE], ltype[MAXLINE];
    char *s;
    PG_dev_geometry *g;

    if (dev == NULL)
       return;

    g = &dev->g;

    PG_make_device_current(dev);

    r = PM_copy_vectors(3, n, x);

    PG_rotate_vectors(dev, 3, n, r);

/* NOTE: temporary for diagnostics */
    PM_vector_extrema(3, n, r, box);

    strcpy(ltype, type);
    SC_strtok(ltype, " *", s);
    if (ltype != NULL)
       {id = SC_type_id(ltype, FALSE);

/* fixed point types (proper) */
	if (SC_is_type_fix(id) == TRUE)
	   {int *fi;

	    fi = SC_convert_id(SC_INT_I, NULL, 0, 1, id, f, 0, 1, n, FALSE);
	    for (i = 0; i < n; i++)
	        {snprintf(lbl, MAXLINE, "%d", fi[i]);
		 p[0] = r[0][i];
		 p[1] = r[1][i];
		 PG_write_n(dev, 2, WORLDC, p, lbl);};

	    SFREE(fi);}

/* floating point types (proper) */
       else if (SC_is_type_fp(id) == TRUE)
	  {double *fi;

	   fi = PM_array_real(type, f, n, NULL);
	   for (i = 0; i < n; i++)
	       {snprintf(lbl, MAXLINE, _PG_gattrs.text_format, fi[i]);
		p[0] = r[0][i];
		p[1] = r[1][i];
		PG_write_n(dev, 2, WORLDC, p, lbl);};

	   SFREE(fi);}

       else if (id == SC_CHAR_I)
	  {char t[2], *fi;

	   t[1] = '\0';
	   fi   = f;
	   for (i = 0; i < n; i++)
	       {t[0] = fi[i];
		p[0] = r[0][i];
		p[1] = r[1][i];
		PG_write_n(dev, 2, WORLDC, p, t);};

	   SFREE(fi);};

	PM_free_vectors(3, r);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_3D_DOMAIN - draw a single domain set */

static void _PG_draw_3d_domain(PG_device *dev, PM_set *dom, PM_set *ran,
			       double *dextr, double **r)
   {long ne;
    void *cnnct;
    pcons *alst;

    if ((dev == NULL) || (dom == NULL))
       return;

/* get the options for this mesh */
    PG_get_render_info(dom, alst);

    ne = dom->n_elements;

    if (_PG.scatter || (dom->dimension == 0))
       _PG_dom_0d_3d(dev, ne, r, dextr, alst);

    else if (dom->topology == NULL)
       {cnnct = (void *) dom->max_index;
        _PG_dom_lr_3d(dev, r, dextr, cnnct, alst);}

    else if (strcmp(dom->topology_type, PM_MESH_TOPOLOGY_P_S) == 0)
       {cnnct = dom->topology;
        if (_PG.ptype == PLOT_SURFACE)
	   _PG_dom_ac_3d_shaded(dev, r, cnnct, alst);
        else
           _PG_dom_ac_3d(dev, r, dextr, cnnct, alst);}

    else if ((_PG.ptype == PLOT_MESH) && (ran != NULL))
       {void **f;

        f = (void **) ran->elements;
	PG_label_nodes_3(dev, r, ne, f[0], ran->element_type);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DOM_3D - draw a 3d domain mesh */

static void _PG_dom_3d(PG_device *dev, PM_set *dom, PM_set *ran)
   {int n, nd, color, style, alloc;
    double width;
    double *dextr;
    double **r, **t;
    PM_set *p;

    if (dev == NULL)
       return;

    nd = 3;

    dextr = NULL;
    if (dev->autodomain == FALSE)
       dextr = PM_get_limits(dom);

    alloc = (dextr == NULL);

    if (alloc == TRUE)
       dextr = PM_set_extrema(dom);

/* setup the view space */
    for (p = dom; p != NULL; p = p->next)
        {n = p->n_elements;
	 r = (double **) p->elements;

	 PG_get_attrs_set(p,
			  "LINE-COLOR", SC_INT_I,    &color, dev->WHITE,
			  "LINE-STYLE", SC_INT_I,    &style, LINE_SOLID,
			  "LINE-WIDTH", SC_DOUBLE_I, &width, 0.0,
			  NULL);

	 PG_set_color_line(dev, color, TRUE);
	 PG_set_line_style(dev, style);
	 PG_set_line_width(dev, width);

	 t = PM_convert_vectors(nd, n, r, p->element_type);

	 _PG_draw_3d_domain(dev, p, ran, dextr, t);

	 PM_free_vectors(nd, t);};

    if (alloc == TRUE)
       {SFREE(dextr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_MESH - setup a window for a mesh rendering
 *                       - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_mesh(PG_device *dev, PG_graph *data,
				       int save, int clear)
   {int nde, clr, sty, axfl, clip, change;
    double wid, th, ph, ch, thl, phl;
    double *dpex, *ddex, *pdx;
    double *ndc;
    PM_set *dom;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;

    if (dev == NULL)
       return(NULL);

    dom = data->f->domain;

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);

    alst = pd->alist;
    PG_get_attrs_alist(alst,
		       "PLOT-TYPE",   SC_INT_I,     &_PG.ptype,   PLOT_MESH,
		       "LINE-COLOR",  SC_INT_I,     &clr,      dev->WHITE,
		       "LINE-STYLE",  SC_INT_I,     &sty,      LINE_SOLID,
		       "LINE-WIDTH",  SC_DOUBLE_I,  &wid,      0.0,
		       "THETA-LIGHT", SC_DOUBLE_I,  &thl,      45.0,
		       "PHI-LIGHT",   SC_DOUBLE_I,  &phl,      45.0,
		       "CENTERING",   SC_INT_I,     (int *) &_PG.cntrg,   N_CENT,
		       "PALETTE",     SC_POINTER_I, &_PG.palette, dev->current_palette->name,
		       "SCATTER",     SC_INT_I,     &_PG.scatter, _PG_gattrs.scatter_plot,
		       "THETA",       SC_DOUBLE_I,  &th,       0.0,
		       "PHI",         SC_DOUBLE_I,  &ph,       0.0,
		       "CHI",         SC_DOUBLE_I,  &ch,       0.0,
		       NULL);

    pd->legend_palette_fl = FALSE;
    pd->legend_contour_fl = FALSE;

    pri = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = PLOT_MESH;};};

    if (change)
       {ndc = pd->viewport;
	if (ndc != NULL)
	   PG_set_viewspace(dev, 2, NORMC, ndc);

/* find the extrema for this frame */
	PG_find_extrema(data, 0.0, &dpex, NULL, &nde, &ddex, NULL, NULL);

/* setup the domain limits */
	if (dev->autodomain == TRUE)
	   {pdx = (dpex == NULL) ? ddex : dpex;
	    PG_set_viewspace(dev, nde, WORLDC, pdx);};

	SFREE(ddex);

	axfl = pd->axis_fl;
	if (axfl && (nde == 3))
	   pd->ax_type = CARTESIAN_3D;

	if (nde == 3)
	   {clip = FALSE;
	    dev->autodomain = (dpex == NULL);
	    dev->autorange  = (dpex == NULL);}
	else
	   clip = TRUE;

	PG_set_clipping(dev, clip);};

    PG_set_view_angle(dev, th, ph, ch);
    PG_set_light_angle(dev, thl, phl);

    PG_set_palette(dev, "standard");

    PG_set_color_text(dev, dev->WHITE, TRUE);
    PG_set_line_color(dev, clr);
    PG_set_line_width(dev, wid);
    PG_set_line_style(dev, sty);

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DOMAIN_PLOT - plot the mesh of the given domain set */

void PG_domain_plot(PG_device *dev, PM_set *dom, PM_set *ran)
   {int color, style;
    double width;
    PM_mapping f;
    PG_graph g, *data;
    PG_picture_desc *pd;

    if (dev == NULL)
       return;

    memset(&f, 0, sizeof(PM_mapping));
    f.domain = dom;
    f.name   = dom->name;

    data = &g;
    memset(data, 0, sizeof(PG_graph));
    g.f         = &f;
    g.rendering = PLOT_MESH;
    g.info_type = SC_PCONS_P_S;
    g.info      = dom->info;

    pd = PG_setup_picture(dev, data, TRUE, TRUE, TRUE);

    PG_adorn_before(dev, pd, data);

    PG_get_attrs_set(dom,
		     "LINE-COLOR",   SC_INT_I,    &color, dev->WHITE,
		     "LINE-STYLE",   SC_INT_I,    &style, LINE_SOLID,
		     "LINE-WIDTH",   SC_DOUBLE_I, &width, 0.0,
		     NULL);

    PG_set_color_line(dev, color, TRUE);
    PG_set_line_width(dev, width);
    PG_set_line_style(dev, style);
    PG_set_clipping(dev, TRUE);

    PG_set_view_angle(dev, pd->va[0], pd->va[1], pd->va[2]);

/* plot the domain */
    switch (dom->dimension_elem)
       {case 1  : 
	     _PG_dom_1d(dev, dom, ran);
	     break;

        case 2  :
	     _PG_dom_2d(dev, dom, ran);
	     break;

        case 3 :
	     _PG_dom_3d(dev, dom, ran);
	     break;};

    if (_PG_gattrs.ref_mesh == FALSE)
       PG_finish_picture(dev, data, pd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_MESH_PLOT - plot the domain mesh and maybe the range values */

void PG_mesh_plot(PG_device *dev, PG_graph *data, ...)
   {int nde;
    PG_graph *g;
    PM_mapping *f;
    PM_set *domain;
    void *info;
    pcons *mlst, *dlst, *tlst;
    double *extr, *dextr;
    int color;

    SC_VA_START(data);

    for (g = data; g != NULL; g = g->next)
        {for (f = g->f; f != NULL; f = f->next)
	     {domain = f->domain;

/* save the domain info to restore it later */
	      info = domain->info;

/* make a master list of attributes from the mapping, domain, and graph */
	      if (strcmp(f->map_type, SC_PCONS_P_S) == 0)
		 mlst = SC_copy_alist((pcons *) f->map);
	      else
		 mlst = NULL;

	      dlst = SC_copy_alist((pcons *) domain->info);
	      tlst = SC_copy_alist((pcons *) g->info);
	      tlst = SC_append_alist(tlst, mlst);
	      tlst = SC_append_alist(tlst, dlst);

	      if (f == g->f)
		 {extr = PM_get_limits(domain);
		  PG_get_attrs_alist(tlst,
				     "LINE-COLOR", SC_INT_I, &color, dev->BLUE,
				     NULL);}
	      else
		 tlst = PG_set_attrs_alist(tlst,
					   "DRAW-AXIS",  SC_INT_I, FALSE, FALSE,
					   "DRAW-LABEL", SC_INT_I, FALSE, FALSE,
					   "LINE-COLOR", SC_INT_I, FALSE, ++color,
					   NULL);

/* temporarily make the domain info be the master list */
	      domain->info = (void *) tlst;
	      if (f != g->f)
		 {nde   = 2*domain->dimension;
		  dextr = FMAKE_N(double, nde, "PG_MESH_PLOT:dextr");
		  memcpy(dextr, extr, nde*sizeof(double));
		  PM_set_limits(domain, dextr);};

	      PG_domain_plot(dev, domain, f->range);

/* free the master list
 * NOTE: because of the way SC_append_alist works we don't need
 *       to free mlst and dlst explicitly
 */
	      SC_free_alist(tlst, 3);

/* restore the domain info */
	      domain->info = info;};};

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/

/*                   DOMAIN BOUNDARY PLOT ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _PG_DRAW_BND_LR_2D - draw boundary around 2d LR domain */

static void _PG_draw_bnd_lr_2d(PG_device *dev, PM_set *domain, char *emap)
   {int i, j, k, l, m, npts, clip;
    int  km, lm, kmm, lmm, kmax, lmax;
    int *maxes;
    char *e1, *e2, *e3, *e4;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double **r, **d;

    npts  = domain->n_elements;
    maxes = domain->max_index;
    r     = (double **) domain->elements;
    d     = PM_convert_vectors(2, npts, r, domain->element_type);

    clip = dev->clipping;

    kmax = maxes[0];
    lmax = maxes[1];
    km   = kmax - 1;
    lm   = lmax - 1;
    kmm  = km - 1;
    lmm  = lm - 1;

    e1 = emap + 1;
    e2 = emap + km;
    e3 = emap - 1;
    e4 = emap - km;

    for (l = 0; l < lm; l++)
        for (k = 0; k < km; k++)
	    {m = l*km + k;
	     if (emap[m])
	        {if ((k == kmm) || !e1[m])
		    {i  = l*kmax + k + 1;
		     j  = i + kmax;
		     x1[0] = d[0][i];
		     x1[1] = d[1][i];
		     x2[0] = d[0][j];
		     x2[1] = d[1][j];
		     PG_draw_line_n(dev, 2, WORLDC, x1, x2, clip);};

		 if ((l == lmm) || !e2[m])
		    {i  = (l + 1)*kmax + k + 1;
		     j  = i - 1;
		     x1[0] = d[0][i];
		     x1[1] = d[1][i];
		     x2[0] = d[0][j];
		     x2[1] = d[1][j];
		     PG_draw_line_n(dev, 2, WORLDC, x1, x2, clip);};

		 if ((k == 0) || !e3[m])
		    {i  = (l + 1)*kmax + k;
		     j  = i - kmax;
		     x1[0] = d[0][i];
		     x1[1] = d[1][i];
		     x2[0] = d[0][j];
		     x2[1] = d[1][j];
		     PG_draw_line_n(dev, 2, WORLDC, x1, x2, clip);};

		 if ((l == 0) || !e4[m])
		    {i  = l*kmax + k;
		     j  = i + 1;
		     x1[0] = d[0][i];
		     x1[1] = d[1][i];
		     x2[0] = d[0][j];
		     x2[1] = d[1][j];
		     PG_draw_line_n(dev, 2, WORLDC, x1, x2, clip);};};};

    PM_free_vectors(2, d);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_DOMAIN_BOUNDARY - draw the domain boundary of the mapping F
 *                         - if the attribute of the graph DATA demands
 */

void PG_draw_domain_boundary(PG_device *dev, PM_mapping *f)
   {int nd, cl, clr, sty, dbclr, dbsty, eflag;
    char *emap;
    double wid, dbwid;
    double wc[PG_BOXSZ], extr[PG_BOXSZ];
    PM_set *domain;
    PG_palette *pl;

    if (dev == NULL)
       return;

    domain = f->domain;
    nd     = domain->dimension;
    pl     = dev->current_palette;

    PG_set_palette(dev, "standard");

    PG_get_attrs_set(domain,
		     "DOMAIN-BORDER-WIDTH", SC_DOUBLE_I, &dbwid, -1.0,
		     "DOMAIN-BORDER-COLOR", SC_INT_I,    &dbclr, dev->WHITE,
		     "DOMAIN-BORDER-STYLE", SC_INT_I,    &dbsty, LINE_SOLID,
		     NULL);

    if (dbwid > -1.0)
       {PG_get_line_color(dev, &clr);
	PG_get_line_style(dev, &sty);
	PG_get_line_width(dev, &wid);
	PG_get_clipping(dev, &cl);

	PG_set_clipping(dev, TRUE);
	PG_set_line_color(dev, dbclr);
	PG_set_line_style(dev, dbsty);
	PG_set_line_width(dev, dbwid);

	LR_MAPPING_INFO(f->map, f->range->n_elements);

	PM_array_real(domain->element_type, domain->extrema, 2*nd, extr);

/* GOTCHA: this needs to be reworked for domains other than
 *         rectangular ones
 */
	switch (nd)
	   {case 1 :
		 PG_get_viewspace(dev, WORLDC, wc);
		 wc[0] = extr[0];
		 wc[1] = extr[1];
		 PG_draw_box_n(dev, 2, WORLDC, wc);
	         break;

	    case 2 :
	         if (strcmp(f->category, PM_LR_S) == 0)
		    _PG_draw_bnd_lr_2d(dev, domain, emap);
		 break;

	    case 3 :
	         break;};

	if (eflag)
	   SFREE(emap);

	PG_set_clipping(dev, cl);
	PG_set_line_color(dev, clr);
	PG_set_line_style(dev, sty);
	PG_set_line_width(dev, wid);};

    dev->current_palette = pl;

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_REF_MESH - plot the domain mesh and maybe the range values */

void PG_ref_mesh(PG_device *dev, PG_graph *data, int ndims,
		 double *bx, double *ndc)
   {int save_axis_on, save_title_on, mshf, clp;
    PM_mapping *f;
    PM_set *domain;
    double *pv;
    int *pc;
    
    if (dev == NULL)
       return;

    mshf = data->mesh;
    if (mshf || (_PG_gattrs.ref_mesh == TRUE))
       {data->mesh = FALSE;

/* set the viewport to incoming values */
	if (ndc == NULL)
	   {pv = FMAKE_N(double, 4, "PG_REF_MESH:pv");
	    PG_box_copy(2, pv, bx);
	    PG_set_attrs_graph(data,
			       "VIEW-PORT", SC_DOUBLE_I, TRUE, pv,
			       NULL);};

/* save some state */
	save_axis_on  = _PG_gattrs.axis_on;
	save_title_on = _PG_gattrs.plot_title;
	PG_get_clipping(dev, &clp);
	PG_set_clipping(dev, TRUE);

/* don't plot axis or title for reference mesh */
	_PG_gattrs.axis_on    = FALSE;
	_PG_gattrs.plot_title = FALSE;

	f      = data->f;
	domain = f->domain;

/* save the line color */
	PG_get_attrs_set(domain,
			 "LINE-COLOR", SC_POINTER_I, &pc, NULL,
			 NULL);

	_PG_gattrs.ref_mesh_color = _PG_trans_color(dev, _PG_gattrs.ref_mesh_color);

/* set the line color for the reference mesh */
	PG_set_attrs_set(domain,
			 "LINE-COLOR", SC_INT_I, FALSE, _PG_gattrs.ref_mesh_color,
			 NULL);

	PG_mesh_plot(dev, data, ndims);

/* restore the state */
	_PG_gattrs.axis_on    = save_axis_on;
	_PG_gattrs.plot_title = save_title_on;
	PG_set_clipping(dev, clp);

	if (ndc == NULL)
	   {PG_rem_attrs_graph(data, "VIEW-PORT", NULL);
	    SFREE_N(pv, 4);};

	if (pc == NULL)
	   PG_rem_attrs_set(domain, "LINE-COLOR", NULL);
	else
	   PG_set_attrs_set(domain,
			    "LINE-COLOR", SC_INT_I, TRUE, pc,
			    NULL);

	data->mesh = mshf;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
