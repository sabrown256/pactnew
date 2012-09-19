/*
 * SHPRC2.C - C and Scheme Process control routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

#if 0
#include "shell/common.h"
#include "shell/libpsh.c"
#include "shell/libdb.c"
#include "shell/libpgrp.c"
#else
#include "../psh/common.h"
#include "../psh/libpsh.c"
#include "../psh/libdb.c"
#include "../psh/libpgrp.c"
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LIST_STRINGS - return a list of strings derived from ARGL */

static char **_SS_list_strings(SS_psides *si, object *argl)
   {int i;
    char *s, **al;
    object *o;
    SC_array *a;

    a = SC_STRING_ARRAY();

    for (i = 0; SS_consp(argl); argl = SS_cdr(si, argl), i++)
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

/* _SS_STRING_LIST - return a list of derived from string S */

static object *_SS_string_list(SS_psides *si, char *s)
   {int i, n;
    char *t, **sa;
    object *o, *lst;

    sa = SC_tokenize(s, " \t");
    SC_ptr_arr_len(n, sa);

    lst = SS_null;
    for (i = 0; i < n; i++)
        {t = sa[i];
	 if (SC_intstrp(t, SC_gs.radix))
	    o = SS_mk_integer(si, SC_stol(t));
	 else if (SC_fltstrp(t))
	    o = SS_mk_float(si, SC_stof(t));
	 else if (SC_cmplxstrp(t))
	    o = SS_mk_complex(si, SC_stoc(t));
	 else
	    o = SS_mk_string(si, t);

	 lst = SS_mk_cons(si, o, lst);};
	 
    SC_free_strings(sa);

    lst = SS_reverse(si, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PROC_EXEC - evaluate a function for gexec */

static int _SS_proc_exec(char *db, io_mode m, FILE **fp,
			 char *name, int c, char **v)
   {int i, l, rv, type[MAXLINE];
    object *fnc, *expr;
    void *ptr[MAXLINE];
    SS_psides *si;
    char *ioc[] = { "stdin", "stdout", "stderr" };

    si = SS_get_current_scheme(-1);

    fnc = _SS_lk_var_valc(si, name, si->env);
    if (fnc == NULL)
       SS_error(si, "UNKNOWN PROCEDURE - _SS_PROC_EXEC",
		SS_mk_string(si, name));

    l = 0;

/* start with the function mode */
    type[l] = SC_INTEGER_I;
    ptr[l]  = &m;
    l++;

/* add the standard I/O channels */
    for (i = 0; i < N_IO_CHANNELS; i++)
        {type[l] = SS_OBJECT_I;
         if (i == 0)
	    {SC_block_file(fp[i]);
	     ptr[l] = SS_mk_inport(si, SC_fwrap(fp[i]), ioc[i]);}
	 else
	    ptr[l] = SS_mk_outport(si, SC_fwrap(fp[i]), ioc[i]);
	 l++;};

/* add the arguments */
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

    rv = -1;
    SS_args(si, si->val,
	    SC_INTEGER_I, &rv,
	    0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAPS - map function name to procedure for function execution */

static PFPCAL _SS_maps(char *s)
   {PFPCAL f;

    if (strcmp(s, "var") == 0)
       f = gexec_var;
    else if (strcmp(s, "file") == 0)
       f = gexec_file;
    else
       f = _SS_proc_exec;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GEXEC - gexec wrapper/access for SCHEME */

static object *_SSI_gexec(SS_psides *si, object *argl)
   {int n;
    char t[MAXLINE];
    char *s, *db, **al;
    object *o;
    client *cl;

    db = getenv("PERDB_PATH");
    if (db == NULL)
       {snprintf(t, MAXLINE, "PERDB_PATH=%s/.gexecdb", getenv("HOME"));
	SC_putenv(t);
	db = getenv("PERDB_PATH");};

    cl = make_client(db, CLIENT);
    dbset(cl, "gstatus", "");

    n  = SS_length(si, argl);
    al = _SS_list_strings(si, argl);

    gexec(db, n, al, NULL, _SS_maps);

    s = dbget(cl, FALSE, "gstatus");
    o = _SS_string_list(si, s);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_PGRP - install the primitives for process group control */

void _SS_inst_pgrp(SS_psides *si)
   {process_group_state *ps;

    ps = get_process_group_state();

    SS_install_cf(si, "process-group-debug",
		  "Variable: Flag controlling level of diagnostic output for gexec",
		  SS_acc_int,
                  &ps->dbg_level);

    SS_install(si, "!",
               "Procedure: Exec a process group",
               SS_nargs,
               _SSI_gexec, SS_UR_MACRO);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
