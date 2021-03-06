/*
 * GF-BL5.C - F90 wrappers for bl5
 *          - NOTE: this file was automatically generated by blang
 *          - any manual changes will not be effective
 *
 */

#include "cpyright.h"
#include "sx_int.h"
#include "bl5_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f1(PFDouble a)| */

void FF_ID(a_f1_f, A_F1_F)(PFDouble a)
   {
    
    a_f1(a);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f2(int (*b)(int *a))| */

void FF_ID(a_f2_f, A_F2_F)(int (*b)(int *a))
   {
    
    a_f2(b);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f3(PFDouble a, int (*b)(int *a))| */

void FF_ID(a_f3_f, A_F3_F)(PFDouble a, int (*b)(int *a))
   {
    
    a_f3(a, b);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f4(int (*b)(int *a), PFDouble a)| */

void FF_ID(a_f4_f, A_F4_F)(int (*b)(int *a), PFDouble a)
   {
    
    a_f4(b, a);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f1(PFDouble a, int b)| */

void FF_ID(b_f1_f, B_F1_F)(PFDouble a, int *b)
   {
    int _lb;
    
    _lb        = (int) *b;

    B_f1(a, _lb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f2(int (*a)(int *a), int b)| */

void FF_ID(b_f2_f, B_F2_F)(int (*a)(int *a), int *b)
   {
    int _lb;
    
    _lb        = (int) *b;

    B_f2(a, _lb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f3(PFDouble a, int (*b)(int *a), int c)| */

void FF_ID(b_f3_f, B_F3_F)(PFDouble a, int (*b)(int *a), int *c)
   {
    int _lc;
    
    _lc        = (int) *c;

    B_f3(a, b, _lc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f4(int (*a)(int *a), PFDouble b, int c)| */

void FF_ID(b_f4_f, B_F4_F)(int (*a)(int *a), PFDouble b, int *c)
   {
    int _lc;
    
    _lc        = (int) *c;

    B_f4(a, b, _lc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f5(int a, PFDouble b, int (*c)(int *a))| */

void FF_ID(b_f5_f, B_F5_F)(int *a, PFDouble b, int (*c)(int *a))
   {int _la;
    
    _la        = (int) *a;

    B_f5(_la, b, c);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f6(int a, int (*b)(int *a), PFDouble c)| */

void FF_ID(b_f6_f, B_F6_F)(int *a, int (*b)(int *a), PFDouble c)
   {int _la;
    
    _la        = (int) *a;

    B_f6(_la, b, c);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
