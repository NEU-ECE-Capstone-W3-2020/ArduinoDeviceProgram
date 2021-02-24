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
const int debugLedPin = 10; //For range testing, debugging w/o serial
const int readPowerRFID = 1000; // 10db. DO NOT EXCEED 20db

SoftwareSerial softSerial(2, 3); //RX, TX
SoftwareSerial emicSerial(10, 11);
RFID nano;
String tagMessage; 

void setup()
{
  pinMode(debugLedPin, OUTPUT);
  tagMessage = "";
  Serial.begin(9600);
  while (!Serial); //Wait for the serial port to come online

  Capstone_EMIC::setupEmic(rxPin, txPin, ledPin, emicSerial); 
  if (Capstone_RFID::setupNano(nano, softSerial, 38400, readPowerRFID) == false) //Configure nano to run at 38400bps
  {
    Serial.println(F("Module failed to respond. Please check wiring."));
    while (1); //Freeze!
  }

  Serial.println(F("Press a key to begin scanning for tags."));
  while (!Serial.available()); //Wait for user to send a character
  Serial.read(); //Throw away the user's character

  nano.startReading(); //Begin scanning for t ags

    // -- SETTING UP SERIAL CONNECTION TO PI
  Serial.begin(9600);
}

void loop()
{
  // getting strings from the nano
  if (nano.check() == true) //Check to see if any new data has come in from module
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

  // getting strings from the serial connection to pi
    if (Serial.available() > 0) {

    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    
    // grabbing data from the pi
    String data = Serial.readStringUntil('\n');

    emicSerial.print('S');
    emicSerial.print(data);
    emicSerial.print('\n');
    digitalWrite(ledPin, HIGH);         // Turn on LED while Emic is outputting audio
    while (emicSerial.read() != ':');   // Wait here until the Emic 2 responds with a ":" indicating it's ready to accept the next command
    digitalWrite(ledPin, LOW);
      
    delay(500);    // 1/2 second delay
    
  }
}
