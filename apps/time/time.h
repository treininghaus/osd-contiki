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

#ifdef LOCAL_COMPILE
#define tm l_tm
#define timeval l_timeval
#define time_t l_time_t
#define gmtime l_gmtime
#define localtime l_localtime
#define gettimeofday l_gettimeofday
#define clock_seconds() 1
#define clock_time() 1
#endif

#define DEFAULT_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

typedef signed long long time_t;
typedef signed long      suseconds_t;

#ifdef __cplusplus
extern "C" {
#endif

/* tm_gmtoff and tm_zone are BSD additions */
struct tm {
    uint32_t    tm_year;        /* year */
    uint16_t    tm_yday;        /* day in the year */
    uint8_t     tm_sec;         /* seconds */
    uint8_t     tm_min;         /* minutes */
    uint8_t     tm_hour;        /* hours */
    uint8_t     tm_mday;        /* day of the month */
    uint8_t     tm_mon;         /* month */
    uint8_t     tm_wday;        /* day of the week */
    uint8_t     tm_isdst;       /* daylight saving time */
    int32_t     tm_gmtoff;      /* Seconds east of UTC */
    const char *tm_zone;        /* Timezone abbreviation */
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

/*
 * Maximum length of all timezone names, this is much longer in UNIX
 * implementations but we have limited space here. Note that the length
 * includes a trailing \0 byte for each timezone name.
 */
#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS 16
#endif

/* Maximum length of buffer to reserve for timezone string */
#define MAXTZLEN (TZ_MAX_CHARS+9+2*(2+6+1+8)+1)

int set_tz (const char *tzstring);
const char *get_tz (char *buffer, size_t buflen);
size_t len_tz (void);

#ifdef __cplusplus
}
#endif

#endif // time_h
/** @} */
