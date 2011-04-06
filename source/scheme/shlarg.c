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

object *_SSI_vectp(object *obj)
   {object *o;

    o = SS_vectorp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/

/*                             VECTOR ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _SSI_MKVECT - make-vector for Scheme */

static object *_SSI_mkvect(object *arg)
   {int i;
    object *o;

    if (!SS_integerp(arg))
       SS_error("ARGUMENT NOT INTEGER - MAKE-VECTOR", arg);

    i = (int) SS_INTEGER_VALUE(arg);

    o = SS_mk_vector(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VECTOR - vector for Scheme */

static object *_SSI_vector(object *argl)
   {object *o;

    o = SS_lstvct(argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VCTLEN - vector-length for Scheme */

static object *_SSI_vctlen(object *arg)
   {int i;
    object *o;

    if (!SS_vectorp(arg))
       SS_error("ARGUMENT NOT VECTOR - VECTOR-LENGTH", arg);

    i = SS_VECTOR_LENGTH(arg);

    o = SS_mk_integer(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VCTREF - vector-ref for Scheme */

static object *_SSI_vctref(object *argl)
   {int i, n;
    object **va, *o;
    vector *vct;

    vct = NULL;
    i   = -1;
    SS_args(argl,
	    SS_VECTOR_I,  &vct,
	    SC_INT_I, &i,
	    0);

    n  = vct->length;
    va = vct->vect;

    if (i >= n)
       SS_error("BAD INDEX - VECTOR-REF", argl);

    o = va[i];

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VCTSET - vector-set! for Scheme */

static object *_SSI_vctset(object *argl)
   {int i, n;
    object *val, **va, *o;
    vector *vct;

    vct = NULL;
    i   = -1;
    val = SS_null;
    SS_args(argl,
	    SS_VECTOR_I,  &vct,
	    SC_INT_I, &i,
	    SS_OBJECT_I,  &val,
	    0);

    n  = vct->length;
    va = vct->vect;
    if (i >= n)
       SS_error("BAD INDEX - VECTOR-SET!", argl);

    SS_Assign(va[i], val);

    o = va[i];

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VCTLST - vector->list for Scheme */

object *SS_vctlst(object *arg)
   {int i, k;
    object **va, *ret, *o;

    if (!SS_vectorp(arg))
       SS_error("ARGUMENT NOT VECTOR - VECTOR->LIST", arg);

    k   = SS_VECTOR_LENGTH(arg);
    va  = SS_VECTOR_ARRAY(arg);
    ret = SS_null;
    for (i = 0; i < k; i++)
        ret = SS_mk_cons(va[i], ret);

    o = SS_reverse(ret);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LSTVCT - list->vector for Scheme */

object *SS_lstvct(object *arg)
   {int i, k;
    object **va, *vct;

    if (!SS_consp(arg))
       SS_error("ARGUMENT NOT LIST - LIST->VECTOR", arg);

    k   = SS_length(arg);
    vct = SS_mk_vector(k);
    va  = SS_VECTOR_ARRAY(vct);

    for (i = 0; i < k; i++)
        {SS_Assign(va[i], SS_car(arg));
         arg = SS_cdr(arg);};

    return(vct);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_DEFINE_GLOBAL - define a variable in the global environment frame */

object *_SSI_define_global(object *argl)
   {object *obj, *val, *t;
    char *s;

    val = SS_null;

    obj  = SS_cdr(argl);
    argl = SS_car(argl);

    if (SS_consp(argl))
       {obj  = SS_mk_cons(SS_cdr(argl), obj);
        argl = SS_car(argl);
        val  = SS_mk_procedure(argl, obj, SS_Global_Env);

        s = SS_PROCEDURE_NAME(val);
        CFREE(s);
        s = SS_VARIABLE_NAME(argl);
        SS_PROCEDURE_NAME(val) = CSTRSAVE(s);}

    else if (SS_variablep(argl))
       {obj = SS_car(obj);
        val = SS_exp_eval(obj);

/* this preserves things for compound procedures (e.g. autoload) */
        if (SS_procedurep(val))
           {int ptype;

            ptype = SS_PROCEDURE_TYPE(val);
            if ((ptype == SS_PROC) || (ptype == SS_MACRO))
	       {t = SS_proc_env(val);
		SS_MARK(t);
		t = SS_proc_body(val);
		SS_MARK(t);
		t = SS_params(val);};};}

    else
       SS_error("CAN'T DEFINE NON-VARIABLE OBJECT - _SSI_DEFINE_GLOBAL", argl);

    if (strcmp(SS_PROCEDURE_NAME(SS_Fun), "define-global-macro") == 0)
       SS_PROCEDURE_TYPE(val) = SS_MACRO;

    SS_def_var(argl, val, SS_Global_Env);

    return(argl);}

/*--------------------------------------------------------------------------*/

/*                                 RANDOMS                                  */

/*--------------------------------------------------------------------------*/

/* _SSI_SLEEP - sleep for n milliseconds */

static object *_SSI_sleep(object *obj)
   {int n;

    n = 0;
    SS_args(obj,
            SC_INT_I, &n,
            0);

    SC_sleep(n);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GETCWD - return the value of the current working directory */

static object *_SSI_getcwd(void)
   {char *vr;
    object *wd;

    wd = SS_null;

    vr = SC_getcwd();
    if (vr != NULL)
       {wd = SS_mk_string(vr);
	CFREE(vr);};

    return(wd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GETENV - return the value of the specified environment variable */

static object *_SSI_getenv(object *obj)
   {char *vr, *vl;
    object *ev;

    vr = NULL;
    vl = NULL;
    SS_args(obj,
            SC_STRING_I, &vr,
            0);

    if (vr != NULL)
       vl = getenv(vr);

    if (vl != NULL)
       ev = SS_mk_string(vl);
    else
       ev = SS_null;

    CFREE(vr);

    return(ev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SETENV - return the value of the specified environment variable */

static object *_SSI_setenv(object *obj)
   {int ok;
    char s[MAXLINE];
    char *vr, *vl;
    object *rv;

    ok = -1;
    vr = NULL;
    vl = NULL;
    SS_args(obj,
            SC_STRING_I, &vr,
            SC_STRING_I, &vl,
            0);

    if (vr != NULL)
       {snprintf(s, MAXLINE, "%s=%s", vr, vl);
	ok = SC_putenv(s);};

    CFREE(vr);
    CFREE(vl);

    rv = SS_mk_integer(ok);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PRINTENV - return a list of the specified environment variables
 *               - somewhat of a misnomer but appeals to familiar printenv
 *               - program
 *               - Usage: (printenv)
 *                        (printenv "HOME" "PWD")
 */

static object *_SSI_printenv(object *argl)
   {int i, n;
    char s[MAXLINE];
    char *vr, **vrs;
    object *l, *v, *vl, *lst, *pr;

/* make a list of names from the global environment frame */
    if (SS_nullobjp(argl))
       vrs = SS_bound_vars("$*", SS_Global_Env);

/* make a list of names from the argument list */
    else
       {n   = SS_length(argl);
	vrs = CMAKE_N(char *, n+1);

	n = 0;
	for (l = argl; !SS_nullobjp(l); l = SS_cdr(l))
	    {v = SS_car(l);
	     SS_args(v,
		     SC_STRING_I, &vr,
		     0);
	     snprintf(s, MAXLINE, "$%s", vr);
	     vrs[n++] = CSTRSAVE(s);

	     CFREE(vr);};

	vrs[n++] = NULL;};

    lst = SS_null;

#if 0

/* print the list of names and variable values */
    dspo = SS_set_display_flag(TRUE);

    for (i = 0; vrs[i] != NULL; i++)
        {vr = vrs[i];
	 vl = _SS_lk_var_valc(vr, SS_Global_Env);
	 snprintf(s, MAXLINE, "%s = ", vr);
	 if (vl == NULL)
	    SS_print(SS_null, s, "\n", SS_outdev);
	 else
	    SS_print(vl, s, "\n", SS_outdev);};

    SS_set_display_flag(dspo);

#endif

/* make a list of variable,value pairs */
    SC_ptr_arr_len(n, vrs);
    for (i = n-1; i >= 0; i--)
        {vr = vrs[i];
	 vl = _SS_lk_var_valc(vr, SS_Global_Env);
	 pr = SS_make_list(SC_STRING_I, vr,
			   SS_OBJECT_I, vl,
			   0);
	 SS_Assign(lst, SS_mk_cons(pr, lst));};

    SC_mark(lst, -1);

    SC_free_strings(vrs);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PRINT_ENV - print the specified environment frame */

static object *_SSI_print_env(object *obj)
   {int i, n;
    object *penv;
    char bf[MAXLINE];

    n = 0;
    SS_args(obj,
            SC_INT_I, &n,
            0);

    penv = SS_Env;
    for (i = 0; (i < n) && !SS_nullobjp(penv); i++, penv = SS_cdr(penv));

    snprintf(bf, MAXLINE, "Environment frame #%d:\n", n+1);
    SS_print(penv, bf, "\n\n", SS_outdev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GET_PNAME - return the full path name of the program running as PID */

static object *_SSI_get_pname(object *obj)
   {int id, rv;
    char path[PATH_MAX];
    object *s;

    id = -1;
    SS_args(obj,
            SC_INT_I, &id,
            0);

    rv = SC_get_pname(path, PATH_MAX, id);
    if (rv == 0)
       s = SS_mk_string(path);
    else
       s = SS_null;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LOAD_AVE - return the load averages for the current host */

static object *_SSI_load_ave(void)
   {int rv;
    double av[3];
    object *s;

    rv = SC_load_ave(av);
    if (rv == TRUE)
       s = SS_make_list(SC_DOUBLE_I, av,
			SC_DOUBLE_I, av+1,
			SC_DOUBLE_I, av+2,
			0);
    else
       s = SS_null;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FREE_MEM - return free memory info */

static object *_SSI_free_mem(void)
   {int rv;
    double mem[2];
    object *s;

    rv = SC_free_mem(mem);
    if (rv == TRUE)
       s = SS_make_list(SC_DOUBLE_I, mem,
			SC_DOUBLE_I, mem+1,
			0);
    else
       s = SS_null;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GET_NCPU - return the number of cpus */

static object *_SSI_get_ncpu(void)
   {int rv;
    object *s;

    rv = SC_get_ncpu();
    s = SS_mk_integer(rv);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FOPEN - fopen in Scheme */

object *_SSI_fopen(object *argl)
   {FILE *str;
    char *name, *mode;
    object *prt;

    name = NULL;
    mode = NULL;
    SS_args(argl,
	    SC_STRING_I, &name,
	    SC_STRING_I, &mode,
	    0);

    if (mode == NULL)
       str = io_open(name, "r");
    else
       str = io_open(name, mode);

    if (str == NULL)
       SS_error("CAN'T OPEN FILE - FOPEN", argl);

    if ((mode != NULL) && (mode[0] == 'r'))
       prt = SS_mk_inport(str, name);
    else
       prt = SS_mk_outport(str, name);

    CFREE(name);
    CFREE(mode);

    return(prt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FCLOSE - fclose in Scheme */

object *_SSI_fclose(object *obj)
   {FILE *str;

    str = NULL;
    if (SS_outportp(obj))
       str = SS_OUTSTREAM(obj);
    else if (SS_inportp(obj))
       str = SS_INSTREAM(obj);
    else
       SS_error("BAD PORT TO FCLOSE", obj);

    if (str != NULL)
       io_close(str);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_WALL_CLOCK_TIME - return the accumulated wall clock time in seconds */

static object *_SSI_wall_clock_time(void)
   {object *ret;

    ret = SS_mk_float(SC_wall_clock_time());

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_USG - return the memory usage info */

static object *_SSI_mem_usg(void)
   {long a, f, d;
    object *ret;

    SC_mem_stats(&a, &f, &d, NULL);

    ret = SS_make_list(SC_INT_I, &a,
		       SC_INT_I, &f,
		       SC_INT_I, &d,
		       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_MAP - wrapper around SC_mem_map */

static object *_SSI_mem_map(object *arg)
   {FILE *fp;

    if (SS_nullobjp(arg))
       arg = SS_outdev;

    else if (!SS_outportp(arg))
       SS_error("BAD PORT - SC_MEM_MAP", arg);

    fp = SS_OUTSTREAM(arg);
    SC_mem_map(fp, FALSE);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_MONITOR - wrapper around SC_mem_monitor */

static object *_SSI_mem_monitor(object *arg)
   {int old, lev;
    long nb;
    char msg[MAXLINE], id[MAXLINE];
    char *s;
    object *o;

    old = 0;
    lev = 0;
    s   = "scheme";
    SS_args(arg,
            SC_INT_I, &old,
            SC_INT_I, &lev,
            SC_STRING_I, &s,
            0);

/* make this local copy to avoid reporting that S leaked */
    strncpy(id, s, MAXLINE);
    CFREE(s);

    nb = SC_mem_monitor(old, lev, id, msg);

    if ((old >= 0) && (msg[0] != '\0'))
       PRINT(stdout, "\n%s\n\n", msg);

    o = SS_mk_integer(nb);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_TRACE - wrapper around SC_mem_check */

static object *_SSI_mem_trace(void)
   {int64_t nb;
    object *o;

    nb = SC_mem_chk(3);
    o  = SS_mk_integer(nb);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEM_CHK - wrapper around SC_mem_check */

static object *_SSI_mem_chk(void)
   {int64_t na, nf, nr;
    object *o;

    na = SC_mem_chk(1);
    nf = SC_mem_chk(2);
    nr = SC_mem_chk(4);
    o  = SS_make_list(SC_INT_I, &na,
		      SC_INT_I, &nf,
		      SC_INT_I, &nr,
		      0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SIZEOF - wrapper around SIZEOF */

static object *_SSI_sizeof(object *arg)
   {int64_t nb;
    char *type;
    object *o;

    type = NULL;
    SS_args(arg,
	    SC_STRING_I, &type,
	    0);

    nb = (type == NULL) ? 0 : SIZEOF(type);

    CFREE(type);

    o = SS_mk_integer(nb);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ATTACH - wrapper around SC_ATTACH_DBG */

static object *_SSI_attach(object *arg)
   {int pid;
    int64_t rv;
    object *o;

    pid = -1;
    SS_args(arg,
	    SC_INT_I, &pid,
	    0);

    rv = SC_attach_dbg(pid);

    o = SS_mk_integer(rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RETRACE - wrapper around SC_RETRACE_DBG */

static object *_SSI_retrace(object *arg)
   {int pid, to;
    int64_t rv;
    object *o;

    pid = -1;
    to  = -1;
    SS_args(arg,
	    SC_INT_I, &pid,
	    SC_INT_I, &to,
	    0);

    rv = SC_retrace_exe(NULL, pid, to);
    o  = SS_mk_integer(rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_INTERACTP - interactive mode predicate */

static object *_SSI_interactp(void)
   {object *o;

    o = SS_interactive ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LOAD_ENV - load the environment variables into the
 *               - global environment
 */

static object *_SSI_load_env(object *arg)
   {

    SS_env_vars(NULL, NULL);

    return(SS_f);}

/*--------------------------------------------------------------------------*/

/*                                INSTALLATION                              */

/*--------------------------------------------------------------------------*/

/* _SS_INST_LRG - install the primitives making up the LARGE Scheme */

void _SS_inst_lrg(void)
   {

    SS_install("attach-debugger",
               "Procedure: attach a debugger to the specified pid",
               SS_nargs,
               _SSI_attach, SS_PR_PROC);

    SS_install("call-stack-trace",
               "Procedure: print out the call stack trace of the specified pid",
               SS_nargs,
               _SSI_retrace, SS_PR_PROC);

    SS_install("wall-clock-time",
               "Procedure: Returns the accumulated wall clock time in seconds",
               SS_zargs,
               _SSI_wall_clock_time, SS_PR_PROC);

    SS_install("define-global",
               "Special Form: defines variables and procedures in the global environment",
               SS_nargs,
               _SSI_define_global, SS_UE_MACRO);

    SS_install("define-global-macro",
               "Special Form: defines variables and procedures in the global environment",
               SS_nargs,
               _SSI_define_global, SS_UE_MACRO);

    SS_install("env",
               "Procedure: print the specified environment frame",
               SS_sargs,
               _SSI_print_env, SS_PR_PROC);

    SS_install("fopen",
               "Procedure: C style file open",
               SS_nargs,
               _SSI_fopen, SS_PR_PROC);

    SS_install("fclose",
               "Procedure: C style file close",
               SS_sargs,
               _SSI_fclose, SS_PR_PROC);

    SS_install("free-mem",
               "Procedure: return the memory and free memory of the current host",
               SS_zargs,
               _SSI_free_mem, SS_PR_PROC);

    SS_install("getcwd",
               "Procedure: return the current working directory",
               SS_zargs,
               _SSI_getcwd, SS_PR_PROC);

    SS_install("getenv",
               "Procedure: return the value of the specified environment variable",
               SS_sargs,
               _SSI_getenv, SS_PR_PROC);

    SS_install("get-ncpu",
               "Procedure: return the number of CPUs on the current host",
               SS_zargs,
               _SSI_get_ncpu, SS_PR_PROC);

    SS_install("get-pid-name",
               "Procedure: return the name of the program running as the given process id",
               SS_nargs,
               _SSI_get_pname, SS_PR_PROC);

    SS_install("interactive?",
	       "Procedure: Return #t iff interactive mode is on\n",
	       SS_zargs,
	       _SSI_interactp, SS_PR_PROC);

    SS_install("list->vector",
               "Procedure: Returns a vector whose elements are the same as the lists",
               SS_sargs,
               SS_lstvct, SS_PR_PROC);

    SS_install("load-ave",
               "Procedure: return the load averages of the current host",
               SS_zargs,
               _SSI_load_ave, SS_PR_PROC);

    SS_install("load-env",
               "Procedure: load environment variables into the global environment",
               SS_znargs,
               _SSI_load_env, SS_PR_PROC);

    SS_install("make-vector",
               "Procedure: Return a new vector whose length is specified by the argument",
               SS_sargs,
               _SSI_mkvect, SS_PR_PROC);

    SS_install("memory-map",
               "Procedure: print a memory map to the given port",
               SS_sargs,
               _SSI_mem_map, SS_PR_PROC);

    SS_install("memory-monitor",
               "Procedure: map out leaks from the heap",
               SS_nargs,
               _SSI_mem_monitor, SS_PR_PROC);

    SS_install("memory-trace",
               "Procedure: return the number of allocated memory blocks",
               SS_zargs,
               _SSI_mem_trace, SS_PR_PROC);

    SS_install("memory-check",
               "Procedure: return the number of allocated, freed, and registered memory blocks",
               SS_zargs,
               _SSI_mem_chk, SS_PR_PROC);

    SS_install("memory-usage",
               "Procedure: Returns the number of bytes allocated, freed, and diff",
               SS_zargs,
               _SSI_mem_usg, SS_PR_PROC);

    SS_install("printenv",
               "Procedure: print the environment variables specified",
               SS_nargs,
               _SSI_printenv, SS_PR_PROC);

    SS_install("setenv",
               "Procedure: set the value of the specified environment variable",
               SS_nargs,
               _SSI_setenv, SS_PR_PROC);

    SS_install("sizeof",
               "Procedure: Returns the byte size of the data type named by its argument",
               SS_sargs,
               _SSI_sizeof, SS_PR_PROC);

    SS_install("sleep",
               "Procedure: Sleep n milliseconds",
               SS_sargs,
               _SSI_sleep, SS_PR_PROC);

    SS_install("vector?",
               "Procedure: Returns #t iff the object is of type vector",
               SS_sargs,
               _SSI_vectp, SS_PR_PROC);

    SS_install("vector",
               "Procedure: Analog to list procedure for vectors",
               SS_nargs,
               _SSI_vector, SS_PR_PROC);

    SS_install("vector-length",
               "Procedure: Returns the number of elements in the specified vector",
               SS_sargs,
               _SSI_vctlen, SS_PR_PROC);

    SS_install("vector->list",
               "Procedure: Returns a list whose elements are the same as the vectors",
               SS_sargs,
               SS_vctlst, SS_PR_PROC);

    SS_install("vector-ref",
               "Procedure: Returns the nth element of the given vector",
               SS_nargs,
               _SSI_vctref, SS_PR_PROC);

    SS_install("vector-set!",
               "Procedure: Sets the nth element of the given vector",
               SS_nargs,
               _SSI_vctset, SS_PR_PROC);

    SS_install_cf("interactive",
                  "Variable: Controls display of ouput data in functions\n     Usage: interactive <on|off>",
                  SS_acc_int,
                  &SS_interactive);

    SS_install_cf("lines-page",
                  "Variable: Controls the number of lines per page for selected printing commands\n     Usage: lines-page <integer>",
                  SS_acc_int,
                  &SS_lines_page);

    SS_install_cf("trace-env",
                  "Variable: Show upto <n> variables in environment frames entering function calls\n     Usage: trace-env <n>",
                  SS_acc_int,
                  &SS_trace_env);

    _SS_inst_hash();
    _SS_inst_str();
    _SS_inst_chr();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
