## Assignment 3

### Question 1

### Question 2

A valid queue ID in Xinu is an integer less than the number of elements in ```queuetab[]``` and, generally, greater than 0. More specifically, a valid queue ID really should be greater than or equal to NPROC, to allow for sufficient entries in ```queuetab[]``` and to prevent accidentally accessing an arbitrary process instead of the head or tail of a particular queue.

### Question 3

The original ```resched``` function's .o file contains 49 instructions. My ```resched2.o``` contains 53.

The extra commands between the two come from the extra checks needed to ensure the next state is an eligible state, whether the state should remain PR_CURR or change to PR_READY if the process does remain eligible, and assigning the state of the current process within resched2 instead of outside the function (as with the original resched). ```resched2``` uses an extra register compared to ```resched``` for the argument ```nextstate``` that is now passed in instead of referenced from the current process' ```procent``` struct.
