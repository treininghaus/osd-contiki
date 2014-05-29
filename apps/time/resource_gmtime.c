/**
 * \file
 *      Resource for gmtime (utc) / localtime handling
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get time as a string in utc or localtime
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

RESOURCE \
    ( localtime
    , METHOD_GET
    , "clock/localtime"
    , "title=\"Time\";rt=\"localtime\""
    );

RESOURCE \
    ( gmtime
    , METHOD_GET
    , "clock/utc"
    , "title=\"Time\";rt=\"utc\""
    );

void
time_handler
    ( void* request
    , void* response
    , uint8_t *buffer
    , uint16_t preferred_size
    , int32_t *offset
    , struct tm *(*method)(const time_t *,struct tm *)
    )
{
  int success = 1;

  char temp[100];
  int index = 0;
  int length = 0; /*           |<-------->| */
  struct timeval tv;
  struct tm tm;
  int i = 0;
  int n_acc = 0;
  const uint16_t *accept = NULL;
  uint16_t a_ctype = REST.type.APPLICATION_JSON; /* for now json is default */

  /* Looks like accepted content-type isn't currently supported */
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

  switch(REST.get_method_type(request)){
    case METHOD_GET:
      gettimeofday (&tv, NULL);
      method (&tv.tv_sec, &tm);
      if (a_ctype == REST.type.TEXT_PLAIN) {
        index += sprintf
          ( temp + index
          , "%lu-%02u-%02u %02u:%02u:%02u\n"
          , 1900 + tm.tm_year
          , tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec
          );
      } else { // jSON Format
        index += sprintf
          ( temp + index
          ,"{\n \"localtime\" : \"%lu-%02u-%02u %02u:%02u:%02u\"\n"
          , 1900 + tm.tm_year
          , tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec
          );
        index += sprintf(temp + index,"}\n");
      }
      length = strlen(temp);
      memcpy (buffer, temp, length);
      REST.set_header_content_type (response, REST.type.APPLICATION_JSON);
      REST.set_response_payload (response, buffer, length);

      break;
    default:
      success = 0;
  }
  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}

void
localtime_handler
    ( void* request
    , void* response
    , uint8_t *buffer
    , uint16_t preferred_size
    , int32_t *offset
    )
{
    return time_handler
        (request, response, buffer, preferred_size, offset, &localtime_r);
}

void
gmtime_handler
    ( void* request
    , void* response
    , uint8_t *buffer
    , uint16_t preferred_size
    , int32_t *offset
    )
{
    return time_handler
        (request, response, buffer, preferred_size, offset, &gmtime_r);
}
