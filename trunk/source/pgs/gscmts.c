/*
 * GSCMTS.C - test the color map display routines in PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

main()
   {char *s;
    PG_device *SCR_dev, *SCR_dew;

    s = SC_strsavef("WINDOW", "char*:GSCMTS:s");

    SCR_dev = PG_make_device(s, "COLOR", "PGS Image Test");
    PG_white_background(SCR_dev, FALSE);
    PG_open_device(SCR_dev, 0.05, 0.1, 0.4, 0.4);

    SCR_dew = PG_make_device(s, "COLOR", "PGS Image Test");
    PG_white_background(SCR_dew, TRUE);
    PG_open_device(SCR_dew, 0.55, 0.1, 0.4, 0.4);

    PG_set_viewspace(SCR_dev, 2, NORMC, NULL);
    PG_set_viewspace(SCR, 2, WORLDC, NULL);

    PG_set_viewspace(SCR_dew, 2, NORMC, NULL);
    PG_set_viewspace(SCR, 2, WORLDC, NULL);

/* draw the first image */
    PG_set_palette(SCR_dev, "spectrum");
    PG_show_colormap(SCR_dev, FALSE);

    PG_set_palette(SCR_dew, "bw");
    PG_show_colormap(SCR_dew, TRUE);

    SC_pause();

    PG_close_device(SCR_dev);
    PG_close_device(SCR_dew);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
