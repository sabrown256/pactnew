/*
 * PCPARC.C - parallel communications routines for PPC for distributed
 *          - or multitasked configuration
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc_int.h"
#include "scope_proc.h"
#include "scope_mpi.h"

#ifdef HAVE_MPI
# define HAVE_DPE
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_PUT_DATA - put the data out and return the confirmation info */

static int _PC_put_data(PROCESS *pp, char *bf, char *type, size_t ni,
			int nb, int dn, long *pni, char *types, int nc,
			int *pi)
   {int nbo, data;
    char reply[MAXLINE], fmt[MAXLINE];
    char *pbf;
    

    PC_printf(pp, "%c,%s,%ld,%d\n", PC_FWRITE, type, ni, dn);

    data = pp->data;
    pbf  = bf;
    while (nb > 0)
       {PC_buffer_data_in(pp);
	nbo = SC_write_sigsafe(data, pbf, nb);
	if (nbo < 0)
	   continue;
	else if (_SC_ps.debug)
	   {fprintf(_PC_diag, ".%d", nbo);
	    fflush(_PC_diag);};
	nb  -= nbo;
	pbf += nbo;};

    PC_block(pp);
    PC_gets(reply, MAXLINE, pp);

    snprintf(fmt, MAXLINE, "%%ld,%%%ds,%%d\n", nc);
    sscanf(reply, fmt, pni, types, pi);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_EXTRACT_FILTER_INFO - extract the message description information
 *                         - from the message filter array
 */

void _PC_extract_filter_info(int *filt, int *pti, int *pit,
			     int *phn, int *pdn, int *pdi,
			     int *pbs, int *pbz, int *pnn,
			     int **pnl, int *pnp, int **ppl)
   {int type_index, ityp, host_node, default_node, default_id;
    int block_state, buffer_size, more, nn, np;
    int *nl, *pl, *p;
			    
    type_index   = 0;
    default_node = -1;
    default_id   = -1;
    host_node    = -1;
    block_state  = TRUE;
    buffer_size  = 0;

    ityp = 0;
    np   = 0;
    nn   = 0;
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
                     nl = p;
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

	        case PC_BUFFER_SIZE :
                     buffer_size = *p++;
                     break;

	        default :
		     more = FALSE;
		     break;};};};

    *pti = type_index;
    *pit = ityp;
    *pbs = block_state;
    *pbz = buffer_size;
    *pnn = nn;
    *pnl = nl;
    *pnp = np;
    *ppl = pl;

    if (phn != NULL)
       *phn = host_node;

    if (phn != NULL)
       *phn = default_node;

    if (pdi != NULL)
       *pdi = default_id;

    return;}

/*--------------------------------------------------------------------------*/

#ifdef HAVE_DPE

/*--------------------------------------------------------------------------*/

/* PC_PUSH_PENDING - push an I/O transaction onto the pending list */

void PC_push_pending(PROCESS *pp, int op, char *bf, char *type,
		     size_t ni, void *vr, void *req)
   {SC_pending_msg *pm;

    pm = CMAKE(SC_pending_msg);

    pm->bf     = bf;
    pm->type   = CSTRSAVE(type);
    pm->nitems = ni;
    pm->vr     = vr;
    pm->oper   = op;
    pm->req    = pp->n_pending++;
    pm->nxt    = pp->pending;

    pp->pending = pm;

#ifdef HAVE_MPI

    {MPI_Request requ;

     requ = *(MPI_Request *) req;

     if (_PC.reqs == NULL)
        _PC.reqs = CMAKE_ARRAY(MPI_Request, NULL, 0);

     SC_array_push(_PC.reqs, &requ);};

#endif

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* PC_POP_PENDING - pop a completed pending message */

void PC_pop_pending(PROCESS *pp, int *po, char **pbf, char **pty,
		    size_t *pni, void **pvr)
   {SC_pending_msg *pm;

    pm = pp->pending;

    if (pm != NULL)
       {pp->pending = pm->nxt;
	pp->n_pending--;

	*po  = pm->oper;
	*pbf = pm->bf;
	*pty = pm->type;
	*pni = pm->nitems;
	*pvr = pm->vr;

	CFREE(pm);};

#ifdef HAVE_MPI

    SC_array_pop(_PC.reqs);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_SYNC_EXECUTION - synchronize the execution of the tasks */

void PC_sync_execution(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_OPEN_GROUP - open a copy of the named executable on each available
 *               - node
 */

int PC_open_group(char **argv, int *pn)
   {int i, argc, offs;
    char **args, s[MAXLINE], *t, *p;
    PROCESS *pp;

#if defined(HAVE_POSIX_SYS)

    PC_init_communications(NULL);

    _PC.n_nodes = 0;

    offs = 3;
    argc = 0;
    while (argv[argc++] != NULL);

    args = CMAKE_N(char *, argc + offs);

    args[0] = CSTRSAVE("pcexec");
    args[1] = CSTRSAVE("-r");
    args[2] = CSTRSAVE("1");

    for (i = 0; i < argc; i++)
        args[i + offs] = argv[i];

    pp = PC_open(args, NULL, "rb+");
    if (pp != NULL)
       {PC_block(pp);
	PC_gets(s, MAXLINE, pp);

	p = SC_strtok(s, ",", t);
	if (p != NULL)
	   strcpy(_PC.server, p);

	_PC.server_port = SC_stoi(SC_strtok(NULL, ",\n", t));
	_PC.n_nodes  = SC_stoi(SC_strtok(NULL, ",\n", t));
	_SC_ps.debug   = SC_stoi(SC_strtok(NULL, ",\n", t));

	for (i = 0; i < offs; i++)
	    CFREE(args[i]);
	CFREE(args);

	if (pn != NULL)
	   *pn = _PC.n_nodes;

	_PC.server_link = pp;};

#endif

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                            NATIVE DP ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PC_OPEN_MEMBER_N - open a copy of the specified executable on this node */

static PROCESS *_PC_open_member_n(char **argv, int *pnn)
   {int port, argc;
    char *tok, t[MAXLINE], srvr[MAXLINE], *s, *p;
    PROCESS *pp;

#if defined(HAVE_POSIX_SYS)

/* if the server is the parent of this process, argv will have the
 * server name and port number just after the first NULL item in argv
 */
    srvr[0] = '\0';
    port    = -1;
    for (argc = 0; argv[argc] != NULL; argc++);

    SC_strncpy(t, MAXLINE, argv[--argc], MAXLINE-1);

    tok = SC_strtok(t, ":", s);
    if ((tok != NULL) && (strcmp(tok, "HOST") == 0))
       {p = SC_strtok(NULL, ",\n", s);
	if (p != NULL)
	   strcpy(srvr, p);

	p = SC_strtok(NULL, ",\n", s);
	port = (p != NULL) ? SC_stoi(p) : 0;

        argv[argc] = NULL;};

    pp = PC_mk_process(argv, "rb+", SC_CHILD);

    if (pnn != NULL)
       {if ((srvr[0] == '\0') || (port < 0))
	   {PC_open_group(argv, pnn);

	    strcpy(srvr, _PC.server);
	    port = _PC.server_port;

	    pp->io[0] = _PC.server_link->io[0];
	    pp->io[1] = _PC.server_link->io[1];
	    pp->io[2] = _PC.server_link->io[2];}

       else
	  {pp->io[0] = 0;
	   pp->io[1] = 1;
	   pp->io[2] = 2;};

	PC_block(pp);
	PC_gets(t, MAXLINE, pp);

	sscanf(t, "%d,%d,%d\n", &pp->acpu, pnn, &_SC_ps.debug);

	PC_printf(pp, "%s,%d,%d\n", srvr, port, (int) getpid());
	pp->data = PC_init_client(srvr, port);

	SC_gs.comm_size = *pnn;};

/* conditional diagnostic messages */
    if (_SC_ps.debug)
       {snprintf(t, MAXLINE, "PC_clnt_log.%d", (int) getpid());
	_PC_diag = fopen(t, "w");
	if (_PC_diag != NULL)
	   {fprintf(_PC_diag, "\n\n   Node #%d at %s:%d.%d\n",
		    pp->acpu, srvr, port, pp->data);
	    fflush(_PC_diag);};};

#else
	pp = NULL;
#endif

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_CLOSE_MEMBER_N - close the member process */

static void _PC_close_member_n(PROCESS *pp)
   {

#if defined(HAVE_POSIX_SYS)

    PC_close(pp);

/* conditional diagnostic messages */
    if (_SC_ps.debug)
       {fclose(_PC_diag);};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PC_SIZE_MESSAGE_N - return the number of items of the specified type
 *                    - in a message 
 *                    -   SP   - destination processor index
 *                    -   TYPE - type of items
 *                    -   TAG  - message tag
 */

static long _PC_size_message_n(int sp, char *type, int tag)
   {int ni;

    ni = 0;

    return((long) ni);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PC_GLMN_MESSAGE_N - return the global minimum of some quantity */

static double _PC_glmn_message_n(double vi)
   {int i, ip, np;
    double vo, vt;
    static int sp[] = {SC_MATCH_NODE, -1,
		       SC_MATCH_TAG, 0,
		       0};

    ip = PC_get_processor_number();
    np = PC_get_number_processors();

    sp[1] = -1;
    PC_out(&vi, SC_DOUBLE_S, 1, NULL, sp);

    vo = vi;
    vt = 0.0;

    for (i = 0; i < np; i++)
        {if (i == ip)
	    continue;

	 sp[1] = i;
	 PC_in(&vt, SC_DOUBLE_S, 1, NULL, sp);

	 vo = min(vo, vt);};

    return(vo);}

/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_OUT_N - write data out to the filtered list of nodes
 *           - this does a message passing system's SEND command
 *           - or a LINDA-like PUT
 */

static long _PC_out_n(void *vr, char *type, size_t ni, PROCESS *pp, int *filt)
   {int i, ityp, dn, nb, nbr, is, ok;
    int block, buf_siz;
    int type_index, default_node, default_id, host_node;
    int *nl, *pnl, *pl, nn, np;
    long nis, nib;
    char reply[MAXLINE], types[MAXLINE], *bf;
    PDBfile *vif, *tf;

    ok = SC_ERR_TRAP();
    if (ok != 0)
       return(-1L);

    vif = pp->vif;

    _PC_extract_filter_info(filt,
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
    if (_SC_ps.debug)
       {fprintf(_PC_diag, "   Write");
	fprintf(_PC_diag, " Attempt(%d,%s,%d)",	(int) ni, type, pp->acpu);
	fflush(_PC_diag);};

/* get the buffer size */
    nbr = PD_sizeof(vif, type, ni, vr);
    nb  = (buf_siz > 0) ? buf_siz : nbr;
    if (nb < nbr)
       SC_error(-1, "SPECIFIED BUFFER SIZE TOO SMALL - PC_OUT");

/* allocate the buffer */
    bf = CMAKE_N(char, nb);

/* convert the data into a message buffer */
    nis = 0;
    tf  = PN_open(vif, bf);
    if (tf != NULL)
       {nis = PN_write(tf, type, ni, vr) ? ni : 0;
	PN_close(tf);};

/* send the message now */
    PC_unblock(pp);
    while (PC_gets(reply, MAXLINE, pp) != NULL);
    PC_block(pp);

    if (nn == -1)
       {for (dn = 0; dn < _PC.n_nodes; dn++)
            _PC_put_data(pp, bf, type, ni, nb, dn, &nib,
			 types, MAXLINE, &is);}

    else
       {dn = 0;
	for (i = 0; i < nn; i++)
            {dn = *pnl++;
	     if (dn == SC_GROUP_LEADER)
  	        dn = host_node;

             _PC_put_data(pp, bf, type, ni, nb, dn, &nib,
			  types, MAXLINE, &is);};};

/* conditional diagnostic messages */
    if (_SC_ps.debug)
       {fprintf(_PC_diag, " Sent(%ld,%s,%d)\n", nis, types, dn);
	fflush(_PC_diag);};

    SC_ERR_UNTRAP();

    return(nis);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_IN_N - read data in from the filtered list of nodes
 *          - this does a message passing system's RECV command
 *          - or a LINDA-like GET
 *          - return the number of items successfully read in
 */

static long _PC_in_n(void *vr, char *type, size_t ni, PROCESS *pp, int *filt)
   {int ip, nis, ityp, nn, np, *nl, *pl;
    int type_index, block, buf_siz, bs;
    long nir, nb, nbt, nbr;
    char reply[MAXLINE], types[MAXLINE], *bf, *pbf;
    PDBfile *vif, *tf;

    vif = pp->vif;
    nir = ni;

    _PC_extract_filter_info(filt,
			    &type_index, &ityp,
			    NULL, NULL, NULL,
			    &block, &buf_siz,
			    &nn, &nl, &np, &pl);

    if (nl == NULL)
       ip = 0;
    else
       ip  = nl[1];

/* conditional diagnostic messages */
    if (_SC_ps.debug)
       {fprintf(_PC_diag, "   Read");
	fflush(_PC_diag);};

/* get the buffer size and allocate it */
    if (buf_siz > 0)
       nb = buf_siz;
    else
       nb = PC_size_message(ip, type, ityp);

    bf = CMAKE_N(char, nb);

    strcpy(types, type);

    if (block)
       PC_block(pp);
    else
       PC_unblock(pp);

    pbf = bf;
    nbr = nb;
    for (nbr = nb, bs = FALSE; nbr > 0; pbf += nbt, bs = block)
        {nbt = PC_buffer_data_out(pp, pbf, nbr, bs);
	 if (nbt > 0)
	    {if (_SC_ps.debug)
	        {fprintf(_PC_diag, " Recv(%ld,%s)", nbt, types);
		 fflush(_PC_diag);};};

         nbr -= nbt;
         if (nbr > 0)
	    {PC_printf(pp, "%c,%s,%ld,%d\n", PC_FREAD,
		       SC_CHAR_S, nbr, block);

	     PC_gets(reply, MAXLINE, pp);
	     sscanf(reply, "%d,%254s\n", &nis, types);

	     if (_SC_ps.debug && block && (nis > 0))
	        {fprintf(_PC_diag, " Expect(%d,%s)", nis, types);
		 fflush(_PC_diag);};};

         if (!block)
            break;};

/* convert the message to the requested output data */
    if (block)
       {tf = PN_open(vif, bf);
	if (tf != NULL)
	   {nir = PN_read(tf, type, ni, vr);
	    PN_close(tf);};

	CFREE(bf);};

/* conditional diagnostic messages */
    if (_SC_ps.debug)
       {if (nir > 0)
	   fprintf(_PC_diag, "\n");
	else
           fprintf(_PC_diag, " Nothing\n");
	fflush(_PC_diag);};

    return(nir);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_WAIT_N - wait for all pending communications to finish
 *            - complete the work, free the buffers, and
 *            - return the number of pendings honored
 */

static long _PC_wait_n(PROCESS *pp)
   {int i, np, oper;
    char *type, *bf;
    void *vr;
    size_t ni;
    PDBfile *vif, *tf;

    vif = pp->vif;

    np = pp->n_pending;

/* conditional diagnostic messages */
    if (_SC_ps.debug)
       {fprintf(_PC_diag, "   Wait");
	fflush(_PC_diag);};

/* convert the message to the requested output data */
    for (i = 0; i < np; i++)
        {PC_pop_pending(pp, &oper, &bf, &type, &ni, &vr);

	 if (oper == SC_READ_MSG)
	    {tf = PN_open(vif, bf);
	     if (tf != NULL)
	        {PN_read(tf, type, ni, vr);
		 PN_close(tf);};};

	 CFREE(type);
	 CFREE(bf);};

/* conditional diagnostic messages */
    if (_SC_ps.debug)
       fprintf(_PC_diag, "\n");

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_INIT_N - init for function pointers */

static void _PC_init_n(PC_par_fnc *p)
   {

    p->open_member  = _PC_open_member_n;
    p->close_member = _PC_close_member_n;
    p->size_message = _PC_size_message_n;
    p->glmn_message = _PC_glmn_message_n;
    p->out          = _PC_out_n;
    p->in           = _PC_in_n;
    p->wait         = _PC_wait_n;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_INIT_COMMUNICATIONS - initialize inter processor communications */

void PC_init_communications(void (*init)(PC_par_fnc *p))
   {

    if (init == NULL)
       _PC_init_n(&PC_gs.oper);
    else
       (*init)(&PC_gs.oper);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
