# buttplug-remote

Simple Arduino sketch that randomly transmits the on and off commands for a RF controlled butt plug found on eBay.

The butt plug was supposed to be Bluetooth/App controlled, but the seller sent a model that only works via a remote control. Well that wasn't was desired so threw together a simple sketch that randomly turns on the toy for 1-3 mins with breaks of 5-30 mins inbetween.

Don't forget to change platformio.ini for your device and serial port. Or just cut & paste the contents of main.cpp into your IDE of choice.

Sketch uses [rc-switch](https://github.com/sui77/rc-switch) to handle the transmission and [FastLED](https://github.com/FastLED/FastLED) to control a WS2852 LED I had on hand for a simple status LED.  
Both of which could have been bitbanged but Meh, I'm lazy!

433Mhz transmitter is wired on pin 15 and the LED on pin 4 of a ESP32. But any Arduino micro should handle it just fine.  
Why use an ESP32? Well 1) I have them on hand 2) I might add bluetooth or wi-fi support if I cba'ed!
