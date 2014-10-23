/**
 * \file
 *      Resource for Arduino analog read
 * \author
 *      Harald Pichler <harald@the-develop.net>
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
#include "resource_water.h"
#include "Arduino.h"

size_t
water_v (const char *name, uint8_t is_json, char *buf, size_t bufsize)
{
  water_voltage = analogRead(water_pin);            
  return snprintf
    (buf, bufsize, "%d", water_voltage);
}

GENERIC_RESOURCE \
    ( water, METHOD_GET
    , "water/v"
    , Moisture voltage
    , V
    , NULL
    , water_v
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */
