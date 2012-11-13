/*
 * STRUCT.C - derived from structmodule.c
 *          - we rely on PDBlib to do all of the file/native conversion
 *          - so we do not need to explicitly pact/unpack floats/doubles
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

#define STATIC
/* #define STATIC static */

/* The translation function for each format character is table driven */

typedef struct _formatdef
   {char format;
    int size;
    int alignment;
    PyObject* (*unpack)(const char *,
                        const struct _formatdef *);
    int (*pack)(char *, PyObject *,
                const struct _formatdef *);} formatdef;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Helper to get a PyLongObject by hook or by crook.  Caller should decref. */

STATIC PyObject *get_pylong(PyObject *v)
   {PyNumberMethods *m;

    assert(v != NULL);
    if (PY_INT_CHECK(v))
       return(PY_INT_LONG(PY_INT_AS_LONG(v)));

    if (PyLong_Check(v))
       {Py_INCREF(v);
	return(v);};

    m = PY_TYPE(v)->tp_as_number;
    if (m != NULL && m->nb_long != NULL)
       {v = m->nb_long(v);
	if (v == NULL)
	   return(NULL);

	if (PyLong_Check(v))
	   return(v);

	Py_DECREF(v);};

    PP_error_set_user(v, "cannot convert argument to long");

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Helper routine to get a Python integer and raise the appropriate error
   if it isn't one */

static int get_long(PyObject *v, long *p)
   {
        long x = PY_INT_AS_LONG(v);
        if (x == -1 && PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_TypeError))
                    PP_error_set_user(v, "required argument is not an integer");
                return(-1);
        }
        *p = x;
    return(0);}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Same, but handling unsigned long */

static int get_ulong(PyObject *v, unsigned long *p)
{
        if (PyLong_Check(v)) {
                unsigned long x = PyLong_AsUnsignedLong(v);
                if (x == (unsigned long)(-1) && PyErr_Occurred())
                        return(-1);
                *p = x;
                return(0);
        }
        else {
                return(get_long(v, (long *)p));
        }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef HAVE_LONG_LONG2_3

/* Same, but handling native long long. */

static int get_longlong(PyObject *v, PY_LONG_LONG *p)
{
        PY_LONG_LONG x;

        v = get_pylong(v);
        if (v == NULL)
                return(-1);
        assert(PyLong_Check(v));
        x = PyLong_AsLongLong(v);
        Py_DECREF(v);
        if (x == (PY_LONG_LONG)-1 && PyErr_Occurred())
                return(-1);
        *p = x;
        return(0);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Same, but handling native unsigned long long. */

static int get_ulonglong(PyObject *v, unsigned PY_LONG_LONG *p)
{
        unsigned PY_LONG_LONG x;

        v = get_pylong(v);
        if (v == NULL)
                return(-1);
        assert(PyLong_Check(v));
        x = PyLong_AsUnsignedLongLong(v);
        Py_DECREF(v);
        if (x == (unsigned PY_LONG_LONG)-1 && PyErr_Occurred())
                return(-1);
        *p = x;
        return(0);
}

#endif

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* Floating point helpers
 *     const char *p   start of 4-byte string
 *     int le          TRUE for little-endian, FALSE for big-endian
 */

static PyObject *unpack_float(const char *p, int le)
   {double x;

    x = _PyFloat_Unpack4((unsigned char *)p, le);
    if (x == -1.0 && PyErr_Occurred())
       return(NULL);

    return(PyFloat_FromDouble(x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *unpack_double(const char *p, int le)
   {double x;

    x = _PyFloat_Unpack8((unsigned char *)p, le);
    if (x == -1.0 && PyErr_Occurred())
       return(NULL);

    return(PyFloat_FromDouble(x));}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* NOTE:
 *  In all n[up]_<type> routines handling types larger than 1 byte, there is
 *  *no* guarantee that the p pointer is properly aligned for each type,
 *  therefore memcpy is called.  An intermediate variable is used to
 *  compensate for big-endian architectures.
 *  Normally both the intermediate variable and the memcpy call will be
 *  skipped by C optimisation in little-endian architectures (gcc >= 2.91
 *  does this)
 */

static PyObject *nu_char(void *p, long nitems)
   {

    return(PY_STRING_STRING_SIZE(p, nitems));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *nu_byte(void *p, long nitems)
   {

    return(PY_INT_LONG(*(signed char *)p));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *nu_ubyte(void *p, long nitems)
   {

    return(PY_INT_LONG(*(unsigned char *)p));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *nu_short(void *p, long nitems)
   {short x;

    memcpy((char *) &x, p, sizeof x);

    return(PY_INT_LONG((long)x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC PyObject *nu_ushort(void *p, long nitems)
   {unsigned short x;

    memcpy((char *) &x, p, sizeof x);

    return(PY_INT_LONG((long)x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *nu_int(void *p, long nitems)
   {int x;

    memcpy((char *) &x, p, sizeof x);

    return(PY_INT_LONG((long)x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC PyObject *nu_uint(void *p, long nitems)
   {unsigned int x;

    memcpy((char *) &x, p, sizeof x);

    return(PyLong_FromUnsignedLong((unsigned long)x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *nu_long(void *p, long nitems)
   {long x;

    memcpy((char *) &x, p, sizeof x);

    return(PY_INT_LONG(x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC PyObject *nu_ulong(void *p, long nitems)
   {unsigned long x;

    memcpy((char *) &x, p, sizeof x);

    return(PyLong_FromUnsignedLong(x));}

/*--------------------------------------------------------------------------*/

#ifdef HAVE_LONG_LONG2_3

/*--------------------------------------------------------------------------*/

/* Native mode doesn't support q or Q unless the platform C supports
 * long long (or, on Windows, __int64).
 */

static PyObject *nu_longlong(void *p, long nitems)
   {PY_LONG_LONG x;

    memcpy((char *) &x, p, sizeof x);

    return(PyLong_FromLongLong(x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *nu_ulonglong(void *p, long nitems)
   {unsigned PY_LONG_LONG x;

    memcpy((char *) &x, p, sizeof x);

    return(PyLong_FromUnsignedLongLong(x));}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

static PyObject *nu_float(void *p, long nitems)
   {float x;

    memcpy((char *) &x, p, sizeof x);

    return(PyFloat_FromDouble((double)x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *nu_double(void *p, long nitems)
   {double x;

    memcpy((char *) &x, p, sizeof x);

    return(PyFloat_FromDouble(x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC PyObject *nu_void_p(void *p, long nitems)
   {void *x;
    
    memcpy((char *) &x, p, sizeof x);
    
    return(PyLong_FromVoidPtr(x));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int np_byte(void *p, PyObject *v, long nitems, PP_types tc)
   {long x;

    if (get_long(v, &x) < 0)
       return(-1);

    if (x < -128 || x > 127)
       {PP_error_set_user(v,
			  "byte format requires -128<=number<=127");
	return(-1);};

    *((char *) p) = (char) x;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int np_ubyte(void *p, PyObject *v, long nitems, PP_types tc)
   {long x;

    if (get_long(v, &x) < 0)
       return(-1);

    if (x < 0 || x > 255)
       {PP_error_set_user(v,
			  "ubyte format requires 0<=number<=255");
	return(-1);};

    * ((char *) p) = (char)x;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 1

static int np_char(void *p, PyObject *v, long nitems, PP_types tc)
   {Py_ssize_t nc;

    if (!PY_STRING_CHECK(v) || PY_STRING_SIZE(v) > nitems)
       {PP_error_set_user(v, 
			  "char format require string of length %d", nitems);
	return(-1);};

    nc = PY_STRING_SIZE(v);
    if (nc >= nitems)
       memcpy(p, PY_STRING_AS_STRING(v), nitems);

    else
       {memset(p, ' ', nitems);
	memcpy(p, PY_STRING_AS_STRING(v), nc);};

    return(0);}

#else

static int np_char(void *p, PyObject *v, long nitems, PP_types tc)
   {

    if (!PY_STRING_CHECK(v) || PY_STRING_SIZE(v) != 1)
       {PP_error_set_user(v, 
			  "char format require string of length 1");
	return(-1);};

    *((char *) p) = *PY_STRING_AS_STRING(v);

    return(0);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int np_short(void *p, PyObject *v, long nitems, PP_types tc)
   {long x;
    short y;
    
    if (get_long(v, &x) < 0)
       return(-1);

    if (x < SHRT_MIN || x > SHRT_MAX)
       {PP_error_set_user(v, 
			  "short format requires %d<=number<=%d",
			  SHRT_MIN, SHRT_MAX);
	return(-1);};

    y = (short) x;
    memcpy(p, (char *) &y, sizeof y);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC int np_ushort(void *p, PyObject *v, long nitems, PP_types tc)
   {long x;
    unsigned short y;

    if (get_long(v, &x) < 0)
       return(-1);

    if (x < 0 || x > USHRT_MAX)
       {PP_error_set_user(v, "short format requires 0<=number<=%d", SHRT_MAX);
	return(-1);};

    y = (unsigned short)x;
    memcpy(p, (char *) &y, sizeof y);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int np_int(void *p, PyObject *v, long nitems, PP_types tc)
   {long x;
    int y;

    if (get_long(v, &x) < 0)
       return(-1);

    y = (int)x;
    memcpy(p, (char *) &y, sizeof y);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC int np_uint(void *p, PyObject *v, long nitems, PP_types tc)
   {unsigned long x;
    unsigned int y;

    if (get_ulong(v, &x) < 0)
       return(-1);
    
    y = (unsigned int)x;
    memcpy(p, (char *) &y, sizeof y);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int np_long(void *p, PyObject *v, long nitems, PP_types tc)
   {long x;

    if (get_long(v, &x) < 0)
       return(-1);

    memcpy(p, (char *) &x, sizeof x);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC int np_ulong(void *p, PyObject *v, long nitems, PP_types tc)
   {unsigned long x;
    
    if (get_ulong(v, &x) < 0)
       return(-1);

    memcpy(p, (char *) &x, sizeof x);

    return(0);}

/*--------------------------------------------------------------------------*/

#ifdef HAVE_LONG_LONG2_3

/*--------------------------------------------------------------------------*/

static int np_longlong(void *p, PyObject *v, long nitems, PP_types tc)
   {PY_LONG_LONG x;

    if (get_longlong(v, &x) < 0)
       return(-1);

    memcpy(p, (char *) &x, sizeof x);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int np_ulonglong(void *p, PyObject *v, long nitems, PP_types tc)
   {unsigned PY_LONG_LONG x;

    if (get_ulonglong(v, &x) < 0)
       return(-1);

    memcpy(p, (char *) &x, sizeof x);

    return(0);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

static int np_float(void *p, PyObject *v, long nitems, PP_types tc)
   {float x;

    x = (float) PyFloat_AsDouble(v);
    if (x == -1 && PyErr_Occurred())
       {PP_error_set_user(v, "required argument is not a float");
	return(-1);};

    memcpy(p, (char *) &x, sizeof x);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int np_double(void *p, PyObject *v, long nitems, PP_types tc)
   {double x;

    x = PyFloat_AsDouble(v);
    if (x == -1 && PyErr_Occurred())
       {PP_error_set_user(v, "required argument is not a float");
	return(-1);};

    memcpy(p, (char *) &x, sizeof x);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC int np_void_p(void *p, PyObject *v, long nitems, PP_types tc)
   {void *x;

    x = PyLong_AsVoidPtr(v);
    if (x == NULL && PyErr_Occurred())

/* ### hrm. PyLong_AsVoidPtr raises SystemError */
       {if (PyErr_ExceptionMatches(PyExc_TypeError))
	   PP_error_set_user(v, "required argument is not an integer");
	return(-1);};

    memcpy(p, (char *) &x, sizeof x);

    return(0);}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* original table */

static formatdef native_table[] = {
        {'x',   sizeof(char),   0,              NULL},
        {'b',   sizeof(char),   0,              nu_byte,        np_byte},
        {'B',   sizeof(char),   0,              nu_ubyte,       np_ubyte},
        {'c',   sizeof(char),   0,              nu_char,        np_char},
        {'s',   sizeof(char),   0,              NULL},
        {'p',   sizeof(char),   0,              NULL},
        {'h',   sizeof(short),  SHORT_ALIGN,    nu_short,       np_short},
        {'H',   sizeof(short),  SHORT_ALIGN,    nu_ushort,      np_ushort},
        {'i',   sizeof(int),    INT_ALIGN,      nu_int,         np_int},
        {'I',   sizeof(int),    INT_ALIGN,      nu_uint,        np_uint},
        {'l',   sizeof(long),   LONG_ALIGN,     nu_long,        np_long},
        {'L',   sizeof(long),   LONG_ALIGN,     nu_ulong,       np_ulong},
        {'f',   sizeof(float),  FLOAT_ALIGN,    nu_float,       np_float},
        {'d',   sizeof(double), DOUBLE_ALIGN,   nu_double,      np_double},
        {'P',   sizeof(void *), VOID_P_ALIGN,   nu_void_p,      np_void_p},
#ifdef HAVE_LONG_LONG
        {'q',   sizeof(PY_LONG_LONG), LONG_LONG_ALIGN, nu_longlong, np_longlong},
        {'Q',   sizeof(PY_LONG_LONG), LONG_LONG_ALIGN, nu_ulonglong,np_ulonglong},
#endif
        {0}
};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

PP_unpack_func _PP_get_unpack_func(PP_types tc)
   {

    switch (tc)
       {case PP_CHAR_I:
	     return(nu_char);
	case PP_UBYTE_I:
	     return(nu_ubyte);
	case PP_SBYTE_I:
	     return(nu_byte);
	case PP_SHORT_I:
	     return(nu_short);
	case PP_INT_I:
	     return(nu_int);
	case PP_LONG_I:
	     return(nu_long);
	case PP_LONG_LONG_I:
	     return(NULL);  /* return(nu_long_long); */
	case PP_FLOAT_I:
	     return(nu_float);
	case PP_DOUBLE_I:
	     return(nu_double);
	case PP_CFLOAT_I:
	     return(NULL);
	case PP_CDOUBLE_I:
	     return(NULL);
	default:
	     return(NULL);};

    return(NULL);}    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PP_pack_func _PP_get_pack_func(PP_types tc)
   {

    switch (tc)
       {case PP_CHAR_I:
	     return(np_char);
	case PP_UBYTE_I:
	     return(np_ubyte);
	case PP_SBYTE_I:
	     return(np_byte);
	case PP_SHORT_I:
	     return(np_short);
	case PP_INT_I:
	     return(np_int);
	case PP_LONG_I:
	     return(np_long);
	case PP_LONG_LONG_I:
	     return(NULL); /* return(np_long_long); */
	case PP_FLOAT_I:
	     return(np_float);
	case PP_DOUBLE_I:
	     return(np_double);
	case PP_CFLOAT_I:
	     return(NULL);
	case PP_CDOUBLE_I:
	     return(NULL);
	default:
	     return(NULL);};

    return(NULL);}    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
