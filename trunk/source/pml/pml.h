/*
 * PML.H - header support Portable Mathematics Library
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_PML

#define PCK_PML

#include "score.h"
#include "scope_constants.h"

#define TOLERANCE             1.0e-10
#define PM_SPACEDM            3         /* maximum spatial dimension */

#ifndef HYPOT
#define HYPOT hypot
#endif

#ifndef PM_random
#define PM_random PM_random_48
#endif

#define PM_SET_S                PM_gs.tnames[0]
#define PM_SET_P_S              PM_gs.tnames[1]
#define PM_MAP_INFO_S           PM_gs.tnames[2]
#define PM_MAP_INFO_P_S         PM_gs.tnames[3]
#define PM_MAPPING_S            PM_gs.tnames[4]
#define PM_MAPPING_P_S          PM_gs.tnames[5]
#define PM_MESH_TOPOLOGY_S      PM_gs.tnames[6]
#define PM_MESH_TOPOLOGY_P_S    PM_gs.tnames[7]
#define PM_AC_S                 PM_gs.tnames[8]
#define PM_LR_S                 PM_gs.tnames[9]
#define PM_N_TYPES              10

/*--------------------------------------------------------------------------*/

/*                          PROCEDURAL MACROS                               */

/*--------------------------------------------------------------------------*/

/* PM_ARRAY_CONTENTS - extract the contents of a C_array
 *                   - NOTE: _TN must be an existing char array
 */

#define PM_ARRAY_CONTENTS(_a, _ty, _n, _tn, _d)                              \
    {char *_tl;                                                              \
     (_n) = (_a)->length;                                                    \
     (_d) = ((_ty *) (_a)->data);                                            \
     _tl  = _tn;                                                             \
     if (_tl != NULL)                                                        \
        {strcpy(_tl, (_a)->type);                                            \
         SC_dereference(_tl);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SAME - return TRUE if the points are too close */

#define PM_SAME(x1, y1, x2, y2)                                              \
    ((ABS(x1-x2) < TOLERANCE) && (ABS(y1-y2) < TOLERANCE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PM_CLOSETO_FIX(_ok, _x, _y)     (_ok = ((_x) != (_y)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PM_CLOSETO_FLOAT(_ok, _x, _y, _tol)                                  \
   {long double _del;                                                        \
    _del = (2.0*(_x - _y)/(ABS(_x) + ABS(_y) + SMALL));                      \
    _ok  = ((_del < -_tol) || (_tol < _del));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CLOSETO_ABS - return TRUE if the numbers are close enough */

#define PM_CLOSETO_ABS(x1, x2)                                               \
    ((ABS(x1-x2) < TOLERANCE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CLOSETO_REL - return TRUE if the numbers are close enough */

#define PM_CLOSETO_REL(x1, x2)                                               \
    ((2.0*ABS((x1)-(x2))/(ABS(x1)+ABS(x2)+SMALL) < TOLERANCE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LESS_LESS - return TRUE if |x| << |y| as defined by TOLERANCE
 *              - NOTE: this breaks down if |y| ~ SMALL
 */

#define PM_LESS_LESS(x, y) (ABS(x)/(ABS(y) + SMALL) < TOLERANCE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ELEMENT - access a matrix element */

#define PM_element(mp, r, c) (*((mp->array) + (r-1)*(mp->ncol) + c-1))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LOGICAL_ZONE - define a template for logical rectangular meshes */

#define PM_LOGICAL_ZONE(b, b1, b2, b3, b4, kx)                               \
   {b1 = b + 1;                                                              \
    b2 = b1 + kx;                                                            \
    b3 = b2 - 1;                                                             \
    b4 = b;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERP - interpolate between a pair of points */

#define PM_interp(_rv, _xp, _x0, _y0, _x1, _y1)                              \
   {double _xta, _xtb, _xtc, _yta, _ytb;                                     \
    _xta = _x0;                                                              \
    _xtb = _x1;                                                              \
    _xtc = _xp;                                                              \
    _yta = _y0;                                                              \
    _ytb = _y1;                                                              \
    if (_xta != _xtb)                                                        \
       _rv = _yta + (_ytb - _yta)*(_xtc - _xta)/(_xtb - _xta);               \
    else                                                                     \
       _rv = 0.5*(_yta + _ytb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PM_DELTA_CROSS_2D(x1, y1, x2, y2, x3, y3)                            \
    (((x2)-(x1))*((y3)-(y1)) - ((x3)-(x1))*((y2)-(y1)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PM_DELTA_DOT_2D(x1, y1, x2, y2, x3, y3)                              \
    (((x2)-(x1))*((x3)-(x1)) + ((y2)-(y1))*((y3)-(y1)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VOL_TRI_CYL - compute the volume of a triangle of rotation by
 *                - 2*PI about the Z axis
 */

#define PM_VOL_TRI_CYL(_v, a, b, c)                                          \
    PM_VOL_TRI_CYLR(_v, z ## a, r ## a, z ## b, r ## b, z ## c, r ## c)

#define PM_VOL_TRI_CYLR(_v, _z1, _r1, _z2, _r2, _z3, _r3)                    \
   {double _ra;                                                              \
    _ra = (_r1 + _r2 + _r3)/3.0;                                             \
    _ra = ABS(_ra);                                                          \
    _v  = PI*_ra*((_z1 - _z2)*(_r3 - _r2) - (_r1 - _r2)*(_z3 - _z2));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VOLTET - compute the volume of a tetrahedron */

#define PM_VOLTET(a, b, c, d)                                                \
   PM_VOLTETR(x ## a, y ## a, z ## a, x ## b, y ## b, z ## b,                \
              x ## c, y ## c, z ## c, x ## d, y ## d, z ## d)

#define PM_VOLTETR(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4)           \
    0.16666666667*((x2-x1)*((y3-y1)*(z4-z1) - (y4-y1)*(z3-z1)) +             \
		   (y2-y1)*((z3-z1)*(x4-x1) - (z4-z1)*(x3-x1)) +             \
		   (z2-z1)*((x3-x1)*(y4-y1) - (x4-x1)*(y3-y1)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DAX - compute the volume contribution of a face of a tetrahedron
 *        - NOTE: use Green's theorem to integrate dV*div(X) - this
 *        - gives a sum of dA.X terms - (X1 x X3).(X4 - X2)
 */

#define PM_DAX(a, b, c, d)                                                   \
   PM_DAXR(x ## a, y ## a, z ## a, x ## b, y ## b, z ## b,                   \
           x ## c, y ## c, z ## c, x ## d, y ## d, z ## d)

#define PM_DAXR(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4)              \
    0.16666666667*((z1*y3 - y1*z3)*(x4-x2) +                                 \
		   (x1*z3 - z1*x3)*(y4-y2) +                                 \
                   (y1*x3 - x1*y3)*(z4-z2))

/*--------------------------------------------------------------------------*/

#define PM_copy_point(_nd, _xd, _xs)                                         \
    {int _ld;                                                                \
     for (_ld = 0; _ld < _nd; _ld++)                                         \
         _xd[_ld] = _xs[_ld];}

#define PM_vector_get_point(_nd, _xd, _vs, _is)                              \
    {int _ld;                                                                \
     for (_ld = 0; _ld < _nd; _ld++)                                         \
         _xd[_ld] = _vs[_ld][_is];}

#define PM_vector_put_point(_nd, _xs, _vd, _i)                               \
    {int _ld;                                                                \
     for (_ld = 0; _ld < _nd; _ld++)                                         \
         _vd[_ld][_i] = _xs[_ld];}

#define PM_vector_copy_point(_nd, _xd, _id, _xs, _is)                        \
    {int _ld;                                                                \
     for (_ld = 0; _ld < _nd; _ld++)                                         \
         _xd[_ld][_id] = _xs[_ld][_is];}

#define PM_polygon_get_point(_xd, _ps, _i)                                   \
    {int _ld;                                                                \
     for (_ld = 0; _ld < _ps->nd; _ld++)                                     \
         _xd[_ld] = _ps->x[_ld][_i];}

#define PM_polygon_put_point(_xs, _pd, _i)                                   \
    {int _ld;                                                                \
     for (_ld = 0; _ld < _pd->nd; _ld++)                                     \
         _pd->x[_ld][_i] = _xs[_ld];}

#define PM_set_value  PM_array_set

/*--------------------------------------------------------------------------*/

/*                         STRUCT DEFINITIONS                               */

/*--------------------------------------------------------------------------*/

typedef struct s_PM_lagrangian_mesh PM_lagrangian_mesh;
typedef struct s_PM_sp_lin_sys PM_sp_lin_sys;
typedef struct s_coefficient coefficient;
typedef struct s_C_array C_array;
typedef struct s_PM_polygon PM_polygon;
typedef struct s_PM_matrix PM_matrix;
typedef struct s_PM_field PM_field;
typedef struct s_PM_conic_curve PM_conic_curve;
typedef struct s_PM_end_point PM_end_point;
typedef struct s_PM_side PM_side;
typedef struct s_PM_part PM_part;
typedef struct s_PM_mesh PM_mesh;
typedef struct s_PM_mesh_topology PM_mesh_topology;
typedef struct s_PM_mesh_map PM_mesh_map;
typedef struct s_PM_set PM_set;
typedef struct s_PM_map_info PM_map_info;
typedef struct s_PM_mapping PM_mapping;
typedef struct s_PM_scope_public PM_scope_public;
typedef struct s_sort_link sort_link;

FUNCTION_POINTER(PM_set, *(*PFPPM_set));
FUNCTION_POINTER(PM_mapping, *(*PFPPM_mapping));

enum e_PM_binary_operation
   {PM_PLUS = 1, PM_MINUS, PM_TIMES, PM_DIVIDE,
    PM_INTERSECT, PM_UNION};

typedef enum e_PM_binary_operation PM_binary_operation;

/*
 * #bind derived PM_centering integer SC_ENUM_I SC_ENUM_I U_CENT
 */

enum e_PM_centering
   {Z_CENT = -1,                                      /* zone centered */
    N_CENT = -2,                                      /* node centered */
    F_CENT = -3,                                      /* face centered */
    E_CENT = -4,                                      /* edge centered */
    U_CENT = -5};                                     /* uncentered */

typedef enum e_PM_centering PM_centering;

struct s_PM_lagrangian_mesh
   {double *x;
    double *y;
    int kmax;
    int lmax;};

struct s_coefficient
   {int n_points;
    int *indexes;
    double *weights;};

/* PM_SP_LIN_SYS - description of sparse linear system structure
 *
 *   ilo  - low row index
 *
 *   ihi  - high row index
 * 
 *   n_ups - number of unknowns per linear system
 *   n_rhs - number of RHS to solve with
 * 
 *   j    - pointer to an array of SC_arrays, one SC_array
 *          corresponding to each row of the sparse linear system, each
 *          SC_array containing indices of the nonzero columns in
 *          the row
 *
 *   aj   - pointer to an array of SC_arrays, one SC_array
 *          corresponding to each row of the sparse linear system, each
 *          SC_array containing a matrix coefficient in ordered
 *          one-to-one correspondence with the column indices of *jj
 *
 *   b    - pointer to arrays of doubles containing for instance the right-
 *          hand side of the linear system, each double accessed by a row index
 *
 *   x    - pointer to arrays of doubles containing for instance the initial
 *        - guess, each double accessed by a row index
 */

struct s_PM_sp_lin_sys
   {int ilo;
    int ihi;
    int n_ups;
    int n_rhs;
    int symmetric;
    int transpose;
    SC_array *j;
    SC_array *aj;
    SC_array *jt;
    SC_array *atj;
    int *diagonal;
    double **b;
    double **x;
    PM_sp_lin_sys *next;
    void (*pre)(PM_sp_lin_sys *sls);
    void (*cmp_Ap)(double *Ax, PM_sp_lin_sys *sls, double *x);
    void (*cmp_Lr)(double *Lr, PM_sp_lin_sys *sls, double *r);};

/* C_ARRAY - define an encapulated vector/array type
 *         - this is PD_defstr'd in PANACE.C changes must be reflected there!!!
 *         - NOTE: the TYPE member is used for a PD_cast and as such must be
 *         - "T *" if the data is of type T
 */

struct s_C_array
   {char *type;                                                /* data type */
    long length;                                      /* number of elements */
    void *data;};                                    /* pointer to the data */


/* PM_MATRIX - specify a 2d rank matrix */

struct s_PM_matrix
   {int nrow;
    int ncol;
    double *array;};


/* PM_POLYGON - specify a 1d piecewise linear curve thru N space */

struct s_PM_polygon
   {int nd;                             /* number of dimensions of a point */
    int np;                               /* size of the coordinate arrays */
    int nn;                                  /* number of nodes in polygon */
    double **x;};


/* FIELD - specify the operations which define the field for a set
 *       - depending on which members are NULL one is left with an
 *       - algebra, a ring, a group, etc
 */

struct s_PM_field
   {PFByte add;
    PFByte sub;
    PFByte scalar_mult;
    PFByte mult;
    PFByte div;};

/* PM_CONIC_CURVE - quadratic line descriptor
 *                - this is PD_defstr'd in PDBX.C
 */

struct s_PM_conic_curve
   {char *type;
    double xx;
    double xy;
    double yy;
    double x;
    double y;
    double c;};


/* PM_END_POINT - line segment end point for mesh generation
 *              - this is PD_defstr'd in PDBX.C
 */

struct s_PM_end_point
   {double rn;
    double rx;
    int k;
    int dk;
    double rat;
    double drn;
    double drx;
    PM_end_point *next;};


/* PM_SIDE - mesh patch edge descriptor
 *         - this is PD_defstr'd in PDBX.C
 */

struct s_PM_side
   {double x;
    double y;
    double ratio;
    int dk;
    int dl;
    int k;
    int l;
    double side_rat;
    double min_dr_f;
    double max_dr_f;
    double mag_start;
    double exp_start;
    double mag_end;
    double exp_end;
    int fill;
    double scale;
    double cosine;
    double sine;
    double c0;
    double c1;
    double c2;
    double c3;
    int dir;
    PM_conic_curve *crve;
    PM_side *match;
    PM_side *next;};


/* PM_PART - mesh patch part descriptor
 *         - this is PD_defstr'd in PDBX.C
 */

struct s_PM_part
   {int n_sides;
    PM_side *leg;
    PM_end_point *ends;
    void *comp;
    char *name;
    int reg;
    double k_ratio;
    double k_mag_start;
    double k_exp_start;
    double k_mag_end;
    double k_exp_end;
    double l_ratio;
    double l_mag_start;
    double l_exp_start;
    double l_mag_end;
    double l_exp_end;
    PM_part *next;};


/* PM_MESH - mesh patch descriptor */

struct s_PM_mesh
   {int kmin;
    int kmax;
    int lmin;
    int lmax;
    int n_zones;
    int n_nodes;
    int frn;
    int lrn;
    int frz;
    int lrz;
    int *reg_map;
    double *zone;
    double *nodet;
    double *rx;
    double *ry;
    double *kra;
    double *lra;
    double *apk;
    double *apl;};


/* MESH_CELL - description of a cell in a mesh topology hierarchy
 *           - the cell information is kept as a contiguous block of
 *           - long integer values (n_bound_params) in an array
 *           - describing a list of n dimensional cells which are either
 *           - the highest dimensional units or boundary elements of (n+1)
 *           - dimensional cells.
 *           -
 *           - Different simulations want more or less information for
 *           - optimization purposes, but it is all integer and
 *           - accompanying the list of cells, is a number specifying
 *           - the number of values used to describe the cells. These
 *           - values are listed here in order of importance from
 *           - mandatory to least likely to be used:
 *           -    BND_CELL_MIN       beg block of bounding (N-1)-cells
 *           -    BND_CELL_MAX       end block of bounding (N-1)-cells
 *           -    OPPOSITE_CELL      N-cell which has opposite orientation
 *           -    PARENT_CELL        (N+1)-cell of whose bnd this is a part
 *           -    NGB_CELL           (N+1)-cell parent neighbor sharing this
 *           -    CENTER_CELL        0-cell which is center of this cell
 *           -    DEC_CELL_MIN       beg of block of decomposition N-cells
 *           -    DEC_CELL_MAX       end of block of decomposition N-cells
 *           - Acceptable values for number of values are: 
 *           -    BND_CELL_MAX, OPPOSITE_CELL, PARENT_CELL, NGB_CELL,
 *           -    CENTER_CELL, and DEC_CELL_MAX
 *           -
 *           - There is redundancy in this information. It is permitted
 *           - for computational efficiency, however. This is especially
 *           - useful since an application can use the minimal
 *           - representation and avoid the redundancy.
 *           -
 *           - for the bottom of the hierarchy the BND_CELL_MIN and
 *           - BND_CELL_MAX indices refer to node indices not a contiguous
 *           - block of MESH_CELL value indices in another level of the
 *           - heirarchy
 */

#define BND_CELL_MIN  0
#define BND_CELL_MAX  1
#define OPPOSITE_CELL 2
#define PARENT_CELL   3
#define NGB_CELL      4
#define CENTER_CELL   5
#define DEC_CELL_MIN  6
#define DEC_CELL_MAX  7

/* MESH_TOPOLOGY - attempt to describe in complete generality a
 *               - mesh of points and their neighbors as a heirarchy
 *               - in which the highest dimensional volumes sit at
 *               - the top and the line segment descriptions sit
 *               - at the bottom.
 *               - A place is kept for the nodes and is in fact used
 *               - to record the number of nodes in the mesh. This
 *               - is the level 0 of the heirarchy.
 *               - At each level in the heirarchy an n-dimensional
 *               - surface is described by a contiguous block of
 *               - (n-1)-dimensional boundary sections (next lower
 *               - down in the heirarchy.
 *               -
 *               - assuming the dimensionality to be n the heirarchy
 *               - goes like:
 *               - boundaries[n]   - the description for the n-dim surfaces
 *               - boundaries[n-1] - the description for the (n-1)-dim surfaces
 *               -            .
 *               -            .
 *               -            .
 *               -
 *               - boundaries[1]   - the description for the 1-dim surfaces
 *               -                 - i.e. line segments whose endpoints
 *               -                 - are the node indices
 *               - boundaries[0]   - a mimimal description for the nodes
 */

struct s_PM_mesh_topology
   {int n_dimensions;              /* number of dimensions */
    int *n_bound_params;           /* number of bound params at each level */
    int *n_cells;                  /* number of cells at each level */
    long **boundaries;};           /* boundary info array at each level */


/* SET - attempt to describe a usefully general mathematical set for
 *     - purposes of data I/O (PDBLib), graphics (PGS), data
 *     - analysis (ULTRA and SX)
 *     -
 *     -    name           - every set should have a unique identifier so
 *     -                   - that large identical sets don't happen
 *     -    element_type   - the type of elements in the set so that sets are
 *     -                   - self-describing
 *     -    dimension      - the dimensionality of the set
 *     -    max_index      - an array of the maximum index along each
 *     -                   - dimension such that the product of max_index
 *     -                   - should equal dimension
 *     -    dimension_elem - the dimensionality of an element
 *     -                   - this permits things such as 3 vectors on 2 spheres   
 *     -    n_elements     - the number of elements in the set
 *     -    elements       - a pointer to the actual elements of the set
 *     -    es_type        - the type of the extrema and the scales arrays
 *     -                   - typically, element_type is "double **" while
 *     -                   - es_type is "double *"
 *     -    extrema        - an array of minimum and maximum values,
 *     -                   - respectively, for each component
 *     -                   - extrema is 2*dimension_elem long
 *     -    scales         - an array of scales for each dimension
 *     -                   - which is used in some mathematical operations
 *     -                   - such as interpolation
 *     -                   - NOTE: it is unclear what this means if
 *     -                   -       dimension != dimension_elem
 *     -    opers          - specifies the field for the set
 *     -    metric         - a metric if the set has or needs one
 *     -    symmetry_type  - a string to identify the type of the symmetries
 *     -                   - member
 *     -    symmetry       - a pointer to some object (struct or function)
 *     -                   - describing the symmetries of the set
 *     -    topology_type  - a string to identify the type of the topology
 *     -                   - member
 *     -    topology       - a pointer to some object describing the topology
 *     -                   - of the set e.g. connectivity and ordering
 *     -    info_type      - a string to identify the type of the info
 *     -                   - member
 *     -    info           - a pointer to some object containing any other
 *     -                   - information about the set
 *     -
 *     - Assume that for anything after elements which is NULL that a set
 *     - is a part of flat, singly connected Euclidean N space over the
 *     - field of reals (doubles) and has a logical mesh structure 
 *
 * #bind derived PM_set character-A G_SET PyObject NULL
 */

struct s_PM_set
   {char *name;
    char *element_type;
    int dimension;
    int *max_index;
    int dimension_elem;
    long n_elements;
    void *elements;
    char *es_type;
    void *extrema;
    void *scales;
    PM_field *opers;
    double *metric;
    char *symmetry_type;                         /* describe any symmetries */
    void *symmetry;
    char *topology_type;              /* describe ordering and connectivity */
    void *topology;
    char *info_type;                  /* provide any additional information */
    void *info;
    struct s_PM_set *next;};


/* PM_MAP_INFO - should tell how two sets (domain and range) are to
 *             -  be combined for purposes of manipulation and graphics
 *             - this information should help to interpret the range as the
 *             - image of the domain and all such issues
 *             - as disparate sizes (injections, surjections, and 
 *             - bijections) should be dealt with.
 *             - at the moment the pressing example to be dealt with is that
 *             - of centering
 */

struct s_PM_map_info
   {char *name;
    int centering;};


/* PM_MAPPING - describe mapping of domain set to range set
 *
 * #bind derived PM_mapping character-A G_MAPPING PyObject NULL
 */

struct s_PM_mapping
   {char *name;                                       /* the mapping's name */
    char *category;                /* the category to which mapping belongs */
    PM_set *domain;                                 /* the mapping's domain */
    PM_set *range;             /* a subset of the image of DOMAIN under MAP */
    char *map_type;             /* describe the data type pointed to by map */
    void *map;         /* describe the mapping between the DOMAIN and RANGE */
    int file_type;                          /* file type ASCII, BINARY, PDB */
    void *file_info;           /* file info - cast to some struct with info */
    char *file;                                    /* file name for mapping */
    struct s_PM_mapping *next;};

struct s_PM_mesh_map
   {int nd;          /* number of dimensions */
    int nl;          /* number of cells in any linear dimension */
    int nc;          /* number of cells */
    long *indx;      /* index array */
    int *dim;        /* mesh dimensions (ND) */
    double **r;      /* array of coordinate arrays (ND) */
    double *dr;      /* cell size in each dimension (ND) */
    double *rmin;    /* minimum value in each dimension (ND) */
    char *emap;};    /* existence map of the mesh */

struct s_sort_link
   {int count;
    struct s_sort_link *next;};

struct s_PM_scope_public
   {

/* non-zero initial value variables */
    PM_field *fp_opers;
    PM_field *int_opers;
    PM_field *long_opers;

    char *tnames[PM_N_TYPES];

/* zero initial value variables */
    char error[MAXLINE];};

    
#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern PM_scope_public
 PM_gs;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* MLCG.C declarations */

extern PM_sp_lin_sys
 *PM_mk_sp_lin_sys(int sz, int n_rhs, int n_ods, int sym, int trans,
		   void (*pre)(PM_sp_lin_sys *sls),
		   void (*clr)(double *Lr, PM_sp_lin_sys *sls, double *r));

extern void
 PM_iccg_pre(PM_sp_lin_sys *sls),
 PM_iccg_cmp_Lr(double *Lr, PM_sp_lin_sys *sls, double *r),
 PM_set_sls_coef(PM_sp_lin_sys *sls, int i, int j, double a),
 PM_rl_sp_lin_sys(PM_sp_lin_sys *sls);

extern int
 PM_sls_sym_chk(PM_sp_lin_sys *sls),
 PM_sls_pat_chk(PM_sp_lin_sys *sls),
 PM_sls_col_chk(PM_sp_lin_sys *sls),
 PM_sls_b_chk(PM_sp_lin_sys *sls),
 PM_dsbicg(PM_sp_lin_sys *sls, int no, int *pit, double *ptol),
 PM_dscg(PM_sp_lin_sys *sls, int no, int *pit, double *ptol);


/* MLCHKSM.C declarations */

extern long
 PM_checksum_array(void *arr, int ni, int bpi, int nbts),
 PM_checksum_file(FILE* file, int64_t start, int64_t stop, int nbts);

extern void
 PM_checksum(long *psum, int *prmdr, void *parr, int boff,
	     int ni, int bpi, int nbts),
 PM_md5_checksum_array(void *arr, uint64_t ni, uint64_t bpi, unsigned char *dig),
 PM_md5_checksum_file(FILE* file, int64_t start, int64_t stop, 
                      unsigned char *dig);


/* MLCMSH.C declarations */

extern PM_mesh
 *PM_make_lr_mesh(PM_part *parts, hasharr *curves,
		  int strategy, int method, int constr,
		  double dspat, double drat, double orient);

extern PM_conic_curve
 *PM_mk_cline(char *type, double axx, double ayy, double axy,
	      double ax, double ay, double ac);

extern PM_side
 *PM_mk_side(PM_conic_curve *cur, int dir, double x, double y,
	     int dk, int dl, double krat, double lrat,
	     double srat, double drn, double drx,
	     double ms, double es, double me, double ee,
	     PM_side *match, PM_side *next);

extern PM_part
 *PM_mk_part(char *name, int rg, int nsides,
	     PM_side *leg, PM_end_point *ends,
	     void *comp, double kms, double kes,
	     double kme, double kee, double krat,
	     double lms, double les, double lme,
	     double lee, double lrat, PM_part *next);

extern PM_end_point
 *PM_mk_endpt(double rn, double rx, int dk,
	      double rat, double drn, double drx,
	      PM_end_point *nxt);

extern int
 PM_reg_base(PM_part *parts, PM_side *base);


/* MLEIGEN.C declarations */

extern PM_matrix
 *PM_eigenvalue(PM_matrix *a),
 *PM_eigensys(PM_matrix *a);

extern int
 PM_eigenvectors(PM_matrix *a);


/* MLFFT.C declarations */

extern double
 *PM_uniform_real_x(int no, double xmin, double xmax, int flag),
 *PM_uniform_real_y(int no, double *xo, int ni, double *xi, double *yi);

extern int
 PM_near_power(int n, int a),
 PM_next_exp_two(int n),
 PM_next_power_two(int n),
 PM_fft_sc_real_simul(double *fx1, double *fx2, double *fw1, double *fw2, int n),
 PM_convolve(double *gx, double *gy, int gn, double *hx, double *hy, int hn,
	     double dt, double **pxr, double **pyr, int *pnr),
 PM_convolve_logical(double *g, int n, double *h, int m, int isign, double *cnv);


/* MLFPE.C declarations */

extern void
 *PM_save_fpu(void),
 PM_restore_fpu(void *a),
 PM_detect_fpu(unsigned int *pf, unsigned int *pm),
 PM_enable_fpe_n(int flg, PFSignal_handler hnd, void *a),
 PM_enable_fpe(int flg, PFSignal_handler hand),
 PM_clear_fpu(void);

extern int
 PM_fpu_status(int pfl),
 PM_fpe_traps(int pfl),
 PM_fp_typef(float f),
 PM_fp_typed(double d);

extern long
 PM_fix_nand(long n, double *d, int mask, double v),
 PM_count_nand(long n, double *d, int mask),
 PM_fix_nanf(long n, float *f, int mask, float v),
 PM_count_nanf(long n, float *f, int mask);


/* MLGEOM.C declarations */

extern PM_polygon
 *PM_polygon_get(SC_array *a, int n),
 *PM_convex_hull(double *p1, double *p2, int nh);

extern SC_array
 *PM_polygon_array(void),
 *PM_intersect_polygons(SC_array *a, PM_polygon *pa, PM_polygon *pb),
 *PM_union_polygons(SC_array *a, PM_polygon *pa, PM_polygon *pb);

extern int
 PM_cross(double *x1, double *x2, double *x3, double *x4, double *x0),
 PM_cross_seg(double *x1, double *x2, double *x3, double *x4, double *x0),
 PM_cross_line_plane(double *x1, double *x2,
		     double **px, double *x0, int line),
 PM_colinear_nd(int nd, int n, double **x),
 PM_contains_nd(double *xc, PM_polygon *py),
 PM_intersect_line_polygon(int *pni, double ***pxi, int **psides,
			   double *x1, double *x2, PM_polygon *py, int wh),
 PM_polygon_orient(PM_polygon *p);

extern void
 PM_polygon_push(SC_array *a, PM_polygon *py),
 PM_free_polygons(SC_array *a, int rl),
 PM_orient_polygon(PM_polygon *p),
 PM_nearest_point(PM_polygon *py, double *xs, double *xt, int *pi),
 PM_compute_quad_rot_volume(double *vol, int nc, int *indx, double **x),
 PM_compute_hex_volume(double *vol, int nc, int *indx, double **x),
 PM_vector_extrema(int nd, int n, double **x, double *extr),
 PM_vector_select_extrema(int nd, int n, double **x,
			  char *map, double *extr),
 PM_scale_vectors(int nd, int n, double **x, double *s),
 PM_translate_vectors(int nd, int n, double **x, double *d),
 PM_rotate_vectors(int nd, int n, double **x, double *x0, double *a);

extern double
 PM_distance(int nd, double *x1, double *x2, double *g),
 PM_polygon_area(PM_polygon *py),
 **PM_convert_vectors(int nd, int n, void *v, char *typ);


/* MLICCG.C declarations */

extern double
 PM_iccg(int km, int lm, double eps, int ks, int maxit,
	 double *a0, double *a1, double *b0, double *b1, double *bm1,
	 double *x, double *y),
 PM_dot(double *x, double *y, int n);


/* MLINTM.C declarations */

extern double
 **PM_interpolate_mapping_id(PM_mapping *dest, PM_mapping *source,
			     int wgtfl, double *prm),
 **PM_interpolate_mapping_mq(PM_mapping *dest, PM_mapping *source,
			     int wgtfl, double *prm),
 **PM_interpolate_mapping(PM_mapping *dest, PM_mapping *source,
			  int wgtfl, double *prm);

extern int
 PM_interp_mesh_id(int nd, int nf, int ni, double **xi, double **fi,
		   int *mxo, double **xo, double **fo,
		   double *prm),
 PM_interp_mesh_mq(int nd, int nf, int ni, double **xi, double **fi,
		   int *mxo, double **xo, double **fo,
		   double *prm);


/* MLINTP.C declarations */

extern PM_mapping
 *PM_node_centered_mapping(PM_mapping *s);

extern void
 *PM_connectivity(PM_mapping *f),
 PM_zone_centered_mesh_2d(double **px, double **py, double *rx, double *ry,
			  int kmax, int lmax);

extern double
 *PM_z_n_lr_2d(double *f, double *x, double *y, int mode,
	       void *cnnct, pcons *alist),
 *PM_z_n_ac_2d(double *f, double *x, double *y, int mode,
	       void *cnnct, pcons *alist),
 *PM_zone_node_lr_2d(double *f, void *cnnct, pcons *alist),
 *PM_zone_node_ac_2d(double *f, void *cnnct, pcons *alist),
 *PM_node_zone_lr_2d(double *f, void *cnnct, pcons *alist),
 *PM_node_zone_ac_2d(double *f, void *cnnct, pcons *alist),
 **PM_interpol(PM_lagrangian_mesh *grid, double **pts,
	       int n_pts, double **fncs, int n_fncs),
 PM_interpolate_value(coefficient *vertices, double *f);

extern int
 PM_inside(double x, double y, double *px, double *py, int *map, int n),
 PM_inside_fix(int x, int y, int *px, int *py, int n, int direct),
 PM_find_vertices(double x, double y,
		  PM_lagrangian_mesh *grid, coefficient *vertices),
 PM_find_coefficients(double x, double y,
		      PM_lagrangian_mesh *grid, coefficient *vertices);

extern coefficient
 *PM_alloc_vertices(PM_lagrangian_mesh *grid);


/* MLINTS.C declarations */

extern double
 *PM_compute_splines(double *x, double *y, int n, double dyl, double dyr),
 PM_cubic_spline_int(double *x, double *f, double *df, int n, double xc),
 PM_linear_int(double *x, double *f, int n, double xc);


/* MLLSQ.C declarations */

extern double
 *PM_lsq_fit(int nd, int n, double **x, double *dextr, int ord),
 **PM_lsq_polynomial(int n, int ord, double *cf, double *dextr);


/* MLMATH.C declarations */

extern void
 PM_stats_mean(int n, double *x, double *pmn, double *pmdn,
	       double *pmod, double *pstd),
 PM_derivative(int n, double *fncx, double *fncy,
	       double *derx, double *dery),
 PM_array_scale(double *p, int n, double f),
 PM_array_shift(double *p, int n, double f),
 PM_array_set(double *p, int n, double f),
 PM_array_copy(double *s, double *t, int n),
 PM_err(char *fmt, ...);

extern int
 PM_del_col(PM_matrix *a, long *col, long ncol),
 PM_array_equal(double *s, double *t, int n, double tol),
 PM_set_opers(PM_set *set),
 PM_thin_1d_der(int n, double *fncx, double *fncy, double *thnx, double *thny,
		double toler),
 PM_thin_1d_int(int n, double *fncx, double *fncy, double *thnx, double *thny,
		double toler),
 PM_filter_coeff(double *y, int n, double *coef, int nc),
 PM_smooth_int_ave(double *x, double *y, int n, int pts),
 PM_value_equal(double x1, double x2, double tol),
 PM_value_compare(double x1, double x2, double tol);

#ifdef HAVE_ANSI_FLOAT16

extern int
 PM_qvalue_compare(long double x1, long double x2, long double tol);

#endif

extern double
 PM_romberg(double (*func)(double x), double x0, double x1, double tol),
 PM_integrate_tzr(double xmn, double xmx, int *pn,
		  double *fncx, double *fncy, double *intx, double *inty),
 PM_curve_len_2d(double *x, double *y, int n),
 PM_curve_len_3d(double *x, double *y, double *z, int n),
 PM_machine_precision(void);


/* MLMATR.C declarations */

extern PM_matrix
 *PM_create(int nrow, int ncol),
 *PM_transpose(PM_matrix *a),
 *PM_times(PM_matrix *a, PM_matrix *b),
 *PM_plus(PM_matrix *a, PM_matrix *b),
 *PM_minus(PM_matrix *a, PM_matrix *b),
 *PM_zero(PM_matrix *a),
 *PM_ident(PM_matrix *a),
 *PM_copy(PM_matrix *to, PM_matrix *from),
 *PM_print(PM_matrix *a);

extern double
 *PM_matrix_done(PM_matrix *mp);

extern int
 PM_is_zero(PM_matrix *a),
 PM_destroy(PM_matrix *mp),
 PM_sort_on_col(PM_matrix *a, int col);


/* MLMESH.C declarations */

extern void
 PM_mesh_part(double *rx, double *ry, double *nodet,
	      int *reg_map, int n, int k, int l,
	      int kmn, int kmx, int lmn, int lmx, 
	      int kmax, int lmax, int m,
	      double kr, double lr,
	      double *kra, double *lra, double *apk, double *apl,
	      double ask, double xsk, double aek, double xek,
	      double asl, double xsl, double ael, double xel,
	      int strategy, int method, int constr,
	      double dspat, double drat, double orient);


/* MLMM.C declarations */

extern C_array
 *PM_make_array(char *type, long size, void *data);

extern PM_set
 *PM_make_set_alt(char *name, char *type, int cp, int nd,
		  int *diml, int nde, void **elml),
 *PM_make_set(char *name, char *type, int rl,
	      int nd, ...),
 *PM_make_ac_set(char *name, char *type, int cp,
		 PM_mesh_topology *mt, int nde, ...),
 *PM_copy_set(PM_set *s),
 *PM_make_lr_index_domain(char *name, char *type,
			  int nd, int nde, int *maxes,
			  double *extrema, double *ratio),
 *PM_make_lr_cp_domain(char *name, char *type, int nd,
		       PM_set **sets),
 *PM_mk_set(char *name, char *type, int cp, long ne,
	    int nd, int nde, int *maxes, void *elem,
	    PM_field *opers, double *metric,
	    char *symtype, void *sym,
	    char *toptype, void *top,
	    char *inftype, void *inf,
	    PM_set *next);

extern PM_mapping
 *PM_make_mapping(char *name, char *cat, PM_set *domain, PM_set *range,
		  PM_centering centering, PM_mapping *next),
 *PM_build_grotrian(char *name, char *type, int cp,
		    void *xv, void *yv, char **labels, int n_s,
		    void *tr, void *up, void *low, int n_tr);

extern PM_mesh_topology
 *PM_make_topology(int nd, int *bnp, int *bnc, long **bnd),
 *PM_copy_topology(PM_mesh_topology *mt),
 *PM_lr_ac_mesh_2d(double **px, double **py, int kmax, int lmax,
		   int k1, int k2, int l1, int l2, int ord);

extern PM_polygon
 *PM_init_polygon(int nd, int n),
 *PM_make_polygon(int nd, int n, ...),
 *PM_polygon_box(int nd, double *bx),
 *PM_copy_polygon(PM_polygon *py);

extern void
 PM_init_vectors(int nd, int n, double **x, double v),
 PM_free_vectors(int nd, double **x),
 PM_free_polygon(PM_polygon *py),
 PM_polygon_copy_points(PM_polygon *pa, PM_polygon *pb),
 PM_promote_array(C_array *a, char *ntyp, int flag),
 PM_promote_set(PM_set *s, char *ntyp, int flag),
 PM_find_exist_extrema(PM_set *s, char *typ, void *em),
 PM_find_extrema(PM_set *s),
 PM_rel_array(C_array *arr),
 PM_rel_set(PM_set *set, int mfl),
 PM_rel_topology(PM_mesh_topology *mt),
 PM_rel_mapping(PM_mapping *f, int rld, int rlr),
 PM_rel_real_set_elements(double **r),
 PM_set_limits(PM_set *s, double *extr);

extern pcons
 *PM_mapping_info(PM_mapping *h, ...),
 *PM_map_info_alist(PM_map_info *ti);

extern double
 *PM_set_extrema(PM_set *s),
 **PM_make_vectors(int nd, int n),
 **PM_copy_vectors(int nd, int n, double **x),
 *PM_array_real(char *type, void *p, int n, double *x),
 **PM_make_real_set_elements(PM_set *s),
 *PM_get_limits(PM_set *set),
 **PM_generate_lr_index(int nd, int *maxes,
			double *extrema, double *ratio),
 **PM_generate_lr_cp(int nd, int *maxes, double **x1);

extern int
 PM_resolve_type(char *ltyp, char *ntyp, char *btyp);

extern char
 *PM_check_emap(int *peflag, pcons *alst, long n);


/* MLMSRCH.C declarations */

extern PM_mesh_map
 *PM_init_mesh_search(int nd, int *dims, double **r, int *map);

extern void
 PM_fin_mesh_search(PM_mesh_map *sm);

extern long
 PM_lr_mesh_lookup(PM_mesh_map *sm, double *p);


/* MLMEXTR.C declarations */

extern void
 PM_mesh_extr(double *x, int *smn, int *smx, int *mn, int *mx,
	      int ng, int nd, int id, int side,
	      int method, int constr,
	      double dspat, double drat, double orient);


/* MLNLS.C declarations */

extern double
 PM_newtondl(int neqns, double *y, double *x, double *tol, int maxiter,
	     double atol, double rtol,
             void (*linsolv)(int neqns, double *dy, double *y, int iter, void *arg),
             void *arg),
 PM_newtonul(int neqns, double *y, double *x, double *tol, int maxiter,
	     double atol, double rtol,
             void (*linsolv)(int neqns, double *dy, double *y, int iter, void *arg),
             void *arg);

extern void
 PM_nls_monotone(double *px, double *py, int *pitx, double *perr,
		 double xm, double xp, double ym, double yp,
		 double ytol, double xtol, int meth, double asymm,
		 double (*fnc)(double x, void *data), void *arg);


/* MLOPER.C declarations */

extern int
 PM_iplus(int x, int y),
 PM_iminus(int x, int y),
 PM_itimes(int x, int y),
 PM_idivide(int x, int y),
 PM_imodulo(int x, int y),
 PM_feq(double x, double y),
 PM_fneq(double x, double y),
 PM_fge(double x, double y),
 PM_fgt(double x, double y),
 PM_fle(double x, double y),
 PM_flt(double x, double y),
 PM_leq(int64_t x, int64_t y),
 PM_lneq(int64_t x, int64_t y),
 PM_lge(int64_t x, int64_t y),
 PM_lgt(int64_t x, int64_t y),
 PM_lle(int64_t x, int64_t y),
 PM_llt(int64_t x, int64_t y);

extern int64_t
 PM_lplus(int64_t x, int64_t y),
 PM_lminus(int64_t x, int64_t y),
 PM_ltimes(int64_t x, int64_t y),
 PM_ldivide(int64_t x, int64_t y),
 PM_lmodulo(int64_t x, int64_t y),
 PM_land(int64_t x, int64_t y),
 PM_lor(int64_t x, int64_t y),
 PM_lxor(int64_t x, int64_t y),
 PM_llshft(int64_t x, int64_t y),
 PM_lrshft(int64_t x, int64_t y),
 PM_lcmp(int64_t x);

extern double
 PM_fplus(double x, double y),
 PM_fminus(double x, double y),
 PM_ftimes(double x, double y),
 PM_fdivide(double x, double y),
 PM_fmin(double x, double y),
 PM_fmax(double x, double y),
 PM_ffeq(double x, double y),
 PM_ffneq(double x, double y),
 PM_ffge(double x, double y),
 PM_ffgt(double x, double y),
 PM_ffle(double x, double y),
 PM_fflt(double x, double y);

extern int
 PM_conv_array(C_array *dst, C_array *src, int rel),
 PM_set_equal(PM_set *a, PM_set *b, double tol);

extern C_array
 *PM_accumulate_oper(PFVoid *proc, C_array *acc,
		     C_array *operand, int id, void *val);


/* MLRAND.C declarations */

extern void
 PM_srand_48(double x);

extern double
 PM_grand_48(void),
 PM_random_48(double x),
 PM_random_s(double x);


/* MLRFNC.C declarations */

extern long
 PM_ipow(int m, int n);

extern double
 PM_sqr(double x),
 PM_sqrt(double x),
 PM_ln(double x),
 PM_log(double x),
 PM_acos(double x),
 PM_asin(double x),
 PM_atan(double x, double y),
 PM_cot(double x),
 PM_coth(double x),
 PM_floor(double x),
 PM_sign2(double x, double y),
 PM_recip(double x),
 PM_pow(double x, double y),
 PM_power(double x, int p),
 PM_sign(double x),
 PM_round(double x),
 PM_frac(double x),
 PM_fix(double value),
 PM_sgn(double value, double sign),
 PM_hypot(double x, double y),
 PM_random(double x),
 PM_j0(double x),
 PM_j1(double x),
 PM_jn(double x, double n),
 PM_y0(double x),
 PM_y1(double x),
 PM_yn(double x, double n),
 PM_i0(double x),
 PM_i1(double x),
 PM_in(double x, double n),
 PM_k0(double x),
 PM_k1(double x),
 PM_kn(double x, double n),
 PM_tchn(double x, double n),
 PM_horner(double x, double *c, int mn, int mx);


/* MLSFNC.C declarations */

extern double
 PM_cf_eval(int n, double *a, double *b, double tol),
 PM_factorial(int n),
 PM_binomial(int n, int k),
 PM_ln_gamma(double x),
 PM_beta(double z, double w),
 PM_igamma_tolerance(double tol),
 PM_igamma_p(double x, double a),
 PM_igamma_q(double x, double a),
 PM_erf(double x),
 PM_erfc(double x),
 PM_legendre(double x, int l, int m),
 PM_elliptic_integral_g2(double x, double k, double a, double b),
 PM_elliptic_integral_gc(double k, double p, double a, double b),
 PM_elliptic_integral_l1(double x, double k),
 PM_elliptic_integral_l2(double x, double k),
 PM_elliptic_integral_c1(double k),
 PM_elliptic_integral_c2(double k),
 PM_elliptic_integral_c3(double k, double n),
 PM_sn(double u, double k),
 PM_cn(double u, double k),
 PM_dn(double u, double k);


/* MLSOLV.C declarations */

extern int
 PM_block_rsys_elim(double *a, double *b, double *c, double *u,
		    int ns, int nb, int *elim_flag),
 PM_block_nf_backsub(double *a, double *c, double *u,
		     int ns, int nb, int *lnbrs),
 PM_block_tridi_nf(double *af, double *a, double *b, double *c,
		   double *cl, double *u,
		   int nf, int nb, int nl, int ns, int *lnbrs),
 PM_block_tridi(double *a, double *b, double *c, double *u, int ns, int nb),
 PM_tridi(double *a, double *b, double *c, double *r, double *u, int n);

extern double
 PM_determinant(PM_matrix *a);

extern PM_matrix
 *PM_solve(PM_matrix *a, PM_matrix *b),
 *PM_decomp(PM_matrix *a, int *ips, int flag, int *pnc),
 *PM_decompose(PM_matrix *a, int *ips, int flag),
 *PM_sol(PM_matrix *ul, PM_matrix *b, int *ips, int flag),
 *PM_inverse(PM_matrix *a),
 *PM_upper(PM_matrix *a),
 *PM_lower(PM_matrix *a),
 *PM_decb(int n, int ml, int mu, PM_matrix *b, int *ip),
 *PM_solb(int n, int ml, int mu, 
	  PM_matrix *b, PM_matrix *y, int *ips);


/* MLSORT.C declarations */

extern int
 *PM_t_sort(int *in, int n_dep, int n_pts, int *ord);

extern void
 PM_q_sort(double *z, int *ind, int n),
 PM_val_sort(int n, double *xp, double *yp);


/* MLSRCH.C declarations */

extern void
 PM_minmax(void *p, int n, void *pn, void *px,
	   int *imin, int *imax),
 PM_maxmin(double *xp, double *xmin, double *xmax, int n),
 PM_sub_array(void *in, void *out, long *dims,
	      long *reg, long bpi),
 PM_search(int nx, double *x, int ny, double *y, int offs,
	   int *indx);

extern int
 PM_find_index(void *p, double f, int n),
 PM_index_min(double *p, int n);

extern double
 PM_max_value(double *p, int imin, int n);


/* MLSVD.C declarations */

extern int
 PM_svd_solve(double *u, double *w, double *v, int m, int n,
	      double *b, double *x),
 PM_svd_decompose(double *a, int m, int n, double *w, double *v),
 PM_svd_fit(double *x, double *y, double *sig, int np,
	    double *a, int ma,
	    double *u, double *v, double *w, double *pcs,
	    void (*fnc)(double x, double *f, int n)),
 PM_svd_covariance(double **v, int ma, double *w, double **cvm);


/* MLVND.c declarations */

extern double
 *PM_vandermonde(int n, double *x, double *b);


/* MLVECT.c declarations */

extern int
 PM_vct_equal(int nd, double *x, double **v, int i, double tol);

extern double
 PM_lnnorm(double *v, double *x, int n, int order);


#ifdef __cplusplus
}
#endif

#endif


