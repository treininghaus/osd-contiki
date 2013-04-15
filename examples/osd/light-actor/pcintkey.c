/*
 *  Copyright (c) 2010  harald pichler
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file
 *
 * \brief
 *      This file provides Raven KEY support.
 *
 * \author
 *      Harald Pichler harald@the-develop.net
 *
 */

#include <avr/interrupt.h>
#include "dev/led.h"
#include "pcintkey.h"

/*---------------------------------------------------------------------------*/

ISR(PCINT0_vect)
{
//  if(BUTTON_CHECK_IRQ()) {
//    if(timer_expired(&debouncetimer)) {
    led1_on();
//      timer_set(&debouncetimer, CLOCK_SECOND / 4);
//      sensors_changed(&button_sensor);
    led1_off();
//    }
//  }
}
/**
 *   \brief This will intialize the KEY for button readings.
*/
void
key_init(void)
{
    // Pairing Button
    DDRB |= (0<<DDB4); // Set pin as input
    PORTB |= (1<<PORTB4); // Set port PORTE bint 5 with pullup resistor
    // ext1
    DDRB |= (0<<DDB5); // Set pin as input
    PORTB |= (1<<PORTB5); // Set port PORTE bint 5 with pullup resistor
    // ext2
    DDRB |= (0<<DDB6); // Set pin as input
    PORTB |= (1<<PORTB6); // Set port PORTE bint 5 with pullup resistor
    // Interrupt
    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT6);
}

/*---------------------------------------------------------------------------*/

/**
 *   \brief This will poll run key_task() to determine if a button has been pressed.
 *
 *   \retval True if button is pressed
 *   \retval False if button is not pressed
*/
uint8_t
is_button_ext4(void)
{
    /* Return true if button has been pressed. */
    if ( PINB & (1<<PINB4) ) {
        return 0;
    }
    else{
        return 1;
    }
}
uint8_t
is_button_ext5(void)
{
    /* Return true if button has been pressed. */
    if ( PINB & (1<<PINB5) ) {
        return 0;
    }
    else{
        return 1;
    }
}
uint8_t
is_button_ext6(void)
{
    /* Return true if button has been pressed. */
    if ( PINB & (1<<PINB6) ) {
        return 0;
    }
    else{
        return 1;
    }
}
