/*
 * jsoncomm.c
 *
 * Created: 2/14/2020 3:09:19 PM
 *  Author: shady
 */

#include <asf.h>
#include <freertos_usart_serial.h>
#include <semphr.h>
#include <string.h>

#include "conf_uart_serial.h"
#include "darksky.h"

static uint8_t receive_buffer[JSON_BUFFER_SIZE];
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
    .interrupt_priority = 0x06,
    // The operation_mode value.
    .operation_mode = USART_RS232,
    // Flags set to allow access from multiple tasks.  Note in this case the
    // WAIT_TX_COMPLETE flag is *not* used.
    .options_flags = (USE_TX_ACCESS_SEM | USE_RX_ACCESS_MUTEX)};
static const sam_usart_opt_t usart_settings = {.baudrate = CONF_UART_BAUDRATE,
                                               .char_length = US_MR_CHRL_8_BIT,
                                               .parity_type = US_MR_PAR_NO,
                                               .stop_bits = US_MR_NBSTOP_1_BIT,
                                               .channel_mode =
                                                   US_MR_CHMODE_NORMAL,
                                               .irda_filter = 0};
void JSONCommInit(Context *context) {
  // Declare the variables used as parameters to the
  // freertos_usart_serial_init function.
  // Declare a buffer to be used as the UART receive DMA buffer.  The FreeRTOS
  // peripheral control drivers manage this buffer, and use it as a circular
  // buffer.

  // Handle used to access the initialized port by other FreeRTOS ASF functions.
  // The RS232 configuration.  This structure, and the values used in its
  // setting, are from the standard ASF USART driver.
  // Call the USART specific FreeRTOS ASF driver initialization function,
  // storing the return value as the driver handle.
  context->jsonComm.freertos_usart =
      freertos_usart_serial_init(CONF_UART, &usart_settings, &driver_options);

  vSemaphoreCreateBinary(context->jsonComm.txMutex);
}

void JSONCommTask(void *data) {
  Context *context = (Context *)data;

  const char *output = "I'm a test!";

  for (;;) {
    freertos_usart_write_packet_async(
        context->jsonComm.freertos_usart, (const uint8_t *)output,
        strlen(output) + 1, 5000 / portTICK_RATE_MS, context->jsonComm.txMutex);

    // printf("test!");
    // Show some sign of life!
    vTaskDelay(10000 / portTICK_RATE_MS);
  }

  vTaskDelete(NULL);
}