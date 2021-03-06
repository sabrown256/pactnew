/*
 * SHLARG.C - support for the functions of the Scheme
 *          - not in the SMALL (core) Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VECTP - function version of SS_vectorp macro */

object *_SSI_vectp(SS_psides *si, object *obj)
   {object *o;

    o = SS_vectorp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/

/*                             VECTOR ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _SSI_MKVECT - make-vector for Scheme */

static object *_SSI_mkvect(SS_psides *si, object *arg)
   {int i;
    object *o;

    if (!SS_integerp(arg))
       SS_error(si, "ARGUMENT NOT INTEGER - MAKE-VECTOR", arg);

    i = (int) SS_INTEGER_VALUE(arg);

    o = SS_mk_vector(si, i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VECTOR - vector for Scheme */

static object *_SSI_vector(SS_psides *si, object *argl)
   {object *o;

    o = SS_lstvct(si, argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VCTLEN - vector-length for Scheme */

static object *_SSI_vctlen(SS_psides *si, object *arg)
   {int i;
    object *o;

    if (!SS_vectorp(arg))
       SS_error(si, "ARGUMENT NOT VECTOR - VECTOR-LENGTH", arg);

    i = SS_VECTOR_LENGTH(arg);

    o = SS_mk_integer(si, i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VCTREF - vector-ref for Scheme */

static object *_SSI_vctref(SS_psides *si, object *argl)
   {int i, n;
    object **va, *o;
    SS_vector *vct;

    vct = NULL;
    i   = -1;
    SS_args(si, argl,
	    G_SS_VECTOR_I,  &vct,
	    G_INT_I, &i,
	    0);

    n  = vct->length;
    va = vct->vect;

    if (i >= n)
       SS_error(si, "BAD INDEX - VECTOR-REF", argl);

    o = va[i];

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VCTSET - vector-set! for Scheme */

static object *_SSI_vctset(SS_psides *si, object *argl)
   {int i, n;
    object *val, **va, *o;
    SS_vector *vct;

    vct = NULL;
    i   = -1;
    val = SS_null;
    SS_args(si, argl,
	    G_SS_VECTOR_I,  &vct,
	    G_INT_I, &i,
	    G_OBJECT_I,  &val,
	    0);

    n  = vct->length;
    va = vct->vect;
    if (i >= n)
       SS_error(si, "BAD INDEX - VECTOR-SET!", argl);

    SS_assign(si, va[i], val);

    o = va[i];

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VCTLST - vector->list for Scheme */

object *SS_vctlst(SS_psides *si, object *arg)
   {int i, k;
    object **va, *ret, *o;

    if (!SS_vectorp(arg))
       SS_error(si, "ARGUMENT NOT VECTOR - VECTOR->LIST", arg);

    k   = SS_VECTOR_LENGTH(arg);
    va  = SS_VECTOR_ARRAY(arg);
    ret = SS_null;
    for (i = 0; i < k; i++)
        ret = SS_mk_cons(si, va[i], ret);

    o = SS_reverse(si, ret);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VCTLST - wrapper for SS_vctlst */

static object *_SSI_vctlst(SS_psides *si, object *arg)
   {object *o;

    o = SS_vctlst(si, arg);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LSTVCT - list->vector for Scheme */

object *SS_lstvct(SS_psides *si, object *arg)
   {int i, k;
    object **va, *vct;

    if (!SS_consp(arg))
       SS_error(si, "ARGUMENT NOT LIST - LIST->VECTOR", arg);

    k   = SS_length(si, arg);
    vct = SS_mk_vector(si, k);
    va  = SS_VECTOR_ARRAY(vct);

    for (i = 0; i < k; i++)
        {SS_assign(si, va[i], SS_car(si, arg));
         arg = SS_cdr(si, arg);};

    return(vct);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LSTVCT - wrapper for SS_lstvct */

static object *_SSI_lstvct(SS_psides *si, object *arg)
   {object *o;

    o = SS_lstvct(si, arg);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_DEFINE_GLOBAL - define a variable in the global environment frame */

object *_SSI_define_global(SS_psides *si, object *argl)
   {object *obj, *val, *t;
    char *s;

    val = SS_null;

    obj  = SS_cdr(si, argl);
    argl = SS_car(si, argl);

    if (SS_consp(argl))
       {obj  = SS_mk_cons(si, SS_cdr(si, argl), obj);
        argl = SS_car(si, argl);
        val  = SS_mk_procedure(si, argl, obj, si->global_env);

        s = SS_PROCEDURE_NAME(val);
        CFREE(s);
        s = SS_VARIABLE_NAME(argl);
        SS_PROCEDURE_NAME(val) = CSTRSAVE(s);}

    else if (SS_variablep(argl))
       {obj = SS_car(si, obj);
        val = SS_exp_eval(si, obj);

/* this preserves things for compound procedures (e.g. autoload) */
        if (SS_procedurep(val))
           {int ptype;

            ptype = SS_PROCEDURE_TYPE(val);
            if ((ptype == SS_PROC) || (ptype == SS_MACRO))
	       {t = SS_proc_env(si, val);
		SS_mark(t);
		t = SS_proc_body(si, val);
		SS_mark(t);
		t = SS_params(si, val);};};}

    else
       SS_error(si, "CAN'T DEFINE NON-VARIABLE OBJECT - _SSI_DEFINE_GLOBAL",
		argl);

    s = SS_PROCEDURE_NAME(si->fun);
    if (strcmp(s, "define-global-macro") == 0)
       SS_PROCEDURE_TYPE(val) = SS_MACRO;
    else if (strcmp(s, "define-global-macro-ev") == 0)
       SS_PROCEDURE_TYPE(val) = SS_MACRO_EV;

    SS_def_var(si, argl, val, si->global_env);

    return(argl);}

/*--------------------------------------------------------------------------*/

/*                                 RANDOMS                                  */

/*--------------------------------------------------------------------------*/

/* _SSI_SLEEP - sleep for n milliseconds */

static object *_SSI_sleep(SS_psides *si, object *obj)
   {int n;

    n = 0;
    SS_args(si, obj,
            G_INT_I, &n,
            0);

    SC_sleep(n);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GETCWD - return the value of the current working directory */

static object *_SSI_getcwd(SS_psides *si)
   {char *vr;
    object *wd;

    wd = SS_null;

    vr = SC_getcwd();
    if (vr != NULL)
       {wd = SS_mk_string(si, vr);
	CFREE(vr);};

    return(wd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GETENV - return the value of the specified environment variable */

static object *_SSI_getenv(SS_psides *si, object *obj)
   {char *vr, *vl;
    object *ev;

    vr = NULL;
    vl = NULL;
    SS_args(si, obj,
            G_STRING_I, &vr,
            0);

    if (vr != NULL)
       vl = getenv(vr);

    if (vl != NULL)
       ev = SS_mk_string(si, vl);
    else
       ev = SS_null;

    CFREE(vr);

    return(ev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SETENV - return the value of the specified environment variable */

static object *_SSI_setenv(SS_psides *si, object *obj)
   {int ok;
    char s[MAXLINE];
    char *vr, *vl;
    object *rv;

    ok = -1;
    vr = NULL;
    vl = NULL;
    SS_args(si, obj,
            G_STRING_I, &vr,
            G_STRING_I, &vl,
            0);

    if (vr != NULL)
       {snprintf(s, MAXLINE, "%s=%s", vr, vl);
	ok = SC_putenv(s);};

    CFREE(vr);
    CFREE(vl);

    rv = SS_mk_integer(si, ok);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PRINTENV - return a list of the specified environment variables
 *               - somewhat of a misnomer but appeals to familiar printenv
 *               - program
 *               - Usage: (printenv)
 *                        (printenv "HOME" "PWD")
 */

static object *_SSI_printenv(SS_psides *si, object *argl)
   {int i, n;
    char s[MAXLINE];
    char *vr, **vrs;
    object *l, *v, *vl, *lst, *pr;
    static int dsp = FALSE;

/* make a list of names from the global environment frame */
    if (SS_nullobjp(argl))
       vrs = SS_bound_vars(si, "$*", si->global_env);

/* make a list of names from the argument list */
    else
       {n   = SS_length(si, argl);
	vrs = CMAKE_N(char *, n+1);

	n = 0;
	for (l = argl; !SS_nullobjp(l); l = SS_cdr(si, l))
	    {v = SS_car(si, l);
	     SS_args(si, v,
		     G_STRING_I, &vr,
		     0);
	     snprintf(s, MAXLINE, "$%s", vr);
	     vrs[n++] = CSTRSAVE(s);

	     CFREE(vr);};

	vrs[n++] = NULL;};

    lst = SS_null;

    if (dsp == TRUE)
       {int dspo;

/* print the list of names and variable values */
        dspo = SS_set_display_flag(TRUE);

	for (i = 0; vrs[i] != NULL; i++)
	    {vr = vrs[i];
	     vl = _SS_lk_var_valc(si, vr, si->global_env);
	     snprintf(s, MAXLINE, "%s = ", vr);
	     if (vl == NULL)
	        SS_print(si, si->outdev, SS_null, s, "\n");
	     else
	        SS_print(si, si->outdev, vl, s, "\n");};

	SS_set_display_flag(dspo);};

/* make a list of variable,value pairs */
    SC_ptr_arr_len(n, vrs);
    for (i = n-1; i >= 0; i--)
        {vr = vrs[i];
	 vl = _SS_lk_var_valc(si, vr, si->global_env);
	 pr = SS_make_list(si, G_STRING_I, vr,
			   G_OBJECT_I, vl,
			   0);
	 SS_assign(si, lst, SS_mk_cons(si, pr, lst));};

    SC_mark(lst, -1);

    SC_free_strings(vrs);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PRINT_ENV - print the specified environment frame */

static object *_SSI_print_env(SS_psides *si, object *obj)
   {int i, n;
    object *penv;
    char bf[MAXLINE];

    n = 0;
    SS_args(si, obj,
            G_INT_I, &n,
            0);

    penv = si->env;
    for (i = 0; (i < n) && !SS_nullobjp(penv); i++, penv = SS_cdr(si, penv));

    snprintf(bf, MAXLINE, "Environment frame #%d:\n", n+1);
    SS_print(si, si->outdev, penv, bf, "\n\n");

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GET_PNAME - return the full path name of the program running as PID */

static object *_SSI_get_pname(SS_psides *si, object *obj)
   {int id, rv;
    char path[PATH_MAX];
    object *s;

    id = -1;
    SS_args(si, obj,
            G_INT_I, &id,
            0);

    rv = SC_get_pname(path, PATH_MAX, id);
    if (rv == 0)
       s = SS_mk_string(si, path);
    else
       s = SS_null;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LOAD_AVE - return the load averages for the current host */

static object *_SSI_load_ave(SS_psides *si)
   {int rv;
    double av[3];
    object *s;

    rv = SC_load_ave(av);
    if (rv == TRUE)
       s = SS_make_list(si, G_DOUBLE_I, av,
			G_DOUBLE_I, av+1,
			G_DOUBLE_I, av+2,
			0);
    else
       s = SS_null;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GET_NCPU - return the number of cpus */

static object *_SSI_get_ncpu(SS_psides *si)
   {int rv;
    object *s;

    rv = SC_get_ncpu();
    s = SS_mk_integer(si, rv);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FOPEN - fopen in Scheme */

object *_SSI_fopen(SS_psides *si, object *argl)
   {FILE *str;
    char *name, *mode;
    object *prt;

    name = NULL;
    mode = NULL;
    SS_args(si, argl,
	    G_STRING_I, &name,
	    G_STRING_I, &mode,
	    0);

    if (mode == NULL)
       str = io_open(name, "r");
    else
       str = io_open(name, mode);

    if (str == NULL)
       SS_error(si, "CAN'T OPEN FILE - FOPEN", argl);

    if ((mode != NULL) && (mode[0] == 'r'))
       prt = SS_mk_inport(si, str, name);
    else
       prt = SS_mk_outport(si, str, name);

    CFREE(name);
    CFREE(mode);

    return(prt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FCLOSE - fclose in Scheme */

object *_SSI_fclose(SS_psides *si, object *obj)
   {FILE *str;

    str = NULL;
    if (SS_outportp(obj))
       str = SS_OUTSTREAM(obj);
    else if (SS_inportp(obj))
       str = SS_INSTREAM(obj);
    else
       SS_error(si, "BAD PORT TO FCLOSE", obj);

    if (str != NULL)
       io_close(str);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_WALL_CLOCK_TIME - return the accumulated wall clock time in seconds */

static object *_SSI_wall_clock_time(SS_psides *si)
   {object *ret;

    ret = SS_mk_float(si, SC_wall_clock_time());

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_USG - return the memory usage info */

static object *_SSI_mem_usg(SS_psides *si)
   {int64_t a, f, d;
    object *ret;

    SC_mem_stats(&a, &f, &d, NULL);

    ret = SS_make_list(si, G_INT_I, &a,
		       G_INT_I, &f,
		       G_INT_I, &d,
		       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_MAP - wrapper around SC_mem_map */

static object *_SSI_mem_map(SS_psides *si, object *arg)
   {FILE *fp;

    if (SS_nullobjp(arg))
       arg = si->outdev;

    else if (!SS_outportp(arg))
       SS_error(si, "BAD PORT - SC_MEM_MAP", arg);

    fp = SS_OUTSTREAM(arg);
    SC_mem_map(fp, FALSE);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_MONITOR - wrapper around SC_mem_monitor */

static object *_SSI_mem_monitor(SS_psides *si, object *arg)
   {int old, lev;
    long nb;
    char msg[MAXLINE], id[MAXLINE];
    char *s;
    object *o;

    old = 0;
    lev = 0;
    s   = "scheme";
    SS_args(si, arg,
            G_INT_I, &old,
            G_INT_I, &lev,
            G_STRING_I, &s,
            0);

/* make this local copy to avoid reporting that S leaked */
    strncpy(id, s, MAXLINE);
    CFREE(s);

    nb = SC_mem_monitor(old, lev, id, msg);

    if ((old >= 0) && (msg[0] != '\0'))
       PRINT(stdout, "\n%s\n\n", msg);

    o = SS_mk_integer(si, nb);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_TRACE - wrapper around SC_mem_check */

static object *_SSI_mem_trace(SS_psides *si)
   {int64_t nb;
    object *o;

    nb = SC_mem_chk(3);
    o  = SS_mk_integer(si, nb);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_CHK - wrapper around SC_mem_check */

static object *_SSI_mem_chk(SS_psides *si)
   {int64_t na, nf, nr;
    object *o;

    na = SC_mem_chk(1);
    nf = SC_mem_chk(2);
    nr = SC_mem_chk(4);
    o  = SS_make_list(si, G_INT_I, &na,
		      G_INT_I, &nf,
		      G_INT_I, &nr,
		      0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SIZEOF - wrapper around SIZEOF */

static object *_SSI_sizeof(SS_psides *si, object *arg)
   {int64_t nb;
    char *type;
    object *o;

    type = NULL;
    SS_args(si, arg,
	    G_STRING_I, &type,
	    0);

    nb = (type == NULL) ? 0 : SIZEOF(type);

    CFREE(type);

    o = SS_mk_integer(si, nb);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ATTACH - wrapper around SC_ATTACH_DBG */

static object *_SSI_attach(SS_psides *si, object *arg)
   {int pid;
    int64_t rv;
    object *o;

    pid = -1;
    SS_args(si, arg,
	    G_INT_I, &pid,
	    0);

    rv = SC_attach_dbg(pid);

    o = SS_mk_integer(si, rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RETRACE - wrapper around SC_RETRACE_DBG */

static object *_SSI_retrace(SS_psides *si, object *arg)
   {int pid, to;
    int64_t rv;
    object *o;

    pid = -1;
    to  = -1;
    SS_args(si, arg,
	    G_INT_I, &pid,
	    G_INT_I, &to,
	    0);

    rv = SC_retrace_exe(NULL, pid, to);
    o  = SS_mk_integer(si, rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_INTERACTP - interactive mode predicate */

static object *_SSI_interactp(SS_psides *si)
   {object *o;

    o = si->interactive ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LOAD_ENV - load the environment variables into the
 *               - global environment
 */

static object *_SSI_load_env(SS_psides *si, object *arg)
   {

    SS_env_vars(si, NULL, NULL);

    return(SS_f);}

/*--------------------------------------------------------------------------*/

/*                                INSTALLATION                              */

/*--------------------------------------------------------------------------*/

/* _SS_INST_LRG - install the primitives making up the LARGE Scheme */

void _SS_inst_lrg(SS_psides *si)
   {

    SS_install(si, "attach-debugger",
               "Procedure: attach a debugger to the specified pid",
               SS_nargs,
               _SSI_attach, SS_PR_PROC);

    SS_install(si, "call-stack-trace",
               "Procedure: print out the call stack trace of the specified pid",
               SS_nargs,
               _SSI_retrace, SS_PR_PROC);

    SS_install(si, "wall-clock-time",
               "Procedure: Returns the accumulated wall clock time in seconds",
               SS_zargs,
               _SSI_wall_clock_time, SS_PR_PROC);

    SS_install(si, "define-global",
               "Special Form: defines variables and procedures in the global environment",
               SS_nargs,
               _SSI_define_global, SS_UE_MACRO);

    SS_install(si, "define-global-macro",
               "Special Form: defines variables and procedures in the global environment",
               SS_nargs,
               _SSI_define_global, SS_UE_MACRO);

    SS_install(si, "define-global-macro-ev",
               "Special Form: defines variables and procedures in the global environment",
               SS_nargs,
               _SSI_define_global, SS_UE_MACRO);

    SS_install(si, "env",
               "Procedure: print the specified environment frame",
               SS_sargs,
               _SSI_print_env, SS_PR_PROC);

    SS_install(si, "fopen",
               "Procedure: C style file open",
               SS_nargs,
               _SSI_fopen, SS_PR_PROC);

    SS_install(si, "fclose",
               "Procedure: C style file close",
               SS_sargs,
               _SSI_fclose, SS_PR_PROC);

    SS_install(si, "getcwd",
               "Procedure: return the current working directory",
               SS_zargs,
               _SSI_getcwd, SS_PR_PROC);

    SS_install(si, "getenv",
               "Procedure: return the value of the specified environment variable",
               SS_sargs,
               _SSI_getenv, SS_PR_PROC);

    SS_install(si, "get-ncpu",
               "Procedure: return the number of CPUs on the current host",
               SS_zargs,
               _SSI_get_ncpu, SS_PR_PROC);

    SS_install(si, "get-pid-name",
               "Procedure: return the name of the program running as the given process id",
               SS_nargs,
               _SSI_get_pname, SS_PR_PROC);

    SS_install(si, "interactive?",
	       "Procedure: Return #t iff interactive mode is on\n",
	       SS_zargs,
	       _SSI_interactp, SS_PR_PROC);

    SS_install(si, "list->vector",
               "Procedure: Returns a vector whose elements are the same as the lists",
               SS_sargs,
               _SSI_lstvct, SS_PR_PROC);

    SS_install(si, "load-ave",
               "Procedure: return the load averages of the current host",
               SS_zargs,
               _SSI_load_ave, SS_PR_PROC);

    SS_install(si, "load-env",
               "Procedure: load environment variables into the global environment",
               SS_znargs,
               _SSI_load_env, SS_PR_PROC);

    SS_install(si, "make-vector",
               "Procedure: Return a new vector whose length is specified by the argument",
               SS_sargs,
               _SSI_mkvect, SS_PR_PROC);

    SS_install(si, "memory-map",
               "Procedure: print a memory map to the given port",
               SS_sargs,
               _SSI_mem_map, SS_PR_PROC);

    SS_install(si, "memory-monitor",
               "Procedure: map out leaks from the heap",
               SS_nargs,
               _SSI_mem_monitor, SS_PR_PROC);

    SS_install(si, "memory-trace",
               "Procedure: return the number of allocated memory blocks",
               SS_zargs,
               _SSI_mem_trace, SS_PR_PROC);

    SS_install(si, "memory-check",
               "Procedure: return the number of allocated, freed, and registered memory blocks",
               SS_zargs,
               _SSI_mem_chk, SS_PR_PROC);

    SS_install(si, "memory-usage",
               "Procedure: Returns the number of bytes allocated, freed, and diff",
               SS_zargs,
               _SSI_mem_usg, SS_PR_PROC);

    SS_install(si, "printenv",
               "Procedure: print the environment variables specified",
               SS_nargs,
               _SSI_printenv, SS_PR_PROC);

    SS_install(si, "setenv",
               "Procedure: set the value of the specified environment variable",
               SS_nargs,
               _SSI_setenv, SS_PR_PROC);

    SS_install(si, "sizeof",
               "Procedure: Returns the byte size of the data type named by its argument",
               SS_sargs,
               _SSI_sizeof, SS_PR_PROC);

    SS_install(si, "sleep",
               "Procedure: Sleep n milliseconds",
               SS_sargs,
               _SSI_sleep, SS_PR_PROC);

    SS_install(si, "vector?",
               "Procedure: Returns #t iff the object is of type vector",
               SS_sargs,
               _SSI_vectp, SS_PR_PROC);

    SS_install(si, "vector",
               "Procedure: Analog to list procedure for vectors",
               SS_nargs,
               _SSI_vector, SS_PR_PROC);

    SS_install(si, "vector-length",
               "Procedure: Returns the number of elements in the specified vector",
               SS_sargs,
               _SSI_vctlen, SS_PR_PROC);

    SS_install(si, "vector->list",
               "Procedure: Returns a list whose elements are the same as the vectors",
               SS_sargs,
               _SSI_vctlst, SS_PR_PROC);

    SS_install(si, "vector-ref",
               "Procedure: Returns the nth element of the given vector",
               SS_nargs,
               _SSI_vctref, SS_PR_PROC);

    SS_install(si, "vector-set!",
               "Procedure: Sets the nth element of the given vector",
               SS_nargs,
               _SSI_vctset, SS_PR_PROC);

    SS_install_cf(si, "interactive",
                  "Variable: Controls display of ouput data in functions\n     Usage: interactive <on|off>",
                  SS_acc_int,
                  &si->interactive);

    SS_install_cf(si, "lines-page",
                  "Variable: Controls the number of lines per page for selected printing commands\n     Usage: lines-page <integer>",
                  SS_acc_int,
                  &si->lines_page);

    SS_install_cf(si, "trace-env",
                  "Variable: Show upto <n> variables in environment frames entering function calls\n     Usage: trace-env <n>",
                  SS_acc_int,
                  &si->trace_env);

    _SS_inst_hash(si);
    _SS_inst_str(si);
    _SS_inst_chr(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
