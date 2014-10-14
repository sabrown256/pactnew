/*
 * PNMPI.C - MPI parallel communications routines for distributed
 *         - or multitasked configuration
 *
 */

#include "cpyright.h"

#include "ppc_int.h"
#include "scope_proc.h"
#include "scope_mpi.h"

#ifdef HAVE_MPI

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_OPEN_MEMBER_D - open a copy of the specified executable on this node */

static PROCESS *_PN_open_member_d(char **argv, int *pnn)
   {int port, argc;
    char *tok, t[MAXLINE], srvr[MAXLINE], *s, *u;
    PROCESS *pp;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

/* if the server is the parent of this process, argv will have the
 * server name and port number just after the first NULL item in argv
 */
    srvr[0] = '\0';
    port    = -1;
    for (argc = 0; argv[argc] != NULL; argc++);

    SC_strncpy(t, MAXLINE, argv[--argc], MAXLINE-1);

    tok = SC_strtok(t, ":", s);
    if (strcmp(tok, "HOST") == 0)
       {u = SC_strtok(NULL, ",\n", s);
        SC_strncpy(srvr, MAXLINE, t, -1);
	u    = SC_strtok(NULL, ",\n", s);
	port = SC_stoi(u);
        argv[argc] = NULL;};

    pp = PN_mk_process(argv, "rb+", SC_CHILD);

/* initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &SC_gs.comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &SC_gs.comm_size);

/* conditional diagnostic messages */
    if (ps->debug)
       {snprintf(t, MAXLINE, "PN_clnt_log.%d", (int) getpid());
	ps->diag = SC_fopen_safe(t, "w");
	fprintf(ps->diag, "\n\n   Node #%d at %s:%d.%d\n",
		pp->acpu, srvr, port, pp->data);
	SC_fflush_safe(ps->diag);};

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_CLOSE_MEMBER_D - close the member process */

static void _PN_close_member_d(PROCESS *pp)
   {SC_thread_proc *ps;

    ps = pp->tstate;

    MPI_Finalize();
    SC_close(pp);

/* conditional diagnostic messages */
    if (ps->debug)
       {SC_fclose_safe(ps->diag);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_SIZE_MESSAGE_D - return the number of items of the specified type
 *                    - in a message 
 *                    -   SP   - destination processor index
 *                    -   TYPE - type of items
 *                    -   TAG  - message tag
 */

static long _PN_size_message_d(int sp, char *type, int tag)
   {int ni;

    MPI_Status stat;

    MPI_Probe(sp, tag, MPI_COMM_WORLD, &stat);
    MPI_Get_count(&stat, MPI_CHAR, &ni);

    return((long) ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_GLMN_MESSAGE_D - return the global minimum of some quantity */

static double _PN_glmn_message_d(double vi)
   {double vo;

    MPI_Allreduce(&vi, &vo, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

    return(vo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_OUT_D - write data out to the filtered list of nodes
 *           - this does a message passing system's SEND command
 *           - or a LINDA-like PUT
 */

static long _PN_out_d(void *vr, char *type, size_t ni, PROCESS *pp, int *filt)
   {int i, ityp, dn, nb, nbr, ok;
    int block, buf_siz;
    int type_index, default_node, default_id, host_node;
    int *nl, *pnl, *pl, nn, np;
    long nis;
    char types[MAXLINE], *bf;
    PDBfile *vif, *tf;
    SC_thread_proc *ps;

    ps = pp->tstate;

    ok = SC_ERR_TRAP();
    if (ok != 0)
       return(-1L);

    vif = pp->vif;

    _PN_extract_filter_info(filt,
			    &type_index, &ityp,
			    &host_node, &default_node, &default_id,
			    &block, &buf_siz,
			    &nn, &nl, &np, &pl);
			    
    if (nl == NULL)
       {nn = 1;
	pnl = &default_node;}
    else
       {pnl = nl;
	nn  = *pnl++;};

/* conditional diagnostic messages */
    if (ps->debug)
       {fprintf(ps->diag, "   Write");
	fprintf(ps->diag, " Attempt(%d,%s,%d)",	(int) ni, type, pp->acpu);
	SC_fflush_safe(ps->diag);};

/* get the buffer size */
    nbr = PD_sizeof(vif, type, ni, vr);
    nb  = (buf_siz > 0) ? buf_siz : nbr;
    if (nb < nbr)
       SC_error(-1, "SPECIFIED BUFFER SIZE TOO SMALL - PN_OUT");

/* allocate the buffer */
    bf = CMAKE_N(char, nb);

/* convert the data into a message buffer */
    tf  = PN_open_process(vif, bf);
    nis = SC_write(tf, type, ni, vr) ? ni : 0;
    SC_close(tf);

/* send the message now */
   {MPI_Request requ;

    for (i = 0; i < nn; i++)
        {dn = *pnl++;

	 if (dn == -1)
	    {dn = SC_get_processor_number();
	     MPI_Bcast(bf, nb, MPI_CHAR, dn, MPI_COMM_WORLD);
	     break;}

	 else if (block)
	    {MPI_Send(bf, nb, MPI_CHAR, dn, ityp,
		      MPI_COMM_WORLD);
	     CFREE(bf);}

	 else
	    {MPI_Isend(bf, nb, MPI_CHAR, dn, ityp,
		       MPI_COMM_WORLD, &requ);

	     PN_push_pending(pp, SC_WRITE_MSG,
			     bf, type, ni, vr, (void *) &requ);};};};

/* conditional diagnostic messages */
    if (ps->debug)
       {fprintf(ps->diag, " Sent(%ld,%s,%d)\n", nis, types, dn);
	SC_fflush_safe(ps->diag);};

    SC_ERR_UNTRAP();

    return(nis);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_IN_D - read data in from the filtered list of nodes
 *          - this does a message passing system's RECV command
 *          - or a LINDA-like GET
 *          - return the number of items successfully read in
 */

static long _PN_in_d(void *vr, char *type, size_t ni, PROCESS *pp, int *filt)
   {long nir, nb;
    int ip, ityp, nn, np, *nl, *pl;
    int type_index, block, buf_siz;
    char *bf;
    PDBfile *vif, *tf;
    SC_thread_proc *ps;

    ps = pp->tstate;

    vif = pp->vif;
    nir = ni;

    _PN_extract_filter_info(filt,
			    &type_index, &ityp,
			    NULL, NULL, NULL,
			    &block, &buf_siz,
			    &nn, &nl, &np, &pl);

    if (nl == NULL)
       ip = 0;
    else
       ip  = nl[1];

/* conditional diagnostic messages */
    if (ps->debug)
       {fprintf(ps->diag, "   Read");
	SC_fflush_safe(ps->diag);};

/* get the buffer size and allocate it */
    if (buf_siz > 0)
       nb = buf_siz;
    else
       nb = PN_size_message(ip, type, ityp);

    bf = CMAKE_N(char, nb);

    if (block)
       {MPI_Status stat;

	MPI_Recv(bf, nb, MPI_CHAR, ip, ityp,
		 MPI_COMM_WORLD, &stat);}

    else
       {MPI_Request requ;

	MPI_Irecv(bf, nb, MPI_CHAR, ip, ityp,
		  MPI_COMM_WORLD, &requ);

	PN_push_pending(pp, SC_READ_MSG,
			bf, type, ni, vr, (void *) &requ);};

/* convert the message to the requested output data */
    if (block)
       {tf  = PN_open_process(vif, bf);
	nir = SC_read(tf, type, ni, vr);
	SC_close(tf);

	CFREE(bf);};

/* conditional diagnostic messages */
    if (ps->debug)
       {if (nir > 0)
	   fprintf(ps->diag, "\n");
	else
           fprintf(ps->diag, " Nothing\n");
	SC_fflush_safe(ps->diag);};

    return(nir);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PN_WAIT_D - wait for all pending communications to finish
 *            - complete the work, free the buffers, and
 *            - return the number of pendings honored
 */

static long _PN_wait_d(PROCESS *pp)
   {int i, np, oper;
    char *type, *bf;
    void *vr;
    size_t ni;
    PDBfile *vif, *tf;
    SC_thread_proc *ps;

    ps = pp->tstate;

    vif = pp->vif;

    np = pp->n_pending;

/* conditional diagnostic messages */
    if (ps->debug)
       {fprintf(ps->diag, "   Wait");
	SC_fflush_safe(ps->diag);};

   {MPI_Status *stats;
    MPI_Request *reqs;

    stats = CMAKE_N(MPI_Status, np);

    reqs = SC_array_array(_PC.reqs);

    MPI_Waitall(np, reqs, stats);

    CFREE(reqs);

    CFREE(stats);}

/* convert the message to the requested output data */
    for (i = 0; i < np; i++)
        {PN_pop_pending(pp, &oper, &bf, &type, &ni, &vr);

	 if (oper == SC_READ_MSG)
	    {tf = PN_open_process(vif, bf);
	     SC_read(tf, type, ni, vr);
	     SC_close(tf);};

	 CFREE(type);
	 CFREE(bf);};

/* conditional diagnostic messages */
    if (ps->debug)
       fprintf(ps->diag, "\n");

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_INIT_MPI - init for function pointers */

void PN_init_mpi(PN_par_fnc *p)
   {

    p->open_member  = _PN_open_member_d;
    p->close_member = _PN_close_member_d;
    p->size_message = _PN_size_message_d;
    p->glmn_message = _PN_glmn_message_d;
    p->out          = _PN_out_d;
    p->in           = _PN_in_d;
    p->wait         = _PN_wait_d;

    return;}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* PN_INIT_MPI - init for function pointers */

void PN_init_mpi(PN_par_fnc *p)
   {

    p->open_member  = NULL;
    p->close_member = NULL;
    p->size_message = NULL;
    p->glmn_message = NULL;
    p->out          = NULL;
    p->in           = NULL;
    p->wait         = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

