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

/* _PD_CAST_CHECK_LIST - complete the set up of
 *                     - the casts in the given chart
 */

static void _PD_cast_check_list(hasharr *chrt, char **lst, inti n)
   {long i;
    inti j;
    memdes *memb, *desc;
    defstr *dp;

/* assume a single-element, linked-list hasharr */
    for (i = 0; SC_hasharr_next(chrt, &i, NULL, NULL, (void **) &dp); i++)
        {for (desc = dp->members; desc != NULL; desc = desc->next)
	     {for (j = 0L; j < n; j += 3)
		  {if ((strcmp(dp->type, lst[j]) == 0) &&
		       (strcmp(desc->member, lst[j+1]) == 0))
		      {desc->cast_memb = lst[j+2];
		       desc->cast_offs = _PD_member_location(desc->cast_memb,
							     chrt, dp,
							     &memb);
		       SC_mark(lst[j+2], 1);};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_CHECK_HASH - complete the set up of the casts
 *                     - in the given chart
 */

void _PD_cast_check_hash(hasharr *chrt, char **lst, inti n)
   {long i;
    inti j;
    defstr *dp;
    memdes *memb, *desc;

/* check each hash element
 * do not assume a single-element, linked-list hasharr
 */
    for (i = 0; SC_hasharr_next(chrt, &i, NULL, NULL, (void **) &dp); i++)
        {for (desc = dp->members; desc != NULL; desc = desc->next)
	     {for (j = 0L; j < n; j += 3)
		  {if ((strcmp(dp->type, lst[j]) == 0) &&
		       (strcmp(desc->member, lst[j+1]) == 0))
		      {desc->cast_memb = lst[j+2];
		       desc->cast_offs = _PD_member_location(desc->cast_memb,
							     chrt, dp,
							     &memb);
		       SC_mark(lst[j+2], 1);};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_CHECK - work over the casts */

void _PD_cast_check(PDBfile *file)
   {int i, nc;
    char **clst;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    nc    = pa->n_casts;
    clst  = pa->cast_lst;

    if (nc > 0)

/* check the casts for the file->chart */
       {_PD_cast_check_list(file->chart, clst, nc);

/* check the casts for the file->host_chart */
	_PD_cast_check_list(file->host_chart, clst, nc);

/* clean up the mess */
	for (i = 0L; i < nc; i += 3)
	    {CFREE(clst[i]);
	     CFREE(clst[i+1]);};};

    CFREE(pa->cast_lst);
    pa->n_casts = 0L;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INQUIRE_MEMBER - return the member of DP named MEMB */

static memdes *_PD_inquire_member(defstr *dp, char *memb)
   {memdes *desc, *rv;

    rv = NULL;

    if ((dp != NULL) && (memb != NULL))
       {for (desc = dp->members; desc != NULL; desc = desc->next)
	    {if (strcmp(memb, desc->name) == 0)
	        {rv = desc;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CAST - tell PDBLib that the type of a particular member MEMB
 *         - (which must be a pointer) is specified by another member CONTR
 *         - (which must be a character pointer).
 *         - Put this information into the FILE chart.
 *         - return TRUE iff successful
 *
 * #bind PD_cast fortran() scheme() python()
 */

int PD_cast(PDBfile *file ARG(,,cls), char *type, char *memb, char *contr)
   {int rv;
    defstr *dp;
    hasharr *tab;
    memdes *desc, *lst;

    rv = TRUE;

/* first make sure that "type" has a member "contr" which is of type char
 * this assumes that file->chart is a linked list
 */
    dp   = PD_inquire_type(file, type);
    desc = _PD_inquire_member(dp, contr);
    if ((desc == NULL) ||
	(strcmp(desc->base_type, SC_CHAR_S) != 0) ||
	!_PD_indirection(desc->type))
       {PD_error("BAD CAST CONTROLLER - PD_CAST", PD_GENERIC);
	rv = FALSE;};

/* now that we know such a cast is reasonable
 * add the cast to the host charts
 */
    if (rv == TRUE)

/* add the cast to the file->host_chart */
       {dp   = PD_inquire_host_type(file, type);
	desc = _PD_inquire_member(dp, memb);
	if (desc != NULL)
	   {tab = file->host_chart;

/* make an independent copy in case the one in the file chart is released */
	    CFREE(desc->cast_memb);
	    desc->cast_memb = CSTRSAVE(contr);
	    desc->cast_offs = _PD_member_location(contr, tab, dp, &lst);

	    SC_mark(contr, 1);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SIZE_FROM - tell PDBLib that the size of a particular member MEMB
 *              - (which must be a pointer) is specified by a
 *              - comma delimited list of other members CONTR
 *              - (which must be fixed point or pointers to fixed point)
 *              - Put this information into the FILE chart.
 *              - return TRUE iff successful
 *
 * #bind PD_size_from fortran() scheme() python()
 */

int PD_size_from(PDBfile *file ARG(,,cls), char *type,
		 char *memb, char *contr)
   {int rv;
    long is, ns;
    inti off;
    int64_t *so;
    char **sa, **sm;
    hasharr *tab;
    defstr *dp;
    memdes *desc, *lst;

    rv = TRUE;

    sa = SC_tokenize(contr, ", \t");
    SC_ptr_arr_len(ns, sa);

/* first make sure that "type" has a member "contr" which is of type char
 * this assumes that file->chart is a linked list
 */
    dp = PD_inquire_type(file, type);
    for (is = 0; (is < ns) && (rv == TRUE); is++)
        {desc = _PD_inquire_member(dp, sa[is]);
	 if (SC_fix_type_id(desc->base_type, FALSE) == -1)
	    {PD_error("BAD CAST CONTROLLER - PD_CAST", PD_GENERIC);
	     rv = FALSE;};};

/* now that we know such a size_from is reasonable,
 * add the size_from to the host charts
 */
    if (rv == TRUE)

/* add the size_from to the file->host_chart */
       {dp   = PD_inquire_host_type(file, type);
	desc = _PD_inquire_member(dp, memb);
	if (desc != NULL)
	   {tab = file->host_chart;

	    sm = CMAKE_N(char *, ns);
	    so = CMAKE_N(int64_t, ns);
	    if ((sm == NULL) || (so == NULL))
	       rv = FALSE;

	    else
	       {SC_free_strings(desc->size_memb);
		CFREE(desc->size_offs);

		desc->size_memb = sm;
		desc->size_offs = so;

		for (is = 0; is < ns; is++)
		    {off = _PD_member_location(sa[is], tab, dp, &lst);
		     sm[is] = CSTRSAVE(sa[is]);
		     so[is] = off;

		     SC_mark(sa[is], 1);};};};};

    SC_free_strings(sa);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
