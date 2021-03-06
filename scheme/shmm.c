/*
 * SHMM.C - memory management routines for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

typedef struct s_obj_map obj_map;

struct s_obj_map
   {int indep;
    int type;
    int nref;
    object *p;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MK_C_PROC - instantiate a SS_C_procedure */

SS_C_procedure *_SS_mk_C_proc(PFPHand phand, int n, PFVoid *pr)
   {SS_C_procedure *cp;

    cp = CMAKE(SS_C_procedure);
    if (cp != NULL)
       {cp->handler = phand;
	cp->n       = n;
	cp->proc    = pr;};

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_C_PROC - release a SS_C_procedure */

void _SS_rl_C_proc(SS_C_procedure *cp)
   {

    CFREE(cp->proc);
    CFREE(cp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MK_C_PROC_VA - instantiate a SS_C_procedure */

SS_C_procedure *_SS_mk_C_proc_va(PFPHand phand, int n, ...)
   {int i;
    SS_C_procedure *cp;
    PFVoid *pr;

    pr = CMAKE_N(PFVoid, n);

    SC_VA_START(n);
    for (i = 0; i < n; i++)
        pr[i] = SC_VA_ARG(PFVoid);
    SC_VA_END;

    cp = _SS_mk_C_proc(phand, n, pr);

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MK_SCHEME_PROC - instantiate a scheme procedure */

SS_procedure *_SS_mk_scheme_proc(const char *pname, const char *pdoc,
				 SS_form ptype, SS_C_procedure *cp)
   {char *ds, *ns;
    SS_procedure *pp;

    pp = CMAKE(SS_procedure);
    if (pp != NULL)
       {ds = NULL;
	if (pdoc != NULL)
	   ds = CSTRSAVE(pdoc);

	ns = NULL;
	if (pname != NULL)
	   ns = CSTRSAVE(pname);

	pp->doc   = ds;
	pp->name  = ns;
	pp->trace = FALSE;
	pp->type  = ptype;
	pp->proc  = (object *) cp;};

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INSTALL - install procedure objects in the symbol table
 *             - a procedure object is a struct
 *             - and the proc member points off to a SS_C_procedure for
 *             - the purposes of _SS_install
 */

void _SS_install(SS_psides *si, const char *pname, const char *pdoc,
		 PFPHand phand,
		 int n, PFVoid *pr, SS_form ptype)
   {object *op, *vp;
    SS_procedure *pp;
    SS_C_procedure *cp;

/* create the C level procedure */
    cp = _SS_mk_C_proc(phand, n, pr);
    if (cp == NULL)
       {PRINT(ERRDEV, "\nError installing procedure %s\n", pname);
        LONGJMP(SC_gs.cpu, ABORT);};
    SS_UNCOLLECT(cp);

/* create the Scheme level procedure */
    pp = _SS_mk_scheme_proc(pname, pdoc, ptype, cp);
    if (pp == NULL)
       {PRINT(ERRDEV, "\nError installing procedure %s\n", pname);
        LONGJMP(SC_gs.cpu, ABORT);};

    op = SS_mk_proc_object(si, pp);
    SS_UNCOLLECT(op);

/* create the variable which puts it all together */
    vp = SS_mk_variable(si, pname, op);
    SS_UNCOLLECT(vp);

    SC_hasharr_install(si->symtab, pname, vp, G_OBJECT_P_S, 3, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INSTALL - install procedure objects in the symbol table
 *            - NOTE: this is not a vararg function, we are using
 *            - varargs to defeat type checking on the function
 *            - pointers which is inappropriate for this general
 *            - purpose mechanism
 */

void SS_install(SS_psides *si, const char *pname, const char *pdoc,
		PFPHand phand, ...)
/*                PFVoid pproc, SS_form ptype */
   {SS_form ptype;
    PFVoid *pr;

    pr = CMAKE(PFVoid);

    SC_VA_START(phand);
    pr[0] = SC_VA_ARG(PFVoid);
    ptype = SC_VA_ARG(SS_form);
    SC_VA_END;

    _SS_install(si, pname, pdoc, phand, 1, pr, ptype);

    return;}

/*--------------------------------------------------------------------------*/

/*                            DESTRUCTORS                                   */

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SS_RL_CHAR - release an char object */

static void _SS_rl_char(SS_psides *si, object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_VECTOR - release a vector object */

static void _SS_rl_vector(SS_psides *si, object *obj)
   {int i, k;
    object **va;

    k  = SS_VECTOR_LENGTH(obj);
    va = SS_VECTOR_ARRAY(obj);

    for (i = 0; i < k; i++)
        {SS_assign(si, va[i], SS_null);};

    CFREE(va);
    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SS_RL_INTEGER - release an integer object */

static void _SS_rl_integer(SS_psides *si, object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_FLOAT - release a float object */

static void _SS_rl_float(SS_psides *si, object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_COMPLEX - release a complex object */

static void _SS_rl_complex(SS_psides *si, object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_QUATERNION - release a quaternion object */

static void _SS_rl_quaternion(SS_psides *si, object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_STRING - release a string object */

static void _SS_rl_string(SS_psides *si, object *obj)
   {

    CFREE(SS_STRING_TEXT(obj));
    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_VARIABLE - release a variable object */

static void _SS_rl_variable(SS_psides *si, object *obj)
   {

    CFREE(SS_VARIABLE_NAME(obj));
    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_REFERENCE - release a reference object */

static void _SS_rl_reference(SS_psides *si, object *obj)
   {

    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_CONS - release a SS_CONS object
 *             - since this also SS_gc's lists be careful of
 *             - conses with multiple pointers
 */

static void _SS_rl_cons(SS_psides *si, object *obj)
   {int nr;
    object *lst, *cdr, *car;
    SS_cons *cell;

    lst = obj;
    while (lst != NULL)
       {nr = SC_ref_count(lst);

/* if more than 1 reference simply reduce the reference count */
	if (nr > 1)
           {CFREE(lst);}

	else
	   {cdr = SS_cdr(si, lst);
	    car = SS_car(si, lst);

/* free the cons cell now that the car and cdr have been extracted */
	    cell = SS_OBJECT(lst); 
	    CFREE(cell);

/* free the object that wrapped the cell */
	    SS_rl_object(si, lst);

/* deal with the car */
	    SS_gc(si, car);

/* deal with the cdr */
	    if (SS_nullobjp(cdr) || !SS_consp(cdr))
	       {SS_gc(si, cdr);
		lst = NULL;}
	    else
	       lst = cdr;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_PROCEDURE - release a procedure object */

static void _SS_rl_procedure(SS_psides *si, object *obj)
   {object *pr;
    SS_S_procedure *sp;
    SS_procedure *pp;

    pp = SS_GET(SS_procedure, obj);

    switch (pp->type)

/* since the environment part of the procedure was not SS_marked at the
 * time the procedure was made because of the circularity of the situation 
 * GC the name and lambda only
 */
       {case SS_MACRO :
        case SS_PROC  :
	     sp = SS_COMPOUND_PROCEDURE(obj);
	     SS_gc(si, sp->name);
	     SS_gc(si, sp->lambda);
/*	     SS_gc(si, sp->env); */
	     CFREE(sp);

        case SS_PR_PROC  :
        case SS_UR_MACRO :
        case SS_EE_MACRO :
        case SS_UE_MACRO :
        case SS_ESC_PROC :
	     pr = pp->proc;
             CFREE(pr);
        default :
             break;};

/* release the name of the procedure */
    CFREE(pp->name);
    CFREE(pp);

/* free the object wrapper of the procedure */
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_INPORT - release an object of type SS_INPUT_PORT which encapsulates
 *               - a port struct with FILE pointer str
 */

static void _SS_rl_inport(SS_psides *si, object *obj)
   {char *name;

    name = SS_IFILE_NAME(obj);

    CFREE(name);
    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_OUTPORT - release an object of type SS_OUTPUT_PORT which encapsulates
 *                - a port struct with FILE pointer str
 */

static void _SS_rl_outport(SS_psides *si, object *obj)
   {char *name;

    name = SS_OFILE_NAME(obj);

    CFREE(name);
    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_BOOLEAN - release a boolean object */

static void _SS_rl_boolean(SS_psides *si, object *obj)
   {

    CFREE(SS_BOOLEAN_NAME(obj));
    CFREE(SS_OBJECT(obj));
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RL_OBJECT - release a SCHEME object */

void SS_rl_object(SS_psides *si, object *obj)
   {

    if (_SS.trace_object == obj)
       PRINT(stdout, "free>  %p\n", obj);

    CFREE(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GC - garbage collection, incremental
 *       - each object has number of pointers to itself (except #t, #f, etc)
 *       - when the number of pointers is 1 then garbage collection means
 *       - freeing the space associated with the object
 *       - when the number is greater than 1 garbage collection means
 *       - decrementing the number of pointers to itself
 */

void SS_gc(SS_psides *si, object *obj)
   {

    if (obj == NULL)
       return;

    if ((SS_OBJECT_GC(obj) != 1) || (obj->val == NULL))
       {CFREE(obj);}
    else
       SS_OBJECT_FREE(si, obj);

    return;}

/*--------------------------------------------------------------------------*/

/*                                MONITORS                                  */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*                                PRINTERS                                  */

/*--------------------------------------------------------------------------*/

/* _SS_WR_INPORT - print an input port */

static void _SS_wr_inport(SS_psides *si, object *obj, object *strm)
   {long loc;
    SS_input_port *ip;
    FILE *fp;

    ip  = SS_GET(SS_input_port, obj);
    loc = ip->ptr - ip->buffer;
    fp  = SS_OUTSTREAM(strm);

    PRINT(fp, "<INPUT_PORT|%s|%ld>", ip->name, loc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_WR_OUTPORT - print an output port */

static void _SS_wr_outport(SS_psides *si, object *obj, object *strm)
   {SS_output_port *op;
    FILE *fp;

    op = SS_GET(SS_output_port, obj);
    fp = SS_OUTSTREAM(strm);


    PRINT(fp, "<OUTPUT_PORT|%s>", op->name);

    return;}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SS_WR_VECTOR - print a vector object */

static void _SS_wr_vector(SS_psides *si, object *obj, object *strm)
   {object *lst;

    lst = SS_vctlst(si, obj);
    SS_mark(lst);
    PRINT(SS_OUTSTREAM(strm), "#");
    SS_wr_lst(si, lst, strm);
    SS_gc(si, lst);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                            CONSTRUCTORS                                  */

/*--------------------------------------------------------------------------*/

/* SS_MK_PROC_OBJECT - encapsulate a procedure as an object */

object *SS_mk_proc_object(SS_psides *si, SS_procedure *pp)
   {object *op;
    
    op = SS_mk_object(si, pp, G_SS_PROCEDURE_I, SELF_EV, pp->name,
		      SS_wr_proc, _SS_rl_procedure);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_PROCEDURE - make-procedure returns a procedure
 *                 - the proc part of the procedure struct is a pointer
 *                 - to an SS_S_procedure (compound or Scheme procedure)
 */

object *SS_mk_procedure(SS_psides *si, object *name,
			object *lam_exp, object *penv)
   {SS_S_procedure *sp;
    SS_procedure *pp;
    object *op;

    sp = CMAKE(SS_S_procedure);

/* it is a circular gc problem to have a procedure point
 * to the environment in which it is defined
 * such an environment can never be reclaimed
 */
    sp->name   = name;
    sp->lambda = lam_exp;
    sp->env    = penv;

    SS_mark(sp->name);
    SS_mark(sp->lambda);
/*    SS_mark(sp->env); */

    pp = CMAKE(SS_procedure);

    pp->type  = SS_PROC;
    pp->doc   = NULL;
    pp->name  = CSTRSAVE("lambda");
    pp->trace = FALSE;
    pp->proc  = (object *) sp;

    op = SS_mk_proc_object(si, pp);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_ESC_PROC - make an escape procedure object */

object *SS_mk_esc_proc(SS_psides *si, int err, int type)
   {int cont, stck;
    SS_procedure *pp;
    SS_esc_procedure *ep;
    object *op;

    cont = si->cont_ptr;
    stck = SC_array_get_n(si->stack) - 1;

    ep = CMAKE(SS_esc_procedure);
    ep->cont = cont;
    ep->stck = stck;
    ep->err  = err;
    ep->type = type;

    pp = CMAKE(SS_procedure);
    pp->type = SS_ESC_PROC;
    pp->proc = (object *) ep;
    pp->name = CSTRSAVE("escape");

    op = SS_mk_proc_object(si, pp);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_VARIABLE - encapsulate an SS_VARIABLE in an object */

object *SS_mk_variable(SS_psides *si, const char *n, object *v)
   {SS_variable *vp;
    object *op;

    vp = CMAKE(SS_variable);
    vp->name  = CSTRSAVE(n);
    vp->value = v;

    op = SS_mk_object(si, vp, G_SS_VARIABLE_I, VAR_EV, vp->name,
		      SS_wr_atm, _SS_rl_variable);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_REFERENCE - encapsulate an SS_REFERENCE in an object */

object *SS_mk_reference(SS_psides *si, const char *n,
			PFREFGet get, PFREFSet set, void *a)
   {SS_reference *rp;
    object *op;

    rp = CMAKE(SS_reference);
    rp->name  = CSTRSAVE(n);
    rp->value = SS_null;
    rp->get   = get;
    rp->set   = set;
    rp->a     = a;

    op = SS_mk_object(si, rp, G_SS_REFERENCE_I, VAR_EV, rp->name,
		      SS_wr_atm, _SS_rl_reference);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_STRING - make an SS_string and imbed it in an object */

object *SS_mk_string(SS_psides *si, const char *s)
   {SS_string *sp;
    object *op;

    if (s != NULL)
       {sp = CMAKE(SS_string);
	sp->length = strlen(s);
	sp->string = CSTRSAVE(s);

	op = SS_mk_object(si, sp, G_STRING_I, SELF_EV, sp->string,
			  SS_wr_atm, _SS_rl_string);}
    else
       op = SS_null;

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_INPORT - make an object of type SS_INPUT_PORT which encapsulates
 *              - a port struct with FILE pointer str
 */

object *SS_mk_inport(SS_psides *si, FILE *str, const char *name)
   {SS_input_port *pp;
    object *op;

    pp = CMAKE(SS_input_port);
    pp->name = CSTRSAVE(name);
    pp->iln  = 1;
    pp->ichr = 1;
    pp->str  = str;
    pp->ptr  = pp->buffer;
    *pp->ptr = '\0';

    op = SS_mk_object(si, pp, G_SS_INPUT_PORT_I, SELF_EV, NULL,
		      _SS_wr_inport, _SS_rl_inport);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_OUTPORT - make an object of type SS_OUTPUT_PORT which encapsulates
 *               - a port struct with FILE pointer str
 */

object *SS_mk_outport(SS_psides *si, FILE *str, const char *name)
   {SS_output_port *pp;
    object *op;

    if (str != NULL)
       SC_setbuf(str, NULL);

    pp = CMAKE(SS_output_port);
    pp->name = CSTRSAVE(name);
    pp->str  = str;

    op = SS_mk_object(si, pp, G_SS_OUTPUT_PORT_I, SELF_EV, NULL,
		      _SS_wr_outport, _SS_rl_outport);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_INTEGER - make an integer object */

object *SS_mk_integer(SS_psides *si, int64_t i)
   {int64_t *lp;
    object *op;

    lp  = CMAKE(int64_t);
    *lp = i;

    op = SS_mk_object(si, lp, G_INT_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_integer);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_FLOAT - make an float object */

object *SS_mk_float(SS_psides *si, double d)
   {double *dp;
    object *op;

    dp  = CMAKE(double);
    *dp = d;

    op = SS_mk_object(si, dp, G_DOUBLE_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_float);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_COMPLEX - make an complex object */

object *SS_mk_complex(SS_psides *si, double _Complex d)
   {double _Complex *dp;
    object *op;

    dp  = CMAKE(double _Complex);
    *dp = d;

    op = SS_mk_object(si, dp, G_DOUBLE_COMPLEX_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_complex);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_QUATERNION - make an quaternion object */

object *SS_mk_quaternion(SS_psides *si, quaternion q)
   {quaternion *qp;
    object *op;

    qp  = CMAKE(quaternion);
    *qp = q;

    op = SS_mk_object(si, qp, G_QUATERNION_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_quaternion);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_BOOLEAN - encapsulate a BOOLEAN in an object */

object *SS_mk_boolean(SS_psides *si, const char *s, int v)
   {SS_boolean *bp;
    object *op;

    bp = CMAKE(SS_boolean);
    bp->name  = CSTRSAVE(s);
    bp->value = v;

    op = SS_mk_object(si, bp, G_BOOL_I, SELF_EV, bp->name,
		      SS_wr_atm, _SS_rl_boolean);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_CONS - put the two args together into a new SS_cons,
 *            - puts that into a new object, and
 *            - returns a pointer to it
 *            - marks both car and cdr to do the garbage collection
 *            - bookkeeping this is the C version of SS_cons
 */

object *SS_mk_cons(SS_psides *si, object *ca, object *cd)
   {SS_cons *cp;
    object *op;

    cp = CMAKE(SS_cons);
    SS_mark(ca);
    SS_mark(cd);
    cp->car = ca;
    cp->cdr = cd;

    op = SS_mk_object(si, cp, G_SS_CONS_I, PROC_EV, NULL,
		      SS_wr_lst, _SS_rl_cons);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_OBJECT_MAP - do a memory dump of all active objects
 *                - in the style of SC_mem_map
 */

int _SS_object_map(SS_psides *si, FILE *fp, int flag)
   {int i, j, n, ni, no, nox, nr, ityp;
    char s[MAXLINE];
    object *p, *q, *ca, *cd;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;
    mem_inf *info;
    obj_map *map;

    ph = _SC_tid_mm();

    no = 0;

    if (ph->latest_block != NULL)
       {n = ph->nx_mem_blocks + BLOCKS_UNIT_DELTA;

	if (fp == NULL)
	   fp = stdout;

	nox = 10000;
	map = CMAKE_N(obj_map, nox);

/* find all the relevant blocks */
	no   = 0;
	space = ph->latest_block;
	for (i = 0; (i < n) && (space != NULL); space = desc->next, i++)
            {desc = &space->block;
	     info = &desc->desc.info;
	     nr   = info->ref_count;
	     ityp = info->type;
	     if ((ityp >= G_OBJECT_I) && (nr != UNCOLLECT))
	        {map[no].indep = TRUE;
		 map[no].type  = ityp;
		 map[no].nref  = nr;
		 map[no].p     = (object *) (space + 1);
		 no++;
		 if (no >= nox)
		    {nox <<= 1;
		     CREMAKE(map, obj_map, nox);};};

	     if (desc->next == ph->latest_block)
	        break;};

/* mark all the objects as dependent if someone else points to them */
	for (i = 0; i < no; i++)
	    {p = map[i].p;
	     if (SS_consp(p))
	        {ca = SS_car(si, p);
		 cd = SS_cdr(si, p);
		 for (j = 0; j < no; j++)
		     {q = map[j].p;
		      if ((q == ca) || (q == cd))
			 map[j].indep = FALSE;};};};

/* print all of the independent objects */
	ni = 0;
	for (i = 0; i < no; i++)
            {if (map[i].indep)
	        {ni++;
		 p    = map[i].p;
		 ityp = map[i].type;
		 nr   = map[i].nref;
		 snprintf(s, MAXLINE, " %p  %3d %3d : ", p, ityp, nr);
		 SS_print(si, si->outdev, p, s, "\n");};};

	CFREE(map);

	PRINT(fp, "\n");};

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_OBJECT - make a new object and initialize its garbage collection */

object *SS_mk_object(SS_psides *si,
		     void *np, int type, SS_eval_mode evt, const char *pname,
		     void (*print)(SS_psides *si, object *obj, object *strm),
		     void (*release)(SS_psides *si, object *obj))
   {object *o;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = G_OBJECT_I;
    opt.where.pfunc = __func__;
    opt.where.pfile = __FILE__;
    opt.where.line  = __LINE__;

    o = _SC_ALLOC_N(1L, sizeof(object), &opt);

    if ((pname != NULL) && (SC_arrlen(pname) < 1))
       pname = CSTRSAVE(pname);

    SC_arrtype(o, type);

    o->eval_type  = evt;
    o->print_name = pname;
    o->val        = np;
    o->print      = print;
    o->release    = release;

    if (_SS.trace_object == o)
       {PRINT(stdout, "alloc> %p\n", o);
        SS_print(si, si->outdev, o, "       ", "\n");};

    return(o);}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* SS_MK_CHAR - make character object */

object *SS_mk_char(SS_psides *si, int i)
   {int *ip;
    object *o;

    ip  = CMAKE(int);
    *ip = i;

    o = SS_mk_object(si, ip, SS_CHARACTER_I, SELF_EV, NULL,
		     SS_wr_atm, _SS_rl_char);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_VECTOR - make a vector object */

object *SS_mk_vector(SS_psides *si, int l)
   {SS_vector *vp;
    int i;
    object **va;
    object *o;

    vp = CMAKE(SS_vector);
    va = CMAKE_N(object *, l);
    for (i = 0; i < l; i++)
        va[i] = SS_null;

    vp->length = l;
    vp->vect   = va;

    o = SS_mk_object(si, vp, G_SS_VECTOR_I, SELF_EV, NULL,
		     _SS_wr_vector, _SS_rl_vector);

    return(o);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* SS_REGISTER_TYPES - register the types used by the interpreter */

void SS_register_types(void)
   {

    ONCE_SAFE(TRUE, NULL)

    SC_init_base_types();

    G_register_score_types();
    G_register_scheme_types();

    SS_EOF_I   = SC_type_register("eof", KIND_STRUCT, B_F, sizeof(SS_boolean),
				  SC_TYPE_FREE, _SS_rl_boolean, 0);
    SS_NULL_I  = SC_type_register("nil", KIND_STRUCT, B_F, sizeof(SS_boolean),
				  SC_TYPE_FREE, _SS_rl_boolean, 0);
    SS_ERROR_I = SC_type_register("error", KIND_OTHER,  B_F, 0, 0);

#ifdef LARGE

    SS_CHARACTER_I = SC_type_register("character", KIND_OTHER, B_T,
				      sizeof(int),
				      SC_TYPE_FREE, _SS_rl_char, 0);
#endif

    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_GET_OBJECT_LENGTH - compute the number of items in a object */

int _SS_get_object_length(SS_psides *si, object *obj)
   {int ni, ityp;

    ityp = SC_arrtype(obj, -1);
    if (ityp == G_SS_CONS_I)
       ni = SS_length(si, obj);

    else if (ityp == G_SS_VECTOR_I)
       ni = SS_VECTOR_LENGTH(obj);

    else if (ityp == G_STRING_I)
       ni = SS_STRING_LENGTH(obj) + 1;

    else if ((ityp == G_INT_I) ||
	     (ityp == G_DOUBLE_I))
       ni = 1;

    else
       ni = 0;

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_NUMTYPE_TO_OBJECT_ID - convert the Nth element of numeric array P
 *                          - and type TYPE to an object
 */

object *_SS_numtype_to_object_id(SS_psides *si, int id, void *p, long n)
   {object *o;

/* character types (proper) */
    if (SC_is_type_char(id) == TRUE)
       {long long v;
	SC_convert_id(G_LONG_LONG_I, &v, 0, 1, id, p, n, 1, 1, FALSE);
	o = SS_mk_integer(si, v);}

/* fixed point types (proper) */
    else if (SC_is_type_fix(id) == TRUE)
       {long long v;
	SC_convert_id(G_LONG_LONG_I, &v, 0, 1, id, p, n, 1, 1, FALSE);
	o = SS_mk_integer(si, v);}

/* floating point types (proper) */
    else if (SC_is_type_fp(id) == TRUE)
       {long double v;
	SC_convert_id(G_LONG_DOUBLE_I, &v, 0, 1, id, p, n, 1, 1, FALSE);
	o = SS_mk_float(si, v);}

/* complex floating point types (proper) */
    else if (SC_is_type_cx(id) == TRUE)
       {long double _Complex v;
	SC_convert_id(G_LONG_DOUBLE_COMPLEX_I, &v, 0, 1,
		      id, p, n, 1, 1, FALSE);
	o = SS_mk_complex(si, v);}

    else if (id == G_BOOL_I)
       {bool *v;
	v = (bool *) p;
	o = SS_mk_boolean(si, "#boolean", v[n]);}

    else if (id == G_QUATERNION_I)
       {quaternion *v;
	v = (quaternion *) p;
	o = SS_mk_quaternion(si, v[n]);}

    else
       o = SS_null;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_NUMTYPE_TO_OBJECT - convert the Nth element of numeric array P
 *                       - and type TYPE to an object
 */

object *_SS_numtype_to_object(SS_psides *si, const char *type,
			      void *p, long n)
   {int id;
    object *o;

    id = SC_type_id(type, FALSE);
    o  = _SS_numtype_to_object_id(si, id, p, n);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_NUMTYPE_TO_LIST_ID - convert an array of N numerical elements P
 *                        - which has type TYPE into a list
 */

object *_SS_numtype_to_list_id(SS_psides *si, int id, void *p, long n)
   {long i;
    object *o, *lst;

    lst = SS_null;

    if (SC_is_type_num(id) == TRUE)
       {for (i = 0L; i < n; i++)
	    {o   = _SS_numtype_to_object_id(si, id, p, i);
	     lst = SS_mk_cons(si, o, lst);};}

    else
       SS_error(si, "DATA TYPE NOT SUPPORTED - _SS_NUMTYPE_TO_LIST", SS_null);

    if (lst != SS_null)
       lst = SS_reverse(si, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_NUMTYPE_TO_LIST - convert an array of N numerical elements P
 *                     - which has type TYPE into a list
 */

object *_SS_numtype_to_list(SS_psides *si, const char *type,
			    void *p, long n)
   {int id;
    object *lst;

    id  = SC_type_id(type, FALSE);
    lst = _SS_numtype_to_list_id(si, id, p, n);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_OBJECT_TO_NUMTYPE_ID - set the Nth element of array P which
 *                          - has type ID to the value of object VAL
 *                          - return TRUE iff successful
 */

int _SS_object_to_numtype_id(int vid, void *p, long n, object *val)
   {int oid, num, rv;
    long double d;

    rv  = TRUE;
    oid = SC_arrtype(val, -1);

    num = ((G_BOOL_I <= oid) && (oid <= G_LONG_DOUBLE_I));

    if ((oid == G_CHAR_I) || (oid == SS_CHARACTER_I))
       d = SS_CHARACTER_VALUE(val);
    else if (oid == G_BOOL_I)
       d = SS_BOOLEAN_VALUE(val);
    else if (oid == G_INT_I)
       d = SS_INTEGER_VALUE(val);
    else if (oid == G_DOUBLE_I)
       d = SS_FLOAT_VALUE(val);
    else
       d = 0.0;

    if (vid == G_QUATERNION_I)
       {quaternion q, *pv;
	pv  = (quaternion *) p;
        q.i = 0.0;
        q.j = 0.0;
        q.k = 0.0;
	if (num == TRUE)
	   q.s = d;
        else if (oid == G_DOUBLE_COMPLEX_I)
           {double _Complex z;
            z   = SS_COMPLEX_VALUE(val);
            q.s = creal(z);
            q.i = cimag(z);}
        else if (oid == G_QUATERNION_I)
           q = SS_QUATERNION_VALUE(val);
        else
           rv = FALSE;
        if (rv == TRUE)
           pv[n] = q;}

    else if (SC_is_type_cx(vid) == TRUE)
       {double _Complex z;
	if (num == TRUE)
	   z = d;
        else if (oid == G_DOUBLE_COMPLEX_I)
           z = SS_COMPLEX_VALUE(val);
        else if (oid == G_QUATERNION_I)
           {quaternion q;
            q = SS_QUATERNION_VALUE(val);
            z = q.s + q.i*I;}
        else
           rv = FALSE;
        if (rv == TRUE)
	   SC_convert_id(vid, p, n, 1,
			 G_DOUBLE_COMPLEX_I, &z, 0, 1, 1, FALSE);}

    else if (SC_is_type_num(vid) == TRUE)
       {if ((num == TRUE) || (oid == SS_CHARACTER_I))
	   SC_convert_id(vid, p, n, 1,
			 G_LONG_DOUBLE_I, &d, 0, 1, 1, FALSE);

        else if (oid == G_DOUBLE_COMPLEX_I)
           {double _Complex z;
            z = SS_COMPLEX_VALUE(val);
	    d = creal(z);
	    SC_convert_id(vid, p, n, 1,
			  G_LONG_DOUBLE_I, &d, 0, 1, 1, FALSE);}

        else if (oid == G_QUATERNION_I)
           {quaternion q;
            q = SS_QUATERNION_VALUE(val);
	    d = q.s;
	    SC_convert_id(vid, p, n, 1,
			  G_LONG_DOUBLE_I, &d, 0, 1, 1, FALSE);}

        else
           rv = FALSE;}

    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_OBJECT_TO_NUMTYPE - set the Nth element of array P which
 *                       - has type TYPE to the value of object VAL
 *                       - return TRUE iff successful
 */

int _SS_object_to_numtype(const char *type, void *p, long n, object *val)
   {int vid, rv;

    vid = SC_type_id(type, FALSE);
    rv  = _SS_object_to_numtype_id(vid, p, n, val);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LIST_TO_NUMTYPE_ID - convert a list of objects, O, to an array P which
 *                        - has type index VID and length N
 *                        - it is an error if the length of the list is
 *                        - greater than N
 *                        - return TRUE iff successful
 */

int _SS_list_to_numtype_id(SS_psides *si, int vid, void *p, long n, object *o)
   {int ityp, rv;
    long no;
    char *msg;

    rv = TRUE;

/* print out the type */
    if (SC_is_type_char(vid) == TRUE)
       {ityp = SC_arrtype(o, -1);
	if (ityp == G_STRING_I)
	   strncpy(p, SS_STRING_TEXT(o), n);
	else if (ityp == G_SS_CONS_I)
	   strncpy(p, SS_STRING_TEXT(SS_car(si, o)), n);
        else
	   SS_error(si, "EXPECTED A STRING - _SS_LIST_TO_NUMTYPE", o);

        return(rv);};

/* only need to check non-char types 
 * NOTE: the '\0' terminator on char strings cause the following
 *       test to fail unnecessarily (DRS.SCM for example)
 */
    no = _SS_get_object_length(si, o);
    if (n < no)
       {msg = SC_dsnprintf(FALSE,
			   "MORE DATA THAN EXPECTED %ld > %ld - _SS_LIST_TO_NUMTYPE",
			   no, n);
        SS_error(si, msg, o);};

    if (SC_is_type_num(vid) == TRUE)
       {int oid;
	long i;
	object *to, **ao;

	oid = SC_arrtype(o, -1);
	if (oid == G_SS_CONS_I)
	   {if (SS_consp(SS_car(si, o)))
	       o = SS_car(si, o);
	    for (i = 0; i < n; i++)
	        {to = SS_car(si, o);
		 _SS_object_to_numtype_id(vid, p, i, to);
		 to = SS_cdr(si, o);
		 if (to != SS_null)
		    o = to;};}

	else if (oid == G_SS_VECTOR_I)
	   {ao = SS_VECTOR_ARRAY(o);
	    for (i = 0; i < n; i++)
	        _SS_object_to_numtype_id(vid, p, i, ao[i]);}

	else
	   _SS_object_to_numtype_id(vid, p, 0, o);}

    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LIST_TO_NUMTYPE - convert a list of objects, O, to an array P which
 *                     - has type TYPE and length N
 *                     - return TRUE iff successful
 */

int _SS_list_to_numtype(SS_psides *si, const char *type,
			void *p, long n, object *o)
   {int vid, rv;

    vid = SC_type_id(type, FALSE);
    rv  = _SS_list_to_numtype_id(si, vid, p, n, o);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LIST_TYPE - scan list LST and each element has the same type
 *               - handy special case often used in numerical work
 *               - return an SC_type describing it
 *               - otherwise return NULL
 */

SC_type *_SS_list_type(SS_psides *si, object *lst)
   {int id, ok;
    object *o, *l;
    SC_type *td;

    td = NULL;

    if (SS_consp(lst) == TRUE)
       {o  = SS_CAR_MACRO(lst);
	id = SC_arrtype(o, -1);
	ok = TRUE;

	for (l = SS_CDR_MACRO(lst);
	     (ok == TRUE) && !SS_nullobjp(l);
	     l = SS_CDR_MACRO(l))
	    {o  = SS_CAR_MACRO(l);
	     ok = (id == SC_arrtype(o, -1));};

	if (ok == TRUE)
	   td = _SC_get_type_id(id);};

    return(td);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LIST_TO_ARRAY - convert a list of objects, O, to an array P which
 *                   - has type TYPE and length N
 *                   - return TRUE iff successful
 */

void *_SS_list_to_array(SS_psides *si, object *o)
   {int vid, rv;
    long n, bpi;
    void *p;
    SC_type *td;

    p  = NULL;
    n  = SS_length(si, o);
    td = _SS_list_type(si, o);
    if (td != NULL)
       {bpi = td->bpi;
	vid = td->id;
	p   = CMAKE_N(char, bpi*n);
	rv  = _SS_list_to_numtype_id(si, vid, p, n, o);
        if (rv == FALSE)
	   CFREE(p);};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_VECTOR_TYPE - scan vector VCT and each element has the same type
 *                 - handy special case often used in numerical work
 *                 - return an SC_type describing it
 *                 - otherwise return NULL
 */

SC_type *_SS_vector_type(SS_psides *si, object *vct)
   {int i, n, id, ok;
    object **va;
    SC_type *td;

    td = NULL;

    if (SS_vectorp(vct) == TRUE)
       {va = SS_VECTOR_ARRAY(vct);
	n  = SS_VECTOR_LENGTH(vct);

	id = SC_arrtype(va[0], -1);
	ok = TRUE;

	for (i = 0; (ok == TRUE) && (i < n); i++)
	    ok = (id == SC_arrtype(va[i], -1));

	if (ok == TRUE)
	   td = _SC_get_type_id(id);};

    return(td);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_VECTOR_TO_ARRAY - convert a vector of objects, O, to an array P which
 *                     - has type TYPE and length N
 *                     - return TRUE iff successful
 */

void *_SS_vector_to_array(SS_psides *si, object *vct)
   {int vid;
    long i, n, bpi;
    object **va;
    void *p;
    SC_type *td;

    p  = NULL;
    td = _SS_vector_type(si, vct);
    if (td != NULL)
       {bpi = td->bpi;
	vid = td->id;

	va  = SS_VECTOR_ARRAY(vct);
	n   = SS_VECTOR_LENGTH(vct);

        p   = CMAKE_N(char, bpi*n);
	for (i = 0; i < n; i++)
	    _SS_object_to_numtype_id(vid, p, i, va[i]);};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAX_NUMERIC_TYPE - return the type id of the 'largest' numeric
 *                      - type in ARGL
 *                      - return the length of the list via PN
 *                      - return -1 if non-numeric types are present
 */

int _SS_max_numeric_type(SS_psides *si, object *argl, long *pn)
   {int id, idx;
    long i;
    object *o;

    idx = -1;
    for (i = 0; !SS_nullobjp(argl); argl = SS_cdr(si, argl), i++)
        {o   = SS_car(si, argl);
	 id  = SC_arrtype(o, -1);
	 idx = max(idx, id);};

    if ((idx < G_BOOL_I) || (G_QUATERNION_I < idx))
       idx = -1;

    if (pn != NULL)
       *pn = i;

    return(idx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

