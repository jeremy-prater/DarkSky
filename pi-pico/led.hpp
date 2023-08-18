#pragma once

#include <pico/stdlib.h>

class LED {
public:
  LED(uint32_t gpioPin = PICO_DEFAULT_LED_PIN);

  void On() noexcept;
  void Off() noexcept;
  void Toggle() noexcept;
  [[nodiscard]] bool Value() const noexcept;

private:
  const uint32_t pin;
  bool on;

  void updateState() noexcept;
};
