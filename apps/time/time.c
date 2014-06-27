/**
 * \addgroup Time related functions
 *
 * @{
 */
#include <errno.h>
#include "contiki.h"
#include "contiki-lib.h"
#include "time.h"

/* Used for gmtime and localtime, according to manpage on linux the
 * internal value may be overwritten "by subsequent calls to any of the
 * date  and  time  functions".
 */
static struct tm tm;

/*
 * Internal variables to manage offset of utc from the contiki clock
 * and timezone offset from utc in minutes.
 * For now we don't manage the sub-second offset -- time of setting the
 * clock and the precisiont of the clock in use don't warrant this
 * effort.
 * The last_seconds is used to check if we had a seconds overflow,
 * although this happens only every 136 years :-)
 */
time_t   clock_offset;
uint32_t last_seconds;
int16_t  minuteswest;

# define LEAP_YEAR(_year) \
    ((_year % 4) == 0 && (_year % 100 != 0 || _year % 400 == 0))
# define YDAYS(_year) (LEAP_YEAR(year) ? 366 : 365)

struct tm *
gmtime_r (const time_t *timep, struct tm *ptm)
{
    unsigned int year;
    int days, month, month_len;
    time_t t = *timep;
    ptm->tm_sec = t % 60;
    t /= 60;
    ptm->tm_min = t % 60;
    t /= 60;
    ptm->tm_hour = t % 24;
    t /= 24;
    ptm->tm_wday = (t+4) % 7;
    year = 1970;
    days =  0;
    while ((days += YDAYS (year)) <= t)
    {
        year++;
    }
    ptm->tm_year = year - 1900;
    days -= YDAYS(year);
    t    -= days;
    ptm->tm_yday = t;
    for (month=0; month<12; month++)
    {
        if (month == 1)
        {
            month_len = LEAP_YEAR(year) ? 29 : 28;
        }
        else
        {
            int m = month;
            if (m >= 7)
            {
                m -= 1;
            }
            m &= 1;
            month_len = m ? 30 : 31;
        }
        if (t >= month_len)
        {
            t -= month_len;
        }
        else
        {
            break;
        }
    }
    ptm->tm_mon   = month;
    ptm->tm_mday  = t + 1;
    ptm->tm_isdst = 0;
    return ptm;
}

struct tm *
gmtime (const time_t *timep)
{
    return gmtime_r (timep, &tm);
}

struct tm *
localtime_r (const time_t *timep, struct tm *ptm)
{
    time_t t = *timep;
    t += minuteswest * 60;
    return gmtime_r (&t, ptm);
}

struct tm *
localtime (const time_t *timep)
{
    return localtime_r (timep, &tm);
}

/**
 * \brief Get time in seconds and microseconds
 * gettimeofday will return the clock time as the microseconds part
 * while settimeofday will *ignore* the microseconds part (for now).
 * Note that the contiki clock interface is broken anyway, we can't read
 * seconds and sub-seconds atomically. We try to work around this by
 * repeatedly reading seconds, sub-seconds, seconds until first and
 * second read of seconds match.
 */
int
gettimeofday (struct timeval *tv, struct timezone *tz)
{
    uint32_t cs;
    if (tz) {
        tz->tz_minuteswest = minuteswest;
        tz->tz_dsttime     = 0;
    }
    if (tv) {
        int i;
        /* Limit tries to get the same second twice to two */
        for (i=0; i<2; i++) {
            cs = clock_seconds ();
            if (cs < last_seconds) {
                clock_offset += 0xFFFFFFFFL;
                clock_offset ++;
            }
            last_seconds = cs;
            tv->tv_sec = cs + clock_offset;
            tv->tv_usec = ((time_t)(clock_time () % CLOCK_SECOND))
                        * 1000000L / CLOCK_SECOND;
            if (cs == clock_seconds ()) {
                break;
            }
        }
    }
    return 0;
}

/**
 * \brief Set time in seconds, microseconds ignored for now
 */
int
settimeofday (const struct timeval *tv, const struct timezone *tz)
{
    /* Don't allow setting timezone */
    if (tz) {
        errno = ERANGE;
        return -1;
    }
    if (tv) {
        uint32_t cs;
        cs = clock_seconds ();
        clock_offset = tv->tv_sec - cs;
    }
    return 0;
}

/** @} */
