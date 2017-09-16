/* hello - says hello to you */


#include <xinu.h>
#include <stdio.h>
#include <string.h>

process printing(){
	printf("process %d says hi!\n", getpid());
	return 0;
}


/* xsh_testresched(int, char*): tests resched2
*/
shellcmd xsh_testresched2(int nargs, char *args[]) {
 create(printing, 1024, 25, "printing-hiprio", 0);
 resched2(PR_READY);
 printf("parent process %d says hi!\n", getpid());
 create(printing, 1024, 20, "printing-sameprio",0);
 resched2(PR_CURR);
 printf("parent process %d says hi! (should be second to last?)\n", getpid());
 return 0;
}
