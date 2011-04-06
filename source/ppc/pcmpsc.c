/*
 * PCMPSC.C - parallel communications routines for PPC under MPSC
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc_int.h"
#include <mpsc/mpsc.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_OPEN_GROUP - open a copy of the named executable on each available
 *               - node
 */

PC_open_group(argv, pn)
   char **argv;
   int *pn;
   {
#if 0
int i, n;

    mpsc_init();

/* start up the nodes, the number of nodes is determined by 
 * the resource management environment variables
 */
    load(name, -1, 0);

    n = numnodes();
    for (i = 0; i < n; i++)
        nl[i].acpu = i;    

    *pn = n;
#endif
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_OPEN_MEMBER - open a copy of the named executable on each available
 *                - node
 */

PROCESS *PC_open_member(argv, pnn)
   char **argv;
   int *pnn;
   {PROCESS *pp;

    mpsc_init();

    pp = CMAKE(PROCESS);
    pp->acpu = mynode();
    pp->rcpu = myhost();

    *pnn = numnodes();
    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_OUT - write data out to the filtered list of nodes
 *        - this does a message passing system's SEND command
 *        - or a LINDA-like PUT
 */

PC_out(bf, type, ni, pp, filt)
   void *bf;
   char *type;
   size_t ni;
   PROCESS *pp;
   int *filt;
   {int i, j, ityp, dn, did, nb, t, more, *p;
    int type_index, default_node, default_id, host_node;
    int *nl, *pnl, *pl, *ppl, nn, np;

    type_index   = 0;
    default_node = -1;
    default_id   = 0;
    host_node    = myhost();

    ityp = 0;
    nl   = NULL;
    pl   = NULL;
    if (filt != NULL)
       {p = filt;
	more = TRUE;
	while (more)
	   {switch (*p++)
	       {case SC_MATCH_TYPE :
		     ityp |= *p++;
		     ityp |= ((type_index << 16) & PC_TYPE_MASK);
		     break;

	        case SC_MATCH_TAG :
		     ityp |= (*p++ & PC_TAG_MASK);
		     break;

	        case SC_MATCH_NODE :
                     nl = p++;
                     nn = *p++;
                     p += nn;
		     break;

	        case SC_MATCH_PID :
		     pl = p;
                     np = *p++;
                     p += np;
		     break;

	        default :
		     more = FALSE;
		     break;};};};

    nb = ni*sizeof(int);

    if (nl == NULL)
       {nn = 1;
	pnl = &default_node;}
    else
       {pnl = nl;
	nn  = *pnl++;};

    for (i = 0; i < nn; i++)
        {dn = *pnl++;
	 if (dn == PC_GROUP_LEADER)
	    dn = host_node;

	 if (pl == NULL)
	    {np = 1;
	     ppl = &default_id;}
	 else
	    {ppl = pl;
	     np  = *ppl++;};

	 for (j = 0; i < np; i++)
	     {did = *ppl++;
	      csend(ityp, bf, nb, dn, did);

	      if (_SC_debug)
	         PRINT(stdout, "\t\t\tNode %d sent %d bytes to %d\n",
		       pp->acpu, nb, dn);};};

    return((int) ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_IN - read data in from the filtered list of nodes
 *       - this does a message passing system's RECV command
 *       - or a LINDA-like GET
 */

PC_in(bf, type, ni, pp, filt)
   void *bf;
   char *type;
   size_t ni;
   PROCESS *pp;
   int *filt;
   {int i, n, ityp, dn, did, nb, t, more, *p;
    int type_index;

    ityp = 0;
    type_index = 0;
    if (filt != NULL)
       {p = filt;
	more = TRUE;
	while (more)
	   {switch (*p++)
	       {case SC_MATCH_TYPE :
		     ityp |= *p++;
		     ityp |= (type_index << 16) & PC_TYPE_MASK;
		     break;

	        case SC_MATCH_TAG :
		     ityp |= (*p++ & PC_TAG_MASK);
		     break;

	        case SC_MATCH_NODE :
	        case SC_MATCH_PID  :
		     break;

	        default :
		     more = FALSE;
		     break;};};};

    nb = ni*sizeof(int);
    crecv(ityp, bf, nb);

    if (_SC_debug)
       PRINT(stdout, "Node %d got %d bytes\n", pp->acpu, nb);

    return((int) ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_SYNC_EXECUTION - synchronize the execution of the tasks */

void PC_sync_execution()
   {gsync();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
