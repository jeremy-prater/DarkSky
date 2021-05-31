#include "motor.hpp"
#include "pins.hpp"
#include <functional>

MotorPins Motor::motorPins[NUM_MOTORS] = {
    {
        .name = "ALT",
        .pwm = MOTOR_A_PWM,
        .forward = MOTOR_A_FORWARD,
        .reverse = MOTOR_A_REVERSE,
        .quad_a = MOTOR_A_QUAD_A,
        .quad_b = MOTOR_A_QUAD_B,
        .id = 0,
    },
    {
        .name = "AZ",
        .pwm = MOTOR_B_PWM,
        .forward = MOTOR_B_FORWARD,
        .reverse = MOTOR_B_REVERSE,
        .quad_a = MOTOR_B_QUAD_A,
        .quad_b = MOTOR_B_QUAD_B,
        .id = 1,
    },
};

Motor *Motor::motors[NUM_MOTORS] = {
    nullptr,
    nullptr,
};

const int8_t Motor::quadLookup[4][4] = {
    {Q_NOP, Q_DEC, Q_INC, Q_ERR},
    {Q_INC, Q_NOP, Q_ERR, Q_DEC},
    {Q_DEC, Q_ERR, Q_NOP, Q_INC},
    {Q_ERR, Q_INC, Q_DEC, Q_NOP},
};

Motor::Motor(uint8_t motorID) : pins(&motorPins[motorID]) {
  motors[pins->id] = this;
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
  case MOTOR_A_QUAD_A:
  case MOTOR_A_QUAD_B:
    motors[0]->quad_event(gpio, events);
    break;

  case MOTOR_B_QUAD_A:
  case MOTOR_B_QUAD_B:
    motors[1]->quad_event(gpio, events);
    break;

  default:
    // Ohhh, error log
    break;
  }
}

void Motor::quad_event(uint gpio, uint32_t events) {
    QuadatureState newState = generateQuadratureState();
    int8_t delta = quadLookup[static_cast<uint8_t>(quadState)][static_cast<uint8_t>(newState)];
    quadState = newState;
    if (delta == Q_ERR)
    {
        // Log a quadrature error!
    }
    else if(delta != Q_NOP)
    {
        onMove(delta);
    }
    else
    {
        // Maybe log Q_NOP??
    }

}

void Motor::onMove(int delta) { position += delta; }
