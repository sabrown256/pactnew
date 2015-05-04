/*
 * SXCRV.C - support for curve database in ULTRA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

#define NCURVE  100    /* number of additional curves to allocate at a time */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_CURVE_OBJ - return the specified curve object */

object *SX_get_curve_obj(int j)
   {object *o;

    o = _SX.crv_obj[j];

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_CURVE_PROC - return the specified curve procedure */

object *SX_get_curve_proc(int j)
   {object *o;

    o = _SX.crv_proc[j];

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_CURVE_VAR - return the specified curve variable */

object *SX_get_curve_var(int j)
   {object *o;

    o = _SX.crv_varbl[j];

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CURVE_ID - given the curve character return the integer identifier */

static int _SX_curve_id(const char *s)
   {int id, c, len, i;
    int alldigits = TRUE;

    id = -1;

    len = strlen(s);
    c   = s[0];

    if (c == '@')
       {for (i = 1; i < len; i++)
            {if (!isdigit((int) s[i]))
                alldigits = FALSE;}

/* should there be two cases here--one if numeric_data_id
 * is set and one not?
 */
         if (alldigits)
	    id = SC_stoi(s+1) - 1;}                

    else if (isalpha(c))
       {if (islower(c))
           id = toupper(c)-'A';
        else if (isupper(c))
           id = c - 'A';}

    id = max(id, 0);
    id = min(id, SX_gs.n_curves-1);

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_CURVE_ID - given the curve object return the integer identifier */

int SX_curve_id(object *c)
   {int id;

    id = _SX_curve_id(SS_get_string(c));

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_ENLARGE_DATASET - allocate or reallocate the arrays whose size
 *                    - depends on the number of curves when more curves
 *                    - are requested (in blocks of NCURVE)
 *                    - also adjust any places that reference the number
 *                    - of curves
 */

void SX_enlarge_dataset(SS_psides *si, PFVoid eval)
   {int i, nc;
    char s[10], s1[10], t[10];
    SS_procedure *pp;
    object *o, *v, *p;
    curve *pc;

    nc = SX_gs.n_curves;

    if (_SX.cproc == NULL)
       _SX.cproc = _SS_mk_C_proc_va(SS_sargs, 1, eval);

    if (nc == 0)
       {SX_gs.dataset    = CMAKE_N(curve, NCURVE);
        SX_gs.number     = CMAKE_N(int, NCURVE);
        SX_gs.data_index = CMAKE_N(int, NCURVE);
	_SX.crv_obj   = CMAKE_N(object *, NCURVE);
	_SX.crv_proc  = CMAKE_N(object *, NCURVE);
	_SX.crv_varbl = CMAKE_N(object *, NCURVE);

        SX_gs.n_curves += NCURVE;}

    else
       {SX_gs.n_curves += NCURVE;
        CREMAKE(SX_gs.dataset, curve, SX_gs.n_curves);
        CREMAKE(SX_gs.number, int, SX_gs.n_curves);
        CREMAKE(SX_gs.data_index, int, SX_gs.n_curves);
        CREMAKE(_SX.crv_obj, object *, SX_gs.n_curves);
        CREMAKE(_SX.crv_proc, object *, SX_gs.n_curves);
        CREMAKE(_SX.crv_varbl, object *, SX_gs.n_curves);};

/* initialize the new curves */
    for (i = nc; i < SX_gs.n_curves; i++)
        {pc = SX_gs.dataset + i;

	 pc->file_type = SC_NO_FILE;
         pc->text      = NULL;
	 pc->file_info = NULL;
	 pc->file      = NULL;

	 pc->id       = ' ';
         pc->obj      = (void *) SS_null;
         pc->n        = 0;
         pc->modified = FALSE;
         pc->info     = PG_set_line_info(NULL,
					 PLOT_CARTESIAN,
					 CARTESIAN_2D,
					 LINE_SOLID,
					 FALSE, 0, 0,
					 0, 0.0);
         SX_gs.number[i]           = -1;
	 SX_gs.data_index[i]       = -1;

/* initialize the curve reference variables, procedures, and objects */

         if ((i < 26) && (SX_gs.sm == SX_MODE_ULTRA))
            {s[0] = i + 'a';
	     s[1] = '\0';
	     t[0] = i + 'A';
	     t[1] = '\0';
             snprintf(s1, 10, "@%d", i + 1);}
         else
            {snprintf(s, 10, "@%d", i + 1);
             snprintf(t, 10, "@%d", i + 1);};

         o = SS_mk_variable(si, s, SS_null);
         SS_UNCOLLECT(o);

	 pp = _SS_mk_scheme_proc(t, NULL, SS_PR_PROC, _SX.cproc);

         p = SS_mk_proc_object(si, pp);
         SS_UNCOLLECT(p);

         v = SS_mk_variable(si, s, o);
         SS_UNCOLLECT(v);

         SS_GET(SS_variable, o)->value = v;

         _SX.crv_obj[i]   = o;
         _SX.crv_proc[i]  = p;
         _SX.crv_varbl[i] = v;

         if ((i < 26) && (SX_gs.sm == SX_MODE_ULTRA))
            {if (SC_hasharr_install(si->symtab, s1, o, G_OBJECT_S, 3, -1) == NULL)
                LONGJMP(SC_gs.cpu, ABORT);};

         if (SC_hasharr_install(si->symtab, s, o, G_OBJECT_S, 3, -1) == NULL)
            LONGJMP(SC_gs.cpu, ABORT);

         if (SC_hasharr_install(si->symtab, t, o, G_OBJECT_S, 3, -1) == NULL)
            LONGJMP(SC_gs.cpu, ABORT);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_ASSIGN_NEXT_ID - assign next available curve id  */

void SX_assign_next_id(SS_psides *si, int i, object *(*plt)(SS_psides *si))
   {int j;

    for (j = 0; j < SX_gs.n_curves; j++)
        {if (SX_gs.data_index[j] == -1)
	    {if (j >= 0)
	        {SX_gs.dataset[i].id = 'A' + j;
		 SX_gs.data_index[j] = i;};

	     return;};};

    if (plt != NULL)
       {plt(NULL);
	SS_error(si,
		   "ALL 26 CURVE ID'S IN USE - SX_ASSIGN_NEXT_ID",
		   SS_null);};

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_NEXT_SPACE - return the index for the next available curve data slot */

int SX_next_space(SS_psides *si)
   {int i;

    for (i = _SX.next_avail; i < SX_gs.n_curves; i++)
        if (SX_gs.dataset[i].n == 0)
           {_SX.next_avail = i + 1;
            return(i);};

    SX_enlarge_dataset(si, NULL);

    i = SX_next_space(si);

    return(i);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_DATA_INDEX - return the curve index
 *                   - this means an index in SX_gs.dataset (i.e. I) space
 */

int SX_get_data_index(const char *s)
   {int i, j;

    j = _SX_curve_id(s);
    i = SX_gs.data_index[j];

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_CRV_INDEX_I - return the curve index if the object is a curve
 *                    - that is currently visible
 *                    - this means an index in SX_gs.dataset (i.e. I) space
 *                    - otherwise return -1
 */

int SX_get_crv_index_i(object *obj)
   {int i;
    char *s;

    i = -1;

    if (SX_curvep_a(obj))
       {s = SS_get_string(obj);
	i = SX_get_data_index(s);};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_CRV_INDEX_J - return the index if the object is a curve that is
 *                    - currently visible
 *                    - this means an index in SX_gs.data_index (i.e. J) space
 *                    - otherwise return -1
 */

int SX_get_crv_index_j(object *obj)
   {int j;
    char *s;

    j = -1;

    if (SX_curvep_a(obj))
       {s = SS_get_string(obj);
	j = SX_get_curve_id(s);};

    return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_CURVE_ID - return the curve id
 *                 - this means an index in SX_gs.data_index (i.e. J) space
 */

int SX_get_curve_id(const char *s)
   {int j;

    j = _SX_curve_id(s);

    return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_ZERO_CURVE - clear out the entry in the data set */

void SX_zero_curve(int i)
   {curve *crv;

    crv = SX_gs.dataset + i;

    crv->id       = ' ';
    crv->obj      = (void *) SS_null;
    crv->n        = 0;
    crv->modified = FALSE;

    crv->info = PG_set_line_info(crv->info,
			         PLOT_CARTESIAN, 0, 0, 0,
			         0, 0, 0, 0.0);

    if (i < _SX.next_avail)
       _SX.next_avail = i;

    CFREE(crv->text);
    CFREE(crv->file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_CURVE_PROC - return a Scheme procedure object made from the
 *                  - specified curve
 *                  - also bind the variable whose name is the lower
 *                  - case version of the curve label (curve.id) to
 *                  - the new object
 */

object *SX_mk_curve_proc(int i)
   {int j, ndi;
    char s[MAXLINE];
    object *obj, *p;

    PG_get_attrs_glb(TRUE,
		     "numeric-data-id", &ndi,
		     NULL);

    if ((SX_gs.dataset[i].id >= 'A') && (SX_gs.dataset[i].id <= 'Z'))
       {s[0] = SX_gs.dataset[i].id;
        s[1] = '\0';}

    else if (ndi == TRUE)
       {snprintf(s, MAXLINE, "@%d", SX_gs.dataset[i].id);}

    else
       {snprintf(s, MAXLINE, "@%d", SX_gs.dataset[i].id - 'A' + 1);};
    
    j = _SX_curve_id(s);

    obj = SX_get_curve_obj(j);
    p   = SX_get_curve_proc(j);

    SS_PROCEDURE_DOC(p)    = SX_gs.dataset[i].text;
    SS_VARIABLE_VALUE(obj) = p;
    SS_VARIABLE_NAME(obj)  = SS_PROCEDURE_NAME(p);

    SX_gs.dataset[i].obj = (void *) obj;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_CURVE - the C level function that creates a new curve object */

object *SX_mk_curve(SS_psides *si, int na, double **x,
		    const char *label, const char *filename,
		    object *(*plt)(SS_psides *si))
   {int i, j, id, nd, ng;
    int lnc, lns;
    double lnw, tmp;
    double wc[PG_BOXSZ];
    double *xi[PG_SPACEDM];
    curve *ds;
    object *o;

    nd = 2;

    i = SX_next_space(si);
    SX_zero_curve(i);
    SX_assign_next_id(si, i, plt);

    ds = SX_gs.dataset + i;

    ds->text     = CSTRSAVE(label);
    ds->modified = FALSE;

    if (filename != NULL)
       ds->file = CSTRSAVE(filename);
    else
       ds->file = NULL;

    PG_box_init(2, wc, HUGE, -HUGE);

    ng = FALSE;

/* #pragma omp parallel for private(j, tmp) */
    for (id = 0; id < nd; id++)

/* find limits */
        {for (j = 0; j < na; j++)
	     {tmp          = x[id][j];
	      wc[2*id]     = (tmp < wc[2*id])     ? tmp : wc[2*id];
	      wc[2*id + 1] = (tmp > wc[2*id + 1]) ? tmp : wc[2*id + 1];};

/* allocate new arrays */
         xi[id] = CMAKE_N(double, na);
	 if (xi[id] == NULL)
	    ng = TRUE;

/* copy data */
	 else
	    {for (j = 0; j < na; j++)
	         xi[id][j] = x[id][j];

	     ds->x[id] = xi[id];};};

    if (ng == TRUE)
       SS_error(si, "OUT OF MEMORY - CREATE_CURVE", SS_null);

    PG_box_copy(2, ds->wc, wc);

    ds->n = na;

/* get the default line attributes */
    PG_get_attrs_glb(TRUE,
		     "line-style", &lns,
		     "line-width", &lnw,
		     NULL);
    lnc = SX_next_color(SX_gs.graphics_device);

    PG_set_line_info(ds->info, PLOT_CARTESIAN, CARTESIAN_2D,
		     lns, FALSE, 0, lnc, 0, lnw);

    o = SX_mk_curve_proc(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_RL_CURVE - release a curve object */

object *SX_rl_curve(int j)
   {int i;
    object *obj, *v;

    i = SX_gs.data_index[j];

/* release the curve data points */
    CFREE(SX_gs.dataset[i].x[0]);
    CFREE(SX_gs.dataset[i].x[1]);

/* make the curve object a simple variable for the benefit of objects
 * still pointing to it
 */
    obj = SX_get_curve_obj(j);
    v   = SX_get_curve_var(j);

    SS_VARIABLE_VALUE(obj) = v;
    SS_VARIABLE_NAME(obj)  = SS_VARIABLE_NAME(v);

/* re-initialize the curve and mark */
    SX_zero_curve(i);
    SX_gs.data_index[j] = -1;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SET_CRV_ID - change the data-id of curve I to ID
 *               - also handle the curve object procedure/variable
 *               - variables beginning with J are logical indeces
 *               - variables beginning with I are "physical" indeces
 *               - and must be derived from J type variables via
 *               - SX_gs.data_index
 */

object *SX_set_crv_id(int j, const char *id)
   {int i, ja;
    object *obj, *v;

/* get the correct entry in SX_gs.dataset for the
 * curve whose id is to be changed
 */
    i = SX_gs.data_index[j];

/* compute the index of the new id in SX_gs.data_index */
    if (id[0] == '@')
       ja = SC_stoi(id+1) - 1;
    else
       ja = id[0] - 'A';

    ja = max(ja, 0);
    ja = min(ja, SX_gs.n_curves-1);

/* sever the connection with the old id using J */
    obj = SX_get_curve_obj(j);
    v   = SX_get_curve_var(j);
    SS_VARIABLE_VALUE(obj) = v;
    SS_VARIABLE_NAME(obj)  = SS_VARIABLE_NAME(v);
    SX_gs.data_index[j]  = -1;

/* make the connection with the new id using JA */
    SX_gs.dataset[i].id  = ja + 'A';
    obj               = SX_mk_curve_proc(i);
    SX_gs.data_index[ja] = i;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_RE_ID - reassign curve id's in sequence with no skipping */

object *SX_re_id(SS_psides *si)
   {int i, j;
    char id[10];
    object *obj;

    i = 0;

    memset(id, 0, 10);

    for (j = 0; j < SX_gs.n_curves; j++)
        {if (SX_gs.data_index[j] != -1)
	    {id[0] = 'A' + i;
             if (j != i)
	        {obj = SX_set_crv_id(j, id);
		 SC_ASSERT(obj != NULL);};

             i++;};}

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_DATA_DOMAIN - return the union of all curve domains */

object *SX_get_data_domain(SS_psides *si, object *argl)
   {int i;
    double xmn, xmx, xt;
    object *ch, *s, *o;

    SX_prep_arg(si, argl);

    xmn = HUGE;
    xmx = -HUGE;
    for (ch = argl; !SS_nullobjp(ch); ch = SS_cdr(si, ch))
        {s = SS_car(si, ch);
	 if (SX_curvep_a(s))
	    {i   = SX_get_crv_index_i(s);
	     xt  = SX_gs.dataset[i].wc[0];
	     xmn = min(xmn, xt);
	     xt  = SX_gs.dataset[i].wc[1];
	     xmx = max(xmx, xt);};};

    o = SS_mk_cons(si, SS_mk_float(si, xmn), SS_mk_float(si, xmx));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_DATA_RANGE - return the union of all curve ranges */

object *SX_get_data_range(SS_psides *si, object *argl)
   {int i;
    double ymn, ymx, yt;
    object *ch, *s, *o;

    SX_prep_arg(si, argl);

    ymn = HUGE;
    ymx = -HUGE;
    for (ch = argl; !SS_nullobjp(ch); ch = SS_cdr(si, ch))
        {s = SS_car(si, ch);
	 if (SX_curvep_a(s))
	    {i   = SX_get_crv_index_i(s);
	     yt  = SX_gs.dataset[i].wc[2];
	     ymn = min(ymn, yt);
	     yt  = SX_gs.dataset[i].wc[3];
	     ymx = max(ymx, yt);};};

    o = SS_mk_cons(si, SS_mk_float(si, ymn), SS_mk_float(si, ymx));

    return(o);}

/*--------------------------------------------------------------------------*/

/*                          CURVE TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* SX_CURVEP - tests whether object is legitimate curve */

int SX_curvep(const char *s)
   {int i, j, rv;
        
    rv = FALSE;

    j = _SX_curve_id(s);
    if ((0 <= j) && (j < SX_gs.n_curves) && SC_is_print_char(*s, 4))
       {i = SX_gs.data_index[j];
        if ((-1 < i) && (i < SX_gs.n_curves))
           if (SX_gs.dataset[i].n > 0)
              rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_CURVEP_A - tests whether Scheme object is legitimate curve
 *             - regardless of whether or not the object is a procedure
 *             - return TRUE if it is an Ultra curve and
 *             - return FALSE otherwise
 */

int SX_curvep_a(object *obj)
   {int i, len, rv;
    char *s;
    
    if (SS_nullobjp(obj) || SS_consp(obj))
       return(FALSE);

    s   = SS_get_string(obj);
    len = strlen(s);

    rv = FALSE;

    if ((len == 1) && (islower((int) s[0]) || isupper((int) s[0])))
       rv = SX_curvep(s);

    else if ((len > 1) && (s[0] == '@'))
       {for (i = 1; i < len; i++)
            {if (!isdigit((int) s[i]))
                {rv = FALSE;
		 break;};};

	if (i >= len)
	   rv = SX_curvep(s);};

     return(rv);}
       
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_CURVEP_B - return TRUE iff the given object is a procedure whose
 *             - name is an active curve
 */

int SX_curvep_b(object *obj)
   {int rv;

    rv = FALSE;
    if (SS_procedurep(obj))
       {if (SX_curvep(SS_PROCEDURE_NAME(obj)))
           rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CURVEOBJP - Scheme predicate testing for Ultra curves */

object *_SXI_curveobjp(SS_psides *si, object *obj)
   {object *o;

    o = SX_curvep_a(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
