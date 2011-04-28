/*
 * SHTLEV.C - top level of scheme interpreter
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

#define SS_PRINT_ERR_MSG (*_SS.pr_err)

char
 *SS_OBJECT_S,
 *SS_POBJECT_S;

object
 *SS_anon_proc,
 *SS_anon_macro,
 *SS_block_proc,
 *SS_scheme_symtab,
 *SS_quoteproc,
 *SS_quasiproc,
 *SS_unqproc,
 *SS_unqspproc,
 *SS_setproc,
 *SS_null,
 *SS_eof,
 *SS_t,
 *SS_f,
 *SS_else;

psides
 _SS_si;

SS_state
 _SS = { -1, 1, -1, 1, };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FPE_HANDLER - handle floating point exception signals */

static void _SS_fpe_handler(int sig)
   {

#ifdef SIGFPE
    SC_signal(SIGFPE, _SS_fpe_handler);
#endif

    SS_error("FLOATING POINT EXCEPTION - _SS_FPE_HANDLER",
	     SS_mk_cons(_SS_si.fun, _SS_si.argl));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SIG_HANDLER - handle various signals */

static void _SS_sig_handler(int sig)
   {char msg[MAXLINE];

    SC_signal(sig, SIG_IGN);

    snprintf(msg, MAXLINE, "%s - _SS_SIG_HANDLER", SC_signal_name(sig));

    PRINT(stdout, "\n%s\n", msg);
    SC_retrace_exe(NULL, -1, 120000);

#if 1
    SS_end_scheme(sig);
#else
    {object *o;

     o = SS_mk_cons(_SS_si.fun, _SS_si.argl);

     SC_signal(sig, _SS_sig_handler);

     SS_error(msg, o);}
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PRINT_ERR_MSG - default error message print function for
 *                   - _SS.pr_err
 */

static void _SS_print_err_msg(FILE *str, char *s, object *obj)
   {char atype[MAXLINE];
    char *p;

    if (obj == NULL)
       PRINT(str, "(%d):  ERROR: %s\n", _SS_si.errlev, s);

    else
       {PRINT(str, "(%d):  ERROR: %s\n      BAD OBJECT (", _SS_si.errlev, s);

	p = SS_object_type_name(obj, atype);
	if (p == NULL)
	   {switch (SC_arrtype(obj, -1))
	       {default :
		     PRINT(str, "unknown");
		     break;

	        case '\0' :
                     PRINT(str, "0x0): POINTER 0x%lx IS FREE\n\n",
			   (long) (obj - (object *) NULL));
		     return;};};

	PRINT(str, atype);

	SS_print(obj, "): ", "\n\n", _SS_si.outdev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PRINT_ERR_MSG_A - alternate error message print function for
 *                     - _SS.pr_err
 */

static void _SS_print_err_msg_a(FILE *str, char *s, object *obj)
   {PRINT(str, "ERROR : %s : ", s);

    if (SC_arrtype(obj, -1) == 0)
       PRINT(str, "MEMORY PROBABLY CORRUPTED\n");
    else
       SS_print(obj, "", "\n", _SS_si.outdev);

    SS_print(_SS_si.fun, "CURRENT FUNCTION: ", "\n\n", _SS_si.outdev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_PRINT_ERR_FUNC - set the _SS.pr_err and
 *                       - return the old value
 *                       - if DFLT is TRUE NULL value results in the
 *                       - default hook being assigned
 */

PFPrintErrMsg SS_set_print_err_func(PFPrintErrMsg fnc, int dflt)
   {PFPrintErrMsg rv;

    rv = _SS.pr_err;

    if ((dflt == TRUE) && (fnc == NULL))
       _SS.pr_err = _SS_print_err_msg_a;
    else
       _SS.pr_err = fnc;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_PRINT_ERR_FUNC - return the _SS.pr_err */

PFPrintErrMsg SS_get_print_err_func(void)
   {PFPrintErrMsg rv;

    rv = _SS.pr_err;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_REPL - run a READ-EVAL-PRINT Loop */

static int _SS_repl(void)
   {long a, f, d;
    double evalt;

    SS_Assign(_SS_si.rdobj, SS_null);
    SS_Assign(_SS_si.evobj, SS_null);

    _SS_set_ans_prompt();

    d = 0L;
    while (TRUE)
       {SC_mem_stats_set(0L, 0L);
        _SS_si.ngoc       = 0;
        _SS_si.nsetc      = 0;
        _SS_si.nsave      = 0;
        _SS_si.nrestore   = 0;

/* print the prompt call the Reader */
	_SS.pr_prompt = _SS_si.prompt;
        SS_Assign(_SS_si.rdobj, SS_read(_SS_si.indev));

        if (_SS_si.post_read != NULL)
           (*_SS_si.post_read)(_SS_si.indev);

        _SS_si.interactive = TRUE;

/* eval the object returned by the Reader */
	evalt = SC_cpu_time();
        SS_Assign(_SS_si.evobj, SS_eval(_SS_si.rdobj));
	evalt = SC_cpu_time() - evalt;

        if (_SS_si.post_eval != NULL)
           (*_SS_si.post_eval)(_SS_si.indev);

/* print the evaluated object */
        if (_SS_si.print_flag)
           SS_print(_SS_si.evobj, _SS_si.ans_prompt, "\n", _SS_si.outdev);

        SS_Assign(_SS_si.env,   _SS_si.global_env);
        SS_Assign(_SS_si.this,  SS_null);
        SS_Assign(_SS_si.exn,   SS_null);
        SS_Assign(_SS_si.val,   SS_null);
        SS_Assign(_SS_si.unev,  SS_null);
        SS_Assign(_SS_si.argl,  SS_null);
        SS_Assign(_SS_si.fun,   SS_null);
        SS_Assign(_SS_si.rdobj, SS_null);
        SS_Assign(_SS_si.evobj, SS_null);

/* restore the global environment */
	SC_mem_stats(&a, &f, NULL, NULL);
        d += a - f;
        if (_SS_si.stat_flag)
           {PRINT(stdout, "Stack Usage (S/R): (%d/%d)",
                  _SS_si.nsave, _SS_si.nrestore);
            PRINT(stdout, "   Continuations (S/G): (%d/%d)\n",
                  _SS_si.nsetc, _SS_si.ngoc);
            PRINT(stdout, "Memory Usage(A/F): (%ld/%ld)",
                  a, f);
            PRINT(stdout, "   Net (A-F): (%ld)",
		  d);
            PRINT(stdout, "   Time: (%10.3e)\n",
		  evalt);};

        if (_SS_si.post_print != NULL)
           (*_SS_si.post_print)();
        else
           PRINT(stdout, "\n");}

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_REPL - run a READ-EVAL-PRINT Loop */

void SS_repl(void)
   {char *t;

    while (TRUE)
       {SS_err_catch(_SS_repl, NULL);

/* reset the input buffer */
        t = SS_BUFFER(_SS_si.indev);
        SS_PTR(_SS_si.indev) = t;
        *t = '\0';};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_END_SCHEME - gracefully exit from Scheme */

void SS_end_scheme(int val)
   {int ev;

    if (!SS_nullobjp(_SS_si.histdev))
       SS_trans_off();

    if (_SS.active_objects == TRUE)
       _SS_object_map(stdout, TRUE);

    switch (val)
       {case ERR_FREE :
	     ev = _SS.exit_val;
	     break;

        case ABORT :
        default    :
	     ev = SC_EXIT_SELF;
	     break;};

    exit(ev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_QUIT - exit from Scheme */

static object *_SSI_quit(object *arg)
   {

    _SS.exit_val = 0;
    SS_args(arg,
            SC_INT_I, &_SS.exit_val,
            0);

    LONGJMP(SC_gs.cpu, ERR_FREE);

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_SCHEME_ENV - set the SCHEME environment variable if
 *                   - not already set
 *                   - return TRUE iff successful
 */

int SS_set_scheme_env(char *exepath, char *path)
   {int rv, err, nc, ok;
    char rpath[PATH_MAX];
    char *s, *npath;

    rv = TRUE;
    s  = getenv("SCHEME");
    nc = PATH_MAX - 1;

    if (s == NULL)
       {ok = SC_full_path(exepath, rpath, nc);
	if (ok == 0)
           {npath = SC_dsnprintf(TRUE, "SCHEME=%s", rpath);
	    SC_lasttok(npath, "/");
	    SC_lasttok(npath, "/");
	    npath = SC_dstrcat(npath, "/scheme");

            if (path != NULL)
	       {npath = SC_dstrcat(npath, ":");
		npath = SC_dstrcat(npath, path);};

            err = SC_putenv(npath);
            rv  = (err == 0);

	    CFREE(npath);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SCHEME_PATH_ERR - report missing SCHEME file */

void SS_scheme_path_err(char *path)
   {char *s;

    s = NULL;

#ifndef WIN32
    s = getenv("SCHEME");
#endif

    if (s != NULL)
       {PRINT(stdout, "File %s not found in:\n", path);
	PRINT(stdout, "   %s\n", s);
	PRINT(stdout, "Check your SCHEME environment variable\n");}

    else
       {PRINT(stdout, "File %s not found\n", path);
	PRINT(stdout, "Your SCHEME environment variable is not set\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_SCHEME - initialize the interpreter */

void SS_init_scheme(char *code, char *vers)
   {object *fr;
    PFSignal_handler hnd;

    hnd = SC_which_signal_handler(SIGINT);
    SC_setup_sig_handlers(_SS_sig_handler, TRUE);
    PM_enable_fpe(TRUE, (PFSignal_handler) _SS_fpe_handler);
    SC_signal(SIGINT, hnd);

#ifdef SIGFPE
    SC_signal(SIGFPE, _SS_fpe_handler);
#endif

#ifdef LARGE
    _SS_si.stack_size = 128;
#else
    _SS_si.stack_size = 32;
#endif
    _SS_si.stack_mask = _SS_si.stack_size - 1;

    SS_register_types();

    SS_inst_prm();
    SS_inst_const();

    _SS_si.trap_error = TRUE;
    _SS_si.err_state  = SS_null;
    _SS_si.env        = SS_null;

    fr            = SS_mk_new_frame(SS_mk_string("global-environment"), NULL);
    _SS_si.global_env = SS_mk_cons(fr, SS_null);
    SS_UNCOLLECT(_SS_si.global_env);

    SS_Assign(_SS_si.env, _SS_si.global_env);

    SS_define_constant(1,
		       "system-id", SC_STRING_I, SYSTEM_ID,
		       "argv",      SS_OBJECT_I, SS_null,
		       NULL);

    _SS_si.this  = SS_null;
    _SS_si.exn   = SS_null;
    _SS_si.val   = SS_null;
    _SS_si.unev  = SS_null;
    _SS_si.argl  = SS_null;
    _SS_si.fun   = SS_null;
    _SS_si.rdobj = SS_null;
    _SS_si.evobj = SS_null;

/* give default values to the lisp package interface variables  */
    _SS_si.post_read  = NULL;
    _SS_si.post_eval  = NULL;
    _SS_si.post_print = NULL;
    _SS_si.pr_ch_un        = SS_unget_ch;
    _SS_si.pr_ch_out       = SS_put_ch;

    SC_set_put_line(SS_printf);
    SC_set_put_string(SS_fputs);

#ifdef NO_SHELL
    SC_set_get_line(PG_wind_fgets);
#else
    SC_set_get_line(io_gets);
#endif

    _SS_si.interactive = TRUE;
    _SS_si.lines_page  = 50;
    _SS_si.print_flag  = TRUE;
    _SS_si.stat_flag   = TRUE;
    _SS_si.nsave       = 0;
    _SS_si.nrestore    = 0;
    _SS_si.nsetc       = 0;
    _SS_si.ngoc        = 0;

    SC_mem_stats_set(0L, 0L);

    SC_set_banner(" %s  -  %s\n\n", code, vers);

    SS_set_print_err_func(_SS_print_err_msg, FALSE);

    SC_init_path(2, "HOME", "SCHEME");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_EXT_REF - get a reference to a SCHEME level function NAME out
 *                - at the C level
 */

static object *SS_get_ext_ref(char *name)
   {char uname[MAXLINE];
    haelem *hp;
    object *o;
   
    hp = SC_hasharr_lookup(_SS_si.symtab, name);

    if (hp == NULL)
       {strcpy(uname, name);
	SC_str_upper(uname);
	PRINT(ERRDEV, "Error initializing %s\n", uname);
        LONGJMP(SC_gs.cpu, ABORT);};  

    o = (object *) hp->def;

    SS_UNCOLLECT(o);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MAKE_EXT_BOOLEAN - install a boolean in the symbol table and
 *                     - get a reference at the C level
 */

static object *SS_make_ext_boolean(char *name, int val)
   {object *o;
   
    o = SS_mk_boolean(name, val);

    SC_hasharr_install(_SS_si.symtab, name, o, SS_POBJECT_S, TRUE, TRUE);

    SS_UNCOLLECT(o);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INST_CONST - install Scheme constants */

void SS_inst_const(void)
   {

    SS_OBJECT_S  = CSTRSAVE("object");
    SS_POBJECT_S = CSTRSAVE("object *");

    SS_quoteproc = SS_get_ext_ref("quote");
    SS_quasiproc = SS_get_ext_ref("quasiquote");
    SS_unqproc   = SS_get_ext_ref("unquote");
    SS_unqspproc = SS_get_ext_ref("unquote-splicing");
    SS_setproc   = SS_get_ext_ref("set!");

    SS_null = SS_make_ext_boolean("nil",  FALSE);
    SS_eof  = SS_make_ext_boolean("#eof", TRUE);
    SS_t    = SS_make_ext_boolean("#t",   TRUE);
    SS_f    = SS_make_ext_boolean("#f",   FALSE);
    SS_else = SS_make_ext_boolean("else", TRUE);

    SC_arrtype(SS_null, SS_NULL_I);
    SC_arrtype(SS_eof, SS_EOF_I);
    
    _SS_si.histdev = SS_null;
    _SS_si.indev   = SS_mk_inport(stdin, "stdin");
    SS_UNCOLLECT(_SS_si.indev);
    _SS_si.outdev  = SS_mk_outport(stdout, "stdout");
    SS_UNCOLLECT(_SS_si.outdev);

    SS_anon_proc  = SS_mk_string("lambda");
    SS_UNCOLLECT(SS_anon_proc);
    SS_anon_macro = SS_mk_string("lambda-macro");
    SS_UNCOLLECT(SS_anon_macro);
    SS_block_proc = SS_mk_string("block");
    SS_UNCOLLECT(SS_block_proc);

/* initialize the stack and the continuation stack */
    SS_init_stack();
    SS_init_cont();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_STACK - rewind the stack to the beginning */

void SS_init_stack(void)
   {

    _SS_si.stack = CMAKE_ARRAY(object *, NULL, 0);

    _SS_si.nsave    = 0;
    _SS_si.nrestore = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_CONT - rewind the continuation stack to the beginning */

void SS_init_cont(void)
   {int i;

    _SS_si.nsetc = 0;
    _SS_si.ngoc  = 0;

    _SS_si.continue_int = CMAKE_N(continuation, _SS_si.stack_size);
    if (_SS_si.continue_int == NULL)
       LONGJMP(SC_gs.cpu, ABORT);
    for (i = 0; i < _SS_si.stack_size; _SS_si.continue_int[i++].signal = SS_null);
    _SS_si.cont_ptr = 0;

    _SS_si.continue_err = CMAKE_N(err_continuation, _SS_si.stack_size);
    if (_SS_si.continue_err == NULL)
       LONGJMP(SC_gs.cpu, ABORT);
    for (i = 0; i < _SS_si.stack_size; _SS_si.continue_err[i++].signal = SS_null);
    _SS_si.err_cont_ptr = 0;

    _SS_si.err_stack = CMAKE_N(object *, _SS_si.stack_size);
    if (_SS_si.err_stack == NULL)
       LONGJMP(SC_gs.cpu, ABORT);
    for (i = 0; i < _SS_si.stack_size; _SS_si.err_stack[i++] = NULL);
    _SS_si.errlev = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EXPAND_STACK - double the size of the continuation and err stacks */

void SS_expand_stack(void)
   {int i, size;

    size = _SS_si.stack_size;
    _SS_si.stack_size = 2*size;

    CREMAKE(_SS_si.continue_int, continuation, _SS_si.stack_size);
    for (i = size; i < _SS_si.stack_size; _SS_si.continue_int[i++].signal = SS_null);

    CREMAKE(_SS_si.continue_err, err_continuation, _SS_si.stack_size);
    for (i = size; i < _SS_si.stack_size; _SS_si.continue_err[i++].signal = SS_null);

    CREMAKE(_SS_si.err_stack, object *, _SS_si.stack_size);
    for (i = size; i < _SS_si.stack_size; _SS_si.err_stack[i++] = NULL);

    return;}

/*--------------------------------------------------------------------------*/

/*                      SYMBOL TABLE MANIPULATORS                           */

/*--------------------------------------------------------------------------*/

/* _SSI_SYNONYM - make synonyms for the given function */

static object *_SSI_synonym(object *argl)
   {object *func;
    char *synname;

    func = SS_exp_eval(SS_car(argl));
    if (!SS_procedurep(func))
       SS_error("FIRST ARG MUST BE FUNCTION - _SSI_SYNONYM", func);

    for (argl = SS_cdr(argl); SS_consp(argl); argl = SS_cdr(argl))
        {synname = SS_get_string(SS_car(argl));
         SC_hasharr_remove(_SS_si.symtab, synname);
         SC_hasharr_install(_SS_si.symtab, synname, func, SS_OBJECT_S, TRUE, TRUE);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOOKUP_OBJECT - interpreter level call to get an object in the
 *                  - environment given only its name
 *                  - Usage:  (string->object <string>)
 */

object *SS_lookup_object(object *obj)
   {char *name, *vnm;

/* take anything that will give a name - procedure, string, variable ... */
    name = NULL;
    SS_args(obj,
	    SC_STRING_I, &name,
	    0);

    obj = SS_bound_name(name);

    CFREE(name);

    if (SS_variablep(obj))
       {vnm = SS_VARIABLE_NAME(obj);
	obj = _SS_lk_var_valc(vnm, _SS_si.env);
	if (obj == NULL)
	   obj = SS_null;};

    return(obj);}

/*--------------------------------------------------------------------------*/

/*                              ERROR HANDLERS                              */

/*--------------------------------------------------------------------------*/

/* SS_PUSH_ERR - push an error stack frame */

void SS_push_err(int flag, int type)
   {object *x;

    SS_save_registers(flag);

    x = SS_mk_esc_proc(_SS_si.errlev, type);
    SS_MARK(x);
    _SS_si.err_stack[_SS_si.errlev] = x;

    _SS_si.errlev = (_SS_si.errlev + 1) & _SS_si.stack_mask;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_POP_ERR - pop an error stack frame and restore the state of the
 *            - interpreter to the time that the frame was pushed
 *            - return the escape procedure object associated with the
 *            - correct error stack frame
 */

object *SS_pop_err(int n, int flag)
   {object *x;

    if (_SS_si.errlev < 1)
       {PRINT(stdout, "\nERROR: ERROR STACK BLOWN - SS_POP_ERR\n\n");
        exit(SC_EXIT_CORRUPT);};

/* GC the other error stack frames */
    while (TRUE)
       {x = _SS_si.err_stack[--_SS_si.errlev];
        _SS_si.err_stack[_SS_si.errlev] = NULL;
        if (_SS_si.errlev <= n)
           {_SS_restore_state(x);
            break;}
        else
           SS_GC(x);};

    SS_restore_registers(flag);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RESTORE_STATE_PRIM - do the actual work of restoring the state
 *                        - of the interpreter
 */

static void _SS_restore_state_prim(int ns, int nc, int ne)
   {int n;
    object *x, *esc;

    n = SC_array_get_n(_SS_si.stack) - 1;

/* restore the stack */
    if (n < ns)
       SS_error("CORRUPT STACK FRAME - _SS_RESTORE_STATE_PRIM", SS_null);

    for (; n > ns; n--)
        {_SS_si.nrestore++;

         x = SC_array_pop(_SS_si.stack);

         SS_GC(x);};

/* restore the continuation stack */
    if (_SS_si.cont_ptr < nc)
       SS_error("CORRUPT CONTINUATION FRAME - _SS_RESTORE_STATE_PRIM",
                SS_null);

    for (; _SS_si.cont_ptr > nc; _SS_si.cont_ptr--)
        {_SS_si.ngoc++;
         SS_Assign(_SS_si.continue_int[_SS_si.cont_ptr].signal, SS_null);};

/* restore the error stack */
    if (_SS_si.errlev < ne)
       SS_error("CORRUPT ERROR FRAME - _SS_RESTORE_STATE_PRIM", SS_null);

    for (; _SS_si.errlev > ne; _SS_si.errlev--)
        {esc = _SS_si.err_stack[_SS_si.errlev-1];
         _SS_si.err_stack[_SS_si.errlev-1] = NULL;
         SS_GC(esc);};

    _SS_set_ans_prompt();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RESTORE_STATE - restore the state of Scheme to that
 *                   - implied by the given escape procedure
 */

void _SS_restore_state(object *esc_proc)
   {int ns, nc, ne;

    ns = SS_ESCAPE_STACK(esc_proc);
    nc = SS_ESCAPE_CONTINUATION(esc_proc);
    ne = SS_ESCAPE_ERROR(esc_proc);

    _SS_restore_state_prim(ns, nc, ne);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_BREAK - enter a Scheme break from the Scheme level
 *            - return with return-level or reset
 *            - create a higher level REPL and push on
 */

static object *_SSI_break(void)
   {

    SS_Save(_SS_si.evobj);
    SS_Save(_SS_si.rdobj);

    SS_push_err(TRUE, SS_ERROR_I);
    PRINT(stdout,"\n");

    _SS_repl();

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RESET - unwind the error/break stack and return to top level */

static object *_SSI_reset(void)
   {

    _SS_restore_state_prim(0, 1, 0);
    PRINT(stdout,"\n");

    LONGJMP(_SS_si.continue_int[1].cont, ABORT);
/*    LONGJMP(_SS_si.continue_int[1].cont, RETURN_OK); */

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RETLEV - pops n levels off the error/break stack and
 *             - returns the second arg as value
 *             - GOTCHA: there are residual GC issues here
 */

static object *_SSI_retlev(object *argl)
   {int n;
    object *x, *expr, *val;

    x    = SS_car(argl);
    argl = SS_cdr(argl);
    if (!SS_integerp(x))
       SS_error("FIRST ARG MUST BE AN INTEGER - _SSI_RETLEV", x);

    n = (int) SS_INTEGER_VALUE(x);
    n = _SS_si.errlev - n;
    n = max(1, n);

    if (!SS_consp(argl))
       SS_error("SECOND ARG MISSING - _SSI_RETLEV", x);
    val = SS_car(argl);    

    if (_SS_si.errlev > 1)
       {x = SS_pop_err(n, TRUE);

        SS_Restore(_SS_si.rdobj);
        SS_Restore(_SS_si.evobj);

	expr = SS_make_form(x, SS_make_form(SS_quoteproc, val, LAST), LAST);
        x    = SS_exp_eval(expr);}

    else
       x = SS_f;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INTERRUPT_HANDLER - handle interrupts for SCHEME */

void SS_interrupt_handler(int sig)
   {char bf[MAXLINE], *cmnd, *arg, *t;
    int nl;
    object *argl;

    SC_signal(SIGINT, SS_interrupt_handler);

    PRINT(stdout, "\n\nInterrupt (%d frames):\n", _SS_si.errlev - 1);
    PRINT(stdout, "  a     - Reset to starting frame\n");
    PRINT(stdout, "  b     - Enter SCHEME break\n");
    PRINT(stdout, "  r     - Resume from here\n");
    PRINT(stdout, "  u <n> - Return n levels\n");
    PRINT(stdout, "  q     - Quit\n");
    PRINT(stdout, "\nI-> ");
    GETLN(bf, MAXLINE, stdin);
    cmnd = SC_strtok(bf, " \t\n\r", t);
    arg  = SC_strtok(bf, " \t\n\r", t);

    if (cmnd == NULL)
       return;

#ifndef NO_SHELL
    io_flush(stdout);
#endif

    switch (cmnd[0])
       {case 'r' :
	     PRINT(stdout, "\nResuming\n\n");
	     break;

        case 'a' :
	     PRINT(stdout, "\nResetting\n\n");
	     _SSI_reset();
	     break;

        case 'b' :
	     PRINT(stdout, "\nEntering SCHEME break\n\n");
	     t = SS_BUFFER(_SS_si.indev);
	     SS_PTR(_SS_si.indev) = t;
	     *t = '\0';
	     _SSI_break();
	     break;

        case 'u' :
	     nl = SC_stoi(arg);
	     PRINT(stdout, "\nReturning %d frames\n\n", 2*nl);
	     argl = SS_mk_cons(SS_mk_integer(nl),
			       SS_mk_cons(SS_t, SS_null));
	     _SSI_retlev(argl);
	     break;

        default : 
        case 'q' :
	     PRINT(stdout, "\nExiting\n\n");
	     exit(SC_EXIT_SELF);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ERR_CATCH - execute the function specified by the first argument
 *              - in an environment which catches errors from SS_error
 *              - if there is an error execute the function specified by
 *              - the second argument
 *              - return TRUE if the ABORT branch is not taken
 */

int SS_err_catch(PFInt func, PFInt errf)
   {object *esc;

    _SS.ret = TRUE;
    _SS_si.cont_ptr++;
    SS_push_err(FALSE, SS_ERROR_I);
    switch (SETJMP(_SS_si.continue_int[_SS_si.cont_ptr].cont))
       {case ABORT :
	     if (errf != NULL)
	        _SS.ret = (*errf)();
	     else
	        _SS.ret = FALSE;
	     break;

        case RETURN_OK :
	     break;

        default :
	     if (func != NULL)
	        _SS.ret = (*func)();

        case ERR_FREE :
	     esc = SS_pop_err(_SS_si.errlev - 1, FALSE);
	     SS_GC(esc);};

    _SS_si.cont_ptr--;

    return(_SS.ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ERROR - signal an error
 *          - create a higher level REPL and push on
 */

void SS_error(char *s, object *obj)
   {int nc;
    char *t;
    FILE *str;
    object *esc;

    t = CSTRSAVE(s);

    SS_Assign(_SS_si.err_state, SS_make_list(SS_OBJECT_I, _SS_si.fun,
					 SS_OBJECT_I, _SS_si.argl,
					 SS_OBJECT_I, obj,
					 SC_STRING_I, t,
					 0));

    str = SS_OUTSTREAM(_SS_si.outdev);
    if (_SS.pr_err != NULL)
       SS_PRINT_ERR_MSG(str, t, obj);

    CFREE(t);

    t = SS_BUFFER(_SS_si.indev);
    SS_PTR(_SS_si.indev) = t;
    *t = '\0';

    esc = SS_pop_err(_SS_si.errlev - 1, FALSE);
    nc  = SS_ESCAPE_CONTINUATION(esc);
    SS_GC(esc);

    if (_SS_si.trap_error)
       LONGJMP(_SS_si.continue_int[nc].cont, ABORT);

    else
       LONGJMP(SC_gs.cpu, ABORT);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_OBJECT_TYPE_NAME - return the ASCII name of the type of
 *                     - the specified object O
 */

char *SS_object_type_name(object *o, char *atype)
   {int itype;
    char *p;

    itype = SC_arrtype(o, -1);
    p     = atype;

    if (itype == SC_INT_I)
       strcpy(atype, SC_INT_S);

    else if (itype == SC_FLOAT_I)
       strcpy(atype, SC_FLOAT_S);

    else if (itype == SC_STRING_I)
       strcpy(atype, "string");

    else if (itype == SS_CONS_I)
       strcpy(atype, "pair");

    else if (itype == SS_VARIABLE_I)
       strcpy(atype, "variable");

    else if (itype == SS_PROCEDURE_I)
       strcpy(atype, "procedure");

    else if (itype == SC_BOOL_I)
       strcpy(atype, "boolean");

    else if (itype == SS_INPUT_PORT_I)
       strcpy(atype, "input port");

    else if (itype == SS_OUTPUT_PORT_I)
       strcpy(atype, "output port");

    else if (itype == SS_EOF_I)
       strcpy(atype, "eof");

    else if (itype == SS_NULL_I)
       strcpy(atype, "nil");

    else if (itype == SS_VECTOR_I)
       strcpy(atype, "vector");

    else if (itype == SS_CHARACTER_I)
       strcpy(atype, SC_CHAR_S);

    else if (itype == SS_HAELEM_I)
       strcpy(atype, "hash element");

    else if (itype == SS_HASHARR_I)
       strcpy(atype, "hash array");

    else if (itype == SS_PROCESS_I)
       strcpy(atype, "process");

    else if (itype == SS_ERROR_I)
       strcpy(atype, "error object");

    else
       p = NULL;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SYSTEM - pass command to operating system from Scheme
 *             - timeout after optionally specified interval
 *             - return the exit status of the command
 */

static object *_SSI_system(object *argl)
   {int rv, to;
    char *cmd;
    object *o;

    rv  = -1;
    cmd = NULL;
    to  = -1;
    SS_args(argl,
	    SC_STRING_I,  &cmd,
	    SC_INT_I, &to,
	    0);

    if (to > 0)
       {if (SC_time_allow(to) == 0)
	   {rv = SYSTEM(cmd);
	    SC_time_allow(0);}
        else
	   PRINT(stdout, "'%s' timed out after %d seconds\n", cmd, to);}

    else
       rv = SYSTEM(cmd);

    CFREE(cmd);

    o = SS_mk_integer(rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SYSCMND - pass command to operating system from Scheme, return
 *              - output in a list
 */

static object *_SSI_syscmnd(object *argl)
   {int i, to;
    char *cmd, **output;
    object *lst;

    output = NULL;
    cmd    = NULL;
    to     = -1;
    SS_args(argl,
	    SC_STRING_I,  &cmd,
	    SC_INT_I, &to,
	    0);

    if (to > 0)
       {if (SC_time_allow(to) == 0)
	   {output = SC_syscmnd(cmd);
	    SC_time_allow(0);};}

    else
       output = SC_syscmnd(cmd);

    CFREE(cmd);

    lst = SS_null;
    if (output != NULL)
       {for (i = 0; output[i] != NULL; i++)
            {SS_Assign(lst, SS_mk_cons(SS_mk_string(output[i]), lst));};

        SS_Assign(lst, SS_reverse(lst));
	SC_mark(lst, -1);

        SC_free_strings(output);}

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INST_PRM - install the Scheme primitives */

void SS_inst_prm(void)
   {

    if (_SS_si.symtab == NULL)
       _SS_si.symtab = SC_make_hasharr(HSZHUGE, DOC, SC_HA_NAME_KEY);

    SS_install("break",
               "Procedure: enter a Scheme break, return with return-level",
               SS_zargs,
               _SSI_break, SS_PR_PROC);

    SS_install("defined?",
               "Special Form: returns #t if its argument has been defined in the current environment",
               SS_sargs,
               SS_defp, SS_UR_MACRO);

    SS_install("display-object-table",
               "Procedure: Prints information about all known objects",
               SS_zargs,
               SS_pr_obj_map, SS_PR_PROC);

    SS_install("list",
               "Return a new list made up of the arguments",
               SS_nargs, 
               SS_list, SS_PR_PROC);

    SS_install("quit",
               "Procedure: Exit from Scheme",
               SS_znargs,
               _SSI_quit, SS_PR_PROC);

    SS_install("reset",
               "Procedure: unwinds the Error/Break stack and returns to top level",
               SS_zargs,
               _SSI_reset, SS_PR_PROC);

    SS_install("return-level",
               "Procedure: pops n levels off the Error/Break stack and returns the second arg as value",
               SS_nargs,
               _SSI_retlev, SS_PR_PROC);

    SS_install("string->object",
               "Returns the object named by the given string",
               SS_sargs, 
               SS_lookup_object, SS_PR_PROC);

    SS_install("synonym",
               "Macro: Define synonyms for the given function",
               SS_nargs,
               _SSI_synonym, SS_UR_MACRO);

    SS_install("syscmnd",
               "Procedure: process the given string as a command line-return results in list",
               SS_nargs,
               _SSI_syscmnd, SS_PR_PROC);

    SS_install("system",
               "Procedure: invokes the operation system to process the given string as a command line",
               SS_nargs,
               _SSI_system, SS_PR_PROC);

#ifdef LARGE
    _SS_inst_lrg();
    _SS_inst_proc();
#endif

    _SS_inst_eval();
    _SS_inst_read();
    _SS_inst_print();
    _SS_inst_prm1();
    _SS_install_math();
    _SS_install_complex();
    _SS_install_quaternion();
    _SS_inst_prm3();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
