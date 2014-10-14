/*
 * PNET.C - PDBNet routines
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "scope_proc.h"
#include "scope_dp.h"

PN_scope_public
  PN_gs;

PN_scope_private
 _PN;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_OPEN_PROCESS - open up a process that may be used
 *                 - for binary mode data
 */

PROCESS *PN_open_process(char **argv, char **envp, char *mode)
   {char md[MAXLINE];
    PROCESS *pp;

    snprintf(md, MAXLINE, "%sv", mode);
    pp = SC_open(argv, envp, md, "INIT", PN_mk_process, LAST);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_MK_PROCESS - initialize and return a PROCESS */

PROCESS *PN_mk_process(char **argv, char *mode, int type)
   {PROCESS *pp;
    PDBfile *pf;

    pp = SC_mk_process(argv, mode, type, -1);

    if (SC_BINARY_MODEP(mode))
       {pf = PD_open_vif(argv[0]);
	if (pf != NULL)
	   {pf->virtual_internal = FALSE;
	    pf->stream           = (FILE *) pp;};

        pp->vif = pf;};

/* setup PDB based binary data handling */
    pp->read         = _PN_bin_read;
    pp->write        = _PN_bin_write;
    pp->recv_formats = PN_recv_formats;
    pp->send_formats = PN_send_formats;
    pp->rl_vif       = (PFVAP) _PD_rl_pdb;

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
