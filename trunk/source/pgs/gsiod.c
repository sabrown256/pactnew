/*
 * GSIOD.C - interface object definition routines for PGS
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

static int
 zero    = 0,
 one     = 1,
 hundred = 100;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MAKE_IOB_BOX - make a PG_curve to delimit an interface object */

static PG_curve *_PG_make_iob_box(PG_device *dev, double *cbx)
   {double xd[5], yd[5];
    double xo[PG_SPACEDM];
    double *r[PG_SPACEDM];
    PG_curve *crv;

    xo[0] = cbx[0];
    xo[1] = cbx[2];

    xd[0] = cbx[0];
    yd[0] = cbx[2];

    xd[1] = cbx[1];
    yd[1] = cbx[2];

    xd[2] = cbx[1];
    yd[2] = cbx[3];

    xd[3] = cbx[0];
    yd[3] = cbx[3];

    xd[4] = cbx[0];
    yd[4] = cbx[2];

    r[0] = xd;
    r[1] = yd;

    crv = PG_make_curve(dev, NORMC, TRUE, 5, xo, r);

    return(crv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_IOB_MAKE_CONTAINER - make a container interface object */

PG_interface_object *PG_iob_make_container(PG_device *dev, char *name,
					   double *bx)
   {int fc, bc;
    int flags[N_IOB_FLAGS];
    PG_curve *crv;
    PG_interface_object *cnt;

    flags[0] = TRUE;
    flags[1] = TRUE;
    flags[2] = FALSE;

    fc = dev->BLUE;
    bc = dev->BLACK;

    crv = _PG_make_iob_box(dev, bx);
    cnt = PG_make_interface_object_n(dev, name, "Container", NULL,
				     crv, flags, fc, bc,
				     NULL, NULL, NULL, NULL, NULL);

    return(cnt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SCALE_RECT_TO_CONTAINER - given a rectangle specified in NDC
 *                             - scale the coordinates to be the rectangle
 *                             - relative to the container
 */

static void _PG_scale_rect_to_container(PG_device *dev, 
					PG_curve *crv, double *bx)
   {int i, n;
    int ixc[PG_SPACEDM], xo[PG_SPACEDM];
    int ix[PG_BOXSZ];
    int *xi, *yi;
    double dx[PG_SPACEDM];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];

    xi = crv->x;
    yi = crv->y;
    n  = crv->n;
    xo[0] = crv->x_origin;
    xo[1] = crv->y_origin;

    ix[0] = INT_MAX;
    ix[1] = INT_MIN;
    ix[2] = INT_MAX;
    ix[3] = INT_MIN;
    for (i = 0; i < n; i++)
        {ixc[0] = xi[i] + xo[0];
	 ixc[1] = yi[i] + xo[1];
	 ix[0] = min(ix[0], ixc[0]);
	 ix[1] = max(ix[1], ixc[0]);
	 ix[2] = min(ix[2], ixc[1]);
	 ix[3] = max(ix[3], ixc[1]);};

/* find the NCD limits of the curve */
    xl[0] = ix[0];
    xl[1] = ix[2];
    PG_trans_point(dev, 2, PIXELC, xl, NORMC, xl);
    xr[0] = ix[1];
    xr[1] = ix[3];
    PG_trans_point(dev, 2, PIXELC, xr, NORMC, xr);

/* exchange to make xl[1] less than xr[1] */
    if (xl[1] > xr[1])
       {SC_SWAP_VALUE(double, xl[1], xr[1]);};

    dx[0] = xr[0] - xl[0];
    dx[1] = xr[1] - xl[1];

    bx[0] = xl[0] + bx[0]*dx[0];
    bx[1] = xl[0] + bx[1]*dx[0];
    bx[2] = xl[1] + bx[2]*dx[1];
    bx[3] = xl[1] + bx[3]*dx[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ADD_IOB - add the specified interface object to the given container */

PG_interface_object *PG_add_iob(PG_device *dev,
				PG_interface_object *cnt,
				double *bx,
				char *type, ...)
   {int fc, bc, ok, tag, ifl;
    int flags[N_IOB_FLAGS];
    void *obj;
    char *name, *item, *fsel, *fdrw, *fact;
    PG_interface_object *iob;
    PG_curve *crv;

    if (cnt != NULL)
       _PG_scale_rect_to_container(dev, cnt->curve, bx);

    flags[0] = FALSE;
    flags[1] = FALSE;
    flags[2] = FALSE;

    fc   = dev->BROWN;
    bc   = dev->BLACK;
    name = NULL;
    obj  = NULL;
    fsel = NULL;
    fdrw = NULL;
    fact = NULL;

    SC_VA_START(type);

    ok = TRUE;
    while (ok)
       {tag = SC_VA_ARG(int);

        switch (tag)
           {case PG_IOB_NAME:
                 name = SC_VA_ARG(char *);
                 break;
	    case PG_IOB_CLR:
                 fc = SC_VA_ARG(int);
                 bc = SC_VA_ARG(int);
                 break;
	    case PG_IOB_FLG:
                 ifl = SC_VA_ARG(int);
		 flags[ifl] = TRUE;
                 break;
	    case PG_IOB_OBJ:
                 obj = SC_VA_ARG(void *);
                 break;
	    case PG_IOB_DRW:
                 fdrw = SC_VA_ARG(char *);
                 break;
	    case PG_IOB_ACT:
                 fact = SC_VA_ARG(char *);
                 item = SC_VA_ARG(char *);
		 if (item != NULL)
		    obj = PG_find_object(dev, item, cnt);
                 break;
	    case PG_IOB_SEL:
                 fsel = SC_VA_ARG(char *);
                 break;
	    case PG_IOB_END:
                 ok = FALSE;
                 break;};};

    SC_VA_END;

    crv = _PG_make_iob_box(dev, bx);
    iob = PG_make_interface_object_n(dev, name, type, obj,
				     crv, flags, fc, bc,
				     NULL, fsel, fdrw, fact, cnt);

    if (cnt != NULL)
       {PG_PUSH_CHILD_IOB(cnt, iob);};

    return(iob);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_IOB_TOGGLE_VAR - toggle a variable on or off */

static void _PG_iob_toggle_var(PG_interface_object *iob, PG_event *ev)
   {int *pv;
    haelem *hp;
    PG_interface_object *piob;

    piob = (PG_interface_object *) iob->obj;
    hp   = PG_lookup_variable(piob->name);
    if (hp != NULL)
       {pv = ((int **) hp->def)[0];
	if (pv != NULL)
	   {*pv = !(*pv);

	    SC_SWAP_VALUE(int, iob->foreground, iob->background);
	    PG_draw_interface_object(iob);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_IOB_REGISTER_TOGGLE - register a variable as a button that toggles
 *                        - on and off when pressed
 */

void PG_iob_register_toggle(PG_device *dev, PG_interface_object *cnt,
			    char *name, char *abbrv, int *pv,
			    double *bx, int on_clr, int off_clr)
   {int bc, btxtc;
    double abx[PG_BOXSZ];
    PG_interface_object *var, *btn, *iob;

    PG_register_variable(name, SC_INT_S, pv, &zero, &one);
    PG_register_callback("toggle-var", _PG_iob_toggle_var);

/*
 Container CLR(0,0) BND(RECT) (0.005,0.800) (0.995,0.900)
   {Button CLR(3,2) DRW(draw-button) ACT(Start) BND(RECT) (0.1,0.2) (0.3,0.8)
      {Text NAME(Start) CLR(10,-1) FLG(IsVis) DRW(draw-text) BND(RECT) (0.05,0.3) (0.95,0.95)}
*/

    bc    = -1;
    btxtc = dev->WHITE;

    abx[0] = bx[0];
    abx[1] = bx[1];
    abx[2] = bx[2];
    abx[3] = bx[3];
    var = PG_add_iob(dev, cnt, abx, "Variable",
		     PG_IOB_CLR, bc, bc,
		     PG_IOB_NAME, name,
		     PG_IOB_FLG, PG_IsVis,
		     PG_IOB_FLG, PG_IsSel,
		     PG_IOB_END);

    PG_box_init(2, abx, 0.0, 1.0);
    btn = PG_add_iob(dev, var, abx, "Button",
		     PG_IOB_CLR, off_clr, on_clr,
		     PG_IOB_FLG, PG_IsVis,
		     PG_IOB_FLG, PG_IsSel,
		     PG_IOB_ACT, "toggle-var", name,
		     PG_IOB_END);

    PG_box_init(2, abx, 0.0, 1.0);
    iob = PG_add_iob(dev, btn, abx, "Text",
		     PG_IOB_NAME, abbrv,
		     PG_IOB_CLR, btxtc, bc,
		     PG_IOB_FLG, PG_IsVis,
		     PG_IOB_DRW, "draw-text",
		     PG_IOB_END);

    SC_ASSERT(iob != NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_IOB_REGISTER_SLIDER - register a variable as a slider
 *                        - with a value between 0 and 100
 *                        - DX and DY determine the size off the slider knob
 *                        - DX = 1 makes a vertical (up/down) slider
 *                        - DY = 1 makes a horizontal (left/right) slider
 *                        - both less makes 2D slider
 */

void PG_iob_register_slider(PG_device *dev, PG_interface_object *cnt,
			    char *name, char *abbrv, int *pv,
			    double *bx, double dx, double dy)
   {PG_interface_object *var, *btn, *c1, *c2;
    double abx[PG_BOXSZ];

    PG_register_variable(name, SC_INT_S, pv, &zero, &hundred);

/*
   Container CLR(0,0) BND(RECT) (0.005,0.400) (0.995,0.500)
     {Container CLR(0,0) BND(RECT) (0.2,0.2) (0.8,0.8)
        {Variable NAME(Gain) CLR(9,-1) BND(RECT) (-0.3,0.2) (-0.1,1.0)
           {Text NAME(0.7) CLR(7,-1) BND(RECT) (7.0,0.2) (7.5,1.0)}
         Container CLR(2,2) BND(RECT) (0.0,0.0) (1.0,1.0)
           {Button CLR(0,0) DRW(draw-slider) ACT(slider) BND(RECT) (0.0,0.0) (0.05,1.0)}}
     }
*/
    c1 = PG_add_iob(dev, cnt, bx, "Container",
		    PG_IOB_CLR, 0, 0,
		    PG_IOB_FLG, PG_IsVis,
		    PG_IOB_FLG, PG_IsSel,
		    PG_IOB_END);

    abx[0] = -0.3;
    abx[1] = -0.1;
    abx[2] =  0.2; 
    abx[3] =  1.0;
    var = PG_add_iob(dev, c1, abx, "Variable",
		     PG_IOB_NAME, name,
		     PG_IOB_FLG, PG_IsSel,
		     PG_IOB_END);

    SC_ASSERT(var != NULL);

    PG_box_init(2, abx, 0.0, 1.0);
    c2 = PG_add_iob(dev, c1, abx, "Container",
		    PG_IOB_CLR, 2, 2,
		    PG_IOB_FLG, PG_IsVis,
		    PG_IOB_FLG, PG_IsSel,
		    PG_IOB_END);

    abx[0] = 0.0;
    abx[1] = dx;
    abx[2] = 0.0; 
    abx[3] = dy;
    btn = PG_add_iob(dev, c2, abx, "Button",
		     PG_IOB_DRW, "draw-slider",
		     PG_IOB_ACT, "slider", name,
		     PG_IOB_CLR, 0, 0,
		     PG_IOB_FLG, PG_IsVis,
		     PG_IOB_FLG, PG_IsSel,
		     PG_IOB_END);

    SC_ASSERT(btn != NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
