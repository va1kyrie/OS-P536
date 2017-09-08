/* process_ring.h - header file for process_ring
 * shell command.
 */

#include "../apps/process_ring.c"

//FUNCTION DEFINITIONS
shellcmd xsh_process_ring(int, char*);
// process process_ring_poll();
// process process_ring_sync();
int parse_args(int, char[]);


//VARIABLE DEFINITIONS
#define MAXP 64     // max number of processes
#define MINP 1      // min number of processes
#define DEFAULTP 2  // default process number
#define MAXR 100    // max value of r
#define MINR 1      // min value of r
#define DEFAULTR 3  // default roll number
#define POLL 80     // indicate poll mode
#define SYNC 83     // indiacate sync mode
