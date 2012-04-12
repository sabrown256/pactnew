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

/* _PD_INQUIRE_MEMBER - return the member of DP MEMB */

static memdes *_PD_inquire_member(defstr *dp, char *memb)
   {memdes *desc, *rv;

    rv = NULL;

    if ((dp != NULL) && (memb != NULL))
       {for (desc = dp->members; desc != NULL; desc = desc->next)
	    {if (strcmp(memb, desc->member) == 0)
	        {rv = desc;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INQUIRE_MEMBER_NAME - return the member of DP named MEMB */

static memdes *_PD_inquire_member_name(defstr *dp, char *memb)
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

/* _PD_CAST_INJECT - setup the cast specs from S into DESC */

static int _PD_cast_inject(hasharr *tab, defstr *dp, memdes *desc, char *s)
   {int rv;
    memdes *lst;

    rv = TRUE;

/* make an independent copy in case the one in the file chart is released */
    CFREE(desc->cast_memb);
    desc->cast_memb = CSTRSAVE(s);
    desc->cast_offs = _PD_member_location(s, tab, dp, &lst);

    SC_mark(s, 1);

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
				 memdes *desc, char *s)
   {int rv;
    char **sa;

    rv = FALSE;

    if ((s != NULL) && (desc != NULL))

/* size_from */
       {if (s[0] == '[')
	   {sa = SC_tokenize(s, "[], \t");
	    rv = _PD_size_from_inject(chrt, dp, desc, sa);
	    SC_free_strings(sa);}

/* type cast */
	else
	    rv = _PD_cast_inject(chrt, dp, desc, s);};

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
    memdes *desc;

    rv = TRUE;

/* first make sure that "type" has a member "contr" which is of type char
 * this assumes that file->chart is a linked list
 */
    dp   = PD_inquire_type(file, type);
    desc = _PD_inquire_member_name(dp, contr);
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
	desc = _PD_inquire_member_name(dp, memb);
	if (desc != NULL)
	   {tab = file->host_chart;
	    rv  = _PD_cast_inject(tab, dp, desc, contr);};};

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
