/*
* darksky_main.c
*
* Created: 2/13/2020 10:28:44 PM
*  Author: shady
*/

#include <asf.h>
#include "darksky.h"
#include <FreeRTOSConfig.h>

extern void DarkSkyMain(void * data);
extern void JSONComm(void * data);

DarkSkyTask darkSkyTasks[TASK_NUM_TASKS] = {
	{
		//entryPoint
		DarkSkyMain,
		//name
		"Main",
		//stackSize
		400,
		//context
		&darkSkyContext,
		//priority [0,4]
		4,
		//taskHandle
		NULL
	},
	{
		//entryPoint
		JSONComm,
		//name
		"JSONComm",
		//stackSize
		400,
		//context
		&darkSkyContext,
		//priority
		4,
		//taskHandle
		NULL
	}
};

void DarkSkyMain(void *data)
{
	Context * context = (Context*)data;
	for (;;)
	{
		// Show some sign of life!
	}
	vTaskDelete(NULL);
}