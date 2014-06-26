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

uint8_t pwm          = 128;
uint8_t period_100ms = 10; /* one second */
}

void setup (void)
{
    rest_init_engine ();
    rest_activate_resource (&resource_led_pwm);
    rest_activate_resource (&resource_led_period);
}

void loop (void)
{
    static uint8_t last_pwm = 0;
    if (last_pwm != pwm) {
        last_pwm = pwm;
        analogWrite (LED_PIN, pwm);
        printf
            ( "TCNT3: %04X TCCR3A: %04X TCCR3B: %04X TCCR3C: %04X OCR3C: %04X\n"
            , TCNT3, TCCR3A, TCCR3B, TCCR3C, OCR3C
            );
    }

    // Originally I wanted to sleep here to make the LED blink.
    // Sleeping currently doesn't work, something turns off the chip.
    // Maybe a mechanism to guard agains proto-threads taking too long?
    //clock_wait (CLOCK_SECOND * period_100ms / 10);
    //analogWrite (LED_PIN, 0);
    //printf ("After write\n");
}
