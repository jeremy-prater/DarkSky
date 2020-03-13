#include "leds.h"

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(void);

void vApplicationMallocFailedHook(void) {
  // Oh no... Some memory failed!
}

void vApplicationIdleHook(void) {
  // Change the LED to signal idle condition
}

void vApplicationStackOverflowHook(void) {
  // Light Up LEDs for stack overflow
}
