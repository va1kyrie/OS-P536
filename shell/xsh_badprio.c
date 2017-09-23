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
	send(parentid, OK);
	suspend(mypid);

	resched();
	printf("process %d is running again\n", mypid);
	//kill(mypid);
	//resume2(parentid);
	return 0;
}

process childtest2(pid32 parentid, pid32 otherchild){
	pid32 mypid = getpid();
	printf("process %d has priority %d\n", mypid, getprio(mypid));
	//int i;
	// for(i = 65; i < 91; i++){
	// 	printf("%c", i);
	// }
	//printf("\n");
	//pid32 chpid = create(childtest, 1024, 40, "childchild", 1, parentid);
  //pri16 mychiprio = resume2(chpid);
	//receive();
	//printf("child process: my child %d has priority %d\n", chpid, mychiprio);
	//chprio(chpid, 50);
	//resume2(chpid);
	kill(otherchild);
	printf("otherchild dead\n");
	send(parentid, OK);
	//suspend(mypid);

	//resched();
	//printf("process %d is running again\n", mypid);
	//kill(mypid);
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

	pid32 childpid = create(childtest, 1024, 35, "child", 1, parentid);
	pid32 child2 = create(childtest2, 1024, 45, "child2", 1, parentid);
	pri16 ch1 = resume(childpid);
	pri16 ch2 = resume(child2);
	resched();
	//printf("parent process: child prio is %d initially\n", childprioinit);

	receive();
	//pri16 ch1 = resume(childpid);
	//resched2();
	//receive();

	sleep(5);
  ch1 = resume(childpid);
	//receive();
	//pid32 ch2 = resume2(child2);
	//printf("childpid == %d\n", childpid);
	//pri16 chprio = resume(childpid);

	printf("parent process: child %d prio is %d\n", childpid, ch1);
	printf("parent process: child %d prio is %d\n", child2, ch2);

	//printf("parent process: child prio is %d\n", resume2(create(childtest, 1024, 35, "child2", 1, parentid)));

	return 0;
}
