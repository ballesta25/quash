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
#include "builtins.h"

#define BUF_LEN 2048


char** getTokens(char* input, int* numArgs, int* isBackground) 
{
	char** tokens = NULL;

	char* newInput = malloc(1024 * sizeof(char));
	//strcpy(newInput, input);

	int si = 0;
	int si2 = 0;
	for (; si < strlen(input) - 1; si++, si2++)
	{
		if (input[si + 1] == '|' && input[si] != ' ')
		{
			newInput[si2] = input[si];
			newInput[si2+1] = ' ';
			si2++;
		}
		else if (input[si + 1] != ' ' && input[si] == '|')
		{
			newInput[si2] = '|';
			newInput[si2+1] = ' ';
			si2++;
		}
		else
		{
			newInput[si2] = input[si];
		}
	}
	newInput[si2] = input[si];
	newInput[si2+1] = 0;

	/*if (input[strlen(input)-1] == '&')
	{
		printf("YUP, BACKGROUND\n");
		*isBackground = 1;
		input[strlen(input)-1] = 0;
	}*/
	//printf("input: %s\n", input);
	//printf("newInput: %s\n", newInput);
	input = newInput;
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
		else if (thisToken[0] == '"' && thisToken[tokenLength-1] == '"')
		{
			thisToken[tokenLength-1] = 0;
			thisToken += 1;
			tokenLength = strlen(thisToken);
		}
		if (strncmp(thisToken, "$PATH", 5) == 0)
		{
			char* p = getenv("PATH");
			char* newToken = malloc(sizeof(char) * (strlen(p) + strlen(thisToken) - 4));
			sprintf(newToken, "%s%s", p, thisToken + 5);
			thisToken = newToken;
			tokenLength = strlen(thisToken);
		}
		else if (strncmp(thisToken, "$HOME", 5) == 0)
		{
			char* p = getenv("HOME");
			char* newToken = malloc(sizeof(char) * (strlen(p) + strlen(thisToken) - 4));
			sprintf(newToken, "%s%s", p, thisToken + 5);
			thisToken = newToken;
			tokenLength = strlen(thisToken);
		}

		tokens[numTokens-1] = thisToken;
		//*numArgs = *numArgs + 1;

		thisToken = strtok(NULL, " ");

	}

	tokens = realloc(tokens, (numTokens+1) * sizeof(char*));
	tokens[numTokens] = 0;
	if (!stringFinalAmpersand && tokens[numTokens-1][strlen(tokens[numTokens-1])-1] == '&')
	{
		if (strcmp(tokens[numTokens-1], "&") == 0)
		{
			numTokens--;
		}
		else
		{
			tokens[numTokens-1][strlen(tokens[numTokens-1])-1] = 0;
		}
		*isBackground = 1;
	}
	*numArgs = numTokens;

	/*int i = 0;
	for (; i < numTokens; i++)
	{
		printf("Token %d: %s\n", i, tokens[i]);
	}*/
	return tokens;
}

int main(int argc, char* argv[], char* envp[]) 
{
	char* input;
	char prompt[128];
	int numArgs;
	numJobs = 0;


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
		snprintf(prompt, sizeof(prompt), "[%s@%s;IN QUASH]$", user, host);
		input = readline(prompt);
		cleanJobs();
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
			//else if (strcmp(tokens[0],"jobs") == 0)
			//{
			//	printJobs();
			//}
			else
			{
				if (isBackground)
				{
					int jid = nextJobID++;
					char newJob[BUF_LEN];
					int pid;
					pid = fork();
					if (pid == 0)
					{
						int oldPid = getpid();
						int nestedPid = fork();
						if (nestedPid == 0)
						{
							printf("[%d] %d\n", jid, oldPid);
							execTokens(numArgs, tokens);
							int tmp_pid;
							while (tmp_pid = waitpid(-1, NULL, 0))
							{
								if (errno == ECHILD)
								{
									break;
								}
							}
							printf("\n[%d] %d finished %s\n", jid, oldPid, input);
							return EXIT_SUCCESS;
						}
						else
						{
							int tmp_pid;
							while (tmp_pid = waitpid(-1, NULL, 0))
							{
								if (errno == ECHILD)
								{
									break;
								}
							}
							return EXIT_SUCCESS;
						}

					}
					else
					{
						sprintf(newJob, "[%d] %d %s", jid, pid, input);
						addJob(newJob);
						printJobs();
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
						else if (!isJobByPid(tmp_pid))
						{
							break;
						}
					}
				}
			}
			add_history(input);
			free(inputcpy);
		}
		free(input);
	}
	printf("%s", "bye\n");
}
