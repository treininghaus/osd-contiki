/*
 * Sample arduino sketch using contiki features.
 * We turn the LED off 
 * We allow read the moisture sensor
 * Unfortunately sleeping for long times in loop() isn't currently
 * possible, something turns off the CPU (including PWM outputs) if a
 * Proto-Thread is taking too long. We need to find out how to sleep in
 * a Contiki-compatible way.
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */

extern "C" {
#include <stdio.h>
#include "moisture.h"

#define LED_PIN 4

uint8_t moisture_pin = A5;
uint16_t moisture_voltage = 0;
}

void setup (void)
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    rest_init_engine ();
    rest_activate_resource (&resource_moisture);
}

void loop (void)
{

}
