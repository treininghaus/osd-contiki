/*
 * Copyright (c) 2016, Dr. Ralf Schlatterbeck Open Source Consulting
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
 *  \brief This module contains PicoRV32-specific code to implement
 *  the Contiki core clock functions.
 *  
 *  \author Ralf Schlatterbeck <rsc@runtux.com>.
 *
 */
/** \addtogroup picorv32
 * @{
 */
/**
 *  \defgroup picorv32clock PicoRV32 clock implementation
 * @{
 */
/**
 *  \file
 *  This file contains PicoRV32-specific code to implement the Contiki
 *  core clock functions.
 *
 */
/**
 * These routines define the PicoRV32-specific calls declared in
 * /core/sys/clock.h  CLOCK_SECOND is the number of ticks per second.
 * It is defined through CONF_CLOCK_SECOND in the contiki-conf.h for
 * each platform.
 * The usual AVR defaults are 128 or 125 ticks per second, counting a
 * prescaled CPU clock using the 8 bit timer0. We use the same in the
 * timer interrupt: 1/128 second ticks, this can be changed by modifying
 * CLOCK_TIMER_PERIOD below.
 * 
 * clock_time_t is usually declared by the platform as an unsigned 16
 * bit data type, thus intervals up to 512 or 524 seconds can be
 * measured with ~8 millisecond precision.
 * For longer intervals the 32 bit clock_seconds() is available.
 * We directly use the 64-bit cycle counter provided by the CPU.
 */
#include "sys/clock.h"
#include "sys/etimer.h"
#include "icosoc.h"

#include <stdio.h>

// 1/128 second ticks
#define CLOCK_TIMER_PERIOD (F_CPU >> 7)

unsigned long offset;

/** \brief irq handler
 * for running interrupts every 1/128 second.
 */

void
clock_irq_handler (void)
{
    // FIXME: Want to call rtimer_run_next();
    icosoc_timer(CLOCK_TIMER_PERIOD);
    if(etimer_pending()) {
        etimer_request_poll();
    }
}

/*---------------------------------------------------------------------------*/
/**
 * Initialize interrupt handler
 */
    void
clock_init(void)
{
    register_irq (TIMER_IRQ, clock_irq_handler);
    enable_irq (TIMER_IRQ);
    icosoc_timer(CLOCK_TIMER_PERIOD);
}
/*---------------------------------------------------------------------------*/
/**
 * Return the tick counter. We use the full 64bit counter which makes
 * computation of seconds etc. easier later.
 */
    clock_time_t
clock_time(void)
{
    volatile uint32_t low, high;
    asm ("1: rdcycleh %1\n"
            "rdcycle  %0\n"
            "rdcycleh t0\n"
            "bne      %1,t0,1b\n"
            : "=r" (low), "=r" (high)
            :
            : "t0"
        );
    return ((clock_time_t)high) << 32 | low;
}
/*---------------------------------------------------------------------------*/
/**
 * Return seconds, default is time since startup.
 */
    unsigned long
clock_seconds(void)
{
    return (unsigned long)(clock_time () / CLOCK_CONF_SECOND) + offset;
}
/*---------------------------------------------------------------------------*/
/**
 * Set seconds, e.g. to a standard epoch for an absolute date/time.
 */
    void
clock_set_seconds(unsigned long sec)
{
    offset = sec - (unsigned long)(clock_time () / CLOCK_CONF_SECOND);
}
/*---------------------------------------------------------------------------*/
/*
 * Wait for a number of clock ticks.
 */
    void
clock_wait(clock_time_t t)
{
    clock_time_t endticks = clock_time() + t;
    while (clock_time () < endticks)
    {;}
}
/*---------------------------------------------------------------------------*/
    void
clock_delay_usec(uint16_t dt)
{
    clock_wait (CLOCK_SECOND * dt / 1000000ULL);
}
/*---------------------------------------------------------------------------*/
/**
 * Legacy delay. The original clock_delay for the msp430 used a granularity
 * of 2.83 usec. This approximates that delay for values up to 1456 usec.
 * (The largest core call in leds.c uses 400).
 */
void
clock_delay(unsigned int howlong)
{
  clock_delay_usec((45*howlong)>>4);
}
/*---------------------------------------------------------------------------*/
/**
 * Adjust the system current clock time.
 * \param howmany   How many ticks to add
 *
 * Typically used to add ticks after an MCU sleep
 * clock_seconds will increment if necessary to reflect the tick addition.
 * Leap ticks or seconds can (rarely) be introduced if the ISR is not blocked.
 */
    void
clock_adjust_ticks(clock_time_t howmany)
{
}

/** @} */
/** @} */
