#include <xinu.h>
#include <prodcons.h>
#include <future.h>

sid32 produced, consumed;

shellcmd xsh_prodcons(int nargs, char *args[])
{
      //Argument verifications and validations
  produced = semcreate(0);
  consumed = semcreate(1);

  int count = 2000;             //local varible to hold count

  //check args[1] if present assign value to count

  //create the process producer and consumer and put them in ready queue.
  //Look at the definations of function create and resume in exinu/system folder for reference.
  //resume( create(producer, 1024, 20, "producer", 1, count));
  //resume( create(consumer, 1024, 20, "consumer", 1, count));

  future_t* f_exclusive,
        * f_shared,
        * f_queue;

f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
printf("exclusive state == %d\n", f_exclusive->state);
f_shared    = future_alloc(FUTURE_SHARED);
f_queue     = future_alloc(FUTURE_QUEUE);

// Test FUTURE_EXCLUSIVE
resume(create(future_cons, 1024, 21, "fcons1", 1, f_exclusive));
resume(create(future_prod, 1024, 21, "fprod1", 2, f_exclusive, 1));

// Test FUTURE_SHARED
resume( create(future_cons, 1024, 21, "fcons2", 1, f_shared) );
resume( create(future_cons, 1024, 21, "fcons3", 1, f_shared) );
resume( create(future_cons, 1024, 21, "fcons4", 1, f_shared) );
resume( create(future_cons, 1024, 21, "fcons5", 1, f_shared) );
resume( create(future_prod, 1024, 21, "fprod2", 2, f_shared, 2) );

Test FUTURE_QUEUE
resume( create(future_cons, 1024, 21, "fcons6", 1, f_queue) );
resume( create(future_cons, 1024, 21, "fcons7", 1, f_queue) );
resume( create(future_cons, 1024, 21, "fcons8", 1, f_queue) );
resume( create(future_cons, 1024, 21, "fcons9", 1, f_queue) );
resume( create(future_prod, 1024, 21, "fprod3", 2, f_queue, 3) );
resume( create(future_prod, 1024, 21, "fprod4", 2, f_queue, 4) );
resume( create(future_prod, 1024, 21, "fprod5", 2, f_queue, 5) );
resume( create(future_prod, 1024, 21, "fprod6", 2, f_queue, 6) );

  return (0);
}
