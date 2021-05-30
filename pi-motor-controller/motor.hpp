#pragma once

#include <pico/stdlib.h>
#include <string>

struct MotorPins {
  const std::string name;
  const uint32_t pwm;
  const uint32_t forward;
  const uint32_t reverse;
  const uint32_t quad_a;
  const uint32_t quad_b;
};

class Motor {
public:
  static const MotorPins motors[2];
  Motor(const MotorPins *const setup);


private:
  const MotorPins *const pins;

  void quad_a_event(uint gpio, uint32_t events);
  void quad_b_event(uint gpio, uint32_t events);
};
