/*
 * GSIOB.C - interface object handling routines for PGS
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define SET_VAL(t, fnc)                                                     \
    {t **p;                                                                 \
     p     = (t **) (hp)->def;                                              \
     *p[0] = fnc(val);}

#define RESET_VAL(t, hp)                                                    \
    {t **p;                                                                 \
     p     = (t **) (hp)->def;                                              \
     *p[0] = (1.0 - f)*(*p[1]) + f*(*p[2]);}

#define GET_VAL(t, hp, xo, dx)                                              \
    {t **p;                                                                 \
     double f, p1, p2, p3;                                                  \
     p   = (t **) (hp)->def;                                                \
     p1  = *p[0];                                                           \
     p2  = *p[1];                                                           \
     p3  = *p[2];                                                           \
     f   = (p1 - p2)/(p3 - p2);                                             \
     xo += f*dx;}

char
 *PG_TEXT_OBJECT_S = "Text",
 *PG_VARIABLE_OBJECT_S = "Variable",
 *PG_BUTTON_OBJECT_S = "Button",
 *PG_CONTAINER_OBJECT_S = "Container";

SC_THREAD_LOCK(PG_iob_lock);

extern PG_interface_object
 *PG_find_object(PG_device *dev, char *s,
		 PG_interface_object *parent);

static void
 _PG_setup_callback_table(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DEFINE_REGION - graphically define a region
 *                  - COORD defines the meaning of the return values
 */

void PG_define_region(PG_device *dev, PG_coord_sys cs, double *bx)
   {int lnc, lns, btn, mod, first;
    int ir[PG_SPACEDM];
    double wc[PG_BOXSZ], pc[PG_BOXSZ];
    double xa[PG_SPACEDM], xb[PG_SPACEDM];
    PG_logical_operation lop;

    if (dev == NULL)
       return;

    PG_get_line_color(dev, &lnc);
    PG_get_line_style(dev, &lns);
    PG_get_logical_op(dev, &lop);

    if (dev->xor_parity)
       {PG_set_color_line(dev, dev->WHITE, FALSE);}
    else
       PG_set_color_line(dev, dev->BLACK, FALSE);

    PG_set_line_style(dev, LINE_DOTTED);
    PG_set_logical_op(dev, GS_XOR);

/* wait for the left button to be pressed */
    for (btn = FALSE; btn != MOUSE_LEFT; )
        PG_query_pointer(dev, ir, &btn, &mod);

    xa[0] = ir[0];
    xa[1] = ir[1];

    PG_trans_point(dev, 2, PIXELC, xa, WORLDC, xa);
    wc[0] = xa[0];
    wc[2] = xa[1];
    wc[1] = wc[0];
    wc[3] = wc[2];
    first = TRUE;
    while (btn == MOUSE_LEFT)
       {PG_query_pointer(dev, ir, &btn, &mod);
	xb[0] = ir[0];
	xb[1] = ir[1];

	if (first)
	   first = FALSE;
	else
	   PG_draw_box_n(dev, 2, WORLDC, wc);

	PG_trans_point(dev, 2, PIXELC, xb, WORLDC, xb);
	wc[1] = xb[0];
	wc[3] = xb[1];

/* draw the new one */
	PG_draw_box_n(dev, 2, WORLDC, wc);};

    PG_set_line_color(dev, lnc);
    PG_set_line_style(dev, lns);
    PG_set_logical_op(dev, lop);

    pc[0] = min(xa[0], xb[0]);
    pc[1] = max(xa[0], xb[0]);
    pc[2] = 0;
    pc[3] = 0;
    pc[2] = min(xa[1], xb[1]);
    pc[3] = max(xa[1], xb[1]);

    PG_trans_box(dev, 2, WORLDC, pc, cs, bx);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PRINT_POINTER_LOCATION - print the location of the mouse
 *                           - if COORD is TRUE print in WC else
 *                           - print in NDC
 */

void PG_print_pointer_location(PG_device *dev, double cx, double cy, int coord)
   {int btn, mod;
    int ir[PG_SPACEDM];
    double ndc[PG_BOXSZ], c[PG_SPACEDM], dx[PG_SPACEDM];
    char bf[MAXLINE];
    static int oir[PG_SPACEDM] = {0, 0, 0};

    if (dev == NULL)
       return;

    PG_query_pointer(dev, ir, &btn, &mod);
    if ((oir[0] != ir[0]) || (oir[1] != ir[1]))
       {SC_LOCKON(PG_iob_lock);

	c[0] = cx;
	c[1] = cy;

	oir[0] = ir[0];
        oir[1] = ir[1];

        if (coord)
           snprintf(bf, MAXLINE, "x = -0.000e000, y = -0.000e000");
        else
           snprintf(bf, MAXLINE, "x = 0000, y = 0000");
        PG_get_text_ext_n(dev, 2, NORMC, bf, dx);

	ndc[0] = c[0];
	ndc[1] = c[0] + dx[0];
	ndc[2] = c[1];
	ndc[3] = c[1] + dx[1];
        PG_clear_region(dev, 2, NORMC, ndc, 2);

        if (coord == TRUE)
           {dx[0] = ir[0];
	    dx[1] = ir[1];
	    PG_trans_point(dev, 2, PIXELC, dx, WORLDC, dx);
            PG_set_text_color(dev, dev->WHITE);
            PG_write_n(dev, 2, NORMC, c, "x = %10.3e, y = %10.3e",
		       dx[0], dx[1]);}
        else
           PG_write_n(dev, 2, NORMC, c, "x = %d, y = %d", ir[0], ir[1]);

        PG_update_vs(dev);

	SC_LOCKOFF(PG_iob_lock);};

    return;}

/*--------------------------------------------------------------------------*/

/*                        CALLBACK MANAGEMENT ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* _PG_GET_VAL - get the value of the haelem def given
 *             - only numeric types are applicable here
 */

static int _PG_get_val(haelem *hp, int xo, int dx)
   {char *type;

    type = hp->type;
    if (strcmp(type, SC_INTEGER_S) == 0)
       {GET_VAL(int, hp, xo, dx);}

    else if (strcmp(type, SC_LONG_S) == 0)
       {GET_VAL(long, hp, xo, dx);}

    else if (strcmp(type, SC_DOUBLE_S) == 0)
       {GET_VAL(double, hp, xo, dx);}

    else if (strcmp(type, SC_FLOAT_S) == 0)
       {GET_VAL(float, hp, xo, dx);};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SET_VAL - set the value of the haelem def given
 *             - only numeric types are applicable here
 */

static void _PG_set_val(haelem *hp, int dxn, int dxd)
   {char *type;
    double f;

    f = ((double) dxn) / ((double) dxd);

    type = hp->type;
    if (strcmp(type, SC_INTEGER_S) == 0)
       {RESET_VAL(int, hp);}

    else if (strcmp(type, SC_LONG_S) == 0)
       {RESET_VAL(long, hp);}

    else if (strcmp(type, SC_DOUBLE_S) == 0)
       {RESET_VAL(double, hp);}

    else if (strcmp(type, SC_FLOAT_S) == 0)
       {RESET_VAL(float, hp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_VALUE_MATCH - return TRUE iff the given text string value
 *                 - matches the value in the specified haelem
 */

static int _PG_value_match(int ityp, haelem *hp, char *val)
   {int match;
    char *ps;

    match = FALSE;
    if (ityp == SC_INTEGER_I)
       match = (**(int **) hp->def == SC_stoi(val));

    else if (ityp == SC_LONG_I)
       match = (**(long **) hp->def == SC_stoi(val));

    else if (ityp == SC_DOUBLE_I)
       match = (**(double **) hp->def == SC_stof(val));

    else if (ityp == SC_FLOAT_I)
       match = (**(float **) hp->def == SC_stof(val));

    else if (ityp == SC_STRING_I)
       {ps = **(char ***) hp->def;
	match = (ps == NULL) ? FALSE : (strcmp(ps, val) == 0);}

    else
       match = FALSE;

    return(match);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_VALUE_STRING - return an ASCII representation of the given value */

static int _PG_value_string(int ityp, haelem *hp, char *s)
   {int match;

    match = TRUE;
    if (ityp == SC_INTEGER_I)
       sprintf(s, "%d", **(int **) hp->def);

    else if (ityp == SC_LONG_I)
       sprintf(s, "%ld", **(long **) hp->def);

    else if (ityp == SC_DOUBLE_I)
       sprintf(s, "%.1f", **(double **) hp->def);

    else if (ityp == SC_FLOAT_I)
       sprintf(s, "%.1f", **(float **) hp->def);

    else if (ityp == SC_STRING_I)
       sprintf(s, "%s", **(char ***) hp->def);

    else
       match = FALSE;

    return(match);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIND_REGISTERED - get the call back function info for this object */

static void _PG_find_registered(PG_interface_object *iob, haelem **php,
                                int *pityp)
   {haelem *hp;
    int ityp;
    char *name, *type;

    if (iob != NULL)
       {name = iob->name;
	hp   = SC_hasharr_lookup(_PG.callback_tab, name);
	if (hp != NULL)
	   {type = hp->type;
	    ityp = -1;
	    if (strcmp(type, SC_INTEGER_S) == 0)
	       ityp = SC_INTEGER_I;

	    else if (strcmp(type, SC_LONG_S) == 0)
	       ityp = SC_LONG_I;

	    else if (strcmp(type, SC_DOUBLE_S) == 0)
	       ityp = SC_DOUBLE_I;

	    else if (strcmp(type, SC_FLOAT_S) == 0)
	       ityp = SC_FLOAT_I;

	    else if (strcmp(type, SC_STRING_S) == 0)
	       ityp = SC_STRING_I;};}
    else
       {hp   = NULL;
	ityp = 0;};

    *php   = hp;
    *pityp = ityp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REGISTER_CALLBACK - assign a name to a call back function
 *                      - this lets the read and write connect names
 *                      - with call back functions
 *                      - NOTE: this is not a vararg function, we are using
 *                      - varargs to defeat type checking on the function
 *                      - pointers which is inappropriate for this general
 *                      - purpose mechanism
 */

void PG_register_callback(char *name, ...)
   {long *ip;
    SC_address addr;

    if (_PG.callback_tab == NULL)
       _PG_setup_callback_table();

    SC_VA_START(name);
    addr.funcaddr = SC_VA_ARG(PFInt);
    SC_VA_END;

    ip  = FMAKE(long, "PG_REGISTER_CALLBACK:ip");
    *ip = addr.diskaddr;

    SC_hasharr_install(_PG.callback_tab, name, ip, "procedure", TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LOOKUP_CALLBACK - lookup and return the function pointer registered
 *                    - under name
 */

PFVoid PG_lookup_callback(char *name)
   {long *ip;
    SC_address addr;

    ip = (long *) SC_hasharr_def_lookup(_PG.callback_tab, name);
    if (ip == NULL)
       addr.memaddr = NULL;
    else
       addr.diskaddr = *ip;

    return((PFVoid) addr.funcaddr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REGISTER_VARIABLE - assign a name to a call back variable
 *                      - this lets the read and write connect names
 *                      - with call back variables
 */

void PG_register_variable(char *name, char *type,
                          void *var, void *vn, void *vx)
   {char **s;

    if (_PG.callback_tab == NULL)
       _PG_setup_callback_table();

    s = FMAKE_N(char *, 3, "PG_REGISTER_VARIABLE:s");
    s[0] = (char *) var;
    s[1] = (char *) vn;
    s[2] = (char *) vx;

    SC_hasharr_install(_PG.callback_tab, name, s, type, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LOOKUP_VARIABLE - lookup the named VARIABLE object */

haelem *PG_lookup_variable(char *name)
   {haelem *hp;
    char **sp;

    hp = SC_hasharr_lookup(_PG.callback_tab, name);

/* if the variable doesn't exist then implicitly define a string
 * this is useful for controlling transient objects at least
 */
    if (hp == NULL)
       {sp = FMAKE(char *, "PG_LOOKUP_VARIABLE:sp");
        *sp = NULL;
	PG_register_variable(name, SC_STRING_S, sp, NULL, NULL);
	hp = SC_hasharr_lookup(_PG.callback_tab, name);};

    return(hp);}

/*--------------------------------------------------------------------------*/

/*                           INTERFACE OBJECT ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* PG_ALIGN_INTERFACE_OBJECT - adjust the absolute origins of the interface
 *                           - object tree
 */

static void PG_align_interface_object(PG_interface_object *iob, int xo, int yo)
   {int i, n;
    double ndc[PG_BOXSZ], p[PG_SPACEDM];
    PG_interface_object **ch;
    PG_device *dev;
    PG_curve *crv;

    dev = iob->device;
    crv = iob->curve;

    _PG_parent_limits(dev, iob, ndc);

    p[0] = ndc[0];
    p[1] = ndc[2];
    PG_trans_point(dev, 2, NORMC, p, PIXELC, p);

    n  = SC_array_get_n(iob->children);
    ch = SC_array_array(iob->children);
    for (i = 0; i < n; i++)
        {crv = ch[i]->curve;

	 crv->x_origin = p[0];
	 crv->y_origin = p[1];

	 PG_align_interface_object(ch[i], p[0], p[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ALIGN_INTERFACE_OBJECTS - align all of the interface objects for
 *                            - the given device
 */

static void PG_align_interface_objects(PG_device *dev)
   {int i, niobs, ix, iy;
    PG_interface_object **iobs, *iob;
    PG_curve *crv;

/* align the interface objects, setting the absolute origins of each */
    iobs  = SC_array_array(dev->iobjs);
    niobs = SC_array_get_n(dev->iobjs);

    for (i = 0; i < niobs; i++)
        {iob = iobs[i];
	 if (iob != NULL)
	    {crv = iob->curve;
	     ix  = crv->x_origin;
	     iy  = crv->y_origin;
	     PG_align_interface_object(iob, ix, iy);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_EDIT_INTERFACE_OBJECT - edit an interface object
 *                          - options are: move it around the screen
 *                          -              change the size of the object
 */

static void PG_edit_interface_object(PG_interface_object *iob)
   {double bx[PG_BOXSZ];

    PG_box_init(2, bx, 0, 10000);
    PG_move_object(iob, bx, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_OBJECT - move an interface object */

void PG_move_object(PG_interface_object *iob, double *obx, int redraw)
   {int xo, yo, dx, dy;
    int btn, mod, first, lclr, fclr, lns, bc;
    int ir[PG_SPACEDM];
    double p[PG_SPACEDM];
    PG_logical_operation lop;
    PG_curve *crv;
    PG_device *dev;

    dev = iob->device;
    crv = iob->curve;

    if (dev == NULL)
       return;

    PG_make_device_current(dev);

    PG_get_logical_op(dev, &lop);
    PG_get_line_color(dev, &lclr);
    PG_get_line_style(dev, &lns);
    PG_get_fill_color(dev, &fclr);

    if (obx[0] > obx[1])
       SC_SWAP_VALUE(double, obx[0], obx[1]);

    if (obx[2] > obx[3])
       SC_SWAP_VALUE(double, obx[2], obx[3]);

    bc = dev->BLACK;

    PG_set_color_line(dev, bc, FALSE);
    PG_draw_curve(dev, crv, TRUE);
    PG_set_fill_color(dev, bc);
    PG_fill_curve(dev, crv);

    if (dev->xor_parity)
       {PG_set_color_line(dev, dev->WHITE, FALSE);}
    else
       PG_set_color_line(dev, dev->BLACK, FALSE);

    PG_set_line_style(dev, LINE_DOTTED);
    PG_set_logical_op(dev, GS_XOR);

    first = TRUE;
    btn   = TRUE;
    while (btn || first)
       {PG_query_pointer(dev, ir, &btn, &mod);

        if (first)
           {dx = ir[0] - crv->x_origin;
            dy = ir[1] - crv->y_origin;
            first = FALSE;}

/* erase the old one */
	else
	   PG_draw_curve(dev, crv, TRUE);

	p[0] = ir[0] - dx;
	p[0] = min(p[0], obx[1]);
	p[0] = max(p[0], obx[0]);
	crv->x_origin = p[0];

	p[1] = ir[1] - dy;
	p[1] = min(p[1], obx[3]);
	p[1] = max(p[1], obx[2]);
	crv->y_origin = p[1];

	PG_trans_point(dev, 2, PIXELC, p, NORMC, p);
        crv->rx_origin = p[0];
	crv->ry_origin = p[1];

/* draw the new one */
	PG_draw_curve(dev, crv, TRUE);};

    PG_draw_curve(dev, crv, TRUE);
    PG_set_logical_op(dev, lop);
    PG_set_fill_color(dev, fclr);
    PG_set_line_color(dev, lclr);
    PG_set_line_style(dev, lns);

    xo = crv->x_origin;
    yo = crv->y_origin;

    PG_align_interface_object(iob, xo, yo);
    if (redraw)
       PG_draw_interface_object(iob);

    return;}

/*--------------------------------------------------------------------------*/

/*                        INTERFACE OBJECT DRAW ROUTINES                    */

/*--------------------------------------------------------------------------*/

/* PG_DRAW_INTERFACE_OBJECT - draw a tree of interface objects starting with
 *                          - the given one
 */

void PG_draw_interface_object(PG_interface_object *iob)
   {int i, id, n;
    int oflg[PG_SPACEDM], nflg[PG_SPACEDM];
    PG_interface_object **ch;
    PG_device *dev;
    PG_dev_geometry *g;

    if (iob != NULL)
       {if (iob->is_visible)
	   {dev = iob->device;
	    if ((dev != NULL) && (iob->draw != NULL))
	       {g = &dev->g;

	        PG_make_device_current(dev);

		PG_get_axis_log_scale(dev, 2, oflg);

		for (id = 0; id < 2; id++)
		    nflg[id] = FALSE;

		PG_set_axis_log_scale(dev, 2, nflg);

		(*iob->draw)(iob);

		PG_set_axis_log_scale(dev, 2, oflg);};

	    n  = SC_array_get_n(iob->children);
	    ch = SC_array_array(iob->children);
	    for (i = 0; i < n; i++)
	        {PG_draw_interface_object(ch[i]);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_INTERFACE_OBJECTS - draw all of the visible interface objects
 *                           - which belong to the given device
 */

void PG_draw_interface_objects(PG_device *dev)
   {int i, niobs;
    PG_interface_object **iobs;

    iobs  = SC_array_array(dev->iobjs);
    niobs = SC_array_get_n(dev->iobjs);

    for (i = 0; i < niobs; i++)
        PG_draw_interface_object(iobs[i]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAP_INTERFACE_OBJECT - map a function over a tree of interface objects
 *                         - starting with the given one
 */

void PG_map_interface_object(PG_interface_object *iob, PFEvCallback fnc, void *a)
   {int i, n;
    PG_interface_object **ch;

    if (iob != NULL)
       {(*fnc)(iob, a);

	n  = SC_array_get_n(iob->children);
	ch = SC_array_array(iob->children);
	for (i = 0; i < n; i++)
	    PG_map_interface_object(ch[i], fnc, a);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAP_INTERFACE_OBJECTS - map all of the visible interface objects
 *                          - which belong to the given device
 */

void PG_map_interface_objects(PG_device *dev, PFEvCallback fnc, void *a)
   {int i, niobs;
    PG_interface_object **iobs;

    iobs  = SC_array_array(dev->iobjs);
    niobs = SC_array_get_n(dev->iobjs);

    for (i = 0; i < niobs; i++)
        PG_map_interface_object(iobs[i], fnc, a);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REGISTER_INTERFACE_OBJECT - add the given interface object to the
 *                              - list which the specified device maintains
 */

void PG_register_interface_object(PG_device *dev, PG_interface_object *iob)
   {

    if (dev->iobjs == NULL)
       dev->iobjs = SC_MAKE_ARRAY("PG_REGISTER_INTERFACE_OBJECT",
				  PG_interface_object *, NULL);

    SC_array_push(dev->iobjs, &iob);
    SC_mark(iob, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_COPY_INTERFACE_OBJECTS - copy the interface objects from one
 *                           - device, DVS, to a second one, DVD
 *                           - remove the existing objects from DVD
 *                           - iff RM is TRUE
 */

void PG_copy_interface_objects(PG_device *dvd, PG_device *dvs, int rm)
   {int i, sn;
    PG_interface_object *iob, **si;
    SC_array *sa, *da;

    sa = dvs->iobjs;
    if (sa != NULL)
       {si = SC_array_array(dvs->iobjs);
	sn = SC_array_get_n(dvs->iobjs);

	if (rm == TRUE)
	   PG_clear_interface_objects(dvd);

	da = dvd->iobjs;
        for (i = 0; i < sn; i++)
            {iob = PG_copy_interface_object(dvd, si[i]);
	     if (iob != NULL)
	        {SC_array_push(da, &iob);
		 SC_mark(iob, 1);};};};

    PG_align_interface_objects(dvd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FREE_INTERFACE_OBJECT - release all interface objects for the 
 *                           - specified device
 */

static int _PG_free_interface_object(void *a)
   {PG_interface_object *iob;

    if (a != NULL)
       {iob = *(PG_interface_object **) a;
	if (iob != NULL)
	   _PG_rl_interface_object(iob, TRUE);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FREE_INTERFACE_OBJECTS - release all interface objects for the 
 *                           - specified device
 */

void PG_free_interface_objects(PG_device *dev)
   {

    SC_free_array(dev->iobjs, _PG_free_interface_object);
    dev->iobjs = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_INTERFACE_OBJECTS - like PG_free_interface_object
 *                            - but leaves an empty array in DEV
 */

void PG_clear_interface_objects(PG_device *dev)
   {

    PG_free_interface_objects(dev);
    dev->iobjs = SC_MAKE_ARRAY("PG_CLEAR_INTERFACE_OBJECTS",
			       PG_interface_object *, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PARENT_IS_VARIABLE - is an ancestor of this interface_object a
 *                        - variable?
 */

static PG_interface_object *_PG_parent_is_variable(PG_interface_object *iob)
   {

    for (; iob != NULL; iob = iob->parent)
        if (strcmp(iob->type, PG_VARIABLE_OBJECT_S) == 0)
           return(iob);

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SHIFT_CURVE - shift the curve to the specified origin */

static void _PG_shift_curve(PG_curve *crv, int xo, int yo)
   {int i, n, dx, dy;
    int *x, *y;

    x = crv->x;
    y = crv->y;
    n = crv->n;

    dx = x[0] - xo;
    dy = y[0] - yo;

    for (i = 0; i < n; i++)
        {*x++ -= dx;
         *y++ -= dy;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SET_TEXT_BACKGROUND_COLOR - fixup the background color of the
 *                               - text object if it is unset (-1)
 */

static int _PG_set_text_background_color(PG_interface_object *iob, int flag)
   {int clr;
    PG_text_box *b;
    PG_interface_object *piob;

    if (iob != NULL)
       {clr = iob->device->BLACK;
	if (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0)
	   {b   = (PG_text_box *) iob->obj;
	    clr = flag ? -1 : b->background;
	    for (piob = iob; (piob != NULL) && (clr == -1); piob = piob->parent)
	        clr = piob->background;

	    b->background = clr;};}
    else
       clr = 4;

    return(clr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_TEXT_OBJECT - the draw function for TEXT interface objects */

static void _PG_draw_text_object(PG_interface_object *iob)
   {PG_text_box *b;

    if ((strcmp(iob->type, PG_TEXT_OBJECT_S) != 0) || !iob->is_visible)
       return;

    b = (PG_text_box *) iob->obj;

    b->dev = iob->device;

    _PG_set_text_background_color(iob, TRUE);
    if (b->foreground != b->background)
       PG_refresh_text_box(b);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_VARIABLE_OBJECT - the draw function for
 *                          - VARIABLE interface objects
 */

static void _PG_draw_variable_object(PG_interface_object *iob)
   {int i, nch, ityp, match, isa;
    double dx, dy;
    double ndc[PG_BOXSZ], p[PG_SPACEDM], tx[PG_SPACEDM];
    char s[MAXLINE];
    char *val;
    haelem *hp;
    PG_device *dev;
    PG_text_box *b;
    PG_interface_object **chn, *ch;

    if ((strcmp(iob->type, PG_VARIABLE_OBJECT_S) != 0) ||
	!iob->is_visible)
       return;

    dev = iob->device;

    _PG_find_registered(iob, &hp, &ityp);
    if (hp != NULL)
       {nch = SC_array_get_n(iob->children);
	chn = SC_array_array(iob->children);
	for (i = 0; i < nch; i++)
	    {ch = chn[i];
	     if (ch->is_selectable)
	        {if (strcmp(ch->type, PG_BUTTON_OBJECT_S) == 0)
		    {val   = ch->action_name;
		     match = _PG_value_match(ityp, hp, val);
                     isa   = ch->is_active;
		     if (match && !isa)
		        {SC_SWAP_VALUE(int, ch->foreground, ch->background);}
		     else if (!match && isa)
		        {SC_SWAP_VALUE(int, ch->foreground, ch->background);};

		     ch->is_active = match;}

	         else if (strcmp(ch->type, PG_TEXT_OBJECT_S) == 0)
                    {b = (PG_text_box *) ch->obj;
		     strcpy(s, b->text_buffer[0]);
		     if (_PG_value_string(ityp, hp, s))
		        strcpy(b->text_buffer[0], s);};};};};

    _PG_draw_text_object(iob);

    if ((dev != NULL) && (iob->foreground != iob->background))
       {snprintf(s, MAXLINE, "%s", iob->name);
        PG_get_text_ext_n(dev, 2, NORMC, s, tx);

        _PG_parent_limits(dev, iob, ndc);
	dx = ndc[1] - ndc[0];
	dy = ndc[3] - ndc[2];

	ndc[0] += 0.5*(dx - tx[0]);
	ndc[2] += 0.5*(dy - tx[1]);

	PG_set_color_text(dev, iob->foreground, TRUE);

	p[0] = ndc[0];
	p[1] = ndc[2];
	PG_write_n(dev, 2, NORMC, p, "%s", s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_BUTTON_OBJECT - the draw function for BUTTON interface objects */

static void _PG_draw_button_object(PG_interface_object *iob)
   {int lclr, fclr;
    PG_device *dev;
    PG_curve *crv;

    if (((strcmp(iob->type, PG_BUTTON_OBJECT_S) != 0) &&
	(strcmp(iob->type, PG_CONTAINER_OBJECT_S) != 0)) ||
        !iob->is_visible)
       return;

    dev = iob->device;
    crv = iob->curve;

    if (dev != NULL)
       {PG_get_fill_color(dev, &fclr);
	PG_set_color_fill(dev, iob->background, TRUE);
	PG_fill_curve(dev, crv);
	PG_set_color_fill(dev, fclr, TRUE);

	PG_get_line_color(dev, &lclr);
	PG_set_color_line(dev, dev->WHITE, TRUE);
	PG_draw_curve(dev, crv, TRUE);
	PG_set_color_line(dev, lclr, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_SLIDER_BUTTON - the draw function for SLIDERS */

static void _PG_draw_slider_button(PG_interface_object *iob)
   {int i, nv, nch;
    int dx[PG_SPACEDM];
    double rbx[PG_BOXSZ], pbx[PG_BOXSZ];
    char *name;
    haelem *hp;
    PG_curve *pcrv, *crv;
    PG_interface_object *piob, **chn, *ch;
    PG_device *dev;

    if (((strcmp(iob->type, PG_BUTTON_OBJECT_S) != 0) &&
	(strcmp(iob->type, PG_CONTAINER_OBJECT_S) != 0)) ||
        !iob->is_visible)
       return;

    dev  = iob->device;
    crv  = iob->curve;
    piob = iob->parent;
    pcrv = piob->curve;

    PG_get_curve_extent(dev, crv, PIXELC, rbx);
    PG_get_curve_extent(dev, pcrv, PIXELC, pbx);

    dx[0] = (pbx[1] - pbx[0]) - (rbx[1] - rbx[0]);
    dx[1] = (pbx[3] - pbx[2]) - (rbx[3] - rbx[2]);

    piob = iob->parent->parent;
    nch  = SC_array_get_n(piob->children);
    chn  = SC_array_array(piob->children);
    nv   = 0;
    for (i = 0; i < nch; i++)
        {ch = chn[i];
	 if ((ch != NULL) &&
	     (strcmp(ch->type, PG_VARIABLE_OBJECT_S) == 0))
	    {name = ch->name;
	     hp   = PG_lookup_variable(name);
	     if (hp != NULL)
	        {if ((dx[0] != 0) && (nv == 0))
		    {crv->x_origin = _PG_get_val(hp, pcrv->x_origin, dx[0]);
		     if (dx[1] != 0)
		        {nv++;
			 continue;};
		     break;}
		 else if (dx[1] != 0)
		    {crv->y_origin = _PG_get_val(hp, pcrv->y_origin, dx[1]);
		     break;};};};};

    _PG_draw_button_object(iob);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_KEY_ACTION - carry out the action of BF[0] in the keymap of B */

void PG_key_action(PG_text_box *b, char *bf, int nc)
   {unsigned int c;
    PFKeymap key;

    c   = bf[0];
    key = b->keymap[c];

    _PG_draw_cursor(b, FALSE);
    if (key != NULL)
       (*key)(b);
    else
       (*b->write)(b, bf);

    _PG_draw_cursor(b, TRUE);

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _PG_DRAW_BND_OBJECT - the draw function for boundary interface objects */

static void _PG_draw_bnd_object(PG_interface_object *iob)
   {PG_curve *crv;
    PG_device *dev;

    if (iob->is_visible)
       {dev = iob->device;
        crv = iob->curve;

        PG_draw_curve(dev, crv, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*                       INTERFACE OBJECT ACTION ROUTINES                   */

/*--------------------------------------------------------------------------*/

/* PG_SLIDER - handle a slider button */

static void PG_slider(PG_interface_object *iob, PG_event *ev)
   {PG_device *dev;

    if (ev != NULL)
       {int i, nv, nch;
	int dx[PG_SPACEDM];
        double rbx[PG_BOXSZ], pbx[PG_BOXSZ], obx[PG_BOXSZ];
        char *name;
        haelem *hp;
        PG_curve *pcrv, *crv;
        PG_interface_object *piob, **chn, *ch;

        dev  = iob->device;
        crv  = iob->curve;
        piob = iob->parent;
        pcrv = piob->curve;

        PG_get_curve_extent(dev, crv, PIXELC, rbx);
        PG_get_curve_extent(dev, pcrv, PIXELC, pbx);

        dx[0]  = (pbx[1] - pbx[0]) - (rbx[1] - rbx[0]);
        obx[0] = pcrv->x_origin;
        obx[1] = obx[0] + dx[0];

	dx[1] = (pbx[3] - pbx[2]) - (rbx[3] - rbx[2]);

        obx[2] = pcrv->y_origin;
        obx[3] = obx[2] + dx[1];

	PG_move_object(iob, obx, FALSE);

        if ((dx[0] != 0) || (dx[1] != 0))
	   {piob = piob->parent;
	    nch  = SC_array_get_n(piob->children);
	    chn  = SC_array_array(piob->children);
	    nv   = 0;
	    for (i = 0; i < nch; i++)
	        {ch = chn[i];
		 if ((ch != NULL) &&
		     (strcmp(ch->type, PG_VARIABLE_OBJECT_S) == 0))
		    {name = ch->name;
		     hp   = PG_lookup_variable(name);
		     if (hp != NULL)
		        {if ((dx[0] != 0) && (nv == 0))
			    {obx[1] = crv->x_origin;
			     _PG_set_val(hp, (obx[1] - obx[0]), dx[0]);
			     if (dx[1] != 0)
			        {nv++;
				 continue;};
			     break;}
			 else if (dx[1] != 0)
			    {obx[3] = crv->y_origin;
			     _PG_set_val(hp, (obx[3] - obx[2]), dx[1]);
			     break;};};};};};

        PG_draw_interface_object(piob);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SWITCH_OBJECT_STATE - change the objects state from that given
 *                         - if FLAG is TRUE toggle the item iff
 *                         - the corresponding argument is TRUE
 *                         - if FLAG is FALSE set the item to the value of
 *                         - the corresponding argument
 */

static void _PG_switch_object_state(PG_interface_object *iob, int isv,
                                    int iss, int isa, int flag, int redraw)
   {int iv, is, ia;
    PG_device *dev;

    if (flag)
       {iv = iob->is_visible;
        is = iob->is_selectable;
        ia = iob->is_active;
        iv = isv ? !iv : iv;
        is = iss ? !is : is;
        ia = isa ? !ia : ia;}
    else
       {iv = isv;
        is = iss;
        ia = isa;};

    iob->is_visible    = iv;
    iob->is_selectable = is;
    iob->is_active     = ia;

    if (iv)
       PG_draw_interface_object(iob);
    else
       {dev = iob->device;

        PG_set_fill_color(dev, iob->background);
	PG_fill_curve(dev, iob->curve);
	PG_draw_curve(dev, iob->curve, TRUE);

        if (redraw)
	   PG_draw_interface_objects(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TOGGLE - handle a toggle button */

static void PG_toggle(PG_interface_object *iob, PG_event *ev)
   {int i, niobs;
    char *name, *type, *val;
    haelem *hp;
    PG_interface_object *trans, **iobs, *ch, *ths;

    type = iob->type;
    if (strcmp(type, PG_BUTTON_OBJECT_S) == 0)
       {PG_handle_button_press(iob, ev);

        trans = (PG_interface_object *) iob->obj;
        _PG_switch_object_state(trans, TRUE, TRUE, FALSE, TRUE, TRUE);}

    else if (strcmp(type, PG_VARIABLE_OBJECT_S) == 0)
       {PG_draw_interface_object(iob);

        hp = PG_lookup_variable(iob->name);
        if (hp == NULL)
           return;
        val   = **(char ***) hp->def;

	niobs = SC_array_get_n(iob->children);
	iobs  = SC_array_array(iob->children);
        ths   = NULL;
        for (i = 0; i < niobs; i++)
            {ch = iobs[i];
             if (strcmp(ch->type, PG_BUTTON_OBJECT_S) == 0)
                {name  = ch->action_name;
		 trans = PG_find_object(iob->device, name, NULL);
		 if (trans != NULL)
                    {if (strcmp(val, name) == 0)
		        ths = trans;
		    else
		        _PG_switch_object_state(trans,
						FALSE, FALSE, FALSE,
						FALSE, FALSE);};};};
	if (ths != NULL)
	   _PG_switch_object_state(ths,
				   TRUE, TRUE, FALSE,
				   TRUE, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_VARIABLE - handle the value assignment of the variable bound
 *                    - to the button
 */

static void PG_handle_variable(PG_interface_object *iob, PG_event *ev)
   {char *name, *type, *val;
    haelem *hp;
    PG_interface_object *piob;

    if (_PG_gattrs.edit_mode == TRUE)
       {SC_LOCKON(PG_iob_lock);

	PG_edit_interface_object(iob);
	_PG_gattrs.edit_mode = FALSE;

	SC_LOCKOFF(PG_iob_lock);}

/* find the ancestor containing the variable reference */
    else
       {for (piob = iob; piob != NULL; piob = piob->parent)
            if (strcmp(piob->type, PG_VARIABLE_OBJECT_S) == 0)
	       break;

	if (piob != NULL)
	   {name = piob->name;
	    hp   = PG_lookup_variable(name);
	    if (hp != NULL)
	       {type = hp->type;
		val  = iob->action_name;
		if (strcmp(type, SC_INTEGER_S) == 0)
		   {SET_VAL(int, SC_stoi);}

		else if (strcmp(type, SC_LONG_S) == 0)
		   {SET_VAL(long, SC_stoi);}

		else if (strcmp(type, SC_DOUBLE_S) == 0)
		   {SET_VAL(double, SC_stof);}

		else if (strcmp(type, SC_FLOAT_S) == 0)
		   {SET_VAL(float, SC_stof);}

		else if (strcmp(type, SC_STRING_S) == 0)
		   {SFREE((**(char ***) hp->def));
		    SET_VAL(char *, SC_strsave);};};

	    if (piob->action != NULL)
	       (*piob->action)(piob, ev);
	    else
	       PG_draw_interface_object(piob);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_BUTTON_PRESS - handle a button press
 *                        - return the device associate with a non-NULL event
 *                        - or interpet D as a device
 */

PG_device *PG_handle_button_press(void *d, PG_event *ev)
   {PG_device *dev;

    if (ev == NULL)
       dev = (PG_device *) d;
    else
       {int btn, mod, ityp;
	int ir[PG_SPACEDM];
        PG_interface_object *iob;

        iob  = (PG_interface_object *) d;
        dev  = iob->device;
        ityp = (strcmp(iob->type, PG_BUTTON_OBJECT_S) == 0);

	if (_PG_gattrs.edit_mode == TRUE)
	   {SC_LOCKON(PG_iob_lock);

	    PG_edit_interface_object(iob);
	    _PG_gattrs.edit_mode = FALSE;

	    SC_LOCKOFF(PG_iob_lock);

            return(NULL);};

        if (ityp)
           {SC_SWAP_VALUE(int, iob->foreground, iob->background);
            PG_draw_interface_object(iob);};

        btn = TRUE;
        while (btn)
           PG_query_pointer(dev, ir, &btn, &mod);

        if (ityp)
           {SC_SWAP_VALUE(int, iob->foreground, iob->background);};

        PG_draw_interface_object(iob);};

    return(dev);}

/*--------------------------------------------------------------------------*/

#ifdef HAVE_WINDOW_DEVICE

/*--------------------------------------------------------------------------*/

/* _PG_STRING_VALUE - reset the value in the haelem from the value in
 *                  - the string
 */

static int _PG_string_value(int ityp, haelem *hp, char *s)
   {int match;
    char *ps;

    match = TRUE;
    if (ityp == SC_INTEGER_I)
       **(int **) hp->def = SC_stoi(s);

    else if (ityp == SC_LONG_I)
       **(long **) hp->def = SC_stoi(s);

    else if (ityp == SC_DOUBLE_I)
       **(double **) hp->def = SC_stof(s);

    else if (ityp == SC_FLOAT_I)
       **(float **) hp->def = SC_stof(s);

    else if (ityp == SC_STRING_I)
       {ps = **(char ***) hp->def;
	SFREE(ps);
	**(char ***) hp->def = SC_strsavef(s, "char*:_PG_STRING_VALUE:string");}

    else
       match = FALSE;

    return(match);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_HANDLE_KEYBOARD_EVENT - helper for PG_HANDLE_KEY_PRESS */

void PG_handle_keyboard_event(PG_interface_object *iob, PG_event *ev)
   {char bf[80], *s;
    int iev[PG_SPACEDM], mod, ityp;
    haelem *hp;
    PG_text_box *b;
    PG_device *dev;
    PG_interface_object *par;

    dev = iob->device;

    for (bf[0] = '\0'; TRUE; )
        {PG_key_event_info(dev, ev, iev, bf, 80, &mod);
	 if ((bf[0] == '\0') && (mod != 0))
	    PG_get_next_event(ev);
	 else
	    break;};
    bf[1] = '\0';

    if (bf[0] != '\0')
       {b   = (PG_text_box *) iob->obj;
	par = _PG_parent_is_variable(iob);
	if (par != NULL)
	   switch (bf[0])
	      {case '\r' :
	       case '\n' :
		    _PG_find_registered(par, &hp, &ityp);
		    if (hp != NULL)
		       {s = b->text_buffer[0];
			_PG_string_value(ityp, hp, s);
			if (par->parent != NULL)
			   PG_draw_interface_object(par->parent);};

		    return;

	       default :
		    break;};
                   
	KEY_ACTION(b, bf, 80);};

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* PG_HANDLE_KEY_PRESS - handle key down events */

PG_device *PG_handle_key_press(void *d, PG_event *ev)
   {PG_device *dev;

#ifdef HAVE_WINDOW_DEVICE

    if (ev == NULL)
       dev = (PG_device *) d;
    else
       {PG_interface_object *iob;
	int type;

        type = PG_EVENT_TYPE(*ev);
	dev  = NULL;

/* mouse events are used in edit mode so handle them */
	if (type == MOUSE_DOWN_EVENT)
           PG_handle_button_press(d, ev);

        else if (type == KEY_DOWN_EVENT)
	   {iob = (PG_interface_object *) d;
	    if (iob != NULL)
	       {dev = iob->device;
		PG_handle_keyboard_event(iob, ev);};};};

#else

    dev = (PG_device *) d;

#endif

    return(dev);}

/*--------------------------------------------------------------------------*/

/*                     INTERFACE OBJECT SELECTION ROUTINES                  */

/*--------------------------------------------------------------------------*/

/* PG_EVENT_SELECT_VISUAL - check the event to see if it occured in
 *                        - the given interface object or
 *                        - one of its children
 *                        - return the interface object if any
 *                        - and NULL otherwise
 *                        - this is based on visual containment
 *                        - other event selectors would be appropriate to the
 *                        - specific interface object (e.g. a PG_graph)
 */

static PG_interface_object *PG_event_select_visual(PG_interface_object *iob,
                                                   PG_event *ev)
   {

#ifdef HAVE_WINDOW_DEVICE

    int i, n, nch, qualifier;
    int iev[PG_SPACEDM];
    PG_mouse_button button;
    PG_interface_object *ch, **chn;
    PG_device *dev;
    PG_curve *crv;

    if (!iob->is_selectable || PG_is_inactive_text_box(iob))
       return(NULL);

    dev = iob->device;

    PG_mouse_event_info(dev, ev, iev, &button, &qualifier);

    crv = iob->curve;

    iev[0] -= crv->x_origin;
    iev[1] -= crv->y_origin;
    n       = crv->n - 1;

    if (PM_inside_fix(iev[0], iev[1], crv->x, crv->y, n, 1))
       {nch = SC_array_get_n(iob->children);
	chn = SC_array_array(iob->children);
        for (i = 0; i < nch; i++)
            {ch = PG_event_select_visual(chn[i], ev);
             if (ch != NULL)
                return(ch);};

        return(iob);}

    else

#endif

       return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_EVENT_SELECT_LOGICAL - check the event to see if it occured in
 *                         - the given interface object or
 *                         - one of its children
 *                         - return the interface object if any
 *                         - and NULL otherwise
 *                         - this is based on logical containment
 */

static PG_interface_object *PG_event_select_logical(PG_interface_object *iob,
                                                    PG_event *ev)
   {PG_interface_object *lch;

    lch = NULL;

#ifdef HAVE_WINDOW_DEVICE

    if (iob->is_selectable)
       {int i, n, nch, qualifier;
	int iev[PG_SPACEDM], jev[PG_SPACEDM];
	PG_mouse_button button;
	PG_interface_object *ch, **chn;
	PG_device *dev;
	PG_curve *crv;

	dev = iob->device;

	PG_mouse_event_info(dev, ev, jev, &button, &qualifier);

	nch = SC_array_get_n(iob->children);
	chn = SC_array_array(iob->children);
	for (i = 0; i < nch; i++)
	    {ch = chn[i];
	     if (ch->is_selectable)
	        {lch = PG_event_select_logical(ch, ev);
		 if (lch == NULL)
		    {crv    = ch->curve;
		     iev[0] = jev[0] - crv->x_origin;
		     iev[1] = jev[1] - crv->y_origin;
		     n      = crv->n - 1;

		     if (PM_inside_fix(iev[0], iev[1], crv->x, crv->y, n, -1))
		        {lch = ch;
			 break;};}
		 else
		    break;};};

	if (i >= nch)
	   {crv    = iob->curve;
	    iev[0] = jev[0] - crv->x_origin;
	    iev[1] = jev[1] - crv->y_origin;
	    n      = crv->n - 1;
	    if (PM_inside_fix(iev[0], iev[1], crv->x, crv->y, n, -1))
	       lch = iob;};}; 

#endif

    return(lch);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_OBJECT_EVENT - return the interface object in which the 
 *                     - given event occured
 */

PG_interface_object *PG_get_object_event(PG_device *dev, PG_event *ev)
   {int i, niobs, flag;
    PG_interface_object *tiob, *iob, **iobs;

    iob   = NULL;
    iobs  = SC_array_array(dev->iobjs);
    niobs = SC_array_get_n(dev->iobjs);

    for (i = 0; i < niobs; i++)
        {tiob = iobs[i];
	 if (tiob != NULL)
	    {flag = (tiob->is_selectable && (tiob->select != NULL));
	     if (flag == TRUE)
	        {iob = (*tiob->select)(tiob, ev);
		 if (iob != NULL)
		    break;};};};

    return(iob);}

/*--------------------------------------------------------------------------*/

/*                          MEMORY MANAGEMENT ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* PG_MAKE_INTERFACE_OBJECT - create and return an graphical interface
 *                          - object
 */

PG_interface_object *PG_make_interface_object(PG_device *dev,
                                              char *name, char *type,
					      void *obj,
					      PG_text_alignment align,
					      double ang,
                                              PG_curve *crv,
                                              int *flags, int fc, int bc,
                                              char *select, char *draw, char *action,
                                              PG_interface_object *parent)
   {PG_interface_object *iob;
    PFVoid fnc;

    iob = FMAKE(PG_interface_object, "PG_MAKE_INTERFACE_OBJECT:iob");
   
    iob->device        = dev;
    iob->curve         = crv;
    iob->name          = SC_strsavef(name,
				     "char*:PG_MAKE_INTERFACE_OBJECT:name");
    iob->type          = SC_strsavef(type,
				     "char*:PG_MAKE_INTERFACE_OBJECT:type");
    iob->obj           = obj;
    iob->foreground    = fc;
    iob->background    = bc;
    iob->is_visible    = flags[0];
    iob->is_active     = flags[2];
    iob->is_selectable = flags[1];

    iob->draw_name     = NULL;
    iob->action_name   = NULL;
    iob->select_name   = NULL;

    iob->draw          = NULL;
    iob->action        = NULL;
    iob->select        = NULL;

    iob->parent        = parent;
    iob->children        = SC_MAKE_ARRAY("PG_MAKE_INTERFACE_OBJECT",
				       PG_interface_object *, NULL);

    if (strcmp(type, PG_TEXT_OBJECT_S) == 0)
       {PG_text_box *b;

	b = PG_open_text_rect(dev, "IOB", FALSE, NULL, crv, 0.0, TRUE);

	SFREE(b->text_buffer[0]);
	b->text_buffer[0] = SC_strsavef(name,
					"char*:PG_MAKE_INTERFACE_OBJECT:name");
	b->n_chars_line = max(b->n_chars_line, strlen(name));
	b->align        = align;
	b->angle        = ang;
        b->foreground   = fc;
        b->background   = bc;

        if (b->n_lines > 2)
           b->type = SCROLLING_WINDOW;
        else
           {b->type   = FALSE;
	    b->border = 0.0;};

        iob->obj    = (void *) b;
        iob->action = (PFIobAction) PG_handle_key_press;
	iob->draw   = _PG_draw_text_object;

        _PG_set_text_background_color(iob, FALSE);}

    else if (strcmp(type, PG_VARIABLE_OBJECT_S) == 0)
       iob->draw = _PG_draw_variable_object;

    else
       iob->draw = _PG_draw_button_object;

    if (select != NULL)
       {iob->select_name = SC_strsavef(select,
				       "char*:PG_MAKE_INTERFACE_OBJECT:select");
        iob->select      = (PFIobSelect) PG_lookup_callback(select);};

    if (iob->select == NULL)
       {if (strcmp(type, PG_CONTAINER_OBJECT_S) == 0)
           iob->select = PG_event_select_logical;
	else
           iob->select = PG_event_select_visual;};

    if (draw != NULL)
       {iob->draw_name = SC_strsavef(draw,
				     "char*:PG_MAKE_INTERFACE_OBJECT:draw");
        iob->draw      = (PFIobDraw) PG_lookup_callback(draw);};

    if (action != NULL)
       {iob->action_name = SC_strsavef(action,
				       "char*:PG_MAKE_INTERFACE_OBJECT:action");
        fnc              = PG_lookup_callback(action);
        if (fnc == (PFVoid) PG_slider)
           _PG_shift_curve(iob->curve, 0, 0);

        if (fnc != NULL)
           iob->action = (PFIobAction) fnc;

        else if (_PG_parent_is_variable(parent) != NULL)
           iob->action = PG_handle_variable;};

/* make things at least editable by default */
    if (iob->action == NULL)
       iob->action = (PFIobAction) PG_handle_button_press;

    return(iob);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_COPY_INTERFACE_OBJECT - copy an interface object IOB into
 *                          - the specified device DVD
 */

PG_interface_object *PG_copy_interface_object(PG_device *dvd,
                                              PG_interface_object *iob)
   {int i, sn;
    PG_device *dvs;
    PG_interface_object *niob, **sch;
    SC_array *da;

    if (iob == NULL)
       return(NULL);

    dvs = iob->device;

    niob = FMAKE(PG_interface_object, "PG_COPY_INTERFACE_OBJECT:niob");

    *niob = *iob;

    niob->device = dvd;
    niob->name   = SC_strsavef(iob->name,
			       "char*:PG_COPY_INTERFACE_OBJECT:name");
    niob->type   = SC_strsavef(iob->type,
			       "char*:PG_COPY_INTERFACE_OBJECT:type");

/* check to see if this object is a text box */
    if (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0)
       niob->obj = PG_copy_text_object(iob->obj, dvd, dvs);

/* copy the curve mapping PC values from iob->device -> niob->device */
    niob->curve = PG_copy_curve(iob->curve, dvd, dvs);

/* copy the parent */
    niob->parent = PG_copy_interface_object(dvd, iob->parent);

/* copy the children */
    sn  = SC_array_get_n(iob->children);
    sch = SC_array_array(iob->children);

    da = SC_MAKE_ARRAY("PG_COPY_INTERFACE_OBJECT", PG_interface_object *, NULL);
    for (i = 0; i < sn; i++)
        SC_array_push(da, sch[i]);

    niob->children = da;

    if (MONOCHROME_POSTSCRIPT_DEVICE(dvd))
       niob->background = dvd->BLACK;

    return(niob);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_INTERFACE_OBJECT - release an interface object
 *                         - if flag is TRUE recursively release children
 */

void _PG_rl_interface_object(PG_interface_object *iob, int flag)
   {int i, n;
    PG_interface_object **ch;

    if (flag)
       {n  = SC_array_get_n(iob->children);
	ch = SC_array_array(iob->children);
        for (i = 0; i < n; i++)
            _PG_rl_interface_object(ch[i], TRUE);

        SFREE(ch);};

    if (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0)
       PG_close_text_box((PG_text_box *) (iob->obj));

    else if (strcmp(iob->type, PG_VARIABLE_OBJECT_S) == 0)
       SFREE(iob->obj);

    SFREE(iob->name);
    SFREE(iob->type);
    SFREE(iob);
   
    return;}

/*--------------------------------------------------------------------------*/

/*                       SIMPLE OBJECTS FOR APPLICATIONS                    */

/*--------------------------------------------------------------------------*/

/* PG_ADD_TEXT_OBJ - register a TEXT interface object with the device */

void PG_add_text_obj(PG_device *dev, double *obx, char *s)
   {int flags[3];
    double dx[PG_SPACEDM], co[PG_SPACEDM], tbx[PG_BOXSZ];
    PG_curve *tcrv;
    PG_interface_object *tiob;

    if (dev != NULL)
       {PG_get_text_ext_n(dev, 2, NORMC, s, dx);

        co[0] = obx[0];
	co[1] = obx[2];

	tbx[0] = obx[0] + 0.5*(obx[1] - obx[0] - dx[0]);
	tbx[1] = tbx[0] + dx[0];
	tbx[2] = obx[2] + 0.5*(obx[3] - obx[2] - dx[1]);
	tbx[3] = tbx[2] + dx[1];

	flags[0] = TRUE;
	flags[1] = FALSE;
	flags[2] = FALSE;

	tcrv = PG_make_box_curve(dev, NORMC, co, tbx);
	tiob = PG_make_interface_object(dev, s, PG_TEXT_OBJECT_S, NULL,
					CENTER, 0.0, tcrv, flags,
					dev->DARK_BLUE, -1,
					NULL, "draw-text", NULL,
					NULL);

	PG_register_interface_object(dev, tiob);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ADD_BUTTON - register a BUTTON interface object with the device */

void PG_add_button(PG_device *dev, double *obx, char *s, PFEvCallback fnc)
   {int flags[3];
    double dx[PG_SPACEDM], co[PG_SPACEDM], tbx[PG_BOXSZ];
    PG_interface_object *tiob, *biob;
    PG_curve *bcrv, *tcrv;

    if ((dev == NULL) || (fnc == NULL))
       return;

    co[0] = 0.0;
    co[1] = 0.0;

    PG_register_callback(s, fnc);

    bcrv = PG_make_box_curve(dev, NORMC, co, obx);

    PG_get_text_ext_n(dev, 2, NORMC, s, dx);

    co[0] = obx[0];
    co[1] = obx[2];

    tbx[0] = obx[0] + 0.5*(obx[1] - obx[0] - dx[0]);
    tbx[1] = tbx[0] + dx[0];
    tbx[2] = obx[2] + 0.5*(obx[3] - obx[2] - dx[1]);
    tbx[3] = tbx[2] + dx[1];

    flags[0] = TRUE;
    flags[1] = TRUE;
    flags[2] = FALSE;

    tcrv = PG_make_box_curve(dev, NORMC, co, tbx);
    biob = PG_make_interface_object(dev, "BUTTON", PG_BUTTON_OBJECT_S, s,
				    CENTER, 0.0, bcrv, flags,
				    dev->BLACK, dev->GRAY,
				    NULL, "draw-button", s,
                                    NULL);

    flags[0] = TRUE;
    flags[1] = FALSE;
    flags[2] = FALSE;

    tiob = PG_make_interface_object(dev, s, PG_TEXT_OBJECT_S, NULL,
				    CENTER, 0.0, tcrv, flags,
				    dev->DARK_BLUE, -1,
				    NULL, "draw-text", NULL,
                                    biob);

    PG_PUSH_CHILD_IOB(biob, tiob);

    PG_register_interface_object(dev, biob);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SETUP_CALLBACK_TABLE - initialize the call back table */

static void _PG_setup_callback_table(void)
   {

    SC_LOCKON(PG_iob_lock);

    _PG.callback_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);

/* action methods */
    PG_register_callback("slider", PG_slider);
    PG_register_callback("toggle", PG_toggle);
    PG_register_callback("set-var", PG_handle_variable);

/* draw methods */
    PG_register_callback("draw-button", _PG_draw_button_object);
    PG_register_callback("draw-container", _PG_draw_button_object);
    PG_register_callback("draw-slider", _PG_draw_slider_button);
    PG_register_callback("draw-text", _PG_draw_text_object);
    PG_register_callback("draw-variable", _PG_draw_variable_object);

/* select methods */
    PG_register_callback("select-visual", PG_event_select_visual);
    PG_register_callback("select-logical", PG_event_select_logical);

    SC_LOCKOFF(PG_iob_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
