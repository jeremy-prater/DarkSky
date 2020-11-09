#include <asf.h>
#include <leds.h>

#include "darksky.h"
#include "leds.h"

Context darkSkyContext;

int main(void) {
  sysclk_init();
  board_init();

  CommInit();
  LNBInit();
  MotorInit();
  LedInit();

  for (uint32_t taskNum = 0; taskNum < (uint32_t)TASK_NUM_TASKS; taskNum++) {
    DarkSkyTask *task = &darkSkyTasks[taskNum];
    xTaskCreate(task->entryPoint, task->name, task->stackSize, task->context,
                task->priority, &task->taskHandle);
  }

  vTaskStartScheduler();

  while (1) {
  }
}
