#include <Arduino.h>
#include <RCSwitch.h>
#include <FastLED.h>

#define DEBUG             // enable debug messages to serial monitor (comment out to disable)
#define USE_LED           // enable LED strip (comment out to disable)

#define TOY_RF_PIN    15  // The pin used to connect the RF transmitter
#define TOY_RF_REPEAT 3   // The number of times to repeat the RF transmission (less than 3 doesn't work reliably, 3 or more will be more reliable)

#define TOY_ON_MIN    1   // minimum time in minutes to turn on toy
#define TOY_ON_MAX    3   // maximum time in minutes to turn on toy
#define TOY_OFF_MIN   5   // minimum time in minutes to turn off toy
#define TOY_OFF_MAX   30  // maximum time in minutes to turn off toy

#define TOY_ON  "101010100101010100001010"  // RF command to turn on toy
#define TOY_OFF "101010100101010100001100"  // RF command to turn off toy

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

  // setup RF to work with toy
  vibratorRF.enableTransmit(TOY_RF_PIN);
  vibratorRF.setRepeatTransmit(TOY_RF_REPEAT);
  // Might need to change these values depending on the toy
  vibratorRF.setProtocol(1);        // toy in question uses protocol 1
  vibratorRF.setPulseLength(250);   // toy in question uses a pulse length of 250

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

  // one second tick - basically only used for the debug code, I really should wrap this with the DEBUG define but leaving it incase I want it for something else.
  if (millis() >= lastOneSecondTick)
  {
    lastOneSecondTick = millis() + 1000;
#ifdef DEBUG
    Serial.println("Current Millis: " + String(millis()) + " Toy State: " + String(toyState ? "ON" : "OFF") + " nextToyActionTime: " + String(nextToyActionTime) + " Millis until nextToyActionTime: " + String(nextToyActionTime - millis()));
#endif
  }

  // code thats fired once nextToyActionTime is reached
  if (millis() >= nextToyActionTime)
  {
    toyState = !toyState; // toggle the toy state
    if (toyState) // Turn the toy on
    {
      vibratorRF.send(TOY_ON); // send the RF command to turn on toy
      nextToyActionTime = millis() + randomMilliSeconds(TOY_ON_MIN, TOY_ON_MAX); // update nextToyActionTime to turn off the toy after a random amount of time between min and max
#ifdef USE_LED
      // set the LED to green and "show" it (show actually spits out the data to the LED)
      leds[0] = CRGB::Green;
      FastLED.show();
#endif
#ifdef DEBUG
      Serial.println("ON - On until " + String(nextToyActionTime));
#endif
    }
    else // Turn the toy off
    {
#ifdef USE_LED
      // set the LED to black and "show" it - black will turn off the LED and "show" will update the LED
      leds[0] = CRGB::Black;
      FastLED.show();
#endif
      // to make sure the toy is turned off we will send the off command multiple times - threw 10 into the for loop because of habbit, 10 is prob overkill.
      for (int i = 0; i < 10; i++)
        vibratorRF.send(TOY_OFF); // send the RF command to turn off toy
      nextToyActionTime = millis() + randomMilliSeconds(TOY_OFF_MIN, TOY_OFF_MAX); // update nextToyActionTime to turn on the toy after a random amount of time between min and max
#ifdef DEBUG
      Serial.println("OFF - Off until " + String(nextToyActionTime));
#endif
    }
  }
}