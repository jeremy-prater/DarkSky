/*
* darksky.h
*
* Created: 2/13/2020 10:01:04 PM
*  Author: shady
*/

#ifndef DARKSKY_H_
#define DARKSKY_H_

#include <asf.h>
#include <semphr.h>
#include <queue.h>
#include "darksky_tasks.h"

#define ARRAY_SIZE(array) \
    (sizeof(array) / sizeof(array[0]))

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
	xSemaphoreHandle debugWriteLock;
	//Queue debugLinesOut;
} Debugger;

typedef struct {
	Motor motor1;
	Motor motor2;
	
	Debugger debug;
} Context;

extern DarkSkyTask darkSkyTasks[];

#endif /* DARKSKY_H_ */
