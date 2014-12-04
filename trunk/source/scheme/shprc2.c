/*
 * SHPRC2.C - C and Scheme Process control routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

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

/* _SS_PROC_EXEC - evaluate a function for gexeca */

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
        {type[l] = G_OBJECT_I;
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
       f = PS_gexec_var;
    else if (strcmp(s, "file") == 0)
       f = PS_gexec_file;
    else
       f = _SS_proc_exec;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GEXEC - gexeca wrapper/access for SCHEME */

static object *_SSI_gexec(SS_psides *si, object *argl)
   {int n;
    char t[MAXLINE];
    char *db, **al;
    object *o;
    process_state *ps;
    static client *cl = NULL;

    db = getenv("PERDB_PATH");
    if (db == NULL)
       {snprintf(t, MAXLINE, "PERDB_PATH=%s/.gexecdb", getenv("HOME"));
	SC_putenv(t);
	db = getenv("PERDB_PATH");};

    if (cl == NULL)
       {int auth;

	auth = FALSE;
	cl = PS_make_client(CLIENT, DB_PORT, auth, db, PS_cl_logger, NULL);};

    n  = SS_length(si, argl);
    al = _SS_list_strings(si, argl);

    ps = PS_get_process_state();

    PS_gexeca(db, n, al, NULL, FALSE, _SS_maps);

    o = _SS_string_list(si, ps->gstatus);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_PGRP - install the primitives for process group control */

void _SS_inst_pgrp(SS_psides *si)
   {process_state *ps;

    ps = PS_get_process_state();

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
