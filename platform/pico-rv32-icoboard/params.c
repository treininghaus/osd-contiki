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
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

#define DEBUG 1
#if DEBUG
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...)
#endif

#include "contiki.h"
#include <stdio.h>
#include <string.h>

#include "contiki-net.h"
#include "params.h"

#if CONTIKI_CONF_RANDOM_MAC
extern uint8_t rng_get_uint8(void);
static void
generate_new_eui64(uint8_t eui64[8]) {
  eui64[0] = 0x02;
  eui64[1] = rng_get_uint8();
  eui64[2] = rng_get_uint8();
  eui64[3] = 0xFF;
  eui64[4] = 0xFE;
  eui64[5] = rng_get_uint8();
  eui64[6] = rng_get_uint8();
  eui64[7] = rng_get_uint8();
}
#endif

const uint8_t default_mac_address[8] = 
    {0x02, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x01};

/* Hard coded parameters. */
uint8_t
params_get_eui64(uint8_t *eui64) {

#if CONTIKI_CONF_RANDOM_MAC
    PRINTD("Generating random EUI64 MAC\n");
    generate_new_eui64(eui64);
    return 1;
#else
    uint8_t i;
    for (i=0;i<sizeof(default_mac_address);i++) {
      eui64[i] = default_mac_address [i];
    }
    return 0;
#endif
}
