#ifndef LEDS_H_
#define LEDS_H_

#include <ioport.h>

#define IOPORT_LED_ST IOPORT_CREATE_PIN(PIOB, 27)
#define IOPORT_LED_RX IOPORT_CREATE_PIN(PIOC, 30)
#define IOPORT_LED_TX IOPORT_CREATE_PIN(PIOA, 21)

void LedInit(void);
void FlashErrorLEDs(void);

#endif /* LEDS_H_ */