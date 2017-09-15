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
	pid32	head;

	if (isempty(q)) {
		return EMPTY;
	}

	head = queuehead(q);
	return getitem(head->qnext);
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

	if (isempty(q)) {
		return EMPTY;
	}

	tail = queuetail(q);
	return getitem(tail->qprev);
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
		qid16		qid
	)
{
	quentry*	prev, next;

	int i;
	qentry* curr = queuetab[qid].qnext;

	while (curr->pid != pid && curr->pid != EMPTY) {
		curr = queuetab[curr].qnext;
	}

	if(curr->pid == EMPTY){
		return SYSERR;
	}

	next = curr->qnext;	/* Following node in list	*/
	prev = curr->qprev;	/* Previous node in list	*/
	curr->qnext = next;
	curr->qprev = prev;
	return curr->pid;
}
