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

//get the value of a future set by an operation and change the state of the future from VALID to EMPTY.
syscall future_get(future_t* future, int* val){
  intmask mask;
  struct procent *prptr;
  mask = disable();

  if(future->state == FUTURE_WAITING && (future->mode == FUTURE_QUEUE || future->mode == FUTURE_SHARED)){
    //if the state is waiting and the state is either shared or queue

    //if shared and waiting, just enqueue
    if(future->mode == FUTURE_SHARED){
      prptr = &proctab[getpid()];
      prptr->prstate = PR_WAIT;
      prptr->prfut = future;
      enqueue(getpid(), future->get_queue);
      resched();
    }
    //if queue and waiting, also enqueue?
    //no, check set_queue first.
    if(future->mode == FUTURE_QUEUE){
      if(!isempty(future->set_queue)){
        ready(dequeue(future->set_queue));
        resched();
      }else{
        prptr = &proctab[getpid()];
        prptr->prstate = PR_WAIT;
        prptr->prfut = future;
        enqueue(getpid(), future->get_queue);
        resched();
      }
    }
  }

  if(future->state == FUTURE_EMPTY){
    //printf("we are in the empty if statement\n");
    future->state = FUTURE_WAITING;
    if(future->mode == FUTURE_QUEUE){
      //if there is no thread in set_queue, enqueue self and wait
      // in other words, do nothing here, just leave the if statement

      //if there is at least one thread in set_queue, then dequeue the first and make it ready. then enqeue self (to be dequeued by the setting thread in question)
      if(!isempty(future->set_queue)){
        ready(dequeue(future->set_queue));
      }
    }
   //if the mode is EXCLUSIVE or SHARED, the behavior is the same if we're in EMPTY mode, with one exception: we need to also check if the state is WAITING. which we do above, because we set the state here.

    //because we're waiting, caller has to block
    prptr = &proctab[getpid()];
    prptr->prstate = PR_WAIT;
    prptr->prfut = future;
    enqueue(getpid(), future->get_queue);
    resched();
  }

  //printf("outside the ready check\n");
  //printf("future_get: exclusive state == %d\n", future->state);
  if(future->state == FUTURE_READY){
    *val = future->value;
    //printf("value of val = %d\n", *val);
  }

  restore(mask);
  return OK;
}

//sets value in a future and changes state from EMPTY to VALID.
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
      //printf("future_set: exclusive set to ready (state == %d)\n", future->state);
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
