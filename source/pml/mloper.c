/*
 * MLOPER.C - field operation routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define _ACC_OPER(_a, _f, _ts, _s)                                           \
   {_ts *_sp;                                                                \
    _sp = (_ts *) (_s);                                                      \
    for (i = 0; i < n; i++)                                                  \
   	_a[i] = (*(_f))((double) _a[i], (double) _sp[i]);}
	    								    
/* GOTCHA: handle all primitive types as for SCHEME and SX */
PM_field
 _fp_opers      = {(PFByte) PM_fplus,  (PFByte) PM_fminus,   NULL,
		   (PFByte) PM_ftimes, (PFByte) PM_fdivide},
 _int_opers     = {(PFByte) PM_iplus,  (PFByte) PM_iminus,   NULL, 
	 	   (PFByte) PM_itimes, (PFByte) PM_idivide},
 _long_opers    = {(PFByte) PM_lplus,  (PFByte) PM_lminus,   NULL, 
                   (PFByte) PM_ltimes, (PFByte) PM_ldivide};

PM_global_state
 PM_gs = { &_fp_opers, &_int_opers, &_long_opers, };

PM_state
 _PM = { -1, 0.05, -1.0, -1.0, 3.0e-8 };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IPLUS - integer addition in C */

int PM_iplus(int x, int y)
   {

    return(x+y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IMINUS - integer subtraction in C */

int PM_iminus(int x, int y)
   {

    return(x - y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ITIMES - integer multiplication in C */

int PM_itimes(int x, int y)
   {

    return(x*y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IDIVIDE - integer division in C */

int PM_idivide(int x, int y)
   {

    if (y != 0)
       return(x/y);
    else
       return(INT_MAX);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IMODULO - return the remainder after integer division in C */

int PM_imodulo(int x, int y)
   {

    if (y != 0)
       return(x % y);
    else
       return(0);}

/*--------------------------------------------------------------------------*/

/*                           int64_t OPERATORS                               */

/*--------------------------------------------------------------------------*/

/* PM_LPLUS - int64_t integer addition in C */

int64_t PM_lplus(int64_t x, int64_t y)
   {

    return(x + y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LMINUS - int64_t integer subtraction in C */

int64_t PM_lminus(int64_t x, int64_t y)
   {

    return(x-y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LTIMES - int64_t integer multiplication in C */

int64_t PM_ltimes(int64_t x, int64_t y)
   {

    return(x*y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LDIVIDE - int64_t integer division in C */

int64_t PM_ldivide(int64_t x, int64_t y)
   {int64_t rv;

    if (_PM.bmx == 0)
       SC_fix_lmt(sizeof(int64_t), NULL, &_PM.bmx, NULL);

    if (y != 0)
       rv = x/y;
    else
       rv = _PM.bmx;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LMODULO - return the remainder after int64_t integer division in C */

int64_t PM_lmodulo(int64_t x, int64_t y)
   {int64_t rv;

    if (y != 0)
       rv = x % y;
    else
       rv = (int64_t) 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LAND - bitwise & (and) in C */

int64_t PM_land(int64_t x, int64_t y)
   {

    return(x & y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LOR - bitwise | (or) in C */

int64_t PM_lor(int64_t x, int64_t y)
   {

    return(x | y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LXOR - bitwise ^ (exclusive or) in C */

int64_t PM_lxor(int64_t x, int64_t y)
   {

    return(x ^ y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LLSHFT - bitwise << in C */

int64_t PM_llshft(int64_t x, int64_t y)
   {

    return(x << y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LRSHFT - bitwise >> in C */

int64_t PM_lrshft(int64_t x, int64_t y)
   {

    return(x >> y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LCMP - bitwise ~ in C */

int64_t PM_lcmp(int64_t x)
   {

    return(~x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LEQ - integer == in C */

int PM_leq(int64_t x, int64_t y)
   {

    return(x == y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LNEQ - integer != in C */

int PM_lneq(int64_t x, int64_t y)
   {

    return(x != y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LGE - integer >= in C */

int PM_lge(int64_t x, int64_t y)
   {

    return(x >= y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LGT - integer > in C */

int PM_lgt(int64_t x, int64_t y)
   {

    return(x > y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LLE - integer <= in C */

int PM_lle(int64_t x, int64_t y)
   {

    return(x <= y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LLT - integer < in C */

int PM_llt(int64_t x, int64_t y)
   {

    return(x < y);}

/*--------------------------------------------------------------------------*/

/*                           DOUBLE OPERATORS                               */

/*--------------------------------------------------------------------------*/

/* PM_FPLUS - floating point addition in C */

double PM_fplus(double x, double y)
   {

    return(x + y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FMINUS - floating point subtraction in C */

double PM_fminus(double x, double y)
   {

    return(x-y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FTIMES - floating point multiplication in C */

double PM_ftimes(double x, double y)
   {

    return(x*y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FDIVIDE - floating point division in C */

double PM_fdivide(double x, double y)
   {double rv;

    if (y != 0.0)
       rv = x/y;
    else
       rv = DBL_MAX;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FMIN - floating point min in C */

double PM_fmin(double x, double y)
   {

    return(min(x, y));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FMAX - floating point max in C */

double PM_fmax(double x, double y)
   {

    return(max(x, y));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFEQ - floating point == in C */

double PM_ffeq(double x, double y)
   {double d, a;

    d = 2.0*(x - y)/(ABS(x) + ABS(y) + SMALL);
    a = ABS(d);

    return((double) (a < 1.0e-15));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFNEQ - floating point != in C */

double PM_ffneq(double x, double y)
   {

    return((double) (x != y));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFGE - floating point >= in C */

double PM_ffge(double x, double y)
   {

    return((double) (x >= y));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFGT - floating point > in C */

double PM_ffgt(double x, double y)
   {

    return((double) (x > y));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFLE - floating point <= in C */

double PM_ffle(double x, double y)
   {

    return((double) (x <= y));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFLT - floating point < in C */

double PM_fflt(double x, double y)
   {

    return((double) (x < y));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FEQ - floating point == in C */

int PM_feq(double x, double y)
   {double d, a;

    d = 2.0*(x - y)/(ABS(x) + ABS(y) + SMALL);
    a = ABS(d);

    return(a < 1.0e-15);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FNEQ - floating point != in C */

int PM_fneq(double x, double y)
   {

    return(x != y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FGE - floating point >= in C */

int PM_fge(double x, double y)
   {

    return(x >= y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FGT - floating point > in C */

int PM_fgt(double x, double y)
   {

    return(x > y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FLE - floating point <= in C */

int PM_fle(double x, double y)
   {

    return(x <= y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FLT - floating point < in C */

int PM_flt(double x, double y)
   {

    return(x < y);}

/*--------------------------------------------------------------------------*/

/*                           VECTOR OPERATOR ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* PM_CONV_ARRAY - convert the data in SRC into the type
 *               - for the array DST
 *               - free the array if REL is TRUE
 */

int PM_conv_array(C_array *dst, C_array *src, int rel)
   {int n, sid, did, ret;
    char t[MAXLINE];
    void *da, *sa;

    ret = -1;

    if ((dst != NULL) && (src != NULL))
       {n  = dst->length;
	da = dst->data;
	strcpy(t, dst->type);
	did = SC_type_id(strtok(t, " *"), FALSE);

	sa = src->data;
	strcpy(t, src->type);
	sid = SC_type_id(strtok(t, " *"), FALSE);
	
	if ((did != -1) && (sid != -1))
	   {da  = SC_convert_id(did, da, 0, 1, sid, sa, 0, 1, n, rel);
	    ret = (da != NULL);};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ACC_OPER - do ACC = FNC(ACC, ARR)
 *              - NOTE: ACC must be type double
 */

static int _PM_acc_oper(PFVoid *proc, C_array *acc,
			C_array *operand, int id, void *val)
   {int i, n, ret, sid, did;
    char *styp, *dtyp;
    void *sa;

    dtyp = acc->type;
    did  = SC_deref_id(dtyp, TRUE);

    ret = FALSE;
    n   = acc->length;
    if ((operand != NULL) && (operand->data != NULL) && (id == -1))
       {styp = operand->type;
	sa   = operand->data;
	sid  = SC_deref_id(styp, TRUE);

/* fixed and floating point types (ok) */
	if ((sid == SC_BOOL_I) ||
	    (SC_is_type_char(sid) == TRUE) ||
	    (SC_is_type_fix(sid) == TRUE) ||
	    (SC_is_type_fp(sid) == TRUE))
	   {double *s, *da;
	    PFDoubledd fnc;

	    da  = (double *) acc->data;
	    fnc = (PFDoubledd) proc[0];

	    s = SC_convert_id(did, NULL, 0, 1, sid, sa, 0, 1, n, FALSE);

	    for (i = 0; i < n; i++)
	        da[i] = fnc(da[i], s[i]);

	    CFREE(s);

	    ret = TRUE;}

/* complex floating point types (ok) */
	else if (SC_is_type_cx(sid) == TRUE)
	   {double _Complex *s, *da;
	    PFComplexcc fnc;

	    da  = (double _Complex *) acc->data;
	    fnc = (PFComplexcc) proc[1];

	    s = SC_convert_id(did, NULL, 0, 1, sid, sa, 0, 1, n, FALSE);

	    for (i = 0; i < n; i++)
	        da[i] = fnc(da[i], s[i]);

	    CFREE(s);

	    ret = TRUE;}

	else if (sid == SC_QUATERNION_I)
	   {quaternion *s, *da;
	    PFQuaternionqq fnc;

	    da  = (quaternion *) acc->data;
	    fnc = (PFQuaternionqq) proc[2];

	    s = SC_convert_id(did, NULL, 0, 1, sid, sa, 0, 1, n, FALSE);

	    for (i = 0; i < n; i++)
	        da[i] = fnc(da[i], s[i]);

	    CFREE(s);

	    ret = TRUE;};}

    else if (id != -1)
       {if (operand == NULL)
           sid = id;
        else
	   sid = SC_deref_id(operand->type, TRUE);

/* fixed and floating point types (ok) */
	if ((sid == SC_BOOL_I) ||
	    (SC_is_type_char(sid) == TRUE) ||
	    (SC_is_type_fix(sid) == TRUE) ||
	    (SC_is_type_fp(sid) == TRUE))
	   {double s, *da;
	    PFDoubledd fnc;

	    da  = (double *) acc->data;
	    fnc = (PFDoubledd) proc[0];

	    SC_convert_id(did, &s, 0, 1, id, val, 0, 1, 1, FALSE);

	    for (i = 0; i < n; i++)
	        da[i] = fnc(da[i], s);

	    ret = TRUE;}

/* complex floating point types (ok) */
	else if (SC_is_type_cx(sid) == TRUE)
	   {double _Complex s, *da;
	    PFComplexcc fnc;

	    da  = (double _Complex *) acc->data;
	    fnc = (PFComplexcc) proc[1];

	    SC_convert_id(did, &s, 0, 1, id, val, 0, 1, 1, FALSE);

	    for (i = 0; i < n; i++)
	        da[i] = fnc(da[i], s);

	    ret = TRUE;}

	else if (sid == SC_QUATERNION_I)
	   {quaternion s, *da;
	    PFQuaternionqq fnc;

	    da  = (quaternion *) acc->data;
	    fnc = (PFQuaternionqq) proc[2];

	    SC_convert_id(did, &s, 0, 1, id, val, 0, 1, 1, FALSE);

	    for (i = 0; i < n; i++)
	        da[i] = fnc(da[i], s);

	    ret = TRUE;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ACCUMULATE_OPER - do ACC = FNC(ACC, OPERAND)
 *                    - if ACC is NULL allocate it
 *                    - this is used to initialize a cycle
 */

C_array *PM_accumulate_oper(PFVoid *proc, C_array *acc,
			    C_array *operand, int id, void *val)
   {int n;

    if (acc == NULL)
       {if (operand != NULL)
	   {n   = operand->length;
	    acc = PM_make_array(SC_DOUBLE_P_S, n, NULL);

	    PM_conv_array(acc, operand, FALSE);};}

    else
       _PM_acc_oper(proc, acc, operand, id, val);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SET_EQUAL - return TRUE iff the two sets are equal upto
 *              - tolerance
 */

int PM_set_equal(PM_set *a, PM_set *b, double tol)
   {int id, nde, ne, ok;
    char *typ;
    double *aed, *bed;
    void **ae, **be;

    ok = TRUE;

    ok &= (a->dimension == b->dimension);
    ok &= (a->dimension_elem == b->dimension_elem);
    ok &= (a->n_elements == b->n_elements);

    if ((a->element_type != NULL) && (b->element_type != NULL))
       ok &= (strcmp(a->element_type, b->element_type) == 0);
    else
       ok = FALSE;

    if ((a->es_type != NULL) && (b->es_type != NULL))
       ok &= (strcmp(a->es_type, b->es_type) == 0);

    if ((a->symmetry_type != NULL) && (b->symmetry_type != NULL))
       ok &= (strcmp(a->symmetry_type, b->symmetry_type) == 0);

    if ((a->topology_type != NULL) && (b->topology_type != NULL))
       ok &= (strcmp(a->topology_type, b->topology_type) == 0);

    if ((a->info_type != NULL) && (b->info_type != NULL))
       ok &= (strcmp(a->info_type, b->info_type) == 0);

    if (ok)
       {ae = a->elements;
	be = b->elements;

	nde = a->dimension_elem;
	ne  = a->n_elements;
	typ = a->element_type;

/* check the elements */
	aed = NULL;
	bed = NULL;
	ok  = TRUE;
	for (id = 0; (id < nde) && ok; id++)
	    {aed = PM_array_real(typ, ae[id], ne, aed);
	     bed = PM_array_real(typ, be[id], ne, bed);

	     ok = PM_array_equal(aed, bed, ne, tol);};

	CFREE(aed);
	CFREE(bed);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
