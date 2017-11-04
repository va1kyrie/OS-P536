# Assignment 6

I will be the first to admit this assignment got the best of me in a lot of ways. I spent a fair amount of time just trying to figure out what was going on with the code and how, exactly, all the structs and arrays interacted with each other.

My implementation of the filesystem functions is largely based on what was there already and the test file; there are additional things I would like to implement in theory concerning the file system, but I'm not sure where to start in terms of actually implementing them. For instance, my implementation of ```fs_read``` and ```fs_write``` both laregly assume friendly inputs - while I did add a rudimentary 'size' option to my filesystem struct so I could prevent _reading_ over the end of a file, the size is entirely decided by the writing function and doesn't do any "border control" (for lack of a better term) that way.
