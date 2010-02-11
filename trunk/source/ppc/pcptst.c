/*
 * PCPRTS.C - comprehensive SPMD message passing test program
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc.h"

#define K     1024

#define BSIZE  20*K
#define LOOPS 10000

int
 *buf1,
 *buf2,
 filt[10],
 iter,
 nodes;

static void
 test_nb_read_patterned(PROCESS *pp),
 test_ring_b(PROCESS *pp),
 test_ring_nb(PROCESS *pp),
 test_type_sel(PROCESS *pp),
 test_broadcast(PROCESS *pp),
 test_rates(PROCESS *pp),
 test_error(PROCESS *pp),
 setup(void),
 clear_buf(int *b, int len),
 fill_buf(int *b, int len);

static int
 check_buf(int *b, int len);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start test suite
 *      - reserved args:
 *      -   -a #  The assigned node number of the parent node
 *      -   -n #  The number of nodes to use
 *      -   -l    Turn on logging of message traffic
 */

int main(int argc, char **argv)
   {int i, pid, which;
    PROCESS *pp;

    iter = 3;

    pp = PC_open_member(argv, &nodes);
    if (pp == NULL)
       return(1);

    SC_setbuf(stdout, NULL);

    pid = pp->acpu;
    if (pid == 0)
       PRINT(stdout, "\n");
    else
       sleep(1);

    if (_SC_debug)
       {fprintf(_PC_diag, "Initialized node #%d of %d\n", pid, nodes);
	fflush(_PC_diag);};
    PRINT(stdout, "Initialized node #%d of %d\n", pid, nodes);

    if (_SC_debug)
       {fprintf(_PC_diag, "Setup\n");
	fflush(_PC_diag);};
    setup();

    if (_SC_debug)
       {fprintf(_PC_diag, "Non-blocking patterned read\n");
	fflush(_PC_diag);};
    test_nb_read_patterned(pp);

    if (_SC_debug)
       {fprintf(_PC_diag, "Blocking ring\n");
	fflush(_PC_diag);};
    test_ring_b(pp);

    if (_SC_debug)
       {fprintf(_PC_diag, "Non-blocking ring\n");
	fflush(_PC_diag);};
    test_ring_nb(pp);

    if (_SC_debug)
       {fprintf(_PC_diag, "Type selection\n");
	fflush(_PC_diag);};
    test_type_sel(pp);

    if (_SC_debug)
       {fprintf(_PC_diag, "Broadcast\n");
	fflush(_PC_diag);};
    test_broadcast(pp);

    if (_SC_debug)
       {fprintf(_PC_diag, "Transfer rates\n");
	fflush(_PC_diag);};
    test_rates(pp);

    if (_SC_debug)
       {fprintf(_PC_diag, "Error handling test\n");
	fflush(_PC_diag);};
    test_error(pp);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_NB_READ_PATTERNED - test non-blocking reads */

static void test_nb_read_patterned(pp)
   PROCESS *pp;
   {int pid, count, n, nir;
    int *pbf;

    pid = pp->acpu;

    clear_buf(buf2, BSIZE);

    filt[0] = SC_MATCH_TAG;
    filt[1] = 1066;
    filt[2] = SC_MATCH_NODE;
    filt[3] = 1;
    filt[5] = PC_BLOCK_STATE;
    filt[6] = FALSE;

    if (pid == 1)
       {count = 1;
    
	filt[4] = 0;
        nir = 0;
        pbf = buf2;
        while (nir < BSIZE)
	   {n    = PC_in(pbf, SC_INTEGER_S, BSIZE-nir, pp, filt);
	    nir += n;
	    pbf += n;
	    sleep(1);
	    count++;};
	PRINT(stdout,
	      "   Read %d patterned integers in %d attempts to node %d\n",
	      nir, count, pid);

	if (check_buf(buf2, BSIZE) > 0)
	   PRINT(stdout, "   Data incorrect\n");
	else
	   PRINT(stdout, "   Data confirmed\n");}
  
    else if (pid == 0)
       {PRINT(stdout, "Test non-blocking read with patterned data\n");

	sleep(2);
	fill_buf(buf1, BSIZE);
	filt[4] = 1;
        PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);

        PRINT(stdout, "Test done\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_RING_B - run messages around a ring */

static void test_ring_b(pp)
   PROCESS *pp;
   {int i, pid, next, prev;
    double tstart, tend;

    pid = pp->acpu;
    if (pid == 0)
       PRINT(stdout, "Test blocking sends around intertwined ring\n");
  
    next = (pid + 1) % nodes;
    prev = (pid + nodes - 1) % nodes;

    tstart = SC_wall_clock_time();
  
    filt[0] = SC_MATCH_TAG;
    filt[1] = 99;
    filt[2] = SC_MATCH_NODE;
    filt[3] = 1;
    filt[5] = PC_BLOCK_STATE;
    filt[6] = TRUE;
    for (i = 0; i < iter; i++)
        {fill_buf(buf1, BSIZE);
	 clear_buf(buf2, BSIZE);

/* odd nodes go one way */
	 if (pid & 1)
	    {filt[4] = next;
	     PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);
	     filt[4] = prev;
	     PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);}

/* even nodes go the other */
	 else
	    {filt[4] = prev;
	     PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);
	     filt[4] = next;
	     PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);};

	 check_buf(buf2, BSIZE);};
    PRINT(stdout, "   %d iterations made\n", i);

    tend = SC_wall_clock_time();

    if (pid == 0)
       PRINT(stdout, "Test done (%6.2f secs)\n", tend - tstart);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_RING_NB - send message around the ring */

static void test_ring_nb(pp)
   PROCESS *pp;
   {int pid, rlen, next, prev;

    pid = pp->acpu;
    if (pid == 0)
       PRINT(stdout, "Talk around a ring blocking non-blocking sends\n");

    next = (pid + 1) % nodes;
    prev = (pid - 1) % nodes;

    fill_buf(buf1, BSIZE);
    clear_buf(buf2, BSIZE);

    filt[0] = SC_MATCH_TAG;
    filt[1] = 99;
    filt[2] = SC_MATCH_NODE;
    filt[3] = 1;
    filt[5] = PC_BLOCK_STATE;
    filt[6] = FALSE;

    filt[4] = next;
    PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);
    filt[4] = prev;
    PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);

    check_buf(buf2, BSIZE);

    PRINT(stdout, "Check mismatched messge sizes\n");
    if (pid == 0)
       {filt[4] = next;
	PC_out(buf1, SC_INTEGER_S, BSIZE/2, pp, filt);};

    if (pid == 1)
       {filt[4] = prev;
	rlen = PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);
	if (rlen == BSIZE/2)
	   PRINT(stdout, "Got right size OK\n");};
    
    if (pid == 0)
       PRINT(stdout, "Test done\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_TYPE_SEL - test message type selection */

static void test_type_sel(pp)
   PROCESS *pp;
   {int pid, next, prev;

    pid = pp->acpu;
    if (pid == 0)
       PRINT(stdout, "Check message type selection needs 3 node 0, 1, 2\n");

    next = (pid + 1) % nodes;
    prev = (pid - 1) % nodes;

    filt[0] = SC_MATCH_TAG;
    filt[2] = SC_MATCH_NODE;
    filt[3] = 1;
    filt[5] = PC_BLOCK_STATE;
    filt[6] = TRUE;

    if (pid == 0)
       {filt[4] = next;
	filt[1] = 66;
	PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);
	filt[1] = 99;
	PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);};

    if (pid == 2)
       {filt[4] = prev;
	filt[1] = 66;
	PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);
	filt[1] = 99;
	PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);};

    if (pid == 1)
       {filt[4] = prev;
	filt[1] = 99;
	PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);
	PRINT(stdout,
	      "   Got message from %d tag %d length %d\n",
	      prev, 99, BSIZE);
	check_buf(buf2, BSIZE);

        filt[4] = next;
	filt[1] = 99;
	PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);
	PRINT(stdout,
	      "   Got message from %d tag %d length %d\n",
	      next, 99, BSIZE);
	check_buf(buf2, BSIZE);

        filt[4] = prev;
	filt[1] = 66;
	PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);
	PRINT(stdout,
	      "   Got message from %d tag %d length %d\n",
	      prev, 66, BSIZE);
	check_buf(buf2, BSIZE);

        filt[4] = next;
	filt[1] = -1;
	PC_in(buf2, SC_INTEGER_S, BSIZE, pp, filt);
	PRINT(stdout,
	      "   Got message from %d tag %d length %d\n",
	      next, filt[1], BSIZE);
	check_buf(buf2, BSIZE);};
  
    if (pid == 0)
       PRINT(stdout, "Test done\n");

    return;}

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* TEST_BROADCAST - broadcast a message */

static void test_broadcast(pp)
   PROCESS *pp;
   {int i, pid;

    pid = pp->acpu;
    if (pid == 0)
       PRINT(stdout, "\nTest broadcast\n");
  
    filt[0] = SC_MATCH_TAG;
    filt[1] = 99;
    filt[2] = SC_MATCH_NODE;
    filt[3] = -1;
    filt[5] = PC_BLOCK_STATE;
    filt[6] = TRUE;

    for (i = 0; i < nodes; i++)
        {fill_buf(buf1, BSIZE);

         PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);

	 PRINT(stdout, "   Node %d got message from %d\n", pid, i);
	 check_buf(buf1, BSIZE); 
    
	 sleep(2);};
  
    if (pid == 0)
       PRINT(stdout, "Test done\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_RATES - test data transfer rates */

static void test_rates(pp)
   PROCESS *pp;
   {int i, j, pid, rlen, reps, nbytes;
    double tstart, tend, tpm, rate, secs;
    int bytes[] = {1, 16, 64, K, 10*K, BSIZE};

    nbytes = sizeof(bytes)/sizeof(int);

    pid = pp->acpu;
    if (pid == 0)
       PRINT(stdout, "\nTest Data rate\n");

    filt[0] = SC_MATCH_TAG;
    filt[1] = 0;
    filt[2] = SC_MATCH_NODE;
    filt[3] = 1;
    filt[5] = PC_BLOCK_STATE;
    filt[6] = TRUE;

    if (pid == 0)
       {for (i = 0 ; i < nbytes; i++)
	    {if (bytes[i] > K)
	        reps = LOOPS*1000/bytes[i];
	     else
	        reps = LOOPS;

	     tstart = SC_wall_clock_time();
             filt[4] = 1;
	     for (j = 0 ; j < reps; j++)
                 PC_in(buf1, "int", bytes[i] >> 2, pp, filt);

	     tend = SC_wall_clock_time();
	     secs = tend - tstart;
     
	     tpm  = (secs * 1000000.0)/ reps;
	     rate = (bytes[i] * reps)/(secs * K);
      
	     if (rate > 1000.0)
	        {rate /= 1000.0;
		 PRINT(stdout, "Size %4d bytes time %6.2f secs %6.2f uS / message, %6.2f Mbps\n",  bytes[i], tend-tstart, tpm, rate);}
	     else
	        PRINT(stdout, "Size %4d bytes time %6.2f secs %6.2f uS / message, %6.2f Kbps\n",  bytes[i], tend-tstart, tpm, rate);             
	     sleep(1);};};
  
    if (pid == 1)
       {for (i = 0 ; i < nbytes; i++)
	    {if (bytes[i] > K)
	        reps = LOOPS*1000/bytes[i];
	     else
	        reps = LOOPS;
     
             filt[4] = 0;
	     rlen = bytes[i];
	     for (j = 0 ; j < reps; j++)
	         rlen = PC_out(buf2, "char", rlen, pp, filt);};};

    if (pid == 0)
       PRINT(stdout, "Test done\n");

    return;}

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* TEST_ERROR - test error handling */

static void test_error(pp)
   PROCESS *pp;
   {int pid;

    pid = pp->acpu;
    if (pid == 0)
       {PRINT(stdout, "\nTest Bad dest exception\n");
	filt[0] = SC_MATCH_TAG;
	filt[1] = 0;
	filt[2] = SC_MATCH_NODE;
	filt[3] = 1;
	filt[4] = -999;
	filt[5] = 0;

        PC_out(buf1, SC_INTEGER_S, BSIZE, pp, filt);};

    return;}

/*--------------------------------------------------------------------------*/

/*                              HELPER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* SETUP - setup various global data */

static void setup()
   {int i;

    for (i = 0; i < 10; i++)
        filt[i] = 0;

/* check buffer alignment */
    buf1 = FMAKE_N(int, BSIZE + 8, "SETUP:buf1");
    buf2 = FMAKE_N(int, BSIZE + 8, "SETUP:buf2");

    if (buf1 == NULL)
       PRINT(stdout, "FAILED TO ALLOCATE buf1\n");
  
    if (buf2 == NULL)
       PRINT(stdout, "FAILED TO ALLOCATE buf2\n");

/*
    buf1 = (int *) ((int) buf1 + (32 - ((int) buf1 % 32)));
    buf2 = (int *) ((int) buf2 + (32 - ((int) buf2 % 32)));
  
    if (((int) buf1 % 32) != 0)
       PRINT(stdout, "Buffer 1 not 32 byte aligned %d \n",(int)buf1 % 32);

    if (((int) buf2 % 32) != 0)
       PRINT(stdout, "Buffer 2 not 32 byte aligned %d\n",(int)buf2 % 32);

    ew_touchBuf(buf1, sizeof(buf1));
    ew_touchBuf(buf2, sizeof(buf2));
*/

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAR_BUF - fill the buffer with -1's */

static void clear_buf(b, len)
   int *b, len;
   {int i, pat;
    int *p;
  
    for (i = 0; i < len; *b++ = -1, i++);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILL_BUF - fill the buffer with increasing integers */

static void fill_buf(b, len)
   int *b, len;
   {int i, pat;
    int *p;
  
    for (i = 0; i < len; *b++ = i++);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_BUF - verify that the buffer contains the specified
 *           - integer pattern
 */

static int check_buf(b, len)
   int *b, len;
   {int i, bdata;
    int *p;
    int errors;

    p = b;
    errors = 0;
    for (i = 0; i < len; i++)
        {bdata = *p++;
	 if (bdata != i)
	    {PRINT(stdout,
		   "ERROR: data(%d) = %d (not %d) - CHECK_BUF\n",
		    i, bdata, i);
	     errors++;
	     if (errors > 5)
	        return(errors);};};

    return(errors);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

