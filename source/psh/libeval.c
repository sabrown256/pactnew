/*
 * LIBEVAL.C - eval shell expresions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBEVAL

# define LIBEVAL

# ifndef SCOPE_SCORE_PREPROC

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPAND_REGX - expand wild cards in S
 *             - return the result in D
 */

char *expand_regx(char *d, int nd, char *s)
   {int id, is, ne, ns, st, qu;
    char **da, **sa;
    wordexp_t w;

    d[0] = '\0';

    sa = tokenize(s, " \t\n", 0);
    ns = lst_length(sa);
    for (is = 0; is < ns; is++)
        {if (sa[is][0] == '\'')
	    vstrcat(d, nd, "%s ", sa[is]);
	 else
	    {qu = (sa[is][0] == '"');
	     st = wordexp(sa[is], &w, 0);
	     if (st == 0)
	        {da = w.we_wordv;
		 ne = w.we_wordc;

		 if (qu == TRUE)
		    vstrcat(d, nd, "\"");

		 for (id = 0; id < ne; id++)
		     vstrcat(d, nd, "%s ", da[id]);

		 if (qu == TRUE)
		    vstrcat(d, nd, "\"");

		 wordfree(&w);}
	     else
	        vstrcat(d, nd, "%s ", sa[is]);};};

    free_strings(sa);

    LAST_CHAR(d) = '\0';

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPAND_ENV - expand all environment variables in EXPR */

char *expand_env(char *expr, int nc, char *varn, int rnull)
   {int i, n;
    char var[BFLRG], t[BFLRG];
    char *rv, *p, *val;
    static char s[BFLRG];

    nstrncpy(s, BFLRG, expr, nc);

    for (i = 0; TRUE; i++)
        {p = strchr(s, '$');
	 if (p == NULL)
	    break;
	 else
	    {n = strspn(p, "{abcdefghijklmnopqrstuvwxyz"
		 	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			   "0123456789_$?@}");
	     nstrncpy(var, BFLRG, p, n);

	     if (var[1] == '$')
	        {snprintf(t, BFLRG, "%d", (int) getpid());
		 val = t;}
	     else if (var[1] == '{')
	        {nstrncpy(t, BFLRG, var, -1);
		 LAST_CHAR(t) = '\0';
		 val = cgetenv(TRUE, t+2);}
	     else if (var[1] == '?')
		val = cdefenv(var+2) ? "1" : "0";
	     else if ((var[1] == '@') && (varn != NULL))
		val = cgetenv(TRUE, varn);
	     else if (cdefenv(var+1) == TRUE)
	        val = cgetenv(TRUE, var+1);

/* if we encounter an undefined variable - stop and return NULL */
	     else
	        {if (rnull == TRUE)
		    return(NULL);
		 else
		    {snprintf(s, BFLRG, "'%s'", trim(expr, BOTH, "\""));
		     break;};};

	     nstrncpy(s, nc, subst(s, var, val, -1), -1);};};

    rv = s;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVAL - evaluate a shell expression */

char *eval(char *expr, int nc, char *varn)
   {char *rv, *s;
    static char res[20];

    rv = expand_env(expr, nc, varn, TRUE);

    if (rv != NULL)
       s = strpbrk(rv, "`*+");
    else
       s = NULL;

    if ((rv != NULL) && (s != NULL))

/* purely numeric expression, e.g. 2 * 3 */
       {if (strspn(rv, "0123456789*+-/ \t") == strlen(rv))
	   rv = run(FALSE, "csh -f -c '(set noglob ; @ foo = %s ; echo $foo)'",
		    rv);

        else if ((strstr(rv, "=~") != NULL) || (strstr(rv, "!~") != NULL))
	   {rv = run(FALSE, "csh -f -c '(set noglob ; if %s echo yes)'", rv);
	    if (strcmp(rv, "yes") == 0)
	       strcpy(res, "1 -eq 1");
	    else
	       strcpy(res, "1 -eq 0");

	    rv = res;}

/* all other expression types */
	else
	   rv = run(FALSE, "csh -f -c '(set noglob ; echo %s)'", rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRUNE_ENV - clear out part of the environment specfied by TGT */

void prune_env(char *tgt, char *info)
   {

    if (IS_NULL(tgt) == FALSE)

/* remove make rule variables */
       {int i, nr;
	static char *rnames[] = { "IRules_CCP",
				  "IRules_CCObj", "IRules_CCArc",
				  "IRules_LexObj", "IRules_LexArc", "IRules_LexC",
				  "IRules_YaccObj", "IRules_YaccArc", "IRules_YaccC",
				  "IRules_FCObj", "IRules_FCArc", "IRules_TemplH",
				  "IRules_CCObj_AC", "IRules_CCArc_AC",
				  "IRules_LexObj_AC", "IRules_LexArc_AC",
				  "IRules_YaccObj_AC", "IRules_YaccArc_AC" };

	if (strcmp(tgt, "rules") == 0)
	   {nr = sizeof(rnames)/sizeof(char *);
	    for (i = 0; i < nr; i++)
		cunsetenv(rnames[i]);}

/* remove PACT config variables */
        else if (strcmp(tgt, "pact") == 0)
	   {char s[BFLRG], env[BFLRG];
	    char **sa, *p;

	    nstrncpy(s, BFLRG, path_head(cwhich(info)), -1);
	    snprintf(env, BFLRG, "%s/etc/env-pact.csh", path_head(s));
	    if (file_exists(env) == TRUE)
	       {sa = file_text(FALSE, env);

		if (sa != NULL)
		   {for (i = 0; sa[i] != NULL; i++)
		        {if (strncmp(sa[i], "setenv ", 7) == 0)
			    {nstrncpy(s, BFLRG, sa[i], -1);
			     p = strchr(s+7, ' ');
			     if (p != NULL)
			        *p = '\0';
			     p = s + 7;
                             if (strcmp(p, "PATH") != 0)
			        {cunsetenv(p);};};};};

		free_strings(sa);};}

/* remove variables matching the pattern */
	else
	   {};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

# endif
#endif
