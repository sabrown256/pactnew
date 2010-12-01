/*
 * MLINTP.C - interpolation routines (N-dimensional and otherwise)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONNECTIVITY - return a pointer to the mesh topology for mappings on
 *                 - Arbitrarily-Connected sets or
 *                 - to the maximum indices for Logical-Rectangular sets
 */

void *PM_connectivity(PM_mapping *f)
   {void *cnnct;

    if (strcmp(f->category, PM_LR_S) == 0)
       cnnct = (void *) f->domain->max_index;

    else if (strcmp(f->category, PM_AC_S) == 0)
       cnnct = (void *) f->domain->topology;

    else
       cnnct = NULL;

    return(cnnct);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_REDIST_NODES_LR - given a logical rectangular zone centered
 *                     - mesh array return a node centered version
 *                     - by redistributing the values to the node
 *                     - using a uniform fractional value (1/4)
 */

static double *_PM_redist_nodes_lr(double *f, int km, int lm, int kmax, int lmax,
				   char *emap)
   {int i, j, k, l, nn, nz, eflag;
    double val;
    double *fp, *fp1, *fp2, *fp3, *fp4;
    double *ip, *ip1, *ip2, *ip3, *ip4;

    nn    = kmax*lmax;
    eflag = (emap == NULL);
    if (eflag)
       {emap = FMAKE_N(char, nn, "_PM_REDIST_NODES_LR:emap");
	memset(emap, 1, nn);};

    ip = FMAKE_N(double, nn, "_PM_REDIST_NODES_LR:ip");
    fp = FMAKE_N(double, nn, "_PM_REDIST_NODES_LR:fp");
    if (SC_zero_on_alloc() == FALSE)
       {PM_set_value(ip, nn, 0.0);
	PM_set_value(fp, nn, 0.0);};

    PM_LOGICAL_ZONE(fp, fp1, fp2, fp3, fp4, kmax);
    PM_LOGICAL_ZONE(ip, ip1, ip2, ip3, ip4, kmax);

    nz = km*lm;
    for (j = 0; j < nz; j++)
        {if (emap[j] != 0)
            {k = j % km;
	     l = j / km;
	     i = l*kmax + k;

	     val = f[j];

	     ip1[i]++;
	     ip2[i]++;
	     ip3[i]++;
	     ip4[i]++;

	     fp1[i] += val;
	     fp2[i] += val;
	     fp3[i] += val;
	     fp4[i] += val;};};

    for (j = 0; j < nn; j++)
        if (ip[j] > 0.0)
           fp[j] /= ip[j];
        else
           fp[j] = 0.0;

    SFREE(ip);
    if (eflag)
       SFREE(emap);

    return(fp);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_REDIST_NODES_AC - given a Arbitrarily-Connected zone centered mesh
 *                     - mesh array return a node centered version
 *                     - by redistributing the values to the node
 *                     - using a uniform fractional value
 */

static double *_PM_redist_nodes_ac(double *f, int nz, int nn,
				   long *zones, long *sides,
				   int nzp, int nsp)
   {int in, iz, is, is1, is2, os, oz;
    int *np;
    double *fp, fv;

    fp = FMAKE_N(double, nn, "_PM_REDIST_NODES_AC:fp");
    if (SC_zero_on_alloc() == FALSE)
       PM_set_value(fp, nn, 0.0);

    np = FMAKE_N(int, nn, "_PM_REDIST_NODES_AC:np");
    if (SC_zero_on_alloc() == FALSE)
       memset(np, 0, nn*sizeof(int));

/* accumulate nodal values from the zones */
    for (iz = 0; iz < nz; iz++)
        {oz  = iz*nzp;
	 is1 = zones[oz];
	 is2 = zones[oz+1];

         fv  = f[iz];

	 for (is = is1; is <= is2; is++)
	     {os = is*nsp;
	      in = sides[os];
              fp[in] += fv;
              np[in]++;};};
    
/* normalize the nodal values */
    for (in = 0; in < nn; in++)
        {fv = (double) np[in] + SMALL;
	 fp[in] /= fv;};

    SFREE(np);

    return(fp);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INTERP_NODES_LR - given a logical rectangular zone centered
 *                     - mesh array return a node centered version
 *                     - by interpolating the values to the nodes
 */

static double *_PM_interp_nodes_lr(double *f, double *x, double *y,
				   int km, int lm, int kmax, int lmax,
				   char *emap)
   {int i, j, k, l, nn, nz, eflag;
    double val, xc, yc, dm;
    double x1c, x2c, x3c, x4c;
    double y1c, y2c, y3c, y4c;
    double dx1, dx2, dx3, dx4;
    double dy1, dy2, dy3, dy4;
    double ds1, ds2, ds3, ds4;
    double w1, w2, w3, w4;
    double *fp, *fp1, *fp2, *fp3, *fp4;
    double *ip, *ip1, *ip2, *ip3, *ip4;
    double *x1, *x2, *x3, *x4;
    double *y1, *y2, *y3, *y4;

    nn    = kmax*lmax;
    eflag = (emap == NULL);
    if (eflag)
       {emap = FMAKE_N(char, nn, "_PM_INTERP_NODES_LR:emap");
	memset(emap, 1, nn);};

    ip = FMAKE_N(double, nn, "_PM_INTERP_NODES_LR:ip");
    fp = FMAKE_N(double, nn, "_PM_INTERP_NODES_LR:fp");
    if (SC_zero_on_alloc() == FALSE)
       {PM_set_value(ip, nn, 0.0);
	PM_set_value(fp, nn, 0.0);};

    PM_LOGICAL_ZONE(fp, fp1, fp2, fp3, fp4, kmax);
    PM_LOGICAL_ZONE(x, x1, x2, x3, x4, kmax);
    PM_LOGICAL_ZONE(y, y1, y2, y3, y4, kmax);
    PM_LOGICAL_ZONE(ip, ip1, ip2, ip3, ip4, kmax);

    dm = 1.0e-20;
    nz = km*lm;
    for (j = 0; j < nz; j++)
        {if (emap[j] != 0)
            {k = j % km;
	     l = j / km;
	     i = l*kmax + k;

	     val = f[j];

	     x1c = x1[i];
	     x2c = x2[i];
	     x3c = x3[i];
	     x4c = x4[i];

	     y1c = y1[i];
	     y2c = y2[i];
	     y3c = y3[i];
	     y4c = y4[i];

	     xc = 0.25*(x1c + x2c + x3c + x4c);
	     yc = 0.25*(y1c + y2c + y3c + y4c);

	     dx1 = x1c - xc;
	     dx2 = x2c - xc;
	     dx3 = x3c - xc;
	     dx4 = x4c - xc;

	     dy1 = y1c - yc;
	     dy2 = y2c - yc;
	     dy3 = y3c - yc;
	     dy4 = y4c - yc;

	     ds1 = dx1*dx1 + dy1*dy1;
	     ds2 = dx2*dx2 + dy2*dy2;
	     ds3 = dx3*dx3 + dy3*dy3;
	     ds4 = dx4*dx4 + dy4*dy4;

	     w1 = 1.0/(ds1 + dm);
	     w2 = 1.0/(ds2 + dm);
	     w3 = 1.0/(ds3 + dm);
	     w4 = 1.0/(ds4 + dm);

	     ip1[i] += w1;
	     ip2[i] += w2;
	     ip3[i] += w3;
	     ip4[i] += w4;

	     fp1[i] += val*w1;
	     fp2[i] += val*w2;
	     fp3[i] += val*w3;
	     fp4[i] += val*w4;};};

    for (j = 0; j < nn; j++)
        {if (ip[j] > 0.0)
            fp[j] /= ip[j];
         else
            fp[j] = 0.0;};

    SFREE(ip);
    if (eflag)
       SFREE(emap);

    return(fp);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INTERP_NODES_AC - given a Arbitrarily-Connected zone centered mesh
 *                     - mesh array return a node centered version
 *                     - by interpolation
 */

static double *_PM_interp_nodes_ac(double *f, double *x, double *y, int nz, int nn,
				   long *zones, long *sides, int nzp, int nsp)
   {int in, iz, is, js, is1, is2;
    int os, oz, ns, off;
    double nrm, fv, dm, wc;
    double xc, yc, dxc, dyc;
    double *fp, *np, *w;

    fp = FMAKE_N(double, nn, "_PM_INTERP_NODES_AC:fp");
    np = FMAKE_N(double, nn, "_PM_INTERP_NODES_AC:np");
    w  = FMAKE_N(double, 20, "_PM_INTERP_NODES_AC:w");
    if (SC_zero_on_alloc() == FALSE)
       {PM_set_value(fp, nn, 0.0);
	PM_set_value(np, nn, 0.0);
	PM_set_value(w, 20, 0.0);};

    dm = 1.0e-20;

/* accumulate nodal values from the zones */
    off = (nzp < 2);
    for (iz = 0; iz < nz; iz++)
        {oz  = iz*nzp;
	 is1 = zones[oz];
	 is2 = zones[oz+1] - off;

         fv  = f[iz];

	 ns  = is2 - is1 + 1;
	 nrm = 1.0/((double) ns);

/* find the cell center */
	 xc = 0.0;
	 yc = 0.0;
	 for (is = is1; is <= is2; is++)
	     {os  = is*nsp;
	      in  = sides[os];
	      xc += x[in];
	      yc += y[in];};
	 xc *= nrm;
	 yc *= nrm;

/* find the weights */
	 for (js = 0; js < ns; js++)
	     w[js] = 0.0;

	 for (is = is1; is <= is2; is++)
	     {js = is - is1;
	      os  = is*nsp;
	      in  = sides[os];
	      dxc = x[in] - xc;
	      dyc = y[in] - yc;
	     
	      w[js] += (dxc*dxc + dyc*dyc);};

	 for (js = 0; js < ns; js++)
	     w[js] = 1.0/(sqrt(w[js]) + dm);

	 for (is = is1; is <= is2; is++)
	     {js = is - is1;
	      os = is*nsp;
	      in = sides[os];
	      wc = w[js];

              fp[in] += fv*wc;
              np[in] += wc;};};
    
/* normalize the nodal values */
    for (in = 0; in < nn; in++)
        {if (np[in] > 0.0)
	    fp[in] /= np[in];
	 else
	    fp[in] = 0.0;};

    SFREE(np);
    SFREE(w);

    return(fp);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_Z_N_LR_2D - given a Logical-Rectangular zone centered mesh array
 *              - return a node centered version
 */

double *PM_z_n_lr_2d(double *f, double *x, double *y, int mode, void *cnnct,
		     pcons *alist)
   {int *maxes, n, km, lm, kmax, lmax, eflag;
    char *emap;
    double *ret;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];

    n = SC_arrlen(f)/sizeof(double);
    if (n == (kmax - 1)*(lmax - 1))
       {km = kmax - 1;
	lm = lmax - 1;}

    else
       return(f);

    emap = PM_check_emap(&eflag, alist, n);

    if ((x != NULL) && (y != NULL) && mode)
       ret = _PM_interp_nodes_lr(f, x, y,
				 km, lm, kmax, lmax, emap);
    else
       ret = _PM_redist_nodes_lr(f, km, lm, kmax, lmax, emap);

    if (eflag)
       SFREE(emap);

    return(ret);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ZONE_NODE_LR_2D - given a Logical-Rectangular zone centered mesh array
 *                    - return a node centered version
 *                    - by redistributing the values to the node
 *                    - using a uniform fractional value (1/4)
 */

double *PM_zone_node_lr_2d(double *f, void *cnnct, pcons *alist)
   {double *fp;

    fp = PM_z_n_lr_2d(f, NULL, NULL, FALSE, cnnct, alist);

    return(fp);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NODE_ZONE_LR_2D - given a Logical-Rectangular node centered mesh
 *                    - mesh array return a zone centered version
 *                    - by redistributing the values to the zone
 *                    - using a uniform fractional value (1/4)
 */

double *PM_node_zone_lr_2d(double *f, void *cnnct, pcons *alist)
   {int *maxes, kmax, lmax, *pc, corner;
    int i, j, km, lm, n, eflag, npts, delta;
    char *emap;
    double *fp, *f1, *f2, *f3, *f4;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];

    SC_assoc_info(alist,
		  "CORNER", &pc,
		  NULL);

    corner = (pc == NULL) ? 2 : *pc;

    npts = kmax*lmax;
    emap = PM_check_emap(&eflag, alist, npts);

    switch (corner)
       {case 1 :
	     delta = 1;
	     break;

	default :
        case 2 :
	     delta = kmax + 1;
	     break;

        case 3 :
	     delta = kmax;
	     break;

        case 4 :
	     delta = 0;
	     break;};

    emap += delta;

    fp = FMAKE_N(double, npts, "PM_NODE_ZONE_LR_2D:fp");
    if (SC_zero_on_alloc() == FALSE)
       PM_set_value(fp, npts, 0.0);
    PM_LOGICAL_ZONE(f, f1, f2, f3, f4, kmax);

    km = kmax - 1;
    lm = lmax - 1;
    n  = km*lm;
    for (j = 0; j < n; j++)
        {i = j + j/km;
	 if (emap[i] != 0)
            fp[i] = 0.25*(f1[i] + f2[i] + f3[i] + f4[i]);};

    emap -= delta;

    if (eflag)
       SFREE(emap);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_Z_N_AC_2D - given a Arbitrarily-Connected zone centered mesh
 *              - mesh array return a node centered version
 *              - by redistributing the values to the node
 *              - using a uniform fractional value
 */

double *PM_z_n_ac_2d(double *f, double *x, double *y, int mode, void *cnnct,
		     pcons *alist)
   {int *nc, nz, *np, nzp, nsp, nn;
    long **cells, *zones, *sides;
    double *fp;
    PM_mesh_topology *mt;

    mt = (PM_mesh_topology *) cnnct;

    cells = mt->boundaries;
    zones = cells[2];
    sides = cells[1];

    nc = mt->n_cells;
    nz = nc[2];
    nn = nc[0];

    np  = mt->n_bound_params;
    nzp = np[2];
    nsp = np[1];

    if ((x != NULL) && (y != NULL) && mode)
       fp = _PM_interp_nodes_ac(f, x, y, nz, nn, zones, sides, nzp, nsp);
    else
       fp = _PM_redist_nodes_ac(f, nz, nn, zones, sides, nzp, nsp);

    return(fp);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ZONE_NODE_AC_2D - given a Arbitrarily-Connected zone centered mesh
 *                    - mesh array return a node centered version
 *                    - by redistributing the values to the node
 *                    - using a uniform fractional value
 */

double *PM_zone_node_ac_2d(double *f, void *cnnct, pcons *alist)
   {double *rv;

    rv = PM_z_n_ac_2d(f, NULL, NULL, FALSE, cnnct, alist);

    return(rv);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NODE_ZONE_AC_2D - given a Arbitrarily-Connected node centered mesh
 *                    - mesh array return a zone centered version
 *                    - by redistributing the values to the zone
 *                    - using a uniform fractional value
 */

double *PM_node_zone_ac_2d(double *f, void *cnnct, pcons *alist)
   {int in, iz, is, is1, is2, os, oz;
    int *nc, nz, *np, nzp, nsp;
    long **cells, *zones, *sides;
    double *fp, fv;
    PM_mesh_topology *mt;

    mt = (PM_mesh_topology *) cnnct;

    cells = mt->boundaries;
    zones = cells[2];
    sides = cells[1];

    nc = mt->n_cells;
    nz = nc[2];

    np  = mt->n_bound_params;
    nzp = np[2];
    nsp = np[1];

    fp = FMAKE_N(double, nz, "PM_NODE_ZONE_AC_2D:fp");

/* accumulate nodal values from the zones */
    for (iz = 0; iz < nz; iz++)
        {oz  = iz*nzp;
	 is1 = zones[oz];
	 is2 = zones[oz+1];

         fv = 0.0;
	 for (is = is1; is <= is2; is++)
	     {os  = is*nsp;
	      in  = sides[os];
              fv += f[in];};

         fp[iz] = fv/((double) (is2 - is1 + 1));};

    return(fp);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ZONE_CENTERED_MESH_2D - compute a zone centered set of coordinates
 *                          - from the given logical rectangular mesh
 */

void PM_zone_centered_mesh_2d(double **px, double **py, double *rx, double *ry,
                              int kmax, int lmax)
   {int i, n;
    double *xc, *x1, *x2, *x3, *x4;
    double *yc, *y1, *y2, *y3, *y4;

    n = kmax*lmax;

    xc = FMAKE_N(double, n, "PM_ZONE_CENTERED_MESH_2D:xc");
    yc = FMAKE_N(double, n, "PM_ZONE_CENTERED_MESH_2D:yc");

    PM_LOGICAL_ZONE(rx, x1, x2, x3, x4, kmax);
    PM_LOGICAL_ZONE(ry, y1, y2, y3, y4, kmax);

    for (i = 0; i < n; i++)
        {xc[i] = 0.25*(x1[i] + x2[i] + x3[i] + x4[i]);
         yc[i] = 0.25*(y1[i] + y2[i] + y3[i] + y4[i]);};

    *px = xc;
    *py = yc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SHIFT_CENTER_LR - given a logical rectangular zone centered
 *                     - mesh set return a node centered version
 *                     - with the points at the cell centers
 */

static PM_set *_PM_shift_center_lr(PM_set *s)
   {int j, k, id, jd, nc, nd, ng, nn, nz;
    int io, jo, ig, off;
    int *mxn, *mxc, *ngb, *strn, *strc;
    double nrm;
    double **y, *py, *px;
    void **x;
    PM_set *ns;

    nd = s->dimension;
    x  = s->elements;

    mxn = s->max_index;
    mxc = FMAKE_N(int, nd, "_PM_SHIFT_CENTER_LR:mxc");

    ng  = 2*nd;
    ngb = FMAKE_N(int, ng, "_PM_SHIFT_CENTER_LR:ngb");

    strn = FMAKE_N(int, nd, "_PM_SHIFT_CENTER_LR:strn");
    strc = FMAKE_N(int, nd, "_PM_SHIFT_CENTER_LR:strc");

/* compute the number of nodes and cells
 * also the new index limits and the strides
 */
    nn  = 1;
    nz  = 1;
    for (id = 0; id < nd; id++)
        {nc = mxn[id];

	 mxc[id]  = nc - 1;
	 strn[id] = nn;
	 strc[id] = nz;

	 nn *= nc;
	 nz *= (nc - 1);};

/* compute the neighborhood template */
    off = 1;
    ng  = 0;
    ngb[ng++] = 0;
    ngb[ng++] = 1;
    for (id = 1; id < nd; id++)
        {off *= mxn[id-1];
	 for (ig = 0; ig < ng; ig++)
	     ngb[ig + ng] = ngb[ig] + off;
	 ng *= 2;};

    y = PM_make_vectors(nd, nz);

    px = NULL;
    for (id = 0; id < nd; id++)
        {px = PM_array_real(s->element_type, x[id], nn, px);
	 py = y[id];

	 for (j = 0; j < nz; j++)
	     {jo = j;
	      io = 0;
	      for (jd = nd-1; jd > -1; jd--)
		  {k = jo / strc[jd];
		   io += k*strn[jd];
		   jo -= k*strc[jd];};
	 
	      for (ig = 0; ig < ng; ig++)
		  py[j] += px[io+ngb[ig]];};};

/* normalize the coordinates */
    nrm = 1.0/((double) ng);
    for (id = 0; id < nd; id++)
        {py = y[id];
         for (j = 0; j < nz; j++)
	     py[j] *= nrm;};

    ns = PM_mk_set(s->name, s->element_type, TRUE,
		   nz, nd, s->dimension_elem,
		   mxc, y, s->opers, s->metric,
		   s->symmetry_type, s->symmetry,
		   s->topology_type, s->topology,
		   s->info_type, s->info,
		   NULL);

    SFREE(ngb);
    SFREE(strn);
    SFREE(strc);

    return(ns);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_AVERAGE_CELL_PT - return the sum of the node positions and the
 *                     - number of contributing points
 */

static void _PM_average_cell_pt(double *xc, int *pnc, int nd, int id,
				long **bnd, int *bnp,
				int j1, int j2, double **x)
   {int ib, jd, i1, i2, l1, l2;
    int np;

    if (id <= 1)
       {for (jd = 0; jd < nd; jd++)
	    {xc[jd] += x[jd][j1];
	     xc[jd] += x[jd][j2];};
	*pnc += 2;}

    else
       {i1 = bnd[id][j1];
	i2 = bnd[id][j2];

	id--;

	np = bnp[id];
	for (ib = i1; ib < i2; ib++)
	    {l1 = bnd[id][np*ib];
	     l2 = bnd[id][np*ib+1];
	    _PM_average_cell_pt(xc, pnc, nd, id, bnd, bnp, l1, l2, x);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SHIFT_CENTER_AC - given an arbitrarily connect zone centered
 *                     - mesh set return a node centered version
 *                     - with the points at the cell centers
 */

static PM_set *_PM_shift_center_ac(PM_set *s)
   {int ic, id, nc, nd, ne, np;
    int *bnc, *bnp;
    long **bnd;
    double nrm;
    double *xc, **x, **y;
    void **elem;
    PM_set *ns;
    PM_mesh_topology *mt;

    mt = (PM_mesh_topology *) s->topology;

    nd  = mt->n_dimensions;
    bnc = mt->n_cells;
    bnp = mt->n_bound_params;
    bnd = mt->boundaries;

    ne   = s->n_elements;
    elem = s->elements;

    nc = bnc[nd];

    xc = FMAKE_N(double, nd, "_PM_SHIFT_CENTER_AC:xc");
    y  = PM_make_vectors(nd, nc);
    x  = PM_make_vectors(nd, ne);
    for (id = 0; id < nd; id++)
        PM_array_real(s->element_type, elem[id], ne, x[id]);

    nc = bnc[nd];
    for (ic = 0; ic < nc; ic++)
        {for (id = 0; id < nd; id++)
	     xc[id] = 0.0;
	 np = 0;
	 _PM_average_cell_pt(xc, &np, nd, nd, bnd, bnp, ic, ic+1, x);

/* normalize the coordinates */
	 nrm = 1.0/((double) np);
	 for (id = 0; id < nd; id++)
	     y[id][ic] = xc[id]*nrm;};

    ns = PM_mk_set(s->name, s->element_type, FALSE,
		   nc, nd, s->dimension_elem,
		   NULL, y, s->opers, s->metric,
		   s->symmetry_type, s->symmetry,
		   NULL, NULL,
		   s->info_type, s->info,
		   NULL);

    PM_free_vectors(nd, x);
    SFREE(xc);

    return(ns);}
                       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NODE_CENTERED_MAPPING - return a node centered mapping based on the
 *                          - given mapping S
 *                          - if S is node centered return a copy of it
 */

PM_mapping *PM_node_centered_mapping(PM_mapping *s)
   {PM_set *sd, *sr;
    PM_set *nsd, *nsr;
    PM_mapping *ns;

    sd = s->domain;
    sr = s->range;

    if (sd->n_elements == sr->n_elements)
       nsd = PM_copy_set(sd);

    else if (strcmp(s->category, PM_LR_S) == 0)
       nsd = _PM_shift_center_lr(sd);

    else if (strcmp(s->category, PM_AC_S) == 0)
       nsd = _PM_shift_center_ac(sd);

    else
       nsd = NULL;

    nsr = PM_copy_set(sr);
    ns  = PM_make_mapping(s->name, s->category, nsd, nsr,
			  N_CENT, NULL);

    return(ns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERPOL - given a mesh GRID, an array of points to interpolate to PTS,
 *             - and an array of functions to interpolate to the points FNCS
 *             - return an array of function values at the interpolation
 *             - points for each interpolation point we find the zone which
 *             - contains it by looping over zones and accepting the zone
 *             - for which the cross product of the vector from vertex to
 *             - point and vertex to next vertex is negative for each
 *             - vertex of the zone (note the vertices are ordered so as
 *             - to traverse the zone boundary in a counter-clockwise
 *             - fashion). FIND_VERTICES does this.
 *             - 
 *             - next, given the list of vertices defining the zone in which
 *             - the interpolation point resides, we find a set of weights for
 *             - the function values associated with the vertices so that the
 *             - value A0 at the interpolation point R0 is given by
 *             -
 *             -    A0 = sum(Ai x Wi)
 *             -
 *             - where the Ai are the values at the vertices and Wi are the 
 *             - weights. FIND_COEFFICIENTS does this.
 *             -
 *             - finally, given the set of weights which are only functions of
 *             - the geometry of the zones and are completely independent of
 *             - the function being interpolated, we interpolate each supplied
 *             - function to the interpolation point. INTERPOLATE_VALUE does
 *             - this.
 *             -
 *             - the rest of this routine handles the accessing and packaging
 *             - of the results.  this routine is completely general for 2d
 *             - meshes and as such is not particularly optimal for some
 *             - specific meshes.  it does best when given a large number of
 *             - functions to interpolate to each interpolation point.
 */

double **PM_interpol(PM_lagrangian_mesh *grid, double **pts, int np,
		     double **fncs, int nf)
   {int i, j, ok;
    double *rix, *riy, **vals;
    coefficient *vertices;

/* allocate the return values */
    vals = PM_make_vectors(nf, np);

/* get pointers to the points to interpolate to */
    rix = pts[0];
    riy = pts[1];

    vertices = PM_alloc_vertices(grid);

    ok = TRUE;
    for (i = 0; i < np; i++)

/* find the vertices surrounding the IP */
        {ok = PM_find_vertices(rix[i], riy[i], grid, vertices);

/* build the coefficients for the IP */
         ok = PM_find_coefficients(rix[i], riy[i], grid, vertices);

/* interpolate all of the functions at the IP */
         for (j = 0; j < nf; j++)
             vals[j][i] = PM_interpolate_value(vertices, fncs[j]);};

    return(vals);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INSIDE - return TRUE if the point is "inside" the boundary segment
 *           -   (X, Y) is the interpolation point
 *           -   PX and PY are the arrays of mesh coordinates
 *           -   MAP is an array of indices into PX and PY which defines a
 *           -       zone (it's the list of vertices) the indices are ordered
 *           -       so as to traverse the boundary in a counter-clockwise
 *           -       direction
 *           -   N is the length of map, i.e. the number of vertices or sides
 *           -       to the zone
 *           -
 *           - the point is inside the zone if the cross product of the
 *           - vector from vertex to next vertex and vertex to point is
 *           - positive for each vertex of the zone
 *           - 
 *           - if any cross product is negative the point is outside the zone
 */

int PM_inside(double x, double  y, double *px, double *py, int *map, int n)
   {int ia, ib, ja, jb;
    double dxba, dyba, dx0a, dy0a, cross;

    for (ja = 0; ja < n; ja++)
        {jb = (ja + 1) % n;
         ia = map[ja];
         ib = map[jb];

         dxba = px[ib] - px[ia];
         dyba = py[ib] - py[ia];
         dx0a = x - px[ia];
         dy0a = y - py[ia];

         cross = (dxba*dy0a - dyba*dx0a);
         if (TOLERANCE > cross)
            return(FALSE);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INSIDE_FIX - return TRUE if the point is "inside" the boundary
 *               -   (X, Y) is the interpolation point
 *               -   PX and PY are the arrays of boundary points
 *               -   N is the length of boundary
 *               -
 *               - the point is inside the zone if the cross product of the
 *               - vector from vertex to next vertex and vertex to point is
 *               - positive for each vertex of the boundary
 *               - 
 *               - if any cross product is negative the point is outside the
 *               - boundary
 */

int PM_inside_fix(int x, int y, int *px, int *py, int n, int direct)
   {int ja, jb;
    int dxba, dyba, dx0a, dy0a, cross;

    for (ja = 0; ja < n; ja++)
        {jb = (ja + 1) % n;

         dxba = px[jb] - px[ja];
         dyba = py[jb] - py[ja];
         dx0a = x - px[ja];
         dy0a = y - py[ja];

         cross = direct*(dxba*dy0a - dyba*dx0a);
         if (TOLERANCE > cross)
            return(FALSE);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ALLOC_VERTICES - return an array containing the vertices */

coefficient *PM_alloc_vertices(PM_lagrangian_mesh *grid)
   {int n_v;
    coefficient *vertices;

    grid = NULL;

    vertices = FMAKE(coefficient, "PM_ALLOC_VERTICES:vertices");

    vertices->n_points = n_v = 4;
    vertices->indexes  = FMAKE_N(int, n_v,
                         "PM_ALLOC_VERTICES:indexes");
    vertices->weights  = FMAKE_N(double, n_v,
                         "PM_ALLOC_VERTICES:weights");

    return(vertices);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIND_VERTICES - fill the array containing the vertices whose
 *                  - corresponding sides define the zone in which the
 *                  - given point resides
 *                  -
 *                  - it turns out that in this routine we assume that
 *                  - no connectivity information is supplied and proceed
 *                  - under the assumption that we have a logical
 *                  - rectangular mesh
 *                  -
 *                  - this being the case we compute the list of vertices
 *                  - (correctly ordered) for each zone as we go and test
 *                  - whether or not the point (X, Y) is inside
 *                  -
 *                  - when finished we will have a valid coefficient struct
 *                  - for the point (X, Y) which at this point will contain
 *                  - the vertices surrounding the interpolation point.
 */

int PM_find_vertices(double x, double y, PM_lagrangian_mesh *grid,
                     coefficient *vertices)
   {int i, km, lm, n, nv, ok;
    int *map;
    double *px, *py;

    px = grid->x;
    py = grid->y;
    km = grid->kmax;
    lm = grid->lmax;
    n  = km*lm;

    nv  = vertices->n_points;
    map = vertices->indexes;

    ok = FALSE;
    for (i = km+1; (i < n) && (ok == FALSE); i++)
        {map[0] = i - km;
         map[1] = i;
         map[2] = i - 1;
         map[3] = i - km - 1;

         if (PM_inside(x, y, px, py, map, nv))
            ok = TRUE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIND_COEFFICIENTS - find the set of normalized weights which 
 *                      - specify the interpolating coefficients of the
 *                      - vertices
 *                      -
 *                      - this routine is entered with an interpolation
 *                      - point (X, Y), a mesh GRID, and a coefficient struct
 *                      - which contains the vertices surrounding the
 *                      - interpolation point
 *                      -
 *                      - when we leave the coefficient struct will also have
 *                      - a list of weights corresponding to the vertices
 *                      -
 *                      - for each vertex we compute the intersection of the
 *                      - ray from the vertex thru the interpolation point
 *                      - (IP) with one of the other zone boundary segments
 *                      - as defined by the other vertices.  we construct
 *                      - linear interpolation weights for this intersection
 *                      - point and then from those weights construct
 *                      - weights for the linear interpolation from the
 *                      - original vertex to the intersection point for
 *                      - the IP.  the partial weights are resolved into
 *                      - contributions from the three vertices involved
 *                      - and accumulated into values for all of the vertices.
 *                      - the contributions to the weights are further
 *                      - weighted by the inverse distance from the vertex
 *                      - to the IP.  this is done to insure that the
 *                      - contributions of the near vertices dominate over
 *                      - farther ones. consider the following situation
 *                      -
 *                      -                          .
 *                      -                         / \
 *                      -                        /   \
 *                      -                       /     \
 *                      -                      /       \
 *                      -      these should ->.    x    .
 *                      -      dominate        \       /
 *                      -                       \     /
 *                      -                        \   /
 *                      -                         \ /
 *                      -        not these ------> .
 *                      -
 *                      -
 *                      - when all of the vertices have been processed the
 *                      - weights are renormalized so that they sum to 1.
 */

int PM_find_coefficients(double x, double y, PM_lagrangian_mesh *grid,
                         coefficient *vertices)
   {int ia, j, ja, ka, kb, la, lb, n;
    int i1, i2, i3, ok;
    int *map;
    double *px, *py;
    double xi, yi, dx0i, dy0i, d0i;
    double sx, sy, dsl;
    double dxj1j, dyj1j, dxij, dyij, cross, parallelp;
    double dsjx, dsjy, dsj, dj1j, dsign;
    double u, v, w;
    double *weights;

    px = grid->x;
    py = grid->y;
    i1 = 0;
    i2 = 0;
    i3 = 0;

    n       = vertices->n_points;
    map     = vertices->indexes;
    weights = vertices->weights;

    for (j = 0; j < n; j++)
        weights[j] = 0.0;

    ok = TRUE;
    for (ja = 0; (ja < n) && (ok == TRUE); ja++)
        {ia = map[ja];

         xi = px[ia];
         yi = py[ia];

/* compute the distances from the vertices to the IP */
         dx0i = x - xi;
         dy0i = y - yi;
         d0i  = HYPOT(dx0i, dy0i);

/* find the side crossed by the line from the vertex to the IP */
         u = HUGE;
         v = HUGE;
         for (j = 1; (j < n-1) && (ok == TRUE); j++)
             {ka = (ja + j) % n;
              kb = (ka + 1) % n;
              la = map[ka];
              lb = map[kb];

              dxj1j = px[lb] - px[la];
              dyj1j = py[lb] - py[la];

              parallelp = dx0i*dyj1j - dy0i*dxj1j;
              if (d0i < TOLERANCE)
                 {i1 = ja;
                  i2 = ka;
                  i3 = kb;
                  u = 1.0;
                  v = 0.0;}
              else if (ABS(parallelp) > TOLERANCE)
                 {dxij  = xi - px[la];
                  dyij  = yi - py[la];
                  cross = dxj1j*dyij - dyj1j*dxij;
                  sx    = xi + dx0i*cross/parallelp;
                  sy    = yi + dy0i*cross/parallelp;

                  dsl   = HYPOT(sx-xi, sy-yi);
                  dsjx  = sx - px[lb];
                  dsjy  = sy - py[lb];
                  dsj   = HYPOT(dsjx, dsjy);
                  dj1j  = HYPOT(dxj1j, dyj1j);
                  dsign = -(dsjx*dxj1j + dsjy*dyj1j);

/* dsl > d0i - says that the crossing is further away from xi than x0
 * dj1j >= dsj - d(Xj, Xj+1) >= d(cross, Xj)
 * dsign > 0 - says that the crossing is on the same side of Xj as Xj+1
 */
                  if ((dsl > d0i - TOLERANCE) &&
                      (dj1j >= dsj) && (dsign >= 0.0))
                     {i1 = ja;
                      i2 = ka;
                      i3 = kb;
                      u  = dsj/dj1j;
                      v  = d0i/dsl;
                      break;};};};

         if ((u > 1.0 + TOLERANCE) || (v > 1.0 + TOLERANCE))
            ok = FALSE;

         weights[i1] += 1.0 - v;
         weights[i2] += v*u;
         weights[i3] += v*(1.0 - u);};

    if (ok == TRUE)

/* renormalize the weights */
       {w = 0.0;
	for (j = 0; j < n; j++)
	    w += weights[j];

	w = 1.0/w;
	for (j = 0; j < n; j++)
	    weights[j] *= w;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERPOLATE_VALUE - given the interpolating coefficients and the
 *                      - all of function values compute and return the
 *                      - value of the function at the interpolation point
 */

double PM_interpolate_value(coefficient *vertices, double *f)
   {int i, j, n, *map;
    double *weights, value;

    n       = vertices->n_points;
    map     = vertices->indexes;
    weights = vertices->weights;
    value   = 0.0;
    for (i = 0; i < n; i++)
        {j = map[i];
         value += (*(weights++))*f[j];};

    return(value);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
