#include <asf.h>

#include <FreeRTOSConfig.h>

#include "comm.h"
#include "command_processor.h"
#include "darksky.h"
#include "leds.h"
#include <string.h>

DarkSkyTask darkSkyTasks[TASK_NUM_TASKS] = {
    {// entryPoint
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
     NULL},
    {// entryPoint
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
     NULL}};

void DarkSkyMain(void *data) {
  // Context *context = (Context *)data;
  CommPacket packet;
  memset(&packet, 0, sizeof(CommPacket));
  packet.header = COMM_PACKET_HEADER;

  for (;;) {
    // Show some sign of life!
    vTaskDelay(500 / portTICK_RATE_MS);

    packet.command = MOTOR_DEC_STATE;
    packet.arg1 = darkSkyContext.motor1.state;
    SendCommPacket(&packet);

    packet.command = MOTOR_DEC_POSITION;
    packet.arg1 = darkSkyContext.motor1.position;
    SendCommPacket(&packet);

    packet.command = MOTOR_DEC_STOP_POS;
    packet.arg1 = darkSkyContext.motor1.stopPosition;
    SendCommPacket(&packet);

    packet.command = MOTOR_RA_STATE;
    packet.arg1 = darkSkyContext.motor2.state;
    SendCommPacket(&packet);

    packet.command = MOTOR_RA_POSITION;
    packet.arg1 = darkSkyContext.motor2.position;
    SendCommPacket(&packet);

    packet.command = MOTOR_RA_STOP_POS;
    packet.arg1 = darkSkyContext.motor2.stopPosition;
    SendCommPacket(&packet);

    packet.command = LNB_POWER_STATE;
    packet.arg1 = 0;
    SendCommPacket(&packet);
  }
  vTaskDelete(NULL);
}