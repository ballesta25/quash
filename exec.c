/* 
 * exec.c
 * 
 * Executes given command.
 * execString() parses and executes a string. It is intended as the primary 
 * interface to the functionality in this file
 */

#include <unistd.h>

typedef struct 
{
	char* name;
	char** args; // null terminated, begins w/ copy of name
} simpleCmd;


void execString(char* cmdString)
{
	char** tokens = tokenize(cmdString);
	
};

int execSimple(simpleCmd cmd, int* pipeIn, int* pipeOut)
{
	int pid = fork();
	if (pid = 0)
	{
		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);
		// use execvpe ?  -- automatically finds based on $PATH in passed-in environment
		execv(cmd.name, cmd.args);

		exit(0);
	}

	return pid;
};

char** tokenize(char* cmdString)
{
	int len =  strlen(cmdString);

	for(int i = 0; i < len; i++)
	{
		// ...
	}
};

