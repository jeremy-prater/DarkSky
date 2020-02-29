#ifndef MOTOR_H_
#define MOTOR_H_

#include <ioport.h>
#include <semphr.h>

#define MOTOR_DEC_UP    IOPORT_CREATE_PIN(PIOC, 13) // Pin 50
#define MOTOR_DEC_DOWN  IOPORT_CREATE_PIN(PIOC, 15) // Pin 48
#define MOTOR_DEC_ENC_1 IOPORT_CREATE_PIN(PIOC, 17) // Pin 46
#define MOTOR_DEC_ENC_2 IOPORT_CREATE_PIN(PIOC, 19) // Pin 44

#define MOTOR_RA_UP     IOPORT_CREATE_PIN(PIOC, 12) // Pin 51
#define MOTOR_RA_DOWN   IOPORT_CREATE_PIN(PIOC, 14) // Pin 49
#define MOTOR_RA_ENC_1  IOPORT_CREATE_PIN(PIOC, 16) // Pin 47
#define MOTOR_RA_ENC_2  IOPORT_CREATE_PIN(PIOC, 18) // Pin 45

typedef enum {
	MOTOR_STOP,
	MOTOR_FORWARD,
	MOTOR_REVERSE,
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
	uint16_t position; // Range [0, 11000) pulses per revolution
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

void MotorStop(Motor * motor);
void MotorForward(Motor * motor);
void MotorReverse(Motor * motor);

// Interrupt declarations
void encoder_handler_dec(const uint32_t id, const uint32_t index);
void encoder_handler_ra(const uint32_t id, const uint32_t index);

#endif /* MOTOR_H_ */