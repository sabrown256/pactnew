/*
 * LIBDB.C - persistent database routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

extern char
 *name_log(char *root);

#include <setjmp.h>

#define SERVER  0
#define CLIENT  1

#define EOM     "++ok++"

#define WHICH_PROC()       ((ioc->server == CLIENT) ? "CLIENT" : "SERVER")

typedef struct s_database database;
typedef struct s_db_comm db_comm;

struct s_database
   {int ne;
    char *root;                 /* root path name of database */
    char *file;                 /* name of the file image of the database */
    char *flog;                 /* name of the log file */
    char *fpid;                 /* name of the pid file */
    char **entries;};

struct s_db_comm
   {int (*exists_comm)(char *fmt, ...);
    int (*open_comm)(char *root);                          /* server only */
    int (*close_comm)(char *root);
    int (*read_comm)(char *root, int ch, char *s, int nc);
    int (*write_comm)(char *root, int ch, char *s, int nc);
    int server;};

extern db_comm
 *ioc;

#include "libsock.c"
#include "libfifo.c"

static int
 dbg_db = FALSE;

db_comm
 *ioc = &sock;
/*  *ioc = &fifo; */

database
 *db = NULL;

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

/* COMM_READ - read from ROOT into S
 *           - quit if it hasn't heard anything in TO seconds
 */

int comm_read(char *root, int ch, char *s, int nc, int to)
   {int nb;

#ifdef IO_ALARM
    signal(SIGALRM, sigtimeout);

    alarm(to);

    if (setjmp(cpu) == 0)
       nb = ioc->read_comm(root, SERVER, s, nc);
    else
       nb = -1;

    alarm(0);
#else
    nb = ioc->read_comm(root, SERVER, s, nc);
#endif

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMM_WRITE - write S to ROOT
 *            - quit if it hasn't heard anything in TO seconds
 */

int comm_write(char *root, int ch, char *s, int nc, int to)
   {int nb;

#ifdef IO_ALARM
    signal(SIGALRM, sigtimeout);

    alarm(to);

    if (setjmp(cpu) == 0)
       nb = ioc->write_comm(root, SERVER, s, nc);
    else
       nb = -1;

    alarm(0);
#else
    nb = ioc->write_comm(root, SERVER, s, nc);
#endif

    return(nb);}

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

/* PUT_DB - add VAR to DB */

char *put_db(database *db, char *var, char *val)
   {int i, nv;
    char **vars;

    nv   = db->ne;
    vars = db->entries;
    for (i = 0; i < nv; i++)
        {if (strcmp(var, vars[i]) == 0)
	    break;};

    if (i >= nv)
       {db->entries = lst_push(vars, var);
	db->ne++;};

    if (val == NULL)
       val = "";

    csetenv(var, val);

    log_activity(db->flog, dbg_db, "SERVER", "put |%s|=|%s|", var, val);

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
    char s[MAXLINE], t[LRG];
    char *val, *pk, *pm, *ps, *pt;

    val = NULL;

/* resolve var of form 'a.b.c' */
    nstrncpy(s, MAXLINE, var, -1);
    for (ps = s; TRUE; ps = NULL)
        {pm = strtok(ps, ".");

/* we are at the end the val has the answer */
         if (pm == NULL)
	    break;

/* get the entire structure */
	 else if (val == NULL)
	    val = cgetenv(TRUE, pm);

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

    log_activity(db->flog, dbg_db, "SERVER", "get |%s|=|%s|", var, val);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DUMP_DB - dump variable VAR to FP */

int dump_db(database *db, char *var, FILE *fp)
   {int i, rv, nv;
    char s[LRG], t[LRG];
    char *vl, *vr, **vrs;

    rv = FALSE;

    if (db != NULL)
       {vrs = db->entries;
	nv  = db->ne;
	for (i = 0; i < nv; i++)
	    {vr = vrs[i];
             if ((var == NULL) || (var[0] == '\0') || (strcmp(var, vr) == 0))
		{vl = cgetenv(TRUE, vr);
		 if ((vl[0] != '"') && (strpbrk(vl, " \t") != NULL))
		    {snprintf(t, MAXLINE, "\"%s\"", vl);
		     vl = t;};

/* write to the communicator if FP is NULL */
	         if (fp == NULL)
		    {snprintf(s, LRG, "%s=%s", vr, vl);
		     comm_write(db->root, CLIENT, s, 0, 10);}
		 else
		    fprintf(fp, "%s=%s\n", vr, vl);};};

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOAD_DB - load the DB into memory from FP */

void load_db(database *db, FILE *fp)
   {char s[LRG];
    char *var, *val;

    while (fgets(s, LRG, fp) != NULL)
       {key_val(&var, &val, s, "=\n");
	val = put_db(db, var, val);};

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

    db          = MAKE(database);
    db->ne      = 0;
    db->root    = STRSAVE(root);
    db->file    = fname;
    db->flog    = flog;
    db->fpid    = fpid;
    db->entries = NULL;

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_DB - release the DB */

void free_db(database *db)
   {

    FREE(db->root);
    FREE(db->file);
    FREE(db->flog);
    FREE(db->fpid);
    FREE(db);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_CREATE - initialize the DB */

database *db_srv_create(char *root)
   {database *db;

    db = make_db(root);

    unlink(db->file);

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_LOAD - load the DB into memory */

database *db_srv_load(char *root)
   {database *db;
    FILE *fp;

    db = make_db(root);

    fp = fopen(db->file, "r");
    if (fp != NULL)
       {load_db(db, fp);
	fclose(fp);};

    log_activity(db->flog, dbg_db, "SERVER", "load %d |%s|", db->ne, db->file);

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_OPEN - open the database */

database *db_srv_open(char *root, int init)
   {int rv;
    database *db;
    FILE *fp;

    if (init == TRUE)
       db = db_srv_create(root);
    else
       db = db_srv_load(root);

/* if a server is already running there will be a PID file */
    if (file_exists(db->fpid) == FALSE)
       {ioc->server = SERVER;
	rv = ioc->open_comm(root);

	fp = fopen(db->fpid, "w");
	fprintf(fp, "%d", getpid());
	fclose(fp);}

    else
       {free_db(db);
        db = NULL;};

    return(db);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_CLOSE - close the database */

void db_srv_close(database *db)
   {

    ioc->close_comm(db->root);

    unlink(db->fpid);

    free_db(db);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_SAVE - save the DB to disk */

int db_srv_save(database *db)
   {int rv;
    FILE *fp;

    rv = FALSE;

    log_activity(db->flog, dbg_db, "SERVER", "save %d |%s|", db->ne, db->file);

    if (db != NULL)
       {fp = fopen(db->file, "w");
	if (fp != NULL)
	   {dump_db(db, NULL, fp);
	    fclose(fp);
	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_LAUNCH - launch a DB server */

int db_srv_launch(char *root)
   {int i, st, pid, rv;
    char s[MAXLINE], t[MAXLINE];
    char *flog, *fpid;
    FILE *fp;

    rv = FALSE;

    if (root != NULL)
       {fpid = name_pid(root);
	flog = name_log(root);

	st   = 0;
	s[0] = '\0';

/* wait until we have a server going */
	for (i = 0; file_exists(fpid) == FALSE; i++)
	    {if (i == 1)
		{if (ioc == &fifo)
		    snprintf(s, MAXLINE, "perdb -f %s -s -x fifo", root);
		 else
		    snprintf(s, MAXLINE, "perdb -f %s -s -x socket", root);
		 st = system(s);
		 log_activity(flog, dbg_db, "CLIENT", "launch |%s| (%d)",
			      s, st);}
	     else
	        nsleep(100);};

/* check the pid */
        fp = fopen(fpid, "r");
	if (fgets(t, MAXLINE, fp) != NULL)
	   pid = atoi(t);
	else
	   pid = -1;

        log_activity(flog, dbg_db, "CLIENT", "server pid %d (%d)",
		     pid, i);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DB_SRV_RESTART - restart the server */

void db_srv_restart(database *db)
   {int rv;

    if (db != NULL)
       {ioc->close_comm(db->root);
	db_srv_save(db);

	log_activity(db->flog, dbg_db, "SERVER", "restart");

	ioc->server = SERVER;
	rv = ioc->open_comm(db->root);};

    return;}

/*--------------------------------------------------------------------------*/

/*                             CLIENT SIDE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _DB_CLNT_EX - do a transaction from the client side of the database */

char **_db_clnt_ex(char *root, char *req)
   {int nb, to;
    char **p, *flog, *fpid;
    static char s[MAXLINE];

    p = NULL;

    flog = name_log(root);

    log_activity(flog, dbg_db, "CLIENT", "begin |%s|", req);

    fpid = name_pid(root);

/* make sure that there is a server running */
    db_srv_launch(root);

/* send the request */
    nb = comm_write(root, SERVER, req, 0, 10);

/* get the reply */
    if (nb > 0)
       {to = 4;
	while (TRUE)
           {nb = comm_read(root, CLIENT, s, MAXLINE, to);
	    if ((nb < 0) || (strcmp(s, EOM) == 0))
	       break;
	    else
	       p = lst_push(p, s);};};

    log_activity(flog, dbg_db, "CLIENT", "end");

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBSET - set the database variable VAR to VAL (counterpart to csetenv) */

int dbset(char *root, char *var, char *fmt, ...)
   {int err, nc;
    char s[LRG];
    char *t, **ta;

    VA_START(fmt);
    VSPRINTF(s, fmt);
    VA_END;

    err = 0;
    nc  = strlen(var) + strlen(s) + 2;
    t   = malloc(nc);
    if (t != NULL)
       {snprintf(t, nc, "%s=%s", var, s);
	ta  = _db_clnt_ex(root, t);
	err = (ta != NULL);

/* GOTCHA: temporarily also add it to the environment
 * this can go when utilities using this are completely converted
 * away from using the environment
 */
	err = putenv(t);

	note(Log, TRUE, "setenv %s %s", var, s);

	lst_free(ta);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBGET - get the value of database variable VAR
 *       - if LIT is TRUE return as is otherwise strip off
 *       - surrounding quotes
 *       - (counterpart to cgetenv)
 */

char *dbget(char *root, int lit, char *fmt, ...)
   {char var[MAXLINE];
    char *t, **ta;
    static char none[] = "";

    VA_START(fmt);
    VSPRINTF(var, fmt);
    VA_END;

    ta = _db_clnt_ex(root, var);
    if (ta == NULL)
       t = none;

    else if (lit == FALSE)
       t = strip_quote(ta[0]);

    lst_free(ta);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBDEF - return TRUE iff the database variable VAR is defined
 *       - (counterpart to cdefenv)
 */

int dbdef(char *root, char *fmt, ...)
   {int rv;
    char var[MAXLINE];
    char **ta;

    VA_START(fmt);
    VSPRINTF(var, fmt);
    VA_END;

    ta = _db_clnt_ex(root, var);
    rv = (ta != NULL);

    lst_free(ta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBCMP - return a strcmp style value for the
 *       - comparison of the database variable VAR value and VAL
 *       - (counterpart to cmpenv)
 */

int dbcmp(char *root, char *var, char *val)
   {int rv;
    char **ta;

    ta = _db_clnt_ex(root, var);
    if (ta != NULL)
       rv = strcmp(ta[0], val);
    else
       rv = -2;

    lst_free(ta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBINITV - set the database variable VAR to VAL
 *         - only if it is undefined
 *         - (counterpart to cinitenv)
 */

int dbinitv(char *root, char *var, char *fmt, ...)
   {int err;
    char s[LRG];

    VA_START(fmt);
    VSPRINTF(s, fmt);
    VA_END;

    err = 0;
    if (dbdef(root, var) == FALSE)
       err = dbset(root, var, s);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
