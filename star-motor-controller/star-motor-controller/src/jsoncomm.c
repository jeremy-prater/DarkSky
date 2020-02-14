/*
 * jsoncomm.c
 *
 * Created: 2/14/2020 3:09:19 PM
 *  Author: shady
 */ 

#include <asf.h>
#include "darksky.h"

void JSONComm(void* data)
{
	Context * context = (Context*)data;

	for (;;)
	{
		//printf("test!");
		// Show some sign of life!
	}

	vTaskDelete(NULL);
}