/*
 * PDCOMM.C - functions to manage MPI communicators
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "scope_mpi.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INITIALIZE_COMM - set the _PD.comm_initialized flag */

int _PD_initialize_comm(int v)
   {int rv;

    rv = _PD.comm_initialized;
    _PD.comm_initialized = v;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMM_CONNECT_NONMPI - connect up the stub communicators */

static void _PD_comm_connect_nonmpi(void)
   {

    _PD.use_comm[COM_NULL]  = _PD.def_comm[COM_NULL];
    _PD.use_comm[COM_WORLD] = _PD.def_comm[COM_WORLD];
    _PD.use_comm[COM_SELF]  = _PD.def_comm[COM_SELF];;

    _PD_initialize_comm(TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PAR_SET_COMM - set the communicator to use for open/create for
 *                  - this thread
 */

void _PD_par_set_comm(SC_communicator comm)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);
    pa->communicator = comm;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_COMM_NULL - return TRUE iff COMM is _PD.use_comm[COM_NULL] */

int _PD_is_comm_null(SC_communicator comm)
   {int rv;

    if (_PD.comm_initialized == FALSE)
       _PD_comm_connect_nonmpi();

    rv = (comm == _PD.use_comm[COM_NULL]);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_COMM_SELF - return TRUE iff COMM is _PD.use_comm[COM_SELF] */

int _PD_is_comm_self(SC_communicator comm)
   {int rv;

    if (_PD.comm_initialized == FALSE)
       _PD_comm_connect_nonmpi();

    rv = (comm == _PD.use_comm[COM_SELF]);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_NULL_COMM - set the communicator to _PD.use_comm[COM_NULL] */

void _PD_set_null_comm(int tid)
   {PD_smp_state *pa;

    if (_PD.comm_initialized == FALSE)
       _PD_comm_connect_nonmpi();

    pa = _PD_get_state(tid);
    pa->communicator = _PD.use_comm[COM_NULL];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_WORLD_COMM - set the communicator to _PD.use_comm[COM_WORLD] */

void _PD_set_world_comm(int tid)
   {PD_smp_state *pa;

    if (_PD.comm_initialized == FALSE)
       _PD_comm_connect_nonmpi();

    pa = _PD_get_state(tid);
    pa->communicator = _PD.use_comm[COM_WORLD];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


