/* hello - says hello to you */


#include <xinu.h>
#include <stdio.h>
#include <string.h>

process printing(){
	printf("process %d says hi!\n", getpid());
	return 0;
}


/*
 * xsh_testresched(int, char*): tests resched2
 */
shellcmd xsh_testresched(int nargs, char *args[]) {
	resume(create(printing, 1024, 25, "printing-hiprio", 0));
	resched2(PR_READY);
	resume(create(printing, 1024, 20, "printing-sameprio",0));
	resched2(PR_CURR);
	return 0;
}
