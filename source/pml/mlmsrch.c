/*
 * MLMSRCH.C - mesh search routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INIT_SEARCH_MAP - initialize the existence map for a mesh search */

static void _PM_init_search_map(PM_mesh_map *sm, int *map)
   {int i, j, k, l, n, id, nd;
    int imx, jmx, kmx;
    int *dims;
    char *emap;

    nd   = sm->nd;
    dims = sm->dim;

    for (n = 1, id = 0; id < nd; id++)
        n *= dims[id];

    emap = FMAKE_N(char, n, "_PM_INIT_SEARCH_MAP:emap");

    if (map == NULL)
       {for (i = 0; i < n; i++)
	    emap[i] = 1;}
    else
       {for (i = 0; i < n; i++)
	    emap[i] = (map[i] != 0);};

/* GOTCHA: this limits things to 3D at most */
    imx = dims[0];
    jmx = (nd > 1) ? dims[1] : 1;
    kmx = (nd > 2) ? dims[2] : 1;

    switch (nd)
       {case 3:
	     for (k = 0; k < kmx; k++)
	         for (j = 0; j < jmx; j++)
		     for (i = 0; i < imx; i++)
		         {if ((k == 0) || (k == kmx-1) ||
			      (j == 0) || (j == jmx-1) ||
			      (i == 0) || (i == imx-1))
			     {l = i + imx*(j + jmx*k);
			      emap[l] = 0;};};
	     break;

        case 2:
	     for (j = 0; j < jmx; j++)
	         for (i = 0; i < imx; i++)
		     {if ((j == 0) || (j == jmx-1) ||
			  (i == 0) || (i == imx-1))
			 {l = i + imx*j;
			  emap[l] = 0;};};
	     break;

        case 1:
	     for (i = 0; i < imx; i++)
	         {if ((i == 0) || (i == imx-1))
		     emap[i] = 0;};
	     break;};

    sm->emap = emap;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INIT_MESH_SEARCH - setup a mesh search database */

PM_mesh_map *PM_init_mesh_search(int nd, int *dims, double **r, int *map)
   {int i, n, id, nc, nl;
    long *indx;
    double rc, rmn, rmx;
    double *dr, *rmin, *pr;
    PM_mesh_map *sm;

    nl = PM_ipow(2, 9-nd);
    nc = PM_ipow(nl, nd);

    indx = FMAKE_N(long, nc, "PM_INIT_MESH_SEARCH:indx");
    for (i = 0; i < nc; i++)
        indx[i] = -1;

    dr   = FMAKE_N(double, nd, "PM_INIT_MESH_SEARCH:dr");
    rmin = FMAKE_N(double, nd, "PM_INIT_MESH_SEARCH:rmin");

    for (n = 1, id = 0; id < nd; id++)
        n *= dims[id];

    for (id = 0; id < nd; id++)
        {rmn =  HUGE;
	 rmx = -HUGE;
	 pr  = r[id];
	 if (map == NULL)
	    {for (i = 0; i < n; i++)
	         {rc  = pr[i];
		  rmn = min(rmn, rc);
		  rmx = max(rmx, rc);};}
	 else
	    {for (i = 0; i < n; i++)
	         {if (map[i] != 0)
		     {rc  = pr[i];
		      rmn = min(rmn, rc);
		      rmx = max(rmx, rc);};};};

	 rmin[id] = rmn;
	 dr[id]   = (rmx - rmn)/nl;};

    sm = FMAKE(PM_mesh_map, "PM_INIT_MESH_SEARCH:sm");
    sm->nd   = nd;
    sm->nl   = nl;
    sm->nc   = nc;
    sm->indx = indx;
    sm->dim  = dims;
    sm->r    = r;
    sm->dr   = dr;
    sm->rmin = rmin;

    _PM_init_search_map(sm, map);

    return(sm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIN_MESH_SEARCH - release a mesh search database */

void PM_fin_mesh_search(PM_mesh_map *sm)
   {

    SFREE(sm->emap);
    SFREE(sm->dr);
    SFREE(sm->rmin);
    SFREE(sm->indx);
    SFREE(sm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LR_MESH_LOOKUP - given a physical point return the index of the cell
 *                   - in the LR mesh containing it
 */

long PM_lr_mesh_lookup(PM_mesh_map *sm, double *p)
   {int i, l, n, np, id, ic, it, i1, i2, nd, ne, nl;
    int e1, e2;
    int *dim;
    char *emap;
    long j;
    long *indx, ip[10], nxt[5], in[5], cross[5];
    double *rmn, *dr, **r;

    nd   = sm->nd;
    nl   = sm->nl;
    rmn  = sm->rmin;
    dr   = sm->dr;
    r    = sm->r;
    dim  = sm->dim;
    indx = sm->indx;
    emap = sm->emap;

/* get logical coordinate and index I into the map */
    for (i = 0, id = 0; id < nd; id++)
        {ic = (p[id] - rmn[id])/dr[id];
	 ic = max(ic, 0);
	 ic = min(ic, nl-1);

	 ip[id] = ic;
         i     += (i*nd + ic);};

/* get initial guess */
    j = indx[i];
    if (j < 0)
       {for (j = dim[nd-1]/2, id = nd-2; id >= 0; id--)
	    j = (j*dim[id] + dim[id]/2);};

    n  = 4;
    np = n + 1;
    nxt[0] = dim[0];
    nxt[1] = -1;
    nxt[2] = -nxt[0];
    nxt[3] = 1;
    nxt[4] = nxt[0];

    for (it = 0; TRUE; it++)

/* get the points for a polygon */
        {in[0]    = j;
	 cross[0] = 0;
	 for (l = 1; l < np; l++)
	     {in[l]    = in[l-1] + nxt[l];
	      cross[l] = 0;};

/* loop over edges of the polygon */
	 for (ne = 0, l = 0; l < n; l++)
	     {i1 = in[l];
	      i2 = in[l+1];
	      e1 = emap[i1];
	      e2 = emap[i2];

/* if we are on the other side of this edge remember to cross */
	      if ((e1 || e2) &&
                  (PM_DELTA_CROSS_2D(r[0][i1], r[1][i1], r[0][i2], r[1][i2],
				     p[0], p[1]) < 0.0))
		 cross[l] = nxt[l];
	      else
	         ne++;};

/* if it is inside all edges of the polygon this is it */
	 if (ne == n)
	    break;
	 else
	    {for (l = 0; l < np; l++)
	         j += cross[l];};};

/* remember this one */
    indx[i] = j;

    return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

