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
#include "generic_resource.h"

void timestamp_from_string (const char *name, const char *s)
{
  struct timeval tv;
  // FIXME: Platform has no strtoll (long long)?
  tv.tv_sec = strtol (s, NULL, 10);
  settimeofday (&tv, NULL);
}

size_t
timestamp_to_string (const char *name, uint8_t is_json, char *buf, size_t bsize)
{
  struct timeval tv;
  char *fmt = "%ld";
  if (is_json) {
    fmt = "\"%ld\"";
  }
  gettimeofday (&tv, NULL);
  // FIXME: Platform doesn't seem to support long long printing
  // We get empty string
  return snprintf (buf, bsize, fmt, (long)tv.tv_sec);
}

GENERIC_RESOURCE
  ( timestamp
  , METHOD_GET | METHOD_PUT
  , "clock/timestamp"
  , Time
  , s
  , timestamp_from_string
  , timestamp_to_string
  );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

