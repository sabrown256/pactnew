/*
 * PDTGT.C - help for targeting platforms
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

typedef struct s_target target;

struct s_target
   {char *name;
    data_standard *std;
    data_alignment *algn;};

static target
 _PD_target_platforms[] = { {"dos",     &I386_STD, &WORD2_ALIGNMENT},
			    {"rs6000",  &PPC32_STD,  &GNU3_PPC64_ALIGNMENT},
			    {"mips",    &PPC32_STD,  &GNU3_PPC64_ALIGNMENT},
			    {"mips64",  &PPC64_STD,  &GNU4_X86_64_ALIGNMENT},
			    {"alpha64", &X86_64_STD,  &GNU4_X86_64_ALIGNMENT},
			    {"sun3",    &PPC32_STD,  &WORD2_ALIGNMENT},
			    {"sun4",    &PPC32_STD,  &SPARC_ALIGNMENT},
			    {"mac",     &M68X_STD,  &WORD2_ALIGNMENT},
			    {"vax",     &VAX_STD,    &WORD4_ALIGNMENT},
			    {"cray",    &CRAY_STD,   &WORD8_ALIGNMENT} };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET_PLATFORM - target the next PDBfile created to platform TGT
 *                    - return TRUE iff platform is known
 */

int PD_target_platform(char *tgt)
   {int i, n, rv;
    target *pt;

    rv = FALSE;

    if (tgt != NULL)
       {n = sizeof(_PD_target_platforms)/sizeof(target);
	for (i = 0; i < n; i++)
	    {pt = _PD_target_platforms + i;
	     if (strcmp(tgt, pt->name) == 0)
	        {PD_target(pt->std, pt->algn);
		 rv = TRUE;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET_PLATFORM_N - target the next PDBfile created to platform NP
 *                      - return TRUE iff platform is known
 */

int PD_target_platform_n(int np)
   {int n, rv;
    target *pt;

    rv = FALSE;

    n = sizeof(_PD_target_platforms)/sizeof(target);
    if ((0 <= np) && (np < n))
       {pt = _PD_target_platforms + n;
	PD_target(pt->std, pt->algn);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET - setup for the target machine data formats and alignments
 *           - this information is recorded in the PDBfiles to correctly
 *           - handle things when many files are open at once
 *           -
 *           - to correctly handle the situation in which there are
 *           - PD_OPEN'd files around (this may reset previously set
 *           - file->std), remember a standard specifically requested
 *           - with PD_TARGET
 *           - (note that PD_OPEN sets file->std and file->align)
 */

int PD_target(data_standard *data, data_alignment *align)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    _PD_init_state(FALSE);

    pa->req_std   = data;
    pa->req_align = align;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

/* RUN_TEST - run a particular test through all targeting modes */

static int run_test(PFTest test, int n, char *host)
   {int cs, fail;
    long bytaa, bytfa, bytab, bytfb;
    char s[MAXLINE], msg[MAXLINE];
    double time;
    static int dbg = 0;

/* NOTE: under the debugger set dbg to 1 or 2 for additional
 *       memory leak monitoring
 */
    cs = SC_mem_monitor(-1, dbg, "N", msg);

    SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    time = SC_wall_clock_time();

    fail = 0;

    if (!native_only)

/* Cray target test */
       {strcpy(s, "cray");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Sun4 target test */
        strcpy(s, "sun4");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Mips target test */
        strcpy(s, "mips");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Mips64 target test */
        strcpy(s, "mips64");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Alpha64 target test */
        strcpy(s, "alpha64");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Dos target test */
        strcpy(s, "dos");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Macintosh target test */
        strcpy(s, "mac");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Sun3 target test */
        strcpy(s, "sun3");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};

/* Vax target test */
        strcpy(s, "vax");
        if (!(*test)(host, s, n))
	   {PRINT(STDOUT, "Test #%d %s failed\n", n, s);
	    fail++;};};

/* native test */
    if (!(*test)(host, NULL, n))
       {PRINT(STDOUT, "Test #%d native failed\n", n);
	fail++;};

    SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

    bytaa -= bytab;
    bytfa -= bytfb;
    time   = SC_wall_clock_time() - time;

    cs = SC_mem_monitor(cs, dbg, "N", msg);

    PRINT(STDOUT,
          "\t\t     %3d    %8d  %8d   %7d     %.2g\n",
          n, bytaa, bytfa, bytaa - bytfa, time);

    return(fail);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
