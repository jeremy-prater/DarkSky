/*
 * darksky_main.c
 *
 * Created: 2/13/2020 10:28:44 PM
 *  Author: shady
 */

#include <asf.h>

#include <FreeRTOSConfig.h>

#include "darksky.h"

DarkSkyTask darkSkyTasks[TASK_NUM_TASKS] = {{// entryPoint
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
                                             NULL},
                                            {// entryPoint
                                             JSONCommTask,
                                             // name
                                             "JSONComm",
                                             // stackSize
                                             configMINIMAL_STACK_SIZE + 100,
                                             // context
                                             &darkSkyContext,
                                             // priority
                                             4,
                                             // taskHandle
                                             NULL}};

void DarkSkyMain(void *data) {
  Context *context = (Context *)data;
  for (;;) {
    // Show some sign of life!
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
  vTaskDelete(NULL);
}