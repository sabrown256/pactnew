/*
 * TRSQLG.C - generic SQL support for SQL spoke in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trsql.h"

#ifdef HAVE_SQL

#define PD_NEXT_LOC(_a, _inc)                                                \
   {*(_a)       += (_inc);                                                   \
    _a ## _offs += (_inc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_MK_TABLE - make and return an SQL_TABLE */

sql_table *_SQL_mk_table(int nr, int nf)
   {int ne;
    sql_table *tab;

    ne = nr*nf;

    tab = CMAKE(sql_table);

    tab->nr    = nr;
    tab->nf    = nf;
    tab->table = CMAKE_N(char *, ne);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_RL_TABLE - free an SQL_TABLE */

void _SQL_rl_table(sql_table *tab)
   {

    if (tab != NULL)
       {SC_free_strings(tab->table);

	CFREE(tab);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_INIT_TYPES - initialize the SQL type table */

void _SQL_init_types(PDBfile *file)
   {

/* character types */
    PD_typedef(file, SC_CHAR_S, "character");
    PD_typedef(file, SC_CHAR_S, "character varying");
    PD_typedef(file, SC_CHAR_S, "varchar");
    PD_typedef(file, SC_CHAR_S, "national character");
    PD_typedef(file, SC_CHAR_S, "nchar");
    PD_typedef(file, SC_CHAR_S, "national character varying");
    PD_typedef(file, SC_CHAR_S, "nvarchar");

/* fixed point types (ok) */
    PD_typedef(file, SC_SHORT_S,     "smallint");
    PD_typedef(file, SC_INT_S,       "serial");
    PD_typedef(file, SC_LONG_LONG_S, "bigint");
    PD_typedef(file, SC_LONG_LONG_S, "bigserial");

/* floating point types (ok) */
    PD_typedef(file, SC_FLOAT_S,  "real");
    PD_typedef(file, SC_DOUBLE_S, "double precision");
    PD_typedef(file, SC_DOUBLE_S, "numeric");
    PD_typedef(file, SC_DOUBLE_S, "decimal");

/* time/date types */
#if 1
    PD_defchr(file, "timestamp", 20, 1, TEXT_ORDER, TRUE);
    PD_defchr(file, "interval",  32, 1, TEXT_ORDER, TRUE);
    PD_defchr(file, "date",      10, 1, TEXT_ORDER, TRUE);
    PD_defchr(file, "time",      10, 1, TEXT_ORDER, TRUE);
    PD_defchr(file, "datetime",  20, 1, TEXT_ORDER, TRUE);
#else
    PD_defix(file, "timestamp", 20, 1, TEXT_ORDER);
    PD_defix(file, "interval",  32, 1, TEXT_ORDER);
    PD_defix(file, "date",      10, 1, TEXT_ORDER);
    PD_defix(file, "time",      10, 1, TEXT_ORDER);
    PD_defix(file, "datetime",  20, 1, TEXT_ORDER);
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_MK_DESCRIPTOR - build a MEMber DEScriptor out of the given string */

memdes *_SQL_mk_descriptor(PDBfile *file, char *member, int defoff)
   {int ie, nb;
    char s[MAXLINE];
    char *ms, *bs, *ns, *p;
    memdes *desc;
    dimdes *nd;
    defstr *dp;

    desc = CMAKE(memdes);

/* get rid of any leading white space or parens */
    p = SC_trim_left(member, " (\t\n\r\f");

/* get the member name */
    ns = p;
    ie = strcspn(ns, " \t");
    p += ie;
    *p++ = '\0';

/* get rid of any intervening white space */
    p = SC_trim_left(p, " \t\n\r\f");

/* get the base type */
    bs = p;
    ie = strcspn(bs, ",() \t\n\r\f");
    p += ie;
    *p++ = '\0';

/* get any display width qualifiers */
    ie = strcspn(p, ",) \t\n\r\f");
    p[ie] = '\0';

/* get size for character types */
    dp = PD_inquire_type(file, bs);
    nb = SC_stoi(p);
    if ((dp != NULL) && (strcmp(dp->type, SC_CHAR_S) != 0))
       nb = 1;

    nd = _PD_mk_dimensions(defoff, nb);

    if (p[0] == '\0')
       snprintf(s, MAXLINE, "%s %s", bs, ns);
    else if (nb > 1)
       snprintf(s, MAXLINE, "%s %s[%d]", bs, ns, nb);
    else
       snprintf(s, MAXLINE, "%s %s", bs, ns);

    ms = CSTRSAVE(s);
    bs = CSTRSAVE(bs);
    ns = CSTRSAVE(ns);
	
    desc->member      = ms;
    desc->type        = bs;
    desc->is_indirect = FALSE;
    desc->base_type   = bs;
    desc->name        = ns;
    desc->dimensions  = nd;

    SC_mark(ms, 1);
    SC_mark(bs, 2);
    SC_mark(ns, 1);
    SC_mark(nd, 1);

    desc->number      = _PD_comp_num(desc->dimensions);
    desc->member_offs = -1L;
    desc->cast_offs   = -1L;
    desc->cast_memb   = NULL;
    desc->next        = NULL;

    return(desc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_WR_DEFSTR - make the defstr from TYPE for FILE */

static int _SQL_wr_defstr(PDBfile *file, char *type)
   {int ok, rv, id;
    char mbr[MAXLINE], qu[MAX_BFSZ];
    memdes *desc;
    defstr *dp;
    sql_table *members;
    sql_cmd *cl;
    FILE *fp;

    fp = file->stream;
    cl = GET_SYSTEM(fp);

    rv = TRUE;

    members = cl->describe(fp, type);
    if ((members == NULL) || (members->nr == 0))
       {dp = PD_inquire_type(file, type);
	if (dp != NULL)
	   {snprintf(qu, MAX_BFSZ, "create table %s\n(", type);

	    for (desc = dp->members; desc != NULL; desc = desc->next)
	        {id = SC_type_id(desc->type, FALSE);
		 if (id == SC_INT_I)
		    snprintf(mbr, MAXLINE, "%s integer,\n", desc->name);

		 else if (id == SC_SHORT_I)
		    snprintf(mbr, MAXLINE, "%s smallint,\n", desc->name);

		 else if (id == SC_LONG_LONG_I)
		    snprintf(mbr, MAXLINE, "%s bigint,\n", desc->name);

		 else if (id == SC_FLOAT_I)
		    snprintf(mbr, MAXLINE, "%s real,\n", desc->name);

		 else if (id == SC_DOUBLE_I)
		    snprintf(mbr, MAXLINE, "%s double precision,\n",
			     desc->name);

		 else if (id == SC_CHAR_I)
		    snprintf(mbr, MAXLINE, "%s varchar(%ld),\n",
			     desc->name, desc->number);

		 else if (strcmp(desc->type, "date") == 0)
		    snprintf(mbr, MAXLINE, "%s date,\n", desc->name);

		 else if (strcmp(desc->type, "time") == 0)
		    snprintf(mbr, MAXLINE, "%s time,\n", desc->name);

		 else if (strcmp(desc->type, "datetime") == 0)
		    snprintf(mbr, MAXLINE, "%s datetime,\n", desc->name);

		 else if (strcmp(desc->type, "timestamp") == 0)
		    snprintf(mbr, MAXLINE, "%s timestamp,\n", desc->name);

		 else if (strcmp(desc->type, "interval") == 0)
		    snprintf(mbr, MAXLINE, "%s interval,\n", desc->name);

		 else
		    PD_error("UNKNOWN TYPE - _SQL_WR_DEFSTR", PD_GENERIC);

		 SC_strcat(qu, MAX_BFSZ, mbr);};

	    SC_LAST_CHAR(qu) = '\0';
	    SC_LAST_CHAR(qu) = '\0';
	    SC_strcat(qu, MAX_BFSZ, ");\n");

	    ok = cl->oper(fp, qu);
	    if (ok == FALSE)
	       rv = FALSE;};};

    _SQL_rl_table(members);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_RD_DEFSTR - make the defstr from TYPE for FILE */

static defstr *_SQL_rd_defstr(PDBfile *file, char *type)
   {int doffs;
    char *s;
    char **sa, **ml, *ptype;
    hasharr *fchrt;
    memdes *desc, *lst, *prev;
    defstr *dp, *dp2;
    sql_table *members;
    sql_cmd *cl;
    PD_smp_state *pa;
    FILE *fp;

    fp = file->stream;
    cl = GET_SYSTEM(fp);

    pa = _PD_get_state(-1);

    members = cl->describe(fp, type);
    ml      = members->table + 1;
    sa      = NULL;
    if (ml[0] == NULL)
       {s  = members->table[0];
	s  = strchr(s, '(') + 1;
	sa = SC_tokenize(s, ",\n");
        ml = sa;};
    
    prev  = NULL;
    lst   = NULL;
    fchrt = file->chart;
    doffs = file->default_offset;
    for (s = *ml++; s != NULL; s = *ml++)
        {desc  = _SQL_mk_descriptor(file, s, doffs);
	 ptype = desc->base_type;
	 if (SC_hasharr_lookup(fchrt, ptype) == NULL)
	    if ((strcmp(ptype, type) != 0) || !_PD_indirection(s))
	       {snprintf(pa->err, MAXLINE,
			 "ERROR: %s BAD MEMBER TYPE - _SQL_RD_DEFSTR\n",
			 s);
		return(NULL);};
         
	 dp2 = PD_inquire_table_type(fchrt, ptype);
	 if ((dp2 != NULL)  && !(_PD_indirection(desc->type)))
	    if (dp2->n_indirects > 0)
	       {snprintf(pa->err, MAXLINE,
			 "ERROR: STATIC MEMBER STRUCT %s CANNOT CONTAIN INDIRECTS\n",
			 ptype);
		return(NULL);};

	 if (lst == NULL)
	    lst = desc;
	 else
	    {prev->next = desc;
	     SC_mark(desc, 1);};

	 prev = desc;};

/* install the type in all charts */
    dp = _PD_defstr_inst(file, type, STRUCT_KIND, lst,
			 TEXT_ORDER, NULL, NULL, PD_CHART_HOST);

    if (dp == NULL)
       PD_error("CAN'T HANDLE PRIMITIVE TYPE - _SQL_RD_DEFSTR", PD_GENERIC);

    SC_free_strings(sa);
    _SQL_rl_table(members);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_MK_ENTRY - make the entry from NAME for FP */

static void _SQL_mk_entry(PDBfile *file, char *name, defstr *dp)
   {long ni;
    syment *ep;
    dimdes *dm;
    sql_cmd *cl;
    FILE *fp;

    fp = file->stream;
    cl = GET_SYSTEM(fp);

    ni = cl->nitems(fp, name);

    dm = _PD_mk_dimensions(1L, ni);
    ep = _PD_mk_syment(name, ni, 0, NULL, dm);

    _PD_e_install(file, name, ep, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_SYM_CHART - make the symbol table and structure chart */

int _SQL_sym_chart(PDBfile *file)
   {int i, nr, rv;
    char *type;
    defstr *dp;
    sql_table *types;
    sql_cmd *cl;
    FILE *fp;

    rv = TRUE;
    fp = file->stream;
    cl = GET_SYSTEM(fp);

    PD_set_offset(file, 1);

    _SQL_init_types(file);

    if (file->mode != PD_CREATE)
       {types = cl->tables(fp);
	nr    = types->nr;
	for (i = 0; i < nr; i++)
	    {type = types->table[i];

/* make the structure chart entry */
	     dp = _SQL_rd_defstr(file, type);

/* make the symbol table entry */
	     _SQL_mk_entry(file, type, dp);};

	_SQL_rl_table(types);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_READ - read an entry */

int _SQL_read(PDBfile *file, char *name, long ni, dimind *dim, void *vr)
   {int rv;
    long i, ne, start, stop, step;
    char qu[MAXLINE];
    char **sa, *s, *out, *in;
    hasharr *chi, *cho;
    defstr *dpo;
    sql_table *entry;
    sql_cmd *cl;
    FILE *fp;

    rv = TRUE;
    fp = file->stream;
    cl = GET_SYSTEM(fp);

    start = dim->start;
    stop  = dim->stop;
    step  = dim->step;

    ne = stop - start + 1;

    snprintf(qu, MAXLINE, "%s limit %ld offset %ld",
	     name, ne, start);

    entry = cl->select(fp, qu);
    sa    = entry->table;

    PD_set_text_delimiter(file, "|");

    chi = file->chart;
    cho = file->host_chart;
    dpo = PD_inquire_table_type(cho, name);
    if (dpo != NULL)
       {in = (char *) vr;

	for (i = 0; i < ne; i += step)
	    {s = sa[i];

	     out = in + i*dpo->size;

	     PD_convert(&out, &s, name, name, 1,
			file->std, file->host_std, file->host_std,
			chi, cho, 0, PD_READ);};};

    _SQL_rl_table(entry);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_WR_ENTRY - write an entry */

static syment *_SQL_wr_entry(PDBfile *file, char *tab, int ni, void *vr)
   {int ok;
    char bf[MAX_BFSZ], qu[MAX_BFSZ];
    char *out, *in;
    syment *ep;
    sql_cmd *cl;
    FILE *fp;

    fp = file->stream;
    cl = GET_SYSTEM(fp);

    ep = NULL;

    out = bf;
    in  = (char *) vr;

    PD_set_text_delimiter(file, ",");

    PD_convert(&out, &in, tab, tab, ni,
	       file->host_std, file->std, file->host_std,
	       file->host_chart, file->chart,
	       0, PD_WRITE);

    SC_LAST_CHAR(bf) = '\0';

    snprintf(qu, MAX_BFSZ, "insert into %s values (%s)", tab, bf);

    ok  = TRUE;
    ok &= cl->oper(fp, "begin");
    ok &= cl->oper(fp, qu);
    ok &= cl->oper(fp, "commit");

    if (ok == TRUE)
       ep = _PD_mk_syment(tab, ni, -1, NULL, NULL);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_WRITE - write an entry */

syment *_SQL_write(PDBfile *file, char *tab, int ni, void *vr)
   {int ok;
    syment *ep;

    ep = NULL;

/* add or verify the type TAB */
    ok = _SQL_wr_defstr(file, tab);

/* if the type is good try writing the entry */
    if (ok == TRUE)
       ep = _SQL_wr_entry(file, tab, ni, vr);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

