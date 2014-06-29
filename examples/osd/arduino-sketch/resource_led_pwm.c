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
#include "led_pwm.h"

void pwm_from_string (const char *name, const char *s)
{
    uint32_t tmp = strtoul (s, NULL, 10);
    if (tmp > 255) {
      tmp = 255;
    }
    pwm = tmp;
}

size_t
pwm_to_string (const char *name, uint8_t is_json, char *buf, size_t bufsize)
{
  return snprintf (buf, bufsize, "%d", pwm);
}

GENERIC_RESOURCE \
    ( led_pwm, METHOD_GET | METHOD_PUT
    , "led/pwm"
    , LED PWM
    , duty-cycle
    , pwm_from_string
    , pwm_to_string
    );

void period_from_string (const char *name, const char *s)
{
    uint32_t tmp = (strtoul (s, NULL, 10) + 50) / 100;
    if (tmp > 10) {
        tmp = 10;
    }
    if (tmp == 0) {
        tmp = 1;
    }
    period_100ms = tmp;
}

size_t
period_to_string (const char *name, uint8_t is_json, char *buf, size_t bufsize)
{
  return snprintf (buf, bufsize, "%d", period_100ms * 100);
}

GENERIC_RESOURCE \
    ( led_period, METHOD_GET | METHOD_PUT
    , "led/period"
    , LED Period
    , ms
    , period_from_string
    , period_to_string
    );

size_t
analog1_v (const char *name, uint8_t is_json, char *buf, size_t bufsize)
{
  return snprintf
    (buf, bufsize, "%d.%03d", analog1_voltage / 1000, analog1_voltage % 1000);
}

GENERIC_RESOURCE \
    ( analog1_voltage, METHOD_GET
    , "analog/1"
    , Analog 1 voltage
    , V
    , NULL
    , analog1_v
    );

size_t
analog5_v (const char *name, uint8_t is_json, char *buf, size_t bufsize)
{
  return snprintf
    (buf, bufsize, "%d.%03d", analog5_voltage / 1000, analog5_voltage % 1000);
}

GENERIC_RESOURCE \
    ( analog5_voltage, METHOD_GET
    , "analog/5"
    , Analog 5 voltage
    , V
    , NULL
    , analog5_v
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */
