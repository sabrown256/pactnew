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

/* PD_MP_SET_SERIAL - Set the mpi_file field of PDBfile FILE to
 *                  - the value FLAG.
 *                  - Return the old value.
 *
 * #bind PD_mp_set_serial fortran() scheme(pd-mp-set-serial!) python()
 */

int PD_mp_set_serial(PDBfile *file ARG(,,cls), int flag)
   {int ret;

   ret = file->mpi_mode;
   file->mpi_mode = (flag == TRUE);

   return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MP_SET_COLLECTIVE - Set the value of the global I/O
 *                      - collective operations flag.
 *                      - If FLAG is TRUE then collective I/O operations
 *                      - are done and if FALSE then non-collective I/O
 *                      - operations are done.
 *                      - Return the old value.
 *
 * #bind PD_mp_set_collective fortran() scheme(pd-mp-set-collective!) python()
 */

int PD_mp_set_collective(int flag)
   {int ret;

    ret = _PD.mp_collective;
    _PD_set_collective(flag);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MP_CREATE - Create a PDBfile NAME in distributed parallel mode.
 *              - If COMM equals SC_COMM_SELF the file is opened only
 *              - on the local processor.
 *              - Return the PDBfile if successful otherwise return NULL.
 *
 * #bind PD_mp_create fortran() scheme() python()
 */

PDBfile *PD_mp_create(const char *name, SC_communicator *comm)
   {PDBfile *file;

    if (!_PD_IS_DP_INIT)
       PD_init_mpi(-1, 1, NULL);

    _PD_par_set_comm(*comm);

    file = _PD_open_bin(name, "w", comm);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MP_OPEN - Open the existing PDB file NAME in distributed parallel mode.
 *            - As with PD_open MODE is the read/write/append mode
 *            - specification.
 *            - If COMM equals SC_COMM_SELF the file is opened only on the
 *            - local processor.
 *            - Return a pointer to the PDBfile if successful otherwise
 *            - return NULL.
 *
 * #bind PD_mp_open fortran() scheme() python()
 */

PDBfile *PD_mp_open(const char *name, const char *mode,
		    SC_communicator comm)
   {PDBfile *file;

    if (!_PD_IS_DP_INIT)
       PD_init_mpi(-1, 1, NULL);

    _PD_par_set_comm(comm);

    file = _PD_open_bin(name, mode, &comm);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

