/*
 * SCMKDP.C - make style, rule based dependency analysis routines
 *          - provide an API to produce a set of actions
 *          - given a set of rules
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_make.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SHOW_IS_NEWER - print out the "is_newer" relationship
 *                   - for diagnostic/verbose purposes
 */

static void _SC_show_is_newer(int rv, char *fa, time_t ta,
			      char *fb, time_t tb)
   {struct tm ca, cb, safe, *pca, *pcb;
    char sa[MAXLINE], sb[MAXLINE];

    pca = SC_localtime(&ta, &safe);
    ca  = *pca;

    snprintf(sa, MAXLINE, "%4d/%02d/%02d %02d:%02d:%02d",
	     1900+ca.tm_year, ca.tm_mon+1, ca.tm_mday,
	     ca.tm_hour, ca.tm_min, ca.tm_sec);

    pcb = SC_localtime(&tb, &safe);
    cb  = *pcb;

    snprintf(sb, MAXLINE, "%4d/%02d/%02d %02d:%02d:%02d",
	     1900+cb.tm_year, cb.tm_mon+1, cb.tm_mday,
	     cb.tm_hour, cb.tm_min, cb.tm_sec);

    if (rv == TRUE)
       {io_printf(stdout, "   Newer: %s(%s) >=\n", fb, sb);
	io_printf(stdout, "          %s(%s)\n", fa, sa);}
/*
    else
       {io_printf(stdout, "   Older: %s(%s) <\n", fb, sb);
	io_printf(stdout, "          %s(%s)\n", fa, sa);};
*/
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IS_NEWER_FILE - return TRUE if FA is older than FB (time TB) */

static int _SC_is_newer_file(char *fa, char *fb, time_t tb, anadep *state)
   {int rv;
    struct stat ba;
    time_t ta;

    rv = TRUE;
    if (stat(fa, &ba) == 0)
       {ta = ba.st_mtime;
	rv = (tb > ta);

        if (state->verbose == TRUE)
           _SC_show_is_newer(rv, fa, ta, fb, tb);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IS_NEWER_ARCHIVE - return TRUE if FA is older than FB (time TB)
 *                      - FA is of the form AR(BASE.o)
 */

static int _SC_is_newer_archive(char *fa, char *fb, time_t tb, anadep *state)
   {int rv;
    char arf[MAXLINE], s[MAXLINE];
    char *pa;
    time_t ta;
    fcdes *ar;
    fcent *ae;

    if (state->archives == NULL)
       state->archives = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

/* get the name of the archive into arf */
    strcpy(arf, fa);
    pa = strstr(arf, ".a(");
    if (pa != NULL)
       pa[2] = '\0';

/* get the base name of the .o in the archive */
    strcpy(s, pa + 3);
    pa = strchr(s, '.');
    if (pa != NULL)
       {pa[1] = 'o';
	pa[2] = '\0';};
		
    ar = (fcdes *) SC_hasharr_def_lookup(state->archives, arf);
    if (ar == NULL)
       {ar = SC_scan_archive(arf);
	if (ar != NULL)
	   SC_hasharr_install(state->archives, arf, ar, "archive", TRUE, TRUE);};

    rv = TRUE;
    if (ar != NULL)
       {ae = (fcent *) SC_hasharr_def_lookup(ar->entries, s);
	if (ae != NULL)
	   ta = ae->date;
	else
	   ta = 0;

	rv = (tb > ta);

	if (state->verbose == TRUE)
	   _SC_show_is_newer(rv, fa, ta, fb, tb);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IS_NEWER - return TRUE if FB is or will be newer than
 *              - either FA or FA(BASE.o) if BASE is not NULL
 *              - if FORCE is TRUE the answer is TRUE if FB exists
 */

int _SC_is_newer(char *fa, char *fb, int force, anadep *state)
   {int i, ns, rv;
    struct stat bb;
    time_t tb;
    cmdes *as;

    ns = SC_array_get_n(state->actions);
    as = SC_array_array(state->actions);

/* if there is a target matching FB then it will be newer as the
 * commands are executed
 */
    for (i = 0; i < ns; i++)
        {if (strcmp(fb, as[i].target) == 0)
	    {force = TRUE;
	     break;};};

    SFREE(as);

    rv = force;
    if ((force == FALSE) && (stat(fb, &bb) == 0))
       {tb = bb.st_mtime;
        if (strstr(fa, ".a(") == NULL)
	   rv = _SC_is_newer_file(fa, fb, tb, state);
	else
	   rv = _SC_is_newer_archive(fa, fb, tb, state);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             HIGH LEVEL MAKE                              */

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

static char *_SC_subst_macro(char *src, int off,
			     int whch, char *tgt, char *dep, char *sfx)
   {int c, nf, ns, nr;
    char s[MAXLINE];
    char *dst, *base, *p, *t;

    if (src == NULL)
       dst = NULL;

    else
       {p = strchr(src+off, '$');
	if (p == NULL)
	   {dst = SC_strsavef(src, "_SC_SUBST_MACRO:dsta");
	    SFREE(src);}

	else
	   {*p++ = '\0';
	    c    = *p++;
	    base = NULL;
	    switch (c)
	       {case '*':
		     strcpy(s, tgt);
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
		     strcpy(s, tgt);
		     if (whch == ARCHIVE)
		        {t = strtok(s, "(");
			 t = strtok(NULL, ")");
			 if (t != NULL)
			    {base = strchr(t, '.');
			     if (base != NULL)
			        base[1] = sfx[1];
			     base = t;};}
		     else if (whch == EXPLICIT)
		        base = dep;
		     else
		        {base = strchr(s, '.');
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
	    dst = FMAKE_N(char, nf+ns+nr+2, "_SC_SUBST_MACRO:dstb");
	    snprintf(dst, nf+ns+nr+2,
		     "%s%s%s", src, base, p);
	    SFREE(src);

	    dst = _SC_subst_macro(dst, nf+ns, whch, tgt, dep, sfx);};};

    return(dst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REL_ACTIONS - release the given set of actions */

static void _SC_rel_actions(anadep *state, int i)
   {int ns;
    cmdes *pa;

    ns = SC_array_dec_n(state->actions, 1L, -1);
    pa = SC_array_get(state->actions, i);

    SFREE(pa->target);
    SFREE(pa->dependent);
    SFREE(pa->suffix);

    pa->n       = 0;
    pa->kind    = 0;
    pa->actions = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ADD_ACTIONS - remember this set of actions and the info for
 *                 - subsequent macro expansions
 */

static int _SC_add_actions(char *tgt, char *dep, char *sfx,
			   ruledes *rd, int whch, anadep *state)
   {int i, n, ns, ok;
    char s[MAXLINE];
    char **b, *t;
    cmdes a, *as;

    n = rd->n_actions;
    b = rd->actions;

    a.n         = n;
    a.actions   = b;
    a.kind      = whch;
    a.target    = SC_strsavef(tgt, "char*:_SC_ADD_ACTIONS:target");

    if (sfx == NULL)
       a.suffix = SC_strsavef("", "char*:_SC_ADD_ACTIONS:suffixa");
    else
       a.suffix = SC_strsavef(sfx, "char*:_SC_ADD_ACTIONS:suffixb");

    if (dep == NULL)
       {strcpy(s, tgt);
	t = strrchr(s, '(');
	if (t == NULL)
	   t = s;
	else
	   t++;
	t   = strtok(t, ".");
	dep = SC_dsnprintf(TRUE, "%s%s", t, a.suffix);
        a.dependent = dep;}

    else
        a.dependent = SC_strsavef(dep, "char*:_SC_ADD_ACTIONS:dependent");

    ns = SC_array_get_n(state->actions);
    as = SC_array_array(state->actions);

/* check whether we have an explicit rule that should override 
 * an implicit one
 */
    ok = TRUE;
    if (whch == EXPLICIT)
       {for (i = 0; i < ns; i++)
	    {if ((strcmp(tgt, as[i].target) == 0) &&
		 (strcmp(dep, as[i].dependent) == 0) &&
		 (as[i].kind != EXPLICIT))

/* replace the implicit actions only if the explicit actions are not empty */
	        {if (b != NULL)
		    {_SC_rel_actions(state, i);
		     as[i] = a;}

		 ok = FALSE;
		 break;};};};

    SFREE(as);

/* add the rule at the end of the list */
    if (ok)
       SC_array_push(state->actions, &a);

    if (state->verbose)
       _SC_print_rule_info(state, -1, rd);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IMPLICIT_AR - process dependencies relating to implicit rules
 *                 - involving archives (.a)
 *                 - if FORCE is TRUE the actions must be done
 *                 - return a list of commands to execute in CMNDS
 *                 - to satisfy the target (in STATE)
 *                 - return the number of actions sets added
 */

static int _SC_implicit_ar(char *tgt, char *dep, int nc,
			   int force, anadep *state)
   {int i, na, ns, knd, frc;
    char s[MAXLINE], src[MAXLINE];
    char *base, *sfx, *ptrn, **sa;
    ruledes *rd;
    int (*pred)(char *fa, char *fb, int force, anadep *state);

    pred = state->pred;

    strcpy(s, tgt + nc + 3);
    base = strtok(s, ".");
    sfx  = "a";
    knd  = ARCHIVE;

    ns = SC_array_get_n(state->suffices);
    sa = SC_array_array(state->suffices);

/* looping over suffices check each implicit rule for match */
    na = 0;
    for (i = 0; i < ns; i++)
        {ptrn = SC_dsnprintf(FALSE, "%s.%s", sa[i], sfx);
	 rd = (ruledes *) SC_hasharr_def_lookup(state->rules, ptrn);
	 if (rd != NULL)
	    {snprintf(src, MAXLINE, "%s%s", base, sa[i]);
	     if (dep != NULL)
	        {if (SC_isfile(src))
		    frc = (strcmp(dep, src) == 0) || force;
		 else
		    frc = (strcmp(dep, src) == 0) && force;}
	     else
	        frc = force;

	     if ((*pred)(tgt, src, frc, state))
	        {na += _SC_add_actions(tgt, NULL, sa[i], rd, knd, state);
		 break;};};};

    SFREE(sa);

    return(na);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TRY_IMPLICIT_RULE - determine whether an implicit rule RD
 *                       - satisfies the dependency relationship between
 *                       - TGT and DEP
 */

static int _SC_try_implicit_rule(char *tgt, char *dep, char *sfx,
				 ruledes *rd, int force, anadep *state)
   {int na, nt, knd;
    int (*pred)(char *fa, char *fb, int force, anadep *state);

    pred = state->pred;
    nt   = 0;
    knd  = IMPLICIT;

/* see if we hit a rule to update dep
 * GOTCHA: we could hit a loop here
 */
    na = SC_analyze_dependencies(dep, state);
    nt += na;

    if ((*pred)(tgt, dep, force, state))
       {na  = _SC_add_actions(tgt, NULL, sfx, rd, knd, state);
	nt += na;};

    return(nt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IMPLICIT_OBJ - process dependencies relating to implicit rules
 *                  - involving objects (.o)
 *                  - return a list of commands to execute in CMNDS
 *                  - to satisfy the target
 *                  - return the number of actions sets added
 */

static int _SC_implicit_obj(char *tgt, char *dep, int nc,
			    int force, anadep *state)
   {int i, na, ns, nt, found;
    char s[MAXLINE], src[MAXLINE];
    char *base, *sfx, *ptrn, *ps, **sa;
    ruledes *rd;

    nc = max(nc, 0);
    nc = min(nc, MAXLINE-1);

    ns = SC_array_get_n(state->suffices);
    sa = SC_array_array(state->suffices);

/* find the base and suffix of the target */
    strcpy(s, tgt);
    s[nc] = '\0';
    base  = s;
    sfx   = s + nc + 1;

    nt = 0;
    if (sfx[0] != 'a')
       {found = FALSE;

/* check each implicit rule matching dep */
	if (dep != NULL)
	   {for (i = 0; (i < ns) && (found == FALSE); i++)
	        {ptrn = SC_dsnprintf(FALSE, "%s.%s", sa[i], sfx);
		 rd   = (ruledes *) SC_hasharr_def_lookup(state->rules, ptrn);
		 if (rd != NULL)
		    {snprintf(src, MAXLINE, "%s%s", base, sa[i]);
		     if (strcmp(dep, src) == 0)
		        {na    = _SC_try_implicit_rule(tgt, dep, sa[i], rd,
						       FALSE, state);
			 nt   += na;
			 found = (na > 0);};};};};

/* check each implicit rule period */
	for (i = 0; (i < ns) && (found == FALSE); i++)
	    {ptrn = SC_dsnprintf(FALSE, "%s.%s", sa[i], sfx);
	     rd   = (ruledes *) SC_hasharr_def_lookup(state->rules, ptrn);
	     if (rd != NULL)
	        {snprintf(src, MAXLINE, "%s%s", base, sa[i]);
		 ps    = (dep != NULL) ? dep : src;
		 na    = _SC_try_implicit_rule(tgt, ps, sa[i], rd,
					       FALSE, state);
		 nt   += na;
		 found = (na > 0);};};};

    SFREE(sa);

    return(nt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ANALYZE_IMPLICIT - analyze a target TGT for which there is
 *                      - no explicit rule
 *                      - if FORCE is TRUE assume the TGT needs to
 *                      - be updated if there is an implicit rule match
 *                      - TGT may also have a dependency DEP other
 *                      - than one from an implicit rule
 *                      - return the number of actions added
 */

static int _SC_analyze_implicit(char *tgt, char *dep,
				int force, anadep *state)
   {int nc, na;
    char *p, *pa;

    nc = strlen(tgt);
    p  = tgt + nc;
    pa = strstr(tgt, ".a(");

    na = 0;

    if (nc > 2)

/* consider rules of the form '.x.a' involving archives */
       {if (pa != NULL)
	   na += _SC_implicit_ar(tgt, dep, pa-tgt, force, state);

/* consider rules of the form '.x.o' involving objects */
        else if (p[-2] == '.')
	   na += _SC_implicit_obj(tgt, dep, nc-2, force, state);};

    return(na);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIND_RULE - find explicit or implicit rule to make TGT from DEP
 *               - use explicit rule info in RD
 *               - return the number of commands added
 */

static int _SC_find_rule(int *pfnd, char *tgt, char *dep, int force,
			 ruledes *rd, anadep *state)
   {int na;
    char s[MAXLINE], sfx[MAXLINE];
    char *t;
    int (*pred)(char *fa, char *fb, int force, anadep *state);

    pred = state->pred;
    na   = 0;

    if ((*pred)(tgt, dep, force, state))
       {if (rd->actions != NULL)
	   {strcpy(s, dep);
	    strtok(s, ".\n");
	    t = strtok(NULL, "\n");
	    if (t == NULL)
	       snprintf(sfx, MAXLINE, ".");
	    else
	       snprintf(sfx, MAXLINE, ".%s", t);
	    na = _SC_add_actions(tgt, dep, sfx, rd, EXPLICIT, state);};

/* if the explict rule has no actions
 * force any implicit rule to add its actions
 */
	if (na == 0)
	   na = _SC_analyze_implicit(tgt, dep, TRUE, state);

	*pfnd  = (na > 0);};

    return(na);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ADD_SUFFICES - add to the suffix list from .SUFFIXES */

void _SC_add_suffices(anadep *state)
   {int i, n;
    char **dp;
    ruledes *rd;

    if (state->need_suffix == TRUE)
       {rd = (ruledes *) SC_hasharr_def_lookup(state->rules, ".SUFFIXES");
	if (rd != NULL)
	   {n  = rd->n_dependencies;
	    dp = rd->dependencies;
	    for (i = 0; i < n; i++)
	        SC_array_string_add(state->suffices, dp[i]);};

	state->need_suffix = FALSE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ANALYZE_DEPENDENCIES - starting from the target TGT
 *                         - and using the rule table
 *                         - return a list of commands to execute in CMNDS
 *                         - to satisfy the target
 *                         - return the number of command to execute
 */

int SC_analyze_dependencies(char *tgt, anadep *state)
   {int i, n, na, nt, found;
    char **dp;
    ruledes *rd;

    _SC_do_substitutions(state);
    _SC_add_suffices(state);

    nt = 0;
    rd = (ruledes *) SC_hasharr_def_lookup(state->rules, tgt);

/* if there is no explicit rule check for implicit ones */
    if (rd == NULL)
       nt += _SC_analyze_implicit(tgt, NULL, FALSE, state);

/* process the explict rule */
    else
       {n  = rd->n_dependencies;
	dp = rd->dependencies;

/* recurse to find all rules governing the dependencies */
	for (i = 0; i < n; i++)
            nt += SC_analyze_dependencies(dp[i], state);

/* find a rule to update tgt from any dependency */
	found = FALSE;
	for (i = 0; (i < n) && (found == FALSE); i++)
	    nt += _SC_find_rule(&found, tgt, dp[i], FALSE, rd, state);

/* find an implicit rule to update tgt - no dependencies */
	if (found == FALSE)
	   {na    = _SC_analyze_implicit(tgt, NULL, FALSE, state);
	    found = (na > 0);
	    nt   += na;};

/* if there are no dependencies and target doesn't exist
 * or if one of the dependencies invoked an action
 * add the explicit actions for the target
 */
	if ((nt != 0) ||
            ((dp == NULL) && (found == FALSE) && (SC_isfile(tgt) == FALSE)))
	   nt += _SC_add_actions(tgt, NULL, NULL, rd, EXPLICIT, state);};

    return(nt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IS_SAME - return TRUE iff the given cmdes A and B
 *             - imply the same set of commands
 */

static int _SC_is_same(cmdes *a, cmdes *b)
   {int ka, kb, rv;
    char **ca, **cb, *ta, *tb;

    ca = a->actions;
    ta = a->target;
    ka = a->kind;

    cb = b->actions;
    tb = b->target;
    kb = b->kind;

    if (cb != ca)
       rv = FALSE;

    else if (strcmp(ta, tb) == 0)
       rv = TRUE;

    else if ((ka == IMPLICIT) && (kb == IMPLICIT))
       rv = FALSE;

    else if ((ka == ARCHIVE) && (kb == ARCHIVE))
       rv = FALSE;

    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_JOIN_SPLIT_LINES - reassemble split lines
 *                      - 1) scan lines for non-printing characters and
 *                      -    terminate at the first such occurence
 *                      - 2) look for "\" at the end of line
 *                      - make an new array of lines, fill, and return it
 *                      - free the original lines
 */

static char **_SC_join_split_lines(char **cmnds)
   {int i, j, l, n, c;
    int ifrst, join, nc;
    char **ncmd, *s, *t;

/* count the lines */
    SC_ptr_arr_len(n, cmnds);

    ncmd = FMAKE_N(char *, n+1, "_SC_JOIN_SPLIT_LINES:ncmd");
    
    ifrst = -1;
    for (i = 0, j = 0; i < n; i++)
        {join = FALSE;
	 for (s = cmnds[i]; TRUE; s++)
	     {c = *s;
	      if (!SC_is_print_char(c, 0))
	         {join = (c != 0);
		  *s   = '\0';
		  break;};};

	 s  = cmnds[i];
	 nc = strlen(s);
	 if ((s[nc-1] == '\\') && !IS_BARRIER(s))
	    {s[nc-1] = '\0';
	     join = TRUE;};

/* remember the first line index of a sequence of lines that
 * must be joined
 */
	 if ((ifrst == -1) && join)
	    ifrst = i;

/* this line ended properly so see if there are lines to join */
	 else if ((ifrst != -1) && !join)

/* count the number of characters to allocate for the joined line */
	    {nc = 0;
	     for (l = ifrst; l <= i; l++)
	         nc += strlen(cmnds[l]);

	     t = FMAKE_N(char, nc, "_SC_JOIN_SPLIT_LINES:t");

/* cat the lines together in t */
	     t[0] = '\0';
	     for (l = ifrst; l <= i; l++)
	         SC_strcat(t, nc, cmnds[l]);

	     ifrst = -1;

	     ncmd[j++] = t;}

/* a single line was good so copy it over */
         else if (!join)
	   ncmd[j++] = SC_strsavef(s, "_SC_JOIN_SPLIT_LINES:ncmd[j]");};

    ncmd[j++] = NULL;

    SC_free_strings(cmnds);

    return(ncmd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SPLICE_IN_STRINGS - return a new list of strings made by
 *                       - taking strings A[0], ..., A[I-1] followed by
 *                       - B[0], ..., B[Nb] followed by
 *                       - A[I+1], ..., A[Na]
 *                       - both A and B are NULL terminated arrays
 *                       - so Na and Nb are derived by counting non-NULL
 *                       - entries
 *                       - NOTE: A[I] is omitted
 */

char **_SC_splice_in_strings(char **a, int i, char **b)
   {int j, l, na, nb, nc;
    char **c;

/* count the number of strings in each array */
    SC_ptr_arr_len(na, a);
    SC_ptr_arr_len(nb, b);
    
    nc = na + nb;
    c  = FMAKE_N(char *, nc, "_SC_SPLICE_IN_STRINGS:c");
    
    l = 0;

/* add A[0], ..., A[I-1] */
    for (j = 0; j < i; j++)
        c[l++] = SC_strsavef(a[j], "_SC_SPLICE_IN_STRINGS:c1");

/* add B[0], ..., B[Nb] */
    for (j = 0; j < nb; j++)
        c[l++] = SC_strsavef(b[j], "_SC_SPLICE_IN_STRINGS:c2");

/* add A[I+1], ..., A[Na] */
    for (j = i+1; j < na; j++)
        c[l++] = SC_strsavef(a[j], "_SC_SPLICE_IN_STRINGS:c3");

/* add null termination */
    c[l++] = NULL;

/* free the input arrays */
    SC_free_strings(a);
    SC_free_strings(b);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RECURSE_MAKE - expand simple, explicit, recursive invocations
 *                  - of PACT/DMAKE in a list of commands
 *                  - only if they do NOT use any option
 *                  - assume that the original set of dependencies are
 *                  - sufficient (i.e. that the dependency analysis will
 *                  - NOT need to know the command history at the
 *                  - point of replacement
 */

static char **_SC_recurse_make(char **cmnds, anadep *state)
   {int i, n;
    char **ncmd, **ecmd, *s, *p, *t, *tgt;

/* count the lines */
    SC_ptr_arr_len(n, cmnds);

/* find the first instance of an invocation */
    t    = NULL;
    ecmd = NULL;
    for (i = 0; i < n; i++)
        {s = cmnds[i];
	 if ((strncmp(s, "pact ", 5) == 0) ||
	     (strncmp(s, "dmake ", 6) == 0))
	    {t = SC_dstrcat(t, s);
	     SC_strtok(t, " \t", p);

/* find the target if it exists */
	     tgt = SC_strtok(NULL, " \t\n", p);
	     if ((tgt != NULL) &&
		 (tgt[0] != '-') &&
		 (strchr(tgt, '=') == NULL))
	        {SC_analyze_dependencies(tgt, state);
		 ecmd = SC_action_commands(state, TRUE);
		 break;};

	     t[0] = '\0';};};

    SFREE(t);

    ncmd = cmnds;
    if (ecmd != NULL)
       {ncmd = _SC_splice_in_strings(ncmd, i, ecmd);
	ncmd = _SC_recurse_make(ncmd, state);};

    return(ncmd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ACTION_COMMANDS - look through the command actions
 *                    - eliminate duplicates
 *                    - and return an array of commands to be executed
 *                    - this is the time to carry out the substitutions for
 *                    - the $*, $@, $<, and $% macros
 */

char **SC_action_commands(anadep *state, int recur)
   {int i, j, m, na, nc, ns, whch;
    char *tgt, *dep, *sfx;
    char **cmnds, **ca, **cb, *p;
    cmdes *as;

    ns = SC_array_get_n(state->actions);
    as = SC_array_array(state->actions);

/* remove duplicate action sets */
    for (i = 0; i < ns; i++)
        {ca = as[i].actions;
         if (ca == NULL)
	    continue;

	 for (j = i+1; j < ns; j++)
	     {cb = as[j].actions;
	      if (cb == NULL)
		 continue;

	      if (_SC_is_same(&as[i], &as[j]))
		 _SC_rel_actions(state, j);};};

/* count the commands */
    nc = 0;
    for (i = 0; i < ns; i++)
        {na = as[i].n;
	 ca = as[i].actions;
	 if (ca != NULL)
	    nc += na;};

/* build the command set */
    cmnds = FMAKE_N(char *, nc+1, "SC_ACTION_COMMANDS:cmnds");

    if (state->verbose)
       io_printf(stdout, "-------------------------------------------------------\n");

    m = 0;
    for (i = 0; i < ns; i++)
        {na = as[i].n;
	 ca = as[i].actions;
	 if (ca != NULL)
	    {whch = as[i].kind;
	     tgt  = as[i].target;
	     dep  = as[i].dependent;
	     sfx  = as[i].suffix;

             if (state->verbose)
	        _SC_print_rule_info(state, i, NULL);

	     for (j = 0; j < na; j++)

/* save a copy so that the rules can safely be freed later */
	         {p = SC_strsavef(ca[j], "SC_ACTION_COMMANDS:p");
		  
/* carry out any required macro substitutions before adding to A
 * NOTE: _SC_subst_macro will free the input string and return a
 * freshly allocated one
 */
		  if ((tgt != NULL) && (sfx != NULL))
		     p = _SC_subst_macro(p, 0, whch, tgt, dep, sfx);

		  cmnds[m++] = p;};};

	 _SC_rel_actions(state, i);};

    cmnds[m++] = NULL;

    SFREE(as);

/* join split command lines */
    cmnds = _SC_join_split_lines(cmnds);

/* expand simple recursions */
    if (recur == TRUE)
       cmnds = _SC_recurse_make(cmnds, state);

    return(cmnds);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
