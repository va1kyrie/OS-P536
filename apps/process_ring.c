/* apps/process_ring.c - helper for
 * process_ring shell command.
 */

#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <process_ring.h>

process process_ring_poll(volatile int *pol, int ind, int len, int ival, int mrnd){
  //code
  int last = ival+1;  //starter val to indicate you haven't started calc.
  int rnd = 0;
  if(len == 1){
    //special case if we're only making one process
    int j;
    for(j = ival; j > -1; j--){
      printf("Ring Element %d : Round %d : Value : %d\n", ind, rnd, j);
      rnd++;
    }
    pol[ind] = 0;
  }else{
    //else we have multiple children
    while(last >= 0 && rnd < mrnd){
      //poll to see if there's a new value to print
      int tmp = pol[ind];
      if(last != tmp && tmp >= 0){
        printf("Ring Element %d : Round %d : Value : %d\n", ind, rnd, tmp);
        if(ind+1 == len){
          //if we're at the end of the list, update the first inbox
          pol[0] = tmp-1;
        }else{
          //else just update the next one
          pol[ind+1] = tmp-1;
        }
        last = tmp; //last value is the one just printed
        rnd++;      //increase the round count
      }
    }
  }
  //if we're out here mrnds have passed and we shouldn't do more comp.
  return 0;
}

process process_ring_sync(pid32 *pids, int ind, int len, int val, pid32 parent, int mrnds){
  int last = val+1;
  int rnd = 0;
  if(len == 1){
    //again, if we only have one process we need a special case
    int j;
    for(j = val; j > -1; j--){
      printf("Ring Element %d : Round %d : Value : %d\n", ind, rnd, j);
      rnd++;
    }
  }else{
    while(last > 0 && rnd < mrnds){
      //printf("pid of proc %d: %d \n", ind, (int) getpid());
      //printf("I am in the while loop on round %d\n", rnd);
      if(last == val+1 && rnd == 0 && ind == 0){
        //special case if first element on first round -- nothing to receive!
        last--;
        printf("Ring Element %d : Round %d : Value : %d\n", ind, rnd, last);
      }else{
        //printf("process %d: I am in the while loop in the else on round %d\n", ind, rnd);
        last = receive();
      	printf("Ring Element %d : Round %d : Value : %d\n", ind, rnd, last);
      }
      //printf("PID of next element: %d\n", (int) pids[ind+1]);
      if(ind+1 == len){
        send(pids[0], last-1);
      }else{
        send(pids[ind+1], last-1);
      }
      rnd++;
      printf("process %d about to go around again\n", ind);
    }
  }

  //if we're here we're done, leave and never come back
  send(parent, OK);
  return 0;
}
