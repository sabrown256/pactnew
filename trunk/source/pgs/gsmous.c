/*
 * GSMOUS.C - mouse handling routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#ifndef NO_WINDOWS

/*--------------------------------------------------------------------------*/

/* PG_GET_MOUSE_EVENT - wait for a mouse button to be pressed
 *                    - return the position at which it was pressed
 *                    - through the pointers in the arg list
 *                    - the position is in world coordinates
 *                    - and return the button pressed
 */

PG_get_mouse_event(PG_device *dev, double *px, double *py)
   {double x[PG_SPACEDM], ix[PG_SPACEDM];
    PG_mouse_location *pm;

    while (TRUE)
       {pm = PG_get_mouse_released(MOUSE_LEFT);
        if (pm->count > 0)
           {ix[0] = pm->x;
            ix[1] = pm->y;
	    PG_trans_point(dev, 2, PIXELC, ix, WORLDC, x);
            *px = x[0];
            *py = x[1];
            return(MOUSE_LEFT);};

        pm = PG_get_mouse_released(MOUSE_RIGHT);
        if (pm->count > 0)
           {ix[0] = pm->x;
            ix[1] = pm->y;
	    PG_trans_point(dev, 2, PIXELC, ix, WORLDC, x);
            *px = x[0];
            *py = x[1];
            return(MOUSE_RIGHT);};};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
