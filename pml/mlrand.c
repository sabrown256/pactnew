/*
 * MLRAND.C - random number generation routines
 *          - most of the routines were written by Jim Rathkopf, LLNL
 *          - with modifications for portability and efficiency by SAB
 *          - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#ifndef RAND_MAX
#define RAND_MAX 2147483647.0
#endif

#define BASE24        16777216.0                                    /* 2^24 */
#define TWO8               256.0                                    /* 2^8 */
#define TWO16            65536.0                                    /* 2^16 */
#define TWO48  281474976710656.0                                    /* 2^48 */

#define UPPERPART(r)            floor(r/BASE24)
#define LOWERPART(r, rupper)    (r - rupper*BASE24)

typedef union u_rand_seed rand_seed;

union u_rand_seed
   {double sd;
    unsigned short sa[4];};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_RANDOM_48 - generate pseudo-random numbers through a 48-bit linear
 *               - congruential algorithm
 *               - this emulates the drand48 library
 *               - of random number generators available on many, but not all, 
 *               - UNIX machines
 *               - return a double x such that 0.0 <= x < 1.0
 *
 * algorithm:
 *    x(n+1) = (a*x(n) + c)mod m
 *
 *    where
 *
 *    defaults, for standard unix rand48, Cray defaults used in coding
 *
 *         double name               hexdecimal         decimal
 *   x - seed-dseed[1], [2]         1234abcd330e      20017429951246 
 *   a - multiplier-dmult[1], [2]    5deece66d         25214903917
 *   c - adder-dadder                  b                  11
 *   m -                        2**481000000000000  281474976710656
 *
 *    24-bit defaults (decimal) (lower bits listed first)
 *   x - dseed[1], [2]- 13447950.0, 1193131.0
 *   a - dmult[1], [2]- 15525485.0, 1502.0
 *   c - dadder- 11.0
 *
 */

static double _PM_random_48(void)
   {double t1_48, t2_48, t1_24[2], t2_24[2];
    double d;
    double *pd, *pm;
    PM_smp_state *ma;

    ma = _PM_get_state(-1);

    pd = ma->dseed;
    pm = ma->dmult;

/* perform 48-bit arithmetic using two part data */
    t1_48 = pm[0]*pd[0] + ma->dadder;
    t2_48 = pm[1]*pd[0] + pm[0]*pd[1];

    t1_24[1] = UPPERPART(t1_48);
    t1_24[0] = LOWERPART(t1_48, t1_24[1]);

    t2_24[1] = UPPERPART(t2_48);
    t2_24[0] = LOWERPART(t2_48, t2_24[1]);

    t2_48 = t2_24[0] + t1_24[1];

    t2_24[1] = UPPERPART(t2_48);
    t2_24[0] = LOWERPART(t2_48, t2_24[1]);

    d = (pd[0] + pd[1]*BASE24)/TWO48;

    pd[0] = t1_24[0];
    pd[1] = t2_24[0];

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_RAND48_16TO24 - take a number stored in 3 16-bit shorts and move to
 *                   - 2 doubles each containing 24 bits of data
 *                   -   X16, the 3 16-bit shorts
 *                   -   X24, the 2 doubles of 24 bits returned
 *
 */

static void _PM_rand48_16to24(unsigned short x16[3], double x24[2])
   {double t0, t1, t2, t1u, t1l;

    t0 = (double) x16[0];
    t1 = (double) x16[1];
    t2 = (double) x16[2];

    t1u = floor(t1/TWO8);
    t1l = t1 - t1u*TWO8;

    x24[0] = t0 + t1l*TWO16;
    x24[1] = t1u + t2*TWO8;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_RAND48_24TO16 - take a number stored in 2 doubles each containing
 *                   - 24 bits of data move to 3 16-bit shorts
 *                   -   X24, the 2 doubles of 24 bits
 *                   -   X16, the 3 16-bit shorts returned
 */

static void _PM_rand48_24to16(double x24[2], unsigned short x16[3])
   {double t0u, t0l, t1u, t1l;

    t0u = floor(x24[0]/TWO16);
    t0l = x24[0] - t0u*TWO16;

    t1u = floor(x24[1]/TWO8);
    t1l = x24[1] - t1u*TWO8;

    x16[0] = t0l;
    x16[1] = t0u + t1l*TWO8;
    x16[2] = t1u;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SEED_48 - convert a 3 by 16 representation of a seed
 *             - return a pointer to old seed
 *             -   RS, new seed
 *
 */

static void _PM_seed_48(unsigned short rs[3], unsigned short aso[3])
   {double sdo[2], *pd;
    PM_smp_state *ma;

    ma = _PM_get_state(-1);

    pd = ma->dseed;

    sdo[0] = pd[0];
    sdo[1] = pd[1];

    _PM_rand48_24to16(sdo, aso);
    _PM_rand48_16to24(rs, pd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_RNSET - set random number generator seed, multiplier
 *           -   S, the random number seed
 *           -   MLT, the random number multiplier
 */

static void _PM_rnset(unsigned short s[3], unsigned short mlt[3])
   {double *pd, *pm;
    PM_smp_state *ma;

    ma = _PM_get_state(-1);

    pd = ma->dseed;
    pm = ma->dmult;

/* construct 24-bit versions of 'seed' and 'mlt'
 * and store in 64-bit doubles
 */
    _PM_rand48_16to24(s, pd);
    _PM_rand48_16to24(mlt, pm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SRAND_48 - change the value of the random number seed
 *             - ranset must be passed a 64 bit double
 */

void PM_srand_48(double x)
   {int j;
    unsigned short wrk_seed[3];
    rand_seed seed;
    PM_smp_state *ma;

    ma = _PM_get_state(-1);

    if (x == 0.0)
       {wrk_seed[0] = 0x9cd1;
        wrk_seed[1] = 0x53fc;
        wrk_seed[2] = 0x9482;}

/* store x in wrk_seed */
    else
       {seed.sd = x;
        for (j = 0; j < 3; j++)
            wrk_seed[j] = seed.sa[j];};

   _PM_rnset(wrk_seed, ma->multplr);
  
   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_GRAND_48 - return the value of the current seed */

double PM_grand_48(void)
   {int j;
    unsigned short wrk_seed[3];
    rand_seed seed;
    PM_smp_state *ma;

    ma = _PM_get_state(-1);

/* get current seed */
    _PM_seed_48(_PM.dseed, wrk_seed);

/* set seed and multiplier */
    _PM_rnset(wrk_seed, ma->multplr);

/* store wrk_seed in seed */
    for (j = 0; j < 3; j++)
        seed.sa[j] = wrk_seed[j];

    seed.sa[3] = 0x0;
  
    return(seed.sd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RANDOM_48 - generate a pseudo-random number
 *              - return a pseudo-random number between -1 and 1
 *              - use the linear congruential sequence function
 *
 * #bind PM_random_48 fortran() scheme() python()
 */

double PM_random_48(double x)
   {double v;

    v = 2.0*_PM_random_48() - 1.0;
        
    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RANDOM_S - return a random number between -1 and 1
 *             - use the system rand function
 *
 * #bind PM_random_s fortran() scheme() python()
 */

double PM_random_s(double x)
   {double v;

    if (_PM.rn == 0.0)
       _PM.rn = 2.0/RAND_MAX;

    v = _PM.rn*rand() - 1.0;
        
    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

