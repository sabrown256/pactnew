/*
 * PCBIN.C - binary IO for PPC
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc_int.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_RECV_FORMATS - receive binary formats from the given PROCESS */

int PC_recv_formats(PROCESS *pp)
   {int ok;
    long nc, nr;
    char s[MAX_BFSZ+2];
    char *ps;
    PDBfile *file;

    if (pp != NULL)
       {ok = SC_ERR_TRAP();
	if (ok != 0)
	   return(FALSE);

	file = pp->vif;

	PC_block(pp);
	PC_set_attr(pp, SC_LINE, TRUE);

	if (PC_status(pp) == SC_RUNNING)
	   {for (nc = 0; nc < MAX_BFSZ; nc = strlen(s))
	        {nr = MAX_BFSZ - nc;
		 ps = s + nc;
		 PC_gets(ps, nr, pp);
		 if (strcmp(ps, "END FORMATS\n") == 0)
		    break;};}
	else
	   SC_error(-1, "PROCESS NOT RUNNING %d %d - PC_RECV_FORMATS",
		    pp->status, pp->reason);
	 
	(*file->rd_prim_types)(file, s);

	PC_unblock(pp);

	SC_ERR_UNTRAP();};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_SEND_FORMATS - send binary formats to the parent PROCESS connected
 *                 - to standard in and out
 */

int PC_send_formats(void)
   {int rv;
    char *_PD_tbuffer;
    PDBfile *pf;
    PROCESS *tty;
    FILE *fp;

    fp = stdout;

    fflush(fp);
    SC_setbuf(fp, NULL);

    tty = SC_get_terminal_process();
    if (tty == NULL)
       return(FALSE);

    pf = (PDBfile *) tty->vif;
    if (pf == NULL)
       rv = FALSE;

    else
       {(*pf->wr_prim_types)(fp, pf->chart);
	_PD_tbuffer = _PD_get_tbuffer();
	io_write(_PD_tbuffer, 1, strlen(_PD_tbuffer), fp);
	io_flush(fp);
	CFREE(_PD_tbuffer);

	PRINT(fp, "END FORMATS\n");

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_BIN_READ - do binary read from a PROCESS */

int _PC_bin_read(void *ptr, char *type, size_t nitems, PROCESS *pp)
   {int ni;
    syment *ep;
    PDBfile *file;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    file = pp->vif;

    switch (SETJMP(pa->read_err))
       {case ABORT    : return(FALSE);
        case ERR_FREE : return(TRUE);
        default       : memset(PD_err, 0, MAXLINE);
                        break;};

    ep = _PD_mk_syment(type, nitems, 0L, NULL, NULL);

    ni = _PD_sys_read(file, ep, type, ptr);

    _PD_rl_syment(ep);

    return((int) ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PC_BIN_WRITE - do binary write to a PROCESS */

int _PC_bin_write(void *ptr, char *type, size_t nitems, PROCESS *pp)
   {int ni;
    PDBfile *file;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    file = pp->vif;

    switch (SETJMP(pa->write_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(PD_err, 0, MAXLINE);
	     break;};

    ni = _PD_sys_write(file, ptr, nitems, type, type);

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
