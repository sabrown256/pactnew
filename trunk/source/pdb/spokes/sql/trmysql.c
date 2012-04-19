/*
 * TRMYSQL.C - Mysql support for SQL spoke in SX
 *
 * mySQL commands
 *
 *   show tables;
 *   describe $table;
 *   select * from $table;
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trsql.h"

#ifdef HAVE_MYSQL

#include <mysql.h>

#define GET_CONNECTION(_f) ((MYSQL *) GET_SQL_FILE(_f)->conn)

typedef struct s_mysql_methods mysql_methods;

struct s_mysql_methods
   {MYSQL *(*init)(MYSQL *sql);                               /* mysql_init */
    MYSQL *(*connect)(MYSQL *sql, const char *host,   /* mysql_real_connect */
		      const char *user,
		      const char *passwd,
		      const char *db,
		      unsigned int port,
		      const char *unix_socket,
		      unsigned long clientflag);
    void (*close)(MYSQL *sql);                               /* mysql_close */
    int	(*query)(MYSQL *sql, const char *q);                 /* mysql_query */
    MYSQL_RES *(*result)(MYSQL *mysql);               /* mysql_store_result */
    my_ulonglong (*num_rows)(MYSQL_RES *res);             /* mysql_num_rows */
    unsigned int (*num_fields)(MYSQL_RES *res);         /* mysql_num_fields */
    MYSQL_ROW (*fetch_row)(MYSQL_RES *result);           /* mysql_fetch_row */
    void (*free_result)(MYSQL_RES *result);            /* mysql_free_result */
    const char *(*error)(MYSQL *mysql);};                    /* mysql_error */

static mysql_methods
 _MS_mth;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define DYNAMICALLY_LINKED

/* _MYSQL_SET_METHODS - setup the methods for MYSQL access */

void _MYSQL_set_methods(void)
   {

    if (_MS_mth.init == NULL)

#ifdef DYNAMICALLY_LINKED

/* dynamically linked way */
       {int rv;

	rv = SC_so_register_func(OBJ_SO, "libmysql.so", "mysql",
				 NULL, NULL, NULL, NULL);
	if (rv == FALSE)
	   PD_error("CANNOT LOAD 'libmysql.so'", PD_OPEN);

        _MS_mth.init        = SC_so_get(OBJ_SO, "mysql", "mysql_init");
	_MS_mth.connect     = SC_so_get(OBJ_SO, "mysql", "mysql_real_connect");
	_MS_mth.close       = SC_so_get(OBJ_SO, "mysql", "mysql_close");
	_MS_mth.query       = SC_so_get(OBJ_SO, "mysql", "mysql_query");
	_MS_mth.result      = SC_so_get(OBJ_SO, "mysql", "mysql_store_result");
	_MS_mth.num_rows    = SC_so_get(OBJ_SO, "mysql", "mysql_num_rows");
	_MS_mth.num_fields  = SC_so_get(OBJ_SO, "mysql", "mysql_num_fields");
	_MS_mth.fetch_row   = SC_so_get(OBJ_SO, "mysql", "mysql_fetch_row");
	_MS_mth.free_result = SC_so_get(OBJ_SO, "mysql", "mysql_free_result");
	_MS_mth.error       = SC_so_get(OBJ_SO, "mysql", "mysql_error");};

#else

/* statically linked way */
       {_MS_mth.init        = mysql_init;
	_MS_mth.connect     = mysql_real_connect;
	_MS_mth.close       = mysql_close;
	_MS_mth.query       = mysql_query;
	_MS_mth.result      = mysql_store_result;
	_MS_mth.num_rows    = mysql_num_rows;
	_MS_mth.num_fields  = mysql_num_fields;
	_MS_mth.fetch_row   = mysql_fetch_row;
	_MS_mth.free_result = mysql_free_result;
	_MS_mth.error       = mysql_error;};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_ERROR - handle MYSQL errors */

static void _MYSQL_error(MYSQL *conn, char *msg)
   {

    printf("MYSQL %s: %s failed\n", msg, _MS_mth.error(conn));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_DB_CLOSE - close the database conection */

static int _MYSQL_db_close(FILE *fp)
   {MYSQL *conn;
    sql_file *sys;

    sys  = (sql_file *) fp;
    conn = (MYSQL *) sys->conn;

    _MS_mth.close(conn);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_SPLIT_ROW - split line S at newlines and add them to OUT */

static void _MYSQL_split_row(SC_array *out, char *s)
   {char *p, *u, *l;

    for (p = s; TRUE; p = NULL)
        {l = SC_strtok(p, "\n", u);
	 if (l != NULL)
	    SC_array_string_add_copy(out, l);
	 else
	    break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_GATHER_ROW - make lines out of the tokens of row IN */

static void _MYSQL_gather_row(SC_array *out, MYSQL_ROW in,
			      int nf, char *delim)
   {int i;
    char s[MAX_BFSZ];

    s[0] = '\0';
    for (i = 0; i < nf; i++)
        {if (in[i][0] == '\0')
	    break;
         strcat(s, in[i]);
	 strcat(s, delim);};

    SC_LAST_CHAR(s) = '\0';

    _MYSQL_split_row(out, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_OPER - do the specified SQL operation */

int _MYSQL_oper(FILE *fp, char *sql)
   {int rv;
    MYSQL *conn;

    conn = GET_CONNECTION(fp);

    if (_MS_mth.query(conn, sql) != 0)
       {_MYSQL_error(conn, "operation");
        printf("-> %s\n", sql);
        rv = FALSE;}

    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_QUERY - do the specified query */

sql_table *_MYSQL_query(FILE *fp, char *sql, char *delim, int add)
   {int nf, nr;
    SC_array *arr;
    MYSQL_RES *res;
    MYSQL_ROW row;
    sql_table *tab;
    MYSQL *conn;

    tab  = NULL;
    conn = GET_CONNECTION(fp);

    if (_MS_mth.query(conn, sql) != 0)
       {_MYSQL_error(conn, "query");
        printf("-> %s\n", sql);}

    else
       {res = _MS_mth.result(conn);
	if (res == NULL)
	   _MYSQL_error(conn, "store");

	else
	   {nr = _MS_mth.num_rows(res);
	    nf = _MS_mth.num_fields(res);

	    tab = _SQL_mk_table(nr, nf);

	    arr = CMAKE_ARRAY(char *, NULL, 0);

	    if (add == TRUE)
	       SC_array_string_add_copy(arr, sql);

/* gather results */
	    while ((row = _MS_mth.fetch_row(res)) != NULL)
	       {nf = _MS_mth.num_fields(res);
		_MYSQL_gather_row(arr, row, nf, delim);};

	    _MS_mth.free_result(res);

	    SC_array_string_add(arr, NULL);

	    CFREE(tab->table);
	    tab->table = SC_array_done(arr);};};

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_TABLES - list the tables for MYSQL */

static sql_table *_MYSQL_tables(FILE *fp)
   {sql_table *tab;

    tab = _MYSQL_query(fp, "show tables;", " ", FALSE);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_DESC - describe a type for MYSQL */

static sql_table *_MYSQL_desc(FILE *fp, char *s)
   {char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "describe %s;", s);

    tab = _MYSQL_query(fp, cmd, " ", TRUE);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_NITEMS - return the number of rows in table S */

static int _MYSQL_nitems(FILE *fp, char *s)
   {int nr;
    char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "select count(*) from %s", s);

    tab = _MYSQL_query(fp, cmd, " ", FALSE);

    nr = SC_stoi(tab->table[0]);

    _SQL_rl_table(tab);

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_SELECT - do a select for MYSQL */

static sql_table *_MYSQL_select(FILE *fp, char *s)
   {char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "select * from %s;", s);

    tab = _MYSQL_query(fp, cmd, "|", FALSE);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

sql_cmd
 _MYSQL_cmd = { _MYSQL_tables, _MYSQL_desc,
		_MYSQL_nitems, _MYSQL_select,
                _MYSQL_oper };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_FILE - return a file_io_desc cast as a FILE for
 *             - use by the IO_ functions in the case of an
 *             - MYSQL connection file
 */

FILE *_MYSQL_file(sql_file *fs)
   {file_io_desc *fid;
    FILE *fp;

    fid         = SC_make_io_desc(fs);
    fid->fclose = _MYSQL_db_close;

    fp = (FILE *) fid;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_OPEN - open an Mysql database */

FILE *_MYSQL_open(PDBfile *file, char *name, char *mode)
   {int st;
    char s[MAXLINE];
    char *hst, *usr, *pwd, *db, *p;
    FILE *fp;
    pcons *pc;
    SC_udl *pu;
    MYSQL *conn;
    sql_file *fs;

    _MYSQL_set_methods();

    fp = NULL;
    pu = file->udl;

    if ((pu->protocol != NULL) && (strcmp(pu->protocol, "mysql") == 0))
       {hst = pu->server;
	db  = pu->path;
        p   = strchr(db, '/');
	if (p == NULL)
	   usr = pu->path;
	else
	   {*p++ = '\0';
	    usr = p;};

	if ((hst == NULL) || (hst[0] == '\0'))
	   hst = "localhost";

	pc = SC_assoc_entry(pu->info, "pwd");

/* if no pwd attribute given assume none required */
	if (pc == NULL)
	   pwd = NULL;

/* if a pwd attribute has a value use it */
	else if (pc->cdr != NULL)
	   pwd = pc->cdr;

/* otherwise prompt for pwd */
	else
           {printf("password: ");
	    fflush(stdout);
	    SC_echo_off_file(stdin);
	    pwd = fgets(s, MAXLINE, stdin);
	    SC_LAST_CHAR(s) = '\0';
	    SC_echo_on_file(stdin);};

	conn = _MS_mth.init(NULL);
	if (_MS_mth.connect(conn, hst, usr, pwd, db, 0, NULL, 0) == NULL)
	   _MYSQL_error(conn, "connect");

        else
	   {st = _MS_mth.query(conn, "SET NAMES 'utf8'");
	    if (st != 0)
	       _MYSQL_error(conn, "set names");

	    fs       = CMAKE(sql_file);
	    fs->conn = (FILE *) conn;
	    fs->sys  = &_MYSQL_cmd;

	    fp = _MYSQL_file(fs);

	    if (fp != NULL)
	       {file->stream = fp;
		 _SQL_sym_chart(file);};};};

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

