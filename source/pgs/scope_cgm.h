/*
 * GSCGM.H - definitions for CGM device in PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/*
 * COMMAND LAYOUT - category, id,
 *                  effective # parameters,
 *                  <parameter descriptor> ...
 *
 * PARAMETER DESCRIPTOR - number of items, type of items
 *
 */

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 0 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#define CGM_NO_OP                         0,0,1,1,SC_STRING_S
#define BEGIN_METAFILE                    0,1,1,1,SC_STRING_S
#define END_METAFILE                      0,2,0,0,NULL
#define BEGIN_PICTURE                     0,3,1,1,SC_STRING_S
#define BEGIN_PICTURE_BODY                0,4,0,0,NULL
#define END_PICTURE                       0,5,0,0,NULL

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 1 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#define METAFILE_VERSION                  1,1,1,1,SC_INT_S
#define METAFILE_DESCRIPTION              1,2,1,1,SC_STRING_S
#define VDC_TYPE                          1,3,1,1,SC_INT_S
#define INTEGER_PRECISION                 1,4,1,1,SC_INT_S
#define REAL_PRECISION                    1,5,1,3,SC_INT_S
#define MAXIMUM_COLOUR_INDEX              1,9,1,1,SC_CHAR_S
#define METAFILE_ELEMENT_LIST             1,11,1,3,SC_INT_S
#define METAFILE_DEFAULTS_REPLACEMENT(n)  1,12,1,n,NULL
#define FONT_LIST(n)                      1,13,1,n,SC_STRING_S

/* not currently used */

#if 0

#define INDEX_PRECISION                   1,6,1
#define COLOUR_PRECISION                  1,7,1
#define COLOUR_INDEX_PRECISION            1,8,1
#define COLOUR_VALUE_EXTENT               1,10,2
#define CHARACTER_SET_LIST                1,14,1
#define CHARACTER_CODING_ANNOUNCER        1,15,1

#endif

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 2 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#define COLOUR_SELECTION_MODE           2,2,1,1,SC_INT_S
#define BACKGROUND_COLOUR               2,7,1,3,SC_CHAR_S
#define LINE_WIDTH_SPECIFICATION_MODE   2,3,1,1,SC_INT_S

/* not currently used */

#if 0

#define SCALING_MODE                    2,1,2
#define MARKER_SIZE_SPECIFICATION_MODE  2,4,1
#define EDGE_WIDTH_SPECIFICATION_MODE   2,5,1
#define VDC_EXTENT                      2,6,2

#endif

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 3 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#define CLIP_RECTANGLE                  3,5,1,4,SC_INT_S
#define CLIP_INDICATOR                  3,6,1,1,SC_INT_S

/* not currently used */

#if 0

#define VDC_INTEGER_PRECISION           3,1,1
#define VDC_REAL_PRECISION              3,2,3
#define AUXILLIARY_COLOUR               3,3,1
#define TRANSPARENCY                    3,4,1

#endif

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 4 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#define POLYLINE(n)                     4,1,1,n,SC_INT_S
#define DISJOINT_POLYLINE(n)            4,2,1,n,SC_INT_S

#ifdef TEXT
# undef TEXT
#endif

#define TEXT                            4,4,2,3,SC_INT_S
#define POLYGON(n)                      4,7,1,n,SC_INT_S
#define CELL_ARRAY(p, n)                4,9,2,10,SC_INT_S,p,n,SC_CHAR_S

/* not currently used */

#if 0

#define POLYMARKER(n)                   4,3,n,SC_INT_S
#define RESTRICTED_TEXT                 4,5,5
#define APPEND_TEXT                     4,6,2
#define POLYGON_SET                     4,8,2                 /* unusual */
#define GENERALIZED_DRAWING_PRIMITIVE   4,10,1,"gdp *"        /* unusual */
#define RECTANGLE                       4,11,4,SC_INT_S
#define CIRCLE                          4,12,2
#define CIRCULAR_ARC_3_POINT            4,13,3
#define CIRCULAR_ARC_3_POINT_CLOSE      4,14,4
#define CIRCULAR_ARC_CENTRE             4,15,6
#define CIRCULAR_ARC_CENTRE_CLOSE       4,16,7
#define ELLIPSE                         4,17,3
#define ELLIPTICAL_ARC                  4,18,7
#define ELLIPTICAL_ARC_CLOSE            4,19,8

#endif

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 5 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#undef TEXT_ALIGNMENT

#define LINE_TYPE                       5,2,1,1,SC_INT_S
#define LINE_WIDTH                      5,3,1,1,SC_INT_S
#define LINE_COLOUR                     5,4,1,1,SC_CHAR_S
#define TEXT_FONT_INDEX                 5,10,1,1,SC_INT_S
#define TEXT_PRECISION                  5,11,1,1,SC_INT_S
#define TEXT_COLOUR                     5,14,1,1,SC_CHAR_S
#define CHARACTER_HEIGHT                5,15,1,1,SC_INT_S
#define CHARACTER_ORIENTATION           5,16,1,4,SC_INT_S
#define TEXT_ALIGNMENT(al, ca)          5,18,2,2,SC_INT_S,al,2,SC_DOUBLE_S,ca
#define INTERIOR_STYLE                  5,22,1,1,SC_INT_S
#define FILL_COLOUR                     5,23,1,1,SC_CHAR_S
#define COLOUR_TABLE(n)                 5,34,1,n,"PG_palette"

/* not currently used */

#if 0

#define LINE_BUNDLE_INDEX               5,1,1
#define MARKER_BUNDLE_INDEX             5,5,1
#define MARKER_TYPE                     5,6,1
#define MARKER_SIZE                     5,7,1
#define MARKER_COLOUR                   5,8,1
#define TEXT_BUNDLE_INDEX               5,9,1
#define CHARACTER_EXPANSION_FACTOR      5,12,1
#define CHARACTER_SPACING               5,13,1
#define TEXT_PATH                       5,17,1
#define CHARACTER_SET_INDEX             5,19,1
#define ALTERNATE_CHARACTER_SET_INDEX   5,20,1
#define FILL_BUNDLE_INDEX               5,21,1
#define HATCH_INDEX                     5,24,1
#define PATTERN_INDEX                   5,25,1
#define EDGE_BUNDLE_INDEX               5,26,1
#define EDGE_TYPE                       5,27,1
#define EDGE_WIDTH                      5,28,1
#define EDGE_COLOUR                     5,29,1
#define EDGE_VISIBILITY                 5,30,1
#define FILL_REFERENCE_POINT            5,31,1
#define PATTERN_TABLE                   5,32,5
#define PATTERN_SIZE                    5,33,4
#define ASPECT_SOURCE_FLAGS             5,35,1,"psf *"     /* unusual */

#endif

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 6 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#define ESCAPE                          6,1,2

/*--------------------------------------------------------------------------*/

/*                           CATEGORY 7 ELEMENTS                            */

/*--------------------------------------------------------------------------*/

#define MESSAGE                         7,1,2
#define APPLICATION_DATA                7,2,2

/*--------------------------------------------------------------------------*/
