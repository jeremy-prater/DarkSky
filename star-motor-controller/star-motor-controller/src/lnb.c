/*
 * lnb.c
 *
 * Created: 6/21/2020 10:47:32 AM
 *  Author: shady
 */

#include "asf.h"
#include "darksky.h"
#include <pio.h>
#include <pmc.h>

void LNBInit(void)
{
    // Enable lnb carrier ioport pin
    ioport_enable_pin(LNB_CARRIER);
    ioport_enable_pin(LNB_POWER);

    LNBPower(LNB_POWER_OFF);
	LNBCarrier(LNB_CARRIER_OFF);
}

void LNBPower(LNB_POWER_STATE powerState)
{
    darkSkyContext.lnb.power = powerState;
    if (powerState == LNB_POWER_OFF)
    {
        ioport_set_pin_dir(LNB_POWER, IOPORT_DIR_INPUT);
    }
    else
    {
        ioport_set_pin_dir(LNB_POWER, IOPORT_DIR_OUTPUT);
        if (powerState == LNB_POWER_13V)
        {
            ioport_set_pin_level(LNB_POWER, IOPORT_PIN_LEVEL_LOW);
        }
        else if (powerState == LNB_POWER_18V)
        {
            ioport_set_pin_level(LNB_POWER, IOPORT_PIN_LEVEL_HIGH);
        }
    }
}

void LNBCarrier(LNB_CARRIER_STATE carrierState)
{
    darkSkyContext.lnb.carrier = carrierState;
    if (carrierState == LNB_CARRIER_OFF)
    {
        ioport_set_pin_level(LNB_CARRIER, IOPORT_PIN_LEVEL_LOW);
    }
    else if (carrierState == LNB_CARRIER_ON)
    {
        ioport_set_pin_level(LNB_CARRIER, IOPORT_PIN_LEVEL_HIGH);
    }
}
