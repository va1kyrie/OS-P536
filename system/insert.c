/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status	insert(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Key for the inserted process	*/
	)
{
	int16	pcurr;			/* Runs through items in a queue*/
	struct qentry*	prev;
	struct qentry*	curr;			/* Holds previous node index	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	pcurr = firstid(q);
	while (queuetab[pcurr].qkey >= key) {
		pcurr = (queuetab[pcurr].qnext)->pid;
	}
	curr = &(queuetab[pcurr]);

	/* Insert process between curr node and previous node */

	prev = queuetab[pcurr].qprev;	/* Get index of previous node	*/
	queuetab[pid].qnext = curr;
	queuetab[pid].qprev = prev;
	queuetab[pid].qkey = key;
	queuetab[prev->pid].qnext = &queuetab[pid];
	queuetab[curr->pid].qprev = &queuetab[pid];
	return OK;
}
