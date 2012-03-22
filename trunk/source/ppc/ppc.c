/*
 * PPC.C - Portable Process Control system
 *       - mostly pushed down into SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc_int.h"

PC_global_state
  PC_gs;

PC_state
 _PC;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_OPEN - open up a process that may be used for binary mode data */

PROCESS *PC_open(char **argv, char **envp, char *mode)
   {char md[MAXLINE];
    PROCESS *pp;

    snprintf(md, MAXLINE, "%sv", mode);
    pp = SC_open(argv, envp, md, "INIT", PC_mk_process, LAST);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_IO_CONNECT - connect the PDB io hooks for remote access */

int PC_io_connect(int flag)
   {int ret;

    ret = SC_io_connect(flag);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PC_MK_PROCESS - initialize and return a PROCESS */

PROCESS *PC_mk_process(char **argv, char *mode, int type)
   {PROCESS *pp;
    PDBfile *pf;

    pp = SC_mk_process(argv, mode, type, -1);

    if (PC_BINARY_MODEP(mode))
       {pf = PD_open_vif(argv[0]);
	if (pf != NULL)
	   {pf->virtual_internal = FALSE;
	    pf->stream           = (FILE *) pp;};

        pp->vif = pf;};

/* setup PDB based binary data handling */
    pp->read         = _PC_bin_read;
    pp->write        = _PC_bin_write;
    pp->recv_formats = PC_recv_formats;
    pp->send_formats = PC_send_formats;
    pp->rl_vif       = (PFVAP) _PD_rl_pdb;

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
