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
#include "resource_door.h"
#include "Arduino.h"

size_t
door_v (const char *name, uint8_t is_json, char *buf, size_t bufsize)
{
  door_value = digitalRead(door_pin);            
  return snprintf
    (buf, bufsize, "%d", door_value);
}

GENERIC_RESOURCE \
    ( door, METHOD_GET
    , "door/v"
    , Moisture value
    , V
    , NULL
    , door_v
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */
