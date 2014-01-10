#ifndef LANEINDICATORS_H_
#define LANEINDICATORS_H_

#include <Servo.h>

namespace LaneIndicators
{
  void init(uint8_t laneAPin, uint8_t laneBPin);
  void setAll(uint8_t state);
  void setCompare(int compare);
}

#endif
