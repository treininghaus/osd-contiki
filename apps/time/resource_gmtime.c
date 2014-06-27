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
#include "generic_resource.h"

size_t time_to_string (const char *name, uint8_t is_json, char *buf, size_t bs)
{
    struct timeval tv;
    struct tm tm;
    struct tm *(*method)(const time_t *, struct tm *) = gmtime_r;
    if (0 == strcmp (name, "localtime")) {
        method = localtime_r;
    }
    gettimeofday (&tv, NULL);
    method (&tv.tv_sec, &tm);
    return snprintf
      ( buf
      , bs
      , "%s%lu-%02u-%02u %02u:%02u:%02u%s"
      , is_json ? "\"" : ""
      , 1900 + tm.tm_year
      , tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec
      , is_json ? "\"" : ""
      );
}

GENERIC_RESOURCE \
    ( localtime
    , METHOD_GET
    , "clock/localtime"
    , Local time
    , formatted time
    , NULL
    , time_to_string
    );

GENERIC_RESOURCE \
    ( utc
    , METHOD_GET
    , "clock/utc"
    , UTC
    , formatted time
    , NULL
    , time_to_string
    );

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

