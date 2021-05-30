#include "darksky.hpp"

DarkSky::DarkSky() : altMotor(&Motor::motors[0]), azMotor(&Motor::motors[1]) {}