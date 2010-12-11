
template<char|long double>
int PM_find_index(void *p, double f, int n)
   {int indx;
    <TYPE> *d, v;
    long i, j;

    d    = (<TYPE> *) p;
    v    = (<TYPE>) f;
    indx = 0L;
    if (v > d[0])
       {indx = n - 1;
	if (v < d[indx])
	   {i = 0L;
	    j = indx;
	    for (indx = (i+j) >> 1L; indx > i; indx = (i+j) >> 1L)
	        {if (v < d[indx])
		    j = indx;
		 else
		    i = indx;};};};
    indx++;

    return(indx);}

template<char|long double>
void PM_min_max(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    <TYPE> *d, *pvn, *pvx, vn, vx, v;
    d   = (<TYPE> *) p;
    pvn = (<TYPE> *) pn;
    pvx = (<TYPE> *) px;
    vn  =  <MAX>;
    vx  = -<MAX>;
    in  = -1;
    ix  = n;
    for (i = 0; i < n; i++)
        {v = *d++;
	 if (v < vn)
            {vn = v;
             in = i;};
	 if (v > vx)
            {vx = v;
             ix = i;};};
    if (imn != NULL)
       *imn = in;
    if (imx != NULL)
       *imx = ix;
    if (pvn != NULL)
       *pvn = vn;
    if (pvx != NULL)
       *pvx = vx;}

template<char|long double>
int _PM_find_index_fast(void *p, double f, int n, int *plast)
   {int last;
    <TYPE> *d, v;
    long imn, imx;

    last = *plast;

    d   = (<TYPE> *) p;
    v   = (<TYPE>) f;
    imn = 0L;
    imx = n - 1L;
    for (indx = last; indx > imn; indx = (imn + imx) >> 1L)
        {if (v < d[indx])
	    imx = indx;
	 else
	    imn = indx;};
    last = ++indx;

    *plast = last;

    return(indx);}

template<char|long double>
int _PM_find_value(int nx, void *x, int (*prd)(double u, double v),
                   double v, int *nout, int **out, int nin, int *in)
   {int i, no, init;
    int *oind;
    <TYPE> *f;
    SC_array *ta;
    double u;

    init = FALSE;
    f    = (<TYPE> *) x;
    no   = 0;
    oind = NULL;
    if ((nin > 0) && (in != NULL))
       {for (i = 0; i < nin; i++)
            {u = (double) f[in[i]];
             if ((*prd)(u, v))
                {if (!init)
                    {ta   = SC_MAKE_ARRAY("FIND_VALUE", int, NULL);
                     init = TRUE;}
                 SC_array_push(ta, in+i);};};}
    else
       {for (i = 0; i < nx; i++)
            {u = (double) f[i];
             if ((*prd)(u, v))
                {if (init == FALSE)
                    {ta   = SC_MAKE_ARRAY("FIND_VALUE", int, NULL);
                     init = TRUE;}
                 SC_array_push(ta, &i);};};};

    if (init == TRUE)
       {no   = SC_array_get_n(ta);
        oind = FMAKE_N(int, no, "FIND_VALUE:oind");
        memcpy(oind, ta->array, no*sizeof(int));
        SC_free_array(ta, NULL);};

    *nout = no;
    *out  = oind;

    return(no);}

template<float _Complex|quaternion>
#define FIND_VALUE_TUPLE(<TYPE>, _ipt, nx, x, prd, val, _no, _o, _ni, _in)
   {int _i, _init;
    long _ne;
    <TYPE> *_f;
    SC_array *_ta;
    double _u, _v;
    _init = FALSE;
    _f    = (<TYPE> *) (x);
    _v    = (val);
    *_no  = 0;
    if ((_ni > 0) && (_in != NULL))
       {_ne = (_ipt)*(_ni);
        for (_i = 0; _i < _ne; _i++)
            {_u = (double) (_f[_in[_i]]);
             if ((*prd)(_u, _v))
                {if (!_init)
                    {_ta   = SC_MAKE_ARRAY("FIND_VALUE_TUPLE", int, NULL);
                     _init = TRUE;}
                 SC_array_push(_ta, (_in)+(_i));};};}
    else
       {_ne = (_ipt)*(nx);
        for (_i = 0; _i < _ne; _i++)
            {_u = (double) (_f[_i]);
             if ((*prd)(_u, _v))
                {if (_init == FALSE)
                    {_ta   = SC_MAKE_ARRAY("FIND_VALUE_TUPLE", int, NULL);
                     _init = TRUE;}
                 SC_array_push(_ta, &(_i));};};}
    if (_init == TRUE)
       {*(_no) = SC_array_get_n(_ta);
        *(_o)  = FMAKE_N(int, *_no, "FIND_VALUE_TUPLE:_o");
        memcpy(*(_o), _ta->array, (*(_no))*sizeof(int));
        SC_free_array(_ta, NULL);};}
