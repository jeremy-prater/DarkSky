#ifndef LEDS_H_
#define LEDS_H_

#include <ioport.h>

#define IOPORT_LED_0 IOPORT_CREATE_PIN(PIOB, 27)
#define IOPORT_LED_1 IOPORT_CREATE_PIN(PIOC, 30)
#define IOPORT_LED_2 IOPORT_CREATE_PIN(PIOA, 21)

void LedInit(void);
void FlashErrorLEDs(void);

#endif /* LEDS_H_ */