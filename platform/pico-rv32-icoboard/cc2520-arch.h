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
 */

/**
 * \file
 *         Platform configuration for the pico-rv32-icoboard
 *         Definitions for the radio hardware access
 */

#ifndef RADIO_CONF_H_
#define RADIO_CONF_H_

#include "icosoc.h"

extern void icosoc_cc2520_fifop_irq (void);

/* the low-level radio driver */
#define NETSTACK_CONF_RADIO   cc2520_driver

/*
 * SPI bus - CC2520 pin configuration.
 */

// Two instructions loop at F_CPU for 326us
#define CC2520_CONF_SYMBOL_LOOP_COUNT (F_CPU / (3067 * 2))

#define CC2520_RESET_SHIFT 0
#define CC2520_CCA_SHIFT   1

/* Pin status.CC2520 */
/* This implementation doesn't have enough pins for CC2520_FIFO_IS_1
 * on a separate pin. So we need to explicitly check for FIFO in
 * register with a SPI command, this is bit 7 in FSMSTAT1, see p.119 of
 * spec.
 */
#define CC2520_FIFOP_IS_1 (!!(icosoc_cc2520_fifop_read()))
#define CC2520_FIFO_IS_1  (!!(getreg(CC2520_FSMSTAT1) & BV(7)))
#define CC2520_SFD_IS_1   (!!(icosoc_cc2520_sfd_read()))
#define CC2520_CCA_IS_1   (!!(icosoc_cc2520_io_get() & (1<<CC2520_CCA_SHIFT)))

/* The CC2520 reset pin. */
#define SET_RESET_INACTIVE() \
    (icosoc_cc2520_io_set(icosoc_cc2520_io_get() | (1<<CC2520_RESET_SHIFT)))
#define SET_RESET_ACTIVE()   \
    (icosoc_cc2520_io_set(icosoc_cc2520_io_get() & (~(1<<CC2520_RESET_SHIFT))))

/* CC2520 voltage regulator enable pin: direct connection to VCC,
   we don't have enough pins to turn power on/off and the fpga will
   always need full power anyway
 */
#define SET_VREG_ACTIVE()
#define SET_VREG_INACTIVE()

/* CC2520 rising edge trigger for external interrupt 3 (FIFOP). */
#define CC2520_FIFOP_INT_INIT()                                       \
    ( register_irq(3, icosoc_cc2520_fifop_irq)                        \
    , icosoc_cc2520_fifop_set_config (icosoc_cc2520_fifop_trigger_re) \
    , icosoc_cc2520_fifop_read()                                      \
    )

/* FIFOP on external interrupt 3. */
#define CC2520_ENABLE_FIFOP_INT()     (enable_irq(3))
#define CC2520_DISABLE_FIFOP_INT()    (disable_irq(3))
#define CC2520_CLEAR_FIFOP_INT()      (icosoc_cc2520_fifop_read())

/*
 * Enables/disables CC2520 access to the SPI bus (Chip Select)
 */

// ENABLE CSn (active low)
#define CC2520_SPI_ENABLE()     icosoc_cc2520_spi_cs(0)
// DISABLE CSn (active low)
#define CC2520_SPI_DISABLE()    icosoc_cc2520_spi_cs(1)
#define CC2520_SPI_IS_ENABLED() icosoc_cc2520_spi_getcs()

#if TIMESYNCH_CONF_ENABLED
#undef CC2520_CONF_SFD_TIMESTAMPS
#define CC2520_CONF_SFD_TIMESTAMPS       1
#endif /* TIMESYNCH_CONF_ENABLED */

#endif /* RADIO_CONF_H_ */
