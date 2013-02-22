/*
 * Copyright (c) 2007, Swedish Institute of Computer Science
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
 */

/*
 * Device driver for the Sensirion SHT1x/SHT7x family of humidity and
 * temperature sensors.
 */

#include "contiki.h"
#include <stdio.h>
#include <dev/servo.h>

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*
 * servo device
 */

#define SERVO_OFFSET	1000
#define SERVO_MAX	1000
#define SERVO_INIT	 500

unsigned int servoa=SERVO_INIT;
unsigned int servob=SERVO_INIT;

void
servo_init(void)
{
// Port B initialization
// Func7=Out Func6=Out Func5=Out Func4=In Func3=In Func2=In Func1=In Func0=In
// State7=0 State6=0 State5=0 State4=T State3=T State2=T State1=T State0=T
PORTB=0x00;
DDRB=0xE0;
// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 2000.000 kHz
// Mode: Ph. & fr. cor. PWM top=ICR1
// OC1A output: Connected
// OC1B output: Connected
// OC1C output: Connected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=0xA8;
TCCR1B=0x12;
TCNT1H=0x00;
TCNT1L=0x00;
// ICR1 has a computed value of 20,000 - see the ESawdust.com/blog article for how this
// value was derived.
// 20000 == 0x4e20 so that's what goes into the high and low byte of the ICR1 register
// alternatively, Codevision would let you just do ICR1 = 20000;
ICR1H=0x4E;
ICR1L=0x20;

/* OCR1AH=0x00;
OCR1AL=0x00;
*/
// OCR1A will govern the steering servo, OCR1B will govern throttle
OCR1A = 1500;  // set it to an initial position somewhere in the middle of the 1 to 2ms range

// OCR1A will govern the steering servo, OCR1B will govern throttle
OCR1B = 1500;  // set it to an initial position somewhere in the middle of the 1 to 2ms range
// start with motor off - no duty cycle at all
OCR1CH=0x00;
OCR1CL=0x00;
}
/*---------------------------------------------------------------------------*/
/*
 * Power of device.
 */
void
servo_off(void)
{

}
/*---------------------------------------------------------------------------*/
/*
 * get servo position
 */
unsigned int
servo_get(unsigned int i)
{
  if(i==0)
    return servoa;
  if(i==1)
    return servob;
  return 0;
}
/*---------------------------------------------------------------------------*/
/*
 * Set servo position
 */
unsigned int
servo_set(unsigned i,unsigned int j)
{
  if(j > SERVO_MAX)
	j=SERVO_MAX;

  if(i==0)
    servoa=j;
    OCR1A = SERVO_OFFSET + servoa;
  return 1;
  if(i==1)
    servob=j;
    OCR1A = SERVO_OFFSET + servob;
  return 1;
}
