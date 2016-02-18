/**
 * \file
 *      Resource for timezone handling
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put timezone string
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "time.h"
#include "time_resource.h"
#include "jsonparse.h"
#include "er-coap.h"
#include "generic_resource.h"

void timezone_from_string (const char *name, const char *s)
{
  set_tz (s);
}

size_t
timezone_to_string (const char *name, uint8_t is_json, char *buf, size_t bsize)
{
  if (get_tz (buf, bsize) == NULL) {
    *buf = '\0';
  }
  return strlen (buf);
}

GENERIC_RESOURCE
  ( timezone
  , TZ
  , s
  , timezone_from_string
  , timezone_to_string
  );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

