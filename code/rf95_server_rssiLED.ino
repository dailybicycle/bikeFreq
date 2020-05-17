/*Code below is adapted from the "rf95_server" example from the Radiohead library (https://www.airspayce.com/mikem/arduino/RadioHead/index.html)
  This sketch uses the Radiohead library to configure a LoRa module as a server, and then reads the RSSI from incoming messages to set
  an LED color, effectively using RSSI as a proxy for proximity sensing.
*/

// rf95_server.pde
// -*- mode: C++ -*-
/* Example sketch showing how to create a simple messaging server with the RH_RF95 class.
  so you should only use RH_RF95 if you do not need the higher level messaging abilities.
  It is designed to work with the other example rf95_client
*/

#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95;

//LED SETUP
#include <Adafruit_NeoPixel.h>
#define LED_COUNT 1
#define LED_PIN 6
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Set range of RSSI values. These values found through testing
int rssiStrongest = -20;
int rssiWeakest = -130;

void setup()

{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())//
    Serial.println("init failed");

  //Initiate LED
  pinMode(LED_PIN, OUTPUT);
  strip.begin();
  strip.setBrightness(255);
  strip.show();
}

void loop()
{
  if (rf95.available())
  {
    // there should be a message incoming
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      RH_RF95::printBuffer("request: ", buf, len);
      Serial.print("Client Says: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

      //map rssi to color range
      int redLightMap = map(rf95.lastRssi(), rssiWeakest, rssiStrongest, 255, 0);
      int redledVal = constrain(redLightMap, 0, 255);
      Serial.print("redledVal:");
      Serial.println(redledVal);

      int greenLightMap = map(rf95.lastRssi(), rssiWeakest, rssiStrongest, 255, 0);
      int greenledVal = constrain(greenLightMap, 255, 0);
      Serial.print("greenledVal:");
      Serial.println(greenledVal);

      strip.setPixelColor(0, redledVal, greenledVal, 0);
      strip.show();

      // Send a reply
      uint8_t data[] = "Hi, I'm the server!";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");

    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
