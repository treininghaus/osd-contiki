/*
 *  Copyright (c) 2012  harald pichler
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
 *      This file provides Raven LED support.
 *
 * \author
 *      Harald Pichler harald@the-develop.net
 *
 */

#include "relay.h"

/**
 * \addtogroup relay
 * \{
*/
/*---------------------------------------------------------------------------*/

/**
 * \brief Turns the RELAY1 on.
*/
void
relay1_off(void)
{
    PORTB &= ~(1<<PINB0);
    DDRB  |=  (1<<DDB0);
}

/*---------------------------------------------------------------------------*/

/**
 *  \brief Turns the RELAY1 off
*/
void
relay1_on(void)
{
    PORTB |= (1<<PINB0);
    DDRB  |= (1<<DDB0);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Turns the RELAY2 on.
*/
void
relay2_off(void)
{
    PORTB &= ~(1<<PINB1);
    DDRB  |=  (1<<DDB1);
}

/*---------------------------------------------------------------------------*/

/**
 *  \brief Turns the RELAY2 off.
*/
void
relay2_on(void)
{
    PORTB |= (1<<PINB1);
    DDRB  |= (1<<DDB1);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Turns the RELAY3 on.
*/
void
relay3_off(void)
{
    PORTB &= ~(1<<PINB2);
    DDRB  |=  (1<<DDB2);
}

/*---------------------------------------------------------------------------*/

/**
 *  \brief Turns the RELAY3 off.
*/
void
relay3_on(void)
{
    PORTB |= (1<<PINB2);
    DDRB  |= (1<<DDB2);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Turns the RELAY4 on.
*/
void
relay4_off(void)
{
    PORTB &= ~(1<<PINB3);
    DDRB  |=  (1<<DDB3);
}

/*---------------------------------------------------------------------------*/

/**
 *  \brief Turns the RELAY4 off.
*/
void
relay4_on(void)
{
    PORTB |= (1<<PINB3);
    DDRB  |= (1<<DDB3);
}
/*---------------------------------------------------------------------------*/
