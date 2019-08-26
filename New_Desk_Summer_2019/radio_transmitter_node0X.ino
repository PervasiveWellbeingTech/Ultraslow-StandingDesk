/*
  Arduino Wireless Network - Multiple NRF24L01 Tutorial
        == Node 02 (Child of Master node 00) ==    
*/

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 02;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format


const unsigned long interval = 10;  //ms  // How often to send data to the other unit
unsigned long last_sent = 0;            // When did we last send?

void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
}

void loop() {
  network.update();
  //===== Recieving =====//
  while(network.available()) {
    RF24NetworkHeader header;
    const char text1[32] = "";
    network.read(header, &text1, sizeof(text1));
    RF24NetworkHeader header1(master00);   // (Address where the data is going)
    bool ok = network.write(header1, &text1, sizeof(text1)); // Send the data
  }
  //===== Sending =====//
  unsigned long now = millis();
  if (now - last_sent >= interval) {   // If it's time to send a data, send it!
    const char text[] = "Radio 5!";
    RF24NetworkHeader header(master00);   // (Address where the data is going)
    bool ok = network.write(header, &text, sizeof(text)); // Send the data
    last_sent = now;
  }
}
