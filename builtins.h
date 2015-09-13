#ifndef BUILTINS_H
#define BUILTINS_H
#include "simpleCmd.h"

int isBuiltin(simpleCmd* cmd);
int executeBuiltin(simpleCmd* cmd);
#endif
