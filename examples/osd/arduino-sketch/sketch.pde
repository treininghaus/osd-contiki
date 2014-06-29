/*
 * Sample arduino sketch using contiki features.
 * We turn the LED on and off and allow setting the interval and the
 * brightness of the LED via coap.
 * Unfortunately sleeping for long times in loop() isn't currently
 * possible, something turns off the CPU (including PWM outputs) if a
 * Proto-Thread is taking too long. We need to find out how to sleep in
 * a Contiki-compatible way.
 * Note that for a normal arduino sketch you won't have to include any
 * of the contiki-specific files here, the sketch should just work.
 */

extern "C" {
#include <stdio.h>
#include "led_pwm.h"
#define LED_PIN 5

uint8_t  pwm             = 128;
uint8_t  period_100ms    = 10; /* one second */
uint16_t analog1_voltage = 0;
uint16_t analog5_voltage = 0;
}

void setup (void)
{
    rest_init_engine ();
    rest_activate_resource (&resource_led_pwm);
    rest_activate_resource (&resource_led_period);
    rest_activate_resource (&resource_analog1_voltage);
    rest_activate_resource (&resource_analog5_voltage);
}

void loop (void)
{
    /* Use 255 - pwm, LED on merkur-board is wired to +3.3V */
    analogWrite (LED_PIN, 255 - pwm);
    analog1_voltage = analogRead (1) * 1600L / 1023L;
    analog5_voltage = analogRead (5) * 1600L / 1023L;
    printf ("clock : %u\nmillis: %lu\n", clock_time (), millis ());
    delay (period_100ms * 100);
    analogWrite (LED_PIN, 255); /* OFF: LED on merkur-board is wired to +3.3V */
    delay (period_100ms * 100);
}
