/*
 *      %W%
 *
 *      Time of last change:            %G%  %U%
 *
 *      Time of get for this file:      %H%  %T%
 *
 *  *************************************************************
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
#ifndef GENMACRO_H
#define GENMACRO_H

#include <stdlib.h>  /* abort() */
#include <signal.h>  /* abort() */
#include <string.h>  /* memcpy  */

/* Macro to take the absolute value */
#define ABS(A) ( ((A) < 0) ? -(A) : (A) )
 
/* Macros to find the largest and smallest of two numbers */
#ifndef MAX
#define MAX(a,b)  ( ((a) >= (b)) ? (a) : (b) )
#endif

#ifndef MIN
#define MIN(a,b)  ( ((a) <= (b)) ? (a) : (b) )
#endif

/*----------------------------------------------------------------------
** NOTE:  If size_t undefined during compile time...
**        look in /usr/include/stdio.h to determine declaration, or
**        man fread and see how the second/third arguement is declared
**----------------------------------------------------------------------*/

#define EN_FREAD(dataptr,dtype,dcount,fileptr) \
  (fread((dataptr),(size_t)(dtype),(size_t)(dcount),(fileptr)))

#define EN_FWRITE(dataptr,dtype,dcount,fileptr) \
  (fwrite((dataptr),(size_t)(dtype),(size_t)(dcount),(fileptr)))


#if defined(DEBUG)
#define FREAD_MAC(rout_name,dataptr,dtype,dcount,fileptr) \
{ \
  int ifread = dcount; \
  if((ifread)) { \
    if (fread((dataptr),sizeof(dtype),(dcount),(fileptr)) != (dcount)) \
    { \
      perror(rout_name); \
      return(A_ERR); \
    } \
  } \
  if(Rwopen && Rwinfo) { \
    fprintf(Restf,"RW: %s %s %s %d\n",rout_name, #dataptr, #dtype, dcount); \
  } \
}
#else
#define FREAD_MAC(rout_name,dataptr,dtype,dcount,fileptr) \
{ \
  int ifread = dcount; \
  if((ifread)) { \
    if (fread((dataptr),sizeof(dtype),(dcount),(fileptr)) != (dcount)) \
    { \
      perror(rout_name); \
      return(A_ERR); \
    } \
  } \
}
#endif

#if defined(DEBUG)
#define FWRITE_MAC(rout_name,dataptr,dtype,dcount,fileptr) \
{ \
  int ifwrite = dcount; \
  if((ifwrite)) { \
    if(fwrite((dataptr),sizeof(dtype),(dcount),(fileptr)) != (dcount)) \
    { \
      perror(rout_name); \
      return(A_ERR); \
    } \
  } \
  if(Rwopen && Rwinfo) { \
    fprintf(Savef,"RW: %s %s %s %d\n",rout_name, #dataptr, #dtype, dcount); \
  } \
}
#else
#define FWRITE_MAC(rout_name,dataptr,dtype,dcount,fileptr) \
{ \
  int ifwrite = dcount; \
  if((ifwrite)) { \
    if(fwrite((dataptr),sizeof(dtype),(dcount),(fileptr)) != (dcount)) \
    { \
      perror(rout_name); \
      return(A_ERR); \
    } \
  } \
}
#endif


#define IS_VERSION(sect_vers,desired_version) \
  ( sect_vers > (desired_version - 0.0025) &&\
    sect_vers < (desired_version + 0.0025) )

#define GE_VERSION(sect_vers,desired_version) \
  ( sect_vers > (desired_version - 0.0025) )

#define GT_VERSION(sect_vers,desired_version) \
  ( sect_vers > (desired_version + 0.0025) )

#define LE_VERSION(sect_vers,desired_version) \
  ( sect_vers < (desired_version + 0.0025) )

#define LT_VERSION(sect_vers,desired_version) \
  ( sect_vers < (desired_version - 0.0025) )

/*---------------------------------------------
 * Macro to swap the bytes of a 2-byte number
 *---------------------------------------------
 * (Send in the number, not the pointer)
 *---------------------------------------------*/
#define SWAP_2_BYTES(number) \
{ \
  char *ptr; \
  char newword[2]; \
  ptr = (char *) &number; \
  newword[1] = *ptr; \
  ++ptr; \
  newword[0] = *ptr; \
  memcpy(&number,newword,2); \
}

/*---------------------------------------------
 * Macro to swap the bytes of a 4-byte number
 *---------------------------------------------
 * (Send in the number, not the pointer)
 *---------------------------------------------*/
#define SWAP_4_BYTES(number) \
{ \
  char *ptr; \
  char newword[4]; \
  ptr = (char *) &number; \
  newword[3] = *ptr; \
  ++ptr; \
  newword[2] = *ptr; \
  ++ptr; \
  newword[1] = *ptr; \
  ++ptr; \
  newword[0] = *ptr; \
  memcpy(&number,newword,4); \
}

/*---------------------------------------------
 * Macro to swap the bytes of a 8-byte number
 *---------------------------------------------
 * (Send in the number, not the pointer)
 *---------------------------------------------*/
#define SWAP_8_BYTES(number) \
{ \
  char *ptr; \
  char newword[8]; \
  ptr = (char *) &number; \
  newword[7] = *ptr; \
  ++ptr; \
  newword[6] = *ptr; \
  ++ptr; \
  newword[5] = *ptr; \
  ++ptr; \
  newword[4] = *ptr; \
  ++ptr; \
  newword[3] = *ptr; \
  ++ptr; \
  newword[2] = *ptr; \
  ++ptr; \
  newword[1] = *ptr; \
  ++ptr; \
  newword[0] = *ptr; \
  memcpy(&number,newword,8); \
}

/*-----------------------------------------------
 * Macro to swap the bytes of a number
 *-----------------------------------------------
 * -> It determines number of bytes from the type
 *-----------------------------------------------*/
#define SWAP_BYTES_ONE(dtype,number) \
{ \
  if(sizeof(dtype) == 4) { \
    SWAP_4_BYTES(number); \
  } \
  else if(sizeof(dtype) == 8) { \
    SWAP_8_BYTES(number); \
  } \
  else if(sizeof(dtype) == 2) { \
    SWAP_2_BYTES(number); \
  } \
}

/*-----------------------------------------------
 * Macro to swap the bytes of a 1D array
 *-----------------------------------------------
 * -> It determines number of bytes from the type
 * -> begin and end range must be specified
 *     (Note:  loop=loop_beg; loop<=loop_end)
 *-----------------------------------------------*/
#define SWAP_BYTES_ARRAY(dtype,number,loop_beg,loop_end) \
{ \
  int  zloopn; \
  for(zloopn=loop_beg; zloopn<=loop_end; ++zloopn) { \
    if(sizeof(dtype) == 4) { \
      SWAP_4_BYTES(number[zloopn]); \
    } \
    else if(sizeof(dtype) == 8) { \
      SWAP_8_BYTES(number[zloopn]); \
    } \
    else if(sizeof(dtype) == 2) { \
      SWAP_2_BYTES(number[zloopn]); \
    } \
  } \
}

/*-----------------------------------------------
 * Macro to swap the bytes of a 2D array
 *-----------------------------------------------
 * -> It determines number of bytes from the type
 * -> begin and end range must be specified for both row and col
 *     (Note:  row_loop=row_beg; row_loop<=row_end
 *             col_llop=col_beg; col_loop<=col_end)
 *-----------------------------------------------*/
#define SWAP_BYTES_2DARRAY(dtype,number,row_beg,row_end,col_beg,col_end) \
{ \
  int  zrown; \
  int  zcoln; \
  for(zrown=row_beg; zrown<=row_end; ++zrown) { \
    for(zcoln=col_beg; zcoln<=col_end; ++zcoln) { \
      if(sizeof(dtype) == 4) { \
        SWAP_4_BYTES(number[zrown][zcoln]); \
      } \
      else if(sizeof(dtype) == 8) { \
        SWAP_8_BYTES(number[zrown][zcoln]); \
      } \
      else if(sizeof(dtype) == 2) { \
        SWAP_2_BYTES(number[zrown][zcoln]); \
      } \
    } \
  } \
}


#endif /*GENMACRO_H*/



