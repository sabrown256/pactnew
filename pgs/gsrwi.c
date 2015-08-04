/*
 * GSRWI.C - read and write a portable user interface desription
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

extern void
 _PG_find_callback(PG_interface_object *iob,
		   haelem **php, int *pityp);

/*--------------------------------------------------------------------------*/

/*                       INTERFACE READ/WRITE ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* _PG_PARENT_LIMITS - find the limits of the parent entity in NDC */

void _PG_parent_limits(PG_device *dev, PG_interface_object *parent,
		       double *ndc)
   {int i, n, *px, *py, ix0, iy0;
    double x[PG_SPACEDM];
    PG_curve *crv;

    PG_box_init(2, ndc, 0.0, 1.0);

    if (parent != NULL)
       {crv = parent->curve;
        n   = crv->n;

        px  = crv->x;
        py  = crv->y;
        ix0 = crv->x_origin;
        iy0 = crv->y_origin;

	PG_box_init(2, ndc, 1.0, 0.0);

        for (i = 0; i < n; i++)
            {x[0] = px[i] + ix0;
             x[1] = py[i] + iy0;

	     PG_trans_point(dev, 2, PIXELC, x, NORMC, x);

             ndc[0] = min(ndc[0], x[0]);
             ndc[1] = max(ndc[1], x[0]);
             ndc[2] = min(ndc[2], x[1]);
             ndc[3] = max(ndc[3], x[1]);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RECTANGLE_P - return TRUE iff the curve is a rectangle */

static int _PG_rectangle_p(PG_curve *crv)
   {int rv, npt;
    int *x, *y;

    npt = crv->n;
    x = crv->x;
    y = crv->y;

    rv = FALSE;

    if (npt == 5)
       rv = ((x[0] == x[4]) && (x[0] == x[3]) && (x[1] == x[2]) &&
	     (y[0] == y[4]) && (y[0] == y[1]) && (y[2] == y[3]));
            
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_INTERFACE_OBJECT - write out an interface object to the
 *                           - given file
 *                           - return TRUE iff successful
 */

static int PG_write_interface_object(FILE *fp, PG_interface_object *iob,
                                     char *indent, int nopf)
   {int i, nc, niobs, npt, xo, yo, fc, bc;
    int flags[N_IOB_FLAGS];
    int *x, *y;
    double xs[PG_SPACEDM], dx, dy;
    double ndc[PG_BOXSZ];
    char *type, s[MAXLINE];
    PG_device *dev;
    PG_interface_object *ch;
    PG_curve *crv;

    PRINT(fp, "%s%s", indent, iob->type);
    nc = strlen(indent);
    if (nc > 0)
       indent[nc-1] = ' ';

    dev  = iob->device;
    type = iob->type;

    if (strcmp(type, PG_TEXT_OBJECT_S) == 0)
       {PG_text_box *b;

        CFREE(iob->name);
	b = (PG_text_box *) iob->obj;
	
	snprintf(s, MAXLINE, "\"%s\"", b->text_buffer[0]);
	iob->name = CSTRSAVE(s);};

    if (iob->name != NULL)
       PRINT(fp, " NAME(%s)", iob->name);

    fc = iob->foreground;
    if (fc == dev->BLACK)
       fc = 0;
    else if (fc == dev->WHITE)
       fc = 1;

    bc = iob->background;
    if (bc == dev->BLACK)
       bc = 0;
    else if (bc == dev->WHITE)
       bc = 1;

    PRINT(fp, " CLR(%d,%d)", fc, bc);

    flags[0] = iob->is_visible;
    flags[1] = iob->is_selectable;
    flags[2] = iob->is_active;
    if ((flags[0] != TRUE) || (flags[1] != TRUE) || (flags[2] != FALSE))
       {for (i = 0, nc = 0; i < N_IOB_FLAGS; i++)
            nc += flags[i];

        PRINT(fp, " FLG(");
        if (nc == 0)
	   PRINT(fp, " ");

        for (i = 0; i < N_IOB_FLAGS; i++)
            {if (flags[i])
                {switch (i)
                    {case 0 :
		          PRINT(fp, "IsVis");
			  break;
                     case 1 :
		          PRINT(fp, "IsSel");
			  break;
                     case 2 :
		          PRINT(fp, "IsAct");
			  break;};
		  if (nc-- > 1)
		     PRINT(fp, ",");};};
	PRINT(fp, ")");};

    if (iob->draw_name != NULL)
       PRINT(fp, " DRW(%s)", iob->draw_name);

    if (iob->action_name != NULL)
       {char *name;

	name = iob->action_name;
        if (strcmp(name, "toggle") == 0)
	   {char *lty;
            PG_interface_object *trans;

            lty = iob->type;
	    if (strcmp(lty, PG_BUTTON_OBJECT_S) == 0)
	       {trans = (PG_interface_object *) iob->obj;
		PRINT(fp, " ACT(toggle,%s)", trans->name);}

	    else if (strcmp(lty, PG_VARIABLE_OBJECT_S) == 0)
	       PRINT(fp, " ACT(toggle)");}

        else
	   PRINT(fp, " ACT(%s)", iob->action_name);};

    if (iob->select_name != NULL)
       PRINT(fp, " SEL(%s)", iob->select_name);

    crv = iob->curve;
    npt = crv->n;
    xo  = crv->x_origin;
    yo  = crv->y_origin;

    _PG_parent_limits(dev, iob->parent, ndc);
    dx = ndc[1] - ndc[0];
    dy = ndc[3] - ndc[2];

    x = crv->x;
    y = crv->y;
    if (_PG_rectangle_p(crv))
       {PRINT(fp, " BND(RECT)");
	xs[0] = x[0] + xo;
	xs[1] = y[0] + yo;
	PG_trans_point(dev, 2, PIXELC, xs, NORMC, xs);
	xs[0] = (xs[0] - ndc[0])/dx;
	xs[1] = (xs[1] - ndc[2])/dy;
	PRINT(fp, " (%5.3f,%5.3f)", xs[0], xs[1]);

	xs[0] = x[2] + xo;
	xs[1] = y[2] + yo;
	PG_trans_point(dev, 2, PIXELC, xs, NORMC, xs);
	xs[0] = (xs[0] - ndc[0])/dx;
	xs[1] = (xs[1] - ndc[2])/dy;
	PRINT(fp, " (%5.3f,%5.3f)", xs[0], xs[1]);}

    else
       {PRINT(fp, "\n%sBND(%d)", indent, npt);
	for (i = 0; i < npt; i++)
	    {xs[0] = x[i] + xo;
	     xs[1] = y[i] + yo;
	     PG_trans_point(dev, 2, PIXELC, xs, NORMC, xs);
	     xs[0] = (xs[0] - ndc[0])/dx;
	     xs[1] = (xs[1] - ndc[2])/dy;
	     PRINT(fp, " (%5.3f,%5.3f)", xs[0], xs[1]);};};

    niobs = SC_array_get_n(iob->children);
    if (niobs > 0)
       {PRINT(fp, "\n");

        npt = strlen(indent);
        memset(indent, ' ', npt+1);
/*	indent[npt]   = ' '; */
	indent[npt+2] = '{';
	indent[npt+3] = '\0';

	for (i = 0; i < niobs; i++)
	    {ch = IOB(iob->children, i);
	     if (!PG_write_interface_object(fp, ch, indent, FALSE))
	        return(FALSE);
	     if (i < niobs - 1)
                PRINT(fp, "\n");
	     indent[npt+2]   = ' ';};

	PRINT(fp, "}");
        indent[npt+3] = ' ';
	indent[npt]   = '\0';};

    if (nopf)
       PRINT(fp, "\n");

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_INTERFACE - Write out an ASCII description of all the
 *                    - interface objects associated with device DEV
 *                    - to the file NAME.
 *                    - Return TRUE if successful and return FALSE otherwise.
 *
 * #bind PG_write_interface fortran() scheme() python()
 */

int PG_write_interface(PG_device *dev ARG(,,cls), const char *name)
   {int i, niobs, ret;
    char indent[MAXLINE];
    PG_interface_object *iob;
    FILE *fp;

    fp = _PG_fopen(name, "w");
    if (fp == NULL)
       return(FALSE);

    memset(indent, ' ', MAXLINE);

    niobs = SC_array_get_n(dev->iobjs);

/* print documentation */
    PRINT(fp, "#\n");
    PRINT(fp, "# PGS User Interface Object Description\n");
    PRINT(fp, "#\n");
    PRINT(fp, "# <type> [<oper>(<parameters>)]* BND(<param>)");
    PRINT(fp, " <boundary points>*\n");
    PRINT(fp, "#\n");

    PRINT(fp, "# Operators:\n");
    PRINT(fp, "# NAME - object name\n");
    PRINT(fp, "# FLG  - state flags\n");
    PRINT(fp, "# CLR  - fore and background colors\n");
    PRINT(fp, "# DRW  - names function that draws object when visible\n");
    PRINT(fp, "# ACT  - names function that does object action when active\n");
    PRINT(fp, "# SEL  - names function that identifies object as selected");
    PRINT(fp, " when selectable\n");
    PRINT(fp, "# BND  - boundary specifier\n");
    PRINT(fp, "#\n");

    PRINT(fp, "# FLG parameters\n");
    PRINT(fp, "#   IsVis - object is visible\n");
    PRINT(fp, "#   IsAct - object is active\n");
    PRINT(fp, "#   IsSel - object is selectable\n");
    PRINT(fp, "#\n");

    PRINT(fp, "# BND parameters\n");
    PRINT(fp, "#   <n>  - n NDC points to follow (x1,y1) ...");
    PRINT(fp, " (xn, yn)\n");
    PRINT(fp, "#   RECT - two NDC points to follow (x1,y1) (x2,y2)\n");
    PRINT(fp, "#\n");
    PRINT(fp, "# BND takes n points defining a boundary relative to the");
    PRINT(fp, " parent object limits\n");

    PRINT(fp, "# The descriptions of children of an object are indented");
    PRINT(fp, " relative to their parent\n");
    PRINT(fp, "# with 3 spaces but are otherwise just like any");
    PRINT(fp, " interface object\n");
    PRINT(fp, "#\n\n");

    ret = TRUE;
    indent[0] = '\0';
    for (i = 0; i < niobs; i++)
        {iob = IOB(dev->iobjs, i);
	 ret = PG_write_interface_object(fp, iob, indent, TRUE);
	 if (!ret)
	    break;
         PRINT(fp, "\n");};

    io_close(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_XY - pull an (x,y) value pair of the current token stream
 *            - return TRUE iff successful
 */

static int _PG_get_xy(double *px, double *py,
                      double xmn, double ymn, double dx, double dy,
                      char *ps)
   {char *tok;

    tok = SC_firsttok(ps, "(,) ");
    if (tok == NULL)
       return(FALSE);

    if (*tok == '*')
       return(FALSE);

    *px = xmn + dx*SC_stof(tok);

    tok = SC_firsttok(ps, "(,) ");
    if (tok == NULL)
       return(FALSE);

    *py = ymn + dy*SC_stof(tok);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIX_COLOR - parse the color spec and return the values */

static void _PG_fix_color(PG_device *dev, int *pfc, int *pbc, char *ps)
   {int fc, bc;

    fc = SC_stoi(SC_firsttok(ps, ","));
    if (fc == 0)
       fc = dev->BLACK;
    else if (fc == 1)
       fc = dev->WHITE;

    bc = SC_stoi(SC_firsttok(ps, ")"));
    if (bc == 0)
       bc = dev->BLACK;
    else if (bc == 1)
       bc = dev->WHITE;

    *pfc = fc;
    *pbc = bc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIX_FLAGS - parse the flags */

static void _PG_fix_flags(PG_device *dev, int *flags, char *ps)
   {char t[MAXLINE];
    char *s, *tok;

/* reset the flags if you come here */
    flags[0] = FALSE;
    flags[1] = FALSE;
    flags[2] = FALSE;

    s = SC_firsttok(ps, ")");
    SC_strncpy(t, MAXLINE, s, -1);

    while (TRUE)
       {tok = SC_firsttok(t, " ,");
	if (tok == NULL)
	   break;

	if (strcmp(tok, "IsVis") == 0)
	   flags[0] = TRUE;

	else if (strcmp(tok, "IsSel") == 0)
	   flags[1] = TRUE;

	else if (strcmp(tok, "IsAct") == 0)
	   flags[2] = TRUE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FIND_OBJECT - search the interface object tree in the device for
 *                - the named object
 */

PG_interface_object *PG_find_object(PG_device *dev, const char *s,
                                    PG_interface_object *parent)
   {int i, niobs;
    PG_interface_object *rv, *iob;

    rv = NULL;

    if (parent != NULL)
       {if (parent->name != NULL)
	        {if (strcmp(parent->name, s) == 0)
                    return(parent);};
	niobs = SC_array_get_n(parent->children);
	for (i = 0; i < niobs; i++)
	    {iob = IOB(parent->children, i);
	     if (iob != NULL)
	        {rv = PG_find_object(dev, s, iob);
		 if (rv != NULL)
		    break;};};}

    else
       {niobs = SC_array_get_n(dev->iobjs);
	for (i = 0; i < niobs; i++)
	    {iob = IOB(dev->iobjs, i);
	     if (iob != NULL)
	        {rv = PG_find_object(dev, s, iob);
		 if (rv != NULL)
		    break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIX_ACTION - fix the action and object of the action */

static void _PG_fix_action(PG_device *dev, char **pfct, void **pobj, char *ps)
   {char *fact, *item;
    void *obj;

    obj  = NULL;
    fact = SC_firsttok(ps, ")");
    if (fact != NULL)
       {item = strchr(fact, ',');
	if (item != NULL)
	   {*item++ = '\0';
	    obj = (void *) PG_find_object(dev, item,
					  (PG_interface_object *) NULL);};};

    *pfct = fact;
    *pobj = obj;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIX_BOUNDARY - process the boundary specfication */

static void _PG_fix_boundary(PG_device *dev, int *pnp, double **pxd, double **pyd,
                             double xmn, double ymn, double dx, double dy,
                             char *ps)
   {double *xd, *yd;
    double xs, ys;
    int i, npt, ok;
    char *tok;

    tok = SC_firsttok(ps, ")");
    if ((tok != NULL) && (strcmp(tok, "RECT") == 0))
       {npt = 5;
	xd  = CMAKE_N(double, npt);
	yd  = CMAKE_N(double, npt);
        ok  = FALSE;
	if (_PG_get_xy(&xs, &ys, xmn, ymn, dx, dy, ps))
           {xd[0] = xs;
            yd[0] = ys;
            yd[1] = ys;
            xd[3] = xs;
            xd[4] = xs;
            yd[4] = ys;
	    if (_PG_get_xy(&xs, &ys, xmn, ymn, dx, dy, ps))
               {xd[1] = xs;
                xd[2] = xs;
                yd[2] = ys;
                yd[3] = ys;
                ok    = TRUE;};};
        if (!ok)
           {CFREE(xd);
            CFREE(yd);
            npt = -1;};}

    else
       {npt = SC_stoi(tok);
	xd  = CMAKE_N(double, npt);
	yd  = CMAKE_N(double, npt);
	for (i = 0; i < npt; i++)
	    {if (!_PG_get_xy(&xd[i], &yd[i], xmn, ymn, dx, dy, ps))
                {CFREE(xd);
                 CFREE(yd);
                 npt = -1;
                 break;};};};

    *pnp = npt;
    *pxd = xd;
    *pyd = yd;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_READ_INTERFACE_OBJECT - read in an interface object from the
 *                           - given file
 *                           - return a PG_interface_object iff successful
 */

static PG_interface_object *_PG_read_interface_object(PG_device *dev, FILE *fp,
                                                      char *bf,
                                                      PG_interface_object *parent,
                                                      char *ps)
   {int npt, fc, bc;
    int flags[N_IOB_FLAGS];
    void *obj;
    double dx, dy;
    double ndc[PG_BOXSZ], xo[PG_SPACEDM];
    double *xd, *yd;
    double *r[PG_SPACEDM];
    char *tok, *type, *name;
    char *fsel, *fdrw, *fact;
    PG_interface_object *iob;
    PG_curve *crv;

    _PG_parent_limits(dev, parent, ndc);
    dx = ndc[1] - ndc[0];
    dy = ndc[3] - ndc[2];

    type = SC_firsttok(ps, " ");
    if (type == NULL)
       return(NULL);

    flags[0] = TRUE;
    flags[1] = TRUE;
    flags[2] = FALSE;

    fc   = 1;
    bc   = 0;
    npt  = -1;
    obj  = NULL;
    fsel = NULL;
    fdrw = NULL;
    fact = NULL;
    name = NULL;
    xd   = NULL;
    yd   = NULL;
    while (TRUE)
       {tok = SC_firsttok(ps, " ()\n");
        if (tok == NULL)
           {if (GETLN(bf, MAXLINE, fp) == NULL)
               return(NULL);
	    ps = bf;
            tok = SC_firsttok(ps, " ()\n");
            if (tok == NULL)
               return(NULL);};

        if (strcmp(tok, "FLG") == 0)
           _PG_fix_flags(dev, flags, ps);

        else if (strcmp(tok, "CLR") == 0)
           _PG_fix_color(dev, &fc, &bc, ps);

        else if (strcmp(tok, "NAME") == 0)
	   {if (ps[0] == '"')
	       name = SC_firsttok(ps, "\"");
	    else
	       name = SC_firsttok(ps, ")");}

        else if (strcmp(tok, "SEL") == 0)
           fsel = SC_firsttok(ps, ")");

        else if (strcmp(tok, "DRW") == 0)
           fdrw = SC_firsttok(ps, ")");

        else if (strcmp(tok, "ACT") == 0)
           _PG_fix_action(dev, &fact, &obj, ps);

        else if (strcmp(tok, "BND") == 0)
           {_PG_fix_boundary(dev, &npt, &xd, &yd, ndc[0], ndc[2], dx, dy, ps);
            break;}

        else
           break;};

    if (npt == -1)
       {PRINT(stdout, "Illegal boundary for %s (%s)\n", name, type);
        return(NULL);};

/* make Klocworks happy */
    npt = min(npt, 1000000);

    xo[0] = ndc[0];
    xo[1] = ndc[2];
    r[0]  = xd;
    r[1]  = yd;

    crv = PG_make_curve(dev, NORMC, TRUE, npt, xo, r);
    iob = PG_make_interface_object_n(dev, name, type, obj,
				     crv, flags, fc, bc,
				     NULL, fsel, fdrw, fact, parent);

    return(iob);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_READ_INTERFACE - Read in the descriptions of a set interface objects
 *                   - to associate with the device DEV from
 *                   - the file NAME.
 *                   - Return TRUE iff successful, and return FALSE otherwise.
 *
 * #bind PG_read_interface fortran() scheme() python()
 */

int PG_read_interface(PG_device *dev ARG(,,cls), const char *name)
   {int ns;
    PG_interface_object *iob, *piob, *parent;
    FILE *fp;
    char bf[MAXLINE];
    char *ps;

    fp = _PG_fopen(name, "r");
    if (fp == NULL)
       return(FALSE);

    iob    = NULL;
    piob   = NULL;
    parent = NULL;
    while (TRUE)
       {if (GETLN(bf, MAXLINE, fp) == NULL)
           break;

        if (SC_blankp(bf, "#"))
           continue;

        for (ns = 0, ps = bf; *ps == ' '; ns++, ps++);
        
        switch (*ps)
           {case '{' :
                 parent = piob;
                 ps++;
                 break;
            case '}' :
                 if (parent != NULL)
		    {for (parent = parent->parent;
			  *(++ps) == '}';
			  parent = parent->parent);};
                 break;};

        if (*ps == '\n')
           continue;

        iob = _PG_read_interface_object(dev, fp, bf, parent, ps);
        if (parent == NULL)
           SC_array_push(dev->iobjs, &iob);
        else
           {PG_PUSH_CHILD_IOB(parent, iob);};

        piob = iob;

        ps = SC_firsttok(ps, " \t\n\r");
        if (ps != NULL)
	   {switch (*ps)
	       {case '{' :
		     parent = piob;
		     ps++;
		     break;
		case '}' :
		     if (parent != NULL)
		        {for (parent = parent->parent;
			      *(++ps) == '}';
			      parent = parent->parent);};
		     break;};};};

    io_close(fp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                          DISPLAY TREE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* DPRITF - print out the interface descriptions to the terminal */

int dpritf(PG_device *dev)
   {int i, niobs, ret;
    char indent[MAXLINE];
    PG_interface_object *iob;

    memset(indent, ' ', MAXLINE);

    ret = TRUE;
    indent[0] = '\0';

    niobs = SC_array_get_n(dev->iobjs);
    for (i = 0; i < niobs; i++)
        {iob = IOB(dev->iobjs, i);
	 ret = PG_write_interface_object(stdout, iob, indent, TRUE);
	 if (!ret)
	    break;
         PRINT(stdout, "\n");};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
