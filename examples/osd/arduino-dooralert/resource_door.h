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

#ifndef door_h
#define door_h
#include "contiki.h"
#include "contiki-net.h"
#include "erbium.h"
#include "er-coap-13.h"

extern uint8_t door_pin;
extern uint16_t door_value;

extern resource_t resource_door;

#endif // door_h
/** @} */
