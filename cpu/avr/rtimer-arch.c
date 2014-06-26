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

/**
 * \file
 *         AVR-specific rtimer code
 *         Defaults to Timer3 for those ATMEGAs that have it.
 *         If Timer3 not present Timer1 will be used.
 * \author
 *         Fredrik Osterlind <fros@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 */

/* OBS: 8 seconds maximum time! */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "sys/energest.h"
#include "sys/rtimer.h"
#include "rtimer-arch.h"

/* Track flow through rtimer interrupts*/
#if DEBUGFLOWSIZE&&0
extern uint8_t debugflowsize,debugflow[DEBUGFLOWSIZE];
#define DEBUGFLOW(c) if (debugflowsize<(DEBUGFLOWSIZE-1)) debugflow[debugflowsize++]=c
#else
#define DEBUGFLOW(c)
#endif

/*---------------------------------------------------------------------------*/
#if RTIMER_ARCH_PRESCALER
ISR (PLAT_VECT) {
  DEBUGFLOW('/');
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  /* Disable rtimer interrupts */
  PLAT_TIMSK &= 
   ~( (1 << PLAT_OCIEA)
    | (1 << PLAT_OCIEB)
    | (1 << PLAT_OCIEC)
    | (1 << PLAT_TOIE)
    | (1 << PLAT_ICIE)
    );

#if RTIMER_CONF_NESTED_INTERRUPTS
  /* Enable nested interrupts. Allows radio interrupt during rtimer interrupt. */
  /* All interrupts are enabled including recursive rtimer, so use with caution */
  sei();
#endif

  /* Call rtimer callback */
  rtimer_run_next();

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
  DEBUGFLOW('\\');
}
#endif
/*---------------------------------------------------------------------------*/
void
rtimer_arch_init(void)
{
#if RTIMER_ARCH_PRESCALER
  /* Disable interrupts (store old state) */
  uint8_t sreg;
  sreg = SREG;
  cli ();

  /* Disable all timer functions */
  PLAT_TIMSK &=
   ~( (1 << PLAT_OCIEA)
    | (1 << PLAT_OCIEB)
    | (1 << PLAT_OCIEC)
    | (1 << PLAT_TOIE)
    | (1 << PLAT_ICIE)
    );
  /* Write 1s to clear existing timer function flags */
  PLAT_TIFR |=
    ( (1 << PLAT_ICF)
    | (1 << PLAT_OCFA)
    | (1 << PLAT_OCFB)
    | (1 << PLAT_OCFC)
    | (1 << PLAT_TOV)
    );

  /* Default timer behaviour */
  PLAT_TCCRA = 0;
  PLAT_TCCRB = 0;
  PLAT_TCCRC = 0;

  /* Reset counter */
  PLAT_TCNT  = 0;

#if RTIMER_ARCH_PRESCALER==1024
  PLAT_TCCRB |= 5;
#elif RTIMER_ARCH_PRESCALER==256
  PLAT_TCCRB |= 4;
#elif RTIMER_ARCH_PRESCALER==64
  PLAT_TCCRB |= 3;
#elif RTIMER_ARCH_PRESCALER==8
  PLAT_TCCRB |= 2;
#elif RTIMER_ARCH_PRESCALER==1
  PLAT_TCCRB |= 1;
#else
#error Timer PRESCALER factor not supported.
#endif

  /* Restore interrupt state */
  SREG = sreg;
#endif /* RTIMER_ARCH_PRESCALER */
}
/*---------------------------------------------------------------------------*/
void
rtimer_arch_schedule(rtimer_clock_t t)
{
#if RTIMER_ARCH_PRESCALER
  /* Disable interrupts (store old state) */
  uint8_t sreg;
  sreg = SREG;
  cli ();
  DEBUGFLOW(':');
  /* Set compare register */
  PLAT_OCRA = t;
  /* Write 1s to clear all timer function flags */
  PLAT_TIFR |=
    ( (1 << PLAT_ICF)
    | (1 << PLAT_OCFA)
    | (1 << PLAT_OCFB)
    | (1 << PLAT_OCFC)
    | (1 << PLAT_TOV)
    );
  /* Enable interrupt on OCRXA match */
  PLAT_TIMSK |= (1 << PLAT_OCIEA);
  /* Restore interrupt state */
  SREG = sreg;
#endif /* RTIMER_ARCH_PRESCALER */
}

#if RDC_CONF_MCU_SLEEP
/*---------------------------------------------------------------------------*/
void
rtimer_arch_sleep(rtimer_clock_t howlong)
{
/* Deep Sleep for howlong rtimer ticks. This will stop all timers except
 * for TIMER2 which can be clocked using an external crystal.
 * Unfortunately this is an 8 bit timer; a lower prescaler gives higher
 * precision but smaller maximum sleep time.
 * Here a maximum 128msec (contikimac 8Hz channel check sleep) is assumed.
 * The rtimer and system clocks are adjusted to reflect the sleep time.
 */
#include <avr/sleep.h>
#include <dev/watchdog.h>
uint32_t longhowlong;
#if AVR_CONF_USE32KCRYSTAL
/* Save TIMER2 configuration if clock.c is using it */
    uint8_t savedTCNT2=TCNT2, savedTCCR2A=TCCR2A, savedTCCR2B = TCCR2B, savedOCR2A = OCR2A;
#endif
    cli();
	watchdog_stop();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);

/* Set TIMER2 clock asynchronus from external source, CTC mode */
    ASSR |= (1 << AS2);
    TCCR2A =(1<<WGM21);
/* Set prescaler and TIMER2 output compare register */
#if 0    //Prescale by 1024 -   32 ticks/sec, 8 seconds max sleep
    TCCR2B =((1<<CS22)|(1<<CS21)|(1<<CS20));
	longhowlong=howlong*32UL; 
#elif 0  // Prescale by 256 -  128 ticks/sec, 2 seconds max sleep
	TCCR2B =((1<<CS22)|(1<<CS21)|(0<<CS20));
	longhowlong=howlong*128UL;
#elif 0  // Prescale by 128 -  256 ticks/sec, 1 seconds max sleep
	TCCR2B =((1<<CS22)|(0<<CS21)|(1<<CS20));
	longhowlong=howlong*256UL;
#elif 0  // Prescale by  64 -  512 ticks/sec, 500 msec max sleep
	TCCR2B =((1<<CS22)|(0<<CS21)|(0<<CS20));
	longhowlong=howlong*512UL;
#elif 1  // Prescale by  32 - 1024 ticks/sec, 250 msec max sleep
	TCCR2B =((0<<CS22)|(1<<CS21)|(1<<CS20));
	longhowlong=howlong*1024UL;
#elif 0  // Prescale by   8 - 4096 ticks/sec, 62.5 msec max sleep
	TCCR2B =((0<<CS22)|(1<<CS21)|(0<<CS20));
	longhowlong=howlong*4096UL;
#else    // No Prescale -    32768 ticks/sec, 7.8 msec max sleep
	TCCR2B =((0<<CS22)|(0<<CS21)|(1<<CS20));
	longhowlong=howlong*32768UL;
#endif
	OCR2A = longhowlong/RTIMER_ARCH_SECOND;

/* Reset timer count, wait for the write (which assures TCCR2x and OCR2A are finished) */
    TCNT2 = 0; 
    while(ASSR & (1 << TCN2UB));

/* Enable TIMER2 output compare interrupt, sleep mode and sleep */
    TIMSK2 |= (1 << OCIE2A);
    SMCR |= (1 <<  SE);
	sei();
	ENERGEST_OFF(ENERGEST_TYPE_CPU);
	if (OCR2A) sleep_mode();
	  //...zzZZZzz...Ding!//

/* Disable sleep mode after wakeup, so random code cant trigger sleep */
    SMCR  &= ~(1 << SE);

/* Adjust rtimer ticks if rtimer is enabled. TIMER3 is preferred, else TIMER1 */
#if RTIMER_ARCH_PRESCALER
    PLAT_TCNT += howlong;
#endif
	ENERGEST_ON(ENERGEST_TYPE_CPU);

#if AVR_CONF_USE32KCRYSTAL
/* Restore clock.c configuration */
    cli();
    TCCR2A = savedTCCR2A;
    TCCR2B = savedTCCR2B;
    OCR2A  = savedOCR2A;
    TCNT2  = savedTCNT2;
    sei();
#else
/* Disable TIMER2 interrupt */
    TIMSK2 &= ~(1 << OCIE2A);
#endif
    watchdog_start();

/* Adjust clock.c for the time spent sleeping */
	longhowlong=CLOCK_CONF_SECOND;
	longhowlong*=howlong;
    clock_adjust_ticks(longhowlong/RTIMER_ARCH_SECOND);

}
#if !AVR_CONF_USE32KCRYSTAL
/*---------------------------------------------------------------------------*/
/* TIMER2 Interrupt service */

ISR(TIMER2_COMPA_vect)
{
//    TIMSK2 &= ~(1 << OCIE2A);       //Just one interrupt needed for waking
}
#endif /* !AVR_CONF_USE32KCRYSTAL */
#endif /* RDC_CONF_MCU_SLEEP */

