/*
 * PAPP.C - generic post processor routines for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

#define NCURVE 100

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_NEXT_UF - close the current ultra file, advance the name, and
 *             - initialize the next ultra file in the family
 */

static void _PA_next_uf(char *ufname, int flag)
   {

    if (flag)
       {if (_PA.pduf != NULL)
           PA_ERR(!PD_close(_PA.pduf),
                  "TROUBLE CLOSING ULTRA FILE %s", ufname);

        PRINT(stdout, "Ultra file %s closed\n", ufname);

/* generate the next name in the family */
        PA_advance_name(ufname);};

/* create the ultra file */
    _PA.pduf = PD_open(ufname, "w");
    PA_ERR((_PA.pduf == NULL),
           "CAN'T CREATE THE ULTRA FILE %s - _PA_NEXT_UF", ufname);

    PRINT(stdout, "Ultra file %s opened\n", ufname);

    _PA.n_curve = 0;

/* find the first cycle number */
    _PA.first_cycle = 0;
    if (!PD_read(PA_gs.pp_file, "first-cycle", &_PA.first_cycle))
       return;

/* find the last cycle number */
    _PA.last_cycle = 0;
    if (!PD_read(PA_gs.pp_file, "last-cycle", &_PA.last_cycle))
       return;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TRANSPOSE_PP - transpose the family of .pxx files and create a
 *                 - corresponding family of .uxx files for ULTRA
 *                 - NOTE: the family part isn't implemented yet
 */

void PA_transpose_pp(const char *ppname, int ntp, int nuv)
   {char s[MAXLINE], *ufname, *ps;

    if (ntp <= 0)
       return;

/* close the pp file */
    if (PA_gs.pp_file != NULL)
       {PD_write(PA_gs.pp_file, "n-time-plots",  G_INT_S, &ntp);
        PD_write(PA_gs.pp_file, "n-time-arrays", G_INT_S, &nuv);
        PD_write(PA_gs.pp_file, "last-cycle",    G_INT_S, &PA_gs.current_pp_cycle);

        PA_ERR(!PD_close(PA_gs.pp_file),
               "TROUBLE CLOSING POST PROCESSOR FILE %s - PA_TRANSPOSE_PP",
               ppname);};

/* name the first edit file */
    SC_strncpy(s, MAXLINE, ppname, -1);
    ps = strchr(s, '.');
    if (ps != NULL)
       ps[1] = 'u';
    else
       SC_strcat(s, MAXLINE, ".u00");

    ufname = CSTRSAVE(s);

/* open the pp file */
    PA_gs.pp_file = PD_open(ppname, "r");
    PA_ERR((PA_gs.pp_file == NULL),
           "CAN'T OPEN THE POST PROCESSOR FILE %s - PA_TRANSPOSE_PP",
           ppname);

/* create the ultra file */
    _PA_next_uf(ufname, FALSE);

/* process the pp file */
    _PA_proc_time(ufname);

    PA_ERR(!PD_close(_PA.pduf),
           "TROUBLE CLOSING ULTRA FILE %s - PA_TRANSPOSE_PP", ufname);
    printf("Ultra file %s closed\n", ufname);

    PA_ERR(!PD_close(PA_gs.pp_file),
           "TROUBLE CLOSING POST PROCESSOR FILE %s - PA_TRANSPOSE_PP",
           ppname);

    PA_gs.pp_file = NULL;
    _PA.pduf = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_T_WR_DATA - write out the time data
 *               - which means gather the time stripes from the pp file
 *               - and transform it into curves
 *               - GOTCHA: this logic is not correct for file families
 *               -         and it is way wrong for time plots with
 *               -         different frequencies of editting!!!!
 *               -         This gotcha may or may not be true now.        
 */

static void _PA_t_wr_data(int fcyc, int nfirst, int nlast, int n_dom)
   {int i, j, n_arrays, nptm;
    long ind[3];
    char type[10];
    double *stripe;
    double **crve;

/* allocate the data arrays */
    n_arrays = nlast - nfirst + 1 + n_dom;
    crve     = CMAKE_N(double *, n_arrays);
    stripe   = CMAKE_N(double, n_arrays);

    SC_strncpy(type, 10, G_DOUBLE_S, -1);

/* find the maximum possible curve length */
    nptm = _PA.last_cycle - _PA.first_cycle + 1;

    for (i = 0; i < n_arrays; i++)
        crve[i] = CMAKE_N(double, nptm);

/* read the data in from the pp file */
    for (i = 0; i < nptm; i++)

/* read in the domain data */
        {if (n_dom > 0)
            {snprintf(_PA.pp_title, MAXLINE, "td%d[0:%d]", i+fcyc, n_dom);
             PA_ERR(!PD_read(PA_gs.pp_file, _PA.pp_title, stripe),
                    "EXPECTED DOMAIN DATA UNDER %s - _PA_T_WR_DATA",
                    _PA.pp_title);};

/* read in the range data */
         snprintf(_PA.pp_title, MAXLINE, "td%d[%d:%d]",
                 i+fcyc, nfirst + n_dom, nlast + n_dom);
         PA_ERR(!PD_read(PA_gs.pp_file, _PA.pp_title, stripe + n_dom),
                "EXPECTED TIME PLOT DATA UNDER %s - _PA_T_WR_DATA",
                _PA.pp_title);

/* transpose through the copy */
         for (j = 0; j < n_arrays; j++)
             crve[j][i] = stripe[j];};

/* write out the time plot domain data */
    ind[0] = 1L;
    ind[2] = 1L;
    for (i = 0; i < n_dom; i++)
        {nptm   = SC_arrlen(crve[i])/SIZEOF(G_DOUBLE_S);
         ind[1] = nptm;

         snprintf(_PA.pp_title, MAXLINE, "td%d", i);

         PD_write_alt(_PA.pduf, _PA.pp_title, type, crve[i], 1, ind);
         CFREE(crve[i]);};

/* write out the time plot range data */
    ind[0] = 1L;
    ind[2] = 1L;
    for (i = nfirst, j = n_dom; i <= nlast; i++, j++)
        {nptm   = SC_arrlen(crve[j])/SIZEOF(G_DOUBLE_S);
         ind[1] = nptm;

         snprintf(_PA.pp_title, MAXLINE, "tn%d", i);
         PD_write(_PA.pduf, _PA.pp_title, G_INT_S, &nptm);

         snprintf(_PA.pp_title, MAXLINE, "td%d", i + n_dom);
         PD_write_alt(_PA.pduf, _PA.pp_title, type, crve[j], 1, ind);

         CFREE(crve[j]);};

    CFREE(stripe);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROC_TIME - process time plots
 *               - NOTE: the logic here may give trouble if the number of
 *               -       time plots requested is greater than NCURVE.
 *               -       The problem involves data for one axis ending up
 *               -       in a different file from the other axis.
 *               - GOTCHA: this logic is not correct for file families
 *               -         and it is way wrong for time plots with
 *               -         different frequencies of editting!!!!
 *               -         This gotcha may or may not be true now.        
 */

void _PA_proc_time(char *ufname)
   {int it, nt, ntu, nfirst, n_dom, nd;
    long ind[3];
    char *labl, *npts, *s;

/* find the number of time plots */
    if (!PD_read(PA_gs.pp_file, "n-time-plots", &nt))
       return;
    if (nt < 1)
       return;

/* find the number of time data arrays */
    if (!PD_read(PA_gs.pp_file, "n-time-arrays", &ntu))
       return;

/* find the number of time domain data arrays */
    nd = 0;
    if (!PD_read(PA_gs.pp_file, "n-time-domains", &n_dom))
       n_dom = 2;

/* process the time plot tags */
    ind[0] = 0L;
    ind[2] = 1L;
    nfirst = 0;
    for (it = 0; it < nt; it++)
        {snprintf(_PA.pp_title, MAXLINE, "tt%d", it);                     /* read the tag */

         PA_ERR(!PD_read(PA_gs.pp_file, _PA.pp_title, _PA.pp_bf),
                "EXPECTED TIME PLOT %s - PROC-TIME", _PA.pp_title);

/* enter the ultra tag */
         snprintf(_PA.pp_title, MAXLINE, "curve%04d", _PA.n_curve++);
         ind[1] = strlen(_PA.pp_bf) + 1;
         PD_write_alt(_PA.pduf, _PA.pp_title, G_CHAR_S, _PA.pp_bf, 1, ind);

/* extract the names of the label, x array, y array and number of points */
         labl = SC_strtok(_PA.pp_bf, "|", s);
         if (labl == NULL)
            break;
         npts = SC_strtok(NULL, "|", s);
         if (npts == NULL)
            break;

/* copy the label */
         PA_ERR(!PD_read(PA_gs.pp_file, labl, _PA.pp_ubf),
                "BAD LABEL - PROC-TIME");
         ind[1] = strlen(_PA.pp_ubf) + 1;
         PD_write_alt(_PA.pduf, labl, G_CHAR_S, _PA.pp_ubf, 1, ind);

/* if the maximum number of curves is reached
 * write out the curves and start a new file
 */
         if (_PA.n_curve >= NCURVE)
            {_PA_t_wr_data(_PA.first_cycle, nfirst, it, nd);
             nfirst = it + 1;
             nd     = n_dom;
             _PA_next_uf(ufname, TRUE);};};

/* write out the remaining curves */
    _PA_t_wr_data(_PA.first_cycle, nfirst, ntu, nd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
