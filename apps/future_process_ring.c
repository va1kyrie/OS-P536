#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <process_ring.h>
#include <future.h>

process future_process_ring(future_t** futs, int ind, int len, int ival, int mrnd){
  int status;
  int val;
  int last = ival+1; //starter val to indicate circle hasn't started.
  int rnd = 0;
  if(len == 1){
    //we have special case of one process
    int j;
    for(j = ival; j > -1; j--){
      printf("Ring Element %d : Round %d : Value : %d\n", ind, rnd, j);
      rnd++;
    }
  }else{
    //else multiple children
    while(last >= 0 && rnd < mrnd){
    //  printf("in the while loop round %d\n", rnd);
      future_get(futs[ind], &val);
      if(last != val && val >= 0){
        printf("Ring Element %d : Round %d : Value %d\n", ind, rnd, val);
        if(ind+1 == len){
          future_set(futs[0], val-1);
        }else{
          future_set(futs[ind+1], val-1);
        }
        last = val;
        rnd++;
      }
    }
  }
  //if we're out here, we're done
  //future_set(done_fut,10);
  return 0;
}
