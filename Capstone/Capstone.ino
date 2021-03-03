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
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 8, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int rxPin = 10;  // Serial input (connects to Emic 2's SOUT pin)
const int txPin = 11;  // Serial output (connects to Emic 2's SIN pin)
const int ledPin = 13; // Most Arduino boards have an on-board LED on this pin
const int debugLedPin = 7; //For range testing, debugging w/o serial
const int readPowerRFID = 1000; // 10db. DO NOT EXCEED 20db

SoftwareSerial softSerial(2, 3); //RX, TX
SoftwareSerial emicSerial(10, 11);
RFID nano;
String tagMessage; 
static bool DEBUG = true;
String bleMessage = "";

void setup()
{
  debug_init();
  pinMode(debugLedPin, OUTPUT);
  tagMessage = "";

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  Capstone_EMIC::setupEmic(rxPin, txPin, ledPin, emicSerial); 
  if (Capstone_RFID::setupNano(nano, softSerial, 38400, readPowerRFID) == false) //Configure nano to run at 38400bps
  {
    debug_print("Module failed to respond. Please check wiring.");
    while (1); //Freeze!
  }

  // ble USB
  Serial.begin(9600);

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
        msgToLCD(msg);
        tagMessage = msg; 
      }
    } else {
      debug_print("Error code: " + responseType);
      digitalWrite(debugLedPin, LOW);
    }
  }

  if (Serial.available()) {
    String cur_msg = Serial.readStringUntil('\n');
    if (cur_msg != bleMessage) {
      bleMessage = cur_msg;
      Serial.println("Received from Pi: " + Serial.readString());
      msgToEmic(bleMessage);
      msgToLCD(bleMessage);
      Serial.flush();
    }
  }
}

void msgToLCD(String msg) {
  lcd.clear();
  if (msg.length() > 12) {
    lcd.setCursor(0,0);
    lcd.print(msg.substring(0, 14));
    lcd.setCursor(0, 1);
    lcd.print(msg.substring(14, msg.length()));
  } else {
    lcd.setCursor(0,0);
    lcd.print(msg); 
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
