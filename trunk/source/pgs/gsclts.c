/*
 * GSCLTS.C - test of graphics library
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

char
 *clrs[] = {"BLACK",
            "WHITE",
            "GRAY",
            "DARK_GRAY",
            "BLUE",
            "GREEN",
            "CYAN",
            "RED",
            "MAGENTA",
            "BROWN",
            "DARK_BLUE",
            "DARK_GREEN",
            "DARK_CYAN",
            "DARK_RED",
            "YELLOW",
            "DARK_MAGENTA"};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(argc, argv)
   int argc;
   char **argv;
   {int i, n, mapped;
    double y, dy;
    double ndc[PG_BOXSZ];
    double x1[PG_SPACEDM], x2[PG_SPACEDM], p[PG_SPACEDM];
    PG_device *SCR_dev, *SCR_dew, *PS_dev, *CGM_dev;

    mapped = TRUE;
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'm' : mapped = FALSE;
                            break;};}
         else
            break;};

    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Test A");
    PG_white_background(SCR_dev, TRUE);

    x1[0] = 0.0;
    x1[1] = 0.0;
    PG_fset_viewport_pos(SCR_dev, x1);

    x1[0] = 1.0;
    x1[1] = 0.0;
    PG_fset_viewport_shape(SCR_dev, x1, 1.0);

    PG_open_device(SCR_dev, 0.1, 0.1, 0.4, 0.4);

    PG_set_viewspace(SCR_dev, 2, NORMC, NULL);
    PG_set_viewspace(SCR_dev, 2, WORLDC, NULL);

    SCR_dew = PG_make_device("WINDOW", "COLOR", "PGS Test B");
    PG_white_background(SCR_dew, FALSE);

    x1[0] = 0.0;
    x1[1] = 0.0;
    PG_fset_viewport_pos(SCR_dew, x1);

    x1[0] = 1.0;
    x1[1] = 0.0;
    PG_fset_viewport_shape(SCR_dew, x1, 1.0);

    PG_open_device(SCR_dew, 0.5, 0.1, 0.4, 0.4);

    PG_set_viewspace(SCR_dew, 2, NORMC, NULL);
    PG_set_viewspace(SCR_dew, 2, WORLDC, NULL);

    PS_dev = PG_make_device("PS", "COLOR", "gsclts");
    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

    ndc[0] = 0.2;
    ndc[1] = 0.8;
    ndc[2] = 0.2;
    ndc[3] = 0.8;
    PG_set_viewspace(PS_dev, 2, NORMC, ndc);
    PG_set_viewspace(PS_dev, 2, WORLDC, NULL);

    CGM_dev = PG_make_device("CGM", "COLOR", "gsclts");
    PG_open_device(CGM_dev, 0.0, 0.0, 0.0, 0.0);

    PG_set_viewspace(CGM_dev, 2, NORMC, NULL);
    PG_set_viewspace(CGM_dev, 2, WORLDC, NULL);

/* clear screens - this also sets the CGM page properly and is mandatory */
    PG_clear_window(SCR_dev);
    PG_clear_window(SCR_dew);
    PG_clear_window(PS_dev);
    PG_clear_window(CGM_dev);

    x1[0] = 0.0;
    x2[0] = 0.5;

    n  = 16;
    dy = 1.0/(n + 1.0);
    y  = 0.5*dy;
    for (i = 0; i < n; i++)
        {x1[1] = y;
	 x2[1] = y;

	 p[0] = 0.6;
	 p[1] = y;

	 PG_make_device_current(SCR_dev);
         PG_set_color_line(SCR_dev, i, mapped);
	 PG_draw_line_n(SCR_dev, 2, WORLDC, x1, x2, SCR_dev->clipping);
         PG_set_color_text(SCR_dev, i, mapped);
         PG_write_n(SCR_dev, 2, WORLDC, p, "%d %s", i, clrs[i]);

         PG_make_device_current(SCR_dew);
         PG_set_color_line(SCR_dew, i, mapped);
	 PG_draw_line_n(SCR_dew, 2, WORLDC, x1, x2, SCR_dew->clipping);
         PG_set_color_text(SCR_dew, i, mapped);
         PG_write_n(SCR_dew, 2, WORLDC, p, "%d %s", i, clrs[i]);

         PG_make_device_current(PS_dev);
         PG_set_color_line(PS_dev, i, mapped);
	 PG_draw_line_n(PS_dev, 2, WORLDC, x1, x2, PS_dev->clipping);
         PG_set_color_text(PS_dev, i, mapped);
         PG_write_n(PS_dev, 2, WORLDC, p, "%d %s", i, clrs[i]);

         PG_make_device_current(CGM_dev);
         PG_set_color_line(CGM_dev, i, mapped);
	 PG_draw_line_n(CGM_dev, 2, WORLDC, x1, x2, CGM_dev->clipping);
         PG_set_color_text(CGM_dev, i, mapped);
         PG_write_n(CGM_dev, 2, WORLDC, p, "%d %s", i, clrs[i]);

         y += dy;};

    PG_update_vs(SCR_dew);
    PG_update_vs(SCR_dev);
    PG_update_vs(PS_dev);
    PG_update_vs(CGM_dev);

    SC_pause();

    PG_close_device(CGM_dev);
    PG_close_device(PS_dev);
    PG_close_device(SCR_dew);
    PG_close_device(SCR_dev);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
