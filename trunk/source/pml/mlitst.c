/*
 * MLITST.C - driver to study the interpolation routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

int
 Ximax,
 XxY,
 Yimax;

REAL
 *rtorx,
 *rtory,
 *rtovx,
 *rtovy,
 *rx,
 *ry,
 *vtorx,
 *vtory,
 *vx,
 *vy;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_RTOV - interpolate the mapping from voltages to positions
 *              - into one from positions to voltages
 */

compute_rtov(int kmax, int lmax, int ni)
   {int i, ix, iy, fsw, fsh;
    REAL **vals, **pts, **fncs;
    PM_lagrangian_mesh *grid;

    grid       = FMAKE(PM_lagrangian_mesh, "COMPUTE_RTOV:grid");
    grid->x    = vtorx;
    grid->y    = vtory;
    grid->kmax = kmax;
    grid->lmax = lmax;

    pts     = FMAKE_N(REAL *, 2, "COMPUTE_RTOV:pts");
    pts[0]  = rx;
    pts[1]  = ry;

    fncs    = FMAKE_N(REAL *, 4, "COMPUTE_RTOV:fncs");
    fncs[0] = vx;
    fncs[1] = vy;
    fncs[2] = vtorx;
    fncs[3] = vtory;

    vals    = PM_interpol(grid, pts, ni, fncs, 4);

    rtovx   = vals[0];
    rtovy   = vals[1];
    rtorx   = vals[2];
    rtory   = vals[3];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DUMP - make an ASCII dump of a map */

dump(char *s, REAL *px, REAL *py, int n, FILE *fp)
   {int i, j;

    fprintf(fp, "\n\n%s\n\n", s);

    for (i = 0; i < n; i++)
        fprintf(fp, "(%6.3f, %6.3f) ", *px++, *py++);

    fprintf(fp, "\n\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

main(int argc, char **argv)
   {int i, n;
    long d;
    FILE *fp, *out;
    char s[MAXLINE], *token, *t;

/* interupt handler */
    SC_signal(SIGINT, SIG_DFL);

    fp = fopen("remap.asc", "r");
    if (fp == NULL)
       {printf("CAN'T OPEN REMAP.ASC\n");
        exit(1);};

/* get in the number of logical voltages */
    SC_fgets(s, MAXLINE, fp);
    XxY   = atoi(s);
    Ximax = Yimax = sqrt(XxY);
    vtorx = FMAKE_N(REAL, XxY, "INTER.C:vtorx");
    vtory = FMAKE_N(REAL, XxY, "INTER.C:vtory");
    vx    = FMAKE_N(REAL, XxY, "INTER.C:vx");
    vy    = FMAKE_N(REAL, XxY, "INTER.C:vy");

/* get in the coordinates from the logical voltages */
    for (i = 0; i < XxY; i++)
        {SC_fgets(s, MAXLINE, fp);
         token    = SC_strtok(s, " ", t);
         vtorx[i] = ATOF(token);
         token    = SC_strtok(NULL, " ", t);
         vtory[i] = ATOF(token);};

    SC_fgets(s, MAXLINE, fp);

/* get in the logical voltages */
    for (i = 0; i < XxY; i++)
        {SC_fgets(s, MAXLINE, fp);
         token = SC_strtok(s, " ", t);
         vx[i] = ATOF(token);
         token = SC_strtok(NULL, " ", t);
         vy[i] = ATOF(token);};

    SC_fgets(s, MAXLINE, fp);

/* get in the number of interpolation points */
    SC_fgets(s, MAXLINE, fp);
    n  = atoi(s);
    rx = FMAKE_N(REAL, n, "INTER.C:rx");
    ry = FMAKE_N(REAL, n, "INTER.C:ry");

/* get in the list of interpolation points */
    for (i = 0; i < n; i++)
        {SC_fgets(s, MAXLINE, fp);
         token = SC_strtok(s, " ", t);
         rx[i] = ATOF(token);
         token = SC_strtok(NULL, " ", t);
         ry[i] = ATOF(token);};

    fclose(fp);

    compute_rtov(Ximax, Yimax, n);

    if ((out = fopen("foo.bar", "w")) == NULL)
       {printf("CAN'T OPEN FOO.BAR\n");
        exit(1);};

    dump("LOGICAL VOLTAGES - V", vx, vy, XxY, out);
    dump("R(V)", vtorx, vtory, XxY, out);
    dump("INTERPOLATION POINTS - Ri", rx, ry, n, out);
    dump("V(Ri)", rtovx, rtovy, n, out);
    dump("R(Ri)", rtorx, rtory, n, out);

    fclose(out);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
