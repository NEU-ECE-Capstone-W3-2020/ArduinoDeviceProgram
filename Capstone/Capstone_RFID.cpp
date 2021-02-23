#include <Arduino.h>

#include "Capstone_RFID.h"

namespace Capstone_RFID {
  //Gracefully handles a reader that is already configured and already reading continuously
  //Because Stream does not have a .begin() we have to do this outside the library
  boolean setupNano(int rxPin, int txPin, RFID& nano, SoftwareSerial& softSerial, long baudRate, int readPower)
  {
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    nano.begin(softSerial); //Tell the library to communicate over software serial port
  
    //Test to see if we are already connected to a module
    //This would be the case if the Arduino has been reprogrammed and the module has stayed powered
    softSerial.begin(baudRate); //For this test, assume module is already at our desired baud rate
    while (softSerial.isListening()); //Wait for port to open
  
    //About 200ms from power on the module will send its firmware version at 115200. We need to ignore this.
    while (softSerial.available()) softSerial.read();
  
    nano.getVersion();
  
    if (nano.msg[0] == ERROR_WRONG_OPCODE_RESPONSE)
    {
      //This happens if the baud rate is correct but the module is doing a ccontinuous read
      nano.stopReading();
  
      Serial.println(F("Module continuously reading. Asking it to stop..."));
  
      delay(1500);
    }
    else
    {
      //The module did not respond so assume it's just been powered on and communicating at 115200bps
      softSerial.begin(115200); //Start software serial at 115200
  
      nano.setBaud(baudRate); //Tell the module to go to the chosen baud rate. Ignore the response msg
  
      softSerial.begin(baudRate); //Start the software serial port, this time at user's chosen baud rate
  
      delay(250);
    }
  
    //Test the connection
    nano.getVersion();
    if (nano.msg[0] != ALL_GOOD) return false; //Something is not right
  
    //The M6E has these settings no matter what
    nano.setTagProtocol(); //Set protocol to GEN2
  
    nano.setAntennaPort(); //Set TX/RX antenna ports to 1
  
    nano.setRegion(REGION_NORTHAMERICA); //Set to North America
  
    nano.setReadPower(readPower); //5.00 dBm. Higher values may caues USB port to brown out
    //Max Read TX Power is 27.00 dBm and may cause temperature-limit throttling
  
    return true; //We are ready to rock
  }

  String tagToString(uint8_t data[], uint8_t size) {
  if (size == 0) {
    Serial.println("No bytes");
    return "";
  }

  if (data[EPC_OFFSET] != VERSION_TAG) {
    Serial.println("Wrong version");
    return "";
  }
  
  String message = "";
  for (int i = 1; i < size; i++) {
    switch(data[EPC_OFFSET + i]) {
      case END_OF_TAG: 
        return message;
      case WOMENS_BATHROOM:
        message.concat("Women's Bathroom ");
        break;
      case MENS_BATHROOM:
        message.concat("Men's Bathroom ");
        break;     
      case UNISEX_BATHROOM:
        message.concat("Unisex Bathroom ");
        break;
      case FLOOR:
        message.concat("Floor ");
        break;
      case ELEVATOR:
        message.concat("Elevator ");
        break;
      case STAIRCASE:
        message.concat("Staircase ");
        break;
      case CLASSROOM:
        message.concat("Classroom ");
        break;
      case EXIT:
        message.concat("Exit ");
        break;
      case 0x01:
        message.concat("one ");
        break;
      case 0x02:
        message.concat("two ");
        break;
      case 0x03:
        message.concat("three ");
        break;
      case 0x04:
        message.concat("four ");
        break;
      case 0x05:
        message.concat("five ");
        break;
      case 0x06:
        message.concat("size ");
        break;
      default:
        message.concat(data[EPC_OFFSET + i]);
        message.concat(" ");
        break;
    }    
  }
  message.remove(message.length() - 1);
  return message;
}
}
