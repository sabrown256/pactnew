/*
 * TRHDF5.H - the HDF5 support header for PDBLib
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_HDF5

#define PCK_HDF5
 
#ifdef HAVE_HDF5

/*--------------------------------------------------------------------------*/

/*                              DEFINED CONSTANTS                           */

/*--------------------------------------------------------------------------*/

#define BUFFER_SIZE     1024

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

struct s_compound_member_info
   {char *member_name;
    long member_offset; 
    struct s_compound_member_info *next; };

typedef struct s_compound_member_info compound_member_info;

struct s_compound_desc 
   {char *compound_name;
    int num_members;
    compound_member_info *info;
    struct s_compound_desc *next; };

typedef struct s_compound_desc compound_desc;

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* TRHDF5.C declarations */

#endif

#endif

