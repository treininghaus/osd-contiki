// Copyright (C) 2014 Marcus Priesch, All rights reserved
// In Prandnern 31, A--2122 Riedenthal, Austria. office@priesch.co.at
// ****************************************************************************
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// ****************************************************************************
//
//++
// Name
//    t4-servo-config
//
// Purpose
//    configuration for t4-servo
//
// Revision Dates
//    10-Mar-2014 (MPR) Creation
//    ««revision-date»»···
//--

#include <avr/io.h>

#ifndef __T4_SERVO_CONFIG_H__
#define __T4_SERVO_CONFIG_H__ 1

// timing: (X:pwm, _:zero)
//
//  X_______________XXXXXXXXXXXXX_______________XXXXXXXXXXXXX
//
//  |<------------->|<--------->|<------------->|<--------->| ...
//    SERVO_OFFSET    SERVO_MAX   SERVO_OFFSET    SERVO_MAX
//
//  within SERVO_MAX you get the PWM set with servo_set (id, pwm)
//  if you set SERVO_OFFSET to zero, you get a pwm from 0 to 100% duty cycle
//
//  the period of the smallest tick is derived from the timer4 prescaler
//  plus the ISR overhead. however, the latter one should be constant
//  regardless of the pwm signal generated

#define SERVO_OFFSET 0
#define SERVO_MIN 0
#define SERVO_MAX 64

// Counter value for timer 4 without any prescaler for a single tick
#define PWMFREQ 200

#define T4_VALUE F_CPU/SERVO_MAX/PWMFREQ

#define SERVO_COUNT 8

#define T4_SENSOR_NAME "Dual RGBW Led"

#endif


