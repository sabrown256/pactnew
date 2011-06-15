/*
 * PDBMP.C - API functions for distributed parallel operation
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#ifdef HAVE_MPI

#include "scope_mpi.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MP_SET_SERIAL - set the file->mpi_file field to
 *                  - the input value
 *                  - returns the old value
 *
 * #bind PD_mp_set_serial fortran() scheme(pd-mp-set-serial!)
 */

int PD_mp_set_serial(PDBfile *file, int flag)
   {int ret;

   ret = file->mpi_mode;
   file->mpi_mode = (flag == TRUE);

   return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MP_SET_COLLECTIVE - set the I/O (write right now)
 *                      - operations to be collective
 *
 * #bind PD_mp_set_collective fortran() scheme(pd-mp-set-collective!)
 */

int PD_mp_set_collective(int flag)
   {int ret;

    ret = _PD_set_collective(flag);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MP_CREATE - create a PDB file in mp parallel mode 
 *              - and return an initialized PDBfile
 *              - if successful else NULL
 *              - if comm == SC_COMM_SELF the file is opened only on the
 *              - local processor
 *
 * #bind PD_mp_create fortran() scheme()
 */

PDBfile *PD_mp_create(char *name, SC_communicator *comm)
   {PDBfile *file;

    if (!_PD_IS_DP_INIT)
       PD_init_mpi(-1, 1, NULL);

    _PD_par_set_comm(*comm);

    file = _PD_open_bin(name, "w", comm);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MP_OPEN - open an existing PDB file in mp parallel mode, 
 *            - extract the symbol table and
 *            - structure chart, and return a pointer to the PDB file
 *            - if successful else NULL
 *            - if comm == SC_COMM_SELF the file is opened only on the
 *            - local processor
 *
 * #bind PD_mp_open fortran() scheme()
 */

PDBfile *PD_mp_open(char *name, char *mode, SC_communicator comm)
   {PDBfile *file;

    if (!_PD_IS_DP_INIT)
       PD_init_mpi(-1, 1, NULL);

    _PD_par_set_comm(comm);

    file = _PD_open_bin(name, mode, &comm);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

