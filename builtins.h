#ifndef BUILTINS_H
#define BUILTINS_H
#include "simpleCmd.h"
#define NUM_BUILTINS 6
#define SET_STR "set"
#define CD_STR "cd"
#define PWD_STR "pwd"
#define JOBS_STR "jobs"
#define WRITEF_STR "writef"
#define APPENDF_STR "appendf"

int isBuiltin(simpleCmd* cmd);
int executeBuiltin(simpleCmd* cmd);
int addJob(char*);
int removeJob(int);
void printJobs();
void cleanJobs();
int isJobByPid(int);
#define MAX_JOBS 128

#define buffSize 256

char* jobs[MAX_JOBS];
int numJobs;
#endif
