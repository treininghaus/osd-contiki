/*
** Copyright (C) 2013 Marcus Priesch, All rights reserved
** In Prandnern 31, A--2122 Riedenthal, Austria. office@priesch.co.at
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the Institute nor the names of its contributors
**    may be used to endorse or promote products derived from this software
**    without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
**++
** Name
**    t4-servo
**
** Purpose
**    Implements software pwm on any portpins via timer 4
**
**
** Revision Dates
**    31-Mar-2013 (MPR) Creation
**    ��revision-date�����
**--
*/

#include "t4-servo.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"

static servo_channel_type servo_channels [SERVO_COUNT];

// timer 4: CTC OCR4A
#define WGM4 0x4

void t4_servo_init (void)
{
  unsigned char channel;

  for (channel = 0; channel < SERVO_COUNT;  channel ++)
    {
      servo_channels [channel].duty = SERVO_MAX/2;
      t4_servo_set_io (channel, DEFAULT_PORT, DEFAULT_DDR, DEFAULT_PIN);
    }

  cli ();
  TCCR4A               = 0x00;
  TCCR4A_struct.wgm4   =  WGM4 & 0x3;
  TCCR4B_struct.wgm4   = (WGM4 & 0xc) >> 2;
  TCCR4B_struct.cs4    = 0x1; // No prescaler
  TCCR4C               = 0x00;
  OCR4A                = T4_VALUE;
  TIMSK4_struct.ocie4a = 1;
  sei();
}

void t4_servo_off (void)
  {
    TIMSK4_struct.ocie4a = 0;
  }

int t4_servo_set_io
  ( unsigned char channel
  , unsigned char port
  , unsigned char ddr
  , unsigned char pin
  )
  {
    if (channel >= SERVO_COUNT)
      return -1;

    servo_channels [channel].port = port;
    servo_channels [channel].ddr  = ddr;
    servo_channels [channel].pin  = 1 << pin;

    _SFR_IO8 (ddr) |= pin;

    return 0;
  }

int t4_servo_get (unsigned int channel)
  {
    if (channel >= SERVO_COUNT)
      return -1;

    return servo_channels [channel].duty;
  }

int t4_servo_set (unsigned int channel, unsigned int duty)
  {
    if (channel >= SERVO_COUNT)
      return -1;

    if (duty > SERVO_MAX)
      return -2;

    if (duty < SERVO_MIN)
      return -3;

    servo_channels [channel].duty = duty;
    return 0;
  }

ISR (TIMER4_COMPA_vect, ISR_NOBLOCK)
  {
    unsigned char channel;
    static unsigned int tick_count = 0;

  cli ();
    for (channel = 0; channel < SERVO_COUNT;  channel ++)
      {
        if (tick_count < servo_channels [channel].duty)
          {
            // turn on
            _SFR_IO8 (servo_channels [channel].ddr ) |= servo_channels [channel].pin;
            _SFR_IO8 (servo_channels [channel].port) |= servo_channels [channel].pin;
          }
        else
          {
            // turn off
            _SFR_IO8 (servo_channels [channel].port) &= ~(servo_channels [channel].pin);
            _SFR_IO8 (servo_channels [channel].ddr ) |=   servo_channels [channel].pin;
          }
      }

    tick_count ++;

    if (tick_count >= (SERVO_MAX + SERVO_OFFSET))
      {
        tick_count = 0;
      }
  sei();
  }
