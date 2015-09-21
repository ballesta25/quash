#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "simpleCmd.h"
#include "builtins.h"

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
	else if (strcmp(cmd->name, CD_STR) == 0)
	{
		if (cmd->args[1] != NULL)
		{
			if (chdir(cmd->args[1]) < 0)
			{
				fprintf(stderr, "%s: No such file or directory\n", cmd->args[1]);
			}
		}
	}
	else if (strcmp(cmd->name, PWD_STR) == 0)
	{
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			printf("%s", cwd);
		}
		else
		{
			fprintf(stderr, "Error getting current working directory: %s\n", cwd);
		}
	}
}
