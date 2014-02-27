/*
 * QUEV.C - standalone utility to query mouse events
 *        - for use in shell scripting
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
   {int i, ip, rv, mod, nc, np, st;
    int iev[10][PG_SPACEDM], dx[PG_SPACEDM];
    int *lev;
    char *sb[10], *sm[10];
    PG_event ev;
    PG_mouse_button btn;
    PG_device *dev;
    extern int PG_X_grab_mouse(PG_device *dev);

    np = 0;

/* process the command line arguments */
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'b' :
                      np = 2;
		      break;
                 case 'h' :
                      printf("\n");
                      printf("Usage: quev [-h] -b | -p\n");
                      printf("   b  select two points defining a box\n");
                      printf("   h  this help message\n");
                      printf("   p  select a single point\n");
                      printf("\n");
		      break;
                 case 'p' :
                      np = 1;
		      break;};};};

    SC_setbuf(STDOUT, NULL);

    dev = PG_make_device("WINDOW", "COLOR", "QUEV");
    PG_query_screen_n(dev, dx, &nc);

    st = PG_grab_pointer(dev);
    if (st == TRUE)
       {for (ip = 0; ip < np; ip++)
	    {lev = iev[ip];
	     PG_get_next_event(&ev);
	     PG_mouse_event_info(dev, &ev, lev, &btn, &mod);
	     if (btn != MOUSE_NONE)
	        {switch (btn)
		    {case MOUSE_LEFT :
		          sb[ip] = "left";
			  break;
		     case MOUSE_MIDDLE :
			  sb[ip] = "middle";
			  break;
		     case MOUSE_RIGHT :
			  sb[ip] = "right";
			  break;
		     case MOUSE_WHEEL_UP :
			  sb[ip] = "wheel up";
			  break;
		     case MOUSE_WHEEL_DOWN :
			  sb[ip] = "wheel down";
			  break;
		     default :
		          sb[ip] = "unknown";
			  break;};

		 switch (mod)
		    {case KEY_SHIFT :
		          sm[ip] = "shift";
			  break;
		     case KEY_LOCK :
			  sm[ip] = "lock";
			  break;
		     case KEY_CNTL :
			  sm[ip] = "ctl";
			  break;
		     case KEY_ALT :
			  sm[ip] = "alt";
			  break;
		     default :
			  sm[ip] = "";
			  break;};};};

	st = PG_release_pointer(dev);

	if (np == 2)
	   {int ul[PG_SPACEDM], lr[PG_SPACEDM];

	    ul[0] = min(iev[0][0], iev[1][0]);
	    lr[0] = max(iev[0][0], iev[1][0]);

	    ul[1] = min(iev[0][1], iev[1][1]);
	    lr[1] = max(iev[0][1], iev[1][1]);

	    printf("%d %d %d %d\n", ul[0], ul[1], lr[0], lr[1]);}

	else if (np == 1)
	   printf("%d %d\n", iev[0][0], iev[0][1]);

	else
	   {for (ip = 0; ip < np; ip++)
	        {lev = iev[ip];
		 printf("-> (%d,%d) %s %s\n",
			lev[0], lev[1], sm[ip], sb[ip]);};};}
    else
       printf("Failed to grab mouse - exiting\n");

    PG_close_device(dev);

    rv = (st != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
