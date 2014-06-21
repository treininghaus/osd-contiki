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
 *        Header file for hardware timer of AVR microcontrollers
 * \author
 *        Ralf Schlatterbeck <rsc@runtux.com>
 */

#include <avr/pgmspace.h>
#include "contiki.h"
#include "rtimer-arch.h"
#include "hw_timer.h"

#ifndef PLAT_TIMER
#define PLAT_TIMER 0xFF /* invalid timer for comparison */
#endif

int8_t hwtimer_ini (uint8_t timer, uint8_t wgm, uint8_t clock, uint16_t maxt)
{
  int8_t i;
  HWT_CHECK_TIMER (timer);
  if (wgm > HWT_WGM_MASK || wgm == HWT_WGM_RESERVED) {
    return HWT_ERR_INVALID_WGM;
  }
  if (clock > HWT_CLOCK_MASK) {
    return HWT_ERR_INVALID_CLOCK;
  }
  /* Turn off clock, no need to disable interrupt */
  *HWT_TCCRB (timer) &= ~HWT_CLOCK_MASK;

  *HWT_TCCRA (timer) &= ~(HWT_WGM_MASK_LOW         << HWT_WGM_SHIFT_LOW);
  *HWT_TCCRA (timer) |= ((wgm & HWT_WGM_MASK_LOW)  << HWT_WGM_SHIFT_LOW);
  *HWT_TCCRB (timer) &= ~(HWT_WGM_MASK_HIGH        << HWT_WGM_SHIFT_HIGH);
  *HWT_TCCRB (timer) |= ((wgm & HWT_WGM_MASK_HIGH) << HWT_WGM_SHIFT_HIGH);

  for (i=0; i<3; i++) {
    HWT_SET_COM (timer, i, HWT_COM_NORMAL);
  }

  if (  wgm == HWT_WGM_PWM_PHASE_FRQ_ICR
     || wgm == HWT_WGM_PWM_PHASE_ICR
     || wgm == HWT_WGM_CTC_ICR
     || wgm == HWT_WGM_PWM_FAST_ICR
     )
  {
    *HWT_ICR (timer) = maxt;
  }

  if (  wgm == HWT_WGM_CTC_OCRA
     || wgm == HWT_WGM_PWM_PHASE_FRQ_OCRA
     || wgm == HWT_WGM_PWM_PHASE_OCRA
     || wgm == HWT_WGM_PWM_FAST_OCRA
     )
  {
    *HWT_OCRA (timer) = maxt;
  }

  /* Set clock, finally */
  *HWT_TCCRB (timer) |= clock;
  return 0;
}

/*
 * ex:ts=8:et:sw=2
 */

/** @} */
