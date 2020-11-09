#include <asf.h>
#include <string.h>

#include "conf_uart_serial.h"
#include "darksky.h"
#include "darksky_tasks.h"
#include "leds.h"

// Communication static variables
xSemaphoreHandle ApplicationBufferMutex;
uint8_t ApplicationBuffer[COMM_APP_BUFFER_SIZE];
uint32_t ApplicationBufferLevel;
static bool CommsReady;
static uint8_t CommRxBuffer[COMM_BUFFER_SIZE];
static uint8_t CommTxBuffer[COMM_BUFFER_SIZE];

// Configuration structure.
static const freertos_peripheral_options_t driver_options = {
    // This peripheral has full duplex asynchronous operation, so the
    // receive_buffer value is set to a valid buffer location (declared
    // above).
    .receive_buffer = CommRxBuffer,
    // receive_buffer_size is set to the size, in bytes, of the buffer
    // pointed to by the receive_buffer value.
    .receive_buffer_size = COMM_BUFFER_SIZE,
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
    .options_flags = WAIT_TX_COMPLETE // (USE_TX_ACCESS_SEM | WAIT_TX_COMPLETE |
                                      // USE_RX_ACCESS_MUTEX)
};

// static const sam_usart_opt_t usart_settings = {
//	.baudrate = CONF_USART_BAUDRATE,
//	.char_length = US_MR_CHRL_8_BIT,
//	.parity_type = US_MR_PAR_NO,
//	.stop_bits = US_MR_NBSTOP_1_BIT,
//	.channel_mode = US_MR_CHMODE_NORMAL,
//  .irda_filter = 0};

static sam_uart_opt_t uart_settings = {
    .ul_baudrate = CONF_UART_BAUDRATE, .ul_mck = 0, .ul_mode = 0};

// Communication function implementations

void CommInit(void) {
  uart_settings.ul_mck = sysclk_get_cpu_hz();

  sysclk_enable_peripheral_clock(ID_USART0);
  sysclk_enable_peripheral_clock(ID_UART);

  darkSkyContext.comm.freertos_uart =
      freertos_uart_serial_init(CONF_UART, &uart_settings, &driver_options);

  configASSERT(darkSkyContext.comm.freertos_uart);

  // context->comm.freertos_usart =
  // freertos_usart_serial_init(CONF_USART, &usart_settings, &driver_options);

  vSemaphoreCreateBinary(darkSkyContext.comm.txMutex);
  vSemaphoreCreateBinary(darkSkyContext.comm.txQueueMutex);
  vSemaphoreCreateBinary(ApplicationBufferMutex);
}

void SendCommPacketArgs(uint16_t command, uint16_t arg1, uint16_t arg2, uint16_t arg3) {
  static CommPacket packet;
  packet.header = COMM_PACKET_HEADER;
  packet.command = command;
  packet.arg1 = arg1;
  packet.arg2 = arg2;
  packet.arg3 = arg3;
  return SendCommPacket(&packet);
}


void SendCommPacket(const CommPacket *packet) {
  // I'll probably regret this...
  // xSemaphoreHandle lock = darkSkyContext.comm.txQueueMutex;
  // xSemaphoreTake(lock, portMAX_DELAY);
  if (darkSkyContext.comm.txHead >= COMM_BUFFER_SIZE) {
    darkSkyContext.comm.txHead = 0;
  }
  memcpy (&darkSkyContext.comm.txQueue[darkSkyContext.comm.txHead++], packet, sizeof (CommPacket));
  // darkSkyContext.comm.txHead++;
  // xSemaphoreGive(lock);
}

static status_code_t TxCommBlob(const uint8_t *blob, size_t length) {
  status_code_t result;
  xSemaphoreHandle lock = darkSkyContext.comm.txMutex;

  xSemaphoreTake(lock, portMAX_DELAY);

  result = freertos_uart_write_packet(
      darkSkyContext.comm.freertos_uart, blob, length,
      1000 / portTICK_RATE_MS); // context->comm.txMutex);

  xSemaphoreGive(lock);
  // ioport_toggle_pin_level(IOPORT_LED_TX);

  return result;
}

// Main communication loop

void CommTask(void *data) {
  CommPacket tempPacket = {.header = COMM_PACKET_HEADER,
                           .command = BOOT};

  CommsReady = true;
  SendCommPacket(&tempPacket);

  for (;;) {
    // Send all outgoing packets!
    xSemaphoreHandle lock = darkSkyContext.comm.txQueueMutex;
    xSemaphoreTake(lock, portMAX_DELAY);
    while (darkSkyContext.comm.txTail != darkSkyContext.comm.txHead) {
      TxCommBlob(&darkSkyContext.comm.txQueue[darkSkyContext.comm.txTail++], sizeof (CommPacket));
      if (darkSkyContext.comm.txTail == COMM_BUFFER_SIZE) {
        darkSkyContext.comm.txTail = 0;
      }
    }
    xSemaphoreGive(lock);

    // Attempt to read COMM_BUFFER_SIZE bytes from freertos_uart.
    // If fewer than COMM_BUFFER_SIZE bytes are available,
    // then wait a maximum of 100ms for the rest to arrive.
    //
    // This is a main communication thread loop.
    //
    // All functions here must have memory access protection
    //

    uint32_t bytes_received = freertos_uart_serial_read_packet(
        darkSkyContext.comm.freertos_uart, CommRxBuffer, COMM_PACKET_SIZE,
        100 / portTICK_RATE_MS); // context->comm.txMutex);

    if (bytes_received > 0) {
      xSemaphoreTake(ApplicationBufferMutex, portMAX_DELAY);
      if (ApplicationBufferLevel + bytes_received < COMM_APP_BUFFER_SIZE) {
        memcpy(&ApplicationBuffer[ApplicationBufferLevel], CommRxBuffer,
              bytes_received);
        ApplicationBufferLevel += bytes_received;
        ioport_toggle_pin_level(IOPORT_LED_RX);
        xTaskNotifyGive(darkSkyTasks[TASK_CommandProcessor].taskHandle);
      }
      xSemaphoreGive(ApplicationBufferMutex);
    }
  }

  vTaskDelete(NULL);
}