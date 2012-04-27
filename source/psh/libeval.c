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

#define LIBEVAL

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPAND - expand all environment variables in EXPR */

char *expand(char *expr, int nc, char *varn, int rnull)
   {int i, n;
    char var[MAXLINE], t[MAXLINE];
    char *rv, *p, *val;
    static char s[LRG];

    nstrncpy(s, LRG, expr, nc);

    for (i = 0; TRUE; i++)
        {p = strchr(s, '$');
	 if (p == NULL)
	    break;
	 else
	    {n = strspn(p, "{abcdefghijklmnopqrstuvwxyz"
		 	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			   "0123456789_$?@}");
	     nstrncpy(var, MAXLINE, p, n);

	     if (var[1] == '$')
	        {snprintf(t, MAXLINE, "%d", (int) getpid());
		 val = t;}
	     else if (var[1] == '{')
	        {nstrncpy(t, MAXLINE, var, -1);
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
		    {snprintf(s, LRG, "'%s'", trim(expr, FRONT | BACK, "\""));
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

    rv = expand(expr, nc, varn, TRUE);

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
	static char *rnames[] = { "CCP", "CCObj", "CCArc",
				  "LexObj", "LexArc", "LexC",
				  "YaccObj", "YaccArc", "YaccC",
				  "FCObj", "FCArc", "TemplH",
				  "CCObj_BP", "CCArc_BP",
				  "LexObj_BP", "LexArc_BP",
				  "YaccObj_BP", "YaccArc_BP" };

	if (strcmp(tgt, "rules") == 0)
	   {nr = sizeof(rnames)/sizeof(char *);
	    for (i = 0; i < nr; i++)
	        cunsetenv(rnames[i]);}

/* remove PACT config variables */
        else if (strcmp(tgt, "pact") == 0)
	   {int i;
	    char s[MAXLINE], env[MAXLINE];
	    char **sa, *p;

	    nstrncpy(s, MAXLINE, path_head(cwhich(info)), -1);
	    snprintf(env, MAXLINE, "%s/etc/env-pact.csh", path_head(s));
	    if (file_exists(env) == TRUE)
	       {sa = file_text(FALSE, env);

		if (sa != NULL)
		   {for (i = 0; sa[i] != NULL; i++)
		        {if (strncmp(sa[i], "setenv ", 7) == 0)
			    {nstrncpy(s, MAXLINE, sa[i], -1);
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

#endif
