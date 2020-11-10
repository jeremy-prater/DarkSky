#ifndef COMM_H_
#define COMM_H_

#include "asf.h"
#include <freertos_uart_serial.h>
#include <semphr.h>
#include <queue.h>

#define COMM_PACKET_SIZE 16
#define COMM_BUFFER_SIZE 64
#define COMM_APP_BUFFER_SIZE 512

#define COMM_PACKET_HEADER 0xDADAF00D

typedef enum {
  BOOT = 0x0000,
  MOTOR_ALT_STATE = 0x0101,
  MOTOR_ALT_POSITION = 0x0102,
  MOTOR_ALT_DELTA_POS = 0x0103,
  MOTOR_ALT_PWM = 0x0104,
  MOTOR_AZ_STATE = 0x0201,
  MOTOR_AZ_POSITION = 0x0202,
  MOTOR_AZ_DELTA_POS = 0x0203,
  MOTOR_AZ_PWM = 0x0204,
  LNB_STATE = 0x0301,
  STOP_ALL_MOTORS = 0x1001,
  ERROR = 0x2000,
  REBOOT = 0xFFFF
} CommPacketCommand;

typedef struct {
  uint32_t header;
  uint16_t command;
  uint16_t arg1;
  uint16_t arg2;
  uint16_t arg3;
  uint32_t checksum;
} CommPacket; // Communication packet is 16 bytes

typedef struct {
  freertos_uart_if freertos_uart;
  xSemaphoreHandle txMutex;
  QueueHandle_t txQueue;
  StaticQueue_t txQueueInternal;
  CommPacket txQueueBuffer[COMM_BUFFER_SIZE];

} Comm;

void CommInit(void);
void SendCommPacketArgs(bool inISR, uint16_t command, uint16_t arg1, uint16_t arg2, uint16_t arg3);
void SendCommPacket(bool inISR, const CommPacket *packet);
void CommTask(void *data);

// Shared Comm variables
extern xSemaphoreHandle ApplicationBufferMutex;
extern uint8_t ApplicationBuffer[COMM_APP_BUFFER_SIZE];
extern uint32_t ApplicationBufferLevel;

#endif /* COMM_H_ */