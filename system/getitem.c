/* getitem.c - getfirst, getlast, getitem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getfirst  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	intmask mask;
	pid32 first;

	//mask = disable();
	if(isbadqid(q)){
		//restore(mask);
		return (qid16)SYSERR;
	}

	pid32	head;

	if (isempty(q)) {
		return EMPTY;
	}

	head = queuehead(q);
	first = getitem(queuetab[head].qnext);
	//restore(mask);
	return first;
}

/*------------------------------------------------------------------------
 *  getlast  -  Remove a process from end of queue
 *------------------------------------------------------------------------
 */
pid32	getlast(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32 tail;
	pid32 last;
	//intmask mask;

	//mask = disable();
	if(isbadqid(q)){
		//restore(mask);
		return (qid16)SYSERR;
	}

	if (isempty(q)) {
		return EMPTY;
	}

	tail = queuetail(q);
	last = getitem(queuetab[tail].qprev);
	//restore(mask);
	return last;
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
	)
{
	pid32	prev, next;

	if(isbadpid(pid)){
		return SYSERR;
	}

	next = queuetab[pid].qnext;	/* Following node in list	*/
	prev = queuetab[pid].qprev;	/* Previous node in list	*/
	queuetab[prev].qnext = next;
	queuetab[next].qprev = prev;
	return pid;
}
