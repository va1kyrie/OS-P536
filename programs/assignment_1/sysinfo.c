#include <stdio.h>

int main(int nargs, char *args[]){

	int pid ;
	int ppid;
	int cpid;

	pipe(args[1]);
	
	pid = fork();
	if(pid == 0) {
		//we are in the child
		cpid = getpid();
		
	}

}
