/*
 * SCOPE_MPI.H - define MPI scope
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_MPI

#define PCK_SCOPE_MPI

# include "cpyright.h"
# include "score.h"

#ifdef HAVE_MPI

# include <mpi.h>

/* # define SC_communicator MPI_Comm */
# define SC_COMM_NULL           ((SC_communicator) MPI_COMM_NULL)
# define SC_COMM_WORLD          ((SC_communicator) MPI_COMM_WORLD)
# define SC_COMM_SELF           ((SC_communicator) MPI_COMM_SELF)
# define SC_COMM_F2C(_x)        ((SC_communicator) MPI_Comm_f2c(_x))

#else

# define SC_COMM_NULL           ((SC_communicator) 0)
# define SC_COMM_WORLD          ((SC_communicator) 0xdeaf)
# define SC_COMM_SELF           ((SC_communicator) 0xdeaf)
# define MPI_Request            void *
# define SC_COMM_F2C(_x)

#endif

#endif

