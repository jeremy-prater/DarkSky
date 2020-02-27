/*
* jsoncomm.c
*
* Created: 2/14/2020 3:09:19 PM
*  Author: shady
*/

#include <asf.h>
#include <freertos_usart_serial.h>
#include <freertos_uart_serial.h>
#include <semphr.h>
#include <string.h>

#include "conf_uart_serial.h"
#include "darksky.h"
#include "leds.h"

static uint8_t receive_buffer[JSON_BUFFER_SIZE];
static char outBuffer[100];
status_code_t result;

// Configuration structure.
static const freertos_peripheral_options_t driver_options = {
	// This peripheral has full duplex asynchronous operation, so the
	// receive_buffer value is set to a valid buffer location (declared
	// above).
	.receive_buffer = receive_buffer,
	// receive_buffer_size is set to the size, in bytes, of the buffer
	// pointed to by the receive_buffer value.
	.receive_buffer_size = JSON_BUFFER_SIZE,
	// The interrupt priority.  The FreeRTOS driver provides the interrupt
	// service routine, and handles all interrupt interactions.  The
	// application writer does not need to provide any interrupt handling
	// code, but does need to specify the priority of the DMA interrupt here.
	// IMPORTANT!!!  see <a
	// href="http://www.freertos.org/RTOS-Cortex-M3-M4.html">how to set
	// interrupt priorities to work with FreeRTOS</a>
	.interrupt_priority = 0x05,
	// The operation_mode value.
	.operation_mode = UART_RS232,
	// Flags set to allow access from multiple tasks.  Note in this case the
	// WAIT_TX_COMPLETE flag is *not* used.
	.options_flags = WAIT_TX_COMPLETE // (USE_TX_ACCESS_SEM | WAIT_TX_COMPLETE | USE_RX_ACCESS_MUTEX)
};

static const sam_usart_opt_t usart_settings = {
	.baudrate = CONF_USART_BAUDRATE,
	.char_length = US_MR_CHRL_8_BIT,
	.parity_type = US_MR_PAR_NO,
	.stop_bits = US_MR_NBSTOP_1_BIT,
	.channel_mode =
	US_MR_CHMODE_NORMAL,
.irda_filter = 0};

static sam_uart_opt_t uart_settings = {
	.ul_baudrate = CONF_UART_BAUDRATE,
	.ul_mck = 0,
	.ul_mode = 0
};

void JSONCommInit(Context *context) {
	uart_settings.ul_mck = sysclk_get_cpu_hz();

	sysclk_enable_peripheral_clock(ID_USART0);
	sysclk_enable_peripheral_clock(ID_UART);

	context->jsonComm.freertos_uart =
	freertos_uart_serial_init(CONF_UART, &uart_settings, &driver_options);

	configASSERT(context->jsonComm.freertos_uart);

	//context->jsonComm.freertos_usart =
	//freertos_usart_serial_init(CONF_USART, &usart_settings, &driver_options);

	vSemaphoreCreateBinary(context->jsonComm.txMutex);
}

void JSONCommTask(void *data) {
	Context *context = (Context *)data;

	const char *output = "I'm a test!\r\n";
	strcpy (outBuffer, output);

	for (;;) {
		//freertos_usart_write_packet(
		//context->jsonComm.freertos_usart, (const uint8_t *)output,
		//strlen(output) + 1, 100 / portTICK_RATE_MS); //context->jsonComm.txMutex);

		result = freertos_uart_write_packet(
		context->jsonComm.freertos_uart, (const uint8_t *)outBuffer,
		strlen(outBuffer), 1000 / portTICK_RATE_MS); //context->jsonComm.txMutex);


		// printf("test!");
		// Show some sign of life!
		vTaskDelay(500 / portTICK_RATE_MS);
		ioport_toggle_pin_level(IOPORT_LED_1);
	}

	vTaskDelete(NULL);
}