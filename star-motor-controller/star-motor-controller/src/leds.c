#include "leds.h"
#include <delay.h>

void LedInit(void) {
  ioport_set_pin_dir(IOPORT_LED_ST, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(IOPORT_LED_RX, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(IOPORT_LED_TX, IOPORT_DIR_OUTPUT);

  ioport_set_pin_level(IOPORT_LED_ST, IOPORT_PIN_LEVEL_HIGH);
  ioport_set_pin_level(IOPORT_LED_RX, IOPORT_PIN_LEVEL_HIGH);
  ioport_set_pin_level(IOPORT_LED_TX, IOPORT_PIN_LEVEL_HIGH);
}

void FlashErrorLEDs(void) {
  ioport_set_pin_level(IOPORT_LED_ST, IOPORT_PIN_LEVEL_HIGH);
  ioport_set_pin_level(IOPORT_LED_RX, IOPORT_PIN_LEVEL_HIGH);
  ioport_set_pin_level(IOPORT_LED_TX, IOPORT_PIN_LEVEL_HIGH);

  for (;;) {
    ioport_toggle_pin_level(IOPORT_LED_ST);
    ioport_toggle_pin_level(IOPORT_LED_RX);
    ioport_toggle_pin_level(IOPORT_LED_TX);
    delay_ms(100);
  }
}