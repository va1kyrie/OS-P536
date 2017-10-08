/* future.c - the implementations of the system calls for futures. */

#include <xinu.h>
#include <future.h>

// //newfq() -- create new queue for futures
// // returns a pointer to the head of the new queue
// struct fentry *newfq(void) {
//   //TODO: IMPLEMENT
//
//   struct fentry head;
//   struct fentry tail;
//
//   //initialize head
//   head.prev = HEAD;
//   head.next = &tail;
//   head.thread = HEAD;
//
//   //initialize tail
//   tail.prev = &head;
//   tail.next = TAIL;
//   tail.thread = TAIL;
//
//   return *head;
// }


// allocate a new future in EMPTY state with given mode. uses getmem()
future_t* future_alloc(future_mode_t mode){
  future_t *alloc = (future_t*) getmem(sizeof(future_t));

  alloc->value = NULL;
  alloc->state = FUTURE_EMPTY;
  alloc->mode = mode;
  alloc->pid = getpid(); //need to change for multi relationships?... maybe. don't think so but maybe
  if(mode == FUTURE_QUEUE){
    //if the mode is many-to-many need both get and set queues
    alloc->set_queue = newqueue();
    alloc->get_queue = newqueue();
  }else if(mode == FUTURE_SHARED || mode == FUTURE_EXCLUSIVE){
    //if mode is one-to-many need only get queue
    //also if mode is 1-1 we only need the get_queue
    alloc->get_queue = newqueue();
    alloc->set_queue = EMPTY; //assign empty indicating value instead of null?
  }

  return alloc;
}

//frees the future. uses freemem()
syscall future_free(future_t* future){
  //TODO: implement this
  pid32 proc;
  if(future->get_queue != EMPTY){
    while(!isempty(future->get_queue)){
      proc = dequeue(future->get_queue);
    }

    future->get_queue = EMPTY;
  }

  freemem((char *) future, sizeof(future_t));

  return OK;
}

/* future_get(future_t*, val*) - get the value of a future set by an operation
 * and change the state of the future from EMPTY to WAITING if necessary. If the mode is exclusive, it either gets the value that has already been set or sets the future's state to WAITING and enqueues itself in the get_queue, where it will be woken up by the setting thread. If the mode is SHARED, it acts in much the same manner. If the mode is QUEUE, it will check if there is a thread waiting in the set_queue; if there is, it will wake it up before */
syscall future_get(future_t* future, int* val){
  intmask mask;
  struct procent *prptr;
  mask = disable();

  //if we're in shared mode and waiting, just enqueue
  if(future->mode == FUTURE_SHARED && future->state == FUTURE_WAITING){
    prptr = &proctab[getpid()];
    prptr->prstate = PR_WAIT;
    prptr->prfut = future;
    enqueue(getpid(), future->get_queue);
    resched();
  }else if(future->mode == FUTURE_QUEUE){
    //else if the state is waiting and we're in queued mode
    if(!isempty(future->set_queue)){
      ready(dequeue(future->set_queue));
    }
    prptr = &proctab[getpid()];
    prptr->prstate = PR_WAIT;
    prptr->prfut = future;
    enqueue(getpid(), future->get_queue);
    resched();

  }

  if(future->state == FUTURE_EMPTY){
    //printf("we are in the empty if statement\n");
    future->state = FUTURE_WAITING;
    if(future->mode == FUTURE_QUEUE){
      //if there is at least one thread in set_queue, then dequeue the first and make it ready. then enqeue self (to be dequeued by the setting thread in question)
      if(!isempty(future->set_queue)){
        ready(dequeue(future->set_queue));
      }
      //if there is no thread in set_queue, enqueue self and wait -- in other words, do nothing here, just leave the if statement
    }
   //if the mode is EXCLUSIVE or SHARED, the behavior is the same if we're in EMPTY mode, with one exception: we need to also check if the state is WAITING. which we do above, because we set the state here.

    //because we're now waiting, caller has to block
    prptr = &proctab[getpid()];
    prptr->prstate = PR_WAIT;
    prptr->prfut = future;
    enqueue(getpid(), future->get_queue);
    resched();
  }

  if(future->state == FUTURE_READY){
    *val = future->value;
  }

  restore(mask);
  return OK;
}

/* future_set(future_t*, int) - sets the value in a future and changes state
 * from EMPTY to READY. If the mode is SHARED, it will wake up any and all
 * threads in the get_queue. If the mode is QUEUE, it will wake up the first
 * thread in get_queue. If there are no threads in the get_queue, then it will
 * queue itself in the set_queue and wait to be woken up.*/
syscall future_set(future_t* future, int val){

  intmask mask;
  struct procent *prptr;
  //pid32 waiting;
  mask = disable();
  //if the mode is exclusive (1-1) then just make sure the future is waiting or empty and set the val and change the state
  //gotta remember to take the wait state off the process in the get_queue if it's waiting
  if(future->mode == FUTURE_EXCLUSIVE){
    if(future->state == FUTURE_WAITING ||future->state == FUTURE_EMPTY){
      future->value = val;
      future->state = FUTURE_READY;
      if(!isempty(future->get_queue)){
        ready(dequeue(future->get_queue));
      }
    }
  }

  if(future->mode == FUTURE_SHARED){
    //1-many relationship.

    //check if set already -- if so, throw error. (in exclusive mode too?)
    if(future->state == FUTURE_READY){
      restore(mask);
      return SYSERR;
    }else if(future->state == FUTURE_WAITING){
      //else set value
      future->value = val;
      future->state = FUTURE_READY;

      //check if threads are in get_queue -- if they are, make all of them ready
      while(!isempty(future->get_queue)){
        ready(dequeue(future->get_queue));
      }
    }
  }

  if(future->mode == FUTURE_QUEUE){
    //many-to-many relationship

    //check get_queue for waiting threads. if there are, set value, resume first thread in get_queue.
    if(!isempty(future->get_queue)){
      future->value = val;
      future->state = FUTURE_READY;
      ready(dequeue(future->get_queue));
    }else{
      //else enqueue self into set_queue and wait
      prptr = &proctab[getpid()];
      prptr->prstate = PR_WAIT;
      prptr->prfut = future;
      enqueue(getpid(), future->set_queue);
    }
  }

  restore(mask);
  return OK;
}
