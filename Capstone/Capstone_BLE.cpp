#include "Capstone_BLE.h"

#define BUFFER_SIZE 32

namespace Capstone_BLE {
    // anonymous namespace
    namespace {
        String decodeValue(Encoded_Strings val) {
            switch(val) {
            case DrMasseyOffice:
                return "Dr. Massey's Office";
            case Room203:
                return "Room 203";
            case ElevatorFloor2:
                return "Elevator Floor 2";
            case CafeStregaClosed:
                return "Cafe Strega, currently closed. Hours 10am to 6pm";
            case CafeStregaOpen:
                return "Welcome to Cafe Strega, open until 6pm. Come grab a pastry!";
            case Calc2:
                return "Calculus 2 - Lupi, ending at 9am ";
            case IsecExit:
                return "Exit to isec quad";
            case IsecWelcome:
                return "Welcome to ISEC";
            default:
                return "Unkown BLE Value";
            }
        }
    }

    String receiveFromBle(SoftwareSerial& bleSerial){
        if(bleSerial.available()){
            uint8_t len = bleSerial.peek();
            if(len >= bleSerial.available()) {
                uint8_t idx = 0;
                uint8_t buffer[BUFFER_SIZE];
                if(len > BUFFER_SIZE) {
                    while(bleSerial.available()) {
                        bleSerial.read(); // clear buffer
                    }
                    return "Bad Length";
                }

                bleSerial.read(); // drop len we already read
                while(idx < len) {
                    buffer[idx] = bleSerial.read();
                    idx++;
                }

                String msg;
                for(int i = 0; i < idx; ++i) {
                    msg += decodeValue((Encoded_Strings) buffer[idx]);
                }
                return msg;
            }
        }
        return "";
    }
}
