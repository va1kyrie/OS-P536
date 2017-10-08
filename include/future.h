#ifndef _FUTURE_H_
#define _FUTURE_H_

#ifndef NFUT
#define NFUT 100
#endif

typedef enum {
  FUTURE_EMPTY,         //future is allocated
  FUTURE_WAITING,       //future on which a process is actively waiting (via future_get())
  FUTURE_READY          //future holding a set value
} future_state_t;

typedef enum {
  FUTURE_EXCLUSIVE,     //max. of 2 threads may be involved. 1 sets, 1 gets.
  FUTURE_SHARED,        //1-to-many relationship. 1 set, many get.
  FUTURE_QUEUE          //many-to-many relationships. need to implement queue type
} future_mode_t;

// struct fentry {
//   struct fent *fnext;
//   struct fent *fprev;
//   pid32 thread;
// }; //need to continue this one

typedef struct future_t {
  int value;            //value to be held in the future
  future_state_t state; //may be EMPTY, WAITING, or VALID
  future_mode_t mode;   //mode of opertaion
  pid32 pid;            //pid of thread waiting for value
  qid16 set_queue;   //for the queue types
  qid16 get_queue;   //need to implement!!!!
} future_t;

/* Interface for the Futures system calls */
future_t* future_alloc(future_mode_t mode);
syscall future_free(future_t*);
syscall future_get(future_t*, int*);
syscall future_set(future_t*, int);

//process interfaces
uint future_prod(future_t*,int);
uint future_cons(future_t*);

#endif /* _FUTURE_H_ */
