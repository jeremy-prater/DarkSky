#include "asf.h"
#include "darksky.h"
#include <pio.h>
#include <pmc.h>

#define MOTOR_DEC_UP IOPORT_CREATE_PIN(PIOC, 13)    // Pin 50
#define MOTOR_DEC_DOWN IOPORT_CREATE_PIN(PIOC, 15)  // Pin 48
#define MOTOR_DEC_ENC_1 IOPORT_CREATE_PIN(PIOC, 17) // Pin 46
#define MOTOR_DEC_ENC_2 IOPORT_CREATE_PIN(PIOC, 19) // Pin 44

#define MOTOR_RA_LEFT IOPORT_CREATE_PIN(PIOC, 12)  // Pin 51
#define MOTOR_RA_RIGHT IOPORT_CREATE_PIN(PIOC, 14) // Pin 49
#define MOTOR_RA_ENC_1 IOPORT_CREATE_PIN(PIOC, 16) // Pin 47
#define MOTOR_RA_ENC_2 IOPORT_CREATE_PIN(PIOC, 18) // Pin 45

static inline void CheckMotorPositionBounds(Motor *motor) {
  if (motor->position < 0) {
    motor->position += MOTOR_POSITION_MAX;
  } else if (motor->position >= MOTOR_POSITION_MAX) {
    motor->position -= MOTOR_POSITION_MAX;
  }

  if (motor->position == motor->stopPosition)
  {
    MotorStop(motor);
  }
}

static inline QUADRATURE_STATE GenerateQuadratureState(bool encoder1,
                                                       bool encoder2) {
  return (QUADRATURE_STATE)((encoder1 ? 0b01 : 0b00) +
                            (encoder2 ? 0b10 : 0b00));
}

static inline int16_t GeneratePositionChange(QUADRATURE_STATE oldState,
                                             QUADRATURE_STATE newState) {
  if (oldState == newState)
    return 0;

  switch (oldState) {
  case QUAD_LO_LO:
    if (newState == QUAD_LO_HI)
      return -1;
    if (newState == QUAD_HI_LO)
      return 1;
    Assert(0);
    break;

  case QUAD_HI_LO:
    if (newState == QUAD_LO_LO)
      return -1;
    if (newState == QUAD_HI_HI)
      return 1;
    Assert(0);
    break;

  case QUAD_HI_HI:
    if (newState == QUAD_HI_LO)
      return -1;
    if (newState == QUAD_LO_HI)
      return 1;
    Assert(0);
    break;

  case QUAD_LO_HI:
    if (newState == QUAD_HI_HI)
      return -1;
    if (newState == QUAD_LO_LO)
      return 1;
    Assert(0);
    break;
  }

  // Not sure how we could even get here...
  return 0;
}

void MotorInit(void) {
  darkSkyContext.motor1.pinForward = MOTOR_DEC_UP;
  darkSkyContext.motor1.pinReverse = MOTOR_DEC_DOWN;
  darkSkyContext.motor1.pinEnc_1 = MOTOR_DEC_ENC_1;
  darkSkyContext.motor1.pinEnc_2 = MOTOR_DEC_ENC_2;

  darkSkyContext.motor2.pinForward = MOTOR_RA_UP;
  darkSkyContext.motor2.pinReverse = MOTOR_RA_DOWN;
  darkSkyContext.motor2.pinEnc_1 = MOTOR_RA_ENC_1;
  darkSkyContext.motor2.pinEnc_2 = MOTOR_RA_ENC_2;

  // Enable all ioport pins
  ioport_enable_pin(MOTOR_DEC_UP);
  ioport_enable_pin(MOTOR_DEC_DOWN);
  ioport_enable_pin(MOTOR_DEC_ENC_1);
  ioport_enable_pin(MOTOR_DEC_ENC_2);
  ioport_enable_pin(MOTOR_RA_LEFT);
  ioport_enable_pin(MOTOR_RA_RIGHT);
  ioport_enable_pin(MOTOR_RA_ENC_1);
  ioport_enable_pin(MOTOR_RA_ENC_2);

  // Setup motor control output pins
  ioport_set_pin_dir(MOTOR_DEC_UP, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(MOTOR_DEC_DOWN, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(MOTOR_RA_LEFT, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(MOTOR_RA_RIGHT, IOPORT_DIR_OUTPUT);

  StopAllMotors();

  // Setup motor encoder input pins
  ioport_set_pin_dir(MOTOR_DEC_ENC_1, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(MOTOR_DEC_ENC_2, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(MOTOR_RA_ENC_1, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(MOTOR_RA_ENC_2, IOPORT_DIR_INPUT);

  darkSkyContext.motor1.quadratureState =
      GenerateQuadratureState(ioport_get_pin_level(MOTOR_DEC_ENC_1),
                              ioport_get_pin_level(MOTOR_DEC_ENC_2));
  darkSkyContext.motor2.quadratureState =
      GenerateQuadratureState(ioport_get_pin_level(MOTOR_RA_ENC_1),
                              ioport_get_pin_level(MOTOR_RA_ENC_2));

  // Setup interrupts for Encoder GPIOs
  pmc_enable_periph_clk(ID_PIOC);

  // Dec Encoder 1/2
  pio_set_input(PIOC, PIO_PC17, PIO_DEFAULT);
  pio_set_input(PIOC, PIO_PC19, PIO_DEFAULT);

  // RA Encoder 1/2
  pio_set_input(PIOC, PIO_PC16, PIO_DEFAULT);
  pio_set_input(PIOC, PIO_PC18, PIO_DEFAULT);

  pio_handler_set(PIOC, ID_PIOC, PIO_PC17, PIO_IT_EDGE, encoder_handler_dec);
  pio_handler_set(PIOC, ID_PIOC, PIO_PC19, PIO_IT_EDGE, encoder_handler_dec);
  pio_handler_set(PIOC, ID_PIOC, PIO_PC16, PIO_IT_EDGE, encoder_handler_ra);
  pio_handler_set(PIOC, ID_PIOC, PIO_PC18, PIO_IT_EDGE, encoder_handler_ra);

  pio_enable_interrupt(PIOC, PIO_PC17);
  pio_enable_interrupt(PIOC, PIO_PC19);
  pio_enable_interrupt(PIOC, PIO_PC16);
  pio_enable_interrupt(PIOC, PIO_PC18);

  NVIC_EnableIRQ(PIOC_IRQn);
}

void StopAllMotors(void) {
  // Set all motor drives of off
  ioport_set_pin_level(MOTOR_DEC_UP, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(MOTOR_DEC_DOWN, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(MOTOR_RA_LEFT, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(MOTOR_RA_RIGHT, IOPORT_PIN_LEVEL_LOW);

  darkSkyContext.motor1.state = MOTOR_STOP;
  darkSkyContext.motor2.state = MOTOR_STOP;
}

void MotorStop(Motor *motor) {
  motor->state = MOTOR_STOP;
  ioport_set_pin_level(motor->pinForward, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(motor->pinReverse, IOPORT_PIN_LEVEL_LOW);
}

void MotorForward(Motor *motor) {
  if (motor->state == MOTOR_FORWARD) {
    return;
  }

  if (motor->state == MOTOR_REVERSE) {
    MotorStop(motor);
  }

  ioport_set_pin_level(motor->pinForward, IOPORT_PIN_LEVEL_HIGH);
  motor->state = MOTOR_FORWARD;
}

void MotorReverse(Motor *motor) {
  if (motor->state == MOTOR_REVERSE) {
    return;
  }

  if (motor->state == MOTOR_FORWARD) {
    MotorStop(motor);
  }

  ioport_set_pin_level(motor->pinReverse, IOPORT_PIN_LEVEL_HIGH);
  motor->state = MOTOR_REVERSE;
}

void MotorSetStop(Motor *motor, int16_t stopPos)
{
  motor->stopPosition = stopPos;
}

void encoder_handler_dec(const uint32_t id, const uint32_t index) {
  Assert(id == ID_PIOC);
  Assert((index == PIO_PC17) || (index == PIO_PC19));

  QUADRATURE_STATE newState =
      GenerateQuadratureState(pio_get(PIOC, PIO_TYPE_PIO_INPUT, PIO_PC17),
                              pio_get(PIOC, PIO_TYPE_PIO_INPUT, PIO_PC19));

  darkSkyContext.motor1.position +=
      GeneratePositionChange(darkSkyContext.motor1.quadratureState, newState);
  CheckMotorPositionBounds(&darkSkyContext.motor1);
  darkSkyContext.motor1.quadratureState = newState;
}

void encoder_handler_ra(const uint32_t id, const uint32_t index) {
  Assert(id == ID_PIOC);
  Assert((index == PIO_PC16) || (index == PIO_PC18));

  QUADRATURE_STATE newState =
      GenerateQuadratureState(pio_get(PIOC, PIO_TYPE_PIO_INPUT, PIO_PC16),
                              pio_get(PIOC, PIO_TYPE_PIO_INPUT, PIO_PC18));

  darkSkyContext.motor2.position +=
      GeneratePositionChange(darkSkyContext.motor1.quadratureState, newState);
  CheckMotorPositionBounds(&darkSkyContext.motor2);
  darkSkyContext.motor2.quadratureState = newState;
}
