/*
* jsoncomm.c
*
* Created: 2/14/2020 3:09:19 PM
*  Author: shady
*/

#include <asf.h>
#include <freertos_uart_serial.h>
#include <semphr.h>
#include <string.h>

#include "conf_uart_serial.h"
#include "darksky.h"
#include "leds.h"

static uint8_t jsonDMABuffer[JSON_BUFFER_SIZE];
static char jsonDataBuffer[JSON_BUFFER_SIZE];

// Configuration structure.
static const freertos_peripheral_options_t driver_options = {
	.receive_buffer = jsonDMABuffer,
	.receive_buffer_size = JSON_BUFFER_SIZE,
	.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY,
	.operation_mode = UART_RS232,
	.options_flags =
(USE_TX_ACCESS_SEM | WAIT_TX_COMPLETE | USE_RX_ACCESS_MUTEX)};

static sam_uart_opt_t uart_settings = {
.ul_baudrate = CONF_UART_BAUDRATE, .ul_mck = 0, .ul_mode = 0};

void JSONCommInit(Context *context) {
	uart_settings.ul_mck = sysclk_get_cpu_hz();

	sysclk_enable_peripheral_clock(ID_USART0);
	sysclk_enable_peripheral_clock(ID_UART);

	context->jsonComm.freertos_uart =
	freertos_uart_serial_init(CONF_UART, &uart_settings, &driver_options);

	configASSERT(context->jsonComm.freertos_uart);

	// This is for using the USART
	// context->jsonComm.freertos_usart =
	// freertos_usart_serial_init(CONF_USART, &usart_settings, &driver_options);

	vSemaphoreCreateBinary(context->jsonComm.txMutex);
}

status_code_t JSONCommWrite(Context *context, const char *data, size_t length) {
	return freertos_uart_write_packet_async(
	context->jsonComm.freertos_uart, (const uint8_t *)data, length,
	100 / portTICK_RATE_MS, context->jsonComm.txMutex);
}

void JSONCommTask(void *data) {
	Context *context = (Context *)data;

	char jsonByte;
	char output[50];
	for (;;) {
		jsonByte = 0;
		status_code_t result = freertos_uart_serial_read_packet(
		context->jsonComm.freertos_uart, &jsonByte, 1, 100 / portTICK_RATE_MS);
		if (result == STATUS_OK) { // && jsonByte != 0x00) {
			sprintf(output, "--> Data in [%d] : %d\r\n", result, jsonByte);
			JSONCommWrite(context, output, strlen(output));
			ioport_toggle_pin_level(IOPORT_LED_1);
		}
	}

	vTaskDelete(NULL);
}