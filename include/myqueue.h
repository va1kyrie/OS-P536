/* myqueue.h - header file for myqueue. */

/* constants, structs, and inline functions for myqueue */

#ifndef NQENT
#define NQENT (NPROC + 4 + NSEM + NSEM)
#endif

#define EMPTY (-1) //null value
#define MAXKEY
#define MINKEY

struct myentry {
  int32 mkey;
  int32 mnext;
  int32 mprev;
};
