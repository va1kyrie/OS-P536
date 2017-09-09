/* process_ring.h - header file for process_ring
 * shell command.
 */

//#include "../apps/process_ring.c"

//function defs
process process_ring_poll(volatile int*, int, int, int, int);
process process_ring_sync(sid32*, int, int, volatile int*, int, sid32);

//VARIABLE DEFINITIONS
#define MAXP 64     // max number of processes
#define MINP 1      // min number of processes
#define DEFAULTP 2  // default process number
#define MAXR 100    // max value of r
#define MINR 1      // min value of r
#define DEFAULTR 3  // default roll number
#define POLL 80     // indicate poll mode
#define SYNC 83     // indiacate sync mode
