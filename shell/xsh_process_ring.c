/* process_ring - coordinates processes to count
* from some number down to zero. */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
#include <process_ring.h>

parse_args(int32 len, char arg[]){
  int32 val;
  char *endp;

}

xsh_process_ring(int nargs, char *args[]){
  int i = POLL;
  int p = DEFAULTP;
  int r = DEFAULTR;
  int val = 0;
  int start;
  uint32 now;
  int finish;

  //get flags (if they exist)
  int i;
  for(i = 1; i < nargs; i++;){
    if(0 == strncmp("-p", args[i], 3)){
      //if flag is '-p'
      if(!(i+1 < nargs)){
        //if no argument after flag
        print_usage();
        printf("-p flag expected an argument\n");
        return SHELL_ERROR;
      }

      p = atoi(args[i+1]);
      if(p == 0){
        print_usage();
        printf("-p received invalid integer\n");
        return SHELL_ERROR;
      }else if(!(MINP <= p && p <= MAXP)){
        print_usage();
        printf("-p expected a number between %d and %d\n", MINP, MAXP);
        return SHELL_ERROR;
      }
      i++;
    }else if(0 == strncmp("-r", args[i], 3)){
      //if flag is '-r'
      if(!(i+1<nargs)){
        //no argument after flag
        print_usage();
        printf("-r flag expected an argument\n");
        return SHELL_ERROR;
      }
      r = atoi(args[i+1]);
      if(r == 0){
        print_usage();
        printf("-r received invalid integer\n");
        return SHELL_ERROR;
      }else if(r < 0){
        print_usage();
        printf("-r expected positive integer\n");
        return SHELL_ERROR;
      }
      i++;
    }else if(0 == strncmp("-i", args[i], 3)){
      //if flag is '-i'
      if(!(i+1<nargs)){
        //no argument after flag
        print_usage();
        printf("-i flag expected an argument\n");
        return SHELL_ERROR;
      }
      if(0 == strncmp("poll", args[i+1], 5)){
        r = POLL;
      }else if(0 == strncmp("sync", args[i+1], 5)){
        r = SYNC;
      }else{
        print_usage();
        printf("-i expected either 'sync' or 'poll'\n");
        return SHELL_ERROR;
      }
      i++;
    }else if(0 == strncmp("--help", args[i], 7)){
      //if it's the help flag
      printf("process_ring - count down to 0 from an integer in a number of
      rounds using a number of processes.\n\n - the starting number depends on
      the number of rounds (r) and processes (p)\n - default r = 3\n - default
      p = 2\n -r should be a positive integer less than %d \n - p should be
      between %d and %d.\n\n flags:\n -p [num_processes]\n -r [num_rounds]\n -i
      [poll/sync]\n --help (this text)\n", MAXR, MINP, MAXP);
    }else{
      //else it's invalid
      print_usage();
      printf("invalid arguments given\n");
      return SHELL_ERROR;
    }

    //done parsing arguments, move on to process prep and call
    val = (p*r)-1;
    printf("Number of Processes: %d\n", p);
    printf("Number of Rounds: %d\n", r);
    start = gettime(&now);
    if (start == SYSERR) {
      fprintf(stderr,
        "%s: could not obtain the current date\n",
        args[0]);
        return 1;
      }
      // start setting up and queueing up processes
      if(i == POLL){
        //if polling is chosen
        int pol[p];
        pol[0] = val;
        int len = p;
        char name[6] = "proc_";
        char str[12];
        for(i = 0; i < p; i++){
          //start queueing up processes
          name[5] = sprintf(str, "%d", i);
          resume(create(process_ring_poll, 1024, 20, name, *pol, i, p));
        }
      }else if(i == SYNC){
        //else if sync is chosen
      }
      finish = gettime(&now);
      if (finish == SYSERR) {
        fprintf(stderr,
          "%s: could not obtain the current date\n",
          args[0]);
          return 1;
        }
      }

      return 0;
    }
