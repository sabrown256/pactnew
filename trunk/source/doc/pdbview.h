TXT: PDBView User's Manual
MOD: 11/14/2014

<CENTER>
<P>
<H1><font color="#ff0000">$TXT</font></H1>
<H2>An interactive tool for working with PDB files</H2>
<H4>Stewart A. Brown</H4>
</CENTER>

<BLOCKQUOTE>

<P>
<HR>

<a href="#pdv_intro">
<H3>Introduction</H3>
</a>
<dl>
<a href="#pdv_syntax">
<dt><dd>PDBView Syntax</dd></dt>
</a>
<a href="#pdv_start">
<dt><dd>Starting PDBView</dd></dt>
</a>
<a href="#pdv_mysql">
<dt><dd>Accessing mySQL Databases</dd></dt>
</a>
<a href="#pdv_script">
<dt><dd>Shell Scripting with PDBView</dd></dt>
</a>
<a href="#pdv_auto">
<dt><dd>Automatic or Implicit Plotting</dd></dt>
</a>
<a href="#pdv_demo">
<dt><dd>Demonstration Script</dd></dt>
</a>
<a href="#pdv_deflt">
<dt><dd>Default State</dd></dt>
</a>
<a href="#pdv_odoc">
<dt><dd>Other PACT Documents</dd></dt>
</a>
</dl>

<a href="#pdv_comm">
<h3>PDBView Commands</h3>
</a>

<dt><dd><font color="#ff0000">o </font> <font color="#000000">
indicates functions that are heavily used</dd></dt>
</font>
<dt><dd><font color="#009900">o </font> <font color="#000000">
indicates functions for more optional use</dd></dt>
</font>
<p>

<dl>
<a href="#pdv_files">
<dt><dd>Opening, Creating, Closing, and Controlling PDB Files</dd></dt> 
</a>

<blockquote>

<table>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_cd">
CD
</a>
</td>
<td width="200">Change directory</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_cf">
CF
</a>
</td>
<td width="200">Change file</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_close">
CLOSE
</a>
</td>
<td width="200">Close file</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_clt">
CLT
</a>
</td>
<td width="200">Close text file</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_file">
FILE
</a>
</td>
<td width="200">Print current file info</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_lf">
LF
</a>
</td>
<td width="200">List files</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_mkdir">
MKDIR
</a>
</td>
<td width="200">Make a directory</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_n_entries">
N-ENTRIES
</a>
</td>
<td width="200"># file variables</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_ot">
OT
</a>
</td>
<td width="200">Open text file</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_pwd">
PWD
</a>
</td>
<td width="200">Print working directory</td>
</tr>

</table>

</blockquote>

<p>

<a href="#pdv_windows">
<dt><dd>Opening, Closing, and Controlling Windows</dd></dt>

</a><blockquote><table>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_clv">
CLV
</a>

</td>
<td width="200">
Close viewport
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_clw">
CLW
</a>

</td>
<td width="200">
Close window
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_cv">
CV
</a>

</td>
<td width="200">
Create viewport
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_cw">
CW
</a>

</td>
<td width="200">
Create window
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_create_palette">
CREATE-PALETTE
</a>

</td>
<td width="200">
Create palette
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_font">
FONT
</a>

</td>
<td width="200">
Set font
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_hcv">
HCV
</a>

</td>
<td width="200">
Hardcopy viewport
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_hc">
HC
</a>

</td>
<td width="200">
Hardcopy window
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_list_palettes">
LIST-PALETTES
</a>

</td>
<td width="200">
List all palettes
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_list_windows">
LIST-WINDOWS
</a>

</td>
<td width="200">
List active graphics devices
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_load_palette">
LOAD-PALETTE
</a>

</td>
<td width="200">
Load a palette
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_nxm">
NXM
</a>

</td>
<td width="200">
Create multi-viewport window
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_save_palette">
SAVE-PALETTE
</a>

</td>
<td width="200">
Save a palette
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_show_palettes">
SHOW-PALETTES
</a>

</td>
<td width="200">
Show available palettes
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_vp_area">
VIEWPORT-AREA
</a>

</td>
<td width="200">
Set viewport
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_wu">
WU
</a>

</td>
<td width="200">
Update window
</td>
</tr>

</table>

</blockquote>

<p>

<a href="#pdv_plot">
<dt><dd>Plotting Data</dd></dt>
</a>

<blockquote>

<table>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_anim">
ANIMATE
</a>
</td>
<td width="200">
Animate a rotation
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_annot">
ANNOT
</a>
</td>
<td width="200">
Annotate a window
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_autoplot">
AUTOPLOT
</a>
</td>
<td width="200">
Set/get autoplot flag
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_copy_map">
COPY-MAP
</a>
</td>
<td width="200">
Copy display mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_data_id">
DATA-ID
</a>
</td>
<td width="200">
Set/get data-id flag
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_data_ref">
DATA-REFERENCE
</a>
</td>
<td width="200">
Mapping from index
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_vr">
VR
</a>
</td>
<td width="200">
Set default rendering
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_dl">
DL
</a>
</td>
<td width="200">
Delete a mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_disp_dom">
DISPLAY-DOMAIN
</a>
</td>
<td width="200">
Print the domain
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_dr">
DR
</a>
</td>
<td width="200">
Set mapping rendering
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_domm">
DOMM
</a>
</td>
<td width="200">
Set mapping domain
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_domv">
DOMV
</a>
</td>
<td width="200">
Set viewport domain
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_get_domain">
GET_DOMAIN
</a>
</td>
<td width="200">
Get domain limits
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_get_dnpts">
GET-DOMAIN-NUMBER-POINTS
</a>
</td>
<td width="200">
Get # points in domain 
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_get_label">
GET_LABEL
</a>
</td>
<td width="200">
Get mapping label
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_get_range">
GET-RANGE
</a>
</td>
<td width="200">
Get range limits
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_get_rnpts">
GET-RANGE-NUMBER-POINTS
</a>
</td>
<td width="200">
Get # points in range
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_histo">
HISTOGRAM
</a>
</td>
<td width="200">
Histogram rendering
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_labdr">
LABEL-DRAWABLE
</a>
</td>
<td width="200">
Set mapping label
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_levels">
LEVELS
</a>
</td>
<td width="200">
Set contour levels
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_light_src">
LIGHT-SOURCE
</a>
</td>
<td width="200">
Set light source
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_hide">
HIDE
</a>
</td>
<td width="200">
Suppress plot
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_lncolor">
LNCOLOR
</a>
</td>
<td width="200">
Set line color
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_lnstyle">
LNSTYLE
</a>
</td>
<td width="200">
Set line style
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_lnwidth">
LNWIDTH
</a>
</td>
<td width="200">
Set line width
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_logical">
LOGICAL
</a>
</td>
<td width="200">
Logical rendering
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_lsv">
LSV
</a>
</td>
<td width="200">
List viewport mappings
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_marker_scale">
MARKER-SCALE
</a>
</td>
<td width="200">
Set marker size
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_menu">
MENU
</a>
</td>
<td width="200">
Print menu
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_move">
MOVE
</a>
</td>
<td width="200">
Move mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_overlay">
OVERLAY
</a>
</td>
<td width="200">
Set/get overlay flag
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_plane">
PLANE
</a>
</td>
<td width="200">
Create hyper-plane
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_pl">
PL
</a>
</td>
<td width="200">
Plot a menu item
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_plot">
PLOT
</a>
</td>
<td width="200">
Construct mapping and plot
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_palette">
PALETTE
</a>
</td>
<td width="200">
Set default palette
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_ranm">
RANM
</a>
</td>
<td width="200">
Set mapping range
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_ranv">
RANV
</a>
</td>
<td width="200">
Set viewport range
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_refmesh">
REFMESH
</a>
</td>
<td width="200">
Set/get refmesh flag
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_refmeshcolor">
REFMESHCOLOR
</a>
</td>
<td width="200">
Set/get refmesh line color
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_scatter">
SCATTER
</a>
</td>
<td width="200">
Scatter plot
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_vect_att">
SET-VECTOR-ATT
</a>
</td>
<td width="200">
Set vector rendering
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_span">
SPAN
</a>
</td>
<td width="200">
Create line
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_unhide">
UNHIDE
</a>
</td>
<td width="200">
Undo HIDE
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_va">
VA
</a>
</td>
<td width="200">
Set view angle
</td>
</tr>

</table>

</blockquote>

<p>

<a href="#pdv_data">
<dt><dd>Working with Data</dd></dt>
</a>

<blockquote>

<table>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_ands">
NDS
</a>

</td>
<td width="200">
Make set from array
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_change">
CHANGE
</a>

</td>
<td width="200">
Change data values
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_change_dim">
CHANGE-DIMENSION
</a>

</td>
<td width="200">
Change dimensions
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_cmc">
CMC
</a>

</td>
<td width="200">
Copy mapping as curve
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_cp">
CP
</a>

</td>
<td width="200">
Copy variable(s)
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_cm">
CM
</a>

</td>
<td width="200">
Copy mapping to file
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_def">
DEF
</a>

</td>
<td width="200">
Define variable to workspace
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_defv">
DEFINE-FILE-VARIABLE
</a>

</td>
<td width="200">
Define new file variable
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_desc">
DESC
</a>

</td>
<td width="200">
Describe variable
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_find">
FIND
</a>

</td>
<td width="200">
Find array values
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_ls">
LS
</a>

</td>
<td width="200">
List file variables
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_ls_attr">
LS-ATTR
</a>

</td>
<td width="200">
List variable attributes
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_lst">
LST
</a>

</td>
<td width="200">
List workspace
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_ac">
AC
</a>

</td>
<td width="200">
Make an AC set
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_cps">
CPS
</a>

</td>
<td width="200">
Make Cartesian product set
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_lr">
LR
</a>

</td>
<td width="200">
Make LR mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_lrs">
LRS
</a>

</td>
<td width="200">
Synthesize LR mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_minmax">
MINMAX
</a>

</td>
<td width="200">
Print mim/max of variable
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_print">
PRINT
</a>

</td>
<td width="200">
Print file variable
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_pm">
PM
</a>

</td>
<td width="200">
Print mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_sm">
SM
</a>

</td>
<td width="200">
Save mapping to file
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_scv">
SCALE-FILE-VARIABLE
</a>

</td>
<td width="200">
Scale file variable
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_struct">
STRUCT
</a>

</td>
<td width="200">
Display struct definition
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_table">
TABLE
</a>

</td>
<td width="200">
Write table to text file
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_types">
TYPES
</a>

</td>
<td width="200">
List file data types
</td>
</tr>

</table>

</blockquote>

<p>

<a href="#pdv_analysis">
<dt><dd>Data Analysis</dd></dt>
</a>

<blockquote>

<table>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_der">
DERIVATIVE
</a>

</td>
<td width="200">
Derivative of 1D mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_int">
INTEGRATE
</a>

</td>
<td width="200">
Integrate mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_norm">
NORM
</a>

</td>
<td width="200">
Norm of mapping
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_dfncs">
DOMAIN FUNCTIONS
</a>

</td>
<td width="200">
Unary operators on domain
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_rfncs">
RANGE FUNCTIONS
</a>

</td>
<td width="200">
Unary operators on range
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_binop">
BINARY OPERATORS
</a>

</td>
<td width="200">
Binary operators on mappings
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_mathop">
MATH OPERATORS
</a>

</td>
<td width="200">
Functions on mappings
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_ssop">
SHIFT/SCALE OPERATORS
</a>

</td>
<td width="200">
Shift or scale mappings
</td>
</tr>

</table>

</blockquote>

<p>

<a href="#pdv_misc">
<dt><dd>Working with PDBView</dd></dt>
</a>

<blockquote>

<table>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_command_log">
COMMAND-LOG
</a>

</td>
<td width="200">
Control command logging
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_end">
END
</a>

</td>
<td width="200">
Exit PDBView
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_format">
FORMAT
</a>

</td>
<td width="200">
Control data print formats
</td>
</tr>

<tr>
<td><font color="#ff0000">o </font></td>
<td width="180">
<a href="#pv_help">
HELP
</a>

</td>
<td width="200">
Get on-line help
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_ld">
LD
</a>

</td>
<td width="200">
Load user functions
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_mode">
MODE
</a>

</td>
<td width="200">
Struct print control
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_set">
SET
</a>

</td>
<td width="200">
Set printing parameters
</td>
</tr>

<tr>
<td><font color="#009900">o </font></td>
<td width="180">
<a href="#pv_syn">
SYNONYM
</a>

</td>
<td width="200">
Define synonyms or aliases
</td>
</tr>

</table>

</blockquote>

<p>

</dl>

<hr>

<a name="pdv_intro"></a>
<h3 ALIGN="CENTER">Introduction</h3>


The PACT system provides a number of tools for working with PDB files
in a fairly generic fashion. In particular, PDBView lets you interactively
browse a PDB file displaying the contents graphically or textually.
<p>

PDBView is an SX program. SX is the SCHEME dialect of the LISP
programming language with PACT extensions. The extensions provide functionality
for graphics, binary data handling, and other areas of functionality.
<p>

PDBView has a <B>help</B> command which provides information about available
commands.
<p>

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_syntax"></a>
<h3>PDBView Syntax</h3>


PDBView uses a slightly different data description syntax than PDBLib.
With PDBLib variable names and names of members of structures
cannot contain the characters: ".", "(", ")", "[", and "]". The characters
"(", ")", "[", and "]" are used in array reference and dimension definition
expressions. In PDBView, "(" and ")" CANNOT be used in variable reference
expressions because they are special characters for PDBView. For example,
<p>

<UL>
print a(2)
</UL>

is illegal and results in an error. The legal expression is:<p>

<UL>
print a[2]
</UL>

Other examples of legal expressions are:<p>

<BLOCKQUOTE>
          print a.b[3,2]
<P>
          print a.b[3][2]
<P>
          print a[3].b[2][5].c
<P>
          print a[3].b[2][3].c[3:4,5:50:10]
<P>
</BLOCKQUOTE>

The first two forms are not identical. In the first form an element of a
two dimensional array is being referenced. In the second form, the third
element of the fourth array of arrays is being referenced (assuming zero
based indexing in the definition of the b member of a).
<p>


For completeness and clarity in the following discussion, an index expression
is defined as:
<p>

<PRE>
         index expression := [index list]

         index list := index | index, index list

         index := simple index |

                       index-min : index-max |                 (*)

                       index-min : index-max : increment       (*)

         simple index := integer

         index-min := integer

         index-max := integer

         increment := integer

</PRE>


If an index expression uses either of the two starred forms it is said to
be a hyper-index expression. A hyper-index expression implies more than
one data item. Only the terminating index expression in a data reference
may be a hyper-index expression. An index expression is said to dereference
an indirection (or pointer or array). For each level of indirection
a suitable index expression dereferences the desired data.
<p>


For example, this means that a variable defined as:<p>

<UL>
          char **s
</UL>

is said to have two levels of indirection and can have parts accessed
as follows:
<p>

<PRE>
          print s			prints the entire item

          print s[2]			prints the third character array of s

          print s[3][10]		prints the eleventh character
                                        of the fourth character array of s

</PRE>

In the above example a zero based index is assumed.<p>


When referring to part of a variable, especially a structured variable, the
terminal node must be of primitive type or a structure containing no indirections
and whose descendant members contain no indirections. Furthermore, the path
to the desired part must contain one array reference for each level of
indirection traversed.
<p>

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_start"></a>
<h3>Starting PDBView</h3>


On UNIX systems a shell script called <B>pdbview</B> is provided; it starts up
SX which then loads the PDBView SCHEME forms. You must add a line to your
<B>.cshrc</B> or <B>.profile</B> that defines the
environment variable SCHEME. This
variable tells SX where to find the PDBView SCHEME files. Consult your system
administrator or some other knowledgeable source to find out where these
files are located on your system.  For example, if the directory
<B>/usr/local/scheme</B> contains the PDBView SCHEME files, add the line:
<p>
<UL>
setenv SCHEME /usr/local/scheme
</UL>
to your <B>.cshrc</B> or <B>.profile</B> file.<p>

<I>Usage:</I> pdbview [-d] [-h] [-l command-file] [-p n] [data-file]
<BR><br><I>Options:</I>
<P>

<PRE>
      d - dump the formatted contents of the data file in ASCII to stdout

      h - print execute line information to stdout

      l - load SCHEME forms (e.g. PDBView commands) from a file

            (not available with -d option)

      p - number of digits to use in displaying floating point numbers

            (only available with -d option)
</PRE>

<!-- -------------------------------------------------------------------- --> 

<a name="pdv_mysql"><h2>Accessing mySQL Databases</h2></a>

You might ask yourself why use PDBView to look at an SQL database.
The short answer is that certain database operations are not easy to
express in SQL.  PDBView offers a different view of the database in
some sense - a view with different limitations than those imposed by
SQL.  You may want to grab data out of an SQL database in order to
plot it, for example.

To access mySQL databases you will need to have some understanding
of the way mySQL operates.  Since a complete description is not apropriate
here, we will give a set of examples.<p>

Accessing mySQL databases with PDBView may require the use of the
<b>usr</b> and <b>pwd</b> attributes as spelled out in the section
on UDL syntax above.<p>

Example 1)  If you ran the mysql client as follows:

<pre>
    mysql -h host.net -u db db
</pre>

to access database <i>db</i> as user <i>db</i> on host <i>host.net</i>
then you would access this using PDBView as:<p>

<pre>
    pdbview mysql://host.net/db
</pre>

Example 2)  If you ran the mysql client as follows:

<pre>
    mysql -h host.net -u me db
</pre>

to access database <i>db</i> as user <i>me</i> on host <i>host.net</i>
then you would access this using PDBView as:<p>

<pre>
    pdbview mysql://host.net/db,usr=me
</pre>

Example 3)  If you ran the mysql client as follows:

<pre>
    mysql -h host.net -u me -pfubar db
</pre>

to access database <i>db</i> as user <i>me</i> on host <i>host.net</i>
using password <i>fubar</i> (a very insecure and bad thing to do)
then you would access this using PDBView as:<p>

<pre>
    pdbview mysql://host.net/db,usr=me,pwd=fubar
</pre>

Example 4)  If you ran the mysql client as follows:

<pre>
    mysql -h host.net -u me -p db
</pre>

to access database <i>db</i> as user <i>me</i> on host <i>host.net</i>
then you would access this using PDBView as:<p>

<pre>
    pdbview mysql://host.net/db,usr=me,pwd
</pre>

and you would be prompted for a password during the process of opening
the database.<p>

Example 5)  If you ran the mysql client as follows:

<pre>
    mysql -u me db
</pre>

to access database <i>db</i> as user <i>me</i> on host the local host
then you would access this using PDBView as:<p>

<pre>
    pdbview mysql:///db,usr=me
</pre>

and you would be prompted for a password during the process of opening
the database.<p>

<!-- -------------------------------------------------------------------- --> 
<!-- -------------------------------------------------------------------- --> 
<hr>

<a name="pdv_script"></a>
<h3>Shell Scripting with PDBView</h3>


Users are often faced with having to issue complex combinations of
commands for PDBView.  Since PDBView is programmable they write
files for the interpreter to read.  At some point they may want to
run PDBView with their files over and over again.
<p>
PDBView has a few features to make this easier.  Suppose you have
written a file foo.scm with all kinds of procedures and commands
and now you want to "run" it.  The first and most obvious way to
do so is to issue the command:

<PRE>
   pdbview -l foo.scm
</PRE>

That is nice enough, but suppose you want to supply command line
arguments in order to vary the operation of the commands in foo.scm.
<p>

You could write foo.scm so that it has a main function, foo, which
controls all the work.  Then you can run it as follows:

<PRE>
   pdbview -l foo.scm \(foo 1 x "abc"\)
</PRE>

This is a very simple feature of PDBView that if the command line
contains a single Scheme expression it is evaluated and then exits.
This always works but isn't very convenient to use.
<p>

The next feature that PDBView provides is that depending on the
command line invocation, the command line arguments are gathered up
and made available as a global list called <B>argv</B>.  The procedures
in foo.scm can use the standard list operations to access the
items in <B>argv</B>.
<p>

Now to run your script with the command line arguments you can do:

<PRE>
   pdbview foo.scm 1 x "abc"
</PRE>

In this case <B>argv</B> is:

<PRE>
   (1 x "abc")
</PRE>

Notice that there is no -l in this invocation.  This is an important
point.  With the -l, no command line arguments are gathered and <B>argv</B>
is not defined.  This is done to give flexibility to the handling
of data files and interpreter files where for example you may have:

<PRE>
   pdbview -l foo.scm a.dat -l bar.scm b.dat
</PRE>

When PDBView trying to decide about whether or not to provide the
command line arguments in <B>argv</B> it looks for an ASCII file
not preceded by -l.  There is another reason for this too.
<p>

The next feature of PDBView is that a text file which conforms to
the '#!' convention common in UNIX and is also executable can
be run without the explicit reference to PDBView on the command line.
In other words, it acts just like any other shell script.  For example,
suppose you add:

<PRE>
   #!/usr/bin/env pdbview
</PRE>

to the top of foo.scm and call that file foo.  Make foo executable
and you can run it:

<PRE>
   foo 1 x "abc"
</PRE>

just like the example above.  The limitations of /usr/bin/env prevent
you from having the first line of foo look like:

<PRE>
   #!/usr/bin/env pdbview -l
</PRE>

This is another reason why the form

<PRE>
   pdbview foo.scm 1 x "abc"
</PRE>

is treated as it is, i.e. without the -l.
<p>

To summarize and keeping in mind the discussion above you have the
following alternative to running PDBView scripts:

<PRE>
   pdbview -l foo.scm \(foo 1 x "abc"\)

   pdbview foo.scm 1 x "abc"

   foo 1 x "abc"
</PRE>


<!--------------------------------------------------------------------------> 

<a name="pdv_auto"></a>
<h3>Automatic or Implicit Plotting</h3>


PDBView can be used in several different contexts and in these contexts
the behaviour of it with respect to refreshing the current window or viewport
when new mappings are created can be quite different.  Consider the following
cases: (1) an interactive session doing visualization or data analysis; and
(2) an interactive session that is not doing visualization but does create
new mappings.
<p>
In the first case, it is very convenient for users if new data is plotted
as it is available.  This includes data read from file for display or
data created via any of number of PDBView commands.
<p>
The autoplot flag is the important means of controlling PDBView's behaviour.
By default it is on and data is automatically plotted in the current
viewport of the current window as it becomes available.  You can
exert explicit control of what gets plotted where, but we are speaking
about implicit or default behaviour here.
<p>
If you want to assert explicit control of all plotting then set the
autoplot flag to off.  This point can be confusing, but a little experimentation
will help to familiarize you with the behavior of PDBView in these different
contexts.
<p>

<!--------------------------------------------------------------------------> 

<a name="pdv_demo"></a>
<h3>Demonstration Script</h3>


In order to help make sense of the sometimes bewildering variety of
commands and capabilities in PDBView, we have included a demonstration
script which can be started as follows:

<pre>
   pdbview -l pdbvdemo.scm
</pre>

The demo proceeds in small sections.  Before each section begins, it
describes in broad terms what it is about to do.  It then prompts you
to go ahead with the commands in that section.  Each command is printed
out as you would do yourself and then it is executed so that you can
see its effect.  In this way, you get a chance to go through the demo at
your own pace and study each section without losing your way in a
blizzard of output.
<p>
The demo will open several plotting windows so you may want to run it
with a relatively un-cluttered desktop.
<p>
The demo will create some data files at the beginning for later use.
Therefore, be sure to run the demo in a directory to which you have
read and write permission.
<p>

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_comm"></a>
<h3>PDBView Commands</h3>


In this section the PDBView commands are listed alphabetically. The command
name is followed by synonyms, if any. Each command is given with a brief
description of its function and its usage. The examples are intended to
illustrate the various ways of invoking each command. Optional arguments
are enclosed in brackets. Alternate argument forms are separated by `|'.
Formal arguments in italic font are replaced by actual arguments. Command
and argument keywords are in regular non-italic font.
<p>

The file most recently referred to in a <B>cf (change-file)</B> command is the
current file. Most commands implicitly refer to the current file or its
contents. Some commands permit another file to be explicitly specified. A
file may be referred to by name or by the alias assigned when the file was
opened. Commands that operate on the contents of more than one file
(e.g. <B>cp, copy-mapping</B>), require that the target file be specified, while
the current file is assumed to be the source.
<p>

For the sake of brevity all menu items including mappings, images, and ULTRA
curves are collectively referred to as mappings. All commands that accept
mappings as arguments, accept images and curves as well.
<p>

<!--------------------------------------------------------------------------> 

<!                       WORKING WITH FILES                                 >

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_files"></a>
<h3>Opening, Creating, Closing, and Controlling PDB Files</h3>

A large part of PDBView has to do with browsing binary data files.  It
was specifically designed for PDB files, but it is extensible and the
user can write functions which permit PDBView to examine many kinds of
binary data files.
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cd"></a>
<h3>CD</h3>

Change the current file directory. The new directory may be specified by
either a relative path or a full path.
<p>
Usage:	cd [directory]
<p>
Examples:
<p>
<UL>
cd /zoo/mammals
<BR>cd ../reptiles
<BR>cd
</UL>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cf"></a>
<h3>CHANGE-FILE / CF</h3>

Change the current file. If it is not already open, open the file. Many
commands refer to the current file or its contents by default. Any number
of files may be open - only one is the current file. The mode may be:
<B>r</B>, open file read only; 
<B>a</B>, open file read/write (default); or
<B>w</B>, overwrite
any existing file. If no alias is specified, <B>fd</B> is assigned, where d is
an increasing decimal integer. The options available for type are determined
by the output spokes you have installed. The default type is <B>pdb</B>.
Files are maintained in a circular list and, if given no argument, CF
changes to the next file in the list.
<p>
Procedure version of this command is change-file*
<p>
Usage:	change-file [filename [mode [alias [type]]]]
<p>
Examples:
<p>
<UL>
change-file foo
<BR>cf foo a
<BR>cf foo w bar pdb
<BR>cf
</UL>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_close"></a>
<h3>CLOSE</h3>

Close a file. The default is to close the current file.
<p>
Usage:	close [file]
<p>
Examples:
<UL>
close
<BR>close foo
<BR>close f1
</UL>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_clt"></a>
<h3>CLOSE-TEXTFILE / CLT</h3>

Close the opened text file.
<p>
Usage:	close-textfile
<p>
Examples:
<pre>
          close-textfile
          clt
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_file"></a>
<h3>FILE</h3>

Describe the current file. Information returned is: name; type;
creation date; version of PDB that created the file; default offset;
major order; types that will require conversion when read on the current
host platform; header address; symbol table address; and structure char
address.
<p>
Optionally, the file type alone may be requested.
<p>
Usage:	file [t | type]
<p>
Example:
<pre>
          file type
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lf"></a>
<h3>LIST-FILES / LF</h3>

List the open files. The current file is indicated by an asterisk.
<p>
Usage:	list-files
<p>
Examples:
<pre>
          list-files
          lf
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_mkdir"></a>
<h3>MKDIR</h3>

Create a new directory in the current file
<p>
Usage: mkdir [directory-name]
<p>
Examples: mkdir /foo/bar
          mkdir ../baz
          mkdir mydir
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_n_entries"></a>
<h3>N-ENTRIES</h3>

Print the total number of variables, links, and directories in the current file.
<p>
Usage:	n-entries
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ot"></a>
<h3>OPEN-TEXTFILE / OT</h3>

Open a text file.  These are used to dump out tabular ASCII versions of
entries in binary files.
<p>
Usage: open-textfile filename
<p>
Example: open-textfile foo
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_pwd"></a>
<h3>PWD</h3>

Print the current file directory.
<p>
Usage:	pwd
<p>

<!--------------------------------------------------------------------------> 

<!                       WORKING WITH WINDOWS                               >

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_windows"></a>
<h3>Opening, Closing, and Controlling Windows</h3>

An important aspect of handling data is visualization.  The foundation of
visualization is the display.  PDBView gives the user many options for
displaying data.  It supports: multiple screen windows; multiple viewports
in a window; and multiple other output devices - PS, CGM, JPEG, and MPEG.
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_clv"></a>
<h3>CLOSE-VIEWPORT / CLV</h3>

Close a viewport. If no viewport is specified, close the current viewport.
<p>
Usage:	close-viewport [viewport [window]]
<p>
Examples:
<pre>
          close-viewport "ABC"
          clv
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_clw"></a>
<h3>CLOSE-WINDOW / CLW</h3>

Close a graphics window. If no window is specified, close the current window.
<p>
Usage:	close-window [window]
<p>
Examples:
<pre>
          close-window "ABC"
          clw
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cv"></a>
<h3>CHANGE-VIEWPORT / CV</h3>

Change the current viewport. When the NXM command has been used to make
a multi-viewport window, CV is used to move the focus to a new viewport
in such a window.
Many commands refer to the current viewport or its contents by default. 
Viewports are maintained in a circular list and, if given no argument,
CV changes to the next viewport in the list.
<p>
Usage: change-viewport [name [x y dx dy]]
<p>
Examples:
<pre>
          change-viewport
          change-viewport "B" 0.01 0.01 0.98 0.48
          cv "V2"
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cw"></a>
<h3>CHANGE-WINDOW / CW</h3>

Change the current window. If it is not already open, open the window. Many
commands refer to the current window or its contents by default. Any number
of windows may be open - only one is the current window.
Windows are maintained in a circular list and, if given no argument, CW
changes to the next window in the list.
<p>
Usage: change-window [title [mode type x y dx dy]]
<p>
Examples:
<pre>
          change-window
          change-window "B" "COLOR" "WINDOW" 0.5 0.1 0.4 0.4
	  cw "bar"
	  cw
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_create_palette"></a>
<h3>CREATE-PALETTE</h3>

Create a new palette.  See the SAVE-PALETTE command for saving to disk.
<p>
Usage: create-palette name [ndims nx ny]
<p>
Examples:
<pre>
         create-palette new_palette
         create-palette new_palette 2 24 8
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_font"></a>
<h3>FONT</h3>

Set/get the current font information.
<p>
Usage: font [face point-size style]
<dl>
            face         - helvetica | times | courier
            point-size   - 8 | 10 | 12 | 14 ...
            style        - medium | bold | italic
</dl>
<p>
Examples:
<pre>
         font
         font helvetica 12 medium
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_hcv"></a>
<h3>HARDCOPY-VIEWPORT</h3>

Send the current graph to all open hardcopy devices. Color output is
indicated by an optional argument on the first call for a given device.
The optional resolution scale factor is an integer factor by which the
resolution will be decreased below the full resolution of the device.
<p>
Usage: hardcopy-viewport [color | monochrome] [portrait | landscape] [resolution-scale-factor]]
<p>
Examples:
<pre>
          hardcopy-viewport
          hardcopy-viewport color
          hardcopy-viewport color 8
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_hc"></a>
<h3>HARDCOPY-WINDOW / HC</h3>

Draw the contents of the current window to all open hardcopy devices. Color
output is indicated by the optional keyword, color, on the first call for
a given device. The resolution is decreased below full device resolution
by an integer resolution scale factor, if present. Hardcopy files remain
open until explicitly closed or until PDBView is terminated.
<p>
Usage:	hardcopy-window [color] [resolution-scale-factor]
<p>
Examples:
<pre>
          hardcopy-window
          hc color 8
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_list_palettes"></a>
<h3>LIST-PALETTES</h3>

List the available palettes.
<p>
Usage: list-palettes [window]
<p>
Examples:
<pre>
          list-palettes "A"
          list-palettes
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_list_windows"></a>
<h3>LIST-WINDOWS</h3>

List the open windows.
<p>
Usage: list-windows
<p>
Examples:
<pre>
          list-windows
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_load_palette"></a>
<h3>LOAD-PALETTE</h3>

Load a palette from a file.
<p>
Usage: load-palette fname
<p>
Example:
<pre>
          load-palette foo.pal
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_nxm"></a>
<h3>NXM</h3>

If there is a current window, this command changes it to a multi-viewport
window which are tiled n x m (hence the name).  If there is no open
window such a multi-viewport window will be created.  See CW for further
details.
<p>
Usage:	nxm <# viewport in x> <# viewport in y> [<viewport-name>]+
<p>
Examples:
<pre>
          nxm 2 1 "A" "B"
	  nxm 3 2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_save_palette"></a>
<h3>SAVE_PALETTE</h3>

Write palette pname out to disk file fname.
<p>
Usage: save-palette pname fname
<p>
Example:
<pre>
          save-palette foo foo.pal
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_show_palettes"></a>
<h3>SHOW_PALETTES</h3>

Show the available palettes and make selection current.
<p>
Usage: show-palettes [window]
<p>
Examples:
<pre>
          show-palettes
          show-palettes "B"
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_vp_area"></a>
<h3>VIEWPORT-AREA</h3>

Set drawing area, in NDC, of the current viewport.
<p>
Usage: viewport-area x-min x-max y-min ymax
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_wu"></a>
<h3>UPDATE-WINDOW / WU</h3>

Update all of the viewports of the specified windows.  Sometimes plotting
operations are done without a window update.  User defined functions
very commonly do this.  WU lets you explicitly do the update.  It also
marks the completion of a picture for a non-screen device window.  After
an update a new frame or picture will be started.
<p>
Just typing a carriage return will implicity do a WU command.
<p>
Usage:	update-window [window]+
<p>
Examples:
<pre>
          update-window "2x2"
	  wu
</pre>

<!--------------------------------------------------------------------------> 

<!                           PLOTTING DATA                                  >

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_plot"></a>
<h3>Plotting Data</h3>

PDBView can be used to plot data.  The most common usage is to plot data
from files.  It has a variety of commands to let users gather diverse
kinds of data from files and assemble them into plots.  It also possesses
the capability of creating data from scratch to be plotted.
<p>
Plotted datasets are kept in a list associated with a viewport.  Each dataset
or mapping is referred to by an integer index.  In many of the following
commands and elsewhere you will see mappings referred to by a number.  The
meaning is the mapping whose index is specified in the viewport implicitly
or explicitly specified.  The LSV command is used to examine the list of
mappings in a viewport.
<p>
Mappings in files are also specified by an integer index.  These can be
examined by using the MENU command.  There can be some confusion as to whether
an index refers to a file or a viewport.  Try to be mindful of this.  This
behaviour differs from ULTRA where the displayed curves are referred to
by letters.  Unfortunately, this limited one to having a maximum of 26
curves displayed at once.  PDBView removes this limitation at the cost
of using integer indices in both contexts.
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_anim"></a>
<h3>ANIMATE</h3>

Rotate the specified mapping thru 360 degress
<p>
Usage:	animate [theta] [phi] [chi]
<p>
Examples:
<pre>
          animate 30 20 0
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_annot"></a>
<h3>ANNOT</h3>

Add an annotation to the current window.
<p>
Usage: annot label color xmin xmax ymin ymax
<p>
Example:
<pre>
          annot "look here" blue 0.4 0.6 0.95 0.98
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_autoplot"></a>
<h3>AUTOPLOT</h3>

Get/set the autoplot flag.
<p>
Usage: autoplot [off | on]
<p>
Example:
<pre>
          autoplot
          autoplot off
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_copy_map"></a>
<h3>COPY-MAP</h3>

Copy the specified mappings
<p>
Usage: copy-map [mapping]*
<p>
Example:
<pre>
          copy-map 12
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_data_id"></a>
<h3>DATA-ID</h3>

DATA-ID - Turn on/off the plotting of data ids.
<p>
Usage: data-id [on | off]
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_data_ref"></a>
<h3>DATA-REFERENCE</h3>

Refer to the nth mapping in the optionally specified file.  This is primarily
for the benefit of those who want to write their own functions.
<p>
The procedure version is data-reference*.
<p>
Usage: data-reference n [file-name|file-alias|file-type]
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_vr"></a>
<h3>VR</h3>

Specify a default rendering mode for a viewport.
<p>
Usage: default-viewport-rendering [viewport [window]] info
<p>
<pre>
       for 1d domain - 1d range mappings
       info - cartesian | polar | insel

       for 2d domain - 1d range mappings
       info - contour [n-levels] |
              image      |
              dvb        |
              fill-poly  |
              shaded     |
              points     |
              wire-frame |
              mesh

       for 2d domain - 2d range mappings
       info - vect   |
              shaded |
              mesh

       for 3d domain
       info - mesh   |
              shaded
</pre>
Examples:
<pre>
          default-viewport-rendering "A" "A" polar
          default-viewport-rendering contour
          default-viewport-rendering contour 15
          default-viewport-rendering image
          default-viewport-rendering fill-poly
          vr wire-frame
          vr shaded
          vr vect
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_dl"></a>
<h3>DL</h3>

Delete mappings from a viewport.
<p>
Usage: delete-mapping [vname [wname]] mapping*
<p>
Examples:
<pre>
          delete-mapping 2 4 5
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_disp_dom"></a>
<h3>DISPLAY-DOMAIN</h3>

Plot a domain mesh in a viewport.
<p>
Usage: display-domain [viewport [window]] domain*
<p>
Example:
<pre>
          display-domain 18
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_dr"></a>
<h3>DR</h3>

Specify the rendering mode for a drawable.  This permits the user to render
different datasets differently in the same viewport.
<p>
Usage: drawable-rendering drawable info
<p>
       info - [spec]*
       spec - (render render) |
              - (view-angle theta phi chi) |
              - (change-palette palette)
       for 1d domain - 1d range mappings
       render - cartesian | polar | insel
       for 2d domain - 1d range mappings
       render - contour [n-levels] |
                image      |
                dvb        |
                fill-poly  |
                shaded     |
                points     |
                wire-frame |
                mesh
       for 2d domain - 2d range mappings
       render - vect       |
                shaded
       for 3d domain
       render - mesh       |
                shaded
Examples:
<pre>
          drawable-rendering 3 (render polar)
          drawable-rendering 1 (render contour)
          drawable-rendering 1 (render contour 15)
          drawable-rendering 12 (render image) (change-palette spectrum)
          drawable-rendering 18 (render fill-poly)
          drawable-rendering 4 (render wire-frame) (view-angle 60 45 0)
          dr 1 (render shaded) (change-palette cyans)
          dr 1 (render vect)
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_domm"></a>
<h3>DOMM</h3>

Select plotting limits for the domain of a mapping.
If invoked with no limit values the domain
will default to that implied by the data set.
<p>
Usage: domm [viewport [window]] mapping x1_min x1_max ...
<p>
Examples:
<pre>
          domm 1 -5.0 10.0 10 20
          domm 1
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_domv"></a>
<h3>DOMV</h3>

Select plotting limits for the domain of a viewport.
If invoked with no limit values the domain
will default to that implied by the data sets.
<p>
Usage: domv [viewport [window]] x1_min x1_max ...
<p>
Examples:
<pre>
          domv -5.0 10.0 10 20
          domv
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_get_domain"></a>
<h3>GET-DOMAIN</h3>

Get the domain of the given mapping.
<p>
Usage: get-domain mapping
<p>
Examples:
<pre>
          get-domain 2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_get_dnpts"></a>
<h3>GET-DOMAIN-NUMBER-POINTS</h3>

Get the domain of the given mapping.
<p>
Usage: get-domain-number-points mapping
<p>
Examples:
<pre>
          get-domain-number-points 2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_get_label"></a>
<h3>GET-LABEL</h3>

Get the domain of the given mapping.
<p>
Usage: get-domain-number-points mapping
<p>
Examples:
<pre>
          get-domain-number-points 2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_get_range"></a>
<h3>GET-RANGE</h3>

Get the range of the given mapping.
<p>
Usage: get-range mapping
<p>
Examples:
<pre>
          get-range 2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_get_rnpts"></a>
<h3>GET-RANGE-NUMBER-POINTS</h3>

Get the range of the given mapping.
<p>
Usage: get-range-number-points mapping
<p>
Examples:
<pre>
          get-range-number-points 2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_histo"></a>
<h3>HISTOGRAM</h3>

Display given mappings from current viewport as histograms.
<p>
Usage: histogram on | off | left | right | center [integer]+
<p>
Examples:
<pre>
          histogram on 1 2
          histogram left 3
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_labdr"></a>
<h3>LABEL-DRAWABLE</h3>

Label a drawable.
<p>
Usage: label-drawable [vname [wname]] mapping
<p>
Examples:
<pre>
          label-drawable 2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_levels"></a>
<h3>LEVELS</h3>

Set the contour levels
<p>
Usage: levels [clev]*
<p>
Examples:
<pre>
          levels 1.0 1.2 1.5 1.9 2.5 5.8 10.3
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_light_src"></a>
<h3>LIGHT-SOURCE</h3>

Set the theta and phi angles for the light source.  This is used in
the rendering of surface of 3D renderings.
<p>
Usage: light-source theta phi mapping*
<p>
Examples:
<pre>
          light-source 30 -20 1
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_hide"></a>
<h3>HIDE</h3>

Don't plot the specified mappings.  They remain on the list for the viewport
and can be made visible with subsequent unhide commands.
<p>
Usage: hide [integer]+
<p>
Examples:
<pre>
          hide 1 3 19
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lncolor"></a>
<h3>LNCOLOR</h3>

Set the line color used in drawing the specified mappings.
<p>
Usage: lncolor color [integer]+
<p>
Examples:
<pre>
          lncolor blue 1 3 19
          lncolor red 2 5 7
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lnstyle"></a>
<h3>LNSTYLE</h3>

Set the line style used in drawing the specified mappings.
<p>
Usage: lnstyle style [integer]+
<p>
Examples:
<pre>
          lnstyle dotted 1 3 19
          lnstyle dashed 2 5 7
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lnwidth"></a>
<h3>LNWIDTH</h3>

Set the line width used in drawing the specified mappings.
<p>
Usage: lnwidth width [integer]+
<p>
Examples:
<pre>
          lnwidth 1.5 1 3 19
          lnwidth 2.0 2 5 7
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_logical"></a>
<h3>LOGICAL</h3>

Display given mappings from current viewport versus index.
<p>
Usage: logical on | off [integer]+
<p>
Examples:
<pre>
          logical on 1 3 19
          logical off 2 5 7
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lsv"></a>
<h3>LSV</h3>

List the mappings or images associated with a viewport.
<p>
Usage: list-mappings-in-viewport [vname [wname]]
<p>
Examples:
<pre>
          lsv
          lsv "V2"
          lsv "V2" "2x1"
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_marker_scale"></a>
<h3>MARKER-SCALE</h3>

Set the marker scale for the specified mappings.
<p>
Usage: marker-scale scale [integer]+
<p>
Examples:
<pre>
          marker-scale 1.8 1 3 19
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_menu"></a>
<h3>MENU</h3>

List the labels of mappings in the current file directory. The ordinals
preceding the labels are used to reference the mappings in other commands.
An optional selection pattern may be specified. Each `?' in the pattern
matches any single character in a label. Each `*' in the pattern matches
any string of zero or more characters. Note that the pattern, if supplied,
must match the entire label. Liberal use of "*"
<p>
Usage:	menu [pattern]
<p>
Examples:
<pre>
          menu
          menu ?*d
          menu *foo*
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_move"></a>
<h3>MOVE</h3>

Move items from the current viewport and window to the named
viewport and window.
<p>
Usage: move vname wname item*
<p>
Example:
<pre>
          move "V0" "B" 1 2 3
          move "V2" A 3
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_overlay"></a>
<h3>OVERLAY</h3>

Get/set the overlay flag.  If on, the overlay flag plots all mappings in
the current window viewport in a common graphical viewport.  Otherwise
the mappings will have their own graphical viewport and won't in general
register with one another unless they have the same rendering mode.
<p>
Usage: overlay [off | on]
<p>
Example:
<pre>
          overlay
          overlay off
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_plane"></a>
<h3>PLANE</h3>

Create and draw a hyper plane.
<p>
The proceure version is plane*.
<p>
Usage: plane c0 (c1 x1min x1max npts1) ...
<p>
Examples:
<pre>
          plane 1.0 (2.0 -5.0 5.0 100)
          plane 0.0 (2.0 -5.0 5.0 10) (0.5 -5.0 5.0 10)
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_pl"></a>
<h3>DISPLAY-MAPPING / PL</h3>

Plot the specified mappings. Mappings are referenced by the numbers displayed
by the menu command or by the names of variables containing mapping information.
<p>
Usage:	display-mapping mapping-list
<p>
Examples:
<pre>
          display-mapping 5
          pl dir1/Mapping8
          pl 5 8
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_plot"></a>
<h3>PLOT</h3>

Plot one variable or part of a variable (range) against another (domain).
Plot versus index (logical) if no domain given.
<p>
Procedure version is plot*.
<p>
Usage:	plot range [domain]
<p>
Examples:
<pre>
          plot y x
          plot z
          plot dir1/ddd[0:60, 0:15, 20]
          plot log10 y x
          plot log10 z (cps log10 x log10 y)
          plot (nds x 10 15)
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_palette"></a>
<h3>CHANGE-PALETTE / PALETTE</h3>

Set the current color palette.
<p>
Usage: change-palette standard | spectrum | rainbow | bw
<p>
Example:
<pre>
          palette rainbow
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ranm"></a>
<h3>RANM</h3>

Select plotting limits for the range of a mapping.
If invoked with no limit values the range
will default to that implied by the data set.
<p>
Usage: ranm [viewport [window]] mapping x1_min x1_max ...
<p>
Examples:
<pre>
          ranm 1 -5.0 10.0 10 20
          ranm 1
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ranv"></a>
<h3>RANV</h3>

Select plotting limits for the range of a viewport.
If invoked with no limit values the range
will default to that implied by the data sets.
<p>
Usage: ranv [viewport [window]] x1_min x1_max ...
<p>
Examples:
<pre>
          ranv -5.0 10.0 10 20
          ranv
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_refmesh"></a>
<h3>REFMESH</h3>

Overlay the plot of a mapping with its mesh iff on.
<p>
Usage: refmesh on | off
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_refmeshcolor"></a>
<h3>REFMESHCOLOR</h3>

Set the line color for the reference mesh.
<p>
Usage: refmeshcolor color
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_scatter"></a>
<h3>SCATTER</h3>

Display given mappings from current viewport as scatter plots.
<p>
Usage: scatter on | off | plus | star | triangle [integer]+
<p>
Examples:
<pre>
          scatter on 1 3 19
          scatter star 2 5 7
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_vect_att"></a>
<h3>SET-VECTOR-ATT</h3>

Set vector plotting attributes.
<p>
Usage: set-vector-att scale | headsize | color value
<p>
Examples:
<pre>
          set-vector-att 0.1 0.2 red
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_span"></a>
<h3>SPAN</h3>

Create a line with slope 1 and intercept 0.  This a restricted special case
of the PLANE command.
<p>
Usage: span xmin xmax
<p>
Example:
<pre>
          span 0 10
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_unhide"></a>
<h3>UNHIDE</h3>

Do plot the specified mappings.  This undoes what the HIDE command does.
<p>
Usage: unhide [integer]+
<p>
Examples:
<pre>
          unhide 1 3 19
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_va"></a>
<h3>VIEW-ANGLE / VA</h3>

Set the three Euler viewing angles.
The view angle is a rotation by phi about the z axis
starting from the negative y axis counterclockwise
followed by a rotation by theta about the x' axis
counterclockwise and followed by a rotation by
chi about the z'' axis again starting from the negative
y'' axis counterclockwise.  Angles are measured in degrees.
<p>
Usage: view-angle theta phi chi
<p>
Examples:
<pre>
          va 20 30 0
</pre>

<!--------------------------------------------------------------------------> 

<!                         WORKING WITH DATA                                >

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_data"></a>
<h3>Working with Data</h3>

PDBView allows the user to manipulate various kinds of data in non-graphical
ways.  Data in files can be perused, manufactured, or modified.  Data types
can also be defined and be used to add new data to a file.
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ands"></a>
<h3>ARRAY-ND-SET / NDS</h3>

Construct a set from an array which has specific dimensions.
<p>
Usage: array-nd-set var [dim]*
<p>
Example:
<pre>
           array-nd-set x 10 20
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_change"></a>
<h3>CHANGE</h3>

Change one or more values in a variable (array or scalar) or structure member.
Note that the command
keyword may be omitted. To change an array element, qualify the name with
index expressions. (See the <a href="#pdv_syntax">PDBView Syntax</a> section for a discussion of index
expressions). Always use '[' and ']' to access array elements. 
<p>
This command has an implicit form in which the "change" can be omitted.  The
implicit form can be used at the command prompt when the file variable
name does not conflict with a PDBView command name.
<p>
Usage:	[change] variable | structure-member value
<p>
Examples:
<pre>
          change a[10,15] 3.5
</pre>
Reset the value of a[10,15] to 3.5.<p>
<pre>
          change time 0.0
</pre>
Reset the value of time to 0.0.<p>
<pre>
          x[1:2] 1 2
</pre>
Reset the values x[1] and x[2] to 1 and 2 respectively.<p>
<pre>
          a[5,10,3] 5.7e4
</pre>
Reset the value of a[5,10,3] to 5.7e4.<p>
<pre>
          dir1/jkl.k 2
</pre>
Reset the value of the k member of struct instance jkl in directory dir1 to 2.<p>
<pre>
          a  (1 2 3)
</pre>
Suppose a is an array of 5 integers.  This sets a[0] to 1, a[1] to 2, and
a[2], a[3], and a[4] to 3.  This is why you must NOT refer array elements
using parentheses.  An expression such as:

<pre>
          a(2)
</pre>

would be interpreted as a request to set all values of a to 2!<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_change_dim"></a>
<h3>CHANGE-DIMENSION / CHDIM</h3>

Change the dimensions of a variable in memory.
<p>
Usage: change-dimension name dimension_list
<p>
Examples:
<pre>
          change-dimension foo 20
          change-dimension bar 10 10
          change-dimension foobar (2 . 10) (3 . 5)
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cmc"></a>
<h3>CMC</h3>

Copy 1d mappings from the current file to another file
as ULTRA curves. Mappings are referenced by the numbers
displayed by the menu command. If the mapping list is *,
copy all 1d mappings in the current directory.
<p>
Usage: cmc file mapping-list
<p>
Examples:
<pre>
          cmc foo *
          cmc bar 1 5
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cp"></a>
<h3>CP</h3>

Copy variables from the current file to another file.
This is designed to be like the UNIX cp command.
If variable list is *, copy all variables in current directory.
If variable list starts with ~, copy all but the following variables
in current directory.
<p>
Usage: cp [<I>flags</I>] <I>variable-list</I> <I>file</I>[:<I>dir</I>]
<p>
            where the value of flags is:
<pre>
         -R  copy directories recursively
         -w  remove and create the destination file
         -a  append to the destination file
</pre>
Examples:
<pre>
          cp * foo
          cp -R * foo
          cp -R * foo:/new
          cp -R -w * foo:/new
          cp ~ bird cat foo
          cp bird cat dog f3"
</pre>
Procedure version is cp*
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cm"></a>
<h3>COPY-MAPPING / CM</h3>

Copy mappings from the current file to another file.
Mappings are referenced by the numbers displayed
by the <B>menu</B> command. If the mapping list is *,
copy all mappings in the current directory.
<p>
Usage: copy-mapping file mapping-list
<p>
Examples:
<pre>
          copy-mapping foo *
          copy-mapping bar 1 5
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_def"></a>
<h3>DEF</h3>

Install an object in the workspace under the given name.
<p>
Usage: def name object
<p>
Example:
<pre>
          def foo m3
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_defv"></a>
<h3>DEFINE-FILE-VARIABLE</h3>

Create a new variable in a PDB file
<p>
Procedure version is define-file-variable*.
<p>
Usage: define-file-variable type name [values]*
<p>
Examples:
<pre>
          define-file-variable double foo 3.42
          define-file-variable double bar[1:50,2] 0.0
          define-file-variable "char *" bar "Hi" "there"
          define-file-variable "int" baz 1
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_desc"></a>
<h3>DESC</h3>

Describe variables or structure members in the current file directory. To
get a description of part of a variable or structure member, qualify the
name with index expressions. (See the <a href="#pdv_syntax">PDBView Syntax</a> section for a discussion
of index expressions.) If a selection pattern is specified in place of a
variable name, all variables matching the pattern will be described. Each
`?' in the pattern matches any single character. Each `*' in the pattern
matches any string of zero or more characters. An optional type qualifier
may also be specified in order restrict the list to a given type.
<p>
Procedure version is desc*.
<p>
Usage:	desc variable | structure-member | pattern [type]
<p>
Examples:
<pre>
          desc Mapping1
          desc dir1/a.b.c[12:14]
          desc * double
          desc var? integer
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_find"></a>
<h3>FIND</h3>

Find and return the indices in an array of numbers which meet the 
criteria specified.
<p>
This command has an implicit form in which the "find" can be omitted.  The
implicit form can be used at the command prompt when the file variable
name does not conflict with a PDBView command name.
<p>
Usage: [find] arr [predicate # [conjunction]]*
<br>
<br>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
            arr&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp :=&nbsp a c-array of values
<br>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
            predicate&nbsp :=&nbsp = | != | <= | < | >= | >
<br>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
            conjuntion :=&nbsp and | or
<p>
Examples:
<pre>
          find x < 1 and > 0.5
          find x > 1 or < 0.5
          x > 0.1 and < 0.75 or > 1.25 and < 1.75
          x < 0.15 or > 0.75 and < 1.25 or > 1.75
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ls"></a>
<h3>LS</h3>

List the names of variables, links, and directories in the current file
directory. Directories have a terminal slash. An optional selection pattern
may be specified. Each `?' in the pattern matches any single character. Each
`*' in the pattern matches any string of zero or more characters. An optional
type qualifier may also be specified in order restrict the list to a given
type.
<p>
Usage:	ls [pattern [type]]
<p>
Examples:
<pre>
          ls dir1/curve*
          ls var? integer
          ls * Directory
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ls_attr"></a>
<h3>LS-ATTR</h3>

List the attributes in the current file.
<p>
Usage: ls-attr
<p>
Examples:
<pre>
          ls-attr foo
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lst"></a>
<h3>LST</h3>

List the labels of mappings in the work space.
A selection pattern may be specified.
<p>
Usage: lst [pattern]
<p>
Examples:
<pre>
          lst
          lst ?*d
          lst *foo*
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ac"></a>
<h3>MAKE-AC-MAPPING-DIRECT / AC</h3>

Construct an arbitrarily connect mapping from component sets.
<p>
Usage: make-ac-mapping-direct ran dom [centering [label]]
<p>
Example:
<pre>
         ac ran dom
         ac ran dom ncent "my label"
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_cps"></a>
<h3>MAKE-CP-SET / CPS</h3>

Construct a set which is the Cartesian product of
a list of sets or arrays.  Unary operations can
optionally be applied to the components.
<p>
Usage: make-cp-set [item]*
      item := var | oper var
<p>
Example:
<pre>
         cps x y z
         cps log10 x y exp z
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lr"></a>
<h3>MAKE-LR-MAPPING-DIRECT / LR</h3>

Construct a logically rectangular mapping from component sets.
<p>
Usage: make-lr-mapping-direct ran dom [centering [existence-map [label]]]
<p>
Example:
<pre>
         lr ran dom zcent nil "some data"
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_lrs"></a>
<h3>MAKE-LR-MAPPING-SYNTH / LRS</h3>

Construct a logically rectangular mapping by synthesis from lower
dimensional datasets.
<p>
Usage: make-lr-mapping-synth ran dom ...
<p>
Example:
<pre>
        lrs '("rng" 0 (td23 td24 td25 td26 td27 td28 td29
                       td30 td31 td32))
            '("dmn" 1 (td0 td0 td0 td0 td0 td0 td0
                       td0 td0 td0))
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_minmax"></a>
<h3>MINMAX</h3>

Print the minimum and maximum values and their offsets from the
beginning of an array in a file.
<p>
Usage: minmax name
<p>
Example:
<pre>
        minmax foo
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_print"></a>
<h3>PRINT</h3>

Print out all or part of the specified variable or structure member. Note
that the command keyword may be omitted. To print part of a variable or
structure member, qualify the name with index expressions. (See the PDBView
Syntax section for a discussion of index expressions).
<p>
This command has an implicit form in which the "print" can be omitted.  The
implicit form can be used at the command prompt when the file variable
name does not conflict with a PDBView command name.
<p>
Usage:	[print] variable | structure-member
<p>
Examples:
<pre>
          print Mapping4.domain.elements
          print dir1/Mapping2.range.elements[1]
          print a.b[3].c[5,10:20,1:8:3]
          Mapping2
          a[5,10:20,1:8:3]
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_pm"></a>
<h3>PRINT-MAPPING / PM</h3>

Print out the specified mappings. Mappings are referenced by the numbers
displayed by the menu command.
<p>
Usage:	print-mapping mapping-list
<p>
Example:
<pre>
          print-mapping 3 4
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_sm"></a>
<h3>SAVE-MAPPING / SM</h3>

Save mappings from the current viewport to a file.
Mappings are referenced by the numbers displayed
by the lsv command. If the mapping list is *,
save all mappings in the current viewport.
<p>
Usage: save-mapping file mapping-list
<p>
Examples:
<pre>
          save-mapping foo *
          save-mapping bar 1 5
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_scv"></a>
<h3>SCALE-FILE-VARIABLE</h3>

Scale variable name by scale_factor and
write it into the current file with name aliasname.
<p>
Usage: scale-file-variable name aliasname scale_factor
<p>
Examples:
<pre>
          scale-file-variable foo two_foo 2.0
          scale-file-variable bar -bar -1.0
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_struct"></a>
<h3>STRUCT</h3>

Describe the specified data type in the current file.
<p>
Usage:	struct data-type
<p>
Examples:
<pre>
          struct double
          struct PM_mapping
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_table"></a>
<h3>TABLE</h3>

Write out all or part of a variable or
member structure to an opened textfile
To write part of a variable or member qualify
the name with index expressions whose parts
are in one of the three forms:
<p>
  index
<p>
  index-min:index-max
<p>
  index-min:index-max:increment
<p>
Only the first form may be used to qualify
variables or terminal structure members with
embedded pointers and non-terminal members.
<p>
Procedure version is table*.
<p>
Usage: table variable | structure-member
<p>
Examples:
<pre>
          table Mapping2
          table Mapping4.domain.elements
          table Mapping2.range.elements[1]
          table a[5,10:20,1:8:3]
          table a.b[3].c[5,10:20,1:8:3]
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_types"></a>
<h3>TYPES</h3>

List the data types in the current file.
<p>
Usage:	types
<p>

<!--------------------------------------------------------------------------> 

<!                           DATA ANALYSIS                                  >

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_analysis"></a>
<h3>Data Analysis</h3>

In a more graphical context, PDBView offers the user many tools for data
analysis.  These include algebraic manipulations, calculus, and a wide
variety of elementary functions (e.g. sin, exp, j0).
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_der"></a>
<h3>DERIVATIVE / DER</h3>

Take simple derivative of scalar function of one variable.
<p>
Usage:	derivative mapping*
<p>
Examples:
<pre>
          der 2
          der 4 6:9
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_int"></a>
<h3>INTEGRATE / INT</h3>

Integrate the specified mapping.
<p>
Usage:	integrate mapping*
<p>
Examples:
<pre>
          integrate 2
          integrate 4 6:9
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_norm"></a>
<h3>NORM</h3>

Take the Euclidean norm of the range values of a mapping.
<p>
Usage:	norm mapping*
<p>
Examples:
<pre>
          norm 2
          norm 4 6:9
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_dfncs"></a>
<h3>DOMAIN FUNCTIONS</h3>

The following functions are appplied to all components of the domain values
of the given mappings:
<pre>
   Absolute value             absx
   Logarithm and Expontional  lnx, log10x, expx
   Trigonometry               cosx, sinx, tanx
                              acosx, asinx, atanx
   Hyperbolic Functions       coshx, sinhx, tanhx
   Bessel Functions           j0x, j1x
                              y0x, y1x
   Others                     sqrx, sqrtx, recipx
</pre>
<p>
Usage:	fnc mapping*
<p>
Examples:
<pre>
          sqrx 2
          lnx (j1x 3)
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_rfncs"></a>
<h3>RANGE FUNCTIONS</h3>

The following functions are appplied to all components of the range values
of the given mappings:
<pre>
   Absolute value             abs
   Logarithm and Expontional  ln,log10,exp
   Trigonometry               cos, sin, tan
                              acos, asin, atan
   Hyperbolic Functions       cosh, sinh, tanh
   Bessel Functions           j0, j1
                              y0, y1
   Tchebyshev Functions       tchn
   Others                     sqr, sqrt, recip, random
</pre>
<p>
Usage:	fnc mapping*
<p>
Examples:
<pre>
          sin 2
          cos (log10 3)
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_binop"></a>
<h3>BINARY OPERATORS</h3>

The following binary functions combine mappings two at a time:
<pre>
   m+      Sum
   m*      Product
   m-      Difference
   m/      Quotient
   hypot   sqrt(a^2 + b^2)
</pre>
The result is a new mapping.
<p>
Usage:	oper [mapping]*
<p>
Examples:
<pre>
          m+ 1 3 5 9
          m/ 2 8 19
          hypot 3 17
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_mathop"></a>
<h3>MATH OPERATORS</h3>

The following functions operate on mappings using auxilliary information:
<pre>

powrx, powx   Raise domain values to a power x = x^a
powax         Raise a to the power of the x value x = a^x

powr, pow     Raise range values to a power y = y^a
powa          Raise a to the power of the y value y = a^y

jn            Nth order Bessel function of the first kind on range values
yn            Nth order Bessel function of the second kind on range values

smooth        Smooth mapping using n point integral to average
xmm           Excerpt part of a mapping

</pre>
These do not result in a new mapping.
<p>
Usage:	oper [info] [mapping]*
<p>
Examples:
<pre>
          powax 3.0 8
          jn 2 2 10 56
          smooth 3  1 18
	  xmm '(1.0 3.0 4.0 18.0) 25
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ssop"></a>
<h3>SHIFT/SCALE OPERATORS</h3>

The following functions shift or scale the specified mappings
<pre>

dx      Shift domain values of curve by a constant
dy      Shift range values of curve by a constant
mx      Scale domain values of curve by a constant
my      Scale range values of curve by a constant
divx    Divide domain values by a constant
divy    Divide range values by a constant

</pre>
These do not result in a new mapping.
<p>
Usage:	oper [info] [mapping]*
<p>
Examples:
<pre>
          dx -2.5 1 5 9
          divy 3.14 18
</pre>

<!--------------------------------------------------------------------------> 

<!                         WORKING WITH PDBVIEW                             >

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_misc"></a>
<h3>Working with PDBView</h3>

In a more graphical context, PDBView offers the user many tools for data
analysis.  These include algebraic manipulations, calculus, and a wide
variety of elementary functions (e.g. sin, exp, j0).
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_command_log"></a>
<h3>COMMAND-LOG</h3>

Turn logging of typed commands on or off. A log file name may be specified
in place of the keyword <B>on</B>. The default log file name is
<B>pdbview.log</B>. If
the log file already exists, it is appended to. Logging is off by default.
<p>
Usage:	command-log [on | off | filename]
<p>
Examples:
<pre>
          command-log on
          command-log mylog2
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_end"></a>
<h3>END / QUIT</h3>

End the session of PDBView.
<p>
Usage:	end
<p>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_format"></a>
<h3>FORMAT</h3>

Set the printing format for a specified data type. If the argument specifying
the type has `1' or `2' appended, then the new format is applied only to
arrays shorter than <B>array-length</B> or to arrays longer than or equal to
<B>array-length</B>, respectively. Otherwise, the format applies to both. Invoking
the format command with the single argument, default, causes the formats
for all types to be reset to their default values.
The single argument, all, causes all the current formats to be listed.
The format argument must
be a Standard C I/O Library format string. Double quotes are only necessary
if the format string contains embedded blanks. See the set command for more
information about the <B>array-length</B> variable. This command overrides the
settings of the decimal-precision and bits-precision display control parameters.
<p>
Usage:	format integer[1 | 2] | long[1 | 2] | float[1 | 2] |
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
double[1 | 2] | short[1 | 2] | char[1 | 2]
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
longlong[1 | 2] | bit[1 | 2] | suppress-member &lt;format&gt;
<br>
Usage:	format default
<br>
Usage:	format all
<p>
Examples:
<pre>
          format double %12.5e
          format double2 %10.2e
          format char "%s"
          format default
          format all
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_help"></a>
<h3>HELP</h3>

Print a list of commands or documentation for an optionally specified command.
<p>
Usage:	help [command]
<p>
Examples:
<pre>
          help
          help menu
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_ld"></a>
<h3>LD</h3>

Read SCHEME forms (e.g. PDBView commands) from the specified ASCII disk
file. The -l execute line option can be used to cause PDBView to read a
file of SCHEME forms at start-up.
<p>
Usage:	ld &lt;filename&gt;
<p>
Example:
<pre>
          ld script.scm
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_mode"></a>
<h3>MODE</h3>

Set the print mode for structures.
<p>
Display modes are:
<pre>
          full-path - the full path name is printed at each branch, e.g. foo.bar[3].baz
          indent - indent 4 spaces at each branch (default)
          tree - display as a tree (lines connecting branches)
</pre>

Type display is controlled by:
<pre>
          no-type - turns off the display of types (default)
          type - displays the type of each item and branch
</pre>

Display of recursive structures is controlled by:
<pre>
          recursive - indent each level of recursive structures
          iterative - number each level of recursive structures (default)
</pre>

Usage:	mode full-path | indent | tree | no-type | type | recursive | iterative
<p>
Example:
<pre>
          mode full-path
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_set"></a>
<h3>SET</h3>

Set the value of some array display parameters.
If the single argument, all, is given show the values
of all parameters.
Parameters:
<pre>
          line-length - the number of array elements per line
          array-length - for arrays shorter than value, label each element individually
          bits-precision - number of mantissa bits of precision
          decimal-precision - number of digits of precision
</pre>

Usage:	set line-length | array-length | bits-precision | decimal-precision value
<br>
Usage:  set all
<p>
Examples:
<pre>
          set line-length 3
          set decimal-precision 6
          set all
</pre>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 

<a name="pv_syn"></a>
<h3>SYNONYM</h3>

Define synonyms for the given function.
<p>
Usage: synonym func [synonym]*
<p>
Examples:
<pre>
          synonym change-file cf
</pre>

<!--------------------------------------------------------------------------> 

<!                             DEFAULT STATE                                >

<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_deflt"></a>
<h3>Default State</h3>


The state in which PDBView starts out is:
<p>
<ul>

<li>mode iterative
<li>mode indent
<li>mode no-type
<li>set array-length 6
<li>set line-length 3
<li>set decimal-precision 4
<li>palette spectrum
<li>command-log off

</ul>

<!--------------------------------------------------------------------------> 
<!--------------------------------------------------------------------------> 
<hr>

<a name="pdv_odoc"></a>
<h3>Other PACT Documents</h3>


Interested readers may wish to refer to the other PACT documents which describe
the data structures and functionality underlying the more common PDB files
upon which PDBView operates. The PANACEA, PGS, and PDBLib manuals are of
special interest to people who wish to generate and view data files with PACT.
<p>


The list of PACT Documents is:

<BLOCKQUOTE>
<a href=pact.html>PACT User&#146;s Guide</a><br><br>
<a href=score.html>SCORE User&#146;s Manual</a><br><br>
<a href=ppc.html>PPC User&#146;s Manual</a><br><br>
<a href=pml.html>PML User&#146;s Manual</a><br><br>
<a href=pdb.html>PDBLib User&#146;s Manual</a><br><br>
<a href=pgs.html>PGS User&#146;s Manual</a><br><br>
<a href=pan.html>PANACEA User&#146;s Manual</a><br><br>
<a href=ultra.html>ULTRA II User&#146;s Manual</a><br><br>
<a href=pdbdiff.html>PDBDiff User&#146;s Manual</a><br><br>
<a href=pdbview.html>PDBView User&#146;s Manual</a><br><br>
<a href=sx.html>SX User&#146;s Manual</a>
</BLOCKQUOTE>

<br>
<hr>
For questions and comments, 
please contact the <A HREF="pactteam.html">PACT Development Team</A>.

<hr>
Last Updated: $MOD
