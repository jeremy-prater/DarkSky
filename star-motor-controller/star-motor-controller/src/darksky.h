/*
* darksky.h
*
* Created: 2/13/2020 10:01:04 PM
*  Author: shady
*/

#ifndef DARKSKY_H_
#define DARKSKY_H_

#include <asf.h>
#include <freertos_uart_serial.h>
#include <queue.h>
#include <semphr.h>

#include "darksky_tasks.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define JSON_BUFFER_SIZE 500

typedef enum {
	MOTOR_UNKNOWN,
	MOTOR_ENCODER_A,
	MOTOR_ENCODER_B,
	MOTOR_STALL
} MOTOR_STATE;

typedef struct {
	xSemaphoreHandle stateMutex;
	MOTOR_STATE state;

	xSemaphoreHandle positionMutex;
	uint16_t position; // Range [0, 11000) pulses per revolution
} Motor;

typedef struct {
	//freertos_usart_if freertos_usart;
	freertos_uart_if freertos_uart;
	xSemaphoreHandle txMutex;
} JSONComm;

typedef struct {
	xSemaphoreHandle debugWriteLock;
	// Queue debugLinesOut;
} Debugger;

typedef struct {
	Motor motor1;
	Motor motor2;
	JSONComm jsonComm;
	Debugger debug;
} Context;

extern void DarkSkyMain(void *data);

extern void JSONCommInit(Context *context);
extern status_code_t JSONCommWrite(Context * context, const char *data, size_t length);
extern void JSONCommTask(void *data);

extern Context darkSkyContext;
extern DarkSkyTask darkSkyTasks[TASK_NUM_TASKS];

#endif /* DARKSKY_H_ */
