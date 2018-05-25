/*
 * Copyright (c) 2013, Matthias Kovatsch
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

/*
From:
http://tools.ietf.org/id/draft-ietf-core-interfaces-01.txt

Appendix A.  Profile example

   The following is a short definition of simple profile.  This
   simplistic profile is for use in the examples of this document.

         +--------------------+-----------+------------+---------+
         |       Function Set | Root Path | RT         | IF      |
         +--------------------+-----------+------------+---------+
         | Device Description | /d        | simple.dev | core.ll |
         |            Sensors | /s        | simple.sen | core.b  |
         |          Actuators | /a        | simple.act | core.b  |
         +--------------------+-----------+------------+---------+

                           List of Function Sets

       +-------+----------+----------------+---------+------------+
       |  Type | Path     | RT             | IF      | Data Type  |
       +-------+----------+----------------+---------+------------+
       |  Name | /d/name  | simple.dev.n   | core.p  | xsd:string |
       | Model | /d/model | simple.dev.mdl | core.rp | xsd:string |
       +-------+----------+----------------+---------+------------+

                      Device Description Function Set

   +-------------+-------------+----------------+--------+-------------+
   |        Type | Path        | RT             | IF     | Data Type   |
   +-------------+-------------+----------------+--------+-------------+
   |       Light | /s/light    | simple.sen.lt  | core.s | xsd:decimal |
   |             |             |                |        | (lux)       |
   |    Humidity | /s/humidity | simple.sen.hum | core.s | xsd:decimal |
   |             |             |                |        | (%RH)       |
   | Temperature | /s/temp     | simple.sen.tmp | core.s | xsd:decimal |
   |             |             |                |        | (degC)      |
   +-------------+-------------+----------------+--------+-------------+

                           Sensors Function Set

       +------+------------+----------------+--------+-------------+
       | Type | Path       | RT             | IF     | Data Type   |
       +------+------------+----------------+--------+-------------+
       |  LED | /a/{#}/led | simple.act.led | core.a | xsd:boolean |
       +------+------------+----------------+--------+-------------+

                          Actuators Function Set
*/

/**
 * \file
 *      Derived from Erbium (Er) REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 * \file
 *      OSD-irrigation example application
 * \author
 *      Thomas Reininghaus <thomas.reininghaus@iot-austria.at>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "time.h"
#include "time_resource.h"
#include "jsonparse.h"
#include <avr/eeprom.h>
#include "erbium.h"


/* Define which resources to include to meet memory constraints. */

/* <OSD-irrigation> */
#define REST_RES_WATERING 1
#if defined (PLATFORM_HAS_RELAY)
#define REST_RES_RELAY 1
#endif
#define REST_RES_BUTTON 1
/* </OSD-irrigation> */
/* Standard from er-example-server.c */
#define REST_RES_MODEL 1
#define REST_RES_NAME 1
#define REST_RES_SW   1
#define REST_RES_EVENT 1
#define REST_RES_LED 1
#define REST_RES_TOGGLE 1
#define REST_RES_BATTERY 1
#define REST_RES_TEMPERATURE 1

/* <OSD-irrigation> */
#if defined (PLATFORM_HAS_RELAY)
#include "dev/relay-sensor.h"
#endif
/* </OSD-irrigation> */
#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LED)
#include "dev/leds.h"
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif
#if defined (PLATFORM_HAS_TEMPERATURE)
#include "dev/temperature-sensor.h"
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

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/* <OSD-irrigation> */

/******************************************************************************/
#if REST_RES_WATERING
/*
* Resources are defined by the RESOURCE macro.
* Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
*/

RESOURCE(wat_tods, METHOD_POST | METHOD_GET, "a/wat_tods", "title=\"Watering start time of day, POST tods=hh:mm\";rt=\"Control\"");

#define P_WAT_TODS "13:00"
#define P_WAT_TODS_MAX 6
char s_wat_tods[P_WAT_TODS_MAX] = P_WAT_TODS;

/* eeprom space */
uint8_t eemem_p_wat_tods[P_WAT_TODS_MAX] EEMEM = P_WAT_TODS;

/*
* A handler function named [resource name]_handler must be implemented for each RESOURCE.
* A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
* preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
* If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
*/

void
wat_tods_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint8_t eebuffer[32];
  char message[100];
  int index = 0;
  int length = 0; /* |<-------->| */
  const char *wat_tods = NULL;
  int success = 1;

  switch(REST.get_method_type(request)){
    case METHOD_GET:
      cli();
      eeprom_read_block (eebuffer, &eemem_p_wat_tods, sizeof(eemem_p_wat_tods));
      sei();

      length = sizeof(eemem_p_wat_tods);
      memcpy(&s_wat_tods, eebuffer, length);
      PRINTF("Getting Watering Time of Day %s\n", s_wat_tods);

      /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
      // jSON Format
      index += sprintf(message + index,"{\n \"wat_tods\" : \"%s\"\n", eebuffer);
      index += sprintf(message + index,"}\n");

      length = strlen(message);
      memcpy(buffer, message, length);

      REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
      REST.set_response_payload(response, buffer, length);
      break;

    case METHOD_POST:
      if (success && (length=REST.get_post_variable(request, "tods", &wat_tods))) {
        if (length < P_WAT_TODS_MAX) {
          memcpy(&eebuffer, wat_tods, length);
          eebuffer[length]=0;
          cli();
          eeprom_write_block(&eebuffer, &eemem_p_wat_tods, sizeof(eemem_p_wat_tods));
          sei();

          memcpy(&s_wat_tods, wat_tods, length);
          PRINTF("Setting Watering Time of Day %s\n", s_wat_tods);

        } else {
          success = 0;
        }
      } else {
      success = 0;
    }
    break;
    default:
    success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}

RESOURCE(wat_dur, METHOD_POST | METHOD_GET, "a/wat_dur", "title=\"Watering duration sec., POST time=XXX\";rt=\"Control\"");

#define P_WAT_DUR "10"
#define P_WAT_DUR_MAX 6
int i_wat_dur = 0;

/* eeprom space */
uint8_t eemem_p_wat_dur[P_WAT_DUR_MAX] EEMEM = P_WAT_DUR;

/*
* A handler function named [resource name]_handler must be implemented for each RESOURCE.
* A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
* preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
* If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
*/

void
wat_dur_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint8_t eebuffer[32];
  char message[100];
  int index = 0;
  int length = 0; /* |<-------->| */
  const char *wat_dur = NULL;
  int success = 1;

  switch(REST.get_method_type(request)){
    case METHOD_GET:
      cli();
      eeprom_read_block (eebuffer, &eemem_p_wat_dur, sizeof(eemem_p_wat_dur));
      sei();
      /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
      // jSON Format
      index += sprintf(message + index,"{\n \"wat_dur\" : \"%s\"\n", eebuffer);
      index += sprintf(message + index,"}\n");

      length = strlen(message);
      memcpy(buffer, message, length);
      PRINTF("Getting Watering Duration %s\n", buffer);

      REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
      REST.set_response_payload(response, buffer, length);
      break;

    case METHOD_POST:
      if (success && (length=REST.get_post_variable(request, "time", &wat_dur))) {
        PRINTF("Setting Watering Duration %s\n", wat_dur);
        if (length < P_WAT_DUR_MAX) {
          memcpy(&eebuffer, wat_dur, length);
          eebuffer[length]=0;
          cli();
          eeprom_write_block(&eebuffer, &eemem_p_wat_dur, sizeof(eemem_p_wat_dur));
          sei();

          i_wat_dur = atoi( wat_dur);

        } else {
          success = 0;
        }
      } else {
      success = 0;
    }
    break;
    default:
    success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif /* REST_RES_WATERING */

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
  /* REST_MAX_CHUNK_SIZE seems to be 64 */
       // jSON Format
     index += sprintf(message + index,"{\n \"Version\" : \"V1.0pre0\",\n");
     index += sprintf(message + index," \"name\" : \"OSD-irrigation\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
#endif

/******************************************************************************/
#if REST_RES_RELAY
/*A simple actuator example*/
RESOURCE(relay, METHOD_GET | METHOD_POST | METHOD_PUT , "a/relay", "title=\"RELAY: ?type=1|2|3|4, POST/PUT, POST/PUT mode=on|off\";rt=\"Control\"");

void
relay_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *type = NULL;
  const char *mode = NULL;
  static char name1[10]="R1";
  static char name2[10]="R2";
  static char name3[10]="R3";
  static char name4[10]="R4";

  char temp[100];
  int index = 0;
  size_t len = 0;
  int relay = 0;
  int success = 1;

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     // jSON Format
     index += sprintf(temp + index,"{\n \"%s\" : ",name1);
     if(relay_sensor.value(RELAY_SENSOR_1) == 0)
         index += sprintf(temp + index,"\"off\",\n");
     if(relay_sensor.value(RELAY_SENSOR_1) == 1)
         index += sprintf(temp + index,"\"on\",\n");
     index += sprintf(temp + index," \"%s\" : ",name2);
     if(relay_sensor.value(RELAY_SENSOR_2) == 0)
         index += sprintf(temp + index,"\"off\",\n");
     if(relay_sensor.value(RELAY_SENSOR_2) == 1)
         index += sprintf(temp + index,"\"on\",\n");
     index += sprintf(temp + index," \"%s\" : ",name3);
     if(relay_sensor.value(RELAY_SENSOR_3) == 0)
         index += sprintf(temp + index,"\"off\",\n");
     if(relay_sensor.value(RELAY_SENSOR_3) == 1)
         index += sprintf(temp + index,"\"on\",\n");
     index += sprintf(temp + index," \"%s\" : ",name4);
     if(relay_sensor.value(RELAY_SENSOR_4) == 0)
         index += sprintf(temp + index,"\"off\"\n");
     if(relay_sensor.value(RELAY_SENSOR_4) == 1)
         index += sprintf(temp + index,"\"on\"\n");
     index += sprintf(temp + index,"}\n");

     len = strlen(temp);
     memcpy(buffer, temp,len );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, len);
     break;

   case METHOD_POST:
     success = 0;
     break;
   case METHOD_PUT:
      if ((len=REST.get_query_variable(request, "type", &type))) {
        PRINTF("type %.*s\n", len, type);
        if (strncmp(type, "1", len)==0) {
          relay = RELAY_SENSOR_1;
        } else if(strncmp(type,"2", len)==0) {
          relay = RELAY_SENSOR_2;
        } else if(strncmp(type,"3", len)==0) {
          relay = RELAY_SENSOR_3;
        } else if(strncmp(type,"4", len)==0) {
          relay = RELAY_SENSOR_4;
        } else {
          success = 0;
        }
      } else {
        success = 0;
      }

      if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
        PRINTF("mode %s\n", mode);

        if (strncmp(mode, "on", len)==0) {
          relay_sensor.configure(relay,1);
        } else if (strncmp(mode, "off", len)==0) {
          relay_sensor.configure(relay,0);
        } else {
          success = 0;
        }
      } else {
        success = 0;
      }
    break;
  default:
    success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif /* REST_RES_RELAY */

/* </OSD-irrigation> */

/******************************************************************************/
#if REST_RES_MODEL
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(model, METHOD_GET, "p/model", "title=\"model\";rt=\"simple.dev.mdl\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
model_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"model\" : \"Merkurboard\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
#endif

/******************************************************************************/
#if REST_RES_SW
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(sw, METHOD_GET, "p/sw", "title=\"Software Version\";rt=\"simple.dev.sv\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
sw_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
       // jSON Format
     index += sprintf(message + index,"{\n \"sw\" : \"V0.1\"\n");
     index += sprintf(message + index,"}\n");

    length = strlen(message);
    memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  REST.set_response_payload(response, buffer, length);
}
#endif

/******************************************************************************/
#if REST_RES_NAME
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(name,  METHOD_POST | METHOD_GET, "p/name", "title=\"name\";rt=\"simple.dev.n\"");
/* eeprom space */
#define P_NAME "OSD-irrigation"
#define P_NAME_MAX 17
uint8_t eemem_p_name[P_NAME_MAX] EEMEM = P_NAME;

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
name_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint8_t eebuffer[32];
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  const char *name = NULL;
  int success = 1;

  switch(REST.get_method_type(request)){
   case METHOD_GET:
     cli();
     eeprom_read_block (eebuffer, &eemem_p_name, sizeof(eemem_p_name));
     sei();
     /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
     // jSON Format
     index += sprintf(message + index,"{\n \"name\" : \"%s\"\n",eebuffer);
     index += sprintf(message + index,"}\n");

     length = strlen(message);
     memcpy(buffer, message,length );

     REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
     REST.set_response_payload(response, buffer, length);
     break;

   case METHOD_POST:
     if (success &&  (length=REST.get_post_variable(request, "name", &name))) {
       PRINTF("name %s\n", name);
       if (length < P_NAME_MAX) {
         memcpy(&eebuffer, name,length);
         eebuffer[length]=0;
         cli();
         eeprom_write_block(&eebuffer,  &eemem_p_name, sizeof(eemem_p_name));
         sei();
       } else {
         success = 0;
       }
     } else {
       success = 0;
     }
     break;
  default:
    success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif

/******************************************************************************/
#if REST_RES_EVENT && defined (PLATFORM_HAS_BUTTON)
/*
 * Example for an event resource.
 * Additionally takes a period parameter that defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions and manages a list of subscribers to notify.
 */
EVENT_RESOURCE(button_event, METHOD_GET, "s/button", "title=\"Irrigation Start/Stop Button\";obs");

void
button_event_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  int button = button_sensor.value(0);

  index += sprintf(message + index,"%d",button);
  length = strlen(message);
  memcpy(buffer, message,length );

  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, buffer, length);

  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}

/* Additionally, a handler function named [resource name]_event_handler must be implemented for each PERIODIC_RESOURCE defined.
 * It will be called by the REST manager process with the defined period. */
void
button_event_event_handler(resource_t *r)
{
  static uint16_t event_counter = 0;
  static char content[12];

  ++event_counter;

  PRINTF("TICK %u for /%s\n", event_counter, r->url);

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_CON, REST.status.OK, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "EVENT %u", event_counter));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, event_counter, notification);
}

/*
 * remote button event resource.
 * Additionally takes a period parameter that defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions and manages a list of subscribers to notify.
 */
EVENT_RESOURCE(remote_button_event, METHOD_POST | METHOD_GET, "a/remote_button", "title=\"Remote button\";rt=\"simple.act.rbutton\";obs");

void
remote_button_event_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char message[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  int remote_button = 0;
  int success = 1;

  switch(REST.get_method_type(request)) {
   case METHOD_GET:
     index += sprintf(message + index,"%d", remote_button);
     length = strlen(message);
     memcpy(buffer, message, length);

     REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
     REST.set_response_payload(response, buffer, length);
     break;
   case METHOD_POST:
     if (success) {
         // METHOD_POST sets the button...
         remote_button = 1;
         PRINTF("remote_button %d\n", remote_button);
         // imitates the "real" button
         sensors_changed(&button_sensor);
         // ...and clears the button
         remote_button = 0;
       } else {
         success = 0;
     }
     break;
  default:
    success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}

/* Additionally, a handler function named [resource name]_event_handler must be implemented for each PERIODIC_RESOURCE defined.
 * It will be called by the REST manager process with the defined period. */
void
remote_button_event_event_handler(resource_t *r)
{
  static uint16_t event_counter = 0;
  static char content[12];

  ++event_counter;

  PRINTF("TICK %u for /%s\n", event_counter, r->url);

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_CON, REST.status.OK, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "EVENT %u", event_counter));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, event_counter, notification);
}

#endif /* PLATFORM_HAS_BUTTON */


/******************************************************************************/
#if defined (PLATFORM_HAS_LED)
/******************************************************************************/
#if REST_RES_LED
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(led, METHOD_POST | METHOD_PUT , "a/led", "title=\"LED: POST/PUT mode=on|off\";rt=\"simple.act.led\"");

void
led_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *mode = NULL;
  uint8_t led = 0;
  int success = 1;

  led = LEDS_RED;

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
RESOURCE(toggle, METHOD_POST, "a/toggle", "title=\"Red LED\";rt=\"Control\"");
void
toggle_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  leds_toggle(LEDS_RED);
}
#endif
#endif /* PLATFORM_HAS_LED */

/******************************************************************************/
#if REST_RES_BATTERY && defined (PLATFORM_HAS_BATTERY)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(battery, METHOD_GET, "s/battery", "title=\"Battery status\";rt=\"Battery\"");
void
battery_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int battery = battery_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", battery/1000, battery % 1000);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\"battery\":%d.%02d}", battery/1000, battery % 1000);

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

/******************************************************************************/
#if REST_RES_TEMPERATURE && defined (PLATFORM_HAS_TEMPERATURE)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(temperature, METHOD_GET, "s/cputemp", "title=\"CPU Temperature\";rt=\"simple.sen.tmp\"");
void
temperature_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int temperature = temperature_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02d", temperature/100, temperature % 100);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'temperature':%d.%02d}", temperature/100, temperature % 100);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
#endif /* PLATFORM_HAS_TEMPERATURE */

int wat_dursec = -1;
int tic = 1;
int relaystate = 0;
// int wat_start = 0;
struct timezone tz;
struct timeval tv;
struct tm tm;
struct tm *(*method)(const time_t *, struct tm *) = gmtime_r;
char tmpstr[2];

void
hw_init()
{
  // uint8_t response[12];

  tz.tz_minuteswest = -120;
  tz.tz_dsttime = 0;
  // wat_tods_handler(METHOD_GET, &response, &s_wat_tods, NULL, NULL)
#if defined (PLATFORM_HAS_LED)
  leds_off(LEDS_ALL);
#endif
}

#define SEC_DIVIDER 8
#define BUTTON_READ_INTERVAL		(CLOCK_SECOND / SEC_DIVIDER)
#define CHECK_INTERVALTIMER_INTERVAL	(CLOCK_SECOND * 60.0)

PROCESS(rest_server_example, "OSD Merkurboard Irrigation Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data)
{
  static struct etimer ds_periodic_timer;
  static struct etimer ds_periodic_min;
  PROCESS_BEGIN();

  PRINTF("Starting OSD Merkurboard Irrigation Server\n");

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

  /* Initialize the OSD Hardware. */
  hw_init();
  /* Initialize the REST engine. */
  rest_init_engine();

/* Activate the application-specific resources. */
#if REST_RES_WATERING
  uint8_t eebuffer[32];
  int length = 0;
  char wat_dur[P_WAT_DUR_MAX] = P_WAT_DUR;

  rest_activate_resource(&resource_wat_dur);
  // cli();
  eeprom_read_block (eebuffer, &eemem_p_wat_dur, sizeof(eemem_p_wat_dur));
  // sei();

  // length = sizeof(eemem_p_wat_dur);
  length = P_WAT_TODS_MAX;
  // PRINTF("Length of wat_dur eeprom var %d\n", length);

  memcpy(&wat_dur, eebuffer, length);
  // das war der Böse, wenn index out of range läufen die Timer gar nicht
  wat_dur[P_WAT_DUR_MAX - 1] = 0;
  PRINTF("Initializing Watering Duration from EEPROM %s\n", wat_dur);
  if ( wat_dur[0] > 0x30 && wat_dur[0] < 0x40)
    i_wat_dur = atoi( wat_dur);

  rest_activate_resource(&resource_wat_tods);
  // cli();
  eeprom_read_block (eebuffer, &eemem_p_wat_tods, sizeof(eemem_p_wat_tods));
  // sei();

  length = sizeof(eemem_p_wat_tods);
  memcpy(&s_wat_tods, eebuffer, length);
  PRINTF("Initializing Watering Time of Day from EEPROM %s\n", s_wat_tods);

#endif
#if REST_RES_INFO
  rest_activate_resource(&resource_info);
#endif
#if REST_RES_RELAY
  SENSORS_ACTIVATE(relay_sensor);
  rest_activate_resource(&resource_relay);
#endif


  /* Activate the application-specific resources. */
#if REST_RES_MODEL
  rest_activate_resource(&resource_model);
#endif
#if REST_RES_SW
  rest_activate_resource(&resource_sw);
#endif
#if REST_RES_NAME
  rest_activate_resource(&resource_name);
#endif
#if defined (PLATFORM_HAS_BUTTON) && REST_RES_EVENT
  rest_activate_event_resource(&resource_button_event);
  rest_activate_event_resource(&resource_remote_button_event);
  SENSORS_ACTIVATE(button_sensor);
#endif
#if defined (PLATFORM_HAS_LED)
#if REST_RES_LED
  rest_activate_resource(&resource_led);
#endif
#if REST_RES_TOGGLE
  rest_activate_resource(&resource_toggle);
#endif
#endif /* PLATFORM_HAS_LED */
#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&resource_battery);
#endif
#if defined (PLATFORM_HAS_TEMPERATURE) && REST_RES_TEMPERATURE
  SENSORS_ACTIVATE(temperature_sensor);
  rest_activate_resource(&resource_temperature);
#endif

  rest_activate_resource(&resource_timestamp);
  rest_activate_resource(&resource_localtime);
  rest_activate_resource(&resource_utc);

  /* Define application-specific events here. */
  etimer_set(&ds_periodic_timer, BUTTON_READ_INTERVAL);
  etimer_set(&ds_periodic_min, CHECK_INTERVALTIMER_INTERVAL);

 while(1) {
    PROCESS_WAIT_EVENT();
    if (etimer_expired(&ds_periodic_timer)) {
       // PRINTF("periodic event timer\n");
       if (tic++ >= 8) {
          tic = 1;
          // if watering timer was initialized, start counting
          if (wat_dursec != -1) {
             wat_dursec++;
             PRINTF("Periodic event timer: %d.\n", wat_dursec);
             leds_toggle(LEDS_ALL);
          }
       }
       // if watering timer reaches timeout value
       if (wat_dursec >= i_wat_dur) {
          // PRINTF("EEPROM value time out %d seconds.\n", i_wat_dur);
          PRINTF("Watering timer timed out after %d seconds.\n", wat_dursec);
          // clear timer
	        wat_dursec = -1;
          // close the relay
          relaystate = 0;
          leds_off(LEDS_ALL);
#if defined (PLATFORM_HAS_RELAY)
          relay_sensor.configure(RELAY_SENSOR_4, relaystate);
#endif
       }
       else if (wat_dursec > -1) {  // check if timer is running...
          if (!relaystate) {         // ... but watering is stopped then it was manually stopped
             PRINTF("Watering was stopped manually.\n");
             // clear timer
             wat_dursec = -1;
             leds_off(LEDS_ALL);
          }
       }
       etimer_reset(&ds_periodic_timer);
    }
    if (etimer_expired(&ds_periodic_min)) {
       gettimeofday(&tv, &tz);
       localtime_r (&tv.tv_sec, &tm);
       PRINTF("Current Time %02u:%02u:%02u.\n", tm.tm_hour + 2, tm.tm_min, tm.tm_sec);
       PRINTF("Watering Time of Day: %s.\n", s_wat_tods);
       PRINTF("Watering Duration: %d.\n", i_wat_dur);
       strncpy(tmpstr, s_wat_tods, 2);
       if (atoi(tmpstr) == tm.tm_hour + 2) {
          PRINTF("Hours equal %02u.\n", tm.tm_hour + 2);
          strncpy(tmpstr, s_wat_tods + 3, 2);
          if (atoi(tmpstr) == tm.tm_min) {
             PRINTF("Minutes equal %02u.\n", tm.tm_min);
             PRINTF("Starting scheduled watering.\n");
             wat_dursec = 0;
             relaystate = 1;
#if defined (PLATFORM_HAS_RELAY)
             relay_sensor.configure(RELAY_SENSOR_4, relaystate);
#endif
             PRINTF("relaystate set to: %d.\n", relaystate);
          }
       }
       etimer_reset(&ds_periodic_min);
    }

#if defined (PLATFORM_HAS_BUTTON)
    if (ev == sensors_event && data == &button_sensor) {
      PRINTF("BUTTON\n");
      // toggle the relay state value
#if defined (PLATFORM_HAS_RELAY)
      relaystate = !relay_sensor.value(RELAY_SENSOR_4);
#else
      relaystate = !relaystate;
#endif
      if (relaystate) {        // if relay shall open
         // start timer by changing value of "seconds running" from -1 to 0
         wat_dursec = 0;
         PRINTF("Timer initialized.\n");
      }
#if defined (PLATFORM_HAS_RELAY)
      relay_sensor.configure(RELAY_SENSOR_4, relaystate);
#endif
      PRINTF("relaystate set to: %d.\n", relaystate);
#if REST_RES_EVENT
      /* Call the event_handler for this application-specific event. */
      button_event_event_handler(&resource_button_event);
#endif
    }
#endif /* PLATFORM_HAS_BUTTON */
  } /* while (1) */

  PROCESS_END();
}
