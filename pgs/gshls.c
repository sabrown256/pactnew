/*
 * GSHLS.C - hidden line/surface removal routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define COMPLETE 2            /* completely hidden */
#define PARTIAL  1            /* partially obscured */

#define REMEMBER_TRI(a, lst, n, nx)                                         \
    {if (lst == NULL)                                                       \
        {nx  = 1000;                                                        \
	 n   = 0;                                                           \
	 lst = CMAKE_N(PG_triangle, nx);};                                  \
     lst[n++] = *a;                                                         \
     if (n >= nx)                                                           \
	{nx += 1000;                                                        \
	 CREMAKE(lst, PG_triangle, nx);};}

static int
 debug = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRNODES - diagnostic print of N nodes */

static void dprnodes(PG_node *a, int n)
   {int i;
    PG_node *node;
    double *X;

    for (i = 0, node = a; i < n; i++, node++)
        {X = node->x;
         PRINT(stdout, "(%9.2e,%9.2e,%9.2e) %d %9.2e\n",
	       X[0], X[1], X[2], node->indx, node->val);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRIANGULARIZE_AC - build a list of triangles from an
 *                     - Arbitrarily-Connected description of connectivity
 */

static PG_triangle *PG_triangularize_ac(PG_device *dev, PM_mesh_topology *mt,
					double **r, double *f,
					double norm, int *pn, int *pnx,
					double **orient)
   {int i, id, jf, je, je1, je2, nfp, nep;
    int of, oe, nf, ne, nn, ni, nd, ntri, ntrix, off;
    int *nc, *np, n[PG_SPACEDM];
    long **cells, *faces, *edges;
    double p1, nrm;
    double x1[PG_SPACEDM], x2[PG_SPACEDM], x3[PG_SPACEDM];
    double p[PG_SPACEDM], d[PG_SPACEDM][2];
    double *box;
    double *X, *lorient, **t;
    PG_node *node;
    PG_triangle *tri, *a;
    PG_dev_geometry *g;

    g = &dev->g;

    nd    = 3;
    tri   = NULL;
    ntri  = 0;
    ntrix = 0;

    cells = mt->boundaries;
    faces = cells[2];
    edges = cells[1];

    nc = mt->n_cells;
    nf = nc[2];
    ne = nc[1];
    nn = 2*ne;

    SC_ASSERT(nn > 0);

    t = PM_copy_vectors(nd, nc[0], r);
    if (t == NULL)
       return(NULL);

    box = g->wc;

    PG_rotate_vectors(dev, nd, nc[0], t);

    np  = mt->n_bound_params;
    nfp = np[2];
    nep = np[1];
    switch (mt->n_dimensions)
	{case 2 :
	      ntri = 0;
	      for (jf = 0; jf < nf; jf++)
		  {of  = jf*nfp;
		   je1 = faces[of];
		   je2 = faces[of+1];
                   ntri += (je2 - je1 - 1);};

              ntrix = 1000*(1 + ((int) (ntri/1000)));
              tri   = CMAKE_N(PG_triangle, ntrix);

              *orient = CMAKE_N(double, ntrix);
              lorient = *orient;

              ntri = 0;
	      off  = (nfp < 2);
	      for (jf = 0; jf < nf; jf++)
		  {of  = jf*nfp;
		   je1 = faces[of];
		   je2 = faces[of+1] - 2*off;

                   n[0] = edges[je1*nep];
		   for (je = je1+1; je < je2; je++)
		       {oe   = je*nep;
			n[1] = edges[oe];
			n[2] = edges[oe+1];

			for (id = 0; id < nd; id++)
			    {x1[id] = t[id][n[0]];
			     x2[id] = t[id][n[1]];
			     x3[id] = t[id][n[2]];};

			if ((PG_box_contains(nd, box, x1) == TRUE) &&
			    (PG_box_contains(nd, box, x2) == TRUE) &&
			    (PG_box_contains(nd, box, x3) == TRUE))
			   {for (id = 0; id < nd; id++)
			        {d[id][0] = x3[id] - x2[id];
				 d[id][1] = x1[id] - x2[id];};
				 
			    p[0] = d[1][0]*d[2][1] - d[2][0]*d[1][1];
			    p[1] = d[2][0]*d[0][1] - d[0][0]*d[2][1];
			    p[2] = d[0][0]*d[1][1] - d[1][0]*d[0][1];

/* normalize the perpendicular */
			    p1 = SMALL;
			    for (id = 0; id < nd; id++)
			        p1 += p[id]*p[id];
			    nrm = 1.0/sqrt(p1);
			    for (id = 0; id < nd; id++)
			        p[id] *= nrm;

			    if (_PG.no_order || _PG.depth_sort)
			       {lorient[ntri] = 1.0;

				a = tri + ntri++;

/* get in the node info for the triangle */
				for (i = 0; i < 3; i++)
				    {node = &TRI_NODE(a, i);
				     ni   = n[i];

				     X = node->x;
				     for (id = 0; id < nd; id++)
				         X[id] = t[id][ni];

				     node->indx = ni;
				     if (f != NULL)
				        node->val = f[ni];
				     else
				        node->val = 0.0;};

/* get in the perpendicular for the triangle */
				for (id = 0; id < nd; id++)
				    a->norm[id] = p[id];};};};};
	    break;};

    PM_free_vectors(nd, t);

    *pn  = ntri;
    *pnx = ntrix;

    return(tri);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRIANGULARIZE_LR - build a list of triangles from a Logical-Rectangular
 *                     - description of connectivity
 */

static PG_triangle *PG_triangularize_lr(PG_device *dev,
					int *maxes, double **r, double *f,
					double norm, int *pn, int *pnx,
					double **orient)
   {int ntri, ntrix;
    PG_triangle *tri;

    tri   = NULL;
    ntri  = 0;
    ntrix = 0;

    *pn  = ntri;
    *pnx = ntrix;

    *orient = NULL;

    return(tri);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRIANGULARIZE - build a list of triangles from a PM_mesh_topology */

static PG_triangle *PG_triangularize(PG_device *dev, char *type, double **r,
				     double *f, void *cnnct,
				     double norm, int *pn, int *pnx,
				     double **orient)
   {PG_triangle *tri;

    if (strcmp(type, G_PM_MESH_TOPOLOGY_S) == 0)
       tri = PG_triangularize_ac(dev, (PM_mesh_topology *) cnnct,
				 r, f,
				 norm, pn, pnx, orient);
    else
       tri = PG_triangularize_lr(dev, (int *) cnnct, r, f,
				 norm, pn, pnx, orient);

    return(tri);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DEPTH_SORT - sort an array of PG_triangles on z */

static PG_triangle *_PG_depth_sort(PG_triangle *trial, int ntr,
				   double *orient, int *pno, int *pni)
   {int i, id, itr;
    int *ind;
    double third, zc, *z;
    PG_node *node[PG_SPACEDM];
    PG_triangle *triout, *a;
 
    z      = CMAKE_N(double, ntr);
    ind    = CMAKE_N(int, ntr);
    triout = CMAKE_N(PG_triangle, ntr);

    third = 1.0/3.0;

    for (i = 0; i < ntr; i++)
        {a = trial + i;

	 zc = 0.0;
	 for (id = 0; id < PG_SPACEDM; id++)
	     {node[id] = &TRI_NODE(a, id);
	      zc += node[id]->x[2];};

         z[i]   = third*zc;
         ind[i] = i;}

    PM_q_sort(z, ind, ntr);

    if (orient != NULL)
       {itr = 0;
        for (i = 0; i < ntr; i++)
            if (orient[ind[i]] <= 0)
               {triout[itr] = trial[ind[i]];
                itr++;};
	*pni = itr;

        for (i = 0; i < ntr; i++)
            if (orient[ind[i]] > 0)
               {triout[itr] = trial[ind[i]];
	       itr++;};
	*pno = itr;}

    else
       {for (i = 0; i < ntr; i++)
            triout[i] = trial[ind[i]];

	*pno = ntr;
	*pni = 0;};

    CFREE(z);
    CFREE(ind);

    return(triout);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_HLS_REMOVE - process all elements building up a list of visible ones
 *                - input is the set of nodal coordinate vectors
 *                - and their connectivity
 *                - output is the visible triangles
 */

PG_triangle *_PG_hls_remove(PG_device *dev, char *type, 
			    double **r, double *f,
			    void *cnnct, pcons *alst, int *pn)
   {int ntr, ntrx, nvi;
    int ntrin, ntrout;
    double norm;
    double *orient;
    PG_node inter[9];
    PG_triangle *trial, *vis;

    PG_get_attrs_alist(alst,
		       "NORMAL-DIRECTION", G_DOUBLE_I, &norm, 1.0,
		       NULL);
    nvi    = 0;
    vis    = NULL;
    orient = NULL;
    trial  = PG_triangularize(dev, type, r, f, cnnct,
			      norm, &ntr, &ntrx, &orient);
    if (trial == NULL)
       return(NULL);

    SC_MEM_INIT_N(PG_node, inter, 9);

/* reference dprnodes to make some warnings go away */
    if (debug)
       dprnodes(inter, 9);

    if (_PG.no_order)
       {vis = trial;
	nvi = ntr;}

    else if (_PG.depth_sort)
       {vis = _PG_depth_sort(trial, ntr, orient, &ntrout, &ntrin);
        nvi = ntr;
        CFREE(trial);};

    if (orient != NULL)
       {CFREE(orient);};

    *pn = nvi;

    return(vis);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

