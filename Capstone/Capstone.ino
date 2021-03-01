/*
  Reading multiple RFID tags, simultaneously!
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 3rd, 2016
  https://github.com/sparkfun/Simultaneous_RFID_Tag_Reader

  Constantly reads and outputs any tags heard

  If using the Simultaneous RFID Tag Reader (SRTR) shield, make sure the serial slide
  switch is in the 'SW-UART' position
*/

#include <SoftwareSerial.h> //Used for transmitting to the device

#include "Capstone_EMIC.h"
#include "Capstone_RFID.h"

const int rxPin = 10;  // Serial input (connects to Emic 2's SOUT pin)
const int txPin = 11;  // Serial output (connects to Emic 2's SIN pin)
const int ledPin = 13; // Most Arduino boards have an on-board LED on this pin
const int debugLedPin = 7; //For range testing, debugging w/o serial
const int readPowerRFID = 1000; // 10db. DO NOT EXCEED 20db

SoftwareSerial softSerial(2, 3); //RX, TX
SoftwareSerial emicSerial(10, 11);
RFID nano;
String tagMessage; 
static bool DEBUG = false;

void setup()
{
  debug_init();
  pinMode(debugLedPin, OUTPUT);
  tagMessage = "";

  
  Capstone_EMIC::setupEmic(rxPin, txPin, ledPin, emicSerial); 
  if (Capstone_RFID::setupNano(nano, softSerial, 38400, readPowerRFID) == false) //Configure nano to run at 38400bps
  {
    debug_print("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }

  nano.startReading(); //Begin scanning for tags
}

void loop()
{
  if (nano.check() == true) //Check to see if any new data has come in from module
  {
    byte responseType = nano.parseResponse(); //Break response into tag ID, RSSI, frequency, and timestamp

    if (responseType == RESPONSE_IS_TAGFOUND)
    {
      digitalWrite(debugLedPin, HIGH);
      String msg = Capstone_RFID::tagToString(nano.msg, nano.getTagEPCBytes()); 
      debug_print(msg);
      
      if (!msg.equals(tagMessage)) {
        msgToEmic(msg);
        tagMessage = msg; 
      }
    } else {
      debug_print("Error code: " + responseType);
      digitalWrite(debugLedPin, LOW);
    }
  }
}

void msgToEmic(String msg) {
  digitalWrite(ledPin, HIGH);
  Capstone_EMIC::sendToEmic(emicSerial, msg);
  digitalWrite(ledPin, LOW);
  debug_print("Sent to Emic: " + msg);
}

void debug_print(String msg) 
{
  if(DEBUG) {
    Serial.println(msg);
  }
  
}

void debug_init() {
  if(DEBUG) {
    Serial.begin(9600);
    while (!Serial); //Wait for the serial port to come online
    debug_print("Debug print line ON");
  }
}
