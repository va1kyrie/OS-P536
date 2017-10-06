/* future.c - the implementations of the system calls for futures. */

#include <stdio.h>
#include <stdlib.h>
#include <xinu.h>
#include <future.h>

//newfq() -- create new queue for futures
// returns a pointer to the head of the new queue
struct fentry *newfq(void) {
  //TODO: IMPLEMENT

  struct fentry head;
  struct fentry tail;

  //initialize head
  head.prev = HEAD;
  head.next = *tail;
  head.thread = HEAD;

  //initialize tail
  tail.prev = *head;
  tail.next = TAIL;
  tail.thread = TAIL;

  return *head;
}


// allocate a new future in EMPTY state with given mode. uses getmem()
future_t* future_alloc(future_mode_t mode){
  future_t *alloc = (future_t*) getmem(sizeof(future_t));

  alloc->value = NULL;
  alloc->state = FUTURE_EMPTY;
  alloc->mode = mode;
  if(mode == FUTURE_QUEUE){
    alloc->set_queue = newqueue();
    alloc->get_queue = newqueue();
  }else if(mode == FUTURE_SHARED){
    alloc->get_queue = newqueue();
    alloc->set_queue = NULL;
  }else{
    alloc->get_queue = NULL;
    alloc->set_queue = NULL;
  }
}

//frees the future. uses freemem()
syscall future_free(future_t*){
  //TODO: implement this
}

//get the value of a future set by an operation and change the state of the future from VALID to EMPTY.
syscall future_get(future_t*, int*){
  //TODO: implement this
}

//sets value in a future and changes state from EMPTY to VALID.
syscall future_set(future_t*, int){
  //TODO: implement this
}
