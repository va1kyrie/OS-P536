/* hello - says hello to you */


#include <xinu.h>
#include <stdio.h>
#include <string.h>


/*
 * xsh_testitem(int, char*): tests whether getitem(), getfirst(), and getlast() really do check
 *			     things now.
 */
shellcmd xsh_testitem(int nargs, char *args[]) {
	
	int test;
	
	//check args
	if(nargs > 2){
		//if there are too many arguments, return error and exit

		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	}
	if(nargs < 2){
		//else if too few, set default (larger than NQENT and NPROC)
		test = NQENT + NPROC;
	}else{
		//get the integer value of the argument
		test = atoi(args[1]);
	}

	if(getitem(test) == SYSERR){
		printf("getitem() successfully threw an error on %d\n", test);
	}else{
		printf("getitem() did not throw error on %d\n", test);
	}
	if(getfirst(test) == SYSERR){
		printf("getfirst() successfully threw an error on %d\n", test);
	}else{
		printf("getfirst() did not throw error on %d\n", test);
	}
	if(getlast(test) == SYSERR){
		printf("getlast() successfully threw an error on %d\n", test);
	}else{
		printf("getlast() did not throw error on %d\n", test);
	}

	return 0;
}
