/*
 * TGSSCM.C - display the device colormap
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define SHOW_MAP 1
#define SHOW_PAL 2
#define MAKE_PAL 3
#define DUMP_MAP 4

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print the help info */

int help(void)
   {

    printf("\nUsage: tgsscm [-c | -s] [-m <# colors> | -p | -t] [ -b | -w]\n");
    printf("   Show various aspects of the color handling in PGS.\n");
    printf("   Options:\n");
    printf("      b - use black background\n");
    printf("      c - put the color map or palettes out to a CGM file\n");
    printf("      h - print this help message\n");
    printf("      s - put the color map or palettes out to a PostScript file\n");
    printf("      w - use white background (default)\n");
    printf("\n");
    printf("      m - make a new palette\n");
    printf("      p - show the built in palettes\n");
    printf("      t - show the root PGS color table\n");
    printf("\n");

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {char device[MAXLINE];
    int i, nc, mode, wbck;

    wbck = TRUE;
    nc   = 16;
    mode = MAKE_PAL;
    SC_strncpy(device, MAXLINE, "WINDOW", -1);

    for (i = 1; v[i] != NULL; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
	        {case 'b' :
		      wbck = FALSE;
		      break;

                 case 'c' :
		      SC_strncpy(device, MAXLINE, "CGM", -1);
		      break;

                 case 'd' :
		      mode = DUMP_MAP;
		      break;

                 case 'h' :
		      help();
		      return(1);

                 case 'm' :
		      mode = MAKE_PAL;
		      nc = SC_stoi(v[++i]);
		      break;

                 case 'p' :
		      mode = SHOW_PAL;
		      break;

                 case 's' :
		      SC_strncpy(device, MAXLINE, "PS", -1);
		      break;

                 case 't' :
		      mode = SHOW_MAP;
		      break;

                 case 'w' :
		      wbck = TRUE;
		      break;};}
         else
            break;};

    switch (mode)
       {case SHOW_MAP :
	     PG_show_colormap(device, wbck);
	     break;

        case DUMP_MAP :
	     PG_dump_colormap(device, "device.colors");
	     break;

        case SHOW_PAL :
	     PG_show_palettes(NULL, device, wbck);
	     break;

        case MAKE_PAL :
	     PG_make_palette(NULL, NULL, nc, wbck);
	     break;};

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
