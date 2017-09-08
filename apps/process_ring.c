/* apps/process_ring.c - helper for
 * process_ring shell command.
 */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

process process_ring_poll(int *pol, int ind, int len, int ival){
  //code
  int last = ival+1;  //starter val to indicate you haven't started calc.
  int rnd = 0;
  while(last >= 0){
    //poll to see if there's a new value to print
    int tmp = pol[ind];
    if(last != tmp){
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
  return 0;
}

process process_ring_sync(){
  //code
}
