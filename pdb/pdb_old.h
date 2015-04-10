/*
 * PDB_OLD.H - include file for deprecated PDB functionality
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PDB_OLD

#define PCK_PDB_OLD

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                                  MACROS                                  */

/*--------------------------------------------------------------------------*/

/* refactored 03/29/2012 */

#define INT_STANDARD       PD_gs.int_standard
#define REQ_STANDARD       PD_gs.req_standard
#define PD_std_standards   PD_gs.std_standards

#define INT_ALIGNMENT      PD_gs.int_alignment
#define REQ_ALIGNMENT      PD_gs.req_alignment
#define PD_std_alignments  PD_gs.std_alignments

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                           VARIABLE DECLARATIONS                          */

/*--------------------------------------------------------------------------*/

extern char
 PD_err[];

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif

