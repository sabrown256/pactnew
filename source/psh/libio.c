/*
 * LIBIO.C - special I/O routines
 *
 * #include "cpyright.h"
 *
 */

#ifndef LIBIO

# define LIBIO

# include "common.h"
# include "libpsh.c"

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

typedef struct s_io_ring io_ring;

struct s_io_ring
   {unsigned int ib_in;                /* input pointer */
    unsigned int ob_in;                /* output pointer */
    unsigned int nb_ring;              /* ring buffer size */
    unsigned char *in_ring;};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

/*--------------------------------------------------------------------------*/

/* RING_INIT - initialize the io_ring RING with an NB bytes long buffer */

int ring_init(io_ring *ring, unsigned int nb)
   {unsigned int n;

    if (ring != NULL)

/* NOTE: make actual buffer size incommensurate with common array sizes
 * which are often passed in as the buffer size
 * this avoids problems in which the buffer copied in or out is
 * the same size so that ib_in and ob_in have the same value
 * indicating and empty buffer when there would in fact be a "full" buffer
 */
       {n = 1.3*nb;

	ring->nb_ring = n;
	ring->in_ring = MAKE_N(unsigned char, n);
	ring->ib_in   = 0;
	ring->ob_in   = 0;

	memset(ring->in_ring, 0, n);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RING_CLEAR - clear out the io_ring RING */

int ring_clear(io_ring *ring)
   {

    if (ring != NULL)
       {FREE(ring->in_ring);
	ring->nb_ring = 0;
	ring->ib_in   = 0;
	ring->ob_in   = 0;};

    return(TRUE);}

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

/* _RING_ADJUST - adjust the RING to be ready to
 *              - receive NC bytes
 */

static void _ring_adjust(io_ring *ring, int nc)
   {int i;
    unsigned int ib, ob, ab, db, mb, nb, nnb;
    unsigned char *bf, *po, *pn;

    ib = ring->ib_in;
    ob = ring->ob_in;
    nb = ring->nb_ring;
    bf = ring->in_ring;    

    while (TRUE)

/* compute available space remaining in the ring */
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

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RING_PUSH - push NC bytes from S into the io_ring RING
 *           - everything must go into the buffer
 *           - the routine that extracts from the buffer will
 *           - decide whether ASCII or BINARY information is expected
 *           - return TRUE iff there is enough room to hold the bytes
 */

int ring_push(io_ring *ring, char *s, int nc)
   {int i, rv;
    unsigned int ib, nb;
    unsigned char *bf;

    rv = TRUE;
    if ((s != NULL) && (nc > 0))
       {_ring_adjust(ring, nc);

	ib = ring->ib_in;
	nb = ring->nb_ring;
	bf = ring->in_ring;    

/* insert the text in the ring */
	for (i = 0; i < nc; i++, ib = (ib + 1) % nb)
            bf[ib] = *s++;

        ring->ib_in = ib;};

    return(rv);}

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

# endif
#endif
