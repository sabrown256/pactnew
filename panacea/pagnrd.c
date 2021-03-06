/*
 * PAGNRD.C - generator core for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INST_PCK_GEN_CMMNDS - install the generator commands which each
 *                        - package needs and provides
 */

void PA_inst_pck_gen_cmmnds(void)
   {PA_package *pck;
    PFPkgGencmd pck_cmd;

/* loop over all packages */
    for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
        {pck_cmd  = pck->gencmd;

/* execute the package generator command installers */
         if (pck_cmd != NULL)
            {PA_control_set(pck->name);
             (*pck_cmd)();};};

/* reconnect the global controls */
    PA_control_set("global");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTERN_PCK_DB - execute the functions which intern package variables
 *                  - in the data base
 */

void PA_intern_pck_db(void)
   {PA_package *pck;
    PFPkgIntrn pck_intrn;

/* loop over all packages */
    for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
        {pck_intrn = pck->intrn;

/* execute the package variable interners */
         if (pck_intrn != NULL)
            {PA_control_set(pck->name);
             (*pck_intrn)();};};

/* reset to the global controls */
    PA_control_set("global");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PA_INST_COM - install the commands
 *             - only the commands which are generic to all PANACEA
 *             - generators should be here
 *             - the others belong with the packages they serve
 *
 * #bind PA_inst_com fortran() scheme()
 */
 
hasharr *PA_inst_com(void)
   {

    PA_gs.alias_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);
    PA_gs.command  = CSTRSAVE("PA_command");

/* this call should be moved into PA_inst_pck_gen_cmmnds */
    PA_gencmd();

/* install the commands serving the packages */
    PA_inst_pck_gen_cmmnds();
 
/* build the data base now */
    PA_definitions();
    PA_variables(FALSE);

    return(PA_gs.command_tab);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PA_INST_C - install generator commands in the command table
 *           - a command object is defined as:
 *           -      struct s_PA_command
 *           -         {char type;
 *           -          int *var;
 *           -          int num;
 *           -          char *name;
 *           -          PFVoid proc;
 *           -          PFInt handler;};
 *           -
 *           -      typedef struct s_PA_command PA_command;
 *           -
 */
 
void PA_inst_c(const char *cname, void *cvar, int ctype, int cnum,
	       PFVoid cproc, PFPanHand chand)
   {PA_command *cp;
 
    if (PA_gs.command_tab == NULL)
       PA_gs.command_tab  = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

    cp          = CMAKE(PA_command);
    cp->name    = CSTRSAVE(cname);
    cp->type    = ctype;
    cp->num     = cnum;
    cp->proc    = cproc;
    cp->handler = chand;
    cp->vr      = (int *) cvar;
 
    SC_hasharr_install(PA_gs.command_tab, cname, cp, PA_gs.command, 3, -1);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_ALIAS - define an alias for a constant value */

void PA_def_alias(const char *name, const char *type, void *pv)
   {int id;
    void *v;

    id = SC_type_id(type, FALSE);
    if (id == G_STRING_I)
       v = CSTRSAVE(pv);
    else
       v = SC_convert_id(id, NULL, 0, 1, id, pv, 0, 1, 1, FALSE);

    SC_hasharr_install(PA_gs.alias_tab, name, v, type, 3, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ALIAS_VALUE - return the value aliased to the string S as a double
 *                - NOTE: the value can be cast to its true type
 */

double PA_alias_value(const char *s)
   {int id;
    double d;
    haelem *hp;

    d = -2.0*HUGE;

    if (s != NULL)
       {hp = SC_hasharr_lookup(PA_gs.alias_tab, s);
	if (hp == NULL)
	   {if (SC_numstrp(s))
	       d = SC_stof(s);}

	else
	   {id = SC_type_id(hp->type, FALSE);
	    if (SC_is_type_num(id) == TRUE)
	       SC_convert_id(G_DOUBLE_I, &d, 0, 1,
			     id, hp->def, 0, 1, 1, FALSE);

	    else if (id == G_STRING_I)
	       d = -2.0*HUGE;

	    else
	       PA_ERR(TRUE, "BAD ALIAS - PA_ALIAS_VALUE");};};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEFH - define a string valued constant
 *         - this can be used as a macro facility by command
 *         - which decide to use it
 */

void PA_defh(void)
   {char *var, *val, *delim, *sv;
    long lv;
    double dv;

    var = PA_get_field("NAME", "DEFINE", REQU);

/* save the tokenizing delimiter */
    delim = PA_gs.token_delimiters;

/* change the tokenizing delimiter for an indefinite string field */
    PA_gs.token_delimiters = "\n";
    val = PA_get_field("BODY", "DEFINE", REQU);

    if (val != NULL)

/* strip off leading white space */
       {while (TRUE)
	   {if (strchr(" \t\f\r", *val++) == NULL)
	       break;};
	val--;

/* restore the tokenizing delimiter */
	PA_gs.token_delimiters = delim;

	if (SC_fltstrp(val))
	   {dv = SC_stof(val);
	    PA_def_alias(var, G_DOUBLE_S, &dv);}

	else if (SC_intstrp(val, 10))
	   {lv = SC_stoi(val);
	    PA_def_alias(var, G_LONG_S, &lv);}

	else
	   {sv = CSTRSAVE(val);
	    PA_def_alias(var, G_STRING_S, &sv);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SPECIFYH - handler for specify command */

void PA_specifyh(void)
   {char *s, *ivfn, *ivident;
    int ivtype, interp;
    double nxt, *nxtp;
    pcons *first, *prev;

    s = PA_get_field("TYPE", "PA_SPECIFY", REQU);
    PA_ERR((s == NULL),
	   "BAD TYPE - PA_SPECIFY");

    if (strcmp(s, "bc") == 0)
       {ivtype = 'b';
        ivident = PA_get_field("IDENTIFIER", "PA_SPECIFY", REQU);}
    else if (strcmp(s, "src") == 0)
       {ivtype = 's';
        ivident = PA_get_field("IDENTIFIER", "PA_SPECIFY", REQU);}
    else
       {ivtype = 'v';
        ivident = CSTRSAVE(s);};

    first  = NULL;
    _PA.ivlst  = NULL;
    ivfn   = NULL;
    _PA.ivnum  = 0;
    interp = TRUE;
    while ((s = PA_get_field("SPECIFICATION", "PA_SPECIFY", OPTL)) != NULL)
       {if (strcmp(s, "from") == 0)
           {ivfn = PA_get_field("FILE NAME", "PA_SPECIFY", REQU);
            continue;}
        else if (strcmp(s, "interpolate") == 0)
           {interp = TRUE;
            continue;}
        else if (strcmp(s, "discrete") == 0)
           {interp = FALSE;
            continue;}
        else
           {if (strcmp(s, "in") == 0)
               {PFRegID reg_hook;

                s = PA_get_field("REGION", "PA_SPECIFY", REQU);
                reg_hook = PA_GET_FUNCTION(PFRegID, "region_id");
                PA_ERR((reg_hook == NULL),
                       "CAN'T USE 'in' WITHOUT A 'region_id' HOOK - PA_SPECIFY");
                nxt = (double) (*reg_hook)(s);}

            else if (strcmp(s, "at") == 0)
               {s = PA_get_field("LOCATION", "PA_SPECIFY", REQU);
                nxt = PA_alias_value(s);}

            else if (strcmp(s, "along") == 0)
               {s = PA_get_field("LOCATION", "PA_SPECIFY", REQU);
                nxt = PA_alias_value(s);}

            else
               nxt = PA_alias_value(s);

            nxtp = CMAKE(double);
            *nxtp = nxt;
            _PA.ivlst = SC_mk_pcons(G_DOUBLE_P_S, nxtp, G_PCONS_P_S, NULL);
            if (first == NULL)
               first = _PA.ivlst;
            else
               prev->cdr = (void *) _PA.ivlst;
            prev = _PA.ivlst;
            _PA.ivnum++;};};
    
    PA_gs.iv_spec_lst = PA_mk_spec(ivident, ivtype, ivfn, _PA.ivnum, interp,
			     first, PA_gs.iv_spec_lst);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SH - handler s command */

void PA_sh(void)
   {char *s;
    double nxt, *nxtp;
    pcons *next;

    while ((s = PA_get_field("SPECIFICATION", "S", OPTL)) != NULL)
       {nxt = PA_alias_value(s);
        nxtp = CMAKE(double);
        *nxtp = nxt;
        next = SC_mk_pcons(G_DOUBLE_P_S, nxtp, G_PCONS_P_S, NULL);
        if (_PA.ivlst == NULL)
           PA_gs.iv_spec_lst->spec = next;
        else
           _PA.ivlst->cdr = (void *) next;
        _PA.ivlst = next;
        _PA.ivnum++;};

    PA_ERR((PA_gs.iv_spec_lst == NULL),
           "%s", "NO SPECIFICY COMMAND BEFORE SPECIFICATION");

    PA_gs.iv_spec_lst->num = _PA.ivnum;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PROC_IV_SPEC - process the initial value specifications
 *                 - take the data out of the PA_iv_specification structs
 *                 - and put it into some nice arrays for the restart dump
 */

void PA_proc_iv_spec(PA_iv_specification *lst)
   {PA_iv_specification *sp;
    double *data, *ths;
    pcons *pp, *nxt;

    for (sp = lst; sp != NULL; sp = sp->next)
        {if (sp->spec == NULL)
            continue;

/* if a data base variable is being specified then connect to it */
         data = CMAKE_N(double, sp->num);
         if (sp->type == 'v')
            PA_INTERN(data, sp->name);

         sp->data = data;

/* put the list of specifications into the data array and free the list */
         for (pp = sp->spec; pp != NULL; pp = nxt)
             {ths = (double *) (pp->car);
              nxt = (pcons *) (pp->cdr);
              *(data++) = *ths;
              SC_rl_pcons(pp, 1);};

         sp->spec = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_READ_FILE - handle the work of the read command */

void PA_read_file(const char *str, int sfl)
   {char s[MAXLINE], *t;
    PFBaseName hook;
    PFGenErr errfnc;

    ONCE_SAFE(TRUE, NULL)
       _PA.input_stream = stdin;
       _PA.inf          = CMAKE_ARRAY(FILE *, NULL, 3);
    END_SAFE;

    if (sfl)
       PA_control_set("global");

/* get the base names set up */
    SC_strncpy(s, MAXLINE, str, -1);
    SC_strtok(s, ".", t);
    hook = PA_GET_FUNCTION(PFBaseName, "base_name");
    if (hook != NULL)
       (*hook)(s);

    SC_array_push(_PA.inf, &_PA.input_stream);

/* open the input deck */
    _PA.input_stream = io_open(str, "r");
    PA_ERR((_PA.input_stream == NULL),
           "Couldn't open file %s", str);

    errfnc = PA_GET_FUNCTION(PFGenErr, "generation-error");
    PA_get_commands(_PA.input_stream, errfnc);

    _PA.input_stream = *(FILE **) SC_array_pop(_PA.inf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_READH - handle the read command
 *
 * #bind PA_readh fortran() scheme(pa-read-commands)
 */

void PA_readh(const char *str)
   {

/* by now verify/force the PDBNet hooks to be setup */
    PN_init_communications(NULL);

    PA_read_file(str, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_COMMANDS - get commands from the current input stream */

void PA_get_commands(FILE *fp, PFGenErr errfnc)
   {char prompt[MAXLINE];
    char *s, *token;
    PA_command *cp;
    haelem *hp;
    FILE *old_stream;

    old_stream          = _PA.input_stream;
    _PA.input_stream = fp;
    if (PA_gs.input_prompt == NULL)
       SC_strncpy(prompt, MAXLINE, "-> ", -1);
    else
       SC_strncpy(prompt, MAXLINE, PA_gs.input_prompt, -1);

/* dispatch on commands from the deck */
    while (TRUE)
       {if (fp == stdin)
           PRINT(stdout, prompt);
        s = PA_get_next_line();
        if (s == NULL)
           {if (fp != stdin)
               {io_close(fp);
                _PA.input_stream = stdin;};

            break;};

        if (SC_blankp(s, "c#"))
           continue;

	token = PA_get_field("COMMAND", "PARSE", REQU);
        if (token != NULL)
           {hp = SC_hasharr_lookup(PA_gs.command_tab, token);
            if (hp != NULL)
               {cp = (PA_command *) hp->def;
                (*(cp->handler))(cp);}

            else if (errfnc != NULL)
               (*errfnc)(token);};};

    _PA.input_stream = old_stream;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_NEXT_LINE - get the next line of input from the current
 *                  - input stream
 *                  - has the same return semantics as fgets
 */

char *PA_get_next_line(void)
   {char *s;

    _PA.input_flag = TRUE;

    s = GETLN(_PA.input_bf, MAXLINE, _PA.input_stream);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_FIELD - get the next field from the command string */

char *PA_get_field(const char *s, const char *t, int optp)
   {char *token, *b;

    b = (_PA.input_flag) ? _PA.input_bf : NULL;

    token = SC_strtok(b, PA_gs.token_delimiters, PA_gs.strtok_p);
    PA_ERR(((token == NULL) && (optp == REQU)),
           "Bad %s field in %s command\n", s, t);

    _PA.input_flag = FALSE;

    return(token);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_NUM_FIELD - get a numeric field
 *                  - does alias expansion
 */

double PA_get_num_field(const char *s, const char *t, int optp)
   {char *token;
    double val;

    token = PA_get_field(s, t, optp);

    if ((token == NULL) && (optp == OPTL))
       val = 0.0;
    else
       val = PA_alias_value(token);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INIT_SYSTEM - initialize the code system
 *                - connect the global variables, open the post-processor
 *                - file and the edit file, and run the package initializers
 */

void _PA_init_cont(void)
   {PA_package *pck;
    PFPkgDefcnt pck_defcnt;

/* initialize the package controls */
    for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
        {pck_defcnt = pck->defcnt;
         if (pck_defcnt != NULL)
            (*pck_defcnt)(pck);};

/* reset the global control arrays */
    PA_control_set("global");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_HA_RL_VARIABLE - release a variable in a halem */

static int _PA_ha_rl_variable(haelem *hp, void *a)
   {int ok;
    char *name, *type;
    PA_variable *v;
    hasharr *ha;

    ok = SC_haelem_data(hp, &name, &type, (void **) &v, TRUE);
    if (ok == TRUE)
       {ha = (hasharr *) a;
	if (strcmp(type, PA_gs.variable) == 0)
	   _PA_rl_variable(v);
        SC_hasharr_remove(ha, name);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CLEAR - re-initialize the system in preparation for new IV problem */

void PA_clear(void)
   {

    SC_hasharr_foreach(PA_gs.variable_tab, _PA_ha_rl_variable, PA_gs.variable_tab);

    _PA_rl_spec(PA_gs.iv_spec_lst);
    _PA_rl_request(PA_gs.plot_reqs);

    PA_gs.plot_reqs   = NULL;
    PA_gs.iv_spec_lst = NULL;

    _PA_init_cont();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PACKH - handle the package command */

void PA_packh(void)
   {char *s;
    PA_package *pck;

/* get the package name */
    s = PA_get_field("NAME", "PACKAGE", REQU);

    pck = PA_INQUIRE_PACKAGE(s);
    if (pck == NULL)
       {PRINT(stdout, "This code has no %s package - ending\n", s);
	exit(2);};

/* set the control pointers */
   PA_control_set(s);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PSHAND - handle the setting of switches and parameters */

void PA_pshand(PA_command *cp)
   {int i, ival;
    double fval;
    char *sval;

    if (strcmp(cp->name, "switch") == 0)
       {i    = SC_stoi(PA_get_field("INDEX", "SWITCH", REQU));
        ival = SC_stoi(PA_get_field("VALUE", "SWITCH", REQU));
        SWTCH[i] = ival;}

    else if (strcmp(cp->name, "parameter") == 0)
       {i    = SC_stoi(PA_get_field("INDEX", "PARAMETER", REQU));
        fval = PA_alias_value(PA_get_field("VALUE", "PARAMETER", REQU));
        PARAM[i] = fval;}

    else if (strcmp(cp->name, "name") == 0)
       {i    = SC_stoi(PA_get_field("INDEX", "NAME", REQU));
        sval = SC_strtok(NULL, "\n\r", PA_gs.strtok_p);
        NAME[i] = CSTRSAVE(sval);}

    else if (strcmp(cp->name, "unit") == 0)
       {i    = SC_stoi(PA_get_field("INDEX", "UNIT", REQU));
        fval = PA_alias_value(PA_get_field("VALUE", "UNIT", REQU));
        PA_gs.units[i] = fval;}

    else if (strcmp(cp->name, "conversion") == 0)
       {i    = SC_stoi(PA_get_field("INDEX", "CONVERSION", REQU));
        fval = PA_alias_value(PA_get_field("VALUE", "CONVERSION", REQU));
        PA_gs.convrsns[i] = fval;}

    else
       {int did;
	double d;

	i    = cp->num;
        sval = PA_get_field("VALUE", cp->name, REQU);
	d    = PA_alias_value(sval);
	did  = cp->type;

	if (SC_is_type_num(did) == TRUE)
	   SC_convert_id(did, cp->vr, i, 1, G_DOUBLE_I, &d, 0, 1, 1, FALSE);

        else if ((did == G_CHAR_I) || (did == G_STRING_I))
           ((char **) cp->vr)[i] = CSTRSAVE(sval);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_NPLOTH - handler for the new plot command */

void PA_nploth(void)
   {char *spec, *ran, *dom, text[MAXLINE];
    PA_set_spec *range, *domain;

    PA_gs.n_graphs++;

    spec = SC_strtok(NULL, "\n", PA_gs.strtok_p);
    if (spec != NULL)
       SC_strncpy(text, MAXLINE, spec, -1);

    ran = SC_strtok(spec, "{(", PA_gs.strtok_p);
    dom = SC_strtok(NULL, "\n", PA_gs.strtok_p);

    range = NULL;
    while ((spec = SC_strtok(ran, "{,}", PA_gs.strtok_p)) != NULL)
       {range = _PA_proc_set_spec(spec, range);
        ran   = NULL;};

    domain = NULL;
    while ((spec = SC_strtok(dom, ",)", PA_gs.strtok_p)) != NULL)
       {domain = _PA_proc_set_spec(spec, domain);
        dom    = NULL;};

/* reverse the order of the range specifications */
    SC_REVERSE_LIST(PA_set_spec, range, next);

/* reverse the order of the domain specifications */
    SC_REVERSE_LIST(PA_set_spec, domain, next);

    PA_gs.plot_reqs = _PA_mk_plot_request(range, domain, text, PA_gs.plot_reqs);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TIME_PLOT - make a time plot request from the source */

void PA_time_plot(const char *rname, void *vr)
   {char text[MAXLINE];
    PA_set_spec *range, *domain;

    PA_gs.n_graphs++;

    snprintf(text, MAXLINE, "%s->{t}", rname);
    range  = _PA_proc_set_spec(rname, NULL);
    domain = _PA_proc_set_spec("t", NULL);

    PA_gs.plot_reqs             = _PA_mk_plot_request(range, domain, text,
						      PA_gs.plot_reqs);
    PA_gs.plot_reqs->data       = (PM_set *) vr;
    PA_gs.plot_reqs->data_index = 0;
    PA_gs.plot_reqs->conv       = 1.0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_MK_PLOT_REQUEST - create a new plot request structure
 *                     - and initialize it
 */

PA_plot_request *_PA_mk_plot_request(PA_set_spec *range, PA_set_spec *domain,
				     const char *text, PA_plot_request *next)
   {PA_plot_request *req;

    req = CMAKE(PA_plot_request);

    req->range            = range;
    req->range_name       = NULL;
    req->domain           = domain;
    req->base_domain_name = NULL;
    req->domain_map       = NULL;
    req->text             = CSTRSAVE(text);
    req->time_plot        = FALSE;
    req->mesh_plot        = FALSE;
    req->status           = EDIT;
    req->size             = 0L;
    req->offset           = 0L;
    req->stride           = -1L;
    req->str_index        = -1;
    req->conv             = 0.0;
    req->centering        = U_CENT;
    req->allocate_data    = TRUE;
    req->data             = NULL;
    req->data_index       = (int) -INT_MAX;
    req->data_type        = G_DOUBLE_I;
    req->next             = next;

    return(req);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_FUNCTION_FORM - fill the SPEC with appropriate values and return
 *                  - TRUE iff T is of the form func(a;b;c)
 */

int PA_function_form(const char *t, PA_set_spec *spec)
   {int n, ret;
    char *ps;
    char fnc[MAXLINE], s[MAXLINE], arg[MAXLINE];
    double vc;
    SC_array *val;

    SC_strncpy(s, MAXLINE, t, -1);

    val = CMAKE_ARRAY(double, NULL, 0);

    ps = strchr(s, '(');
    if (ps != NULL)
       {if (*s == '(')
           SC_strncpy(fnc, MAXLINE, "limit", -1);
	else
	   SC_strncpy(fnc, MAXLINE, SC_firsttok(s, " \t()"), -1);

/* NOTE: the "(" will come from forms like a=(i;j;k)
 *       and fall under the "limit" operator
 */
	SC_strncpy(arg, MAXLINE, SC_firsttok(s, "()"), -1);
	while (TRUE)
	   {ps = SC_firsttok(arg, ";");
	    if (ps != NULL)
	       {vc = PA_alias_value(ps);
		SC_array_push(val, &vc);}
	    else
	       break;};
	
	n = SC_array_get_n(val);

	PA_ERR(((n != 3) && (strcmp(fnc, "step") == 0)),
	       "MISSING STEP VALUE - PA_FUNCTION_FORM");

	ret = TRUE;}

/* if there is no match define this to be the case "var=<string>" */
    else
       {SC_strncpy(fnc, MAXLINE, t, -1);

	ret = FALSE;};

    spec->function = CSTRSAVE(fnc);
    spec->n_values = SC_array_get_n(val);
    spec->values   = SC_array_done(val);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROC_SET_SPEC - process an set element variable specification
 *                   - in a plot request
 */

PA_set_spec *_PA_proc_set_spec(const char *s, PA_set_spec *lst)
   {char t[MAXLINE], *token;
    double vc;
    PA_set_spec *spec;

    SC_strncpy(t, MAXLINE, s, -1);

    spec = CMAKE(PA_set_spec);

    token = SC_firsttok(t, " \t=");
    if (token == NULL)
       return(lst);

    spec->var_name = CSTRSAVE(token);
    spec->text     = NULL;
    spec->values   = NULL;

/* handle case "var" */
    token = SC_firsttok(t, "\n");
    if (token == NULL)
       spec->function = NULL;

/* handle case "var=#" */
    else
       {vc = PA_alias_value(token);
	if (SC_numstrp(token) || (vc != -2.0*HUGE))
	   {spec->function = NULL;
	    spec->n_values = 1;
	    spec->values   = CMAKE(double);
	    *spec->values  = vc;}

/* handle cases "var=func(#;#;#)" and "var=<string>" */
	else
	   PA_function_form(token, spec);};

    spec->next = lst;

    return(spec);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_WRRSTRTH - handle the writing of the restart dump
 *             - first see to the building of the data base and
 *             - the proper final initialization of the pure panacea
 *             - variables
 */

void PA_wrrstrth(void)
   {PFRegSetup db_hook;

/* reverse the order of the new plot requests */
    SC_REVERSE_LIST(PA_plot_request, PA_gs.plot_reqs, next);

    PA_proc_units();

/* intern the controls */
    PA_INTERN(PA_gs.units, "unit");
    PA_INTERN(PA_gs.convrsns, "convrsn");

/* intern the plot requests */
    PA_INTERN(PA_gs.plot_reqs, "plot-requests");

/* intern the initial value data list after the package data have been
 * interned
 */
    PA_proc_iv_spec(PA_gs.iv_spec_lst);
    PA_INTERN(PA_gs.iv_spec_lst, "initial-value-specifications");

/* an extra hook for anything which does NOT fall under the control of
 * intern_pck_db
 */
    db_hook = PA_GET_FUNCTION(PFRegSetup, "setup_region");
    if (db_hook != NULL)
       (*db_hook)();

    PA_intern_pck_db();

    _PA_wrrstrt(_PA.rsname, EXT_INT);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DONE - gracefully leave the generator */

NORETURN void PA_done(void)
   {

    PN_close_member(PA_gs.pp);

    LONGJMP(SC_gs.cpu, ERR_FREE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_NAME_FILES - name the various files and log them in the name array */

void PA_name_files(const char *base, char **ped, char **prs,
		   char **ppp, char **pgf)
   {char s[50], t[MAXLINE];
    char *token, *pt, *nm;

    nm = CSTRSAVE(base);

/* silence compilers that warn about uninitialized variables */
    pt = NULL;

/* strip off any directory names */
    token = SC_strtok(nm, "/\\:", pt);
    if (token != NULL)
       SC_strncpy(t, MAXLINE, token, -1);

    CFREE(nm);

    while ((token = SC_strtok(NULL, "/\\:", pt)) != NULL)
       SC_strncpy(t, MAXLINE, token, -1);

/* name the first edit file */
    if (ped != NULL)
       {snprintf(s, 50, "%s.e00", t);
	*ped = CSTRSAVE(s);};

/* name the restart dump file */
    if (prs != NULL)
       {snprintf(s, 50, "%s.r00", t);
	*prs = _PA.rsname = CSTRSAVE(s);};

/* name the post-processor file */
    if (ppp != NULL)
       {snprintf(s, 50, "%s.t00", t);
	*ppp = CSTRSAVE(s);};

/* name the post-processor file */
    if (pgf != NULL)
       {snprintf(s, 50, "%s.s00", t);
	*pgf = CSTRSAVE(s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
