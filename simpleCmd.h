#ifndef SIMPLECMD_H
#define SIMPLECMD_H
typedef struct 
{
	char* name;
	char** args; // null terminated, begins w/ copy of name
} simpleCmd;
#endif
