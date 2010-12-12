#
# SCFNCA.C - some more core routines used by many packages
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

template<int8_t|long long>
void SC_unpack_bits(char *out, long i, long fld)
   {<TYPE> *pv;

    pv    = (<TYPE> *) out;
    pv[i] = (<TYPE>) fld;

    return;}
