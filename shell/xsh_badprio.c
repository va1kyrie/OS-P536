/* badprio - demonstrating the foibles of resume() */


#include <xinu.h>
#include <stdio.h>
#include <string.h>

process childtest(pid32 parentid){
	pid32 mypid = getpid();
	printf("process %d has priority %d\n", mypid, getprio(mypid));
	int i;
	for(i = 65; i < 91; i++){
		printf("%c", i);
	}
	printf("\n");
  //pri16 mychiprio = resume2(create(childtest, 1024, 40, "child2", 1, parentid));
	send(parentid, mypid);
	suspend(mypid);
	resched();
	printf("process %d is running again\n", mypid);
	kill(mypid);
	//resume2(parentid);
	return 0;
}

/*
 * xsh_badprio(int, char*): spawns a child process to demonstrate that
 * resume() may return a bad priority should it reference prptr-->prprio
 * after calling ready()
 */
shellcmd xsh_badprio(int nargs, char *args[]) {

	//check args
	if(nargs > 1){
		//if there are too many arguments, return error and exit
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}

	pid32 parentid = getpid();

	pri16 chprio = resume2(create(childtest, 1024, 25, "child", 1, parentid));
	resched();
	pri16 chprio2 = resume2(create(childtest, 1024, 25, "child2", 1, parentid));
	resched();
	//printf("parent process: child prio is %d initially\n", childprioinit);
	pid32 childpid = receive();
	pid32 child2 = receive();

	pid32 ch1 = resume2(childpid);
	pid32 ch2 = resume2(child2);
	//printf("childpid == %d\n", childpid);
	//pri16 chprio = resume(childpid);

	printf("parent process: child %d prio is %d\n", childpid, ch1);
	printf("parent process: child %d prio is %d\n", child2, ch2);

	//printf("parent process: child prio is %d\n", resume2(create(childtest, 1024, 35, "child2", 1, parentid)));

	return 0;
}
