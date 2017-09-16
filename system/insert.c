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
	pid32	cpid;			/* Runs through items in a queue*/
	struct qentry *prev;
	struct qentry *curr;			/* Holds previous node index	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	cpid = firstid(q);
	while (queuetab[cpid].qkey >= key) {
		cpid = (queuetab[cpid].qnext)->pid;
	}

	//now cpid's key is less than the key to be inserted
	curr = &(queuetab[cpid]);

	/* Insert process between curr node and previous node */

	prev = queuetab[cpid].qprev;	/* Get previous node	*/
	queuetab[pid].qnext = curr;
	queuetab[pid].qprev = prev;
	queuetab[pid].qkey = key;
	queuetab[prev->pid].qnext = &queuetab[pid];
	queuetab[curr->pid].qprev = &queuetab[pid];
	//again, need to explicitly declare pid now because pointers
	queuetab[pid].pid = pid;
	return OK;
}
