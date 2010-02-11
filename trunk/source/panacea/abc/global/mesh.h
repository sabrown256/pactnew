/*
 * MESH.H - header supporting mesh generation for the LR global
 *        - package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#ifndef PCK_MESH

#define PCK_MESH

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* VECSET4 - set up the pointers for the four corner vector set */

#define vecset4(v,v1,v2,v3,v4)                                               \
   v2 = v;                                                                   \
   v3 = v2 - 1;                                                              \
   v4 = v3 - kbnd;                                                           \
   v1 = v4 + 1

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NODE_OF - compute the node associated with (k, l) */

#define NODE_OF(k, l) (((l) - 1)*kbnd + (k) - 1)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* K_OF_NODE - compute the k line on which the node lies */

#define K_OF_NODE(i) (((i) % kbnd) + 1)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* L_OF_NODE - compute the l line on which the node lies */

#define L_OF_NODE(i) (((i) / kbnd) + 1)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VOLR - compute a zone volume in rectangular coordinates */

#define VOLR(x1, x2, x3, x4, y1, y2, y3, y4)                                 \
    0.5*((x2-x4)*(y3-y1) - (x3-x1)*(y2-y4))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VOLQR - volume of rectangular quadrant in cartesian coordinates
 *       - also the area
 */

#define VOLQR(x1, x2, x3, x4, y1, y2, y3, y4)                                \
    ((x2-x4)*(y2+y3+y4-3.0*y1) - (y2-y4)*(x2+x3+x4-3.0*x1))/16.0

/*--------------------------------------------------------------------------*/

#endif

