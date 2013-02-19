/*
* Copyright (c) 2012, BinaryLabs.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the Institute nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*
* @(#)$Id: adc.c,v 1.1 2010/08/25 19:34:06 nifi Exp $
*/

/**
* \file
* ADC file for Atmega128rfa1.
* \author
* Paulo Louro <paulolouro@binarylabs.dk>
*/

#include <avr/io.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int readADC(uint8_t pin)
{
  int result = 0;

  if ( pin >= 14 )
    pin -= 14;

  ADMUX = _BV(REFS1) | _BV(REFS0) | ( pin & 7 ) ;
  ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS2) ;
  sbi(ADCSRA,ADSC);
  loop_until_bit_is_clear(ADCSRA,ADSC);


  result = ADC;
  
  ADCSRA=0; //disable ADC
  ADMUX=0; //turn off internal vref

  return result;
}

/**
* \return Internal temperature in 0.01C, e.g. 25C is 2500
*/
int readInternalTemp(void)
{
  int reading = 0;

  ADCSRB |= _BV(MUX5);
  ADMUX = _BV(REFS1) | _BV(REFS0) | 0b1001 ;
  ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS2) ;

  sbi(ADCSRA,ADSC);
  loop_until_bit_is_clear(ADCSRA,ADSC);
  reading = ADC;

  ADCSRA=0; //disable ADC
  ADCSRB=0; //disable ADC
  ADMUX=0; //turn off internal vref

  return reading * 113 - 27280;
}