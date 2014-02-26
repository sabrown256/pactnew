/*
 * QUEV.C - standalone utility to query mouse events
 *        - for use in shell scripting
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

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
/*
        if (strcmp(s, "end\n") == 0)
           end_prog(dev, NULL);
*/
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
   {int i, rv, mod;
    int iev[PG_SPACEDM];
    PG_event ev;
    PG_mouse_button btn;
    PG_device *dev;

/* process the command line arguments */
    SC_set_io_interrupts(TRUE);
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'i' :
                      SC_set_io_interrupts(FALSE);
		      break;};};};

    SC_setbuf(STDOUT, NULL);

    PG_setup_ctrls_glb("X11");

/* connect the I/O functions */
    PG_open_console("PGS Test", "COLOR", TRUE, 0.1, 0.7, 0.5, 0.3);

    dev = PG_gs.console;

    for (btn = MOUSE_NONE; btn == MOUSE_NONE; )
        {PG_get_next_event(&ev);
	 PG_mouse_event_info(dev, &ev, iev, &btn, &mod);};

    while (TRUE)
       rv = PG_get_next_event(&ev);

#if 0
    for (bf[0] = '\0'; TRUE; )
        {
/*
	 PG_key_event_info(dev, &ev, iev, bf, 80, &mod);
	 if ((bf[0] == '\0') && (mod != 0))
	    PG_get_next_event(ev);
	 else
	    break;
 */
	 PG_mouse_event_info(dev, &ev, &iev, &eb, &eq);};

    bf[1] = '\0';
#endif
/*       get_input(dev, &ev); */

    rv = 0;

    exit(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
