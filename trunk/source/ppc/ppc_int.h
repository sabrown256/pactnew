/*
 * PPC_INT.H - internal header for Portable Process Control system
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PPC_INT

#include "cpyright.h"

#define PCK_PPC_INT

#include "pdb_int.h"
#include "ppc.h"

/*--------------------------------------------------------------------------*/

/*                             TYPEDEFS AND STRUCTS                         */

/*--------------------------------------------------------------------------*/

typedef void (*PFVAP)(void *);

typedef struct s_PC_state PC_state;

struct s_PC_state
   {

/* initializes to non-zero values */

/* initializes to 0 bytes */

/* PCPARC.C */
    SC_array *reqs;
    int server_port;
    int n_nodes;
    PROCESS *server_link;
    char server[MAXLINE];};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern PC_state
 _PC;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/


/* PCBIN.C declarations */

extern int
 _PC_bin_read(void *ptr, char *type, size_t ni, PROCESS *pp),
 _PC_bin_write(void *ptr, char *type, size_t ni, PROCESS *pp);


/* PCPARC.C declarations */

extern void
 _PC_extract_filter_info(int *filt, int *pti, int *pit,
			 int *phn, int *pdn, int *pdi,
			 int *pbs, int *pbz, int *pnn,
			 int **pnl, int *pnp, int **ppl);


#ifdef __cplusplus
}
#endif

#endif


