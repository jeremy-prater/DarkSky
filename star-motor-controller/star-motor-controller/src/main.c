/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <leds.h>

const uint32_t delay = 100;

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	board_init();
	sysclk_init();
	
	ioport_set_pin_dir(IOPORT_LED_0, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(IOPORT_LED_1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(IOPORT_LED_2, IOPORT_DIR_OUTPUT);

	ioport_set_pin_level(IOPORT_LED_0, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_1, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(IOPORT_LED_2, IOPORT_PIN_LEVEL_HIGH);

	/* Insert application code here, after the board has been initialized. */
	while (true)
	{
		ioport_toggle_pin_level(IOPORT_LED_0);
		delay_ms(delay);
		ioport_toggle_pin_level(IOPORT_LED_1);
		delay_ms(delay);
		ioport_toggle_pin_level(IOPORT_LED_2);
		delay_ms(delay);
	}
}
