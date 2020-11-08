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
    ioport_toggle_pin_level(IOPORT_LED_TX);

    // Show some sign of life!
    vTaskDelay(200 / portTICK_RATE_MS);

    packet.command = STOP_ALL_MOTORS;
    packet.arg1 = darkSkyContext.allMotorStop;
    SendCommPacket(&packet);

    packet.command = MOTOR_ALT_STATE;
    packet.arg1 = darkSkyContext.motorAlt.state;
    SendCommPacket(&packet);

    packet.command = MOTOR_ALT_POSITION;
    packet.arg1 = darkSkyContext.motorAlt.position;
    SendCommPacket(&packet);

    packet.command = MOTOR_ALT_DELTA_POS;
    packet.arg1 = darkSkyContext.motorAlt.deltaPosition;
    SendCommPacket(&packet);

    packet.command = MOTOR_AZ_STATE;
    packet.arg1 = darkSkyContext.motorAz.state;
    SendCommPacket(&packet);

    packet.command = MOTOR_AZ_POSITION;
    packet.arg1 = darkSkyContext.motorAz.position;
    SendCommPacket(&packet);

    packet.command = MOTOR_AZ_DELTA_POS;
    packet.arg1 = darkSkyContext.motorAz.deltaPosition;
    SendCommPacket(&packet);

    packet.command = LNB_STATE;
    packet.arg1 = darkSkyContext.lnb.power;
    packet.arg2 = darkSkyContext.lnb.carrier;
    SendCommPacket(&packet);
  }
  vTaskDelete(NULL);
}