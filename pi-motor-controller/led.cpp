#include "led.hpp"
#include <stdio.h>

LED::LED(uint32_t gpioPin) : pin(gpioPin), on(false) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
  updateState();
}

void LED::On() noexcept {
  on = true;
  updateState();
}

void LED::Off() noexcept {
  on = false;
  updateState();
}

void LED::Toggle() noexcept {
  on = !on;
  updateState();
}

[[nodiscard]] bool LED::Value() const noexcept { return on; }

void LED::updateState() noexcept {
  printf("LED : %d -> %d\n", pin, on);
  gpio_put(pin, on);
}