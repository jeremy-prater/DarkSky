#ifndef DARKSKY_TASKS_H_
#define DARKSKY_TASKS_H_

#include <portmacro.h>

typedef void (*EntryPoint)(void *);

typedef struct {
  EntryPoint entryPoint;
  const char *name;
  unsigned short stackSize;
  void *context;
  unsigned portBASE_TYPE priority;
  xTaskHandle taskHandle;
} DarkSkyTask;

// Extern C Task Names

typedef enum {
  TASK_DarkSkyMain,
  TASK_Comm,
  TASK_CommandProcessor,
  TASK_NUM_TASKS
} DarkSkyTaskIDs;

extern DarkSkyTask darkSkyTasks[TASK_NUM_TASKS];

#endif /* DARKSKY_TASKS_H_ */
