/*
* star-motor-controller.cpp
*
* Created: 2/17/2020 1:33:31 PM
* Author : shady
*/

#include <asf.h>
#include <leds.h>

#include "darksky.h"

const uint32_t delay = 100;

Context darkSkyContext;

int main(void) {
	/* Initialize the SAM system */
	//SystemInit();
	
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();

	ioport_set_pin_dir(IOPORT_LED_0, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(IOPORT_LED_1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(IOPORT_LED_2, IOPORT_DIR_OUTPUT);

	ioport_set_pin_level(IOPORT_LED_0, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_1, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_2, IOPORT_PIN_LEVEL_HIGH);

	// /* Insert application code here, after the board has been initialized. */
	// while (true)
	// {
	// 	ioport_toggle_pin_level(IOPORT_LED_0);
	// 	delay_ms(delay);
	// 	ioport_toggle_pin_level(IOPORT_LED_1);
	// 	delay_ms(delay);
	// 	ioport_toggle_pin_level(IOPORT_LED_2);
	// 	delay_ms(delay);
	// }

	JSONCommInit(&darkSkyContext);

	for (uint32_t taskNum = 0; taskNum < (uint32_t)TASK_NUM_TASKS; taskNum++) {
		DarkSkyTask *task = &darkSkyTasks[taskNum];
		xTaskCreate(task->entryPoint, task->name, task->stackSize, task->context,
		task->priority, &task->taskHandle);
	}

	vTaskStartScheduler();

	while (1) {
	}
}