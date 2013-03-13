/*
 * SCMKPR.C - parse Makefile syntax
 *          - provide an API to produce a table of rules
 *          - for subseqent dependency analysis
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_make.h"

#define DEFAULT_BARRIER   "@sync"

typedef struct s_mkvdes mkvdes;

struct s_mkvdes
   {int line;
    char *text;};

/*--------------------------------------------------------------------------*/

/*                           VARIABLE DATABASE                              */

/*--------------------------------------------------------------------------*/

/* _SC_VAR_INSTALL - install variable NAME with value VAL in STATE */

void _SC_var_install(anadep *state, char *name, char *val, int line)
   {mkvdes *vd;

    vd = SC_hasharr_def_lookup(state->variables, name);
    if ((vd == NULL) || (vd->line != -2))
       {vd = CMAKE(mkvdes);
	vd->line = line;
	vd->text = CSTRSAVE(val);

	SC_hasharr_install(state->variables, name, vd, "mkvdes", 3, -1);};

    if (state->show_vars)
       {switch (line)
	   {case -2 :
	         io_printf(state->log, "Variable> %s = %s (command line)\n",
			   name, val);
		 break;
	    case -1 :
	         io_printf(state->log, "Variable> %s = %s (built in)\n",
			   name, val);
		 break;
	    default :
	         io_printf(state->log, "Variable> %s = %s (line %d)\n",
			   name, val, line);
		 break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_VAR_LOOKUP - lookup variable NAME in the STATE variable table */

char *_SC_var_lookup(anadep *state, char *name)
   {char *vl;
    mkvdes *vd;

    vd = SC_hasharr_def_lookup(state->variables, name);
    vl = (vd != NULL) ? vd->text : NULL;

    return(vl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_MKVDES - free a variable table entry */

static int _SC_free_mkvdes(haelem *hp, void *a)
   {mkvdes *vd;

    vd = hp->def;

    CFREE(vd->text);
    CFREE(vd);

    hp->def = NULL;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_VARIABLES - free the variable table */

static void _SC_free_variables(anadep *state)
   {

    if (state->variables != NULL)
       {SC_free_hasharr(state->variables, _SC_free_mkvdes, NULL);

	state->variables = NULL;};

    return;}

/*--------------------------------------------------------------------------*/

/*                             RULE DEFINITION                              */

/*--------------------------------------------------------------------------*/

/* _SC_PREP_RULE - make sure we have enough space for the rule text */

static void _SC_prep_rule(ruledef *a, char *s)
   {int nt, ns, nx, nn;
    char *tx;

    ns = strlen(s);
    if (a->text == NULL)
       {nx      = 8*ns;
	a->text = CMAKE_N(char, nx);
	a->nc   = nx;

	memset(a->text, 0, nx);};

    nx = a->nc;
    tx = a->text;

    nt = strlen(tx);
    nn = (nt + ns) << 1;

    if (nn > nx)
       {CREMAKE(a->text, char, nn);
	a->nc = nn;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_START_RULE - begin defining a rule */

static void _SC_start_rule(ruledef *a, char *s, int line)
   {

    _SC_prep_rule(a, s);

    strcpy(a->text, s);
    a->line = line;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ADD_RULE - add to the specified rule */

static void _SC_add_rule(ruledef *a, char *s)
   {char *p;

    _SC_prep_rule(a, s);

    p = strstr(a->text, "\\\n");
    if (p != NULL)
       *p = '\0';

    for (p = s; SC_is_print_char(*p, 3) != FALSE; p++);

    strcat(a->text, p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_RULE - tokenize the text S by DELIM
 *                  - and return the number of tokens in PN
 *                  - and the tokens strings in PT
 */

static void _SC_process_rule(int *pn, char ***pt, char *s, char *delim)
   {int n;
    char **sa;

    n  = 0;
    sa = NULL;

    if (s != NULL)
       {sa = SC_tokenize(s, delim);
	if (sa != NULL)
	   {for (n = 0; sa[n] != NULL; n++);
	    if (n == 0)
	       CFREE(sa);};};

    *pn = n;
    *pt = sa;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MERGE_RULE - merge additional dependencies DEP or actions ACT
 *                - into an existing rule ORD
 *                - according to the policies for each kind
 */

static void _SC_merge_rule(char *tgt, char *dep, char *act,
			   ruledes *ord, anadep *state)
   {char *odp, *ndp;

/* add the dependencies */
    odp = ord->depend;
    if (odp == NULL)
       ord->depend = CSTRSAVE(dep);

    else
       {ndp = SC_dsnprintf(TRUE, "%s %s", odp, dep);

	ord->depend = ndp;
	CFREE(odp);};

/* replace the actions */
    if ((act != NULL) && (act[0] != '\0') && (act != ord->act))
       {if (state->verbose)
	   {FILE *fp;

	    fp = state->log;

	    io_printf(fp, "PACT: overriding actions for target '%s'\n", tgt);
	    io_printf(fp, "PACT: old actions:\n %s", ord->act);
	    io_printf(fp, "PACT: new actions:\n %s", act);};

	CFREE(ord->act);
	ord->act = CSTRSAVE(act);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_RULE - allocate and initialize a ruledes instance */

static ruledes *_SC_make_rule(char *tname, char *fname, int line,
			      char *dep, char *act)
   {ruledes *rd;

    rd = CMAKE(ruledes);

    rd->name           = CSTRSAVE(tname);
    rd->file           = CSTRSAVE(fname);
    rd->line           = line;
    rd->n_dependencies = 0;
    rd->n_actions      = 0;
    rd->dependencies   = NULL;
    rd->actions        = NULL;
    rd->depend         = CSTRSAVE(dep);
    rd->act            = CSTRSAVE(act);

    return(rd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_COPY_RULE - make a copy of the rule RD but give the name NM */

static ruledes *_SC_copy_rule(char *nm, ruledes *rd)
   {ruledes *nrd;

    nrd = _SC_make_rule(nm, rd->file, rd->line, rd->depend, rd->act);

    return(nrd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_RULE_INFO - as a diagnostic
 *                     - print out helpful info about a rule
 */

void _SC_print_rule_info(anadep *state, int n, ruledes *rd)
   {int i, na, ns, whch;
    char *tgt, *dep, **acts;
    cmdes *as;
    FILE *fp;

    fp = state->log;

    ns = SC_array_get_n(state->actions);
    if (ns > 0)
       {if ((n < 0) || (ns < n))
	   n = ns - 1;

	as = SC_array_get(state->actions, n);

	tgt  = as->target;
	dep  = as->dependent;
	whch = as->kind;
	na   = as->n;
	acts = as->actions;

	n++;

	if (rd == NULL)
	   {switch (whch)
	       {case IMPLICIT :
		     io_printf(fp, "#%d implicit rule\n", n);
		     break;
		case ARCHIVE :
		     io_printf(fp, "#%d archive rule\n", n);
		     break;
	        default:
		     io_printf(fp, "#%d explicit rule\n", n);
		     break;};}

	else
	   {switch (whch)
	       {case IMPLICIT :
		     io_printf(fp, "#%d implicit rule (%s) %s:%d\n",
			       n, rd->name, rd->file, rd->line);
		     break;
		case ARCHIVE :
		     io_printf(fp, "#%d archive rule  (%s) %s:%d\n",
			       n, rd->name, rd->file, rd->line);
		     break;
		default:
		     io_printf(fp, "#%d explicit rule  %s:%d\n",
			       n, rd->file, rd->line);
		     break;};};

	io_printf(fp, "     %s\n", tgt);
	io_printf(fp, "  <- %s\n", dep);
	for (i = 0; i < na; i++)
	    io_printf(fp, "     %s\n", acts[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_END_RULE - finish the definition of the specified rule */

static void _SC_end_rule(ruledef *a, anadep *state)
   {char *name, *dep, *act;
    ruledes *rd, *ord;

    if (a->text != NULL)
       {name = strtok(a->text, ":");
	name = SC_trim_right(name, " \t");
	dep  = strtok(NULL, "");

	dep = SC_trim_left(dep, ": \t");
        if (*dep == '\n')
	   {act = dep + 1;
	    dep = NULL;}
	else
	   {act = strchr(dep, '\n');
	    if (act != NULL)
	       *act++ = '\0';};

	ord = (ruledes *) SC_hasharr_def_lookup(state->rules, name);

/* if the rule already exists process the new specifications */
	if (ord != NULL)
           _SC_merge_rule(name, dep, act, ord, state);

	else
	   {rd = _SC_make_rule(name, a->name, a->line, dep, act);
	    SC_hasharr_install(state->rules, name, rd, "ruledes", 3, -1);};

	CFREE(a->text);
	a->nc   = 0;
        a->line = -1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_DEF_RULE - define a rule
 *                  - RULE is a string of the form:
 *                  -   "<tgt> : <dep>+\n<act>+"
 *                  -   <tgt> := target for the rule
 *                  -   <dep> := whitespace delimited dependency
 *                  -   <act> := newline delimited action/command
 *                  -            which must begin with a TAB character
 *                  - <>+ means zero or more
 */

void SC_make_def_rule(anadep *state, char *rule)
   {ruledef a;

    a.text = NULL;
    a.nc   = 0;

    _SC_prep_rule(&a, rule);

    strcpy(a.text, rule);
    a.line    = -1;
    a.name[0] = '\0';

    _SC_end_rule(&a, state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REL_RULE - release a single rule */

static int _SC_rel_rule(haelem *hp, void *a)
   {int ok;
    char **sa, *name;
    ruledes *rd;

    ok = SC_haelem_data(hp, &name, NULL, (void **) &rd, FALSE);
    SC_ASSERT(ok == TRUE);

    if (rd != NULL)
       {if (SC_ref_count(rd) < 2)
	   {sa = rd->dependencies;
	    SC_free_strings(sa);
	    rd->dependencies = NULL;

	    sa = rd->actions;
	    SC_free_strings(sa);
	    rd->actions = NULL;

	    CFREE(rd->act);
	    CFREE(rd->depend);

	    CFREE(rd->name);
	    CFREE(rd->file);};};

/*    CFREE(rd); */

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_RULES - release the rule table */

void _SC_free_rules(anadep *state)
   {

    SC_hasharr_clear(state->rules, _SC_rel_rule, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_RULE - print a single rule */

static int _SC_print_rule(haelem *hp, void *arg)
   {int i, n, ok;
    char **a, *name;
    ruledes *rd;
    anadep *state;
    FILE *fp;

    state = (anadep *) arg;
    fp    = state->log;

    ok = SC_haelem_data(hp, &name, NULL, (void **) &rd, FALSE);
    SC_ASSERT(ok == TRUE);

    io_printf(fp, "Rule> %s\n", name);

    n = rd->n_dependencies;
    a = rd->dependencies;
    for (i = 0; i < n; i++)
        io_printf(fp, "   dep: %s\n", a[i]);

    n = rd->n_actions;
    a = rd->actions;
    for (i = 0; i < n; i++)
        io_printf(fp, "   act: %s\n", a[i]);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SHOW_MAKE_RULES - print out all of the known rules */

void SC_show_make_rules(anadep *state)
   {FILE *fp;

    fp = state->log;

    if (state->show_rules == TRUE)
       {io_printf(fp,
		  "-------------------------------------------------------\n");

	SC_hasharr_foreach(state->rules, _SC_print_rule, state);

	io_printf(fp,
		  "-------------------------------------------------------\n");};

    return;}

/*--------------------------------------------------------------------------*/

/*                           VARIABLE DEFINITION                            */

/*--------------------------------------------------------------------------*/

/* _SC_START_VAR - begin defining a variable */

static void _SC_start_var(vardef *v, char *s, int line)
   {

    v->text = SC_dstrcpy(v->text, s);
    v->line = line;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ADD_VAR - add to the specified variable */

static void _SC_add_var(vardef *v, char *s)
   {char *p;

    p = strstr(v->text, "\\\n");
    if (p != NULL)
       *p = '\0';

    for (p = s; SC_is_print_char(*p, 3) != FALSE; p++);
    
    v->text = SC_dstrcat(v->text, " ");
    v->text = SC_dstrcat(v->text, p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_END_VAR - finish the definition of the specified variable */

static void _SC_end_var(vardef *v, anadep *state)
   {char *name, *text;

    if (v->text != NULL)
       {name = strtok(v->text, "= \t");
	text = strtok(NULL, "\n");
	if (text != NULL)
	   text = SC_trim_left(text, "= \t");
	else
	   text = "";

	_SC_var_install(state, name, text, v->line);

	CFREE(v->text);
        v->line = -1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_DEF_VAR - define a variable */

void SC_make_def_var(anadep *state, char *var, int wh)
   {vardef v;

    v.text    = SC_dstrcpy(NULL, var);
    v.line    = wh;
    v.name[0] = '\0';

    _SC_end_var(&v, state);

    CFREE(v.text);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BEGIN_VAR_REF - find the beginning of a variable reference in S
 *                   - which will be of the form
 *                   -   ${foo}, ${...${foo}...}
 *                   - or
 *                   -   $(foo), $(...$(foo)...)
 */

static char *_SC_begin_var_ref(ruledes *rd, char *s)
   {int i, id, np, nd, ok;
    int cp, cc, cn;
    char *p, *pm;

    id = -1;
    np = 0;
    nd = 0;
    ok = TRUE;
    p  = NULL;

    cp = '\0';
    cc = s[0];
    for (i = 0; (s[i] != '\0') && (ok == TRUE); i++)
        {cn = s[i+1];
	 switch (cc)
	    {case '{' :
	     case '(' :
	          np++;
		  break;
	     case '}' :
	     case ')' :
	          np--;
		  if ((id != -1) && (nd > 0))
		     {ok = FALSE;
		      p = s + id;};
		  break;
	     case '$' :
	          pm = strchr("$<@*%", cn);
	          if ((pm == NULL) && (cp != '$'))
		     {nd++;
		      id = i;}
/*
		  else if ((pm != NULL) && (rd != NULL))
		     {char *sfx, *mac;
		      sfx = NULL;

		      mac = _SC_subst_macro(s, i, EXPLICIT,
					    rd->name, rd->depend,
					    sfx);
		      i  = -1;
		      cp = '\0';
		      cc = s[0];};
*/
		  break;};
	 cp = cc;
         cc = cn;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_END_VAR_REF - find the end of a variable reference in S
 *                 - which will be of the form
 *                 -   ${foo}
 *                 - or
 *                 -   $(foo)
 */

static char *_SC_end_var_ref(char *s)
   {char c;

    for (c = *s++; (c != '}') && (c != ')'); c = *s++);

    s--;

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SUBST_MACRO - carry out the substitutions for the
 *                 - the macros:
 *                 -    $@  the full target
 *                 -    $*  base of the full target (i.e. $@:r)
 *                 -    $<  the dependent file
 *                 -    $%  the name of the archive member
 *                 -    $$  a single '$'
 *                 - the returned string must be freshly allocated
 */

char *_SC_subst_macro(char *src, int off, SC_rule_cat whch, int exd,
		      char *tgt, char *dep, char *sfx)
   {int c, nf, ns, nr;
    char s[MAXLINE];
    char *dst, *base, *p, *t;

    if (src == NULL)
       dst = NULL;

    else
       {p = strchr(src+off, '$');
	if (p == NULL)
	   {dst = CSTRSAVE(src);
	    CFREE(src);}

	else
	   {*p++ = '\0';
	    c    = *p++;
	    base = NULL;
	    switch (c)
	       {case '*':
		     SC_strncpy(s, MAXLINE, tgt, -1);
		     if (whch == ARCHIVE)
		        {t = strtok(s, "(");
			 t = strtok(NULL, ")");
			 base = strtok(t, ".");}
		     else
			base = strtok(s, ".");
		     break;

		case '@':
		     base = tgt;
		     break;

		case '<':
		     SC_strncpy(s, MAXLINE, tgt, -1);
		     if (whch == ARCHIVE)
		        {t = strtok(s, "(");
			 t = strtok(NULL, ")");
			 if (t != NULL)
			    {base = strrchr(t, '.');
			     if (base != NULL)
			        base[1] = sfx[1];
			     base = t;};}
		     else if (whch == EXPLICIT)
		        base = dep;
		     else
		        {base = strrchr(s, '.');
			 if (base != NULL)
			    base[1] = sfx[1];
			 base = s;};
		     break;

		case '%':
		     io_printf(stdout, "Not handling $% macro yet\n");
		     dst = NULL;
		     break;

		case '$':
		     snprintf(s, MAXLINE, "$");
		     base = s;
		     break;

		default:
		     snprintf(s, MAXLINE, "$%c", c);
		     base = s;
		     break;};

	    nf  = strlen(src);
	    ns  = (base != NULL) ? strlen(base) : 0;
	    nr  = strlen(p);
	    dst = CMAKE_N(char, nf+ns+nr+2);
	    snprintf(dst, nf+ns+nr+2,
		     "%s%s%s", src, base, p);
	    CFREE(src);

	    dst = _SC_subst_macro(dst, nf+ns, whch, exd, tgt, dep, sfx);};};

    return(dst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SUBST_STR - recursively substitute variable references in the
 *               - given string SRC until there are no more
 *               - each substitution goes into DST and sent to the next pass
 *               - return DST when done
 */

static char *_SC_subst_str(anadep *state, ruledes *rd, char *src)
   {int i;
    char *pf, *pe, *dst, *text;

    for (i = 0; TRUE; i++)
        {pf = _SC_begin_var_ref(rd, src);
	 if (pf == NULL)

/* DST absolutely must be a fresh copy */
	    {dst = CSTRSAVE(src);
	     CFREE(src);
	     break;}

	 else
	    {*pf++ = '\0';
	     *pf++ = '\0';

	     pe = _SC_end_var_ref(pf);
	     *pe++ = '\0';

             if ((rd != NULL) && (strchr(pf, '$') != NULL))
	        pf = _SC_subst_macro(pf, 0, EXPLICIT, FALSE,
		                     rd->name, rd->depend,
				     NULL);

	     text = _SC_var_lookup(state, pf);
	     if (text == NULL)
	        dst = SC_dsnprintf(TRUE, "%s%s", src, pe);
	     else
	        dst = SC_dsnprintf(TRUE, "%s%s%s", src, text, pe);

	     CFREE(src);
	     src = dst;};};

    return(dst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SUBST_STRINGS - substitute all variable references in A
 *                   - return the number of substitutions made
 */

static int _SC_subst_strings(anadep *state, ruledes *rd, char **a)
   {int ns;
    char *pa, *pb;

    ns = 0;
    if (*a != NULL)
       {pa = *a;
	pb = _SC_subst_str(state, rd, *a);
	if (pa != pb)
	   {ns++;
	    *a = pb;
/*	    CFREE(pa); */};};

    return(ns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_SUBST_NAME - do all variable substitutions in a rule name */

static int _SC_do_subst_name(haelem *hp, void *a)
   {int i, ok;
    char *s, *t, *ps;
    char *key;
    ruledes *rd, *erd, *nrd, *enr;
    anadep *state;

    ok = SC_haelem_data(hp, &key, NULL, (void **) &rd, FALSE);
    SC_ASSERT(ok == TRUE);

    state = (anadep *) a;

/* do substitutions in the rule name and enter the expanded rule
 * into the new rule table
 */
    s = CSTRSAVE(key);
    _SC_subst_strings(state, NULL, &s);

/* the rule name may have expanded into more than one item
 * enter the rule under each item
 */
    for (i = 0, ps = s; TRUE; i++, ps = NULL)
        {t = strtok(ps, " \t");
	 if (t == NULL)
	    break;

	 enr = (ruledes *) SC_hasharr_def_lookup(state->temp, t);
	 if (enr == NULL)
	    {erd = (ruledes *) SC_hasharr_def_lookup(state->rules, t);

/* if there already is a rule merge to the existing rule */
	     if ((erd != NULL) && (erd != rd))
	        {_SC_merge_rule(t, rd->depend, rd->act, erd, state);
		 nrd = erd;}
	     else
                nrd = _SC_copy_rule(t, rd);

	     SC_hasharr_install(state->temp, t, nrd, "ruledes", 3, -1);}
	 else
	    _SC_merge_rule(t, rd->depend, rd->act, enr, state);};

    CFREE(s);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_SUBST_BODY - do all variable substitutions in rule
 *                   - dependencies and actions
 */

static int _SC_do_subst_body(haelem *hp, void *a)
   {int ok;
    ruledes *rd;
    anadep *state;

    state = (anadep *) a;
    ok    = SC_haelem_data(hp, NULL, NULL, (void **) &rd, FALSE);
    SC_ASSERT(ok == TRUE);

/* do substitutions in the dependencies and actions */
    _SC_subst_strings(state, rd, &rd->depend);
    _SC_subst_strings(state, rd, &rd->act);

/* now breaks dependencies and actions down into arrays */
    _SC_process_rule(&rd->n_dependencies, &rd->dependencies,
		     rd->depend, " \t");

    _SC_process_rule(&rd->n_actions, &rd->actions,
		     rd->act, "\n");

    CFREE(rd->depend);
    CFREE(rd->act);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DO_SUBSTITUTIONS - do all variable substitutions in all rules */

void _SC_do_substitutions(anadep *state)
   {

    if ((state->rules != NULL) && (state->need_subst == TRUE))
       {state->temp = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

/* do substitutions in the rule names */
	SC_hasharr_foreach(state->rules, _SC_do_subst_name, state);

/* now that the names are expanded replace the old table with the new one */
	_SC_free_rules(state);

	state->rules = state->temp;
	state->temp  = NULL;

/* do substitutions in the remainder of the rule */
	SC_hasharr_foreach(state->rules, _SC_do_subst_body, state);

	state->need_subst = FALSE;}

    return;}

/*--------------------------------------------------------------------------*/

/*                             HIGH LEVEL MAKE                              */

/*--------------------------------------------------------------------------*/

/* _SC_INIT_SUFFICES - build the suffix list */

static void _SC_init_suffices(anadep *state)
   {SC_array *a;

    a = SC_STRING_ARRAY();

    state->suffices = a;

    SC_array_string_add_copy(a, ".c");
    SC_array_string_add_copy(a, ".h");
    SC_array_string_add_copy(a, ".y");
    SC_array_string_add_copy(a, ".l");
    SC_array_string_add_copy(a, ".i");
    SC_array_string_add_copy(a, ".f");
    SC_array_string_add_copy(a, ".f90");
    SC_array_string_add_copy(a, ".c++");
    SC_array_string_add_copy(a, ".cxx");
    SC_array_string_add_copy(a, ".C");
    SC_array_string_add_copy(a, ".H");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_STATE - allocate and initialize an anadep instance */

anadep *SC_make_state(void)
   {char *s;
    anadep *state;

    state = CMAKE(anadep);

    state->complete    = FALSE;
    state->literal     = FALSE;
    state->quotes      = FALSE;
    state->need_suffix = TRUE;
    state->need_subst  = TRUE;
    state->show_vars   = FALSE;
    state->show_rules  = FALSE;
    state->verbose     = FALSE;
    state->log         = stdout;
    state->rules       = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);
    state->variables   = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);
    state->archives    = NULL;
    state->pred        = _SC_is_newer;

    s = SC_dsnprintf(TRUE, "SHELL = %s", DEFAULT_SHELL);
    SC_make_def_var(state, s, -1);
    CFREE(s);

    s = SC_dsnprintf(TRUE, "BARRIER = %s", DEFAULT_BARRIER);
    SC_make_def_var(state, s, -1);
    CFREE(s);

    state->actions = CMAKE_ARRAY(cmdes, NULL, 0);

    _SC_init_suffices(state);

    return(state);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REL_AR - release an archive */

static int _SC_rel_ar(haelem *hp, void *a)
   {int ok;
    fcdes *ar;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &ar, TRUE);
    SC_ASSERT(ok == TRUE);

    SC_free_fcontainer(ar);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_ARCHIVES - release all archives in STATE */

static void _SC_free_archives(anadep *state)
   {

    if (state->archives != NULL)
       {SC_hasharr_clear(state->archives, _SC_rel_ar, NULL);

	state->archives = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_STATE - release an anadep instance */

void SC_free_state(anadep *state)
   {

    _SC_free_archives(state);
    _SC_free_rules(state);
    _SC_free_variables(state);

    SC_free_array(state->actions, NULL);
    SC_free_array(state->suffices, NULL);

    CFREE(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PARSE_MAKEFILE - parse the file FNAME
 *                   - building up the variable and rule tables
 *                   - return TRUE iff successful
 */

int SC_parse_makefile(anadep *state, char *fname)
   {int i, na, nb, rv, ok, st;
    char *s, *pcl, *t, *pt;
    char **sa;
    FILE *fp;
    ruledef a;
    vardef v;

    rv = FALSE;
    ok = SC_ERR_TRAP();
    if (ok == 0)
       {state->need_suffix = TRUE;
	state->need_subst  = TRUE;

	fp = io_open(fname, "r");
	if (fp == NULL)
	   SC_error(-1, "cannot open file %s", fname);

	else
	   {SC_strncpy(v.name, MAXLINE, fname, -1);
	    SC_strncpy(a.name, MAXLINE, fname, -1);
	    v.text = NULL;
	    a.text = NULL;
	    a.nc   = 0;

	    s  = NULL;
	    nb = 0;
	    st = TRUE;

	    for (i = 1; st == TRUE; i++)
	        {s = SC_dgets(s, &nb, fp);
		 if (s == NULL)
		    break;

		 if (SC_blankl(s, "#") == TRUE)
		    {_SC_end_rule(&a, state);
		     _SC_end_var(&v, state);}

		 else if (s[0] == '\t')
		    {if (a.text != NULL)
		        _SC_add_rule(&a, s);
		     else if (v.text != NULL)
		        _SC_add_var(&v, s);}

		 else if ((strncmp(s, "include ", 8) == 0) ||
			  (strncmp(s, "include\t", 8) == 0))
		    {t  = strtok(s+8, " \t\n\r");
		     pt = CSTRSAVE(t);
		     _SC_end_rule(&a, state);
		     _SC_end_var(&v, state);
		     _SC_subst_strings(state, NULL, &pt);
		     st = SC_parse_makefile(state, pt);
		     if (st == FALSE)
		        io_printf(stdout,
				  "Cannot open include file '%s' at %s:%d\n",
				  pt, fname, i);}

		 else
		    {sa = SC_tokenize(s, " \t");
		     if (sa == NULL)
		        SC_error(-1, "syntax error on line %d: %s\n", i, s);
		     SC_ptr_arr_len(na, sa);
		     ok = ((strchr(sa[0], '=') != NULL) ||
			   ((na > 1) && (sa[1][0] == '=')));

		     if (ok == TRUE)
		        {_SC_end_rule(&a, state);
			 _SC_end_var(&v, state);
			 _SC_start_var(&v, s, i);}

		     else
		        {pcl = strchr(s, ':');
			 if (pcl != NULL)
			    {_SC_end_rule(&a, state);
			     _SC_end_var(&v, state);
			     _SC_start_rule(&a, s, i);}

			 else if (a.text != NULL)
			    _SC_add_rule(&a, s);

			 else if (v.text != NULL)
			    _SC_add_var(&v, s);

			 else
			    {io_printf(state->log, "Syntax error in line %d\n", i);
			     break;};};

		     SC_free_strings(sa);};};

	    _SC_end_rule(&a, state);
	    _SC_end_var(&v, state);

	    io_close(fp);

	    rv = st;};

	t = _SC_var_lookup(state, "BARRIER");
	if (t != NULL)
	   {if (_SC.barrier != NULL)
	       {CFREE(_SC.barrier);};
	    _SC.barrier = CSTRDUP(t, 3);};};

    SC_ERR_UNTRAP();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PARSE_PREMAKE - parse the file pre-Make FNAME
 *                  - need to know ROOT directory where this
 *                  - executable lives to find make-def and make-macros
 *                  - building up the variable and rule tables
 *                  - return TRUE iff successful
 */

int SC_parse_premake(anadep *state, char *fname)
   {int rv, ok;
    char *s;

    ok = SC_execs(NULL, 0, NULL, -1, "mkdir -p %s/obj >& /dev/null", state->arch);

    if (ok != 0)
       rv = -1;
    else
       rv = TRUE;

    if (rv == TRUE)
       {s  = SC_dsnprintf(TRUE, "%s/etc/make-def", state->root);
	ok = SC_parse_makefile(state, s);
        if (ok != TRUE)
	   rv = -2;
	CFREE(s);};
	
    if (rv == TRUE)
       {s = SC_dsnprintf(TRUE, "PACTTmpDir = %s/obj", state->arch);
	SC_make_def_var(state, s, -1);
	CFREE(s);

	s = SC_dsnprintf(TRUE, "PACTSrcDir = ../..");
	SC_make_def_var(state, s, -1);
	CFREE(s);};

    if (rv == TRUE)
       {ok = SC_parse_makefile(state, fname);
	if (ok != TRUE)
	   rv = -3;};

    if (rv == TRUE)
       {s  = SC_dsnprintf(TRUE, "%s/etc/make-macros", state->root);
	ok = SC_parse_makefile(state, s);
        if (ok != TRUE)
	   rv = -4;
	CFREE(s);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
