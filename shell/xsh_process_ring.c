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

  //get flags (if they exist)
  //char *endp;
  int i;
  for(i = 1; i < nargs; i++;){
    if(0 == strncmp("-p", args[i], 3)){
      //if flag is '-p'
      if(!(i+1 < argc)){
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
      }else if(!(0 <= p && p <= 64)){
        print_usage();
        printf("-p expected a number between 0 and 64\n");
        return SHELL_ERROR;
      }
      i++;
    }else if(0 == strncmp("-r", args[i], 3)){
      //if flag is '-r'

    }else if(0 == strncmp("-i", args[i], 3)){
      //if flag is '-i'

    }else if(0 == strncmp("--help", args[i], 4)){
      //if it's the help flag
      
    }else{
      //else it's invalid
      print_usage();
      printf("invalid arguments given\n");
      return SHELL_ERROR;
    }
  }

}
