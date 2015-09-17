/*
 * pipeline.h
 *
 * Data blocks for a simple linked list of simpleCmd objects.
 * 
 */

#ifndef PIPELINE_H
#define PIPELINE_H

#include "simpleCmd.h"

typedef struct pipeline_ 
{
	simpleCmd* command;
	struct pipeline_* next;
} pipeline;

#endif
