// #include <xinu.h>
// #include <string.h>
// #include <stdio.h>
// #include <process_ring.h>
// #include <future.h>
//
// uint future_process_ring(future_t* futs, int ind, int len, int ival, int mrnd, future_t* done_fut){
//   int status;
//   int i;
//   int last = ival+1; //starter val to indicate circle hasn't started.
//   int rnd = 0;
//   if(len == 1){
//     //we have special case of one process
//     int j;
//     for(j = ival; j > -1; j--){
//       printf("Ring Element %d : Round %d : Value : %d\n", ind, rnd, j);
//       rnd++;
//     }
//     pol[ind] = 0;
//   }else{
//     //else multiple children
//     while(last > 0 && rnd < mrnd){
//       future_get(futs[ind]);
//     }
//   }
// }
