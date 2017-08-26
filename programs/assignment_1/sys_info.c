#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define CMD_LEN 100		//allowed maximum length of the command argument

/*
 * sys_info(str): takes in a filepath for a Linux command (ex: "/bin/ls") and creates a child
 * 		  process which executes said command with execl(). If the command is "/bin/echo",
 *		  it prints "Hello world!"
 *
 *		  In addition, prints the PID of both the parent and child processes.
 *
 *		  The command filepath must be the only argument given to sys_info, and it
 *		  cannot exceed the length defined in CMD_LEN (100). (This should not be an issue,
 *		  as most, if not all, of the command filepaths are under 100 characters, and a
 *		  random string of text will be rejected as an invalid filepath.)
 */
int main(int nargs, char *args[]){

	//define variables
	int frk;
	int ppid;
	int cpid;
	char cmd[CMD_LEN];
	int fd[2];

	//check # of args
	if(nargs > 2){
		//too many arguments; return error
		
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		return 1;
	} else if(nargs < 2){
		//too few arguments; return error
		
		fprintf(stderr, "%s: too few arguments\n", args[0]);
		return 1;
	}

	//check that your command is valid
	if(access(args[1], F_OK) !=0){
		//filepath doesn't exist in the system; return error
		
		fprintf(stderr, "%s: filepath %s does not exist\n", args[0], args[1]);
		return 1;
	}
	if(access(args[1], X_OK) != 0){
		//file may not be executed; return error
		
		fprintf(stderr, "%s: filepath %s may not be executed\n", args[0], args[1]);
		return 1;
	}

	//check that argument is within the allowed length
	if(strlen(args[1]) > CMD_LEN){
		//filepath is too long; return error
		//unlikely to enter this if statement, but it's a sanity check
		
		fprintf(stderr, "filepath is too long\n");
		return 1;
	}
	
	//create pipe
	if(pipe(fd)){
		//pipe failed to create; return error

		fprintf(stderr, "pipe failed\n");
		return 1;
	}

	//start forking	
	frk = fork();
	if(frk < 0) {
		//an error occured, return error and exit
		fprintf(stderr, "Fork failed\n");
		exit(-1);
	}else if(frk == 0) {
		//we are in the child process

		cpid = (int) getpid();
		printf("Child PID: %ld\n", cpid);

		//read from pipe to get info from parent
		close(fd[1]);
		read(fd[0], &cmd, CMD_LEN);
		close(fd[0]);

		//check if cmd is "/bin/echo"
		if(strncmp("/bin/echo", cmd, strlen("/bin/echo")) == 0){
			execl(cmd, cmd, "Hello world!", (char *) NULL);
		} else{
			execl(cmd, cmd, (char *) NULL);
		}
		_exit(1);
	}else {
		//else we are in the parent process

		ppid = (int) getpid();
		printf("Parent PID: %ld\n", ppid);

		//write to pipe
		close(fd[0]);
		write(fd[1], args[1], strlen(args[1]));
		close(fd[1]);
		
		//wait for child to finish, then exit
		wait(NULL);
		exit(0);
	}
	
	return 0;
}
