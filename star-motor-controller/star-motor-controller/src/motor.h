#ifndef MOTOR_H_
#define MOTOR_H_

#include <ioport.h>
#include <semphr.h>

#define MOTOR_POSITION_MAX 11000

#define MOTOR_AZ_FORWARD IOPORT_CREATE_PIN(PIOC, 3)    // Pin 37
#define MOTOR_AZ_REVERSE IOPORT_CREATE_PIN(PIOC, 5)  // Pin 35
#define MOTOR_AZ_ENC_1 IOPORT_CREATE_PIN(PIOC, 4) // Pin 36
#define MOTOR_AZ_ENC_2 IOPORT_CREATE_PIN(PIOC, 2) // Pin 34

#define MOTOR_ALT_FORWARD IOPORT_CREATE_PIN(PIOC, 9)    // Pin 41
#define MOTOR_ALT_REVERSE IOPORT_CREATE_PIN(PIOC, 7)  // Pin 39
#define MOTOR_ALT_ENC_1 IOPORT_CREATE_PIN(PIOC, 8) // Pin 40
#define MOTOR_ALT_ENC_2 IOPORT_CREATE_PIN(PIOC, 6) // Pin 38

typedef enum {
  MOTOR_STOP,
  MOTOR_FORWARD,
  MOTOR_REVERSE,
  MOTOR_COMPLETE,
  MOTOR_STALL
} MOTOR_STATE;

typedef enum {
  QUAD_LO_LO = 0b00,
  QUAD_LO_HI = 0b01,
  QUAD_HI_HI = 0b11,
  QUAD_HI_LO = 0b10,
} QUADRATURE_STATE;

typedef struct {
  xSemaphoreHandle positionMutex;
  int16_t position; // Range [0, 11000) pulses per revolution
  uint16_t deltaPosition;
  QUADRATURE_STATE quadratureState;

  xSemaphoreHandle stateMutex;
  MOTOR_STATE state;

  ioport_pin_t pinForward;
  ioport_pin_t pinReverse;
  ioport_pin_t pinEnc_1;
  ioport_pin_t pinEnc_2;
} Motor;

void MotorInit(void);
void StopAllMotors(void);

void MotorStop(Motor *motor);
void MotorForward(Motor *motor);
void MotorReverse(Motor *motor);
void MotorSetDelta(Motor *motor, int16_t stopPos);
void MotorCompleteMove(Motor *motor);

// Interrupt declarations
void encoder_handler_alt(const uint32_t id, const uint32_t index);
void encoder_handler_az(const uint32_t id, const uint32_t index);

#endif /* MOTOR_H_ */