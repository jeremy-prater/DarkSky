#include "command_processor.h"
#include "asf.h"
#include "comm.h"
#include "darksky.h"
#include "leds.h"
#include "motor.h"
#include "comm.h"
#include <portmacro.h>
#include <semphr.h>
#include <string.h>


void CommandProcessorTask(void *data) {
  uint32_t bufferIndex = 0;

  for (;;) {
    uint32_t taskNotificationResult =
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));

    if (taskNotificationResult != 0) {
      xSemaphoreTake(ApplicationBufferMutex, portMAX_DELAY);

      bool foundHeader = true;

      while (foundHeader) {
        CommPacket *packet;
        foundHeader = false;

        for (bufferIndex = 0; bufferIndex < ApplicationBufferLevel;
             ++bufferIndex) {
          packet = (CommPacket *)&ApplicationBuffer[bufferIndex];
          if (packet->header == COMM_PACKET_HEADER) {
            foundHeader = true;
            break;
          }
        }

        if (bufferIndex > 0 ) {
          // Move everything back to 0 since we found a header marker
          // ... unless were already at 0
          memmove(ApplicationBuffer, &ApplicationBuffer[bufferIndex], bufferIndex);
          ApplicationBufferLevel -= bufferIndex;
        }

        packet = (CommPacket *)ApplicationBuffer;


        if (foundHeader) {
          ioport_toggle_pin_level(IOPORT_LED_ST);
          switch (packet->command) {
          // case BOOT:
          // break;
          case MOTOR_AZ_STATE:
            switch (packet->arg1) {
            case MOTOR_FORWARD:
              MotorForward(&darkSkyContext.motor1);
              break;
            case MOTOR_REVERSE:
              MotorReverse(&darkSkyContext.motor1);
              break;
            case MOTOR_STOP:
            default:
              MotorStop(&darkSkyContext.motor1);
            }
            break;

          case MOTOR_AZ_POSITION:
            if (packet->arg1 < MOTOR_POSITION_MAX) {
              darkSkyContext.motor1.position = packet->arg1;
            }
            break;

          case MOTOR_AZ_DELTA_POS:
            MotorSetDelta(&darkSkyContext.motor1, packet->arg1);
            break;

          case MOTOR_ALT_STATE:
            switch (packet->arg1) {
            case MOTOR_FORWARD:
              MotorForward(&darkSkyContext.motor2);
              break;
            case MOTOR_REVERSE:
              MotorReverse(&darkSkyContext.motor2);
              break;
            case MOTOR_STOP:
            default:
              MotorStop(&darkSkyContext.motor2);
            }
            break;

          case MOTOR_ALT_POSITION:
            if (packet->arg1 < MOTOR_POSITION_MAX) {
              darkSkyContext.motor2.position = packet->arg1;
            }
            break;

          case MOTOR_ALT_DELTA_POS:
            MotorSetDelta(&darkSkyContext.motor2, packet->arg1);
            break;


          case STOP_ALL_MOTORS:
            darkSkyContext.allMotorStop = packet->arg1;
            if (darkSkyContext.allMotorStop) {
              MotorStop(&darkSkyContext.motor1);
              MotorStop(&darkSkyContext.motor2);
            }
            break;

          case LNB_STATE:
            LNBPower((LNB_POWER_STATE)packet->arg1);
            LNBCarrier((LNB_CARRIER_STATE)packet->arg2);
            break;

          case ERROR:
            break;
          default:
            break;
          }

          SendCommPacket(packet);

          memmove(ApplicationBuffer, &ApplicationBuffer[sizeof(CommPacket)], sizeof(CommPacket));
          ApplicationBufferLevel -= sizeof(CommPacket);
        }
      }
      xSemaphoreGive(ApplicationBufferMutex);
    }
  }
}