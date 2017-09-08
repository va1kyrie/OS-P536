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
      if((0 == strncmp("0", args[i+1],3)) || (0 == strncmp("00", args[i+1],3))){
        p = 0;
      }else{
        p = atoi(args[i+1]);
        if(p == 0){
          print_usage();
          printf("-p received invalid integer\n");
          return SHELL_ERROR;
        }else if(!(0 <= p && p <= 64)){
          print_usage();
          printf("-p expected a number between %d and %d\n", MINP, MAXP);
          return SHELL_ERROR;
        }
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
      if((0 == strncmp("0", args[i+1],3)) || (0 == strncmp("00", args[i+1],3))){
        r = 0; //not convinced this is the right route...
      }else{
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
      printf("process_ring - count down to 0 from an integer in a number of rounds using a number of processes.\n\n - the starting number depends on the number of rounds (r) and processes (p)\n - default r = 3\n - default p = 2\n -r should be a positive integer\n - p should be between 0 and 64.\n\n flags:\n -p [num_processes]\n -r [num_rounds]\n -i [poll/sync]\n --help (this text)\n");
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
    if(val == 0 && p == 0){
      //if p == 0
      printf("Ring Element 0 : Round 0 : Value : 0\n");
    }else if(val == 0 && r == 0){
      //what is the defined behavior if r or p == 0 actually? just return with elapsed time?
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
