/*
 * TRSQL.H - the SQL support header for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SQL

#include "cpyright.h"
 
#include "../../pdb_int.h"

#define PCK_SQL

#ifdef HAVE_SQL

/*--------------------------------------------------------------------------*/

/*                              DEFINED CONSTANTS                           */

/*--------------------------------------------------------------------------*/

#define GET_SQL_FILE(_f)   ((sql_file *) ((file_io_desc *)(_f))->info)
#define GET_SYSTEM(_f)     (GET_SQL_FILE(_f)->sys)

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

typedef struct s_sql_table sql_table;
typedef struct s_sql_cmd sql_cmd;
typedef struct s_sql_file sql_file;

struct s_sql_table
   {int nr;
    int nf;
    char **table;};

struct s_sql_cmd
   {sql_table *(*tables)(FILE *fp);
    sql_table *(*describe)(FILE *fp, const char *s);
    int (*nitems)(FILE *fp, const char *s);
    sql_table *(*select)(FILE *fp, const char *s);
    int (*oper)(FILE *fp, const char *sql);};

struct s_sql_file
   {FILE *conn;
    sql_cmd *sys;};

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern char
 *SQL_DATABASE_S;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* TRSQLITE.C declarations */

extern FILE
 *_SQLITE_open(PDBfile *file, const char *name, const char *mode);


/* TRMYSQL.C declarations */

extern FILE
 *_MYSQL_open(PDBfile *file, const char *name, const char *mode);


/* TRSQLG.C declarations */

extern sql_table
 *_SQL_mk_table(int nr, int nf);

extern void
 _SQL_rl_table(sql_table *tab);

extern int
 _SQL_sym_chart(PDBfile *fp),
 _SQL_read(PDBfile *fp, const char *name,
	   long ni, dimind *dim, void *vr);

extern syment
 *_SQL_write(PDBfile *file, const char *tab, int ni, void *vr);

#endif

#endif

