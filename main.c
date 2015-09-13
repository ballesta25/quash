//main.c

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <readline/readline.h>

#define BUF_LEN 2048

char** getTokens(char* input, int* argc) 
{
	char **tokens = NULL;

	char *thisToken = strtok(input, " ");
	int numTokens = 0;

	while(thisToken)
	{
		numTokens++;
		tokens = realloc(tokens, numTokens * sizeof(char*));

		if (tokens == NULL)
		{
			printf("\nUnable to allocate memory during tokenization of input: %s\n", input);
			return tokens;
		}

		if (thisToken[0] == '"')
		{
			thisToken += 1;
			sprintf(thisToken, "%s %s", thisToken, strtok(NULL, "\""));
		}

		tokens[numTokens-1] = thisToken;
		*argc = *argc + 1;

		thisToken = strtok(NULL, " ");

	}

	tokens = realloc(tokens, (numTokens+1) * sizeof(char*));
	tokens[numTokens] = 0;

	return tokens;
}

int main(int argc, char* argv[], char* envp[]) 
{

	char* input;
	char prompt[128];
	int numArgs;
	while(1)
	{
    char* user = getenv("USER");
    char* home = getenv("HOME");
    char* path = getenv("PATH");
    char host[64];
    gethostname(host, sizeof(host));
		snprintf(prompt, sizeof(prompt), "[%s@%s]$", user, host);
		input = readline(prompt);
		if (*input)
		{
			char** tokens = getTokens(input, &numArgs);

			if (strcmp(tokens[0],"exit") == 0 || strcmp(tokens[0], "quit") == 0)
			{
				break;
			}
			else
			{
				//DO THE COMMAND
			}
			printf("Input was:\n");
			int i = 0;
			for (; i < numArgs; i++)
			{
				printf("%s\n", tokens[i]);
			}

		}
		printf("Untokenized input was: %s\n", input);
		return EXIT_SUCCESS;
	}
}
