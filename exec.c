/* 
 * exec.c
 * 
 * Executes given command.
 * execTokens(...) is intended as the primary interface to the functionality 
 * in this file
 * 
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "simpleCmd.h"
#include "pipeline.h"
#include "builtins.h"


bool isSpecialToken(char* token);
void runPipeline(pipeline* pl, int* pipeIn);
int execSimple(simpleCmd* cmd, int* pipeIn, int* pipeOut);
void freePipeline(pipeline* pl);

void execTokens(int numTokens, char** tokens)
{
	pipeline* pipe = malloc(sizeof(pipeline));
	pipe->next = NULL;
	pipe->command = NULL; 

	pipeline* lastPipe = pipe;

	bool hasSpecial = false;
	int i;
	for(i = 0; i < numTokens; hasSpecial = false)
	{
		simpleCmd* cmd = malloc(sizeof(simpleCmd));
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
		i += j; // advances i to the next token to read
		if (hasSpecial)
		{
			switch(tokens[i][0])
			{

				// deal w/ special token:
				// '|' -> no action (?)
			case '|' :
				i++;
				break;
				// '<' -> convert to cat, move to front
				// '>' -> convert to new builtin (writef)
			case '>' :
				tokens[i] = "writef";
				break;
				// '&' -> ? (deal w/ later? Should only occur at end)
			}			
		}
		cmd->args[j] = NULL;

		pipeline* nextPipe = malloc(sizeof(pipeline));
		nextPipe->command = cmd;
		nextPipe->next = NULL;
		lastPipe->next = nextPipe;
		lastPipe = nextPipe;
	}
	// drop dummy block, pass null for stdin
	runPipeline(pipe->next, NULL);
	freePipeline(pipe);
}

bool isSpecialToken(char* token)
{
	switch(token[0])
	{
	case '|':
	case '<':
	case '>':
		//case '&': ? 
		return true;
	default:
		return false;
	}
}

void runPipeline(pipeline* pl, int* pipeIn)
{
	if(pl == NULL)
	{
		//printf("First Block\n");
		return;
	}
	if(pl->next == NULL)
	{
		//printf("Second Block\n");
		execSimple(pl->command, pipeIn, NULL);
		return;
	}
	//printf("Third 'block'\n");
	int pipeOut[2];
	pipe(pipeOut);
	int pid = execSimple(pl->command, pipeIn, pipeOut);
	runPipeline(pl->next, pipeOut);
	close(pipeOut[0]);
	close(pipeOut[1]);
}

void freePipeline(pipeline* pl)
{
	if(pl->next != NULL)
	{
		freePipeline(pl->next);
	}
	if(pl->command != NULL)
	{
		free(pl->command->args);
		free(pl->command);
	}
	free(pl);
}

int execSimple(simpleCmd* cmd, int* pipeIn, int* pipeOut)
{
	//printf("HERE!\n");
	if (strcmp(cmd->name, CD_STR) == 0)
	{
		executeBuiltin(cmd);
		return getpid();
	}
	else
	{
		int pid = fork();
		if (pid == 0)
		{
			//printf("Inside child!\n");
			if(pipeIn != NULL)
			{
				//printf("STDIN??\n");
				dup2(pipeIn[0], STDIN_FILENO);
				close(pipeIn[0]);
				close(pipeIn[1]);
			}
			if(pipeOut != NULL)
			{
				dup2(pipeOut[1], STDOUT_FILENO);
				close(pipeOut[0]);
				close(pipeOut[1]);
			}
			if(isBuiltin(cmd))
			{
				executeBuiltin(cmd);
			}
			else
			{
				//printf("Doing it...\n");
				// use execvp -- automatically finds based on $PATH
				if (execvp(cmd->name, cmd->args) < 0)
				{
					fprintf(stderr, "Something went wrong!\n");
					if (errno == ENOENT)
					{
						fprintf(stderr, "command not found...\n");
					}
				}
				printf("This is the exec call's return value: %d\n", execv(cmd->name, cmd->args));
				printf("With errno=%d\n",errno);
			}

			exit(0);
		}
		return pid;
	}

};

