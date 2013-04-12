/*
 * PCPIO.C - I/O hooks suitable for use with PDBLib and parallel communications
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc_int.h"
#include "scope_proc.h"

#define DATA_BLOCK 4096

struct s_REMOTE_FILE
   {PROCESS *pp;
    int type;
    int fid;
    int64_t file_size;
    char *buffer;
    long sb_addr;
    long size;
    int64_t cf_addr;};

typedef struct s_REMOTE_FILE REMOTE_FILE;

#define MESSAGES(x)         PC_procs.m[(x)].msg
#define MESSAGE_TYPES(x)    PC_procs.m[(x)].type
#define MESSAGE_LENGTHS(x)  PC_procs.m[(x)].ni
#define N_MESSAGES(x)       PC_procs.m[(x)].n
#define MAX_MESSAGES(x)     PC_procs.m[(x)].nx

struct s_NODE_LIST
   {int n;
    int nx;
    PROCESS **p;
    SC_message *m;
    SC_poll_desc *fd;};

typedef struct s_NODE_LIST NODE_LIST;

static NODE_LIST
 PC_procs = {0, 0, NULL, NULL};

/*--------------------------------------------------------------------------*/

/*                           SERVER SIDE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PC_REGISTER_PROC - register a child process */

static int _PC_register_proc(PROCESS *pp, int i)
   {int n, nx;
    SC_poll_desc pd;

    if (pp != NULL)
       {memset(&pd, 0, sizeof(pd));

	n  = PC_procs.n;
	nx = PC_procs.nx;
	if (PC_procs.p == NULL)
	   {nx = i + 8;
	    PC_procs.p  = CMAKE_N(PROCESS *, nx);
	    PC_procs.m  = CMAKE_N(SC_message, nx);
	    PC_procs.fd = CMAKE_N(SC_poll_desc, nx);};

	if (i >= nx)
	   {nx = i + 8;
	    CREMAKE(PC_procs.p, PROCESS *, nx);
	    CREMAKE(PC_procs.m, SC_message, nx);
	    CREMAKE(PC_procs.fd, SC_poll_desc, nx);};

	pd.fd     = pp->io[0];
	pd.events = POLLIN | POLLPRI;

	PC_procs.p[i]  = pp;
	PC_procs.fd[i] = pd;

	PC_procs.n  = max(n, i) + 1;
	PC_procs.nx = nx;

	PC_unblock(pp);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_SETUP_CHILDREN - spawn the specified set of children */

static int _PC_setup_children(char **argv, char *mode)
   {int i, n, port, argc, which, cbk, off;
    long parentp;
    char **args, s[MAXLINE], host[MAXLINE];
    PROCESS *pp;

#if defined(HAVE_POSIX_SYS)
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    gethostname(host, MAXLINE);
    port = PC_init_server(SC_GET_PORT, FALSE);
    if (port == -1)
       return(FALSE);

    n         = 0;
    which     = 0;
    off       = 1;
    parentp   = FALSE;
    ps->debug = FALSE;
    for (i = 1; argv[i] != NULL; i++)
        {if (argv[i][0] == '-')
	    {switch (argv[i][1])
	        {case 'a' :
	              which = SC_stoi(argv[++i]);
		      break;
	         case 'l' :
		      ps->debug = TRUE;
		      break;
		 case 'n' :
	              n = SC_stoi(argv[++i]);
		      break;
		 case 'r' :
		      if (SC_numstrp(argv[i+1]))
			 parentp = SC_stoi(argv[++i]);
		      break;};}
	 else
	    off = i;};

    argv += off;

    if (ps->debug)
       {snprintf(s, MAXLINE, "PC_srvr_log.%d", (int) getpid());
	ps->diag = SC_fopen_safe(s, "w");
	if (ps->diag != NULL)
	   {fprintf(ps->diag, "\n   ------ Begin Setup ------\n\n");
	    fprintf(ps->diag, "Requesting %d nodes (%d)\n", n, which);
	    fprintf(ps->diag, "Exec: %s %s\n\n", argv[0], argv[1]);
	    SC_fflush_safe(ps->diag);};};

    if (parentp)

/* the process on the other end of stdin, and stdout is either the
 * master application or the first of an SPMD application
 * remember it as node specified by "which"
 */
       {pp = PC_mk_process(argv, mode, SC_CHILD);
	if ((ps->debug) && (ps->diag != NULL))
	   {fprintf(ps->diag, "Open node #%d (%p)\n", which, pp);
	    SC_fflush_safe(ps->diag);};

	if (pp != NULL)
	   {pp->io[0] = 0;
	    pp->io[1] = 1;
	    pp->io[2] = 2;
	    pp->acpu  = which;

/* this goes to PC_open_group */
	    PC_printf(pp, "%s,%d,%d,%d\n", host, port, n, ps->debug);

/* this goes to PC_open_member */
	    PC_printf(pp, "%d,%d,%d\n", which, n, ps->debug);
	    PC_block(pp);
	    PC_gets(s, MAXLINE, pp);
	    if ((ps->debug) && (ps->diag != NULL))
	       {fprintf(ps->diag, "Latch up with parent: %s", s);
		SC_fflush_safe(ps->diag);};

	    if ((ps->debug) && (ps->diag != NULL))
	       {fprintf(ps->diag, "Get data socket for node #%d\n",
			which);
		SC_fflush_safe(ps->diag);};
	    pp->data = PC_init_server(SC_GET_CONNECTION, FALSE);
	    if ((ps->debug) && (ps->diag != NULL))
	       {fprintf(ps->diag, "Data socket for node #%d: %d\n",
			which, pp->data);
		SC_fflush_safe(ps->diag);};

	    cbk = _PC_register_proc(pp, which);
	    if ((ps->debug) && (ps->diag != NULL))
	       {fprintf(ps->diag, "I/O interrupt: %d\n\n", cbk);
		SC_fflush_safe(ps->diag);};};}

/* the process on the other end of stdin and stdout is a terminal */
    else
       which = -1;

/* setup a special new arglist for the new children */
    for (argc = 0; argv[argc] != NULL; argc++);
    args = CMAKE_N(char *, argc + 1);
    for (argc = 0; argv[argc] != NULL; argc++)
        args[argc] = argv[argc];

/* add an additional argument at the end of the old list */
    snprintf(s, MAXLINE, "HOST:%s,%d", host, port);
    args[argc] = CSTRSAVE(s);
    args[argc+1] = NULL;

/* open the requested additional nodes */
    for (i = 0; i < n; i++)
        {if (i == which)
            continue;

         pp = PC_open(args, NULL, mode);

	 if ((ps->debug) && (ps->diag != NULL))
	    {fprintf(ps->diag, "Open node #%d (%p)\n", i, pp);
	     SC_fflush_safe(ps->diag);};

	 if (pp != NULL)
	    {PC_printf(pp, "%d,%d,%d\n", i, n, ps->debug);
	     pp->acpu = i;

	     PC_block(pp);
	     PC_gets(s, MAXLINE, pp);
	     if ((ps->debug) && (ps->diag != NULL))
	        {fprintf(ps->diag, "Latch up with child: %s", s);
	         fprintf(ps->diag, "Get data socket for node #%d\n", i);
		 SC_fflush_safe(ps->diag);};
	     pp->data = PC_init_server(SC_GET_CONNECTION, FALSE);
	     if ((ps->debug) && (ps->diag != NULL))
	        {fprintf(ps->diag, "Data socket for node #%d: %d\n",
			 i, pp->data);
		 SC_fflush_safe(ps->diag);};

	     cbk = _PC_register_proc(pp, i);
	     if ((ps->debug) && (ps->diag != NULL))
	        {fprintf(ps->diag, "I/O interrupt: %d\n\n", cbk);
		 SC_fflush_safe(ps->diag);};};};

    CFREE(args[argc]);
    CFREE(args);

    if ((ps->debug) && (ps->diag != NULL))
       {fprintf(ps->diag, "   ------ End of Setup ------\n\n");
	SC_fflush_safe(ps->diag);};

#endif

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_GET_MSG - send the first message on the queue for the specified
 *             - node
 */

static int _PC_get_msg(int i)
   {inti ni, nbo, nbe;
    intb bpi;
    int *nis, data;
    char *type, **msg, **typ, *pbf;
    PROCESS *pi;
    PDBfile *pf;
    SC_thread_proc *ps;

    msg = MESSAGES(i);
    typ = MESSAGE_TYPES(i);
    nis = MESSAGE_LENGTHS(i);
    pi  = PC_procs.p[i];
    pf  = (PDBfile *) pi->vif;

    ps = pi->tstate;

    if ((msg != NULL) && (typ != NULL) && (nis != NULL))
       {pbf  = msg[0];
        type = typ[0];
        ni   = nis[0];
        data = pi->data;
        if (pbf != NULL)
	   {if (ps->debug)
	       {fprintf(ps->diag, "   Read");
		SC_fflush_safe(ps->diag);};

	    PC_printf(pi, "%s,%s\n", SC_itos(NULL, 0, ni, NULL), type);

	    if (ps->debug)
	       {fprintf(ps->diag, " Put(%s,%s)",
			SC_itos(NULL, 0, ni, NULL), type);
		SC_fflush_safe(ps->diag);};

	    bpi = PN_sizeof(type, pf->host_chart);
	    nbo = ni*bpi;
	    while (nbo > 0)
 	       {PC_buffer_data_in(pi);
		nbe = SC_write_sigsafe(data, pbf, nbo);
                if (nbe < 0)
                   continue;
		else if (ps->debug)
		   {fprintf(ps->diag, ".%s", SC_itos(NULL, 0, nbe, NULL));
		    SC_fflush_safe(ps->diag);};

		pbf += nbe;
		nbo -= nbe;};

	    if (ps->debug)
	       {fprintf(ps->diag, " Sent(%s,%s,%d)\n",
			SC_itos(NULL, 0, ni - nbo/bpi, NULL),
			type, i);
		SC_fflush_safe(ps->diag);};

            PC_pop_message(i);

	    return(TRUE);};};

    PC_printf(pi, "0,none\n");

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_PUT_MSG - get the message from the specified node */

static int _PC_put_msg(PROCESS *pi, char *type, inti ni, int indx)
   {inti nbi, nbt, nir;
    intb bpi;
    char *bf;
    PDBfile *pf;
    SC_thread_proc *ps;

    ps = pi->tstate;

    if ((pi != NULL) && (type != NULL))
       {pf = (PDBfile *) pi->vif;

	bpi = PN_sizeof(type, pf->host_chart);
	nbi = ni*bpi;
	bf  = CMAKE_N(char, nbi);
	if (ps->debug)
	   {fprintf(ps->diag, "   Write Get(%s,%s,%d)",
		    SC_itos(NULL, 0, ni, NULL), type, pi->acpu);
	    SC_fflush_safe(ps->diag);};

	nbt = PC_buffer_data_out(pi, bf, nbi, TRUE);
	nir = nbt/bpi;

	PC_push_message(PC_procs.m + indx, indx, ni, type, bf);

	if (ps->debug)
	   {fprintf(ps->diag, " Recv(%s,%s,%d)\n",
		    SC_itos(NULL, 0, nir, NULL), type, indx);
	    SC_fflush_safe(ps->diag);};

	PC_printf(pi, "%d,%s,%d\n", nir, type, indx);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_GET_MESSAGE - handle input from the specified child */

static int _PC_get_message(int i)
   {int ni, code, indx, c;
    char s[MAXLINE], *type, *t;
    PROCESS *pi;
    SC_thread_proc *ps;

    pi = PC_procs.p[i];
    if (pi == NULL)
       return(FALSE);

    ps = pi->tstate;

    s[0] = '\0';
    if (PC_gets(s, MAXLINE, pi) == NULL)
       return(FALSE);

    c = s[0];
    if (ps->debug && SC_is_print_char(c, 4))
       {fprintf(ps->diag, "Message from node #%d: %s", i, s);
	SC_fflush_safe(ps->diag);};

    code = s[0];

    SC_strtok(s, ",", t);
    switch (code)
       {case SC_FSETVBUF :
             break;

        case SC_FCLOSE :
             break;

        case SC_FFLUSH :
             break;

        case SC_FTELL :
             break;

        case SC_FSEEK :
             break;

        case SC_FREAD :
	     type = SC_strtok(NULL, ",\n", t);
	     ni   = SC_stol(SC_strtok(NULL, ",\n", t));

/*	     blk  = SC_stol(SC_strtok(NULL, ",\n", t)); */
	     SC_strtok(NULL, ",\n", t);

	     _PC_get_msg(i);

	     break;

        case SC_FWRITE :
             type = SC_strtok(NULL, ",\n", t);
	     ni   = SC_stol(SC_strtok(NULL, ",\n", t));
	     indx = SC_stol(SC_strtok(NULL, ",\n", t));

             _PC_put_msg(pi, type, ni, indx);

	     break;

        case SC_FPUTS :
             break;

        case SC_FGETS :
             break;

        case SC_FGETC :
             break;

        case SC_FUNGETC :
             break;

        case SC_FEXIT :
             exit(0);

        default :
             break;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_PROCESS_ACCESS - carry out file access commands on the named host
 *                   - this is the server end
 *                   - the command codes are:
 *                   -   SC_FOPEN
 *                   -   SC_FCLOSE
 *                   -   SC_FFLUSH
 *                   -   SC_FTELL
 *                   -   SC_FSEEK
 *                   -   SC_FREAD
 *                   -   SC_FWRITE
 *                   -   SC_FPUTS
 *                   -   SC_FGETS
 *                   -   SC_FGETC
 *                   -   SC_FUNGETC
 *                   -   SC_FEXIT
 *                   -   SC_NUM_NODES
 */

int PC_process_access(char **argv, char *mode)
   {int rv = FALSE;

#if defined(HAVE_POSIX_SYS)

    int i, n, rev, msgs, *nim;

    _PC_setup_children(argv, mode);
            
    while (TRUE)
       {n = PC_poll(PC_procs.fd, PC_procs.n, -1);
	if (n > 0)
	   {do {msgs = FALSE;
		for (i = 0; i < PC_procs.n; i++)
		    {rev = PC_procs.fd[i].revents;
		     if ((rev & POLLIN) || (rev & POLLPRI))
		        {msgs |= _PC_get_message(i);};};

/* for anybody who may be doing a blocking read and there is a message
 * available let it through before another poll
 */
		for (i = 0; i < PC_procs.n; i++)
		    {nim = MESSAGE_LENGTHS(i);
		     if (nim != NULL)
		        _PC_get_msg(i);};}

	    while (msgs);};};

# ifdef OSF
    rv = TRUE;
# endif

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_PUSH_MESSAGE - register a message for a node */

int PC_push_message(SC_message *msg, int i, int ni, char *type, char *bf)
   {int n, nx;

    if (msg == NULL)
       return(FALSE);

    n   = msg->n;
    nx  = msg->nx;
    if (msg->msg == NULL)
       {nx = i + 8;
	msg->ni   = CMAKE_N(int, nx);
	msg->type = CMAKE_N(char *, nx);
	msg->msg  = CMAKE_N(char *, nx);};

    if (i >= nx)
       {nx = i + 8;
	CREMAKE(msg->ni,   int, nx);
	CREMAKE(msg->type, char *, nx);
	CREMAKE(msg->msg,  char *, nx);};

    msg->ni[n]   = ni;
    msg->type[n] = CSTRSAVE(type);
    msg->msg[n]  = bf;

    msg->n  = n + 1;
    msg->nx = nx;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_POP_MESSAGE - pop a message off the queue */

void PC_pop_message(int i)
   {int j, n;
    int *nis;
    char **msg, **typ;
    
    msg = MESSAGES(i);
    typ = MESSAGE_TYPES(i);
    nis = MESSAGE_LENGTHS(i);

    n = N_MESSAGES(i)--;
    CFREE(msg[0]);
    CFREE(typ[0]);
    for (j = 0; j < n; j++)
        {msg[j] = msg[j+1];
	 typ[j] = typ[j+1];
	 nis[j] = nis[j+1];};

    msg[n] = NULL;
    typ[n] = NULL;
    nis[n] = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_BUFFER_DATA_IN - read from the data connection of the PROCESS
 *                   - and save them in an internal buffer
 */

int PC_buffer_data_in(PROCESS *pp)
   {int fd, nbr, nbs;
    unsigned long nb, nx, nbt;
    char s[DATA_BLOCK], *bf;

    fd = pp->data;
    bf = pp->data_buffer;
    nb = pp->nb_data;
    nx = pp->nx_data;
    if (bf == NULL)
       {nx = DATA_BLOCK;
	nb = 0L;
	bf = CMAKE_N(char, nx);};

    PC_unblock_fd(fd);

    nbs = 0;
    while (TRUE)
       {nbr = SC_read_sigsafe(fd, s, DATA_BLOCK);
        if (nbr < 0)
	   break;

        nbt = nb + nbr;
        nbs += nbr;

        if (nbt > nx)
           {nx += DATA_BLOCK;
	    CREMAKE(bf, char, nx);};

	memcpy(bf+nb, s, nbr);
	nb = nbt;};

    pp->data_buffer = bf;
    pp->nb_data     = nb;
    pp->nx_data     = nx;

    return(nbs > -1 ? nbs : -errno);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_BUFFER_DATA_OUT - get data from the data connection of the given
 *                    - process including any buffered data
 *                    - and put it NBI bytes in BF
 */

int PC_buffer_data_out(PROCESS *pp, char *bf, int nbi, int block_state)
   {int nbe, nb0, fd;
    char *pbf, *dbf;

    pbf = bf;
    fd  = pp->data;
    dbf = pp->data_buffer;    
    nbe = pp->nb_data;
    nb0 = nbi;
    if (nbe < nbi)
       {memcpy(pbf, dbf, nbe);
        pbf += nbe;
	nbi -= nbe;
        memset(dbf, 0, nbe);
        pp->nb_data = 0;

        if (block_state)
	   PC_block_fd(fd);
        else
	   PC_unblock_fd(fd);
	    
	while (nbi > 0)
	   {nbe = SC_read_sigsafe(fd, pbf, nbi);
	    if (nbe < 0)
	       break;
	    pbf += nbe;
	    nbi -= nbe;};}

    else
       {memcpy(pbf, dbf, nbi);
        pp->nb_data = nbe - nbi;
        memcpy(dbf, dbf+nbi, pp->nb_data);};

    return(nb0 - nbi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

