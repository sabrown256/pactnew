/*
 * MISCIN.H - header for MISC package in ABC
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "global.h"
#include "misc.h"
#include "lag.h"
#include "mesh.h"

/*--------------------------------------------------------------------------*/

/*                              MISC CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                               MISC MACROS                                */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                               MISC STRUCTS                               */

/*--------------------------------------------------------------------------*/

struct s_component
   {int index;
    int z;
    double a;
    double frac;
    struct s_component *next;};

typedef struct s_component component;

struct s_material
   {int index;
    char *name;
    double rho;
    double n;
    double a;
    double z;
    double tm;
    double p;
    component *composition;};

typedef struct s_material material;

/*--------------------------------------------------------------------------*/

/*                              MISC VARIABLES                              */

/*--------------------------------------------------------------------------*/

extern double
 *ab,
 *tm,
 *zb;

extern int
 N_z;

/*--------------------------------------------------------------------------*/

/*                              MISC FUNCTIONS                              */

/*--------------------------------------------------------------------------*/



