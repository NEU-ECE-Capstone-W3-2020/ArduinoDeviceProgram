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
#include "Capstone_Message.h"

const int rxPin = 10;  // Serial input (connects to Emic 2's SOUT pin)
const int txPin = 11;  // Serial output (connects to Emic 2's SIN pin)
const int ledPin = 12; // Most Arduino boards have an on-board LED on this pin
const int debugLedPin = 13; //For range testing, debugging w/o serial
const int readPowerRFID = 1700; // 10db. DO NOT EXCEED 20db

SoftwareSerial softSerial(2, 3); //RX, TX
SoftwareSerial emicSerial(10, 11);
RFID nano;
String tagMessage; 
String incomingByte = "";

bool SOLO_MODE = true;

static bool DEBUG = true;
static String NEXT_MSG_PROMPT = "Press button to hear more";
static int AUDIO_OUTPUT_GAP = 5; 
static int WAIT_FOR_INPUT = 5; 

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
  Message *msg = checkForBeacon();
  
  if(SOLO_MODE && msg != NULL) {
    emicOutputMessage(msg);
  }
}

Message* checkForNewTag() {
if (nano.check() == true) //Check to see if any new data has come in from module
  {
    byte responseType = nano.parseResponse(); //Break response into tag ID, RSSI, frequency, and timestamp

    if (responseType == RESPONSE_IS_TAGFOUND)
    {
      digitalWrite(debugLedPin, HIGH);
      String msg = Capstone_RFID::tagToString(nano.msg, nano.getTagEPCBytes()); 
      debug_print(msg);
      
      if (!msg.equals(tagMessage)) {
        tagMessage = msg;
        return Capstone_Message::createTagMessage(1, msg);
      }
    } else {
      //debug_print("Error code: " + responseType);
      digitalWrite(debugLedPin, LOW);
      return NULL;
    }
  }
  digitalWrite(debugLedPin, LOW);
  return NULL;
}

Message* checkForBeacon() {
  return Capstone_Message::createBeaconMessage(10, "Hello", 10, "Follow up");
}

void emicOutputMessage(Message* msg) {
  if (msg->content == "") {
    msgToEmic(msg->intro);
    delay(AUDIO_OUTPUT_GAP);
  } else {
    msgToEmic(msg->intro);
    delay(AUDIO_OUTPUT_GAP);
    msgToEmic(NEXT_MSG_PROMPT);
    delay(AUDIO_OUTPUT_GAP);
    if(true) { //TO-DO: CHECK FOR BUTTON
      msgToEmic(msg->content);
    }
    // wait for button
  }
  Capstone_Message::freeMessage(msg);
  debug_print("Finished Audio output");
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
