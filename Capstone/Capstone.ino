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

#include "Capstone_Pi.h"
#include "Capstone_EMIC.h"
#include "Capstone_RFID.h"

#define BUFFER_SIZE    256

const int rxPin = 10;  // Serial input (connects to Emic 2's SOUT pin)
const int txPin = 11;  // Serial output (connects to Emic 2's SIN pin)
const int ledPin = 13; // Most Arduino boards have an on-board LED on this pin
const int debugLedPin = 10; //For range testing, debugging w/o serial
const int readPowerRFID = 1000; // 10db. DO NOT EXCEED 20db

SoftwareSerial softSerial(2, 3); //RX, TX
SoftwareSerial emicSerial(rxPin, txPin);
SoftwareSerial piSerial(8, 9);
RFID nano;
String tagMessage; 
char buffer[BUFFER_SIZE];
int buf_idx;

void setup()
{
  buf_idx = 0;
  pinMode(debugLedPin, OUTPUT);
  tagMessage = "";
  Serial.begin(9600);
  while (!Serial); //Wait for the serial port to come online

  pinMode(8, INPUT);
  pinMode(9, OUTPUT);
  piSerial.begin(9600);

  Capstone_EMIC::setupEmic(rxPin, txPin, ledPin, emicSerial); 
  if (Capstone_RFID::setupNano(nano, softSerial, 38400, readPowerRFID) == false) //Configure nano to run at 38400bps
  {
    Serial.println(F("Module failed to respond. Please check wiring."));
    while (1); //Freeze!
  }

  Serial.println(F("Press a key to begin scanning for tags."));
  while (!Serial.available()); //Wait for user to send a character
  Serial.read(); //Throw away the user's character

  nano.startReading(); //Begin scanning for tags
}

void loop()
{
  if (nano.check()) //Check to see if any new data has come in from module
  {
    byte responseType = nano.parseResponse(); //Break response into tag ID, RSSI, frequency, and timestamp

    if (responseType == RESPONSE_IS_KEEPALIVE)
    {
      // Do nothing 
      digitalWrite(debugLedPin, LOW);
    }
    else if (responseType == RESPONSE_IS_TAGFOUND)
    {
      digitalWrite(debugLedPin, HIGH);
      String msg = Capstone_RFID::tagToString(nano.msg, nano.getTagEPCBytes()); 
      if (!msg.equals(tagMessage)) {
        digitalWrite(ledPin, HIGH);
        Capstone_EMIC::sendToEmic(emicSerial, msg);
        digitalWrite(ledPin, LOW);
        tagMessage = msg; 
      }
      Serial.println(msg);
    }
    else if (responseType == ERROR_CORRUPT_RESPONSE)
    {
      digitalWrite(debugLedPin, LOW);
      Serial.println("Bad CRC");
    }
    else
    {
      //Unknown response
      digitalWrite(debugLedPin, LOW);
      Serial.println("Unknown error");
    }
  }
  while(piSerial.available()) {
    char c = piSerial.read();
    if(c != -1) {
        buffer[buf_idx++] = c;
    }
  }
  if(buf_idx > 0) {
    int rc = Capstone_Pi::parsePacket(buffer, buf_idx, emicSerial);
    if(rc > 0) {
        memmove(buffer, buffer + rc, buf_idx - rc);
        buf_idx -= rc;
    }
  }

}
