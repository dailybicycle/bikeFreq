// rf95_client.pde
// -*- mode: C++ -*-
/* Example sketch showing how to create a simple messageing client
  with the RH_RF95 class. RH_RF95 class does not provide for addressing or
  reliability, so you should only use RH_RF95 if you do not need the higher
  level messaging abilities.
  It is designed to work with the other example rf95_server
*/

#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <RH_RF95.h>

#define LED_COUNT 1
#define LED_PIN    6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Singleton instance of the radio driver
RH_RF95 rf95;

//rssi values found through testing
int rssiStrongest = -25;
int rssiWeakest = -40;

void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.

  //Initiate LED
  strip.begin();
  strip.setBrightness(255);
  strip.show();
}


void loop()

//calibrate distance

{
  //  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
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
      //      Serial.print("Server says: ");
      //      Serial.println((char*)buf)
      Serial.print("Raw RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

      //map rssi to color range
      int rssiVal = abs(rf95.lastRssi());
      int redColorMap = map(rssiVal, rssiWeakest, rssiStrongest, 0, 255);
      int greenColorMap = map(rssiVal, rssiWeakest, rssiStrongest, 255, 0);
      strip.setPixelColor(0, redColorMap, greenColorMap, 0);
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
    //set led blue if no reply
    strip.setPixelColor(0, 0, 0, 255);
    strip.show();
  }

  delay(500);

}

