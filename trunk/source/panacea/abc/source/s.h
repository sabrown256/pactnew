/*
 * S.H - header file for S
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_S

#define PCK_S

#include "cpyright.h"

#include <panace.h>
#include <global.h>
#include <lag.h>
#include <misc.h>

#undef A_LONG
#undef A_CHAR
#undef A_PART

enum e_A_type
   {A_INTEGER  = 1,
    A_LONG,
    A_FLOAT,
    A_DOUBLE,
    A_CHAR,
    A_PART};

typedef enum e_A_type A_type;

/*--------------------------------------------------------------------------*/

/*                           STRUCT DEFINITIONS                             */

/*--------------------------------------------------------------------------*/

struct s_time_list
   {int length;
    pcons *list;};

typedef struct s_time_list time_list;

struct s_source_record
   {char *name;
    double time;
    int *indexes;
    double *data;};

typedef struct s_source_record source_record;

/*--------------------------------------------------------------------------*/

/*                             PROCEDURAL MACROS                            */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*                           VARIABLE DECLARATIONS                          */

/*--------------------------------------------------------------------------*/

extern PDBfile
 *pdsf;

extern FILE
 *strm;

extern hasharr
 *commands,
 *srctab;

extern double
 conversion_s;

extern char
 in_deck[],                                       /* global input deck name */
 msg[];                                  /* global temporary string storage */

/*--------------------------------------------------------------------------*/

/*                           FUNCTION DECLARATIONS                          */

/*--------------------------------------------------------------------------*/

/* SCMD.C declarations */

extern void
 inst_s(void),
 readh(char *str),
 S_gen(char *s);

#endif



