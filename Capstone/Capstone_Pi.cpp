#include <Arduino.h>

#include "Capstone_Pi.h"
#include "Capstone_EMIC.h"

namespace Capstone_Pi {
  int parsePacket(char buffer[], int length, SoftwareSerial& emicSerial) {
    if(length < HDR_SIZE) return 0;
    if(length < buffer[LEN_IDX]) return 0;
    int idx = 0;
    switch(buffer[TYPE_IDX]) {
      case TTS_MSG_TYPE:
      {
        String msg;
        idx = HDR_SIZE;
        while(idx < buffer[LEN_IDX]) {
          msg += buffer[idx++];
        }
        Serial.print(F("TTS: "));
        Serial.println(msg);
#ifndef DEBUG
        Capstone_EMIC::sendToEmic(emicSerial, msg);
#endif
        return buffer[LEN_IDX];
      }
      default:
        return -1;
    }
  }
}
