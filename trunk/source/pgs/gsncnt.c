/*
 * GSNCNT.C - N-dimensional contour plotter
 *
 * Source Version: 3.0
 * Software Release: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/* ALGORITHM
 *
 * A) For each contour level do the following:
 *     Find all cells which this level "crosses"
 *       Foreach marked cell do the following (_PG_single_cell):
 *        1) fill out a template of node positions (too restrictive)
 *           which define a decompostion of the cell into simpleces
 *           (an index range of ifs->domain)
 *        2) make out an array of d(x) - d values
 *        3) locate the level_surface
 *
 * B) Complete the construction (complete_level_surface) by:
 *    1) build the complete array of level_surface sets (make_level_surface_set)
 *    2) resizing arrays to their final size
 *    3) complete the cell map
 *
 */

/* LOCATION node attribute values
 *    INSIDE_SURF  inside D
 *    ON_SURF      on D
 *    OUTSIDE_SURF outside D
 */

#define INSIDE_SURF     1
#define ON_SURF         2
#define OUTSIDE_SURF    3

#define DIMSPACE       3             /* maximum spacial dimension */

#define START_TIME(v)                                                      \
   {double __timing_tmp;                                                   \
    __timing_tmp = SC_cpu_time()

#define STOP_TIME(v)                                                       \
    v += SC_cpu_time() - __timing_tmp;}

#ifdef LOCAL_TIMING
# define LOCAL_START_TIME  START_TIME
# define LOCAL_STOP_TIME   STOP_TIME
#else
# define LOCAL_START_TIME(x)
# define LOCAL_STOP_TIME(x)
#endif

#define SIZED(type, pa, nn, n1)                                            \
   ((pa == NULL) ?                                                         \
    (pa = CMAKE_N(type, 2*(nn+n1))) :                                      \
    ((nn+n1) >= SC_arrlen(pa)/((long) sizeof(type))) ?                     \
    (CREMAKE(pa, type, nn+n1+200)) :                                      \
    pa)

#define GET_POLY_TOPOLOGY(pt, _nd, _df, _nn, _nl, _ne, _el,                \
                          _nf, _fel, _fnl, _nc, _cl)                       \
   {int _ie, _ife, _ifn, _ic;                                              \
    _nn  = pt[0];                                                          \
    if (_nd == 2)                                                          \
       _df = _nn + 2;                                                      \
    else                                                                   \
       _df = 6;                                                            \
    _ie  = 1 + _nn;                                                        \
    _ne  = pt[_ie];                                                        \
    _ife = 1 + _ie + 3*_ne;                                                \
    _nf  = pt[_ife];                                                       \
    _ifn = 1 + _ife + _df*_nf;                                             \
    _ic  = _ifn + _df*_nf;                                                 \
    _nc  = pt[_ic];                                                        \
    _nl  = pt + 1;                                                         \
    _el  = pt + 1 + _ie;                                                   \
    _fel = pt + 1 + _ife;                                                  \
    _fnl = pt + _ifn;                                                      \
    _cl  = pt + 1 + _ic;}

#define	EXCHANGE(nlst, _i, _j)                                             \
   {int _it, _jt;                                                          \
    _it = 2*(_i);                                                          \
    _jt = 2*(_j);                                                          \
    SC_SWAP_VALUE(int, nlst[_it], nlst[_jt]);                              \
    _it++;                                                                 \
    _jt++;                                                                 \
    SC_SWAP_VALUE(int, nlst[_it], nlst[_jt]);}

#undef ERROR
#define ERROR        _PG_error_hand

#define DIAGNOSE_INIT(msg)                                                   \
   {if (debug_cnv)                                                           \
       {PRINT(stdout, "\n===== %s\n", msg);};                                \
    if (logfl)                                                               \
       {PRINT(log, "# table for call %d\n", ++count);};}

#define DIAGNOSE_CNV(msg)                                                    \
   {if (debug_cnv)                                                           \
       {PRINT(stdout, "----- %s\n", msg);                                    \
	PRINT(stdout, "   D %25.16e  %25.16e  %25.16e     %d\n",             \
	      xm, x1, xp, it);                                               \
	PRINT(stdout, "   V %25.16e  %25.16e  %25.16e\n",                    \
	      ym, y1, yp);};                                                 \
    if (logfl)                                                               \
       {PRINT(log, "%6d %25.16e %25.16e %25.16e %25.16e %25.16e %25.16e\n",  \
	      it, xm, x1, xp, ym, y1, yp);};}

/*--------------------------------------------------------------------------*/

/*                           TYPEDEFS AND STRUCTS                           */

/*--------------------------------------------------------------------------*/

typedef struct s_timing_lev_surf timing_lev_surf;
typedef struct s_db_ls_info db_ls_info;
typedef struct s_level_surface level_surface;
typedef struct s_itf_array itf_array;

typedef void (*PFParTsk)(PFInt fnc, void **pa, int *pnp, int *poff, int *pon);

struct s_timing_lev_surf
   {double init;                  /* _PG_init_cell */
    double ld_topology;           /* _PG_fill_topology */
    double surf_seg_inter;        /* _PG_examine_poly */
    double merge_surface;         /* make_level_surface_set */
    double net_time;              /* total of the above */
    int n_volumes;};

struct s_db_ls_info
   {int nd;                    /* number of dimensions */
    int nuc;                   /* number of cells */
    int nmc;                   /* number of crossed cells */
    int nlev;                  /* number of levels */
    PG_rendering plot_type;    /* either PLOT_SURFACE or PLOT_WIRE_MESH */
    int parallel;              /* do the IR in parallel if not FALSE */
    int nthread;               /* number of threads to use */
    int chunk_strategy;        /* parallelization strategy */
    int *cell;                 /* lr index for crossed cell */

    int *dims;                 /* dimensions of FD */
    pcons *alist;              /* additional properties */    
    double *levels;              /* level values */
    double *fd;                  /* the function whose levels are being plotted */
    double **x;                  /* LR mesh on which FD exists */
    PFParTsk par_task;};

struct s_level_surface
   {int ip;                   /* id for this piece */
    int nd;
    int j_ac;
    int j_lr;
    int ilev;
    int *poly_topology;
    double d;
    double *dn;
    double *fn;
    int analyze;              /* for debugging purposes */
    FILE *debug;
    void (*plot)(itf_array *ifs, double d, double v);};

struct s_itf_array
   {int n_pieces;              /* number of pieces for thread parallel */
    int ilev;                  /* current level */
    PM_set **domain;           /* AC domain (parallel) */
    level_surface **itf;       /* level_surface scratch worker (parallel) */
    int **chunk_desc;          /* describe the chunks (parallel) */
    db_ls_info *db_info;};

typedef struct s_par_info par_info;

struct s_par_info
   {itf_array *ifs;
    int it;};

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

timing_lev_surf
 cntr_times = {0.0, 0.0, 0.0, 0.0, 0.0, 0};

static int
 pt1d[] = {2,                          /* nodes */
	   0, 2,

	   1,                          /* edges */
	   1, 0, 2,

	   0,                          /* faces */
	   0},

 pt2d[] = {4,                          /* nodes */
	   0, 2, 6, 8,

	   4,                          /* edges */
	   1, 0, 2,
	   5, 2, 8,
	   7, 8, 6,
	   3, 6, 0,

	   1,                          /* faces */
	   4, 0, 1, 2, 3, -1,
	   4, 0, 2, 8, 6, -1,

	   0},

 pt3d[] = {8,                          /* nodes */
	   0, 2, 6, 8, 18, 20, 24, 26,

	   12,                         /* edges */
	    1,  0,  2,
	    5,  2,  8,
	    7,  8,  6,
	    3,  6,  0,
	    9,  0, 18,
	   11,  2, 20,
	   15,  6, 24,
	   17,  8, 26,
	   19, 18, 20,
	   23, 20, 26,
	   25, 26, 24,
	   21, 24, 18,

	   6,                          /* faces */
	    4,  0,  3,  2,  1, -1,
           10,  0,  5,  8,  4, -1,
           12,  3,  4, 11,  6, -1,
           14,  1,  7,  9,  5, -1,
           16,  2,  6, 10,  7, -1,
           22,  8,  9, 10, 11, -1,

	    4,  0,  6,  8,  2, -1,
           10,  0,  2, 20, 18, -1,
           12,  0, 18, 24,  6, -1,
           14, 26, 20,  2,  8, -1,
           16, 26,  8,  6, 24, -1,
           22, 26, 24, 18, 20, -1,

	   1,
	   13, 0, 26, 6, 20, 2, 24, 8, 18, -1};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ERROR_HAND - a special error handler for level surface plotting */

static void _PG_error_hand(int test, itf_array *ifs, int it, char *fmt, ...)
   {

    if (test == TRUE)
       {SC_VA_START(fmt);
	SC_VSNPRINTF(_PG.iso_err, MAXLINE, fmt);
	SC_VA_END;};

    return;}

/*--------------------------------------------------------------------------*/

/*                       GEOMETRY/TOPOLOGY ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PG_COMPUTE_ORIENTED_FACE - compute the outward normal of an oriented
 *                           - surface and return both the normal and
 *                           - the area
 */

double _PG_compute_oriented_face(double *n, double **x, int i1, int i2, int i3)
   {int i;
    double as, ac, nm;
    double dx1[DIMSPACE], dx2[DIMSPACE];

    for (i = 0; i < DIMSPACE; i++)
        {dx1[i] = x[i][i2] - x[i][i1];
	 dx2[i] = x[i][i3] - x[i][i2];};

    n[0] = dx1[1]*dx2[2] - dx1[2]*dx2[1];
    n[1] = dx1[2]*dx2[0] - dx1[0]*dx2[2];
    n[2] = dx1[0]*dx2[1] - dx1[1]*dx2[0];

    as = 0.0;
    for (i = 0; i < DIMSPACE; i++)
        {ac = n[i];
	 as += ac*ac;};

    as = sqrt(as);
    nm = (as > 0.0) ? 1.0/as : 0.0;

    for (i = 0; i < DIMSPACE; i++)
        n[i] *= nm;

    return(as);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIX_ORIENTATION - adjust the orientation of a face on the surface
 *                     - so that its normal is outward facing
 */

static int _PG_fix_orientation(int *js, int in, long *edge, double **x, int nd)
   {int ib, id, alt;
    double nc, om, nm;
    double xa, xb, xc, xd, xf, xz;
    double or[3], dx1[3], dx2[3], dxn[3];
    double y[3][27] = {{0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0,
		      0.0, 0.5, 1.0},

		     {0.0, 0.0, 0.0,
		      0.5, 0.5, 0.5,
		      1.0, 1.0, 1.0,
		      0.0, 0.0, 0.0,
		      0.5, 0.5, 0.5,
		      1.0, 1.0, 1.0,
		      0.0, 0.0, 0.0,
		      0.5, 0.5, 0.5,
		      1.0, 1.0, 1.0},

                     {0.0, 0.0, 0.0,
		      0.0, 0.0, 0.0,
		      0.0, 0.0, 0.0,
		      0.5, 0.5, 0.5,
		      0.5, 0.5, 0.5,
		      0.5, 0.5, 0.5,
		      1.0, 1.0, 1.0,
		      1.0, 1.0, 1.0,
		      1.0, 1.0, 1.0}};
    double *px;

    memset(dx1, 0, sizeof(dx1));
    memset(dx2, 0, sizeof(dx2));
    memset(dxn, 0, sizeof(dxn));

    if (x == NULL)
       {js[0] -= in;
	js[1] -= in;
	js[2] -= in;
	js[3] -= in;};

    for (id = 0; id < nd; id++)
        {px = (x == NULL) ? y[id] : x[id];

	 xa = px[js[0]];
	 xb = px[js[1]];
         xc = px[js[2]];
         xd = px[js[3]];

	 xf = xa + xb + xc;
	 xz = xf + xd;

	 dx1[id] = xb - xa;
	 dx2[id] = xc - xb;
	 dxn[id] = xf/3.0 - 0.25*xz;};

    or[0] = dx1[1]*dx2[2] - dx1[2]*dx2[1];
    or[1] = dx1[2]*dx2[0] - dx1[0]*dx2[2];
    or[2] = dx1[0]*dx2[1] - dx1[1]*dx2[0];

    nc = or[0]*dxn[0] + or[1]*dxn[1] + or[2]*dxn[2];
    om = or[0]*or[0] + or[1]*or[1] + or[2]*or[2];
    nm = dxn[0]*dxn[0] + dxn[1]*dxn[1] + dxn[2]*dxn[2];

    nc /= sqrt(ABS(om*nm) + SMALL);

    alt = (nc < -1.0e-8);
    if (alt)
       {ib      = edge[1];
	edge[1] = edge[2];
	edge[2] = ib;};

    return(alt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FILL_TOPOLOGY - fill the Nth slot in the BND and BNC arrays of
 *                   - mesh topology
 */

static void _PG_fill_topology(itf_array *ifs, int it, int n)
   {int id, is, in, iz, nc, nd, nn, nz, nbc, nmc;
    int ia, ib, ic;
    int i1, i2, i3;
    int js[4];
    int *bnc, *bnp, *pc;
    long **bnd;
    long *ncell, *pcell;
    double **xn;
    PM_set *domain;
    PM_mesh_topology *mt;
    db_ls_info *iri;
    static int ncell_count[3][4] = {{ 3,   2,   0,  0},
				    { 9,  32,   8,  0},
				    {27, 768, 192, 48}};

    LOCAL_START_TIME(cntr_times.ld_topology);

    domain = ifs->domain[it];
    iri    = ifs->db_info;

    nmc = (iri->nuc)/ifs->n_pieces;

    nd = domain->dimension;
    xn = (double **) domain->elements;
    mt = (PM_mesh_topology *) domain->topology;

    bnd = mt->boundaries;
    bnc = mt->n_cells;
    bnp = mt->n_bound_params;

    pc = ncell_count[nd-1];

/* fix the cell count */
    for (id = 0; id <= nd; id++)
        bnc[id] = pc[id];

/* make sure the coordinate arrays are big enough */
    nn = bnc[0];
    for (id = 0; id < nd; id++)
        {SIZED(double, xn[id], nn, 1000);};

/* fix the boundary array sizes
 * NOTE: this is the single most expensive operation in the
 * entire level surfact construction process
 * getting this down in time would be noticed
 */
    for (id = 1; id <= nd; id++)
        {nc = 1.1*nmc*pc[id];
	 nc = max(nc, bnc[id]);
	 SIZED(long, bnd[id], nc, 1000);

	 ERROR((bnd[id] == NULL), ifs, it,
	       "COULDN'T ALLOCATE %ld BYTES - _PG_FILL_TOPOLOGY",
	       nc);};

/* fill the n-cells down to the edges - there is some work there */
    for (id = nd; id > 1; id--)
        {ncell = bnd[id];
	 nbc   = id + 1 + (id == 2);

	 i1 = 0;
	 i2 = ncell_count[nd-1][id];
	 for (iz = i1; iz < i2; iz++)
	     {ncell[iz]   = nbc*iz;
	      ncell[iz+1] = nbc*(iz + 1);};};

/* fill the 1-cells */
    is = 0;
    in = 0;
    nn = pc[0];
    nz = pc[nd];

    ncell = bnd[1] + is*bnp[1];
    switch (nd)
       {case 1 :
	     for (iz = 0; iz < nz; iz++)
	         {ncell[2*iz]   = iz + in;
		  ncell[2*iz+1] = iz + in + 1;};

	     break;

        case 2 :
	     {int edges[] = { 4, 3, 0, 4, 0, 1,
			      4, 1, 2, 4, 2, 5,
			      4, 7, 6, 4, 6, 3,
			      4, 5, 8, 4, 8, 7 };
	      
	      for (iz = 0; iz < nz; iz++)
	          {ia = 3*iz;
		   ib = ia + 1;
		   ic = ib + 1;

		   i1 = edges[ia] + in;
		   i2 = edges[ib] + in;
		   i3 = edges[ic] + in;

		   pcell    = ncell + 4*iz;
		   pcell[0] = i1;
		   pcell[1] = i2;
		   pcell[2] = i3;
		   pcell[3] = i1;};};

	     break;

        case 3 :
	     {int edges[] = {  0,  3,  4,  0,  4,  1,  /* bottom */
			       2,  1,  4,  2,  4,  5,
			       6,  7,  4,  6,  4,  3, 
			       8,  5,  4,  8,  4,  7,

			       0, 10,  9,  0,  1, 10,  /* front */
			       2, 10,  1,  2, 11, 10,
			      18, 10, 19, 18,  9, 10, 
			      20, 10, 11, 20, 19, 10,

			       2, 14, 11,  2,  5, 14,  /* right */
			       8, 14,  5,  8, 17, 14,
			      20, 14, 23, 20, 11, 14, 
			      26, 14, 17, 26, 23, 14,

			       8, 16, 17,  8,  7, 16,  /* back */
			       6, 16,  7,  6, 15, 16,
			      26, 16, 25, 26, 17, 16, 
			      24, 16, 15, 24, 25, 16,

			       6, 12, 15,  6,  3, 12,  /* left */
			       0, 12,  3,  0,  9, 12,
			      24, 12, 21, 24, 15, 12, 
			      18, 12,  9, 18, 21, 12,

			      18, 22, 21, 18, 19, 22,  /* top */
			      20, 22, 19, 20, 23, 22,
			      24, 22, 25, 24, 21, 22, 
			      26, 22, 23, 26, 25, 22};

	      id = 13;
	      for (iz = 0; iz < nz; iz++)
	          {ia = 3*iz;
		   ib = ia + 1;
		   ic = ib + 1;

		   pcell = ncell + 16*iz;

		   js[0] = edges[ia] + in;
		   js[1] = edges[ib] + in;
		   js[2] = edges[ic] + in;

		   *pcell++ = js[0];
		   *pcell++ = js[1];
		   *pcell++ = js[2];
		   *pcell++ = js[0];

		   js[0] = id + in;
		   js[1] = edges[ib] + in;
		   js[2] = edges[ia] + in;

		   *pcell++ = js[0];
		   *pcell++ = js[1];
		   *pcell++ = js[2];
		   *pcell++ = js[0];

		   js[0] = id + in;
		   js[1] = edges[ia] + in;
		   js[2] = edges[ic] + in;

		   *pcell++ = js[0];
		   *pcell++ = js[1];
		   *pcell++ = js[2];
		   *pcell++ = js[0];

		   js[0] = id + in;
		   js[1] = edges[ic] + in;
		   js[2] = edges[ib] + in;

		   *pcell++ = js[0];
		   *pcell++ = js[1];
		   *pcell++ = js[2];
		   *pcell++ = js[0];};};

	     break;};

    LOCAL_STOP_TIME(cntr_times.ld_topology);

    return;}

/*--------------------------------------------------------------------------*/

/*                                OTHERS                                    */

/*--------------------------------------------------------------------------*/

/* _PG_MAKE_DOMAIN - make the initial estimated domain for the crossed cells
 *                 - mainly want a good guess at the space required and
 *                 - to setup templates for the mesh topology
 */

static PM_set *_PG_make_domain(itf_array *ifs, int it)
   {int id, nd, nn;
    int *bnc, *bnp;
    long **bnd;
    double **xn;
    PM_mesh_topology *mt;
    PM_set *domain;
    db_ls_info *iri;

    iri = ifs->db_info;

    nd = iri->nd;

/* allocate the domain coordinate arrays */
    nn = PM_ipow(3, nd);
    xn = PM_make_vectors(nd, nn);

/* allocate the number of boundary parameters array */
    bnp = CMAKE_N(int, nd+1);
    for (id = 0; id <= nd; id++)
        bnp[id] = 1;

/* allocate the number of cells array */
    bnc = CMAKE_N(int, nd+1);
    for (id = 0; id <= nd; id++)
        bnc[id] = 0;

/* allocate the boundary arrays */
    bnd    = CMAKE_N(long *, nd+1);
    bnd[0] = NULL;
    for (id = 1; id <= nd; id++)
        bnd[id] = CMAKE_N(long, bnp[id]*bnc[id]+1);

/* put it all together */
    mt = PM_make_topology(nd, bnp, bnc, bnd);

    domain = PM_make_ac_set("cross-cell-domain", G_DOUBLE_S, TRUE,
			    mt, nd, xn[0], xn[min(1,nd-1)], xn[min(2,nd-1)]);

/* the domain can't own the component arrays
 * because they can be resized so we have to
 * release the space now
 */
    PM_free_vectors(nd, xn);

    return(domain);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_LOAD_CELL - load up the local template with the function values */

static void _PG_load_cell(itf_array *ifs, int jo, int it)
   {int i, id, jd, io, nd, nn, dj, dk, off, ofn;
    int *dm, *cc;
    double *fn, *f, *pf, **xm, **xn, *px, *py;
    PM_set *domain;
    db_ls_info *iri;
    level_surface *itf;

    iri    = ifs->db_info;
    itf    = ifs->itf[it];
    domain = ifs->domain[it];

    xn = (double **) domain->elements;

    dm = iri->dims;
    nd = iri->nd;
    f  = iri->fd;
    cc = iri->cell;
    xm = iri->x;

    fn = itf->fn;
    io = cc[jo];

    dj = dm[0];
    dk = dm[0]*dm[1];

    pf = f + io;
    for (id = 1; id < nd; id++)
        {fn[0] = pf[0];
	 fn[2] = pf[1];
	 fn[6] = pf[dj];
	 fn[8] = pf[dj + 1];

	 pf += dk;
	 fn += 18;};

/* fill in the template node coordinates */
    off = io;
    ofn = 0;
    for (id = 1; id < nd; id++)
        {for (jd = 0; jd < nd; jd++)
	     {px = xm[jd] + off;
	      py = xn[jd] + ofn;

/* get in the corner values */
	      py[0] = px[0];
	      py[2] = px[1];
	      py[6] = px[dj];
	      py[8] = px[dj + 1];

/* get in the edge values */
	      py[1] = 0.5*(py[0] + py[2]);
	      py[3] = 0.5*(py[0] + py[6]);
	      py[5] = 0.5*(py[2] + py[8]);
	      py[7] = 0.5*(py[6] + py[8]);

	      py[4] = 0.5*(py[3] + py[5]);};

	 off += dk;
	 ofn += 18;};

    if (nd > 2)
       {nn = 9;                            /* GOTCHA: LR assumption */
	for (jd = 0; jd < nd; jd++)
	    {py = xn[jd] + nn;
	     for (i = 0; i < nn; i++)
	         py[i] = 0.5*(py[i-nn] + py[i+nn]);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_INIT_CELL - setup the state for the JO cell */

int *_PG_init_cell(itf_array *ifs, int jo, int io, int it)
   {int i, nd, nt, nv;
    int *pt, *pp;
    level_surface *itf;
    db_ls_info *iri;

    itf = ifs->itf[it];
    iri = ifs->db_info;

    nd = iri->nd;
    nt = PM_ipow(3, nd);

    itf->j_lr = io;
    itf->j_ac = jo;

/* stamp out the topology template */
    _PG_fill_topology(ifs, it, jo);

    LOCAL_START_TIME(cntr_times.init);

    pt = NULL;
    pp = NULL;
    nv = 0;
    switch (nd)
       {case 1 :
	     nv = sizeof(pt1d)/sizeof(int);
	     pp = pt1d;
	     break;
        case 2 :
	     nv = sizeof(pt2d)/sizeof(int);
	     pp = pt2d;
	     break;
        case 3 :
	     nv = sizeof(pt3d)/sizeof(int);
	     pp = pt3d;
	     break;};

    if (pp != NULL)
       {pt = CMAKE_N(int, nv);
	for (i = 0; i < nv; i++)
	    pt[i] = pp[i];

	itf->poly_topology = pt;
	itf->dn            = CMAKE_N(double, nt);
	itf->fn            = CMAKE_N(double, nt);};

    _PG_load_cell(ifs, jo, it);

    LOCAL_STOP_TIME(cntr_times.init);

    return(pt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIN_CELL - clean out the state for the JO cell */

void _PG_fin_cell(itf_array *ifs, int it)
   {level_surface *itf;

    itf = ifs->itf[it];

    CFREE(itf->fn);
    CFREE(itf->dn);
    CFREE(itf->poly_topology);

    return;}

/*--------------------------------------------------------------------------*/

/*                            SOLVER ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _PG_GRAB_POINTS - return the indices of the nodes associated with the
 *                 - cell IFC in the array JS
 */

void _PG_grab_points(int *js, int nd, int ifc, int off, double *dn, long **bnd)
   {int i, i1, i2, io, j, jo, jfc;
    long *edge;

    switch (nd)
       {case 2 :
	     edge = bnd[1] + ifc;

	     for (j = 0; j < 2; j++)
	         js[j] = edge[j] - off;

	     if (ifc % 4)
	        jo = edge[j-3] - off;
	     else
	        jo = edge[j] - off;

	     js[2] = jo;
	     break;

        case 3 :
	     i1   = 4*ifc;
	     i2   = i1 + 3;
	     edge = bnd[1];

	     for (i = i1; i <= i2; i++)
	         {io = i - i1;
		  jo = edge[i] - off;

		  js[io] = jo;};

/* pick another face of this cell */
	     if (ifc % 4)
	        jfc = ifc - 1;
	     else
	        jfc = ifc + 1;

/* find the node of the cell not on the surface */
	     i1   = 4*jfc;
	     i2   = i1 + 3;
	     edge = bnd[1];

	     jo = 0;
	     for (i = i1; i <= i2; i++)
	         {jo = edge[i] - off;
		  for (io = 0; io < nd; io++)
		      if (js[io] == jo)
			 break;

		  if (io == 3)
		     break;};

	     js[3] = jo;
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_AVE_X - move the point I0 to the average of the points NL
 *           - and return the value of F there
 */

double _PG_ave_x(int i0, int *nl, int nd, double **x, double *f)
   {int i, id, in;
    double nm, f0, x0[3];

    nm = 0.0;
    f0 = 0.0;
    for (id = 0; id < nd; id++)
        x0[id] = 0.0;

    for (i = 0; nl[i] != -1; i++)
        {nm += 1.0;
	 in  = nl[i];
	 f0 += f[in];
	 for (id = 0; id < nd; id++)
	     x0[id] += x[id][in];};

    nm = 1.0/nm;

    for (id = 0; id < nd; id++)
        x[id][i0] = nm*x0[id];

    f0 *= nm;

    return(f0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_AVE_L - move the point I0 which is between I1 and I2 to the
 *           - average of I1 and I2
 */

int _PG_ave_l(int i0, int i1, int i2, int nd, double **x)
   {int id;

    for (id = 0; id < nd; id++)
        x[id][i0] = 0.5*(x[id][i1] + x[id][i2]);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MOVE_X - move the point I0 which is between I1 and I2
 *            - to be on the surface
 */

static int _PG_move_x(int i0, int i1, int i2, int nd, double d,
		      double **x, double *f)
   {int i;
    double f1, f2, t1, t2, eps, epsm;
    double x1[3], x2[3];

    for (i = 0; i < nd; i++)
        {x1[i] = x[i][i1];
	 x2[i] = x[i][i2];};

    eps  = -1.0e-8;
    epsm = 1 - eps;

    f1 = f[i1] - d;
    f2 = f[i2] - d;

/* move the end point which is a center of the highest N cell */
    if (f2 != f1)
       {t1 = f2/(f2 - f1);
	if ((eps < t1) && (t1 < epsm))
	   {t2 = 1.0 - t1;
	    for (i = 0; i < nd; i++)
	        x[i][i0] = x1[i]*t1 + x2[i]*t2;}
	else
	   t2 = 0.0;}
    else
       t2 = -1.0;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_EXAMINE_POLY - find the d values at the fixed points
 *                  - and N-cell centers
 */

static int _PG_examine_poly(itf_array *ifs, int it)
   {int m, im, in, nc, nd, nn;
    int ia, ie, i1, i2, iu, e1;
    int nte, ntf, ntn, sit;
    int df;
    int en[3];
    int nin, nout, non, ok;
    int *nl, *el, *fn, *fe, *cl, *pt;
    int *em, *il;
    double d0, d;
    double *dn, **xn, *f;
    PM_set *domain;
    PM_mesh_topology *mt;
    level_surface *itf;

    LOCAL_START_TIME(cntr_times.surf_seg_inter);

    itf    = ifs->itf[it];
    domain = ifs->domain[it];

    d  = itf->d;
    dn = itf->dn;
    f  = itf->fn;
    pt = itf->poly_topology;

    nd = domain->dimension;
    mt = (PM_mesh_topology *) domain->topology;
    xn = (double **) domain->elements;

    nn = mt->n_cells[0];

    GET_POLY_TOPOLOGY(pt, nd, df,
		      ntn, nl, nte, el, ntf, fe, fn, nc, cl);

    SC_ASSERT(nc >= 0);

/* make it easy to spot trouble later */
    for (in = 0; in < nn; in++)
        dn[in] = HUGE;

/* determine the locations of the fixed mesh nodes with respect to
 * the surface
 */
    nin  = 0;
    nout = 0;
    non  = 0;
    for (in = 0; in < ntn; in++)
        {d0 = f[nl[in]] - d;

/* count the number of nodes inside, outside and on the surface */
	 if (d0 < 0.0)
	    nin++;
	 else if (d0 > 0.0)
	    nout++;
	 else
	    non++;

	 dn[nl[in]] = d0;};

    if (nin == ntn)
       sit = INSIDE_SURF;

    else if (nout == ntn)
       sit = OUTSIDE_SURF;

    else
       {sit = ON_SURF;

	em = CMAKE_N(int, nte);
	il = CMAKE_N(int, 10*nte);

/* check all the edges for intersection with the surface */
	for (ie = 0; ie < nte; ie++)
	    {in = el[3*ie];
	     im = in;
	     i1 = el[3*ie+1];
	     i2 = el[3*ie+2];

	     em[ie] = FALSE;

/* if the points are on opposite sides of the level_surface the
 * product of the dn's will be negative
 */
	     ok = (dn[i1]*dn[i2] < 0.0);

/* if the nodes of an edge are on opposite sides of the level_surface
 * place the logical edge center position on the surface
 */
	     if (ok)
	        {em[ie] = TRUE;
		 _PG_move_x(im, i1, i2, nd, d, xn, f);
		 dn[in] = 0.0;}

/* otherwise place the logical edge center position in the middle */
	     else
	        {en[0] = i1;
		 en[1] = i2;
		 en[2] = -1;
		 dn[in] = _PG_ave_x(im, en, nd, xn, f) - d;};};

	if (nd > 1)
	   {for (ia = 0; ia < ntf; ia++)
	        {in = fn[df*ia];
		 im = in;

/* if two edges of the face had crossings check the line
 * between the edge intersections
 */
		 m = 0;
		 for (ie = 0; ie < nte; ie++)
		     {e1 = fe[df*ia + ie + 1];
		      if (e1 == -1)
			 break;
		      if (em[e1])
			 il[m++] = el[3*e1];}

/* this test allows non-convex polygons */
		 if ((m > 0) && (m % 2 == 0))
		    {i1 = il[0];
		     i2 = il[1];
		     _PG_ave_l(im, i1, i2, nd, xn);
		     dn[in] = 0.0;}

/* if there has been a problem check for face diagonals
 * whose nodes are on opposite side of the level_surface
 */
		 else
		    {i1 = fn[df*ia + 1];
		     i2 = fn[df*ia + 3];
		     if (dn[i1]*dn[i2] >= 0.0)
		        {i1 = fn[df*ia + 2];
			 iu = min(nte, 4);
			 i2 = fn[df*ia + iu];};

/* if the points are on opposite sides of the level_surface the
 * product of the dn's will be negative
 */
		     ok = (dn[i1]*dn[i2] < 0.0);

/* if there are suitable points as determined by edge crossings
 * place the logical face center position on the line between i1 and i2
 */
		     if (ok)
		        {_PG_move_x(im, i1, i2, nd, d, xn, f);
			 dn[in] = 0.0;}

/* otherwise average the nodes of the face to find the
 * face center position
 */
		     else
		        dn[in] = _PG_ave_x(im, fn+df*ia+1, nd, xn, f) - d;};};};

	if (nd > 2)
	   {in = cl[0];
	    im = in;
	    ok = FALSE;
	    i1 = 0;
	    i2 = 0;

/* check the diagonals thru the cell */
	    for (ia = 0; TRUE; ia++)
	        {if (cl[2*ia+1] == -1)
		    break;

		 i1 = cl[2*ia+1];
		 i2 = cl[2*ia+2];

/* if the points are on opposite sides of the level_surface the
 * product of the dn's will be negative
 * NOTE: one of these must succeed
 */
		 ok = (dn[i1]*dn[i2] < 0.0);
		 if (ok)
		    {_PG_move_x(im, i1, i2, nd, d, xn, f);
		     dn[in] = 0.0;
		     break;};};

	    if ((dn[in] == HUGE) && !ok)
	       {_PG_move_x(im, i1, i2, nd, d, xn, f);
		dn[in] = 0.0;};};

      CFREE(em);
      CFREE(il);};

    LOCAL_STOP_TIME(cntr_times.surf_seg_inter);

    return(sit);}

/*--------------------------------------------------------------------------*/

/*                            MEMORY ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _PG_MAKE_SURFACE_SET - make a mapping of the contour level NO surface */

static PM_set *_PG_make_surface_set(itf_array *ifs, int no, int it)
   {int id, nd, md;
    int *bsc, *bsp;
    long **bsd;
    char t[MAXLINE];
    char *pclr;
    double *xs[DIMSPACE];
    PM_set *surface;
    PM_mesh_topology *mts;
    db_ls_info *iri;
    static char *colors[] = {"blues", "greens", "cyans",
                             "reds", "magentas", "yellows"};

    LOCAL_START_TIME(cntr_times.merge_surface);

    surface = NULL;

    iri = ifs->db_info;
    nd  = iri->nd;
    md  = nd - 1;

    for (id = 0; id < DIMSPACE; id++)
        xs[id] = NULL;

/* setup the number of cells array */
    bsc = CMAKE_N(int, nd);
    for (id = 0; id < nd; id++)
        bsc[id] = 0;

/* setup the number of boundary parameters array */
    bsp = CMAKE_N(int, nd);
    for (id = 0; id <= md; id++)
        bsp[id] = 1;

    if (nd == 2)
       bsp[1] = 2;

/* allocate the boundary arrays */
    bsd = CMAKE_N(long *, md+1);
    for (id = 0; id <= md; id++)
        bsd[id] = NULL;

/* put it all together */
    mts = PM_make_topology(md, bsp, bsc, bsd);

/* allow space in the coordinate arrays and the mesh topology */
    snprintf(t, MAXLINE, "level[%d]", no);
    surface = PM_make_ac_set(t, G_DOUBLE_S,
			     FALSE, mts, nd, xs[0], xs[1], xs[2]);

    pclr = colors[no % 6];

    surface->info = SC_append_alist(surface->info, SC_copy_alist(iri->alist));
    PG_set_attrs_set(surface,
/*		     "DRAW-AXIS",  G_INT_I, FALSE, FALSE, */
		     "LINE-COLOR", G_INT_I, FALSE, no+4,
		     "PALETTE",    G_CHAR_I,    TRUE,  pclr,
		     NULL);

    LOCAL_STOP_TIME(cntr_times.merge_surface);

    return(surface);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MAKE_IFS - initialize an itf_array */

itf_array *_PG_make_ifs(db_ls_info *iri, int ilv)
   {int ip, np;
    itf_array *ifs;

    np = iri->nthread;

    ifs = CMAKE(itf_array);

    ifs->ilev     = ilv;
    ifs->db_info  = iri;
    ifs->n_pieces = np;

/* make the parallel chunk map */
    ifs->chunk_desc = CMAKE_N(int *, np);

/* setup the single cell work spaces */
    ifs->itf = CMAKE_N(level_surface *, np);
	
/* setup the domain of the crossed cell mapping */
    ifs->domain = CMAKE_N(PM_set *, np);
	
/* initialize the domains and ranges */
    for (ip = 0; ip < np; ip++)
        {ifs->chunk_desc[ip] = CMAKE_N(int, 2);

	 ifs->itf[ip]        = NULL;
         ifs->domain[ip]     = NULL;};

/* make the N-1 dimensional boundary sets */
    _PG.bnd_surf[ilv] = _PG_make_surface_set(ifs, ilv, 0);
    SC_mark(_PG.bnd_surf[ilv], 1);

    return(ifs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MAKE_INTER_SURFACE - initialize a level intersection surface */

level_surface *_PG_make_inter_surface(db_ls_info *iri, int it)
   {int nd;
    level_surface *itf;

    nd  = iri->nd;
    itf = CMAKE(level_surface);

    itf->ip       = it;
    itf->dn       = NULL;
    itf->nd       = nd;
    itf->analyze  = FALSE;
    itf->debug    = NULL;
    itf->plot     = NULL;

    return(itf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FREE_INTER_SURFACE - release a level intersection surface */

void _PG_free_inter_surface(itf_array *ifs)
   {int ip, np;
    level_surface *itf;

    PM_rel_set(ifs->domain[0], TRUE);
    CFREE(ifs->domain);

    np = ifs->n_pieces;

    if (ifs->itf != NULL)
       {for (ip = 0; ip < np; ip++)
	    {itf = ifs->itf[ip];

	     if (itf != NULL)
	        {CFREE(itf->fn);
		 CFREE(itf->dn);
		 CFREE(itf);};

	     ifs->itf[ip] = NULL;};

	CFREE(ifs->itf);};

    for (ip = 0; ip < np; ip++)
        CFREE(ifs->chunk_desc[ip]);
    CFREE(ifs->chunk_desc);

    CFREE(ifs);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_INIT_CHUNK - initialize the memory for the IT'the chunk of work */

void _PG_init_chunk(itf_array *ifs, int it)
   {db_ls_info *iri;

    iri = ifs->db_info;

    ifs->itf[it]    = _PG_make_inter_surface(iri, it);
    ifs->domain[it] = _PG_make_domain(ifs, it);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SURFACE_CONTAINED - return TRUE iff any of the boundary n-cells in the
 *                       - range from I1 to I2 are contained in the surface
 */

static pboolean _PG_surface_contained(int *js, itf_array *ifs,
				      int ic, int id,
				      long **bnd, int it)
   {int io, off;
    pboolean ok;
    double dc, *dn;
    level_surface *itf;

    itf = ifs->itf[it];
    dn  = itf->dn;
    off = 0;

    _PG_grab_points(js, id+1, ic, off, dn, bnd);

    ok = B_T;
    for (io = 0; (io <= id) && ok; io++)
        {dc = dn[js[io]];
	 if (dc == HUGE)
	    {ok = B_F;
	     break;};

	 ok &= (dc == 0.0);}

/* if this face is on the surface we have to check that the
 * remaining node of the cell is inside the surface
 * this removes the redundant neighboring face
 */
    if (io > id)
       {dc = dn[js[io]];
	if (dc == HUGE)
	   ok = B_F;

	else
	   ok &= (dc < 0.0);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ADD_SURF_SEG - add segments for 2d contour plots */

static void _PG_add_surf_seg(int id, int nc, int nd, int *js, double **xn,
			     long **b1d, int *b1c, int *b1p, double **x1,
			     int nex, int pty)
   {int ic1, ic2, ln, me;
    long *p1;
    double *px1, *pxn;

    ic1 = js[0];
    ic2 = js[1];

    ln = b1c[0];
    me = b1c[1]*b1p[1];

/* copy the topology information over */
    p1 = SIZED(long, b1d[1], me, nex);

    p1[me++] = ln;
    p1[me++] = ln + 1;

/* copy the coordinates over */
    for (id = 0; id < nd; id++)
        {pxn = xn[id];
	 px1 = SIZED(double, x1[id], ln, nex);

	 px1[ln]   = pxn[ic1];
	 px1[ln+1] = pxn[ic2];};

    b1c[1]++;
    b1c[0] += 2;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ADD_SURF_FACET - add facets for 3d shaded contour plots */

static void _PG_add_surf_facet(int id, int nc, int nd, int *js, double **xn,
			       long **b1d, int *b1c, int *b1p, double **x1,
			       int nex, int pty)
   {int ie, in, je, ln;
    int mc, me, mn;
    long *p1;
    double or[3], om;
    double *px1, *pxn;

    mn = b1c[0];
    me = b1c[1];
    mc = b1c[2];

/* copy the topology information over */
    p1       = SIZED(long, b1d[2], mc, nex);
    p1[mc++] = me;
    p1[mc]   = me + nc;

    for (ie = 0, ln = b1c[0], je = b1c[1];
	 ie < nc;
	 ie++, ln++, je++)
        {p1     = SIZED(long, b1d[1], me, nex);
	 p1[je] = (ie == nc-1) ? b1c[0] : ln;};

    _PG_fix_orientation(js, 0, p1+b1c[1], xn, nd);

/* copy the points */
    for (in = 0, ln = b1c[0]; in < nd; in++, ln++)
        {for (id = 0; id < nd; id++)
	     {pxn = xn[id];
	      px1 = SIZED(double, x1[id], ln, nex);

	      px1[ln] = pxn[js[in]];};};

/* compute the surface normal */
    om = _PG_compute_oriented_face(or, x1, mn, mn+1, mn+2);

    SC_ASSERT(om > 1.0e-8);

/* update the cell counts if the surface is not degenerate */
/*    if (om > 1.0e-8) */
       {b1c[2]++;
	b1c[1] += nc;
	b1c[0] += (nc - 1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SORT_PERIMETER - order the perimeter segments so that the last
 *                    - point of the Nth segment is the first point of
 *                    - the (N+1)th segment
 *                    - the end of the last segment should be the same
 *                    - as the beginnin of the first segment
 *                    - that is a closed loop
 */

static void _PG_sort_perimeter(int np, int *nlst)
   {int i, j, ie, in, npm;
    int *pe, *ps;

    ps = nlst;
    pe = nlst + 1;

    npm = np - 1;
    for (i = 0; i < npm; i++)
        {ie = pe[2*i];
	 if (ie == ps[2*(i+1)])
	    continue;

	 for (j = i+2; j < np; j++)
	     {in = ps[2*j];
	      if (ie == in)
		 {EXCHANGE(nlst, i+1, j);
		  break;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ADD_SURF_PERIM - add perimeter segments for 3d contour plots */

static void _PG_add_surf_perim(int count, int last, int *nlst,
			       int id, int nc, int nd, int *js, double **xn,
			       long **b1d, int *b1c, int *b1p, double **x1,
			       int nex, int pty)
   {int ie, in, je, ln;
    int mc, me;
    long *p1;
    double *px1, *pxn;

    if (last)
       {_PG_sort_perimeter(count, nlst);

	nc = count+1;

	me = b1c[1];
	mc = b1c[2];

/* copy the topology information over */
	p1       = SIZED(long, b1d[2], mc, nex);
	p1[mc++] = me;
	p1[mc]   = me + nc;

	for (ie = 0, ln = b1c[0], je = b1c[1];
	     ie < nc;
	     ie++, ln++, je++)
	    {p1     = SIZED(long, b1d[1], me, nex);
	     p1[je] = (ie == nc-1) ? b1c[0] : ln;};

/* copy the points */
	for (in = 0, ln = b1c[0]; in < count; in++, ln++)
	    {for (id = 0; id < nd; id++)
	         {pxn = xn[id];
		  px1 = SIZED(double, x1[id], ln, nex);

		  px1[ln] = pxn[nlst[2*in]];};};

	b1c[2]++;
	b1c[1] += nc;
	b1c[0] += (nc - 1);}

    else
       {nlst[2*count]   = js[1];
	nlst[2*count+1] = js[2];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_COMPUTE_SURFACE - fill the array of markers which identify the
 *                     - actual level_surface
 *                     - naming convention
 *                     -   j -> index on surface boundary  (b1d)
 *                     -   i -> index on cell boundary  (bnd)
 *                     -   m -> number of cells
 *                     -   n -> node
 *                     -   e -> edge
 */

static void _PG_compute_surface(itf_array *ifs, int it)
   {int i, i1, i2, id, nd, ns, os;
    int ilv, nc, nex;
    int js[4], nlst[100];
    int *bnc, *bsc, *bsp;
    long **bnd, **bsd;
    double **xn, **xs;
    PM_mesh_topology *mt, *ms;
    PM_set *domain, *srf;
    PG_rendering pty;
    db_ls_info *iri;

    iri    = ifs->db_info;
    ilv    = ifs->ilev;
    domain = ifs->domain[it];

    nex = 10*iri->nmc;
    pty = iri->plot_type;

    mt = (PM_mesh_topology *) domain->topology;
    xn = (double **) domain->elements;
    nd = mt->n_dimensions;

    srf = _PG.bnd_surf[ilv];
    ms  = (PM_mesh_topology *) srf->topology;
    xs  = (double **) srf->elements;

    if (nd > 0)
       {bnd = mt->boundaries;
	bnc = mt->n_cells;

	bsd = ms->boundaries;
	bsc = ms->n_cells;
	bsp = ms->n_bound_params;

	id = nd - 1;

	i1 = 0;
	i2 = bnc[id];

	ns = 0;
	nc = nd + 1;
	for (i = i1; i < i2; i++)
	    {if (((i + 1) % 4) || (nd != 2))
	        {os = _PG_surface_contained(js, ifs, i, id, bnd, it);
		 if (os)
		    {switch (id)
		        {case 1 :
			      _PG_add_surf_seg(id, nc, nd, js, xn,
					       bsd, bsc, bsp, xs,
					       nex, pty);
			      break;

			 case 2 :
			      if (pty == PLOT_SURFACE)
				 _PG_add_surf_facet(id, nc, nd, js, xn,
						    bsd, bsc, bsp, xs,
						    nex, pty);
			      else
				 _PG_add_surf_perim(ns, FALSE, nlst,
						    id, nc, nd, js, xn,
						    bsd, bsc, bsp, xs,
						    nex, pty);
			      break;};

		     ns++;};};};

	if ((pty == PLOT_WIRE_MESH) && (ns > 0) && (nd == 3))
	   _PG_add_surf_perim(ns, TRUE, nlst,
			      id, nc, nd, js, xn,
			      bsd, bsc, bsp, xs,
			      nex, pty);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SINGLE_CELL - find the intersections in a single cell */

static void _PG_single_cell(double d0, itf_array *ifs, int jo, int io, int it)
   {int nd;
    level_surface *itf;
    db_ls_info *iri;

    itf = ifs->itf[it];
    iri = ifs->db_info;

    nd = iri->nd;

    itf->nd   = nd;
    itf->j_ac = jo;
    itf->j_lr = io;
    itf->d    = d0;

    if (jo >= 0)

/* find the intersection of the surface with the crossed cell JO */
       {_PG_examine_poly(ifs, it);

/* find the intersection surface */
	_PG_compute_surface(ifs, it);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DO_CELLS - do a chunk of crossed cells */

static int _PG_do_cells(itf_array **pifs, int *pit)
   {int i, j, it, jmn, jmx, ilev;
    int *mmc, *cd;
    double d0;
    double *lev;
    itf_array *ifs;
    db_ls_info *iri;

    ifs = *pifs;
    it  = (pit == NULL) ? 0 : *pit - 1;

    _PG_init_chunk(ifs, it);

    cd  = ifs->chunk_desc[it];
    jmn = cd[0];
    jmx = cd[1];

    ilev = ifs->ilev;
    iri  = ifs->db_info;

    mmc = iri->cell;
    lev = iri->levels;

    d0 = lev[ilev];

    for (j = jmn; j < jmx; j++)
        {i = mmc[j];

	 _PG_init_cell(ifs, j, i, it);

	 _PG_single_cell(d0, ifs, j, i, it);

	 _PG_fin_cell(ifs, it);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIND_CHUNKS - compute the chunking for this pass
 *                 - return FALSE if there are fewer chunks of work to
 *                 - do than there are processors
 */

static int _PG_find_chunks(itf_array *ifs)
   {int di, ip, np, nmc, par, cs;
    int *cd;
    db_ls_info *iri;

    np  = ifs->n_pieces;
    iri = ifs->db_info;

    nmc = iri->nmc;
    cs  = iri->chunk_strategy;
    np  = max(np, 1);

    par = TRUE;

/* by crossed cell */
    if (cs == 1)

/* if the number of crossed cells is smaller than NP then only 1 chunk */
       {di = nmc/np;
	if (di < 1)
	   {par = FALSE;
	    np  = 1;
	    ifs->n_pieces = np;};

	for (ip = 0; ip < np; ip++)
	    {cd    = ifs->chunk_desc[ip];
	     cd[0] = ip*di;
	     cd[1] = cd[0] + di;};
	cd[1] = nmc;};

    return(par);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIND_CROSSED - find all of the cells which are crossed by the
 *                  - current iso surface level
 */

static void _PG_find_crossed(itf_array *ifs)
   {int i, j, k, l, nc, nd, ilev;
    int imx, jmx, kmx, im, jm, km, dpl;
    int *dm, *cc;
    double fc, fmn, fmx;
    double *f, *levs;
    double *f1a, *f1b, *f2a, *f2b, *f3a, *f3b, *f4a, *f4b;
    db_ls_info *iri;

    iri = ifs->db_info;
    f   = iri->fd;
    nd  = iri->nd;
    dm  = iri->dims;

    levs = iri->levels;
    ilev = ifs->ilev;
    fc   = levs[ilev];

    nc = 0;
    cc = iri->cell;

    imx = dm[0];
    jmx = dm[1];
    kmx = dm[2];

    im = imx - 1;
    jm = jmx - 1;
    km = kmx - 1;

    if (nd == 2)
       {f1a = f + 0;
	f1b = f + imx + 1;
	f2a = f + 1;
	f2b = f + imx;

	for (i = 0; i < im; i++)
	    for (j = 0; j < jm; j++)
	        {l  = j*imx + i;
		 fmn = f1a[l];
		 fmx = fmn;
		 fmn = min(fmn, f1b[l]);
		 fmx = max(fmx, f1b[l]);
		 fmn = min(fmn, f2a[l]);
		 fmx = max(fmx, f2a[l]);
		 fmn = min(fmn, f2b[l]);
		 fmx = max(fmx, f2b[l]);
		 if ((fmn <= fc) && (fc <= fmx))
		    cc[nc++] = l;};}

    else if (nd == 3)
       {dpl = imx*jmx;

	f1a = f + 0;
	f1b = f + dpl + imx + 1;
	f2a = f + 1;
	f2b = f + dpl + imx;
	f3a = f + imx;
	f3b = f + dpl + 1;
	f4a = f + imx + 1;
	f4b = f + dpl;

	for (i = 0; i < im; i++)
	    for (j = 0; j < jm; j++)
	        for (k = 0; k < km; k++)
		    {l  = (k*jmx + j)*imx + i;
		     fmn = f1a[l];
		     fmx = fmn;
		     fmn = min(fmn, f1b[l]);
		     fmx = max(fmx, f1b[l]);
		     fmn = min(fmn, f2a[l]);
		     fmx = max(fmx, f2a[l]);
		     fmn = min(fmn, f2b[l]);
		     fmx = max(fmx, f2b[l]);
		     fmn = min(fmn, f3a[l]);
		     fmx = max(fmx, f3a[l]);
		     fmn = min(fmn, f3b[l]);
		     fmx = max(fmx, f3b[l]);
		     fmn = min(fmn, f4a[l]);
		     fmx = max(fmx, f4a[l]);
		     fmn = min(fmn, f4b[l]);
		     fmx = max(fmx, f4b[l]);
		     if ((fmn <= fc) && (fc <= fmx))
		        cc[nc++] = l;};};

    iri->nmc = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_COMPLETE_LEVEL_SURFACES - doublelocate arrays to their proper size */

static void _PG_complete_level_surfaces(itf_array *ifs)
   {int id, ilv, nd, nde, np;
    int *bsc, *bsp;
    long **bsd;
    double **xs;
    PM_set *srf;
    PM_mesh_topology *ms;

    ilv = ifs->ilev;
    srf = _PG.bnd_surf[ilv];
    ms  = (PM_mesh_topology *) srf->topology;
    xs  = (double **) srf->elements;
    nde = srf->dimension_elem;
    nd  = srf->dimension;

    bsd = ms->boundaries;
    bsc = ms->n_cells;
    bsp = ms->n_bound_params;

    np = bsc[0];

    srf->n_elements = np;

/* resize the coordinates */
    for (id = 0; id < nde; id++)
        {CREMAKE(xs[id], double, np);};

/* resize the cell index arrays */
    for (id = 1; id <= nd; id++)
        {CREMAKE(bsd[id], long, bsc[id]*bsp[id]+1);};

    PM_find_extrema(srf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PLOT_SURFACE - plot all of the surfaces now */

static void _PG_plot_surface(PG_device *dev, int nl, int nd)
   {int i, setup;
    char *nm;
    static char name[] = "";

/* thread the surfaces together */
    for (i = 0; i < nl-1; i++)
        _PG.bnd_surf[i]->next = _PG.bnd_surf[i+1];

    nm = _PG.bnd_surf[0]->name;
    _PG.bnd_surf[0]->name = name;

/* plot surface */
    setup = dev->supress_setup;

    dev->supress_setup = (nd == 2);
    PG_domain_plot(dev, _PG.bnd_surf[0], NULL);
    dev->supress_setup = setup;

    _PG.bnd_surf[0]->name = nm;

/* un-thread the surfaces */
    for (i = 0; i < nl; i++)
        _PG.bnd_surf[i]->next = NULL;

/* release the surfaces now that we are done */
    for (i = 0; i < nl; i++)
        PM_rel_set(_PG.bnd_surf[i], TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PLOT_LEVEL_SURFACES - plot the iso-contour level surfaces
 *                        - as described by the specifications in IFS
 */

void PG_plot_level_surfaces(PG_device *dev, db_ls_info *iri)
   {int i, nl, np, off, par;
    PFParTsk fnc;
    itf_array *ifs;

    nl  = iri->nlev;
    fnc = iri->par_task;
    par = iri->parallel;

    _PG.bnd_surf = CMAKE_N(PM_set *, nl);

    for (i = 0; i < nl; i++)
        {ifs = _PG_make_ifs(iri, i);

	 _PG_find_crossed(ifs);

/* work out the chunking */
	 par &= _PG_find_chunks(ifs);

	 START_TIME(cntr_times.net_time);

/* run the parallel task manager */
	 if (par && (fnc != NULL))
	    {np  = ifs->n_pieces;
	     np  = max(np, 1);
	     off = 1;

	     (*fnc)((PFInt) _PG_do_cells, (void **) &ifs, &np, &off, &par);}

/* do level surface construction sequentially */
	 else
	    _PG_do_cells(&ifs, NULL);

	 STOP_TIME(cntr_times.net_time);

/* resize arrays to their proper lengths */
	 _PG_complete_level_surfaces(ifs);

	 _PG_free_inter_surface(ifs);};

    _PG_plot_surface(dev, nl, iri->nd);

    CFREE(_PG.bnd_surf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ISO_NC_LR_3D - plot iso-contours for the function A on the
 *                  - Logical-Rectangular mesh specified by coordinates
 *                  - X, Y, Z which is IMX by JMX by KMX and node centered
 *                  - plot NLEV contour levels specified in LEV
 *                  - if ID is non-zero label the contour levels starting
 *                  - with ID as the first character
 *                  - the mesh topology is always ignored and is there
 *                  - to be call compatible with the AC version
 */

void _PG_iso_nc_lr_3d(PG_device *dev, double *a,
		      int ndd, double **x, double *lev,
		      int nlev, int id, void *cnnct, pcons *alist,
		      int method)
   {int i, ne, nt, par;
    int *maxes, *cc;
    double lmn, lmx, dl;
    PG_rendering pty;
    db_ls_info iri;

    maxes = (int *) cnnct;
    for (ne = 1, i = 0; i < ndd; i++)
        ne *= maxes[i];

    cc = CMAKE_N(int, ne);

/* NOTE: adjust the levels to try to get them off the nodes
 *       the min and max will certainly have come from node values
 */
    lmn = lev[0];
    lmx = lev[nlev-1];
    dl  = lmx - lmn;
    dl *= 0.001;
    lev[0]      += dl;
    lev[nlev-1] -= dl;

/* extract some needed info */
    PG_get_attrs_alist(alist,
		       "PLOT-TYPE", G_INT_I, &pty, PLOT_WIRE_MESH,
		       NULL);

/* parallel controls */
    nt  = 1;
    par = FALSE;

    iri.nd        = ndd;           /* number of dimensions */
    iri.nuc       = ne;            /* number of cells */
    iri.nmc       = 0;             /* number of crossed cells */
    iri.nlev      = nlev;          /* number of levels */
    iri.dims      = (int *) cnnct;
    iri.plot_type = pty;
    iri.alist     = alist;

    iri.nthread        = nt;
    iri.parallel       = par;      /* do the IR in parallel if not FALSE */
    iri.chunk_strategy = 1;        /* parallelization strategy */
    iri.cell           = cc;       /* lr index for crossed cell */

    iri.levels   = lev;            /* level values */
    iri.fd       = a;              /* the function whose levels are being plotted */
    iri.x        = x;              /* the LR mesh on which FD exists */
    iri.par_task = SC_do_tasks_f;  /* parallelize function */

    PG_plot_level_surfaces(dev, &iri);

    CFREE(cc);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
