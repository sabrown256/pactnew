/*
 * PCO.C - processor for auto-configuration information
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libeval.c"
#include "libtime.c"
#include "libdb.c"
#include "libpgrp.c"

#define STACK_FILE      1
#define STACK_PROCESS   2
#define STACK_TOOL      3
#define STACK_GROUP     4

#define LOG_ON      initlog("a", st.logf)
#define LOG_OFF     finlog()

enum e_phase_id
   { PHASE_READ = 10, PHASE_ANALYZE, PHASE_WRITE };

typedef enum e_phase_id phase_id;

enum e_exoper
   { PNONE, PEQ, PLT, PLE, PGT, PGE, PNE };

typedef enum e_exoper exoper;

typedef struct s_file_stack file_stack;
typedef struct s_file_entry file_entry;
typedef struct s_gt_stack gt_stack;
typedef struct s_gt_entry gt_entry;
typedef struct s_state state;
typedef struct s_dirdes dirdes;
typedef struct s_ruledes ruledes;

struct s_file_entry
   {int itype;
    int iline;
    char *name;
    FILE *fp;};

struct s_file_stack
   {int n;
    file_entry file[N_STACK];};

struct s_gt_entry
   {int itype;
    char *opt;
    char *item;};

struct s_gt_stack
   {int n;
    char curr_grp[BFSML];
    char curr_tool[BFSML];
    gt_entry st[N_STACK];};

struct s_dirdes
   {char root[BFLRG];
    char bin[BFLRG];
    char cfg[BFLRG];
    char inc[BFLRG];
    char etc[BFLRG];
    char lib[BFLRG];
    char mng[BFLRG];
    char scr[BFLRG];};

struct s_ruledes
   {char ccp[BFLRG];            /* .c -> .i */
    char co[BFLRG];             /* .c -> .o */
    char ca[BFLRG];             /* .c -> .a */
    char fo[BFLRG];             /* .f -> .o */
    char fa[BFLRG];             /* .f -> .a */
    char lo[BFLRG];             /* .l -> .o */
    char la[BFLRG];             /* .l -> .a */
    char lc[BFLRG];             /* .l -> .c */
    char yo[BFLRG];             /* .y -> .o */
    char ya[BFLRG];             /* .y -> .a */
    char yc[BFLRG];             /* .y -> .c */
    char th[BFLRG];             /* .t -> .h */
    char co_bp[BFLRG];          /* bad pragma versions */
    char ca_bp[BFLRG];
    char lo_bp[BFLRG];
    char la_bp[BFLRG];
    char yo_bp[BFLRG];
    char ya_bp[BFLRG];};

struct s_state
   {int abs_deb;
    int abs_opt;
    int create_dirs;
    int have_db;
    int async;

    int installp;
    int loadp;
    int analyzep;
    int exep;
    int profilep;

    int tmp_dirp;
    int verbose;
    int launched;               /* TRUE iff PCO launched PERDB
                                 * FALSE if PERDB was already running */
    phase_id phase;

    int np;                     /* number of platforms */
    char **pltfcmd;             /* platform config commands */
    char **pltfcfg;             /* platform config file */

    int na;
    char **args;
    char **env;
    char *db;

    file_stack fstck;
    gt_stack gstck;
    dirdes dir;
    ruledes rules;

    char cfgf[BFLRG];          /* config file name */
    char logf[BFLRG];          /* log file name */

    char def_tools[BFLRG];     /* default tools */
    char def_groups[BFLRG];    /* defaults groups */
    char toolv[BFLRG];         /* tool variables */

    char env_csh[BFLRG];
    char env_sh[BFLRG];
    char env_dk[BFLRG];
    char env_mdl[BFLRG];

    char arch[BFLRG];
    char host[BFLRG];
    char os[BFLRG];
    char osrel[BFLRG];
    char hw[BFLRG];
    char psy_cfg[BFLRG];
    char psy_id[BFLRG];
    char features[BFSML];};

static state
 st = { FALSE, FALSE, FALSE, FALSE, FALSE,
        TRUE, FALSE, TRUE, FALSE, FALSE,
	FALSE, FALSE, FALSE,
        PHASE_READ, 0, NULL, NULL,
        0, NULL, NULL, NULL, };

static void
 parse_line(client *cl, char *s, char *key, char *oper, char *value, int nc);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ECHO - run echo command
 *      - only exec shell if necessary
 */

static char *echo(int log, char *fmt, ...)
   {char s[BFLRG];
    char *rv;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (strpbrk(s, "$*|{}[]\"'") == NULL)
       rv = STRSAVE(trim(s, BOTH, " \t\n"));
    else
       rv = STRSAVE(run(BOTH, "echo %s", s));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_FILE - push S onto the file stack */

static char *push_file(char *s, int itype)
   {int id, n, nd, ok;
    file_entry *se;
    static char lfile[BFLRG], t[BFLRG];
    static char *places[] = { NULL, "local", "std", "features",
			      "compilers", "analyze", "." };

    n  = st.fstck.n++;
    se = st.fstck.file + n;

    ok = TRUE;
    if (itype == STACK_FILE)
       {nd = sizeof(places)/sizeof(char *);
	ok = FALSE;
	for (id = 0; (id < nd) && (ok == FALSE); id++)
	    {if (places[id] == NULL)
	        {nstrncpy(lfile, BFLRG, s, -1);
		 ok = file_exists(lfile);}
	     else
	        {snprintf(lfile, BFLRG, "%s/%s", places[id], s);
		 ok = file_exists("%s/%s", st.dir.mng, lfile);};};

	if (ok == FALSE)
	   {noted(NULL, "***> Cannot find file '%s'\n\n", s);
	    ok = FALSE;};}

    else if (itype == STACK_PROCESS)
       {if (strchr(s, '$') != NULL)
	   {char *p;

	    p = echo(BOTH, "\"%s\"", s);
	    if (p != NULL)
	       {nstrncpy(lfile, BFLRG, p, -1);
		FREE(p);};}
	else
	   nstrncpy(lfile, BFLRG, s, -1);};

    if ((ok == TRUE) && (se != NULL))
       {se->itype = itype;
	se->iline = 0;
	se->name  = STRSAVE(lfile);

	if (itype == STACK_FILE)
	   {if (lfile[0] != '/')
               snprintf(t, BFLRG, "%s/%s", st.dir.mng, lfile);
	    else
	       nstrncpy(t, BFLRG, lfile, -1);
	    se->fp = fopen_safe(t, "r");}

	else if (itype == STACK_PROCESS)
	   se->fp = popen(lfile, "r");};

    return(lfile);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_FILE - pop the file stack */

static void pop_file(void)
   {int n;
    file_entry *se;

    st.fstck.n--;

    n  = st.fstck.n;
    se = st.fstck.file + n;

    if (se->itype == STACK_FILE)
       fclose_safe(se->fp);
    else if (se->itype == STACK_PROCESS)
       pclose(se->fp);

    se->iline = 0;
    se->fp    = NULL;
    FREE(se->name);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_STRUCT - push S onto the struct stack */

static void push_struct(char *item, char *collection, int itype)
   {int n;
    char t[BFLRG], lst[BFLRG];
    char *p, *ps;
    gt_entry *ge;

    strcpy(t, item);
    strcpy(lst, collection);

    ps = lst;
    while (TRUE)
       {p = strtok(ps, " \t");
	if (p == NULL)
	   break;
	else if (strcmp(p, item) != 0)
	   {nstrcat(t, BFLRG, " ");
	    nstrcat(t, BFLRG, p);};
	ps = NULL;};

    strcpy(collection, t);

    n  = st.gstck.n++;
    ge = st.gstck.st + n;
    ge->item  = STRSAVE(item);
    ge->itype = itype;
    ge->opt   = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_STRUCT - pop the struct stack */

static void pop_struct(client *cl)
   {int n;
    gt_entry *ge;

/* release the current stack entry */
    n  = --st.gstck.n;
    ge = st.gstck.st + n;

    FREE(ge->item);

/* reset the current group or tool */
    n  = st.gstck.n - 1;
    ge = st.gstck.st + n;

    nstrncpy(st.gstck.curr_grp,  BFSML, ge->item, -1);
    st.gstck.curr_tool[0] = '\0';

    note(NULL, "--- end\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RESET_ENV - check for and act on specifications to modify the environment
 *           - clear out all environment variables except for PATH
 *           - and those specified in the optional file
 */

static int reset_env(int c, char **v)
   {int i, rv;
    char *p, **sa, **except;

/* get the list of variables that are to be left intact from the -env file */
    sa = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-env") == 0)
	    {sa = file_text(FALSE, v[++i]);
	     break;};};

    rv = 0;
    if (sa != NULL)

/* make a clean list of variables - throwing away comments from the file */
       {except = NULL;

	for (i = 0; sa[i] != NULL; i++)
	    {p = sa[i];
	     if ((IS_NULL(p) == FALSE) && (p[0] != '#'))
	        except = lst_push(except, trim(p, BOTH, " \t\n"));};
	free_strings(sa);

	if (except != NULL)
	   {except = lst_push(except, NULL);

	    rv = cclearenv(except);

	    free_strings(except);};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                                  EMITTERS                                */

/*--------------------------------------------------------------------------*/

/* WRITE_CLASS_PCO - write a PCO form text representation of
 *                 - a class CLSS of type CTYPE
 *                 - look for sub-class SUB of type STYPE
 */

static int write_class_pco(client *cl, FILE *out, char *clss, char *ctype,
			   char *sub, char *stype, char *ind)
   {int i, n, ic, nc, global;
    char cln[BFLRG], fmt[BFLRG];
    char *c, *pc, *t, *var, *val, *entry;
    char **vars, **vals, **sa;

    nstrncpy(cln, BFLRG, clss, -1);
    for (c = cln; c != NULL; c = pc)
        {pc = strchr(c, ' ');
         if (pc == NULL)
            {if (IS_NULL(c) == TRUE)
                break;}
         else
            *pc++ = '\0';

	 global = (strcmp(c, "Glb") == 0);

	 if (global == TRUE)
	    {t  = dbget(cl, FALSE, "PCO_Globals");
	     sa = tokenize(t, " \t\n\r", 0);}
         else
	    {fprintf(out, "%s%s %s {\n", ind, ctype, c);
	     t  = run(BOTH, "env | egrep '^%s_' | sort", c);
	     sa = tokenize(t, "\n\r", 0);};

	 if (sa != NULL)
	    {for (n = 0; sa[n] != NULL; n++);

	     if ((global == TRUE) && (n > 0))
	        fprintf(out, "# Global variables\n");

	     vars = MAKE_N(char *, n+1);
	     vals = MAKE_N(char *, n+1);
	     if ((vars != NULL) && (vals != NULL))
	        {nc = 0;
		 for (i = 0; i < n; i++)
		     {entry = sa[i];
		      if (IS_NULL(entry) == TRUE)
			 continue;
		      if (global == TRUE)
			 {var = entry;
			  val = dbget(cl, TRUE, var);}
		      else
			 {var = entry + strlen(c) + 1;
			  val = strchr(var, '=');
			  if (val != NULL)
			     *val++ = '\0';};

		      ic = strlen(var);
		      nc = max(nc, ic);

		      vars[i] = var;
		      vals[i] = STRSAVE(val);};

		 vars[n] = NULL;
		 vals[n] = NULL;

		 if (global == TRUE)
		    snprintf(fmt, BFLRG, "%%s%%-%ds = %%s\n", nc);
		 else
		    snprintf(fmt, BFLRG, "%%s   %%-%ds = %%s\n", nc);

		 for (i = 0; i < n; i++)
		     {if ((vars[i] != NULL) && (vals[i] != NULL))
			 fprintf(out, fmt, ind, vars[i], vals[i]);};

		 if (global == TRUE)
		    fprintf(out, "\n");
		 else
		    fprintf(out, "}\n\n");};

	     FREE(vars);
	     free_strings(vals);
	     free_strings(sa);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_PCO - write the input form of the database
 *           - to be read back in by pco
 */

static void write_pco(client *cl, state *st, char *dbname)
   {int rv;
    char t[BFLRG];
    FILE *out;

    if (dbname != NULL)
       snprintf(t, BFLRG, "%s.%s.pco", cgetenv(FALSE, "PERDB_PATH"), dbname);
    else
       snprintf(t, BFLRG, "%s.pco", cgetenv(FALSE, "PERDB_PATH"));

    out = open_file("w", t);

    rv = write_class_pco(cl, out, st->def_tools,
			 "Tool", NULL, NULL, "");
    ASSERT(rv == 0);

    rv = write_class_pco(cl, out,
			 st->def_groups, "Group",
			 st->def_tools, "Tool", "");
    ASSERT(rv == 0);

    fclose_safe(out);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SUBLIST - return a list of string from TA 'matching' C
 *         - remove matches from TA
 */

char **sublist(char **ta, int na, char *s, char **exc, int ne)
   {int i, ie, nw, ok;
    char w[BFLRG];
    char *t, **sa;

    snprintf(w, BFLRG, "%s_", s);
    nw = strlen(w);

    sa = NULL;

    for (i = 0; i < na; i++)
        {t = ta[i];
	 if (t == NULL)
	    continue;

	 if (((nw == 1) && (w[0] == '_')) || (strncmp(t, w, nw) == 0))
	    {ta[i] = NULL;

	     ok = TRUE;
	     if (ne != 0)
	        {for (ie = 0; (ie < ne) && (ok == TRUE); ie++)
		     ok &= (strncmp(t, exc[ie], strlen(exc[ie])) != 0);};

	     if (ok == TRUE)
	        sa = lst_push(sa, t);};};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_CLASS_PERL - write a PERL form text representation of
 *                  - a class CLSS of type CTYPE
 *                  - omit NE variables named by EXC
 */

static int write_class_perl(client *cl, FILE *out, char *clss, char *ctype,
			    char *ind, char **ta, int na, char **exc, int ne)
   {int i, l, n, ic, nc, ni, global;
    char cln[BFLRG];
    char *c, *var, *val;
    char **vars, **vals, **ca, **sa;

    nstrncpy(cln, BFLRG, clss, -1);
    ca = tokenize(cln, " \t", 0);
    ni = lst_length(ca);
    string_sort(ca, ni);

    for (l = 0; l < ni; l++)
        {c = ca[l];

	 if (strcmp(c, "Glb") == 0)
	    continue;

	 global = (strcmp(c, "Global") == 0);

	 if (global == TRUE)
	    {sa = sublist(ta, na, "", exc, ne);
	     n  = lst_length(sa);
             string_sort(sa, n);
	     if (n > 0)
	        fprintf(out, "# Global variables\n\n");}
         else
	    {fprintf(out, "%s'%s:%s' => {\n", ind, ctype, c);
	     sa = sublist(ta, na, c, exc, ne);
	     n  = lst_length(sa);};

	 if (sa != NULL)
	    {vars = MAKE_N(char *, n+1);
	     vals = MAKE_N(char *, n+1);
	     if ((vars != NULL) && (vals != NULL))
	        {nc = 0;
		 for (i = 0; i < n; i++)
		     {var = sa[i];
		      if (IS_NULL(var) == TRUE)
			 continue;
		      else if (global == FALSE)
			 var += (strlen(c) + 1);

		      val = strchr(var, '=');
		      if (val != NULL)
			 *val++ = '\0';

		      ic = strlen(var);
		      nc = max(nc, ic);

		      vars[i] = var;
		      vals[i] = STRSAVE(val);};

		 vars[n] = NULL;
		 vals[n] = NULL;

		 for (i = 0; i < n; i++)
		     {if ((vars[i] != NULL) && (vals[i] != NULL))
			 {if (global == TRUE)
			     {if (vals[i][0] == '"')
				 fprintf(out, "%s%-*s => %s,\n",
					 ind, nc, vars[i], vals[i]);
			      else if (strcmp(vals[i], "\\") == 0)
				 fprintf(out, "%s%-*s => '\\\\',\n",
					 ind, nc, vars[i]);
			      else
				 fprintf(out, "%s%-*s => '%s',\n",
					 ind, nc, vars[i], vals[i]);}
			  else
			     {if (vals[i][0] == '"')
				 fprintf(out, "%s   %-*s => %s,\n",
					 ind, nc, vars[i], vals[i]);
			      else if (strcmp(vals[i], "\\") == 0)
				 fprintf(out, "%s%-*s => '\\\\',\n",
					 ind, nc, vars[i]);
			      else
				 fprintf(out, "%s   %-*s => '%s',\n",
					 ind, nc, vars[i], vals[i]);};};};

		 FREE(vars);
		 free_strings(vals);};

	     free_strings(sa);};

	 if (global == TRUE)
	    fprintf(out, "\n");
	 else
	    fprintf(out, "%s},\n\n", ind);};

    free_strings(ca);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_PERL - write the input form of the database
 *            - to be read by mio
 */

static void write_perl(client *cl, state *st, char *dbname)
   {int rv, na, ne;
    char t[BFLRG];
    char **ta;
    FILE *out;
    char *exc[] = { "PCO_Globals",
		    "PCO_EnvGroups", "PCO_EnvVars", "Save_CC", "Save_CFLAGS",
		    "gstatus", "t" };

    ne = sizeof(exc)/sizeof(char *);

    if (dbname != NULL)
       snprintf(t, BFLRG, "%s.%s.pl", cgetenv(FALSE, "PERDB_PATH"), dbname);
    else
       snprintf(t, BFLRG, "%s.pl", cgetenv(FALSE, "PERDB_PATH"));
    out = open_file("w", t);

    fprintf(out, "$PACT = {\n");

    ta = _db_clnt_ex(cl, FALSE, "save:");
    na = lst_length(ta);
    na--;
    if ((ta != NULL) && (na > 0))
       FREE(ta[na]);

    rv = write_class_perl(cl, out, st->def_tools, "Tool",
			  "   ", ta, na, NULL, 0);
    ASSERT(rv == 0);

    rv = write_class_perl(cl, out, st->def_groups, "Group",
			  "   ", ta, na, NULL, 0);
    ASSERT(rv == 0);

    rv = write_class_perl(cl, out, "Global", NULL,
			  "   ", ta, na, exc, ne);
    ASSERT(rv == 0);

    fprintf(out, "};\n");

    free_strings(ta);

    fclose_safe(out);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PCO_SAVE_DB - write a text representation of the configuration database */

static int pco_save_db(client *cl, char *dbname)
   {int rv;
    char t[BFLRG];

    rv = TRUE;

    dbset(cl, "PCO_Tools",  st.def_tools);
    dbset(cl, "PCO_Groups", st.def_groups);

/* save the persistent database */
    if (dbname == NULL)
       nstrncpy(t, BFLRG, "save:", -1);
    else
       snprintf(t, BFLRG, "save:%s:", dbname);
    dbcmd(cl, t);

/* write the input form of the database - to be read back in by pco */
    write_pco(cl, &st, dbname);

/* write the perl form of the database - to be read back in by mio */
    write_perl(cl, &st, dbname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PCO_LOAD_DB - load the specified database */

static int pco_load_db(client *cl, char *dbname)
   {int rv, i, nv;
    char **ta;

    rv = dbrestore(cl, dbname);

    separator(NULL);

    note(NULL, "Restored database variables:\n");

    ta = cenv(TRUE, NULL);
    if (ta != NULL)
       {nv = lst_length(ta);
	for (i = 0; i < nv; i++)
	    note(NULL, "   %s\n", ta[i]);

        note(NULL, "%d variables total\n", i);
	free_strings(ta);};

    separator(NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENV_OUT - write out an environment variable to various files */

static void env_out(FILE *fsh, FILE *fcsh, FILE *fdk, FILE *fmd,
		    char *var, char *val)
   {char s[BFLRG];
    char *vl;

    if (val == NULL)
       val = "\"\"";

    nstrncpy(s, BFLRG, val, -1);
    vl = trim(expand_env(s, BFLRG, NULL, FALSE), BOTH, " \t");

    note(fsh,  "export %s=%s\n",    var, vl);
    note(fcsh, "setenv %s %s\n",    var, vl);
    note(fdk,  "dk_setenv %s %s\n", var, vl);
    note(fmd,  "setenv %s %s;\n",   var, vl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_SPEC_ENV_VARS - add variables specified in the environment
 *                   - variables PCO_EnvGroups and PCO_EnvVars
 *                   - to the environment files
 */

static void add_spec_env_vars(client *cl, int blank,
			      FILE *fcsh, FILE *fsh, FILE *fdk, FILE *fmd)
			      
   {int iv, nv;
    char g[BFSML], s[BFLRG];
    char *p, *v, *var, **gt, **va;

/* check for Groups and Tools to add */
    var = cgetenv(TRUE, "PCO_EnvGroups");
    gt  = tokenize(var, " :", 0);
    nv  = (gt != NULL) ? lst_length(gt) : 0;
    if (nv > 0)
       {int ia, na, nc, eq;
	char **ta, *vr, *vl;

	string_sort(gt, nv);

	ta = _db_clnt_ex(cl, FALSE, "save:");
	na = lst_length(ta);
	na--;
	string_sort(ta, na);

	ia = 0;
	for (iv = 0; iv < nv; iv++)
	    {snprintf(g, BFSML, "%s_", gt[iv]);
	     nc = strlen(g);
	     for ( ; ia < na; ia++)
	         {eq = strncmp(g, ta[ia], nc);
		  if (eq == 0)
		     {vr = ta[ia];
		      vl = strchr(vr, '=');
		      if (vl != NULL)
			 *vl++ = '\0';
		      if ((vl != NULL) &&
			  (strpbrk(vl, " \t()") != NULL) &&
			  (vl[0] != '"'))
			 {snprintf(s, BFLRG, "\"%s\"", vl);
			  vl = s;};
		      env_out(fsh, fcsh, fdk, fmd, vr, vl);}
		  else if (eq > 0)
		     continue;
		  else
		     break;};};};

    free_strings(gt);

/* check for individual variables */
    var = cgetenv(TRUE, "PCO_EnvVars");
    va  = tokenize(var, " :", 0);
    nv  = (va != NULL) ? lst_length(va) : 0;
    for (iv = 0; iv < nv; iv++)
        {v = va[iv];
	 p = dbget(cl, TRUE, v);
	 if ((IS_NULL(p) == FALSE) || (blank == TRUE))
	    env_out(fsh, fcsh, fdk, fmd, v, p);};

    free_strings(va);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_SET_DB - add variables set from database file */

static void add_set_db(FILE *fcsh, FILE *fsh, FILE *fdk, FILE *fmd)
   {int i, n, nc;
    char s[BFLRG];
    char *var, *val, **sa;
    static char *rej[] = { "Log", "ALog",
			   "IRules_CCP", "IRules_CCObj", "IRules_CCArc",
			   "IRules_CCObj_BP", "IRules_CCArc_BP", 
			   "IRules_LexObj", "IRules_LexArc", "IRules_LexC",
			   "IRules_LexObj_BP", "IRules_LexArc_BP", 
			   "IRules_YaccObj", "IRules_YaccArc", "IRules_YaccC",
			   "IRules_YaccObj_BP", "IRules_YaccArc_BP", 
			   "IRules_FCObj", "IRules_FCArc", "TemplH", NULL };

    sa = cenv(TRUE, rej);
    if (sa != NULL)
       {n = lst_length(sa);
	for (i = 0; i < n; i++)
	    {var = sa[i];
	     val = strchr(var, '=');
	     if (val != NULL)
	        {*val++ = '\0';
		    
		 nc = last_char(val);
		 if (val[nc] == '\n')
		    val[nc] = '\0';

/* handle PATH specially - just gather everything that is not $PATH or ${PATH} */
	         if (strcmp(var, "PATH") == 0)
		    push_path(P_APPEND, epath, val);

		 else
		   {snprintf(s, BFLRG, "\"%s\"", val);
		    env_out(fsh, fcsh, fdk, fmd, var, s);};};};

	free_strings(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ENVF - write out the env file because other scripts need to source it
 *            - write it for CSH or SH families
 */

static void write_envf(client *cl, int lnotice)
   {int i, n;
    char *site[] = { "RF_CONFIG_METHOD" };
    char *sfx[]  = { "csh", "sh", "dk", "mdl" };
    FILE *fcsh, *fsh, *fdk, *fmd;

    separator(NULL);
    if (lnotice == TRUE)
       noted(NULL, "   Environment setup files - env-pact (csh, sh, dk, and mdl)\n");
    else
       note(NULL, "   Environment setup files - env-pact (csh, sh, dk, and mdl)\n");
    note(NULL, "\n");

    fcsh = open_file("w", "%s/env-pact.csh", st.dir.etc);
    fsh  = open_file("w", "%s/env-pact.sh",  st.dir.etc);
    fdk  = open_file("w", "%s/env-pact.dk",  st.dir.etc);
    fmd  = open_file("w", "%s/env-pact.mdl", st.dir.etc);

/* initialize module with boilerplate */
    note(fmd,  "#%%Module1.0\n");
    note(fmd,  "#\n");
    note(fmd,  "# pact modulefile\n");
    note(fmd,  "# vi:set filetype=tcl:\n");
    note(fmd,  "#\n");
    note(fmd,  "\n");
    note(fmd,  "# module whatis\n");
    note(fmd,  "module-whatis \"PACT Environment\";\n");
    note(fmd,  "\n");
    note(fmd,  "# module help\n");
    note(fmd,  "proc ModulesHelp { } {\n");
    note(fmd,  "   global version;\n");
    note(fmd,  "\n");
    note(fmd,  "   puts stderr \"\\tThis module sets your environment to use PACT\\n\";\n");
    note(fmd,  "}\n");
    note(fmd, "\n");

/* make a temporary approximation to st.env_csh
 * NOTE: used the C Shell to expand and print unique environment variable settings
 * in Bourne Shell syntax in the past
 */
    if (st.db == NULL)
       {n = sizeof(site)/sizeof(char *);
	for (i = 0; i < n; i++)
	    env_out(fsh, fcsh, fdk, fmd, site[i], dbget(cl, TRUE, site[i]));

	add_spec_env_vars(cl, TRUE, fcsh, fsh, fdk, fmd);}
    else
       add_set_db(fcsh, fsh, fdk, fmd);

    note(fcsh, "\n");
    note(fsh, "\n");
    note(fdk, "\n");
    note(fmd, "\n");

/* emit MANPATH settings */
    note(fcsh, "if ($?MANPATH == 1) then\n");
    note(fcsh, "   setenv MANPATH %s/man:$MANPATH\n", st.dir.root);
    note(fcsh, "else\n");
    note(fcsh, "   setenv MANPATH %s/man:\n", st.dir.root);
    note(fcsh, "endif\n");
    note(fcsh, "\n");

    note(fsh, "if [ \"$MANPATH\" != \"\" ]; then\n");
    note(fsh, "   export MANPATH=%s/man:$MANPATH\n", st.dir.root);
    note(fsh, "else\n");
    note(fsh, "   export MANPATH=%s/man\n", st.dir.root);
    note(fsh, "fi\n");
    note(fsh, "\n");

    note(fdk, "dk_alter MANPATH %s/man\n", st.dir.root);
    note(fmd, "prepend-path MANPATH %s/man;\n", echo(FALSE, st.dir.root));

/* emit PATH settings */
    if (IS_NULL(epath) == FALSE)
       {FOREACH(u, epath, ":\n")
	   note(fdk, "dk_alter PATH %s\n", u);
	   note(fmd, "prepend-path PATH    %s;\n", echo(FALSE, u));
	ENDFOR
        note(fcsh, "setenv PATH    %s/bin:%s:$PATH\n", st.dir.root, epath);
        note(fsh,  "export PATH=%s/bin:%s:$PATH\n", st.dir.root, epath);}
    else
       {note(fcsh, "setenv PATH    %s/bin:$PATH\n", st.dir.root);
        note(fsh, "export PATH=%s/bin:$PATH\n", st.dir.root);};

    note(fdk, "dk_alter PATH    %s/bin\n", st.dir.root);
    note(fmd, "prepend-path PATH    %s/bin;\n", echo(FALSE, st.dir.root));
    note(fdk, "\n");
    note(fmd, "\n");

/* write the CSH version like this because PCSH scripts like
 * PDBView need to get the users SCHEME variable if defined
 */
    note(fcsh, "if ($?SCHEME == 1) then\n");
    note(fcsh, "   setenv SCHEME  ${SCHEME}:%s/scheme\n", st.dir.root);
    note(fcsh, "else\n");
    note(fcsh, "   setenv SCHEME  %s/scheme\n", st.dir.root);
    note(fcsh, "endif\n");
    note(fcsh, "setenv ULTRA   %s/scheme\n", st.dir.root);

    note(fsh, "export SCHEME=%s/scheme\n", st.dir.root);
    note(fsh, "export ULTRA=%s/scheme\n", st.dir.root);
    note(fdk, "dk_setenv SCHEME  %s/scheme\n", st.dir.root);
    note(fdk, "dk_setenv ULTRA   %s/scheme\n", st.dir.root);
    note(fmd, "setenv SCHEME  %s/scheme;\n", st.dir.root);
    note(fmd, "setenv ULTRA   %s/scheme;\n", st.dir.root);

    fclose_safe(fcsh);
    fclose_safe(fsh);
    fclose_safe(fdk);
    fclose_safe(fmd);

/* log the results */
    n = sizeof(sfx)/sizeof(char *);
    for (i = 0; i < n; i++)
        {note(NULL, "----- env-pact.%s -----\n", sfx[i]);
	 cat(NULL, 0, -1, "%s/env-pact.%s", st.dir.etc, sfx[i]);
         note(NULL, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_CROSS - verify that we have a DB_RUN_SIGNATURE if
 *             - we are cross compiling
 */

static int check_cross(client *cl)
   {int rv;
    char *s;

    rv = TRUE;

    s = cgetenv(TRUE, "CROSS_COMPILE");
    if (strcmp(s, "FALSE") != 0)
       {s = cgetenv(TRUE, "DB_RUN_SIGNATURE");
	if (file_exists(s) == FALSE)
	   {noted(NULL, "\n");
	    noted(NULL, "Cross compiling without a run_signature database - exiting\n");
	    noted(NULL, "\n");
	    rv = FALSE;}
	else
	   {dbset(cl, "DB_RUN_SIGNATURE", s);
	    note(NULL, "\n");
	    note(NULL, "Cross compiling with run_signature database %s\n", s);
	    note(NULL, "\n");};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_DIR - check for missing directories */

static void check_dir(client *cl)
   {int i, n;
    char Created[BFLRG], Missing[BFLRG];
    char *sib, *dir;
    static char *dlst[] = {"bin", "lib", "include", "include/shell",
			   "etc", "scheme", "man", "man/man1", "man/man3"};

    n   = sizeof(dlst)/sizeof(char *);
    sib = dbget(cl, TRUE, "PSY_InstRoot");

    if (st.create_dirs == TRUE)
       {Created[0] = '\0';
        if ((IS_NULL(sib) == FALSE) && (strcmp(sib, "none") != 0))
           {for (i = 0; i < n; i++)
                {dir = dlst[i];
                 if (dir_exists("%s/%s", sib, dir) == FALSE)
                    {run(BOTH, "mkdir -p %s/%s", sib, dir);
                     push_tok(Created, BFLRG, ' ', "%s/%s", sib, dir);};};};

        if (IS_NULL(Created) == FALSE)
           {noted(NULL, "\n");
            noted(NULL, "Directories:\n");
            FOREACH(dir, Created, " ")
               noted(NULL, "   %s\n", dir);
            ENDFOR
            noted(NULL, "have been created as requested\n");
            noted(NULL, "\n");};}

    else
       {Missing[0] = '\0';
        if ((IS_NULL(sib) == FALSE) && (strcmp(sib, "none") != 0))
           {for (i = 0; i < n; i++)
                {dir = dlst[i];
                 if (dir_exists("%s/%s", sib, dir) == FALSE)
                    {push_tok(Missing, BFLRG, ' ', "%s/%s", sib, dir);};};};

        if (IS_NULL(Missing) == FALSE)
           {noted(NULL, "\n");
            noted(NULL, "You have asked that PACT be installed in the following\n");
            noted(NULL, "missing directories:\n");
            FOREACH (dir, Missing, " ")
               noted(NULL, "   %s\n", dir);
            ENDFOR
            noted(NULL, "You must either use the -c option to create these\n");
            noted(NULL, "directories, choose another place to install PACT,\n");
            noted(NULL, "or specify -i none for no installation\n");
            noted(NULL, "\n");

            exit(2);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_LINE - read the next line from the input */

static void read_line(char *s, int nc)
   {int n;
    char t[BFLRG];
    char *p, *pc;
    file_entry *se;

    se = &st.fstck.file[st.fstck.n-1];

    p = fgets(s, nc-1, se->fp);
    if (p != NULL)
       {se->iline++;

	n = strlen(s);

/* handle continued lines */
        while ((n > 1) && (s[n-2] == '\\') && (s[n-1] == '\n'))
           {p = fgets(t, BFLRG, se->fp);
	    if (p != NULL)
	       {nstrncpy(s+n-2, nc-n-2, trim(t, BOTH, " \t"), -1);
		n = strlen(s);};
	    se->iline++;};

/* make sure the line ends in '\n'
 * if last line in file does not end in '\n' this
 * can happen with bad consequences
 */
	if (s[n-1] != '\n')
	   s[n++] = '\n';

/* now end the line - stripping off trailing comment or '\n' */
	pc = strchr(s, '#');
	if (pc != NULL)
	   *pc = '\0';
	else
	   s[n-1] = '\0';

/* toss blank and comment lines */
	if (blank_line(s) == TRUE)
           {if (st.verbose == TRUE)
	       noted(NULL, "%s line %d ignored: %s\n",
		      se->name, se->iline, s);
	    s[0] = '\0';};}
    else
       {pop_file();
	if (st.fstck.n > 0)
	   s[0] = '\0';
	else
	   strcpy(s, "++end++");
	note(NULL, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_OPT - work with option specifications */

static void parse_opt(client *cl, char *s, int nc)
   {int i, l, n, ok, mt;
    exoper oper;
    char vr[BFLRG], op[BFLRG], vl[BFLRG];
    char *t, *avl, *arg, *opt, **sa;

    sa = tokenize(s, "[;]\n\r", 0);
    if (sa != NULL)
       {for (n = 0; sa[n] != NULL; n++);

	ok  = FALSE;
	avl = "off";
	for (l = 0; (l < n) && (ok == FALSE); l++)
	    {vr[0] = '\0';

	     t = sa[l];
	     parse_line(cl, t, vr, op, vl, nc);

	     if (IS_NULL(vr) == TRUE)
	        continue;

	     arg = trim(vl, BOTH, " \t");

	     if (strcmp(op, "=") == 0)
	        oper = PEQ;
	     else if (strcmp(op, "<") == 0)
	        oper = PLT;
	     else if (strcmp(op, "<=") == 0)
	        oper = PLE;
	     else if (strcmp(op, ">") == 0)
	        oper = PGT;
	     else if (strcmp(op, ">=") == 0)
	        oper = PGE;
	     else if (strcmp(op, "!=") == 0)
	        oper = PNE;
	     else
	        oper = PNONE;

/* parse the _cmd_ key */
	     if (strcmp(vr, "_cmd_") == 0)
	        {opt = strchr(arg, '@');
		 if (opt != NULL)
		    opt[-1] = '\0';
		 for (i = 0; i < st.na; i++)
		     {if (strcmp(st.args[i], arg) == 0)
			 {if ((opt == NULL) || (strchr(opt, '@') == NULL))
			     avl = "on";
			  else
			     avl = st.args[++i];
			  break;};};}

/* parse the _env_ key */
	     else if (strcmp(vr, "_env_") == 0)
	        avl = dbget(cl, TRUE, arg);

/* select the value */
	     else

/* treat single '*' specially since match will automatically succeed */
	       {if (strcmp(vr, "*") == 0)
		   {if (strcmp(avl, "off") != 0)
		       {nstrncpy(s, nc, arg, -1);
			trim(s, BACK, " *\t\n");
/*
                        if (LAST_CHAR(s) == '\n')
		           LAST_CHAR(s) = '\0';
*/
			nstrcat(s, nc, avl);
			ok = TRUE;};}

	        else if (strcmp(avl, vr) == 0)
		   {nstrncpy(s, nc, arg, -1);
		    ok = TRUE;}

		else
		   {mt = match(avl, vr);
		    switch (oper)
		       {case PEQ :
		             if (mt == 0)
			        {strcpy(s, arg);
				 ok = TRUE;};
			     break;
			case PLT :
			     if (mt < 0)
			        {strcpy(s, arg);
				 ok = TRUE;};
			     break;
			case PLE :
			     if (mt < 1)
			        {strcpy(s, arg);
				 ok = TRUE;};
			     break;
			case PGT :
			     if (mt > 0)
			        {strcpy(s, arg);
				 ok = TRUE;};
			     break;
			case PGE :
			     if (mt > -1)
			        {strcpy(s, arg);
				 ok = TRUE;};
			     break;
			case PNE :
			     if (mt != 0)
			        {strcpy(s, arg);
				 ok = TRUE;};
			     break;
			default :
			     break;};};};};

	if (ok == FALSE)
	   s[0] = '\0';

	free_strings(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_LINE - parse the next line from the input */

static void parse_line(client *cl, char *s,
		       char *key, char *oper, char *value, int nc)
   {char t[BFLRG];
    char *p;

    strcpy(t, s);

    p = strtok(t, " \t");
    if (p != NULL)
       nstrncpy(key, nc, p, -1);

    p = strtok(NULL, " \t");
    if (p != NULL)
       nstrncpy(oper, nc, p, -1);
    else
       oper[0] = '\0';

    p = strtok(NULL, "\n");
    if (p != NULL)
       {nstrncpy(value, nc, p, -1);
	if ((strchr(oper, '=') != NULL) && (p[0] == '['))
	   {while (strchr(value, ']') == NULL)
	       {read_line(t, BFLRG);
		nstrcat(value, nc, " ");
		nstrcat(value, nc, trim(t, FRONT, " \t"));};
	    parse_opt(cl, value, nc);};}
    else
       value[0] = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_RULE - read a rule into the VAR */

static void parse_rule(char *var, int nc)
   {int n;
    char line[BFLRG], val[BFLRG];

    val[0] = '\0';

    while (TRUE)
       {read_line(line, BFLRG);

        if ((IS_NULL(line) == TRUE) || (strcmp(line, "++end++") == 0))
           break;

	nstrcat(val, BFLRG, line);
        nstrcat(val, BFLRG, "\n");};

    n = strlen(val);
    n = min(n, nc-1);
    nstrncpy(var, nc, val, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_ANALYZE_ENV - setup the environment for programs which analyze
 *                   - features
 */

static void setup_analyze_env(client *cl, char *base)
   {char alog[BFLRG];
    FILE *out;

/* setup the analyze log file */
    snprintf(alog, BFLRG, "%s/log/analyze",  st.dir.root);
    out = open_file("w", alog);
    note(out, "%s\n", get_date());
    fclose_safe(out);

    dbset(cl, "HSY_Host",       st.host);
    dbset(cl, "HSY_OS_Name",    st.os);
    dbset(cl, "HSY_OS_Release", st.osrel);

    dbset(cl, "PSY_Cfg",    st.psy_cfg);

    dbset(cl, "RF_DEBUG",    st.abs_deb ? "TRUE" : "FALSE");
    dbset(cl, "RF_OPTIMIZE", st.abs_opt ? "TRUE" : "FALSE");
    dbset(cl, "RF_PROFILE",  st.profilep ? "TRUE" : "FALSE");
    dbset(cl, "USE_TmpDir",  st.tmp_dirp ? "TRUE" : "FALSE");

    dbset(cl, "Log",        st.logf);
    dbset(cl, "ALog",       alog);

    if (strncmp(st.os, "CYGWIN", 6) == 0)
       st.os[6] = '\0';

    if (strcmp(st.os, "Windows_NT") == 0)
       dbset(cl, "AF_CDecls", "TRUE");
    else
       dbset(cl, "AF_CDecls", "FALSE");

/* cross compile front end */
    if (dbcmp(cl, "CROSS_COMPILE", "FALSE") != 0)
       dbinitv(cl, "CROSS_FE", "%s/do-run -m", st.dir.bin);
    else
       dbinitv(cl, "CROSS_FE", "");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_OUTPUT_ENV - setup the environment for programs which write
 *                  - configured files
 */

static void setup_output_env(client *cl, char *base)
   {int i, nv;
    char dlst[BFLRG];
    char **sa;

/* remove duplicate tokens in selected lists */
    nstrncpy(dlst, BFLRG, dbget(cl, FALSE, "PCO_RemDup"), -1);
    sa = tokenize(dlst, " \t", 0);
    nv = lst_length(sa);
    for (i = 0; i < nv; i++)
       dbset(cl, sa[i], unique(dbget(cl, FALSE, sa[i]), FALSE, ' '));
    free_strings(sa);

    dbset(cl, "PSY_CfgDir",  st.dir.cfg);

    dbset(cl, "BinDir",  st.dir.bin);
    dbset(cl, "IncDir",  st.dir.inc);
    dbset(cl, "EtcDir",  st.dir.etc);

    dbset(cl, "PSY_Cfg", st.cfgf);

    dbset(cl, "IRules_CCP",     st.rules.ccp);
    dbset(cl, "IRules_CCObj",   st.rules.co);
    dbset(cl, "IRules_CCArc",   st.rules.ca);
    dbset(cl, "IRules_LexObj",  st.rules.lo);
    dbset(cl, "IRules_LexArc",  st.rules.la);
    dbset(cl, "IRules_LexC",    st.rules.lc);
    dbset(cl, "IRules_YaccObj", st.rules.yo);
    dbset(cl, "IRules_YaccArc", st.rules.ya);
    dbset(cl, "IRules_YaccC",   st.rules.yc);
    dbset(cl, "IRules_FCObj",   st.rules.fo);
    dbset(cl, "IRules_FCArc",   st.rules.fa);
    dbset(cl, "IRules_TemplH",  st.rules.th);

    dbset(cl, "IRules_CCObj_BP",   st.rules.co_bp);
    dbset(cl, "IRules_CCArc_BP",   st.rules.ca_bp);
    dbset(cl, "IRules_LexObj_BP",  st.rules.lo_bp);
    dbset(cl, "IRules_LexArc_BP",  st.rules.la_bp);
    dbset(cl, "IRules_YaccObj_BP", st.rules.yo_bp);
    dbset(cl, "IRules_YaccArc_BP", st.rules.ya_bp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_VAR - setup default variable values */

static void default_var(client *cl, char *base)
   {int i, n;
    char cmd[BFLRG];
    char **sa;

    if (cdefenv("USER") == FALSE)
       {if (cdefenv("LOGNAME") == FALSE)
	   dbset(cl, "USER", "anonymous");
        else
	   dbset(cl, "USER", cgetenv(TRUE, "LOGNAME"));};

    csetenv("PATH", "%s:%s", st.dir.mng, cgetenv(TRUE, "PATH"));

/* log the current environment */
    sa = cenv(TRUE, NULL);
    n  = lst_length(sa);
    for (i = 0; i < n; i++)
        note(NULL, "%s\n", sa[i]);
    free_strings(sa);

/* define the set of specifications which define a tool */
    st.toolv[0] = '\0';
    push_tok(st.toolv, BFLRG, ' ', "Exe");
    push_tok(st.toolv, BFLRG, ' ', "Flags");
    push_tok(st.toolv, BFLRG, ' ', "Version");
    push_tok(st.toolv, BFLRG, ' ', "Debug");
    push_tok(st.toolv, BFLRG, ' ', "Optimize");
    push_tok(st.toolv, BFLRG, ' ', "Inc");
    push_tok(st.toolv, BFLRG, ' ', "Lib");
    push_tok(st.toolv, BFLRG, ' ', "RPath");
    push_tok(st.toolv, BFLRG, ' ', "IFlag");
    push_tok(st.toolv, BFLRG, ' ', "XFlag");

    strcpy(st.psy_cfg, path_tail(st.cfgf));

    unamef(st.host,  BFLRG, "n");
    unamef(st.os,    BFLRG, "s");
    unamef(st.osrel, BFLRG, "r");
    unamef(st.hw,    BFLRG, "m");

/* remove parens from osrel - it is bad for the shells later on */
    sa = tokenize(st.osrel, "()", 0);
    concatenate(st.osrel, BFLRG, sa, 0, -1, ",");
    free_strings(sa);

    snprintf(st.dir.scr, BFLRG, "%s/scripts", base);
    snprintf(cmd, BFLRG, "%s/system-id", st.dir.scr);
    nstrncpy(st.arch, BFLRG, run(BOTH, cmd), -1);

/* check variables which may have been initialized from the command line */
    if (IS_NULL(st.psy_id) == TRUE)
       nstrncpy(st.psy_id, BFLRG, run(BOTH, "%s/cfgman use", st.dir.scr), -1);
    cinitenv("PSY_ID", st.psy_id);

    dbset(cl, "PACT_CFG_ID", st.psy_id);

    dbinitv(cl, "PSY_CfgMan",    "%s/cfgman", st.dir.scr);

/* global variables */
    snprintf(st.dir.root, BFLRG, "%s/dev/%s",  base, st.psy_id);
    snprintf(st.dir.inc,  BFLRG, "%s/include", st.dir.root);
    snprintf(st.dir.etc,  BFLRG, "%s/etc",     st.dir.root);
    snprintf(st.dir.lib,  BFLRG, "%s/lib",     st.dir.root);
    snprintf(st.dir.bin,  BFLRG, "%s/bin",     st.dir.root);

    snprintf(st.env_csh,  BFLRG, "%s/env-pact.csh", st.dir.etc);
    snprintf(st.env_sh,   BFLRG, "%s/env-pact.sh",  st.dir.etc);
    snprintf(st.env_dk,   BFLRG, "%s/env-pact.dk",  st.dir.etc);
    snprintf(st.env_mdl,  BFLRG, "%s/env-pact.mdl", st.dir.etc);

/* variable defaults */
    strcpy(st.def_tools, "");

    push_struct("Glb", st.def_groups, STACK_GROUP);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_RULES - setup the default rules for CC, Lex, Yacc, and FC */

static void default_rules(void)
   {char *ar, *cd, *le, *ye, *rm, *tc;

    le  = "sed \"s|lex.yy.c|$*.c|\" lex.yy.c | sed \"s|yy|$*_|g\" > $*.c";
    ye  = "sed \"s|y.tab.c|$*.c|\" y.tab.c | sed \"s|yy|$*_|g\" > $*.c";
    ar  = "${AR} ${AROpt} ${TGTLib} $*.o 2>> errlog";
    rm  = "${RM} errlog";
    cd  = "cd ${PACTTmpDir}";
    tc  = "touch errlog";

/* C rules */
    snprintf(st.rules.ccp,   BFLRG,
             "\t%s\n",
	     "${CC} -E $<");

    snprintf(st.rules.co, BFLRG,
             "\t@(%s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     "( ${CC} -c $< -o $@ ) || frnsic -e 1 $< $@");

    snprintf(st.rules.ca, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     cd, rm, tc,
	     "( ${CC} -c ${PACTSrcDir}/$< -o $*.o ) || frnsic -e 1 ${PACTSrcDir}/$< $*.o",
	     ar,
	     "${RM} $*.o 2>> errlog");

/* lex rules */
    snprintf(st.rules.lo, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
             "echo \"lex $<\"",
	     rm, tc,
	     "${LEX} $< 2>> errlog",
	     le,
	     "( ${LX} -c $*.c ) || frnsic -e 1 $*.c",
	     "${RM} lex.yy.c $*.c");

    snprintf(st.rules.la, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "( ${LEX} -t ${PACTSrcDir}/$< 1> lex.yy.c 2>> errlog ) || frnsic -e 1 ${PACTSrcDir}/$< lex.yy.c",
	     le,
	     "echo \"${LXAnnounce} -c $*.c\"",
	     "( ${LX} -c $*.c -o $*.o ) || frnsic -e 1 $*.c $*.o",
	     ar,
	     "${RM} lex.yy.c $*.c");

    snprintf(st.rules.lc, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "( ${LEX} ${PACTSrcDir}/$< 2>> errlog ) || frnsic -e 1 ${PACTSrcDir}/$<",
	     le,
	     "${RM} lex.yy.c");

/* yacc rules */
    snprintf(st.rules.yo, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "( ${YACC} ${PACTSrcDir}/$< 2>> errlog ) || frnsic -e 1 ${PACTSrcDir}/$<",
	     ye,
	     "( ${YC} -c $*.c -o $*.o ) || frnsic -e 1 $*.c $*.o",
	     "${RM} $*.c");

    snprintf(st.rules.ya, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "( ${YACC} ${PACTSrcDir}/$< 2>> errlog ) || frnsic -e 1 ${PACTSrcDir}/$<",
	     ye,
	     "echo \"${YCAnnounce} -c $*.c\"",
	     "( ${YC} -c $*.c -o $*.o ) || frnsic -e 1 $*.c $*.o",
	     ar,
	     "${RM} $*.c $*.o");

    snprintf(st.rules.yc, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "( ${YACC} ${PACTSrcDir}/$< 2>> errlog ) || frnsic -e 1 ${PACTSrcDir}/$<",
	     ye,
	     "mv $*.c ${PACTSrcDir}");
      
/* Fortran rules */
    snprintf(st.rules.fo, BFLRG,
	     "\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t%s\n",
	     "@if [ ${FC_Exe} == none ]; then",
	     "   echo \"No Fortran compiler for $<\" ;",
	     "else",
	     "   echo \"${FCAnnounce} -c $<\" ;",
	     "   ( ${FC} -c $< -o $@ ) || frnsic -e 1 $< $@ ;",
	     "fi");

    snprintf(st.rules.fa, BFLRG,
	     "\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t     %s ; \\\n\t     %s ; \\\n\t     %s ; \\\n\t%s \\\n\t     %s ; \\\n\t%s \\\n\t%s\n",
	     "@if [ ${FC_Exe} == none ]; then",
	     "   echo \"No Fortran compiler for $<\" ;",
	     "else",
	     "   echo \"${FCAnnounce} -c $<\" ;",
	     cd, rm, tc,
	     "     ( ${FC} -c ${PACTSrcDir}/$< -o $*.o ) || frnsic -e 1 ${PACTSrcDir}/$< $*.o ;",
	     ar,
	     "     ${RM} $*.o 2>> errlog ;",
	     "fi");

/* template rules */
    snprintf(st.rules.th, BFLRG,
	     "\t@(%s ; \\\n          %s)\n",
	     "echo \"${Template} $< -o ${IncDir}/$*.h\"",
	     "${BinDir}/${Template} $< -o ${IncDir}/$*.h");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BAD_PRAGMA_RULES - setup the rules for CC, Lex, Yacc, and FC
 *                  - allowing for bad handling of _Pragma
 */

static void bad_pragma_rules(void)
   {char *ar, *cd, *le, *ye, *rm, *tc;

    le  = "sed \"s|lex.yy.c|$*.c|\" lex.yy.c | sed \"s|yy|$*_|g\" > $*.c";
    ye  = "sed \"s|y.tab.c|$*.c|\" y.tab.c | sed \"s|yy|$*_|g\" > $*.c";
    ar  = "${AR} ${AROpt} ${TGTLib} $*.o 2>> errlog";
    rm  = "${RM} errlog";
    cd  = "cd ${PACTTmpDir}";
    tc  = "touch errlog";

/* C rules */
    snprintf(st.rules.co_bp, BFLRG,
             "\t@(%s ; \\\n          %s)\n",
	     "echo \"${ACCAnnounce} -c $<\"",
	     "${ACC} -c $< -ao $@");

    snprintf(st.rules.ca_bp, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${ACCAnnounce} -c $<\"",
	     cd, rm, tc,
	     "${ACC} -c  ${PACTSrcDir}/$< -ao $*.o",
	     ar,
	     "${RM} $*.o 2>> errlog");

/* lex rules */
    snprintf(st.rules.lo_bp, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
             "echo \"lex $<\"",
	     rm, tc,
	     "${LEX} $< 2>> errlog",
	     le,
	     "echo \"${ALXAnnounce} -c $*.c\"",
	     "${ALX} -c $*.c -ao $*.o",
	     "${RM} lex.yy.c $*.c");

    snprintf(st.rules.la_bp, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "${LEX} -t ${PACTSrcDir}/$< 1> lex.yy.c 2>> errlog",
	     le,
	     "echo \"${ALXAnnounce} -c $*.c\"",
	     "${ALX} -c $*.c -ao $*.o",
	     ar,
	     "${RM} lex.yy.c $*.c");

/* yacc rules */
    snprintf(st.rules.yo_bp, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "echo \"${ALXAnnounce} -c $*.c\"",
	     "${ALX} -c $.c -ao $*.o",
	     "${RM} $*.c");

    snprintf(st.rules.ya_bp, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "echo \"${ALXAnnounce} -c $*.c\"",
	     "${ALX} -c $*.c -ao $*.o",
	     ar,
	     "${RM} $*.c $*.o");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PCO_SESSION - initialize the state of the config session */

static void init_pco_session(client *cl, char *base, int append)
   {

/* setup default variable values */
    default_var(cl, base);

/* setup the default rules for CC, Lex, Yacc, and FC */
    default_rules();
    bad_pragma_rules();

/* setup the log file */
    snprintf(st.logf, BFLRG, "%s/log/config", st.dir.root);
    if (append == FALSE)
       unlink_safe(st.logf);
    LOG_ON;

    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_INST_BASE - setup the PSY_InstRoot related state */

static void set_inst_base(client *cl, char *ib)
   {

    if (strcmp(ib, "none") != 0)
       {if (LAST_CHAR(ib) == '/')
	   LAST_CHAR(ib) = '\0';

	st.installp = TRUE;};

    dbset(cl, "PSY_InstRoot", ib);
    dbset(cl, "PSY_PubInc",   "%s/include", ib);
    dbset(cl, "PSY_PubLib",   "%s/lib", ib);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENV_SUBST - substitute the text NT for the occurrence of
 *           - the text of environment variable REFVAR in
 *           - any environment variable
 */

static void env_subst(client *cl, char *refvar, char *nt)
   {int i, nc, nv;
    char *ot, *vr, *vl, *p, **ta;

    ot = cgetenv(FALSE, refvar);
    if (IS_NULL(ot) == FALSE)
       {ta = cenv(FALSE, NULL);
	if (ta != NULL)
	   {nv = lst_length(ta);
	    for (i = 0; i < nv; i++)
	        {vr = ta[i];
		 if ((vr != NULL) && (strncmp(vr, "CROSS_REF=", 10) != 0))
		    {vl = strchr(vr, '=');
		     if (vl != NULL)
		        {*vl++ = '\0';

			 nc = strlen(vl);
			 if (vl[nc-1] == '\n')
			    vl[nc-1] = '\0';

			 p = strstr(vl, ot);
			 if (p != NULL)
			    dbset(cl, vr, subst(vl, ot, nt, -1));};};};

	    free_strings(ta);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_VAR - set a variable as directed */

static void set_var(client *cl, char *var, char *oper, char *val)
   {char fvar[BFLRG], nval[BFLRG], mval[BFLRG], lval[BFLRG];
    char s[BFLRG+1];
    char *prfx, *t, *p;
    gt_entry *ge;

    ge   = st.gstck.st + st.gstck.n - 1;
    prfx = ge->item;

/* attach the current group suffix */
    if (strcmp(prfx, "Glb") == 0)
       {t = dbget(cl, FALSE, "PCO_Globals");
	snprintf(s, BFLRG, "%s %s", t, var);
	dbset(cl, "PCO_Globals", unique(s, FALSE, ' '));
	nstrncpy(fvar, BFLRG, var, -1);}
    else
       snprintf(fvar, BFLRG, "%s_%s", prfx, var);

    clean_space(val);

    if ((strcmp(oper, "+=") == 0) || (strcmp(oper, "=+") == 0))
       {if (dbdef(cl, fvar) == FALSE)
           {note(NULL, "Variable %s does not exist changing %s to =\n",
		  fvar, oper);
            oper = "=";};};

/* set variable */
    if (strcmp(oper, "=") == 0)
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {nstrncpy(nval, BFLRG, t, -1);
            dbset(cl, fvar, nval);}

        else
           {nval[0] = '\0';
            dbset(cl, fvar, nval);};

	FREE(t);}

/* add item to beginning of the existing variable */
    else if (strcmp(oper, "+=") == 0)

/* check value of val
 * we want things such as "foo += $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {nstrncpy(lval, BFLRG, t, -1);
	    p = echo(FALSE, "$%s", fvar);
	    if (p != NULL)
	       {nstrncpy(nval, BFLRG, p, -1);
		FREE(p);};
            note(NULL, "Change    |%s|\n", fvar);
            note(NULL, "Prepend   |%s|\n", lval);
            note(NULL, "Old value |%s|\n", nval);

            snprintf(mval, BFLRG, "%s %s", lval, nval);
            note(NULL, "New value |%s|\n", mval);

            note(NULL, "Change expression |setenv %s %s|\n", fvar, mval);
            dbset(cl, fvar, mval);}
        else
           note(NULL, "   += not changing %s - no value for |%s|\n",
		 fvar, val);

	FREE(t);}

/* add item to end of the existing variable */
    else if (strcmp(oper, "=+") == 0)

/* check value of val
 * we want things such as "foo =+ $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {nstrncpy(lval, BFLRG, t, -1);
	    p = echo(FALSE, "$%s", fvar);
	    if (p != NULL)
	       {nstrncpy(nval, BFLRG, p, -1);
		FREE(p);};
            note(NULL, "Change    |%s|\n", fvar);
            note(NULL, "Append    |%s|\n", lval);
            note(NULL, "Old value |%s|\n", nval);

            snprintf(mval, BFLRG, "%s %s", nval, lval);
            note(NULL, "New value |%s|\n", mval);

            note(NULL, "Change expression |setenv %s %s|\n", fvar, mval);
            dbset(cl, fvar, mval);}
         else
            note(NULL, "   =+ not changing %s - no value for |%s|\n",
		  fvar, val);

	FREE(t);}

/* remove literal item from variable */
    else if (strcmp(oper, "-=") == 0)
       {t = echo(FALSE, "$%s", fvar);
	if (IS_NULL(t) == FALSE)
	   {nstrncpy(nval, BFLRG, t, -1);
	    note(NULL, "Change    |%s|\n", fvar);
	    note(NULL, "Remove    |%s|\n", val);
	    note(NULL, "Old value |%s|\n", nval);

	    p = echo(FALSE, "%s | sed 's|%s||'", nval, val);
	    if (p != NULL)
	       {nstrncpy(nval, BFLRG, p, -1);
		FREE(p);};

	    note(NULL, "New value |%s|\n", nval);

	    note(NULL, "Change expression |setenv %s %s|\n", fvar, nval);
	    dbset(cl, fvar, nval);}
         else
            note(NULL, "   -= not changing %s - no value for |%s|\n",
		  fvar, val);

	FREE(t);}

/* set variable to value only if value is defined and not empty */
    else if (strcmp(oper, "=?") == 0)

/* check value of val
 * we want things such as "foo =? $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
	   dbset(cl, fvar, t);
        else
           note(NULL, "   =? not changing %s - no value for |%s|\n",
		 fvar, val);

	FREE(t);}

/* set variable only if variable undefined and value is defined and not empty */
    else if (strcmp(oper, "?=") == 0)
       {if (dbdef(cl, fvar) == FALSE)

/* do same check on value as for '=?' case */
	   {t = echo(FALSE, val);
	    if (IS_NULL(t) == FALSE)
	       dbset(cl, fvar, t);
	    else
	       note(NULL, "   ?= not changing %s - no value for |%s|\n",
		     fvar, val);

	    FREE(t);}
	else
	   note(NULL, "   ?= not changing %s - already exists\n", fvar);}

    else
       noted(NULL, "Bad operator '%s' in SET_VAR\n", oper);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_USE - do all the variable settings implied by a Use specification */

static void process_use(client *cl, char *sg, char *oper)
   {int whch;
    char nvr[BFLRG], ulst[BFLRG];
    char *val, *currg, *currt;

    whch = -1;

/* determine whether or not it is a tool we are filling */
    FOREACH (t, st.def_tools, " ")
       if (strcmp(t, sg) == 0)
          {whch = STACK_TOOL;
           break;};
    ENDFOR

/* determine whether or not it is a group we are filling */
    FOREACH (t, st.def_groups, " ")
       if (strcmp(t, sg) == 0)
          {whch = STACK_GROUP;
           break;};
    ENDFOR

    currg = st.gstck.curr_grp;
    currt = st.gstck.curr_tool;

    switch (whch)

/* fill out a group */
       {case STACK_GROUP:
             note(NULL, "Use group %s to fill group %s\n",
		   sg, currg);

             snprintf(ulst, BFLRG, "%s", dbget(cl, FALSE, "PCO_UseVars"));
             FOREACH(var, ulst, " ")
                snprintf(nvr, BFLRG, "%s_%s", sg, var);
                if (dbdef(cl, nvr) == TRUE)
                   {val = dbget(cl, TRUE, nvr);
                    if (strcmp(var, "Exe") == 0)
                       set_var(cl, var, "=", val);
                    else
                       set_var(cl, var, oper, val);};
             ENDFOR
             break;

/* fill out a tool */
        case STACK_TOOL:
             if (IS_NULL(currt) == TRUE)
                {note(NULL, "Use tool %s to fill group %s\n",
		       sg, currg);

                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, BFLRG, "%s_%s", sg, var);
                    if (dbdef(cl, nvr) == TRUE)
                       {val = dbget(cl, TRUE, nvr);
                        set_var(cl, nvr, oper, val);};
                 ENDFOR}
             else
                {note(NULL, "Use tool %s to fill tool %s\n",
		       sg, currt);
                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, BFLRG, "%s_%s", sg, var);
                    if (dbdef(cl, nvr) == TRUE)
                       {val = dbget(cl, TRUE, nvr);
                        set_var(cl, var, oper, val);};
                 ENDFOR};
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_FEATURES - look through the features FT for any
 *                - that apply to platform NP
 *                - return the results in T
 * feature syntax:
 *     <features> := <feature> | <features> <feature>
 *     <feature>  := <general> | <specific>
 *     <specific> := <cfg-id>.<general> | <specific>.<general>
 *     <general>  := 's' | 't' | 'py' | 'mpi' | ...
 *     <cfg-id>   := 1 based index of platform for specific features
 */

static void parse_features(char *t, int nc, int np, char *ft)
   {int i, nt, i_this, has_other;
    char anp[10];
    char **lst, *tk;

    snprintf(anp, 10, "%d", np);
    lst = tokenize(ft, " .-", 0);
    nt  = lst_length(lst);

    t[0] = '\0';

    i_this    = -1;
    has_other = FALSE;
    for (i = 0; i < nt; i++)
        {tk = lst[i];
	 if (strcmp(anp, tk) == 0)
	    i_this = i;
         else if (is_number(tk, 1) == TRUE)
	    has_other = TRUE;};

/* if there are specifications for this platform */
    if (i_this != -1)
       {for (i = i_this+1; i < nt; i++)
	    {tk = lst[i];
	     if (is_number(tk, 1) == TRUE)
	        break;
	     else
	        vstrcat(t, nc, "-%s", tk);};}

/* if there are no specifications for a different platform */
    else if (has_other == FALSE)
       {for (i = 0; i < nt; i++)
	    vstrcat(t, nc, "-%s", lst[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_PLATFORM - process a platform command
 *             - syntax: platform <cfg> <PSY_InstRoot> [<args>*]
 */

static void do_platform(client *cl, char *oper, char *value)
   {int is;
    char t[BFLRG], cfg[BFSML], sid[BFSML];
    char *p, *sib, **spec;

    st.np++;

    is   = 0;
    spec = tokenize(value, " \t\n\r", 0);
    snprintf(sid, BFSML, "%s.%d", st.psy_id, st.np);

/* determine appropriate features */
    if (st.features[0] != '\0')
       {parse_features(t, BFLRG, st.np, st.features);
	snprintf(cfg, BFSML, "%s%s", oper, t);}
    else
       nstrncpy(cfg, BFSML, oper, -1);

    note(NULL, "\n");

/* assemble the config command line */
    snprintf(t, BFLRG, "./dsys config -plt %s", st.dir.root);

/* add options affecting all platforms */
    if (st.abs_deb == TRUE)
       vstrcat(t, BFLRG, " -g");
    if (st.abs_opt == TRUE)
       vstrcat(t, BFLRG, " -o");
    if (st.create_dirs == TRUE)
       vstrcat(t, BFLRG, " -c");

/* add alias */
    vstrcat(t, BFLRG, " -a %s", sid);

/* add PSY_InstRoot */
    sib = dbget(cl, TRUE, "PSY_InstRoot");
    if ((IS_NULL(sib) == FALSE) && (strcmp(sib, "none") != 0))
       vstrcat(t, BFLRG, " -i %s", spec[is++]);

/* add other options for this platform */
    for ( ; spec[is] != NULL; is++)
        vstrcat(t, BFLRG, " %s", spec[is]);

/* finish up with config file */
    if (strcmp(cfg, "none") != 0)
       vstrcat(t, BFLRG, " %s", cfg);

/* add this command to the list */
    st.pltfcmd = lst_push(st.pltfcmd, t);
    st.pltfcfg = lst_push(st.pltfcfg, cfg);

/* add this platform to the list */
    p = dbget(cl, FALSE, "PSY_Platforms");
    if (IS_NULL(p) == TRUE)
       dbset(cl, "PSY_Platforms", "%s(%s)", cfg, sid);
    else
       dbset(cl, "PSY_Platforms", "%s:%s(%s)", p, cfg, sid);

    free_strings(spec);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONFIG_PLATFORMS - run the config commands for additional platforms */

static void config_platforms(void)
   {int i, ok;
    char *cmd, *cfg;

    noted(NULL, "\n");

    for (i = 0; i < st.np; i++)
        {cmd = st.pltfcmd[i];
	 cfg = st.pltfcfg[i];

	 noted(NULL, "----------------------------------------------\n");

/* run the config for this platform */
	 ok = system(cmd);
	 if (ok != 0)
	    {noted(NULL, "Configuration of platform %s failed - exiting\n",
		    cfg);
	     exit(1);}
	 else
	    note(NULL, "Configuration of platform %s succeeded\n",
		  cfg);};

    if (st.np > 0)
       noted(NULL, "----------------------------------------------\n");

    free_strings(st.pltfcmd);
    free_strings(st.pltfcfg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_RUN_WORK - gather job specifications and execute them asynchronously
 *             - the jobs will add their results to the database directly
 *             - via perdb
 *             - syntax:
 *             - Run {
 *             -    <job>
 *             -     ...
 *             -    <job>
 *             - }
 */

static int do_run_work(client *cl, int il, char *oper, char *value)
   {int i, ok, nc, rv;
    char t[BFLRG], cmd[BFLRG];
    char *db;

    rv = 0;

    if (oper[0] != '{')
       fprintf(stderr, "Syntax error on line %d: '%s %s'\n",
	       il, oper, value);

    else if (st.async == TRUE)
       {cmd[0] = '\0';

	for (i = 0, ok = TRUE; ok == TRUE; i++)
	    {read_line(t, BFLRG);
             if (t[0] == '}')
	        ok = FALSE;
	     else
	        vstrcat(cmd, BFLRG, "%s @ ", echo(FALSE, t));};

	nc = strlen(cmd);
	cmd[nc-3] = '\0';

	CLOG(cl, 1, "pco: async %s", cmd);
	db = cgetenv(TRUE, "PERDB_PATH");
        rv = gexecs(db, cmd, NULL, NULL);}

    else
       {for (i = 0, ok = TRUE; ok == TRUE; i++)
	    {read_line(t, BFLRG);
             if (t[0] == '}')
	        ok = FALSE;
	     else
	        {CLOG(cl, 1, "pco: run-group %s", t);
	         rv |= system(echo(FALSE, t));};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_CONFIG - read the user configuration file */

static void read_config(client *cl, char *cfg, int quiet)
   {int il;
    char line[BFLRG], key[BFLRG], oper[BFLRG], value[BFLRG];
    char *path;

    separator(NULL);
    if (quiet == TRUE)
       {note(NULL, "\n");
	note(NULL, "Reading configuration file %s\n", cfg);}
    else
       {noted(NULL, "\n");
	noted(NULL, "Reading configuration file %s\n", cfg);};
    note(NULL, "\n");

    note(NULL, "\nStrict checking level is %s\n\n", dbget(cl, TRUE, "RF_STRICT"));
    separator(NULL);

    path = push_file(cfg, STACK_FILE);

/* toplevel loop over the input to define configuration parameters */
    for (il = 0; TRUE; il++)
        {if (dbdef(cl, "STOP") == TRUE)
	    {noted(NULL, " \n");
	     exit(1);};

	 read_line(line, BFLRG);

	 if (IS_NULL(line) == TRUE)
            continue;
	 else if (strcmp(line, "++end++") == 0)
            break;

	 parse_line(cl, line, key, oper, value, BFLRG);

/* handle platform directives */
	 if (strcmp(key, "platform") == 0)
            do_platform(cl, oper, value);

/* handle include directives */
	 else if (strcmp(key, "include") == 0)
	    {int n;
	     char ldepth[BFLRG];

	     n = st.fstck.n;
	     memset(ldepth, ' ', 3*n);
	     ldepth[3*n] = '\0';

	     CLOG(cl, 1, "pco: %s", line);
	     path = push_file(oper, STACK_FILE);
	     note(NULL, "\n");
	     noted(NULL, "%sincluding %s\n", ldepth, path);}

/* handle run directives */
	 else if (strcmp(key, "run") == 0)
	    {int n;
	     char ldepth[BFLRG];

	     n = st.fstck.n;
	     memset(ldepth, ' ', 3*n);
	     ldepth[3*n] = '\0';

	     CLOG(cl, 1, "pco: %s", line);
	     path = push_file(line+4, STACK_PROCESS);
	     note(NULL, "\n");
	     if (st.phase == PHASE_READ)
	        noted(NULL, "%srunning %s\n", ldepth, path);}

	 else if (strcmp(key, "PSY_InstRoot") == 0)
	    {if (st.installp == FALSE)
	        {dbset(cl, "PSY_InstRoot", value);
		 dbset(cl, "PSY_PubInc",   "%s/include", value);
		 dbset(cl, "PSY_PubLib",   "%s/lib", value);};}

	 else if (strcmp(key, "setenv") == 0)
	    {char *s;

	     if (strcmp(oper, "PATH") == 0)
                push_path(P_APPEND, epath, value);
	     s = echo(FALSE, value);
	     dbset(cl, oper, s);}

/* handle Note specifications */
	 else if (strcmp(key, "Note") == 0)
            printf("%s\n", trim(line+5, BOTH, "\""));

/* handle Run specifications */
	 else if (strcmp(key, "Run") == 0)
            do_run_work(cl, il, oper, value);

/* handle Tool specifications */
	 else if (strcmp(key, "Tool") == 0)
	    {note(NULL, "--- tool %s\n", oper);
	     nstrncpy(st.gstck.curr_tool, BFSML, oper, -1);
	     note(NULL, "Defining tool %s\n", oper);
	     push_struct(oper, st.def_tools, STACK_TOOL);}

/* handle Group specifications */
	 else if (strcmp(key, "Group") == 0)
	    {note(NULL, "--- group %s\n", oper);
	     nstrncpy(st.gstck.curr_grp, BFSML, oper, -1);
	     push_struct(oper, st.def_groups, STACK_GROUP);}

/* handle Use specifications */
	 else if (strcmp(key, "Use") == 0)
	    process_use(cl, value, oper);

/* handle struct close specifications */
	 else if (strcmp(key, "}") == 0)
            pop_struct(cl);

	 else if ((strcmp(oper, "=") == 0)  ||
		  (strcmp(oper, "+=") == 0) ||
		  (strcmp(oper, "=+") == 0) ||
		  (strcmp(oper, "-=") == 0) ||
		  (strcmp(oper, "=?") == 0) ||
		  (strcmp(oper, "?=") == 0))
	    set_var(cl, key, oper, value);

/* .c.i rule handler */
	 else if (strcmp(key, ".c.i:") == 0)
	    {parse_rule(st.rules.ccp, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .c.i rule:\n%s\n\n", st.rules.ccp);}

/* .c.o rule handler */
	 else if (strcmp(key, ".c.o:") == 0)
	    {parse_rule(st.rules.co, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .c.o rule:\n%s\n\n", st.rules.co);}

/* .c.a rule handler */
	 else if (strcmp(key, ".c.a:") == 0)
	    {parse_rule(st.rules.ca, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .c.a rule:\n%s\n\n", st.rules.ca);}

/* .f.o rule handler */
	 else if (strcmp(key, ".f.o:") == 0)
	    {parse_rule(st.rules.fo, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .f.o rule:\n%s\n\n", st.rules.fo);}

/* .f.a rule handler */
	 else if (strcmp(key, ".f.a:") == 0)
	    {parse_rule(st.rules.fa, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .f.a rule:\n%s\n\n", st.rules.fa);}

/* .l.o rule handler */
	 else if (strcmp(key, ".l.o:") == 0)
	    {parse_rule(st.rules.lo, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .l.o rule:\n%s\n\n", st.rules.lo);}

/* .l.a rule handler */
	 else if (strcmp(key, ".l.a:") == 0)
	    {parse_rule(st.rules.la, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .l.a rule:\n%s\n\n", st.rules.la);}

/* .l.c rule handler */
	 else if (strcmp(key, ".l.c:") == 0)
	    {parse_rule(st.rules.lc, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .l.c rule:\n%s\n\n", st.rules.lc);}

/* .y.o rule handler */
	 else if (strcmp(key, ".y.c:") == 0)
	    {parse_rule(st.rules.yo, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .y.o rule:\n%s\n\n", st.rules.yo);}

/* .y.a rule handler */
	 else if (strcmp(key, ".y.a:") == 0)
	    {parse_rule(st.rules.ya, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .y.a rule:\n%s\n\n", st.rules.ya);}

/* .y.c rule handler */
	 else if (strcmp(key, ".y.c:") == 0)
	    {parse_rule(st.rules.yc, BFLRG);
	     if (st.verbose == TRUE)
	        noted(NULL, "Redefining .y.c rule:\n%s\n\n", st.rules.yc);}

/* print unknown specifications during any non-read phase */
	 else
            printf("%s\n", line);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_DO_RUN - add an entry to the database */

static void add_do_run(FILE *fl, FILE *fp, char *lf)
   {int i;
    char **sa;

    sa = file_text(FALSE, lf);

    for (i = 0; sa[i] != NULL; i++)
        {if (strchr(sa[i], '#') == NULL)
 	    fprintf(fp, "%s\n", sa[i]);};

    free_strings(sa);

    fprintf(fp, "\n");
    fprintf(fp, "#-------------------------------------------------------------------------\n");

    note(fl, "Using %s\n", lf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_RUN_SPEC - return the path to S in LST */

static char *do_run_spec(char *lst, char *s)
   {char *file;
    static char t[BFLRG];

    file = NULL;
    if (file_exists(s) == TRUE)
       file = s;

    else
       {FOREACH(d, lst, " \t\n")
	   snprintf(t, BFLRG, "%s/do-run-%s", d, s);
	   if (file_exists(t) == TRUE)
	      {file = t;
	       break;};
	ENDFOR;};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DO_RUN_DB - write the do-run database */

static void write_do_run_db(client *cl, state *st)
   {int i, l, nm, ns, ok;
    char ldbg[BFLRG], lmpi[BFLRG], lcrs[BFLRG];
    char cfg[BFLRG], db[BFLRG];
    char s[BFLRG];
    char **cnd, **sa, *file, *exe, *p;
    FILE *fp, *fl;

    fl = open_file("w", "%s/log/write.do-run-db", st->dir.root);

    getcwd(s, BFLRG);

    note(fl, "\n");
    separator(fl);
    note(fl, "Command: pco/write_do_run_db\n");
    note(fl, "Current: %s\n", s);
    note(fl, "Date: %s\n", get_date());
    note(fl, "Manager directory: %s\n", dbget(cl, FALSE, "PSY_MngDir"));
    note(fl, "\n");

    p = cgetenv(TRUE, "PSY_CONFIG_PATH");
    if (IS_NULL(p) == TRUE)
       p = "local std";
    nstrncpy(cfg,  BFLRG, p, -1);

    nstrncpy(ldbg, BFLRG, dbget(cl, TRUE, "DO_RUN_DBG"), -1);
    nstrncpy(lmpi, BFLRG, dbget(cl, TRUE, "DO_RUN_MPI"), -1);
    nstrncpy(lcrs, BFLRG, dbget(cl, TRUE, "DO_RUN_CROSS"), -1);

    separator(fl);
    note(fl, "   PSY_CONFIG_PATH = %s\n", cfg);
    note(fl, "   DO_RUN_DBG      = %s\n", ldbg);
    note(fl, "   DO_RUN_MPI      = %s\n", lmpi);
    note(fl, "   DO_RUN_CROSS    = %s\n", lcrs);

/* initialize the database file */
    note(fl, "   Do-run signature database - do-run-db\n");

    snprintf(db, BFLRG, "%s/do-run-db", st->dir.etc);

/* see if file is properly specified via environment variable */
    p   = cgetenv(TRUE, "DB_RUN_SIGNATURE");
    nm  = 0;
    ok  = TRUE;
    ok &= (file_exists(p) == TRUE);
    ok &= (strcmp(p, db) != 0);

/* copy the do-run database */
    if (ok == TRUE)
       {note(fl, "      copying %s\n", p);
        copy(db, p);}

/* write the do-run database */
    else
       {note(fl, "      generating %s\n", db);

	fp = fopen_safe(db, "w");

	fprintf(fp, "#\n");
	fprintf(fp, "# DO-RUN-DB - execution signatures for do-run\n");
	fprintf(fp, "#\n");

	cnd = NULL;

/* check for debug specs */
	if ((IS_NULL(ldbg) == FALSE) && (strcmp(ldbg, "none") != 0))
	   {cnd = lst_push(cnd, ldbg);
	    p   = path_base(ldbg);
            note(fl, "default DBG %s\n\n", p);
	    fprintf(fp, "default DBG %s\n", p);}
	else
	   {cnd = lst_push(cnd, "gdb");
	    cnd = lst_push(cnd, "ts");
	    cnd = lst_push(cnd, "tv");
	    cnd = lst_push(cnd, "vg");
	    cnd = lst_push(cnd, "vgd");
	    cnd = lst_push(cnd, "prf");
	    cnd = lst_push(cnd, "zf");};

/* check for MPI specs */
	if ((IS_NULL(lmpi) == FALSE) && (strcmp(lmpi, "none") != 0))
	   {cnd = lst_push(cnd, lmpi);
	    p   = path_base(lmpi);
            note(fl, "default MPI %s\n\n", p);
	    fprintf(fp, "default MPI %s\n", p);}
	else
	   {cnd = lst_push(cnd, "poe");
	    cnd = lst_push(cnd, "srun");
	    cnd = lst_push(cnd, "dmpirun");
	    cnd = lst_push(cnd, "mpirun");};

/* check for cross compile specs */
	if ((IS_NULL(lcrs) == FALSE) && (strcmp(lcrs, "none") != 0))
	   {if ((IS_NULL(lmpi) == FALSE) && (strcmp(lcrs, lmpi) != 0))
	       {cnd = lst_push(cnd, lcrs);
		p   = path_base(lcrs);
		note(fl, "default CROSS %s\n\n", p);
		fprintf(fp, "default CROSS %s\n", p);}
	    else
	       note(fl, "default CROSS same as default MPI\n\n");}
	else
	   cnd = lst_push(cnd, "submit");

	ns = 0;

/* find the prolog */
	FOREACH(d, cfg, " \t\n")
	   snprintf(s, BFLRG, "%s/do-run-prolog", d);
	   if (file_exists(s) == TRUE)
              {add_do_run(fl, fp, s);
	       ns++;
	       break;};
        ENDFOR;

/* process the specs in CND */
	for (i = 0; cnd[i] != NULL; i++)
	    {note(fl, "Checking %s\n", cnd[i]);

	     file = do_run_spec(cfg, cnd[i]);
	     if (IS_NULL(file) == TRUE)
	        {nm++;
		 note(fl, "   missing do-run config = |%s|\n", file);}

	     else
	        {note(fl, "   using do-run config = |%s|\n", file);

		 exe = cwhich(path_base(cnd[i]));
		 note(fl, "   candidate for %s name = |%s|\n",
		      cnd[i], exe);
		 if (file_executable(exe) == TRUE)
		    {add_do_run(fl, fp, file);
		     ns++;}
		 else
		    {sa = file_text(FALSE, file);

		     for (l = 0; sa[l] != NULL; l++)
		         {if (strstr(sa[l], "Exe") != NULL)
			     {p   = strchr(sa[l], '=');
			      exe = trim(p+1, BOTH, " \t\n\r");
			      if (exe[0] != '/')
				 exe = cwhich(exe);
			      
			      note(fl, "   candidate for %s file = |%s|\n",
				   cnd[i], exe);
			      if (file_executable(exe) == TRUE)
				 {add_do_run(fl, fp, file);
				  ns++;};
			      break;};};

		     free_strings(sa);};};};

	lst_free(cnd);

	fprintf(fp, "\n");

	fclose_safe(fp);};

/* add the signature variable to the databases iff
 * there were specification - ns > 0 
 * and there were no missing specifications - nm == 0
 */
    if ((nm == 0) && (ns > 0))
       dbset(cl, "DB_RUN_SIGNATURE", db);

    separator(fl);

    fclose_safe(fl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_CONFIG_FILES - read the config files */

static void read_config_files(client *cl)
   {double dt;
    char ts[BFSML];

    separator(NULL);
    note(NULL, "Reading config files for %s on %s\n", st.psy_id, st.host);

    dt = wall_clock_time();

    push_file(st.cfgf, STACK_FILE);
    strcpy(st.cfgf, st.fstck.file[st.fstck.n-1].name);
    pop_file();

    if (file_exists("analyze/program-init") == TRUE)
       read_config(cl, "program-init", TRUE);

    read_config(cl, st.cfgf, FALSE);

    dt = wall_clock_time() - dt;
    note(NULL, "Completed - config files read (%s)\n",
	  time_string(ts, BFSML, TIME_HMS, dt));

    separator(NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ANALYZE_CONFIG - analyze the system configuration */

static void analyze_config(client *cl, char *base)
   {double dt;
    char ts[BFSML];

    st.phase = PHASE_ANALYZE;

    separator(NULL);
    noted(NULL, "Analyzing %s on %s\n", st.psy_id, st.host);
    note(NULL, "\n");

    dt = wall_clock_time();

    write_envf(cl, FALSE);
    
/* setup the environment for programs which analyze features */
    setup_analyze_env(cl, base);

    push_dir(st.dir.cfg);

/* read the file which does the analysis */
    if (file_exists("../analyze/program-analyze") == TRUE)
       read_config(cl, "program-analyze", TRUE);

    run(BOTH, "rm * > /dev/null 2>&1");
    pop_dir();

    dt = wall_clock_time() - dt;
    note(NULL, "Completed - analysis done (%s)\n",
	  time_string(ts, BFSML, TIME_HMS, dt));
    separator(NULL);

    noted(NULL, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SUMMARIZE_CONFIG - make a summary report of the configuration */

static void summarize_config(client *cl)
   {

    if (file_executable("analyze/summary") == TRUE)
       printf("%s\n", run(BOTH, "analyze/summary"));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINISH_CONFIG - complete the configuration files */

static void finish_config(client *cl, char *base)
   {double dt;
    char ts[BFSML];

    st.phase = PHASE_WRITE;

/* these are finally determined by now and it is safe to define them */
    snprintf(st.dir.inc, BFLRG, "%s/include", st.dir.root);
    snprintf(st.dir.etc, BFLRG, "%s/etc",     st.dir.root);
    snprintf(st.dir.lib, BFLRG, "%s/lib",     st.dir.root);
    snprintf(st.dir.bin, BFLRG, "%s/bin",     st.dir.root);

    separator(NULL);
    noted(NULL, "Writing system dependent files for %s\n", st.psy_id);
    note(NULL, "\n");

    dt = wall_clock_time();

    setup_output_env(cl, base);

/* write the configured files for PACT */
    write_envf(cl, TRUE);

    LOG_OFF;

    if (file_exists("analyze/program-fin") == TRUE)
       read_config(cl, "program-fin", TRUE);

    LOG_ON;

    dt = wall_clock_time() - dt;
    note(NULL, "Completed - files written (%s)\n",
	  time_string(ts, BFSML, TIME_HMS, dt));
    separator(NULL);

    note(NULL, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAUNCH_PERDB - look for -f arg and if found launch perdb first of all */

int launch_perdb(int c, char **v)
   {int i, l, n, ok, rv;
    char t[BFLRG];
    char *db;
    static char *sfx[] = { "db", "log", "pid" };

    ok = FALSE;

    st.launched = FALSE;

    if (cdefenv("PERDB_PATH") == FALSE)
       {n = sizeof(sfx)/sizeof(char *);

	for (i = 1; i < c; i++)
	    {if (strcmp(v[i], "-f") == 0)
	        {db = v[++i];
		 csetenv("PERDB_PATH", db);

		 for (l = 0; l < n; l++)
		     {snprintf(t, BFLRG, "%s.%s", db, sfx[l]);
		      unlink_safe(t);};

		 rv = execute(FALSE, "perdb -f %s -l -c -s", db);
		 ok = ((rv & 0xff) == 0);
		 break;};};

/* give the server some time to initialize */
	nsleep(100);

	st.launched = ok;};

    return(ok);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* KILL_PERDB - shutdown perdb */

int kill_perdb(void)
   {int ok;
    char *db, *bf;

    ok = TRUE;
    if (st.launched == TRUE)
       {db = cgetenv(FALSE, "PERDB_PATH");
	if (IS_NULL(db) == FALSE)
	   {bf = run(TRUE, "perdb -f %s -l quit:", db);
	    ok = (bf != NULL);};};

    return(ok);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGDONE - handle signals meant to end session */

static void sigdone(int sig)
   {

    if (st.have_db == TRUE)
       kill_perdb();

    exit(sig);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DENV - print the current environment */

void denv(void)
   {int i, n;
    char **ta;

    ta = cenv(TRUE, NULL);
    if (ta != NULL)
       {n = lst_length(ta);
	for (i = 0; i < n; i++)
	    printf("> %s\n", ta[i]);

	free_strings(ta);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - give help message */

static void help(void)
   {

    printf("\n");
    printf("Usage: pco [-a] [-c] [-db <db>] [-f <path>] [-F] [-ft <features>] [-g] [-i <directory>] [-l] [-o] [-p] [-s <sysid>] [-v] | <cfg>\n");
    printf("\n");
    printf("             -a      do NOT perform PACT-ANALYZE step\n");
    printf("             -c      create missing directories for -i option\n");
    printf("             -db     configure using database <db>\n");
    printf("             -f      path to database\n");
    printf("             -F      do builds in /tmp for speed\n");
    printf("             -ft     hyphen delimited feature list\n");
    printf("             -g      build debuggable version\n");
    printf("             -i      base installation directory (default /usr/local)\n");
    printf("             -l      append to the log file\n");
    printf("             -n      do an auto-configuration\n");
    printf("             -o      build optimized version\n");
    printf("             -p      build profiling version\n");
    printf("             -s      explicit system id\n");
    printf("             -v      verbose on (diagnostic)\n");
    printf("             <cfg>   the config file for the installation\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v, char **env)
   {int i, append, ok;
    char base[BFLRG], ib[BFLRG], d[BFLRG];
    char *root, *strct;
    client *cl;

    if (c <= 1)
       {help();
	return(1);};

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    nsigaction(NULL, SIGSEGV, sigdone, SA_RESTART, -1);
    nsigaction(NULL, SIGBUS,  sigdone, SA_RESTART, -1);
    nsigaction(NULL, SIGFPE,  sigdone, SA_RESTART, -1);
    nsigaction(NULL, SIGTERM, sigdone, SA_RESTART, -1);
    nsigaction(NULL, SIGINT,  sigdone, SA_RESTART, -1);

    ok = reset_env(c, v);
    if (ok == -1)
       {printf("Could not clear environment - exiting\n");
	return(1);};

/* locate the tools needed for subshells */
    build_path(NULL,
	       "sed", "grep", "awk", "sort",
	       "ls", "cp", "rm", "mv",
	       "find", "chmod", "cat",
	       "env", "mkdir", "nm", "perdb",
	       NULL);

/* multi-platform configs will go idle for the child configs
 * so give it more time
 */
    csetenv("PERDB_IDLE_TIMEOUT", "600");

    root = cgetenv(TRUE, "PERDB_PATH");
    cl   = make_client(CLIENT, DB_PORT, FALSE, root, cl_logger, NULL);

    st.features[0] = '\0';
    st.have_db = launch_perdb(c, v);

    dbset(cl, "PATH", cgetenv(TRUE, "PATH"));

/* NOTE: because of OSX's nefarious automounter we have to get the current
 * directory this way (rather than via the getcwd library call) so that
 * we get the same value that shell scripts get in other parts of the
 * config process - and consistency is essential
 */
    nstrncpy(st.dir.mng, BFLRG, run(BOTH, "pwd"), -1);

    nstrncpy(base, BFLRG, path_head(st.dir.mng), -1);
    strcpy(st.cfgf, "DEFAULT");

    nstrncpy(ib, BFLRG, "none", -1);
    strct  = "0";
    append = FALSE;

    st.na   = c - 1;
    st.args = v + 1;
    st.env  = env;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-strict") == 0)
	    strct = v[++i];

         else if (strcmp(v[i], "-as") == 0)
            st.async = TRUE;

         else if (strcmp(v[i], "-db") == 0)
	    {nstrncpy(d, BFLRG, "db:", -1);
	     full_path(d+3, BFLRG-3, NULL, v[++i]);
             if (file_exists(d+3) == FALSE)
	        {noted(NULL, "No such database '%s' - exiting\n\n", d+3);
		 if (st.have_db == TRUE)
		    kill_perdb();
		 return(1);};
	     st.db = d;
	     dbset(cl, "RF_CONFIG_METHOD", "database");}
 
/* this was handled in reset_env */
         else if (strcmp(v[i], "-env") == 0)
            i++;
 
         else if (strcmp(v[i], "-ft") == 0)
	    {nstrncpy(st.features, BFSML, v[++i], -1);
	     dbset(cl, "RF_FEATURES", st.features);}

	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'a':
		       st.analyzep = FALSE;
                       break;

                 case 'c':
                      st.create_dirs = TRUE;
                      break;
 
/* this was handled in launch_perdb */
                 case 'f':
		      i++;
                      break;
 
                 case 'F':
                      st.tmp_dirp = FALSE;
                      break;
 
                 case 'g':
                      st.abs_deb = TRUE;
                      break;
 
                 case 'i':
		      nstrncpy(ib, BFLRG, v[++i], -1);
                      break;
 
                 case 'l':
                      append    = TRUE;
                      svs.debug = TRUE;
                      break;
 
                 case 'o':
                      st.abs_opt = TRUE;
                      break;
 
                 case 'p':
                      st.profilep = TRUE;
                      break;
 
                 case 's':
                      nstrncpy(st.psy_id, BFLRG, v[++i], -1);
                      break;
 
                 case 'v':
                      st.verbose = TRUE;
                      break;};}
         else if (v[i][0] == '+')
            {switch (v[i][1])
                {case 'F':
                      st.tmp_dirp = TRUE;
                      break;};}

         else
	    {nstrncpy(st.cfgf, BFLRG, v[i], -1);
	     dbset(cl, "RF_CONFIG_METHOD", "file");};};

    set_inst_base(cl, ib);

    init_pco_session(cl, base, append);

/* make config directory */
    snprintf(st.dir.cfg, BFLRG, "cfg-%s", st.psy_id);
    run(BOTH, "rm -rf %s", st.dir.cfg);
    run(BOTH, "mkdir %s", st.dir.cfg);

    if (st.db == NULL)
       {dbset(cl, "RF_STRICT", strct);

	read_config_files(cl);

        write_do_run_db(cl, &st);

	ok = check_cross(cl);
	if (ok == FALSE)
	   {free_client(cl);

	    if (st.have_db == TRUE)
	       kill_perdb();

	    return(2);};

	pco_save_db(cl, "inp");

	noted(NULL, "\n");

	check_dir(cl);

	if (st.analyzep == TRUE)
	   analyze_config(cl, base);}

    else
       {pco_load_db(cl, st.db);

        write_do_run_db(cl, &st);

/* order matters crucially here */
        env_subst(cl, "PSY_Base",         base);
        env_subst(cl, "PSY_InstRoot",     ib);
        env_subst(cl, "PSY_Root",         st.dir.root);
        env_subst(cl, "PSY_ID",           st.psy_id);
        env_subst(cl, "RF_CONFIG_METHOD", "database");

	snprintf(st.dir.cfg, BFLRG, "cfg-%s", st.psy_id);
	nstrncpy(st.cfgf,    BFLRG, cgetenv(FALSE, "PSY_Cfg"), -1);

/* reset the rules */
	snprintf(st.rules.ccp, BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_CCP"));
	snprintf(st.rules.co,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_CCObj"));
	snprintf(st.rules.ca,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_CCArc"));
	snprintf(st.rules.lo,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_LexObj"));
	snprintf(st.rules.la,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_LexArc"));
	snprintf(st.rules.lc,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_LexC"));
	snprintf(st.rules.yo,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_YaccObj"));
	snprintf(st.rules.ya,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_YaccArc"));
	snprintf(st.rules.yc,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_YaccC"));
	snprintf(st.rules.fo,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_FCObj"));
	snprintf(st.rules.fa,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_FCArc"));
	snprintf(st.rules.th,  BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_TemplH"));

	snprintf(st.rules.co_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_CCObj_BP"));
	snprintf(st.rules.ca_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_CCArc_BP"));
	snprintf(st.rules.lo_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_LexObj_BP"));
	snprintf(st.rules.la_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_LexArc_BP"));
	snprintf(st.rules.yo_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_YaccObj_BP"));
	snprintf(st.rules.ya_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "IRules_YaccArc_BP"));

	check_dir(cl);};

    summarize_config(cl);

    pco_save_db(cl, NULL);

    finish_config(cl, base);

/* configure added platforms */
    config_platforms();

    separator(NULL);
    run(BOTH, "rm -rf %s", st.dir.cfg);

    LOG_OFF;

    free_client(cl);

    if (st.have_db == TRUE)
       kill_perdb();

    log_safe("dump", 0, NULL, NULL);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

