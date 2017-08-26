/* hello - says hello to you */


#include <xinu.h>
#include <stdio.h>
#include <string.h>


/*
 * xsh_hello(int, char*): takes in a string argument (or no string) and says hello to that 
 * argument.
 */
shellcmd xsh_hello(int nargs, char *args[]) {
	
	//check args
	if(nargs > 2){
		//if there are too many arguments, return error and exit

		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}
	if(nargs == 2){
		//if a string was given as an argument, say hello using that string

		printf("Hello %s, Welcome to the world of Xinu!\n", args[1]);
	} else{
		//if no argument was given, just say hello

		printf("Hello, Welcome to the world of Xinu!\n");
	}

	return 0;
}
