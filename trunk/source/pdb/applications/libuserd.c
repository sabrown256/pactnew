/*
   User Defined PDB READER

   A special ASCII input file will inform the READER about
   how the contents of the PDB file are to be understood.
   By default the READER will expect to handle data written
   as PM_mappings.

   The ASCII input file could contain keywords such as:
   FILE:  Input file (names).
   VARIABLE: Variable name filter 
      One or more of these could specify variables to be
      made available to EnSight.
   MESH_X:  The name of a variable containing the X Coordinates
            of the mesh.
   MESH_Y:  Y coordinates.
   MESH_Z:  Z coordinates.
             
*/
   
/*====================*/
/* USERD DSO Routines */           /* Dummy routines */
/*====================*/
/*-------------------------------------------------------------------------
Routine Index:

--------------------------------------
Generally Needed for UNSTRUCTURED data
(Can be dummys if only block data)
--------------------------------------
USERD_get_number_of_global_nodes                number of global nodes
USERD_get_global_coords                         global node coordinates
USERD_get_global_node_ids                       global node ids
USERD_get_element_connectivities_for_part       part's element connectivites
USERD_get_element_ids_for_part                  part's element ids
USERD_get_scalar_values                         global scalar variables
USERD_get_vector_values                         global vector variables

-----------------------------------------
Generally Needed for BLOCK data
(Can be dummys if only unstructured data)
-----------------------------------------
USERD_get_block_coords_by_component             block coordinates
USERD_get_block_iblanking                       block iblanking values
USERD_get_block_scalar_values                   block scalar variables
USERD_get_block_vector_values_by_component      block vector variables 

---------------------------
Generally needed for either
or both kinds of data
---------------------------
USERD_set_filenames                             filenames entered in GUI
USERD_set_time_step                             current time step

USERD_get_name_of_reader                        name of reader for GUI
USERD_get_number_of_files_in_dataset            number of files in model
USERD_get_dataset_query_file_info               info about each model file
USERD_get_changing_geometry_status              changing geometry?
USERD_get_node_label_status                     node labels?
USERD_get_element_label_status                  element labels?
USERD_get_number_of_time_steps                  number of time steps
USERD_get_solution_times                        solution time values
USERD_get_description_lines                     file associated descrip lines
USERD_get_number_of_variables                   number of variables
USERD_get_variable_info                         variable type/descrip etc.
USERD_get_constant_value                        constant variable's value
USERD_get_number_of_model_parts                 number of model parts
USERD_get_part_build_info                       part type/descrip etc.
USERD_get_variable_value_at_specific            node's or element's variable
                                                   value over time

USERD_stop_part_building                        cleanup routine
USERD_bkup                                      archive routine
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "pdb.h"

#include "global_extern.h"          /* Any files containing these routines */
                                    /* should include this header file     */
struct s_var_info
   {char *name;
    int indirect;
    char description[Z_BUFL];
    char filename[Z_BUFL];
    int type;
    int classify;};

typedef struct s_var_info var_info;

PM_mapping
 *Get_requested_mapping(int varnum, int blocknum);

int
 Free_mapping(PM_mapping *mapping),
 Free_set(PM_set *set),
 Get_coords_by_component(PM_mapping *mapping, int which_component, 
                         float *coord_array),
 Get_scalars(PM_mapping *mapping, float *scalar_array);

var_info
 *Get_3D_mappings(char **tvarlist, int nvariables);

/*---------------------------------------------------------------------
 * NOTE: Unless explicitly stated otherwise, all arrays are zero based 
 *       In true C fashion.                                            
 *---------------------------------------------------------------------*/
    
/* Typical Global Variables */      /* You will likely have several more */
/*--------------------------*/
static char Geom_file[Z_MAXFILENP];
static char Resu_file[Z_MAXFILENP];
static int Numparts_available      = 0;
static int Num_unstructured_parts  = 0;
static int Num_structured_blocks   = 0;
static int Num_time_steps          = 1;
static int Num_global_nodes        = 0;
static int Num_variables           = 0;
static int Num_dataset_files       = 0;
static int Current_time_step       = 0;

PDBfile *cur_file = NULL;
char current_file[MAXLINE];
var_info *varlist = NULL;
int kmax = 0, lmax = 0, mmax = 0;

/*-----------------------------
 * UNSTRUCTURED DATA ROUTINES: 
 *-----------------------------*/

/*--------------------------------------------------------------------
 * USERD_get_number_of_global_nodes -                                 
 *--------------------------------------------------------------------
 *                                                                    
 *   Gets the number of global nodes, used for unstructured parts     
 *                                                                    
 *  returns: number of global nodes (>=0 if okay, <0 if problems)     
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * For unstructured data:                                          
 *         EnSight wants  1. A global array of nodes                  
 *                        2. Element connectivities by part, which    
 *                           reference the node numbers of the global 
 *                           node array.                              
 *              IMPORTANT:                                            
 *              ---------                                             
 *              If you provide node ids, then element connectivities  
 *              must be in terms of the node ids.  If you do not      
 *              provide node ids, then element connectivities must be 
 *              in terms of the index into the node array, but shifted
 *              to start at 1                                         
 *                                                                    
 *  Notes:                                                            
 *  * Not called unless Num_unstructured_parts is > 0                 
 *--------------------------------------------------------------------*/
int
USERD_get_number_of_global_nodes()
{
  printf("Error: USERD_get_number_of_global_nodes called\n");  
  Num_global_nodes=0;
  return(Num_global_nodes);
}


/*--------------------------------------------------------------------
 * USERD_get_global_coords -                                          
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the global coordinates                                       
 *                                                                    
 *  (OUT) coord_array             = 1D array of CRD structures,       
 *                                   which contains x,y,z coordinates 
 *                                   of each node.                    
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Num_global_nodes long)           
 *                                                                    
 * For reference, this structure (which is in global_extern) is:      
 *                                                                    
 *         typedef struct {                                           
 *            float xyz[3];                                           
 *         }CRD;                                                      
 *                                                                    
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_unstructured_parts is > 0                 
 *--------------------------------------------------------------------*/
int
USERD_get_global_coords(CRD *coord_array)
{
  printf("Error: USERD_get_global_coords called\n");
  return(Z_ERR);
}



/*--------------------------------------------------------------------
 * USERD_get_global_node_ids -                                        
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the global nodeids                                           
 *                                                                    
 *  (OUT) nodeid_array            = 1D array containing node ids of   
 *                                   each node. The ids must be > 0   
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Num_global_nodes long)           
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_unstructured_parts is > 0  and            
 *    node label status is TRUE                                       
 *--------------------------------------------------------------------*/
int
USERD_get_global_node_ids(int *nodeid_array)
{
  printf("Error: USERD_get_global_node_ids called\n");
  return(Z_ERR);
}


/*--------------------------------------------------------------------
 * USERD_get_element_connectivities_for_part -                        
 *--------------------------------------------------------------------
 *                                                                    
 *   Gets the connectivities for the elements of a part               
 *                                                                    
 *  (IN)  part_number             = The part number                   
 *                                                                    
 *  (OUT) conn_array              = 3D array containing connectivity  
 *                                   of each element of each type.    
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Z_MAXTYPE by num_of_elements of  
 *                                   each type by connectivity length 
 *                                   of each type)                    
 *                                                                    
 *                       ex) If num_of_elements[Z_TRI03] = 25         
 *                              num_of_elements[Z_QUA04] = 100        
 *                              num_of_elements[Z_HEX08] = 30         
 *                           as obtained in:                          
 *                            USERD_get_part_build_info               
 *                                                                    
 *                           Then the allocated dimensions available  
 *                           for this routine will be:                
 *                              conn_array[Z_TRI03][25][3]            
 *                              conn_array[Z_QUA04][100][4]           
 *                              conn_array[Z_HEX08][30][8]            
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_unstructured_parts is > 0                 
 *--------------------------------------------------------------------*/
int
USERD_get_element_connectivities_for_part(int part_number,
                                          int **conn_array[Z_MAXTYPE])
{
  printf("Error: USERD_get_global_coords called\n");
  return(Z_ERR);
}


/*--------------------------------------------------------------------
 * USERD_get_element_ids_for_part -                                   
 *--------------------------------------------------------------------
 *                                                                    
 *   Gets the ids for the elements of a part                          
 *                                                                    
 *  (IN)  part_number             = The part number                   
 *                                                                    
 *  (OUT) elemid_array            = 2D array containing id of each    
 *                                   element of each type.            
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Z_MAXTYPE by num_of_elements of  
 *                                   each type)                       
 *                                                                    
 *                       ex) If num_of_elements[Z_TRI03] = 25         
 *                              num_of_elements[Z_QUA04] = 100        
 *                              num_of_elements[Z_HEX08] = 30         
 *                           as obtained in:                          
 *                            USERD_get_part_build_info               
 *                                                                    
 *                           Then the allocated dimensions available  
 *                           for this routine will be:                
 *                              conn_array[Z_TRI03][25]               
 *                              conn_array[Z_QUA04][100]              
 *                              conn_array[Z_HEX08][30]               
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_unstructured_parts is > 0  and element    
 *    label status is TRUE                                            
 *--------------------------------------------------------------------*/
int
USERD_get_element_ids_for_part(int part_number,
                               int *elemid_array[Z_MAXTYPE])
{
  printf("Error: USERD_get_element_ids_for_part called\n");
  return(Z_ERR);
}


/*--------------------------------------------------------------------
 * USERD_get_scalar_values -                                          
 *--------------------------------------------------------------------
 *                                                                    
 * if Z_PER_NODE:
 *   Get the values at each global node for a given scalar variable.
 *
 * or if Z_PER_ELEM:
 *   Get the values at each element of a specific part and type for a
 *   given scalar variable.
 *                                                                    
 *  (IN)  which_scalar            = The variable "number" to get      
 *                                                                    
 *  (IN)  which_part
 * 
 *           if Z_PER_NODE:         Not used
 * 
 *           if Z_PER_ELEM:       = The part number
 * 
 * (IN)  which_type
 *
 *           if Z_PER_NODE:         Not used
 *
 *           if Z_PER_ELEM:       = The element type
 *                                                    
 *  (OUT) scalar_array
 *
 *            if Z_PER_NODE:       = 1D array containing scalar values
 *                                   for each node.
 *
 *                                   (Array will have been allocated
 *                                    Num_global_nodes long)
 *
 *            if Z_PER_ELEM:       = 1d array containing scalar values for
 *                                   each element of a particular part & type.
 *
 *                                  (Array will have been allocated
 *                                   number_of_elements[which_part][which_type]
 *                                    long.  See USERD_get_part_build_info)
 *
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_unstructured_parts is > 0,                
 *    Num_variables is > 0, and you have some scalar type variables   
 *
 *  * The per_node or per_elem classification must be obtainable from the
 *    variable number (a var_classify array needs to be retained)
 *     
 *--------------------------------------------------------------------*/
int
USERD_get_scalar_values(int which_scalar,
                        int which_part,
                        int which_type,
                        float *scalar_array)
{
  printf("Error: USERD_get_scalar_values called\n");
  return(Z_ERR);
}


/*--------------------------------------------------------------------
 * USERD_get_vector_values -                                          
 *--------------------------------------------------------------------
 *
 *  if Z_PER_NODE:
 *    Get the values at each global node for a given vector variable.
 *
 *  or if Z_PER_ELEM:
 *    Get the values at each element of a specific part and type for a
 *    given vector variable.
 *                                                                    
 *  (IN)  which_vector            = The variable "number" to get      
 *
 *  (IN)  which_part
 *
 *           if Z_PER_NODE:         Not used
 *
 *           if Z_PER_ELEM:       = The part number
 *
 *  (IN)  which_type
 *
 *           if Z_PER_NODE:         Not used
 *
 *           if Z_PER_ELEM:       = The element type
 *                                                      
 *
 *
 * (OUT) vector_array
 *
 *          if Z_PER_NODE:       = 1D array containing vector values
 *                                  for each node.
 *
 *                                 (Array will have been allocated
 *                                  3 by Num_global_nodes long)
 * 
 *                    Info stored in this fashion:
 *                          vector_array[0] = xcomp of node 1
 *                          vector_array[1] = ycomp of node 1
 *                          vector_array[2] = zcomp of node 1
 * 
 *                          vector_array[3] = xcomp of node 2
 *                          vector_array[4] = ycomp of node 2
 *                          vector_array[5] = zcomp of node 2
 * 
 *                          vector_array[6] = xcomp of node 3
 *                          vector_array[7] = ycomp of node 3
 *                          vector_array[8] = zcomp of node 3
 *                          etc.
 * 
 *           if Z_PER_ELEM:      = 1d array containing vector values for
 *                                 each element of a particular part and type.
 * 
 *                              (Array will have been allocated
 *                              3 by number_of_elements[which_part][which_type]
 *                              long.  See USERD_get_part_build_info)
 * 
 *                    Info stored in this fashion:
 *                         vector_array[0] = xcomp of elem 1 (of part and type)
 *                         vector_array[1] = ycomp of elem 1         "
 *                         vector_array[2] = zcomp of elem 1         "
 * 
 *                         vector_array[3] = xcomp of elem 2         "
 *                         vector_array[4] = ycomp of elem 2         "
 *                         vector_array[5] = zcomp of elem 2         "
 * 
 *                         vector_array[6] = xcomp of elem 3         "
 *                         vector_array[7] = ycomp of elem 3         "
 *                         vector_array[8] = zcomp of elem 3         "
 *                         etc.
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_unstructured_parts is > 0,                
 *    Num_variables is > 0,  and you have some vector type variables  
 *
 *  * The per_node or per_elem classification must be obtainable from the
 *    variable number (a var_classify array needs to be retained)
 *    
 *--------------------------------------------------------------------*/
int
USERD_get_vector_values(int which_vector,
                        int which_part,
                        int which_type,
                        float *vector_array)
{
  printf("Error: USERD_get_vector_values called\n");
  return(Z_ERR);
}



/*---------------------------
 * STRUCTURED DATA ROUTINES: 
 *---------------------------*/

/*--------------------------------------------------------------------
 * USERD_get_block_coords_by_component -                              
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the coordinates of a given block, component at a time        
 *                                                                    
 *  (IN)  block_number            = The block number                  
 *                                                                    
 *  (IN)  which_component         = Z_COMPX if x component wanted     
 *                                = Z_COMPY if y component wanted     
 *                                = Z_COMPZ if z component wanted     
 *                                                                    
 *  (OUT) coord_array             = 1D array containing x,y, or z     
 *                                   coordinate component of each node
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   i*j*k for the block long)        
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_structured_blocks is > 0                  
 *--------------------------------------------------------------------*/
int
USERD_get_block_coords_by_component(int block_number,
                                    int which_component,
                                    float *coord_array)
{
/*
   Assumption:  All mappings in the file are defined on the same
                grid.  So for now the mesh coords are obtained from
                mapping 0.
*/
  PM_mapping *current_mapping = NULL;
  int err = Z_OK;

#ifdef DEBUG
  printf("USERD_get_block_coords_by_component called, block_number: \
%d, which_component: %d\n", block_number, which_component);
#endif

  if ((current_mapping = Get_requested_mapping(0, block_number - 1)) == NULL)
      {err = Z_ERR;
       printf("Error getting requested mapping\n");
       return(err);}

  if ((!Get_coords_by_component(current_mapping, which_component, coord_array)))
      {err = Z_ERR;
       printf("Error getting coordinates\n");
       return(err);}   

#if 0
  Free_mapping(current_mapping);
#endif

#ifdef DEBUG
  printf("Returning from USERD_get_block_coords_by_component\n");
#endif

  return(err);
}

/*--------------------------------------------------------------------
 * USERD_get_block_iblanking -                                        
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the iblanking value at each node of a block - If Z_IBLANKED  
 *                                                                    
 *  (IN)  block_number            = The block number                  
 *  (OUT) iblank_array            = 1D array containing iblank value  
 *                                   for each node.                   
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   i*j*k for the block long)        
 *                                                                    
 *          possible values are:   Z_EXT     = exterior (outside)     
 *                                 Z_INT     = interior (inside)      
 *                                 Z_BND     = boundary               
 *                                 Z_INTBND  = internal boundary      
 *                                 Z_SYM     = symmetry plane         
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_structured_blocks is > 0  and you have    
 *    some iblanked blocks                                            
 *--------------------------------------------------------------------*/
int
USERD_get_block_iblanking(int block_number,
                          int *iblank_array)
{

#ifdef DEBUG
  printf("USERD_get_block_iblanking called\n");
#endif

  return(Z_OK);
}


/*--------------------------------------------------------------------
 * USERD_get_block_scalar_values -                                    
 *--------------------------------------------------------------------
 *
 *   if Z_PER_NODE:
 *     Get the values at each node of a block, for a given scalar       
 *     variable.                                                        
 *    
 *   or if Z_PER_ELEM:
 *     Get the values at each element of a block, for a given scalar
 *     variable.
 *
 *  (IN)  block_number            = The block number                  
 *  (IN)  which_scalar            = The variable "number" to get      
 *  (OUT) scalar_array            = 1D array containing scalar values 
 *                                  for each node or element.
 *                   
 *                                  Array will have been allocated:
 *       
 *                                  if Z_PER_NODE:
 *                                     i*j*k for the block long
 *
 *                                  if Z_PER_ELEM:
 *                                     (i-1)*(i-1)*(k-1) for the block long
 *
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_structured_blocks is > 0,                 
 *    Num_variables is > 0, and there are some scalar type variables  
 *
 *  * The per_node or per_elem classification must be obtainable from the
 *    variable number (a var_classify array needs to be retained)
 *
 *--------------------------------------------------------------------*/
int
USERD_get_block_scalar_values(int block_number,
                              int which_scalar,
                              float *scalar_array)
{int err = Z_OK;
 PM_mapping *current_mapping = NULL;

#ifdef DEBUG
  printf("USERD_get_block_scalar_values called, block_number: %d, \
which_scalar: %d\n", block_number, which_scalar);
#endif

 if ((current_mapping = Get_requested_mapping(which_scalar - 1, block_number - 1))
                         == NULL)
     {err = Z_ERR;
      printf("Error getting coordinates\n");
      return(err);}

  if (Get_scalars(current_mapping, scalar_array) != 0)
      {err = Z_ERR;
       printf("Error getting scalars\n");
       return(err);}

  Free_mapping(current_mapping);

#ifdef DEBUG
  printf("Returning from USERD_get_block_scalar_values\n");
#endif

  return(Z_OK);
}


/*--------------------------------------------------------------------
 * USERD_get_block_vector_values_by_component -                       
 *--------------------------------------------------------------------
 *                                                                    
 *  if Z_PER_NODE:
 *    Get the values at each node of a block, for a given vector
 *    variable, one component at a time.
 *
 *  or if Z_PER_ELEM:
 *    Get the values at each element of a block, for a given vector
 *    variable, one component at a time.
 *                                                                    
 *  (IN)  block_number            = The block number                  
 *                                                                    
 *  (IN)  which_vector            = The variable "number" to get      
 *                                                                    
 *  (IN)  which_component         = Z_COMPX if x component wanted     
 *                                = Z_COMPY if y component wanted     
 *                                = Z_COMPZ if z component wanted     
 *                                                                    
 *  (OUT) vector_array            = 1D array containing vector        
 *                                  component value for each node or element. 
 *
 *                                  Array will have been allocated:
 *      
 *                                  if Z_PER_NODE:
 *                                     i*j*k for the block long
 *
 *                                  if Z_PER_ELEM:
 *                                     (i-1)*(i-1)*(k-1) for the block long
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * Not called unless Num_structured_blocks is > 0,                 
 *    Num_variables is > 0, and there are some vector type variables  
 *
 *  * The per_node or per_elem classification must be obtainable from the
 *    variable number (a var_classify array needs to be retained)
 *
 *--------------------------------------------------------------------*/
int
USERD_get_block_vector_values_by_component(int block_number,
                                           int which_vector,
                                           int which_component,
                                           float *vector_array)
{
  int err = Z_ERR;
  
  printf("Error: A vector value was requested\n");

  return(err);
}



/*-------------------
 * GENERAL ROUTINES: 
 *-------------------*/

/*------------------------------
 * NOTE: Only two SET routines. 
 *------------------------------*/

/*--------------------------------------------------------------------
 * USERD_set_filenames -                                              
 *--------------------------------------------------------------------
 *                                                                    
 *   Receives the geometry and result filenames entered in the data   
 *   dialog.  The user written code will have to store and use these  
 *   as needed.  The user written code must manage its own files!!    
 *                                                                    
 *  (IN) filename_1   = the filename entered into the geometry        
 *                         field of the data dialog.                  
 *  (IN) filename_2   = the filename entered into the result          
 *                         field of the data dialog.                  
 *  (IN) the_path     = the path info from the data dialog.           
 *                      Note: filename_1 and filename_2 have already  
 *                            had the path prepended to them.  This   
 *                            is provided in case it is needed for    
 *                            filenames contained in one of the files 
 *  (IN) swapbytes    = TRUE if should swap bytes
 *                    = FALSE normally
 *                                                                    
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * Since you manage these files, they can be whatever.  Perhaps    
 *    you will use only one, and have references to everything else   
 *    you need within it, like EnSight6 does.                         
 *--------------------------------------------------------------------*/
int
USERD_set_filenames(char filename_1[],
                    char filename_2[],
                    char the_path[],
                    int swapbytes)
{
/* 
  First cut will have only one input file (filename_1).  
  If this file is the ASCII definition file it will contain
  all the information necessary to process a PDB file.  If this
  file is a PDB file, default processing will occur.
  
  Default processing will mean scanning the PDB file for PM_mappings
  and defining one variable per mapping.
*/
  char **tvarlist = NULL;
  int err = Z_OK, ierr, i;
  int nvariables = 0;

#ifdef DEBUG
  printf("PDB Custom Reader: USERD_set_filenames called with \
filename_1: %s, filename_2: %s, the_path: %s\n", filename_1,
filename_2, the_path);
#endif

  if (cur_file != NULL)
     PD_close(cur_file);

#ifdef DEBUG
  printf("Attempting to open %s, ", filename_1);
#endif

  if ((cur_file = PD_open(filename_1, "r")) == NULL)
     {err = Z_ERR;
      printf("Error opening input file--PDB file expected\n");
      return(err);}

#ifdef DEBUG
  printf("... succeeded\n");
#endif


  strcpy(current_file, filename_1);

/* Get a list of all PM_mappings in the file */
  tvarlist = PD_ls(cur_file, NULL, "PM_mapping", &nvariables);
  if (nvariables == 0)
     tvarlist = PD_ls(cur_file, NULL, "PM_mapping *", &nvariables);

#ifdef DEBUG
  printf("PD_ls returned nvariables = %d\n", nvariables);
#endif


/* Now pick out just the 3-D mappings (Does EnSight have 1 or 2-d?)     */
  varlist = Get_3D_mappings(tvarlist, nvariables);

/* Free tvarlist */
  SFREE(tvarlist);

  return(err);

}


/*--------------------------------------------------------------------
 * USERD_set_time_step -                                              
 *--------------------------------------------------------------------
 *                                                                    
 *   Set the current time step.  All functions that need time, and    
 *   that do not explicitly pass it in, will use this time step if    
 *   needed.                                                          
 *                                                                    
 *  (IN) time_step - The current time step                            
 *
 * Note: This routine is called from the server exit_rout with a -1
 *       argument.  This is the chance to clean up anything that
 *       should be cleaned up upon exit.  Like temporary files....
 *--------------------------------------------------------------------*/
void
USERD_set_time_step(int time_step)
{
#ifdef DEBUG
  printf("USERD_set_time_step called with time_step: %d\n",
         time_step);
#endif

  Current_time_step = time_step;
  return;
}


/*--------------------------------------------------------------------
 * USERD_get_name_of_reader -                                         
 *--------------------------------------------------------------------
 *                                                                    
 *   Gets the name of the reader, so gui can list as a reader         
 *                                                                    
 *  (OUT) reader_name          = the name of the reader (data format) 
 *                             (max length is Z_MAX_USERD_NAME, which 
 *                              is 20)                                
 *                                                                    
 *  (OUT) *two_fields          = FALSE if only one data field is      
 *                                     required.                      
 *                               TRUE if two data fields required     
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * Always called.  Provide a name for your custom reader format    
 *                                                                    
 *  * If you don't want a custom reader to show up in the data dialog 
 *    choices, return a name of "No_Custom"                           
 *--------------------------------------------------------------------*/
int
USERD_get_name_of_reader(char reader_name[Z_MAX_USERD_NAME],
                         int *two_fields)
{
#ifdef DEBUG
  printf("USERD_get_name_of_reader called\n");
#endif

  strncpy(reader_name,"Custom PDB",Z_MAX_USERD_NAME);
  *two_fields = FALSE;
  return(Z_OK);
}


/*--------------------------------------------------------------------
 * USERD_get_number_of_files_in_dataset -                             
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the total number of files in the dataset.  Used for the      
 *   dataset query option.                                            
 *                                                                    
 *  returns: the total number of files in the dataset                 
 *                                                                    
 *  Notes:                                                            
 *  * You can be as complete as you want about this.  If you don't    
 *    care about the dataset query option, return a value of 0        
 *    If you only want certain files, you can just include them.      
 *--------------------------------------------------------------------*/
int
USERD_get_number_of_files_in_dataset( void )
{
#ifdef DEBUG
  printf("USERD_get_number_of_files_in_dataset called\n");
#endif

  Num_dataset_files = 0;
  return(Num_dataset_files);
}


/*--------------------------------------------------------------------
 * USERD_get_dataset_query_file_info -                                
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the information about files in the dataset.  Used for the    
 *   dataset query option.                                            
 *                                                                    
 *  (OUT) qfiles   = Structure containing information about each file 
 *                   of the dataset. The Z_QFILES structure is defined
 *                   in the global_extern.h file                      
 *                                                                    
 *                   (The structure will have been allocated          
 *                    Num_dataset_files long, with 10 description     
 *                    lines per file).                                
 *                                                                    
 *      qfiles[].name        = The name of the file                   
 *                             (Z_MAXFILENP is the dimensioned length 
 *                              of the name)                          
 *                                                                    
 *      qfiles[].sizeb       = The number of bytes in the file        
 *                             (Typically obtained with a call to the 
 *                              "stat" system routine)                
 *                                                                    
 *      qfiles[].timemod     = The time the file was last modified    
 *                             (Z_MAXTIMLEN is the dimesioned length  
 *                              of this string)                       
 *                             (Typically obtained with a call to the 
 *                              "stat" system routine)                
 *                                                                    
 *      qfiles[].num_d_lines = The number of description lines you    
 *                              are providing from the file. Max = 10 
 *                                                                    
 *      qfiles[].f_desc[]    = The description line(s) per file,      
 *                              qfiles[].num_d_lines of them          
 *                              (Z_MAXFILENP is the allocated length of  
 *                               each line)                           
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * If Num_dataset_files is 0, this routine will not be called.     
 *--------------------------------------------------------------------*/
int
USERD_get_dataset_query_file_info(Z_QFILES *qfiles)
{
#ifdef DEBUG
  printf("USERD_get_dataset_query_file_info called\n");
#endif

  return(Z_OK);
}


/*--------------------------------------------------------------------
 * USERD_get_changing_geometry_status -                               
 *--------------------------------------------------------------------
 *                                                                    
 *   Gets the changing geometry status                                
 *                                                                    
 *  returns:  Z_STATIC        if geometry does not change             
 *            Z_CHANGE_COORDS if changing coordinates only            
 *            Z_CHANGE_CONN   if changing connectivity                
 *                                                                    
 *  Notes:                                                            
 *  * EnSight does not support changing number of parts.  But the     
 *    coords and/or the connectivity of the parts can change.         
 *--------------------------------------------------------------------*/
int
USERD_get_changing_geometry_status( void )
{
#ifdef DEBUG
  printf("USERD_get_changing_geometry_status called\n");
#endif

  return(Z_STATIC);
}


/*--------------------------------------------------------------------
 * USERD_get_node_label_status -                                      
 *--------------------------------------------------------------------
 *                                                                    
 *   Do we have node labels (ids) ?                                   
 *                                                                    
 *  returns:  TRUE        if node labels are available                
 *            FALSE       if no node labels                           
 *                                                                    
 *  Notes:                                                            
 *  * These are needed in order to do any node querying, or node      
 *    labeling on-screen                               .              
 *                                                                    
 *      For unstructured parts, you can read them from your file if   
 *      available, or can assign them, etc. They need to be unique    
 *      per part, and are often unique per model.                     
 *                                                                    
 *        USERD_get_global_node_ids is used to obtain the ids, if the 
 *        status returned here is TRUE.                               
 *                                                                    
 *        Also be aware that if you say node labels are available,    
 *        the connectivity of elements must be according to these     
 *        node ids.                                                   
 *                                                                    
 *      For structured parts, EnSight will assign ids if you return a 
 *        status of TRUE here.  You cannot assign them yourself!!     
 *--------------------------------------------------------------------*/
int
USERD_get_node_label_status( void )
{
#ifdef DEBUG
  printf("USERD_get_node_label_status called\n");
#endif

  return(TRUE);
}


/*--------------------------------------------------------------------
 * USERD_get_element_label_status -                                   
 *--------------------------------------------------------------------
 *                                                                    
 *   Do we have element labels (ids) ?                                
 *                                                                    
 *  returns:  TRUE        if element labels are available             
 *            FALSE       if no element labels                        
 *                                                                    
 *  Notes:                                                            
 *  * These are needed in order to do any element querying, or        
 *    element labeling on-screen.                                     
 *                                                                    
 *      For unstructured parts, you can read them from your file if   
 *      available, or can assign them, etc. They need to be unique    
 *      per part, and are often unique per model.                     
 *                                                                    
 *        USERD_get_element_ids_for_part is used to obtain the ids,   
 *        on a part by part basis, if TRUE status is returned here.   
 *                                                                    
 *      For structured parts, EnSight will assign ids if you return a 
 *        status of TRUE here.  You cannot assign them youself!!      
 *--------------------------------------------------------------------*/
int
USERD_get_element_label_status( void )
{
#ifdef DEBUG
  printf("USERD_get_element_label_status called\n");
#endif

  return(TRUE);
}


/*--------------------------------------------------------------------
 * USERD_get_number_of_time_steps -                                   
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the number of time steps of data available.                  
 *                                                                    
 *  returns: number of time steps (>0 if okay, <=0 if problems).      
 *                                                                    
 *  Notes:                                                            
 *  * This should be >= 1       1 indicates a static problem          
 *                             >1 indicates a transient problem       
 *--------------------------------------------------------------------*/
int
USERD_get_number_of_time_steps( void )
{
#ifdef DEBUG
  printf("USERD_get_number_of_time_steps called\n");
#endif

  Num_time_steps = 1;
  return(Num_time_steps);
}


/*--------------------------------------------------------------------
 * USERD_get_solution_times -                                         
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the solution times associated with each time step.           
 *                                                                    
 *  (OUT) solution_times       = 1D array of solution times/time step 
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Num_time_steps long)             
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * These must be non-negative and increasing.                      
 *--------------------------------------------------------------------*/
int
USERD_get_solution_times(float *solution_times)
{
#ifdef DEBUG
  printf("USERD_get_solution_times called\n");
#endif

  solution_times[0] = 0.0;
  return(Z_OK);
}


/*--------------------------------------------------------------------
 * USERD_get_description_lines -                                      
 *--------------------------------------------------------------------
 *                                                                    
 *   Get two description lines associated with geometry per time step,
 *   or one description line associated with a variable per time step.
 *                                                                    
 *  (IN)  which_type           = Z_GEOM for geometry                  
 *                             = Z_VARI for variable                  
 *                                                                    
 *  (IN)  which_var            = If it is a variable, which one.      
 *                               Ignored if geometry type.            
 *                                                                    
 *  (OUT) line1                = The 1st geometry description line,   
 *                               or the variable description line.    
 *                                                                    
 *  (OUT) line2                = The 2nd geometry description line    
 *                               Not used if variable type.           
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *                                                                    
 *  * These are the lines EnSight can echo to the screen in           
 *    annotation mode.                                                
 *--------------------------------------------------------------------*/
int
USERD_get_description_lines(int which_type,
                            int which_var,
                            char line1[Z_BUFL],
                            char line2[Z_BUFL])
{
#ifdef DEBUG
  printf("USERD_get_description_lines called\n");
  if (which_type == Z_GEOM)
     printf("   with which_type = Z_GEOM\n");
  else
     printf("   with which_type = Z_VARI\n");
#endif

  if (which_type == Z_GEOM)
     {line1[0] = (char) NULL;
      line2[0] = (char) NULL;}
  else
     {if (which_var <= Num_variables)
         strcpy(line1, varlist[which_var - 1].name);}

  return(Z_OK);
}

/*--------------------------------------------------------------------
 * USERD_get_number_of_variables -                                    
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the number of variables (includes constant, scalar, and      
 *   vector types), for which you will be providing info.             
 *                                                                    
 *  returns: number of variables (includes constant, scalar, vector,  
 *            and tensor types)                                       
 *            >=0 if okay                                             
 *            <0 if problem                                           
 *                                                                    
 *  Notes:                                                            
 *  * Variable numbers, by which references will be made, are implied 
 *    here. If you say there are 3 variables, the variable numbers    
 *    will be 1, 2, and 3.                                            
 *--------------------------------------------------------------------*/
int
USERD_get_number_of_variables( void )
{
/*
   Num_variables is set in USERD_set_filenames.
   Have to:  (Default case)
   Process the PDB file, finding all PM_mappings and count them
   each mapping will correspond to a variable.
*/
#ifdef DEBUG
  printf("USERD_get_number_of_variables called\n");
#endif

  return(Num_variables);
}


/*--------------------------------------------------------------------
 * USERD_get_variable_info -                                          
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the variable descriptions, types and filenames               
 *                                                                    
 *  (OUT) var_description      = Variable descriptions                
 *                                                                    
 *                               (Array will have been allocated      
 *                                Num_variables by Z_BUFL long)       
 *                                                                    
 *  (OUT) var_filename         = Variable filenames                   
 *                                                                    
 *                               (Array will have been allocated      
 *                                Num_variables by Z_BUFL long)       
 *                                                                    
 *  (OUT) var_type             = Variable type                        
 *                                                                    
 *                               (Array will have been allocated      
 *                                Num_variables long)                 
 *                                                                    
 *                               types are:  Z_CONSTANT               
 *                                           Z_SCALAR                 
 *                                           Z_VECTOR                 
 *                                       
 *  (OUT) var_classify         = Variable classification
 *
 *                               (Array will have been allocated
 *                                Num_variables long)
 *
 *                               types are:  Z_PER_NODE
 *                                           Z_PER_ELEM
 *                           
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * The implied variable numbers apply, but be aware that the       
 *    arrays are zero based.                                          
 *    So for variable 1, will need to provide   var_description[0]    
 *                                              var_filename[0]       
 *                                              var_type[0]           
 *                                              var_classify[0]           
 *                                                                    
 *       for variable 2, will need to provide   var_description[1]    
 *                                              var_filename[1]       
 *                                              var_type[1]           
 *                                              var_classify[1]           
 *             etc.                                                   
 *--------------------------------------------------------------------*/
int
USERD_get_variable_info(char **var_description,
                        char **var_filename,
                        int *var_type,
                        int *var_classify)
{
  int err = Z_OK;
  int i;

#ifdef DEBUG
  printf("USERD_get_variable_info called\n");
#endif

  for (i = 0; i < Num_variables; i++)
     {strcpy(var_description[i], varlist[i].description);
      strcpy(var_filename[i], varlist[i].filename); 
      var_type[i]   = varlist[i].type;
      *var_classify = varlist[i].classify;}

  return(err);
}     


/*--------------------------------------------------------------------
 * USERD_get_constant_value -                                         
 *--------------------------------------------------------------------
 *                                                                    
 *   Get the value of a constant at a time step                       
 *                                                                    
 *  (IN)  which_var            = Which variable (this is the same     
 *                                               implied variable     
 *                                               number used in other 
 *                                               functions)           
 *                                                                    
 *  returns: value of the requested constant variable                 
 *                                                                    
 *  Notes:                                                            
 *  * This will be based on Current_time_step                         
 *--------------------------------------------------------------------*/
float
USERD_get_constant_value(int which_var)
{
  float constant_val;

#ifdef DEBUG
  printf("USERD_get_constant_value called\n");
#endif

  constant_val = 0.0;
  return(constant_val);
}


/*-------------------------------------------------------------------
 * USERD_get_number_of_model_parts -                                 
 *-------------------------------------------------------------------
 *                                                                   
 *   Gets the total number of unstructured and structured parts      
 *   in the model, for which you can supply information.             
 *                                                                   
 *  returns:  num_parts  (>0 if okay, <=0 if probs)                  
 *                                                                   
 *  Notes:                                                           
 *  * If going to have to read down through the parts in order to    
 *    know how many, you may want to build a table of pointers to    
 *    the various parts, so can easily get to particular parts in    
 *    later processes.  If you can simply read the number of parts   
 *    at the head of the file, then you would probably not build the 
 *    table at this time.                                            
 *-------------------------------------------------------------------*/
int
USERD_get_number_of_model_parts( void )
{
/* 
  for now num_parts is assumed to be 1.  If a variable defined by
  a PM_mapping is a linked list of mappings that could be the way
  that a multi-part model is specified.
*/
#ifdef DEBUG
  printf("USERD_get_number_of_model_parts called\n");
#endif

  int num_parts = 1;
  return(num_parts);
}


/*--------------------------------------------------------------------
 * USERD_get_part_build_info -                                        
 *--------------------------------------------------------------------
 *                                                                    
 *   Gets the info needed for part building process                   
 *                                                                    
 *  (OUT) part_numbers           = Array containing part numbers for  
 *                                  each of the model parts.          
 *                                  IMPORTANT:                        
 *                                   Parts numbers must be >= 1       
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Numparts_available long)         
 *                                                                    
 *  (OUT) part_types             = Array containing one of the        
 *                                  following for each model part:    
 *                                                                    
 *                                       Z_UNSTRUCTURED or            
 *                                       Z_STRUCTURED  or             
 *                                       Z_IBLANKED                   
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Numparts_available long)         
 *                                                                    
 *  (OUT) part_description       = Array containing a description     
 *                                  for each of the model parts       
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Numparts_available by Z_BUFL     
 *                                   long)                            
 *                                                                    
 *  (OUT) number_of_elements     = 2D array containing number of      
 *                                  each type of element for each     
 *                                  unstructured model part.          
 *                                  ------------                      
 *                                  Possible types are:               
 *                                                                    
 *                                Z_POINT   =  point                  
 *                                Z_BAR02   =  2-noded bar            
 *                                Z_BAR03   =  3-noded bar            
 *                                Z_TRI03   =  3-noded triangle       
 *                                Z_TRI06   =  6-noded triangle       
 *                                Z_QUA04   =  4-noded quadrilateral  
 *                                Z_QUA08   =  8-noded quadrilateral  
 *                                Z_TET04   =  4-noded tetrahedron    
 *                                Z_TET10   = 10-noded tetrahedron    
 *                                Z_PYR05   =  5-noded pyramid        
 *                                Z_PYR13   = 13-noded pyramid        
 *                                Z_PEN06   =  6-noded pentahedron    
 *                                Z_PEN15   = 15-noded pentahedron    
 *                                Z_HEX08   =  8-noded hexahedron     
 *                                Z_HEX20   = 20-noded hexahedron     
 *                                                                    
 *                               (Ignored unless Z_UNSTRUCTURED type) 
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Numparts_available by            
 *                                   Z_MAXTYPE long)                  
 *                                                                    
 *  (OUT) ijk_dimensions         = 2D array containing ijk dimensions 
 *                                  for each structured model part.   
 *                                           ----------               
 *                                  (Ignored if Z_UNSTRUCTURED type)  
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Numparts_available by 3 long)    
 *                                                                    
 *                             ijk_dimensions[][0] = I dimension      
 *                             ijk_dimensions[][1] = J dimension      
 *                             ijk_dimensions[][2] = K dimension      
 *                                                                    
 *  (OUT) iblanking_options      = 2D array containing iblanking      
 *                                  options possible for each         
 *                                  structured model part.            
 *                                  ----------                        
 *                                  (Ignored unless Z_IBLANKED type)  
 *                                                                    
 *                                  (Array will have been allocated   
 *                                   Numparts_available by 6 long)    
 *                                                                    
 *      iblanking_options[][Z_EXT]     = TRUE if external (outside)   
 *                       [][Z_INT]     = TRUE if internal (inside)    
 *                       [][Z_BND]     = TRUE if boundary             
 *                       [][Z_INTBND]  = TRUE if internal boundary    
 *                       [][Z_SYM]     = TRUE if symmetry surface     
 *                                                                    
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * If you haven't built a table of pointers to the different parts,
 *    you might want to do so here as you gather the needed info.     
 *                                                                    
 *  * This will be based on Current_time_step                         
 *--------------------------------------------------------------------*/
int
USERD_get_part_build_info(int *part_numbers,
                          int *part_types,
                          char **part_description,
                          int *number_of_elements[Z_MAXTYPE],
                          int *ijk_dimensions[3],
                          int *iblanking_options[6])
{
#ifdef DEBUG
  printf("USERD_get_part_build_info called\n");
#endif

  part_numbers[0] = 1;

  part_types[0]   = Z_STRUCTURED;

  strcpy(part_description[0], "Structured Mesh");

  ijk_dimensions[0][0] = kmax;
  ijk_dimensions[0][1] = lmax;
  ijk_dimensions[0][2] = mmax;

  return(Z_OK);
}


/*--------------------------------------------------------------------
 * USERD_get_variable_value_at_specific -                                 
 *--------------------------------------------------------------------
 *                                                                    
 *  if Z_PER_NODE:
 *    Get the value of a particular variable at a particular node in a
 *    particular part at a particular time.
 *
 *  or if Z_PER_ELEM:
 *    Get the value of a particular variable at a particular element of
 *    a particular type in a particular part at a particular time.
 *                                                                    
 *  (IN)  which_var   = Which variable                                
 *                                                                    
 *  (IN)  which_node_or_elem
 *
 *                     If Z_PER_NODE:
 *                       = The node number.  This is not the id, but is
 *                                           the index of the global node 
 *                                           list (1 based), or the block's
 *                                           node list (1 based).
 *
 *                           Thus,  coord_array[1]
 *                                  coord_array[2]
 *                                  coord_array[3]
 *                                       .      |
 *                                       .      |which_node_or_elem index
 *                                       .             ----
 *
 *
 *                     If Z_PER_ELEM:
 *                       = The element number. This is not the id, but is
 *                                             the element number index
 *                                             of the number_of_element array
 *                                             (see USERD_get_part_build_info),
 *                                             or the block's element list
 *                                             (1 based).
 *
 *                           Thus,  for which_part:
 *                                  conn_array[which_elem_type][0]
 *                                  conn_array[which_elem_type][1]
 *                                  conn_array[which_elem_type][2]
 *                                       .                      |
 *                                       .          which_node_or_elem index
 *                                       .                        ----
 *
 *
 *  (IN)  which_part
 *
 *                       If Z_PER_NODE, or block part:
 *                         = Not used
 *
 *                       If Z_PER_ELEM:
 *                         = The part number
 *
 *  (IN)  which_elem_type
 *
 *                       If Z_PER_NODE, or block part:
 *                         = Not used
 *
 *                       If Z_PER_ELEM:
 *                         = The element type.  This is the element type index
 *                                              of the number_of_element array
 *                                              (see USERD_get_part_build_info)
 *
 *  (IN)  time_step   = Time step to use                              
 *                                                                    
 *  (OUT) values      = scalar or vector component value(s)           
 *                       values[0] = scalar or vector[0]              
 *                       values[1] = vector[1]                        
 *                       values[2] = vector[2]                        
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * This routine is used in node querys over time.  If that         
 *    operation is not critical to you, this can be a dummy routine.  
 *--------------------------------------------------------------------*/
int
USERD_get_variable_value_at_specific(int which_var,
                                     int which_node_or_elem,
                                     int which_part,
                                     int which_elem_type,
                                     int time_step,
                                     float values[3])
{
#ifdef DEBUG
  printf("USERD_get_variable_value_at_specific called\n");
#endif

  return(Z_OK);
}


/*--------------------------------------------------------------------
 * USERD_stop_part_building -                                         
 *--------------------------------------------------------------------
 *                                                                    
 *   Called when part builder is closed for USERD, can be used to     
 *   clean up memory, etc.                                            
 *--------------------------------------------------------------------*/
void
USERD_stop_part_building( void )
{
#ifdef DEBUG
  printf("USERD_stop_part_building called\n");
#endif

  return;
}


/*--------------------------------------------------------------------
 * USERD_bkup -                                                       
 *--------------------------------------------------------------------
 *                                                                    
 *   Used in the archive process.  Save or restore info relating to   
 *   your user defined reader.                                        
 *                                                                    
 *  (IN)  archive_file         = The archive file pointer             
 *                                                                    
 *  (IN)  backup_type          = Z_SAVE_ARCHIVE for saving archive    
 *                               Z_REST_ARCHIVE for restoring archive 
 *                                                                    
 *  returns: Z_OK  if successful                                      
 *           Z_ERR if not successful                                  
 *                                                                    
 *  Notes:                                                            
 *  * Since EnSight's archive file is saved in binary form, it is     
 *    suggested that you also do any writing to it or reading from it 
 *    in binary.                                                      
 *                                                                    
 *  * You should archive any variables, that will be needed for       
 *    future operations, that will not be read or computed again      
 *    before they will be needed.  These are typically global         
 *    variables.                                                      
 *                                                                    
 *  * Make sure that the number of bytes that you write on a save and 
 *    the number of bytes that you read on a restore are identical!!  
 *                                                                    
 *  * And one last reminder.  If any of the variables you save are    
 *    allocated arrays, you must do the allocations before restoring  
 *    into them.                                                      
 *--------------------------------------------------------------------*/
int
USERD_bkup(FILE *archive_file,
           int backup_type)
{
  return(Z_OK);
}
/*-----------------------------------------------------------*/


/* PDB Reader specific routines */

PM_mapping *Get_requested_mapping(int varnum, int blocknum)
   {PM_mapping *mapping = NULL;
    int i;

#ifdef DEBUG
  printf("Inside Get_requested_mapping\n");
  if (varlist == NULL)
     printf("varlist == NULL\n");
  else
     {printf("varlist != NULL\n");
      printf("varlist[%d].indirect: %d\n", varnum, varlist[varnum].indirect);}
#endif

    if (!varlist[varnum].indirect)
       {mapping = FMAKE(PM_mapping, "Get_requested_mapping:mapping");    
        if (!PD_read(cur_file, varlist[varnum].name, mapping))
           {printf("Error 1 getting requested mapping\n");
            return (NULL);};}
    else
       {if (!PD_read(cur_file, varlist[varnum].name, &mapping))
           {printf("Error 2 getting requested mapping\n");
            return (NULL);};}

/* It is assumed that multiple block parts will be stored as */
/* a linked list of PM_mappings.                             */
#ifdef DEBUG
  printf("Finished read\n");
  if (mapping == NULL)
     printf("mapping == NULL\n");
#endif

    for (i = 1; i < blocknum, mapping->next != NULL; i++)
        mapping = mapping->next;
    
    return (mapping);}
/*-----------------------------------------------------------*/

int Free_mapping(PM_mapping *m)
   {
    if (m != NULL)
       {SFREE(m->name);
        SFREE(m->category);
        Free_set(m->domain);
        Free_set(m->range);
        SFREE(m);}

/* GOTCHA - need to handle m->map which is a pcons * */
    return (TRUE);}

/*-----------------------------------------------------------*/

int Free_set(PM_set *s)
   {
    if (s != NULL)
       {SFREE(s->name);
        SFREE(s->element_type);
        SFREE(s->max_index);
        SFREE(s->element_type);
        SFREE(s->elements);
        SFREE(s->es_type);
        SFREE(s->extrema);
        SFREE(s->scales);
        SFREE(s->metric);
        SFREE(s->symmetry_type);
        SFREE(s->symmetry);
        SFREE(s->topology_type);
        SFREE(s->topology);
        SFREE(s->info_type);
        SFREE(s->info);
        SFREE(s);}

    return (TRUE);}

/*-----------------------------------------------------------*/

int Get_coords_by_component(PM_mapping *m, int which_component, 
                            float *coord_array)
   {int ierr = 1, npts;
    PM_set *dom;
    REAL **r, *rcomp;
    char *mtype, *s, bf[MAXLINE];

#ifdef DEBUG
  printf("Inside Get_coords_by_compponent\n");
#endif

    dom = m->domain;
    r = (REAL **)dom->elements;   

#ifdef DEBUG
  if (r == NULL)
     printf("r == NULL\n");
#endif

    switch (which_component)
       {case Z_COMPX :
#ifdef DEBUG
  printf("X component requested\n");
#endif

             rcomp = r[0];
             break;
        case Z_COMPY :
#ifdef DEBUG
  printf("Y component requested\n");
#endif

             rcomp = r[1];
             break;
        case Z_COMPZ :
#ifdef DEBUG
  printf("Z component requested\n");
#endif

             rcomp = r[2];
             break;
	default      :
             printf("Unexpected component requested\n");
             ierr = 1;
             return (ierr);};

    strcpy(bf, dom->element_type);    
    mtype = SC_strtok(bf, " *", s);
    npts = dom->n_elements;

#ifdef DEBUG
  printf("npts = %d, kmax = %d, lmax = %d, mmax = %d\n",
          npts, kmax, lmax, mmax);
#endif


    if (strcmp(mtype, SC_FLOAT_S) == 0)
      {memcpy(coord_array, rcomp, npts * sizeof(float));}
    else
      {CONVERT(SC_FLOAT_S, &coord_array, mtype, rcomp, npts, FALSE);}

    return (ierr);}

/*-----------------------------------------------------------*/

int Get_scalars(PM_mapping *m, float *scalar_array)
   {int ierr = 0, npts;
    PM_set *ran;
    REAL **r;
    char *mtype, *s, bf[MAXLINE];

    ran = m->range;
    r = (REAL **)ran->elements;
    npts = ran->n_elements;
    
    if (npts != ((kmax-1)*(lmax-1)*(mmax-1)))
       {printf("Unexpected number of points in mapping range\n");
        ierr = 1;
        return (ierr);}

    strcpy(bf, ran->element_type);
    mtype = SC_strtok(bf, " *", s);
    
    if (strcmp(mtype, SC_FLOAT_S) == 0)
       memcpy(scalar_array, r[0], npts * sizeof(float));
    else
       CONVERT(SC_FLOAT_S, &scalar_array, mtype, r[0], npts, FALSE);

    return (ierr);}

/*-----------------------------------------------------------*/

var_info *Get_3D_mappings(char **tvarlist, int nvariables)
   {var_info *retlist = NULL;
    PM_mapping *pmapping, **ppmapping, mapping;
    int i, *maxes;
    long number = 0;
    dimdes *dim;
    syment *ep;
    char *type;
    char path[MAXLINE];   
/* 
  have to account for reading everything from single mappings written
  either through pointers or statically allocated to arrays of mappings
  either static or indirect
*/
    retlist = FMAKE_N(var_info, nvariables, "Get_3D_mappings:retlist");

    for (i = 0; i < nvariables; i++)
        {if ((ep = PD_inquire_entry(cur_file, tvarlist[i], TRUE, path)) == NULL)
            {printf("couldn't get syment for %s\n", tvarlist[i]);
             return (NULL);}
         
         type = PD_entry_type(ep);
         dim  = PD_entry_dimensions(ep);
         if (dim != NULL)
            number = _PD_comp_num(dim);

         pmapping = NULL;

         if (_PD_indirection(type))
            {if (dim != NULL)
                { /* Arrays of pointers to PM_mappings not supported at this time */
                  printf("Skipping %s -- Arrays of pointers to PM_mappings not supported\n", tvarlist[i]);}
             else
                {/* Do a simple read into the pointer */
                 if (!PD_read(cur_file, tvarlist[i], &pmapping))
                    {printf("Couldn't read %s : Get_3D_mappings\n", tvarlist[i]);};};}
         else
            {if (dim != NULL)
                { /* arrays of PM_mappings not supported at this time */
                  printf("Skipping %s -- Arrays of PM_mappings not supported\n", tvarlist[i]);}
             else
                {if (!PD_read(cur_file, tvarlist[i], &mapping))
                    {printf("Couldn't read %s : Get_3D_mappings\n", tvarlist[i]);}
                 else
                    {pmapping = &mapping;};};}

#ifdef DEBUG
  printf("Get_3D_mappings read variable %s \n", tvarlist[i]);
  if (pmapping == NULL)
     printf("pmapping == NULL\n");
  else
    {printf("pmapping != NULL, ...->dimension_elem = %d\n",
            pmapping->domain->dimension_elem );}
#endif

         if (pmapping != NULL)
            {if (pmapping->domain->dimension_elem == 3)
                {if (Num_variables == 0)
                    {maxes = pmapping->domain->max_index;
                     kmax = maxes[0];
                     lmax = maxes[1];
                     mmax = maxes[2];}
                 if (pmapping->range->n_elements == kmax*lmax*mmax)
                    {retlist[Num_variables].classify = Z_PER_NODE;}
                 else if (pmapping->range->n_elements == (kmax-1)*(lmax-1)*(mmax-1))
                    {retlist[Num_variables].classify = Z_PER_ELEM;}
                 else
                    {printf("Unclassifiable variable %s\n", tvarlist[i]);
                     continue;}
                 retlist[Num_variables].name = SC_strsavef(tvarlist[i], "Get_3D_mappings:retlist.name");
                 retlist[Num_variables].indirect = (_PD_indirection(type)) ? TRUE : FALSE;
                 strcpy(retlist[Num_variables].description, pmapping->range->name);
                 strcpy(retlist[Num_variables].filename, current_file);
         /* how would a vector type be represented in a mapping? */
                 retlist[Num_variables].type = Z_SCALAR;
#ifdef DEBUG
  printf("Incrementing Num_variables\n");
#endif
                 Num_variables++;};};}
    
#ifdef DEBUG
  printf("Returning from Get_3D_mappings with Num_variables = %d\n", Num_variables);
#endif
    return (retlist);}

/*-------------------- End of libuserd.c --------------------*/







