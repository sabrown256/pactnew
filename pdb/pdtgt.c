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
 _PD_target_platforms[] = { {"sgnu",    &I586L_STD,   &GNU4_X86_64_ALIGNMENT},
			    {"cyg-1.5", &I586L_STD,   &CYGWIN_I686_ALIGNMENT},
			    {"osx5",    &I586O_STD,   &GNU3_PPC64_ALIGNMENT},
			    {"aix32",   &PPC32_STD,   &XLC32_PPC64_ALIGNMENT},
			    {"bgp",     &PPC32_STD,   &GNU4_PPC64_ALIGNMENT},
			    {"aix64",   &PPC64_STD,   &XLC64_PPC64_ALIGNMENT},
			    {"ognu",    &X86_64A_STD, &GNU4_X86_64_ALIGNMENT},
			    {"dos",     &I386_STD,    &WORD2_ALIGNMENT},
			    {"mac",     &M68X_STD,    &WORD2_ALIGNMENT},
			    {"sun3",    &PPC32_STD,   &WORD2_ALIGNMENT},
			    {"sun4",    &PPC32_STD,   &SPARC_ALIGNMENT},
			    {"vax",     &VAX_STD,     &WORD4_ALIGNMENT},
			    {"cray",    &CRAY_STD,    &WORD8_ALIGNMENT} };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET_N_PLATFORMS - Return the number of pre-defined target platforms.
 *                       - This is mainly support for the test suite.
 *
 * #bind PD_target_n_platforms fortran() scheme() python()
 */

int PD_target_n_platforms(void)
   {int n;

    n = sizeof(_PD_target_platforms)/sizeof(target);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET_PLATFORM - Target the next PDBfile created to platform TGT.
 *                    - Return TRUE iff platform is known.
 *                    - This is mainly support for the test suite.
 *
 * #bind PD_target_platform fortran() scheme() python()
 */

int PD_target_platform(const char *tgt)
   {int i, n, rv;
    target *pt;

    rv = FALSE;

    if (tgt != NULL)
       {n = PD_target_n_platforms();
	for (i = 0; i < n; i++)
	    {pt = _PD_target_platforms + i;
	     if (strcmp(tgt, pt->name) == 0)
	        {PD_target(pt->std, pt->algn);
		 rv = TRUE;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET_PLATFORM_N - Target the next PDBfile created to platform NP.
 *                      - Return TRUE iff platform is known.
 *                      - This is mainly support for the test suite.
 *
 * #bind PD_target_platform_n fortran() scheme() python()
 */

int PD_target_platform_n(int np)
   {int n, rv;
    target *pt;

    rv = FALSE;

    n = PD_target_n_platforms();
    if ((0 <= np) && (np < n))
       {pt = _PD_target_platforms + np;
	PD_target(pt->std, pt->algn);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET_PLATFORM_NAME - Return the name of the Nth target platform.
 *                         - This is mainly support for the test suite.
 *
 * #bind PD_target_platform_name fortran() scheme() python()
 */

char *PD_target_platform_name(int np)
   {int n;
    char *rv;
    target *pt;

    rv = NULL;

    n = PD_target_n_platforms();
    if ((0 <= np) && (np < n))
       {pt = _PD_target_platforms + np;
	rv = pt->name;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TARGET - Set the target machine data standard, DATA, and data
 *           - alignment, ALIGN, for the next PDBfile to be created.
 *           - This information is recorded in the PDBfiles to correctly
 *           - handle things when many files are open at once.
 *           - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_target fortran() scheme() python()
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

/* PD_TARGET_I - Set the target machine data standard and data
 *             - alignment for the next PDBfile to be created.
 *             - IS is an index into the builtin list of data_standards
 *             - and IA is an index into the builtin list of data_alignments.
 *             - See the PDBLib Users Manual for full details.
 *             - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_target_i fortran() scheme() python()
 */

int PD_target_i(PD_data_std_i is, PD_data_algn_i ia)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    _PD_init_state(FALSE);

    pa->req_std   = &PD_gs.standards[is];
    pa->req_align = &PD_gs.alignments[ia];

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
