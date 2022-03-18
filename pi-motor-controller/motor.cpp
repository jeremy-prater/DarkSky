#include "motor.hpp"
#include "pins.hpp"
#include <functional>

MotorPins Motor::motorPins[static_cast<int>(MotorTypes::COUNT)] = {
    {
        .name = "ALT",
        .pwm = MOTOR_ALT_PWM,
        .forward = MOTOR_ALT_FORWARD,
        .reverse = MOTOR_ALT_REVERSE,
        .quad_a = MOTOR_ALT_QUAD_A,
        .quad_b = MOTOR_ALT_QUAD_B,
        .id = 0,
    },
    {
        .name = "AZ",
        .pwm = MOTOR_AZ_PWM,
        .forward = MOTOR_AZ_FORWARD,
        .reverse = MOTOR_AZ_REVERSE,
        .quad_a = MOTOR_AZ_QUAD_A,
        .quad_b = MOTOR_AZ_QUAD_B,
        .id = 1,
    },
};

Motor *Motor::motors[static_cast<int>(MotorTypes::COUNT)] = {
    nullptr,
    nullptr,
};

const int8_t Motor::quadLookup[4][4] = {
    {Q_NOP, Q_DEC, Q_INC, Q_ERR},
    {Q_INC, Q_NOP, Q_ERR, Q_DEC},
    {Q_DEC, Q_ERR, Q_NOP, Q_INC},
    {Q_ERR, Q_INC, Q_DEC, Q_NOP},
};

Motor::Motor(MotorTypes mtrType)
    : type(mtrType), pins(&motorPins[static_cast<int>(mtrType)]) {
  motors[static_cast<int>(mtrType)] = this;
  gpio_init_mask(1 << pins->pwm || 1 << pins->forward || 1 << pins->reverse ||
                 1 << pins->quad_a || 1 << pins->quad_b);

  gpio_set_dir(pins->pwm, GPIO_OUT);
  gpio_set_function(pins->pwm, GPIO_FUNC_PWM);
  gpio_put(pins->pwm, false);

  gpio_set_dir(pins->forward, GPIO_OUT);
  gpio_put(pins->forward, false);

  gpio_set_dir(pins->reverse, GPIO_OUT);
  gpio_put(pins->reverse, false);

  gpio_set_dir(pins->quad_a, GPIO_IN);
  gpio_set_irq_enabled_with_callback(pins->quad_a,
                                     GPIO_IRQ_EDGE_FALL || GPIO_IRQ_EDGE_RISE,
                                     true, quad_event_irq);

  gpio_set_dir(pins->quad_b, GPIO_IN);
  gpio_set_irq_enabled_with_callback(pins->quad_b,
                                     GPIO_IRQ_EDGE_FALL || GPIO_IRQ_EDGE_RISE,
                                     true, quad_event_irq);

  quadState = generateQuadratureState();
}

Motor::QuadatureState Motor::generateQuadratureState() const {
  return static_cast<QuadatureState>((gpio_get(pins->quad_a) << 1) |
                                     (gpio_get(pins->quad_b)));
}

void Motor::quad_event_irq(uint gpio, uint32_t events) {
  switch (gpio) {
  case MOTOR_ALT_QUAD_A:
  case MOTOR_ALT_QUAD_B:
    motors[static_cast<int>(MotorTypes::ALT)]->quad_event(gpio, events);
    break;

  case MOTOR_AZ_QUAD_A:
  case MOTOR_AZ_QUAD_B:
    motors[static_cast<int>(MotorTypes::AZ)]->quad_event(gpio, events);
    break;

  default:
    // Ohhh, error log
    break;
  }
}

void Motor::quad_event(uint gpio, uint32_t events) {
  QuadatureState newState = generateQuadratureState();
  int8_t delta = quadLookup[static_cast<uint8_t>(quadState)]
                           [static_cast<uint8_t>(newState)];
  quadState = newState;
  if (delta == Q_ERR) {
    // Log a quadrature error!
  } else if (delta != Q_NOP) {
    onMove(delta);
  } else {
    // Maybe log Q_NOP??
  }
}

void Motor::onMove(int delta) { position += delta; }
