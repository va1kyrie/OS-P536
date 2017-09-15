## Assignment 3

### Question 1

### Question 2

A valid queue ID in Xinu is an integer less than the number of elements in ```queuetab[]``` and, generally, greater than 0. More specifically, a valid queue ID really should be greater than or equal to NPROC, to allow for sufficient entries in ```queuetab[]``` and to prevent accidentally accessing an arbitrary process instead of the head or tail of a particular queue.

### Question 3
