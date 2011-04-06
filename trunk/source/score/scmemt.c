/*
 * SCMEMT.C - functions to manage per thread state
 *          - including memory heaps
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_mem.h"

#define DELTA_THREAD   64
#define DELTA_ELEMENT  8

typedef struct s_SC_per_thread_reg SC_per_thread_reg;
typedef struct s_memt_state memt_state;

struct s_SC_per_thread_reg
   {char name[80];
    char *type;
    size_t bpi;
    int ni;
    void (*init)(void *a, int id);};

struct s_memt_state
   {int init;                           /* TRUE iff initialized */
    int ntx;                            /* max number of threads */
    int nex;                            /* max number of thread elements */
    int nt;                             /* number of threads */
    int ne;                             /* number of thread elements */
    void **data;                        /* nt x ne array of per thread data */
    SC_per_thread_reg mreg[1000];};     /* methods defining per thread data */

static memt_state
 st = { FALSE, 0, 0, 0, 0, NULL, };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_HEAP - return the heap for thread ID
 *              - use current thread if ID == -1
 */

SC_heap_des *_SC_get_heap(int id)
   {SC_heap_des *ph;

/* heaps are registered at per thread state 0 */
    ph = (SC_heap_des *) SC_get_thread_element(id, 0);

    return(ph);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_THREAD_INFO - return the emu_thread_info for thread ID
 *                     - use current thread if ID == -1
 */

emu_thread_info *_SC_get_thread_info(int id)
   {emu_thread_info *ti;

/* thread are registered at per thread state 1 */
    ti = (emu_thread_info *) SC_get_thread_element(id, 1);

    return(ti);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TO_BUF - return the JMP_BUF for timeouts for thread ID
 *                - use current thread if ID == -1
 */

JMP_BUF *_SC_get_to_buf(int id)
   {JMP_BUF *bf;

/* timeouts are registered at per thread state 2 */
    bf = (JMP_BUF *) SC_get_thread_element(id, 2);

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_EV_LOOP - return the event loop for thread ID
 *                 - use current thread if ID == -1
 */

SC_evlpdes **_SC_get_ev_loop(int id)
   {SC_evlpdes **ev;

/* event loops are registered at per thread state 3 */
    ev = (SC_evlpdes **) SC_get_thread_element(id, 3);

    return(ev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_ERROR_STACK - return the error stack for thread ID
 *                     - use current thread if ID == -1
 */

SC_array *_SC_get_error_stack(int id)
   {SC_array *arr;

/* error stacks are registered at per thread state 4 */
    arr = (SC_array *) SC_get_thread_element(id, 4);

    return(arr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIND_THREAD - return the index of the thread TID */

static int _SC_find_thread(SC_thread tid, int lck)
   {int i, it;
    emu_thread_info *ti;

    if (st.data == NULL)
       it = 0;

    else
       {if (lck == TRUE)
	   SC_LOCKON(SC_ts_lock);

	if (tid == SC_NULL_THREAD)
	   tid = SC_THREAD_SELF();

	for (it = 0; it < st.nt; it++)
	    {i  = it*st.nex + 1;
	     ti = st.data[i];
	     if ((ti == NULL) || (ti->tid == tid))
	        break;
	     else if (ti->tid == SC_NULL_THREAD)
	        {ti->tid = tid;
		 break;};};

#if 0
{int jd;
 jd = SC_current_thread();
 if (it != jd)
    printf("find> %d %d\n", it, jd);};
#endif

        if (lck == TRUE)
           SC_LOCKOFF(SC_ts_lock);};

    return(it);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GROW_THREAD_DATA - allocate and inialize new thread data */

static void _SC_grow_thread_data(int nt, int ne)
   {int i, io, in, it, ie, grow;
    int neo, nto, nen, ntn, ni;
    size_t nbn, bpi, nb;
    void **t, *d;
    SC_per_thread_reg *pt;
		  
    neo = st.nex;
    nto = st.ntx;

    ntn = max(st.nt, nt+1);
    ntn = max(ntn, 1);
    nen = max(st.ne, ne+1);

    grow = FALSE;

    if (ntn > nto)
       {st.ntx = ntn + DELTA_THREAD;
        grow   = TRUE;};

    if (nen > neo)
       {st.nex = nen + DELTA_ELEMENT;
        grow   = TRUE;};

    if (grow == TRUE)
       {nbn = (st.ntx * st.nex)*sizeof(void *);
        t   = malloc(nbn);

	assert(t != NULL);

        memset(t, 0, nbn);

        if (st.data != NULL)
           {for (it = 0; it < nto; it++)
	        {for (ie = 0; ie < neo; ie++)
		     {io = it*neo + ie;
		      in = it*st.nex + ie;
		      t[in] = st.data[io];};};

	    free(st.data);};

        st.data = t;};

    st.nt = ntn;
    st.ne = nen;

    for (it = 0; it < st.nt; it++)
        {for (ie = 0; ie < st.ne; ie++)
             {i = it*st.nex + ie;
	      if (st.data[i] == NULL)
		 {pt  = st.mreg + ie;
		  if (pt != NULL)
		     {bpi = pt->bpi;
		      ni  = pt->ni;
		      nb  = ni*bpi;

		      d = malloc(nb);

		      assert(d != NULL);

		      memset(d, 0, nb);

		      st.data[i] = d;

		      if (pt->init != NULL)
			 (*pt->init)(d, it);};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_THREAD_DATA - register NAME for a piece of per thread data
 *                         - return the ID number of the NAME'd piece
 */

int SC_register_thread_data(char *name, char *type, int ni, size_t bpi,
			    PFTinit init)
   {int ie;

#ifdef S_SPLINT_S
    extern char *strdup(const char *s);
#endif

    SC_register_thread_state();

/* see if this has already been registered */
    for (ie = 0; ie < st.ne; ie++)
        {if (strcmp(st.mreg[ie].name, name) == 0)
	    break;};

    if (ie >= st.ne)
       {IF_SAFE(_SC_init_emu_threads == TRUE, &SC_ts_lock, TRUE)

           SC_strncpy(st.mreg[ie].name, 80, name, -1);
           st.mreg[ie].type = strdup(type);
           st.mreg[ie].ni   = ni;
           st.mreg[ie].bpi  = bpi;
           st.mreg[ie].init = init;

           _SC_grow_thread_data(-1, ie);

        END_SAFE;};

    return(ie);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_THREAD_STATE - register common pieces of per thread state
 *                           - they will all be initialized together
 *                           - at the earliest opportunity
 *                           - this avoids race/lock/bootstrap problems
 */

void SC_register_thread_state(void)
   {

    ONCE_SAFE(FALSE, NULL);

       SC_register_thread_data("heaps", "SC_heap_des", 1,
                               sizeof(SC_heap_des),
			       (PFTinit) _SC_init_heap);

       SC_register_thread_data("threads", "emu_thread_info", 1,
			       sizeof(emu_thread_info),
			       (PFTinit) _SC_eth_init_thread);

       SC_register_thread_data("timeouts", "JMP_BUF", 1,
			       sizeof(JMP_BUF),
			       NULL);

       SC_register_thread_data("events", "SC_evlpdes *", 1,
			       sizeof(SC_evlpdes *),
			       NULL);

       SC_register_thread_data("errors", "SC_array", 1,
			       sizeof(SC_array),
			       (PFTinit) _SC_init_error_stack);

       st.init = TRUE;

    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_THREAD_ELEMENT_NL - return the IEth element for thread IT */

void *SC_get_thread_element_nl(int *pit, int ie, int lck)
   {int i, it;
    void *rv;

    rv = NULL;

    it = *pit;
    if (it < 0)
       it = _SC_find_thread(SC_NULL_THREAD, lck);

    if ((st.ntx <= it) || (st.nex <= ie))
       _SC_grow_thread_data(it, ie);

    i  = it*st.nex + ie;
    rv = st.data[i];

    if (rv == NULL)
       _SC_grow_thread_data(it, ie);

    rv = st.data[i];
    if (rv == NULL)
       fprintf(stdout, "ERROR: no state for thread %d %d (%s) - SC_GET_THREAD_ELEMENT_NL\n",
	       it, ie, st.mreg[ie].name);

    *pit = it;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_THREAD_ELEMENT - return the IEth element for thread IT */

void *SC_get_thread_element(int it, int ie)
   {void *rv;

    SC_register_thread_state();

    SC_LOCKON(SC_ts_lock);

    rv = SC_get_thread_element_nl(&it, ie, TRUE);

    SC_LOCKOFF(SC_ts_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_THREAD_ELEMENTS - return an array of elements for thread IT */

void **SC_get_thread_elements(void *a, int *pne, int it, int lck)
   {int is, ie, ne, nex;
    void **dst, **src;

    dst = NULL;

    SC_register_thread_state();

    IF_SAFE(lck == TRUE, &SC_ts_lock, TRUE)

       if (st.ntx <= it)
          _SC_grow_thread_data(it, -1);

       ne  = st.ne;
       nex = st.nex;
       src = st.data;

       dst = (void **) a;
       if (dst == NULL)
          dst = CMAKE_N(void *, ne);

       for (ie = 0; ie < ne; ie++)
           {is      = it*nex + ie;
            dst[ie] = src[is];};

       *pne = ne;

    END_SAFE;

    return(dst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_ELEMENT_THREADS - return an array of elements IE for all threads */

void **SC_get_element_threads(void *a, int *pnt, int ie, int lck)
   {int is, it, ne, nt, nex;
    void **dst, **src;

    dst = NULL;

    SC_register_thread_state();

    IF_SAFE(lck == TRUE, &SC_ts_lock, TRUE)

       if (st.nex <= ie)
          _SC_grow_thread_data(-1, ie);

       ne  = st.ne;
       nt  = st.nt;
       nex = st.nex;
       src = st.data;

       dst = (void **) a;
       if (dst == NULL)
          dst = CMAKE_N(void *, ne);

       for (it = 0; it < nt; it++)
           {is      = it*nex + ie;
            dst[it] = src[is];};

       *pnt = nt;

    END_SAFE;

    return(dst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_THREADS_ELEMENTS - return an array of all elements for all threads */

void **SC_get_threads_elements(void *a, int *pnt, int *pne, int lck)
   {int is, id, ie, it, ne, nt, nex;
    void **dst, **src;

    dst = NULL;

    SC_register_thread_state();

    IF_SAFE(lck == TRUE, &SC_ts_lock, TRUE)

       ne  = st.ne;
       nt  = st.nt;
       nex = st.nex;
       src = st.data;

       dst = (void **) a;
       if (dst == NULL)
          dst = CMAKE_N(void *, nt*ne);

       for (it = 0; it < nt; it++)
           {for (ie = 0; ie < ne; ie++)
                {is      = it*nex + ie;
		 id      = it*ne + ie;
		 dst[id] = src[is];};};

       *pne = ne;
       *pnt = nt;

    END_SAFE;

    return(dst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_THREAD_STATE - set and return the Nth piece of thread state */

void *SC_set_thread_state(int id, int n, void *vl)
   {void *rv;

    SC_register_thread_state();

    rv = NULL;

    SC_LOCKON(SC_ts_lock);

    if ((0 <= n) && (n < st.ne))
       {rv = SC_get_thread_element(id, n);
        rv = vl;};

    SC_LOCKOFF(SC_ts_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_N_THREAD - return the number of known threads */

int SC_get_n_thread(void)
   {int rv;

    rv = st.nt;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_N_THREAD_ELEMENTS - return the number of known thread elements */

int SC_get_n_thread_elements(void)
   {int rv;

    rv = st.ne;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SINGLE_THREAD - run with a single thread
 *                  - for applications which are not parallel
 *                  - but have problems with threads, signals, longjmps, ...
 */

void SC_single_thread(void)
   {

#ifdef PTHREAD_OMP

# ifdef AIX

/* work around broken down AIX OpenMP implementation
 * -qsmp=noauto still attempts automatic parallelizations
 * must explicitly tell it to run with a single thread
 */
    setenv("OMP_NUM_THREADS", "1", 1);
/* you would expect this to do something - not on AIX
    omp_set_num_threads(1);
 */

# else

    omp_set_num_threads(1);

# endif

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRTHRST - diagnostic print of thread state */

void dprthrst(void)
   {int i, ie, it, ni;
    void *d;

    fprintf(stdout, "# entries per thread = %d\n", st.ne);

    for (ie = 0; ie < st.ne; ie++)
        {fprintf(stdout, "%4d  %-18s%-10s[%d]\n",
		 ie+1,
		 st.mreg[ie].type,
		 st.mreg[ie].name,
		 st.mreg[ie].ni);};

    fprintf(stdout, "# threads = %d\n", st.nt);

    ni = 0;
    for (it = 0; it < st.nt; it++)
        {fprintf(stdout, " %4d:\n", it);
         for (ie = 0; ie < st.ne; ie++)
             {i = it*st.nex + ie;
	      d = st.data[i]; 
	      if (d != NULL)
		 fprintf(stdout, "%4d  %-18s  %p\n",
			 ie+1,
			 st.mreg[ie].name,
			 d);
	      else
		 fprintf(stdout, "%4d  %-18s  null\n",
			 ie+1,
			 st.mreg[ie].name);

	      ni += (d == NULL);};};

    fprintf(stdout, "# elements unitialized = %d\n", ni);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRTHRNM - diagnostic print of thread id numbers */

void dprthrnm(void)
   {int i, it, nt, ide;
    int64_t idt;
    emu_thread_info *ti;

    nt = SC_get_n_thread();

    fprintf(stdout, "# threads = %d\n", nt);
    for (it = 0; it < nt; it++)
        {i  = it*st.nex + 1;
	 ti = (emu_thread_info *) st.data[i]; 
	 if (ti != NULL)
            {ide = ti->ide;
	     idt = (int64_t) ti->tid;
	     fprintf(stdout, " %4d: %4d -> %lld\n",
		     it, ide, (long long) idt);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
