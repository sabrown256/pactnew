/*
 * GSDVB.C - discrete value boundary rendering routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

typedef struct s_PG_dvb_data PG_dvb_data;

struct s_PG_dvb_data
   {PG_device *dev;
    int nd;
    int *a;
    int *aext;
    double *x;
    double *y;
    void *cnnct;
    pcons *alist;};


typedef void (*PFDvbZC)(PG_device *dev, int nd, int *a,
                        double *x, double *y, int npts, int *aext,
                        void *cnnct, pcons *alist);

typedef void (*PFDvbNC)(PG_device *dev, int nd, int *a,
                        double *x, double *y, int npts, int *aext,
                        void *cnnct, pcons *alist);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_DV_BND - setup a window for a dvb rendering
 *                         - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_dv_bnd(PG_device *dev, PG_graph *data,
					 int save, int clear)
   {int nde, nre, change;
    double ndc[PG_BOXSZ];
    double *dpex, *ddex, *pdx, *rpex, *rdex, *prx, *vwprt;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);

    pd->legend_palette_fl = FALSE;
    pd->legend_contour_fl = FALSE;

    alst = pd->alist;
    pri  = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = PLOT_DV_BND;};};

/* setup the viewport */
    vwprt = pd->viewport;
    if (vwprt != NULL)
       PG_box_copy(2, ndc, vwprt);
    else
       PG_box_init(2, ndc, 0.175, 0.825);

    if (change)
       {PG_set_viewspace(dev, 2, NORMC, ndc);

/* find the extrema for this frame */
	PG_find_extrema(data, 0.0, &dpex, &rpex, &nde, &ddex, &nre, &rdex);

/* setup the range limits */
	prx = ((dev->autorange == TRUE) || (rpex == NULL)) ? rdex : rpex;
	PG_register_range_extrema(dev, nre, prx);

/* setup the domain limits */
	pdx = ((dev->autodomain == TRUE) || (dpex == NULL)) ? ddex : dpex;
	PG_set_viewspace(dev, 2, WORLDC, pdx);

	CFREE(ddex);
	CFREE(rdex);};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DVB_HAND - setup and dispatch to the correct DVB renderer */

static void PG_dvb_hand(PG_device *dev, PG_graph *g, PFDvbZC fnc_zc,
			PFDvbNC fnc_nc)
   {int npts, clip, prec, same, sid;
    int color, style;
    int *afd, *aext;
    char bf[MAXLINE], *mtype, *s;
    double chsp, width;
    double chup[PG_SPACEDM], chpth[PG_SPACEDM];
    double *afs, **r, **d;
    double dextr[4], *fextr, *rextr, *aextr;
    void *cnnct;
    PM_centering centering;
    PM_mapping *h;
    PM_set *domain, *range;
    PG_dev_attributes *attr;
    pcons *alst;

    if (dev == NULL)
       return;

/* extract attributes from the graph */
    PG_get_attrs_graph(g, FALSE,
		       "LINE-COLOR", SC_INT_I, &color, dev->BLUE,
		       "LINE-STYLE", SC_INT_I, &style, LINE_SOLID,
		       "LINE-WIDTH", SC_DOUBLE_I,    &width, 0.0,
		       NULL);

    PG_set_line_color(dev, color);
    PG_fset_line_style(dev, style);
    PG_fset_line_width(dev, width);

    fextr = dev->range_extrema;

/* process the mappings in the graph */
    for (h = g->f; h != NULL; h = h->next)
        {domain = h->domain;
	 npts   = domain->n_elements;
	 r      = (double **) domain->elements;

	 d = PM_convert_vectors(2, npts, r, domain->element_type);

	 PM_array_real(domain->element_type, domain->extrema, 4, dextr);

	 range = h->range;
	 npts  = range->n_elements;
	 strcpy(bf, range->element_type);
	 mtype = SC_strtok(bf, " *", s);
	 sid   = SC_type_id(mtype, FALSE);

	 same = ((mtype != NULL) && (strcmp(mtype, SC_INT_S) == 0));
	 if (same)
	    afd = *((int **) range->elements);
	 else
	    {afs = *((double **) range->elements);
	     afd = NULL;
	     afd = SC_convert_id(SC_INT_I, NULL, 0, 1,
				 sid, afs, 0, 1, npts, FALSE);};

/* find the range limits if any */
	 rextr = PM_get_limits(range);
	 aextr = ((rextr != NULL) && !dev->autorange) ? rextr : fextr;
	 aext  = SC_convert_id(SC_INT_I, NULL, 0, 1,
			       sid, aextr, 0, 1, 2, FALSE);

/* find the additional mapping information */
	 centering = N_CENT;
	 alst      = PM_mapping_info(h,
				     "CENTERING", &centering,
				     NULL);

/* save user's values for various attributes */
	 attr = PG_get_attributes(dev);

/* set attribute values */
	 clip     = TRUE;
	 prec     = TEXT_CHARACTER_PRECISION;
	 chsp     = 0.0;
	 chup[0]  = 0.0;
	 chup[1]  = 1.0;
	 chpth[0] = 1.0;
	 chpth[1] = 0.0;
	 PG_fset_clipping(dev, clip);
	 PG_fset_char_path(dev, chpth);
	 PG_fset_char_precision(dev, prec);
	 PG_fset_char_up(dev, chup);
	 PG_fset_char_space(dev, chsp);
  
	 cnnct = PM_connectivity(h);

	 switch (centering)
	    {case Z_CENT :
	          (*fnc_zc)(dev, 1, afd, d[0], d[1], npts, aext, cnnct, alst);
		  break;

	     case N_CENT :
                  (*fnc_nc)(dev, 1, afd, d[0], d[1], npts, aext, cnnct, alst);
		  break;

             case F_CENT :
             case U_CENT :
             default     :
                  break;};

	 PG_draw_domain_boundary(dev, h);

	 PG_update_vs(dev);

	 PG_fset_clipping(dev, FALSE);

/* reset user's values for various attributes */
	 PG_set_attributes(dev, attr);
	 CFREE(attr);

	 PM_free_vectors(2, d);

	 CFREE(aext);
	 if (!same)
	    CFREE(afd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DVB_CHUNK_ZC_LR - do a chunk of the DVB rendering work */

static void *_PG_dvb_chunk_zc_lr(void *arg)
   {int in, iz, k, l, nn, nmap, nz;
    int kz, lz, kzm, lzm, dk;
    int lmn, lmx, kmax, lmax, eflag, cnt;
    int a0c, a1c, a2c, a3c, a4c;
    int *a, *maxes;
    int *a1, *a2, *a3, *a4;
    double wc[PG_BOXSZ];
    double x1c[PG_SPACEDM], x2c[PG_SPACEDM];
    double x3c[PG_SPACEDM], x4c[PG_SPACEDM];
    double *x1, *x2, *x3, *x4;
    double *y1, *y2, *y3, *y4;
    double *x, *y;
    char *emap;
    void *cnnct, *rv;
    pcons *alist;
    PG_device *dev;
    PG_dvb_data *par;

    par = (PG_dvb_data *) arg;

    dev   = par->dev;
    a     = par->a;
    x     = par->x;
    y     = par->y;
    cnnct = par->cnnct;
    alist = par->alist;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    nn    = kmax*lmax;
    nmap  = (kmax - 1) * (lmax - 1);
    nz    = SC_MEM_GET_N(int, a);

    SC_ASSERT(nz > 0);
    SC_ASSERT(nn > 0);

    PG_get_attrs_alist(alist,
		       "CENTERING", SC_INT_I, &cnt, N_CENT,
		       NULL);

    emap = PM_check_emap(&eflag, alist, nmap);

    PM_LOGICAL_ZONE(x, x1, x2, x3, x4, kmax);
    PM_LOGICAL_ZONE(y, y1, y2, y3, y4, kmax);

    PG_get_viewspace(dev, WORLDC, wc);

    kz = kmax - 1;
    lz = lmax - 1;

/* template:
 *                 .    .     .    .
 *                         2
 *                 .    .     .    .
 *                    3    0    1
 *                 .    .     .    .
 *                         4
 *                 .    .     .    .
 */
    a1 = a + 1;
    a2 = a + kz;
    a3 = a - 1;
    a4 = a - kz;

    SC_chunk_split(&lmn, &lmx, &rv);

    dk  = (cnt == N_CENT) ? kmax : kz;
    kzm = kz - 1;
    lzm = lz - 1;

    for (l = lmn; l < lmx; l++)
        for (k = 0; k < kz; k++)
            {in = l*kmax + k;
	     iz = l*dk + k;
	     if (emap[iz] == 0)
	        continue;

	     a0c = a[iz];
	     a1c = (k == kzm) ? -1 : a1[iz];
	     a2c = (l == lzm) ? -1 : a2[iz];
	     a3c = (k == 0) ? -1 : a3[iz];
	     a4c = (l == 0) ? -1 : a4[iz];

	     x1c[0] = x1[in];
	     x2c[0] = x2[in];
	     x3c[0] = x3[in];
	     x4c[0] = x4[in];

	     x1c[1] = y1[in];
	     x2c[1] = y2[in];
	     x3c[1] = y3[in];
	     x4c[1] = y4[in];

	     if (a0c != a1c)
	        PG_draw_line_n(dev, 2, WORLDC, x1c, x2c, dev->clipping);

	     if (a0c != a2c)
	        PG_draw_line_n(dev, 2, WORLDC, x2c, x3c, dev->clipping);

	     if (a0c != a3c)
	        PG_draw_line_n(dev, 2, WORLDC, x3c, x4c, dev->clipping);

	     if (a0c != a4c)
	        PG_draw_line_n(dev, 2, WORLDC, x4c, x1c, dev->clipping);};

    if (eflag)
       CFREE(emap);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DV_BND_ZC_LR - do DVB rendering assuming a zone centered
 *                 - Logical-Rectangular mesh
 *                 - the mesh topology is always ignored and is there
 *                 - to be call compatible with the AC version
 */

void PG_dv_bnd_zc_lr(PG_device *dev, int nd, int *a,
		     double *x, double *y, int npts, int *aext,
		     void *cnnct, pcons *alist)
   {int kz, lz;
    int *maxes, kmax, lmax;
    PG_dvb_data par;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];

    kz = kmax - 1;
    lz = lmax - 1;

    SC_ASSERT(kz > 0);

    par.dev   = dev;
    par.nd    = nd;
    par.a     = a;
    par.x     = x;
    par.y     = y;
    par.aext  = aext;
    par.cnnct = cnnct;
    par.alist = alist;

    SC_chunk_loop(_PG_dvb_chunk_zc_lr, 0, lz, TRUE, &par);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DV_BND_NC_LR - draw DVB segments for the variable A on the
 *                 - Logical-Rectangular mesh specified by coordinates
 *                 - X and Y which is KMAX by LMAX
 */

static void PG_dv_bnd_nc_lr(PG_device *dev, int nd, int *a,
			    double *x, double *y, int npts, int *aext,
			    void *cnnct, pcons *alist)
   {int *ia;
    double *ra, *ap;

    ra = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
		       SC_INT_I, a, 0, 1, npts, FALSE);

    ap = PM_node_zone_lr_2d(ra, cnnct, alist);

    ia = SC_convert_id(SC_INT_I, NULL, 0, 1,
		       SC_DOUBLE_I, ap, 0, 1, npts, FALSE);

    PG_dv_bnd_zc_lr(dev, nd, ia, x, y, npts, aext, cnnct, alist);

    CFREE(ap);
    CFREE(ra);
    CFREE(ia);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DV_BND_ZC_AC - do DVB rendering assuming a zone centered
 *                 - Arbitrarily-Connected mesh of with mesh topology MT
 */

void PG_dv_bnd_zc_ac(PG_device *dev, int nd, int *a,
		     double *x, double *y, int npts, int *aext,
		     void *cnnct, pcons *alist)
   {int is, os, iz, oz, is1, is2, in1, in2;
    int ia, ip, jp, jn1, jn2, a1, a2, nb;
    int nz, nzp, nsp, npt, npto;
    int *nc, *np, *nd1, *nd2, *p1, *p2;
    long **cells, *zones, *sides;
    double wc[PG_BOXSZ];
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PM_mesh_topology *mt;

    if (dev == NULL)
       return;

    mt = (PM_mesh_topology *) cnnct;

    cells = mt->boundaries;
    zones = cells[2];
    sides = cells[1];

    nc = mt->n_cells;
    nz = nc[2];

    np  = mt->n_bound_params;
    nzp = np[2];
    nsp = np[1];

    PG_get_viewspace(dev, WORLDC, wc);

/* can compute this by looping over the sides */
    npt = 4*nz;
    nd1 = CMAKE_N(int, npt);
    nd2 = CMAKE_N(int, npt);

    a1 = aext[0];
    a2 = aext[1];

    npt  = 0;
    npto = 0;
    for (ia = a1; ia <= a2; ia++)
        {p1 = nd1 + npt;
	 p2 = nd2 + npt;

/* add all the zone boundaries for this IN */
	 for (iz = 0; iz < nz; iz++)
	     {if (a[iz] != ia)
		 continue;

	      oz  = iz*nzp;
	      is1 = zones[oz];
	      is2 = zones[oz+1];

	      for (is = is1; is <= is2; is++)
		  {os    = is*nsp;
		   *p1++ = sides[os];
		   *p2++ = sides[os+1];
		   npt++;};};

/* remove all duplicates */
	 for (ip = 0; ip < npt; ip++)
	     {in1 = nd1[ip];
	      in2 = nd2[ip];

	      nb  = ip + 1;
	      nb  = max(nb, npto);
	      for (jp = nb; jp < npt; jp++)
		  {jn1 = nd1[jp];
		   jn2 = nd2[jp];
		   if (((in1 == jn1) && (in2 == jn2)) ||
		       ((in1 == jn2) && (in2 == jn1)))
		      {npt--;
		       for ( ; jp < npt; jp++)
			   {nd1[jp] = nd1[jp+1];
			    nd2[jp] = nd2[jp+1];};};};};

	 npto = npt;};

/* plot what's left */
    for (ip = 0; ip < npt; ip++)
        {in1 = nd1[ip];
	 in2 = nd2[ip];

	 x1[0] = x[in1];
	 x1[1] = y[in1];
	 x2[0] = x[in2];
	 x2[1] = y[in2];
	 PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};

    CFREE(nd1);
    CFREE(nd2);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DV_BND_NC_AC - draw DVB segments for the variable A on the
 *                 - Arbitrarily-Connected mesh specified by coordinates
 *                 - X and Y with topology MT
 */

static void PG_dv_bnd_nc_ac(PG_device *dev, int nd, int *a,
			    double *x, double *y, int npts, int *aext,
			    void *cnnct, pcons *alist)
   {int *ia;
    double *ra, *ap;

    ra = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
		       SC_INT_I, a, 0, 1, npts, FALSE);

    ap = PM_node_zone_ac_2d(ra, cnnct, alist);

    ia = SC_convert_id(SC_INT_I, NULL, 0, 1,
		       SC_DOUBLE_I, ap, 0, 1, npts, FALSE);

    PG_dv_bnd_zc_ac(dev, nd, ia, x, y, npts, aext, cnnct, alist);

    CFREE(ap);
    CFREE(ia);
    CFREE(ra);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_DVB_CORE - DVB plot skeleton routine */

static void _PG_dvb_core(PG_device *dev, PG_graph *data,
			 PFDvbZC fnc_zc, PFDvbNC fnc_nc)
   {PG_graph *g;
    PG_picture_desc *pd;

    if ((dev != NULL) && (data != NULL))
       {data->rendering = PLOT_DV_BND;

	pd = PG_setup_picture(dev, data, TRUE, TRUE, TRUE);

	PG_adorn_before(dev, pd, data);

/* plot all of the current functions */
	for (g = data; g != NULL; g = g->next)
	    PG_dvb_hand(dev, g, fnc_zc, fnc_nc);

	PG_finish_picture(dev, data, pd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DV_BND_PLOT - main DVB plot control routine */

void PG_dv_bnd_plot(PG_device *dev, PG_graph *data, ...)
   {

    SC_VA_START(data);

    if (strcmp(data->f->category, PM_LR_S) == 0)
       _PG_dvb_core(dev, data, PG_dv_bnd_zc_lr, PG_dv_bnd_nc_lr);

    else if (strcmp(data->f->category, PM_AC_S) == 0)
       _PG_dvb_core(dev, data, PG_dv_bnd_zc_ac, PG_dv_bnd_nc_ac);

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
