#include "leds.h"
#include <delay.h>

void LedInit(void)
{
	ioport_set_pin_dir(IOPORT_LED_0, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(IOPORT_LED_1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(IOPORT_LED_2, IOPORT_DIR_OUTPUT);

	ioport_set_pin_level(IOPORT_LED_0, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_1, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_2, IOPORT_PIN_LEVEL_HIGH);
}

void FlashErrorLEDs(void)
{
	ioport_set_pin_level(IOPORT_LED_0, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_1, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_2, IOPORT_PIN_LEVEL_HIGH);

	for(;;)
	{
		ioport_toggle_pin_level(IOPORT_LED_0);
		ioport_toggle_pin_level(IOPORT_LED_1);
		ioport_toggle_pin_level(IOPORT_LED_2);
		delay_ms(100);
	}
}