## Assignment 5 Report

### Implementation of futures

My implementation of futures takes advantage of the existing queue structure within Xinu to handle processes waiting on futures (either while getting or setting).

#### FUTURE_EXCLUSIVE
The exclusive implementation of futures uses the ```get_queue``` to wait on the future. It is a fairly straightforward implementation: if the setting thread reaches the future first, it sets the value and sets the state to ```FUTURE_READY``` before exiting. The getting thread then has no issue accessing the value of the future.

If the getting thread accesses the future first, however, it sets the state to ```FUTURE_WAITING``` and queues itself in the ```get_queue``` for the future in question. The setting thread will then set the value and state per usual and dequeue and wake up the getting thread before exiting, allowing the getting thread to access the value.

#### FUTURE_SHARED
Conceptually, this implementation is very similar to the exclusive mode. The main difference is that multiple threads may get the value from the future, meaning that multiple threads may queue themselves up in the ```get_queue```. If the state is ```FUTURE_EMPTY``` when a getting thread tries to access it, the thread sets the state to ```FUTURE_WAITING``` and enqueues itself. If the state is already ```FUTURE_WAITING```, the getting thread simply enqueues itself and waits.

The setting thread operates the same as in the exclusive mode, but when it sets the value, if the ```get_queue``` is nonempty, the setting thread dequeues and readies each thread in the queue, instead of just the one.

#### FUTURE_QUEUE
This implementation is the most unique of the three. There may be many of both getting and setting threads, so both ```future_get``` and ```future_set``` must check the opposite queue before exiting. The getting thread, if the state is ```FUTURE_WAITING``` and the ```set_queue``` is nonempty, we
