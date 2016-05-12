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
 *  Interrupt handling for Timer and other interrupts
 *  
 *  \author Ralf Schlatterbeck <rsc@runtux.com>.
 *
 */
/** \addtogroup picorv32
 * @{
 */
/**
 *  \defgroup picorv32 irq implementation
 * @{
 */
/**
 *  \file
 *  This file contains PicoRV32-specific code to implement
 *  Interrupt handling for Timer and other interrupts
 *
 */
#include "irq.h"
#include <stdio.h>
#include <assert.h>

/* Initially allow bus-error and illegal instruction / sbreak */
static uint32_t irqmask = ~0x6;

/* Interrupt table, for convenience we waste some memory for the lower 3
 * callbacks
 */
static void (*irq_callback_functions [MAXIRQ + 1])(void);

/** \brief irq handler
 */

void
irq_handler(uint32_t irq_mask, uint32_t *regs)
{
    // Other interrupts
    if (irq_mask & (((1 << (MAXIRQ + 2)) - 1) & ~0x6)) {
        int i;
        for (i=0; i<=MAXIRQ; i++) {
            if (irq_mask & (1<<i)) {
                if (irq_callback_functions [i] == NULL) {
                    printf ("Ooops: IRQ without callback: %d\n", i);
                } else {
                    irq_callback_functions [i] ();
                }
            }
        }
    }
    // SBREAK, ILLINS or BUSERROR
    if (irq_mask & 6) {
        if (irq_mask & 4) {
            if (irq_callback_functions [2]) {
                irq_callback_functions [2] ();
            } else {
                printf("Bus Error!\n");
                icosoc_sbreak();
            }
        }
        if (irq_mask & 2) {
            if (irq_callback_functions [1]) {
                irq_callback_functions [1] ();
            } else {
                printf("Illegal Instruction or SBREAK!\n");
                icosoc_sbreak();
            }
        }
    }
}

void irq_init(void)
{
    icosoc_irq(irq_handler);
    icosoc_maskirq(irqmask);
}

void register_irq (int irq, void (*callback)(void))
{
    assert (irq <= MAXIRQ);
    irq_callback_functions [irq] = callback;
}

void enable_irq (int irq)
{
    assert (irq <= MAXIRQ);
    if (irq_callback_functions [irq] != NULL) {
        irqmask &= (~(1<<irq));
        icosoc_maskirq(irqmask);
    }
}

void disable_irq (int irq)
{
    assert (irq <= MAXIRQ);
    irqmask |= (1<<irq);
    icosoc_maskirq(irqmask);
}

/** @} */
/** @} */
