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

static object *_SSI_hash_install(SS_psides *si, object *argl)
   {object *obj;
    char *name;
    haelem *hp;
    hasharr *tab;

    name = NULL;
    obj  = SS_null;
    tab  = si->symtab;
    SS_args(si, argl,
	    G_STRING_I, &name,
	    G_OBJECT_I, &obj,
	    G_HASHARR_I, &tab,
	    0);

    hp = SC_hasharr_install(tab, name, obj, G_OBJECT_P_S, 3, -1);

/* the hash table has one reference and the object will have another
 * without this the haelem can be freed when obj is GC'd - NOT GOOD!
 */
    SC_mark(hp, 1);

    obj = SS_mk_haelem(si, hp);

    CFREE(name);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASH_LOOKUP - lookup at the Scheme level */

static object *_SSI_hash_lookup(SS_psides *si, object *argl)
   {char *name;
    hasharr *tab;
    void *vr;
    object *o;

    name = NULL;
    tab  = si->symtab;
    SS_args(si, argl,
	    G_STRING_I, &name,
	    G_HASHARR_I, &tab,
	    0);

    vr = SC_hasharr_def_lookup(tab, name);
    if (vr == NULL)
       o = SS_f;
    else
       o = SS_mk_cons(si, SS_car(si, argl), vr);

    CFREE(name);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASH_REMOVE - remove at the Scheme level */

static object *_SSI_hash_remove(SS_psides *si, object *argl)
   {object *obj;
    char *name;
    hasharr *tab;

    name = NULL;
    tab  = si->symtab;
    SS_args(si, argl,
	    G_STRING_I, &name,
	    G_HASHARR_I, &tab,
	    0);

/* lookup up the object and do a SS_gc on it */
    if (tab == si->symtab)
       {obj = (object *) SC_hasharr_def_lookup(tab, name);
	if (obj != NULL)
	   SS_gc(si, obj);};

/* now remove it */
    obj = (SC_hasharr_remove(tab, name)) ? SS_t : SS_f;

    CFREE(name);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_HASH_DUMP - hash-dump at the Scheme level
 *              - (hash-dump <table> <pattern> <sort>)
 */

object *SS_hash_dump(SS_psides *si, object *argl)
   {int i, nnames;
    char **names, *name, *patt;
    object *obj, *sort, *to;
    hasharr *tab;

    tab  = si->symtab;
    patt = NULL;
    sort = SS_t;

    SS_args(si, argl,
            G_OBJECT_I, &to,
	    G_STRING_I, &patt,
	    G_OBJECT_I, &sort,
            0);

    if ((patt != NULL) && (strcmp(patt, "nil") == 0))
       patt = NULL;

/* get the names from hasharr */
    if (SS_hasharrp(to) == TRUE)
       {tab    = SS_GET(hasharr, to);
	names  = SC_hasharr_dump(tab, patt, NULL, SS_true(sort));
	SC_ptr_arr_len(nnames, names);}

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
           {SS_assign(si, obj, SS_mk_cons(si, SS_mk_string(si, name), obj));};

/* release the pointers */
    CFREE(names);
    CFREE(patt);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASH_INFO - hash-info at the Scheme level */

static object *_SSI_hash_info(SS_psides *si, object *arg)
   {int64_t ne;
    object *obj, *flg;
    hasharr *tab;

    tab = si->symtab;
    SS_args(si, arg,
	    G_HASHARR_I, &tab,
	    0);

    flg = (tab->docp) ? SS_t : SS_f;
    ne  = SC_hasharr_get_n(tab);

    obj = SS_mk_cons(si,
		     SS_mk_integer(si, tab->size), 
                     SS_mk_cons(si,
				SS_mk_integer(si, ne),
                                SS_mk_cons(si, flg, SS_null)));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MAKE_HASHARR - make-hash-table for Scheme */

static object *_SSI_make_hasharr(SS_psides *si, object *arg)
   {int sz, typ;
    hasharr *tab;
    object *op;

    sz  = 0;
    typ = SC_arrtype(arg, -1);
    if (typ == G_INT_I)
       sz = (int) SS_INTEGER_VALUE(arg);
    else if (typ == G_DOUBLE_I)
       sz = (int) SS_FLOAT_VALUE(arg);
    else
       SS_error(si, "BAD ARGUMENT - MAKE-HASH-TABLE", arg);

    tab = SC_make_hasharr(sz, NODOC, SC_HA_NAME_KEY, 0);
    op  = SS_mk_hasharr(si, tab);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_WR_HASHARR - print a HASH_ARRAY object */

static void _SS_wr_hasharr(SS_psides *si, object *obj, object *strm)
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
    SS_psides *si;

    si = (SS_psides *) a;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &o, TRUE);
    if (ok == TRUE)
       SS_gc(si, o);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_HASHARR - clean up a HASH_ARRAY */

static void _SS_rl_hasharr(SS_psides *si, object *obj)
   {hasharr *tab;

    tab = SS_GET(hasharr, obj);

    SC_free_hasharr(tab, _SS_rl_ha_elem, si);

    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_HASHARR - make HASH_ARRAY object */

object *SS_mk_hasharr(SS_psides *si, hasharr *tb)
   {object *op;

    op = SS_mk_object(si, tb, G_HASHARR_I, SELF_EV, NULL,
		      _SS_wr_hasharr, _SS_rl_hasharr);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_WR_HAELEM - print a haelem object */

static void _SS_wr_haelem(SS_psides *si, object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm),
          "<HASH_ELEMENT|%s>", SS_GET(haelem, obj)->name);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_HAELEM - clean up a HAELEM */

static void _SS_rl_haelem(SS_psides *si, object *obj)
   {haelem *hp;

    hp = SS_GET(haelem, obj);
    CFREE(hp);

    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_HAELEM - make HAELEM object */

object *SS_mk_haelem(SS_psides *si, haelem *hp)
   {object *op;

    op = SS_null;

    if (hp != NULL)
       {SC_mark(hp, 1);

	op = SS_mk_object(si, hp, G_HAELEM_I, SELF_EV, hp->name,
			  _SS_wr_haelem, _SS_rl_haelem);};

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HASHARRP - return #t if the given arg is a HASH_ARRAY */

static object *_SSI_hasharrp(SS_psides *si, object *arg)
   {object *o;

    o = (SS_hasharrp(arg)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_HAELEMP - return #t if the given arg is a HAELEM */

static object *_SSI_haelemp(SS_psides *si, object *arg)
   {object *o;

    o = (SS_haelemp(arg)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_HASH - install the hasharr primitives for Scheme */

void _SS_inst_hash(SS_psides *si)
   {

    SS_install(si, "hash-dump",
               "Procedure: Return a list of the names in the given hash table",
               SS_nargs,
               SS_hash_dump, SS_PR_PROC);

    SS_install(si, "hash-info",
               "Procedure: Return (<size> <#-elements> <doc?>) for given hash table",
               SS_sargs,
               _SSI_hash_info, SS_PR_PROC);

    SS_install(si, "hash-element?",
               "Procedure: Return #t if the object is a hash-element",
               SS_sargs,
               _SSI_haelemp, SS_PR_PROC);

    SS_install(si, "hash-install",
               "Procedure: Install the given object in the given hash table",
               SS_nargs,
               _SSI_hash_install, SS_PR_PROC);

    SS_install(si, "hash-lookup",
               "Procedure: Look up and return the named object in the given hash table",
               SS_nargs,
               _SSI_hash_lookup, SS_PR_PROC);

    SS_install(si, "hash-remove",
               "Procedure: Remove the named object from the given hash table",
               SS_nargs,
               _SSI_hash_remove, SS_PR_PROC);

    SS_install(si, "hash-table?",
               "Procedure: Return #t if the object is a hash-table",
               SS_sargs,
               _SSI_hasharrp, SS_PR_PROC);

    SS_install(si, "make-hash-table",
               "Procedure: Return a new hash table",
               SS_sargs,
               _SSI_make_hasharr, SS_PR_PROC);


    SS_scheme_symtab = SS_mk_hasharr(si, si->symtab);
    SC_hasharr_install(si->symtab, "system-hash-table",
		       SS_scheme_symtab, G_OBJECT_P_S, 3, -1);
    SS_UNCOLLECT(SS_scheme_symtab);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
