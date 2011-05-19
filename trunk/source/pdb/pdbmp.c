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
 */

PDBfile *PD_mp_open(char *name, char *mode, SC_communicator comm)
   {PDBfile *file;

    if (!_PD_IS_DP_INIT)
       PD_init_mpi(-1, 1, NULL);

    _PD_par_set_comm(comm);

    file = _PD_open_bin(name, mode, &comm);

    return(file);}

/*--------------------------------------------------------------------------*/

/*                   FORTRAN INTERFACES FOR DPI ROUTINES                    */

/*--------------------------------------------------------------------------*/

/* PFINMP - FORTRAN interface routine to initialize pdblib for dpi
 * 
 */

FIXNUM F77_FUNC(pfinmp, PFINMP)(FIXNUM *masterproc, FIXNUM *pnthreads, 
				PFTid tid)
   {FIXNUM rv;

    rv = PD_init_mpi(*masterproc, *pnthreads, tid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFTMMP - FORTRAN interface routine to terminate pdblib mpi */

FIXNUM F77_FUNC(pftmmp, PFTMMP)(void)
   {

    PD_term_mpi();

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFMPOP - open a PDBfile for dpi
 *        - save the PDBfile pointer in an internal array
 *        - and return an integer index to the pointer if successful
 *        - return 0 otherwise
 */

FIXNUM F77_FUNC(pfmpop, PFMPOP)(FIXNUM *pnchr, char *name,
				char *mode, FIXNUM *pcomm)
   {FIXNUM rv;
    char s[MAXLINE], t[2];
    PDBfile *file;
    SC_communicator comm;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, mode, 1);
    comm = (SC_communicator) MPI_Comm_f2c(*pcomm);

    rv   = 0;
    file = PD_mp_open(s, t, comm);
    if (file != NULL)
       {file->major_order    = COLUMN_MAJOR_ORDER;
        file->default_offset = 1;

        rv = SC_ADD_POINTER(file);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFMPSS - FORTRAN interface routine to set file->mpi_file */

FIXNUM F77_FUNC(pfmpss, PFMPSS)(FIXNUM *fileid, FIXNUM *v)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);
    rv   = PD_mp_set_serial(file, *v);
  
    return(rv);}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

