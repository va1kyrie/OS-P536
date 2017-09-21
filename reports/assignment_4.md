## Assignment 4

### Question 1
Storing the processor state in the process table instead of on the process stack is an approach akin to Linux's (and other OS's) PCB structure - instead of saving process information inside that process' stack, the information is saved within the structure representing the process in the process table.

With this implementation, the procent struct would look something more like this:

```C
struct procent {		/* Entry in the process table		*/
  uint16	prstate;	/* Process state: PR_CURR, etc.		*/
  pri16	prprio;		/* Process priority			*/
  char	*prstkptr;	/* Saved stack pointer			*/
  char	*prstkbase;	/* Base of run time stack		*/
  uint32	prstklen;	/* Stack length in bytes		*/
  char	prname[PNMLEN];	/* Process name				*/
  sid32	prsem;		/* Semaphore on which process waits	*/
  pid32	prparent;	/* ID of the creating process		*/
  umsg32	prmsg;		/* Message sent to this process		*/
  bool8	prhasmsg;	/* Nonzero iff msg is valid		*/
  int16	prdesc[NDESC];	/* Device descriptors for process	*/
};
```

### Question 2


### Question 3
