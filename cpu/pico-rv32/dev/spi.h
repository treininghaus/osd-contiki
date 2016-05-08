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
 *  SPI routines
 *  
 *  \author Ralf Schlatterbeck <rsc@runtux.com>.
 *
 */
/** \addtogroup picorv32
 * @{
 */
/**
 *  \file
 *  SPI macros
 *
 */

#ifndef SPI_H_
#define SPI_H_

#include "icosoc.h"

#define SPI_RXBUF spi_rxbuf

extern uint8_t spi_rxbuf;

/* Write one character to SPI */
#define SPI_WRITE(data) (spi_rxbuf = icosoc_cc2520_spi_xfer(data))
/* Currently no fast writing without waiting for transmission to
 * terminate
 */
#define SPI_WRITE_FAST(data) SPI_WRITE(data)

/* Read one character from SPI */
#define SPI_READ(data) (data = spi_rxbuf = icosoc_cc2520_spi_xfer(0))

/* No async operation, therefore no flush */
#define SPI_FLUSH()

#define SPI_WAITFORTx_BEFORE()
#define SPI_WAITFORTx_AFTER() SPI_WAITFOREOTx()
#define SPI_WAITFORTx_ENDED()
#define SPI_WAITFORTxREADY()

#define BV(x) (1<<x)

/* Used by cc2520_init probably for disabling interrupts, don't need
 * that here.
 */
#define splhigh() 1
#define splx(x) ((void)(x))

#endif /* SPI_H_ */

/** @} */
