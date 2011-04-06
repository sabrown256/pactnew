/*
 * PCLMPS.C - parallel communications routines for PPC under LMPS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc.h"

extern void
 INITENV(void),
 WAIT_FOR_END(void),
 NUM_NODES(int *count),
 WHO_AM_I(int *pid),
 COPY_ID(int *from, int *to),
 RECEIVE(int *pid, int *msgtype, void *msg,
	 PFInt matching_func, int *len),
 SRECEIVE(int *pid, int *msgtype, void *msg, 
	  int matching_pid, int matching_msgtype, int *len),
 SEND(int *pid, int *msgtype, void *msg, int len, int ack),
 SENDR(int *pid, int *msgtype, void *msg, int len),
 BROADCAST(int *pid, int msgtyp, void *msg, int *len);

extern int
 RECVNW(int *pid, int *msgtype, void *msg, 
	PFInt matching_func, int *len),
 SRECVNW(int *pid, int *msgtype, void *msg, 
	 int matching_pid, int matching_msgtype, int *len);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_OPEN_GROUP - open a copy of the named executable on each available
 *               - node
 */

PC_open_group(name, mode, pn, nl)
   char *name, *mode;
   int *pn;
   PROCESS *nl;
   {int i, n, mytid;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_OPEN_MEMBER - open a copy of the named executable on each available
 *                - node
 */

PROCESS *PC_open_member(pnn)
   int *pnn;
   {PROCESS *pp;

    pp = CMAKE(PROCESS);

    INITENV();  
    NUM_NODES(pnn);
    WHO_AM_I(&(pp->acpu));

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
    int block_state;

    type_index   = 0;
    default_node = -1;
    default_id   = -1;
    host_node    = -1;
    block_state  = TRUE;

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

	        case PC_BLOCK_STATE :
                     block_state = *p++;
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

    if (nn == -1)
       BROADCAST(&dn, ityp, bf, nb);
    else
       {for (i = 0; i < nn; i++)
	    {dn = *pnl++;
	     if (dn == PC_GROUP_LEADER)
	        dn = host_node;

	     SEND(&dn, ityp, bf, nb, 0);

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
    int oid, otyp;
    int type_index, block_state;

    ityp = 0;
    type_index  = 0;
    block_state = TRUE;
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

	        case PC_BLOCK_STATE :
                     block_state = *p++;
                     break;

	        default :
		     more = FALSE;
		     break;};};};

    nb = ni*sizeof(int);
    if (block_state)
       SRECEIVE(&oid, &otyp, bf, pp->acpu, ityp, nb);
    else
       SRECVNW(&oid, &otyp, bf, pp->acpu, ityp, nb);

    if (_SC_debug)
       PRINT(stdout, "Node %d got %d bytes\n", pp->acpu, nb);

    return((int) ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_SYNC_EXECUTION - synchronize the execution of the tasks */

void PC_sync_execution()
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
