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

continuation
 *SS_continue;

err_continuation
 *SS_err_continue;

char
 *SS_OBJECT_S,
 *SS_POBJECT_S;

PFPostRead
 SS_post_read_hook;

PFPostEval
 SS_post_eval_hook;

PFPostPrint
 SS_post_print_hook;

int
 SS_interactive,
 SS_trap_error,
 SS_lines_page,
 SS_print_flag,
 SS_stat_flag,
 SS_nsave,
 SS_nrestore,
 SS_nsetc,
 SS_ngoc,
 SS_stack_size,
 SS_stack_mask,
 SS_cont_ptr,
 SS_err_cont_ptr,
 SS_errlev;

hasharr
 *SS_symtab;

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
 *SS_else,
 *SS_This,
 *SS_Val,
 *SS_Unev,
 *SS_Exn,
 *SS_Argl,
 *SS_Fun,
 *SS_Env,
 *SS_Global_Env,
 *SS_err_state,
 **SS_err_stack,
 *SS_rdobj,
 *SS_evobj;

SC_array
 *SS_stack;

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
	     SS_mk_cons(SS_Fun, SS_Argl));

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

     o = SS_mk_cons(SS_Fun, SS_Argl);

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
       PRINT(str, "(%d):  ERROR: %s\n", SS_errlev, s);

    else
       {PRINT(str, "(%d):  ERROR: %s\n      BAD OBJECT (", SS_errlev, s);

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

	SS_print(obj, "): ", "\n\n", SS_outdev);};

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
       SS_print(obj, "", "\n", SS_outdev);

    SS_print(SS_Fun, "CURRENT FUNCTION: ", "\n\n", SS_outdev);

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

    SS_Assign(SS_rdobj, SS_null);
    SS_Assign(SS_evobj, SS_null);

    _SS_set_ans_prompt();

    d = 0L;
    while (TRUE)
       {SC_mem_stats_set(0L, 0L);
        SS_ngoc       = 0;
        SS_nsetc      = 0;
        SS_nsave      = 0;
        SS_nrestore   = 0;

/* print the prompt call the Reader */
        PRINT(stdout, "%s", SS_prompt);
        SS_Assign(SS_rdobj, SS_read(SS_indev));

        if (SS_post_read_hook != NULL)
           (*SS_post_read_hook)(SS_indev);

        SS_interactive = TRUE;

/* eval the object returned by the Reader */
	evalt = SC_cpu_time();
        SS_Assign(SS_evobj, SS_eval(SS_rdobj));
	evalt = SC_cpu_time() - evalt;

        if (SS_post_eval_hook != NULL)
           (*SS_post_eval_hook)(SS_indev);

/* print the evaluated object */
        if (SS_print_flag)
           SS_print(SS_evobj, SS_ans_prompt, "\n", SS_outdev);

        SS_Assign(SS_Env,   SS_Global_Env);
        SS_Assign(SS_This,  SS_null);
        SS_Assign(SS_Exn,   SS_null);
        SS_Assign(SS_Val,   SS_null);
        SS_Assign(SS_Unev,  SS_null);
        SS_Assign(SS_Argl,  SS_null);
        SS_Assign(SS_Fun,   SS_null);
        SS_Assign(SS_rdobj, SS_null);
        SS_Assign(SS_evobj, SS_null);

/* restore the global environment */
	SC_mem_stats(&a, &f, NULL, NULL);
        d += a - f;
        if (SS_stat_flag)
           {PRINT(stdout, "Stack Usage (S/R): (%d/%d)",
                  SS_nsave, SS_nrestore);
            PRINT(stdout, "   Continuations (S/G): (%d/%d)\n",
                  SS_nsetc, SS_ngoc);
            PRINT(stdout, "Memory Usage(A/F): (%ld/%ld)",
                  a, f);
            PRINT(stdout, "   Net (A-F): (%ld)",
		  d);
            PRINT(stdout, "   Time: (%10.3e)\n",
		  evalt);};

        if (SS_post_print_hook != NULL)
           (*SS_post_print_hook)();
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
        t = SS_BUFFER(SS_indev);
        SS_PTR(SS_indev) = t;
        *t = '\0';};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_END_SCHEME - gracefully exit from Scheme */

void SS_end_scheme(int val)
   {int ev;

    if (!SS_nullobjp(SS_histdev))
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
            SC_INTEGER_I, &_SS.exit_val,
            0);

    LONGJMP(SC_top_lev, ERR_FREE);

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

	    SFREE(npath);};};

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
    SS_stack_size = 128;
#else
    SS_stack_size = 32;
#endif
    SS_stack_mask = SS_stack_size - 1;

    SS_register_types();

    SS_inst_prm();
    SS_inst_const();

    SS_trap_error = TRUE;
    SS_err_state  = SS_null;
    SS_Env        = SS_null;

    fr            = SS_mk_new_frame(SS_mk_string("global-environment"), NULL);
    SS_Global_Env = SS_mk_cons(fr, SS_null);
    SS_UNCOLLECT(SS_Global_Env);

    SS_Assign(SS_Env, SS_Global_Env);

    SS_define_constant(1,
		       "system-id", SC_STRING_I, SYSTEM_ID,
		       "argv",      SS_OBJECT_I, SS_null,
		       NULL);

    SS_This  = SS_null;
    SS_Exn   = SS_null;
    SS_Val   = SS_null;
    SS_Unev  = SS_null;
    SS_Argl  = SS_null;
    SS_Fun   = SS_null;
    SS_rdobj = SS_null;
    SS_evobj = SS_null;

/* give default values to the lisp package interface variables  */
    SS_post_read_hook  = NULL;
    SS_post_eval_hook  = NULL;
    SS_post_print_hook = NULL;
    SS_pr_ch_in        = SS_get_ch;
    SS_pr_ch_un        = SS_unget_ch;
    SS_pr_ch_out       = SS_put_ch;

    SC_set_put_line(SS_printf);
    SC_set_put_string(SS_fputs);

#ifdef NO_SHELL
    SC_set_get_line(PG_wind_fgets);
#else
    SC_set_get_line(io_gets);
#endif

    SS_interactive = TRUE;
    SS_lines_page  = 50;
    SS_print_flag  = TRUE;
    SS_stat_flag   = TRUE;
    SS_nsave       = 0;
    SS_nrestore    = 0;
    SS_nsetc       = 0;
    SS_ngoc        = 0;

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
   
    hp = SC_hasharr_lookup(SS_symtab, name);

    if (hp == NULL)
       {strcpy(uname, name);
	SC_str_upper(uname);
	PRINT(ERRDEV, "Error initializing %s\n", uname);
        LONGJMP(SC_top_lev, ABORT);};  

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

    SC_hasharr_install(SS_symtab, name, o, SS_POBJECT_S, TRUE, TRUE);

    SS_UNCOLLECT(o);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INST_CONST - install Scheme constants */

void SS_inst_const(void)
   {

    SS_OBJECT_S  = SC_strsavef("object", "char*:SS_INST_CONST:object");
    SS_POBJECT_S = SC_strsavef("object *", "char*:SS_INST_CONST:pobject");

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
    
    SS_histdev = SS_null;
    SS_indev   = SS_mk_inport(stdin, "stdin");
    SS_UNCOLLECT(SS_indev);
    SS_outdev  = SS_mk_outport(stdout, "stdout");
    SS_UNCOLLECT(SS_outdev);

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

    SS_stack = SC_MAKE_ARRAY("SC_INIT_STACK", object *, NULL);

    SS_nsave    = 0;
    SS_nrestore = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_CONT - rewind the continuation stack to the beginning */

void SS_init_cont(void)
   {int i;

    SS_nsetc = 0;
    SS_ngoc  = 0;

    SS_continue = FMAKE_N(continuation, SS_stack_size,
			  "SS_INIT_CONT:SS_continue");
    if (SS_continue == NULL)
       LONGJMP(SC_top_lev, ABORT);
    for (i = 0; i < SS_stack_size; SS_continue[i++].signal = SS_null);
    SS_cont_ptr = 0;

    SS_err_continue = FMAKE_N(err_continuation, SS_stack_size,
			      "SS_INIT_CONT:err_continue");
    if (SS_err_continue == NULL)
       LONGJMP(SC_top_lev, ABORT);
    for (i = 0; i < SS_stack_size; SS_err_continue[i++].signal = SS_null);
    SS_err_cont_ptr = 0;

    SS_err_stack = FMAKE_N(object *, SS_stack_size,
			   "SS_INIT_CONT:err_stack");
    if (SS_err_stack == NULL)
       LONGJMP(SC_top_lev, ABORT);
    for (i = 0; i < SS_stack_size; SS_err_stack[i++] = NULL);
    SS_errlev = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EXPAND_STACK - double the size of the continuation and err stacks */

void SS_expand_stack(void)
   {int i, size;

    size = SS_stack_size;
    SS_stack_size = 2*size;

    REMAKE_N(SS_continue, continuation, SS_stack_size);
    for (i = size; i < SS_stack_size; SS_continue[i++].signal = SS_null);

    REMAKE_N(SS_err_continue, err_continuation, SS_stack_size);
    for (i = size; i < SS_stack_size; SS_err_continue[i++].signal = SS_null);

    REMAKE_N(SS_err_stack, object *, SS_stack_size);
    for (i = size; i < SS_stack_size; SS_err_stack[i++] = NULL);

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
         SC_hasharr_remove(SS_symtab, synname);
         SC_hasharr_install(SS_symtab, synname, func, SS_OBJECT_S, TRUE, TRUE);};

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

    SFREE(name);

    if (SS_variablep(obj))
       {vnm = SS_VARIABLE_NAME(obj);
	obj = _SS_lk_var_valc(vnm, SS_Env);
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

    x = SS_mk_esc_proc(SS_errlev, type);
    SS_MARK(x);
    SS_err_stack[SS_errlev] = x;

    SS_errlev = (SS_errlev + 1) & SS_stack_mask;

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

    if (SS_errlev < 1)
       {PRINT(stdout, "\nERROR: ERROR STACK BLOWN - SS_POP_ERR\n\n");
        exit(SC_EXIT_CORRUPT);};

/* GC the other error stack frames */
    while (TRUE)
       {x = SS_err_stack[--SS_errlev];
        SS_err_stack[SS_errlev] = NULL;
        if (SS_errlev <= n)
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

    n = SC_array_get_n(SS_stack) - 1;

/* restore the stack */
    if (n < ns)
       SS_error("CORRUPT STACK FRAME - _SS_RESTORE_STATE_PRIM", SS_null);

    for (; n > ns; n--)
        {SS_nrestore++;

         x = SC_array_pop(SS_stack);

         SS_GC(x);};

/* restore the continuation stack */
    if (SS_cont_ptr < nc)
       SS_error("CORRUPT CONTINUATION FRAME - _SS_RESTORE_STATE_PRIM",
                SS_null);

    for (; SS_cont_ptr > nc; SS_cont_ptr--)
        {SS_ngoc++;
         SS_Assign(SS_continue[SS_cont_ptr].signal, SS_null);};

/* restore the error stack */
    if (SS_errlev < ne)
       SS_error("CORRUPT ERROR FRAME - _SS_RESTORE_STATE_PRIM", SS_null);

    for (; SS_errlev > ne; SS_errlev--)
        {esc = SS_err_stack[SS_errlev-1];
         SS_err_stack[SS_errlev-1] = NULL;
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

    SS_Save(SS_evobj);
    SS_Save(SS_rdobj);

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

    LONGJMP(SS_continue[1].cont, ABORT);
/*    LONGJMP(SS_continue[1].cont, RETURN_OK); */

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
    n = SS_errlev - n;
    n = max(1, n);

    if (!SS_consp(argl))
       SS_error("SECOND ARG MISSING - _SSI_RETLEV", x);
    val = SS_car(argl);    

    if (SS_errlev > 1)
       {x = SS_pop_err(n, TRUE);

        SS_Restore(SS_rdobj);
        SS_Restore(SS_evobj);

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

    PRINT(stdout, "\n\nInterrupt (%d frames):\n", SS_errlev - 1);
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
	     t = SS_BUFFER(SS_indev);
	     SS_PTR(SS_indev) = t;
	     *t = '\0';
	     _SSI_break();
	     break;

        case 'u' :
	     nl = SC_stoi(arg);
	     PRINT(stdout, "\nReturning %d frames\n\n", 2*nl);
	     argl = SS_mk_cons(SS_mk_integer((BIGINT) nl),
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
    SS_cont_ptr++;
    SS_push_err(FALSE, SS_ERROR_I);
    switch (SETJMP(SS_continue[SS_cont_ptr].cont))
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
	     esc = SS_pop_err(SS_errlev - 1, FALSE);
	     SS_GC(esc);};

    SS_cont_ptr--;

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

    t = SC_strsavef(s, "char*:SS_ERROR:t");

    SS_Assign(SS_err_state, SS_make_list(SS_OBJECT_I, SS_Fun,
					 SS_OBJECT_I, SS_Argl,
					 SS_OBJECT_I, obj,
					 SC_STRING_I, t,
					 0));

    str = SS_OUTSTREAM(SS_outdev);
    if (_SS.pr_err != NULL)
       SS_PRINT_ERR_MSG(str, t, obj);

    SFREE(t);

    t = SS_BUFFER(SS_indev);
    SS_PTR(SS_indev) = t;
    *t = '\0';

    esc = SS_pop_err(SS_errlev - 1, FALSE);
    nc  = SS_ESCAPE_CONTINUATION(esc);
    SS_GC(esc);

    if (SS_trap_error)
       LONGJMP(SS_continue[nc].cont, ABORT);

    else
       LONGJMP(SC_top_lev, ABORT);

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

    if (itype == SC_INTEGER_I)
       strcpy(atype, "integer");

    else if (itype == SC_FLOAT_I)
       strcpy(atype, "float");

    else if (itype == SC_STRING_I)
       strcpy(atype, "string");

    else if (itype == SS_CONS_I)
       strcpy(atype, "pair");

    else if (itype == SS_VARIABLE_I)
       strcpy(atype, "variable");

    else if (itype == SS_PROCEDURE_I)
       strcpy(atype, "procedure");

    else if (itype == SS_BOOLEAN_I)
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
       strcpy(atype, "char");

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
	    SC_INTEGER_I, &to,
	    0);

    if (to > 0)
       {if (SC_time_allow(to) == 0)
	   {rv = SYSTEM(cmd);
	    SC_time_allow(0);}
        else
	   PRINT(stdout, "'%s' timed out after %d seconds\n", cmd, to);}

    else
       rv = SYSTEM(cmd);

    SFREE(cmd);

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
	    SC_INTEGER_I, &to,
	    0);

    if (to > 0)
       {if (SC_time_allow(to) == 0)
	   {output = SC_syscmnd(cmd);
	    SC_time_allow(0);};}

    else
       output = SC_syscmnd(cmd);

    SFREE(cmd);

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

    if (SS_symtab == NULL)
       SS_symtab = SC_make_hasharr(HSZHUGE, DOC, SC_HA_NAME_KEY);

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
    _SS_inst_prm3();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
