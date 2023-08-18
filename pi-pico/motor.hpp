#pragma once

#include <pico/stdlib.h>
#include <string>

#define Q_NOP 0
#define Q_INC 1
#define Q_DEC -1
#define Q_ERR 2

class Motor;

struct MotorPins {
  const std::string name;
  const uint32_t pwm;
  const uint32_t forward;
  const uint32_t reverse;
  const uint32_t quad_a;
  const uint32_t quad_b;
  const uint32_t id;
};

class Motor {
public:
  enum class QuadatureState
  {
    // A_B
    LOW_LOW, //0
    LOW_HIGH, // 1
    HIGH_LOW, // 2
    HIGH_HIGH, // 3
  };

  enum class MotorTypes
  {
    ALT,
    AZ,
    COUNT
  };

  Motor(MotorTypes mtrType);

private:
  static MotorPins motorPins[static_cast<int>(MotorTypes::COUNT)];
  static Motor *motors[static_cast<int>(MotorTypes::COUNT)];
  static const int8_t quadLookup[4][4];

  MotorTypes type;
  const MotorPins *const pins;

  uint16_t position;
  QuadatureState quadState;

  QuadatureState generateQuadratureState() const;

  static void quad_event_irq(uint gpio, uint32_t events);
  void quad_event(uint gpio, uint32_t events);

  void onMove(int delta);
};
