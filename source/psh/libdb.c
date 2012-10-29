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

#include "libhash.c"

#define EOM     "++ok++"

#define WHICH_PROC()       ((ioc_server == CLIENT) ? "CLIENT" : "SERVER")

enum e_ckind
 {SERVER, CLIENT};

typedef enum e_ckind ckind;

typedef struct s_database database;
typedef struct s_vardes vardes;
typedef struct s_db_session db_session;

struct s_database
   {char *root;                 /* root path name of database */
    char *file;                 /* name of the file image of the database */
    char *flog;                 /* name of the log file */
    char *fpid;                 /* name of the pid file */
    hashtab *tab;};

struct s_db_session
   {int ioc_server;
    int debug;
    int init;
    int auth;
    int daemon;
    int literal;
    char root[MAXLINE];};

static db_session
  dbs = { CLIENT, FALSE, FALSE, FALSE, FALSE, FALSE, };

static int
 ioc_server = CLIENT,
 dbg_db     = FALSE;

extern char
 *name_log(char *root);

#include "libsock.c"

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

/* SIGTIMEOUT - handle signals meant to timeout session */

#ifdef IO_ALARM

static jmp_buf
 cpu;

static void sigtimeout(int sig)
   {

    longjmp(cpu, 1);

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WHOAMI - return the tokenized list of pw entries for UID
 *        - if UID is -1 take the current user
 */

char **whoami(int id)
   {uid_t uid;
    char bf[MAXLINE];
    char **sa;
    extern int getpw(uid_t uid, char *bf);

    uid = (id < 0) ? getuid() : id;

    getpw(uid, bf);
    sa = tokenize(bf, ":");

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VERIFYX - verify the transaction request
 *         - return TRUE if ANS is correct
 */

#define PUBLIC_KEY  "47fcb593c5399d7c182944589a1bafb4"
#define N_VFY       16

int verifyx(char *root, int nc, char *ans, char *res)
   {int ik, li, ni, nk, rv;
    long long r;
    char lres[N_VFY];
    static char *key = PUBLIC_KEY;
    static char *info = NULL;

    rv = TRUE;

    if (info == NULL)
       {char **sa;

	info = MAKE_N(char, LRG);
	info[0] = '\0';

/* get user info */
	sa = whoami(-1);
	vstrcat(info, LRG, "%s ", sa[0]);
	vstrcat(info, LRG, "%s ", sa[2]);
	vstrcat(info, LRG, "%s ", sa[3]);
	free_strings(sa);

/* get connection info */
	sa = parse_sema(root);
	vstrcat(info, LRG, "%s ", sa[1]);
	vstrcat(info, LRG, "%s ", sa[2]);
	free_strings(sa);};

/* compute the result by folding info with public key */
    r  = 0;
    nk = strlen(key);
    ni = strlen(info);
    for (ik = 0; ik < nk; ik++)
        {li = ik % ni;
	 r  = (r << 1LL) ^ ((long long) (key[ik] ^ info[li]));};

/* client wants the answer */
    if (res != NULL)
       {snprintf(res, nc, "%0*llx", nc-1, r);
	rv = TRUE;}

/* server matches the answer */
    else
       {res = lres;
	snprintf(res, nc, "%0*llx", nc-1, r);
	rv = (strncmp(res, ans, nc-1) == 0);};

    if (rv == FALSE)
       {char *flog;
	char **sa;

	sa = whoami(-1);

	flog = name_log(root);
	log_activity(flog, dbs.debug, 1, "SERVER", "access denied |%s|",
		     sa[0]);

	free_strings(sa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMM_READ - read from CL into S
 *           - quit if it hasn't heard anything in TO seconds
 *           - return -2 if authentication is denied
 */

int comm_read(client *cl, char *s, int nc, int to)
   {int nb;

#if 0
    int nt, ok;
    char *t;

    nt = nc + N_VFY + 1;
    t  = MAKE_N(char, nt);

#ifdef IO_ALARM
    signal(SIGALRM, sigtimeout);

    alarm(to);

    if (setjmp(cpu) == 0)
       nb = read_sock(cl, t, nt);
    else
       nb = -1;

    alarm(0);
#else
    nb = read_sock(cl, t, nt);
#endif

    if (strncmp(t, "auth:", 5) == 0)
       ok = verifyx(cl->root, N_VFY, t+5, NULL);
    else
       ok = TRUE;

    if (ok == FALSE)
       nb = -2;

    else
       nstrncpy(s, nc, t + N_VFY + 5, -1);

    nb -= (N_VFY + 5);

    FREE(t);

#else

#ifdef IO_ALARM
    signal(SIGALRM, sigtimeout);

    alarm(to);

    if (setjmp(cpu) == 0)
       nb = read_sock(cl, s, nc);
    else
       nb = -1;

    alarm(0);
#else
    nb = read_sock(cl, s, nc);
#endif

#endif

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMM_WRITE - write S to CL
 *            - quit if it hasn't heard anything in TO seconds
 *            - return -2 if authentication is denied
 */

int comm_write(client *cl, char *s, int nc, int to)
   {int nb;

#if 0

    int nt, ok;
    char ans[N_VFY];
    char *t;

    ok = verifyx(cl->root, N_VFY, NULL, ans);
    if (ok == FALSE)
       nb = -2;

    else
       {if (nc <= 0)
	   nc = strlen(s);

	nt = nc + N_VFY + 6;
	t  = MAKE_N(char, nt);
	if (cl->auth == TRUE)
	   snprintf(t, nt, "auth:%s %s", ans, s);
	else
	   nstrncpy(t, nt, s, -1);
	nt = strlen(t) + 1;

#ifdef IO_ALARM
	signal(SIGALRM, sigtimeout);

	alarm(to);

	if (setjmp(cpu) == 0)
	   nb = write_sock(cl, t, nt);
	else
	   nb = -1;

	alarm(0);
#else
	nb = write_sock(cl, t, nt);
#endif

	nb -= N_VFY;

	FREE(t);};

#else

#ifdef IO_ALARM
    signal(SIGALRM, sigtimeout);

    alarm(to);

    if (setjmp(cpu) == 0)
       nb = write_sock(cl, s, nc);
    else
       nb = -1;

    alarm(0);
#else
    nb = write_sock(cl, s, nc);
#endif

#endif

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_CLIENT - initialize and return a client connection instance */

client *make_client(char *root, ckind type)
   {char *flog;
    client *cl;

    cl = MAKE(client);
    if (cl != NULL)
       {cl->fd     = -1;
	cl->root   = root;
	cl->type   = type;
	cl->server = &srv;

	flog = name_log(root);
	log_activity(flog, dbg_db, 1,
		     (type == CLIENT) ? "CLIENT" : "SERVER",
		     "----- start client -----");};

    return(cl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_CLIENT - release a client connection instance */

void free_client(client *cl)
   {char *flog;

    if (cl != NULL)
       {if (cl->type == CLIENT)
	   comm_write(cl, "fin:", 0, 10);

	if (cl->fd >= 0)
	   cl->fd = connect_close(cl->fd, cl, NULL);

	flog = name_log(cl->root);
	log_activity(flog, dbg_db, 1,
		     (cl->type == CLIENT) ? "CLIENT" : "SERVER",
		     "----- end client -----");

	FREE(cl);};

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

/* NAME_PID - derive the name of the PID file from ROOT */

char *name_pid(char *root)
   {static char fname[MAXLINE];

    snprintf(fname, MAXLINE, "%s.pid", root);

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
   {

    hash_clear(db->tab);

    log_activity(db->flog, dbg_db, 1, "SERVER", "reset");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUT_DB - add VAR to DB */

char *put_db(database *db, char *var, char *val)
   {

    if (var != NULL)
       {if (val == NULL)
	   val = cnoval();

	_set_var(db, var, val);

	log_activity(db->flog, dbg_db, 1,
		     "SERVER", "put |%s|=|%s|", var, val);};

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
    static char t[LRG];

    val = NULL;

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

    log_activity(db->flog, dbg_db, 1, "SERVER", "get |%s|=|%s|", var, val);

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

int save_db(int fd, database *db, char **vars, FILE *fp, const char *fmt)
   {int rv;
    client *cl;
    vardes pv;

    rv = FALSE;

    if (db != NULL)
       {cl = make_client(db->root, SERVER);
	cl->fd = fd;

	pv.cl   = cl;
	pv.fmt  = (char *) fmt;
	pv.db   = db;
	pv.fp   = fp;
	pv.vars = vars;

        hash_foreach(db->tab, _save_var, &pv);

	cl->fd = -1;
	free_client(cl);

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

static database *make_db(char *root)
   {char *fname, *flog, *fpid, *t;
    database *db;

    t = name_log(root);
    flog = (t == NULL) ? NULL : STRSAVE(t);

    t = name_db(root);
    fname = (t == NULL) ? NULL : STRSAVE(t);

    t = name_pid(root);
    fpid = (t == NULL) ? NULL : STRSAVE(t);

    db = MAKE(database);
    if (db != NULL)
       {db->root    = STRSAVE(root);
	db->file    = fname;
	db->flog    = flog;
	db->fpid    = fpid;
	db->tab     = make_hash_table(-1);}
    else
       {FREE(flog);
	FREE(fname);
	FREE(fpid);};

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
	FREE(db->fpid);
	FREE(db);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_CREATE - initialize the DB */

database *db_srv_create(char *root)
   {database *db;

    db = make_db(root);
    if ((db != NULL) && (db->file != NULL))
       unlink_safe(db->file);

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_LOAD - load the DB into memory */

database *db_srv_load(char *root)
   {database *db;
    FILE *fp;

    db = make_db(root);
    if ((db != NULL) && (db->file != NULL))
       {fp = fopen(db->file, "r");
	if (fp != NULL)
	   {load_db(db, NULL, fp);
	    fclose(fp);};

	log_activity(db->flog, dbg_db, 1, "SERVER", "load %d |%s|",
		     db->tab->ne, db->file);};

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_OPEN - open the database */

database *db_srv_open(char *root, int init)
   {int rv, pid;
    database *db;
    FILE *fp;

    if (init == TRUE)
       db = db_srv_create(root);
    else
       db = db_srv_load(root);

    if (db != NULL)
       {pid = getpid();

/* if a server is already running there will be a PID file */
	if ((db->fpid != NULL) && (file_exists(db->fpid) == FALSE))
	   {ioc_server = SERVER;
	    rv = open_server(root);
	    ASSERT(rv == 0);

	    fp = fopen(db->fpid, "w");
	    if (fp != NULL)
	       {fprintf(fp, "%d", pid);
		fclose(fp);};}

	else
	   {free_db(db);
	    db = NULL;};

	srv.pid = pid;};

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_CLOSE - close the database */

void db_srv_close(database *db)
   {

    if (db != NULL)
       {close_sock(db->root);

	unlink_safe(db->fpid);

	free_db(db);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_SAVE - save the DB to disk */

int db_srv_save(int fd, database *db)
   {int rv;
    FILE *fp;

    rv = FALSE;

    if (db != NULL)
       {log_activity(db->flog, dbg_db, 1, "SERVER", "save %d |%s|",
		     db->tab->ne, db->file);

	fp = fopen(db->file, "w");
	if (fp != NULL)
	   {save_db(fd, db, NULL, fp, NULL);
	    fclose(fp);
	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_LAUNCH - launch a DB server */

int db_srv_launch(char *root)
   {int i, st, pid, rv;
    char s[MAXLINE];
    char *flog, *fpid, **sa;

    rv = FALSE;

    if (root != NULL)
       {fpid = name_pid(root);
	flog = name_log(root);

	st   = 0;
	s[0] = '\0';

/* wait until we have a server going */
	for (i = 0; file_exists(fpid) == FALSE; i++)
	    {if (i == 1)
		{snprintf(s, MAXLINE, "perdb -f %s -s -l", root);
		 st = system(s);
		 st = WEXITSTATUS(st);
		 log_activity(flog, dbg_db, 1, "CLIENT", "launch |%s| (%d)",
			      s, st);}
	     else
	        nsleep(100);};

/* check the pid */
        sa = file_text(FALSE, fpid);

	if ((sa != NULL) && (sa[0] != NULL))
	   pid = atoi(sa[0]);
	else
	   pid = -1;

	free_strings(sa);

/* GOTCHA: if it is not running - clean up and restart */
	st = is_running(pid);

	srv.pid = pid;

        log_activity(flog, dbg_db, 1, "CLIENT", "server pid %d (%d)",
		     pid, i);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_RESTART - restart the server */

void db_srv_restart(database *db)
   {int rv;

    if (db != NULL)
       {close_sock(db->root);
	db_srv_save(-1, db);

	log_activity(db->flog, dbg_db, 1, "SERVER", "restart");

	ioc_server = SERVER;
	rv = open_server(db->root);
	ASSERT(rv == 0);};

    return;}

/*--------------------------------------------------------------------------*/

/*                             CLIENT SIDE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _DB_CLNT_EX - do a transaction from the client side of the database */

char **_db_clnt_ex(client *cl, int init, char *req)
   {int nb, ne, nx;
    char **p, *flog, *root;
    static char s[MAXLINE];

    if (cl == NULL)
       {root = cgetenv(TRUE, "PERDB_PATH");
	cl   = make_client(root, CLIENT);};

    p    = NULL;
    root = cl->root;

    flog = name_log(root);

    log_activity(flog, dbg_db, 1, "CLIENT", "begin request |%s|", req);

/* make sure that there is a server running */
    if (init == TRUE)
       db_srv_launch(root);

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
		      else
			 p = lst_push(p, t);

		      nc = strlen(t) + 1;};

		 if (ok == TRUE)
		    nsleep(0);};};};

    log_activity(flog, dbg_db, 1, "CLIENT", "end request");

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

int db_restore(client *cl, char *db)
   {int i, rv, ok;
    char t[MAXLINE];
    char **ta, *s, *vl;

    rv = TRUE;

/* load the database */
    if (db == NULL)
       nstrncpy(t, MAXLINE, "load:", -1);
    else
       snprintf(t, MAXLINE, "load:%s", db);

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
