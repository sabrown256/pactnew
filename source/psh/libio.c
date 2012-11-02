/*
 * LIBIO.C - special I/O routines
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBIO

#define LIBIO

#include "common.h"
#include "libpsh.c"

#ifndef IO_RING_DEFINED

typedef struct s_io_ring io_ring;

struct s_io_ring
   {unsigned int ib_in;
    unsigned int ob_in;
    unsigned int nb_ring;
    unsigned char *in_ring;};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RING_READY - return TRUE iff there is a:
 *            -   \n terminated string
 *            -   EOF
 *            - in the PROCESS ring buffer
 */

int ring_ready(io_ring *ring, unsigned char ls)
   {int ok;
    unsigned int nb, ob;
    unsigned char c;
    unsigned char *bf;

    ob = ring->ob_in;
    nb = ring->nb_ring;
    bf = ring->in_ring;

    for (ok = FALSE; (ok == FALSE) && ((c = bf[ob]) != '\0'); )
        {ob = (ob + 1) % nb;
         if ((c == ls) || (c == (unsigned char) EOF))
	    ok = TRUE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RING_PUSH - push NC bytes from S into the io_ring RING
 *           - everything must go into the buffer
 *           - the routine that extracts from the buffer will
 *           - decide whether ASCII or BINARY information is expected
 *           - return TRUE iff there is enough room to hold the bytes
 */

int ring_push(io_ring *ring, char *s, int nc)
   {int i;
    unsigned int ib, ob, ab, db, mb, nb, nnb;
    unsigned char *bf, *po, *pn;

    ib = ring->ib_in;
    ob = ring->ob_in;
    nb = ring->nb_ring;
    bf = ring->in_ring;    

/* compute available space remaining in the ring */
    while (TRUE)
       {if (ib >= ob)
	   ab = nb - ib + ob;
        else
	   ab = ob - ib;

	if (ab >= nc)
	   break;

/* find the size delta - try doubling */
	db = nb;

/* resize the ring */
	nnb = nb + db;
	REMAKE(bf, unsigned char, nnb);

/* adjust the ring if the "in use" section is discontiguous */
	if (ib < ob)
	   {mb = nb - ob;
	    pn = bf + nnb - 1;
	    po = bf + nb - 1;
	    for (i = 0; i < mb; i++)
	        *pn-- = *po--;

	    ob += db;};

	nb = nnb;

	ring->in_ring = bf;
	ring->nb_ring = nb;
	ring->ob_in   = ob;};

    if (s != NULL)
       {for (i = 0; i < nc; i++, ib = (ib + 1) % nb)
            bf[ib] = *s++;

        ring->ib_in = ib;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RING_POP - return the oldest message from the ring in
 *          - S which is NC bytes long
 *          - LS is the message delimiter character
 */

int ring_pop(io_ring *ring, char *s, int nc, unsigned int ls)
   {int i, ok;
    unsigned int ib, nb, ob;
    unsigned char c;
    unsigned char *bf;
    char *ps;

/* setup to copy from the ring to the buffer */
    ib = ring->ib_in;
    ob = ring->ob_in;
    nb = ring->nb_ring;
    bf = ring->in_ring;

/* copy until hitting a line separator or NC characters have been copied */
    for (i = 0, ps = s, ok = TRUE;
         (ok == TRUE) && (ob != ib) && (i < nc);
         i++, ob = (ob + 1) % nb)
        {c      = bf[ob];
	 bf[ob] = '\0';

	 *ps++ = c;

	 if (c == ls)
	    ok = FALSE;};

/* null terminate unless the character count maxed out */
    if (i < nc)
       *ps++ = '\0';

    ring->ob_in = ob;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
