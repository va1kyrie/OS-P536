/* process_ring - coordinates processes to count
* from some number down to zero. */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process_ring.h>
#include <future.h>
//#include "process_ring.c"

shellcmd xsh_process_ring(int nargs, char *args[]){
  int i = POLL;
  int p = DEFAULTP;
  int r = DEFAULTR;
  int val = 0;
  int start;
  uint32 beg, end;
  int finish;

  //get flags (if they exist)
  int j;
  for(j = 1; j < nargs; j++){
    if(0 == strncmp("-p", args[j], 2)){
      //if flag is '-p'
      if(!(j+1 < nargs)){
        //if no argument after flag
        fprintf(stderr, "%s: -p flag expected an argument\n", args[0]);
        return SHELL_ERROR;
      }

      p = atoi(args[j+1]);
      if(p == 0){
        fprintf(stderr, "%s: -p received invalid integer\n", args[0]);
        return SHELL_ERROR;
      }else if(!(MINP <= p && p <= MAXP)){
        fprintf(stderr, "%s: -p expected a number between %d and %d\n", args[0], MINP, MAXP);
        return SHELL_ERROR;
      }
      j++;
    }else if(0 == strncmp("-r", args[j], 2)){
      //if flag is '-r'
      if(!(j+1<nargs)){
        //no argument after flag
        fprintf(stderr, "%s: -r flag expected an argument\n", args[0]);
        return SHELL_ERROR;
      }
      r = atoi(args[j+1]);
      //printf("r == %d\n", r);
      if(r == 0){
        fprintf(stderr, "%s: -r received invalid integer\n", args[0]);
        return SHELL_ERROR;
      }else if(r < 0){
        fprintf(stderr, "%s: -r flag expected a positive integer\n", args[0]);
        return SHELL_ERROR;
      }else if(r > MAXR){
        fprintf(stderr, "%s: -r flag expected integer less than %d\n", MAXR);
      }
      j++;
    }else if(0 == strncmp("-i", args[j], 2)){
      //if flag is '-i'
      if(!(j+1<nargs)){
        //no argument after flag
        fprintf(stderr, "%s: -i flag expected an argument\n", args[0]);
        return SHELL_ERROR;
      }
      if(0 == strncmp("poll", args[j+1], 5)){
        i = POLL;
      }else if(0 == strncmp("sync", args[j+1], 5)){
        i = SYNC;
      }else if(0 == strncmp("future", args[j+1], 5)){
        i = FUT;
      }else{
        fprintf(stderr, "%s: -i expected either 'sync' or 'poll'\n", args[0]);
        return SHELL_ERROR;
      }
      j++;
    }else if(0 == strncmp("--help", args[j], 6)){
      //if it's the help flag
      printf("process_ring - count down to 0 from an integer in a number of"
      "rounds using a number of processes.\n\n - the starting number depends on"
      "the number of rounds (r) and processes (p)\n - default r = 3\n - default"
      "p = 2\n -r should be a positive integer less than %d \n - p should be"
      "between %d and %d.\n\n flags:\n -p [num_processes]\n -r [num_rounds]\n -i"
      "[poll/sync]\n --help (this text)\n", MAXR, MINP, MAXP);
    }else{
      //else it's invalid
      fprintf(stderr, "%s: invalid arguments given\n", args[0]);
      return SHELL_ERROR;
    }
  }

  //done parsing arguments, move on to process prep and call
  val = (p*r)-1;
  printf("Number of Processes: %d\n", p);
  printf("Number of Rounds: %d\n", r);
  start = gettime(&beg);
  if (start == SYSERR) {
    fprintf(stderr, "%s: could not obtain the current date\n", args[0]);
    return 1;
  }
  // start setting up and queueing up processes
  char name[6] = "proc_";
  char str[12];
  if(i == POLL){
    //if polling is chosen
    volatile int pol[p];
    pol[0] = val;
    for(j = 1; j < p; j++){
      pol[j] = val+1; //initialize all the other inboxes to larger number to
      //avoid out-of-order processing
    }

    for(j = 0; j < p; j++){
      //start queueing up processes
      name[5] = sprintf(str, "%d", j);
      resume(create(process_ring_poll, 1024, 20, name, 5, &pol, j, p, val, r));
    }
    //wait for polling to finish - last element needs to reach 0
    while(pol[p-1] > 0);
  }else if(i == SYNC){
    //else if sync is chosen
    sid32 sems[p];
    sid32 done_sem = semcreate(0); //create semaphore to signal parent
    volatile int ival = val;
    sems[0] = semcreate(1); //first goes free
    for(j = 1; j < p; j++){
      sems[j] = semcreate(0);
    }

    for(j = 0; j < p; j++){
      name[5] = sprintf(str, "%d", j);
      resume(create(process_ring_sync, 1024, 20, name, 6, &sems, j, p, &ival, r, done_sem));
    }

    j=0;
    while(j < p){
      wait(done_sem); //wait for children to all finish
      j++;
    }
    for(j=0;j<p;j++){
      semdelete(sems[j]);
    }
    semdelete(done_sem);
  }else if(i == FUT){
    //else if future mode chosen
    future_t* futs[p];
    future_t* done_fut = future_alloc(FUTURE_QUEUE);
    for(j = 0; j < p; j++){
      futs[j] = future_alloc(FUTURE_SHARED);
    }
    future_set(futs[0], val);

    for(j = 0; j < p; j++){
      name[5] = sprintf(str, "%d", j);
      resume(create(future_process_ring, 1024, 20, name, 7, &futs, j, p, val, r, &done_fut));
    }
    j=0;
    int done = 0;
    while(j<p){
      future_get(done_fut, &done);
      if(done == 10){
        j++;
      }
    }
    for(j=0; j<p;j++){
      future_free(futs[j]);
    }
    future_free(done_fut);

  }
  finish = gettime(&end);
  if (finish == SYSERR) {
    fprintf(stderr, "%s: could not obtain the current date\n", args[0]);
    return 1;
  }
  printf("Elapsed Seconds: %d\n", end-beg);
  return 0;
}
