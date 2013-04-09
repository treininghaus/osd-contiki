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
**    t4-servo-sensor
**
** Purpose
**    Implements a sensor around t4servo.c
**
**
** Revision Dates
**    31-Mar-2013 (MPR) Creation
**    ««revision-date»»···
**--
*/

#ifndef __T4_SERVO_SENSOR_H__
#define __T4_SERVO_SENSOR_H__

#include "lib/sensors.h"

extern const struct sensors_sensor t4_servo_sensor;

#define T4_SENSOR_NAME "T4-Sensor"

#define T4_SENSOR_1_PORT 0x0E
#define T4_SENSOR_1_DDR  0x0D
#define T4_SENSOR_1_PIN  PINE4

#define T4_SENSOR_2_PORT 0x0E
#define T4_SENSOR_2_DDR  0x0D
#define T4_SENSOR_2_PIN  PINE3

#define T4_SENSOR_3_PORT 0x0E
#define T4_SENSOR_3_DDR  0x0D
#define T4_SENSOR_3_PIN  PINE5

#define T4_SENSOR_4_PORT 0x0E
#define T4_SENSOR_4_DDR  0x0D
#define T4_SENSOR_4_PIN  PINE6

#endif /* __T4_SERVO_SENSOR_H__ */



