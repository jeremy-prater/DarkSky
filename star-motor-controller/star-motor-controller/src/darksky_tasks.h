/*
* darksky_tasks.h
*
* Created: 2/13/2020 10:15:29 PM
*  Author: shady
*/


#ifndef DARKSKY_TASKS_H_
#define DARKSKY_TASKS_H_

#include <portmacro.h>

typedef void (EntryPoint)(void*);

typedef struct
{
	EntryPoint *entryPoint;
	const char * name;
	unsigned short stackSize;
	void * context;
	unsigned portBASE_TYPE priority;
	xTaskHandle taskHandle;
} DarkSkyTask;

// Extern C Task Names

typedef enum
{
	TASK_DarkSkyMain,
	TASK_NUM_TASKS
} DarkSkyTaskIDs;

extern EntryPoint DarkSkyMain;

#endif /* DARKSKY_TASKS_H_ */
