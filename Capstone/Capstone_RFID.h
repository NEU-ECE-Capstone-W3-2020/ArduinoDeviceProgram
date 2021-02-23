#ifndef RFID_H
#define RFID_H
#include <SoftwareSerial.h> 

#include "SparkFun_UHF_RFID_Reader.h"

#define VERSION_TAG 0x01
#define END_OF_TAG 0xFF
#define WOMENS_BATHROOM 0x11
#define MENS_BATHROOM 0x12
#define UNISEX_BATHROOM 0x13
#define CLASSROOM 0x21
#define OFFICE 0x22
#define FLOOR 0x31
#define ELEVATOR 0x32
#define STAIRCASE 0x33
#define EXIT 0xF1

#define EPC_OFFSET 31

namespace Capstone_RFID {
  boolean setupNano(int rxPin, int txPin, RFID& nano, SoftwareSerial& softSerial, long baudRate, int readPower);
  String tagToString(uint8_t data[], uint8_t size);
}

#endif
