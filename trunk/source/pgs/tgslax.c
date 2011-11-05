/*
 * TGSLAX.C - test of log axis scales
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TICK_INFO - print tick description */

static void print_tick_info(char *t, int it, PG_axis_def *ad)
   {int i, n;
    double *dx;
    PG_axis_tick_def *td;

    td = ad->tick + (it >> 1);

    if (it == AXIS_TICK_MINOR)
       td->en[0] = max(SMALL, td->en[0]);

    PRINT(STDOUT,
          "%s  %2d %10.2e %10.2e %10.2e",
          t, td->n, td->en[0], td->en[1], td->space);

    if (it == AXIS_TICK_LABEL)
       PRINT(STDOUT,
	     " %10.2e %10.2e   ", ad->vo[0], ad->vo[1]);
    else
       {PRINT(STDOUT,
	      " %10.2e %10.2e   ", td->vo[0], td->vo[1]);

	n  = td->n;
	dx = td->dx;
	n  = min(n, 3);
	for (i = 0; i < n; i++)
	    PRINT(STDOUT, " %.2g", dx[i]);};

    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_AXIS_DESCRIPTION - print the axis description */

static void print_axis_description(PG_axis_def *ad,
				   double xmin, double ymin,
				   double xmax, double ymax,
				   double vmin, double vmax)
   {static int count = 0;

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "----------------------------------------------------------\n");

    PRINT(STDOUT, "Axis #%d\n", ++count);

    PRINT(STDOUT,
          "Axis description for (%.2e, %.2e) to (%.2e, %.2e):\n",
          xmin, ymin, xmax, ymax);

    PRINT(STDOUT,
          "Label range %.2e to %.2e:\n",
          vmin, vmax);

    PRINT(STDOUT,
          "Origin: (%.2e, %.2e)\n", ad->x0[0], ad->x0[1]);
    PRINT(STDOUT,
          "Axis length: %.2e\n", ad->dr);
    PRINT(STDOUT,
          "Orientation: (%f, %f)\n", ad->cosa, ad->sina);
    PRINT(STDOUT,
          "Scales: %.2e and (%.2e, %.2e)\n", ad->scale, ad->scale_x[0], ad->scale_x[1]);

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "Type    #     Xstart       Xend         dX");
    PRINT(STDOUT, "         Va         Vb");
    PRINT(STDOUT, "   Fractions\n");

    print_tick_info("Major", AXIS_TICK_MAJOR, ad);

    print_tick_info("Minor", AXIS_TICK_MINOR, ad);

    print_tick_info("Label", AXIS_TICK_LABEL, ad);

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "----------------------------------------------------------\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_DIR - show the direction that the axis vector points */

static void show_dir(PG_device *dev, int k)
   {int id;
    int nflg[PG_SPACEDM];
    double va, dv;
    double vbx[PG_BOXSZ];
    double *x[PG_SPACEDM], *u[PG_SPACEDM];

    for (id = 0; id < 2; id++)
        nflg[id] = FALSE;

    PG_fset_axis_log_scale(dev, 2, nflg);

    PG_set_viewspace(dev, 2, WORLDC, NULL);

    dv = 0.2;
    va = 0.5;
	
    switch (k)
       {case 1 :
	     vbx[2] = 0.65;
	     vbx[3] = 0.0;
	     vbx[0] = va - dv;
	     vbx[1] = 2.0*dv;
	     break;
        case 2 :
	     vbx[0] = 0.35;
	     vbx[1] = 0.0;
	     vbx[2] = va - dv;
	     vbx[3] = 2.0*dv;
	     break;
	case 3 :
	     vbx[2] = 0.35;
	     vbx[3] = 0.0;
	     vbx[0] = va + dv;
	     vbx[1] = -2.0*dv;
	     break;
        case 4 :
	     vbx[0] = 0.65;
	     vbx[1] = 0.0;
	     vbx[2] = va + dv;
	     vbx[3] = -2.0*dv;
	     break;};

    x[0] = &vbx[0];
    x[1] = &vbx[2];
    u[0] = &vbx[1];
    u[1] = &vbx[3];

    PG_draw_vector_n(dev, 2, WORLDC, 1, x, u);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_AXIS - test the computation of the axis description parameters */

static void test_axis(PG_device *dev, int ticks, int rev,
		      double t1, double t2,
		      double xmn, double ymn, double xmx, double ymx,
		      double amn, double amx)
   {int lr;
    double tn[2], vw[2];
    double bnd[PG_BOXSZ];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    PG_axis_def *ad;
    extern void _PG_rl_axis_def(PG_axis_def *ad);

    if (dev != NULL)
       {PG_get_viewspace(dev, BOUNDC, bnd);

	lr = (bnd[2] < bnd[3]) ? AXIS_TICK_RIGHT : AXIS_TICK_LEFT;

	xl[0] = xmn;
	xl[1] = ymn;
	xr[0] = xmx;
	xr[1] = ymx;
	tn[0] = t1;
	tn[1] = t2;
	vw[0] = amn;
	vw[1] = amx;
	ad = PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0, "%10.3e",
			    AXIS_TICK_STRADDLE, lr, TRUE,
			    ticks, 0);
/*
       _PG_axis_tick_test(xl, xr, amn, amx);
 */
	if (ad != NULL)
	   {print_axis_description(ad, xmn, ymn, xmx, ymx, amn, amx);

	    _PG_rl_axis_def(ad);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AXIS_CORE - core test routine */

static void axis_core(PG_device *dev, int k, int l, FILE *fp,
		      double t1i, double t2i,
		      int lax, int ticks, int pause)
   {int i, j, id;
    int nflg[PG_SPACEDM];
    char s[MAXLINE], *token, *t;
    double sdx, sdy;
    double ndc[PG_BOXSZ], wc[PG_BOXSZ];
    double axmn, axmx, aymn, aymx, amin, amax;
    double va, vb, t1, t2;

    sdx = 0.125;
    sdy = 0.125;

    PG_clear_window(dev);
    switch (l)
       {case 0:           /* scale increasing outward */
	     PG_center_label(dev, 0.95, "t1 < t2  (outward)");
	     break;
	case 1:           /* scale increasing inward */
	     PG_center_label(dev, 0.95, "t2 < t1  (inward)");
	     break;};

    for (i = 0, j = 0; TRUE; i++)
        {if (fp == stdin)
	    PRINT(STDOUT, "End Points (#%2d): ", j+1);

	 if (GETLN(s, MAXLINE, fp) == NULL)
	    break;

	 if (s[0] == '#')
	    continue;

	 if (strcmp(s, "end\n") == 0)
	    break;

	 token = SC_strtok(s, " ,\n", t);
	 if (token == NULL)
	    continue;

	 amin  = SC_stof(token);
	 token = SC_strtok(NULL, " ,\n", t);
	 amax  = SC_stof(token);

	 token = SC_strtok(NULL, " ,\n", t);
	 if (token != NULL)
	    {t1    = SC_stof(token);
	     token = SC_strtok(NULL, " ,\n", t);
	     t2    = SC_stof(token);}
	 else
	    {t1 = t1i;
	     t2 = t2i;};

/* set the viewport */
	 ndc[0] = 0.1 + j*sdx;
	 ndc[1] = ndc[0] + sdx;
	 ndc[2] = 0.1 + j*sdy;
	 ndc[3] = ndc[2] + sdy;
	 switch (k)
	    {case 1:           /* increasing to the right */
	     case 3:           /* increasing to the left */
	          ndc[0] = 0.1;
		  ndc[1] = 0.9;
		  break;
	     case 2:           /* increasing upward */
	     case 4:           /* increasing downward */
		  ndc[2] = 0.1;
		  ndc[3] = 0.9;
		  break;};

	 PG_set_viewspace(dev, 2, NORMC, ndc);

         show_dir(dev, k);

/* set the world coordinates */
	 for (id = 0; id < 2; id++)
	     nflg[id] = FALSE;

	 switch (k)
	    {case 1:           /* increasing to the right */
	     case 3:           /* increasing to the left */
	          wc[0] = amin;
		  wc[1] = amax;
		  wc[2] = 0.0;
		  wc[3] = 1.0;
		  nflg[0] = lax;
		  break;
	     case 2:           /* increasing upward */
	     case 4:           /* increasing downward */
		  wc[0] = 0.0;
		  wc[1] = 1.0;
		  wc[2] = amin;
		  wc[3] = amax;
		  nflg[1] = lax;
		  break;};

	 PG_fset_axis_log_scale(dev, 2, nflg);

	 PG_set_viewspace(dev, 2, WORLDC, wc);

/* set the axis endpoints and limits */
	 axmn = 0.5;
	 axmx = 0.5;
	 aymn = 0.5;
	 aymx = 0.5;
	 switch (k)
	    {case 1:           /* increasing to the right */
	          axmn = amin;
		  axmx = amax;
		  break;
	     case 2:           /* increasing upward */
		  aymn = amin;
		  aymx = amax;
		  break;
	     case 3:           /* increasing to the left */
		  axmn = amax;
		  axmx = amin;
		  break;
	     case 4:           /* increasing downward */
		  aymn = amax;
		  aymx = amin;
		  break;};

	 switch (l)
	    {case 0:           /* scale increasing outward */
	          va = amin;
		  vb = amax;
		  break;
	     case 1:           /* scale increasing inward */
		  va = amax;
		  vb = amin;
		  break;};

	 test_axis(dev, ticks, l, t1, t2, axmn, aymn, axmx, aymx, va, vb);

	 j++;};

    PG_finish_plot(dev);
    if (pause == TRUE)
       {io_printf(STDOUT, "Hit enter to continue: ");
	io_gets(s, MAXLINE, stdin);};

    io_seek(fp, 0, SEEK_SET);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print the help message */

void help(void)
   {

    PRINT(STDOUT, "Usage: tgslax [-d] [-h] [-i <t1> <t2>] [-l] [-p] [-s] [-t #] [-v #] <file>\n");
    PRINT(STDOUT, "   d    debug memory usage\n");
    PRINT(STDOUT, "   h    this help message\n");
    PRINT(STDOUT, "   i    define interval (t1, t2) - defaults to (0, 1)\n");
    PRINT(STDOUT, "   l    draw linear axes\n");
    PRINT(STDOUT, "   p    stop to see plot and prompt\n");
    PRINT(STDOUT, "   s    PS output\n");
    PRINT(STDOUT, "   t    which ticks: 1 minor, 2 major, 4 label\n");
    PRINT(STDOUT, "   v    direction: 1 left, 2 up, 3 right, 4 down (default all)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, k, l, hc, debug_mode, lax, ret, rv;
    int pause, ticks, dir, dd;
    double t1, t2;
    char s[MAXLINE], f[MAXLINE], *t, *p;
    PG_device *tdev;
    FILE *fp;

    SC_zero_space_n(0, -2);
    debug_mode = FALSE;

    SC_setbuf(STDOUT, NULL);

    PRINT(STDOUT, "\n");

    ret = 0;

/* process the command line arguments */
    t1    = 0.0;
    t2    = 1.0;
    dir   = 1;
    dd    = 1;
    ticks = 7;
    pause = FALSE;
    lax   = TRUE;
    hc    = FALSE;
    fp    = stdin;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
	        {case 'd' :
		      debug_mode = TRUE;
		      break;
                 case 'h' :
		      help();
                      return(1);
		      break;
                 case 'i' :
		      t1 = SC_stoi(v[++i]);
		      t2 = SC_stoi(v[++i]);
		      break;
                 case 'l' :
		      lax = FALSE;
		      break;
                 case 'p' :
		      pause = TRUE;
		      break;
                 case 's' :
		      hc = TRUE;
		      break;
                 case 't' :
		      ticks = SC_stoi(v[++i]);
		      break;
                 case 'v' :
		      t = v[++i];
                      if (strchr(t, ',') == NULL)
			 {dir = SC_stoi(t);
			  dd  = 4;}
		      else
			 {dir = SC_stoi(SC_strtok(t, ",", p));
			  dd  = SC_stoi(SC_strtok(NULL, ",", p));};
		      break;
		 default :
		      break;};}
         else
            {strcpy(f, v[i]);
	     fp = io_open(f, "r");
             break;};};

    if (debug_mode)
       rv = SC_mem_monitor(-1, 2, "A", s);

    if (hc)
       {t    = strrchr(f, '.');
        *t   = '\0';
	t    = strrchr(f, '/');
        if (t == NULL)
	   t = f;
	else
	   t++;
        tdev = PG_make_device("PS", "MONOCHROME", t);}
    else
       {PG_open_console("TGSLAX", "MONOCHROME", 1, 0.55, 0.1, 0.4, 0.8);

        tdev = PG_make_device("WINDOW", "MONOCHROME", "PGS Axis Test");};

    PG_white_background(tdev, TRUE);
    
    PG_open_device(tdev, 0.01, 0.01, 0.99, 0.8);

    PG_set_attrs_glb(TRUE,
		     "axis-major-tick-size", 0.075,
		     NULL);

    for (l = 0; l < 2; l++)
        for (k = dir; k < 5; k += dd)
            axis_core(tdev, k, l, fp, t1, t2, lax, ticks, pause);

    PRINT(STDOUT, "\n");

    PG_close_device(tdev);
    PG_close_console();

    if (fp != stdin)
       io_close(fp);

    PG_rl_all();

    if (debug_mode)
       ret = SC_mem_monitor(rv, 2, "A", s);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
