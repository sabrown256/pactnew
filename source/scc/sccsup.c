/*
 * SCCSUP.C - support routines for SCC
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define EXTERN_VAR

#include "scc_int.h"

expr
 *CC_struct,
 *CC_union,
 *CC_enum,
 *CC_anon,
 *CC_eof,
 *CC_null,
 *CC_t,
 *CC_f;

CC_c_parse_state
 _CC = { FALSE, FALSE, FALSE, FALSE, FALSE };

/* figure out what to do with a newline
 * in general newlines are ignored
 * however if attempting to interpret preprocessing
 * directives they are necessary to render the
 * the grammar unambiguous
 * for example:
 * 
 *   #define FOO
 *   int x;
 * 
 * and
 * 
 *   #define FOO 3
 * 
 * provide a shift/reduce conflict as the grammar is
 * currently defined
 * no sense can be made of this case unlike the if/else
 * shift/reduce conflict
 *
 * use cpp_define member of CC_c_parse_state as control
 * on the parse to resolve the conflict
 */

/*--------------------------------------------------------------------------*/

/*                            GRAMMAR SUPPORT                               */

/*--------------------------------------------------------------------------*/

/* CC_PARSE - C parser */

int CC_parse(FILE *fp)
   {int ret;
    expr *e;

    if (SETJMP(_CC.cpu))
       ret = FALSE;

    else
       {_CC.fp            = fp;
	_CC.cpp_directive = FALSE;
       
	sccgr_parse();

	e = CC_parse_token_val_c();
	SC_ASSERT(e != NULL);

	ret = (_CC.have_eof == FALSE);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_DIAGNOSE_PARSE - turn on/off diagnostics of the parse */

void CC_diagnose_parse(int sw)
   {int *ssdbg;

    ssdbg  = CC_parse_debug_c();
    *ssdbg = sw;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_INIT_C_SYNTAX_MODE - setup for parsing C syntax */

void CC_init_c_syntax_mode(void)
   {

    _CC.rloc.iln = 1;

    if (_CC.mode_init == FALSE)
       {_CC.mode_init        = TRUE;
	_CC.diagnose_grammar = FALSE;

	CC_enum   = CC_mk_type("enum");
	CC_struct = CC_mk_type("struct");
	CC_union  = CC_mk_type("union");

	CC_anon   = CC_mk_variable(NULL, "anonymous", CC_DEF);

	CC_add_type("char",   CC_TYP);
	CC_add_type("short",  CC_TYP);
	CC_add_type("int",    CC_TYP);
	CC_add_type("long",   CC_TYP);
	CC_add_type("float",  CC_TYP);
	CC_add_type("double", CC_TYP);
	CC_add_type("void",   CC_TYP);

	CC_add_type("signed",   CC_TYPEQ);
	CC_add_type("unsigned", CC_TYPEQ);

	CC_diagnose_parse(FALSE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_REL_EXPR - release a expr */

static int _CC_rel_expr(haelem *hp, void *a)
   {int ok;
    char *name;
    expr *e;

    ok = SC_haelem_data(hp, &name, NULL, (void **) &e, TRUE);
    SC_ASSERT(ok == TRUE);

    if (e != NULL)
       {CFREE(e->name);
	CFREE(e->type);
        CFREE(e->ptr);
	CFREE(e);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_FREE_EXPRS - release the table VT filled with exprs */

void _CC_free_exprs(hasharr *vt)
   {

    SC_hasharr_clear(vt, _CC_rel_expr, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_DECL - instantiate and return a decl */

decl *CC_mk_decl(int ln, int nc)
   {decl *pd;

    pd = CMAKE(decl);

    pd->lb      = ln;
    pd->le      = -1;
    pd->ncx     = nc;
    pd->kind    = CC_UNDEF;
    pd->dec     = CC_UNDEF;
    pd->qual    = CC_UNDEF;
    pd->stor    = CC_UNDEF;
    pd->cat     = CC_UNDEF;
    pd->type    = NULL;
    pd->name    = NULL;
    pd->out     = NULL;
    pd->text    = CMAKE_N(char, nc);
    pd->refobjs = NULL;

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_RL_DECL - instantiate and return a decl */

void CC_rl_decl(decl *pd)
   {

    if (pd != NULL)
       {_CC_free_exprs(pd->refobjs);

	 CFREE(pd->name);
	 CFREE(pd->type);
	 CFREE(pd->text);
	 CFREE(pd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_RL_EXPR - instantiate and return a expr */

void CC_rl_expr(expr *e)
   {

    CFREE(e->type);
    CFREE(e->name);
    CFREE(e->ptr);
    CFREE(e);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_REL_TYPE - release a type */

static int _CC_rel_type(haelem *hp, void *a)
   {int ok;
    char *name;
    expr *e;

    ok = SC_haelem_data(hp, &name, NULL, (void **) &e, TRUE);
    SC_ASSERT(ok == TRUE);

    if (e != NULL)
       {CFREE(e->type);
	CFREE(e->name);
        CFREE(e->ptr);
        CFREE(e);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_FREE_TYPES - release the type table */

static void _CC_free_types(void)
   {

    SC_hasharr_clear(_CC.types, _CC_rel_type, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_FINISH - finish up by freeing memory */

void CC_finish(void)
   {int i, n;
    decl *pd;

/* release declarations */
    CC_rl_decl(_CC.cur);

    n = _CC.nd;
    for (i = 0; i < n; i++)
        {pd = _CC.all[i];
         CC_rl_decl(pd);};

    CFREE(_CC.all);

    _CC_free_exprs(_CC.consts);
    _CC_free_exprs(_CC.depend);
    _CC_free_types();

/* release the remainder of the parse state */
    CFREE(_CC.lex_text);
    CFREE(_CC.rloc.fname);

/* release some variables */
    CC_rl_expr(CC_enum);
    CC_rl_expr(CC_struct);
    CC_rl_expr(CC_union);
    CC_rl_expr(CC_anon);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
