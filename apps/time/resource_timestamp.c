/**
 * \file
 *      Resource for timestamp handling
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put time in seconds since 1970 (UNIX time)
 * Note: the internal format of the time in seconds is a 64bit number
 * unfortunately javascript (json) will only support double for which
 * the mantissa isn't long enough for representing that number. So we're
 * back to 32 bit and have a year 2038 problem.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "time.h"
#include "time_resource.h"
#include "jsonparse.h"
/* Only coap 13 for now */
#include "er-coap-13.h"

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
    ( timestamp, METHOD_GET | METHOD_PUT
    , "clock/timestamp"
    , "title=\"Time\";rt=\"timestamp\""
    );

void
timestamp_handler
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
  struct timeval tv;
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
      gettimeofday (&tv, NULL);
      // TEXT format
      if (a_ctype == REST.type.TEXT_PLAIN) {
        index += sprintf (temp + index, "%ld\n", (long)tv.tv_sec);
      } else { // jSON Format
        // FIXME: Platform doesn't seem to support long long printing
        // We get empty string
        index += sprintf
          ( temp + index
          ,"{\n \"timestamp\" : \"%ld\"\n}\n"
          , (long)tv.tv_sec
          );
      }
      length = strlen(temp);
      memcpy (buffer, temp, length);
      REST.set_header_content_type (response, a_ctype);
      REST.set_response_payload (response, buffer, length);

      break;
    case METHOD_PUT:
      if ((len = coap_get_payload(request, &bytes))) {
        if (c_ctype == REST.type.TEXT_PLAIN) {
          temp [sizeof (temp) - 1] = 0;
          strncpy (temp, (char *)bytes, MIN (len, sizeof (temp) - 1));
        } else { // jSON Format
          struct jsonparse_state state;
          struct jsonparse_state *parser = &state;
          PRINTF ("PUT: len: %d, %s\n", len, (char *)bytes);
          jsonparse_setup (parser, (char *)bytes, len);
          if ((tag = jsonparse_next (parser)) != JSON_TYPE_OBJECT) {
            BYE ("OBJECT", tag);
          }
          if ((tag = jsonparse_next (parser)) != JSON_TYPE_PAIR_NAME) {
            BYE ("PAIR_NAME", tag);
          }
          while (jsonparse_strcmp_value (parser, "timestamp") != 0) {
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
        }
        // FIXME: Platform has no strtoll (long long)?
        tv.tv_sec = strtol (temp, NULL, 10);
        settimeofday (&tv, NULL);
        REST.set_response_status(response, REST.status.CHANGED);
      } else {
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
