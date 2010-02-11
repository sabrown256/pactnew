/*
 * TRNACF.H - the NACF support header for TRANSL
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_NACF

#include "cpyright.h"
 
#include "../../pdb_int.h"

#define PCK_NACF

/*--------------------------------------------------------------------------*/

/*                              DEFINED CONSTANTS                           */

/*--------------------------------------------------------------------------*/

#define BMAXNAME   128      /* Max number of chars in a name */
#define BMAXCHILD  1024     /* Max number of children in a directory */

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

enum e_NACF_types
   {NAC_FWFP,
    NAC_HWFP1,
    NAC_HWFP2,
    NAC_DWFP,      /* not to be supported ever */
    NAC_CFP,       /* maybe someday */
    NAC_FWSI,
    NAC_HWUI,
    NAC_QWUI,
    NAC_FWUI,
    NAC_BIT,
    NAC_BYTE,
    NAC_CHARS,
    NAC_ASCII,
    NAC_LJBF,
    NAC_REC};

typedef enum e_NACF_types NACF_types;

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern char
 *TR_NACFILE_S;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* TRNACA.C declarations */

#endif

