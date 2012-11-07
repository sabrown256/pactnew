/*
 * SCMEMH.C - memory usage history functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h"
#include "scope_mem.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_HST_INIT - initialize the history ring in heap PH */

static void _SC_mem_hst_init(SC_heap_des *ph)
   {SC_mem_hst *rng;

    rng = (SC_mem_hst *) calloc(_SC.mem_hst_sz, sizeof(SC_mem_hst));

    _SC_mem_exhausted(rng == NULL);

    ph->ring = rng;
    ph->ih   = 0L;
    ph->nh   = _SC.mem_hst_sz;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_HISTORY - activate/deactivate memory history tracking */

static void _SC_mem_history(int act, void *a)
   {long ih, nh;
    double t[2], dt;
    SC_heap_des *ph;
    SC_mem_hst *hst;
    mem_descriptor *desc;

    desc = (mem_descriptor *) a;

    ph = desc->heap;
    if (ph->ring == NULL)
       _SC_mem_hst_init(ph);

/* get time since the epoch */
    SC_get_time(&t[0], &t[1]);
    if (_SC.t0[0] == -1.0)
       {_SC.t0[0] = t[0];
	_SC.t0[1] = t[1];};

    dt = (t[0] - _SC.t0[0]) + 1.0e-6*(t[1] - _SC.t0[1]);

    ih = ph->ih;
    nh = ph->nh;

    hst = &ph->ring[ih++];

    hst->ncall++;
    hst->action = act;
    hst->time   = dt;
    hst->space  = desc;
    hst->nb     = desc->length;
    if (act == SC_MEM_ALLOC)
       hst->name = (char *) desc->desc.info.pfunc;
    else
       hst->name = NULL;

    ph->ih = ih % nh;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_HISTORY - set number memory history tracking entries to SZ
 *                - SZ = 0 turns it off
 *                - SZ < 0 only returns the current value
 */

long SC_mem_history(long sz)
   {long osz;

    osz = _SC.mem_hst_sz;

    if (sz > 0L)
       _SC.mem_hst = _SC_mem_history;
    else
       _SC.mem_hst = NULL;

    if (sz >= 0L)
       _SC.mem_hst_sz = sz;

    return(osz);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_HST_DUMP_ENT - print a memory history entry HST */

static void _SC_mem_hst_dump_ent(FILE *fp, SC_mem_hst *hst)
   {int it, hr, mn;
    double t, sc;
    char *act, *nm;
    void *p;

    switch (hst->action)
       {case SC_MEM_ALLOC:
             act = "alloc";
             break;
        case SC_MEM_REALLOC:
             act = "realloc";
             break;
        case SC_MEM_FREE:
             act = "free";
             break;
	   default:
             act = NULL;
             break;};

    if (act != NULL)
       {t = hst->time;
	p = ((mem_header *) hst->space) + 1;

	it = t;
	hr = it / 3600;
	mn = (it - 3600*hr) / 60;
	sc = t - 3600.0*hr - 60.0*mn;

	if (hr > 0)
	   fprintf(fp, "   %d:%02d:%10.8f   ", hr, mn, sc);
	else if (mn > 0)
	   fprintf(fp, "   %d:%10.8f   ", mn, sc);
	else
	   fprintf(fp, "   %10.8f   ", sc);

	fprintf(fp, "%-8s %10p  %10ld  ", act, p, hst->nb);

	nm = hst->name;
	if (nm != NULL)
	   fprintf(fp, "%s", nm);

	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_HST_DUMP - print out the memory history of heaps */

static void _SC_mem_hst_dump(FILE *fp, SC_heap_des *ph)
   {int i;
    long ih, nh;
    SC_mem_hst *rng;

    rng = ph->ring;
    ih  = ph->ih;
    nh  = ph->nh;

    fprintf(fp, "History for thread #%d:\n", ph->tid);

    for (i = ih; i < nh; i++)
        _SC_mem_hst_dump_ent(fp, rng+i);

    for (i = 0; i < ih; i++)
        _SC_mem_hst_dump_ent(fp, rng+i);

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_HISTORY_OUT - print out the memory history of heaps */

void SC_mem_history_out(FILE *fp, int tid)
   {int i, nt;
    SC_heap_des *ph;

    if (_SC.mem_hst_sz > 0)
       {if (fp == NULL)
	   fp = STDOUT;

	nt = SC_get_n_thread();
	nt = max(nt, 1);

/* disable to avoid temporaries in print process corrupting the results */
	_SC.mem_hst = NULL;

	if (tid < 0)
	   {for (i = 0; i < nt; i++)
	        {ph = _SC_get_heap(i);
		 _SC_mem_hst_dump(fp, ph);};}
	else
	   {tid = min(tid, nt);
	    ph  = _SC_get_heap(tid);
	    _SC_mem_hst_dump(fp, ph);};

/* re-enable memory history */
	_SC.mem_hst = _SC_mem_history;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
