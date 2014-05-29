/**
 * \defgroup Time related functions
 *
 * This rolls the necessary definition for getting/setting time and
 * managing local time into one include file, on posix systems this
 * lives in at least two include files, time.h and sys/time.h
 *
 * @{
 */

/**
 * \file
 *         Definitions for the time module
 *      
 * \author
 *         Ralf Schlatterbeck <rsc@tux.runtux.com>
 */

#ifndef time_h
#define time_h


typedef signed long long time_t;
typedef signed long      suseconds_t;

#ifdef __cplusplus
extern "C" {
#endif

struct tm {
    uint32_t tm_year;        /* year */
    uint16_t tm_yday;        /* day in the year */
    uint8_t  tm_sec;         /* seconds */
    uint8_t  tm_min;         /* minutes */
    uint8_t  tm_hour;        /* hours */
    uint8_t  tm_mday;        /* day of the month */
    uint8_t  tm_mon;         /* month */
    uint8_t  tm_wday;        /* day of the week */
    uint8_t  tm_isdst;       /* daylight saving time */
};

struct timeval {
    time_t      tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
};

struct timezone {
    int16_t tz_minuteswest;     /* minutes west of Greenwich */
    int     tz_dsttime;         /* type of DST correction, unused */
};

struct tm *gmtime      (const time_t *timep);
struct tm *gmtime_r    (const time_t *timep, struct tm *result);
struct tm *localtime   (const time_t *timep);
struct tm *localtime_r (const time_t *timep, struct tm *result);

int gettimeofday       (struct timeval *tv, struct timezone *tz);
int settimeofday       (const struct timeval *tv, const struct timezone *tz);

#ifdef __cplusplus
}
#endif

#endif // time_h
/** @} */
