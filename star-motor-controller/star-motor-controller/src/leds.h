/*
 * leds.h
 *
 * Created: 2/4/2020 11:36:20 PM
 *  Author: shady
 */ 


#ifndef LEDS_H_
#define LEDS_H_

#include <ioport.h>

#define IOPORT_LED_0 IOPORT_CREATE_PIN(PIOB, 27)
#define IOPORT_LED_1 IOPORT_CREATE_PIN(PIOC, 30)
#define IOPORT_LED_2 IOPORT_CREATE_PIN(PIOA, 21)

#endif /* LEDS_H_ */