/*
 * SCRSCB.C - routines involving resource usage
 *          - highly conditionalized
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#ifdef HAVE_RESOURCE_USAGE
# include <sys/resource.h>
#endif

#if defined(LINUX) || defined(CYGWIN)
# include <dirent.h>
# include <libgen.h>
#elif defined(AIX)
# include <dirent.h>
#elif defined(SOLARIS)
# include <dirent.h>
#elif defined(MACOSX)
# include <sys/sysctl.h>
# include <mach/task.h>
# include <mach/mach_init.h>
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SWAP - swap elements i and j in arrays v and ind */

void _SC_swap(int *ind, int i, int j)
   {

    SC_SWAP_VALUE(int, ind[i], ind[j]);

    return;}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_I_SORT - quicksort, taken from "The C Programming Language",
 *            - 2nd Edition, by Kernighan and Ritchie, pp 87-88
 */

static void _SC_i_sort(int *ind, int left, int right)
   {int i, last;

    if (left >= right)
       return;

    _SC_swap(ind, left, (left+right)/2);

    last = left;

    for (i = left+1; i <= right; i++)
        if (ind[i] < ind[left])
           _SC_swap(ind, ++last, i);

    _SC_swap(ind, left, last);
    _SC_i_sort(ind, left, last-1);
    _SC_i_sort(ind, last+1, right);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SC_I_SORT - Sort an array of indexes on z; 
 *           - input   ind, array of indexes
 *           -           n, number of entries
 *           - output  ind, sorted
 */

void SC_i_sort(int *ind, int n)
   {

    _SC_i_sort(ind, 0, n-1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROCESS_IDS - return an array of all current process ids
 *                - the last entry is -1
 */

int *SC_process_ids(void)
   {int nid;
    int *ids;

    nid = 0;
    ids = NULL;

#if defined(LINUX) || defined(CYGWIN) || defined(SOLARIS) || defined(AIX)

       {DIR *dir;
	struct dirent *dp;

	dir = opendir("/proc");
        if (dir != NULL)

/* count the number of entries */
	   {nid = 0;
	    while (TRUE)
	       {dp = (struct dirent *) readdir(dir);
		if (dp == NULL)
		   break;
		else if (SC_intstrp(dp->d_name, 10) == TRUE)
		   nid++;};

	    rewinddir(dir);
	    ids = CMAKE_N(int, nid+1);

/* add the entries to ids */
	    nid = 0;
	    while (TRUE)
	       {dp = (struct dirent *) readdir(dir);
		if (dp == NULL)
		   break;
		else if (SC_intstrp(dp->d_name, 10) == TRUE)
		   ids[nid++] = SC_stoi(dp->d_name);};

	    closedir(dir);};};

#elif defined(MACOSX)

       {int i;
	size_t ln;
	int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
	struct kinfo_proc *kp;

	if (sysctl(mib, 4, NULL, &ln, NULL, 0) == 0)
	   {kp = CMAKE_N(struct kinfo_proc, ln/sizeof(struct kinfo_proc));
	    if (sysctl(mib, 4, kp, &ln, NULL, 0) == 0)
	       {nid = ln/sizeof(struct kinfo_proc);
		ids = CMAKE_N(int, nid+1);
	        for (i = 0; i < nid; i++)
		    ids[i] = kp[i].kp_proc.p_pid;};};};

#else

       {int i, ns, st;
	char *cmd, **res;

	cmd = SC_dsnprintf(TRUE, "ps -eo pid");
	st  = SC_exec(&res, cmd, NULL, -1);
	if (st == 0)
	   {ns  = SC_MEM_GET_N(int, res);
	    ids = CMAKE_N(int, ns);

	    nid = 0;
	    for (i = 0; res[i] != NULL; i++)
	        {SC_LAST_CHAR(res[i]) = '\0';
		 if (SC_intstrp(res[i], 10) == TRUE)
		    ids[nid++] = SC_stoi(res[i]);};};
	CFREE(cmd);

	SC_free_strings(res);};

#endif

    SC_i_sort(ids, nid);

    if (ids != NULL)
       ids[nid++] = -1;

    return(ids);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
