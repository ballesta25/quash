//main.c

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "exec.c"

#define BUF_LEN 2048
#define MAX_JOBS 128

char* jobs[MAX_JOBS];
int numJobs = 0;


char** getTokens(char* input, int* numArgs, int* isBackground) 
{
	char** tokens = NULL;

	char* thisToken = strtok(input, " ");
	int numTokens = 0;
	int stringFinalAmpersand = 0;
	while(thisToken)
	{
		numTokens++;
		tokens = realloc(tokens, numTokens * sizeof(char*));
		int tokenLength = strlen(thisToken);
		stringFinalAmpersand = 0;
		if (tokens == NULL)
		{
			printf("\nUnable to allocate memory during tokenization of input: %s\n", input);
			return tokens;
		}

		if (thisToken[0] == '"' && thisToken[tokenLength-1] != '"')
		{
			thisToken += 1;
			sprintf(thisToken, "%s %s", thisToken, strtok(NULL, "\""));
			tokenLength = strlen(thisToken);
			if (thisToken[tokenLength-1] == '&')
			{
				stringFinalAmpersand = 1;
			}
		}

		tokens[numTokens-1] = thisToken;
		*numArgs = *numArgs + 1;

		thisToken = strtok(NULL, " ");

	}

	tokens = realloc(tokens, (numTokens+1) * sizeof(char*));
	tokens[numTokens] = 0;
	if (!stringFinalAmpersand && tokens[numTokens-1][strlen(tokens[numTokens-1])-1] == '&')
	{
		tokens[numTokens-1][strlen(tokens[numTokens-1])-1] = 0;
		*numArgs = *numArgs - 1;
		*isBackground = 1;
	}

	return tokens;
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
	char jidStr[64];
	sprintf(jidStr, "%d", jid);
	int i = 0;
	for (; i < numJobs; i++)
	{
		if (strncmp(jidStr, (jobs + 1)[i], strlen(jidStr)) == 0)
		{
			//free(jobs[i]);
			strcpy(jobs[i], jobs[numJobs]);
			jobs[numJobs] = 0;
			numJobs--;
		}
	}
}

int main(int argc, char* argv[], char* envp[]) 
{

	char* input;
	char prompt[128];
	int numArgs;



	int isBackground;
	unsigned int nextJobID = 1;
	while(1)
	{
		numArgs = 0;
		isBackground = 0;
		char* user = getenv("USER");
		char* home = getenv("HOME");
		char* path = getenv("PATH");
		char host[64];
		gethostname(host, sizeof(host));
		snprintf(prompt, sizeof(prompt), "[%s@%s]$", user, host);
		input = readline(prompt);
		char** tokens;
		if (*input)
		{
			char* inputcpy;
			inputcpy = malloc(sizeof(char) * (strlen(input) + 1));
			strcpy(inputcpy, input);
			tokens = getTokens(inputcpy, &numArgs, &isBackground);

			if (strcmp(tokens[0],"exit") == 0 || strcmp(tokens[0], "quit") == 0)
			{
				break;
			}
			else
			{
				if (isBackground)
				{
					//TODO: do paperwork!
					int jid = nextJobID++;
					int pid;
					char newJob[BUF_LEN];
					pid = fork();
					if (pid == 0)
					{
						//setsid?
						daemon(1,1);
						sprintf(newJob, "[%d] %d %s", jid, getpid(), inputcpy);
						addJob(newJob);
						printf("[%d] %d\n", jid, getpid());
						fprintf(stderr, "Dummy Output: I'm running the program!\n");
						execTokens(numArgs, tokens);
						fprintf(stderr, "Dummy Output: This is the last of the program's output!\n");
						int tmp_pid;
						while (tmp_pid = waitpid(-1, NULL, 0))
						{
							if (errno == ECHILD)
							{
								break;
							}
						}
						removeJob(jid);
						printf("\n[%d] %d finished %s\n", jid, getpid(), inputcpy);
						return EXIT_SUCCESS;

					}
					else
					{
					}
				}
				else //run in foreground
				{
					execTokens(numArgs, tokens);
					int tmp_pid;
					while (tmp_pid = waitpid(-1, NULL, 0))
					{
						if (errno == ECHILD)
						{
							break;
						}
					}
				}
			}
			add_history(input);
		}
		free(input);
	}
	printf("%s", "bye\n");
}
