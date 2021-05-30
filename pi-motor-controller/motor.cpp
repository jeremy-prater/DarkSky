#include "motor.hpp"
#include "pins.hpp"
#include <functional>

const MotorPins Motor::motors[2] = {
    {
        .name = "ALT",
        .pwm = MOTOR_A_PWM,
        .forward = MOTOR_A_FORWARD,
        .reverse = MOTOR_A_REVERSE,
        .quad_a = MOTOR_A_QUAD_A,
        .quad_b = MOTOR_A_QUAD_B,
    },
    {
        .name = "AZ",
        .pwm = MOTOR_B_PWM,
        .forward = MOTOR_B_FORWARD,
        .reverse = MOTOR_B_REVERSE,
        .quad_a = MOTOR_B_QUAD_A,
        .quad_b = MOTOR_B_QUAD_B,
    },
};

Motor::Motor(const MotorPins *const setup) : pins(setup) {
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
  gpio_set_irq_enabled_with_callback (pins->quad_a, 
       GPIO_IRQ_EDGE_FALL || GPIO_IRQ_EDGE_RISE, true, quad_a_event));
  gpio_set_dir(pins->quad_b, GPIO_IN);
}
