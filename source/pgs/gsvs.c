/*
 * GSVS.C - view surface routines for SUN
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include <sunwindow/window_hs.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sun/fbio.h>
#include <stdio.h>
#include <usercore.h>

int
 cgpixwindd(),
 gp1pixwindd(),
 pixwindd(),
 bw1dd(),
 bw2dd(),
 cg1dd(),
 cg2dd(),
 gp1dd();

char *getenv();

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_VIEW_SURFACE - try SUN's way of finding a view surface */

get_view_surface(vsptr)
   struct vwsurf *vsptr;
   {int fd, devhaswindows;
    char *wptr, dev[DEVNAMESIZE];
    struct screen screen;
    struct fbtype fbtype;

    if (wptr = getenv("WINDOW_ME"))
       {devhaswindows = TRUE;
        if ((fd = open(wptr, O_RDWR, 0)) < 0)
           {io_printf(stderr, "GET_VIEW_SURFACE: CAN'T OPEN %s\n", wptr);
            return(FALSE);};
        win_screenget(fd, &screen);
        close(fd);
        strncpy(dev, screen.scr_fbname, DEVNAMESIZE);}
    else
       {devhaswindows = FALSE;
        strncpy(dev, "/dev/fb", DEVNAMESIZE);};

    if ((fd = open(dev, O_RDWR, 0)) < 0)
       {io_printf(stderr, "GET_VIEW_SURFACE: CAN'T OPEN %s\n", dev);
        return(FALSE);};

    if (ioctl(fd, FBIOGTYPE, &fbtype) == -1)
       {io_printf(stderr, "GET_VIEW_SURFACE: IOCTL FBIOGTYPE FAILED ON %s\n",
		  dev);
        close(fd);
        return(FALSE);};

    close(fd);

    if (devhaswindows)
       {switch (fbtype.fb_type)
           {case FBTYPE_SUN1BW    :
            case FBTYPE_SUN2BW    : vsptr->dd = pixwindd;
                                    break;
            case FBTYPE_SUN1COLOR :
            case FBTYPE_SUN2COLOR : vsptr->dd = cgpixwindd;
                                    break;
            case FBTYPE_SUN2GP    : vsptr->dd = gp1pixwindd;
                                    break;
            default               : io_printf(stderr,
					      "GET_VIEW_SURFACE: UNKNOWN %s\n",
					      dev);
                                    return(FALSE);};}
    else
       {switch (fbtype.fb_type)
           {case FBTYPE_SUN1BW    : vsptr->dd = bw1dd;
                                    break;
            case FBTYPE_SUN2BW    : vsptr->dd = bw2dd;
                                    break;
            case FBTYPE_SUN1COLOR : vsptr->dd = cg1dd;
                                    break;
            case FBTYPE_SUN2COLOR : vsptr->dd = cg2dd;
                                    break;
            case FBTYPE_SUN2GP    : vsptr->dd = gp1dd;
                                    break;
            default               : io_printf(stderr,
					      "GET_VIEW_SURFACE: UNKNOWN %s\n",
					      dev);
                                    return(FALSE);};};

    strncpy(vsptr->screenname, dev, DEVNAMESIZE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
