/*
 * SCCEM.C - output routines for C parsing
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scc_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_PREPROCESS - preproccess file S using compiler CMP and flags V */

int CC_preprocess(char *s, char *cmp, char **v)
   {int i, nc, st, rv;
    char cmd[MAXLINE], inm[MAXLINE];
    char *p;
    file_loc *fl;

    p = strrchr(s, '/');
    if (p != NULL)
       p++;
    else
       p = s;

    nc = strlen(p);
    fl = &_CC.rloc;

    fl->fname       = CSTRSAVE(p);
    fl->fname[nc-2] = '\0';

    snprintf(inm, MAXLINE, "%s.i", fl->fname);

    SC_remove(inm);

/* prepare CPP command and execute it */
    snprintf(cmd, MAXLINE, "%s", cmp);
    for (i = 0; TRUE; i++)
        {if (v[i] == NULL)
            break;
	 else if (strcmp(v[i], "-c") == 0)
	    continue;
	 else
	    SC_vstrcat(cmd, MAXLINE, " %s", v[i]);};

    SC_vstrcat(cmd, MAXLINE, " -E %s > %s 2>&1", s, inm);

    st = system(cmd);

    rv = ((st & 0xFF) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_COMPILE - compile file S using compiler CMP and flags V */

int CC_compile(char *s, char *cmp, char **v)
   {int i, st, rv;
    char cmd[MAXLINE];

/* prepare compile command and execute it */
    snprintf(cmd, MAXLINE, "%s", cmp);
    for (i = 0; TRUE; i++)
        {if (v[i] == NULL)
            break;
         else if (strncmp(v[i], "-I", 2) == 0)
            continue;
	 else
	    SC_vstrcat(cmd, MAXLINE, " %s", v[i]);};

    SC_vstrcat(cmd, MAXLINE, " %s", s);

    st = system(cmd);

    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_EMIT_ERROR - print error message from emit phase */

#if 0

static void _CC_emit_error(char *fmt, ...)
   {char s[MAXLINE];

    SC_VA_START(fmt);
    SC_VSNPRINTF(s, MAXLINE, fmt);
    SC_VA_END;

    fprintf(stdout, "ERR> %s\n", s);

    _CC.n_error++;

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_EMIT_TYPE_DECL - emit derived type declaration PD */

static void _CC_emit_type_decl(FILE *fp, decl *pd)
   {

    if (pd->stor == CC_TYPEDEF)
       fprintf(fp, "%s\n", pd->out);

    else
       {switch (pd->cat)
	   {case CC_STRUCT :
	    case CC_UNION :
	    case CC_ENUM :
	         fprintf(fp, "%s\n", pd->out);
	         break;
	    default :
	         break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_EMIT_VAR_DECL - emit variable definition from PD */

static void _CC_emit_var_decl(FILE *fp, decl *pd)
   {char *t, *p;

    t = pd->out;
    p = strchr(t, '=');
    if (p != NULL)
       {*p++ = ';';
        *p++ = '\0';};

    if (strncmp(t, "extern", 6) == 0)
       fprintf(fp, "%s\n", t);
    else
       fprintf(fp, "extern %s\n", t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_EMIT_FNC_DECL - emit function specification from PD */

static void _CC_emit_fnc_decl(FILE *fp, decl *pd)
   {char *t, *p;

    t = CSTRSAVE(pd->out);
    p = strchr(t, ')');
    if (p != NULL)
       {p++;
        *p++ = ';';
	*p++ = '\0';};
	       
    if (strncmp(t, "extern", 6) == 0)
       fprintf(fp, "%s\n", t);
    else if (strncmp(t, "static", 6) == 0)
       fprintf(fp, "%s\n", t+7);
    else
       fprintf(fp, "extern %s\n", t);
		    
    CFREE(t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_EMIT_VARDEF - emit variable definitions */

static int _CC_emit_vardef(char *cmp, char **v, int cfl)
   {int i, n, rv;
    char nm[MAXLINE];
    decl *pd;
    FILE *fp;

    rv = TRUE;

    snprintf(nm, MAXLINE, "%s-0.i", _CC.rloc.fname);

    fp = SC_fopen_safe(nm, "w");
    if (fp != NULL)
       {n = _CC.nd;
	for (i = 0; i < n; i++)
	    {pd = _CC.all[i];
	     if (((pd->kind == CC_VAR) && (pd->stor != CC_EXTERN)) ||
		 (pd->kind == CC_TYP))
	        fprintf(fp, "%s\n", pd->out);};

	fprintf(fp, "\n");

	SC_fclose_safe(fp);

	if (cfl == TRUE)
	   rv &= CC_compile(nm, cmp, v);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_EMIT_FNC - emit code for Ith decl which is the Jth function */

static int _CC_emit_fnc(int i, int j, char *cmp, char **v, int cfl)
   {int nl, rv;
    int *dl;
    char nm[MAXLINE];
    char *t;
    decl *pd, *dc;
    FILE *fp;

    rv = TRUE;

    snprintf(nm, MAXLINE, "%s-%d.i", _CC.rloc.fname, j);

    fp = SC_fopen_safe(nm, "w");
    if (fp != NULL)
       {dc = _CC.all[i];
	dl = dc->dep;
	for (nl = 0; dl[nl] != -1; nl++)
	    {pd = _CC.all[dl[nl]];
	     if (pd->kind == CC_TYP)
	        _CC_emit_type_decl(fp, pd);
	     else if (pd->kind == CC_VAR)
	        _CC_emit_var_decl(fp, pd);
	     else if (pd->kind == CC_FNC)
	        _CC_emit_fnc_decl(fp, pd);};

	t = dc->out;

	fprintf(fp, "# %d \"%s\"\n", dc->vlb, _CC.vloc.fname);

	fprintf(fp, "%s\n", t);

	fprintf(fp, "\n");

	SC_fclose_safe(fp);

	if (cfl == TRUE)
	   rv &= CC_compile(nm, cmp, v);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_EMIT - emit code files for CC */

int CC_emit(char *cmp, char **v, int cfl)
   {int i, j, n, rv;
    decl *pd;

    n = _CC.nd;

    rv = _CC_emit_vardef(cmp, v, cfl);

    for (i = 0, j = 0; i < n; i++)
        {pd = _CC.all[i];
	 if ((pd->kind == CC_FNC) && (pd->stor != CC_EXTERN))
	    rv &= _CC_emit_fnc(i, ++j, cmp, v, cfl);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
