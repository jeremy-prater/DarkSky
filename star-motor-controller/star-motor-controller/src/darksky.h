#ifndef DARKSKY_H_
#define DARKSKY_H_

#include <asf.h>
#include <freertos_usart_serial.h>
#include <queue.h>
#include <semphr.h>

#include "darksky_tasks.h"
#include "comm.h"
#include "motor.h"

//#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
	Motor motor1;
	Motor motor2;
	Comm comm;
} Context;

void DarkSkyMain(void *data);

extern Context darkSkyContext;
extern DarkSkyTask darkSkyTasks[TASK_NUM_TASKS];

#endif /* DARKSKY_H_ */
