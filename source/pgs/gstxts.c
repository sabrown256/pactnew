/*
 * GSTXTS.C - test of graphics library
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SF_DT - set the font and draw the text */

static void sf_dt(PG_device *dev, double x1, double y1,
		  char *face, char *style, int size)
   {double bx[PG_BOXSZ], dx[PG_SPACEDM], p[PG_SPACEDM];

    PG_fset_font(dev, face, style, size);

    PG_get_text_ext_n(dev, 2, WORLDC, "foo", dx);

    bx[0] = x1;
    bx[1] = x1 + dx[0];
    bx[2] = y1;
    bx[3] = y1 + dx[1];

/* write some text and draw a box around it */
    p[0] = bx[0];
    p[1] = bx[2];
    PG_write_n(dev, 2, WORLDC, p, "%s", "foo");

    PG_draw_box_n(dev, 2, WORLDC, bx);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_DEV - test the entire device */

static void test_dev(PG_device *dev)
   {

    PG_fset_line_color(dev, dev->BLACK, TRUE);
    PG_fset_text_color(dev, dev->BLACK, TRUE);

    sf_dt(dev, .1, .9, "helvetica", "medium", 12);
    sf_dt(dev, .1, .8, "helvetica", "italic", 12);
    sf_dt(dev, .1, .7, "helvetica", "bold", 12);
    sf_dt(dev, .1, .6, "helvetica", "bold-italic", 12);

    sf_dt(dev, .1, .50, "helvetica", "medium", 10);
    sf_dt(dev, .1, .45, "helvetica", "italic", 10);
    sf_dt(dev, .1, .40, "helvetica", "bold", 10);
    sf_dt(dev, .1, .35, "helvetica", "bold-italic", 10);

    sf_dt(dev, .1, .30, "helvetica", "medium", 8);
    sf_dt(dev, .1, .25, "helvetica", "italic", 8);
    sf_dt(dev, .1, .20, "helvetica", "bold", 8);
    sf_dt(dev, .1, .15, "helvetica", "bold-italic", 8);

    sf_dt(dev, .3, .9, "times", "medium", 12);
    sf_dt(dev, .3, .8, "times", "italic", 12);
    sf_dt(dev, .3, .7, "times", "bold", 12);
    sf_dt(dev, .3, .6, "times", "bold-italic", 12);

    sf_dt(dev, .3, .50, "times", "medium", 10);
    sf_dt(dev, .3, .45, "times", "italic", 10);
    sf_dt(dev, .3, .40, "times", "bold", 10);
    sf_dt(dev, .3, .35, "times", "bold-italic", 10);

    sf_dt(dev, .3, .30, "times", "medium", 8);
    sf_dt(dev, .3, .25, "times", "italic", 8);
    sf_dt(dev, .3, .20, "times", "bold", 8);
    sf_dt(dev, .3, .15, "times", "bold-italic", 8);

    sf_dt(dev, .5, .9, "courier", "medium", 12);
    sf_dt(dev, .5, .8, "courier", "italic", 12);
    sf_dt(dev, .5, .7, "courier", "bold", 12);
    sf_dt(dev, .5, .6, "courier", "bold-italic", 12);

    sf_dt(dev, .5, .50, "courier", "medium", 10);
    sf_dt(dev, .5, .45, "courier", "italic", 10);
    sf_dt(dev, .5, .40, "courier", "bold", 10);
    sf_dt(dev, .5, .35, "courier", "bold-italic", 10);

    sf_dt(dev, .5, .30, "courier", "medium", 8);
    sf_dt(dev, .5, .25, "courier", "italic", 8);
    sf_dt(dev, .5, .20, "courier", "bold", 8);
    sf_dt(dev, .5, .15, "courier", "bold-italic", 8);

    PG_update_vs(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {double ndc[PG_BOXSZ], bx[PG_BOXSZ];
    PG_device *SCR_dev, *PS_dev, *CGM_dev;
    
/* connect the I/O functions */
    PG_open_console("PGS Test", "COLOR", TRUE, 0.1, 0.7, 0.5, 0.3);

    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Test");
    PG_open_device(SCR_dev, 0.1, 0.1, 0.5, 0.6);

    ndc[0] = 0.1;
    ndc[1] = 0.9;
    ndc[2] = 0.2;
    ndc[3] = 0.8;
    PG_set_viewspace(SCR_dev, 2, NORMC, ndc);
    PG_set_viewspace(SCR_dev, 2, WORLDC, NULL);

    PG_box_init(2, bx, -0.02, 1.02);
    PG_draw_box_n(SCR_dev, 2, WORLDC, bx);

    CGM_dev = PG_make_device("CGM", "MONOCHROME", "gstxts");
    PG_open_device(CGM_dev, 0.1, 0.1, 0.8, 0.8);

    PS_dev = PG_make_device("PS", "MONOCHROME", "gstxts");
    PG_open_device(PS_dev, 0.1, 0.1, 0.8, 1.1);

    PG_expose_device(PG_console_device);

    PG_clear_window(SCR_dev);
    PG_clear_window(CGM_dev);
    PG_clear_window(PS_dev);

    test_dev(SCR_dev);
    test_dev(CGM_dev);
    test_dev(PS_dev);

    PG_finish_plot(SCR_dev);
    PG_finish_plot(CGM_dev);
    PG_finish_plot(PS_dev);

    SC_pause();
#if 0
    while (TRUE)
       {char *s;

	PG_make_device_current(SCR_dev);
        PG_update_vs(SCR_dev);

        PRINT(STDOUT, "ENTER x1, y1, face, style, size: ");

        GETLN(s, MAXLINE, stdin);
        if ((token = SC_strtok(s, " ,\r\n", s)) != NULL)
           {bx[0] = ATOF(token);
            if ((token = SC_strtok(NULL, " ,\r\n", s)) != NULL)
               {bx[2] = ATOF(token);
                if ((face = SC_strtok(NULL, " ,\r\n", s)) != NULL)
                   {if ((style = SC_strtok(NULL, " ,\r\n", s)) != NULL)
                       {if ((token = SC_strtok(NULL, " ,\r\n", s)) != NULL)
                           size = ATOF(token);
                        else
                           size = 12;}
                    else
                       {style = "medium";
                        size  = 12;};}
                else
                   {face  = "times";
                    style = "medium";
                    size  = 12;};};}
        else
           break;

        PG_clear_viewport(SCR_dev);

        PG_fset_font(SCR_dev, face, style, size);

        PG_get_text_ext_n(SCR_dev, 2, WORLDC, "foo", dx);
        bx[1] = bx[0] + dx[0];
        bx[3] = bx[2] + dx[1];

/* write some marks */
/*
        r[0][0] = 0.5*(bx[0]+bx[1]);
        r[1][0] = bx[2] - 0.1*dx[1];
        PG_draw_markers_n(SCR_dev, 2, WORLDC, 1, r, 1);

        r[0][0] = bx[0] - 0.1*dx[0];
        r[1][0] = 0.5*(bx[2]+bx[3]);
        PG_draw_markers_n(SCR_dev, 2, WORLDC, 1, r, 0);
*/
/* write some text and draw a box around it */
	p[0] = bx[0];
	p[1] = bx[2];
        PG_write_n(SCR_dev, 2, WORLDC, p, "%s", "foo");
        PG_draw_box_n(SCR_dev, 2, WORLDC, bx);

        PG_update_vs(SCR_dev);};
#endif

    PG_close_device(SCR_dev);
    PG_close_device(CGM_dev);
    PG_close_device(PS_dev);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
