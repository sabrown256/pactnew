/*
 * SHMM.C - memory management routines for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"
#include "scope_mem.h"

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
 SS_BOOLEAN_I     = 127,
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

/* SS_INSTALL - install procedure objects in the symbol table
 *            - a procedure object is a struct
 *            -      struct s_SS_proc
 *            -         {char type;
 *            -          char *doc;
 *            -          char *name;
 *            -          short int trace;
 *            -          object *proc;};
 *            -
 *            -      typedef struct s_SS_proc procedure;
 *            -
 *            - and the proc member points off to a C_procedure for
 *            - the purposes of SS_install
 *            -
 *            -      typedef object *(*PFPHand)(PFVoid pr, object *argl);
 *            - 
 *            -      struct s_SS_C_proc
 *            -         {object *(*handler)();
 *            -          PFVOid proc;}
 *            -
 *            -      typedef struct s_SS_C_proc C_procedure;
 *            -
 *            - NOTE: this is not a vararg function, we are using
 *            - varargs to defeat type checking on the function
 *            - pointers which is inappropriate for this general
 *            - purpose mechanism
 */

void SS_install(char* pname, char *pdoc, PFPHand phand, ...)
/*                PFVoid pproc, SS_form ptype */
   {object *op, *vp;
    procedure *pp;
    C_procedure *Cp;
    SS_form ptype;
    PFVoid pproc;

    SC_VA_START(phand);
    pproc = SC_VA_ARG(PFVoid);
    ptype = SC_VA_ARG(SS_form);
    SC_VA_END;

    pp = FMAKE(procedure, "SS_INSTALL:pp");
    if (pp == NULL)
       {PRINT(ERRDEV, "\nError installing procedure %s\n", pname);
        LONGJMP(SC_top_lev, ABORT);};

    Cp = FMAKE(C_procedure, "SS_INSTALL:Cp");
    if (Cp == NULL)
       {PRINT(ERRDEV, "\nError installing procedure %s\n", pname);
        LONGJMP(SC_top_lev, ABORT);};

    pp->doc = SC_strsavef(pdoc, "char*:SS_INSTALL:doc");
    if (pp->doc == NULL)
       {PRINT(ERRDEV, "\nError installing procedure documentation - %s\n",
                      pname);
        LONGJMP(SC_top_lev, ABORT);};

    pp->name = SC_strsavef(pname, "char*:SS_INSTALL:name");
    if (pp->name == NULL)
       {PRINT(ERRDEV, "\nError installing procedure name - %s\n", pname);
        LONGJMP(SC_top_lev, ABORT);};
        
    pp->trace = FALSE;
    pp->type  = ptype;
    pp->proc  = (object *) Cp;

    Cp->handler = phand;
    Cp->proc    = pproc;

    op     = SS_mk_proc_object(pp);
    SS_UNCOLLECT(op);

    vp     = SS_mk_variable(pname, op);
    SS_UNCOLLECT(vp);

    SC_hasharr_install(SS_symtab, pname, vp, SS_POBJECT_S, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/

/*                            DESTRUCTORS                                   */

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* _SS_RL_CHAR - release an char object */

static void _SS_rl_char(object *obj)
   {

    SFREE(SS_OBJECT(obj));
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

    SFREE(va);
    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SS_RL_INTEGER - release an integer object */

static void _SS_rl_integer(object *obj)
   {

    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_FLOAT - release a float object */

static void _SS_rl_float(object *obj)
   {

    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_COMPLEX - release a complex object */

static void _SS_rl_complex(object *obj)
   {

    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_QUATERNION - release a quaternion object */

static void _SS_rl_quaternion(object *obj)
   {

    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_STRING - release a string object */

static void _SS_rl_string(object *obj)
   {

    SFREE(SS_STRING_TEXT(obj));
    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_VARIABLE - release a variable object */

static void _SS_rl_variable(object *obj)
   {

    SFREE(SS_VARIABLE_NAME(obj));
    SFREE(SS_OBJECT(obj));
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
           {SFREE(lst);}

	else
	   {cdr = SS_cdr(lst);
	    car = SS_car(lst);

/* free the cons cell now that the car and cdr have been extracted */
	    cell = SS_OBJECT(lst); 
	    SFREE(cell);

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
	     SFREE(sp);

        case SS_PR_PROC  :
        case SS_UR_MACRO :
        case SS_EE_MACRO :
        case SS_UE_MACRO :
        case SS_ESC_PROC :
	     pr = pp->proc;
             SFREE(pr);
        default :
             break;};

/* release the name of the procedure */
    SFREE(pp->name);
    SFREE(pp);

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

    SFREE(name);
    SFREE(SS_OBJECT(obj));
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

    SFREE(name);
    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_BOOLEAN - release a boolean object */

static void _SS_rl_boolean(object *obj)
   {

    SFREE(SS_BOOLEAN_NAME(obj));
    SFREE(SS_OBJECT(obj));
    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RL_OBJECT - release a SCHEME object */

void SS_rl_object(object *obj)
   {

    if (_SS.trace_object == obj)
       PRINT(stdout, "free>  %p\n", obj);

    SFREE(obj);

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
       {SFREE(obj);}
    else
       SS_OBJECT_FREE(obj);

    return;}

/*--------------------------------------------------------------------------*/

/*                                MONITORS                                  */

/*--------------------------------------------------------------------------*/

/* SS_PR_OBJ_MAP - dump the list of objects and associated info */

object *SS_pr_obj_map(void)
   {

    return(SS_f);}

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

    lst = SS_vctlst(obj);
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

object *SS_mk_proc_object(procedure *pp)
   {object *op;

    op = SS_mk_object(pp, PROC_OBJ, SELF_EV, pp->name,
		      SS_wr_proc, _SS_rl_procedure);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_PROCEDURE - make-procedure returns a procedure
 *                 - the proc part of the procedure struct is a pointer
 *                 - to an S_procedure (compound or Scheme procedure)
 */

object *SS_mk_procedure(object *name, object *lam_exp, object *penv)
   {S_procedure *sp;
    procedure *pp;
    object *op;

    sp = FMAKE(S_procedure, "SS_MK_PROCEDURE:sp");

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

    pp = FMAKE(procedure, "SS_MK_PROCEDURE:pp");

    pp->type  = SS_PROC;
    pp->doc   = NULL;
    pp->name  = SC_strsavef("lambda", "char*:SS_MK_PROCEDURE:name");
    pp->trace = FALSE;
    pp->proc  = (object *) sp;

    op = SS_mk_proc_object(pp);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_ESC_PROC - make an escape procedure object */

object *SS_mk_esc_proc(int err, int type)
   {int cont, stck;
    procedure *pp;
    Esc_procedure *ep;
    object *op;

    cont = SS_cont_ptr;
    stck = SC_array_get_n(SS_stack) - 1;

    ep = FMAKE(Esc_procedure, "SS_MK_ESC_PROC:ep");
    ep->cont = cont;
    ep->stck = stck;
    ep->err  = err;
    ep->type = type;

    pp = FMAKE(procedure, "SS_MK_ESC_PROC:pp");
    pp->type = SS_ESC_PROC;
    pp->proc = (object *) ep;
    pp->name = SC_strsavef("escape", "char*:SS_MK_ESC_PROC:name");

    op = SS_mk_proc_object(pp);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_VARIABLE - encapsulate a VARIABLE in an object */

object *SS_mk_variable(char *n, object *v)
   {variable *vp;
    object *op;

    vp = FMAKE(variable, "SS_MK_VARIABLE:vp");
    vp->name  = SC_strsavef(n, "char*:SS_MK_VARIABLE:name");
    vp->value = v;

    op = SS_mk_object(vp, SS_VARIABLE_I, VAR_EV, vp->name,
		      SS_wr_atm, _SS_rl_variable);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_STRING - make a string and imbed it in an object */

object *SS_mk_string(char *s)
   {string *sp;
    object *op;

    if (s != NULL)
       {sp = FMAKE(string, "SS_MK_STRING:sp");
	sp->length = strlen(s);
	sp->string = SC_strsavef(s, "char*:SS_MK_STRING:string");

	op = SS_mk_object(sp, SC_STRING_I, SELF_EV, sp->string,
			  SS_wr_atm, _SS_rl_string);}
    else
       op = SS_null;

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_INPORT - make an object of type INPUT_PORT which encapsulates
 *              - a port struct with FILE pointer str
 */

object *SS_mk_inport(FILE *str, char *name)
   {input_port *pp;
    object *op;

    pp = FMAKE(input_port, "SS_MK_INPORT:pp");
    pp->name = SC_strsavef(name, "SS_MK_INPORT:name");
    pp->iln  = 1;
    pp->ichr = 1;
    pp->str  = str;
    pp->ptr  = pp->buffer;
    *pp->ptr = '\0';

    op = SS_mk_object(pp, SS_INPUT_PORT_I, SELF_EV, NULL,
		      _SS_wr_inport, _SS_rl_inport);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_OUTPORT - make an object of type OUTPUT_PORT which encapsulates
 *               - a port struct with FILE pointer str
 */

object *SS_mk_outport(FILE *str, char *name)
   {output_port *pp;
    object *op;

    if (str != NULL)
       SC_setbuf(str, NULL);

    pp = FMAKE(output_port, "SS_MK_OUTPORT:pp");
    pp->name = SC_strsavef(name, "SS_MK_OUTPORT:name");
    pp->str  = str;

    op = SS_mk_object(pp, SS_OUTPUT_PORT_I, SELF_EV, NULL,
		      _SS_wr_outport, _SS_rl_outport);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_INTEGER - make an integer object */

object *SS_mk_integer(BIGINT i)
   {BIGINT *lp;
    object *op;

    lp  = FMAKE(BIGINT, "SS_MK_INTEGER:lp");
    *lp = i;

    op = SS_mk_object(lp, SC_INT_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_integer);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_FLOAT - make an float object */

object *SS_mk_float(double d)
   {double *dp;
    object *op;

    dp  = FMAKE(double, "SS_MK_FLOAT:dp");
    *dp = d;

    op = SS_mk_object(dp, SC_FLOAT_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_float);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_COMPLEX - make an complex object */

object *SS_mk_complex(double _Complex d)
   {double _Complex *dp;
    object *op;

    dp  = FMAKE(double _Complex, "SS_MK_COMPLEX:dp");
    *dp = d;

    op = SS_mk_object(dp, SC_DOUBLE_COMPLEX_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_complex);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_QUATERNION - make an quaternion object */

object *SS_mk_quaternion(quaternion q)
   {quaternion *qp;
    object *op;

    qp  = FMAKE(quaternion, "SS_MK_QUATERNION:qp");
    *qp = q;

    op = SS_mk_object(qp, SC_QUATERNION_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_quaternion);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_BOOLEAN - encapsulate a BOOLEAN in an object */

object *SS_mk_boolean(char *s, int v)
   {SS_boolean *bp;
    object *op;

    bp = FMAKE(SS_boolean, "SS_MK_BOOLEAN:bp");
    bp->name  = SC_strsavef(s, "char*:SS_MK_BOOLEAN:name");
    bp->value = v;

    op = SS_mk_object(bp, SS_BOOLEAN_I, SELF_EV, bp->name,
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

object *SS_mk_cons(object *ca, object *cd)
   {cons *cp;
    object *op;

    cp = FMAKE(cons, "SS_MK_CONS:cp");
    SS_MARK(ca);
    SS_MARK(cd);
    cp->car = ca;
    cp->cdr = cd;

    op = SS_mk_object(cp, SS_CONS_I, PROC_EV, NULL,
		      SS_wr_lst, _SS_rl_cons);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_OBJECT_MAP - do a memory dump of all active objects
 *                - in the style of SC_mem_map
 */

int _SS_object_map(FILE *fp, int flag)
   {int i, j, n, ni, no, nox, nr, ityp;
    char s[MAXLINE];
    object *p, *q, *ca, *cd;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;
    obj_map *map;

    ph = _SC_tid_mm();

    no = 0;

    if (SC_LATEST_BLOCK(ph) != NULL)
       {n = SC_MAX_MEM_BLOCKS(ph) + BLOCKS_UNIT_DELTA;

	if (fp == NULL)
	   fp = stdout;

	nox = 10000;
	map = FMAKE_N(obj_map, nox, "_SS_OBJECT_MAP:map");

/* find all the relevant blocks */
	no   = 0;
	space = SC_LATEST_BLOCK(ph);
	for (i = 0; (i < n) && (space != NULL); space = desc->next, i++)
            {desc = &space->block;
	     nr   = REF_COUNT(desc);
	     ityp = BLOCK_TYPE(desc);
	     if ((ityp >= SS_OBJECT_I) && (nr != UNCOLLECT))
	        {map[no].indep = TRUE;
		 map[no].type  = ityp;
		 map[no].nref  = nr;
		 map[no].p     = (object *) (space + 1);
		 no++;
		 if (no >= nox)
		    {nox <<= 1;
		     REMAKE_N(map, obj_map, nox);};};

	     if (desc->next == SC_LATEST_BLOCK(ph))
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
		 SS_print(p, s, "\n", SS_outdev);};};

	SFREE(map);

	PRINT(fp, "\n");};

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_OBJECT - make a new object and initialize its garbage collection */

object *SS_mk_object(void *np, int type, SS_eval_mode evt, char *pname,
		     void (*print)(object *obj, object *strm),
		     void (*release)(object *obj))
   {object *op;
    SC_mem_opt opt;

    opt.na  = FALSE;
    opt.zsp = _SC_zero_space;
    opt.typ = SS_OBJECT_I;

    op = SC_alloc_nzt(1L, sizeof(object), "SS_MK_OBJECT:op", &opt);

    if ((pname != NULL) && (SC_arrlen(pname) < 1))
       pname = SC_strsavef(pname, "char*:SS_MK_OBJECT:pname");

    SC_arrtype(op, type);

    op->eval_type  = evt;
    op->print_name = pname;
    op->val        = np;
    op->print      = print;
    op->release    = release;

    if (_SS.trace_object == op)
       {PRINT(stdout, "alloc> %p\n", op);
        SS_print(op, "       ", "\n", SS_outdev);};

    return(op);}

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* SS_MK_CHAR - make character object */

object *SS_mk_char(int i)
   {int *ip;
    object *op;

    ip  = FMAKE(int, "SS_MK_CHAR:ip");
    *ip = i;

    op = SS_mk_object(ip, SS_CHARACTER_I, SELF_EV, NULL,
		      SS_wr_atm, _SS_rl_char);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_VECTOR - make a vector object */

object *SS_mk_vector(int l)
   {vector *vp;
    int i;
    object **va;
    object *op;

    vp = FMAKE(vector, "SS_MK_VECTOR:vp");
    va = FMAKE_N(object *, l, "SS_MK_VECTOR:va");
    for (i = 0; i < l; i++)
        va[i] = SS_null;

    vp->length = l;
    vp->vect   = va;

    op = SS_mk_object(vp, SS_VECTOR_I, SELF_EV, NULL,
		      _SS_wr_vector, _SS_rl_vector);

    return(op);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* SS_REGISTER_TYPES - register the types used by the interpreter */

void SS_register_types(void)
   {

    SC_init_base_types();

    SS_OBJECT_I      = SC_register_type("object",      sizeof(object),
					SS_rl_object);
    SS_PROCEDURE_I   = SC_register_type("procedure",   sizeof(procedure),
					_SS_rl_procedure);
    SS_CONS_I        = SC_register_type("pair",        sizeof(cons),
					_SS_rl_cons);
    SS_VARIABLE_I    = SC_register_type("variable",    sizeof(variable),
					_SS_rl_variable);
    SS_BOOLEAN_I     = SC_register_type("boolean",     sizeof(SS_boolean),
					_SS_rl_boolean);
    SS_INPUT_PORT_I  = SC_register_type("input port",  sizeof(input_port),
					_SS_rl_inport);
    SS_OUTPUT_PORT_I = SC_register_type("output port", sizeof(output_port),
					_SS_rl_outport);
    SS_EOF_I         = SC_register_type("eof",         sizeof(SS_boolean),
					_SS_rl_boolean);
    SS_NULL_I        = SC_register_type("nil",         sizeof(SS_boolean),
					_SS_rl_boolean);
    SS_ERROR_I       = SC_register_type("error",       0, NULL);

#ifdef LARGE

    SS_VECTOR_I      = SC_register_type("vector",       sizeof(vector),
					_SS_rl_vector);
    SS_CHARACTER_I   = SC_register_type("character",    sizeof(int),
					_SS_rl_char);
    SS_PROCESS_I     = SC_register_type("process",      sizeof(PROCESS),
					_SS_rl_process);

    SS_HASHARR_I     = SC_register_type("hash array",   sizeof(hasharr),
					NULL);
    SS_HAELEM_I      = SC_register_type("hash element", sizeof(haelem),
					NULL);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

