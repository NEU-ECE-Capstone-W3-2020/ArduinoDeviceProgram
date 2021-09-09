#include <Arduino.h>

#include "Capstone_EMIC.h"

namespace Capstone_EMIC {
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
}
