#include "darksky.hpp"
#include "led.hpp"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <stdio.h>

DarkSky *DarkSky::instance;

LED green;

void core1_entry() {
  DarkSky::instance = new DarkSky();
}

int main() {
  stdio_init_all();

  while (true) {
    printf("---> 'ELLO!' : \n");
    sleep_ms(1000);
  }

  multicore_launch_core1(core1_entry);
}
