/* process_ring.h - header file for process_ring
 * shell command.
 */

//FUNCTION DEFINITIONS
shellcmd xsh_process_ring(int, char*);
process process_ring();

//VARIABLE DEFINITIONS
#define MAXP 64
#define MINP 0
#define DEFAULTP 2
#define MAXR MAXINT
#define DEFAULTR 3
#define POLL "poll"
#define SYNC "sync"
