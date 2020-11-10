#include <asf.h>
#include <string.h>
#include <queue.h>

#include "conf_uart_serial.h"
#include "darksky.h"
#include "darksky_tasks.h"
#include "leds.h"
#include "comm.h"

// Communication static variables
xSemaphoreHandle ApplicationBufferMutex;
uint8_t ApplicationBuffer[COMM_APP_BUFFER_SIZE];
uint32_t ApplicationBufferLevel;
static uint8_t CommRxBuffer[COMM_BUFFER_SIZE];

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

void CommInit(void)
{
  uart_settings.ul_mck = sysclk_get_cpu_hz();

  sysclk_enable_peripheral_clock(ID_USART0);
  sysclk_enable_peripheral_clock(ID_UART);

  darkSkyContext.comm.freertos_uart =
      freertos_uart_serial_init(CONF_UART, &uart_settings, &driver_options);

  configASSERT(darkSkyContext.comm.freertos_uart);

  // context->comm.freertos_usart =
  // freertos_usart_serial_init(CONF_USART, &usart_settings, &driver_options);

  vSemaphoreCreateBinary(darkSkyContext.comm.txMutex);
  vSemaphoreCreateBinary(ApplicationBufferMutex);
  darkSkyContext.comm.txQueue = xQueueCreate(COMM_NUM_PACKETS, sizeof(CommPacket));
}

void SendCommPacketArgs(bool inISR, uint16_t command, uint16_t arg1, uint16_t arg2, uint16_t arg3)
{
  static CommPacket packet;
  packet.header = COMM_PACKET_HEADER;
  packet.command = command;
  packet.arg1 = arg1;
  packet.arg2 = arg2;
  packet.arg3 = arg3;
  return SendCommPacket(inISR, &packet);
}

void SendCommPacket(bool inISR, const CommPacket *packet)
{
  if (inISR)
  {
    // BaseType_t highPriTask;
	  // xQueueSendToBackFromISR(darkSkyContext.comm.txQueue, packet, &highPriTask);
    // if (highPriTask == pdTRUE)
    // {
    //   // Context switch back
    //   taskYIELD();
    // }
  }
  else
  {
    xQueueSendToBack(darkSkyContext.comm.txQueue, packet, portMAX_DELAY);
  }
}

static status_code_t TxCommBlob(const uint8_t *blob, size_t length)
{
  return freertos_uart_write_packet(
      darkSkyContext.comm.freertos_uart, blob, length,
      500 / portTICK_RATE_MS);
}

// Main communication loop

void CommTask(void *data)
{
  CommPacket currentTxPacket;
  CommPacket tempPacket = {.header = COMM_PACKET_HEADER,
                           .command = BOOT};

  SendCommPacket(false, &tempPacket);

  for (;;)
  {
    // Send all outgoing packets!
    xSemaphoreTake(darkSkyContext.comm.txMutex, portMAX_DELAY);

    while (xQueueReceive(darkSkyContext.comm.txQueue, &currentTxPacket, 100 / portTICK_RATE_MS))
    {
      TxCommBlob(&currentTxPacket, sizeof(CommPacket));
    }

    xSemaphoreGive(darkSkyContext.comm.txMutex);

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
        100 / portTICK_RATE_MS);

    if (bytes_received > 0)
    {
      xSemaphoreTake(ApplicationBufferMutex, portMAX_DELAY);
      if (ApplicationBufferLevel + bytes_received < COMM_APP_BUFFER_SIZE)
      {
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