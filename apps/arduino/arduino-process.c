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
 * \addgroup Arduino Process
 *
 * This wraps the Arduino-API entry points `loop` and `setup` in a
 * contiki process.
 *
 * If the normal contiki includes are used and resources initialized in
 * `setup`, Contiki resources can be used in an arduino sketch.
 *
 * @{
 */

/**
 * \file
 *        Wrapper for Arduino sketches
 * \author
 *        Ralf Schlatterbeck <rsc@runtux.com>
 *
 */

#include "arduino-process.h"
#include "hw_timer.h"
#include "hw-arduino.h"

PROCESS(arduino_sketch, "Arduino Sketch Wrapper");

PROCESS_THREAD(arduino_sketch, ev, data)
{
  PROCESS_BEGIN();

  arduino_pwm_timer_init ();
  setup ();
  while (1) {
    loop ();
    /* Give other processes a chance to run */
    process_post (&arduino_sketch, PROCESS_EVENT_CONTINUE, NULL);
    PROCESS_WAIT_EVENT();
  }
  PROCESS_END();
}


/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

/** @} */
