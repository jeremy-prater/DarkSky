#ifndef LNB_H_
#define LNB_H_

#include <ioport.h>

#define LNB_POWER IOPORT_CREATE_PIN(PIOA, 19)    // Pin 42
#define LNB_CARRIER IOPORT_CREATE_PIN(PIOA, 20)  // Pin 43

typedef enum {
  LNB_POWER_OFF,
  LNB_POWER_13V,
  LNB_POWER_18V
} LNB_POWER_STATE;

typedef enum {
  LNB_CARRIER_OFF,
  LNB_CARRIER_ON,
} LNB_CARRIER_STATE;

typedef struct {
  LNB_POWER_STATE power;
  LNB_CARRIER_STATE carrier;
} LNB;

void LNBInit(void);

void LNBPower(LNB_POWER_STATE powerState);
void LNBCarrier(LNB_CARRIER_STATE carrierState);

#endif /* LNB_H_ */