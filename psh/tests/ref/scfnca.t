/*
 * SCFNCA_T.H - generated type handling routines - do not edit
 *
 */

#ifndef PCK_SCFNCA_T

#define PCK_SCFNCA_T

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

static void SC_unpack_bits_int8(char *out, long i, long fld)
   {int8_t *pv;
    pv    = (int8_t *) out;
    pv[i] = (int8_t) fld;
    return;}

/*--------------------------------------------------------------------------*/

static void SC_unpack_bits_shrt(char *out, long i, long fld)
   {short *pv;
    pv    = (short *) out;
    pv[i] = (short) fld;
    return;}

/*--------------------------------------------------------------------------*/

static void SC_unpack_bits_int(char *out, long i, long fld)
   {int *pv;
    pv    = (int *) out;
    pv[i] = (int) fld;
    return;}

/*--------------------------------------------------------------------------*/

static void SC_unpack_bits_lng(char *out, long i, long fld)
   {long *pv;
    pv    = (long *) out;
    pv[i] = (long) fld;
    return;}

/*--------------------------------------------------------------------------*/

static void SC_unpack_bits_ll(char *out, long i, long fld)
   {long long *pv;
    pv    = (long long *) out;
    pv[i] = (long long) fld;
    return;}

/*--------------------------------------------------------------------------*/

typedef void (*PFSC_unpack_bits)(char *out, long i, long fld);

static PFSC_unpack_bits
 SC_unpack_bits_fnc[] = {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                SC_unpack_bits_int8,
                NULL,
                SC_unpack_bits_shrt,
                NULL,
                SC_unpack_bits_int,
                NULL,
                SC_unpack_bits_lng,
                NULL,
                SC_unpack_bits_ll,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
   };


#endif

