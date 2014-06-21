/**
 * \file
 *      Resource for hardware timer handling
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put pwm for LED pin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "jsonparse.h"
/* Only coap 13 for now */
#include "er-coap-13.h"
#include "hw_timer.h"

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

static uint16_t max_ticks = 0, pwm = 0;

void led_pwm_init (void)
{
    int8_t result = hwtimer_pwm_ini (3, 20, HWT_PWM_PHASE_CORRECT, 0);
    PRINTF ("HWTIMER init: %d\n", result);
    max_ticks = hwtimer_pwm_max_ticks (3);
    pwm = max_ticks / 2;
    hwtimer_set_pwm (3, HWT_CHANNEL_C, pwm);
    hwtimer_pwm_inverse (3, HWT_CHANNEL_C);
    DDRE  |= (1<<PINE5);
}

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
        pwm = atoi (temp);
        if (pwm > max_ticks) {
            pwm = max_ticks;
        }
        PRINTF ("Setting: %d (max=%d)\n", pwm, max_ticks);
        hwtimer_pwm_inverse (3, HWT_CHANNEL_C);
        DDRE  |= (1<<PINE5);
        hwtimer_set_pwm (3, HWT_CHANNEL_C, pwm);
        PRINTF
            ( "TCNT3: %04X TCCR3A: %04X TCCR3B: %04X TCCR3C: %04X OCR3C: %04X\n"
            , TCNT3, TCCR3A, TCCR3B, TCCR3C, OCR3C
            );
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
