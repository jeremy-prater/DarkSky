#include "asf.h"
#include "darksky.h"
#include <pio.h>
#include <pmc.h>
#include <pwm.h>
#include "motor.h"

static inline void CheckMotorPositionBounds(Motor *motor) {
  if (motor->position < 0) {
    motor->position += MOTOR_POSITION_MAX;
  } else if (motor->position >= MOTOR_POSITION_MAX) {
    motor->position -= MOTOR_POSITION_MAX;
  }

  motor->deltaPosition--;
  
  if (motor->deltaPosition == 0)
  {
    MotorCompleteMove(motor);
  }

  if (motor->state != MOTOR_FORWARD && motor->state != MOTOR_REVERSE)
  {
    MotorSetDelta(motor, 0);
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
  darkSkyContext.motorAlt.pinForward = MOTOR_ALT_FORWARD;
  darkSkyContext.motorAlt.pinReverse = MOTOR_ALT_REVERSE;
  darkSkyContext.motorAlt.pinEnc_1 = MOTOR_ALT_ENC_1;
  darkSkyContext.motorAlt.pinEnc_2 = MOTOR_ALT_ENC_2;

  darkSkyContext.motorAz.pinForward = MOTOR_AZ_FORWARD;
  darkSkyContext.motorAz.pinReverse = MOTOR_AZ_REVERSE;
  darkSkyContext.motorAz.pinEnc_1 = MOTOR_AZ_ENC_1;
  darkSkyContext.motorAz.pinEnc_2 = MOTOR_AZ_ENC_2;

  // Enable all ioport pins
  ioport_enable_pin(MOTOR_ALT_FORWARD);
  ioport_enable_pin(MOTOR_ALT_REVERSE);
  ioport_enable_pin(MOTOR_ALT_ENC_1);
  ioport_enable_pin(MOTOR_ALT_ENC_2);
  ioport_enable_pin(MOTOR_AZ_FORWARD);
  ioport_enable_pin(MOTOR_AZ_REVERSE);
  ioport_enable_pin(MOTOR_AZ_ENC_1);
  ioport_enable_pin(MOTOR_AZ_ENC_2);

  // Setup motor control output pins
  ioport_set_pin_dir(MOTOR_ALT_FORWARD, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(MOTOR_ALT_REVERSE, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(MOTOR_AZ_FORWARD, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(MOTOR_AZ_REVERSE, IOPORT_DIR_OUTPUT);

  StopAllMotors();

  // Setup motor encoder input pins
  ioport_set_pin_dir(MOTOR_ALT_ENC_1, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(MOTOR_ALT_ENC_2, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(MOTOR_AZ_ENC_1, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(MOTOR_AZ_ENC_2, IOPORT_DIR_INPUT);

  darkSkyContext.motorAlt.quadratureState =
      GenerateQuadratureState(ioport_get_pin_level(MOTOR_ALT_ENC_1),
                              ioport_get_pin_level(MOTOR_ALT_ENC_2));
  darkSkyContext.motorAz.quadratureState =
      GenerateQuadratureState(ioport_get_pin_level(MOTOR_AZ_ENC_1),
                              ioport_get_pin_level(MOTOR_AZ_ENC_2));

  // Setup interrupts for Encoder GPIOs
  pmc_enable_periph_clk(ID_PIOC);
  
  // Enable PWM
  pmc_enable_periph_clk(ID_PWM);

  // Az Encoder 1/2
  pio_set_input(PIOC, PIO_PC2, PIO_DEFAULT);
  pio_set_input(PIOC, PIO_PC4, PIO_DEFAULT);

  // Alt Encoder 1/2
  pio_set_input(PIOC, PIO_PC6, PIO_DEFAULT);
  pio_set_input(PIOC, PIO_PC8, PIO_DEFAULT);

  pio_handler_set(PIOC, ID_PIOC, PIO_PC2, PIO_IT_EDGE, encoder_handler_az);
  pio_handler_set(PIOC, ID_PIOC, PIO_PC4, PIO_IT_EDGE, encoder_handler_az);
  pio_handler_set(PIOC, ID_PIOC, PIO_PC6, PIO_IT_EDGE, encoder_handler_alt);
  pio_handler_set(PIOC, ID_PIOC, PIO_PC8, PIO_IT_EDGE, encoder_handler_alt);

  pio_enable_interrupt(PIOC, PIO_PC2);
  pio_enable_interrupt(PIOC, PIO_PC4);
  pio_enable_interrupt(PIOC, PIO_PC6);
  pio_enable_interrupt(PIOC, PIO_PC8);

  NVIC_EnableIRQ(PIOC_IRQn);
}

void StopAllMotors(void) {
  // Set all motor drives of off
  ioport_set_pin_level(MOTOR_ALT_FORWARD, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(MOTOR_ALT_REVERSE, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(MOTOR_AZ_FORWARD, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(MOTOR_AZ_REVERSE, IOPORT_PIN_LEVEL_LOW);

  darkSkyContext.motorAlt.state = MOTOR_STOP;
  darkSkyContext.motorAz.state = MOTOR_STOP;
}

void MotorStop(Motor *motor) {
  motor->state = MOTOR_STOP;
  ioport_set_pin_level(motor->pinForward, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(motor->pinReverse, IOPORT_PIN_LEVEL_LOW);
}

void MotorForward(Motor *motor) {
  if (darkSkyContext.allMotorStop || motor->deltaPosition == 0) {
    MotorStop(motor);
    return;
  }

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
  if (darkSkyContext.allMotorStop || motor->deltaPosition == 0) {
    MotorStop(motor);
    return;
  }

  if (motor->state == MOTOR_REVERSE) {
    return;
  }

  if (motor->state == MOTOR_FORWARD) {
    MotorStop(motor);
  }

  ioport_set_pin_level(motor->pinReverse, IOPORT_PIN_LEVEL_HIGH);
  motor->state = MOTOR_REVERSE;
}

void MotorSetDelta(Motor *motor, int16_t deltaPos)
{
  motor->deltaPosition = deltaPos;
}

void MotorCompleteMove(Motor *motor) {
  MotorStop(motor);  
  MotorSetDelta(motor, 0);
  motor->state = MOTOR_COMPLETE;
}


void encoder_handler_alt(const uint32_t id, const uint32_t index) {
  //Assert(id == ID_PIOC);
  //Assert((index == PIO_PC16) || (index == PIO_PC18));

  QUADRATURE_STATE newState =
      GenerateQuadratureState(ioport_get_pin_level(MOTOR_ALT_ENC_1),
                              ioport_get_pin_level(MOTOR_ALT_ENC_2));

  darkSkyContext.motorAlt.position +=
      GeneratePositionChange(darkSkyContext.motorAlt.quadratureState, newState);
  CheckMotorPositionBounds(&darkSkyContext.motorAlt);
  darkSkyContext.motorAlt.quadratureState = newState;
}

void encoder_handler_az(const uint32_t id, const uint32_t index) {
  //Assert(id == ID_PIOC);
  //Assert((index == PIO_PC17) || (index == PIO_PC19));

  QUADRATURE_STATE newState =
      GenerateQuadratureState(ioport_get_pin_level(MOTOR_AZ_ENC_1),
                              ioport_get_pin_level(MOTOR_AZ_ENC_2));

  darkSkyContext.motorAz.position +=
      GeneratePositionChange(darkSkyContext.motorAz.quadratureState, newState);
  CheckMotorPositionBounds(&darkSkyContext.motorAz);
  darkSkyContext.motorAz.quadratureState = newState;
}
