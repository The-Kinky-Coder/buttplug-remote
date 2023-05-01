#include <Arduino.h>
#include <RCSwitch.h>
#include <FastLED.h>

// #define DEBUG           // enable debug messages to serial monitor (comment out to disable)
#define USE_LED         // enable LED strip (comment out to disable)

#define TOY_ON_MIN  1   // minimum time in minutes to turn on toy
#define TOY_ON_MAX  3   // maximum time in minutes to turn on toy
#define TOY_OFF_MIN 5   // minimum time in minutes to turn off toy
#define TOY_OFF_MAX 30  // maximum time in minutes to turn off toy

#define TOY_ON "101010100101010100001010"  // RF command to turn on toy
#define TOY_OFF "101010100101010100001100" // RF command to turn off toy

#ifdef USE_LED
#define NUM_LEDS 1
#define DATA_PIN 4
CRGB leds[NUM_LEDS];
bool bootledFlag = false;
#endif

RCSwitch vibratorRF = RCSwitch();

bool toyState = false;
unsigned long nextToyActionTime = 0;
unsigned long lastOneSecondTick = 0;
unsigned long randomMilliSeconds(long min, long max) { return random(min, max) * 60 * 1000; }

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif

  // set a time to turn on toy for the first time
  nextToyActionTime = millis() + randomMilliSeconds(TOY_OFF_MIN, TOY_OFF_MAX);
  // // Demo code
  // nextToyActionTime = millis() + 15*1000;

  // setup RF to work with toy
  vibratorRF.enableTransmit(15);
  vibratorRF.setProtocol(1);
  vibratorRF.setPulseLength(250);
  vibratorRF.setRepeatTransmit(3);

#ifdef USE_LED
  // setup LED strip
  FastLED.addLeds<WS2852, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(8);

  bootledFlag = true;
  leds[0] = CRGB::Red;
  FastLED.show();
#endif

  // make sure the toy is off
  for (int i = 0; i < 10; i++)
    vibratorRF.send(TOY_OFF);

#ifdef DEBUG
  Serial.println("TOY OFF - Off until " + String(nextToyActionTime));
#endif
}

void loop()
{
#ifdef USE_LED
  if (millis() >= 1000 && bootledFlag)
  {
    bootledFlag = false;
    leds[0] = CRGB::Black;
    FastLED.show();
  }
#endif

  if (millis() >= lastOneSecondTick + 1000)
  {
    // one second tick
    lastOneSecondTick = millis();
#ifdef DEBUG
    Serial.println("Current Millis: " + String(millis()) + " Toy State: " + String(toyState ? "ON" : "OFF") + " nextToyActionTime: " + String(nextToyActionTime) + " Millis until nextToyActionTime: " + String(nextToyActionTime - millis()));
#endif
  }

  if (millis() > nextToyActionTime)
  {
    toyState = !toyState;
    if (toyState)
    {
      vibratorRF.send(TOY_ON);
      nextToyActionTime = millis() + randomMilliSeconds(TOY_ON_MIN, TOY_ON_MAX);
      // // Demo code
      // nextToyActionTime = millis() + 10*1000;
#ifdef USE_LED
      leds[0] = CRGB::Green;
      FastLED.show();
#endif
#ifdef DEBUG
      Serial.println("ON - On until " + String(nextToyActionTime));
#endif
    }
    else
    {
#ifdef USE_LED
      leds[0] = CRGB::Black;
      FastLED.show();
#endif
      for (int i = 0; i < 10; i++)
        vibratorRF.send(TOY_OFF);
      nextToyActionTime = millis() + randomMilliSeconds(TOY_OFF_MIN, TOY_OFF_MAX);
#ifdef DEBUG
      Serial.println("OFF - Off until " + String(nextToyActionTime));
#endif
    }
  }
}