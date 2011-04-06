/*
 * SCCINF.C - infrastructure for C parsing
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scc_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_VAR_TYPE - find the type associated with a variable name */

char *CC_var_type(char *name)
   {int id, nd;
    char *type, *nm;
    decl *pd;

    type = NULL;

    nd = _CC.nd;
    for (id = 0; id < nd; id++)
        {pd = _CC.all[id];
	 nm = pd->name;
         if ((nm != NULL) && (strcmp(name, nm) == 0))
	    {type = pd->type;
	     break;};};

    return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_PRIMITIVES - make table of primitive types */

static void _CC_primitives(void)
   {

    if (_CC.primitives == NULL)
       {_CC.primitives = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

        SC_hasharr_install(_CC.primitives, "void",       NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "char",       NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "short",      NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "int",        NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "long",       NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "long long",  NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "float",      NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "double",     NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "_Complex",   NULL, "expr *", TRUE, TRUE);
        SC_hasharr_install(_CC.primitives, "_Imaginary", NULL, "expr *", TRUE, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_ADD_TYPE - add a new data type */

expr *CC_add_type(char *name, qtype k)
   {expr *t;

    if (_CC.types == NULL)
       {_CC.types = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);
	_CC_primitives();};

    if (name == NULL)
       t = CC_null;

    else
       {t           = CC_mk_type(name);
	t->declared = k;
        SC_hasharr_install(_CC.types, name, t, "expr *", TRUE, TRUE);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_ADD_CONST - add a new const expr */

expr *CC_add_const(expr *e)
   {char name[MAXLINE];
    static int i = 0;

    snprintf(name, MAXLINE, "const%d", i++);

    if (_CC.consts == NULL)
       _CC.consts = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

    SC_hasharr_install(_CC.consts, name, e, "expr *", TRUE, TRUE);

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_ADD_DEPEND - add an object to the depend table at parse time */

expr *CC_add_depend(char *name, qtype t)
   {char s[MAXLINE];
    expr *v;

    if (name != NULL)
       {if (_CC.depend == NULL)
	   _CC.depend = SC_make_hasharr(HSZSMALL, FALSE, SC_HA_NAME_KEY);

	v = (expr *) SC_hasharr_def_lookup(_CC.depend, name);
	if (v == NULL)
	   {switch (t)
	       {case CC_TYP :
		     SC_strncpy(s, MAXLINE, name, -1);
		     SC_trim_right(s, "* \t");
		     if (SC_hasharr_lookup(_CC.primitives, s) == NULL)
		        {v = CC_mk_type(s);
			 if (SC_hasharr_install(_CC.depend, s, v, "expr *", TRUE, TRUE) == NULL)
			    v = NULL;};
		     break;

	        case CC_VAR :
		     v = CC_mk_variable(CC_var_type(name), name, CC_UNDEF);
		     if (SC_hasharr_install(_CC.depend, name, v, "expr *", TRUE, TRUE) == NULL)
		        v = NULL;
		     break;

	        default :
		     break;};};};

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_REM_VARC - remove the specified variable */

void _CC_rem_varc(char *name)
   {

    SC_hasharr_remove(_CC.depend, name);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_LOOKUP_VARIABLE - lookup a variable for LEX */

expr *CC_lookup_variable(char *txt, qtype q, int verbose)
   {expr *e;

    e = NULL;
    if ((q != CC_STORAGE) && (q != CC_TYPEQ))
       e = CC_add_depend(txt, CC_VAR);
    else
       e = (expr *) SC_hasharr_def_lookup(_CC.types, txt);

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_LOOKUP_IDENTIFIER_C - lookup an identifier for LEX */

int CC_lookup_identifier_c(char *txt, expr **lval)
   {int type;
    expr *e;
    decl *pd;

    *lval = CC_f;
    pd    = _CC.cur;
    SC_ASSERT(pd != NULL);

    e = (expr *) SC_hasharr_def_lookup(_CC.types, txt);
    if (e != NULL)
       {*lval = e;
	type  = CC_c_tokens[0];}

    else if (txt[strlen(txt)-1] == '*')
       {*lval = CC_mk_type(txt);
	type  = CC_c_tokens[0];}

    else
       {*lval = CC_add_depend(txt, CC_VAR);
	type  = CC_c_tokens[1];};

    return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_COPY_EXPR - return a copy of E */

expr *CC_copy_expr(expr *e)
   {expr *c;

    c = CMAKE(expr);

    c->cat      = e->cat;
    c->declared = e->declared;
    c->type     = CSTRSAVE(e->type);
    c->name     = CSTRSAVE(e->name);
    c->ptr      = NULL;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_TYPE - wrap a type T */

expr *CC_mk_type(char *t)
   {char s[MAXLINE];
    expr *e;

    SC_strncpy(s, MAXLINE, t, -1);
    SC_squeeze_chars(s, "\n\r\t");

    e = CMAKE(expr);

    e->cat      = CC_TYPE;
    e->declared = CC_DEF;
    e->type     = CSTRSAVE(s);
    e->name     = NULL;
    e->ptr      = NULL;

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_VARIABLE - make a variable with NAME and TYPE */

expr *CC_mk_variable(char *type, char *name, qtype declared)
   {expr *e;

    e = CMAKE(expr);

    e->cat      = CC_VARIABLE;
    e->declared = declared;
    e->type     = CSTRSAVE(type);
    e->name     = CSTRSAVE(name);
    e->ptr      = NULL;

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_CHAR - wrap a character const S */

expr *CC_mk_char(char *s)
   {expr *e;

    e = CMAKE(expr);

    e->cat      = CC_CHAR;
    e->declared = CC_DEF;
    e->type     = NULL;
    e->name     = NULL;
    e->ptr      = CSTRSAVE(s);

    CC_add_const(e);

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_STRING - wrap a string S */

expr *CC_mk_string(char *s)
   {expr *e;

    e = CMAKE(expr);

    e->cat      = CC_STRING;
    e->declared = CC_DEF;
    e->type     = NULL;
    e->name     = NULL;
    e->ptr      = CSTRSAVE(s);

    CC_add_const(e);

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_INTEGER - make a integer from L */

expr *CC_mk_integer(int64_t l)
   {int64_t *v;
    expr *e;

    v = CMAKE(int64_t);
    e = CMAKE(expr);

    *v = l;

    e->cat      = CC_INT;
    e->declared = CC_DEF;
    e->type     = NULL;
    e->name     = NULL;
    e->ptr      = v;

    CC_add_const(e);

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_FLOAT - make a double from D */

expr *CC_mk_float(double d)
   {double *v;
    expr *e;

    v = CMAKE(double);
    e = CMAKE(expr);

    *v = d;

    e->cat      = CC_FLOAT;
    e->declared = CC_DEF;
    e->type     = NULL;
    e->name     = NULL;
    e->ptr      = v;

    CC_add_const(e);

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_VARIABLE_NAME - return the name of the variable E */

char *CC_variable_name(expr *e)
   {char *s;

    s = NULL;
    if (e->cat == CC_VARIABLE)
       s = e->name;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_COMPILE_FILE - compile a single file */

int CC_compile_file(char *name, char *cmp, char **v)
   {int rv;
    char inm[MAXLINE];
    FILE *fp;

    rv = CC_preprocess(name, cmp, v);

    snprintf(inm, MAXLINE, "%s.i", _CC.rloc.fname);

    fp = fopen(inm, "r");

    _CC.n_error = 0;

/* parse the preprocessed file declaration by declaration */
    while (rv == TRUE)
       rv = CC_parse(fp);

    if (_CC.check_decls == TRUE)
       rv = CC_check(cmp, v);
    else
       rv = CC_emit(cmp, v, TRUE);

    rv &= (_CC.n_error == 0);

    fclose(fp);

    CFREE(_CC.rloc.fname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_QUAL_DECL - record the declaration type qualifier */

expr *_CC_qual_decl(expr *e)
   {char *s;
    decl *pd;

    s  = _CC.lex_text;
    pd = _CC.cur;
    if (pd->qual == CC_UNDEF)
       {switch (*s)
           {case 'c' :
	         pd->qual = CC_CONST;
	         break;
            case 'v' :
	         pd->qual = CC_VOLATILE;
	         break;};};

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_STOR_DECL - record the declaration storage class */

expr *_CC_stor_decl(expr *e)
   {char *s;
    decl *pd;

    s  = _CC.lex_text;
    pd = _CC.cur;
    if (pd->stor == CC_UNDEF)
       {switch (*s)
           {case 'a' :
	         pd->stor = CC_AUTO;
	         break;
            case 'e' :
	         pd->stor = CC_EXTERN;
	         break;
            case 'r' :
	         pd->stor = CC_REGISTER;
	         break;
            case 's' :
	         pd->stor = CC_STATIC;
	         break;
            case 't' :
	         pd->stor = CC_TYPEDEF;
	         break;};};

    if (e == NULL)
       e = CC_mk_type(s);

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_DEF_TYPE - add a type to the type table */

void _CC_def_type(expr *e)
   {char *s;

    s = _CC.lex_text;
    CC_add_type(s, CC_DECL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_VAR_DEF - mark the variable E as CC_DEF if d is TRUE
 *             - and as CC_UNDEF otherwise
 */

int _CC_var_def(expr *e, int d)
   {

    if ((e != NULL) && (e->cat == CC_VARIABLE))
       {if (d == TRUE)
	   e->declared = CC_DEF;
        else
	   e->declared = CC_UNDEF;};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_NAME_DECL - record the declaration variable name
 *               - including function names
 */

expr *_CC_name_decl(expr *t, expr *v, qtype k)
   {int ok;
    char *nm;
    decl *pd;

    pd = _CC.cur;
    if (pd != NULL)
       {nm = pd->name;
	ok = (nm == NULL);

#if 0
	if ((k == CC_FNC) || (k == CC_VAR) || (k == CC_TYP))
#endif
	if ((k == CC_VAR) || (k == CC_TYP))
	   {pd->kind = k;
	    ok       = TRUE;}

	else if (k == CC_ARG)
	   {CC_add_depend(pd->type, CC_TYP);
	    pd->kind = k;
	    v->cat   = CC_PARAMETER;
	    ok       = FALSE;};

	ok &= (_CC.ncbr == 0);

	if ((ok == TRUE) && (v != NULL))
	   {if (k == CC_IDENT)
	       nm = "__identifier__";
	    else
	       nm = v->name;

	    pd->name = CSTRSAVE(nm);};

	_CC_var_def(v, TRUE);};

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_TYPE_DECL - record the declaration type spec */

expr *_CC_type_decl(expr *t)
   {qtype ct;
    char *s;
    decl *pd;
    expr *tc;

    if (_CC.nprn == 0)
       {if (t != NULL)
	   {if ((t->type == NULL) && (t->name != NULL))
	       {t->type = t->name;
		t->name = NULL;};

	    s = t->type;}
	else
	   s = _CC.lex_text;

	pd = _CC.cur;
	pd->dec = CC_DEF;
	if (strncmp(s, "enum", 4) == 0)
	   {pd->kind = CC_TYP;
	    pd->cat  = CC_ENUM;}

	else if (strncmp(s, "struct", 6) == 0)
	   {pd->kind = CC_TYP;
	    pd->cat  = CC_STRUCT;}

	else if (strncmp(s, "union", 5) == 0)
	   {pd->kind = CC_TYP;
	    pd->cat  = CC_UNION;}

	else
	   {ct = CC_PRIMITIVE;
	    tc = (expr *) SC_hasharr_def_lookup(_CC.types, s);
	    if (tc != NULL)
	       {if (SC_hasharr_lookup(_CC.primitives, tc->type) == NULL)
		   {ct = CC_DERIVED;
		    CC_add_depend(tc->type, CC_TYP);};};
	    pd->cat = ct;};

       if (pd->type != NULL)
	  {CFREE(pd->type);};

       pd->type = CSTRSAVE(s);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_ADD_VAR - add variable V to the list for the current declaration */

expr *_CC_add_var(expr *l, expr *v)
   {decl *pd;

    pd       = _CC.cur;
    pd->name = CSTRSAVE(v->name);

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_VAR_DECL - using T and V adjust a variable declaration */

expr *_CC_var_decl(expr *t, expr *v, qtype wh)
   {decl *pd;

    pd = _CC.cur;


    if (pd != NULL)
       {
/*
	_CC_type_decl(t);
*/
        _CC_name_decl(t, v, wh);

	if (t != NULL)
	   {CFREE(v->type);
	    v->type = CSTRSAVE(t->type);};

	if (pd->stor == CC_TYPEDEF)
	   v->cat = CC_TYPE;};

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_DER_DECL - using T and V adjust a derived type declaration */

expr *_CC_der_decl(expr *t, expr *v)
   {char s[MAXLINE];
    decl *pd;

    pd = _CC.cur;

    _CC_type_decl(t);

    pd->kind = CC_TYP;
    if (strspn(pd->type, " \t") == 0)
       {if (v->type != NULL)
	   snprintf(s, MAXLINE, "%s %s", pd->type, v->type);
	else if (v->name == NULL)
	   snprintf(s, MAXLINE, "%s anonymous", pd->type);
	else
	   snprintf(s, MAXLINE, "%s %s", pd->type, v->name);}
    else
       strncpy(s, pd->type, MAXLINE);

    if (v == CC_anon)
       v = CC_copy_expr(v);

    if (v->name != NULL)
       {CFREE(v->name);};

    v->declared = t->declared;
    v->cat      = t->cat;

    CFREE(v->type);

    v->type = CSTRSAVE(s);

    if (pd->stor == CC_TYPEDEF)
       v->cat = CC_TYPE;

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_MEM_DECL - using T and V adjust a derived type declaration */

expr *_CC_mem_decl(expr *t, expr *v)
   {decl *pd;

    pd = _CC.cur;
    SC_ASSERT(pd != NULL);

    v->cat = CC_MEMBER;

    CC_add_depend(t->type, CC_TYP);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_FNC_DECL - find the type associated with a variable name */

expr *_CC_fnc_decl(expr *e)
   {decl *pd;

    pd = _CC.cur;
    if (pd != NULL)
       {pd->kind = CC_FNC;
	pd->name = CSTRSAVE(e->name);};

    return(e);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_CLEAN_TEXT - cleanup text in front of variable declaration */

char *_CC_clean_text(char *t)
   {int nw, more;
    char *p, *ts;

    for (more = TRUE; more == TRUE; )
        {ts = t;

/* strip off preceding whitespace */
	 nw = strspn(t, " \t\n\r\f");
	 t += nw;

/* strip off # directives */
	 while (*t == '#')
	    {p = strchr(t, '\n');
	     if (p != NULL)
	        t = p + 1;};

	 more = (t != ts);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_CLEAN_VAR - cleanup text in front of variable declaration */

static void _CC_clean_var(decl *pd)
   {int nw;
    char *tx;
    expr *t;

    tx = _CC_clean_text(pd->text);

/* check primitive or derived */
    t = (expr *) SC_hasharr_def_lookup(_CC.types, pd->type);
    if (t != NULL)
       {if (SC_hasharr_lookup(_CC.primitives, t->type) == NULL)
	   pd->cat = CC_DERIVED;
	else
	   pd->cat = CC_PRIMITIVE;};

/* check storage class */
    if (strncmp(tx, "static", 6) == 0)
       {tx += 6;
	nw  = strspn(tx, " \t\n\r\f");
	tx += nw;

	pd->dec = CC_DEF;}

    else if (pd->stor == CC_EXTERN)
       pd->dec = CC_DECL;

    else
       pd->dec = CC_DEF;

    if (pd->stor == CC_TYPEDEF)
       pd->kind = CC_TYP;

/* fix the beginning of the text to be emitted */
    pd->out = tx;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_GET_TYPE_DECL - return the declaration of type T */

static int _CC_get_type_decl(char *t, int nl, int *dl)
   {int id, nd, il, ok;
    char *td;
    decl *pd;
    qtype k;

    ok = FALSE;

    if (SC_hasharr_lookup(_CC.primitives, t) == NULL)
       {nd = _CC.nd;
	for (id = 0; (id < nd) && (ok == FALSE); id++)
	    {pd = _CC.all[id];
	     k  = pd->kind;
	     if ((k != CC_VAR) && (k != CC_FNC))
	        {td = pd->type;
		 ok = ((td != NULL) && (strcmp(t, td) == 0));

		 if (pd->stor == CC_TYPEDEF)
		    {td  = pd->name;
		     ok |= ((td != NULL) && (strcmp(t, td) == 0));};

/* skip this decl if it is in DL already */
		 if (ok == TRUE)
		    {for (il = 0; il < nl; il++)
			 {if (id == dl[il])
			     {ok = FALSE;
			      break;};};};};};

	id--;};

    if (ok == FALSE)
       id = -1;

    return(id);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_DEP_LOCAL - check a local symbol */

static int _CC_dep_local(haelem *hp, void *a)
   {int id, nd, nl, ok;
    int *dl;
    char *name, *nm;
    expr *v;
    decl *pd;

    ok = SC_haelem_data(hp, &name, NULL, (void **) &v);
    SC_ASSERT(ok == TRUE);

    dl = _CC.cur->dep;
    nd = _CC.nd;
    for (nl = 0; dl[nl] != -1; nl++);

/* check types only on the type field */
    if ((v->cat == CC_TYPE) && (v->type != NULL))
       {if (SC_hasharr_lookup(_CC.primitives, v->type) == NULL)
           {for (id = 0; id < nd; id++)
	        {pd = _CC.all[id];
		 if (strcmp(v->type, pd->type) == 0)
		    dl[nl++] = id;};};};

/* check everything on the name field */
    if (v->cat != CC_PARAMETER)
       {for (id = 0; id < nd; id++)
	    {pd = _CC.all[id];
	     nm = pd->name;
	     if ((nm != NULL) && (strcmp(name, nm) == 0))
	        {dl[nl++] = id;
		 break;};};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_SORT_DEP - sort the dependent declarations into order as specified
 *              - also remove duplicates
 */

static void _CC_sort_dep(decl *dc)
   {int il, jl, nl;
    int *dl;

    dl = dc->dep;
    for (nl = 0; dl[nl] != -1; nl++);

    SC_i_sort(dl, nl);

/* remove duplicates */
    for (il = 0; il < nl; il++)
        {if (dl[il] == dl[il+1])
            {for (jl = il+1; jl < nl; jl++)
		 dl[jl-1] = dl[jl];
	     dl[nl-1] = -1;
	     il--;
	     nl--;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_DEP_OBJS - make an ordered list of objects on which DC depends */

static void _CC_dep_objs(decl *dc)
   {int id, il, jl, nl;
    int *dl, *pl;
    char *tr;
    decl *pr;

    nl = 10000;
    dl = CMAKE_N(int, nl);
    for (il = 0; il <= nl; il++)
        dl[il] = -1;

    dc->dep = dl;

    SC_hasharr_foreach(dc->refobjs, _CC_dep_local, NULL);

    for (nl = 0; dl[nl] != -1; nl++);

/* find types on which DC depends */
    for (il = 0; il < nl; il++)
        {pr = _CC.all[dl[il]];
	 tr = pr->type;
	 id = _CC_get_type_decl(tr, nl, dl);
	 if (id != -1)
	    {il = -1;
	     dl[nl++] = id;

	     pl = _CC.all[id]->dep;
	     for (jl = 0; pl[jl] != -1; jl++)
	         dl[nl++] = pl[jl];};};

    _CC_sort_dep(dc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_PUSH_DECL - push a declaration on the the list of declarations */

static void _CC_push_decl(decl *pd)
   {

    _CC_dep_objs(pd);

    if ((_CC.ndx == 0) || (_CC.all == NULL))
       {_CC.ndx = 100;
	_CC.all = CMAKE_N(decl *, _CC.ndx);}

    else if (_CC.nd >= _CC.ndx)
       {_CC.ndx += 100;
	CREMAKE(_CC.all, decl *, _CC.ndx);};

    _CC.all[_CC.nd++] = pd;

    _CC.depend = NULL;
    _CC.cur    = NULL;
    _CC.ich    = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_OBJ_DECL - handle a single declaration, possibly from a list */

void _CC_obj_decl(expr *e, int force)
   {int more, ok;
    char *nm, *out;
    decl tc;
    decl *pd;

    pd = _CC.cur;
    if (pd == NULL)
       ok = FALSE;
    else
       {ok  = (_CC.ncbr == 0);
	ok &= (pd->cat != CC_UNDEF);
	ok &= ((pd->kind != CC_UNDEF) || (force == TRUE));
	ok &= ((e->name != NULL) ||
	       ((pd->cat == CC_STRUCT) && (pd->stor != CC_TYPEDEF)));};

    if (ok == TRUE)

/* save a copy of the current object */
       {tc      = *pd;
	tc.name = NULL;
	tc.lb   = _CC.rloc.iln+1;

/* add the object to the declaration list */
	pd->le = _CC.rloc.iln;

	if ((e->name != NULL) && (strcmp(pd->name, "__identifier__") == 0))
	   pd->name = CSTRSAVE(e->name);

	more = (SC_LAST_CHAR(pd->text) == ',');
	if (more == TRUE)
	   SC_LAST_CHAR(pd->text) = ';';

	_CC_clean_var(pd);

	if (pd->kind != CC_TYP)
	   pd->kind = CC_VAR;

	pd->refobjs = _CC.depend;

	_CC_push_decl(pd);

	if (pd->stor == CC_TYPEDEF)
	   CC_add_type(pd->name, CC_DECL);

/* initialize the next declaration */
	if (more == TRUE)
	   {_CC.cur = CC_mk_decl(0, MAX_BFSZ);

	     pd = _CC.cur;
	     nm = pd->name;

	     *pd = tc;

	     if (pd->stor == CC_EXTERN)
	        out = SC_dsnprintf(TRUE, "extern %s ", pd->type);
	     else
	        out = SC_dsnprintf(TRUE, "%s ", pd->type);

	     pd->name = nm;
	     pd->out  = out;
	     pd->text = CMAKE_N(char, pd->ncx);
	     SC_strncpy(pd->text, pd->ncx, out, -1);

	     _CC.ich = strlen(pd->text);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_DO_DECL - handle a declaration */

void _CC_do_decl(expr *e)
   {decl *pd;

    pd = _CC.cur;

    pd->le = _CC.rloc.iln;

    _CC_clean_var(pd);

    pd->refobjs = _CC.depend;

    _CC_push_decl(pd);

    if (pd->stor == CC_TYPEDEF)
       CC_add_type(pd->name, CC_DECL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_DO_FNC - handle a function definition */

void _CC_do_fnc(expr *e)
   {char *t;
    decl *pd;

    pd = _CC.cur;

    t = _CC_clean_text(pd->text);

    if (strstr(t, "extern") != NULL)
       {pd->stor = CC_EXTERN;
        pd->dec  = CC_DECL;}
    else if (strstr(t, "static") != NULL)
       {t += 7;
        pd->stor = CC_EXTERN;
        pd->dec  = CC_DECL;}
    else
       {pd->dec  = CC_DEF;
	pd->stor = CC_DEF;};

    pd->out     = t;
    pd->le      = _CC.rloc.iln;
    pd->vlb     = _CC.vloc.iln - (pd->le - pd->lb - 7);
    pd->kind    = CC_FNC;
    pd->refobjs = _CC.depend;

    _CC_push_decl(pd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_EXPR_STR - put a printing representation of E into S */

void CC_expr_str(char *s, int nc, expr *e)
   {

    if (s != NULL)
       s[0] = '\0';
    else
       nc = 0;

    if (e != NULL)
       {switch (e->cat)
           {case CC_INT :
	         snprintf(s, nc, "int(%lld)", (long long) *(int64_t *) e->ptr);
		 break;
	    case CC_FLOAT :
	         snprintf(s, nc, "float(%g)", *(double *) e->ptr);
		 break;
	    case CC_STRING :
	         snprintf(s, nc, "string(%s)", (char *) e->ptr);
		 break;
	    case CC_VARIABLE :
                 if (e->type != NULL)
		    snprintf(s, nc, "variable(%s %s)", e->type, e->name);
		 else
		    snprintf(s, nc, "variable(%s)", e->name);
		 break;
	    case CC_TYPE :
	         snprintf(s, nc, "type(%s)", e->type);
		 break;
	    default :
	         break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPREX - print an expr */

void dprex(expr *e)
   {char s[MAXLINE];

    if (e != NULL)
       {CC_expr_str(s, MAXLINE, e);

	fprintf(stdout, "%s\n", s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRDC - print decl with name */

void dprdc(char *name)
   {int id, nd;
    char *nm;
    decl *pd;

    printf("Matches for %s:\n", name);

    nd = _CC.nd;
    for (id = 0; id < nd; id++)
        {pd = _CC.all[id];
	 nm = pd->name;
	 if ((nm != NULL) && (strstr(name, nm) != NULL))
	    printf("   %6d  %20s %s\n",
		   id, pd->type, nm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
