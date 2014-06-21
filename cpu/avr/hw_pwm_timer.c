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
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \addgroup hardware timer
 *
 * @{
 */

/**
 * \file
 *        Alternative initialisation with period in microseconds
 * \author
 *        Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <avr/pgmspace.h>
#include "contiki.h"
#include "rtimer-arch.h"
#include "hw_timer.h"

/* one for each possible timer */
uint16_t hwt_max_ticks [6];

#define PERIOD_MAX (0xFFFFFFFF / (F_CPU / 1000000))
/* for 16-bit timer: */
#define TICKS_MAX  0xFFFF
#define TICKS_MIN  0xFF

int8_t
hwtimer_pwm_ini (uint8_t timer, uint32_t period_us, uint8_t pwm_type, uint8_t ocra)
{
  uint32_t ticks = 0;
  uint8_t  clock = HWT_CLOCK_PRESCALER_1024;
  uint8_t  wgm   = HWT_WGM_NORMAL;
  HWT_CHECK_TIMER (timer);
  if (period_us > PERIOD_MAX) {
    period_us = PERIOD_MAX;
  }
  ticks = (F_CPU / 1000000) * period_us;
  /* Non-fast PWM modes have half the frequency */
  if (pwm_type != HWT_PWM_FAST) {
    ticks >>= 1;
  }

  /*
   * Divisors are 1, 8, 64, 256, 1024, shifts between these are
   * 3, 3, 2, 2, respectively. We modify `ticks` in place, the AVR can
   * shift only one bit in one instruction, so shifting isn't cheap.
   * We try to get the *maximum* prescaler that still permits a tick
   * resolution of at least 8 bit.
   */
  if (ticks <= (TICKS_MIN << 3)) {
    clock = HWT_CLOCK_PRESCALER_1;
  }
  else if ((ticks >>= 3) <= (TICKS_MIN << 3)) {
    clock = HWT_CLOCK_PRESCALER_8;
  }
  else if ((ticks >>= 3) <= (TICKS_MIN << 2)) {
    clock = HWT_CLOCK_PRESCALER_64;
  }
  else if ((ticks >>= 2) <= (TICKS_MIN << 2)) {
    clock = HWT_CLOCK_PRESCALER_256;
  }
  else if ((ticks >>= 2) > TICKS_MAX) {
    ticks = TICKS_MAX;
  }
  hwt_max_ticks [timer] = ticks;
  switch (pwm_type) {
    case HWT_PWM_FAST:
      wgm = ocra ? HWT_WGM_PWM_FAST_OCRA : HWT_WGM_PWM_FAST_ICR;
      break;
    case HWT_PWM_PHASE_CORRECT:
      wgm = ocra ? HWT_WGM_PWM_PHASE_OCRA : HWT_WGM_PWM_PHASE_ICR;
      break;
    case HWT_PWM_PHASE_FRQ_CORRECT:
    default:
      wgm = ocra ? HWT_WGM_PWM_PHASE_FRQ_OCRA : HWT_WGM_PWM_PHASE_FRQ_ICR;
      break;
  }
  /* Special 8- 9- 10-bit modes */
  if (pwm_type == HWT_PWM_FAST || pwm_type == HWT_PWM_PHASE_CORRECT) {
    if (ticks == 0xFF) {
      wgm = (pwm_type == HWT_PWM_FAST)
          ? HWT_WGM_PWM_FAST_8_BIT
          : HWT_WGM_PWM_PHASE_8_BIT;
    }
    else if (ticks == 0x1FF) {
      wgm = (pwm_type == HWT_PWM_FAST)
          ? HWT_WGM_PWM_FAST_9_BIT
          : HWT_WGM_PWM_PHASE_9_BIT;
    }
    else if (ticks == 0x3FF) {
      wgm = (pwm_type == HWT_PWM_FAST)
          ? HWT_WGM_PWM_FAST_10_BIT
          : HWT_WGM_PWM_PHASE_10_BIT;
    }
  }
  return hwtimer_ini (timer, wgm, clock, ticks);
}

uint32_t hwtimer_pwm_max_ticks (uint8_t timer)
{
  if (timer > 5) {
    return 0;
  }
  return hwt_max_ticks [timer];
}

/*
 * ex:ts=8:et:sw=2
 */

/** @} */
