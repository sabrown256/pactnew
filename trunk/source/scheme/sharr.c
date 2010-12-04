/*
 * SHARR.C - support for hasharrs in Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/

/*                              HASH ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _SSI_HASH_INSTALL - install at Scheme level
 *                   - (hash-install <name> <object> <table>)
 */

static object *_SSI_hash_install(object *argl)
   {object *obj;
    char *name;
    haelem *hp;
    hasharr *tab;

    name = NULL;
    obj  = SS_null;
    tab  = SS_symtab;
    SS_args(argl,
	    SC_STRING_I, &name,
	    SS_OBJECT_I, &obj,
	    SS_HASHARR_I, &tab,
	    0);

    hp = SC_hasharr_install(tab, name, obj, SS_POBJECT_S, TRUE, TRUE);

/* the hash table has one reference and the object will have another
 * without this the haelem can be freed when obj is GC'd - NOT GOOD!
 */
    SC_mark(hp, 1);

    obj = SS_mk_haelem(hp);

    SFREE(name);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASH_LOOKUP - lookup at the Scheme level */

static object *_SSI_hash_lookup(object *argl)
   {char *name;
    hasharr *tab;
    void *vr;
    object *o;

    name = NULL;
    tab  = SS_symtab;
    SS_args(argl,
	    SC_STRING_I, &name,
	    SS_HASHARR_I, &tab,
	    0);

    vr = SC_hasharr_def_lookup(tab, name);
    if (vr == NULL)
       o = SS_f;
    else
       o = SS_mk_cons(SS_car(argl), vr);

    SFREE(name);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASH_REMOVE - remove at the Scheme level */

static object *_SSI_hash_remove(object *argl)
   {object *obj;
    char *name;
    hasharr *tab;

    name = NULL;
    tab  = SS_symtab;
    SS_args(argl,
	    SC_STRING_I, &name,
	    SS_HASHARR_I, &tab,
	    0);

/* lookup up the object and do a SS_GC on it */
    if (tab == SS_symtab)
       {obj = (object *) SC_hasharr_def_lookup(tab, name);
	if (obj != NULL)
	   SS_GC(obj);};

/* now remove it */
    obj = (SC_hasharr_remove(tab, name)) ? SS_t : SS_f;

    SFREE(name);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_HASH_DUMP - hash-dump at the Scheme level
 *              - (hash-dump <table> <pattern> <sort>)
 */

object *SS_hash_dump(object *argl)
   {int i, nnames;
    char **names, *name, *patt;
    object *obj, *sort, *to;
    hasharr *tab;

    tab  = SS_symtab;
    patt = NULL;
    sort = SS_t;

    SS_args(argl,
            SS_OBJECT_I, &to,
	    SC_STRING_I, &patt,
	    SS_OBJECT_I, &sort,
            0);

    if ((patt != NULL) && (strcmp(patt, "nil") == 0))
       patt = NULL;

/* get the names from hasharr */
    if (SS_hasharrp(to) == TRUE)
       {tab    = SS_GET(hasharr, to);
	names  = SC_hasharr_dump(tab, patt, NULL, SS_true(sort));
	nnames = SC_MEM_GET_N(char *, names) - 1;}

/* get the names from hasharr */
    else
       {hasharr *ta;
	ta    = SS_GET(hasharr, to);
	names = SC_hasharr_dump(ta, patt, NULL, FALSE);
	SC_ptr_arr_len(nnames, names);
	if (SS_true(sort) == TRUE)
	   SC_string_sort(names, nnames);};

/* listify the names and release them */
    obj = SS_null;
    for (i = nnames-1; i >= 0; i--)
        if ((name = names[i]) != NULL)
           {SS_Assign(obj, SS_mk_cons(SS_mk_string(name), obj));};

/* release the pointers */
    SFREE(names);
    SFREE(patt);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASH_INFO - hash-info at the Scheme level */

static object *_SSI_hash_info(object *arg)
   {int64_t ne;
    object *obj, *flg;
    hasharr *tab;

    tab = SS_symtab;
    SS_args(arg,
	    SS_HASHARR_I, &tab,
	    0);

    flg = (tab->docp) ? SS_t : SS_f;
    ne  = SC_hasharr_get_n(tab);

    obj = SS_mk_cons(SS_mk_integer(tab->size), 
                     SS_mk_cons(SS_mk_integer(ne),
                                SS_mk_cons(flg, SS_null)));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MAKE_HASHARR - make-hash-table for Scheme */

static object *_SSI_make_hasharr(object *arg)
   {int sz, typ;
    hasharr *tab;
    object *op;

    sz  = 0;
    typ = SC_arrtype(arg, -1);
    if (typ == SC_INT_I)
       sz = (int) SS_INTEGER_VALUE(arg);
    else if (typ == SC_FLOAT_I)
       sz = (int) SS_FLOAT_VALUE(arg);
    else
       SS_error("BAD ARGUMENT - MAKE-HASH-TABLE", arg);

    tab = SC_make_hasharr(sz, NODOC, SC_HA_NAME_KEY);
    op  = SS_mk_hasharr(tab);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_WR_HASHARR - print a HASH_ARRAY object */

static void _SS_wr_hasharr(object *obj, object *strm)
   {long ne;
    hasharr *tab;

    tab = SS_GET(hasharr, obj);
    ne  = SC_hasharr_get_n(tab);
    PRINT(SS_OUTSTREAM(strm), "<HASH_ARRAY|0x%lx|%d/%d>",
	  tab, ne, tab->size);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_HA_ELEM - clean up the object pointed to by HP */

static int _SS_rl_ha_elem(haelem *hp, void *a)
   {int ok;
    object *o;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &o);
    if (ok == TRUE)
       {SS_GC(o);
	hp->def = NULL;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_HASHARR - clean up a HASH_ARRAY */

static void _SS_rl_hasharr(object *obj)
   {hasharr *tab;

    tab = SS_GET(hasharr, obj);

    SC_free_hasharr(tab, _SS_rl_ha_elem, NULL);

    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_HASHARR - make HASH_ARRAY object */

object *SS_mk_hasharr(hasharr *tb)
   {object *op;

    op = SS_mk_object(tb, SS_HASHARR_I, SELF_EV, NULL,
		      _SS_wr_hasharr, _SS_rl_hasharr);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_WR_HAELEM - print a haelem object */

static void _SS_wr_haelem(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm),
          "<HASH_ELEMENT|%s>", SS_GET(haelem, obj)->name);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_HAELEM - clean up a HAELEM */

static void _SS_rl_haelem(object *obj)
   {haelem *hp;

    hp = SS_GET(haelem, obj);
    SFREE(hp);

    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_HAELEM - make HAELEM object */

object *SS_mk_haelem(haelem *hp)
   {object *op;

    SC_mark(hp, 1);

    op = SS_mk_object(hp, SS_HAELEM_I, SELF_EV, hp->name,
		      _SS_wr_haelem, _SS_rl_haelem);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASHARRP - return #t if the given arg is a HASH_ARRAY */

static object *_SSI_hasharrp(object *arg)
   {object *o;

    o = (SS_hasharrp(arg)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HAELEMP - return #t if the given arg is a HAELEM */

static object *_SSI_haelemp(object *arg)
   {object *o;

    o = (SS_haelemp(arg)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_HASH - install the hasharr primitives for Scheme */

void _SS_inst_hash(void)
   {

    SS_install("hash-dump",
               "Procedure: Return a list of the names in the given hash table",
               SS_nargs,
               SS_hash_dump, SS_PR_PROC);

    SS_install("hash-info",
               "Procedure: Return (<size> <#-elements> <doc?>) for given hash table",
               SS_sargs,
               _SSI_hash_info, SS_PR_PROC);

    SS_install("hash-element?",
               "Procedure: Return #t if the object is a hash-element",
               SS_sargs,
               _SSI_haelemp, SS_PR_PROC);

    SS_install("hash-install",
               "Procedure: Install the given object in the given hash table",
               SS_nargs,
               _SSI_hash_install, SS_PR_PROC);

    SS_install("hash-lookup",
               "Procedure: Look up and return the named object in the given hash table",
               SS_nargs,
               _SSI_hash_lookup, SS_PR_PROC);

    SS_install("hash-remove",
               "Procedure: Remove the named object from the given hash table",
               SS_nargs,
               _SSI_hash_remove, SS_PR_PROC);

    SS_install("hash-table?",
               "Procedure: Return #t if the object is a hash-table",
               SS_sargs,
               _SSI_hasharrp, SS_PR_PROC);

    SS_install("make-hash-table",
               "Procedure: Return a new hash table",
               SS_sargs,
               _SSI_make_hasharr, SS_PR_PROC);


    SS_scheme_symtab = SS_mk_hasharr(SS_symtab);
    SC_hasharr_install(SS_symtab, "system-hash-table", SS_scheme_symtab, SS_POBJECT_S, TRUE, TRUE);
    SS_UNCOLLECT(SS_scheme_symtab);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
