/*
 * PXRDTS.C - Test attribute features of PDBX (READ version)
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "hdf5.h"
#include "cpyright.h"

#include "pdb.h"

#define CONTAINS_INDIRECTION -1
#define UNKNOWN_TYPE         -2
#define HASH_ERROR           -3
int 
 debug = 1;   /* debug flag */

HASHTAB
 *HDF_ctypes = NULL;

char
 *HID_T_S;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help()
   {PRINT(STDOUT, "\nPDB2HDF - translate a pdb file to an hdf file\n\n");
    PRINT(STDOUT, "Usage: pdb2hdf file\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       where file is a pdbfile\n");
    PRINT(STDOUT, "       output file will be named file.pdb\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*
   PDB_TO_HDF_DIMS - return a dimension list suitable for hdf usage.
                     it is assumed that there is at least 1 dimension.
*/

static hsize_t *PDB_to_HDF_dims(dm, rank)
   dimdes *dm;
   int *rank;
   {dimdes *lst;
    hsize_t *ret;
    int i, j;

    for (lst = dm, i = 0; lst != NULL; lst = lst->next, i++);

    ret =  FMAKE_N(hsize_t, i, "PDB_TO_HDF_DIMS:ret");

    for (lst = dm, j = 0; j < i; lst = lst->next, j++)
        ret[j] = lst->number;

    *rank = i;

    return (ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PDB_TO_HDF_PTYPE - return the hdf equivalent of a pdb
 *                  - primitive type
 */

static int PDB_to_HDF_ptype(defstr *dp, hid_t *datatype)
   {int id, rv;
    char s[MAXLINE];
    static hid_t hfx[] = { H5T_NATIVE_SHORT, H5T_NATIVE_INT,
			   H5T_NATIVE_LONG,  H5T_NATIVE_LONG_LONG };
    static hid_t hfp[] = { H5T_NATIVE_FLOAT, H5T_NATIVE_DOUBLE,
			   H5T_NATIVE_LONG_DOUBLE };

    strcpy(s, dp->type);
    id = SC_type_id(s, FALSE);

    rv = UNKNOWN_TYPE;

    if (_PD_indirection(s))
       rv = CONTAINS_INDIRECTION;

/* check for floating point types (proper) */
    else if (dp->format != NULL)
       {if (SC_is_type_fp(id) == TRUE)
           {*datatype = hfp[id-SC_FLOAT_I];
	    rv        = TRUE;};}

/* check for fixed point types (proper) */
    else if (dp->convert >= 0)
       {if (SC_is_type_fix(id) == TRUE)
           {*datatype = hfx[id-SC_SHORT_I];
	    rv        = TRUE;}

	else if (id == SC_CHAR_I)
	   {*datatype = H5T_NATIVE_CHAR;
	    rv        = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*
   PDB_TO_HDF_CTYPE - return the hdf equivalent of a pdb
                      compound type.
                      after constructing the type save a
                      copy in a hash table to look up the next
                      time.
*/

static int PDB_to_HDF_ctype(pdb_file, dp, datatype)
   PDBfile *pdb_file;
   defstr *dp;
   hid_t  *datatype;
   {hid_t *stype, dtype, nested_dtype;
    hsize_t *dimsf;
    int rank, err;
    memdes *lst, *nxt;
    defstr *mdp;
    dimdes *dm;

    if (dp->n_indirects > 0)
       return(CONTAINS_INDIRECTION);

/* make the static compound types hash table */
    if (HDF_ctypes == NULL)
       {HDF_ctypes = SC_make_hash_table(HSZLARGE, NODOC);
        HID_T_S = SC_strsavef("hid_t *", "char*:PDB_to_HDF_ctype:hid_t");}

/* look for the type in the hash table, if there return a copy  */
    if ((stype = (hid_t *) SC_def_lookup(dp->type, HDF_ctypes))
                  != NULL)
       {*datatype = H5Tcopy(*stype);
        return(TRUE);}
       
    dtype = H5Tcreate(H5T_COMPOUND, dp->size);

    for (lst = dp->members; lst != NULL; lst = nxt)
        {nxt = lst->next;
         mdp = PD_inquire_type(pdb_file, lst->type);

         if (mdp->members != NULL)
            err = PDB_to_HDF_ctype(pdb_file, mdp, &nested_dtype);
         else
            err = PDB_to_HDF_ptype(mdp, &nested_dtype);

         if (err < 0)
            {if (err == CONTAINS_INDIRECTION)
                    {return(CONTAINS_INDIRECTION);}
                 else
                    {return(UNKNOWN_TYPE);};}
         
         if ((dm = lst->dimensions) == NULL)
            {H5Tinsert(dtype, lst->name, lst->member_offs, nested_dtype);}
         else
            {int n;
             hid_t atype;

             dimsf = PDB_to_HDF_dims(dm, &rank);
             n = min(rank, 4);

             atype = H5Tarray_create(nested_dtype, n, dimsf, NULL);
             H5Tinsert(dtype, lst->name, lst->member_offs, atype);

             SFREE(dimsf);};}

/* return the new type, save a copy in the hash table */
    stype = FMAKE(hid_t, "PDB_to_HDF_ctyp:stype");
    *stype = H5Tcopy(dtype);
    if (SC_install(dp->type, stype, HID_T_S, HDF_ctypes)
                  == NULL)
       {return(HASH_ERROR);}

    *datatype = dtype;

    return(TRUE);}
                              

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* 
   PDB_TO_HDF_TYPE - return the hdf equivalent of the pdb type
*/

static int PDB_to_HDF_type(pdb_file, dp, datatype)
   PDBfile *pdb_file;
   defstr *dp;
   hid_t  *datatype;
   {
    if (dp->members == NULL)
       return(PDB_to_HDF_ptype(dp, datatype));
    else
       return(PDB_to_HDF_ctype(pdb_file, dp, datatype));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* 
   TRANSLATE_ENTRY - write the hdf equivalent of a pdb variable to a file
*/

static int translate_entry(pdb_file, hdf_file, var)
   PDBfile *pdb_file;
   hid_t    hdf_file;
   char *var;
   {syment *ep;
    defstr *dp;
    dimdes *dm;
    hid_t dataspace, dataset, datatype;
    char *type, path[MAXLINE];
    hsize_t *dimsf;
    herr_t status;
    int rank, err;
    char *pdb_data;
    long number;

    ep = PD_inquire_entry(pdb_file, var, TRUE, path);
    if (ep == NULL)
       {printf("couldn't get syment for %s -- translate_entry\n", var);
        return(FALSE);}

    type = PD_entry_type(ep);
    if (_PD_indirection(type))
       {printf("Skipping variable %s, type: %s, contains indirections\n",
                   path, type);
        return(FALSE);}

    if ((dp = PD_inquire_host_type(pdb_file, type)) == NULL)
       {printf("Skipping variable %s, type: %s, type look up failed\n",
               path, type);
        return(FALSE);}

    if ((err = PDB_to_HDF_type(pdb_file, dp, &datatype)) < 0)
       {if (err == CONTAINS_INDIRECTION)
           {printf("Skipping variable %s, type: %s, contains indirections\n",
                   path, type);}
        else
           {printf("Skipping variable %s, type: %s, unknown type\n",
                   path, type);}
        return(FALSE);}

    if ((dm = PD_entry_dimensions(ep)) == NULL)
       {dataspace = H5Screate(H5S_SCALAR);}
    else
       {dimsf = PDB_to_HDF_dims(dm, &rank);
        dataspace = H5Screate_simple(rank, dimsf, NULL);
        SFREE(dimsf);}

/*     This needs some logic for handling the group (directory) */
    dataset   =  H5Dcreate(hdf_file, path, datatype, dataspace, H5P_DEFAULT); 

/*     
   See SX_read_filedata in sxpdb.c for memory allocation 
   end read logic.  
*/

    number = PD_entry_number(ep);
    pdb_data =  _PD_alloc_entry(pdb_file, type, number);

    if (!(err = _PD_hyper_read(pdb_file, path, type, ep, pdb_data)))
       {printf("Skipping variable %s, error reading\n", path);
        H5Sclose(dataspace);
        H5Dclose(dataset);
        return(FALSE);}   

    status    = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL,
		          H5P_DEFAULT, pdb_data);

    if (status < 0)
       printf("H5Dwrite error writing variable %s\n", path);
                   
    H5Sclose(dataspace);
    H5Dclose(dataset);
    SFREE(pdb_data);

    return((status < 0) ? FALSE : TRUE);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*
   NEW_SUFFIX - Replace a suffix (insuffix) if it exists with another
                suffix (outsuffix).
                If insuffix is not there append outsuffix.
*/

static char *new_suffix(in, insuffix, outsuffix)
   char *in, *insuffix, *outsuffix;
   {int found = TRUE;
    char in_rev[MAXLINE], insuffix_rev[MAXLINE];
    char *out;
    int len_in, len_insuffix, len_out;

    if (in == NULL)
       return(char *)NULL;

/* look for insuffix at the end of in */
    SC_strrev(strcpy(in_rev, in));
    SC_strrev(strcpy(insuffix_rev, insuffix));

    if (strstr(in_rev, insuffix_rev) != in_rev)
       found = FALSE;

    len_in       = strlen(in);
    len_insuffix = strlen(insuffix);
    len_out      = (found) ? len_in - len_insuffix + strlen(outsuffix) + 1:
                             len_in + strlen(outsuffix) + 1;

    out =  FMAKE_N(char, len_out, "NEW_SUFFIX:out");

    if (found)
       strncpy(out, in, len_in - len_insuffix);
    else
       strcpy(out, in);

    strcat(out, outsuffix);

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*
   TRANSLATE_DIR - translate the contents of a directory.
                   if another directory is encountered 
                   process it recursively.
*/
static int translate_dir(pdb_file, hdf_file, directory)
   PDBfile *pdb_file;
   hid_t    hdf_file;
   char *directory;
   {char **names;
    char *cwd;
    hid_t grp;
    int i, nnames;

    if (!(PD_cd(pdb_file, directory)))
       {printf("couldn't process directory %s\n", directory);
        return(FALSE);}

/* get the full path of the current directory */ 
    cwd = PD_pwd(pdb_file);

/* create a group in the hdf file */
    if ((strcmp(cwd, "/")))
       grp = H5Gcreate(hdf_file, cwd, 0);

    names = PD_ls(pdb_file, NULL, NULL, &nnames);
    if (debug)
       {printf("PD_ls returned %d names in directory %s\n", 
               nnames, directory);}

    for (i = 0; i < nnames; i++)
        {if (names[i][strlen(names[i]) - 1] == '/')
            {if (!(strcmp(names[i], "&ptrs/")))
                continue;
             translate_dir(pdb_file, hdf_file, names[i]);}
         else
            translate_entry(pdb_file, hdf_file, names[i]);}
              
    PD_cd(pdb_file, "..");
    return(TRUE);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(argc, argv)
   int argc;
   char **argv;
   {PDBfile *pdb_file;
    hid_t hdf_file;
    char *hdf_name;

    if (argc == 2)
       {if ((pdb_file = PD_open(argv[1], "r")) == NULL)
          {printf("Error opening %s\n", argv[1]);
           print_help();
	   exit(1);};}
    else
       {print_help();
        exit(1);}

    SC_zero_space(1);

/* generate a name for the output file */
    hdf_name = new_suffix(argv[1], ".pdb", ".hdf");

/* create the hdf output file */
    if ((hdf_file = H5Fcreate(hdf_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT))
                    < 0)
       {printf("Error creating HDF output file %s\n", hdf_name);
        exit(2);}

/* start translating at the root directory */
    if (!(translate_dir(pdb_file, hdf_file, "/")))
       {printf("error translating file %s, directory %s\n", argv[1], "/");
        exit(3);}

#if 0
    files = PD_ls(pdb_file, NULL, NULL, &nfiles);
    
    printf("PD_ls returned %d files in root directory\n", nfiles);

    for (i = 0; i < nfiles; i++)
        {
         if ((ep = PD_inquire_entry(pdb_file, files[i], FALSE, NULL)))
            {printf("name: %s\ttype: %s\t", files[i], PD_entry_type(ep));
             if ((dm = PD_entry_dimensions(ep)) != NULL)
                {printf("dimensions: (");
                 while (TRUE)
                    {printf("%ld : %ld", dm->index_min, dm->index_max);
                     if ((dm = dm->next) == NULL)
                        {printf(")");
                         break;}
                     else
                        {printf(" , ");};};}
             printf("\n");}
         else
            printf("Couldn't get syment for %s\n", files[i]);}  
#endif

/* close files */
    PD_close(pdb_file);
    H5Fclose(hdf_file);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
