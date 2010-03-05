/*
 * MLACTS.C - test the mapping from LR to AC meshes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a sample program */

int main(int c, char **v)
   {int i, j, k, l;
    int nd, nc, np;
    int kmax, lmax, k1, k2, l1, l2, ord;
    long *ncell, *pcell;
    double *x, *y, *px, *py;
    PM_mesh_topology *mt;

    kmax = 6;
    lmax = 8;

    k1 = 2;
    k2 = 4;
    l1 = 1;
    l2 = 4;

    x = FMAKE_N(double, kmax*lmax, "MLACTS.C:x");
    y = FMAKE_N(double, kmax*lmax, "MLACTS.C:y");
    for (l = 0; l < lmax; l++)
        for (k = 0; k < kmax; k++)
            {i = l*kmax + k;
             x[i] = k + 1.0;
             y[i] = l + 1.0;};

    px = x;
    py = y;

    ord = NGB_CELL;
    mt  = PM_lr_ac_mesh_2d(&px, &py, kmax, lmax, k1, k2, l1, l2, ord);

/* print cell info */
    nd = mt->n_dimensions;
    for (k = nd; 0 < k; k--)
        {ncell = mt->boundaries[k];
         nc    = mt->n_cells[k];
         np    = mt->n_bound_params[k];
         for (i = 0; i < nc; i++)
             {pcell = ncell + i*np;
              for (j = 0; j < np; j++)
                  PRINT(STDOUT, "  %4d", pcell[j]);
              PRINT(STDOUT, "\n");};
         PRINT(STDOUT, "\n");};

    SFREE(x);
    SFREE(y);

    SFREE(px);
    SFREE(py);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

