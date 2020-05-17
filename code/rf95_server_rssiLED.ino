// rf95_server.pde
// -*- mode: C++ -*-
/* Example sketch showing how to create a simple messaging server with the RH_RF95 class.
  so you should only use RH_RF95 if you do not need the higher level messaging abilities.
  It is designed to work with the other example rf95_client
*/


#include <SPI.h>
#include <RH_RF95.h> //http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html#details

// Singleton instance of the radio driver
RH_RF95 rf95;

//LED SETUP
#include <Adafruit_NeoPixel.h>
#define LED_COUNT 1
#define LED_PIN 6
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//rssi values found through testing. Rssi closer to 0 is stronger.
int rssiStrongest = -20;
int rssiWeakest = -130;

void setup()

{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())//
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  //  driver.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true.
  // Failure to do that will result in extremely low transmit powers.
  //  driver.setTxPower(14, true);

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
    /*  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t is the same as a byte. A byte stores an 8-bit unsigned number, from 0 to 255
      "buf[x]" creates an array of length within brackets
      If I use a lower value than the max length possible under this freq,
      then it will just truncate the message
    */
    uint8_t len = sizeof(buf);
    /*  uint8_t len = sizeof(buf);
      "get number of bytes occupied by the array"
      The sizeof operator returns the number of bytes in a variable type.
    */
    if (rf95.recv(buf, &len))
      /*recv() Turns the receiver on if it not already on.
        2 arguments
          -x: Location to copy received message (in the array we set at start of loop)
          -y: Pointer to available space in buf. Set to actual number of octets copied
         If there's a valid message available, copy it to buf and return true else return false.
         "&" is a "Pointer Access Operator"
         "&x" represents the memory address of the variable.
      */
    {

      //      RH_RF95::printBuffer("request: ", buf, len);
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

      /*
         char prints characters stored in buf.
         " * " points to the array stores in buf
      */

      // Send a reply
      uint8_t data[] = "Hi, I'm the server!";//array called data of size set by reply
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");

    }
    else
    {
      Serial.println("recv failed");
      //set led blue if server not running
      strip.setPixelColor(0, 0, 0, 255);
      strip.show();
    }
  }
}
