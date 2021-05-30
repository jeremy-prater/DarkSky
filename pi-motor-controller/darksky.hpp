#pragma once

#include "motor.hpp"

class DarkSky {
public:
  static DarkSky *instance;
  DarkSky();

  Motor altMotor;
  Motor azMotor;
};
