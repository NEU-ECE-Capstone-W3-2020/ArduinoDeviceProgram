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

SoftwareSerial softSerial(2, 3); //RX, TX
SoftwareSerial emicSerial(10, 11);
RFID nano;
String tagMessage; 

void setup()
{
  tagMessage = "";
  Serial.begin(9600);
  while (!Serial); //Wait for the serial port to come online

  Capstone_EMIC::setupEmic(rxPin, txPin, ledPin, &emicSerial); 
  if (Capstone_RFID::setupNano(&nano, &softSerial, 38400) == false) //Configure nano to run at 38400bps
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
  if (nano.check() == true) //Check to see if any new data has come in from module
  {
    byte responseType = nano.parseResponse(); //Break response into tag ID, RSSI, frequency, and timestamp

    if (responseType == RESPONSE_IS_KEEPALIVE)
    {
      // Do nothing 
    }
    else if (responseType == RESPONSE_IS_TAGFOUND)
    {
      String msg = Capstone_RFID::tagToString(nano.msg, nano.getTagEPCBytes()); 
      if (!msg.equals(tagMessage)) {
        digitalWrite(ledPin, HIGH);
        Capstone_EMIC::sendToEmic(&emicSerial, msg);
        digitalWrite(ledPin, LOW);
        tagMessage = msg; 
      }
      Serial.println(msg);
    }
    else if (responseType == ERROR_CORRUPT_RESPONSE)
    {
      Serial.println("Bad CRC");
    }
    else
    {
      //Unknown response
      Serial.println("Unknown error");
    }
  }
}
