/* 
 * exec.c
 * 
 * Executes given command.
 * execTokens(...) is intended as the primary interface to the functionality 
 * in this file
 * 
 */

#include <unistd.h>
#include "simpleCmd.h"

void execTokens(int numTokens, char** tokens)
{
	bool hasSpecial = false;
	int i;
	for(i = 0; i < numTokens; hasSpecial = false)
	{
		simpleCmd* cmd = (simpleCmd*) malloc(sizeof(simpleCmd));
		cmd->name = tokens[i];
		
// allocate enough space for all remaining tokens, plus null terminator
// this will be more than we need in many cases, but it's easy
		cmd->args = (char**) malloc((numTokens - i + 1) * sizeof(char*));
		int j;
		for(j = 0; j < numTokens - i; j++)
		{
			if (isSpecialToken(tokens[i + j]))
			{
				hasSpecial = true;
				break;
			}
			cmd->args[j] = tokens[i + j];
		}
		if (hasSpecial)
		{
			// deal w/ special token:
			// '|' -> no action (?)
			// '<' -> convert to echo, move to front
			// '>' -> convert to new builtin
			// '&' -> ? (deal w/ later? Should only occur at end)
		}
		cmd->args[j] = NULL;
		i += j; // advances i to the next token to read
	}
}


int execSimple(simpleCmd* cmd, int* pipeIn, int* pipeOut)
{
	int pid = fork();
	if (pid = 0)
	{
		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);
		// use execvp ?  -- automatically finds based on $PATH
		execv(cmd->name, cmd.->rgs);

		exit(0);
	}

	return pid;
};

