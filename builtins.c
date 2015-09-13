#include "simpleCmd.h"
#include "builtins.h"
#define NUM_BUILTINS 5
#define SET_STR "set"
#define CD_STR "cd"
#define PWD_STR "pwd"
#define JOBS_STR "jobs"
#define WRITEF_STR "writef"

int isBuiltin(simpleCmd* cmd) 
{
	char* builtins[NUM_BUILTINS] = {SET_STR, CD_STR, PWD_STR, JOBS_STR, WRITEF_STR};
	int i = 0;
	for (; i < NUM_BUILTINS; i++)
	{
		if (strcmp(cmd->name, builtins[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int executeBuiltin(simpleCmd* cmd)
{
	if (strcmp(cmd->name, SET_STR) == 0)
	{
		if (cmd->args[1] != NULL)
		{
			char* envVar = strtok(cmd->args[1], "=");
			char* newVal = strtok(NULL, "=");
			if (envVar != NULL && newVal != NULL)
			{
				setenv(envVar, newVal, 1);
				return EXIT_SUCCESS;
			}
			else
			{
				fprintf(stderr, "Error: argument to set command is ill-formatted: %s", cmd->args[1]);
				return EXIT_FAILURE;
			}
		}
	}
}