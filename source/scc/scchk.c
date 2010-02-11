/*
 * SCCHK.C - check declarations for correctness
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scc_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_CHECK_ERROR - print error message from check phase */

static void _CC_check_error(char *fmt, ...)
   {char s[MAXLINE];

    SC_VA_START(fmt);
    SC_VSNPRINTF(s, MAXLINE, fmt);
    SC_VA_END;

    fprintf(stdout, "ERR> %s\n", s);

    _CC.n_error++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_CHECK_DECL - check declarations attributes */

static int _CC_check_decl(decl *pd)
   {int rv;
    char *s;

    rv = TRUE;
    s  = pd->out;
    if (s == NULL)
       return(rv);

/* check qualifiers */
    switch (pd->qual)
       {case CC_CONST :
             rv &= (strstr(s, "const") != NULL);
             break;
        case CC_VOLATILE :
             rv &= (strstr(s, "volatile") != NULL);
             break;
        default :
             break;};

    if (rv == FALSE)
       _CC_check_error("Inconsistent type qualifilers\n%s", s);

/* check struct, union, enum, or var */
    switch (pd->cat)
       {case CC_ENUM :
             rv &= (strstr(s, "enum") != NULL);
             break;
        case CC_STRUCT :
             rv &= (strstr(s, "struct") != NULL);
             break;
        case CC_UNION :
             rv &= (strstr(s, "union") != NULL);
             break;
        default :
             break;};

    if (rv == FALSE)
       _CC_check_error("Inconsistent struct, union, or enum\n%s", s);

/* check storage class */
    switch (pd->stor)
       {case CC_AUTO :
             rv &= (strstr(s, "auto") != NULL);
             break;
        case CC_EXTERN :
             rv &= (strstr(s, "extern") != NULL);
             break;
        case CC_REGISTER :
             rv &= (strstr(s, "register") != NULL);
             break;
        case CC_STATIC :
/*             rv &= (strstr(s, "static") != NULL); */
             break;
        case CC_TYPEDEF :
             rv &= (strstr(s, "typedef") != NULL);
             break;
        default :
             break;};

    if (rv == FALSE)
       _CC_check_error("Inconsistent storage class\n%s", s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_CHECK_TYP - check type declaration PD */

static int _CC_check_typ(decl *pd)
   {int rv;

    rv = _CC_check_decl(pd);

    if (pd->stor == CC_TYPEDEF)
       fprintf(stdout, "typedef %s", pd->name);

    else
       {if (pd->cat == CC_ENUM)
	   fprintf(stdout, "%s", pd->type);
        else if (pd->cat == CC_STRUCT)
	   fprintf(stdout, "%s", pd->type);
	else if (pd->cat == CC_UNION)
	   fprintf(stdout, "%s", pd->type);};

    fprintf(stdout, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_CHECK_VAR - check variable declaration PD */

static int _CC_check_var(decl *pd)
   {int rv;
    char s[MAXLINE];

    rv = _CC_check_decl(pd);
    fprintf(stdout, "variable %s:", pd->name);

    s[0] = '\0';

/* check storage class */
    switch (pd->stor)
       {case CC_AUTO :
	     strncat(s, " ",    MAXLINE);
	     strncat(s, "auto", MAXLINE);
	     break;
        case CC_EXTERN :
	     strncat(s, " ",      MAXLINE);
	     strncat(s, "extern", MAXLINE);
	     break;
	case CC_REGISTER :
	     strncat(s, " ",        MAXLINE);
	     strncat(s, "register", MAXLINE);
	     break;
	case CC_STATIC :
	     strncat(s, " ",      MAXLINE);
	     strncat(s, "static", MAXLINE);
	     break;
        case CC_TYPEDEF :
	     strncat(s, " ",       MAXLINE);
	     strncat(s, "typedef", MAXLINE);
	     break;
        default :
	     break;};

/* check qualifiers */
    switch (pd->qual)
       {case CC_CONST :
	     strncat(s, " ",     MAXLINE);
	     strncat(s, "const", MAXLINE);
	     break;
        case CC_VOLATILE :
	     strncat(s, " ",        MAXLINE);
	     strncat(s, "volatile", MAXLINE);
	     break;
        default :
	     break;};

    fprintf(stdout, "%s", s);

    if (pd->type != NULL)
       fprintf(stdout, " %s", pd->type);

    if (pd->cat == CC_DERIVED)
       fprintf(stdout, " (derived)");

    fprintf(stdout, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_VAR_EXIST - check that the variable has a declaration */

static int _CC_var_exist(haelem *hp, void *a)
   {int i, n, rv, ok;
    char *nm, *name;
    decl *pd, *vd;
    expr *v;

    n  = *(int *) a;
    ok = SC_haelem_data(hp, &name, NULL, (void **) &v);
    pd = _CC.all[n];

    ok = FALSE;
    for (i = 0; (i <= n) && (ok == FALSE); i++)
        {vd = _CC.all[i];
	 nm = vd->name;
	 ok = ((nm != NULL) && (strcmp(nm, name) == 0));};

    if ((ok == FALSE) && (v->declared == CC_UNDEF))
       _CC_check_error("'%s' undefined in function %s",
		       name, nm);

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_CHECK_FNC - check function declaration PD */

static int _CC_check_fnc(decl *pd, int n)
   {int i, nl, rv;
    int *dl;

    rv = _CC_check_decl(pd);

/* check that all dependencies are defined or declared */
    SC_hasharr_foreach(pd->refobjs, _CC_var_exist, &n);

/* list the declarations needed for this function */
    pd = _CC.all[n];
    fprintf(stdout, "Function %s requires:\n", pd->name);

    dl = pd->dep;
    for (nl = 0; dl[nl] != -1; nl++);

    for (i = 0; i < nl; i++)
        {pd = _CC.all[dl[i]];
	 if (pd->kind == CC_VAR)
	    fprintf(stdout, "   variable: %s %s\n",
		    pd->type, pd->name);
	 else if (pd->kind == CC_FNC)
	    fprintf(stdout, "   function: %s %s\n",
		    pd->type, pd->name);
	 else if (pd->kind == CC_TYP)
            {if (pd->stor == CC_TYPEDEF)
		fprintf(stdout, "   typedef: %s\n", pd->name);
	     else
		fprintf(stdout, "   derived type: %s\n", pd->type);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_CHECK - check code files for CC */

int CC_check(char *cmp, char **v)
   {int i, n, rv;
    decl *pd;

    n = _CC.nd;

    rv = TRUE;

    for (i = 0; i < n; i++)
        {pd = _CC.all[i];
	 if (pd->kind == CC_TYP)
	    rv &= _CC_check_typ(pd);
	 else if (pd->kind == CC_VAR)
	    rv &= _CC_check_var(pd);
	 else if (pd->kind == CC_FNC)
	    rv &= _CC_check_fnc(pd, i);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
