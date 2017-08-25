/* hello - says hello to you */


#include <xinu.h>
#include <stdio.h>
#include <string.h>

shellcmd xsh_hello(int nargs, char *args[]) {
	
	//check args
	if(nargs > 2){
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}
	if(nargs == 2){
		printf("Hello %s, Welcome to the world of Xinu!\n", args[1]);
	} else{
		printf("Hello, Welcome to the world of Xinu!\n");
	}

	return 0;
}
