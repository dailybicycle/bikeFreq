/*Code below is adapted from the "rf95_client" example from the Radiohead library (https://www.airspayce.com/mikem/arduino/RadioHead/index.html)
  This sketch uses the Radiohead library to configure a LoRa module as a client, and then reads the RSSI from incoming messages to set
  an LED color, effectively using RSSI as a proxy for proximity sensing.
*/

// rf95_client.pde
// -*- mode: C++ -*-
/* Example sketch showing how to create a simple messageing client
  with the RH_RF95 class. RH_RF95 class does not provide for addressing or
  reliability, so you should only use RH_RF95 if you do not need the higher
  level messaging abilities.
  It is designed to work with the other example rf95_server
*/

#include <SPI.h>
#include <RH_RF95.h>

//LED setup
#include <Adafruit_NeoPixel.h>
#define LED_COUNT 1
#define LED_PIN    6
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Set range of RSSI values. These values found through testing
int rssiStrongest = -20;
int rssiWeakest = -130;

// Singleton instance of the radio driver
RH_RF95 rf95;

void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");

  //Initiate LED
  pinMode(LED_PIN, OUTPUT);
  strip.begin();
  strip.setBrightness(255);
  strip.show();
}


void loop()

{
  Serial.println("Sending to rf95_server");
  //  Send a message to rf95_server
  uint8_t data[] = "Hello, I'm the client";
  rf95.send(data, sizeof(data));

  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
      Serial.print("Server says: ");
      Serial.println((char*)buf);
      Serial.print("RSSI:");
      Serial.println(rf95.lastRssi(), DEC);

      //map rssi to color range.

      // red led (255,0,0) will mean weak signal
      int redLightMap = map(rf95.lastRssi(), rssiWeakest, rssiStrongest, 255, 0);
      int redledVal = constrain(redLightMap, 0, 255);
      Serial.print("redledVal:");
      Serial.println(redledVal);

      // green led (0,255,0) will mean strong signal
      int greenLightMap = map(rf95.lastRssi(), rssiWeakest, rssiStrongest, 255, 0);
      int greenledVal = constrain(greenLightMap, 255, 0);
      Serial.print("greenledVal:");
      Serial.println(greenledVal);

      strip.setPixelColor(0, redledVal, greenledVal, 0);
      strip.show();
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {

    Serial.println("No reply, is rf95_server running?");
    //set led blue if no signal recieved from server
    strip.setPixelColor(0, 0, 0, 255);
    strip.show();
  }

  delay(500);

}
