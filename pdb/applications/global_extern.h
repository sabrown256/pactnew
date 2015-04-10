/*--------------------------------------------------------------*/
/* Header file for EnSight External Reader DSO Library Routines */
/*--------------------------------------------------------------*/
/*  *************************************************************
 *   Copyright 1998 Computational Engineering International, Inc.
 *   All Rights Reserved.
 *
 *        Restricted Rights Legend
 *
 *   Use, duplication, or disclosure of this
 *   software and its documentation by the
 *   Government is subject to restrictions as
 *   set forth in subdivision [(b)(3)(ii)] of
 *   the Rights in Technical Data and Computer
 *   Software clause at 52.227-7013.
 *  *************************************************************
 */
#ifndef GLOBAL_EXTERN_H
#define GLOBAL_EXTERN_H

/*---------------------------------------*/
/* True/False and Error conditions, etc. */
/*---------------------------------------*/
#define Z_ERR                  (-1)          /*Error return value.*/
#define Z_OK                    (1)          /*Success return value.*/
#ifndef TRUE
# define TRUE                   (1)
# define FALSE                  (0)
#endif

#define Z_BUFL                 (80)          /* Typical string length */

#define Z_COMPX                 (0)          /* x component */
#define Z_COMPY                 (1)          /* y component */
#define Z_COMPZ                 (2)          /* z component */

#define Z_STATIC                (0)          /* static geometry          */
#define Z_CHANGE_COORDS         (1)          /* coordinate changing only */
#define Z_CHANGE_CONN           (2)          /* conectivity changing     */

#define Z_GEOM                  (0)          /* Geometry type */
#define Z_VARI                  (1)          /* Variable type */

#define Z_SAVE_ARCHIVE          (0)          /* Save archive    */
#define Z_REST_ARCHIVE          (1)          /* Restore archive */

#define Z_MAX_USERD_NAME        (20)         /* max length of reader name */

#define Z_PER_NODE              (4)          /* At Nodes Variable classif.   */
#define Z_PER_ELEM              (1)          /* At Elements Variable classif.*/


#ifndef GLOBALDEFS_H
/*-----------------------------------*/
/* Unstructured coordinate structure */
/*-----------------------------------*/
typedef struct {
  float xyz[3];
}CRD;
#endif

/*----------------*/ 
/* Variable Types */
/*----------------*/ 
enum z_var_type
{
  Z_CONSTANT,
  Z_SCALAR,
  Z_VECTOR,
  Z_TENSOR,
  MAX_Z_VAR_TYPES
};

/*---------------*/
/* Element Types */
/*---------------*/
enum z_elem_types {
  Z_POINT,         /* 00:  1 node point element */

  Z_BAR02,         /* 01:  2 node bar           */
  Z_BAR03,         /* 02:  3 node bar           */

  Z_TRI03,         /* 03:  3 node triangle      */
  Z_TRI06,         /* 04:  6 node triangle      */
  Z_QUA04,         /* 05:  4 node quad          */
  Z_QUA08,         /* 06:  8 node quad          */

  Z_TET04,         /* 07:  4 node tetrahedron   */
  Z_TET10,         /* 08: 10 node tetrahedron   */
  Z_PYR05,         /* 09:  5 node pyramid       */
  Z_PYR13,         /* 10: 13 node pyramid       */
  Z_PEN06,         /* 11:  6 node pentahedron   */
  Z_PEN15,         /* 12: 15 node pentahedron   */
  Z_HEX08,         /* 13:  8 node hexahedron    */
  Z_HEX20,         /* 14: 20 node hexahedron    */

  Z_MAXTYPE
};


/*-------------------------------*/
/* Unstructured/Structured types */
/*-------------------------------*/
enum z_structured_defs
{
  Z_UNSTRUCTURED,         /* for unstructured part */
  Z_STRUCTURED,           /* for structured (non-iblanked) part */
  Z_IBLANKED,             /* for structured iblanked part */
  Z_MAXMESHTYPES
};

/*----------------------------*/
/* Structured Iblanking types */
/*----------------------------*/
enum z_iblank_domain
{
  Z_EXT,                  /* Exterior */
  Z_INT,                  /* Interior */
  Z_BND,                  /* Boundary */
  Z_INTBND,               /* Internal boundary/baffle */
  Z_SYM,                  /* Symmetry surface */
  Z_NO_OF_IBLANK_DOMAIN_ITEMS
};


/*-----------------------------------*/
/* Dataset Query file info Structure */
/*-----------------------------------*/
#define Z_MAXFILENP    255  /* Max file name and path.*/
#define Z_MAXTIMLEN     40  /* Max time str length */
#define Z_BUFLEN        82  /* Allocated length of the f_desc strings */
typedef struct {
    char name[Z_MAXFILENP];
    long sizeb;
    char timemod[Z_MAXTIMLEN];
    int num_d_lines;
    char **f_desc;
} Z_QFILES;


/* Prototypes for userd_lib */
/*--------------------------*/
#ifdef WIN32
#define W32IMPORT __declspec( dllimport )
#else
#define W32IMPORT extern
#endif

W32IMPORT int
USERD_get_number_of_model_parts( void );

W32IMPORT int
USERD_get_number_of_global_nodes();
     
W32IMPORT int
USERD_get_global_coords(CRD *coord_array);
     
W32IMPORT int
USERD_get_global_node_ids(int *nodeid_array);
     
W32IMPORT int
USERD_get_element_connectivities_for_part(int part_number,
                                          int **conn_array[Z_MAXTYPE]);

W32IMPORT int
USERD_get_element_ids_for_part(int part_number,
                               int *elemid_array[Z_MAXTYPE]);
     
W32IMPORT int
USERD_get_scalar_values(int which_scalar,
                        int which_part,
                        int which_type,
                        float *scalar_array);

W32IMPORT int
USERD_get_vector_values(int which_vector,
                        int which_part,
                        int which_type,
                        float *vector_array);
     
W32IMPORT int
USERD_get_part_build_info(int *part_numbers,
                          int *part_types,
                          char *part_descriptions[Z_BUFL],
                          int *number_of_elements[Z_MAXTYPE],
                          int *ijk_dimensions[3],
                          int *iblanking_options[6]);

W32IMPORT int
USERD_get_block_coords_by_component(int block_number,
                                    int which_component,
                                    float *coord_array);

W32IMPORT int
USERD_get_block_iblanking(int block_number,
                          int *iblank_array);

W32IMPORT int
USERD_get_block_scalar_values(int block_number,
                              int which_scalar,
                              float *scalar_array);

W32IMPORT int
USERD_get_block_vector_values_by_component(int block_number,
                                           int which_vector,
                                           int which_component,
                                           float *vector_array);

W32IMPORT int
USERD_get_name_of_reader(char reader_name[Z_MAX_USERD_NAME],
                         int *two_fields);
     
W32IMPORT int
USERD_set_filenames(char filename_1[],
                    char filename_2[],
                    char the_path[],
                    int swapbytes);
     
W32IMPORT int
USERD_get_number_of_files_in_dataset( void );

W32IMPORT int
USERD_get_dataset_query_file_info(Z_QFILES *qfiles);
     
W32IMPORT int
USERD_get_changing_geometry_status( void );

W32IMPORT int
USERD_get_node_label_status( void );

W32IMPORT int
USERD_get_element_label_status( void );

W32IMPORT int
USERD_get_number_of_time_steps( void );
     
W32IMPORT void
USERD_set_time_step(int time_step);
     
W32IMPORT int
USERD_get_solution_times(float *solution_times);
     
W32IMPORT int
USERD_get_description_lines(int which_type,
                            int which_var,
                            char line1[Z_BUFL],
                            char line2[Z_BUFL]);

W32IMPORT int
USERD_get_number_of_variables( void );

W32IMPORT int
USERD_get_variable_info(char **var_description,
                        char **var_filename,
                        int *var_type,
                        int *var_classify);

W32IMPORT float
USERD_get_constant_value(int which_var);

W32IMPORT int
USERD_get_variable_value_at_specific(int which_var,
                                     int which_node_or_elem,
                                     int which_part,
                                     int which_elem_type,
                                     int time_step,
                                     float values[3]);

W32IMPORT void
USERD_stop_part_building( void );

W32IMPORT int
USERD_bkup(FILE *archive_file,
           int backup_type);

/*--------------------------------------------------------------------*/
#endif /*GLOBAL_EXTERN_H*/

