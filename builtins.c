#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "simpleCmd.h"
#include "builtins.h"

int isBuiltin(simpleCmd* cmd) 
{
	char* builtins[NUM_BUILTINS] = {SET_STR, CD_STR, PWD_STR, JOBS_STR, WRITEF_STR, ECHO_STR, APPENDF_STR};
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
				if (newVal[0] == '"' && newVal[strlen(newVal)-1] == '"')
				{
					newVal[strlen(newVal)-1] = 0;
					newVal += 1;
				}
				if (strncmp(newVal, "$PATH", 5) == 0)
				{
					char* p = getenv("PATH");
					char* newToken = malloc(sizeof(char) * (strlen(p) + strlen(newVal) - 4));
					sprintf(newToken, "%s%s", p, newVal + 5);
					newVal = newToken;
				}
				else if (strncmp(newVal, "$HOME", 5) == 0)
				{
					char* p = getenv("HOME");
					char* newToken = malloc(sizeof(char) * (strlen(p) + strlen(newVal) - 4));
					sprintf(newToken, "%s%s", p, newVal + 5);
					newVal = newToken;
				}
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
		else
		{
			if (chdir(getenv("HOME")) < 0)
			{
				fprintf(stderr, "%s: No such file or directory\n", getenv("HOME"));
			}
		}
	}
	else if (strcmp(cmd->name, PWD_STR) == 0)
	{
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			printf("%s\n", cwd);
		}
		else
		{
			fprintf(stderr, "Error getting current working directory: %s\n", cwd);
		}
	}
	else if (strcmp(cmd->name, JOBS_STR) == 0)
	{
		printJobs();
	}
	else if (strcmp(cmd->name, WRITEF_STR) == 0 || strcmp(cmd->name, APPENDF_STR) == 0)
	{
		char* openMode = (strcmp(cmd->name, WRITEF_STR) == 0) ? "w" : "a";
		char* filename = cmd->args[1];
		FILE* file = fopen(filename, openMode);
		if(file == NULL)
		{
			fprintf(stderr, "quash: could not open file: %s\n", filename);
			return EXIT_FAILURE;
		}
		char buffer[buffSize];
		while (fgets(buffer, buffSize, stdin))
		{
			fprintf(file, "%s", buffer);
			fflush(file);
		}
		fclose(file);
	}
	else if (strcmp(cmd->name, ECHO_STR) == 0)
	{
		int i = 1;
		while (cmd->args[i] != NULL)
		{
			printf("%s ", cmd->args[1]);
			i++;
		}
		printf("\n");
	}
	else
	{
		return EXIT_FAILURE;
	}
}

int addJob(char* job)
{
	if (numJobs < MAX_JOBS)
	{
		char* newJob = malloc(sizeof(char) * strlen(job) + 1);
		strcpy(newJob, job);
		jobs[numJobs] = newJob;
		numJobs++;
		return 0;
	}
	else
	{
		fprintf(stderr, "Too many background jobs!\n");
		return -1;
	}
}

int removeJob(int jid)
{
	if (numJobs > 0)
	{
		char jidStr[64];
		sprintf(jidStr, "[%d]", jid);
		int i = 0;
		for (; i < numJobs; i++)
		{
			if (strncmp(jidStr, jobs[i], strlen(jidStr)) == 0)
			{
				numJobs--;
				if (i != numJobs)
				{
					jobs[i] = jobs[numJobs];
				}
				jobs[numJobs] = NULL;
			}
		}
	}
}

int isJobByPid(int pid)
{
	int i = 0;
	for (; i < numJobs; i++)
	{
		char* newJobStr = malloc(sizeof(char) * strlen(jobs[i]) + 1);
		strcpy(newJobStr, jobs[i]);
		char* pidStr = strtok(newJobStr, " ");
		pidStr = strtok(NULL, " ");
		if (atoi(pidStr) == pid)
		{
			return 1;
		}
	}
	return 0;
}

void printJobs()
{
	int i = 0;
	for (; i < numJobs; i++)
	{
		printf("%s\n", jobs[i]);
	}
}

void cleanJobs()
{
	int i = 0;
	for (; i < numJobs; i++)
	{
		char* newJobStr = malloc(sizeof(char) * strlen(jobs[i]) + 1);
		strcpy(newJobStr, jobs[i]);
		char* pidStr = strtok(newJobStr, " ");
		pidStr += 1;
		int theJid = atoi(pidStr);
		pidStr = strtok(NULL, " ");
		int thePid = atoi(pidStr);
		int tmpPid = waitpid(thePid, NULL, WNOHANG);
		int result = kill(thePid, 0);
		if (result != 0)
		{
			removeJob(theJid); 

		}
	}
}
