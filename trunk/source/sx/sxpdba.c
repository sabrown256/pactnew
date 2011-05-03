/*
 * SXPDBA.C - PDB attribute functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEF_ATTR - define an attribute to the PDB file
 *               - Usage: (def-attribute <file> <name> <type>)
 */

static object *_SXI_def_attr(SS_psides *si, object *argl)
   {char *at, *type;
    PDBfile *file;
    g_file *po;
    object *o;

    po   = NULL;
    at   = NULL;
    type = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &at,
            SC_STRING_I, &type,
            0);

    if (po == NULL)
       file = SX_vif;
    else
       file = FILE_FILE(PDBfile, po);

    PD_def_attribute(file, at, type);

    o = SS_cadr(argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_REM_ATTR - remove an attribute from the PDB file
 *               - Usage: (rem-attribute <file> <name>)
 */

static object *_SXI_rem_attr(SS_psides *si, object *argl)
   {char *at;
    PDBfile *file;
    g_file *po;
    object *o;

    po   = NULL;
    at   = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &at,
            0);

    if (po == NULL)
       file = SX_vif;
    else
       file = FILE_FILE(PDBfile, po);

    PD_rem_attribute(file, at);

    o = SS_cadr(argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_ATTR_VAL - set the value of the named attribute for
 *                   - the named variable
 *                   - Usage: (set-attribute-value! <file>
 *                   -                              <name> <attribute> <value>)
 */

static object *_SXI_set_attr_val(SS_psides *si, object *argl)
   {int id;
    char *at, *vr, *data_type, attr_type[MAXLINE];
    void *data;
    PDBfile *file;
    g_file *po;
    g_pdbdata *pv;
    attribute *atr;
    object *obj;

    po = NULL;
    at = NULL;
    vr = NULL;
    pv = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &vr,
            SC_STRING_I, &at,
            SS_OBJECT_I, &obj,
            0);

    if (po == NULL)
       file = SX_vif;
    else
       file = FILE_FILE(PDBfile, po);

    atr = PD_inquire_attribute(file, at, NULL);
    if (atr == NULL)
       return(SS_f);

    strcpy(attr_type, atr->type);
    PD_dereference(attr_type);
    PD_dereference(attr_type);

    data = NULL;

    id = SC_type_id(attr_type, FALSE);

    if (SX_PDBDATAP(obj))
       {pv = SS_GET(g_pdbdata, obj);

        data_type = pv->ep->type;
        if (strcmp(data_type, attr_type) != 0)
           SS_error(si, "ATTRIBUTE TYPE NOT VALUE TYPE - _SXI_SET_ATTR_VAL",
                    argl);

        data = pv->data;}

    else if ((id == SC_STRING_I) && (SS_stringp(obj)))
        {char **cv;

         cv   = CMAKE(char *);
         *cv  = CSTRSAVE(SS_STRING_TEXT(obj));
         data = (void *) cv;}

    else if (id == SC_CHAR_I)
        {char *cv;

         cv = CMAKE(char);
         if (SS_charobjp(obj))
            *cv = SS_CHARACTER_VALUE(obj);
         else
            *cv = *SS_STRING_TEXT(obj);

         data = (void *) cv;}

    else if ((id == SC_INT_I) && (SS_numbp(obj)))
        {int *iv;

         iv = CMAKE(int);
         if (SS_integerp(obj))
            *iv = SS_INTEGER_VALUE(obj);
         else 
            *iv = SS_FLOAT_VALUE(obj);

         data = (void *) iv;}

    else if ((id == SC_DOUBLE_I) && (SS_numbp(obj)))
        {double *dv;

         dv = CMAKE(double);
         if (SS_integerp(obj))
            *dv = SS_INTEGER_VALUE(obj);
         else 
            *dv = SS_FLOAT_VALUE(obj);

         data = (void *) dv;}

    else
       SS_error(si, "BAD ATTRIBUTE VALUE - _SXI_SET_ATTR_VAL", obj);

    SC_mark(data, 1);
    PD_set_attribute(file, vr, at, data);

    obj = SS_car(SS_cdddr(argl));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_ATTR_VAL - get the value of the named attribute for
 *                   - the named variable
 *                   - Usage: (get-attribute-value <file>
 *                   -                             <name> <attribute>)
 */

static object *_SXI_get_attr_val(SS_psides *si, object *argl)
   {char *at, *vr, attr_type[MAXLINE];
    PDBfile *file;
    g_file *po;
    SC_address data;
    attribute *atr;
    syment *ep;
    object *o;

    po = NULL;
    at = NULL;
    vr = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &vr,
            SC_STRING_I, &at,
            0);

    if (po == NULL)
       file = SX_vif;
    else
       file = FILE_FILE(PDBfile, po);

    atr = PD_inquire_attribute(file, at, NULL);
    if (atr == NULL)
       return(SS_f);

    strcpy(attr_type, atr->type);
    PD_dereference(attr_type);
    PD_dereference(attr_type);

    data.memaddr = PD_get_attribute(file, vr, at);
    if (data.memaddr == NULL)
       o = SS_null;

    else
       {SC_mark(data.memaddr, 1);
	ep = _PD_mk_syment(attr_type, 1, data.diskaddr, NULL, NULL);
	o  = _SX_mk_gpdbdata(si, at, data.memaddr, ep, file);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LST_ATTR - return a list of the names of the attributes in
 *               - the given file
 *               - Usage: (list-attributes <file>)
 */

static object *_SXI_lst_attr(SS_psides *si, object *argl)
   {int n;
    long i, ne;
    char *ty, *name, **entries;
    object *obj;
    hasharr *tab;
    PDBfile *file;
    g_file *po;
    attribute *attr;

    po = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            0);

    if (po == NULL)
       file = SX_vif;
    else
       file = FILE_FILE(PDBfile, po);

    tab = file->attrtab;
    if (tab == NULL)
       return(SS_null);

    ne      = SC_hasharr_get_n(tab);
    entries = CMAKE_N(char *, ne);
    if (entries == NULL)
       SS_error(si, "CAN'T ALLOCATE MEMORY - _SXI_LST_ATTR", SS_null);

/* fill in the list of pointers to names in the hash table */
    n = 0;
    for (i = 0; SC_hasharr_next(tab, &i, &name, &ty, (void **) &attr); i++)
        {if (strcmp(ty, "attribute *") == 0)
	    {if (attr != NULL)
	        {if (attr->data != NULL)
		    entries[n++] = CSTRSAVE(name);};};};

    CREMAKE(entries, char *, n);

/* listify the names and release them */
    obj = SS_null;
    for (i = n-1; i >= 0; i--)
        {name = entries[i];
         if (name != NULL)
            {SS_Assign(obj, SS_mk_cons(si, SS_mk_string(si, name), obj));};

         CFREE(name);};

/* release the pointers */
    CFREE(entries);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_PDB_ATTR_FUNCS - install the PDB attribute functionality
 *                           - in SX
 */
 
void SX_install_pdb_attr_funcs(SS_psides *si)
   {

    SS_install(si, "def-attribute",
               "Define an attribute to a PDB file",
               SS_nargs,
               _SXI_def_attr, SS_PR_PROC);

    SS_install(si, "rem-attribute",
               "Remove the definition of an attribute from a PDB file",
               SS_nargs,
               _SXI_rem_attr, SS_PR_PROC);

    SS_install(si, "set-attribute-value!",
               "Set the value of attribute for a variable",
               SS_nargs,
               _SXI_set_attr_val, SS_PR_PROC);

    SS_install(si, "get-attribute-value",
               "Get the value of attribute for a variable",
               SS_nargs,
               _SXI_get_attr_val, SS_PR_PROC);

    SS_install(si, "list-attributes",
               "List the attributes known to the file",
               SS_nargs,
               _SXI_lst_attr, SS_PR_PROC);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
