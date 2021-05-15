// TX TX TX TX TX TX TX TX TX TX TX TX TX
#include "Arduino.h"

#include <SPI.h>

//#include <RF24.h>
#include <RF24-STM.h>
#define axe_x         PA0
#define axe_y         PA1
#define axe_elevation PA2
#define inter         PB10
/*
   NRF24L01     Arduino_ Uno  Blue_Pill(stm32f01C)
   ___________________________________________________
   VCC  RED    |    3.3v   |      3.3v
   GND  BROWN  |    GND    |      GND
   CSN  YELOW  |   Pin10   | A4 NSS1   (PA4)  \
   CE   ORANGE |   Pin9    | B0 digital(PB0)    |     NB
   SCK  GREEN  |   Pin13   | A5 SCK1   (PA5)  |- All these pins
   MOSI BLUE   |   Pin11   | A7 MOSI1  (PA7)  |  are 3.3v tolerant
   MISO PURPLE |   Pin12   | A6 MISO1  (PA6)  /

      Always use the adapter plate for 5v!
*/
RF24 radio(PB0, PB1);

struct data {
  int x;
  int y;
  int elevation;
  int btn;
  // Total 21, you can have max 32 bytes here
};

// Create an instance of the data container
data myData;

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
  // TX TX TX TX TX TX TX TX TX TX TX TX TX

  Serial.begin(115200);
  Serial.println("TX: THIS IS THE TRANSMITTER CODE - YOU NEED THE OTHER ARDIUNO TO SEND BACK A RESPONSE");
  pinMode(axe_x,         INPUT);
  pinMode(axe_y,         INPUT);
  pinMode(axe_elevation, INPUT);
  pinMode(inter,         INPUT_PULLUP);

  // Initiate the radio object
  radio.begin();

  // Set the transmit power to lowest available to prevent power supply related issues
  radio.setPALevel(RF24_PA_LOW);

  // Set the speed of the transmission to the quickest available
  radio.setDataRate(RF24_1MBPS);

  // Use a channel unlikely to be used by Wifi, Microwave ovens etc 124
  radio.setChannel(104);

  // Give receiver a chance
  radio.setRetries(200, 50);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openReadingPipe(1, 0xB3B4B5B601);
  radio.openWritingPipe(0xB3B4B5B602);

  // Auto ackknowledgment of a transmission
  radio.setAutoAck(true);

  // Debugging information
  radio.printDetails();

  // Random number seeding (we're going to be sending a single random number)
  //randomSeed(analogRead(14));
}

// -----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -----------------------------------------------------------------------------
void loop() {
  int valx = analogRead(axe_x);
  int valy = analogRead(axe_y);
  int valelevation = analogRead(axe_elevation);
  int valinter = digitalRead(inter);
  // Create enum type with some default data
  myData.x = valx;
  myData.y = valy;
  myData.elevation = valelevation;
  myData.btn = valinter;
  // TX TX TX TX TX TX TX TX TX TX TX TX TX

  // Ensure we have stopped listening (even if we're not) or we won't be able to transmit
  radio.stopListening();

  // Did we manage to SUCCESSFULLY transmit that (by getting an acknowledgement back from the other Arduino)?
  // Even we didn't we'll continue with the sketch, you never know, the radio fairies may help us
  Serial.println("TX: Sent out data");
  if (!radio.write(&myData, sizeof(myData))) {
    Serial.println("TX: No ACK");
  } else {
    Serial.println("TX: ACK");
  }
  // Now listen for a response
  radio.startListening();

  // But we won't listen for long
  unsigned long started_waiting_at = millis();

  // Loop here until we get indication that some data is ready for us to read (or we time out)
  while (!radio.available()) {

    // Oh dear, no response received within our timescale
    if (millis() - started_waiting_at > 250) {
      Serial.print("TX: Got no reply");
      delay(5);
      return;
    }
  }

  // Now read the data that is waiting for us in the nRF24L01's buffer
  radio.read(&myData, sizeof(myData));
  delay(25);

  // Show user what we sent and what we got back
  Serial.print("X  : "); Serial.println(myData.x);
  Serial.print("Y  : "); Serial.println(myData.y);
  Serial.print("Y2  : "); Serial.println(myData.elevation);
  Serial.println("-------------------\n");

  // Wash, Rinse, Repeat... a bit later
  delay(5);
}
