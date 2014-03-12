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
//    t4-servo-hardware
//
// Purpose
//    configuration for t4-servo
//
// Revision Dates
//    10-Mar-2014 (MPR) Creation
//    ««revision-date»»···
//--

#include <avr/io.h>

#ifndef __T4_SERVO_HARDWARE_H__
#define __T4_SERVO_HARDWARE_H__ 1

static servo_channel_type servo_channels [] =
{ /* PORTx, DDRx, PINxn, PWM */
  {0x05, 0x04, 1 << PINB7, 0} // r1
, {0x0E, 0x0D, 1 << PINE2, 0} // g1
, {0x05, 0x04, 1 << PINB6, 0} // b1
, {0x05, 0x04, 1 << PINB5, 0} // w1
, {0x05, 0x04, 1 << PINB1, 0} // r2
, {0x05, 0x04, 1 << PINB0, 0} // g2
, {0x05, 0x04, 1 << PINB2, 0} // b2
, {0x05, 0x04, 1 << PINB3, 0} // w2
};

#endif
