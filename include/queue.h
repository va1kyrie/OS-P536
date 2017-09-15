/* queue.h - firstid, firstkey, isempty, lastkey, nonempty		*/

/* Queue structure declarations, constants, and inline functions	*/

/* Default # of queue entries: 2 for ready list plus	*/
/*			2 for sleep list plus 2 per semaphore		*/
#ifndef NQENT
#define NQENT	(NPROC + 4 + NSEM + NSEM)
#endif

#define	EMPTY	(-1)		/* Null value for qnext or qprev index	*/
#define	MAXKEY	0x7FFFFFFF	/* Max key that can be stored in queue	*/
#define	MINKEY	0x80000000	/* Min key that can be stored in queue	*/

typedef struct qentry qentry;

struct	qentry	{		/* One per process plus two per list	*/
	int32	qkey;		/* Key on which the queue is ordered	*/
	qentry*	qnext;		/* Index of next process or tail	*/
	qentry*	qprev;		/* Index of previous process or head	*/
	pid32 pid; // pid of the process in question
};

extern	struct qentry*	queuetab[];

/* Inline queue manipulation functions */

#define	queuehead(q)	(q)
#define	queuetail(q)	((q) +1)
#define	firstid(q)		((queuetab[queuehead(q)]->qnext)->pid)
#define	lastid(q)			((queuetab[queuetail(q)]->qprev)->pid)
#define	isempty(q)		(firstid(q) >= NPROC)
#define	nonempty(q)		(firstid(q) <  NPROC)
#define	firstkey(q)		(queuetab[firstid(q)]->qkey)
#define	lastkey(q)		(queuetab[lastid(q)]->qkey)

/* Inline to check queue id assumes interrupts are disabled */

#define	isbadqid(x)	(((int32)(x) < 0) || (int32)(x) >= NQENT-1)
