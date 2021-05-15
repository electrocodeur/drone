#include "Arduino.h"
#include <SPI.h>
#include <RF24-STM.h>

#define horaireavantplus           PB15
#define horaireavantmoins          PB14
#define horaireavantvitesse        PA8

#define antihoraireavantplus       PB10
#define antihoraireavantmoins      PB11
#define antihoraireavantvitesse    PB1

#define horairederriereplus        PA1
#define horairederrieremoins       PA0
#define horairederrierevitesse     PA2

#define antihorairederriereplus    PB7
#define antihorairederrieremoins   PB6
#define antihorairederrierevitesse PB8

#define pleinevitesse 100
#define mivitesse     0

#define led PB4

int hauteur = 0;

struct data {
  int x;
  int y;
  int elevation;
  int btn;
};

data myDataRx;

/*
   NRF24L01     Arduino_ Uno  Blue_Pill(stm32f01C)
   ___________________________________________________
   VCC  RED    |    3.3v   |      3.3v
   GND  BROWN  |    GND    |      GND
   CSN  YE0  |   Pin10   | A4 NSS1   (PA4)  \
   CE   ORANGE |   Pin9    | B0 digital(PB0)    |     NB
   SCK  GREEN  |   Pin13   | A5 SCK1   (PA5)  |- All these pins
   MOSI BLUE   |   Pin11   | A7 MOSI1  (PA7)  |  are 3.3v tolerant
   MISO PURPLE |   Pin12   | A6 MISO1  (PA6)  /

      Always use the adapter plate for 5v!
*/
RF24 radio(PB0, PA3); // CE, CSN

void setup() {
  Serial.begin(115200);
  Serial.println("THIS IS THE RECEIVER CODE - YOU NEED THE OTHER ARDUINO TO TRANSMIT");
  pinMode(horaireavantplus,           OUTPUT);
  pinMode(horaireavantmoins,          OUTPUT);
  pinMode(horaireavantvitesse,        OUTPUT);

  pinMode(antihoraireavantplus,       OUTPUT);
  pinMode(antihoraireavantmoins,      OUTPUT);
  pinMode(antihoraireavantvitesse,    OUTPUT);

  pinMode(horairederriereplus,        OUTPUT);
  pinMode(horairederrieremoins,       OUTPUT);
  pinMode(horairederrierevitesse,     OUTPUT);

  pinMode(antihorairederriereplus,    OUTPUT);
  pinMode(antihorairederrieremoins,   OUTPUT);
  pinMode(antihorairederrierevitesse, OUTPUT);

  pinMode(led,OUTPUT);
  // Initiate the radio object
  radio.begin();

  // Set the transmit power to 0est available to prevent power supply related issues
  radio.setPALevel(RF24_PA_LOW);

  // Set the speed of the transmission to the quickest available
  radio.setDataRate(RF24_1MBPS);

  // Use a channel unlikely to be used by Wifi, Microwave ovens etc 124
  radio.setChannel(104);

  radio.setRetries(255, 5);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openReadingPipe(1, 0xB3B4B5B602);
  radio.openWritingPipe(0xB3B4B5B601);

  // Start the radio listening for data
  radio.startListening();

  // Auto ackknowledgment of a transmission
  radio.setAutoAck(true);

  // Debugging information
  radio.printDetails();
}

void loop() {
  // Is there any data for us to get?
  if (radio.available()) {
    // Go and read the data and put it into that variable
    //while (radio.available()) {
    radio.read(&myDataRx, sizeof(data));

    delay(100);
    Serial.println("RX: Received");

    // First, stop listening so we can talk
    radio.stopListening();

    // Show user what we sent and what we got back
    Serial.print("X         : ");
    Serial.println(myDataRx.x);
    Serial.print("Y         : ");
    Serial.println(myDataRx.y);
    Serial.print("Y2        : ");
    Serial.println(myDataRx.elevation);
    Serial.print("btn       : ");
    Serial.println(myDataRx.btn);
    hauteur = map(myDataRx.elevation, 1500, 4096, 5, 156);
    digitalWrite(led,myDataRx.btn);
    if (hauteur <= 44) {
      hauteur = 0;
    }
    Serial.println(hauteur);
    elever();
    if (myDataRx.x > 2300 ) {
      droite();
    }
    if (myDataRx.x < 1900 ) {
      gauche();
    }
    if (myDataRx.y > 2200 ) {
      avancer();
    }
    if (myDataRx.y < 1900 ) {
      reculer();
    }
    if (myDataRx.x <= 2299 && myDataRx.x >= 1901 && myDataRx.y <= 2199 && myDataRx.y >= 1901 ) {
      elever();
    }
    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}

void elever() {
  digitalWrite(horaireavantplus,         1);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     1);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      1);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  1);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        hauteur);
  analogWrite(antihoraireavantvitesse,    hauteur);
  analogWrite(horairederrierevitesse,     hauteur);
  analogWrite(antihorairederrierevitesse, hauteur);
}
void droite() {
  digitalWrite(horaireavantplus,         1);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     1);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      1);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  1);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        hauteur + pleinevitesse);
  analogWrite(antihoraireavantvitesse,    hauteur + mivitesse);
  analogWrite(horairederrierevitesse,     hauteur + mivitesse);
  analogWrite(antihorairederrierevitesse, hauteur + pleinevitesse);
}
void gauche() {
  digitalWrite(horaireavantplus,         1);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     1);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      1);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  1);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        hauteur + mivitesse);
  analogWrite(antihoraireavantvitesse,    hauteur + pleinevitesse);
  analogWrite(horairederrierevitesse,     hauteur + pleinevitesse);
  analogWrite(antihorairederrierevitesse, hauteur + mivitesse);
}
void avancer() {
  digitalWrite(horaireavantplus,         1);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     1);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      1);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  1);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        hauteur + mivitesse);
  analogWrite(antihoraireavantvitesse,    hauteur + mivitesse);
  analogWrite(horairederrierevitesse,     hauteur + pleinevitesse);
  analogWrite(antihorairederrierevitesse, hauteur + pleinevitesse);
}
void reculer() {
  digitalWrite(horaireavantplus,         1);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     1);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      1);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  1);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        hauteur + pleinevitesse);
  analogWrite(antihoraireavantvitesse,    hauteur + pleinevitesse);
  analogWrite(horairederrierevitesse,     hauteur + mivitesse);
  analogWrite(antihorairederrierevitesse, hauteur + mivitesse);
}
void rien() {
  digitalWrite(horaireavantplus,          0);
  digitalWrite(horaireavantmoins,         0);
  digitalWrite(antihoraireavantplus,      0);
  digitalWrite(antihoraireavantmoins,     0);
  digitalWrite(horairederriereplus,       0);
  digitalWrite(horairederrieremoins,      0);
  digitalWrite(antihorairederriereplus,   0);
  digitalWrite(antihorairederrieremoins,  0);
  analogWrite(horaireavantvitesse,        0);
  analogWrite(antihoraireavantvitesse,    0);
  analogWrite(horairederrierevitesse,     0);
  analogWrite(antihorairederrierevitesse, 0);
}
/*void droite() {
  digitalWrite(horaireavantplus,         1);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     0);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      0);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  1);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        256);
  analogWrite(antihoraireavantvitesse,    125);
  analogWrite(horairederrierevitesse,     125);
  analogWrite(antihorairederrierevitesse, 256);
  }
  void gauche() {
  digitalWrite(horaireavantplus,         0);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     1);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      1);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  0);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        125);
  analogWrite(antihoraireavantvitesse,    256);
  analogWrite(horairederrierevitesse,     256);
  analogWrite(antihorairederrierevitesse, 125);
  }
  void avancer() {
  digitalWrite(horaireavantplus,         0);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     0);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      1);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  1);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        125);
  analogWrite(antihoraireavantvitesse,    125);
  analogWrite(horairederrierevitesse,     256);
  analogWrite(antihorairederrierevitesse, 256);
  }
  void reculer() {
  digitalWrite(horaireavantplus,         1);
  digitalWrite(horaireavantmoins,        0);
  digitalWrite(antihoraireavantplus,     1);
  digitalWrite(antihoraireavantmoins,    0);
  digitalWrite(horairederriereplus,      0);
  digitalWrite(horairederrieremoins,     0);
  digitalWrite(antihorairederriereplus,  0);
  digitalWrite(antihorairederrieremoins, 0);
  analogWrite(horaireavantvitesse,        256);
  analogWrite(antihoraireavantvitesse,    256);
  analogWrite(horairederrierevitesse,     125);
  analogWrite(antihorairederrierevitesse, 125);
  }*/
