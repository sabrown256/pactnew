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

#define LIBDB

#include "libsrv.c"
#include "libhash.c"

/* #define NEWWAY */

#define EOM     "++ok++"

typedef struct s_database database;
typedef struct s_vardes vardes;
typedef struct s_db_session db_session;

struct s_database
   {int debug;
    int auth;
    int ioc;                    /* CLIENT or SERVER */
    char *root;                 /* root path name of database */
    char *file;                 /* name of the file image of the database */
    char *flog;                 /* name of the log file */
    char *fcon;                 /* name of the connection file */
    hashtab *tab;
    client *cl;};

struct s_db_session
   {svr_session svr;
    int literal;
    char root[MAXLINE];};

extern char
 *name_log(char *root);

struct s_vardes
   {char *fmt;
    client *cl;
    database *db;
    char **vars;
    FILE *fp;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NAME_LOG - make conventional LOG name from ROOT
 *          - .../foo -> .../foo.log
 */

char *name_log(char *root)
   {char *p;
    static char log[MAXLINE];

    p = NULL;

    if (root == NULL)
       root = cgetenv(TRUE, "PERDB_PATH");

    if (root != NULL)
       {snprintf(log, MAXLINE, "%s.log", root);
	p = log;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CL_LOGGER - log messages for the client CL */

static void cl_logger(client *cl, int lvl, char *fmt, ...)
   {char s[MAXLINE];
    char *root, *wh, *flog;

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    wh   = C_OR_S(cl->type == CLIENT);
    root = cl->root;
    flog = name_log(root);

    log_activity(flog, svs.debug, lvl, wh, s);

    return;}

/*--------------------------------------------------------------------------*/

/*                             SERVER SIDE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* NAME_DB - derive the name of the database from ROOT */

char *name_db(char *root)
   {static char fname[MAXLINE];

    snprintf(fname, MAXLINE, "%s.db", root);

    return(fname);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NAME_CONN - derive the name of the connection file from ROOT */

char *name_conn(char *root)
   {static char fname[MAXLINE];

    snprintf(fname, MAXLINE, "%s.conn", root);

    return(fname);}

/*--------------------------------------------------------------------------*/
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

/* _PUSH_COL - push a character collection delimited by CB,CE onto D */

static void _push_col(char **pd, char **ps, int cb, int ce)
   {char *s, *d;

    s = *ps;
    d = *pd;

    *d++ = cb;
    for (*d++ = *s++; *s != ce; *d++ = *s++)
        {if (*s == '\\')
            s++;};
    *d++ = *s++;

    *ps = s;
    *pd = d;

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
    char s[MAXLINE];
    char *val, *pk, *pm, *ps, *pt;
    client *cl;
    static char t[LRG];

    val = NULL;

    if (db != NULL)
       {cl = db->cl;

/* resolve var of form 'a.b.c' */
	nstrncpy(s, MAXLINE, var, -1);
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
		 for (c = *pk++; (c != ')') && (ok == TRUE); c = *pk++)
		     {switch (c)
			 {case '"' :
			  case '\'' :
			       _push_col(&pt, &pk, c, c);
			       break;
			 case '(' :
                              _push_col(&pt, &pk, c, ')');
			      break;
			 case '=' :
			      *pt++ = '\0';
			      if (st == TRUE)
				 {_mem_name(t, pm);
				  ok = FALSE;
				  break;}
			      else if (_mem_name(t, pm) == 0)
				 {st = TRUE;
				  val = t;};
			      pt = t;
			      break;
			 default :
                              *pt++ = c;
			      break;};};};};

	CLOG(cl, 1, "get |%s|=|%s|", var, val);};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SHOW_VAR - show variable VR value */

static int _show_var(database *db, client *cl, FILE *fp, const char *fmt,
		     char *vr, char **vars)
   {int l, rv;
    char s[LRG], t[LRG];
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
	   {snprintf(t, MAXLINE, "\"%s\"", vl);
	    vl = t;};

	if (fmt == NULL)
	   snprintf(s, LRG, "%s=%s", vr, vl);
	else
	   snprintf(s, LRG, fmt, vr, vl);

/* write to the communicator if FP is NULL */
	if (fp == NULL)
	   comm_write(cl, s, 0, 10);
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
    client *cl;
    database *db;
    vardes *pv;

    pv = (vardes *) a;

    cl   = pv->cl;
    db   = pv->db;
    fmt  = pv->fmt;
    fp   = pv->fp;
    vars = pv->vars;

    vr = hp->name;
    rv = _show_var(db, cl, fp, fmt, vr, vars);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SAVE_DB - save variable VAR to FP */

int save_db(database *db, char **vars, FILE *fp, const char *fmt)
   {int rv;
    client *cl;
    vardes pv;

    rv = FALSE;
    cl = db->cl;

    if (db != NULL)
       {pv.cl   = cl;
	pv.fmt  = (char *) fmt;
	pv.db   = db;
	pv.fp   = fp;
	pv.vars = vars;

        hash_foreach(db->tab, _save_var, &pv);

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
    static char v[LRG];

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
		    vstrcat(v, LRG, "%s\n", s);

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
   {char *fname, *flog, *fcon, *root, *t;
    database *db;

    db = NULL;

    if (cl != NULL)
       {root = cl->root;

	t = name_log(root);
	flog = (t == NULL) ? NULL : STRSAVE(t);

	t = name_db(root);
	fname = (t == NULL) ? NULL : STRSAVE(t);

	t = name_conn(root);
	fcon = (t == NULL) ? NULL : STRSAVE(t);

	db = MAKE(database);
	if (db != NULL)
	   {db->debug = dbg;
	    db->auth  = auth;
	    db->root  = STRSAVE(root);
	    db->file  = fname;
	    db->flog  = flog;
	    db->fcon  = fcon;
	    db->tab   = make_hash_table(-1);
	    db->cl    = cl;}
	else
	   {FREE(flog);
	    FREE(fname);
	    FREE(fcon);};};

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_DB - release the DB */

void free_db(database *db)
   {

    if (db != NULL)
       {hash_free(db->tab);

	FREE(db->root);
	FREE(db->file);
	FREE(db->flog);
	FREE(db->fcon);
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
       {fp = fopen(db->file, "r");
	if (fp != NULL)
	   {load_db(db, NULL, fp);
	    fclose(fp);};

	CLOG(cl, 1, "load %d |%s|", db->tab->ne, db->file);};

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_OPEN - open the database */

int db_srv_open(client *cl, int init, int dbg, int auth)
   {int rv, pid;
    database *db;

    if (init == TRUE)
       db = db_srv_create(cl, dbg, auth);
    else
       db = db_srv_load(cl, dbg, auth);

    if (db != NULL)
       {pid   = getpid();
	cl->a = db;

/* if a server is already running there will be a PID file */
	if ((db->fcon != NULL) && (file_exists(db->fcon) == FALSE))
	   rv = open_server(cl, SERVER, db->auth);

	else
	   {free_db(db);
	    db = NULL;};

	srv.pid = pid;};

    rv = (db != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_CLOSE - close the database */

void db_srv_close(database *db)
   {client *cl;

    if (db != NULL)
       {cl = db->cl;

	close_sock(cl);

	unlink_safe(db->fcon);

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

	fp = fopen(db->file, "w");
	if (fp != NULL)
	   {client *tcl;

/* GOTCHA: why do we need this? */
	    tcl = make_client(SERVER, cl->auth, db->root, NULL);
	    tcl->fd = fd;
	    tcl->a  = db;

	    save_db(db, NULL, fp, NULL);

	    tcl->fd = -1;
	    free_client(tcl);

	    fclose(fp);
	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_LAUNCH - launch a DB server */

int db_srv_launch(client *cl)
   {int i, st, pid, rv;
    char s[MAXLINE];
    char *root, *fcon, **sa;

    rv = FALSE;

    if (cl != NULL)
       {root = cl->root;
	fcon = name_conn(root);

	st   = 0;
	s[0] = '\0';

/* wait until we have a server going */
	for (i = 0; file_exists(fcon) == FALSE; i++)
	    {if (i == 1)
		{if (cl->auth == TRUE)
		    snprintf(s, MAXLINE, "perdb -a -f %s -s -l", root);
		 else
		    snprintf(s, MAXLINE, "perdb -f %s -s -l", root);
		 st = system(s);
		 st = WEXITSTATUS(st);
		 CLOG(cl, 1, "launch |%s| (%d)", s, st);}
	     else
	        nsleep(100);};

/* check the pid */
        sa = file_text(FALSE, fcon);

	if ((sa != NULL) && (sa[CONN_PID] != NULL))
	   pid = atoi(sa[CONN_PID]);
	else
	   pid = -1;

	if (cl != NULL)
	   {cl->nkey = N_AKEY;
	    cl->key  = STRSAVE(sa[CONN_KEY]);};

	free_strings(sa);

/* GOTCHA: if it is not running - clean up and restart */
	st = is_running(pid);

	srv.pid = pid;

        CLOG(cl, 1, "server pid %d (%d)", pid, i);};

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

	rv = open_server(cl, SERVER, db->auth);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             CLIENT SIDE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _DB_CLNT_EX - do a transaction from the client side of the database */

char **_db_clnt_ex(client *cl, int init, char *req)
   {int nb, ne, nx;
    char **p;
    static char s[MAXLINE];

    if (cl == NULL)
       {char *root;

        root = cgetenv(TRUE, "PERDB_PATH");
	cl   = make_client(CLIENT, FALSE, root, cl_logger);};

    p = NULL;

    CLOG(cl, 1, "begin request |%s|", req);

/* make sure that there is a server running */
    if (init == TRUE)
       db_srv_launch(cl);

/* send the request */
    nb = comm_write(cl, req, 0, 10);

/* get the reply */
    if (nb > 0)
       {int nc, to, ok;
	char *t;

	ne = 0;
	nx = 100;
	to = 4;
	for (ok = TRUE; ok == TRUE; )
	    {nb = comm_read(cl, s, MAXLINE, to);
	     ne = (nb < 0) ? ne+1 : 0;

/* if more than NX consecutive read failures bail */
	     if (ne >= nx)
	        {p  = NULL;
		 ok = FALSE;}

	     else
	        {for (t = s; nb > 0; t += nc, nb -= nc)
		     {if (strcmp(t, EOM) == 0)
			 {ok = FALSE;
			  break;}
		      else if (strcmp(t, "reject:") == 0)
			 {p  = lst_push(p, "rejected");
			  ok = FALSE;
			  break;}
		      else
			 p = lst_push(p, t);

		      nc = strlen(t) + 1;};

		 if (ok == TRUE)
		    nsleep(0);};};};

    CLOG(cl, 1, "end request");

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBSET - set the database variable VAR to VAL (counterpart to csetenv) */

int dbset(client *cl, char *var, char *fmt, ...)
   {int i, err, nc, nr, ok;
    char s[LRG];
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
	VSNPRINTF(s, LRG, fmt);
	VA_END;

	nc  = strlen(var) + strlen(s) + 2;
	t   = malloc(nc);
	if (t != NULL)
	   {snprintf(t, nc, "%s=%s", var, s);

	    ta  = _db_clnt_ex(cl, TRUE, t);
	    err = (ta != NULL);

/* GOTCHA: temporarily also add it to the environment
 * this can go when utilities using this are completely converted
 * away from using the environment
 */
	    err = putenv(t);

	    note(Log, TRUE, "setenv %s %s", var, s);

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
   {char var[MAXLINE];
    char *t, **ta;

    VA_START(fmt);
    VSNPRINTF(var, MAXLINE, fmt);
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
    char var[MAXLINE];
    char **ta;

    VA_START(fmt);
    VSNPRINTF(var, MAXLINE, fmt);
    VA_END;

    ta = _db_clnt_ex(cl, TRUE, var);
    rv = ((ta != NULL) && (IS_NULL(ta[0]) == FALSE));

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
    char s[LRG];

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    err = 0;
    if (dbdef(cl, var) == FALSE)
       err = dbset(cl, var, s);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_RESTORE - have the server load the specified database
 *            - and set all the variables in the environment
 *            - of the current process
 */

int db_restore(client *cl, char *dname)
   {int i, rv, ok;
    char t[MAXLINE];
    char **ta, *s, *vl;

    rv = TRUE;

/* load the database */
    if (dname == NULL)
       nstrncpy(t, MAXLINE, "load:", -1);
    else
       snprintf(t, MAXLINE, "load:%s", dname);

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
/*--------------------------------------------------------------------------*/

#endif
