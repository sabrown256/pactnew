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

int
 SS_OBJECT_I      = 123,
 SS_PROCEDURE_I   = 124,
 SS_CONS_I        = 124,
 SS_VARIABLE_I    = 125,
 SS_INPUT_PORT_I  = 128,
 SS_OUTPUT_PORT_I = 129,
 SS_EOF_I         = 130,
 SS_NULL_I        = 131,
 SS_VECTOR_I      = 132,
 SS_CHARACTER_I   = 133,
 SS_ERROR_I       = 140;

#ifdef LARGE

int
 SS_HAELEM_I      = 134,
 SS_HASHARR_I     = 135,
 SS_PROCESS_I     = 136;

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MK_C_PROC - instantiate a C_procedure */

C_procedure *_SS_mk_C_proc(PFPHand phand, int n, PFVoid *pr)
   {C_procedure *cp;

    cp = CMAKE(C_procedure);
    if (cp != NULL)
       {cp->handler = phand;
	cp->n       = n;
	cp->proc    = pr;};

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_C_PROC - release a C_procedure */

void _SS_rl_C_proc(C_procedure *cp)
   {

    CFREE(cp->proc);
    CFREE(cp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MK_C_PROC_VA - instantiate a C_procedure */

C_procedure *_SS_mk_C_proc_va(PFPHand phand, int n, ...)
   {int i;
    C_procedure *cp;
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

procedure *_SS_mk_scheme_proc(char *pname, char *pdoc, SS_form ptype,
			      C_procedure *cp)
   {char *ds, *ns;
    procedure *pp;

    pp = CMAKE(procedure);
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
 *             -      struct s_SS_proc
 *             -         {char type;
 *             -          char *doc;
 *             -          char *name;
 *             -          short int trace;
 *             -          object *proc;};
 *             -
 *             -      typedef struct s_SS_proc procedure;
 *             -
 *             - and the proc member points off to a C_procedure for
 *             - the purposes of _SS_install
 *             -
 *             -      typedef object *(*PFPHand)(PFVoid pr, object *argl);
 *             - 
 *             -      struct s_SS_C_proc
 *             -         {object *(*handler)();
 *             -          PFVOid proc;}
 *             -
 *             -      typedef struct s_SS_C_proc C_procedure;
 */

void _SS_install(char* pname, char *pdoc, PFPHand phand,
		 int n, PFVoid *pr, SS_form ptype)
   {object *op, *vp;
    procedure *pp;
    C_procedure *cp;
    SS_psides *si;

    si = &_SS_si;

/* create the C level procedure */
    cp = _SS_mk_C_proc(phand, n, pr);
    if (cp == NULL)
       {PRINT(ERRDEV, "\nError installing procedure %s\n", pname);
        LONGJMP(SC_gs.cpu, ABORT);};

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

    SC_hasharr_install(si->symtab, pname, vp, SS_POBJECT_S, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INSTALL - install procedure objects in the symbol table
 *            - NOTE: this is not a vararg function, we are using
 *            - varargs to defeat type checking on the function
 *            - pointers which is inappropriate for this general
 *            - purpose mechanism
 */

void SS_install(char* pname, char *pdoc, PFPHand phand, ...)
/*                PFVoid pproc, SS_form ptype */
   {SS_form ptype;
    PFVoid *pr;

    pr = CMAKE(PFVoid);

    SC_VA_START(phand);
    pr[0] = SC_VA_ARG(PFVoid);
    ptype = SC_VA_ARG(SS_form);
    SC_VA_END;

    _SS_install(pname, pdoc, phand, 1, pr, ptype);

    return;}

/*--------------------------------------------------------------------------*/

/*                            DESTRUCTORS                                   */

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SS_RL_CHAR - release an char object */

static void _SS_rl_char(object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_VECTOR - release a vector object */

static void _SS_rl_vector(object *obj)
   {int i, k;
    object **va;

    k  = SS_VECTOR_LENGTH(obj);
    va = SS_VECTOR_ARRAY(obj);

    for (i = 0; i < k; i++)
        {SS_Assign(va[i], SS_null);};

    CFREE(va);
    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SS_RL_INTEGER - release an integer object */

static void _SS_rl_integer(object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_FLOAT - release a float object */

static void _SS_rl_float(object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_COMPLEX - release a complex object */

static void _SS_rl_complex(object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_QUATERNION - release a quaternion object */

static void _SS_rl_quaternion(object *obj)
   {

    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_STRING - release a string object */

static void _SS_rl_string(object *obj)
   {

    CFREE(SS_STRING_TEXT(obj));
    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_VARIABLE - release a variable object */

static void _SS_rl_variable(object *obj)
   {

    CFREE(SS_VARIABLE_NAME(obj));
    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_CONS - release a CONS object
 *             - since this also SS_GC's lists be careful of
 *             - conses with multiple pointers
 */

static void _SS_rl_cons(object *obj)
   {int nr;
    object *lst, *cdr, *car;
    cons *cell;

    lst = obj;
    while (lst != NULL)
       {nr = SC_ref_count(lst);

/* if more than 1 reference simply reduce the reference count */
	if (nr > 1)
           {CFREE(lst);}

	else
	   {cdr = SS_cdr(lst);
	    car = SS_car(lst);

/* free the cons cell now that the car and cdr have been extracted */
	    cell = SS_OBJECT(lst); 
	    CFREE(cell);

/* free the object that wrapped the cell */
	    SS_rl_object(lst);

/* deal with the car */
	    SS_GC(car);

/* deal with the cdr */
	    if (SS_nullobjp(cdr) || !SS_consp(cdr))
	       {SS_GC(cdr);
		lst = NULL;}
	    else
	       lst = cdr;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_PROCEDURE - release a procedure object */

static void _SS_rl_procedure(object *obj)
   {object *pr;
    S_procedure *sp;
    procedure *pp;

    pp = SS_GET(procedure, obj);

    switch (pp->type)

/* since the environment part of the procedure was not SS_MARKed at the
 * time the procedure was made because of the circularity of the situation 
 * GC the name and lambda only
 */
       {case SS_MACRO :
        case SS_PROC  :
	     sp = SS_COMPOUND_PROCEDURE(obj);
	     SS_GC(sp->name);
	     SS_GC(sp->lambda);
/*	     SS_GC(sp->env); */
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
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_INPORT - release an object of type INPUT_PORT which encapsulates
 *               - a port struct with FILE pointer str
 */

static void _SS_rl_inport(object *obj)
   {char *name;

    name = SS_IFILE_NAME(obj);

    CFREE(name);
    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_OUTPORT - release an object of type OUTPUT_PORT which encapsulates
 *                - a port struct with FILE pointer str
 */

static void _SS_rl_outport(object *obj)
   {char *name;

    name = SS_OFILE_NAME(obj);

    CFREE(name);
    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_BOOLEAN - release a boolean object */

static void _SS_rl_boolean(object *obj)
   {

    CFREE(SS_BOOLEAN_NAME(obj));
    CFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RL_OBJECT - release a SCHEME object */

void SS_rl_object(object *obj)
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

void SS_gc(object *obj)
   {

    if (obj == NULL)
       return;

    if ((SS_OBJECT_GC(obj) != 1) || (obj->val == NULL))
       {CFREE(obj);}
    else
       SS_OBJECT_FREE(obj);

    return;}

/*--------------------------------------------------------------------------*/

/*                                MONITORS                                  */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*                                PRINTERS                                  */

/*--------------------------------------------------------------------------*/

/* _SS_WR_INPORT - print an input port */

static void _SS_wr_inport(object *obj, object *strm)
   {long loc;
    input_port *ip;
    FILE *fp;

    ip  = SS_GET(input_port, obj);
    loc = ip->ptr - ip->buffer;
    fp  = SS_OUTSTREAM(strm);

    PRINT(fp, "<INPUT_PORT|%s|%ld>", ip->name, loc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_WR_OUTPORT - print an output port */

static void _SS_wr_outport(object *obj, object *strm)
   {output_port *op;
    FILE *fp;

    op = SS_GET(output_port, obj);
    fp = SS_OUTSTREAM(strm);


    PRINT(fp, "<OUTPUT_PORT|%s>", op->name);

    return;}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SS_WR_VECTOR - print a vector object */

static void _SS_wr_vector(object *obj, object *strm)
   {object *lst;
    SS_psides *si = &_SS_si;

    lst = SS_vctlst(si, obj);
    SS_MARK(lst);
    PRINT(SS_OUTSTREAM(strm), "#");
    SS_wr_lst(lst, strm);
    SS_GC(lst);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                            CONSTRUCTORS                                  */

/*--------------------------------------------------------------------------*/

/* SS_MK_PROC_OBJECT - encapsulate a procedure as an object */

object *SS_mk_proc_object(SS_psides *si, procedure *pp)
   {object *op;
    
    op = SS_mk_object(si, pp, PROC_OBJ, SELF_EV, pp->name,
		      SS_wr_proc, _SS_rl_procedure);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_PROCEDURE - make-procedure returns a procedure
 *                 - the proc part of the procedure struct is a pointer
 *                 - to an S_procedure (compound or Scheme procedure)
 */

object *SS_mk_procedure(SS_psides *si, object *name,
			object *lam_exp, object *penv)
   {S_procedure *sp;
    procedure *pp;
    object *op;

    sp = CMAKE(S_procedure);

/* it is a circular gc problem to have a procedure point
 * to the environment in which it is defined
 * such an environment can never be reclaimed
 */
    sp->name   = name;
    sp->lambda = lam_exp;
    sp->env    = penv;

    SS_MARK(sp->name);
    SS_MARK(sp->lambda);
/*    SS_MARK(sp->env); */

    pp = CMAKE(procedure);

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
    procedure *pp;
    Esc_procedure *ep;
    object *op;

    cont = si->cont_ptr;
    stck = SC_array_get_n(si->stack) - 1;

    ep = CMAKE(Esc_procedure);
    ep->cont = cont;
    ep->stck = stck;
    ep->err  = err;
    ep->type = type;

    pp = CMAKE(procedure);
    pp->type = SS_ESC_PROC;
    pp->proc = (object *) ep;
    pp->name = CSTRSAVE("escape");

    op = SS_mk_proc_object(si, pp);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_VARIABLE - encapsulate a VARIABLE in an object */

object *SS_mk_variable(SS_psides *si, char *n, object *v)
   {variable *vp;
    object *op;

    vp = CMAKE(variable);
    vp->name  = CSTRSAVE(n);
    vp->value = v;

    op = SS_mk_object(si, vp, SS_VARIABLE_I, VAR_EV, vp->name,
		      SS_wr_atm, _SS_rl_variable);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_STRING - make a string and imbed it in an object */

object *SS_mk_string(SS_psides *si, char *s)
   {string *sp;
    object *op;

    if (s != NULL)
       {sp = CMAKE(string);
	sp->length = strlen(s);
	sp->string = CSTRSAVE(s);

	op = SS_mk_object(si, sp, SC_STRING_I, SELF_EV, sp->string,
			  SS_wr_atm, _SS_rl_string);}
    else
       op = SS_null;

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_INPORT - make an object of type INPUT_PORT which encapsulates
 *              - a port struct with FILE pointer str
 */

object *SS_mk_inport(SS_psides *si, FILE *str, char *name)
   {input_port *pp;
    object *op;

    pp = CMAKE(input_port);
    pp->name = CSTRSAVE(name);
    pp->iln  = 1;
    pp->ichr = 1;
    pp->str  = str;
    pp->ptr  = pp->buffer;
    *pp->ptr = '\0';

    op = SS_mk_object(si, pp, SS_INPUT_PORT_I, SELF_EV, NULL,
		      _SS_wr_inport, _SS_rl_inport);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_OUTPORT - make an object of type OUTPUT_PORT which encapsulates
 *               - a port struct with FILE pointer str
 */

object *SS_mk_outport(SS_psides *si, FILE *str, char *name)
   {output_port *pp;
    object *op;

    if (str != NULL)
       SC_setbuf(str, NULL);

    pp = CMAKE(output_port);
    pp->name = CSTRSAVE(name);
    pp->str  = str;

    op = SS_mk_object(si, pp, SS_OUTPUT_PORT_I, SELF_EV, NULL,
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

    op = SS_mk_object(si, lp, SC_INT_I, SELF_EV, NULL,
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

    op = SS_mk_object(si, dp, SC_FLOAT_I, SELF_EV, NULL,
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

    op = SS_mk_object(si, dp, SC_DOUBLE_COMPLEX_I, SELF_EV, NULL,
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

    op = SS_mk_object(si, qp, SC_QUATERNION_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_quaternion);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_BOOLEAN - encapsulate a BOOLEAN in an object */

object *SS_mk_boolean(SS_psides *si, char *s, int v)
   {SS_boolean *bp;
    object *op;

    bp = CMAKE(SS_boolean);
    bp->name  = CSTRSAVE(s);
    bp->value = v;

    op = SS_mk_object(si, bp, SC_BOOL_I, SELF_EV, bp->name,
		      SS_wr_atm, _SS_rl_boolean);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_CONS - put the two args together into a new cons,
 *            - puts that into a new object, and
 *            - returns a pointer to it
 *            - marks both car and cdr to do the garbage collection
 *            - bookkeeping this is the C version of cons
 */

object *SS_mk_cons(SS_psides *si, object *ca, object *cd)
   {cons *cp;
    object *op;

    cp = CMAKE(cons);
    SS_MARK(ca);
    SS_MARK(cd);
    cp->car = ca;
    cp->cdr = cd;

    op = SS_mk_object(si, cp, SS_CONS_I, PROC_EV, NULL,
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
	     nr   = desc->ref_count;
	     ityp = desc->type;
	     if ((ityp >= SS_OBJECT_I) && (nr != UNCOLLECT))
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
	        {ca = SS_car(p);
		 cd = SS_cdr(p);
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
		 SS_print(p, s, "\n", si->outdev);};};

	CFREE(map);

	PRINT(fp, "\n");};

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_OBJECT - make a new object and initialize its garbage collection */

object *SS_mk_object(SS_psides *si,
		     void *np, int type, SS_eval_mode evt, char *pname,
		     void (*print)(object *obj, object *strm),
		     void (*release)(object *obj))
   {object *op;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = SS_OBJECT_I;
    opt.fnc  = __func__;
    opt.file = __FILE__;
    opt.line = __LINE__;

    op = _SC_alloc_n(1L, sizeof(object), &opt);

    if ((pname != NULL) && (SC_arrlen(pname) < 1))
       pname = CSTRSAVE(pname);

    SC_arrtype(op, type);

    op->eval_type  = evt;
    op->print_name = pname;
    op->val        = np;
    op->print      = print;
    op->release    = release;

    if (_SS.trace_object == op)
       {PRINT(stdout, "alloc> %p\n", op);
        SS_print(op, "       ", "\n", si->outdev);};

    return(op);}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* SS_MK_CHAR - make character object */

object *SS_mk_char(SS_psides *si, int i)
   {int *ip;
    object *op;

    ip  = CMAKE(int);
    *ip = i;

    op = SS_mk_object(si, ip, SS_CHARACTER_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_char);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_VECTOR - make a vector object */

object *SS_mk_vector(SS_psides *si, int l)
   {vector *vp;
    int i;
    object **va;
    object *op;

    vp = CMAKE(vector);
    va = CMAKE_N(object *, l);
    for (i = 0; i < l; i++)
        va[i] = SS_null;

    vp->length = l;
    vp->vect   = va;

    op = SS_mk_object(si, vp, SS_VECTOR_I, SELF_EV, NULL,
		      _SS_wr_vector, _SS_rl_vector);

    return(op);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* SS_REGISTER_TYPES - register the types used by the interpreter */

void SS_register_types(void)
   {

    SC_init_base_types();

    SS_OBJECT_I      = SC_type_register("object",      KIND_STRUCT, sizeof(object),
					SC_TYPE_FREE,  SS_rl_object,
					0);
    SS_PROCEDURE_I   = SC_type_register("procedure",   KIND_STRUCT, sizeof(procedure),
					SC_TYPE_FREE,  _SS_rl_procedure,
					0);
    SS_CONS_I        = SC_type_register("pair",        KIND_STRUCT, sizeof(cons),
					SC_TYPE_FREE,  _SS_rl_cons,
					0);
    SS_VARIABLE_I    = SC_type_register("variable",    KIND_STRUCT, sizeof(variable),
					SC_TYPE_FREE,  _SS_rl_variable,
					0);
    SS_INPUT_PORT_I  = SC_type_register("input port",  KIND_STRUCT, sizeof(input_port),
					SC_TYPE_FREE,  _SS_rl_inport,
					0);
    SS_OUTPUT_PORT_I = SC_type_register("output port", KIND_STRUCT, sizeof(output_port),
					SC_TYPE_FREE,  _SS_rl_outport,
					0);
    SS_EOF_I         = SC_type_register("eof",         KIND_STRUCT, sizeof(SS_boolean),
					SC_TYPE_FREE,  _SS_rl_boolean,
					0);
    SS_NULL_I        = SC_type_register("nil",         KIND_STRUCT, sizeof(SS_boolean),
					SC_TYPE_FREE,  _SS_rl_boolean,
					0);
    SS_ERROR_I       = SC_type_register("error",       KIND_OTHER,  0, 0);

#ifdef LARGE

    SS_VECTOR_I      = SC_type_register("vector",       KIND_STRUCT, sizeof(vector),
					SC_TYPE_FREE,   _SS_rl_vector,
					0);
    SS_CHARACTER_I   = SC_type_register("character",    KIND_OTHER,  sizeof(int),
					SC_TYPE_FREE,   _SS_rl_char,
					0);
    SS_PROCESS_I     = SC_type_register("process",      KIND_STRUCT, sizeof(PROCESS),
					SC_TYPE_FREE,   _SS_rl_process,
					0);

    SS_HASHARR_I     = SC_type_register("hash array",   KIND_STRUCT, sizeof(hasharr), 0);
    SS_HAELEM_I      = SC_type_register("hash element", KIND_STRUCT, sizeof(haelem),  0);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_GET_OBJECT_LENGTH - compute the number of items in a object */

int _SS_get_object_length(object *obj)
   {int ni, ityp;

    ityp = SC_arrtype(obj, -1);
    if (ityp == SS_CONS_I)
       ni = SS_length(obj);

    else if (ityp == SS_VECTOR_I)
       ni = SS_VECTOR_LENGTH(obj);

    else if (ityp == SC_STRING_I)
       ni = SS_STRING_LENGTH(obj) + 1;

    else if ((ityp == SC_INT_I) ||
	     (ityp == SC_FLOAT_I))
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
	SC_convert_id(SC_LONG_LONG_I, &v, 0, 1, id, p, n, 1, 1, FALSE);
	o = SS_mk_integer(si, v);}

/* fixed point types (proper) */
    else if (SC_is_type_fix(id) == TRUE)
       {long long v;
	SC_convert_id(SC_LONG_LONG_I, &v, 0, 1, id, p, n, 1, 1, FALSE);
	o = SS_mk_integer(si, v);}

/* floating point types (proper) */
    else if (SC_is_type_fp(id) == TRUE)
       {long double v;
	SC_convert_id(SC_LONG_DOUBLE_I, &v, 0, 1, id, p, n, 1, 1, FALSE);
	o = SS_mk_float(si, v);}

/* complex floating point types (proper) */
    else if (SC_is_type_cx(id) == TRUE)
       {long double _Complex v;
	SC_convert_id(SC_LONG_DOUBLE_COMPLEX_I, &v, 0, 1,
		      id, p, n, 1, 1, FALSE);
	o = SS_mk_complex(si, v);}

    else if (id == SC_BOOL_I)
       {bool *v;
	v = (bool *) p;
	o = SS_mk_boolean(si, "#boolean", v[n]);}

    else if (id == SC_QUATERNION_I)
       {quaternion *v;
	v = (quaternion *) p;
	o = SS_mk_quaternion(si, v[n]);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_NUMTYPE_TO_OBJECT - convert the Nth element of numeric array P
 *                       - and type TYPE to an object
 */

object *_SS_numtype_to_object(SS_psides *si, char *type, void *p, long n)
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
       SS_error("DATA TYPE NOT SUPPORTED - _SS_NUMTYPE_TO_LIST", SS_null);

    if (lst != SS_null)
       lst = SS_reverse(lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_NUMTYPE_TO_LIST - convert an array of N numerical elements P
 *                     - which has type TYPE into a list
 */

object *_SS_numtype_to_list(SS_psides *si, char *type, void *p, long n)
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

    num = ((SC_BOOL_I <= oid) && (oid <= SC_LONG_DOUBLE_I));

    if ((oid == SC_CHAR_I) || (oid == SS_CHARACTER_I))
       d = SS_CHARACTER_VALUE(val);
    else if (oid == SC_BOOL_I)
       d = SS_BOOLEAN_VALUE(val);
    else if (oid == SC_INT_I)
       d = SS_INTEGER_VALUE(val);
    else if (oid == SC_FLOAT_I)
       d = SS_FLOAT_VALUE(val);
    else
       d = 0.0;

    if (vid == SC_QUATERNION_I)
       {quaternion q, *pv;
	pv  = (quaternion *) p;
        q.i = 0.0;
        q.j = 0.0;
        q.k = 0.0;
	if (num == TRUE)
	   q.s = d;
        else if (oid == SC_DOUBLE_COMPLEX_I)
           {double _Complex z;
            z   = SS_COMPLEX_VALUE(val);
            q.s = creal(z);
            q.i = cimag(z);}
        else if (oid == SC_QUATERNION_I)
           q = SS_QUATERNION_VALUE(val);
        else
           rv = FALSE;
        if (rv == TRUE)
           pv[n] = q;}

    else if (SC_is_type_cx(vid) == TRUE)
       {double _Complex z;
	if (num == TRUE)
	   z = d;
        else if (oid == SC_DOUBLE_COMPLEX_I)
           z = SS_COMPLEX_VALUE(val);
        else if (oid == SC_QUATERNION_I)
           {quaternion q;
            q = SS_QUATERNION_VALUE(val);
            z = q.s + q.i*I;}
        else
           rv = FALSE;
        if (rv == TRUE)
	   SC_convert_id(vid, p, n, 1,
			 SC_DOUBLE_COMPLEX_I, &z, 0, 1, 1, FALSE);}

    else if (SC_is_type_num(vid) == TRUE)
       {if ((num == TRUE) || (oid == SS_CHARACTER_I))
	   SC_convert_id(vid, p, n, 1,
			 SC_LONG_DOUBLE_I, &d, 0, 1, 1, FALSE);

        else if (oid == SC_DOUBLE_COMPLEX_I)
           {double _Complex z;
            z = SS_COMPLEX_VALUE(val);
	    d = creal(z);
	    SC_convert_id(vid, p, n, 1,
			  SC_LONG_DOUBLE_I, &d, 0, 1, 1, FALSE);}

        else if (oid == SC_QUATERNION_I)
           {quaternion q;
            q = SS_QUATERNION_VALUE(val);
	    d = q.s;
	    SC_convert_id(vid, p, n, 1,
			  SC_LONG_DOUBLE_I, &d, 0, 1, 1, FALSE);}

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

int _SS_object_to_numtype(char *type, void *p, long n, object *val)
   {int vid, rv;

    vid = SC_type_id(type, FALSE);
    rv  = _SS_object_to_numtype_id(vid, p, n, val);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LIST_TO_NUMTYPE_ID - convert multiple objects to an array P which
 *                        - has type TYPE to the value of object VAL
 *                        - return TRUE iff successful
 */

int _SS_list_to_numtype_id(int vid, void *p, long n, object *o)
   {int ityp, rv;
    long no;
    char *msg;

    rv = TRUE;

/* print out the type */
    if (SC_is_type_char(vid) == TRUE)
       {ityp = SC_arrtype(o, -1);
	if (ityp == SC_STRING_I)
	   strncpy(p, SS_STRING_TEXT(o), n);
	else if (ityp == SS_CONS_I)
	   strncpy(p, SS_STRING_TEXT(SS_car(o)), n);
        else
	   SS_error("EXPECTED A STRING - _SS_LIST_TO_NUMTYPE", o);

        return(rv);};

/* only need to check non-char types 
 * NOTE: the '\0' terminator on char strings cause the following
 *       test to fail unnecessarily (DRS.SCM for example)
 */
    no = _SS_get_object_length(o);
    if (n < no)
       {msg = SC_dsnprintf(FALSE, "MORE DATA THAN EXPECTED %ld > %ld - _SS_LIST_TO_NUMTYPE",
			   no, n);
        SS_error(msg, o);};

    if (SC_is_type_num(vid) == TRUE)
       {int oid;
	long i;
	object *to, **ao;

	oid = SC_arrtype(o, -1);
	if (oid == SS_CONS_I)
	   {if (SS_consp(SS_car(o)))
	       o = SS_car(o);
	    for (i = 0; i < n; i++)
	        {to = SS_car(o);
		 _SS_object_to_numtype_id(vid, p, i, to);
		 to = SS_cdr(o);
		 if (to != SS_null)
		    o = to;};}

	else if (oid == SS_VECTOR_I)
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

/* _SS_LIST_TO_NUMTYPE - convert multiple objects to an array P which
 *                     - has type TYPE to the value of object VAL
 *                     - return TRUE iff successful
 */

int _SS_list_to_numtype(char *type, void *p, long n, object *o)
   {int vid, rv;

    vid = SC_type_id(type, FALSE);
    rv  = _SS_list_to_numtype_id(vid, p, n, o);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAX_NUMERIC_TYPE - return the type id of the 'largest' numeric
 *                      - type in ARGL
 *                      - return the length of the list via PN
 *                      - return -1 if non-numeric types are present
 */

int _SS_max_numeric_type(object *argl, long *pn)
   {int id, idx;
    long i;
    object *o;

    idx = -1;
    for (i = 0; !SS_nullobjp(argl); argl = SS_cdr(argl), i++)
        {o   = SS_car(argl);
	 id  = SC_arrtype(o, -1);
	 idx = max(idx, id);};

    if ((idx < SC_BOOL_I) || (SC_QUATERNION_I < idx))
       idx = -1;

    if (pn != NULL)
       *pn = i;

    return(idx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

