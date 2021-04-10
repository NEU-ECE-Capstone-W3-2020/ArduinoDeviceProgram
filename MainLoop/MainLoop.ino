/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#include <SoftwareSerial.h>

const int txPin = 10;
const int rxPin = 11;
const int ledPin = 13;
SoftwareSerial emicSerial(10, 11);
SoftwareSerial bleSerial(2, 3);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  setupEmic(rxPin, txPin, ledPin, emicSerial); 
  Serial.begin(115200);
  bleSerial.begin(115200);
}

void setupEmic(int rxPin, int txPin, int ledPin, SoftwareSerial& emicSerial){
  // define pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  // set the data rate for the SoftwareSerial port
  emicSerial.begin(9600);
  digitalWrite(ledPin, LOW);  // turn LED off
  /*
    When the Emic 2 powers on, it takes about 3 seconds for it to successfully
    initialize. It then sends a ":" character to indicate it's ready to accept
    commands. If the Emic 2 is already initialized, a CR will also cause it
    to send a ":"
  */
  emicSerial.print('\n'); // Send a CR in case the system is already up
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();                 // Flush the receive buffer
}

void sendToEmic(SoftwareSerial& emicSerial, String m) {
  emicSerial.print('S');
  emicSerial.print(m);  // Send the desired string to convert to speech
  emicSerial.print('\n');
  Serial.println("End EmIC output");
}

// the loop function runs over and over again forever
String result = "";
void loop() {
  
  while (foo.available()) {
    char ch = foo.read();
    bar += (char)ch;
  }
  if (bar != "") {
    Serial.println(bar);
    sendToEmic(emicSerial, bar);
  }
}