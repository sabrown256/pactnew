/*
 * PAFI.C - the beginning of a FORTRAN interface to PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "panacea_int.h"

/*--------------------------------------------------------------------------*/

/*                          DATABASE ACCESSORS                              */

/*--------------------------------------------------------------------------*/

/* PA_CONNECT_PTRS_P - give FORTRAN routines access to the database
 *                   - via pointers
 *                   - return TRUE iff successful
 */

int PA_connect_ptrs_p(char *name, char **pvn, void **pp, int *pn)
   {int i, n;
    char *vname, s[MAXLINE];
    void **ptr;

    n = *pn;
    for (i = 0; i < n; i++)
        {vname = pvn[i];
         snprintf(s, MAXLINE, "%s-%s", name, vname);

         ptr  = pp + i;
         PA_get_access(ptr, s, -1L, 0L, TRUE);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                        DATABASE CONSTRUCTORS                             */

/*--------------------------------------------------------------------------*/

/* _PA_PROC_DD_TAB - process the DD table with a view to defining the
 *                 - package controls and getting the connections between
 *                 - the database and the array sizes built
 */

static void _PA_proc_dd_tab(PA_package *pck, hasharr *tab)
   {int j, k, n, n_max;
    int id, isw, ipr, inm;
    int n_doubles, n_integers, n_strings;
    long i;
    char **nl, *t, *type, *pt;
    pcons *c, *alist, *pa;
    PA_variable *pp;

    n_doubles  = 0;
    n_integers = 0;
    n_strings  = 0;

/* count the number of scalar integers, reals, and strings */
    for (i = 0; SC_hasharr_next(tab, &i, NULL, &type, (void **) &pp); i++)
        {if (strcmp(type, PA_gs.package) == 0)
	    continue;

	 pt = PA_VARIABLE_TYPE_S(pp);
	 if (PA_VARIABLE_N_DIMS(pp) == 0)
	    {id = SC_type_id(pt, FALSE);

	     if (id == SC_INT_I)
	        n_integers++;
	     else if (id == SC_DOUBLE_I)
	        n_doubles++;
	     else if (id == SC_STRING_I)
	        n_strings++;};};

    PA_mk_control(pck, pck->name, n_strings, n_doubles, n_integers+1);

/* build an array of names of the unique scalar array
 * size variables from the dd table
 */
    n_max = 50;
    nl    = CMAKE_N(char *, n_max);
    n     = 0;

/* get all of the names */
    for (i = 0; SC_hasharr_next(tab, &i, NULL, NULL, (void **) &alist); i++)
        {for (pa = alist; pa != NULL; pa = (pcons *) pa->cdr)
	     {c = (pcons *) pa->car;
	      t = (char *) c->car;
	      if (strcmp(t, "KEY") == 0)
		 continue;
	      nl[n++] = (char *) c->cdr;
	      if (n >= n_max)
		 {n_max += 50;
		  CREMAKE(nl, char *, n_max);};};};

/* eliminate duplicate names (in general there will be many duplicates) */
    for (i = 0; i < n; i++)
        {if (nl[i] == NULL)
            continue;

         for (j = i + 1; j < n; j++)
             {if (nl[j] == NULL)
                 continue;
              if (strcmp(nl[i], nl[j]) == 0)
                 {for (k = j; k < n; k++)
                      nl[k] = nl[k+1];
                  n--;};};};

/* associate the unique scalar variables with iswtch's, rparam's, and name's
 * intern the pointers to the unique scalar variables in the database
 * e.g. &swtch[14] and so on
 */
    isw = 2;
    ipr = 1;
    inm = 1;
    for (i = 0; i < n; i++)
        {pp = PA_inquire_variable(nl[i]);
         if (pp == NULL)
            continue;

         type = PA_VARIABLE_TYPE_S(pp);
	 id   = SC_type_id(type, FALSE);
         if (id == SC_INT_I)
            {PA_VARIABLE_DATA(pp) = (void *) &SWTCH[isw++];}

         else if (id == SC_DOUBLE_I)
            {PA_VARIABLE_DATA(pp) = (void *) &PARAM[ipr++];}

         else if (id == SC_STRING_I)
            {PA_VARIABLE_DATA(pp) = (void *) &NAME[inm++];};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_GET_ALIST_DIMS - determine variable dimensions
 *                    - return a list of dimensions in the form PANACEA
 *                    - wants to see it
 */

static void _PA_get_alist_dims(PA_package *pck, PA_variable *pp, hasharr *tab)
   {int i, nd, *ip;
    void *leng;
    char **dimns, *dname, *lname, vname[MAXLINE];
    char *key;
    PA_variable *pv;
    pcons *blist, *pa;
    PA_dimens *vdims, *next, *prev;

    if (PA_VARIABLE_N_DIMS(pp) == 0)
       return;

    PA_ERR(!GET_INFO(pp, PA_INFO_N_DIMS, &nd),
           "CAN'T GET NUMBER OF DIMENSIONS - _PA_GET_ALIST_DIMS");
    PA_ERR(!GET_INFO(pp, PA_INFO_DIMS, &dimns),
           "CAN'T GET DIMENSIONS - _PA_GET_ALIST_DIMS");
    PA_ERR(!GET_INFO(pp, PA_INFO_KEY, &key),
           "CAN'T GET DIMENSION MEMBER NAME - _PA_GET_ALIST_DIMS");

    vdims = NULL;
    for (i = 0; i < nd; i++)
        {dname = dimns[i];
         if (SC_numstrp(dname))
            {ip   = CMAKE(int);
             *ip  = SC_stoi(dname);
             leng = (void *) ip;}
         else if (strcmp(dname, "?") == 0)
            leng = NULL;
         else
            {snprintf(vname, MAXLINE, "%s", dname);
             blist = (pcons *) SC_hasharr_def_lookup(tab, vname);

             lname = PA_assoc(blist, key);
             PA_ERR((lname == NULL),
                    "CAN'T FIND MEMBER %s FOR VARIABLE %s - _PA_GET_ALIST_DIMS",
                    key, PA_VARIABLE_NAME(pp));

	     pv = NULL;
             for (pa = pck->db_list; pa != NULL; pa = (pcons *) pa->cdr)
                 {snprintf(vname, MAXLINE, "%s-%s", (char *) pa->car, lname);
                  pv = PA_inquire_variable(vname);
                  if (pv != NULL)
                     break;};

             PA_ERR((pv == NULL),
                    "BAD DIMENSION %s, VARIABLE %s (%s) - _PA_GET_ALIST_DIMS",
                    dname, lname, vname);

             leng = pv->data;}

         next = _PA_mk_dimens(&PM_c.ione.i, (int *) leng, *PA_gs.dul);
         if (vdims == NULL)
            vdims = next;
         else
            prev->next = next;
         prev = next;};

    PA_VARIABLE_DIMS(pp)             = vdims;
    PA_VARIABLE_DESC(pp)->dimensions = _PA_mk_sym_dims(vdims);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROC_INFO - honor a request for info from the ASCII database
 *               - description to define a PANACEA variable
 *               - the possible requests are:
 *                     PA_INFO_TYPE
 *                         - ASCII string naming the type of the variable
 *                         - valid values are: SC_INT_S, SC_DOUBLE_S
 *                         - SC_STRING_S, SC_LONG_S, SC_SHORT_S, SC_FLOAT_S,
 *                         - SC_CHAR_S, and any derived type
 *                     PA_INFO_N_DIMS
 *                         - integer number of dimensions
 *                     PA_INFO_DIMS
 *                         - array of names of dimension variables (char **)
 *                     PA_INFO_SCOPE
 *                         - integer scope designator
 *                         - one of RESTART, RUNTIME, EDIT
 *                     PA_INFO_CLASS
 *                         - integer class designator
 *                         - one of REQUired, OPTionaL, DeMaND, PSEUDO
 *                     PA_INFO_CENTER
 *                         - integer designator of variable centering relative
 *                         - to the spatial mesh
 *                         - one of U_CENT, N_CENT, E_CENT, F_CENT, or Z_CENT
 *                         - for uncentered, node, edge, face, or zone centered
 *                     PA_INFO_PERSISTENCE
 *                         - integer persistence designator
 *                         - one of REL, KEEP, CACHE_F, or CACHE_R
 *                     PA_INFO_ALLOCATION
 *                         - integer designator for static or dynamic allocation
 *                         - one of STATIC or DYNAMIC
 *                     PA_INFO_FILE_NAME
 *                         - ASCII name of state file containing variable data
 *                     PA_INFO_INIT_VAL
 *                         - ASCII representation of initial value constant
 *                     PA_INFO_INIT_FNC
 *                         - pointer to pointer to function returning nothing
 *                     PA_INFO_CONV
 *                         - double precision floating point number specifying
 *                         - the conversion factor CGS->external of the variable
 *                     PA_INFO_UNIT
 *                         - double precision floating point number specifying
 *                         - the conversion factor CGS->internal of the variable
 *                     PA_INFO_KEY
 *                         - ASCII name of member associating dimension name
 *                         - with variable name
 *    
 *               - return TRUE iff the request has successfully honored
 */

static int _PA_proc_info(PA_variable *pp, int req, void *vr)
   {int i, nd, *pi;
    double *pd;
    pcons *alist;
    PFVoid *pfnc;
    char bf[MAXLINE], *token, **dims, **ppc, ***pppc;

    alist = PA_VARIABLE_ATTR(pp);

    pi   = (int *) vr;
    pd   = (double *) vr;
    ppc  = (char **) vr;
    pppc = (char ***) vr;
    pfnc = (PFVoid *) vr;

/* the type of the variable */
    switch (req)
       {case PA_INFO_TYPE :
             *ppc = PA_assoc(alist, "TYPE");
             break;

/* the dimensionality of the variable */
        case PA_INFO_N_DIMS :
             token = PA_assoc(alist, "NDIM");
             nd    = SC_stoi(token);
             *pi   = nd;
             break;

/* the dimensions of the variable */
        case PA_INFO_DIMS :
             token = PA_assoc(alist, "NDIM");
             nd    = SC_stoi(token);

/* make Klocworks happy */
	     nd    = min(nd, 10000);

             dims = CMAKE_N(char *, nd);
             for (i = 1; i <= nd; i++)
                 {snprintf(bf, MAXLINE, "D%d", i);
                  dims[i-1] = PA_assoc(alist, bf);};
             *pppc = dims;
             break;

/* the scope of the variable - RESTART, DMND, RUNTIME, EDIT */
        case PA_INFO_SCOPE :
             token = PA_assoc(alist, "SCOPE");
             if (SC_str_icmp(token, "RESTART") == 0)
                *pi = RESTART;
             else if (SC_str_icmp(token, "DMND") == 0)
                 *pi = DMND;
             else if (SC_str_icmp(token, "RUNTIME") == 0)
                *pi = RUNTIME;
             else if (SC_str_icmp(token, "DEFN") == 0)
                *pi = DEFN;
             else if (SC_str_icmp(token, "EDIT") == 0)
                *pi = EDIT;
             else if (SC_str_icmp(token, "SCRATCH") == 0)
                *pi = SCRATCH;
             break;

/* the class of the variable - REQUired, OPTionaL, DeMaND, PSEUDO */
        case PA_INFO_CLASS :
             token = PA_assoc(alist, "CLASS");
             if (SC_str_icmp(token, "REQU") == 0)
                 *pi = REQU;
             else if (SC_str_icmp(token, "OPTL") == 0)
                 *pi = OPTL;
             else if (SC_str_icmp(token, "PSEUDO") == 0)
                 *pi = PSEUDO;
             break;

/* the centering of the variable */
        case PA_INFO_CENTER :
             token = PA_assoc(alist, "CENTERING");
             if (SC_str_icmp(token, "ZONE") == 0)
                *pi = Z_CENT;
             else if (SC_str_icmp(token, "NODE") == 0)
                *pi = N_CENT;
             else if (SC_str_icmp(token, "FACE") == 0)
                *pi = F_CENT;
             else if (SC_str_icmp(token, "EDGE") == 0)
                *pi = E_CENT;
             else if (SC_str_icmp(token, "NOT") == 0)
                *pi = U_CENT;
             break;

/* the persistence of the variable */
        case PA_INFO_PERSISTENCE :
             token = PA_assoc(alist, "PERSISTENCE");
             if (SC_str_icmp(token, "RELEASE") == 0)
                *pi = REL;
             else if (SC_str_icmp(token, "KEEP") == 0)
                *pi = KEEP;
             else if (SC_str_icmp(token, "CACHE_F") == 0)
                *pi = CACHE_F;
             else if (SC_str_icmp(token, "CACHE_R") == 0)
                *pi = CACHE_R;
             break;

/* the allocation of the variable */
        case PA_INFO_ALLOCATION :
             token = PA_assoc(alist, "ALLOCATION");
             if (SC_str_icmp(token, "STATIC") == 0)
                *pi = STATIC;
             else if (SC_str_icmp(token, "DYNAMIC") == 0)
                *pi = DYNAMIC;
             break;

/* the name of the file with the data of the variable */
        case PA_INFO_FILE_NAME :
             *ppc = NULL;
             break;

/* the initial value of the variable */
        case PA_INFO_INIT_VAL :
             token = PA_assoc(alist, "DEF");
             *ppc  = NULL;
             break;

/* the initializing function of the variable */
        case PA_INFO_INIT_FNC :
             token = PA_assoc(alist, "PROC");
	     *pfnc = SC_HASHARR_LOOKUP_FUNCTION(PA_gs.symbol_tab, PFVoid, token);
             break;

/* the conversion factor CGS->external of the variable */
        case PA_INFO_CONV :
             token = PA_assoc(alist, "EXT-UNIT");
             *pd   = SC_stof(token);
             break;

/* the conversion factor CGS->internal of the variable */
        case PA_INFO_UNIT :
             token = PA_assoc(alist, "INT-UNIT");
             *pd   = SC_stof(token);
             break;

/* the alist key for the reference back into the database table */
        case PA_INFO_KEY :
             *ppc = _PA.lkey;
             break;

        default : PA_WARN(TRUE, "BAD REQUEST - _PA_PROC_INFO");};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROC_DB_TAB - process the variable definitions now that the sizing
 *                 - information has been set up
 */

static void _PA_proc_db_tab(PA_package *pck, hasharr *tab)
   {long i;
    char *ty;
    PA_variable *pp;

    _PA.get_info = _PA_proc_info;

/* set up the variable except for the dimensions */
    for (i = 0; SC_hasharr_next(tab, &i, NULL, &ty, (void **) &pp); i++)
        {if (strcmp(ty, PA_gs.package) == 0)
	    continue;

	 if ((PA_VARIABLE_DIMS(pp) == NULL) &&
	     (PA_VARIABLE_ATTR(pp) != NULL) &&
	     (PA_VARIABLE_DATA(pp) == NULL))
	    {GET_INFO(pp, PA_INFO_TYPE,        &PA_VARIABLE_TYPE_S(pp));
	     GET_INFO(pp, PA_INFO_N_DIMS,      &PA_VARIABLE_N_DIMS(pp));
	     GET_INFO(pp, PA_INFO_SCOPE,       &PA_VARIABLE_SCOPE(pp));
	     GET_INFO(pp, PA_INFO_CLASS,       &PA_VARIABLE_CLASS(pp));
	     GET_INFO(pp, PA_INFO_CENTER,      &PA_VARIABLE_CENTERING(pp));
	     GET_INFO(pp, PA_INFO_PERSISTENCE, &PA_VARIABLE_PERSISTENCE(pp));
	     GET_INFO(pp, PA_INFO_ALLOCATION,  &PA_VARIABLE_ALLOCATION(pp));
	     GET_INFO(pp, PA_INFO_INIT_VAL,    &PA_VARIABLE_INIT_VAL(pp));
	     GET_INFO(pp, PA_INFO_INIT_FNC,    &PA_VARIABLE_INIT_FUNC(pp));
	     GET_INFO(pp, PA_INFO_CONV,        &PA_VARIABLE_EXT_UNIT(pp));
	     GET_INFO(pp, PA_INFO_UNIT,        &PA_VARIABLE_INT_UNIT(pp));
	     GET_INFO(pp, PA_INFO_ATTRIBUTE,   &PA_VARIABLE_ATTR(pp));};};

/* process the dimension definition table */
    _PA_proc_dd_tab(pck, tab);

/* set up the dimensions */
    for (i = 0; SC_hasharr_next(tab, &i, NULL, &ty, (void **) &pp); i++)

/* allocate the scalars now */
        {if (strcmp(ty, PA_gs.package) == 0)
	    continue;

	 if (PA_VARIABLE_N_DIMS(pp) == 0)
	    {PA_VARIABLE_DATA(pp) = _PA_init_space(pp, 1L);
	     PA_VARIABLE_SIZE(pp) = 1L;
	     PA_VARIABLE_DIMS(pp) = NULL;};};

/* now the arrays can be safely set up */
    for (i = 0; SC_hasharr_next(tab, &i, NULL, &ty, (void **) &pp); i++)
        {if (strcmp(ty, PA_gs.package) == 0)
	    continue;

	 if ((PA_VARIABLE_DIMS(pp) == NULL) &&
	     (PA_VARIABLE_ATTR(pp) != NULL) &&
	     (PA_VARIABLE_DATA(pp) == NULL))
	    _PA_get_alist_dims(pck, pp, tab);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RD_TAB_HEAD - read the head of a table
 *                 - return the list of keys, the number of fields, and
 *                 - the number of entries in the table
 */

static PA_tab_head *_PA_rd_tab_head(FILE *fp, char *fn)
   {int j, nf, ne;
    char *token, *name, *key, *pb, bf[MAXLINE], *s;
    pcons *keys, *next, *prev;
    PA_tab_head *pt;

    while (TRUE)
        {if (GETLN(bf, MAXLINE, fp) == NULL)
            return(NULL);

         if (!SC_blankp(bf, "c#"))
            break;};

    pt = CMAKE(PA_tab_head);

    token = SC_strtok(bf, _PA.delim, s);
    if ((token != NULL) && (strcmp(token, "database") == 0))
       {name     = PA_get_field("SPECIFICATION", fn, REQU);
        pt->name = CSTRSAVE(name);
        pt->type = _PA.dict;}
    else
       {pt->type = _PA.dimtab;
        pt->name = _PA.dimtab;};

/* get the number of rows or entries */
    ne = SC_stoi(PA_get_field("NUMBER OF ROWS", fn, OPTL));

/* get in the number of fields per entry */
    nf = SC_stoi(PA_get_field("NUMBER OF ATTRIBUTES", fn, OPTL));

/* get list of field keys */
    PA_ERR((GETLN(bf, MAXLINE, fp) == NULL),
           "BAD KEY LIST - %s", fn);

    keys = NULL;
    pb   = bf;
    for (j = 0; j < 100000; j++)
        {key = SC_strtok(pb, PA_gs.token_delimiters, s);
         if (key == NULL)
            break;

         pb   = NULL;
         next = SC_mk_pcons(SC_STRING_S, CSTRSAVE(key), SC_PCONS_P_S, NULL);
         if (keys == NULL)
            keys = next;
         else
            prev->cdr = (void *) next;
         prev = next;};

    PA_WARN((nf > 0) && (j != nf),
            "TABLE %s, INCONSISTENT FIELD NUMBER, %d GIVEN %d READ - _PA_RD_TAB_HEAD",
            pt->name, nf, j);

    nf = j;

    pt->entries   = keys;
    pt->n_entries = ne;
    pt->n_fields  = nf;

    return(pt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RD_DD_TAB - read a DATA DEFINITION table from the given file */

static void _PA_rd_dd_tab(PA_package *pck, FILE *fp)
   {int i, j, nf, ne, ne_read;
    char bf[MAXLINE], vname[MAXLINE], *old_delim, *token, *key, *pb, *s;
    pcons *pk, *alist, *next, *prev;
    PA_tab_head *pt;

    if (_PA.ddtab == NULL)
       _PA.ddtab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

    old_delim = PA_gs.token_delimiters;

    PA_gs.token_delimiters = _PA.delim;

    pt = _PA_rd_tab_head(fp, "_PA_RD_DD_TAB");
    
    PA_ERR((pt == NULL) || (strcmp(pt->type, _PA.dimtab) != 0),
           "BAD BIND TABLE - _PA_RD_DD_TAB");

    ne = pt->n_entries;
    nf = pt->n_fields;

/* get the entries */
    for (i = 1, ne_read = 0; TRUE; i++, ne_read++)
        {if (GETLN(bf, MAXLINE, fp) == NULL)
            break;
           
         if (bf[0] == '#')
            continue;

         if (SC_blankp(bf, " "))
            break;

         alist = NULL;
	 key   = NULL;
         pk    = pt->entries;
         pb    = bf;
         for (j = 0; j < nf; j++)
             {token = SC_strtok(pb, PA_gs.token_delimiters, s);
              PA_ERR((token == NULL),
                     "BAD FIELD %d IN ENTRY %d - _PA_RD_DD_TAB", j, i);

              token = CSTRSAVE(token);
              if (pb != NULL)
                 key = token;

              pb = NULL;

/* NOTE: clean this out using SC_add_alist */
              next = SC_mk_pcons(SC_PCONS_P_S,
				 SC_mk_pcons(SC_PCONS_P_S, pk->car,
					     SC_STRING_S, token),
				 SC_PCONS_P_S, NULL);
              if (alist == NULL)
                 alist = next;
              else
                 prev->cdr = (void *) next;
              prev = next;
              pk   = (pcons *) pk->cdr;};
              
         snprintf(vname, MAXLINE, "%s", key);

         SC_hasharr_install(_PA.ddtab, vname, alist, _PA.alist, 3, -1);};

    PA_WARN((ne > 0) && (ne != ne_read),
            "TABLE %s, EXPECTED %d ENTRIES FOUND %d - _PA_RD_DD_TAB",
             pt->name, ne, ne_read);

    PA_gs.token_delimiters = old_delim;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RD_DB_TAB - read a DATABASE DEFINITION table from the given file
 *               - return the number of entries in the table
 */

int _PA_rd_db_tab(PA_package *pck, FILE *fp)
   {int i, j, ne, nf, nr, nd;
    int vattr[N_ATTRIBUTES];
    char bf[MAXLINE], vname[MAXLINE], memb[MAXLINE];
    char *old_delim, *pb, *token, *bname;
    char *ptype, *type, *s;
    PA_tab_head *pt;
    PA_variable *pp;
    pcons *alist, *pk, *next, *prev;
    hasharr *fc;
    memdes *desc, *lst, *prv;
    defstr *dp;

    nr = 0;

    if (PA_gs.vif == NULL)
       PA_gs.vif = PA_open("vif", "r+", TRUE);

    old_delim = PA_gs.token_delimiters;

    PA_gs.token_delimiters = _PA.delim;

    vattr[0] = RUNTIME;
    vattr[1] = OPTL;
    vattr[2] = REL;
    vattr[3] = U_CENT;
    vattr[4] = DYNAMIC;

    _PA.get_info = _PA_proc_info;

    pt = _PA_rd_tab_head(fp, "_PA_RD_DB_TAB");
    if (pt == NULL)
       return(0);

    ne = pt->n_entries;
    nf = pt->n_fields;

    fc  = (PA_gs.vif != NULL) ? PA_gs.vif->chart : NULL;
    prv = NULL;
    lst = NULL;

    vattr[0] = RUNTIME;
    vattr[1] = OPTL;
    vattr[2] = U_CENT;

    if (PA_gs.vif != NULL)

/* get the entries */
       {for (i = 1, nr = 0; TRUE; i++, nr++)
	    {if (GETLN(bf, MAXLINE, fp) == NULL)
	        break;
           
	     if (bf[0] == '#')
	        continue;

	     if (SC_blankp(bf, " "))
	        break;

	     pb    = bf;
	     pk    = pt->entries;
	     alist = NULL;
	     for (j = 0; j < nf; j++)
	         {token = SC_strtok(pb, PA_gs.token_delimiters, s);
		  PA_ERR((token == NULL),
			 "BAD FIELD %d IN ENTRY %d - _PA_RD_DB_TAB", j, i);
		  pb = NULL;

		  if (token[0] == '"')
		     {snprintf(vname, MAXLINE, "%s %s",
			       &token[1], SC_strtok(pb, "\"", s));
		      token = vname;};

/* NOTE: clean this out using SC_add_alist */
		  next = SC_mk_pcons(SC_PCONS_P_S,
				     SC_mk_pcons(SC_PCONS_P_S, pk->car,
						 SC_STRING_S,
						 CSTRSAVE(token)),
				     SC_PCONS_P_S, NULL);
		  if (alist == NULL)
		     alist = next;
		  else
		     prev->cdr = (void *) next;
		  prev = next;
		  pk   = (pcons *) pk->cdr;};
              
	     bname = PA_assoc(alist, "KEY");
	     snprintf(vname, MAXLINE, "%s-%s", pt->name, bname);

/* NOTE: make a default variable for now, the rest will come later */
	     pp = _PA_mk_variable(vname, NULL, NULL, NULL,
				  vattr, SC_DOUBLE_S,
				  1.0, 1.0,
				  NULL, NULL, alist);

	     _PA_install_var(vname, pp);

/* build up a defstr corresponding to this table */
	     GET_INFO(pp, PA_INFO_TYPE, &type);

/* while we're here with the actual type fix the pp type */
	     if (_PD_indirection(type))
	        dp = PD_inquire_host_type(PA_gs.vif, "*");
	     else
	        dp = PD_inquire_host_type(PA_gs.vif, type);
	     PA_ERR((dp == NULL),
		    "TYPE %s NOT DEFINED - _PA_RD_DB_TAB", type);

	     PA_VARIABLE_TYPE(pp) = dp;

	     GET_INFO(pp, PA_INFO_N_DIMS, &nd);
	     if (nd > 0)
	        snprintf(memb, MAXLINE, "%s *%s", type, bname);
	     else
	        snprintf(memb, MAXLINE, "%s %s", type, bname);

	     desc  = _PD_mk_descriptor(memb, 0L);
	     type  = CSTRSAVE(memb);
	     ptype = SC_firsttok(type, " \n");
	     if ((ptype != NULL) && (SC_hasharr_lookup(fc, ptype) == NULL))
	        PA_ERR(((strcmp(ptype, pt->name) != 0) ||
			!_PD_indirection(memb)),
		       "%s BAD MEMBER TYPE - _PA_RD_DB_TAB", memb);

	     CFREE(type);
	     if (lst == NULL)
	        lst = desc;
	     else
	        prv->next = desc;
	     prv = desc;};

/* install the type in both charts */
        dp = _PD_defstr_inst(PA_gs.vif, pt->name, STRUCT_KIND, lst,
			     NO_ORDER, NULL, NULL, PD_CHART_HOST);

	PA_ERR((dp == NULL),
	       "STRUCT DEFINITION FAILED - _PA_RD_DB_TAB");

	PA_WARN((ne > 0) && (ne != nr),
		"TABLE %s, EXPECTED %d ENTRIES FOUND %d - _PA_RD_DB_TAB",
		pt->name, ne, nr);

	PA_gs.token_delimiters = old_delim;

/* cons this dictionary name onto the list */
	pck->db_list = SC_mk_pcons(SC_STRING_S, CSTRSAVE(pt->name),
				   SC_PCONS_P_S, pck->db_list);};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_READ_DEF - read the given file and define the contribution to the
 *             - database for the package which the file represents
 */

void PA_read_def(PA_package *pck, char *name)
   {FILE *fp;

    fp = io_open(name, "r");
    PA_ERR((fp == NULL),
           "CAN'T FIND FILE %s - PA_READ_DEF", name);

/* read the dimension definition table */
    _PA_rd_dd_tab(pck, fp);

/* read the database definition table */
    while (_PA_rd_db_tab(pck, fp));

    io_close(fp);

/* re-process the database definitions */
    _PA_proc_db_tab(pck, _PA.ddtab);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_ALIST_LENGTH - return the length of the given alist
 *                  - debugging aid only
 */

int _PA_alist_length(pcons *alist)
   {int n;
    pcons *pa;

    for (n = 0, pa = alist; pa != NULL; pa = (pcons *) pa->cdr, n++);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INSTALL_FUNCTION - install a function which PANACEA can invoke
 *                     - on request on behalf of the application
 */

void PA_install_function(char *s, PFVoid fnc)
   {SC_address addr;

    addr.funcaddr = (PFInt) fnc;

    if (PA_gs.symbol_tab == NULL)
       PA_gs.symbol_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

    SC_hasharr_install(PA_gs.symbol_tab, s, addr.memaddr, "procedure", 2, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INSTALL_IDENTIFIER - install a compiled variable which PANACEA can
 *                       - access on behalf of the application
 */

void PA_install_identifier(char *s, void *vr)
   {

    if (PA_gs.symbol_tab == NULL)
       PA_gs.symbol_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

/* GOTCHA: The variable may or may not be dynamic, so don't mark it.
 *         It's up to the caller to be aware of the reference count.
 */
    SC_hasharr_install(PA_gs.symbol_tab, s, vr, "variable", 2, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ADD_HOOK - add an item to the global list of hooks */

void PA_add_hook(char *name, void *fnc)
   {SC_address ad;

    if (PA_gs.symbol_tab == NULL)
       PA_gs.symbol_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

    ad.funcaddr = (PFInt) fnc;
    SC_hasharr_install(PA_gs.symbol_tab, name, ad.memaddr, SC_POINTER_S, 2, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
