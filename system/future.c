/* future.c - the implementations of the system calls for futures. */


// allocate a new future in EMPTY state with given mode. uses getmem()
future_t* future_alloc(future_mode_t mode){
  future_t *alloc = (future_t*) getmem(sizeof(future_t));

  alloc->value = NULL;
  alloc->state = FUTURE_EMPTY;
  alloc->mode = mode;
  if(mode == FUTURE_QUEUE){
    alloc->set_queue = newfq();
    alloc->get_queue = newfq();
  }
}

//frees the future. uses freemem()
syscall future_free(future_t*){

}

//get the value of a future set by an operation and change the state of the future from VALID to EMPTY.
syscall future_get(future_t*, int*){

}

//sets value in a future and changes state from EMPTY to VALID.
syscall future_set(future_t*, int){

}
