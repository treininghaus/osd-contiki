/*
 * Copyright (c) 2011, Matthias Kovatsch and other contributors.
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
 *      Erbium (Er) REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "contiki.h"
#include "contiki-net.h"


/* Define which resources to include to meet memory constraints. */
#define REST_RES_INFO 1
#define REST_RES_DS1820 1
#define REST_RES_DHT11 1
#define REST_RES_DHT11TEMP 0
#define REST_RES_LEDS 1
#define REST_RES_TOGGLE 0
#define REST_RES_BATTERY 1

#include "erbium.h"

#if REST_RES_DS1820
#include "dev/ds1820.h"
#endif
#if REST_RES_DHT11
#include "dev/dht11.h"
uint16_t dht11_temp=0, dht11_hum=0;
#endif

#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LEDS)
#include "dev/leds.h"
#endif
#if defined (PLATFORM_HAS_TEMPERATURE)
#include "dev/temperature-sensor.h"
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif
#if defined (PLATFORM_HAS_SHT11)
#include "dev/sht11-sensor.h"
#endif


/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#elif WITH_COAP == 12
#include "er-coap-12.h"
#elif WITH_COAP == 13
#include "er-coap-13.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif


/******************************************************************************/

#if REST_RES_INFO
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(info, METHOD_GET, "info", "title=\"Info\";rt=\"text\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
info_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"version\" : \"V0.4.3\",\n");
     index += sprintf(message + index," \"name\" : \"6lowpan-climate\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
#endif

#if REST_RES_DS1820
/* A simple getter example. Returns the reading from ds1820 sensor */
#define DS1820_TEMP_LSB                0
#define DS1820_TEMP_MSB                1
#define DS1820_COUNT_REMAIN    6
#define DS1820_COUNT_PER_C     7
RESOURCE(ds1820, METHOD_GET, "sensors/temp", "title=\"Temperatur DS1820\";rt=\"temperature-c\"");
void
ds1820_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  char message[100];
  int length = 0; /*           |<-------->| */
  union temp_raw {
         int16_t  s_int16;
         uint16_t u_int16;
  } temp_raw;
  double temp_c;
  int temp_integral;
  int temp_centi;

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  // temp = temp_read - 0.25°C + (count_per_c - count_remain) / count_per_c;
  temp_raw.u_int16 = ds1820_ok[DS1820_TEMP_MSB] << 8 | ds1820_ok[DS1820_TEMP_LSB];
  temp_c = temp_raw.s_int16 / 2.0
          - 0.25
          + ((double) ds1820_ok[DS1820_COUNT_PER_C] - (double) ds1820_ok[DS1820_COUNT_REMAIN])
            / (double) ds1820_ok[DS1820_COUNT_PER_C];
  temp_integral = (int) temp_c;
  temp_centi = (int) (fabs (temp_c - (int) temp_c) * 100.0);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf(message, REST_MAX_CHUNK_SIZE, "%d.%02d C", temp_integral, temp_centi);

    length = strlen(message);
    memcpy(buffer, message,length );

    REST.set_response_payload(response, buffer, length);
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf(message, REST_MAX_CHUNK_SIZE, "{\"temp\":\"%d.%02d\"}", temp_integral, temp_centi);

    length = strlen(message);
    memcpy(buffer, message,length );

    REST.set_response_payload(response, buffer, length);
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    REST.set_response_payload(response, (uint8_t *)"Supporting content-types text/plain and application/json", 56);
  }
}
#endif //REST_RES_DS1820

#if REST_RES_DHT11TEMP
/*A simple getter example. Returns the reading from dhtxx sensor*/
RESOURCE(dht11temp, METHOD_GET, "sensors/temp", "title=\"Temperatur DHTxx\";rt=\"temperature-c\"");
void
dht11temp_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int length = 0; /*           |<-------->| */

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf(message, REST_MAX_CHUNK_SIZE, "%4d",dht11_temp);

    length = strlen(message);
    memcpy(buffer, message,length );

    REST.set_response_payload(response, buffer, length);
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf(message, REST_MAX_CHUNK_SIZE, "{\"temp\":\"%4d\"}",dht11_temp);

    length = strlen(message);
    memcpy(buffer, message,length );

    REST.set_response_payload(response, buffer, length);
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    REST.set_response_payload(response, (uint8_t *)"Supporting content-types text/plain and application/json", 56);
  }	
}
#endif //REST_RES_DHT11TEMP

#if REST_RES_DHT11
/*A simple getter example. Returns the reading from dhtxx sensor*/
RESOURCE(dht11, METHOD_GET, "sensors/hum", "title=\"Humidity DHTxx\";rt=\"humidity-%\"");
void
dht11_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int length = 0; /*           |<-------->| */

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf(message, REST_MAX_CHUNK_SIZE, "%2d",dht11_hum);

    length = strlen(message);
    memcpy(buffer, message,length );

    REST.set_response_payload(response, buffer, length);
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf(message, REST_MAX_CHUNK_SIZE, "{\"hum\":\"%d\"}",dht11_hum);

    length = strlen(message);
    memcpy(buffer, message,length );

    REST.set_response_payload(response, buffer, length);
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    REST.set_response_payload(response, (uint8_t *)"Supporting content-types text/plain and application/json", 56);
  }
}
#endif //REST_RES_DHT11

/******************************************************************************/
#if defined (PLATFORM_HAS_LEDS)
/******************************************************************************/
#if REST_RES_LEDS
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(leds, METHOD_POST | METHOD_PUT , "actuators/leds", "title=\"LEDs: ?color=r|g|b, POST/PUT mode=on|off\";rt=\"Control\"");

void
leds_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *color = NULL;
  const char *mode = NULL;
  uint8_t led = 0;
  int success = 1;

  if ((len=REST.get_query_variable(request, "color", &color))) {
    PRINTF("color %.*s\n", len, color);

    if (strncmp(color, "r", len)==0) {
      led = LEDS_RED;
    } else if(strncmp(color,"g", len)==0) {
      led = LEDS_GREEN;
    } else if (strncmp(color,"b", len)==0) {
      led = LEDS_BLUE;
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
    PRINTF("mode %s\n", mode);

    if (strncmp(mode, "on", len)==0) {
      leds_on(led);
    } else if (strncmp(mode, "off", len)==0) {
      leds_off(led);
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif

/******************************************************************************/
#if REST_RES_TOGGLE
/* A simple actuator example. Toggles the red led */
RESOURCE(toggle, METHOD_GET | METHOD_PUT | METHOD_POST, "actuators/toggle", "title=\"Red LED\";rt=\"Control\"");
void
toggle_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  leds_toggle(LEDS_RED);
}
#endif
#endif /* PLATFORM_HAS_LEDS */

/******************************************************************************/
#if REST_RES_BATTERY && defined (PLATFORM_HAS_BATTERY)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(battery, METHOD_GET, "sensors/battery", "title=\"Battery status\";rt=\"battery-mV\"");
void
battery_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int battery = battery_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", battery);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'battery':%d}", battery);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
#endif /* PLATFORM_HAS_BATTERY */

void 
hw_init()
{
#if defined (PLATFORM_HAS_LEDS)
 leds_off(LEDS_RED);
#endif
#if REST_RES_DS1820
  ds1820_temp();
#endif
#if REST_RES_DHT11
  //DHT_INIT();
  DHT_Read_Data(&dht11_temp, &dht11_hum);
#endif
}
#define MESURE_INTERVAL		(20 * CLOCK_SECOND)
#define READ_TIME		( 2 * CLOCK_SECOND)

PROCESS(rest_server_example, "Erbium Example Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data)
{
  static struct etimer ds_periodic_timer;
#if REST_RES_DS1820
  static struct etimer ds_read_timer;
#endif

  PROCESS_BEGIN();

  PRINTF("Starting Erbium Example Server\n");

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

/* if static routes are used rather than RPL */
#if !UIP_CONF_IPV6_RPL && !defined (CONTIKI_TARGET_MINIMAL_NET) && !defined (CONTIKI_TARGET_NATIVE)
  set_global_address();
  configure_routing();
#endif

  /* Initialize the OSD Hardware. */
  hw_init();

  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
#if REST_RES_DS1820
  rest_activate_resource(&resource_ds1820);
#endif
#if REST_RES_DHT11
  rest_activate_resource(&resource_dht11);
#endif
#if REST_RES_DHT11TEMP
  rest_activate_resource(&resource_dht11temp);
#endif
#if REST_RES_INFO
  rest_activate_resource(&resource_info);
#endif
#if defined (PLATFORM_HAS_LEDS)
#if REST_RES_LEDS
  rest_activate_resource(&resource_leds);
#endif
#if REST_RES_TOGGLE
  rest_activate_resource(&resource_toggle);
#endif
#endif /* PLATFORM_HAS_LEDS */
#if defined (PLATFORM_HAS_TEMPERATURE) && REST_RES_TEMPERATURE
  SENSORS_ACTIVATE(temperature_sensor);
  rest_activate_resource(&resource_temperature);
#endif
#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&resource_battery);
#endif

  /* Define application-specific events here. */
  etimer_set(&ds_periodic_timer, MESURE_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT();
#if defined (PLATFORM_HAS_BUTTON)
    if (ev == sensors_event && data == &button_sensor) {
      PRINTF("BUTTON\n");
    }
#endif /* PLATFORM_HAS_BUTTON */
    if(etimer_expired(&ds_periodic_timer)) {
        PRINTF("Periodic\n");
        etimer_reset(&ds_periodic_timer);
#if REST_RES_DHT11
    //    DHT_Read_Data(&dht11_temp, &dht11_hum);
        DHT_Read_Data(&dht11_temp, &dht11_hum);
#endif
#if REST_RES_DS1820
        if(ds1820_convert()){
          etimer_set(&ds_read_timer, READ_TIME);
        }
#endif
    }
#if REST_RES_DS1820
    if(etimer_expired(&ds_read_timer)) {
        PRINTF("DS1820_Read\n");
        ds1820_read();
    }
#endif
  } /* while (1) */

  PROCESS_END();
}
