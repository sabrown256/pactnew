/*
 *  RESTART-BENCH.C
 *
 *  Author: Greg Smethells
 *
 *  Benchmark restart dumps using PDB and HDF5 interfaces
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "restart-bench.h"

#define NUM_VARS 7
#define OPTIONS  "abdhi:m:n:p:r:t:v:w:"

/* GLOBAL */

char* sysname;

/* GLOBAL data to dump into restart file */
int*   i32PrimitiveWrite;
long*  i64PrimitiveWrite;
float* f32PrimitiveWrite;
double* f64PrimitiveWrite;
testStruct* testStructWrite;
nestedStruct* nestedStructWrite;
indirectStruct* indirectStructWrite;

/* GLOBAL space to read restart data into */
int*   i32PrimitiveRead;
long*  i64PrimitiveRead;
float* f32PrimitiveRead;
double* f64PrimitiveRead;
testStruct* testStructRead;
nestedStruct* nestedStructRead;
indirectStruct* indirectStructRead;



/* timer_sub
 *
 * first  -- left-hand operand to subtraction operator
 * second -- right-hand operand to subtraction operator
 *
 * Subtract second from first returning answer 
 */
struct timeval timer_sub( struct timeval first, struct timeval second ) {
  struct timeval answer;

/*  fprintf(stderr, "\n\n%ld sec %ld usec - %ld sec %ld usec",
                 first.tv_sec, first.tv_usec, second.tv_sec, second.tv_usec); */

  if( second.tv_usec > first.tv_usec ) {
    answer.tv_usec = (first.tv_usec - second.tv_usec) + 1000000L;
    answer.tv_sec  = (first.tv_sec - 1L) - second.tv_sec;
  }
  else {
    answer.tv_usec = first.tv_usec - second.tv_usec;
    answer.tv_sec  = first.tv_sec  - second.tv_sec;
  }

/*  fprintf(stderr, " = %ld sec %ld usec\n", answer.tv_sec, answer.tv_usec ); */

  return( answer );
}


/* timer_div 
 *
 * dividend -- left-hand operand to division operator
 * divisor  -- right-hand operand to division operator
 *
 * Divide dividend by divisor returning answer 
 */
struct timeval timer_div( struct timeval dividend, long divisor ) {
  struct timeval answer;

/*  fprintf(stderr, "\n\n%ld sec %ld usec / %ld", 
                      dividend.tv_sec, dividend.tv_usec, divisor); */

  answer.tv_usec = ((long)(dividend.tv_sec % divisor)*1000000L + 
                                                 dividend.tv_usec) / divisor;
  answer.tv_sec  = dividend.tv_sec / divisor;

/*  fprintf(stderr, " = %ld sec %ld usec\n", answer.tv_sec, answer.tv_usec ); */

  return( answer );
}


/* prepDataToWrite
 *
 * Fill our global data variables with arbitrary values 
 *
 * size     -- number of positions in each array to be allocated
 * prevSize -- value of "size" previously 
 *
 * Use the SCORE mem mgr MACROs because we may include indirections 
 * in our benchmark, and the PDB side requires the use of SCORE 
 * if it is to handle writing out structs with pointers 
 */
void prepDataToWrite( int size, int prevSize ) {
  int j, k;

  /* Assume if one is != NULL, they are all != NULL */
  /* Free up the old memory */
  if( i32PrimitiveWrite != NULL ) {
    SFREE_N(i32PrimitiveWrite, prevSize);
    SFREE_N(i64PrimitiveWrite, prevSize);
    SFREE_N(f32PrimitiveWrite, prevSize);
    SFREE_N(f64PrimitiveWrite, prevSize);
    SFREE_N(testStructWrite,   prevSize);
    SFREE_N(nestedStructWrite, prevSize);

    /* Free indirections first */
    for( k = 0 ; k < prevSize ; k++ ) {
      SFREE( indirectStructWrite[k].pt );
      SFREE( indirectStructWrite[k].val );
    }

    SFREE_N(indirectStructWrite, prevSize);
  }

  /* Allocate new memory of the appropriate size */
  i32PrimitiveWrite   = MAKE_N(int, size);
  i64PrimitiveWrite   = MAKE_N(long, size);
  f32PrimitiveWrite   = MAKE_N(float, size);
  f64PrimitiveWrite   = MAKE_N(double, size);
  testStructWrite     = MAKE_N(testStruct, size);
  nestedStructWrite   = MAKE_N(nestedStruct, size);
  indirectStructWrite = MAKE_N(indirectStruct, size);

  for( k = 0 ; k < size ; k++ ) {
   
    /* Setup the primitives */

    i32PrimitiveWrite[k] = (int)42;
    i64PrimitiveWrite[k] = (int)24;
    f32PrimitiveWrite[k] = (float)3.14159;
    f64PrimitiveWrite[k] = (double)6.022E23;
  
   
    /* Setup the structs */

    testStructWrite[k].xCoord = (float)3.14159;
    testStructWrite[k].yCoord = (float)6.022E23;
    testStructWrite[k].zCoord = (float)123456789.0;
    testStructWrite[k].t      = (float)2.995E8;

    for( j = 0 ; j < SMALL_ARRAY_SIZE ; j++ ) {
      testStructWrite[k].alpha[j] = (double)1.1111111;
      testStructWrite[k].beta[j]  = (double)2.2222222;
      testStructWrite[k].gamma[j] = (double)3.3333333;
    }

   
    /* Setup the nested structs */

    nestedStructWrite[k].internalStruct.xCoord = (float)3.14159;
    nestedStructWrite[k].internalStruct.yCoord = (float)6.022E23;
    nestedStructWrite[k].internalStruct.zCoord = (float)123456789.0;
    nestedStructWrite[k].internalStruct.t      = (float)2.995E8;
  
    for( j = 0 ; j < SMALL_ARRAY_SIZE ; j++ ) {
      nestedStructWrite[k].internalStruct.alpha[j] = (double)1.1111111;
      nestedStructWrite[k].internalStruct.beta[j]  = (double)2.2222222;
      nestedStructWrite[k].internalStruct.gamma[j] = (double)3.3333333;
    }

    nestedStructWrite[k].intMember = 3;
    nestedStructWrite[k].doubleMember = (double)3.1415926;


    /* Setup the indirect structs */

    indirectStructWrite[k].t   = (double)3.0; 
    indirectStructWrite[k].pt  = MAKE(pointStruct);
    indirectStructWrite[k].val = MAKE(valStruct);

    indirectStructWrite[k].pt->x = (double)1.0;
    indirectStructWrite[k].pt->y = (double)2.0;
    indirectStructWrite[k].pt->z = (double)3.0;

    indirectStructWrite[k].val->temp = (double)1.23456789;
    indirectStructWrite[k].val->pres = (double)42.0;
  }
}


/* prepDataToRead
 *
 * Fill the global data variables with null/zero values 
 * before reading into them, this will cause the check to
 * fail if we do not read back valid data (assuming valid 
 * data is not a null/zero value in the first place)
 *
 * size                -- number of positions in each array to be allocated
 * prevSize            -- value of "size" previously 
 * includeIndirections -- write indirection var out iff TRUE
 *
 * Use the SCORE mem mgr MACROs because we may include indirections 
 * in our benchmark, and the PDB side requires the use of SCORE 
 * if it is to handle writing out structs with pointers 
 */
void prepDataToRead( int size, int prevSize, int includeIndirections ) {
  int j, k;

  /* Assume if one is != NULL, they are all != NULL */
  /* Free up the old memory */
  if( i32PrimitiveRead != NULL ) {
    SFREE_N(i32PrimitiveRead, prevSize);
    SFREE_N(i64PrimitiveRead, prevSize);
    SFREE_N(f32PrimitiveRead, prevSize);
    SFREE_N(f64PrimitiveRead, prevSize);
    SFREE_N(testStructRead,   prevSize);
    SFREE_N(nestedStructRead, prevSize);
    /* indirectStructRead handled within read function */
  }

  /* Allocate new memory of the appropriate size */
  i32PrimitiveRead   = MAKE_N(int, size);
  i64PrimitiveRead   = MAKE_N(long, size);
  f32PrimitiveRead   = MAKE_N(float, size);
  f64PrimitiveRead   = MAKE_N(double, size);
  testStructRead     = MAKE_N(testStruct, size);
  nestedStructRead   = MAKE_N(nestedStruct, size);
  /* indirectStructRead handled within read function */

  for( k = 0 ; k < size ; k++ ) {
   
    /* Setup the primitives */

    i32PrimitiveRead[k] = (int)0;
    i64PrimitiveRead[k] = (long)0;
    f32PrimitiveRead[k] = (float)0.0;
    f64PrimitiveRead[k] = (double)0.0;
  
   
    /* Setup the structs */

    testStructRead[k].xCoord = (float)0.0;
    testStructRead[k].yCoord = (float)0.0;
    testStructRead[k].zCoord = (float)0.0;
    testStructRead[k].t      = (float)0.0;

    for( j = 0 ; j < SMALL_ARRAY_SIZE ; j++ ) {
      testStructRead[k].alpha[j] = (double)0.0;
      testStructRead[k].beta[j]  = (double)0.0;
      testStructRead[k].gamma[j] = (double)0.0;
    }

   
    /* Setup the nested structs */

    nestedStructRead[k].internalStruct.xCoord = (float)0.0;
    nestedStructRead[k].internalStruct.yCoord = (float)0.0;
    nestedStructRead[k].internalStruct.zCoord = (float)0.0;
    nestedStructRead[k].internalStruct.t      = (float)0.0;
  
    for( j = 0 ; j < SMALL_ARRAY_SIZE ; j++ ) {
      nestedStructRead[k].internalStruct.alpha[j] = (double)0.0;
      nestedStructRead[k].internalStruct.beta[j]  = (double)0.0;
      nestedStructRead[k].internalStruct.gamma[j] = (double)0.0;
    }

    nestedStructRead[k].intMember = (int)0;
    nestedStructRead[k].doubleMember = (double)0.0;
  }
}


/* writeHDF5RestartDump
 *
 * Write some primitives, arrays, and structs to an HDF5 file 
 *
 * machine             -- bit-field flags of machines to convert to
 * size                -- size of the arrays
 * numSets             -- number of sets of NUM_VARS to write to the file 
 * includeIndirections -- write indirection var out iff TRUE
 *
 * We may need to add more error check to properly simulate 
 * reading back a restart dump in real application code 
 */
void writeHDF5RestartDump( int machine, int size, int numSets, int includeIndirections ) {
  int j, k;
  herr_t status;
  hid_t tempFile;
  hid_t convInt_t, convLong_t, convFloat_t, convDouble_t;
  hid_t array_t, arrayConv_t;
  hid_t compound_t, compoundConv_t;
  hid_t nestedCompound_t, nestedCompoundConv_t;
  hid_t pointCompound_t, pointCompoundConv_t;
  hid_t valCompound_t, valCompoundConv_t;
  hid_t dataset;
  hid_t arraySpace;
  hsize_t dim[1], small_dim[1];
  char name[BUFFER_SIZE];
  char filename[BUFFER_SIZE];


  /* Name the file after the arguments */
  snprintf( filename, BUFFER_SIZE, "temp-file-%s-%d-%d-%d-%d.h5", 
             sysname, machine, size, numSets, includeIndirections );

  /* Create a new HDF5 file read-write */
  tempFile = H5Fcreate( filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );

  if( tempFile < 0 ) {
     fprintf(stderr, "\n\nwriteRestartDump: couldn't create hdf5 output file %s\n", filename);
     exit(-1);
  }

  /* Write our data to the file in a converted format if requested */
  if( MIPS & machine ) {
    convInt_t    = H5Tcopy(H5T_STD_I32BE);
    convLong_t   = H5Tcopy(H5T_STD_I64BE);
    convFloat_t  = H5Tcopy(H5T_IEEE_F32BE);
    convDouble_t = H5Tcopy(H5T_IEEE_F64BE);
  } 
  else if( POWERPC & machine ) {
    convInt_t    = H5Tcopy(H5T_STD_I32BE);
    convLong_t   = H5Tcopy(H5T_STD_I32BE);
    convFloat_t  = H5Tcopy(H5T_IEEE_F32BE);
    convDouble_t = H5Tcopy(H5T_IEEE_F64BE);
  } 
  else if( INTEL & machine ) {
    convInt_t    = H5Tcopy(H5T_STD_I32LE);
    convLong_t   = H5Tcopy(H5T_STD_I32LE);
    convFloat_t  = H5Tcopy(H5T_IEEE_F32LE);
    convDouble_t = H5Tcopy(H5T_IEEE_F64LE);
  }
  else if( SPARC & machine ) {  
    convInt_t    = H5Tcopy(H5T_STD_I32BE);
    convLong_t   = H5Tcopy(H5T_STD_I32BE);
    convFloat_t  = H5Tcopy(H5T_IEEE_F32BE);
    convDouble_t = H5Tcopy(H5T_IEEE_F64BE);
  }
  else if( ALPHA & machine ) {
    convInt_t    = H5Tcopy(H5T_STD_I32LE);
    convLong_t   = H5Tcopy(H5T_STD_I64LE);
    convFloat_t  = H5Tcopy(H5T_IEEE_F32LE);
    convDouble_t = H5Tcopy(H5T_IEEE_F64LE);
  }
  else {
    convInt_t    = H5Tcopy(H5T_NATIVE_INT);
    convLong_t   = H5Tcopy(H5T_NATIVE_LONG);
    convFloat_t  = H5Tcopy(H5T_NATIVE_FLOAT);
    convDouble_t = H5Tcopy(H5T_NATIVE_DOUBLE);
  }


  /* Define a NATIVE testStruct compound type */
  compound_t   = H5Tcreate(H5T_COMPOUND, sizeof(testStruct));
  small_dim[0] = SMALL_ARRAY_SIZE;
  array_t      = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, small_dim, NULL);

  H5Tinsert(compound_t, "xCoord", HOFFSET(testStruct, xCoord), H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "yCoord", HOFFSET(testStruct, yCoord), H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "zCoord", HOFFSET(testStruct, zCoord), H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "t",      HOFFSET(testStruct, t),      H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "alpha",  HOFFSET(testStruct, alpha),  array_t);
  H5Tinsert(compound_t, "beta",   HOFFSET(testStruct, beta),   array_t);
  H5Tinsert(compound_t, "gamma",  HOFFSET(testStruct, gamma),  array_t);

  /* Define a TARGETED testStruct compound type */
  if( machine ) {
    compoundConv_t = H5Tcreate(H5T_COMPOUND, 
      (H5Tget_size(convFloat_t)*4 + H5Tget_size(convDouble_t)*SMALL_ARRAY_SIZE*3));
    small_dim[0]   = SMALL_ARRAY_SIZE;
    arrayConv_t    = H5Tarray_create(convDouble_t, 1, small_dim, NULL);

    H5Tinsert(compoundConv_t, "xCoord", 0,                          convFloat_t);
    H5Tinsert(compoundConv_t, "yCoord", H5Tget_size(convFloat_t),   convFloat_t);
    H5Tinsert(compoundConv_t, "zCoord", H5Tget_size(convFloat_t)*2, convFloat_t);
    H5Tinsert(compoundConv_t, "t",      H5Tget_size(convFloat_t)*3, convFloat_t);
    H5Tinsert(compoundConv_t, "alpha",  H5Tget_size(convFloat_t)*4, arrayConv_t);
    H5Tinsert(compoundConv_t, "beta",   
      (H5Tget_size(convFloat_t)*4 + H5Tget_size(convDouble_t)*SMALL_ARRAY_SIZE*1),
      arrayConv_t);
    H5Tinsert(compoundConv_t, "gamma",  
      (H5Tget_size(convFloat_t)*4 + H5Tget_size(convDouble_t)*SMALL_ARRAY_SIZE*2),
      arrayConv_t);
  }
  /* These two compound types are the same if not converting */
  else {
    arrayConv_t    = H5Tcopy(array_t);
    compoundConv_t = H5Tcopy(compound_t);
  }


  /* Define a NATIVE nestedStruct compound type */
  nestedCompound_t = H5Tcreate(H5T_COMPOUND, sizeof(nestedStruct));

  H5Tinsert(nestedCompound_t, "internalStruct",
                      HOFFSET(nestedStruct, internalStruct), compound_t);
  H5Tinsert(nestedCompound_t, "intMember",
                      HOFFSET(nestedStruct, intMember), H5T_NATIVE_INT);
  H5Tinsert(nestedCompound_t, "doubleMember",
                      HOFFSET(nestedStruct, doubleMember), H5T_NATIVE_DOUBLE);

  /* Define a TARGETED nestedStruct compound type */
  if( machine ) {
    nestedCompoundConv_t = H5Tcreate(H5T_COMPOUND, 
      (H5Tget_size(compoundConv_t) + H5Tget_size(convInt_t)) + H5Tget_size(convDouble_t));

    H5Tinsert(nestedCompoundConv_t, "internalStruct", 0, compoundConv_t);
    H5Tinsert(nestedCompoundConv_t, "intMember",
                        H5Tget_size(compoundConv_t), convInt_t);
    H5Tinsert(nestedCompoundConv_t, "doubleMember",
                       (H5Tget_size(compoundConv_t) + H5Tget_size(convInt_t)), 
                        convDouble_t);
  }
  /* These two compound types are the same if not converting */
  else {
    nestedCompoundConv_t = H5Tcopy(nestedCompound_t);
  }


  /* Define a NATIVE "indirect" compound type */
  if( includeIndirections ) {
    pointCompound_t = H5Tcreate(H5T_COMPOUND, 3*H5Tget_size(H5T_NATIVE_DOUBLE));
    valCompound_t   = H5Tcreate(H5T_COMPOUND, 2*H5Tget_size(H5T_NATIVE_DOUBLE));
  
    H5Tinsert(pointCompound_t, "x", HOFFSET(pointStruct, x), H5T_NATIVE_DOUBLE);
    H5Tinsert(pointCompound_t, "y", HOFFSET(pointStruct, y), H5T_NATIVE_DOUBLE);
    H5Tinsert(pointCompound_t, "z", HOFFSET(pointStruct, z), H5T_NATIVE_DOUBLE);
  
    H5Tinsert(valCompound_t, "temp", HOFFSET(valStruct, temp), H5T_NATIVE_DOUBLE);
    H5Tinsert(valCompound_t, "pres", HOFFSET(valStruct, pres), H5T_NATIVE_DOUBLE);
  
    /* Define a TARGETED "indirect" compound type */
    if( machine ) {
      pointCompoundConv_t = H5Tcreate(H5T_COMPOUND, 3*H5Tget_size(convDouble_t));
      valCompoundConv_t   = H5Tcreate(H5T_COMPOUND, 2*H5Tget_size(convDouble_t));
  
      H5Tinsert(pointCompoundConv_t, "x", 0, convDouble_t);
      H5Tinsert(pointCompoundConv_t, "y", H5Tget_size(convDouble_t), convDouble_t);
      H5Tinsert(pointCompoundConv_t, "z", 2*H5Tget_size(convDouble_t), convDouble_t);
    
      H5Tinsert(valCompoundConv_t, "temp", 0, convDouble_t);
      H5Tinsert(valCompoundConv_t, "pres", H5Tget_size(convDouble_t), convDouble_t);
    }
    else {
      pointCompoundConv_t = H5Tcopy(pointCompound_t);
      valCompoundConv_t   = H5Tcopy(valCompound_t);
    }
  }
 
  /* Write out each of the variations of the variables */ 
  for( j = 0 ; j < numSets ; j++ ) {
    snprintf(name, BUFFER_SIZE, "i32Primitive%d", j);
 
    /* int */
    dim[0] = size;
    arraySpace = H5Screate_simple(1, dim, NULL);
    dataset = H5Dcreate(tempFile, name, convInt_t, arraySpace, H5P_DEFAULT);
   
    /* file type and space is held in dataset */
    /* memory type and space is specified explicitly */ 
    status = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, 
                          H5S_ALL, H5P_DEFAULT, i32PrimitiveWrite);
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Sclose(arraySpace);
    H5Dclose(dataset);

  
    /* long */
    snprintf(name, BUFFER_SIZE, "i64Primitive%d", j);

    dim[0] = size;
    arraySpace = H5Screate_simple(1, dim, NULL);
    dataset = H5Dcreate(tempFile, name, convLong_t, arraySpace, H5P_DEFAULT);
   
    /* file type and space is held in dataset */
    /* memory type and space is specified explicitly */ 
    status = H5Dwrite(dataset, H5T_NATIVE_LONG, H5S_ALL, 
                          H5S_ALL, H5P_DEFAULT, i64PrimitiveWrite);
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Sclose(arraySpace);
    H5Dclose(dataset); 


    /* float */
    snprintf(name, BUFFER_SIZE, "f32Primitive%d", j);

    /* NOTE: We do a full construction of the dataspace and dataset from basic types
     *       each time to simulate the amount of work done in real application codes
     *       This applies to everything from primitives to nested structs.
     */
    dim[0] = size;
    arraySpace = H5Screate_simple(1, dim, NULL);
    dataset = H5Dcreate(tempFile, name, convFloat_t, arraySpace, H5P_DEFAULT );
  
    status = H5Dwrite(dataset, H5T_NATIVE_FLOAT, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, f32PrimitiveWrite);
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Sclose(arraySpace);
    H5Dclose(dataset);
  
  
    /* double */
    snprintf(name, BUFFER_SIZE, "f64Primitive%d", j);

    dim[0] = size;
    arraySpace = H5Screate_simple(1, dim, NULL);
    dataset = H5Dcreate(tempFile, name, convDouble_t, arraySpace, H5P_DEFAULT );
  
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, f64PrimitiveWrite );
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Sclose(arraySpace);
    H5Dclose(dataset);
    
  
    /* struct */
    snprintf(name, BUFFER_SIZE, "testStruct%d", j);

    dim[0] = size;
    arraySpace = H5Screate_simple(1, dim, NULL);
    dataset = H5Dcreate(tempFile, name, 
                          compoundConv_t, arraySpace, H5P_DEFAULT);
  
    status = H5Dwrite(dataset, compound_t, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, testStructWrite);
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Sclose(arraySpace);
    H5Dclose(dataset);
  
    /* nested struct */
    snprintf(name, BUFFER_SIZE, "nestedStruct%d", j);

    dim[0] = size;
    arraySpace = H5Screate_simple(1, dim, NULL);
    dataset = H5Dcreate(tempFile, name, 
                          nestedCompoundConv_t, arraySpace, H5P_DEFAULT);
  
    status = H5Dwrite(dataset, nestedCompound_t, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, nestedStructWrite);
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }

    H5Sclose(arraySpace);
    H5Dclose(dataset);

    /* indirect struct */

    if( includeIndirections ) {
      dim[0] = 1;
      arraySpace = H5Screate_simple(1, dim, NULL);

      for( k = 0 ; k < size ; k++ ) {
        snprintf(name, BUFFER_SIZE, "indirectStruct-t%d-%d", j, k);
     
        dataset = H5Dcreate(tempFile, name, convDouble_t, arraySpace, H5P_DEFAULT);
        status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, 
                               H5S_ALL, H5S_ALL, H5P_DEFAULT, &(indirectStructWrite[k].t) );
    
        if( status < 0 ) {
          H5Eprint(stderr);
        }
    
        H5Dclose(dataset);
     
        snprintf(name, BUFFER_SIZE, "indirectStruct-pt%d-%d", j, k);
  
        dataset = H5Dcreate(tempFile, name, pointCompoundConv_t, arraySpace, H5P_DEFAULT);
        status = H5Dwrite(dataset, pointCompound_t,
                               H5S_ALL, H5S_ALL, H5P_DEFAULT, indirectStructWrite[k].pt );
  
        if( status < 0 ) {
          H5Eprint(stderr);
        }
  
        H5Dclose(dataset); 
   
        snprintf(name, BUFFER_SIZE, "indirectStruct-val%d-%d", j, k);
  
        dataset = H5Dcreate(tempFile, name, valCompoundConv_t, arraySpace, H5P_DEFAULT);
        status = H5Dwrite(dataset, valCompound_t,
                               H5S_ALL, H5S_ALL, H5P_DEFAULT, indirectStructWrite[k].val );
  
        if( status < 0 ) {
          H5Eprint(stderr);
        }
  
        H5Dclose(dataset);

      }

      H5Sclose(arraySpace);
    }
  }
 
  /* Close the array and derived types */ 
  H5Tclose(array_t);
  H5Tclose(arrayConv_t);
  H5Tclose(compound_t);
  H5Tclose(compoundConv_t);
  H5Tclose(nestedCompound_t);
  H5Tclose(nestedCompoundConv_t);

  if( includeIndirections ) {
    H5Tclose(pointCompound_t);
    H5Tclose(valCompound_t);
    H5Tclose(pointCompoundConv_t);
    H5Tclose(valCompoundConv_t);
  }

  /* Close the HDF5 file and conversion types */
  H5Tclose( convInt_t );
  H5Tclose( convLong_t );
  H5Tclose( convFloat_t );
  H5Tclose( convDouble_t );
  H5Fclose( tempFile );
}


/* readHDF5RestartDump
 *
 * Read back some primitives, arrays, and structs from an HDF5 file 
 *
 * size                -- size of the arrays
 * numSets             -- number of sets of NUM_VARS to read from the file 
 * includeIndirections -- read indirection vars out iff TRUE
 * 
 * We may need to add more error check to properly simulate 
 * reading back a restart dump in real application code 
 */
void readHDF5RestartDump( int machine, int size, int numSets, int includeIndirections, int prevSize ) {
  int j, k;
  herr_t status;
  hid_t rootGroup;
  hid_t tempFile;
  hid_t array_t; 
  hid_t compound_t, nestedCompound_t;
  hid_t pointCompound_t, valCompound_t;
  hid_t dataset;
  hsize_t small_dim[1];
  char name[BUFFER_SIZE];
  char filename[BUFFER_SIZE];


  /* Name the file after the arguments */
  snprintf( filename, BUFFER_SIZE, "temp-file-%s-%d-%d-%d-%d.h5", 
             sysname, machine, size, numSets, includeIndirections );

  /* Open an existing HDF5 file read-only */
  tempFile  = H5Fopen( filename, H5F_ACC_RDONLY, H5P_DEFAULT );
  rootGroup = H5Gopen( tempFile, "/" );

  if( tempFile < 0 ) {
     fprintf(stderr, "\n\nreadRestartDump: couldn't open hdf5 input file %s\n", filename);
     exit(-1);
  }

  /* Define our NATIVE struct types */
  compound_t   = H5Tcreate(H5T_COMPOUND, sizeof(testStruct));
  small_dim[0] = SMALL_ARRAY_SIZE;
  array_t      = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, small_dim, NULL);
  H5Tinsert(compound_t, "xCoord", HOFFSET(testStruct, xCoord), H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "yCoord", HOFFSET(testStruct, yCoord), H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "zCoord", HOFFSET(testStruct, zCoord), H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "t",      HOFFSET(testStruct, t),      H5T_NATIVE_FLOAT);
  H5Tinsert(compound_t, "alpha",  HOFFSET(testStruct, alpha),  array_t);
  H5Tinsert(compound_t, "beta",   HOFFSET(testStruct, beta),   array_t);
  H5Tinsert(compound_t, "gamma",  HOFFSET(testStruct, gamma),  array_t);

  nestedCompound_t = H5Tcreate(H5T_COMPOUND, sizeof(nestedStruct));
  H5Tinsert(nestedCompound_t, "internalStruct",
                      HOFFSET(nestedStruct, internalStruct), compound_t);
  H5Tinsert(nestedCompound_t, "intMember",
                      HOFFSET(nestedStruct, intMember), H5T_NATIVE_INT);
  H5Tinsert(nestedCompound_t, "doubleMember",
                      HOFFSET(nestedStruct, doubleMember), H5T_NATIVE_DOUBLE);

  if( includeIndirections ) {
    pointCompound_t    = H5Tcreate(H5T_COMPOUND, 3*H5Tget_size(H5T_NATIVE_DOUBLE));
    valCompound_t      = H5Tcreate(H5T_COMPOUND, 2*H5Tget_size(H5T_NATIVE_DOUBLE));
  
    H5Tinsert(pointCompound_t, "x", HOFFSET(pointStruct, x), H5T_NATIVE_DOUBLE);
    H5Tinsert(pointCompound_t, "y", HOFFSET(pointStruct, y), H5T_NATIVE_DOUBLE);
    H5Tinsert(pointCompound_t, "z", HOFFSET(pointStruct, z), H5T_NATIVE_DOUBLE);
  
    H5Tinsert(valCompound_t, "temp", HOFFSET(valStruct, temp), H5T_NATIVE_DOUBLE);
    H5Tinsert(valCompound_t, "pres", HOFFSET(valStruct, pres), H5T_NATIVE_DOUBLE);
  }

  /* Read in each of the variations of the variables */
  for( j = 0 ; j < numSets ; j++ ) {
    prepDataToRead(size, prevSize, includeIndirections);

    /* int */
    snprintf(name, BUFFER_SIZE, "i32Primitive%d", j);

    dataset = H5Dopen( rootGroup, name );
    status = H5Dread(dataset, H5T_NATIVE_INT, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, i32PrimitiveRead);
 
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Dclose(dataset);
 
 
    /* long */
    snprintf(name, BUFFER_SIZE, "i64Primitive%d", j);

    dataset = H5Dopen( rootGroup, name );
    status = H5Dread(dataset, H5T_NATIVE_LONG, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, i64PrimitiveRead);
 
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Dclose(dataset);
  
  
    /* float */
    snprintf(name, BUFFER_SIZE, "f32Primitive%d", j);

    dataset = H5Dopen( rootGroup, name );
    status = H5Dread(dataset, H5T_NATIVE_FLOAT, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, f32PrimitiveRead );
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Dclose(dataset);
  
  
    /* double */
    snprintf(name, BUFFER_SIZE, "f64Primitive%d", j);

    dataset = H5Dopen( rootGroup, name );
    status = H5Dread(dataset, H5T_NATIVE_DOUBLE, 
                          H5S_ALL, H5S_ALL, H5P_DEFAULT, f64PrimitiveRead );
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Dclose(dataset);
    
  
    /* struct */
    snprintf(name, BUFFER_SIZE, "testStruct%d", j);

    dataset = H5Dopen( rootGroup, name );
    status = H5Dread(dataset, compound_t, H5S_ALL, H5S_ALL, H5P_DEFAULT, testStructRead);
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Dclose(dataset);
 
 
    /* nested struct */
    snprintf(name, BUFFER_SIZE, "nestedStruct%d", j);

    dataset = H5Dopen( rootGroup, name );
    status = H5Dread(dataset, nestedCompound_t, 
                        H5S_ALL, H5S_ALL, H5P_DEFAULT, nestedStructRead);
  
    if( status < 0 ) {
      H5Eprint(stderr);
    }
  
    H5Dclose(dataset);


    /* indirect struct */
    if( includeIndirections ) {
      /* Free any previous indirection related memory */
      if( indirectStructRead != NULL ) {
        /* Free indirections first */
        for( k = 0 ; k < prevSize ; k++ ) {
          SFREE( indirectStructRead[k].pt );
          SFREE( indirectStructRead[k].val );
        }

        SFREE_N(indirectStructRead, prevSize);
      }

      indirectStructRead = MAKE_N(indirectStruct, size);
      prevSize = size;

      for( k = 0 ; k < size ; k++ ) {
        /* Setup the indirect structs */
        indirectStructRead[k].pt  = MAKE(pointStruct);
        indirectStructRead[k].val = MAKE(valStruct);
   
 
        snprintf(name, BUFFER_SIZE, "indirectStruct-t%d-%d", j, k);
  
        dataset = H5Dopen( rootGroup, name );
        status = H5Dread(dataset, H5T_NATIVE_DOUBLE,
                               H5S_ALL, H5S_ALL, H5P_DEFAULT, &(indirectStructRead[k].t) );
  
        if( status < 0 ) {
          H5Eprint(stderr);
        }
  
        H5Dclose(dataset);
  
        snprintf(name, BUFFER_SIZE, "indirectStruct-pt%d-%d", j, k);
  
        dataset = H5Dopen( rootGroup, name );
        status = H5Dread(dataset, pointCompound_t,
                               H5S_ALL, H5S_ALL, H5P_DEFAULT, indirectStructRead[k].pt );
  
        if( status < 0 ) {
          H5Eprint(stderr);
        }
  
        H5Dclose(dataset);
  
        snprintf(name, BUFFER_SIZE, "indirectStruct-val%d-%d", j, k);
  
        dataset = H5Dopen( rootGroup, name );
        status = H5Dread(dataset, valCompound_t,
                               H5S_ALL, H5S_ALL, H5P_DEFAULT, indirectStructRead[k].val );
  
        if( status < 0 ) {
          H5Eprint(stderr);
        }
  
        H5Dclose(dataset);
      }
    }
  }   
 
  /* Close the array and derived types */ 
  H5Tclose(array_t);
  H5Tclose(compound_t);
  H5Tclose(nestedCompound_t);

  if( includeIndirections ) {
    H5Tclose(pointCompound_t);
    H5Tclose(valCompound_t);
  }

  /* Close the HDF5 file */
  H5Gclose( rootGroup );
  H5Fclose( tempFile );
}


/* writePDBRestartDump
 *
 * Write some primitives, arrays, and structs to an HDF5 file
 *
 * machine             -- bit-field flags of machines to convert to
 * size                -- size of the arrays
 * numSets             -- number of sets of NUM_VARS to write to the file 
 * includeIndirections -- write indirection var out iff TRUE
 *
 * We may need to add more error check to properly simulate
 * reading back a restart dump in real application code
 */
void writePDBRestartDump( int machine, int size, int numSets, int includeIndirections ) {
  int j;
  int status;
  PDBfile* tempFile;
  char name[BUFFER_SIZE];
  char filename[BUFFER_SIZE];
  char alphaType[BUFFER_SIZE], betaType[BUFFER_SIZE], gammaType[BUFFER_SIZE];
  int int_ord_f[] = {4, 3, 2, 1};
  int int_ord_d[] = {8, 7, 6, 5, 4, 3, 2, 1};
  long int_frm_f[] = {32L,  8L, 23L,  0L,  1L,  9L,  0L, 0x7FL};
  long int_frm_d[] = {64L, 11L, 52L,  0L,  1L, 12L,  0L, 0x3FFL};
  data_standard ALPHA_STD = 
           {8,                           /* bits per byte */
            8,                           /* pointer size */
            2, REVERSE_ORDER,            /* short */
            4, REVERSE_ORDER,            /* int */
            8, REVERSE_ORDER,            /* long */
            8, REVERSE_ORDER,            /* long long */
            4, int_frm_f, int_ord_f,     /* float */
            8, int_frm_d, int_ord_d};    /* double */


  /* Name the file after the arguments */
  snprintf( filename, BUFFER_SIZE, "temp-file-%s-%d-%d-%d-%d.pdb", 
            sysname, machine, size, numSets, includeIndirections );

  /* Create a new PDB file read-write */
  tempFile = PD_open( filename, "w" );

  if( tempFile == NULL ) {
     fprintf(stderr, "\n\nwriteRestartDump: couldn't create pdb output file %s\n", filename);
     exit(-1);
  }

  /* Write our data to the file in a converted format if requested */
  if( MIPS & machine ) {
    PD_target(&IEEEA_STD, &MIPS_ALIGNMENT);
  }
  else if( POWERPC & machine ) {
    PD_target(&IEEEA_STD, &RS6000_ALIGNMENT);
  }
  else if( INTEL & machine ) {
    PD_target(&INTELB_STD, &INTELC_ALIGNMENT);
  }
  else if( SPARC & machine ) {
    PD_target(&IEEEA_STD, &SPARC_ALIGNMENT);
  }
  else if( ALPHA & machine ) {
    PD_target(&ALPHA_STD, &ALPHA64_ALIGNMENT);
  }
  else {
    PD_target(INT_STANDARD, INT_ALIGNMENT);
  }

  snprintf( alphaType, BUFFER_SIZE, "double alpha(%d)", SMALL_ARRAY_SIZE );
  snprintf( betaType,  BUFFER_SIZE, "double beta(%d)",  SMALL_ARRAY_SIZE );
  snprintf( gammaType, BUFFER_SIZE, "double gamma(%d)", SMALL_ARRAY_SIZE );

  /* Define the testStruct structure to the PDB file */
  PD_defstr( tempFile, "testStruct",
                 "float xCoord",
                 "float yCoord",
                 "float zCoord",
                 "float t",
                 alphaType,
                 betaType,
                 gammaType,
                 LAST );

  PD_defstr( tempFile, "nestedStruct",
                 "testStruct internalStruct",
                 "int intMember",
                 "double doubleMember",
                 LAST );

  if( includeIndirections ) {
    PD_defstr( tempFile, "pointStruct",
                   "double x",
                   "double y",
                   "double z",
                   LAST );
  
    PD_defstr( tempFile, "valStruct",
                   "double temp",
                   "double pres",
                   LAST );
  
    PD_defstr( tempFile, "indirectStruct",
                   "double t",
                   "pointStruct *pt",
                   "valStruct *val",
                   LAST );
  }


  /* Write out each of the variations of the variables */
  for( j = 0 ; j < numSets ; j++ ) {
    snprintf(name, BUFFER_SIZE, "i32Primitive%d(0:%d)", j, (size-1));

    /* int */
    status = PD_write(tempFile, name, "int", i32PrimitiveWrite);

    if( !status ) {
      fprintf(stderr, "\n\nError writing pdb file: %s\n", PD_get_error());
    }


    /* long */
    snprintf(name, BUFFER_SIZE, "i64Primitive%d(0:%d)", j, (size-1));
    status = PD_write(tempFile, name, "long", i64PrimitiveWrite);

    if( !status ) {
      fprintf(stderr, "\n\nError writing pdb file: %s\n", PD_get_error());
    }


    /* float */
    snprintf(name, BUFFER_SIZE, "f32Primitive%d(0:%d)", j, (size-1));
    status = PD_write(tempFile, name, "float", f32PrimitiveWrite);

    if( !status ) {
      fprintf(stderr, "\n\nError writing pdb file: %s\n", PD_get_error());
    }


    /* double */
    snprintf(name, BUFFER_SIZE, "f64Primitive%d(0:%d)", j, (size-1));

    status = PD_write(tempFile, name, "double", f64PrimitiveWrite );

    if( !status ) {
      fprintf(stderr, "\n\nError writing pdb file: %s\n", PD_get_error());
    }


    /* struct */
    snprintf(name, BUFFER_SIZE, "testStruct%d(0:%d)", j, (size-1));

    status = PD_write(tempFile, name, "testStruct", testStructWrite);

    if( !status ) {
      fprintf(stderr, "\n\nError writing pdb file: %s\n", PD_get_error());
    }


    /* nested struct */
    snprintf(name, BUFFER_SIZE, "nestedStruct%d(0:%d)", j, (size-1));

    status = PD_write(tempFile, name, "nestedStruct", nestedStructWrite);

    if( !status ) {
      fprintf(stderr, "\n\nError writing pdb file: %s\n", PD_get_error());
    }

    /* indirect struct */
    if( includeIndirections ) {
      snprintf(name, BUFFER_SIZE, "indirectStruct%d(0:%d)", j, (size-1));
  
      status = PD_write(tempFile, name, "indirectStruct", indirectStructWrite);
  
      if( !status ) {
        fprintf(stderr, "\n\nError writing pdb file: %s\n", PD_get_error());
      }
    }
  }


  /* Close the HDF5 file and conversion types */
  PD_close( tempFile );
}


/* readPDBRestartDump
 *
 * Read back some primitives, arrays, and structs from an HDF5 file
 *
 * size                -- size of the arrays
 * numSets             -- number of sets of NUM_VARS to read from the file 
 * includeIndirections -- read indirection vars out iff TRUE
 *
 * We may need to add more error check to properly simulate
 * reading back a restart dump in real application code
 */
void readPDBRestartDump( int machine, int size, int numSets, int includeIndirections, int prevSize ) {
  int j;
  int status;
  PDBfile* tempFile;
  char name[BUFFER_SIZE];
  char filename[BUFFER_SIZE];


  /* Name the file after the arguments */
  snprintf( filename, BUFFER_SIZE, "temp-file-%s-%d-%d-%d-%d.pdb", 
             sysname, machine, size, numSets, includeIndirections );

  for( j = 0 ; j < numSets ; j++ ) {
    /* Open an existing PDB file read-only */
    tempFile = PD_open( filename, "r" );

    if( tempFile == NULL ) {
      fprintf(stderr, "\n\nreadRestartDump: couldn't open pdb input file %s\n", filename);
      exit(-1);
    }

    /* prep data */
    prepDataToRead(size, prevSize, includeIndirections);

    if( indirectStructRead != NULL ) {
      PD_free(tempFile, "indirectStruct", indirectStructRead);
    }

    if( includeIndirections ) {
      indirectStructRead = MAKE_N(indirectStruct, size);
    }

    /* Read in each of the variations of the variables */

    /* int */
    snprintf(name, BUFFER_SIZE, "i32Primitive%d(0:%d)", j, (size-1));

    status = PD_read(tempFile, name, i32PrimitiveRead);

    if( !status ) {
      fprintf(stderr, "\n\nError reading pdb file: %s\n", PD_get_error());
    }


    /* long */
    snprintf(name, BUFFER_SIZE, "i64Primitive%d(0:%d)", j, (size-1));

    status = PD_read(tempFile, name, i64PrimitiveRead);

    if( !status ) {
      fprintf(stderr, "\n\nError reading pdb file: %s\n", PD_get_error());
    }


    /* float */
    snprintf(name, BUFFER_SIZE, "f32Primitive%d(0:%d)", j, (size-1));

    status = PD_read(tempFile, name, f32PrimitiveRead );

    if( !status ) {
      fprintf(stderr, "\n\nError reading pdb file: %s\n", PD_get_error());
    }


    /* double */
    snprintf(name, BUFFER_SIZE, "f64Primitive%d(0:%d)", j, (size-1));

    status = PD_read(tempFile, name, f64PrimitiveRead );

    if( !status ) {
      fprintf(stderr, "\n\nError reading pdb file: %s\n", PD_get_error());
    }


    /* struct */

    /* Struct defined in header file */
    snprintf(name, BUFFER_SIZE, "testStruct%d(0:%d)", j, (size-1));

    status = PD_read(tempFile, name, testStructRead);

    if( !status ) {
      fprintf(stderr, "\n\nError reading pdb file: %s\n", PD_get_error());
    }


    /* nested struct */
    snprintf(name, BUFFER_SIZE, "nestedStruct%d(0:%d)", j, (size-1));

    status = PD_read(tempFile, name, nestedStructRead);

    if( !status ) {
      fprintf(stderr, "\n\nError reading pdb file: %s\n", PD_get_error());
    }

    
    /* indirect struct */
    if( includeIndirections ) {
      snprintf(name, BUFFER_SIZE, "indirectStruct%d(0:%d)", j, (size-1));

      status = PD_read(tempFile, name, indirectStructRead);
      prevSize = size;

/* 
      int k;
      for( k = 0 ; k < prevSize ; k++ ) {
        if( (void*)(indirectStructRead[k].val) == 
            (void*)(&(*(*tempFile->host_chart->table)->next->name)) ) {
          fprintf(stderr, "WARNING: structure has same addr as defstr in host chart\n" );
        }
      } 
*/
 
      if( !status ) {
        fprintf(stderr, "\n\nError reading pdb file: %s\n", PD_get_error());
      }
    }

    /* Close the PDB file */
    PD_close( tempFile );
  }
}


/* comparisionCheck
 *
 * Compare XYZWrite to XYZRead to make sure the data written == data read 
 *
 * size                -- size of the arrays
 * includeIndirections -- check indirection vars iff TRUE
 *
 */
int comparisonCheck( int size, int includeIndirections ) {
  int j, k;
  int status = 1;

  /* Check each position of the array */
  for( k = 0 ; k < size ; k++ ) {

    /* Check the primitives */

    if( i32PrimitiveWrite[k] != i32PrimitiveRead[k] ) {
      fprintf(stderr, "\n\ni32Primitive[%d]: %d != %d\n", k, 
                   i32PrimitiveWrite[k], i32PrimitiveRead[k] );
      status = -1;
      break;
    }

    if( i64PrimitiveWrite[k] != i64PrimitiveRead[k] ) {
      fprintf(stderr, "\n\ni64Primitive[%d]: %ld != %ld\n", k, 
                   i64PrimitiveWrite[k], i64PrimitiveRead[k] );
      status = -1;
      break;
    }

    if( abs(f32PrimitiveWrite[k] -  
            f32PrimitiveRead[k]) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nf32Primitive[%d]: %f != %f\n", k, 
                   f32PrimitiveWrite[k], f32PrimitiveRead[k] );
      status = -1;
      break;
    }

    if( abs(f64PrimitiveWrite[k] - 
            f64PrimitiveRead[k]) > DOUBLE_TOLERANCE ) {
      fprintf(stderr, "\n\nf64Primitive[%d]: %f != %f\n", k, 
                   f64PrimitiveWrite[k], f64PrimitiveRead[k] );
      status = -1;
      break;
    }

    /* Check the struct */

    if( abs(testStructWrite[k].xCoord - 
            testStructRead[k].xCoord) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nstructWrite[%d].xCoord: %g != %g\n", k, 
                 testStructWrite[k].xCoord, testStructRead[k].xCoord ); 
      status = -1;
      break;
    }

    if( abs(testStructWrite[k].yCoord - 
            testStructRead[k].yCoord) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nstructWrite[%d].yCoord: %g != %g\n", k, 
                 testStructWrite[k].yCoord, testStructRead[k].yCoord ); 
      status = -1;
      break;
    }

    if( abs(testStructWrite[k].zCoord - 
            testStructRead[k].zCoord) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nstructWrite[%d].zCoord: %g != %g\n", k, 
                 testStructWrite[k].zCoord, testStructRead[k].zCoord ); 
      status = -1;
      break;
    }

    if( abs(testStructWrite[k].t - 
            testStructRead[k].t) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nstructWrite[%d].t: %g != %g\n", k, 
                 testStructWrite[k].t, testStructRead[k].t ); 
      status = -1;
      break;
    }

    for( j = 0 ; j < SMALL_ARRAY_SIZE ; j++ ) {
      if(abs(testStructWrite[k].alpha[j] - 
             testStructRead[k].alpha[j]) > DOUBLE_TOLERANCE) {
        fprintf(stderr, "\n\ntestStruct[%d].alpha[%d]: %g != %g\n", k, j, 
                 testStructWrite[k].alpha[j], testStructRead[k].alpha[j] );
        status = -1;
        break;
       }

      if(abs(testStructWrite[k].beta[j] - 
             testStructRead[k].beta[j]) > DOUBLE_TOLERANCE) {
        fprintf(stderr, "\n\ntestStruct[%d].beta[%d]: %g != %g\n", k, j, 
                 testStructWrite[k].beta[j], testStructRead[k].beta[j] );
        status = -1;
        break;
       }

      if(abs(testStructWrite[k].gamma[j] - 
             testStructRead[k].gamma[j]) > DOUBLE_TOLERANCE) {
        fprintf(stderr, "\n\ntestStruct[%d].gamma[%d]: %g != %g\n", k, j, 
                 testStructWrite[k].gamma[j], testStructRead[k].gamma[j] );
        status = -1;
        break;
       }
    }

    /* Check the nested struct */

    if( abs(nestedStructWrite[k].internalStruct.xCoord -
            nestedStructRead[k].internalStruct.xCoord) > FLOAT_TOLERANCE  ) {
      fprintf(stderr, "\n\nnestedStruct[%d].internalStruct.xCoord: %g != %g\n",
                k, nestedStructWrite[k].internalStruct.xCoord,
                nestedStructRead[k].internalStruct.xCoord );
      status = -1;
      break;
    }
 
    if( abs(nestedStructWrite[k].internalStruct.yCoord -
            nestedStructRead[k].internalStruct.yCoord) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nnestedStruct[%d].internalStruct.yCoord: %g != %g\n",
                k, nestedStructWrite[k].internalStruct.yCoord,
                nestedStructRead[k].internalStruct.yCoord );
      status = -1;
      break;
    }

    if( abs(nestedStructWrite[k].internalStruct.zCoord !=
            nestedStructRead[k].internalStruct.zCoord) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nnestedStruct[%d].internalStruct.zCoord: %g != %g\n",
                k, nestedStructWrite[k].internalStruct.zCoord,
                nestedStructRead[k].internalStruct.zCoord );
      status = -1;
      break;
    }

    if( abs(nestedStructWrite[k].internalStruct.t !=
            nestedStructRead[k].internalStruct.t) > FLOAT_TOLERANCE ) {
      fprintf(stderr, "\n\nnestedStruct[%d].internalStruct.t: %g != %g\n",
                k, nestedStructWrite[k].internalStruct.t,
                nestedStructRead[k].internalStruct.t );
      status = -1;
      break;
    }

    for( j = 0 ; j < SMALL_ARRAY_SIZE ; j++ ) {
      if( abs(nestedStructWrite[k].internalStruct.alpha[j] -
              nestedStructRead[k].internalStruct.alpha[j]) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nnestedStruct[%d].internalStruct.alpha[%d]: %g != %g\n",
                 k, j, nestedStructWrite[k].internalStruct.alpha[j],
                 nestedStructRead[k].internalStruct.alpha[j]  );
        status = -1;
        break;
      }

      if( abs(nestedStructWrite[k].internalStruct.beta[j] -
              nestedStructRead[k].internalStruct.beta[j]) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nnestedStruct[%d].internalStruct.beta[%d]: %g != %g\n",
                 k, j, nestedStructWrite[k].internalStruct.beta[j],
                 nestedStructRead[k].internalStruct.beta[j]  );
        status = -1;
        break;
      }

      if( abs(nestedStructWrite[k].internalStruct.gamma[j] -
              nestedStructRead[k].internalStruct.gamma[j]) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nnestedStruct[%d].internalStruct.gamma[%d]: %g != %g\n",
                 k, j, nestedStructWrite[k].internalStruct.gamma[j],
                 nestedStructRead[k].internalStruct.gamma[j]  );
        status = -1;
        break;
      }
    }

    if( nestedStructWrite[k].intMember != 
        nestedStructRead[k].intMember ) {
      fprintf(stderr, "\n\nnestedStruct[%d].intMember: %d != %d\n", k,
               nestedStructWrite[k].intMember, 
               nestedStructRead[k].intMember );
      status = -1;
      break;
    }

    if( abs(nestedStructWrite[k].doubleMember - 
            nestedStructRead[k].doubleMember) > DOUBLE_TOLERANCE ) {
      fprintf(stderr, "\n\nnestedStruct[%d].doubleMember: %g != %g\n", k,
               nestedStructWrite[k].doubleMember, 
               nestedStructRead[k].doubleMember );
      status = -1;
      break;
    }

    
    /* Check the indirect struct */
    if( includeIndirections ) {
      if( abs(indirectStructWrite[k].t - 
              indirectStructRead[k].t) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nindirectStruct[%d].t: %g != %g\n", k,
                 indirectStructWrite[k].t, indirectStructRead[k].t );
        status = -1;
        break;
      }
      
      if( abs(indirectStructWrite[k].pt->x - 
              indirectStructRead[k].pt->x) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nindirectStruct[%d].pt->x: %g != %g\n", k,
                 indirectStructWrite[k].pt->x, indirectStructRead[k].pt->x );
        status = -1;
        break;
      } 
  
      if( abs(indirectStructWrite[k].pt->y -
              indirectStructRead[k].pt->y) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nindirectStruct[%d].pt->y: %g != %g\n", k,
                 indirectStructWrite[k].pt->y, indirectStructRead[k].pt->y );
        status = -1;
        break;
      }
  
      if( abs(indirectStructWrite[k].pt->z -
              indirectStructRead[k].pt->z) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nindirectStruct[%d].pt->z: %g != %g\n", k,
                 indirectStructWrite[k].pt->z, indirectStructRead[k].pt->z );
        status = -1;
        break;
      }
  
      if( abs(indirectStructWrite[k].val->temp -
              indirectStructRead[k].val->temp) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nindirectStruct[%d].val->temp: %g != %g\n", k,
                 indirectStructWrite[k].val->temp, indirectStructRead[k].val->temp );
        status = -1;
        break;
      }
  
      if( abs(indirectStructWrite[k].val->pres -
              indirectStructRead[k].val->pres) > DOUBLE_TOLERANCE ) {
        fprintf(stderr, "\n\nindirectStruct[%d].val->pres: %g != %g\n", k,
                 indirectStructWrite[k].val->pres, indirectStructRead[k].val->pres );
        status = -1;
        break;
      }
    }
  }

  return( status );
}


/* getDatasize
 *
 * Determine the size of the data that is being written or read from a file
 *
 * sets                -- number of sets of NUM_VARS to write to the file 
 * size                -- size of the arrays
 * includeIndirections -- include indirection var iff TRUE
 *
 */
double getDatasize( int sets, int size, int includeIndirections ) {
  double result, datasize; 

  datasize = (double)(size);
/*  fprintf(stderr, "\nData size = %f\n", datasize); */

  result  = ((double)sizeof(int))          * datasize;
  result += ((double)sizeof(long))         * datasize;
  result += ((double)sizeof(float))        * datasize;
  result += ((double)sizeof(double))       * datasize;
  result += ((double)sizeof(testStruct))   * datasize;
  result += ((double)sizeof(nestedStruct)) * datasize;

  if( includeIndirections ) {
    result += ((double)(6.0 * sizeof(double))) * datasize;
  }

/*  fprintf(stderr, "Set size = %f bytes\n", result); */

  result *= ((double)sets);
/*  fprintf(stderr, "Total size = %f bytes\n", result); */

  /* kB */
  result /= (double)1024.0; 

  /* MB */
  result /= (double)1024.0;

  return( result );
}


/* getThruput
 *
 * Determine the throughput: the amount of data written in a given time period
 *
 * sets                -- number of sets of NUM_VARS to write to the file 
 * size                -- size of the arrays
 * duration            -- time elapsed during read/write of data
 * includeIndirections -- include indirection var iff TRUE
 *
 */
double getThruput( int sets, int size, struct timeval duration, int includeIndirections ) {
  double result;
 
  /* MB */ 
  result  = getDatasize( sets, size, includeIndirections ); 

  /* MB/s */
  result /= (((double)duration.tv_sec) + ((double)duration.tv_usec) / 1000000.0); 

  return( result );
}


/* runTests
 *
 * Run restart dump and restart read tests over a range of I/O sizes 
 *
 * pdbOnly             -- only run benchs for PDB iff TRUE
 * hdf5Only            -- only run benchs for HDF5 iff TRUE
 * includeIndirections -- include indirection var iff TRUE
 * machineTypes        -- bit-field flags of machines to convert to
 * minDataSize         -- minimum data size to cover during benchs 
 * maxDataSize         -- maximum data size to cover during benchs 
 * iterations          -- number of iterations to use to determine mean
 * minVars             -- minimum number of vars to include in the file (approx)
 * maxVars             -- maximum number of vars to include in the file (approx)
 *
 */
void runTests(int flags, int machineTypes, int minDataSize, int maxDataSize,  
              int iterations, int minVars, int maxVars ) {
  struct timeval startTime, endTime, duration;
  char filename[BUFFER_SIZE];
  int  pdbOnly, hdf5Only, includeIndirections, doFileRead, doFileWrite;
  int  i, j, k, status, numberOfVars;
  int  size, prevSize, minSets, maxSets;
  int  machine;
  long sleepAmt = 1000;    /* Sleep time in microseconds */

  /* === Strip out the flags === */
  pdbOnly             = flags & PDB_ONLY;
  hdf5Only            = flags & HDF5_ONLY;
  includeIndirections = flags & INCLUDE_INDIR;
  doFileRead          = flags & DO_FILE_READ;
  doFileWrite         = flags & DO_FILE_WRITE;

  /* === Setup the number of variables to use === */
  numberOfVars = NUM_VARS;

  if( !includeIndirections ) {
    numberOfVars--;
  }

  minSets = minVars / numberOfVars;
  maxSets = maxVars / numberOfVars;
  prevSize = 0;

  printf("\n Using %d iterations to calculate mean duration\n", iterations );
  printf(" Handling files with sys name: %s\n\n", sysname );

  printf("    Benchmark    Vars    Size      ");
  printf("  Mean Duration   Machine  Check   Thruput\n");
  printf("    ---------   ------   ----      ");
  printf("  -------------   -------  -----   -------\n");

  /* === Loop over the number of sets of vars to include in the file === */
  for( j = minSets ; j <= maxSets ; j = j * 2 ) {
    for( k = 0 ; k <= NUM_TYPES ; k++ ) {
      machine = machineTypes & (int)(pow(2, k));

      /* Do not do "no targeting" case when targeting */
      if(!machine && machineTypes) {
        continue;
      }

      /* Do "no targeting" case last when not targeting */
      if(!machine && k < NUM_TYPES) {
        continue;
      }

      /* === Loop over the range of I/O sizes === */
      for( size = minDataSize ; size <= maxDataSize ; size = size * 2 ) {
        /* Give the data some arbitrary default values */
        prepDataToWrite( size, prevSize );

        /* HDF5 Write Restart Dump Benchmark */ 
        if( !pdbOnly && !doFileRead ) { 
          printf( "%-13s", " HDF5 dump");
          printf( "%4d%-5s  ", (numberOfVars*j),  " vars");
          printf("%3.2f%-3s  ", getDatasize(j, size, includeIndirections ), " MB");
          fflush(stdout);
    
          /* Benchmark writing a restart dump */
          gettimeofday( &startTime, NULL );
    
          for( i = 0 ; i < iterations ; i++ ) {
            writeHDF5RestartDump( machine, size, j, includeIndirections );
          }
    
          gettimeofday( &endTime, NULL );
          duration = timer_sub( endTime, startTime );
          duration = timer_div( duration, iterations );
    
          printf("%2ld%-s%6ld%-s", duration.tv_sec, " sec ", duration.tv_usec, " usec" );

          if(machine) {
            switch(machine) {
              case MIPS:
                printf("    MIPS ");
                break;
              case POWERPC:  
                printf("  POWERPC");
                break;
              case INTEL:
                printf("   INTEL ");
                break;
              case SPARC:
                printf("   SPARC ");
                break;
              case ALPHA:
                printf("   ALPHA ");
                break;
              default:
                printf("   ERROR ");
            }
          }
          else {
            printf("   Native");
          }   

          printf("          ");
          printf("%-.2f MB/s\n", getThruput(j, size, duration, includeIndirections));

          usleep( sleepAmt );
        }
    
        /* HDF5 Read Restart Dump Benchmark */ 
        if( !pdbOnly && !doFileWrite ) { 
          /* Give the data null (zero) values */
          /*prepDataToRead( size, prevSize, includeIndirections );*/

          printf( "%-13s", " HDF5 restart");
          printf( "%4d%-5s  ", numberOfVars*j,  " vars");
          printf("%3.2f%-3s  ", getDatasize(j, size, includeIndirections ), " MB");
          fflush(stdout);
    
          /* Benchmark reading a restart dump */
          gettimeofday( &startTime, NULL );
    
          for( i = 0 ; i < iterations ; i++ ) {
            readHDF5RestartDump( machine, size, j, includeIndirections, prevSize );
          }
  
          gettimeofday( &endTime, NULL );
          duration = timer_sub( endTime, startTime );
          duration = timer_div( duration, iterations );
  
          printf("%2ld%-s%6ld%-s", duration.tv_sec, " sec ", duration.tv_usec, " usec" );

          if(machine) {
            switch(machine) {
              case MIPS:
                printf("    MIPS ");
                break;
              case POWERPC:  
                printf("  POWERPC");
                break;
              case INTEL:
                printf("   INTEL ");
                break;
              case SPARC:
                printf("   SPARC ");
                break;
              case ALPHA:
                printf("   ALPHA ");
                break;
              default:
                printf("   ERROR ");
            }
          }
          else {
            printf("   Native");
          }
  
          usleep( sleepAmt );
  
          /* Compare the data read to the data written as a correctness check */
          status = comparisonCheck( size, includeIndirections );

          if( status < 0 ) {
            printf("    Error\n");
            exit( -1 );
          }
          else {
            printf("    OK");
          }

          printf("    ");
          printf("%-.2f MB/s\n", getThruput(j, size, duration, includeIndirections));

          snprintf( filename, BUFFER_SIZE, "temp-file-%s-%d-%d-%d-%d.h5",
                    sysname, machine, size, j, includeIndirections );

          if( !doFileRead && !doFileWrite ) {
            unlink( filename );
          }
        }

        /* PDB Write Restart Dump Benchmark */
        if( !hdf5Only && !doFileRead ) {
          printf( "%-13s", "  PDB dump");
          printf( "%4d%-5s  ", numberOfVars*j,  " vars");
          printf("%3.2f%-3s  ", getDatasize(j, size, includeIndirections ), " MB");
          fflush(stdout);
  
          /* Benchmark writing a restart dump */
          gettimeofday( &startTime, NULL );
  
          for( i = 0 ; i < iterations ; i++ ) {
            writePDBRestartDump( machine, size, j, includeIndirections );
          }
  
          gettimeofday( &endTime, NULL );
          duration = timer_sub( endTime, startTime );
          duration = timer_div( duration, iterations );
  
          printf("%2ld%-s%6ld%-s", duration.tv_sec, " sec ", duration.tv_usec, " usec" );
    
          if(machine) {
            switch(machine) {
              case MIPS:
                printf("    MIPS ");
                break;
              case POWERPC:  
                printf("  POWERPC");
                break;
              case INTEL:
                printf("   INTEL ");
                break;
              case SPARC:
                printf("   SPARC ");
                break;
              case ALPHA:
                printf("   ALPHA ");
                break;
              default:
                printf("   ERROR ");
            }
          }
          else {
            printf("   Native");
          }

          printf("          ");
          printf("%-.2f MB/s\n", getThruput(j, size, duration, includeIndirections));

          usleep( sleepAmt );
        }
 
        /* PDB Read Restart Dump Benchmark */ 
        if( !hdf5Only && !doFileWrite ) { 
          /* Give the data null (zero) values */
          /*prepDataToRead( size, prevSize, includeIndirections );*/
  
          printf( "%-13s", "  PDB restart");
          printf( "%4d%-5s  ", numberOfVars*j,  " vars");
          printf("%3.2f%-3s  ", getDatasize(j, size, includeIndirections ), " MB");
          fflush(stdout);
  
          /* Benchmark reading a restart dump */
          gettimeofday( &startTime, NULL );
  
          for( i = 0 ; i < iterations ; i++ ) {
            readPDBRestartDump( machine, size, j, includeIndirections, prevSize );
          }
  
          gettimeofday( &endTime, NULL );
          duration = timer_sub( endTime, startTime );
          duration = timer_div( duration, iterations );
  
          printf("%2ld%-s%6ld%-s", duration.tv_sec, " sec ", duration.tv_usec, " usec" );

          if(machine) {
            switch(machine) {
              case MIPS:
                printf("    MIPS ");
                break;
              case POWERPC:  
                printf("  POWERPC");
                break;
              case INTEL:
                printf("   INTEL ");
                break;
              case SPARC:
                printf("   SPARC ");
                break;
              case ALPHA:
                printf("   ALPHA ");
                break;
              default:
                printf("   ERROR ");
            }
          }
          else {
            printf("   Native");
          }

          usleep( sleepAmt );
  
          /* Compare the data read to the data written as a correctness check */
          status = comparisonCheck( size, includeIndirections );
  
          if( status < 0 ) {
            printf("    Error\n");
            exit( -1 );
          }
          else {
            printf("    OK");
          }

          printf("    ");
          printf("%-.2f MB/s\n", getThruput(j, size, duration, includeIndirections));

          snprintf( filename, BUFFER_SIZE, "temp-file-%s-%d-%d-%d-%d.pdb",
                    sysname, machine, size, j, includeIndirections );

          if( !doFileRead && !doFileWrite ) {
            unlink( filename );
          }
        }

        prevSize = size;
        printf("\n");
      }

      prevSize = 0;
      printf("\n");
    } 
  }
}


/* printHelp
 * 
 * Print a help screen to the user 
 *
 * argv -- original argv to main()
 */
void printHelp( char* argv[] ) {
  fprintf(stderr, 
"\n%s [option(s)]\n\n\
	-a              only run PDB benchmarks\n\
	-b              only run HDF5 benchmarks\n\
        -r              read in benchmark files (for types TYPES plus NATIVE)\n\
        -w              write out benchmark files (for types TYPES plus NATIVE)\n\
	-d              include indirections in benchmark\n\
	-h              print this help screen\n\
	-i ITERATIONS   number of iterations used to average\n\
        -m MIN_PAGES    min number of 1/8th pages to read/write per 4 byte var\n\
	-p MAX_PAGES    max number of 1/8th pages to read/write per 4 byte var\n\
        -n MIN_VARS     min number of variables to read/write\n\
        -v MAX_VARS     max number of variables to read/write\n\
	-t TYPES        summation of machine types to do conversions for\n\
	                  0x%.2x -- MIPS    (IEEE, BIG ENDIAN,     MIPS ALIGNMENT)\n\
	                  0x%.2x -- POWERPC (IEEE, BIG ENDIAN,   RS6000 ALIGNMENT)\n\
	                  0x%.2x -- INTEL   (IEEE, LITTLE ENDIAN, INTEL ALIGNMENT)\n\
	                  0x%.2x -- SPARC   (IEEE, BIG ENDIAN,    SPARC ALIGNMENT)\n\
	                  0x%.2x -- ALPHA   (IEEE, LITTLE ENDIAN, ALPHA ALIGNMENT)\n\
                        hence MIPS, INTEL, ALPHA would be \"-t %d\"\n\n",
  argv[0], MIPS, POWERPC, INTEL, SPARC, ALPHA, (MIPS+INTEL+ALPHA));
}


/* main
 *
 * Benchmark PDB and HDF5 
 *
 */
int main( int argc, char* argv[] ) {
  signed char option;
  int  minVars, maxVars;
  int  minDataSize, maxDataSize;
  int  machineTypes;
  int  pagesize;
  int  iterations;
  int  flags;


  /* === Setup global variables === */

  sysname = NULL;

  /* Explicitly null out the global data ptrs */
  i32PrimitiveWrite   = NULL;
  i64PrimitiveWrite   = NULL;
  f32PrimitiveWrite   = NULL;
  f64PrimitiveWrite   = NULL;
  testStructWrite     = NULL;
  nestedStructWrite   = NULL;
  indirectStructWrite = NULL;
  
  i32PrimitiveRead   = NULL;
  i64PrimitiveRead   = NULL;
  f32PrimitiveRead   = NULL;
  f64PrimitiveRead   = NULL;
  testStructRead     = NULL;
  nestedStructRead   = NULL;
  indirectStructRead = NULL;

  /* === Setup local variables === */

  /* Get the system page size */
  pagesize = getpagesize();

  /* Go from about 1/8 page of data to about 2 pages of data per 4 byte var */
  minDataSize = 1 * (pagesize / 4) / 8;  
  maxDataSize = 2 * (pagesize / 4);   

  flags         = 0x00;
  iterations    = ITERATIONS_DEFAULT;
  minVars       = 6;
  maxVars       = MAX_VARS;
  machineTypes  = 0;

  /* === Check for command-line args === */

  /* OPTIONS is #define'd at the top of this file */
  option = getopt( argc, argv, OPTIONS );

  while( option != -1 ) {
    switch( option ) {
      case 'a':
        flags = flags | PDB_ONLY;
        break;

      case 'b':
        flags = flags | HDF5_ONLY;
        break;

      case 'd':
        flags = flags | INCLUDE_INDIR;
        break;
 
      case 'h':
        printHelp( argv );
        exit( 0 );
        break;

      case 'i':
        iterations = atoi(optarg);
        break;

      case 'm':
        minDataSize = atoi(optarg);
        minDataSize = minDataSize * (pagesize / 4) / 8;
        break;

      case 'n':
        minVars = atoi(optarg);
        break;

      case 'p':
        maxDataSize = atoi(optarg);
        maxDataSize = maxDataSize * (pagesize / 4) / 8;
        break;

      case 'r':
        flags = flags | DO_FILE_READ;
        sysname = (char*)malloc(strlen(optarg) + 1);
        strcpy(sysname, optarg);
        break;

      case 't':
        machineTypes  = atoi(optarg); 
        break;
 
      case 'v':
        maxVars = atoi(optarg);
        break;

      case 'w':
        flags = flags | DO_FILE_WRITE;
        sysname = (char*)malloc(strlen(optarg) + 1);
        strcpy(sysname, optarg);
        break;

      default:
        fprintf(stderr, "\n\nUnknown option: %c\n", option);
    }

    option = getopt( argc, argv, OPTIONS );
  }

  /* Validate variables before running tests */
  if( minDataSize < 0 ) {
    minDataSize = 1;
  }

  if( maxDataSize < minDataSize ) {
    maxDataSize = minDataSize;
  }

  if( minVars < NUM_VARS ) {
    minVars = NUM_VARS;
  }

  if( maxVars < minVars ) {
    maxVars = minVars;
  }

  if( machineTypes < 0 || machineTypes >= pow(2, NUM_TYPES) ) {
    fprintf(stderr, "\nUnknown set of architectures: %d\n", machineTypes);
    machineTypes = 0;
  }

  if( sysname == NULL ) {
    sysname = (char*)malloc(strlen("unknown") + 1);
    strcpy( sysname, "unknown" );
  }

  /* Start running the tests */
  runTests( flags, machineTypes, minDataSize, maxDataSize, iterations, minVars, maxVars );

  return( 0 );
}

