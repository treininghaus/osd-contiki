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
 * prescaled CPU clock using the 8 bit timer0.
 * 
 * clock_time_t is usually declared by the platform as an unsigned 16
 * bit data type, thus intervals up to 512 or 524 seconds can be
 * measured with ~8 millisecond precision.
 * For longer intervals the 32 bit clock_seconds() is available.
 */
#include "sys/clock.h"
#include "sys/etimer.h"

static volatile clock_time_t count;
volatile unsigned long seconds;
long sleepseconds;

/*---------------------------------------------------------------------------*/
/**
 * Start the clock by enabling the timer comparison interrupts. 
 */
void
clock_init(void)
{
  //cli ();
  //OCRSetup();
  //sei ();
}
/*---------------------------------------------------------------------------*/
/**
 * Return the tick counter. When 16 bit it typically wraps every 10 minutes.
 * The comparison avoids the need to disable clock interrupts for an atomic
 * read of the multi-byte variable.
 */
clock_time_t
clock_time(void)
{
  static clock_time_t counter = 0;
  return counter++;
}
/*---------------------------------------------------------------------------*/
/**
 * Return seconds, default is time since startup.
 * The comparison avoids the need to disable clock interrupts for an atomic
 * read of the four-byte variable.
 */
unsigned long
clock_seconds(void)
{
  unsigned long tmp;
  do {
    tmp = seconds;
  } while(tmp != seconds);
  return tmp;
}
/*---------------------------------------------------------------------------*/
/**
 * Set seconds, e.g. to a standard epoch for an absolute date/time.
 */
void
clock_set_seconds(unsigned long sec)
{
  seconds = sec;
}
/*---------------------------------------------------------------------------*/
/*
 * Wait for a number of clock ticks.
 */
void
clock_wait(clock_time_t t)
{
  clock_time_t endticks = clock_time() + t;
  if (sizeof(clock_time_t) == 1) {
    while ((signed char )(clock_time() - endticks) < 0) {;}
  } else if (sizeof(clock_time_t) == 2) {
    while ((signed short)(clock_time() - endticks) < 0) {;}
  } else {
    while ((signed long )(clock_time() - endticks) < 0) {;}
  }
}
/*---------------------------------------------------------------------------*/
void
clock_delay_usec(uint16_t dt)
{
}
/*---------------------------------------------------------------------------*/
/**
 * Delay up to 65535 milliseconds.
 * \param howlong   How many milliseconds to delay.
 *
 * Neither interrupts nor the watchdog timer is disabled over the delay.
 * Platforms are not required to implement this call.
 * \note This will break for CPUs clocked above 260 MHz.
 */
void
clock_delay_msec(uint16_t howlong)
{

#if F_CPU>=16000000
  while(howlong--) clock_delay_usec(1000);
#elif F_CPU>=8000000
  uint16_t i=996;
  while(howlong--) {clock_delay_usec(i);i=999;}
#elif F_CPU>=4000000
  uint16_t i=992;
  while(howlong--) {clock_delay_usec(i);i=999;}
#elif F_CPU>=2000000
  uint16_t i=989;
  while(howlong--) {clock_delay_usec(i);i=999;}
#else
  uint16_t i=983;
  while(howlong--) {clock_delay_usec(i);i=999;}
#endif
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
