/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 *
 */

#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include <avr/interrupt.h>

/* Nominal ARCH_SECOND is F_CPU/prescaler, e.g. 8000000/1024 = 7812
 * Other prescaler values (1, 8, 64, 256) will give greater precision
 * with shorter maximum intervals.
 * Setting RTIMER_ARCH_PRESCALER to 0 will leave Timers alone.
 * rtimer_arch_now() will then return 0, likely hanging the cpu if used.
 * Timer1 is used if Timer3 is not available.
 *
 * Note the rtimer tick to clock tick conversion will be nominally correct only
 * when the same oscillator is used for both clocks.
 * When an external 32768 watch crystal is used for clock ticks my raven CPU
 * oscillator is 1% slow, 32768 counts on crystal = ~7738 rtimer ticks.
 * For more accuracy define F_CPU to 0x800000 and optionally phase lock CPU
 * clock to 32768 crystal. This gives RTIMER_ARCH_SECOND = 8192.
 */
#ifndef RTIMER_ARCH_PRESCALER
#define RTIMER_ARCH_PRESCALER 1024UL
#endif
#if RTIMER_ARCH_PRESCALER
#define RTIMER_ARCH_SECOND (F_CPU/RTIMER_ARCH_PRESCALER)
#else
#define RTIMER_ARCH_SECOND 0
#endif

#ifndef PLAT_TIMER
/* By default use timer 3 if available. Fall back to timer1 if not. */
#ifdef TCNT3
#define PLAT_TIMER 3
#else
#define PLAT_TIMER 1
#endif /* TCNT3 */
#endif /* !PLAT_TIMER */

#define _R_CONC_(_x,_y,_z)   _x##_y##_z
#define _C_R_CONC_(_X,_Y,_Z) _R_CONC_(_X,_Y,_Z)

#define PLAT_ICF    _C_R_CONC_(ICF,PLAT_TIMER,)
#define PLAT_ICIE   _C_R_CONC_(ICIE,PLAT_TIMER,)
#define PLAT_OCFA   _C_R_CONC_(OCF,PLAT_TIMER,A)
#define PLAT_OCFB   _C_R_CONC_(OCF,PLAT_TIMER,B)
#define PLAT_OCFC   _C_R_CONC_(OCF,PLAT_TIMER,C)
#define PLAT_OCIEA  _C_R_CONC_(OCIE,PLAT_TIMER,A)
#define PLAT_OCIEB  _C_R_CONC_(OCIE,PLAT_TIMER,B)
#define PLAT_OCIEC  _C_R_CONC_(OCIE,PLAT_TIMER,C)
#define PLAT_OCRA   _C_R_CONC_(OCR,PLAT_TIMER,A)
#define PLAT_TCCRA  _C_R_CONC_(TCCR,PLAT_TIMER,A)
#define PLAT_TCCRB  _C_R_CONC_(TCCR,PLAT_TIMER,B)
#define PLAT_TCCRC  _C_R_CONC_(TCCR,PLAT_TIMER,C)
#define PLAT_TCNT   _C_R_CONC_(TCNT,PLAT_TIMER,)
#define PLAT_TIFR   _C_R_CONC_(TIFR,PLAT_TIMER,)
#define PLAT_TIMSK  _C_R_CONC_(TIMSK,PLAT_TIMER,)
#define PLAT_TOIE   _C_R_CONC_(TOIE,PLAT_TIMER,)
#define PLAT_TOV    _C_R_CONC_(TOV,PLAT_TIMER,)
#define PLAT_VECT   _C_R_CONC_(TIMER,PLAT_TIMER,_COMPA_vect)
#if RTIMER_ARCH_PRESCALER
#define rtimer_arch_now() (PLAT_TCNT)
#else
#define rtimer_arch_now() (0)
#endif
/* some platforms don't have OCIEXC, we rely on the processor
 * definition to #define OCIEXC OCIEXB in that case. This won't hurt
 * since OCIEXC isn't used anyway.
 */

void rtimer_arch_sleep(rtimer_clock_t howlong);
#endif /* RTIMER_ARCH_H_ */
