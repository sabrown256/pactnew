/*
 * SHTLEV.C - top level of scheme interpreter
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"
#include "syntax.h"

#define SS_PRINT_ERR_MSG (*_SS.pr_err)

SS_scope_private
 _SS = { -1, 1, -1, 1, };

SS_scope_public
 SS_gs = { FALSE,
	   { 123, 124, 124, 125 },};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_PUT_STRING - set the _SC_ios.putstr method and
 *                   - register the context
 */

void SS_set_put_string(SS_psides *si, PFfputs ps)
   {
    
    SC_set_put_string(ps);
    SC_REGISTER_CONTEXT(ps, SS_psides, si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_PUT_LINE - set the _SC_ios.putstr method and
 *                 - register the context
 */

void SS_set_put_line(SS_psides *si, int (*pf)(FILE *fp, char *fmt, ...))
   {
    
    SC_set_put_line(pf);
    SC_REGISTER_CONTEXT(pf, SS_psides, si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PRINT_ERR_MSG_A - alternate error message print function for
 *                     - _SS.pr_err
 */

static void _SS_print_err_msg_a(SS_psides *si, FILE *str, char *s, object *obj)
   {

    PRINT(str, "ERROR : %s : ", s);

    if (SC_arrtype(obj, -1) == 0)
       PRINT(str, "MEMORY PROBABLY CORRUPTED\n");
    else
       SS_print(si, si->outdev, obj, "", "\n");

    SS_print(si, si->outdev, si->fun, "CURRENT FUNCTION: ", "\n\n");

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

static int _SS_repl(SS_psides *si)
   {int64_t ba, bf, bd;
    double evalt;

    SS_assign(si, si->rdobj, SS_null);
    SS_assign(si, si->evobj, SS_null);

    _SS_set_ans_prompt(si);

    bd = 0L;
    while (TRUE)
       {SC_mem_stats_set(0L, 0L);
        si->ngoc       = 0;
        si->nsetc      = 0;
        si->nsave      = 0;
        si->nrestore   = 0;

/* print the prompt call the Reader */
	_SS.pr_prompt = si->prompt;
        SS_assign(si, si->rdobj, SS_read(si, si->indev));

        if (si->post_read != NULL)
           (*si->post_read)(si, si->indev);

        si->interactive = TRUE;

/* eval the object returned by the Reader */
	evalt = SC_cpu_time();
        SS_assign(si, si->evobj, SS_eval(si, si->rdobj));
	evalt = SC_cpu_time() - evalt;

        if (si->post_eval != NULL)
           (*si->post_eval)(si, si->indev);

/* print the evaluated object */
        if (si->print_flag)
           SS_print(si, si->outdev, si->evobj, si->ans_prompt, "\n");

        SS_assign(si, si->env,   si->global_env);
        SS_assign(si, si->this,  SS_null);
        SS_assign(si, si->exn,   SS_null);
        SS_assign(si, si->val,   SS_null);
        SS_assign(si, si->unev,  SS_null);
        SS_assign(si, si->argl,  SS_null);
        SS_assign(si, si->fun,   SS_null);
        SS_assign(si, si->rdobj, SS_null);
        SS_assign(si, si->evobj, SS_null);

/* restore the global environment */
	SC_mem_stats(&ba, &bf, NULL, NULL);
        bd += ba - bf;
        if (si->stat_flag)
           {PRINT(stdout, "Stack Usage (S/R): (%d/%d)",
                  si->nsave, si->nrestore);
            PRINT(stdout, "   Continuations (S/G): (%d/%d)\n",
                  si->nsetc, si->ngoc);
            PRINT(stdout, "Memory Usage(A/F): (%ld/%ld)",
                  ba, bf);
            PRINT(stdout, "   Net (A-F): (%ld)",
		  bd);
            PRINT(stdout, "   Time: (%10.3e)\n",
		  evalt);};

        if (si->post_print != NULL)
           (*si->post_print)(si);
        else
           PRINT(stdout, "\n");}

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_REPL - run a READ-EVAL-PRINT Loop */

void SS_repl(SS_psides *si)
   {int i;
    char *t;

    for (i = 0; i < INT_MAX; i++)
        {SS_err_catch(si, _SS_repl, NULL);

/* reset the input buffer */
	 t = SS_BUFFER(si->indev);
	 SS_PTR(si->indev) = t;
	 *t = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_REPL - run a real/eval/print loop from the interpreter
 *           - this differs from the break command and
 *           - the function SCHEME-REPL in interpreted code
 *           - in that it can run the various hooks used in
 *           - PDBView mode and ULTRA mode
 */

static object *_SSI_repl(SS_psides *si, object *arg)
   {char *prompt, *prefix;

    prompt = NULL;
    prefix = NULL;
    SS_args(si, arg,
	    G_STRING_I, &prompt,
	    G_STRING_I, &prefix,
	    0);

    if (prompt != NULL)
       SS_set_prompt(si, prompt);

    SS_repl(si);

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_END_SCHEME - gracefully exit from Scheme */

NORETURN void SS_end_scheme(SS_psides *si, int val)
   {int ev;

    if (!SS_nullobjp(si->histdev))
       SS_trans_off(si);

    if (_SS.active_objects == TRUE)
       _SS_object_map(si, stdout, TRUE);

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

static object *_SSI_quit(SS_psides *si, object *arg)
   {

    _SS.exit_val = 0;
    SS_args(si, arg,
            G_INT_I, &_SS.exit_val,
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

/* _SS_PRINT_ERR_MSG - default error message print function for
 *                   - _SS.pr_err
 */

static void _SS_print_err_msg(SS_psides *si, FILE *str, char *s, object *obj)
   {char atype[MAXLINE];
    char *p;

    if (obj == NULL)
       PRINT(str, "(%d):  ERROR: %s\n", si->errlev, s);

    else
       {PRINT(str, "(%d):  ERROR: %s\n      BAD OBJECT (", si->errlev, s);

	p = SS_object_type_name(obj, atype, MAXLINE);
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

	SS_print(si, si->outdev, obj, "): ", "\n\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_SCHEME - initialize the interpreter */

SS_psides *SS_init_scheme(char *code, char *vers,
			  int c, char **v, char **env, int go)
   {SS_psides *si;

/* setup OMP the very first thing */
    SC_init_omp(c, v);

    SC_set_banner(" %s  -  %s\n\n", code, vers);

/* try to ensure that the SCHEME environment variable is properly set */
    if (v != NULL)
       SS_set_scheme_env(v[0], NULL);

    SC_init_path(2, "HOME", "SCHEME");

    si = SS_get_current_scheme(-1);

/* initialize number output formats */
    SS_gs.fmts[0] = CSTRSAVE("%d");
    SS_gs.fmts[1] = CSTRSAVE("%Lg");
    SS_gs.fmts[2] = CSTRSAVE("%Lg");
    SS_gs.fmts[3] = CSTRSAVE("%Lg");

/* initialize the available syntax modes */
    DEF_SYNTAX_MODES(si);

    SC_mem_stats_set(0L, 0L);

    SS_set_put_line(si, SS_printf);
    SS_set_put_string(si, SS_fputs);

#ifdef NO_SHELL
    SC_set_get_line(PG_wind_fgets);
#else
    SC_set_get_line(io_gets);
#endif

    SS_set_print_err_func(_SS_print_err_msg, FALSE);

    if (env != NULL)
       SS_env_vars(si, env, NULL);

    if (v != NULL)
       SS_define_argv(si, c, v, go);

    return(si);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EXPAND_STACK - double the size of the continuation and err stacks */

void SS_expand_stack(SS_psides *si)
   {int i, size;

    size = si->stack_size;
    si->stack_size = 2*size;

    CREMAKE(si->continue_int, SS_continuation, si->stack_size);
    for (i = size; i < si->stack_size; si->continue_int[i++].signal = SS_null);

    CREMAKE(si->continue_err, SS_err_continuation, si->stack_size);
    for (i = size; i < si->stack_size; si->continue_err[i++].signal = SS_null);

    CREMAKE(si->err_stack, object *, si->stack_size);
    for (i = size; i < si->stack_size; si->err_stack[i++] = NULL);

    return;}

/*--------------------------------------------------------------------------*/

/*                      SYMBOL TABLE MANIPULATORS                           */

/*--------------------------------------------------------------------------*/

/* _SSI_SYNONYM - make synonyms for the given function */

static object *_SSI_synonym(SS_psides *si, object *argl)
   {object *func;
    char *synname;

    func = SS_exp_eval(si, SS_car(si, argl));
    if (!SS_procedurep(func))
       SS_error(si, "FIRST ARG MUST BE FUNCTION - _SSI_SYNONYM", func);

    for (argl = SS_cdr(si, argl); SS_consp(argl); argl = SS_cdr(si, argl))
        {synname = SS_get_string(SS_car(si, argl));
         SC_hasharr_remove(si->symtab, synname);
         SC_hasharr_install(si->symtab, synname, func, G_OBJECT_S, 3, -1);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOOKUP_OBJECT - interpreter level call to get an object in the
 *                  - environment given only its name
 *                  - Usage:  (string->object <string>)
 */

object *SS_lookup_object(SS_psides *si, object *obj)
   {char *name, *vnm;

/* take anything that will give a name - procedure, string, variable ... */
    name = NULL;
    SS_args(si, obj,
	    G_STRING_I, &name,
	    0);

    obj = SS_bound_name(si, name);

    CFREE(name);

    if (SS_variablep(obj))
       {vnm = SS_VARIABLE_NAME(obj);
	obj = _SS_lk_var_valc(si, vnm, si->env);
	if (obj == NULL)
	   obj = SS_null;};

    return(obj);}

/*--------------------------------------------------------------------------*/

/*                              ERROR HANDLERS                              */

/*--------------------------------------------------------------------------*/

/* SS_PUSH_ERR - push an error stack frame */

void SS_push_err(SS_psides *si, int flag)
   {object *x;

    SS_save_registers(si, flag);

    x = SS_mk_esc_proc(si, si->errlev, SS_ERROR_I);
    SS_mark(x);
    si->err_stack[si->errlev] = x;

    si->errlev = (si->errlev + 1) & si->stack_mask;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_POP_ERR - pop an error stack frame and restore the state of the
 *            - interpreter to the time that the frame was pushed
 *            - return the escape procedure object associated with the
 *            - correct error stack frame
 */

object *SS_pop_err(SS_psides *si, int n, int flag)
   {object *x;

    if (si->errlev < 1)
       {PRINT(stdout, "\nERROR: ERROR STACK BLOWN - SS_POP_ERR\n\n");
        exit(SC_EXIT_CORRUPT);};

/* GC the other error stack frames */
    while (TRUE)
       {x = si->err_stack[--si->errlev];
        si->err_stack[si->errlev] = NULL;
        if (si->errlev <= n)
           {_SS_restore_state(si, x);
            break;}
        else
           SS_gc(si, x);};

    SS_restore_registers(si, flag);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RESTORE_STATE_PRIM - do the actual work of restoring the state
 *                        - of the interpreter
 */

static void _SS_restore_state_prim(SS_psides *si, int ns, int nc, int ne)
   {int n;
    object *x, *esc;

    n = SC_array_get_n(si->stack) - 1;

/* restore the stack */
    if (n < ns)
       SS_error(si,
		  "CORRUPT STACK FRAME - _SS_RESTORE_STATE_PRIM",
		  SS_null);

    for (; n > ns; n--)
        {si->nrestore++;

         x = SC_array_pop(si->stack);

         SS_gc(si, x);};

/* restore the continuation stack */
    if (si->cont_ptr < nc)
       SS_error(si,
		  "CORRUPT CONTINUATION FRAME - _SS_RESTORE_STATE_PRIM",
		  SS_null);

    for (; si->cont_ptr > nc; si->cont_ptr--)
        {si->ngoc++;
         SS_assign(si, si->continue_int[si->cont_ptr].signal, SS_null);};

/* restore the error stack */
    if (si->errlev < ne)
       SS_error(si,
		  "CORRUPT ERROR FRAME - _SS_RESTORE_STATE_PRIM",
		  SS_null);

    for (; si->errlev > ne; si->errlev--)
        {esc = si->err_stack[si->errlev-1];
         si->err_stack[si->errlev-1] = NULL;
         SS_gc(si, esc);};

    _SS_set_ans_prompt(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RESTORE_STATE - restore the state of Scheme to that
 *                   - implied by the given escape procedure
 */

void _SS_restore_state(SS_psides *si, object *esc_proc)
   {int ns, nc, ne;

    ns = SS_ESCAPE_STACK(esc_proc);
    nc = SS_ESCAPE_CONTINUATION(esc_proc);
    ne = SS_ESCAPE_ERROR(esc_proc);

    _SS_restore_state_prim(si, ns, nc, ne);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_BREAK - enter a Scheme break from the Scheme level
 *            - return with return-level or reset
 *            - create a higher level REPL and push on
 */

static object *_SSI_break(SS_psides *si)
   {

    SS_save(si, si->evobj);
    SS_save(si, si->rdobj);

    SS_push_err(si, TRUE);
    PRINT(stdout,"\n");

    _SS_repl(si);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RESET - unwind the error/break stack and return to top level */

static object *_SSI_reset(SS_psides *si)
   {

    _SS_restore_state_prim(si, 0, 1, 0);
    PRINT(stdout,"\n");

    LONGJMP(si->continue_int[1].cont, ABORT);
/*    LONGJMP(si->continue_int[1].cont, RETURN_OK); */

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RETLEV - pops n levels off the error/break stack and
 *             - returns the second arg as value
 *             - GOTCHA: there are residual GC issues here
 */

static object *_SSI_retlev(SS_psides *si, object *argl)
   {int n;
    object *x, *expr, *val;

    x    = SS_car(si, argl);
    argl = SS_cdr(si, argl);
    if (!SS_integerp(x))
       SS_error(si, "FIRST ARG MUST BE AN INTEGER - _SSI_RETLEV", x);

    n = (int) SS_INTEGER_VALUE(x);
    n = si->errlev - n;
    n = max(1, n);

    if (!SS_consp(argl))
       SS_error(si, "SECOND ARG MISSING - _SSI_RETLEV", x);
    val = SS_car(si, argl);    

    if (si->errlev > 1)
       {x = SS_pop_err(si, n, TRUE);

        SS_restore(si, si->rdobj);
        SS_restore(si, si->evobj);

	expr = SS_make_form(si, x,
			    SS_make_form(si,
					 SS_quoteproc, val,
					 LAST),
			    LAST);
        x    = SS_exp_eval(si, expr);}

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
    SS_psides *si;

    si = SC_GET_CONTEXT(SS_interrupt_handler);

    SC_signal_n(SIGINT, SS_interrupt_handler, si, sizeof(SS_psides));

    PRINT(stdout, "\n\nInterrupt (%d frames):\n", si->errlev - 1);
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
	     _SSI_reset(si);
	     break;

        case 'b' :
	     PRINT(stdout, "\nEntering SCHEME break\n\n");
	     t = SS_BUFFER(si->indev);
	     SS_PTR(si->indev) = t;
	     *t = '\0';
	     _SSI_break(si);
	     break;

        case 'u' :
	     nl = SC_stoi(arg);
	     PRINT(stdout, "\nReturning %d frames\n\n", 2*nl);
	     argl = SS_mk_cons(si, SS_mk_integer(si, nl),
			       SS_mk_cons(si, SS_t, SS_null));
	     _SSI_retlev(si, argl);
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

int SS_err_catch(SS_psides *si, int (*fint)(SS_psides *si), PFInt errf)
   {object *esc;

    _SS.ret = TRUE;
    si->cont_ptr++;
    SS_push_err(si, FALSE);
    switch (SETJMP(si->continue_int[si->cont_ptr].cont))
       {case ABORT :
	     if (errf != NULL)
	        _SS.ret = (*errf)();
	     else
	        _SS.ret = FALSE;
	     break;

        case RETURN_OK :
	     break;

        default :
	     if (fint != NULL)
	        _SS.ret = fint(si);

        case ERR_FREE :
	     esc = SS_pop_err(si, si->errlev - 1, FALSE);
	     SS_gc(si, esc);};

    si->cont_ptr--;

    return(_SS.ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ERROR - signal an error
 *          - create a higher level REPL and push on
 */

void SS_error(SS_psides *si, char *s, object *obj)
   {int nc;
    char *t;
    FILE *str;
    object *esc;

    t = CSTRSAVE(s);

    SS_assign(si, si->err_state,
	      SS_make_list(si,
			   G_OBJECT_I, si->fun,
			   G_OBJECT_I, si->argl,
			   G_OBJECT_I, obj,
			   G_STRING_I, t,
			   0));

    str = SS_OUTSTREAM(si->outdev);
    if (_SS.pr_err != NULL)
       SS_PRINT_ERR_MSG(si, str, t, obj);

    CFREE(t);

    t = SS_BUFFER(si->indev);
    SS_PTR(si->indev) = t;
    *t = '\0';

    esc = SS_pop_err(si, si->errlev - 1, FALSE);
    nc  = SS_ESCAPE_CONTINUATION(esc);
    SS_gc(si, esc);

    if (si->trap_error)
       LONGJMP(si->continue_int[nc].cont, ABORT);

    else
       LONGJMP(SC_gs.cpu, ABORT);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_OBJECT_TYPE_NAME - return the ASCII name of the type of
 *                     - the specified object O
 */

char *SS_object_type_name(object *o, char *atype, long nb)
   {int itype;
    char *p;

    itype = SC_arrtype(o, -1);
    p     = atype;

    if (itype == G_INT_I)
       SC_strncpy(atype, nb, G_INT_S, -1);

    else if (itype == G_FLOAT_I)
       SC_strncpy(atype, nb, G_FLOAT_S, -1);

    else if (itype == G_STRING_I)
       SC_strncpy(atype, nb, "string", -1);

    else if (itype == G_SS_CONS_I)
       SC_strncpy(atype, nb, "pair", -1);

    else if (itype == G_SS_VARIABLE_I)
       SC_strncpy(atype, nb, "variable", -1);

    else if (itype == G_SS_PROCEDURE_I)
       SC_strncpy(atype, nb, "procedure", -1);

    else if (itype == G_BOOL_I)
       SC_strncpy(atype, nb, "boolean", -1);

    else if (itype == G_SS_INPUT_PORT_I)
       SC_strncpy(atype, nb, "input port", -1);

    else if (itype == G_SS_OUTPUT_PORT_I)
       SC_strncpy(atype, nb, "output port", -1);

    else if (itype == SS_EOF_I)
       SC_strncpy(atype, nb, "eof", -1);

    else if (itype == SS_NULL_I)
       SC_strncpy(atype, nb, "nil", -1);

    else if (itype == G_SS_VECTOR_I)
       SC_strncpy(atype, nb, "vector", -1);

    else if (itype == SS_CHARACTER_I)
       SC_strncpy(atype, nb, G_CHAR_S, -1);

    else if (itype == G_HAELEM_I)
       SC_strncpy(atype, nb, "hash element", -1);

    else if (itype == G_HASHARR_I)
       SC_strncpy(atype, nb, "hash array", -1);

    else if (itype == G_PROCESS_I)
       SC_strncpy(atype, nb, "process", -1);

    else if (itype == SS_ERROR_I)
       SC_strncpy(atype, nb, "error object", -1);

    else
       p = NULL;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SYSTEM - pass command to operating system from Scheme
 *             - timeout after optionally specified interval
 *             - return the exit status of the command
 */

static object *_SSI_system(SS_psides *si, object *argl)
   {int rv, to;
    char *cmd;
    object *o;

    rv  = -1;
    cmd = NULL;
    to  = -1;
    SS_args(si, argl,
	    G_STRING_I,  &cmd,
	    G_INT_I, &to,
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

    o = SS_mk_integer(si, rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SYSCMND - pass command to operating system from Scheme, return
 *              - output in a list
 */

static object *_SSI_syscmnd(SS_psides *si, object *argl)
   {int i, to, ost;
    char *cmd, **output;
    object *lst;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    output = NULL;
    cmd    = NULL;
    to     = -1;
    SS_args(si, argl,
	    G_STRING_I,  &cmd,
	    G_INT_I, &to,
	    0);

    ost = ps->msh_syntax;
    ps->msh_syntax = TRUE;

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
            {SS_assign(si, lst,
		       SS_mk_cons(si, SS_mk_string(si, output[i]),
				  lst));};

        SS_assign(si, lst, SS_reverse(si, lst));
	SC_mark(lst, -1);

        SC_free_strings(output);}

    ps->msh_syntax = ost;

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PR_OBJ_MAP - dump the list of objects and associated info */

static object *_SSI_pr_obj_map(SS_psides *si)
   {

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_PRM - install the Scheme primitives */

void _SS_inst_prm(SS_psides *si)
   {

    SS_install(si, "break",
               "Procedure: enter a Scheme break, return with return-level",
               SS_zargs,
               _SSI_break, SS_PR_PROC);

    SS_install(si, "c-repl",
               "Procedure: enter a Scheme read/eval/print loop using all hooks",
               SS_nargs,
               _SSI_repl, SS_PR_PROC);

    SS_install(si, "defined?",
               "Special Form: returns #t if its argument has been defined in the current environment",
               SS_sargs,
               _SSI_defp, SS_UR_MACRO);

    SS_install(si, "display-object-table",
               "Procedure: Prints information about all known objects",
               SS_zargs,
               _SSI_pr_obj_map, SS_PR_PROC);

    SS_install(si, "list",
               "Return a new list made up of the arguments",
               SS_nargs, 
               SS_list, SS_PR_PROC);

    SS_install(si, "quit",
               "Procedure: Exit from Scheme",
               SS_znargs,
               _SSI_quit, SS_PR_PROC);

    SS_install(si, "reset",
               "Procedure: unwinds the Error/Break stack and returns to top level",
               SS_zargs,
               _SSI_reset, SS_PR_PROC);

    SS_install(si, "return-level",
               "Procedure: pops n levels off the Error/Break stack and returns the second arg as value",
               SS_nargs,
               _SSI_retlev, SS_PR_PROC);

    SS_install(si, "string->object",
               "Returns the object named by the given string",
               SS_sargs, 
               SS_lookup_object, SS_PR_PROC);

    SS_install(si, "synonym",
               "Macro: Define synonyms for the given function",
               SS_nargs,
               _SSI_synonym, SS_UR_MACRO);

    SS_install(si, "syscmnd",
               "Procedure: process the given string as a command line-return results in list",
               SS_nargs,
               _SSI_syscmnd, SS_PR_PROC);

    SS_install(si, "system",
               "Procedure: invokes the operation system to process the given string as a command line",
               SS_nargs,
               _SSI_system, SS_PR_PROC);

#ifdef LARGE
    _SS_inst_lrg(si);
    _SS_inst_proc(si);
#endif

    _SS_inst_eval(si);
    _SS_inst_read(si);
    _SS_inst_print(si);
    _SS_inst_prm1(si);
    _SS_install_math(si);
    _SS_install_complex(si);
    _SS_install_quaternion(si);
    _SS_inst_prm3(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
