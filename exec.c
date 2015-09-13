/* 
 * exec.c
 * 
 * Executes given command.
 * 
 * 
 */

#include <unistd.h>

typedef struct 
{
	char* name
	char** args // null terminated
} simpleCmd;


void execString(char* cmdString)
{
	
}

void execSimple(simpleCmd cmd, int* pipeIn, int* pipeOut)
{
	dup2(pipeIn[0], STDIN_FILENO);
	dup2(pipeOut[1], STDOUT_FILENO);

	
}

char** tokenize(char* cmdString)
{
	
}

