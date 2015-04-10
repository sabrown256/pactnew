/*
 * SCOPE_X11.H - support for X11 in Portable Graphics System
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_X11

#include "cpyright.h"

#define PCK_SCOPE_X11

#ifdef HAVE_X11

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

/*--------------------------------------------------------------------------*/

/*                            DEFINED CONSTANTS                             */

/*--------------------------------------------------------------------------*/

#define HAVE_WINDOW_DEVICE
#define EVENT_DEFINED

#define tie_fighter_width 16
#define tie_fighter_height 16
 
#define star_width 16
#define star_height 16
#define star_x_hot 7
#define star_y_hot 7
 
#define KEY_DOWN_EVENT     KeyPress
#define KEY_UP_EVENT       KeyRelease
#define MOUSE_DOWN_EVENT   ButtonPress
#define MOUSE_UP_EVENT     ButtonRelease
#define UPDATE_EVENT       ConfigureNotify
#define EXPOSE_EVENT       Expose
#define MOTION_EVENT       MotionNotify

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

#define PG_X11_DRAWABLE(d)     ((d->use_pixmap) ? (d)->pixmap : (d)->window)

#define PG_EVENT_TYPE(ev)      (ev).type

/*--------------------------------------------------------------------------*/
 
/*                          TYPEDEFS AND STRUCTS                            */
 
/*--------------------------------------------------------------------------*/

typedef XEvent PG_event;

#endif

#endif
