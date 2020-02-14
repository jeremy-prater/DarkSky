/*
* darksky_hooks.c
*
* Created: 2/13/2020 11:56:29 PM
*  Author: shady
*/

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
 
void vApplicationMallocFailedHook(void) {
	// Oh no... Some memory failed!
}

void vApplicationIdleHook(void) {
	// Change the LED to signal idle condition
}