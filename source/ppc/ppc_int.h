/*
 * PPC_INT.H - internal header for PDBNet system
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

typedef struct s_PN_scope_private PN_scope_private;

struct s_PN_scope_private
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

extern PN_scope_private
 _PN;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/


/* PNBIN.C declarations */

extern int
 _PN_bin_read(void *ptr, char *type, size_t ni, PROCESS *pp),
 _PN_bin_write(void *ptr, char *type, size_t ni, PROCESS *pp);


/* PNPARC.C declarations */

extern void
 _PN_extract_filter_info(int *filt, int *pti, int *pit,
			 int *phn, int *pdn, int *pdi,
			 int *pbs, int *pbz, int *pnn,
			 int **pnl, int *pnp, int **ppl);


#ifdef __cplusplus
}
#endif

#endif


