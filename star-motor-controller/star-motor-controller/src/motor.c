#include "asf.h"
#include "darksky.h"
#include <pio.h>
#include <pmc.h>
#include <pwm.h>
#include "motor.h"
#include "comm.h"

static inline void CheckMotorPositionBounds(Motor *motor)
{
  if (motor->position < 0)
  {
    motor->position += MOTOR_POSITION_MAX;
  }
  else if (motor->position >= MOTOR_POSITION_MAX)
  {
    motor->position -= MOTOR_POSITION_MAX;
  }

  motor->deltaPosition--;

  if (motor->deltaPosition < 50)
  {
    MotorSetPWM(true, motor, 50);
  }
  else if (motor->deltaPosition < 100)
  {
    MotorSetPWM(true, motor, 60);
  }
  else if (motor->deltaPosition < 150)
  {
    MotorSetPWM(true, motor, 70);
  }
  else if (motor->deltaPosition < 200)
  {
    MotorSetPWM(true, motor, 80);
  }
  else if (motor->deltaPosition < 250)
  {
    MotorSetPWM(true, motor, 90);
  }

  if (motor->deltaPosition == 0)
  {
    MotorCompleteMove(true, motor);
  }

  if (motor->state != MOTOR_FORWARD && motor->state != MOTOR_REVERSE)
  {
    MotorSetDelta(true, motor, 0);
  }
}

static inline QUADRATURE_STATE GenerateQuadratureState(bool encoder1,
                                                       bool encoder2)
{
  return (QUADRATURE_STATE)((encoder1 ? 0b01 : 0b00) +
                            (encoder2 ? 0b10 : 0b00));
}

static inline int16_t GeneratePositionChange(QUADRATURE_STATE oldState,
                                             QUADRATURE_STATE newState)
{
  if (oldState == newState)
    return 0;

  switch (oldState)
  {
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

static pwm_channel_t GeneratePWMStruct(uint32_t channel)
{
  pwm_channel_t pwmChannel = {
      .channel = channel,
      .ul_prescaler = PWM_CMR_CPRE_CLKA,
      .alignment = PWM_ALIGN_LEFT,
      .polarity = PWM_HIGH,
      .ul_duty = 20,
      .ul_period = 100};
  return pwmChannel;
}

void MotorInit(void)
{
  darkSkyContext.motorAlt.id = 0x0000;
  darkSkyContext.motorAlt.pinForward = PIN_MOTOR_ALT_FORWARD;
  darkSkyContext.motorAlt.pinReverse = PIN_MOTOR_ALT_REVERSE;
  darkSkyContext.motorAlt.pwm = GeneratePWMStruct(PWM_CHANNEL_6);
  darkSkyContext.motorAlt.pinEnc_1 = PIN_MOTOR_ALT_ENC_1;
  darkSkyContext.motorAlt.pinEnc_2 = PIN_MOTOR_ALT_ENC_2;

  darkSkyContext.motorAz.id = 0x0100;
  darkSkyContext.motorAz.pinForward = PIN_MOTOR_AZ_FORWARD;
  darkSkyContext.motorAz.pinReverse = PIN_MOTOR_AZ_REVERSE;
  darkSkyContext.motorAz.pwm = GeneratePWMStruct(PWM_CHANNEL_5);
  darkSkyContext.motorAz.pinEnc_1 = PIN_MOTOR_AZ_ENC_1;
  darkSkyContext.motorAz.pinEnc_2 = PIN_MOTOR_AZ_ENC_2;

  // Enable all ioport pins
  ioport_enable_pin(PIN_MOTOR_ALT_FORWARD);
  ioport_enable_pin(PIN_MOTOR_ALT_REVERSE);
  ioport_enable_pin(PIN_MOTOR_ALT_ENC_1);
  ioport_enable_pin(PIN_MOTOR_ALT_ENC_2);
  ioport_enable_pin(PIN_MOTOR_AZ_FORWARD);
  ioport_enable_pin(PIN_MOTOR_AZ_REVERSE);
  ioport_enable_pin(PIN_MOTOR_AZ_ENC_1);
  ioport_enable_pin(PIN_MOTOR_AZ_ENC_2);

  // Setup motor control output pins
  ioport_set_pin_dir(PIN_MOTOR_ALT_FORWARD, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(PIN_MOTOR_ALT_REVERSE, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(PIN_MOTOR_AZ_FORWARD, IOPORT_DIR_OUTPUT);
  ioport_set_pin_dir(PIN_MOTOR_AZ_REVERSE, IOPORT_DIR_OUTPUT);

  StopAllMotors();

  // Setup motor encoder input pins
  ioport_set_pin_dir(PIN_MOTOR_ALT_ENC_1, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(PIN_MOTOR_ALT_ENC_2, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(PIN_MOTOR_AZ_ENC_1, IOPORT_DIR_INPUT);
  ioport_set_pin_dir(PIN_MOTOR_AZ_ENC_2, IOPORT_DIR_INPUT);

  darkSkyContext.motorAlt.quadratureState =
      GenerateQuadratureState(ioport_get_pin_level(PIN_MOTOR_ALT_ENC_1),
                              ioport_get_pin_level(PIN_MOTOR_ALT_ENC_2));
  darkSkyContext.motorAz.quadratureState =
      GenerateQuadratureState(ioport_get_pin_level(PIN_MOTOR_AZ_ENC_1),
                              ioport_get_pin_level(PIN_MOTOR_AZ_ENC_2));

  // Setup interrupts for Encoder GPIOs
  pmc_enable_periph_clk(ID_PIOC);

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

  // Setup PWM
  gpio_configure_pin(PIN_MOTOR_AZ_PWM, MOTOR_PWM_FLAGS);
  gpio_configure_pin(PIN_MOTOR_ALT_PWM, MOTOR_PWM_FLAGS);

  // Enable PWM clock
  pmc_enable_periph_clk(ID_PWM);

  // Disable all PWM channels
  pwm_channel_disable(PWM, darkSkyContext.motorAlt.pwm.channel);
  pwm_channel_disable(PWM, darkSkyContext.motorAz.pwm.channel);

  // Init PWM clocks
  pwm_clock_t clock_setting = {
      .ul_clka = 1000 * 100,
      .ul_clkb = 0,
      .ul_mck = 48000000};
  pwm_init(PWM, &clock_setting);

  // Init PWM channels
  pwm_channel_init(PWM, &darkSkyContext.motorAlt.pwm);
  pwm_channel_init(PWM, &darkSkyContext.motorAz.pwm);

  // Enable PWM IRQ
  NVIC_EnableIRQ(PWM_IRQn);

  // Enable all PWM channels
  pwm_channel_enable(PWM, darkSkyContext.motorAlt.pwm.channel);
  pwm_channel_enable(PWM, darkSkyContext.motorAz.pwm.channel);
}

void StopAllMotors(void)
{
  // Set all motor drives of off
  MotorStop(false, &darkSkyContext.motorAlt);
  MotorStop(false, &darkSkyContext.motorAz);
}

static void SetMotorState(bool inISR, Motor *motor, uint16_t state)
{
  motor->state = state;
  SendCommPacketArgs(inISR, MOTOR_ALT_STATE + motor->id, state, 0, 0);
}

void MotorStop(bool isISR, Motor *motor)
{
  ioport_set_pin_level(motor->pinForward, IOPORT_PIN_LEVEL_LOW);
  ioport_set_pin_level(motor->pinReverse, IOPORT_PIN_LEVEL_LOW);
  MotorSetPWM(isISR, motor, 0);
  SetMotorState(isISR, motor, MOTOR_STOP);
}

void MotorForward(Motor *motor)
{
  if (darkSkyContext.allMotorStop || motor->deltaPosition == 0)
  {
    return;
  }

  if (motor->state == MOTOR_FORWARD)
  {
    return;
  }

  if (motor->state == MOTOR_REVERSE)
  {
    MotorStop(false, motor);
  }

  MotorSetPWM(false, motor, 100);
  ioport_set_pin_level(motor->pinForward, IOPORT_PIN_LEVEL_HIGH);
  SetMotorState(false, motor, MOTOR_FORWARD);
}

void MotorReverse(Motor *motor)
{
  if (darkSkyContext.allMotorStop || motor->deltaPosition == 0)
  {
    return;
  }

  if (motor->state == MOTOR_REVERSE)
  {
    return;
  }

  if (motor->state == MOTOR_FORWARD)
  {
    MotorStop(false, motor);
  }

  MotorSetPWM(false, motor, 100);
  ioport_set_pin_level(motor->pinReverse, IOPORT_PIN_LEVEL_HIGH);
  SetMotorState(false, motor, MOTOR_REVERSE);
}

void MotorSetDelta(bool inISR, Motor *motor, int16_t deltaPos)
{
  SendCommPacketArgs(inISR, MOTOR_ALT_DELTA_POS + motor->id, deltaPos, 0, 0);
  motor->deltaPosition = deltaPos;
}

void MotorSetPWM(bool inISR, Motor *motor, uint32_t pwmDuty)
{
  SendCommPacketArgs(inISR, MOTOR_ALT_PWM + motor->id, pwmDuty, 0, 0);
  pwm_channel_update_duty(PWM, &motor->pwm, pwmDuty);
}

void MotorCompleteMove(bool inISR, Motor *motor)
{
  MotorStop(inISR, motor);
  MotorSetDelta(inISR, motor, 0);
  SetMotorState(inISR, motor, MOTOR_COMPLETE);
}

void encoder_handler_alt(const uint32_t id, const uint32_t index)
{
  //Assert(id == ID_PIOC);
  //Assert((index == PIO_PC16) || (index == PIO_PC18));

  QUADRATURE_STATE newState =
      GenerateQuadratureState(ioport_get_pin_level(PIN_MOTOR_ALT_ENC_1),
                              ioport_get_pin_level(PIN_MOTOR_ALT_ENC_2));

  darkSkyContext.motorAlt.position +=
      GeneratePositionChange(darkSkyContext.motorAlt.quadratureState, newState);
  CheckMotorPositionBounds(&darkSkyContext.motorAlt);
  darkSkyContext.motorAlt.quadratureState = newState;
}

void encoder_handler_az(const uint32_t id, const uint32_t index)
{
  //Assert(id == ID_PIOC);
  //Assert((index == PIO_PC17) || (index == PIO_PC19));

  QUADRATURE_STATE newState =
      GenerateQuadratureState(ioport_get_pin_level(PIN_MOTOR_AZ_ENC_1),
                              ioport_get_pin_level(PIN_MOTOR_AZ_ENC_2));

  darkSkyContext.motorAz.position +=
      GeneratePositionChange(darkSkyContext.motorAz.quadratureState, newState);
  CheckMotorPositionBounds(&darkSkyContext.motorAz);
  darkSkyContext.motorAz.quadratureState = newState;
}
