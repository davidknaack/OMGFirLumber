#ifndef GATE_H_
#define GATE_H_

#include <Servo.h>

namespace Gate
{
  void init(uint8_t servoPin, uint8_t storageBase);
  void setOpenClose(uint8_t open, uint8_t close);
  void open();
  void close();
};

#endif
