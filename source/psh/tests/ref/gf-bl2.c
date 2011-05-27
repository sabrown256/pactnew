/*
 * GF-BL2.C - F90 wrappers for bl2
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */


#include "cpyright.h"
#include "bl2_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *FF_ID(fbcv1, FBCV1)(int *sa, float *sb, double *sc, char *sd, PFInt fe, PFInt ff)
   {int _lsa;
    float _lsb;
    double _lsc;
    char _lsd;
    PFInt _lfe;
    PFInt _lff;
    char *_rv;

    _lsa       = (int) *sa;
    _lsb       = (float) *sb;
    _lsc       = (double) *sc;
    _lsd       = (char) *sd;
    _lfe       = (PFInt) fe;
    _lff       = (PFInt) ff;

    _rv = fbcv1(_lsa, _lsb, _lsc, _lsd, _lfe, _lff);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *FF_ID(fbmcv1, FBMCV1)(int *sa, float *sb, double *sc, char *sd)
   {int _lsa;
    float _lsb;
    double _lsc;
    char _lsd;
    char *_rv;

    _lsa       = (int) *sa;
    _lsb       = (float) *sb;
    _lsc       = (double) *sc;
    _lsd       = (char) *sd;

    _rv = fbmcv1(_lsa, _lsb, _lsc, _lsd);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *FF_ID(fbmcr2, FBMCR2)(int *aa, float *ab, double *ac, char *ad, int sncad)
   {int *_laa;
    float *_lab;
    double *_lac;
    char _lad[MAXLINE];
    char *_rv;

    _laa       = (int *) aa;
    _lab       = (float *) ab;
    _lac       = (double *) ac;
    SC_FORTRAN_STR_C(_lad, ad, sncad);

    _rv = fbmcr2(_laa, _lab, _lac, _lad);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
