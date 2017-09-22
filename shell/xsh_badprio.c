/* badprio - demonstrating the foibles of resume() */


#include <xinu.h>
#include <stdio.h>
#include <string.h>

process printing(pid32 parentid){
	pid32 mypid = getpid();
	printf("process %d has priority %d\n", mypid, getprio(mypid));
	send(parentid, mypid);
	suspend(mypid);
	return 0;
}

/*
 * xsh_badprio(int, char*): spawns a child process to demonstrate that
 * resume() may return a bad priority should it reference prptr-->prprio
 * after calling ready()
 */
shellcmd xsh_hello(int nargs, char *args[]) {

	//check args
	if(nargs > 1){
		//if there are too many arguments, return error and exit
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}

	pid32 parentid = getpid();

	resume(create(printing, 1024, 20, "child", 1, parentid);
	pid32 childpid = receive();
	pri16 chprio = resume(childpid);

	printf("parent process: child prio is %d\n", chprio);

	return 0;
}
