#include <asf.h>

#include <FreeRTOSConfig.h>

#include "darksky.h"
#include "leds.h"
#include "comm.h"
#include "command_processor.h"

DarkSkyTask darkSkyTasks[TASK_NUM_TASKS] =
{
	{
		// entryPoint
		DarkSkyMain,
		// name
		"Main",
		// stackSize
		configMINIMAL_STACK_SIZE + 100,
		// context
		&darkSkyContext,
		// priority [0,4]
		4,
		// taskHandle
		NULL
	}, {
		// entryPoint
		CommTask,
		// name
		"Comm",
		// stackSize
		configMINIMAL_STACK_SIZE + COMM_APP_BUFFER_SIZE + COMM_BUFFER_SIZE + 30,
		// context
		&darkSkyContext,
		// priority
		4,
		// taskHandle
		NULL
	}, {
		// entryPoint
		CommandProcessorTask,
		// name
		"Command",
		// stackSize
		configMINIMAL_STACK_SIZE,
		// context
		&darkSkyContext,
		// priority
		4,
		// taskHandle
		NULL
	}
};

void DarkSkyMain(void *data) {
	// Context *context = (Context *)data;
	char outBuffer[64];

	for (;;) {
		// Show some sign of life!
		//vTaskDelay(250 / portTICK_RATE_MS);
		ioport_toggle_pin_level(IOPORT_LED_0);

		snprintf(outBuffer, 64, "dec_pos:%d", darkSkyContext.motor1.position);
		SendCommString(outBuffer);
		snprintf(outBuffer, 64, "dec_state:%d", darkSkyContext.motor1.state);
		SendCommString(outBuffer);
		snprintf(outBuffer, 64, "ra_pos:%d", darkSkyContext.motor2.position);
		SendCommString(outBuffer);
		snprintf(outBuffer, 64, "ra_state:%d", darkSkyContext.motor2.state);
		SendCommString(outBuffer);
	}
	vTaskDelete(NULL);
}