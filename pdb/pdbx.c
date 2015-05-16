/*
 * PDBX.C - a set of higher level library routines on top of PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "pgs_gen.h"

#define PG_SPACEDM 3

char
 *ATTRIBUTE = NULL,
 *ATTRIBUTE_VALUE = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MK_ATTRIBUTE - initialize an attribute with identifier AT and
 *                 - type TYPE
 *
 * #bind PD_mk_attribute fortran() scheme() python()
 */

attribute *PD_mk_attribute(const char *at, const char *type)
   {int i;
    attribute *attr;

    attr = CMAKE(attribute);

    attr->name = CSTRSAVE(at);
    attr->type = CSTRSAVE(type);
    attr->data = CMAKE_N(void *, 50L);
    attr->size = 50L;
    attr->indx = 0L;

    for (i = 0; i < attr->size; i++)
        attr->data[i] = NULL;

    return(attr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_ATTR_DATA - release the attribute data */

static void _PD_rl_attr_data(attribute *attr)
   {long i, indx;
    void **data;

    data = attr->data;
    if (data != NULL)
       {indx = attr->indx;
        for (i = 0; i < indx; i++)
            CFREE(data[i]);

        CFREE(attr->data);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_ATTRIBUTE - release the storage associated with an attribute */

void _PD_rl_attribute(attribute *attr)
   {

    _PD_rl_attr_data(attr);

    CFREE(attr->name);
    CFREE(attr->type);
    CFREE(attr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_ATTRIBUTE_VALUE - make an attribute_value instance */

static attribute_value *_PD_mk_attribute_value(attribute *attr)
   {attribute_value *avl;

    avl        = CMAKE(attribute_value);
    avl->attr  = attr;
    avl->indx  = (attr->indx)++;
    avl->next  = NULL;

    return(avl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_ATTRIBUTE_VALUE - release an attribute_value instance */

static void _PD_rl_attribute_value(attribute_value *avl)
   {attribute_value *pa, *nxt;

    for (pa = avl; pa != NULL; pa = nxt)
        {nxt = pa->next;
	 CFREE(pa);};

    return;}

/*--------------------------------------------------------------------------*/

/*                         ATTRIBUTE API FUNCTIONS                          */

/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_ATTRIBUTE - look up the table entry for the named attribute
 *
 * #bind PD_inquire_attribute fortran() scheme() python()
 */

attribute *PD_inquire_attribute(const PDBfile *file ARG(,,cls),
				const char *name, char *path)
   {haelem *hp;
    attribute *at;

    hp = PD_inquire_symbol(file, name, FALSE, path, file->attrtab);
    
    at = (hp == NULL) ? NULL : (attribute *) hp->def;
	  
    return(at);}
	  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_ATTRIBUTE_VALUE - look up the table entry for
 *                            - the named attribute value
 *
 * #bind PD_inquire_attribute_value fortran() scheme() python()
 */

attribute_value *PD_inquire_attribute_value(const PDBfile *file ARG(,,cls),
					    const char *name, char *path)
   {haelem *hp;
    attribute_value *avl;

    hp = PD_inquire_symbol(file, name, FALSE, path, file->attrtab);
    
    avl = (hp == NULL) ? NULL : (attribute_value *) hp->def;
	  
    return(avl);}
	  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REL_ATTR - release an attribute entry in an attribute table */

static int _PD_rel_attr(haelem *hp, void *a)
   {attribute *attr;
    attribute_value *avl;
    
    if ((ATTRIBUTE != NULL) &&
	(strcmp(hp->type, ATTRIBUTE) == 0))
       {attr = (attribute *) hp->def;
	_PD_rl_attribute(attr);}

    else if ((ATTRIBUTE_VALUE != NULL) &&
	     (strcmp(hp->type, ATTRIBUTE_VALUE) == 0))
       {avl = (attribute_value *) hp->def;
	_PD_rl_attribute_value(avl);};

    hp->def = NULL;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_REL_ATTR_TABLE - release the attribute table of FILE
 *                   - if it exists
 *
 * #bind PD_rel_attr_table fortran() scheme() python()
 */

void PD_rel_attr_table(PDBfile *file ARG(,,cls))
   {
    
    if (file->attrtab != NULL)
       {SC_free_hasharr(file->attrtab, _PD_rel_attr, NULL);
	file->attrtab = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEF_ATTRIBUTE - define an attribute by specifying its type
 *                  - return TRUE if successful and FALSE otherwise
 *
 * #bind PD_def_attribute fortran() scheme() python()
 */

int PD_def_attribute(PDBfile *file ARG(,,cls),
		     const char *at, const char *type)
   {char atype[MAXLINE], path[MAXLINE];
    attribute *attr;

    if (SC_hasharr_lookup(file->chart, "attribute") == NULL)
       PD_def_attr_str(file);
  
    if (file->attrtab == NULL)
       {file->attrtab   = SC_make_hasharr(HSZSMINT, NODOC, SC_HA_NAME_KEY, 0);
        ATTRIBUTE       = CSTRDUP("attribute *", 3);
        ATTRIBUTE_VALUE = CSTRDUP("attribute_value *", 3);};

    if (SC_LAST_CHAR(type) == '*')
       snprintf(atype, MAXLINE, "%s**", type);
    else
       snprintf(atype, MAXLINE, "%s **", type);

/* we can leak a lot of memory if we don't check this!! */
    attr = PD_inquire_attribute(file, at, path);
    if (attr != NULL)
       {SC_hasharr_remove(file->attrtab, path);
        PD_rem_attribute(file, path);};

    attr = PD_mk_attribute(path, atype);
    SC_hasharr_install(file->attrtab, path, attr, ATTRIBUTE, 3, -1);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_REM_ATTRIBUTE - remove an attribute and all variables values for
 *                  - the specified attribute
 *                  - return TRUE if successful and FALSE otherwise
 *                  - NOTE: do not actually remove the attribute from the
 *                  -       table the NULL data pointer is used to tell the
 *                  -       attribute value lookup operation that the
 *                  -       attribute no longer exists
 *
 * #bind PD_rem_attribute fortran() scheme() python()
 */

int PD_rem_attribute(PDBfile *file ARG(,,cls), const char *at)
   {attribute *attr;

    attr = PD_inquire_attribute(file, at, NULL);
    if (attr == NULL)
       return(TRUE);

    if (file->mode == PD_OPEN)
       {PD_error("FILE OPENED READ ONLY - PD_REM_ATTRIBUTE", PD_GENERIC);
        return(FALSE);};

    _PD_rl_attr_data(attr);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_ATTRIBUTE - set the value of the specified attribute for the
 *                  - specified variable
 *                  - return TRUE if successful and FALSE otherwise
 *
 * #bind PD_set_attribute fortran() scheme(pd-set-attribute!) python()
 */

int PD_set_attribute(PDBfile *file ARG(,,cls),
		     const char *vr, const char *at, void *vl)
   {int i;
    long indx;
    attribute *attr;
    attribute_value *avl;
    void **data;
    haelem *hp;
    char fullname[MAXLINE];

    attr = PD_inquire_attribute(file, at, NULL);
    if (attr == NULL)
       return(FALSE);

    data = attr->data;
    if (data != NULL)
       {indx = attr->indx;
        data[indx] = vl;
	SC_mark(vl, 1);

	avl = _PD_mk_attribute_value(attr);}

    else
       {PD_error("ATTRIBUTE DOESN'T EXIST - PD_SET_ATTRIBUTE", PD_GENERIC);
        return(FALSE);};

/* adjust the size of the attribute value data array */
    if (attr->indx >= attr->size)
       {attr->size += 50L;
        CREMAKE(attr->data, void *, attr->size);

/* clear out new data pointers */
        for (i = attr->indx; i < attr->size; i++)
            attr->data[i] = NULL;};

/* cons the new attribute value on if this is not the first
 * attribute for the variable
 */
    hp = PD_inquire_symbol(file, vr, FALSE, fullname, file->attrtab);
    if (hp != NULL)
       {avl->next = (attribute_value *) hp->def;
        hp->def   = (void *) avl;}

/* otherwise create the attribute value list for the variable */
    else
       {avl->next =  NULL;

	if (ATTRIBUTE_VALUE == NULL)
	   ATTRIBUTE_VALUE = CSTRSAVE("attribute_value *");

        SC_hasharr_install(file->attrtab, fullname, avl,
			   ATTRIBUTE_VALUE, 3, -1);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_ATTRIBUTE - get the value of the specified attribute for the
 *                  - specified variable
 *                  - return a pointer to the attribute value if successful
 *                  - and NULL otherwise
 *
 * #bind PD_get_attribute fortran() scheme() python()
 */

void *PD_get_attribute(const PDBfile *file ARG(,,cls),
		       const char *vr, const char *at)
   {char fat[MAXLINE], favl[MAXLINE];
    char *vt;
    void *vl, **data;
    attribute *attr;
    attribute_value *avl;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    attr = PD_inquire_attribute(file, at, fat);
    if (attr == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ATTRIBUTE %s DOESN'T EXIST - PD_GET_ATTR", at);
        return(NULL);};

    avl = PD_inquire_attribute_value(file, vr, favl);
    if (avl == NULL)
       {snprintf(pa->err, MAXLINE,
                "VARIABLE %s HAS NO ATTRIBUTES - PD_GET_ATTR",
                favl);
        return(NULL);};

    data = attr->data;
    if (data == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ATTRIBUTE DATA %s DOESN'T EXIST - PD_GET_ATTR",
		 at);
        return(NULL);};

    for ( ; avl != NULL; avl = avl->next)
        {vt = avl->attr->name;
	 if ((strcmp(at, vt) == 0) || (strcmp(fat, vt) == 0))
           break;};

    if (avl == NULL)
       {snprintf(pa->err, MAXLINE,
		 "VARIABLE %s DOESN'T HAVE ATTRIBUTE %s - PD_GET_ATTR",
		 vr, fat);
        return(NULL);};

    vl = data[avl->indx];

    return(vl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEF_PDB_TYPES - define in one place PDB data types for PDB files
 *
 * #bind PD_def_pdb_types fortran() scheme() python()
 */

int PD_def_pdb_types(PDBfile *file ARG(,,cls))
   {int err;

    err = TRUE;

/* define the SC_array */
    err &= G_SC_ARRAY_D(file);

/* define the deprecated SC_dynamic_array */
    err &= G_SC_DYNAMIC_ARRAY_D(file);

    if (_PD_block_define(file) == FALSE)
       return(FALSE);

    err &= (G_PD_CHECKSUM_MODE_E(file) != NULL);
    err &= (G_PD_MAJOR_OP_E(file) != NULL);
    err &= (G_PD_MAJOR_ORDER_E(file) != NULL);
    err &= (G_PD_DELAY_MODE_E(file) != NULL);
    err &= (G_PD_DATA_LOCATION_E(file) != NULL);

    err &= G_MULTIDES_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE MULTIDES - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_DIMDES_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE DIMDES - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_MEMDES_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE MEMDES - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_SYMINDIR_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE SYMINDIR - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_SYMENT_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE SYMENT - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_CHARDES_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE CHARDES - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_FIXDES_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE FIXDES - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_FPDES_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE FPDES - PD_DEF_PDB_TYPES", PD_GENERIC);

    err &= G_DEFSTR_D(file);
    if (err == FALSE)
       PD_error("COULDN'T DEFINE DEFSTR - PD_DEF_PDB_TYPES", PD_GENERIC);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEF_HASH_TYPES - define in one place hash table types for PDB files
 *                   - FLAG controls which structs get defined
 *                   -  bit 1   define haelem
 *                   -  bit 2   define hasharr
 *
 * #bind PD_def_hash_types fortran() scheme() python()
 */

int PD_def_hash_types(PDBfile *file ARG(,,cls), int flag)
   {int err;

    err = TRUE;

    if (flag & 0x1)
       err &= G_HAELEM_D(file);

    if (flag & 0x2)
       {err &= G_SC_ARRAY_D(file);
	err &= G_HASHARR_D(file);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEF_ATTR_STR - define hashing, attribute, attribute value
 *                 - structures
 *
 * #bind PD_def_attr_str fortran() scheme() python()
 */

int PD_def_attr_str(PDBfile *file ARG(,,cls))
   {int err;

/* hash table types */
    err = PD_def_hash_types(file, 3);

/* attribute types */
    err &= G_ATTRIBUTE_D(file);
    if (err == FALSE)
       {PD_error("COULDN'T DEFINE ATTRIBUTE - _PD_DEF_ATTR_STR", PD_GENERIC);
        return(FALSE);};

    err &= G_ATTRIBUTE_VALUE_D(file);
    if (err == FALSE)
       {PD_error("COULDN'T DEFINE ATTRIBUTE_VALUE - _PD_DEF_ATTR_STR", PD_GENERIC);
        return(FALSE);};
    
    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CHECK_OLD_HASHTAB - convert old style HASHTAB from older PDB files
 *                       - NOTE: this is new since Jan. 2002
 */

static void _PD_check_old_hashtab(PDBfile *file)
   {hasharr *atab;

#ifdef HAVE_HASHTAB
    int cnv;
    defstr *dp;
    memdes *desc;
    struct oHASHTAB                 
       {int size;
	int nelements;
	int docp;
	haelem **table;} *oht;

    dp = PD_inquire_type(file, "HASHTAB");
    if (dp != NULL)
       {cnv = TRUE;
        for (desc = dp->members; desc != 0; desc = desc->next)
	    {if (strcmp(desc->member, "function hash") == 0)
		{cnv = FALSE;
		 break;};};

	if (cnv == TRUE)
	   {oht  = (struct oHASHTAB *) file->attrtab;
	    atab = CMAKE(hasharr);

	    atab->size      = oht->size;
	    atab->ne        = oht->nelements;
	    atab->docp      = oht->docp;
	    atab->table     = oht->table;
	    atab->key_type  = NULL;
	
	    file->attrtab = atab;

	    CFREE(oht);

	    _PD_rl_defstr(dp);
	    _PD_remove_type(file, "HASHTAB");

	    PD_def_hash_types(file, 2);};};
#endif

/* whether or not it was converted, it has been read from a file
 * so re-establish the key strategy
 */
    atab = file->attrtab;
    SC_hasharr_rekey(atab, SC_HA_NAME_KEY);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CHECK_OLD_HASHEL - convert old style haelem from older PDB files */

static void _PD_check_old_hashel(PDBfile *file)
   {

#ifdef HAVE_HASHTAB
    int i, sz, cnv;
    defstr *dp;
    hashel *np, *prev;
    memdes *desc;
    struct ohashel                 
      {char *name;
       char *type;
       void *def;
       struct ohashel *next;} *onp, *nxt, **otb;
    hasharr *atab;

    dp = PD_inquire_type(file, "hashel");
    if (dp != NULL)
       {cnv = TRUE;
	for (desc = dp->members; desc != 0; desc = desc->next)
	    {if (strcmp(desc->member, "int free") == 0)
		{cnv = FALSE;
		 break;};};

	if (cnv == TRUE)
	   {atab = file->attrtab;
	    sz   = atab->size;
	    otb  = (struct ohashel **) atab->table;
	    for (i = 0; i < sz; i++)
	        {prev = NULL;
		 for (onp = otb[i]; onp!= NULL; onp = nxt)
		     {np = CMAKE(hashel);
		      np->name = onp->name;
		      np->type = onp->type;
		      np->def  = onp->def;
		      np->free = TRUE;
		      np->next = NULL;
		      if (prev == NULL)
			 otb[i] = (struct ohashel *) np;
		      else
			 prev->next = np;
		      prev = np;
		      nxt  = onp->next;
		      CFREE(onp);};};

	    _PD_rl_defstr(dp);
	    _PD_remove_type(file, "hashel");

	    PD_def_hash_types(file, 1);};};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONVERT_ATTRTAB - convert old format attribute table to new format */

void _PD_convert_attrtab(PDBfile *file)
   {

/* handle old HASHTAB conversion */
    _PD_check_old_hashtab(file);

/* handle old hashel conversion */
    _PD_check_old_hashel(file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONTAINS_INDIRECTIONS - check the specified type to see it is an
 *                           - indirection or in the case of structs contains
 *                           - an indirection (recursive)
 *                           - return TRUE if so and FALSE if not
 */

int _PD_contains_indirections(hasharr *tab, char *type)
   {char *memb_type;
    memdes *desc;
    defstr *dp;

/* check to see if it is an indirection */
    if (_PD_indirection(type))
        return(TRUE);

/* check to see if it is a known type */
    dp = PD_inquire_table_type(tab, type);
    if (dp == NULL)
       PD_error("BAD TYPE - _PD_CONTAINS_INDIRECTIONS", PD_WRITE);

/* check the members for indirections */
    else
       {for (desc = dp->members; desc != NULL; desc = desc->next)
	    {memb_type = desc->type;
	     if (_PD_indirection(memb_type))
	        return(TRUE);

	     if (_PD_contains_indirections(tab, memb_type))
	        return(TRUE);};};

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_PDB_CURVE - read an Ultra curve
 *                   - fill the label
 *                   - set the number of points
 *                   - fill the x and y extrema values
 *                   - fill the x and y arrays depending on value of flag
 *
 * #bind PD_read_pdb_curve fortran() scheme() python()
 */

int PD_read_pdb_curve(PDBfile *fp ARG(,,cls),
		      const char *name, double **pxp, double **pyp,
		      int *pn, char *label,
		      double *pxmn, double *pxmx, double *pymn, double *pymx,
		      PD_curve_io flag)
   {int n, rv;
    double *x[PG_SPACEDM];
    double extr[2];
    char *s, *t;
    char desc[MAXLINE], prefix[MAXLINE];
    char labl[MAXLINE], npts[MAXLINE], xval[MAXLINE], yval[MAXLINE];
    char txex[MAXLINE], tyex[MAXLINE];

    rv = TRUE;

    prefix[0] = '\0';
    if (PD_has_directories(fp))
       {s = strrchr(name, '/');
	if (s != NULL)
	   {n = s - name + 1;
	    SC_strncpy(prefix, MAXLINE, name, n);};};

    if (!PD_read(fp, name, desc))
       return(FALSE);

/* extract the names of the label, x array, and y array */
    s = SC_firsttok(desc, "|");
    if (s == NULL)
       return(FALSE);

    snprintf(labl, MAXLINE, "%s%s", prefix, s);

/* get the label */
    if (!PD_read(fp, labl, label))
       {PD_error("BAD LABEL - PD_READ_PDB_CURVE", PD_GENERIC);
        return(FALSE);};

    s = SC_firsttok(desc, "|");
    if (s == NULL)
       return(FALSE);
    snprintf(npts, MAXLINE, "%s%s", prefix, s);

/* get the number of points */
    if (!PD_read(fp, npts, &n))
       {PD_error("BAD NUMBER OF POINTS - PD_READ_PDB_CURVE", PD_GENERIC);
        return(FALSE);};

    *pn = n;

    s = SC_firsttok(desc, "|");
    if (s == NULL)
       return(FALSE);
    snprintf(xval, MAXLINE, "%s%s", prefix, s);

    s = SC_firsttok(desc, "|");
    if (s == NULL)
       return(FALSE);
    snprintf(yval, MAXLINE, "%s%s", prefix, s);

    s = SC_firsttok(desc, "|");
    t = SC_firsttok(desc, "|");

    *pxmn =  HUGE;
    *pxmx = -HUGE;
    *pymn =  HUGE;
    *pymx = -HUGE;
    if ((s != NULL) && (t != NULL))
       {snprintf(txex, MAXLINE, "%s%s", prefix, s);
	snprintf(tyex, MAXLINE, "%s%s", prefix, t);

        if (!PD_read_as(fp, tyex, G_DOUBLE_S, extr))
           {PD_error("BAD Y EXTREMA - PD_READ_PDB_CURVE", PD_GENERIC);
            return(FALSE);};

        *pymn = extr[0];
        *pymx = extr[1];

	if (!PD_read_as(fp, txex, G_DOUBLE_S, extr))
           {PD_error("BAD X EXTREMA - PD_READ_PDB_CURVE", PD_GENERIC);
            return(FALSE);};

        *pxmn = extr[0];
        *pxmx = extr[1];}

/* if there are no extrema force the curve data to be read
 * even override the flag setting!
 */
    else
       flag = X_AND_Y;

/* get the x array if requested or forced by lack of extrema */
    if ((flag == X_AND_Y) || (flag == X_ONLY))
       {x[0] = CMAKE_N(double, n);
        if (x[0] == NULL)
           {PD_error("INSUFFICIENT MEMORY - PD_READ_PDB_CURVE", PD_GENERIC);
            return(FALSE);};

        *pxp = x[0];

	if (!PD_read_as(fp, xval, G_DOUBLE_S, x[0]))
	   {PD_error("BAD X ARRAY - PD_READ_PDB_CURVE", PD_GENERIC);
	    return(FALSE);};}
    else
       *pxp = NULL;

/* get the y array if requested or forced by lack of extrema */
    if ((flag == X_AND_Y) || (flag == Y_ONLY))
       {x[1] = CMAKE_N(double, n);
        if (x[1] == NULL)
           {PD_error("INSUFFICIENT MEMORY - PD_READ_PDB_CURVE", PD_GENERIC);
            rv = FALSE;}

	else
	   {*pyp = x[1];

	    if (!PD_read_as(fp, yval, G_DOUBLE_S, x[1]))
	       {PD_error("BAD Y ARRAY - PD_READ_PDB_CURVE", PD_GENERIC);
		rv = FALSE;};};}
    else
       *pyp = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_PDB_CURVE - write the curve as specified by a label,
 *                  - number of points, x values, and y values
 *                  - out to the given file
 *                  - as the icurve'th curve in the file
 *                  - NOTE: the order of the writes is crucial for
 *                  - performance for remote files
 *
 * #bind PD_wrt_pdb_curve fortran() scheme() python()
 */

int PD_wrt_pdb_curve(PDBfile *fp ARG(,,cls), const char *labl, int n,
		     double *px, double *py, int icurve)
   {int i, *np;
    char name[MAXLINE], desc[MAXLINE], *dp, *lp;
    double *x[PG_SPACEDM], *xext, *yext;

/* make a curve entry consisting of:
 *    the name of the variable with the label;
 *    the name of the variable with the number of points for this curve
 *    the name of the variable with the x array
 *    the name of the variable with the y array
 *    the name of the variable with the x extrema
 *    the name of the variable with the y extrema
 */
    snprintf(desc, MAXLINE, "|labl%d|npts%d|xval%d|yval%d|xext%d|yext%d|",
	    icurve, icurve, icurve, icurve, icurve, icurve);

/* for the virtual internal file it is necessary to
 * dynamically allocate the variables to be written.
 * for convenience external files are handled similarly
 * and then the space is freed later
 */
    dp  = CSTRSAVE(desc);
    lp  = CSTRSAVE(labl);
    np  = CMAKE(int);
    *np = n;
    x[0]  = CMAKE_N(double, n);
    x[1]  = CMAKE_N(double, n);
    for (i = 0; i < n; i++)
        {x[0][i] = px[i];
         x[1][i] = py[i];};
    xext = CMAKE_N(double, 2);
    yext = CMAKE_N(double, 2);
    PM_maxmin(x[0], &xext[0], &xext[1], n);
    PM_maxmin(x[1], &yext[0], &yext[1], n);

/* use len to effect the cast from size_t to int */
    snprintf(name, MAXLINE, "curve%04d(%d)", icurve, (int) strlen(dp) + 1);
    if (!PD_write(fp, name, G_CHAR_S, dp))
       return(FALSE);

/* save the curve label */
    snprintf(name, MAXLINE, "labl%d(%d)", icurve, (int) strlen(labl) + 1);
    if (!PD_write(fp, name, G_CHAR_S, lp))
       return(FALSE);

/* save the number of points */
    snprintf(name, MAXLINE, "npts%d", icurve);
    if (!PD_write(fp, name, G_INT_S, np))
       return(FALSE);

/* save the x and y arrays and the extrema */
    snprintf(name, MAXLINE, "yext%d(2)", icurve);
    if (!PD_write(fp, name, G_DOUBLE_S, yext))
       return(FALSE);

    snprintf(name, MAXLINE, "yval%d(%d)", icurve, n);
    if (!PD_write(fp, name, G_DOUBLE_S, x[1]))
       return(FALSE);

    snprintf(name, MAXLINE, "xext%d(2)", icurve);
    if (!PD_write(fp, name, G_DOUBLE_S, xext))
       return(FALSE);

    snprintf(name, MAXLINE, "xval%d(%d)", icurve, n);
    if (!PD_write(fp, name, G_DOUBLE_S, x[0]))
       return(FALSE);

    if (!fp->virtual_internal)
       {CFREE(dp);
        CFREE(lp);
	CFREE(np);
        CFREE(xext);
        CFREE(yext);
        CFREE(x[0]);
        CFREE(x[1])};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_PDB_CURVE_Y - write the curve as specified by a label,
 *                    - number of points, and y values
 *                    - out to the given file
 *                    - as the icurve'th curve in the file
 *                    - the x values are to be taken from the specified
 *                    - curve
 *                    - GOTCHA: at this point no checking is done to
 *                    - see whether the number of points for the specified
 *                    - x values is correct
 *                    - NOTE: the order of the writes is crucial for
 *                    - performance for remote files
 *
 * #bind PD_wrt_pdb_curve_y fortran() scheme() python()
 */

int PD_wrt_pdb_curve_y(PDBfile *fp ARG(,,cls),
		       const char *labl, int n, int ix,
		       double *py, int icurve)
   {int i, len;
    char name[MAXLINE], desc[MAXLINE];
    double extr[2];

/* NOTE: see comment about virtual internal files in PD_WRT_PDB_CURVE */

    i = strlen(labl) + 1;

    if ((ix < 0) || (ix > icurve))
       {PD_error("BAD X VALUE REFERENCE - PD_WRT_PDB_CURVE_Y", PD_GENERIC);
        return(FALSE);};

    PM_maxmin(py, &extr[0], &extr[1], n);

/* make a curve entry consisting of:
 *    the name of the variable with the label;
 *    the name of the variable with the number of points for this curve
 *    the name of the variable with the x array
 *    the name of the variable with the y array
 *    the name of the variable with the x extrema
 *    the name of the variable with the y extrema
 */
    snprintf(desc, MAXLINE, "|labl%d|npts%d|xval%d|yval%d|xext%d|yext%d|",
                  icurve, ix, ix, icurve, ix, icurve);

/* use len to effect the cast from size_t to int */
    len = strlen(desc) + 1;
    snprintf(name, MAXLINE, "curve%04d(%d)", icurve, len);
    if (!PD_write(fp, name, G_CHAR_S, desc))
       return(FALSE);

/* save the curve label */
    snprintf(name, MAXLINE, "labl%d(%d)", icurve, i);
    if (!PD_write(fp, name, G_CHAR_S, (char *) labl))
       return(FALSE);

/* save the number of points */
/*
    snprintf(name, MAXLINE, "npts%d", icurve);
    if (!PD_write(fp, name, G_INT_S, &n))
       return(FALSE);
*/

/* save the y array and the extrema */
    snprintf(name, MAXLINE, "yext%d(2)", icurve);
    if (!PD_write(fp, name, G_DOUBLE_S, extr))
       return(FALSE);

    snprintf(name, MAXLINE, "yval%d(%d)", icurve, n);
    if (!PD_write(fp, name, G_DOUBLE_S, py))
       return(FALSE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PUT_MAPPING - write a mapping F to the PDBfile, FILE, with index
 *                - MAPPING
 *
 * #bind PD_put_mapping fortran() scheme() python()
 */

int PD_put_mapping(PDBfile *file ARG(,,cls), PM_mapping *f, int mapping)
   {char label[MAXLINE];

    snprintf(label, MAXLINE, "Mapping%d", mapping);
    if (!PD_write(file, label, "PM_mapping *", &f))
       {PD_error("CAN'T WRITE MAPPING - PD_PUT_MAPPING", PD_GENERIC);
        return(FALSE);};

    PD_reset_ptr_list(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PUT_IMAGE - write an image F to the PDBfile, FILE, with index IMAGE
 *              - do a slight fudge because PG_image isn't defined
 *
 * #bind PD_put_image fortran() scheme() python()
 */

int PD_put_image(PDBfile *file ARG(,,cls), void *f, int image)
   {char label[MAXLINE];

    snprintf(label, MAXLINE, "Image%d", image);
    if (!PD_write(file, label, "PG_image *", &f))
       {PD_error("CAN'T WRITE IMAGE - PD_PUT_IMAGE", PD_GENERIC);
        return(FALSE);};

    PD_reset_ptr_list(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PUT_SET - write a set S to the PDBfile, FILE, under the name of 
 *            - the set
 *
 * #bind PD_put_set fortran() scheme() python()
 */

int PD_put_set(PDBfile *file ARG(,,cls), PM_set *s)
   {char t[MAXLINE];

    SC_strncpy(t, MAXLINE, s->name, -1);
    PD_process_set_name(t);

    if (!PD_write(file, t, "PM_set *", &s))
       {PD_error("CAN'T WRITE SET - PD_PUT_SET", PD_GENERIC);
        return(FALSE);};

    PD_reset_ptr_list(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PROCESS_SET_NAME - replace '.' with '!' in set names so that
 *                     - PDBLib doesn't mistake the variable for
 *                     - a structure
 *                     - the input string, DNAME, will be mangled!!!
 *
 * #bind PD_process_set_name fortran() scheme() python()
 */

char *PD_process_set_name(char *dname)
   {char *pd;

    for (pd = dname; *pd; pd++)
        {if (*pd == '.')
            *pd = '!';
         else if (*pd == '>')
            {*(--pd) = '\0';
             break;};};

    return(dname);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MESH_STRUCT - define types for mesh generation
 *
 * #bind PD_mesh_struct fortran() scheme() python()
 */

int PD_mesh_struct(PDBfile *file ARG(,,cls))
   {int err;

    err  = TRUE;
    err &= G_PM_CONIC_CURVE_D(file);
    err &= G_PM_END_POINT_D(file);
    err &= G_PM_SIDE_D(file);
    err &= G_PM_PART_D(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEF_MAPPING - define PM_set and PM_mapping to 
 *                - a PDB file thereby preparing it for mappings
 *                - return TRUE iff successful
 *
 * #bind PD_def_mapping fortran() scheme() python()
 */

int PD_def_mapping(PDBfile *fp ARG(,,cls))
   {int err;

    G_register_pml_types();

    err = TRUE;

    err &= (G_PM_CENTERING_E(fp) != NULL);

    err &= G_SC_ARRAY_D(fp);

/* define the deprecated SC_dynamic_array */
    err &= G_SC_DYNAMIC_ARRAY_D(fp);

    err &= G_PCONS_D(fp);

    err &= G_C_ARRAY_D(fp);
    err &= G_PM_POLYGON_D(fp);
    err &= G_PM_FIELD_D(fp);
    err &= G_PM_MESH_TOPOLOGY_D(fp);
    err &= G_PM_SET_D(fp);
    err &= G_PM_MAPPING_D(fp);

    err &= G_RGB_COLOR_MAP_D(fp);
    err &= G_PG_PALETTE_D(fp);
    err &= G_PG_IMAGE_D(fp);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MAKE_IMAGE - create and return a PD_IMAGE
 *
 * #bind PD_make_image fortran() scheme() python()
 */

PD_image *PD_make_image(const char *name, const char *type, void *data,
		        int kmax, int lmax, int bpp,
		        double xmin, double xmax, double ymin, double ymax,
			double zmin, double zmax)
   {PD_image *im;

    im = CMAKE(PD_image);

/* distinguish this from the PG_image */
    im->version_id   = -1;
    im->label        = CSTRSAVE(name);
    im->xmin         = xmin;
    im->xmax         = xmax;
    im->ymin         = ymin;
    im->ymax         = ymax;
    im->zmin         = zmin;
    im->zmax         = zmax;
    im->element_type = CSTRSAVE(type);
    im->buffer       = (unsigned char *) data;
    im->kmax         = kmax;
    im->lmax         = lmax;
    im->size         = kmax*lmax;
    im->bits_pixel   = bpp;
    im->palette      = NULL;

    return(im);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_REL_IMAGE - release a PD_IMAGE
 *
 * #bind PD_rel_image fortran() scheme() python()
 */

void PD_rel_image(PD_image *im)
   {

    CFREE(im->label);
    CFREE(im->element_type);
    CFREE(im->buffer);
    CFREE(im);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISK_ADDR_SORT - sort the array of haelems containing syments
 *                    - in order of increasing disk address
 */

int _PD_disk_addr_sort(haelem **v, int n)
   {int gap, i, j;
    haelem *temp;
    syment *epa, *epb;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i-gap; j >= 0; j -= gap)
                {epa = (syment *) v[j]->def;
                 epb = (syment *) v[j+gap]->def;
                 if (PD_entry_address(epa) <= PD_entry_address(epb))
                    break;
                 temp     = v[j];
                 v[j]     = v[j+gap];
                 v[j+gap] = temp;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
