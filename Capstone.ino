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

SoftwareSerial softSerial(2, 3); //RX, TX
SoftwareSerial emicSerial(10, 11);

#include "SparkFun_UHF_RFID_Reader.h" //Library for controlling the M6E Nano module
#include "Static_Beacon_Translate.h"
RFID nano; //Create instance

#define rxPin   10  // Serial input (connects to Emic 2's SOUT pin)
#define txPin   11  // Serial output (connects to Emic 2's SIN pin)
#define ledPin  13  // Most Arduino boards have an on-board LED on this pin
String tagMessage; 
void setup()
{
  tagMessage = "";
  Serial.begin(9600);
  while (!Serial); //Wait for the serial port to come online
  Serial.println("begin");

  setupEmic(); 
  if (setupNano(38400) == false) //Configure nano to run at 38400bps
  {
    Serial.println(F("Module failed to respond. Please check wiring."));
    while (1); //Freeze!
  }

  nano.setRegion(REGION_NORTHAMERICA); //Set to North America

  nano.setReadPower(500); //5.00 dBm. Higher values may caues USB port to brown out
  //Max Read TX Power is 27.00 dBm and may cause temperature-limit throttling

  Serial.println(F("Press a key to begin scanning for tags."));
  while (!Serial.available()); //Wait for user to send a character
  Serial.read(); //Throw away the user's character

  Serial.println("Checkpoint");
  nano.startReading(); //Begin scanning for tags
  
}

void loop()
{
  //Serial.println("Loop");
  if (nano.check() == true) //Check to see if any new data has come in from module
  {
    byte responseType = nano.parseResponse(); //Break response into tag ID, RSSI, frequency, and timestamp

    if (responseType == RESPONSE_IS_KEEPALIVE)
    {
      Serial.println(F("Scanning"));
    }
    else if (responseType == RESPONSE_IS_TAGFOUND)
    {
      /*
      //If we have a full record we can pull out the fun bits
      int rssi = nano.getTagRSSI(); //Get the RSSI for this tag read

      long freq = nano.getTagFreq(); //Get the frequency this tag was detected at

      long timeStamp = nano.getTagTimestamp(); //Get the time this was read, (ms) since last keep-alive message
      */
      byte tagEPCBytes = nano.getTagEPCBytes(); //Get the number of bytes of EPC from response

      /*
      Serial.print(F(" rssi["));
      Serial.print(rssi);
      Serial.print(F("]"));

      Serial.print(F(" freq["));
      Serial.print(freq);
      Serial.print(F("]"));

      Serial.print(F(" time["));
      Serial.print(timeStamp);
      Serial.print(F("]"));
      */
      //Print EPC bytes, this is a subsection of bytes from the response/msg array
      Serial.print(F(" epc["));
      for (byte x = 0 ; x < tagEPCBytes ; x++)
      {
        if (nano.msg[31 + x] < 0x10) Serial.print(F("0")); //Pretty print
        Serial.print(nano.msg[31 + x], HEX);
        Serial.print(F(" "));
      }
      Serial.print(F("]"));

      Serial.println();
      String temp = printToString(nano.msg, tagEPCBytes); 
      if (!temp.equals(tagMessage)) {
        sendToEmic(temp);
        tagMessage = temp; 
      }
      Serial.println(printToString(nano.msg, tagEPCBytes));
    }
    else if (responseType == ERROR_CORRUPT_RESPONSE)
    {
      Serial.println("Bad CRC");
    }
    else
    {
      //Unknown response
      Serial.print("Unknown error");
    }
  }
}

//Gracefully handles a reader that is already configured and already reading continuously
//Because Stream does not have a .begin() we have to do this outside the library
boolean setupNano(long baudRate)
{
  nano.begin(softSerial); //Tell the library to communicate over software serial port

  //Test to see if we are already connected to a module
  //This would be the case if the Arduino has been reprogrammed and the module has stayed powered
  softSerial.begin(baudRate); //For this test, assume module is already at our desired baud rate
  while (softSerial.isListening() == false); //Wait for port to open

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
  if (nano.msg[0] != ALL_GOOD) return (false); //Something is not right

  //The M6E has these settings no matter what
  nano.setTagProtocol(); //Set protocol to GEN2

  nano.setAntennaPort(); //Set TX/RX antenna ports to 1

  return (true); //We are ready to rock
}

void setupEmic()  // Set up code called once on start-up
{
  // define pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  // set the data rate for the SoftwareSerial port
  emicSerial.begin(9600);
  digitalWrite(ledPin, LOW);  // turn LED off
  Serial.println("LED OFF");
  /*
    When the Emic 2 powers on, it takes about 3 seconds for it to successfully
    initialize. It then sends a ":" character to indicate it's ready to accept
    commands. If the Emic 2 is already initialized, a CR will also cause it
    to send a ":"
  */
  emicSerial.print('\n'); // Send a CR in case the system is already up
  Serial.println("Waiting for emic");
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();                 // Flush the receive buffer
  Serial.print("End emic");
}

void sendToEmic(String m) {
  emicSerial.print('S');
  emicSerial.print(m);  // Send the desired string to convert to speech
  emicSerial.print('\n');
  digitalWrite(ledPin, HIGH);         // Turn on LED while Emic is outputting audio
  //while (emicSerial.read() != ':');   // Wait here until the Emic 2 responds with a ":" indicating it's ready to accept the next command
  digitalWrite(ledPin, LOW);
  Serial.println("End EmIC output");
}

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

String printToString(uint8_t msg[], uint8_t bytes) {
  if (bytes == 0) {
    Serial.println("No bytes");
    return "";
  }

  if (msg[EPC_OFFSET] != VERSION_TAG) {
    Serial.println("Wrong version");
    return "";
  }
  
  String message = "";
  for (int i = 1; i < bytes; i++) {
    switch(msg[31 + i]) {
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
        message.concat("4 ");
        break;
      case 0x05:
        message.concat("5 ");
        break;
      case 0x06:
        message.concat("6 ");
        break;
      default:
        message.concat(nano.msg[EPC_OFFSET + i]);
        message.concat(" ");
        break;
    }    
  }
}
