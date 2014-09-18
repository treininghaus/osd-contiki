/**
 * \file
 *      Resource for Arduino PWM
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put pwm and period for LED pin
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "jsonparse.h"
/* Only coap 13 for now */
#include "er-coap-13.h"
#include "generic_resource.h"
#include "moisture.h"
#include "Arduino.h"

size_t
moisture_v (const char *name, uint8_t is_json, char *buf, size_t bufsize)
{
  moisture_voltage = analogRead(moisture_pin);            
  return snprintf
    (buf, bufsize, "%d", moisture_voltage);
}

GENERIC_RESOURCE \
    ( moisture, METHOD_GET
    , "moisture/v"
    , Moisture voltage
    , V
    , NULL
    , moisture_v
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */
