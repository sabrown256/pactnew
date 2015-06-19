/*
 * PDCAST.C - routines for casts (type and size)
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INQUIRE_MEMBER - return the member of DP S */

static memdes *_PD_inquire_member(defstr *dp, const char *s)
   {memdes *desc, *rv;

    rv = NULL;

    if ((dp != NULL) && (s != NULL))
       {for (desc = dp->members; desc != NULL; desc = desc->next)
	    {if (strcmp(s, desc->member) == 0)
	        {rv = desc;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_MEMBER_NAME - return the member of LST named S */

static memdes *_PD_get_member_name(memdes *lst, const char *s)
   {memdes *desc, *rv;

    rv = NULL;

    if ((lst != NULL) && (s != NULL))
       {for (desc = lst; desc != NULL; desc = desc->next)
	    {if (strcmp(s, desc->name) == 0)
	        {rv = desc;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INQUIRE_MEMBER_NAME - return the member of DP named S */

static memdes *_PD_inquire_member_name(defstr *dp, const char *s)
   {memdes *rv;

    rv = NULL;

    if ((dp != NULL) && (s != NULL))
       rv = _PD_get_member_name(dp->members, s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_INJECT - setup the cast specs from S into DESC */

static int _PD_cast_inject(hasharr *tab, defstr *dp, memdes *desc,
			   const char *s)
   {int rv;
    memdes *lst;

    rv = TRUE;

/* make an independent copy in case the one in the file chart is released */
    CFREE(desc->cast_memb);
    desc->cast_memb = CSTRSAVE(s);
    desc->cast_offs = _PD_member_location(s, tab, dp, &lst);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SIZE_FROM_INJECT - setup the size_from specs from SA into DESC */

static int _PD_size_from_inject(hasharr *tab, defstr *dp,
				memdes *desc, char **sa)
   {int rv;
    inti is, ns, off;
    int64_t *so;
    char **sm;
    memdes *lst;

    SC_ptr_arr_len(ns, sa);

    sm = CMAKE_N(char *, ns+1);
    so = CMAKE_N(int64_t, ns+1);
    if ((sm == NULL) || (so == NULL))
       rv = FALSE;

    else
       {rv = TRUE;

	SC_free_strings(desc->size_memb);
	CFREE(desc->size_offs);

	desc->size_memb = sm;
	desc->size_offs = so;

	for (is = 0; is < ns; is++)
	    {off = _PD_member_location(sa[is], tab, dp, &lst);
	     sm[is] = CSTRSAVE(sa[is]);
	     so[is] = off;};

	sm[is] = NULL;
	so[is] = 0;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_PROCESS_SPEC - process a cast or size_from specification S */

static int _PD_cast_process_spec(hasharr *chrt, defstr *dp,
				 memdes *desc, const char *s)
   {int it, nt, rv;
    char **sa, **ta;

    rv = FALSE;

    if ((s != NULL) && (desc != NULL))
       {ta = SC_tokenize(s, " \t");
	SC_ptr_arr_len(nt, ta);

	for (it = 0; it < nt; it++)

/* size_from */
	    {if (ta[it][0] == '[')
	        {sa = SC_tokenize(ta[it], "[], \t\001");
		 rv = _PD_size_from_inject(chrt, dp, desc, sa);
		 SC_free_strings(sa);}

/* type cast */
	     else
	        rv = _PD_cast_inject(chrt, dp, desc, ta[it]);};

	SC_free_strings(ta);}

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_CHECK_LIST - complete the set up of
 *                     - the casts in the given chart
 */

static void _PD_cast_check_list(hasharr *chrt, char **lst, inti n)
   {long i;
    inti j;
    memdes *desc;
    defstr *dp;

/* assume a single-element, linked-list hasharr */
    for (i = 0; SC_hasharr_next(chrt, &i, NULL, NULL, (void **) &dp); i++)
        {for (j = 0L; j < n; j += 3)
	     {if (strcmp(dp->type, lst[j]) == 0)
		 {desc = _PD_inquire_member(dp, lst[j+1]);
		  if (desc != NULL)
		     _PD_cast_process_spec(chrt, dp, desc, lst[j+2]);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_CHECK - work over the casts */

void _PD_cast_check(PDBfile *file)
   {int i, nc;
    char **clst;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    nc   = pa->n_casts;
    clst = pa->cast_lst;

    if (nc > 0)

/* check the casts for the file->chart */
       {_PD_cast_check_list(file->chart, clst, nc);

/* check the casts for the file->host_chart */
	_PD_cast_check_list(file->host_chart, clst, nc);

/* clean up the mess */
	for (i = 0L; i < nc; i++)
	    CFREE(clst[i]);};

    CFREE(pa->cast_lst);
    pa->n_casts = 0L;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_SIZE - return the size of a pointer described by DESC
 *               - and based on size_from data
 *               - which is a member of a struct described by MEML
 *               - and pointed to by SVR
 *               - return 0 on failure to interpret size_from info
 *               - return 1 if there is no size_from info
 *               - otherwise return the size in items of the member
 */

inti _PD_cast_size(memdes *meml, void *svr, memdes *desc)
   {int sid;
    long in, sn, bpi;
    inti is, ns, ni, n;
    int64_t *so, *dm;
    char **sm, *p;
    memdes *md;

    ni = 1;
    sm = desc->size_memb;
    so = desc->size_offs;
    SC_ptr_arr_len(ns, sm);
    for (is = 0; is < ns; is++)
        {p = (char *) svr + so[is];

	 md = _PD_get_member_name(meml, sm[is]);

	 n   = 0;
	 sid = SC_fix_type_id(md->base_type, FALSE);
	 if (sid != -1)
	    {if (_PD_indirection(md->type) == TRUE)
	        {bpi = SC_type_size_i(sid);
		 p   = DEREF(p);
		 sn  = SC_arrlen(p)/bpi;}
	     else
	        sn = md->number;

	     dm = SC_convert_id(G_INT64_I, NULL, 0, 1,
				sid, p, 0, 1, sn, FALSE);

	     n = 1;
	     for (in = 0; in < sn; in++)
	         n *= dm[in];

	     CFREE(dm);};

	 ni *= n;};

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CAST - Tell PDBLib that the type of a particular member MEMB
 *         - of struct TYPE in PDBfile FILE is specified by another
 *         - member CONTR.
 *         - Both members, MEMB and CONTR, must be character pointers.
 *         - This information is kept in the structure charts of FILE.
 *         - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_cast fortran() scheme() python()
 */

int PD_cast(PDBfile *file ARG(,,cls), const char *type,
	    const char *memb, const char *contr)
   {int rv;
    defstr *dp;
    hasharr *tab;
    memdes *desc;

    rv = TRUE;

/* first make sure that "type" has a member "contr" which is of type char
 * this assumes that file->chart is a linked list
 */
    dp   = PD_inquire_type(file, type);
    desc = _PD_inquire_member_name(dp, contr);
    if ((desc == NULL) ||
	(strcmp(desc->base_type, G_CHAR_S) != 0) ||
	!_PD_indirection(desc->type))
       {PD_error("BAD CAST CONTROLLER - PD_CAST", PD_GENERIC);
	rv = FALSE;};

/* now that we know such a cast is reasonable
 * add the cast to the host charts
 */
    if (rv == TRUE)

/* add the cast to the file->host_chart */
       {dp   = PD_inquire_host_type(file, type);
	desc = _PD_inquire_member_name(dp, memb);
	if (desc != NULL)
	   {tab = file->host_chart;
	    rv  = _PD_cast_inject(tab, dp, desc, contr);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SIZE_FROM - Tell PDBLib that the size of a particular member MEMB
 *              - of struct TYPE is specified by the product of a comma
 *              - delimited list of other members CONTR.
 *              - MEMB must be a character pointer and the members
 *              - specified by CONTR must be fixed point or pointers
 *              - to fixed point types.
 *              - This information is kept in the structure charts of FILE.
 *              - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_size_from fortran() scheme() python()
 */

int PD_size_from(PDBfile *file ARG(,,cls), const char *type,
		 const char *memb, const char *contr)
   {int rv;
    long is, ns;
    char **sa;
    hasharr *tab;
    defstr *dp;
    memdes *desc;

    rv = TRUE;

    sa = SC_tokenize(contr, ", \t");
    SC_ptr_arr_len(ns, sa);

/* first make sure that "type" has a member "contr" which is of type char
 * this assumes that file->chart is a linked list
 */
    dp = PD_inquire_type(file, type);
    for (is = 0; (is < ns) && (rv == TRUE); is++)
        {desc = _PD_inquire_member_name(dp, sa[is]);
	 if (SC_fix_type_id(desc->base_type, FALSE) == -1)
	    {PD_error("BAD CAST CONTROLLER - PD_CAST", PD_GENERIC);
	     rv = FALSE;};};

/* now that we know such a size_from is reasonable,
 * add the size_from to the host charts
 */
    if (rv == TRUE)

/* add the size_from to the file->host_chart */
       {dp   = PD_inquire_host_type(file, type);
	desc = _PD_inquire_member_name(dp, memb);
	if (desc != NULL)
	   {tab = file->host_chart;
	    rv = _PD_size_from_inject(tab, dp, desc, sa);};};

    SC_free_strings(sa);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
