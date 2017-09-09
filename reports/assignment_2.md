## Assignment 2

### 1: What are 2 things that you learned from this assignment?

First, I learned - or reminded myself, at least - that no matter how well I think I have handled race conditions, I probably haven't actually handled _all_ of them. (To be completely honest, I'm not convinced there isn't a race condition still in my code, but it hasn't done anything obvious in a while, so I'm hoping I did okay.)

The second big thing I learned was about message passing - I knew about semaphores before taking this class, and I've even used them a few times, but other ways of syncing data or passing information between threads was more or less completely new to me. I'm still a bit confounded by how I was _almost_ able to make message passing work for my sync option, but there was a nasty race condition at the end, when all the child threads reported back to the parent. Nine times out of ten, the parent hung, waiting for a receive() that would never successfully return because 2 children had sent messages too quickly after each other. I never did figure that out.

### 2: What are 2 mistakes that you made in this assignment, and how did you discover and fix these mistakes?

My biggest mistake was, honestly, not giving myself enough time to easily complete the assignment. i severely underestimated the time I would need for debugging, especially, and so rather than refining the receive() call as I had originally planned (I hoped to create a queue of messages), I changed my sync implementation to semaphores. While it's a bit difficult to fix "not having enough time", changing my implementation is, I think, the closest I can get to a fix.

Another mistake I made continuously throughout the assignment at first was reassigning my 'i' variable. I rather cleverly made ```int i``` to track the ```-i``` flag... and then proceeded to use for loops with ```i```. I replaced all the for loop variables with ```j``` as soon as I noticed (which was fairly quickly, given my code was acting a bit strange).

### 3: If you were restarting this assignment, what would you change about how you went about completing this assignment?

I would give myself a lot more time, and I would plan out my implementation a lot more completely. I would especially start earlier so I could take my more subtle bugs and race conditions to another set of eyes, instead of having to rely on myself.
