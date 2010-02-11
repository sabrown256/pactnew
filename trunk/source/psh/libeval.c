/*
 * LIBEVAL.C - eval shell expresions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPAND - expand all environment variables in EXPR */

char *expand(char *expr, int nc, char *varn)
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
	        {snprintf(t, MAXLINE, "%d", getpid());
		 val = t;}
	     else if (var[1] == '{')
	        {LAST_CHAR(var) = '\0';
		 val = cgetenv(TRUE, var+2);}
	     else if (var[1] == '?')
		val = cdefenv(var+2) ? "1" : "0";
	     else if ((var[1] == '@') && (varn != NULL))
		val = cgetenv(TRUE, varn);
	     else if (cdefenv(var+1) == TRUE)
	        val = cgetenv(TRUE, var+1);

/* if we encounter an undefined variable - stop and return NULL */
	     else
		return(NULL);

	     nstrncpy(s, nc, subst(s, var, val, -1), -1);};};

    rv = s;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVAL - evaluate a shell expression */

char *eval(char *expr, int nc, char *varn)
   {char *rv, *s;
    static char res[20];

    rv = expand(expr, nc, varn);

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
