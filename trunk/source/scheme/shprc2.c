/*
 * SHPRC2.C - C and Scheme Process control routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

#include "shell/common.h"
#include "shell/libpsh.c"
#include "shell/libdb.c"
#include "shell/libpgrp.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LIST_STRINGS - return a list of strings derived from ARGL */

static char **_SS_list_strings(SS_psides *si, object *argl)
   {char *s, **al;
    object *o;
    SC_array *a;

    a = SC_STRING_ARRAY();

    for ( ; SS_consp(argl); argl = SS_cdr(si, argl))
        {o = SS_car(si, argl);
	 s = NULL;
	 SS_args(si, o,
		 SC_STRING_I, &s,
		 0);
	 SC_array_string_add_copy(a, s);};
	 
    al = SC_array_done(a);

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PROC_EXEC - evaluate a function for gexec */

static int _SS_proc_exec(char *db, io_mode m, FILE **fp,
			 char *name, int c, char **v)
   {int i, l, rv, type[MAXLINE];
    object *fnc, *expr;
    void *ptr[MAXLINE];
    SS_psides *si;

    si = SS_get_current_scheme(-1);

    fnc = _SS_lk_var_valc(si, name, si->env);
    if (fnc == NULL)
       SS_error(si, "UNKNOWN PROCEDURE - _SS_PROC_EXEC",
		SS_mk_string(si, name));

    l = 0;

    type[l] = SC_INTEGER_I;
    ptr[l]  = &m;
    l++;

    type[l] = SS_OBJECT_I;
    ptr[l]  = SS_mk_inport(si, fp[0], "stdin");
    l++;

    type[l] = SS_OBJECT_I;
    ptr[l]  = SS_mk_outport(si, fp[1], "stdout");
    l++;

    type[l] = SS_OBJECT_I;
    ptr[l]  = SS_mk_outport(si, fp[2], "stderr");
    l++;

    for (i = 0; i < c; i++)
        {type[l] = SC_STRING_I;
         ptr[l]  = v[i];
         l++;};

    SC_mem_stats_set(0L, 0L);

    expr = SS_mk_cons(si, fnc, _SS_make_list(si, l, type, ptr));
    SC_mark(expr, 1);

    SS_eval(si, expr);

    SS_gc(si, expr);

    SS_assign(si, si->env, si->global_env);
    SS_assign(si, si->this, SS_null);
    SS_assign(si, si->exn, SS_null);
    SS_assign(si, si->unev, SS_null);
    SS_assign(si, si->argl, SS_null);
    SS_assign(si, si->fun, SS_null);

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAPS - map function name to procedure for function execution */

static PFPCAL _SS_maps(char *s)
   {PFPCAL f;

    f = _SS_proc_exec;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GEXEC - gexec wrapper/access for SCHEME */

object *_SSI_gexec(SS_psides *si, object *argl)
   {int n, rv;
    char *db, **al;
    object *o;

    db = getenv("PERDB_PATH");

    n  = SS_length(si, argl);
    al = _SS_list_strings(si, argl);

    rv = gexec(db, n, al, NULL, _SS_maps);

    o = SS_mk_integer(si, rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
