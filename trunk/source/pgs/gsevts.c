/*
 * GSEVTS.C - test of event handling graphics library
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_TO - register a TEXT interface object with the device */

void register_to(PG_device *dev, double *bx, char *s)
   {PG_interface_object *iob;
    PG_curve *crv;
    PG_text_box *b;
    double xo[PG_SPACEDM];
    int flags[3];

    flags[0] = TRUE;
    flags[1] = FALSE;
    flags[2] = FALSE;

    xo[0] = bx[0];
    xo[1] = bx[2];
    crv   = PG_make_box_curve(dev, NORMC, xo, bx);

    iob = PG_make_interface_object(dev, "TEXT", PG_TEXT_OBJECT_S, s, CENTER,
				   0.0, crv, flags, dev->WHITE, dev->CYAN,
                                   NULL, "draw-text", "return-text",
                                   NULL);

    PG_register_interface_object(dev, iob);

    b = (PG_text_box *) iob->obj;
    b->border = 0.2;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RETURN_TEXT - TEXT interface object action */

void return_text(PG_interface_object *iob, PG_event *ev)
   {

/*    PRINT(STDOUT, "Detected %s event\n", X_event_name[ev->type]); */

    PRINT(STDOUT, "\nClicked on TEXT object: %s\n", (char *) iob->obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RETURN_CHILD_TEXT - TEXT interface object action */

void return_child_text(PG_interface_object *iob, PG_event *ev)
   {int n;
    PG_interface_object *ch;

    n = SC_array_get_n(iob->children);
    if (n >= 0)
       {ch = IOB(iob->children, 0);
        PRINT(STDOUT,
              "\nClicked on BUTTON object: %s\n",
              (char *) ch->obj);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAR_WINDOW - clear the screen */

void clear_window(void *d, PG_event *ev)
   {double bx[PG_BOXSZ];
    PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       {PG_clear_window(dev);
        PG_fset_line_color(dev, dev->WHITE, TRUE);
	PG_box_init(2, bx, 0.0, 1.0);
        PG_draw_box_n(dev, 2, WORLDC, bx);
        PG_draw_interface_objects(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REDRAW - redraw the interface objects */

void redraw(void *d, PG_event *ev)
   {PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       PG_draw_interface_objects(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* END_PROG - end the program */

void end_prog(void *d, PG_event *ev)
   {PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       {PG_write_interface(dev, "gsevts.pui");
        PG_close_device(dev);
        printf("\n");

        exit(0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_INPUT - get input */

void get_input(void *d, PG_event *ev)
   {int size;
    double x, y;
    double bx[PG_BOXSZ], p[PG_SPACEDM], dx[PG_SPACEDM];
    char s[MAXLINE];
    char *style, *token, *t, *face;
    PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       {if (ev != NULL)
           PRINT(STDOUT, "\nENTER x1, y1, face, style, size: ");

        s[0] = '\0';
        GETLN(s, MAXLINE, stdin);
        if (strcmp(s, "end\n") == 0)
           end_prog(dev, NULL);

	x     = 0;
	y     = 0;
	bx[0] = 0;
	bx[2] = 0;
	size  = 10;
	style = "bold";
        face  = NULL;
        if ((token = SC_strtok(s, " ,\r\n", t)) != NULL)
           {bx[0] = ATOF(token);
            if ((token = SC_strtok(NULL, " ,\r\n", t)) != NULL)
               {bx[2] = ATOF(token);
                if ((face = SC_strtok(NULL, " ,\r\n", t)) != NULL)
                   {if ((style = SC_strtok(NULL, " ,\r\n", t)) != NULL)
                       {if ((token = SC_strtok(NULL, " ,\r\n", t)) != NULL)
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
	   return;

	PG_fset_font(dev, face, style, size);

	PG_get_text_ext_n(dev, 2, WORLDC, "foo", dx);
	bx[1] = bx[0] + dx[0];
	bx[3] = bx[2] + dx[1];

/* write some text and draw a box around it */
	p[0] = bx[0];
	p[1] = bx[2];
	PG_write_n(dev, 2, WORLDC, p, "%s", "foo");
	PG_draw_box_n(dev, 2, WORLDC, bx);

	PG_update_vs(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_EDIT_MODE - turn on editing of interface objects */

void set_edit_mode(void *d, PG_event *ev)
   {PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       _PG_gattrs.edit_mode = TRUE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNSPECIFIED_EVENT_HANDLER - handle random events */

void unspecified_event_handler(PG_device *dev, PG_event *ev)
   {

/*    PRINT(STDOUT, "Detected %s event\n", X_event_name[ev->type]); */

    PG_print_pointer_location(dev, 0.0, 0.98, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MOUSE_EVENT_HANDLER - handle mouse down events */

void mouse_event_handler(PG_device *dev, PG_event *ev)
   {int mod;
    int iev[PG_SPACEDM];
    double bx[PG_BOXSZ], p[PG_SPACEDM], dx[PG_SPACEDM];
    char bf[80];
    PG_mouse_button btn;
    PG_interface_object *iob;

    iob = PG_get_object_event(dev, ev);
    if (iob != NULL)
       {PRINT(STDOUT, "\n%s\n", (char *) iob->obj);}

    else if (dev != NULL)
       {PG_mouse_event_info(dev, ev, iev, &btn, &mod);

	p[0] = iev[0];
	p[1] = iev[1];
	PG_trans_point(dev, 2, NORMC, p, WORLDC, p);
	bx[0] = p[0];
	bx[2] = p[1];

        bf[0] = '\0';

        if (mod & KEY_SHIFT)
           SC_strcat(bf, 80, "Shift ");

        if (mod & KEY_CNTL)
           SC_strcat(bf, 80, "Control ");

        if (mod & KEY_ALT)
           SC_strcat(bf, 80, "Alt ");

        switch (btn)
           {case MOUSE_LEFT   :
                 SC_strcat(bf, 80, "Left");
                 break;
            case MOUSE_MIDDLE :
                 SC_strcat(bf, 80, "Middle");
                 break;
            case MOUSE_RIGHT  :
                 SC_strcat(bf, 80, "Right");
                 break;
            case MOUSE_WHEEL_UP :
                 SC_strcat(bf, 80, "Wheel Up");
                 break;
            case MOUSE_WHEEL_DOWN :
                 SC_strcat(bf, 80, "Wheel Down");
                 break;
            default :
                 SC_strcat(bf, 80, "Unknown");
                 break;};

        if (bf[0] != '\0')
           {PG_get_text_ext_n(dev, 2, WORLDC, bf, dx);
            bx[1] = bx[0] + dx[0];
            bx[3] = bx[2] + dx[1];

/* write some text and draw a box around it */
	    p[0] = bx[0];
	    p[1] = bx[2];
            PG_write_n(dev, 2, WORLDC, p, "%s", bf);
            PG_draw_box_n(dev, 2, WORLDC, bx);

            PG_update_vs(dev);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
   {int i, ps_flag;
    double phimn, phimx, thetamn, thetamx;
    double phi, theta;
    double ndc[PG_BOXSZ], bx[PG_BOXSZ];
    char *direction;
    PG_device *SCR_dev;

    ps_flag   = TRUE;
    direction = NULL;
    phi       = 30.0;
    theta     = 30.0;

/* process the command line arguments */
    PG_IO_INTERRUPTS(TRUE);
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'i' :
                      PG_IO_INTERRUPTS(FALSE);
		      break;};};};

    SC_setbuf(STDOUT, NULL);

/* connect the I/O functions */
    PG_open_console("PGS Test", "COLOR", TRUE, 0.1, 0.7, 0.5, 0.3);

    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Test");
    PG_fset_pixmap_flag(SCR_dev, FALSE);

    PG_open_device(SCR_dev, 0.1, 0.1, 0.5, 0.6);

    ndc[0] = 0.05;
    ndc[1] = 0.45;
    ndc[2] = 0.2;
    ndc[3] = 0.8;
    PG_set_viewspace(SCR_dev, 2, NORMC, ndc);

    PG_set_viewspace(SCR_dev, 2, WORLDC, NULL);
    PG_box_init(2, bx, -0.02, 1.02);
    PG_draw_box_n(SCR_dev, 2, WORLDC, bx);

    PG_set_default_event_handler(SCR_dev, unspecified_event_handler);
    PG_set_mouse_down_event_handler(SCR_dev, mouse_event_handler);

    PG_register_variable("PS-Flag", SC_INT_S, &ps_flag, NULL, NULL);
    PG_register_variable("Direction", SC_STRING_S, &direction, NULL, NULL);
    phimn = -180.0;
    phimx =  180.0;
    PG_register_variable("Phi", SC_DOUBLE_S, &phi, &phimn, &phimx);
    thetamn = 0.0;
    thetamx = 90.0;
    PG_register_variable("Theta", SC_DOUBLE_S, &theta, &thetamn, &thetamx);

    if (SC_isfile("gsevts.pui"))
       {PG_register_callback("End", end_prog);
        PG_register_callback("Clear", clear_window);
        PG_register_callback("ReDraw", redraw);
        PG_register_callback("Input", get_input);
        PG_register_callback("Edit", set_edit_mode);
	PG_read_interface(SCR_dev, "gsevts.pui");}
    else
       {bx[0] = 0.55;
        bx[1] = 0.90;
	bx[2] = 0.2;
	bx[3] = 0.8;
	register_to(SCR_dev, bx, "Hello World!");

	bx[0] = 0.10;
	bx[1] = 0.20;
	bx[2] = 0.90;
	bx[3] = 0.95;
        PG_add_button(SCR_dev, bx, "End", end_prog);

	bx[0] = 0.20;
	bx[1] = 0.30;
	bx[2] = 0.90;
	bx[3] = 0.95;
        PG_add_button(SCR_dev, bx, "Clear", clear_window);

	bx[0] = 0.30;
	bx[1] = 0.40;
	bx[2] = 0.90;
	bx[3] = 0.95;
        PG_add_button(SCR_dev, bx, "ReDraw", redraw);

	bx[0] = 0.40;
	bx[1] = 0.50;
	bx[2] = 0.90;
	bx[3] = 0.95;
        PG_add_button(SCR_dev, bx, "Input", get_input);

	bx[0] = 0.50;
	bx[1] = 0.60;
	bx[2] = 0.90;
	bx[3] = 0.95;
        PG_add_button(SCR_dev, bx, "Edit", set_edit_mode);};

    PG_expose_device(PG_console_device);
    clear_window(SCR_dev, NULL);

    PG_update_vs(SCR_dev);

    PG_make_device_current(SCR_dev);
    while (TRUE)
       get_input(SCR_dev, NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
