#ifndef COMM_H_
#define COMM_H_

#include "asf.h"
#include <freertos_uart_serial.h>
#include <semphr.h>

#define COMM_PACKET_SIZE 16
#define COMM_BUFFER_SIZE 64
#define COMM_APP_BUFFER_SIZE 256

#define COMM_PACKET_HEADER 0xDADA

typedef enum {
	SIGNAL_BOOT,
	SIGNAL_MOTOR_DEC_STATE,
	SIGNAL_MOTOR_DEC_POSITION,
	SIGNAL_MOTOR_RA_STATE,
	SIGNAL_MOTOR_RA_POSITION,
	SIGNAL_LNB_POWER_STATE,
	COMMAND_MOTOR_DEC_SET_STATE,
	COMMAND_MOTOR_DEC_SET_POSITION,
	COMMAND_MOTOR_RA_SET_STATE,
	COMMAND_MOTOR_RA_SET_POSITION,
	COMMAND_LNB_SET_POWER_STATE,
	COMMAND_STOP_ALL_MOTORS,
	ERROR
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
} Comm;

void CommInit(void);
status_code_t SendCommPacket(const CommPacket * packet);
status_code_t SendCommBlob(const uint8_t * blob, size_t length);
status_code_t SendCommString(const char * message);
void CommTask(void *data);

// Shared Comm variables
extern xSemaphoreHandle ApplicationBufferMutex;
extern uint8_t ApplicationBuffer[COMM_APP_BUFFER_SIZE];
extern uint32_t ApplicationBufferLevel;

#endif /* COMM_H_ */