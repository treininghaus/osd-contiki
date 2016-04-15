/*
 * Copyright (C) 2016, Ralf Schlatterbeck and other contributors.
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
 *      Erbium (Er) Wallclock REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
//FIXME#include "er-coap-engine.h"
//FIXME#include "xtime.h"
//FIXME#include "cron.h"
//FIXME#include "time_resource.h"
//FIXME#include "jsonparse.h"
#include "icosoc.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

void 
hw_init ()
{
  icosoc_leds_dir (0xFFFF);
  icosoc_leds_set (0);
}

PROCESS (wallclock, "Wallclock Example Server");

AUTOSTART_PROCESSES(&wallclock);

#define LOOP_INTERVAL (30 * CLOCK_SECOND)

/*
 * Set led to on or off, we abuse the given pointer to simply carry the
 * on/off flag.
 */
void led_set (void *onoff)
{
    int status = (int)onoff;
    icosoc_leds_set (0xFF * status);
}

extern void print_clocks (void);

PROCESS_THREAD(wallclock, ev, data)
{
  static struct etimer loop_periodic_timer;
  PROCESS_BEGIN();
  PRINTF("Starting Wallclock Example\n");

#ifdef RF_CHANNEL
  PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
  PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

  /* Initialize the Hardware. */
  hw_init ();
  /* Initialize the REST engine. */
  //FIXME rest_init_engine ();

  //FIXME rest_activate_resource (&res_timestamp, "clock/timestamp");
  //FIXME rest_activate_resource (&res_timezone, "clock/timezone");
  //FIXME rest_activate_resource (&res_localtime, "clock/localtime");
  //FIXME rest_activate_resource (&res_utc, "clock/utc");

  /* Register callback function(s) */
  //FIXME cron_register_command ("led_on",  led_set, (void *)1);
  //FIXME cron_register_command ("led_off", led_set, (void *)0);

  /* Allocate all cron entries and the necessary resources */
  //FIXME activate_cron_resources ();

  /* Define application-specific events here.
   * We need to call cron every 30 seconds or so (at least once a
   * minute)
   */
  etimer_set (&loop_periodic_timer, LOOP_INTERVAL);
  while (1) {
    printf ("In while loop\n");
    print_clocks ();
    PROCESS_WAIT_EVENT();
    if (etimer_expired (&loop_periodic_timer)) {
        //cron ();
        etimer_reset (&loop_periodic_timer);
    }
  } /* while (1) */

  PROCESS_END();
}
