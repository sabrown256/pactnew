/*
 * GSDV.C - Portable Graphics System device initialization routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

#define DEBUG_TEXT 0

#define _FGETC(stream)                                                       \
    (((PG_console_device != NULL) && (PG_console_device->ggetc != NULL)) ?   \
     (*PG_console_device->ggetc)(stream) :                                   \
     EOF)

#define _FGETS(buffer, maxlen, stream)                                       \
    (((PG_console_device != NULL) && (PG_console_device->ggets != NULL)) ?   \
     (*PG_console_device->ggets)(buffer, maxlen, stream) :                   \
     NULL)

#define _PUTS(bf)                                                            \
    if ((PG_console_device != NULL) && (PG_console_device->gputs != NULL))   \
       (*PG_console_device->gputs)(bf)


FILE
 *stdscr;              /* this is the effective file pointer for the screen */

PG_device
 *PG_console_device;
 
JMP_BUF
 io_avail;

gcontdes
 _PG_gcont;

int
 _PG_display_page(PG_device *dev, FILE *fp, char *s);

/*--------------------------------------------------------------------------*/
 
/*                          HIGH LEVEL ROUTINES                             */
 
/*--------------------------------------------------------------------------*/
 
/* PG_OPEN_DEVICE - initialize a graphics device and return a pointer to it
 *                - name - a desriptive name of the device
 *                -        WINDOW - display screen window
 *                -        PS     - PostScript
 *                -        CGM    - CGM device
 *                -        RASTER - raster device
 *                - type - a qualifier for the device
 *                -        MONOCHROME - black and white display
 *                -        COLOR      - color display
 *
 * #bind PG_open_device fortran() scheme()
 */
 
PG_device *PG_open_device(PG_device *dev, double xf, double yf,
			  double dxf, double dyf)
   {

#ifdef HAVE_WINDOW_DEVICE 
    if (dev != NULL)
       {if (dev->open_screen != NULL)
           if ((*dev->open_screen)(dev, xf, yf, dxf, dyf) == NULL)
	      return(NULL);

/* connect I/O to graphical functions */
	if (!SC_get_line_cmp(PG_wind_fgets))
	   {if (SC_get_line_cmp(io_gets))
	       SC_set_get_line(dev->ggets);};

	if (!SC_put_line_cmp(PG_wind_fprintf))
	   {if (SC_put_line_cmp(io_printf))
	       SC_set_put_line(PG_fprintf);};

	if (!SC_put_string_cmp(PG_wind_fputs))
	   {if (SC_put_string_cmp(io_puts))
	       SC_set_put_string(PG_fputs);};};

#else
    PG_device *dv;

    if (dev != NULL)
       {if (dev->open_screen != NULL)
           {if (strcmp(dev->name, "WINDOW") == 0)
	       return(NULL);
            else
	      {dv = (*dev->open_screen)(dev, xf, yf, dxf, dyf);
	       if (dv == NULL)
		  return(NULL);};};};
#endif

    return(dev);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DEVICE_FILENAME - make a good file name for a device */

void PG_device_filename(char *fname, char *raw, char *ext)
   {char *s, *pf;

    if (fname != NULL)
       {strcpy(fname, raw);

	SC_strtok(fname, " \t\n\r\\?~,;:<>/'`\"[]{}()=+|!@#$%^&*", s);

	if (ext != NULL)

/* if the file name already has the extension don't cat it on again */
	   {pf = fname + (strlen(fname) - strlen(ext));
	    if (strcmp(pf, ext) != 0)
	       strcat(fname, ext);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_DEVICE - query some physical device characteristics
 *
 * #bind PG_query_device fortran() scheme()
 */

void PG_query_device(PG_device *dev, int *pdx, int *pdy, int *pnc)
   {int dx[PG_SPACEDM];

    PG_query_screen_n(dev, dx, pnc);

    *pdx = dx[0];
    *pdy = dx[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_WINDOW - query the window shape
 *
 * #bind PG_query_window fortran() scheme()
 */

void PG_query_window(PG_device *dev, int *pdx, int *pdy)
   {PG_dev_geometry *g;

    g = &dev->g;

    *pdx = g->hwin[1] - g->hwin[0];
    *pdy = g->hwin[3] - g->hwin[2];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REGISTER_RANGE_EXTREMA - let the device know about the range extrema */

void PG_register_range_extrema(PG_device *dev, int nd, double *extr)
   {int i, no;
    double *re;

    if ((extr == NULL) || (nd < 0))
       return;

    nd *= 2;

    re = dev->range_extrema;
    no = dev->range_n_extrema;

    if ((re == NULL) || (no != nd))
       {CFREE(re);
	re = CMAKE_N(double, nd);};

    for (i = 0; i < nd; i++)
        re[i] = extr[i];

    dev->range_n_extrema = nd;
    dev->range_extrema   = re;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DEFAULT_VIEWSPACE - set the viewspace defaults for DEV */

void _PG_default_viewspace(PG_device *dev, int asp)
   {double dh;

    if (dev->view_x[0] == 0.0)
       {dev->view_x[0] = 0.175;
        dev->view_x[2] = 0.175;};

    if (dev->view_x[1] == 0.0)
       dev->view_x[1] = dev->view_x[0] + 0.65;

    if (dev->view_x[3] == 0.0)
       {dh = dev->view_x[1] - dev->view_x[0];
	if (asp == TRUE)
	   dev->view_x[3] = dev->view_x[2] + dh*dev->view_aspect;
	else
	   dev->view_x[3] = dev->view_x[2] + dh;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PUSH_DEVICE - push a device on the device list */

void _PG_push_device(PG_device *dev)
   {

    if (_PG.devlst == NULL)
       _PG.devlst = CMAKE_ARRAY(PG_device *, NULL, 0);

    SC_array_push(_PG.devlst, &dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_REMOVE_DEVICE - remove a device from the device list */

void _PG_remove_device(PG_device *dev)
   {int i, n;
    PG_device *dv;

    n = SC_array_get_n(_PG.devlst);
    for (i = 0; i < n; i++)
        {dv = *(PG_device **) SC_array_get(_PG.devlst, i);
	 if (dv == dev)
            {n = SC_array_remove(_PG.devlst, i);
	     if (n == 0)
	        {SC_free_array(_PG.devlst, NULL);
		 _PG.devlst = NULL;};
             break;};};

/* clean up the device */
    PG_rl_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_N - print formatted output
 *            - at the specified coordinates in CS
 *
 * #bind PG_write_n fortran() scheme()
 */

int PG_write_n(PG_device *dev, int nd ARG(2),
	       PG_coord_sys cs ARG(NORMC),
	       double *x ARG([0.0,0.0]),
	       char *fmt, ...)
   {int id, rv;
    int oflg[PG_SPACEDM], nflg[PG_SPACEDM];
    double y[PG_SPACEDM];
    char *s;

    if (dev == NULL)
       rv = FALSE;

    else if (dev->gprint_flag == TRUE)
       {SC_VDSNPRINTF(TRUE, s, fmt);

	if ((s != NULL) && (*s != '\0'))
	   {if (cs != WORLDC)
	       {PG_fget_axis_log_scale(dev, nd, oflg);
		for (id = 0; id < nd; id++)
		    nflg[id] = FALSE;
		PG_fset_axis_log_scale(dev, nd, nflg);};

	    PG_trans_point(dev, nd, cs, x, WORLDC, y);
	    PG_move_tx_abs_n(dev, y);
	    PG_write_text(dev, stdscr, s);

	    if (cs != WORLDC)
	       PG_fset_axis_log_scale(dev, nd, oflg);

#if DEBUG_TEXT
	    {double wc[PG_BOXSZ], dx[PG_SPACEDM];

	     PG_get_text_ext_n(dev, s, dx);
	     wc[0] = y[0];
	     wc[1] = y[0] + dx[0];
	     wc[2] = y[1];
	     wc[3] = y[1] + dx[1];
	     PG_draw_box_n(dev, 2, WORLDC, wc);};
#endif
	   };

        CFREE(s);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_DISPLAY_PAGE - write the string out to the appropriate file or
 *                  - device
 */
 
int _PG_display_page(PG_device *dev, FILE *fp, char *s)
   {char *p, *ps, *s1, *bf;

    ps = s;
    bf = NULL;
    while (TRUE)
       {bf  = SC_dstrcpy(bf, ps);
        ps += strlen(bf);
        if (strlen(bf) < 1)
           break;

        p  = bf;
        s1 = bf;
        while (p != NULL)
           {p = strchr(s1, '\n');
            if (p != NULL)
               {*p = '\0';
                p++;
                PG_write_text(dev, fp, s1);
                PG_next_line(dev, fp);}
            else
               PG_write_text(dev, fp, s1);

            if (p != NULL)
               s1 = p;};};

    CFREE(bf);

    return(0);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FPRINTF - variable arg version of fPRINTf for the library
 *            - return FALSE unless the flush option on More is invoked
 *            - see _PG_display_page for more details
 */

int PG_fprintf(FILE *fp, char *fmt, ...)
   {int rv;
    char *s;

    rv = 0;

/* the ifs are nested to get the right behavior wrt the print_flag
 * LEAVE THEM ALONE !!!!
 */
    if (fp != NULL)
       {SC_VDSNPRINTF(FALSE, s, fmt);

	if ((PG_console_device != NULL) && ((fp == stdout) || (fp == stdscr)))
	   {if (PG_console_device->gprint_flag)
	       rv = _PG_display_page(PG_console_device, fp, s);}
	else
	   {rv = io_puts(s, fp);};};

    FLUSH_ON(fp, s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FPUTS - fputs function which is safe for large strings */

int PG_fputs(char *s, FILE *fp)
   {int ret;

    ret = 0;

/* the ifs are nested to get the right behavior wrt the print_flag
 * LEAVE THEM ALONE !!!!
 */
    if (fp != NULL)
       {if ((PG_console_device != NULL) && ((fp == stdout) || (fp == stdscr)))
	   {if (PG_console_device->gprint_flag)
	       ret = _PG_display_page(PG_console_device, fp, s);}
	else
	   {ret = io_puts(s, fp);};};

    FLUSH_ON(fp, s);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WIND_FGETC - window version of fgetc
 *               - event handling loop driver
 */

int PG_wind_fgetc(FILE *stream)
   {int c;

    if (stream == stdin)
       c = _FGETC(stream);
    else
       c = io_getc(stream);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WIND_FGETS - window version of fgets
 *               - event handling loop driver
 */

char *PG_wind_fgets(char *str, int maxlen, FILE *stream)
   {char *p;

    if (stream == stdin)
       p = _FGETS(str, maxlen, stream);
    else
       p = io_gets(str, maxlen, stream);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_INPUT - reset the input state */

void PG_setup_input(char *s, int nc, size_t nr)
   {

    _PG_gcont.input_bf  = s;
    _PG_gcont.input_nr  = nr;
    _PG_gcont.input_nc  = 0;
    _PG_gcont.input_ncx = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RESET_INPUT - reset the input state */

void PG_reset_input(void)
   {

    _PG_gcont.input_bf[0] = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WIND_FPRINTF - handle printing in the window environment */

int PG_wind_fprintf(FILE *fp, char *fmt, ...)
   {int rv;
    char *bf;
    
    rv = 0;

    if (fp != NULL)
       {if ((fp == stdout) && (PG_console_device != NULL))
	   {if (PG_console_device->gprint_flag)
               {SC_VDSNPRINTF(FALSE, bf, fmt);

		rv = strlen(bf);

		_PUTS(bf);};}

       else
	  {SC_VA_START(fmt);
	   rv = SC_VFPRINTF(fp, fmt);
	   SC_VA_END;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* PG_WIND_FPUTS - handle printing in the window environment */

int PG_wind_fputs(char *s, FILE *fp)
   {int ret;
    
    ret = 0;

    if (fp != NULL)
       {if ((fp == stdout) && (PG_console_device != NULL))
	   {if (PG_console_device->gprint_flag)
	       ret = PG_fputs(s, fp);}

        else
	   ret = io_puts(s, fp);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_OPEN_CONSOLE - open a console window */

int PG_open_console(char *title, char *type, int bckgr,
		    double xf, double yf, double dxf, double dyf)
   {int rv;

    rv = FALSE;

    PG_setup_ctrls_glb(NULL);

    if (_PG_gcont.open_console != NULL)
       rv = (*_PG_gcont.open_console)(title, type, bckgr, xf, yf, dxf, dyf);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_WINDOW_DEVICE - setup a PG_device for a screen window */

int PG_setup_window_device(PG_device *dev)
   {int rv;

    rv = FALSE;

    PG_setup_ctrls_glb(NULL);

    if (_PG_gcont.setup_window != NULL)
       rv = (*_PG_gcont.setup_window)(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_CTRLS_GLB - setup global controls for window devices */

void PG_setup_ctrls_glb(char *s)
   {

    if (s == NULL)
       s = _PG_gcont.default_window_device;

    _PG_gcont.get_event    = NULL;
    _PG_gcont.event_device = NULL;
    _PG_gcont.open_console = NULL;
    _PG_gcont.setup_window = NULL;

#if defined(HAVE_QD)
    if (strcmp(s, "QD") == 0)
       PG_qd_setup_ctrls_glb();

#elif defined(WIN32)
    if (strcmp(s, "WIN32") == 0)
       PG_win32_setup_ctrls_glb();
#endif

#if defined(HAVE_OGL)
    if (strcmp(s, "GL") == 0)
       PG_GL_setup_ctrls_glb();
#endif

#if defined(HAVE_X11)
    if (strcmp(s, "X11") == 0)
       PG_X_setup_ctrls_glb();
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FOPEN - open file with attention to buffering issues */

FILE *_PG_fopen(char *name, char *mode)
   {FILE *fp;

    fp = io_open(name, mode);

    if (_PG.buffer_size > 0)
       io_setvbuf(fp, NULL, 0, _PG.buffer_size);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
