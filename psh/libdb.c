/*
 * LIBDB.C - persistent database routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBDB

# define LIBDB

# include "libsrv.c"
# include "libhash.c"

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

enum e_db_constants
  {DB_PORT = 15000};

typedef enum e_db_constants db_constants;

typedef struct s_database database;
typedef struct s_vardes vardes;

struct s_database
   {char *file;                 /* name of the file image of the database */
    char *flog;                 /* name of the log file */
    hashtab *tab;
    client *cl;};

struct s_vardes
   {char *fmt;
    database *db;
    char **vars;
    FILE *fp;};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

/*--------------------------------------------------------------------------*/

/* NAME_DB - derive the name of the database from ROOT */

char *name_db(char *root)
   {static char fname[BFLRG];

    snprintf(fname, BFLRG, "%s.db", root);

    return(fname);}

/*--------------------------------------------------------------------------*/

/*                             CLIENT SIDE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _DB_SRV_LAUNCH - launch a DB server
 *                - return TRUE iff successful
 */

static int _db_srv_launch(client *cl)
   {int i, st, pid, rv, na;
    char s[BFLRG];
    char *root, *fcon;

    rv = FALSE;

    if (cl != NULL)
       {root = cl->root;
	fcon = cl->fcon;

	st   = 0;
	s[0] = '\0';

/* wait until we have a server going - or 10 seconds*/
        na = 100;
	for (i = 0; (file_exists(fcon) == FALSE) && (i < na); i++)
	    {if (i == 1)
		{if (cl->auth == TRUE)
		    snprintf(s, BFLRG, "perdb -a -f %s -s -l", root);
		 else
		    snprintf(s, BFLRG, "perdb -f %s -s -l", root);
		 st = system(s);
		 st = WEXITSTATUS(st);
		 CLOG(cl, 1, "launch |%s| (%d)", s, st);}
	     else
	        nsleep(100);};

	if (i < na)
	   {pid = -1;

/* check the connection file info */
	    pid = get_conn_client(cl);

/* GOTCHA: if it is not running - clean up and restart */
	    st = is_running(pid);

	    cl->scon->pid = pid;

	    rv = TRUE;

	    CLOG(cl, 1, "server pid %d (%d)", pid, i);}
	else
	    CLOG(cl, 1, "server launch failed");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _DB_CLNT_EX - do a transaction from the client side of the database */

char **_db_clnt_ex(client *cl, int init, char *req)
   {int ok;
    char **p, *root;

    if (cl == NULL)
       {root = cgetenv(TRUE, "PERDB_PATH");
	cl   = make_client(CLIENT, DB_PORT, FALSE,
			   root, cl_logger, NULL);};

    ok = TRUE;
    p  = NULL;

/* make sure that there is a server running */
    if (init == TRUE)
       ok = _db_srv_launch(cl);

    if (ok == TRUE)
       p = client_ex(cl, req);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBSET - set the database variable VAR to VAL (counterpart to csetenv) */

int dbset(client *cl, char *var, char *fmt, ...)
   {int i, err, nc, nr, ok;
    char val[BFLRG];
    char *t, **ta;
    static char *rej[] = { "PWD", "PERDB_PATH" };

    err = 0;

/* variables in REJ should never be in the database itself */
    ok = TRUE;
    nr = sizeof(rej)/sizeof(char *);
    for (i = 0; (i < nr) && (ok == TRUE); i++)
        ok = (strcmp(var, rej[i]) != 0);

    if (ok == TRUE)
       {VA_START(fmt);
	VSNPRINTF(val, BFLRG, fmt);
	VA_END;

	nc  = strlen(var) + strlen(val) + 2;
	t   = malloc(nc);
	if (t != NULL)
	   {snprintf(t, nc, "%s=%s", var, val);

	    ta  = _db_clnt_ex(cl, TRUE, t);
	    err = (ta != NULL);

/* GOTCHA: temporarily also add it to the environment
 * this can go when utilities using this are completely converted
 * away from using the environment
 */
	    err = putenv(t);

	    note(NULL, "setenv %s %s\n", var, val);

	    lst_free(ta);};};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBGET - get the value of database variable VAR
 *       - if LIT is TRUE return as is otherwise strip off
 *       - surrounding quotes
 *       - (counterpart to cgetenv)
 */

char *dbget(client *cl, int lit, char *fmt, ...)
   {char var[BFLRG];
    char *t, **ta;

    VA_START(fmt);
    VSNPRINTF(var, BFLRG, fmt);
    VA_END;

    ta = _db_clnt_ex(cl, TRUE, var);
    if (ta == NULL)
       t = cnoval();

    else
       {if (lit == FALSE)
	   t = strip_quote(ta[0]);

        else
	   t = ta[0];

	ta[0] = NULL;};

    lst_free(ta);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBDEF - return TRUE iff the database variable VAR is defined
 *       - (counterpart to cdefenv)
 */

int dbdef(client *cl, char *fmt, ...)
   {int rv;
    char var[BFLRG];
    char **ta;

    VA_START(fmt);
    VSNPRINTF(var, BFLRG, fmt);
    VA_END;

    vstrcat(var, BFLRG, " ?");

    ta = _db_clnt_ex(cl, TRUE, var);
    rv = ((ta != NULL) && (IS_NULL(ta[0]) == FALSE) &&
	  (strcmp(ta[0], "defined{TRUE}") == 0));

    lst_free(ta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBCMP - return a strcmp style value for the
 *       - comparison of the database variable VAR value and VAL
 *       - (counterpart to cmpenv)
 */

int dbcmp(client *cl, char *var, char *val)
   {int rv;
    char **ta;

    ta = _db_clnt_ex(cl, TRUE, var);
    if (ta != NULL)
       rv = strcmp(ta[0], val);
    else
       rv = -2;

    lst_free(ta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBCMD - send a command to the database daemon
 *       - one of: quit, save, load, reset
 */

int dbcmd(client *cl, char *cmd)
   {int rv;
    char **ta;

    ta = _db_clnt_ex(cl, TRUE, cmd);
    rv = (ta != NULL);

    lst_free(ta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBINITV - set the database variable VAR to VAL
 *         - only if it is undefined
 *         - (counterpart to cinitenv)
 */

int dbinitv(client *cl, char *var, char *fmt, ...)
   {int err;
    char s[BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    err = 0;
    if (dbdef(cl, var) == FALSE)
       err = dbset(cl, var, s);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBRESTORE - have the server load the specified database DNAME
 *           - and set all the variables in the environment
 *           - of the current process
 */

int dbrestore(client *cl, char *dname)
   {int i, rv, ok;
    char t[BFLRG];
    char **ta, *s, *vl;

    rv = TRUE;

/* load the database */
    if (dname == NULL)
       nstrncpy(t, BFLRG, "load:", -1);
    else
       snprintf(t, BFLRG, "load:%s", dname);

    ok = dbcmd(cl, t);
    ASSERT(ok == 0);

/* the server has the database - now we need it
 * GOTCHA: this can go when utilities using this are
 * completely converted away from using the environment
 */
    ta = _db_clnt_ex(cl, FALSE, "save:");
    if (ta != NULL)
       {for (i = 0; ta[i] != NULL; i++)
	    {s = ta[i];
	     if (strncmp(s, "saved", 5) == 0)
	        break;
	     else
	        {vl = strchr(s, '=');
		 if (vl != NULL)
		    {*vl++ = '\0';
		     vl = strip_quote(vl);
		     csetenv(s, vl);};};};
	lst_free(ta);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             SERVER SIDE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _GET_VAR - return the value of variable VAR */

static char *_get_var(database *db, char *var)
   {char *val;
    hashtab *tab;

    tab = db->tab;
    val = hash_def_lookup(tab, var);
    if (val == NULL)
       val = cnoval();

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SET_VAR - return the value of variable VAR */

static void _set_var(database *db, char *var, char *val)
   {hashtab *tab;

    tab = db->tab;
    hash_install(tab, var, STRSAVE(val), "char *", TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _DEL_VAR - remove the variable VAR */

static void _del_var(database *db, char *var)
   {hashtab *tab;

    tab = db->tab;
    hash_remove(tab, var);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RESET_DB - reset DB */

void reset_db(database *db)
   {client *cl;

    cl = db->cl;

    hash_clear(db->tab);

    CLOG(cl, 1, "reset");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUT_DB - add VAR to DB */

char *put_db(database *db, char *var, char *val)
   {client *cl;

    if ((db != NULL) && (var != NULL))
       {cl = db->cl;
	if (val == NULL)
	   val = cnoval();

	_set_var(db, var, val);

	CLOG(cl, 1, "put |%s|=|%s|", var, val);};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DELETE_DB - remove VAR from DB */

void delete_db(database *db, char *var)
   {client *cl;

    if ((db != NULL) && (var != NULL))
       {cl = db->cl;

	_del_var(db, var);

	CLOG(cl, 1, "del |%s|", var);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PUSH_COL - push a character collection delimited by CB,CE onto D */

static void _push_col(char **pd, char **ps, int cb, int ce)
   {int nc;
    char dlm[2];
    char *s, *d;

    s = *ps;
    d = *pd;

    *d++ = cb;

    dlm[0] = ce;
    dlm[1] = '\0';
    nc = strcpy_tok(d, -1, s, -1, NULL, dlm, ADD_DELIMITER);

    *ps = s + nc;
    *pd = d + strlen(d);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MEM_NAME - return TRUE if M is the last token of T */

static int _mem_name(char *t, char *pm)
   {int ib, ie, nc, rv;

    nc = strlen(t);
    for (ie = nc-1; ie >= 0; ie--)
        {if (strchr(" \t=", t[ie]) == NULL)
	    break;};

    for (ib = ie; ib >= 0; ib--)
        {if (strchr(" \t=", t[ib]) != NULL)
	    break;};

    ie++;
    ib++;

/* divide T into prior member value and current member name */
    if (ib > 0)
       t[ib-1] = '\0';

    nc = ie - ib;
    rv = strncmp(pm, t+ib, nc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _IS_STRUCT - return TRUE if S matches ["'](* */

static int _is_struct(char **ps, char *s)
   {int c, rv;

    c = *s;
    if ((c == '\'') || (c == '"'))
       s++;

    if (*s == '(')
       {rv  = TRUE;
        *ps = s + 1;}
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_DB - get VAR from DB */

char *get_db(database *db, char *var)
   {int c, st, ok;
    char s[BFLRG];
    char *val, *pk, *pm, *ps, *pt;
    client *cl;
    static char t[BFLRG];

    val = NULL;

    if (db != NULL)
       {cl = db->cl;

/* resolve var of form 'a.b.c' */
	nstrncpy(s, BFLRG, var, -1);
	for (ps = s; TRUE; ps = NULL)
	    {pm = strtok(ps, ".");

/* we are at the end and val has the answer */
	     if (pm == NULL)
	        break;

/* get the entire structure */
	     else if (val == NULL)
	        val = _get_var(db, pm);

/* find the member named by pt */
	     else if (_is_struct(&pk, val) == TRUE)
	        {st = FALSE;
		 pt = t;
		 ok = TRUE;
		 for (c = *pk++;
		      (c != '\0') && (c != ')') && (ok == TRUE);
		      c = *pk++)
		     {switch (c)
			 {case '"' :
			  case '\'' :
			       _push_col(&pt, &pk, c, c);
			       pk++;
			       break;
			  case '(' :
                               _push_col(&pt, &pk, c, ')');
			       pk++;
			       break;
			  case '=' :
			       *pt++ = '\0';
			       if (st == TRUE)
				  {_mem_name(t, pm);
				   ok = FALSE;
				   break;}
			       else if (_mem_name(t, pm) == 0)
				  {st  = TRUE;
				   val = t;};
			       pt = t;
			       break;
			 default :
                               *pt++ = c;
			       break;};};};};

	CLOG(cl, 1, "get |%s|=|%s|", var, val);};

    return(val);}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _SHOW_VAR - show variable VR value */

static int _show_var(database *db, FILE *fp, const char *fmt,
		     char *vr, char **vars)
   {int l, rv;
    char s[BFLRG], t[BFLRG];
    char *vl;

    rv = FALSE;

    if (vars == NULL)
       vl = _get_var(db, vr);
    else
       {vl = NULL;
	for (l = 0; vars[l] != NULL; l++)
	    {if (strcmp(vars[l], vr) == 0)
	        {vl = _get_var(db, vr);
		 break;};};};

    if (vl != NULL)
       {if ((vl[0] != '"') && (strpbrk(vl, " \t") != NULL))
	   {snprintf(t, BFLRG, "\"%s\"", vl);
	    vl = t;};

	if (fmt == NULL)
	   snprintf(s, BFLRG, "%s=%s", vr, vl);
	else
	   snprintf(s, BFLRG, fmt, vr, vl);

/* write to the communicator if FP is NULL */
	if (fp == NULL)
	   comm_write(db->cl, s, 0, 10);
	else
	   fprintf(fp, "%s\n", s);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SAVE_VAR - save variable VR from HP to FP */

static int _save_var(hashen *hp, void *a)
   {int rv;
    char *fmt, *vr, **vars;
    FILE *fp;
    database *db;
    vardes *pv;

    pv = (vardes *) a;

    db   = pv->db;
    fmt  = pv->fmt;
    fp   = pv->fp;
    vars = pv->vars;

    vr = hp->name;
    rv = _show_var(db, fp, fmt, vr, vars);

    return(rv);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _EMIT_VAR - show variable VR value */

static int _emit_var(database *db, FILE *fp, const char *fmt,
		     char *vr, char **vars)
   {int l, ok;
    char s[BFLRG];
    char *vl;

    vl    = strchr(vr, '=');
    *vl++ = '\0';

/* check whether VR is one of VARS */
    if (vars != NULL)
       {ok = FALSE;
	for (l = 0; vars[l] != NULL; l++)
	    {if ((strcmp(vars[l], vr) == 0) ||
		 (fnmatch(vars[l], vr, 0) == 0))
	        {ok = TRUE;
		 break;};};}
    else
       ok = TRUE;

    if (ok == TRUE)
       {if (fmt == NULL)
	   snprintf(s, BFLRG, "%s=%s", vr, vl);
        else
	   snprintf(s, BFLRG, fmt, vr, vl);

/* write to the communicator if FP is NULL */
	if (fp == NULL)
	   comm_write(db->cl, s, 0, 10);
	else
	   fprintf(fp, "%s\n", s);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SAVE_DB - save variable VAR to FP */

int save_db(database *db, char **vars, FILE *fp, const char *fmt)
   {int rv;

    rv = FALSE;

    if (db != NULL)
       {int is, ns;
	char **sa;

	sa = hash_dump(db->tab, NULL, NULL, 3);
	ns = lst_length(sa);

	for (is = 0; is < ns; is++)
	    _emit_var(db, fp, fmt, sa[is], vars);

	free_strings(sa);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_MULTI_LINE - return the next item from an array of strings SA
 *                - starting at array element I
 *                - the items are <key><dlm><value> pairs
 *                - but <value> may span several elements of SA
 *                - return I through PI
 *                - DLM permits processing of forms such as
 *                - <key>=<value> or <key>:<value>
 *                - if QU is TRUE watch for double quotes
 */

enum e_item_state
   {NONE, TOKEN, DONE};
typedef enum e_item_state item_state;

char *get_multi_line(char **sa, int ni, int *pi, char *dlm, int qu)
   {int i, c;
    item_state st;
    char *p, *s, *t, *rv;
    static char v[BFLRG];

    rv = NULL;
    if ((sa != NULL) && (pi != NULL))
       {v[0] = '\0';
	for (i = *pi, st = NONE; (0 <= i) && (i < ni) && (st != DONE); )
	    {s = sa[i++];
	     if (s != NULL)
	        {p = strstr(s, dlm);
		 if (p != NULL)

/* look for white space or characters which are illegal in identifiers */
		    {c  = *p;
		     *p = '\0';
		     t  = strpbrk(s, " \t~!@#$%^&?`|;:.,+-*/=(){}[]<>\'\"\\");
		     *p = c;

/* if we have a valid identifier followed by DLM we have
 * a string of the form <var><dlm>....
 * this is the start of an item
 */
		     if (t == NULL)
		        {if (st == TOKEN)
			    {i--;
			     rv = v;
			     st = DONE;}
			 else
			    st = TOKEN;};};

		 if (st != DONE)
		    vstrcat(v, BFLRG, "%s\n", s);

	         if (i >= ni)
		    {rv = v;
		     st = DONE;};};};

	*pi = i;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOAD_DB - load the DB into memory from FP */

void load_db(database *db, char *vr, FILE *fp)
   {char *var, *val, *p;

    if ((db != NULL) && (fp != NULL))
       {int i, nv;
	char **sa;

	sa = file_strings(fp);
	if (sa != NULL)
	   {nv = lst_length(sa);
	    for (i = 0; i < nv; )
	        {p = get_multi_line(sa, nv, &i, "=", TRUE);
		 if (p != NULL)
		    {key_val(&var, &val, p, "=\n");
		     if ((vr == NULL) || (strcmp(vr, var) == 0))
		        val = put_db(db, var, val);};};

	    free_strings(sa);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_DB - initialize the DB */

static database *make_db(client *cl, int dbg, int auth)
   {char *fname, *flog, *root, *t;
    database *db;

    db = NULL;

    if (cl != NULL)
       {root = cl->root;

	t = name_log(root);
	flog = (t == NULL) ? NULL : STRSAVE(t);

	t = name_db(root);
	fname = (t == NULL) ? NULL : STRSAVE(t);

	db = MAKE(database);
	if (db != NULL)
	   {db->file  = fname;
	    db->flog  = flog;
	    db->tab   = make_hash_table(-1);
	    db->cl    = cl;}
	else
	   {FREE(flog);
	    FREE(fname);};};

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_DB - release the DB */

void free_db(database *db)
   {

    if (db != NULL)
       {hash_free(db->tab);
	free_client(db->cl);
	FREE(db->file);
	FREE(db->flog);
	FREE(db);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_CREATE - initialize the DB */

database *db_srv_create(client *cl, int dbg, int auth)
   {database *db;

    db = make_db(cl, dbg, auth);
    if ((db != NULL) && (db->file != NULL))
       unlink_safe(db->file);

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_LOAD - load the DB into memory */

database *db_srv_load(client *cl, int dbg, int auth)
   {database *db;
    FILE *fp;

    db = make_db(cl, dbg, auth);
    if ((db != NULL) && (db->file != NULL))
       {fp = fopen_safe(db->file, "r");
	if (fp != NULL)
	   {load_db(db, NULL, fp);
	    fclose_safe(fp);};

	CLOG(cl, 1, "load %d |%s|", db->tab->ne, db->file);};

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _IS_SRV_RUNNING - is there is a running server
 *                 - return value is:
 *                 -   0 if there is no running server
 *                 -   1 if the running server is the current process
 *                 -  -1 if the running server is not the current process
 */

int _is_srv_running(client *cl)
   {int rv, pid, w, st;

    rv = 0;

    if (cl->fcon != NULL)
       {int i;
	char **sa;

	CLOG(cl, 1, "_is_srv_running: 1 (%d) %s",
	     file_exists(cl->fcon), cl->fcon);

	sa = file_text(FALSE, cl->fcon);
	if (sa != NULL)
	  {for (i = 0; sa[i] != NULL; i++)
	       CLOG(cl, 1, "%d: %s", i+1, sa[i]);

	   free_strings(sa);};};

/* if a server is already running there will be a PID file */
    pid = get_conn_client(cl);

/* check for a server in this process */
    if (pid == getpid())
       {CLOG(cl, 1, "running server on current process %d (%d)", pid, rv);
	rv = 1;}

/* check for a server in a different process */
    else if (pid != -1)
       {st = waitpid(pid, &w, WNOHANG);
	if (st == -1)
	   {CLOG(cl, 1, "running server on process %d (%d)", pid, rv);
	    rv = -1;}
	else
	   CLOG(cl, 1, "no running server (%d)", rv);}

    else
       CLOG(cl, 1, "running server - no (%d)", rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_OPEN - open the database
 *             - return TRUE iff successful
 */

int db_srv_open(client *cl, int init, int dbg, int auth)
   {int rv, st, pid;
    database *db;

    db = NULL;
    rv = FALSE;
    st = _is_srv_running(cl);
    if (st != -1)
       {if (init == TRUE)
	   db = db_srv_create(cl, dbg, auth);
        else
	   db = db_srv_load(cl, dbg, auth);

	if (db != NULL)
	   {pid   = getpid();
	    cl->a = db;

/* no server is running */
	    if (st == 0)
	       rv = open_server(cl, SERVER, cl->auth);

/* another server is running */
	    else
	       {free_db(db);
		db = NULL;};

	    cl->scon->pid = pid;};};

    rv = (db != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_CLOSE - close the database */

void db_srv_close(database *db)
   {

    if (db != NULL)
       {close_sock(db->cl);

	unlink_safe(db->cl->fcon);

	free_db(db);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_SAVE - save the DB to disk */

int db_srv_save(int fd, database *db)
   {int rv;
    client *cl;
    FILE *fp;

    rv = FALSE;

    if (db != NULL)
       {cl = db->cl;

	CLOG(cl, 1, "save %d |%s|", db->tab->ne, db->file);

	fp = fopen_safe(db->file, "w");
	if (fp != NULL)
	   {save_db(db, NULL, fp, NULL);

	    fclose_safe(fp);

	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_RESTART - restart the server */

int db_srv_restart(database *db)
   {int rv;
    client *cl;

    rv = TRUE;

    if (db != NULL)
       {cl = db->cl;

	close_sock(cl);
	db_srv_save(-1, db);

	CLOG(cl, 1, "restart");

	rv = open_server(cl, SERVER, cl->auth);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

# endif
#endif
