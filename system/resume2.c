/* resume.c - resume */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  resume  -  Unsuspend a process, making it ready
 *------------------------------------------------------------------------
 */
pri16	resume2(
	  pid32		pid		/* ID of process to unsuspend	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	pri16	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		printf("%d not good pid\n", pid);
		restore(mask);
		return (pri16)SYSERR;
	}
	prptr = &proctab[pid];
	if (prptr->prstate != PR_SUSP) {
		//printf("%d not suspended\n", pid);
		restore(mask);
		return (pri16)SYSERR;
	}
	//prio = prptr->prprio;		/* Record priority to return	*/
	ready(pid);
	restore(mask);
	//sleep(5);
	prio = prptr->prprio;
	printf("prio of %d is %d\n", pid, prptr->prprio);
	return prptr->prprio;
}
