/*
 * SXHBO.C - handler for SS_binary operations between mappings
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

double
 SX_interp_scale    = 1.1,
 SX_interp_strength = 0.0,
 SX_interp_power    = 0.0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_BINARY_ARR - the binary array operator handler
 *                - all arrays required to be same size
 *                - first cut all arrays required to be same type
 */

static object *_SX_binary_arr(SS_psides *si, C_procedure *cp, object *argl)
   {int n, id;
    char v[MAX_PRSZ];
    char *otyp;
    PFVoid *proc;
    C_array *reta, *operand, *acc;
    object *obj, *al, *rv;

    proc = (PFVoid *) cp->proc;
    reta = NULL;

    if (SS_nullobjp(argl))
       return(SS_null);

/* figure out what size and type to make the accumulator */
    otyp = NULL;
    acc  = NULL;
    for (al = argl; SS_consp(al); al = SS_cdr(al))
        {obj = SS_car(al);
	 
	 if (SX_NUMERIC_ARRAYP(obj))
	    {otyp = NUMERIC_ARRAY_TYPE(obj);
	     n    = NUMERIC_ARRAY_LENGTH(obj);

	     if (otyp != NULL)
	        reta = PM_make_array(otyp, n, NULL);
	     break;};};

    if (otyp == NULL)
       SS_error("NO ARRAY - _SX_BINARY_ARR", argl);

/* accumulate the results */
    acc = NULL;
    for (al = argl; SS_consp(al); al = SS_cdr(al))
        {obj = SS_car(al);

	 id      = -1;
	 operand = NULL;
	 if (SX_NUMERIC_ARRAYP(obj))
	    operand = NUMERIC_ARRAY(obj);

	 else
	    {id = SC_arrtype(obj, -1);
	     _SS_object_to_numtype_id(id, v, 0, obj);};

	 acc = PM_accumulate_oper(proc, acc, operand, id, v);
	 if (acc == NULL)
	    SS_error("OBJECT DRIVEN FAILURE - _SX_BINARY_ARR", obj);};

    PM_conv_array(reta, acc, TRUE);

    rv = SX_mk_C_array(si, reta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_LINK_MAPPINGS - link the mappings from the arglist
 *                   - return a pointer to the first link in the list
 */

static PM_mapping *_SX_link_mappings(SS_psides *si, object *argl)
   {PM_mapping *h, *ph, *pn;

    h = NULL;
    while (SS_consp(argl))
       {SX_determine_mapping(si, &pn, &argl);
	if (pn == NULL)
	   continue;

        if (h == NULL)
           h = ph = pn;
        else
           {ph->next = pn;
            ph       = pn;};};

    if ((h != NULL) && (pn != NULL))
       pn->next = NULL;

    return(h);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_UNLINK_MAPPINGS - unlink the mappings */

static void _SX_unlink_mappings(PM_mapping *h)
   {PM_mapping *ph, *pn;

    for (ph = h; ph != NULL; ph = pn)
        {pn = ph->next;
         ph->next = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_HAVE_COMMON_DOMAIN - check the list of mappings for a common
 *                        - domain
 *                        - if they have a common domain
 *                        - return a copy of it
 */

static PM_set *_SX_have_common_domain(PM_mapping *h)
   {double tol;
    PM_set *a;
    PM_mapping *ph;

    a = h->domain;

    tol = 1.0e-12;

/* get the max indexes and the extremal values of each component */
    for (ph = h->next; ph != NULL; ph = ph->next)
        {if (!PM_set_equal(a, ph->domain, tol))
	    {a = NULL;
	     break;};};

    if (a != NULL)
       a = PM_copy_set(a);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_BUILD_COMMON_DOMAIN - given a list of mappings
 *                         - build a reasonable domain set
 *                         - which preserves the resolution
 *                         - as much as possible
 *                         - check that domains are commensurate
 *                         - check that ranges are commensurate
 */

static PM_set *_SX_build_common_domain(PM_mapping *h)
   {int i, nde, nd, sk, dk, snd;
    int *dmx, *smx;
    char bf[MAXLINE], *type, *s;
    double sx, sn, dx, dn;
    double *sdextr, *dextr;
    PM_set *sd, *sdm, *domain;
    PM_mapping *ph;

    if (h == NULL)
       return(NULL);

    sd  = h->domain;
    nd  = sd->dimension;
    nde = sd->dimension_elem;

/* get the number of bytes per component of a range element */
    dmx    = CMAKE_N(int, nd);
    dextr  = CMAKE_N(double, 2*nde);
    sdextr = CMAKE_N(double, 2*nde);
    for (i = 0; i < nde; i++)
        {dextr[2*i]   = HUGE;
         dextr[2*i+1] = -HUGE;};

/* get the max indexes and the extremal values of each component */
    for (ph = h; ph != NULL; ph = ph->next)
        {sdm = ph->domain;
	 snd = sdm->dimension;
	 smx = sdm->max_index;
         for (i = 0; i < nd; i++)
             {sk = *smx++;
              dk = dmx[i];
              dmx[i] = max(dk, sk);};

	 PM_array_real(sdm->element_type, sdm->extrema, 2*snd, sdextr);
         for (i = 0; i < nde; i++)
             {sn = sdextr[2*i];
              sx = sdextr[2*i+1];
              dn = dextr[2*i];
              dx = dextr[2*i+1];
              dextr[2*i]   = min(dn, sn);
              dextr[2*i+1] = max(dx, sx);};

/* detect things like (+ a a) */
         if (ph == ph->next)
            break;};

    strcpy(bf, sd->element_type);
    type = SC_strtok(bf, " *", s);
    domain = PM_make_lr_index_domain("Domain", type,
				     sd->dimension, sd->dimension_elem,
				     dmx, dextr, NULL);

    CFREE(dextr);
    CFREE(sdextr);

    return(domain);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ACC_LABEL - make a label for F which reflects the operands H */

static void _SX_acc_label(PM_mapping *f, PM_mapping *h)
   {char label[MAXLINE];
    PM_mapping *ph;
    SS_psides *si;

    si = &_SS_si;

    snprintf(label, MAXLINE, "(%s %s", SS_get_string(si->fun), h->name);

/* build up the label */
    for (ph = h->next; ph != NULL; ph = ph->next)
        {SC_vstrcat(label, MAXLINE, " %s", ph->name);

/* detect things like (+ a a) */
         if (ph == ph->next)
            break;};

    SC_strcat(label, MAXLINE, ")");

    CFREE(f->name);
    f->name = CSTRSAVE(label);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAP_LIST_LABEL - make a label for a mapping from the given list */

static void _SX_map_list_label(char *label, object *argl)
   {char t[MAXLINE];
    object *obj;
    SS_psides *si;

    si = &_SS_si;

    snprintf(label, MAXLINE, "(%s", SS_get_string(si->fun));

/* build up the label */
    for ( ; !SS_nullobjp(argl); argl = SS_cdr(argl))
        {obj = SS_car(argl);
	 snprintf(t, MAXLINE, "%s", SS_get_string(obj));

	 SC_vstrcat(label, MAXLINE, " %s", t);};

    SC_strcat(label, MAXLINE, ")");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INTERPOLATE_MAPPING - interpolate the range of the source mapping
 *                         - onto an intermediate array centered on the
 *                         - destination mapping
 *                         - return the intermediate array which is
 *                         - allocated in this routine
 *                         - NOTE: works very well until the destination
 *                         - mesh is about 4 times finer than the source
 *                         - when the relative granularity of the source
 *                         - leaks into the interpolated values
 */

static double **_SX_interpolate_mapping(PM_mapping *dest, PM_mapping *source,
					int wgtfl)
   {double prm[3];
    double **tre;

    prm[0] = SX_interp_scale;
    prm[1] = SX_interp_strength;
    prm[2] = SX_interp_power;

    tre = PM_interpolate_mapping(dest, source, wgtfl, prm);

    return(tre);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_EXTRACT_RANGE - extract the range data of the source mapping
 *                   - into an intermediate array centered on the
 *                   - destination mapping
 *                   - return the intermediate array which is
 *                   - allocated in this routine
 */

static double **_SX_extract_range(PM_mapping *dest, PM_mapping *source,
				  int wgtfl)
   {int i, id;
    int dne, snre, dnde;
    char *sty;
    double **sre, **s, **tre;
    double *sra, *trc;
    PM_set *sr, *sd, *dr, *dd;

    sd   = source->domain;
    sty  = sd->es_type;

    sr   = source->range;
    sre  = (double **) sr->elements;
    sty  = sr->es_type;
    snre = sr->n_elements;

    dd   = dest->domain;
    dne  = dd->n_elements;

    dr   = dest->range;
    dnde = dr->dimension_elem;

    s = PM_convert_vectors(dnde, dne, sre, sty);

    tre = PM_make_vectors(dnde, dne);

/* compute the weighted sums */
    for (i = 0; i < dnde; i++)
        {sra = s[i];
	 trc = tre[i];

	 for (id = 0; id < dne; id++)
	     {if (id >= snre)
		 SS_error("IMPROPER MAPPING - _SX_EXTRACT_RANGE", SS_null);

/* loop over neighborhood of id */
	      trc[id] = sra[id];};};

    PM_free_vectors(dnde, s);

/* just return the inverse weights */
    if (wgtfl)
       {for (i = 0; i < dnde; i++)
	    {trc = tre[i];
	     PM_set_value(trc, dne, 1.0);};};

    return(tre);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ACCUMULATE_MAPPING - interpolate the range of the source mapping
 *                        - onto an intermediate array centered on the
 *                        - destination mapping
 *                        - return the intermediate array which is
 *                        - allocated in this routine
 *                        - NOTE: works very well until the destination
 *                        - mesh is about 4 times finer than the source
 *                        - when the relative granularity of the source
 *                        - leaks into the interpolated values
 */

static double **_SX_accumulate_mapping(PM_mapping *dest, PM_mapping *source,
				       int wgtfl)
   {double tol;
    double **tre;
    PM_mapping *srcm;

    srcm = PM_node_centered_mapping(source);

    tol = 1.0e-12;

/* if the domains are the same just extract */
    if (PM_set_equal(srcm->domain, dest->domain, tol))
       tre = _SX_extract_range(dest, srcm, wgtfl);

/* otherwise interpolate */
    else
       tre = _SX_interpolate_mapping(dest, srcm, wgtfl);

    PM_rel_mapping(srcm, TRUE, TRUE);

    return(tre);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_BUILD_ACCUMULATOR_MAPPING - given a domain set
 *                               - initialize a range set to act as an
 *                               - accumulator and bind it with the domain
 *                               - to form a mapping which will be returned
 *                               - this checks that the ranges are
 *                               - commensurate
 *                               - WARNING: as its written it precludes
 *                               -          operations such as multiplying a
 *                               -          vector by a scalar (Hmmmm!)
 *                               - NOTE: this is independent of element
 *                               -       type!!!
 */

static PM_mapping *_SX_build_accumulator_mapping(PM_set *domain,
						 PM_mapping *h)
   {int i, nd, nde, nbe, tnd, tnde, ne;
    int *maxes, *dm;
    char label[MAXLINE];
    char *type, *lbl;
    void **elem;
    PM_set *range, *set;
    PM_mapping *f, *ph;

/* check that the ranges are commensurate */
    range = h->range;
    nd    = range->dimension;
    nde   = range->dimension_elem;
    ne    = range->n_elements;
    type  = range->element_type;
    for (ph = h->next; ph != NULL; ph = ph->next)
        {range = ph->range;
         tnd   = range->dimension;
         tnde  = range->dimension_elem;
         if ((tnd != nd) || (tnde != nde))
            SS_error("INCOMMENSURATE RANGE SET - _SX_BUILD_ACCUMULATOR_MAPPING",
                     SS_null);

/* detect things like (+ a a) */
         if (ph == ph->next)
            break;};

    range = h->range;

/* get the number of bytes per component of a range element */
    strcpy(label, range->es_type);
    PD_dereference(label);
    nbe = SIZEOF(label);

/* get the useful information from the domain */
    nd = domain->dimension;
    ne = domain->n_elements;
    dm = domain->max_index;

    maxes = CMAKE_N(int, nd);
    for (i = 0; i < nd; i++)
        maxes[i] = dm[i];

    nbe *= ne;
    elem = CMAKE_N(void *, nde);
    for (i = 0; i < nde; i++)
        elem[i] = CMAKE_N(char, nbe);

    set = PM_mk_set(range->name, type, FALSE,
		    ne, nd, range->dimension_elem,
		    maxes, elem, range->opers, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    lbl = SC_dsnprintf(FALSE, "%s", h->name);
    f   = PM_make_mapping(lbl, PM_LR_S, domain, set, N_CENT, NULL);

    _SX_acc_label(f, h);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INIT_RANGE - initialize the accumulator set */

static void _SX_init_range(PM_mapping *d, PM_mapping *s, object *obj,
			   int wgt)
   {int i, dnde, ne, ok;
    char *dty;
    double **tre;
    void **dre;
    C_array src, dst;
    PM_set *dr, *dd;

    dd = d->domain;
    ne = dd->n_elements;

    dr   = d->range;
    dnde = dr->dimension_elem;
    dre  = dr->elements;
    dty  = dr->es_type;

/* accumulate the results */
    tre = NULL;

    src.length = ne;
    src.type   = SC_DOUBLE_P_S;

    dst.length = ne;
    dst.type   = dty;

    if (s != NULL)
       tre = _SX_accumulate_mapping(d, s, wgt);

    for (i = 0; i < dnde; i++)
        {dst.data = dre[i];
	 if (tre != NULL)
	    src.data = tre[i];
	 else
	    src.data = NULL;

	 ok = PM_conv_array(&dst, &src, FALSE);
	 if (!ok)
	    SS_error("CAN'T INIT ACCUMULATOR - _SX_INIT_RANGE", obj);};

/* clean up the mess */
    if (tre != NULL)
       PM_free_vectors(dnde, tre);

    PM_find_extrema(dr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_BUILD_RETURN_MAPPING - given a domain set
 *                         - initialize a mapping in which to return
 *                         - the result of math operations
 */

PM_mapping *SX_build_return_mapping(PM_mapping *h, char *label,
				    PM_set *domain, int init, int wgt)
   {PM_mapping *f;

/* build the return mapping */
    if (domain == NULL)
       {domain = _SX_have_common_domain(h);
	if (domain == NULL)
	   domain = _SX_build_common_domain(h);};

    f = _SX_build_accumulator_mapping(domain, h);

    if (label != NULL)
       {CFREE(f->name);
	f->name = CSTRSAVE(label);};

    if (init)
       _SX_init_range(f, h, SS_null, wgt);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ACCUMULATE_RANGE - accumulate a range set into the accumulator set
 *                      - using the specified function
 */

static void _SX_accumulate_range(SS_psides *si, PM_mapping *d,
				 object *argl, PFVoid *proc)
   {int i, id, dnde, ne;
    char v[MAX_PRSZ];
    char *dty;
    double **tre;
    void **dre;
    C_array operand, **acc;
    PM_set *dr, *dd;
    PM_mapping *ps;
    object *obj, *al;

    dd = d->domain;
    ne = dd->n_elements;

    dr   = d->range;
    dnde = dr->dimension_elem;
    dre  = dr->elements;
    dty  = dr->es_type;

/* setup accumulators for each component */
    acc = CMAKE_N(C_array *, dnde);
    for (i = 0; i < dnde; i++)
        acc[i] = NULL;

    operand.type   = SC_DOUBLE_P_S;
    operand.length = ne;

/* accumulate the results */
    for (al = argl; SS_consp(al); )
        {tre = NULL;
	 id  = -1;

	 ps  = NULL;
	 obj = NULL;
	 SX_determine_drw_obj(si, &ps, &obj, &al);

	 if (ps != NULL)
	    tre = _SX_accumulate_mapping(d, ps, FALSE);

	 else
	    {id = SC_arrtype(obj, -1);
	     _SS_object_to_numtype_id(id, v, 0, obj);};

	 for (i = 0; i < dnde; i++)
	     {if (tre != NULL)
		 operand.data = tre[i];
	      else
		 operand.data = NULL;

	      acc[i] = PM_accumulate_oper(proc, acc[i], &operand, id, v);
	      if (acc[i] == NULL)
	         SS_error("OBJECT DRIVEN FAILURE - _SX_ACCUMULATE_RANGE",
			  obj);};

	 if (ps != NULL)

/* clean up the mess */
	    {PM_free_vectors(dnde, tre);

/* detect things like (+ a a) */
	     if (ps == ps->next)
	        break;};};

/* save accumulators for each component into the destination */
    operand.type   = dty;
    operand.length = ne;
    for (i = 0; i < dnde; i++)
        {operand.data = dre[i];
	 PM_conv_array(&operand, acc[i], TRUE);};

    CFREE(acc);

    PM_find_extrema(dr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MH_B_S - SX Math Handler for Binary Scalar operators
 *            - first cut will be to apply operator to scalar elements
 *            - of the range set of the given list of mappings
 */

object *_SX_mh_b_s(SS_psides *si, C_procedure *cp, object *argl)
   {int plf;
    char label[MAXLINE+1];
    PFVoid *proc;
    PM_mapping *f, *h;
    object *first, *mo;

    first = SS_car(argl);
    if (SS_floatp(first))
       mo = SS_binary_homogeneous(si, cp, argl);

    else if (SX_NUMERIC_ARRAYP(first))
       mo = _SX_binary_arr(si, cp, argl);

    else
       {proc = cp->proc;
	plf  = SX_have_display_list(si);

	_SX_map_list_label(label, argl);

/* build the return mapping */
	h = _SX_link_mappings(si, argl);
	f = SX_build_return_mapping(h, label, NULL, FALSE, FALSE);
	_SX_unlink_mappings(h);

	_SX_accumulate_range(si, f, argl, proc);

	SX_plot_flag = TRUE;

	mo = SX_mk_mapping(si, f);
	if (plf)
	   mo = SX_display_map(si, mo);};

    return(mo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_BUILD_RESTRICTED_DOMAIN - build a domain whose extent is the
 *                            - intersection of the list of values
 *                            - in ARGL and the extrema of HD
 */

PM_set *SX_build_restricted_domain(PM_set *hd, object *argl)
   {int i, nd, ne, nde;
    int *maxes, *dmx;
    char *name;
    double mn, mx, dn, dx, rd, rf;
    double *extr;
    PM_set *fd;

    nd  = hd->dimension;
    nde = hd->dimension_elem;
    dmx = hd->max_index;
    ne  = SS_length(argl);
    if (ne != 2*nd)
       SS_error("DOMAIN DIMENSION MISMATCH - SX_BUILD_RESTRICTED_DOMAIN",
		argl);

    extr  = CMAKE_N(double, ne);
    maxes = CMAKE_N(int, nd);

    PM_array_real(hd->element_type, hd->extrema, ne, extr);

    for (i = 0; i < nd; i++)
        {SS_args(argl,
		 SC_DOUBLE_I, &mn,
		 SC_DOUBLE_I, &mx,
		 0);
         argl = SS_cddr(argl);

	 dn = extr[2*i];
	 dx = extr[2*i+1];

	 rd  = dmx[i];
	 rf  = (mx - mn)/(dx - dn);
	 rd *= ABS(rf);
	 rd  = max(rd, 2);

	 maxes[i] = (int) rd;

	 dn = max(dn, mn);
	 dx = min(dx, mx);

	 extr[2*i]   = dn;
	 extr[2*i+1] = dx;};

    name = SC_dsnprintf(FALSE, "Sub %s", hd->name);
    fd   = PM_make_lr_index_domain(name, SC_DOUBLE_S, nd, nde,
				   maxes, extr, NULL);

    CFREE(extr);

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_EXTRACT_MAPPING - extract a mapping from the given mapping 
 *                      - from xstart to xstop by xstep
 */

PM_mapping *_SXI_extract_mapping(SS_psides *si, PM_mapping *h, object *argl)
   {int plf;
    char *lbl;
    PM_set *fd, *hd;
    PM_mapping *f;
    object *mo;

    plf = SX_have_display_list(si);

    hd = h->domain;

    lbl = SC_dsnprintf(FALSE, "Extract %s", h->name);

/* build the return mapping */
    fd = SX_build_restricted_domain(hd, argl);
    f  = SX_build_return_mapping(h, lbl, fd, TRUE, FALSE);

    PM_find_extrema(f->range);

    SX_plot_flag = TRUE;

    mo = SX_mk_mapping(si, f);
    if (plf)
       mo = SX_display_map(si, mo);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_BUILD_LR_DOMAIN - given a mapping and a specification for the
 *                    - size of a new domain
 *                    - build and return a new LR domain
 */

static PM_set *SX_build_lr_domain(PM_set *hd, object *argl)
   {int i, nc, nd, ne, nde;
    int *maxes;
    char name[MAXLINE];
    double *extr;
    PM_set *fd;

    nd  = hd->dimension;
    nde = hd->dimension_elem;
    ne  = SS_length(argl);
    if (ne != nd)
       SS_error("DOMAIN DIMENSION MISMATCH - SX_BUILD_LR_DOMAIN",
		argl);

    ne = 2*nd;

    extr  = CMAKE_N(double, ne);
    maxes = CMAKE_N(int, nd);

    PM_array_real(hd->element_type, hd->extrema, ne, extr);

    snprintf(name, MAXLINE, "LR (");
    for (i = 0; i < nd; i++)
        {SS_args(argl,
		 SC_INT_I, &maxes[i],
		 0);

	 nc = strlen(name);
	 snprintf(name+nc, MAXLINE-nc, "%d, ", maxes[i]);

         argl = SS_cdr(argl);};

    SC_NTH_LAST_CHAR(name, 1) = '\0';
    SC_strcat(name, MAXLINE, ")");

    fd = PM_make_lr_index_domain(name, SC_DOUBLE_S, nd, nde,
				 maxes, extr, NULL);

    CFREE(extr);

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_REFINE_MAPPING - make a new mapping on a LR mesh defined by the
 *                     - number of points implied by ARGL and
 *                     - interpolate the mapping H onto it
 */

PM_mapping *_SXI_refine_mapping(PM_mapping *h, object **pargl)
   {int plf;
    char *lbl;
    PM_set *fd, *hd;
    PM_mapping *f;
    object *argl, *obj;
    SS_psides *si = &_SS_si;

    plf = SX_have_display_list(si);
    SC_ASSERT(plf == TRUE);

    hd = h->domain;

    lbl = SC_dsnprintf(FALSE, "Refine(%s)", h->name);

    argl = *pargl;
    obj  = SS_car(argl);
    argl = SS_cdr(argl);
    *pargl = argl;

/* build the return mapping */
    fd = SX_build_lr_domain(hd, obj);
    f  = SX_build_return_mapping(h, lbl, fd, TRUE, FALSE);

    PM_find_extrema(f->range);

    SX_plot_flag = TRUE;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_INTERP_MAPPING - make a new mapping on a LR mesh defined by the
 *                     - number of points implied by ARGL and
 *                     - fill with the interpolation weights
 */

PM_mapping *_SXI_interp_mapping(PM_mapping *h, object **pargl)
   {int plf;
    char *lbl;
    PM_set *fd, *hd;
    PM_mapping *f;
    object *argl, *obj;
    SS_psides *si = &_SS_si;

    plf = SX_have_display_list(si);
    SC_ASSERT(plf == TRUE);

    hd = h->domain;

    lbl = SC_dsnprintf(FALSE, "Interpolate(%s)", h->name);

    argl = *pargl;
    obj  = SS_car(argl);
    argl = SS_cdr(argl);
    *pargl = argl;

/* build the return mapping */
    fd = SX_build_lr_domain(hd, obj);
    f  = SX_build_return_mapping(h, lbl, fd, TRUE, TRUE);

    PM_find_extrema(f->range);

    SX_plot_flag = TRUE;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
