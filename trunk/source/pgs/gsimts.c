/*
 * GSIMTS.C - test the image drawing routines in PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define FAST

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nGSIMTS - test the image plotting in PGS\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "Usage: gsimts [-h] [-p] [-z #]\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "       -h  Print this help message and exit\n");
    PRINT(STDOUT, "       -p  palette 0 - spectrum, 1 - rainbow, 2 - bw\n");
    PRINT(STDOUT, "       -z  Use the specified number of zones on a side\n");

    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int i, k, l;
    int w, h, use_color, palette;
    double dx, dy, x, y, sf, *bf;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    char *s;
    PG_image *calc_im;
    PG_device *SCR_dev, *SCR_dew, *PS_cdev, *CGM_cdev, *PS_mdev, *CGM_mdev;
    char *palettes[] = {"spectrum", "rainbow", "bw"};

    w  = 250;
    h  = 250;
    sf = 256.0;

    palette   = 0;
    use_color = FALSE;
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'c' : use_color = TRUE;
                            break;
                 case 'p' : palette = atoi(argv[++i]);
                            break;
                 case 'h' : print_help();
                            return(1);
                 case 'z' : w = h = atoi(argv[++i]);
                            break;};}

         else
            break;};

#ifndef SLOW

    PS_cdev  = PG_make_device("PS", "COLOR", "gsimts-c");
    CGM_cdev = PG_make_device("CGM", "COLOR", "gsimts-c");
    PS_mdev  = PG_make_device("PS", "MONOCHROME", "gsimts-m");
    CGM_mdev = PG_make_device("CGM", "MONOCHROME", "gsimts-m");

    PG_open_device(PS_cdev, 0.0, 0.0, 0.0, 0.0);
    PG_open_device(CGM_cdev, 0.0, 0.0, 0.0, 0.0);
    PG_open_device(PS_mdev, 0.0, 0.0, 0.0, 0.0);
    PG_open_device(CGM_mdev, 0.0, 0.0, 0.0, 0.0);

#endif

    PG_box_init(2, dbx, 0.0, 0.0);
    PG_box_init(1, rbx, 0.0, 256.0);

    calc_im = PG_make_image_n("Test Image", "double", NULL,
			      2, WORLDC, dbx, rbx, w, h, 8, NULL);
    if (calc_im == NULL)
       {PRINT(STDOUT, "CAN'T ALLOCATE IMAGE");
        exit(1);};

    s = CSTRSAVE("WINDOW");

/* calculate the first image */
    bf = (double *) calc_im->buffer;
    for (l = 0; l < h; l++)
        {for (k = 0; k < w; k++)
             *bf++ = sf*((double) k)*((double) l)/
                           ((double) (h - 1)*(w - 1));};

/* open the first window */
    SCR_dev = PG_make_device(s, "COLOR", "PGS Image Test");

    PG_white_background(SCR_dev, FALSE);
    PG_open_device(SCR_dev, 0.05, 0.1, 0.4, 0.4);
/*    PG_open_device(SCR_dev, 0.05, 0.1, 0.01, 0.01); */

    PG_set_viewspace(SCR_dev, 2, NORMC, NULL);
    PG_set_viewspace(SCR_dev, 2, WORLDC, NULL);

    PG_fset_palette(SCR_dev, "spectrum");

/* draw the first image */
    PG_draw_image(SCR_dev, calc_im, "Test Data A", NULL);

#ifndef SLOW
    PG_fset_palette(PS_mdev, palettes[palette]);
    PG_draw_image(PS_mdev, calc_im, "Test Data HC", NULL);

    PG_fset_palette(PS_cdev, palettes[palette]);
    PG_draw_image(PS_cdev, calc_im, "Test Data HC", NULL);

    PG_white_background(CGM_mdev, TRUE);
    PG_fset_palette(CGM_mdev, palettes[palette]);
    PG_draw_image(CGM_mdev, calc_im, "Test Data HC", NULL);

    PG_white_background(CGM_cdev, TRUE);
    PG_fset_palette(CGM_cdev, palettes[palette]);
    PG_draw_image(CGM_cdev, calc_im, "Test Data HC", NULL);
#endif

    SC_pause();

/* compute the second image */
    dx = 2.0*PI/((double) w);
    dy = 2.0*PI/((double) h);
    bf = (double *) calc_im->buffer;
    for (l = 0; l < h; l++)
        {for (k = 0; k < w; k++)
             {x = ((double) k)*dx;
              y = ((double) l)*dy;
	      *bf++ = sf*(0.5 + 0.5*sin(x)*cos(y));};};

/* open the second window */
    SCR_dew = PG_make_device(s, "COLOR", "PGS Image Test");

    PG_white_background(SCR_dew, TRUE);
    PG_open_device(SCR_dew, 0.55, 0.1, 0.4, 0.4);

    PG_set_viewspace(SCR_dew, 2, NORMC, NULL);
    PG_set_viewspace(SCR_dew, 2, WORLDC, NULL);

    PG_fset_palette(SCR_dew, "cym");
    PG_clear_window(SCR_dew);

/* draw the second image */
    PG_draw_image(SCR_dew, calc_im, "Test Data B", NULL);

    SC_pause();

    PG_close_device(SCR_dev);
    PG_close_device(SCR_dew);

#ifndef SLOW
    PG_close_device(PS_cdev);
    PG_close_device(CGM_cdev);
    PG_close_device(PS_mdev);
    PG_close_device(CGM_mdev);
#endif

    PG_rl_image(calc_im);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
