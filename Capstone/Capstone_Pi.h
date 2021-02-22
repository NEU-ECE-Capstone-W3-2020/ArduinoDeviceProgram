#ifndef CAPSTONE_PI_H
#define CAPSTONE_PI_H
#include <SoftwareSerial.h>

#define MAX_SIZE      256
#define HDR_SIZE        2
#define TYPE_IDX        0
#define LEN_IDX         1

#define TTS_MSG_TYPE  0x01

namespace Capstone_Pi {
  int parsePacket(char buffer[], int length, SoftwareSerial& emicSerial);
}

#endif
