/*
 * PGS.JS - PGS drawing routines for HTML5 canvas and JavaScript
 *
 * #include "cpyright.h"
 *
 */

/*
enum e_PG_coord_sys
   {WORLDC = 1, NORMC, PIXELC, BOUNDC, FRAMEC};
typedef enum e_PG_coord_sys PG_coord_sys;
*/
var WORLDC = 1;
var NORMC  = 2;
var PIXELC = 3;
var BOUNDC = 4;
var FRAMEC = 5;

var LINE_SOLID     = 1;
var LINE_DASHED    = 2;
var LINE_DOTTED    = 3;
var LINE_DOTDASHED = 4;

var BLACK        = '#000000';
var WHITE        = '#FFFFFF';
var GRAY         = '#AAAAAA';
var DARK_GRAY    = '#444444';
var BLUE         = '#0000FF';
var GREEN        = '#00FF00';
var CYAN         = '#00FFFF';
var RED          = '#FF0000';
var MAGENTA      = '#FF00FF';
var BROWN        = '#AAAA00';
var DARK_BLUE    = '#0000AA';
var DARK_GREEN   = '#00AA00';
var DARK_CYAN    = '#00AAAA';
var DARK_RED     = '#AA0000';
var YELLOW       = '#FFFF00';
var DARK_MAGENTA = '#AA00AA';

var TRUE  = 1;
var FALSE = 0;

var gs_wc  = new Array(0.0, 0.0, 1.0, 1.0);
var gs_ndc = new Array(0.0, 0.0, 1.0, 1.0);
var gs_pc  = new Array(0, 0, 1000, 1000);

var gs_gr_x = new Array(0.0, 0.0, 0.0);
var gs_tx_x = new Array(0.0, 0.0, 0.0);

var gs_text_color = BLACK;
var gs_fill_color = BLACK;
var gs_line_color = BLACK;
var gs_line_style = LINE_SOLID;
var gs_line_width = 0.0;

var gs_font_name  = "Helvetica";
var gs_font_style = "medium";
var gs_font_size  = 12;

// var person={firstname:"John", lastname:"Doe", id:5566};
// name=person.lastname;
// name=person["lastname"]; 
// var carname=new String;
// var x=      new Number;
// var y=      new Boolean;
// var cars=   new Array;
// var person= new Object;

/*--------------------------------------------------------------------------*/

/*                            DEVICE ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* PG_OPEN - open the JS device */

function PG_open(name, type)
   {var screen = document.getElementById(name);
    var dev    = screen.getContext(type);
    return dev;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_SCREEN - query screen of DEV */

function PG_query_screen(dev, pdx, pdy, pnc)
   {}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_OPEN_SCREEN - open_screen the JS device */

function PG_open_screen(dev, xf, yf, dxf, dyf)
   {return dev;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_DEVICE_CURRENT - noop */

function PG_make_device_current(dev)
   {}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RELEASE_CURRENT_DEVICE - noop */

function PG_release_current_device(dev)
   {}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_UPDATE_VS - noop */

function PG_update_vs(dev)
   {}

/*--------------------------------------------------------------------------*/

/*                        LINE DRAWING ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_COLOR - return the line color */

function PG_fget_line_color(dev)
   {return gs_line_color;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_COLOR - set the line color to CLR */

function PG_fset_line_color(dev, clr, mapped)
   {gs_line_color = clr;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_WIDTH - return the line width */

function PG_fget_line_width(dev)
   {return gs_line_width;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_WIDTH - set the line width to W */

function PG_fset_line_width(dev, w)
   {gs_line_width = w;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_STYLE - return the line style */

function PG_fget_line_style(dev)
   {var sty = 0;
    return sty;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_STYLE - set the line style to STY */

function PG_fset_line_style(dev, sty)
   {}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_GR_ABS_N - move the graphics point to P */

function PG_move_gr_abs_n(dev, p)
   {gs_gr_x = p;
    dev.moveTo(p[0], p[1]);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_ABS_N - draw from the graphics point to P */

function PG_draw_to_abs_n(dev, p)
   {gs_gr_x = p;
    dev.lineWidth   = gs_line_width;
    dev.strokeStyle = gs_line_color;
    dev.lineTo(p[0], p[1]);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_REL_N - draw from the graphics point to P */

function PG_draw_to_rel_n(dev, p)
   {gs_gr_x = gs_gr_x + p;
    dev.lineWidth   = gs_line_width;
    dev.strokeStyle = gs_line_color;
    dev.lineTo(gs_gr_x[0], gs_gr_x[1]);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_LINE_N - draw a line on DEV from X1 to X2 */

function PG_draw_line_n(dev, nd, cs, x1, x2, clip)
   {dev.beginPath();
    PG_move_gr_abs_n(dev, x1);
    PG_draw_to_abs_n(dev, x2);
    dev.stroke();}

/*--------------------------------------------------------------------------*/

/*                        TEXT DRAWING ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PG_FGET_TEXT_COLOR - return the text color */

function PG_fget_text_color(dev)
   {return gs_text_color;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_TEXT_COLOR - set the text color to CLR */

function PG_fset_text_color(dev, clr, mapped)
   {gs_text_color = clr;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_FONT - set the text FACE, STYLE, and SIZE */

function PG_set_font(dev, face, style, size)
   {var sz = size.toString();
    dev.font=face.concat(sz, "px", face);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_ABS_N - move the text point to P */

function PG_move_tx_abs_n(dev, p)
   {gs_tx_x = p;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_REL_N - move the text point to P */

function PG_move_tx_rel_n(dev, p)
   {gs_tx_x = gs_tx_x + p;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TEXT - draw test S on DEV at X */

function PG_draw_text(dev, s, x)
   {dev.fillStyle   = gs_text_color;
    dev.strokeStyle = gs_text_color;
    dev.fillText(s, x[0], x[1]);}

/*--------------------------------------------------------------------------*/

/*                              FILL ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* PG_FGET_FILL_COLOR - return the fill color */

function PG_fget_fill_color(dev)
   {return gs_fill_color;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FILL_COLOR - set the fill color to CLR */

function PG_fset_fill_color(dev, clr, mapped)
   {gs_text_color = clr;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SHADE_POLY_N - fill the ND polygon defined by the N points R */

function PG_shade_poly_n(dev, nd, n, r)
   {}

/*--------------------------------------------------------------------------*/

/*                             IMAGE ROUTINES                               */

/*--------------------------------------------------------------------------*/

function PG_put_image_n(dev, bf, cs, irg)
   {}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

