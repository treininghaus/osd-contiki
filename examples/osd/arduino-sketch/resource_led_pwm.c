/**
 * \file
 *      Resource for Arduino PWM
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put pwm and period for LED pin
 *
 * quick&dirty implementation, this should factor json parsing.
 * But json format will probably change, there is a draft rfc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "jsonparse.h"
/* Only coap 13 for now */
#include "er-coap-13.h"
#include "led_pwm.h"

/* Error-handling macro */
# define BYE(_exp, _tag) \
   do {                                  \
     PRINTF("Expect "_exp": %d\n",_tag); \
     success=0;                          \
     goto bye;                           \
   } while(0)

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

RESOURCE \
    ( led_pwm, METHOD_GET | METHOD_PUT
    , "led/pwm"
    , "title=\"LED PWM\";rt=\"led pwm\""
    );

void
led_pwm_handler
    ( void* request
    , void* response
    , uint8_t *buffer
    , uint16_t preferred_size
    , int32_t *offset
    )
{
  int success = 1;

  int i;
  char temp[100];
  int index = 0;
  int length = 0;
  int tag = 0;
  const uint8_t *bytes = NULL;
  size_t len = 0;
  int n_acc = 0;
  const uint16_t *accept = NULL;
  uint16_t a_ctype = REST.type.APPLICATION_JSON;
  uint16_t c_ctype = REST.get_header_content_type (request);
  uint32_t tmp = 0;

  /* Seems like accepted type is currently unsupported? */
  n_acc = REST.get_header_accept (request, &accept);
  for (i=0; i<n_acc; i++) {
    if  (  accept [i] == REST.type.TEXT_PLAIN
        || accept [i] == REST.type.APPLICATION_JSON
        )
    {
      a_ctype = accept [i];
      break;
    }
  }

  switch(REST.get_method_type(request)) {
    case METHOD_GET:
      // TEXT format
      if (a_ctype == REST.type.TEXT_PLAIN) {
        index += sprintf (temp + index, "%d\n", pwm);
      } else { // jSON Format
        index += sprintf
          ( temp + index
          ,"{\n \"pwm\" : \"%d\"\n}\n"
          , pwm
          );
      }
      length = strlen(temp);
      memcpy (buffer, temp, length);
      REST.set_header_content_type (response, a_ctype);
      REST.set_response_payload (response, buffer, length);

      break;
    case METHOD_PUT:
      if ((len = coap_get_payload(request, &bytes))) {
        PRINTF ("PUT: len: %d, %s\n", len, (char *)bytes);
        if (c_ctype == REST.type.TEXT_PLAIN) {
          temp [sizeof (temp) - 1] = 0;
          strncpy (temp, (char *)bytes, MIN (len + 1, sizeof (temp) - 1));
        } else { // jSON Format
          struct jsonparse_state state;
          struct jsonparse_state *parser = &state;
          jsonparse_setup (parser, (char *)bytes, len);
          if ((tag = jsonparse_next (parser)) != JSON_TYPE_OBJECT) {
            BYE ("OBJECT", tag);
          }
          if ((tag = jsonparse_next (parser)) != JSON_TYPE_PAIR_NAME) {
            BYE ("PAIR_NAME", tag);
          }
          while (jsonparse_strcmp_value (parser, "pwm") != 0) {
            tag = jsonparse_next (parser);
            if (tag != JSON_TYPE_PAIR) {
              BYE ("PAIR", tag);
            }
            tag = jsonparse_next (parser);
            tag = jsonparse_next (parser);
            if (tag != ',') {
              BYE (",", tag);
            }
            tag = jsonparse_next (parser);
            if (tag != JSON_TYPE_PAIR_NAME) {
              BYE ("PAIR_NAME", tag);
            }
          }
          tag = jsonparse_next (parser);
          if (tag != JSON_TYPE_PAIR) {
            BYE ("PAIR", tag);
          }
          tag = jsonparse_next (parser);
          if (tag != JSON_TYPE_STRING) {
            BYE ("STRING", tag);
          }
          jsonparse_copy_value (parser, temp, sizeof (temp));
          temp [sizeof (temp) - 1] = 0;
        }
        PRINTF ("GOT: %s\n", temp);
        tmp = strtoul (temp, NULL, 10);
        if (tmp > 255) {
            tmp = 255;
        }
        pwm = tmp;
        PRINTF ("Setting: %d\n", pwm);
        REST.set_response_status(response, REST.status.CHANGED);
      } else {
        PRINTF ("PUT: len: %d\n", len);
        success = 0;
      }
      bye :
      break;
    default:
      success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}

RESOURCE \
    ( led_period, METHOD_GET | METHOD_PUT
    , "led/period"
    , "title=\"LED Period\";rt=\"led period\""
    );

void
led_period_handler
    ( void* request
    , void* response
    , uint8_t *buffer
    , uint16_t preferred_size
    , int32_t *offset
    )
{
  int success = 1;

  int i;
  char temp[100];
  int index = 0;
  int length = 0;
  int tag = 0;
  const uint8_t *bytes = NULL;
  size_t len = 0;
  int n_acc = 0;
  const uint16_t *accept = NULL;
  uint16_t a_ctype = REST.type.APPLICATION_JSON;
  uint16_t c_ctype = REST.get_header_content_type (request);
  uint32_t tmp = 0;

  /* Seems like accepted type is currently unsupported? */
  n_acc = REST.get_header_accept (request, &accept);
  for (i=0; i<n_acc; i++) {
    if  (  accept [i] == REST.type.TEXT_PLAIN
        || accept [i] == REST.type.APPLICATION_JSON
        )
    {
      a_ctype = accept [i];
      break;
    }
  }

  switch(REST.get_method_type(request)) {
    case METHOD_GET:
      // TEXT format
      if (a_ctype == REST.type.TEXT_PLAIN) {
        index += sprintf (temp + index, "%d\n", period_100ms * 100);
      } else { // jSON Format
        index += sprintf
          ( temp + index
          ,"{\n \"period\" : \"%d\"\n}\n"
          , period_100ms * 100
          );
      }
      length = strlen(temp);
      memcpy (buffer, temp, length);
      REST.set_header_content_type (response, a_ctype);
      REST.set_response_payload (response, buffer, length);

      break;
    case METHOD_PUT:
      if ((len = coap_get_payload(request, &bytes))) {
        PRINTF ("PUT: len: %d, %s\n", len, (char *)bytes);
        if (c_ctype == REST.type.TEXT_PLAIN) {
          temp [sizeof (temp) - 1] = 0;
          strncpy (temp, (char *)bytes, MIN (len + 1, sizeof (temp) - 1));
        } else { // jSON Format
          struct jsonparse_state state;
          struct jsonparse_state *parser = &state;
          jsonparse_setup (parser, (char *)bytes, len);
          if ((tag = jsonparse_next (parser)) != JSON_TYPE_OBJECT) {
            BYE ("OBJECT", tag);
          }
          if ((tag = jsonparse_next (parser)) != JSON_TYPE_PAIR_NAME) {
            BYE ("PAIR_NAME", tag);
          }
          while (jsonparse_strcmp_value (parser, "period") != 0) {
            tag = jsonparse_next (parser);
            if (tag != JSON_TYPE_PAIR) {
              BYE ("PAIR", tag);
            }
            tag = jsonparse_next (parser);
            tag = jsonparse_next (parser);
            if (tag != ',') {
              BYE (",", tag);
            }
            tag = jsonparse_next (parser);
            if (tag != JSON_TYPE_PAIR_NAME) {
              BYE ("PAIR_NAME", tag);
            }
          }
          tag = jsonparse_next (parser);
          if (tag != JSON_TYPE_PAIR) {
            BYE ("PAIR", tag);
          }
          tag = jsonparse_next (parser);
          if (tag != JSON_TYPE_STRING) {
            BYE ("STRING", tag);
          }
          jsonparse_copy_value (parser, temp, sizeof (temp));
          temp [sizeof (temp) - 1] = 0;
        }
        PRINTF ("GOT: %s\n", temp);
        tmp = (strtoul (temp, NULL, 10) + 50) / 100;
        if (tmp > 10) {
            tmp = 10;
        }
        if (tmp == 0) {
            tmp = 1;
        }
        period_100ms = tmp;
        PRINTF ("Setting: %dms\n", period_100ms * 100);
        REST.set_response_status(response, REST.status.CHANGED);
      } else {
        PRINTF ("PUT: len: %d\n", len);
        success = 0;
      }
      bye :
      break;
    default:
      success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
