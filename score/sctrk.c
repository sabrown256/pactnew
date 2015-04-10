/*
 * SCTRK.C - routines to help track PACT usage
 *
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SEND_TRACKER - simple (overly so) interface to tracker
 *                 - return TRUE iff successful
 */

int SC_send_tracker(char *code, char *version, int status, char *dst)
   {int rv;

    rv = FALSE;

#ifdef HAVE_TRACKER

    {SC_array *path;
     char *trk, *cmd, **pa;

     path = SC_make_search_path(1, "PATH");
     pa   = SC_array_done(path);
     trk  = SC_search_file(pa, "tracker");
     CFREE(pa);

     if (trk == NULL)
        trk = CSTRSAVE(TRACKER_EXE);

/* there is only one call because a time duration is not needed,
 * we leave it unfinished (wrt current tracker implementation)
 */
     if (trk != NULL)
        {if (dst == NULL)
	    {cmd = SC_dsnprintf(FALSE, "%s -b -n %s -v %s > /dev/null 2>&1",
				trk, code, version);}
         else
	    {cmd = SC_dsnprintf(FALSE, "%s -b -d %s -n %s -v %s > /dev/null 2>&1",
				trk, dst, code, version);};

	 rv  = system(cmd);
	 rv  = (rv == 0);};

     CFREE(trk);};

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
