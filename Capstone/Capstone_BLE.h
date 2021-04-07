#ifndef CAPSTONE_BLE
#define CAPSTONE_BLE

#include <SoftwareSerial.h> 

namespace Capstone_BLE {
    // See https://docs.google.com/spreadsheets/d/1hY7SFB_dBNCCz2EGmpBoGptssS9gRMkLEKAGmXN6TGU/edit#gid=0
    enum Encoded_Strings {
        DrMasseyOffice   = 0,
        Room203          = 1,
        ElevatorFloor2   = 2,
        CafeStregaClosed = 3,
        CafeStregaOpen   = 4,
        Calc2            = 5,
        IsecExit         = 6,
        IsecWelcome      = 7
    };

    String receiveFromBle(SoftwareSerial& bleSerial);
}

#endif
