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
	    								    
PM_field
 REAL_Opers     = {(PFByte) PM_fplus,  (PFByte) PM_fminus,   NULL,
		   (PFByte) PM_ftimes, (PFByte) PM_fdivide},
 Int_Opers      = {(PFByte) PM_iplus,  (PFByte) PM_iminus,   NULL, 
		   (PFByte) PM_itimes, (PFByte) PM_idivide},
 Long_Opers     = {(PFByte) PM_lplus,  (PFByte) PM_lminus,   NULL, 
                   (PFByte) PM_ltimes, (PFByte) PM_ldivide},
 *PM_REAL_Opers = &REAL_Opers,
 *PM_Int_Opers  = &Int_Opers,
 *PM_Long_Opers = &Long_Opers;

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

/*                           BIGINT OPERATORS                               */

/*--------------------------------------------------------------------------*/

/* PM_LPLUS - BIGINT integer addition in C */

BIGINT PM_lplus(BIGINT x, BIGINT y)
   {

    return(x + y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LMINUS - BIGINT integer subtraction in C */

BIGINT PM_lminus(BIGINT x, BIGINT y)
   {

    return(x-y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LTIMES - BIGINT integer multiplication in C */

BIGINT PM_ltimes(BIGINT x, BIGINT y)
   {

    return(x*y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LDIVIDE - BIGINT integer division in C */

BIGINT PM_ldivide(BIGINT x, BIGINT y)
   {BIGINT rv;

    if (_PM.bmx == 0)
       SC_fix_lmt(sizeof(BIGINT), NULL, &_PM.bmx, NULL);

    if (y != 0)
       rv = x/y;
    else
       rv = _PM.bmx;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LMODULO - return the remainder after BIGINT integer division in C */

BIGINT PM_lmodulo(BIGINT x, BIGINT y)
   {BIGINT rv;

    if (y != 0)
       rv = x % y;
    else
       rv = (BIGINT) 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LAND - bitwise & (and) in C */

BIGINT PM_land(BIGINT x, BIGINT y)
   {

    return(x & y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LOR - bitwise | (or) in C */

BIGINT PM_lor(BIGINT x, BIGINT y)
   {

    return(x | y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LXOR - bitwise ^ (exclusive or) in C */

BIGINT PM_lxor(BIGINT x, BIGINT y)
   {

    return(x ^ y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LLSHFT - bitwise << in C */

BIGINT PM_llshft(BIGINT x, BIGINT y)
   {

    return(x << y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LRSHFT - bitwise >> in C */

BIGINT PM_lrshft(BIGINT x, BIGINT y)
   {

    return(x >> y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LCMP - bitwise ~ in C */

BIGINT PM_lcmp(BIGINT x)
   {

    return(~x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LEQ - integer == in C */

int PM_leq(BIGINT x, BIGINT y)
   {

    return(x == y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LNEQ - integer != in C */

int PM_lneq(BIGINT x, BIGINT y)
   {

    return(x != y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LGE - integer >= in C */

int PM_lge(BIGINT x, BIGINT y)
   {

    return(x >= y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LGT - integer > in C */

int PM_lgt(BIGINT x, BIGINT y)
   {

    return(x > y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LLE - integer <= in C */

int PM_lle(BIGINT x, BIGINT y)
   {

    return(x <= y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LLT - integer < in C */

int PM_llt(BIGINT x, BIGINT y)
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
   {int n, ret;
    char dt[MAXLINE], st[MAXLINE];
    char *dtyp, *styp;
    void *da, *sa;

    ret  = -1;
    dtyp = NULL;
    styp = NULL;

    if (dst != NULL)
       {n    = dst->length;
	da   = dst->data;
	strcpy(dt, dst->type);
	dtyp = strtok(dt, " *");};

    if (src != NULL)
       {sa   = src->data;
	strcpy(st, src->type);
	styp = strtok(st, " *");};

    if ((dtyp != NULL) && (styp != NULL))
       ret = CONVERT(dtyp, &da, styp, sa, n, rel);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ACC_OPER - do ACC = FNC(ACC, ARR)
 *              - NOTE: ACC must be type double
 */

static int _PM_acc_oper(double (*fnc)(double x, double y), C_array *acc,
			C_array *operand, double val)
   {int i, n, ret;
    char *styp, *dtyp;
    void *sa;
    double *da;

    dtyp = acc->type;
    da   = (double *) acc->data;
    if (strcmp(dtyp, SC_DOUBLE_P_S) != 0)
       ret = FALSE;

    else
       {n   = acc->length;
	ret = TRUE;
	if (operand != NULL)
	   {if ((operand->data != NULL) && (val == -HUGE))
	       {styp = operand->type;
		sa   = operand->data;

		if (strcmp(styp, SC_DOUBLE_P_S) == 0)
		   {_ACC_OPER(da, fnc, double, sa);}
		else if (strcmp(styp, "int *") == 0)
		   {_ACC_OPER(da, fnc, int, sa);}
		else if (strcmp(styp, SC_INTEGER_P_S) == 0)
		   {_ACC_OPER(da, fnc, int, sa);}
		else if (strcmp(styp, SC_LONG_P_S) == 0)
		   {_ACC_OPER(da, fnc, long, sa);}
		else if (strcmp(styp, SC_FLOAT_P_S) == 0)
		   {_ACC_OPER(da, fnc, float, sa);}
		else if (strcmp(styp, SC_SHORT_P_S) == 0)
		   {_ACC_OPER(da, fnc, short, sa);}
		else if (strcmp(styp, SC_STRING_S) == 0)
		   {_ACC_OPER(da, fnc, char, sa);};}
	    								    
	    else if ((operand->data == NULL) && (val != -HUGE))
	       {for (i = 0; i < n; i++)
		    da[i] = (*fnc)(da[i], val);};}

	else
	   ret = FALSE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ACCUMULATE_OPER - do ACC = FNC(ACC, OPERAND)
 *                    - if ACC is NULL allocate it
 *                    - this is used to initialize a cycle
 */

C_array *PM_accumulate_oper(double (*fnc)(double x, double y), C_array *acc,
			    C_array *operand, double val)
   {int n;

    if (acc == NULL)
       {if (operand != NULL)
	   {n   = operand->length;
	    acc = PM_make_array(SC_DOUBLE_P_S, n, NULL);

	    PM_conv_array(acc, operand, FALSE);};}

    else
       _PM_acc_oper(fnc, acc, operand, val);

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

	SFREE(aed);
	SFREE(bed);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
