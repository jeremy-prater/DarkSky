/*
* darksky_main.c
*
* Created: 2/13/2020 10:28:44 PM
*  Author: shady
*/

#include <asf.h>
#include "darksky.h"
#include <FreeRTOSConfig.h>

DarkSkyTask darkSkyTasks[] = {
	{
		//entryPoint
		DarkSkyMain,
		//name
		"Main Task",
		//stackSize
		100,
		//context
		DarkSkyMain,
		//priority
		configLIBRARY_LOWEST_INTERRUPT_PRIORITY,
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