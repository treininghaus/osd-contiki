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
#include "generic_resource.h"

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

void pwm_from_string (const char *name, const char *s)
{
    pwm = atoi (s);
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
}

size_t pwm_to_string (const char *n, uint8_t is_json, char *buf, size_t bufsize)
{
    return snprintf (buf, bufsize, "%d", pwm);
}

GENERIC_RESOURCE \
    ( led_pwm, METHOD_GET | METHOD_PUT
    , "led/pwm"
    , LED PWM
    , duty-cycle
    , pwm_from_string
    , pwm_to_string
    );
