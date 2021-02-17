#ifndef EMIC_H
#define EMIC_H
#include <SoftwareSerial.h> 

namespace Capstone_EMIC {
  void setupEmic(int rxPin, int txPin, int ledPin, SoftwareSerial *emicSerial);
  void sendToEmic(SoftwareSerial *emicSerial, String m);
}

#endif
