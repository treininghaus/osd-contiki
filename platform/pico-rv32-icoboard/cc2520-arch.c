/*
 * Copyright (c) 2011, Swedish Institute of Computer Science
 *               2016, Dr. Ralf Schlatterbeck Open Source Consulting
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

#include "contiki.h"
#include "contiki-net.h"

#include "icosoc.h"
#include "cc2520-arch.h"
#include "dev/cc2520/cc2520.h"

#ifdef CC2520_CONF_SFD_TIMESTAMPS
#define CONF_SFD_TIMESTAMPS CC2520_CONF_SFD_TIMESTAMPS
#endif /* CC2520_CONF_SFD_TIMESTAMPS */

#ifndef CONF_SFD_TIMESTAMPS
#define CONF_SFD_TIMESTAMPS 0
#endif /* CONF_SFD_TIMESTAMPS */

#ifdef CONF_SFD_TIMESTAMPS
#include "cc2520-arch-sfd.h"
#endif

/*---------------------------------------------------------------------------*/
void icosoc_cc2520_fifop_irq (void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  /* See Errata 1.2.2 */
  if (!CC2520_FIFOP_IS_1) {
    return;
  }
  if (!CC2520_FIFOP_IS_1) {
    return;
  }
  cc2520_interrupt();

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
void
cc2520_arch_init(void)
{
  // All inputs except for RESET pin which is an output
  icosoc_cc2520_io_dir (BV(CC2520_RESET_SHIFT));

  // Prescaler set to F_CPU / 4MHz will result in <= 4MHz
  // rate, chip should perform up to 8MHz)
  icosoc_cc2520_spi_prescale (F_CPU / 4000000);

  // SPI mode cpol, cpha
  icosoc_cc2520_spi_mode (0, 0);

#if CONF_SFD_TIMESTAMPS
  cc2520_arch_sfd_init();
#endif

  CC2520_SPI_DISABLE();                /* Unselect radio. */
}
/*---------------------------------------------------------------------------*/
