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

#ifndef moisture_h
#define moisture_h
#include "contiki.h"
#include "contiki-net.h"
#include "erbium.h"
#include "er-coap-13.h"

extern uint8_t moisture_pin;
extern uint16_t moisture_voltage;

extern resource_t resource_moisture;

#endif // moisture_h
/** @} */
