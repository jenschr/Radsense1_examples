#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "Pushsafer.h"
#include "ld2410.h"

// Variables you might want to change
#define SecondsBetweenMessages 240      // Seconds between messages & first message
#define SSID "your-ssid"                // Change this to your WiFi SSID
#define PASSWORD "your-password"        // Change this to your WiFi password
#define PushsaferKey "XXXXXXXXXXXXX"    // Private key, found on your Pushsafer Dashboard
#define DETECTION_RANGE 100             // Detection distance in centimeters
// The radar is measuring centimeters. To use inches, 
// multiply inches by the CM_TO_INCH factor like this:
// #define DETECTION_RANGE 4 * CM_TO_INCH

// Constants
#define RADAR_SERIAL Serial1
#define CM_TO_INCH 25.4
#define SCL_PIN 1
#define SDA_PIN 0
#define LED_PIN 2 // v1.6 = 2, v1.7 = 5
#define SETTINGS_PIN 8
#define IN1_PIN 4
#define IN2_PIN 5 // v1.6 = 5, v1.7 = 2
#define RELAY_OUT1_PIN 6
#define RELAY_OUT2_PIN 7
#define NEOPIXEL_PIN 3
#define BOOT_PIN 9 // Never use apart from booting?
#define RADAR_OUT_PIN 10
#define RADAR_RX_PIN 21
#define RADAR_TX_PIN 20
#define NEOPIXEL_COUNT 8

ld2410 radar;
Adafruit_NeoPixel *pixels;
WiFiClient client;

Pushsafer pushsafer(PushsaferKey, client);
unsigned long lastTimeMessageWasSent = 0;

// Updates all the programmable RGB LEDs
void setPixels(uint8_t red, uint8_t green, uint8_t blue)
{
  // uncomment to never use LEDs
  // return;
  
  // if we don't have a pixel object, make one
  if(pixels == nullptr)
  {
    pixels = new Adafruit_NeoPixel(8, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
    pixels->begin();
  }

  // Set all pixels to the same color
  for(int i = 0; i < NEOPIXEL_COUNT; i++)
  {
    pixels->setPixelColor(i, red,green,blue);
  }
  pixels->show();
}

void sendPushMessageIfItIsTime()
{
  if( millis() - lastTimeMessageWasSent > (SecondsBetweenMessages * 1000) )
  {
    if(WiFi.isConnected())
    {
      Serial.println(F("Sending push message"));
      setPixels(255,255,255); // bright white while sending!
      
      struct PushSaferInput input;
      input.message = "The sensor was triggered!";
      input.title = "Sensor triggered";
      input.sound = "48";
      input.vibration = "1";
      input.icon = "1";
      input.iconcolor = "#FFCCCC";
      input.priority = "1";
      input.device = "a"; // Must be set to 'a' for 'all'
      input.url = "https://maketronics.no/radsense-1/";
      input.urlTitle = "Open maketronics.com";
      // Check Pushsafer documentation for more options

      Serial.println(pushsafer.sendEvent(input));
      
      setPixels(0,0,0); // off
      delay(200);
    }
    else
    {
      Serial.println(F("Wifi not connected. Cannot send Push message."));
      setPixels(255,0,0); // bright red!
      delay(200);
      setPixels(0,0,0); // off
      delay(200);
    }

    // Update the last time a message was sent
    lastTimeMessageWasSent = millis();
  }
}

void setup() {
  // Turn on the programmable RGB LEDs
  setPixels(0,0,55); // Set pixels to blue

  // Turn on and give the serial port time to start
  Serial.begin(115200);
  delay(4000);

  // Start the radar
  RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  delay(500);
  if(radar.begin(RADAR_SERIAL))
  {
    Serial.print(F("Radar OK\n"));
    setPixels(0,55,0); // Set pixels to green
  }
  else
  {
    Serial.print(F("Radar error\n"));
    while(1){
      setPixels(55,0,0); // Set pixels to red
      delay(200);
      setPixels(0,0,0); // Turn off pixels
      delay(200);
    }
  }

  // start the wifi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED && millis() < 10000) {
      // Flash pixels while connecting
      setPixels(0,55,0); // Set pixels to green
      delay(200);
      setPixels(0,0,0); // Turn off pixels
      delay(200);
  }
}

void loop() {
  bool wasRead = radar.read();
  if( wasRead ) // Only do updates if we could read the sensor
  {
    // This is the very simplest way to detect presence
    // It does not support any limiting of detection range
    // but it's very solid and fast
    if( radar.presenceDetected() )
    {
      // Only do something if within range
      int moving = radar.movingTargetDistance();
      int stationary = radar.stationaryTargetDistance();
      if( moving < DETECTION_RANGE || stationary < DETECTION_RANGE )
      {
        setPixels(0,55,0); // Pixels Green
        sendPushMessageIfItIsTime();
      }
      else
      {
        setPixels(55,0,55); // Pixels Pink
      }
    }
    else
    {
      setPixels(0,0,0); // Turn pixels
    }
    delay(50);
  }
}
