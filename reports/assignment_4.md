## Assignment 4

### Question 1
Storing the processor state in the process table instead of on the process stack is an approach akin to Linux's (and other OS's) process control block structure - instead of saving process information inside that process' stack, the information is saved within the structure representing the process in the process table.

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
  uint32 stacktab[14];
};
```
where stacktab contains enough elements to store each process and hardware register, the coprocess status, and the stack pointer stack pointer.

When ```ctsw()``` is called in this implementation, instead of pushing and popping values to and from process stacks, it will simply put them into and get them out of the indices of ```stacktab[]```:

```C
for (i registers):
  stacktab[i] = register[i];
end for;
```
```C
for(i registers):
  register[i] = stacktab[i];
end for;
```

In practice, those for loops would likely be replaced by a series of statements for each index of ```stacktab[]```:

```C
stacktab[0] = r0;
stacktab[1] = r1;
stacktab[2] = r2;
stacktab[3] = r3;
...
stacktab[12] = lr;
stacktab[13] = cpsr;
stacktab[14] = sp;
```

The advantage of this implementation is clear from the point of view of a debugging developer: the array is a much cleaner conceptual implementation in many ways as compared to the process stack. Putting elements back into their respective registers when resuming a process, while not necessarily _actually_ simpler than when using a stack, may be conceptually easier to understand. From the point of view of the operating system, moving this information off of the stack and into memory puts the information in a less potentially volatile location/more protected location than the process stack.

On the other hand, this requires enough extra memory allocated per procent to hold all the register values and process information, instead of that information being stored on the already-allocated process stack. In this aspect, storing the process information on the process' stack has a distinct advantage.

### Question 2

Prompt: create new process state, PR_DYING, and deallocate the stack after kill is called on the process by the process.
  - plan: define PR_DYING (done)
  - find "whatever process searches the process table  for dying processes, free the stack, and move the entry to PR_FREE.

  - idea: simple in kill.c (done already). what file searches proctable for dying processes?

### Question 3
