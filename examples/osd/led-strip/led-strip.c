/*
 * Copyright (c) 2014-15, Ralf Schlatterbeck Open Source Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      LED-strip driver for Seeed Studio LED-Strip
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "generic_resource.h"

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the
 * corresponding sub-directory.
 */
 
#if PLATFORM_HAS_BATTERY
#include "dev/battery-sensor.h"
extern resource_t res_battery;
#endif

#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
extern resource_t res_radio;
#endif

#include "LED_Strip_Suli.h"

int color_rgb [3] = {255, 0, 255};

static uint8_t name_to_offset (const char * name)
{
  uint8_t offset = 0;
  if (0 == strcmp (name, "green")) {
    offset = 1;
  } else if (0 == strcmp (name, "blue")) {
    offset = 2;
  }
  return offset;
}

static size_t
color_to_string (const char *name, uint8_t is_json, char *buf, size_t bsize)
{
  char *fmt = "%d";
  if (is_json) {
    fmt = "\"%d\"";
  }
  return snprintf (buf, bsize, fmt, 255 - color_rgb [name_to_offset (name)]);
}

void color_from_string (const char *name, const char *s)
{
  color_rgb [name_to_offset (name)] = 255 - atoi (s);
  led_strip_begin ();
  led_strip_set_color (color_rgb [0], color_rgb [1], color_rgb [2]);
  led_strip_end ();
  printf ("Set to R:%d G:%d B:%d\n"
         , color_rgb [0], color_rgb [1], color_rgb [2])
         ;
}

GENERIC_RESOURCE
  ( red
  , RED_LED
  , s
  , color_from_string
  , color_to_string
  );

GENERIC_RESOURCE
  ( green
  , GREEN_LED
  , s
  , color_from_string
  , color_to_string
  );

GENERIC_RESOURCE
  ( blue
  , BLUE_LED
  , s
  , color_from_string
  , color_to_string
  );


PROCESS(led_strip, "LED Strip Example");
AUTOSTART_PROCESSES(&led_strip);

PROCESS_THREAD(led_strip, ev, data)
{

  PROCESS_BEGIN();

  /* Initialize the REST engine. */
  rest_init_engine ();
  printf ("Initialized\n");
  led_strip_init   (3, 14);
  led_strip_begin ();
  led_strip_set_color (color_rgb [0], color_rgb [1], color_rgb [2]);
  led_strip_end ();
  printf ("Set to R:%d G:%d B:%d\n"
         , color_rgb [0], color_rgb [1], color_rgb [2])
         ;

  /* Activate the application-specific resources. */
#if PLATFORM_HAS_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource (&res_battery,"s/battery");
#endif
  rest_activate_resource (&res_red,   "led/R");
  rest_activate_resource (&res_green, "led/G");
  rest_activate_resource (&res_blue,  "led/B");

  /* Define application-specific events here. */
  /* Don't do anything for now, everything done in resources */
  while(1) {
    PROCESS_WAIT_EVENT();
  } /* while (1) */

  PROCESS_END();
}
