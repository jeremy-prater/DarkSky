#include "led.hpp"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <stdio.h>

LED green;

void core1_entry() {
  int count = 0;
  while (true) {
    printf("--- tick2 : %d\n", count++);
    sleep_ms(1000);
  }
}

int main() {
  stdio_init_all();

  multicore_launch_core1(core1_entry);

  while (true) {
    green.Toggle();
    sleep_ms(1000);
  }
}
