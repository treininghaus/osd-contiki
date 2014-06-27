/*
 * Copyright (c) 2014, Ralf Schlatterbeck Open Source Consulting
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
 */

/**
 * \addtogroup Generic CoAP Resource Handler
 *
 * @{
 */


/**
 * \file
 *      Generic CoAP Resource Handler
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "jsonparse.h"
/* Only coap 13 for now */
#include "er-coap-13.h"
#include "generic_resource.h"

/* Error-handling macro */
# define BYE(_exp, _tag) \
   do {                                  \
     PRINTF("Expect "_exp": %d\n",_tag); \
     return -1;                          \
   } while(0)

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

int8_t
json_parse_variable
  (const uint8_t *bytes, size_t len, char *name, char *buf, size_t buflen)
{
  int tag = 0;
  struct jsonparse_state state;
  struct jsonparse_state *parser = &state;
  PRINTF ("PUT: len: %d, %s\n", len, (const char *)bytes);
  jsonparse_setup (parser, (const char *)bytes, len);
  if ((tag = jsonparse_next (parser)) != JSON_TYPE_OBJECT) {
    BYE ("OBJECT", tag);
  }
  if ((tag = jsonparse_next (parser)) != JSON_TYPE_PAIR_NAME) {
    BYE ("PAIR_NAME", tag);
  }
  while (jsonparse_strcmp_value (parser, name) != 0) {
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
  jsonparse_copy_value (parser, buf, buflen);
  return 0;
}

void generic_handler
  ( void *request
  , void *response
  , uint8_t *buffer
  , uint16_t preferred_size
  , int32_t *offset
  , char *name
  , void (*from_str)(const char *name, const char *s)
  , size_t (*to_str)(const char *name, uint8_t is_json, char *buf, size_t bsize)
  )
{
  int success = 1;
  char temp [100];
  int i = 0;
  size_t len = 0;
  int n_acc = 0;
  const uint8_t  *bytes  = NULL;
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
        len += to_str (name, 0, temp + len, sizeof (temp) - len);
        if (len > sizeof (temp)) {
          success = 0;
          break;
        }
        len += snprintf (temp + len, sizeof (temp) - len, "\n");
        if (len > sizeof (temp)) {
          success = 0;
          break;
        }
      } else { // jSON Format
        len += snprintf
          (temp + len, sizeof (temp) - len, "{\n \"%s\" : ", name);
        if (len > sizeof (temp)) {
          success = 0;
          break;
        }
        len += to_str (name, 1, temp + len, sizeof (temp) - len);
        if (len > sizeof (temp)) {
          success = 0;
          break;
        }
        len += snprintf (temp + len, sizeof (temp) - len, "\n}\n");
        if (len > sizeof (temp)) {
          success = 0;
          break;
        }
      }
      memcpy (buffer, temp, len);
      REST.set_header_content_type (response, a_ctype);
      REST.set_response_payload (response, buffer, len);
      break;
    case METHOD_PUT:
      if (from_str && (len = coap_get_payload(request, &bytes))) {
        if (c_ctype == REST.type.TEXT_PLAIN) {
          temp [sizeof (temp) - 1] = 0;
          strncpy (temp, (const char *)bytes, MIN (len, sizeof (temp) - 1));
        } else { // jSON Format
          if (json_parse_variable (bytes, len, name, temp, sizeof (temp)) < 0) {
            success = 0;
            break;
          }
        }
        from_str (name, temp);
        REST.set_response_status(response, REST.status.CHANGED);
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

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

/** @} */

