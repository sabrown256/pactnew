/*
 * SCOPE_QUICKDRAW.H - MAC quickdraw support
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#if defined(MACOSX)
#ifndef SCOPE_QUICKDRAW_GRAPHICS
#define SCOPE_QUICKDRAW_GRAPHICS

# define HAVE_WINDOW_DEVICE
# define MAC_COLOR
# define PACT_INCLUDES

# undef user2

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

#include "pgs.h"

# define appleID            128                    /* This is a resource ID */
# define fileID             129                    /* This is a resource ID */
# define editID             130                    /* This is a resource ID */
# define fontID               5                    /* This is a resource ID */
# define sizeID             131                    /* This is a resource ID */

# define appleMenu        0                      /* MyMenus[] array indexes */
# define fileMenu         1
# define editMenu         2
# define fontMenu         3
# define sizeMenu         4
# define menuCount        5

# define DEFAULT_FONT_SIZE  12
# define MIN_FONT_SIZE       8
# define MENU_HEIGHT        19

/* items in appleMenu */
# define aboutMeCommand   1

/* items in fileMenu */
# define quitCommand      1

/* items in editMenu */
# define undoCommand      1
# define cutCommand       3
# define copyCommand      4
# define pasteCommand     5
# define clearCommand     6

/* items in sizeMenu */
# define Point6         1
# define Point8         2
# define Point10        3
# define Point12        4
# define Point16        5
# define Point20        6
# define Point24        7

/* For the one and only text window */
# define windowID       128

/* For the About Sample... DLOG */
# define aboutMeDLOG        128
# define okButton           1
# define authorItem         2                               /* For SetDialogItemText */
# define languageItem       3                               /* For SetDialogItemText */
# define versionItem        4                               /* For SetDialogItemText */

# undef KEY_DOWN_EVENT
# undef KEY_UP_EVENT
# undef MOUSE_DOWN_EVENT
# undef MOUSE_UP_EVENT
# undef UPDATE_EVENT
# undef EXPOSE_EVENT
# undef MOTION_EVENT

# define KEY_DOWN_EVENT     keyDown
# define KEY_UP_EVENT       keyUp
# define MOUSE_DOWN_EVENT   mouseDown
# define MOUSE_UP_EVENT     mouseUp
# define UPDATE_EVENT       updateEvt
# define EXPOSE_EVENT       activateEvt
# define MOTION_EVENT       100

# define MAXPIX 65535

# define PG_EVENT_TYPE(ev) (ev).type

/* SETRECT - Inline SetRect() macro, efficient when (rectp) is a constant
 *         - Must not be used if (rectp) has side effects
 *         - We could do an InsetRect() macro in a similar vein
 */
 
#define SETRECT(rectp, _left, _top, _right, _bottom)                         \
    (rectp)->left = (_left);                                                 \
    (rectp)->top = (_top);                                                   \
    (rectp)->right = (_right);                                               \
    (rectp)->bottom = (_bottom)


/* HIWORD - get the high 16 bits of a 32 bit long */

#define HIWORD(aLong) (((aLong) >> 16) & 0xFFFF)


/* LOWORD - get the low 16 bits of a 32 bit long */

#define LOWORD(aLong) ((aLong) & 0xFFFF)


#ifndef HAVE_X11
typedef RGBColor RGB_color_map;
#endif

#endif
#endif
