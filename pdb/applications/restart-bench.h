
/* 
 *  HDF5-RESTART-BENCH.H
 * 
 *  Author: Greg Smethells
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "hdf5.h"
#include "pdb.h"

/* DEFINITIONS */

#define FALSE 0
#define TRUE  1

#define SMALL_ARRAY_SIZE   10
#define BUFFER_SIZE        256
#define ITERATIONS_DEFAULT 5
#define MAX_VARS           50

#define MIPS      0x01
#define POWERPC   0x02
#define INTEL     0x04
#define SPARC     0x08
#define ALPHA     0x10
#define NUM_TYPES 5

#define FLOAT_TOLERANCE  1e-10
#define DOUBLE_TOLERANCE 1e-10

#define PDB_ONLY      0x01
#define HDF5_ONLY     0x02
#define DO_FILE_READ  0x04
#define DO_FILE_WRITE 0x08
#define INCLUDE_INDIR 0x10

/* Simple test struct */
typedef struct s_testStruct {
  float xCoord;
  float yCoord;
  float zCoord;
  float t;
  double alpha[SMALL_ARRAY_SIZE];
  double beta[SMALL_ARRAY_SIZE];
  double gamma[SMALL_ARRAY_SIZE];
} testStruct;

/* Nested struct */
typedef struct s_nestedStruct {
  testStruct internalStruct;
  int intMember;
  double doubleMember;
} nestedStruct;

/* Point struct */

typedef struct s_pointStruct {
  double x;
  double y;
  double z;
} pointStruct;

typedef struct s_valStruct {
  double temp;
  double pres;
} valStruct;

/* Indirections within struct */
typedef struct s_indirectStruct {
  double t;
  pointStruct* pt;
  valStruct* val;
} indirectStruct;

/* GLOBALS */


/* PROTOTYPES */

/* Subtract the second timeval from the first timeval returning answer */
struct timeval timer_sub( struct timeval first, struct timeval second );

/* Divide the timeval dividend by the long divisor returning answer */
struct timeval timer_div( struct timeval dividend, long divisor );

/* Allocate and fill the global write vars with some arbitrary values 
 * Free any previous mem that was allocated 
 */
void prepDataToWrite( int size, int prevSize );

/* Allocate and fill the global read vars with zero values 
 * Free any previous mem that was allocated 
 */
void prepDataToRead( int size, int prevSize, int includeIndirections );

/* Simluate a writing a restart dump with or w/o conversions
 * and of the requested I/O size
 */
void writeHDF5RestartDump( int machine, int size, int numVars, int includeIndirections );

/* Simluate a reading a restart dump with or w/o conversions
 * and of the requested I/O size
 */
void readHDF5RestartDump( int machine, int size, int numVars, int includeIndirections, int fullSize );

/* Simluate a writing a restart dump with or w/o conversions
 * and of the requested I/O size
 */
void writePDBRestartDump( int machine, int size, int numVars, int includeIndirections );

/* Simluate a reading a restart dump with or w/o conversions
 * and of the requested I/O size
 */
void readPDBRestartDump( int machine, int size, int numVars, int includeIndirections, int fullSize );

/* Compare each value in data read from the file to that 
 * which was written and error if they are different
 */ 
int comparisonCheck( int size, int includeIndirections );

/* Do the tests over the range of parameters set by the user */
void runTests(int flags, int machineTypes, int minSize, int maxSize, 
              int iterations, int minVars, int maxVars );

/* Print a help screen to the user */
void printHelp( char* argv[] );

