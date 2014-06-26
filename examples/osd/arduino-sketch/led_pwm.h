/**
 * \defgroup Arduino LED PWM example
 *
 * Resource definition for Arduino LED PWM module
 *
 * @{
 */

/**
 * \file
 *         Resource definitions for the Arduino LED PWM module
 *      
 * \author
 *         Ralf Schlatterbeck <rsc@tux.runtux.com>
 */

#ifndef led_pwm_h
#define led_pwm_h
#include "contiki.h"
#include "contiki-net.h"
#include "erbium.h"
#include "er-coap-13.h"

extern uint8_t pwm;
extern uint8_t period_100ms;
extern resource_t resource_led_pwm;
extern resource_t resource_led_period;

#endif // led_pwm_h
/** @} */
