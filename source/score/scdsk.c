/*
 * SCDSK.C - compute and return the amount of disk space
 *         - available including quota restrictions
 *         - in the current directory
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DSK_SPACE - determine the disk space available for a 
 *              - file in directory DIR
 */

int64_t SC_dsk_space(char *dir, int64_t nbx)
   {int fd, nw, st, ok;
    int64_t bsz, nb, no;
    char *t, *cwd;

/* remember where we are */
    cwd = SC_getcwd();

/* go to the directory we are interested in */
    chdir(cwd);

/* get a temporary file */
    t  = SC_dsnprintf(FALSE, "__tmp_XXXXXX");
    fd = mkstemp(t);

    bsz = 4096;
    for (nb = bsz, ok = TRUE; (nb <= nbx) && ok; nb += bsz)
        {ok = FALSE;
	 no = lseek(fd, bsz, SEEK_CUR);
	 if (no < 0)
	    io_error(errno, "lseek to %s on %d failed",
		     SC_itos(NULL, 0, bsz, NULL), fd);

	 if (no >= nb)
	    {nw = write(fd, " ", 1);
	     if (nw < 0)
	        io_error(err, "write of 1 byte on %d failed", fd);
	     if (nw == 1)
	        ok = TRUE;};};

/* close the temporary file and remove it */
    st = close(fd);
    st = unlink(t);

/* go back to the directory we came from */
    st = chdir(cwd);

    CFREE(cwd);

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BYTES_STRING - given a number of bytes
 *                 - return a string with an ASCII representation
 *                 - in units of TBy, GBy, MBy, or KBy
 */

int SC_bytes_string(double nb, int nc, char *s, char *fmt)
   {int i;
    char *f;
    static double by[6];
    static char *byn[6] = {"By", "KBy", "MBy", "GBy", "TBy", "PBy"};

    ONCE_SAFE(TRUE, NULL)
       for (i = 0; i <= 5; i++)
	   by[i] = pow(1024.0, i);
    END_SAFE;

    for (i = 5; 0 <= i; i--)
        {if (nb > by[i])
	    {nb /= by[i];
	     f   = SC_dsnprintf(FALSE, "%s %s", fmt, byn[i]);
	     snprintf(s, nc, f, nb);
	     break;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i;
    int64_t nb;
    double fb, dt, rc;
    char s[MAXLINE];

/* default to 1G */
    nb = pow(2.0, 32.0);
    rc = 1.0/pow(2.0, 20.0);

    for (i = 1; i < c; i++)
        fb = SC_stof(v[i]);

    nb = fb;

    dt = SC_wall_clock_time();
    nb = SC_dsk_space(".", nb);
    dt = SC_wall_clock_time() - dt;
    fb = nb;
    SC_bytes_string(nb, MAXLINE, s, "%8.3f");

    printf("Wrote %s in %.2f seconds (%.2f MBy/sec)\n", s, dt, rc*fb/dt);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
