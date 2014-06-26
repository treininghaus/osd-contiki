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
 * \defgroup hardware timer
 *
 * This module wraps hardware timers of AVR microcontrollers.
 * Currently we only support 16-bit timers. The main focus is currently
 * on PWM generation. But input capture and interrupt routines are on
 * the TODO list, see below. We currently support the AVR ATmega128RFA1
 * so this should be generalized to supported timers of other AVR
 * microcontrollers.
 *
 * Datasheet references in the following refer to ATmega128RFA1 data sheet
 *
 * TODO: Allow input capture.
 * TODO: Allow definition of interrupt routine; check if merkur board
 *       supports necessary pins.
 * TODO: Generalize for 8-bit timers.
 * TODO: Check other AVR microcontrollers and the supported timers.
 *
 * @{
 */

/**
 * \file
 *        Header file for hardware timer of AVR microcontrollers
 * \author
 *        Ralf Schlatterbeck <rsc@runtux.com>
 *
 */

#ifndef hw_timer_h
#define hw_timer_h

#include "contiki.h"
#include "rtimer-arch.h"

#ifndef PLAT_TIMER
#define PLAT_TIMER -1
#endif

/*
 * All routines return a negative number for error and 0 for success.
 * The negative return value indicates the error.
 */
#define HWT_ERR_INVALID_TIMER   (-1)
#define HWT_ERR_INVALID_WGM     (-2)
#define HWT_ERR_INVALID_COM     (-3)
#define HWT_ERR_INVALID_CLOCK   (-4)
#define HWT_ERR_INVALID_CHANNEL (-5)

/*
 * Timer waveform generation modes (WGM), see data sheet
 * chapter 18 "16-bit Timer/Counter (Timer/Counter 1,3,4, and 5)
 * 18.9 "Modes of Operation", in particular Table 18-5
 */
#define HWT_WGM_NORMAL                    0
#define HWT_WGM_PWM_PHASE_8_BIT           1
#define HWT_WGM_PWM_PHASE_9_BIT           2
#define HWT_WGM_PWM_PHASE_10_BIT          3
#define HWT_WGM_CTC_OCRA                  4
#define HWT_WGM_PWM_FAST_8_BIT            5
#define HWT_WGM_PWM_FAST_9_BIT            6
#define HWT_WGM_PWM_FAST_10_BIT           7
#define HWT_WGM_PWM_PHASE_FRQ_ICR         8
#define HWT_WGM_PWM_PHASE_FRQ_OCRA        9
#define HWT_WGM_PWM_PHASE_ICR            10
#define HWT_WGM_PWM_PHASE_OCRA           11
#define HWT_WGM_CTC_ICR                  12
#define HWT_WGM_RESERVED                 13
#define HWT_WGM_PWM_FAST_ICR             14
#define HWT_WGM_PWM_FAST_OCRA            15
#define HWT_WGM_MASK                     15
#define HWT_WGM_MASK_LOW                  3
#define HWT_WGM_MASK_HIGH                (HWT_WGM_MASK - HWT_WGM_MASK_LOW)
#define HWT_WGM_SHIFT_LOW                0
#define HWT_WGM_SHIFT_HIGH               1

/*
 * Timer compare output modes (COM), 
 * chapter 18 "16-bit Timer/Counter (Timer/Counter 1,3,4, and 5)
 * 18.8 "Compare Match Output Unit", in particular Tables 18-2,3,4
 */
#define HWT_COM_NORMAL 0
#define HWT_COM_TOGGLE 1
#define HWT_COM_CLEAR  2
#define HWT_COM_SET    3
#define HWT_COM_MASK   3

/*
 * Clock select, clock can be off, use prescaler or external clock
 * source on Tn pin. See Table 18-11 (for Timer 1 but this is the same
 * for all the timers).
 */
#define HWT_CLOCK_OFF             0
#define HWT_CLOCK_PRESCALER_1     1
#define HWT_CLOCK_PRESCALER_8     2
#define HWT_CLOCK_PRESCALER_64    3
#define HWT_CLOCK_PRESCALER_256   4
#define HWT_CLOCK_PRESCALER_1024  5
#define HWT_CLOCK_EXTERN_FALLING  6
#define HWT_CLOCK_EXTERN_RISING   7
#define HWT_CLOCK_MASK            7

/*
 * Timer channels A B C
 */
#define HWT_CHANNEL_A 0
#define HWT_CHANNEL_B 1
#define HWT_CHANNEL_C 2
#define HWT_CHANNEL_D 3
#define HWT_CHANNEL_MASK 3

/* The following macros are defined for timer values 1,3,4,5 */
#define HWT_ICR(t) \
    ((t)<4?((t)==1?(&ICR1) :(&ICR3)) :((t)==4?(&ICR4) :(&ICR5)))
#define HWT_OCRA(t) \
    ((t)<4?((t)==1?(&OCR1A):(&OCR3A)):((t)==4?(&OCR4A):(&OCR5A)))
#define HWT_OCRB(t) \
    ((t)<4?((t)==1?(&OCR1B):(&OCR3B)):((t)==4?(&OCR4B):(&OCR5B)))
#define HWT_OCRC(t) \
    ((t)<4?((t)==1?(&OCR1C):(&OCR3C)):((t)==4?(&OCR4C):(&OCR5C)))

#define HWT_OCR(t,c)                             \
  ( (c)==HWT_CHANNEL_A                           \
  ? (HWT_OCRA(t))                                \
  : ((c)==HWT_CHANNEL_B?HWT_OCRB(t):HWT_OCRC(t)) \
  )

#define HWT_TCCRA(t)               \
    ( (t)<4                        \
    ? ((t)==1?(&TCCR1A):(&TCCR3A)) \
    : ((t)==4?(&TCCR4A):(&TCCR5A)) \
    )
#define HWT_TCCRB(t)               \
    ( (t)<4                        \
    ? ((t)==1?(&TCCR1B):(&TCCR3B)) \
    : ((t)==4?(&TCCR4B):(&TCCR5B)) \
    )
#define HWT_TCCRC(t)               \
    ( (t)<4                        \
    ? ((t)==1?(&TCCR1C):(&TCCR3C)) \
    : ((t)==4?(&TCCR4C):(&TCCR5C)) \
    )
#define HWT_TCNT(t)                \
    ( (t)<4                        \
    ? ((t)==1?(&TCNT1) :(&TCNT3))  \
    : ((t)==4?(&TCNT4) :(&TCNT5))  \
    )

#define HWT_SET_COM(timer, channel, com)                            \
    ((*HWT_TCCRA (timer) &= ~(HWT_COM_MASK << (6 - 2 * (channel)))) \
    ,(*HWT_TCCRA (timer) |=  ((com)        << (6 - 2 * (channel)))) \
    )

#define HWT_CHECK_TIMER(timer)                                                 \
  do {                                                                         \
    if ((timer) == 0 || (timer) == 2 || (timer) == PLAT_TIMER || (timer) > 5) {\
      return HWT_ERR_INVALID_TIMER;                                            \
    }                                                                          \
  } while (0)

#define HWT_CHECK_CHANNEL(chan)         \
  do {                                  \
    if ((chan) > HWT_CHANNEL_C) {       \
      return HWT_ERR_INVALID_CHANNEL;   \
    }                                   \
  } while (0)

#define HWT_PWM_FAST              0
#define HWT_PWM_PHASE_CORRECT     1
#define HWT_PWM_PHASE_FRQ_CORRECT 2

/**
 * \brief Initialize the hardware timer with the given settings
 * \param timer: Timer to use
 * \param wgm: waveform generation mode to use, see definitions
 * \param clock: Prescaler or external clock settings
 * \param maxt: Maximum counter value, not used for fixed modes, this
 *        sets ICRn for the ICR modes and OCRnA for the OCR modes
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 *
 * The initial compare output mode is set to HWT_COM_NORMAL (off) for
 * all outputs (pwm disabled).
 *
 * Note that this sets the compare output mode COM registers to 0,
 * turning off PWM on outputs.
 */
static inline int8_t
hwtimer_ini (uint8_t timer, uint8_t wgm, uint8_t clock, uint16_t maxt)
{
  int8_t i;
  HWT_CHECK_TIMER (timer);
  if (wgm > HWT_WGM_MASK || wgm == HWT_WGM_RESERVED) {
    return HWT_ERR_INVALID_WGM;
  }
  if (clock > HWT_CLOCK_MASK) {
    return HWT_ERR_INVALID_CLOCK;
  }
  /* Turn off clock, no need to disable interrupt */
  *HWT_TCCRB (timer) &= ~HWT_CLOCK_MASK;

  *HWT_TCCRA (timer) &= ~(HWT_WGM_MASK_LOW         << HWT_WGM_SHIFT_LOW);
  *HWT_TCCRA (timer) |= ((wgm & HWT_WGM_MASK_LOW)  << HWT_WGM_SHIFT_LOW);
  *HWT_TCCRB (timer) &= ~(HWT_WGM_MASK_HIGH        << HWT_WGM_SHIFT_HIGH);
  *HWT_TCCRB (timer) |= ((wgm & HWT_WGM_MASK_HIGH) << HWT_WGM_SHIFT_HIGH);

  for (i=0; i<3; i++) {
    HWT_SET_COM (timer, i, HWT_COM_NORMAL);
  }

  if (  wgm == HWT_WGM_PWM_PHASE_FRQ_ICR
     || wgm == HWT_WGM_PWM_PHASE_ICR
     || wgm == HWT_WGM_CTC_ICR
     || wgm == HWT_WGM_PWM_FAST_ICR
     )
  {
    *HWT_ICR (timer) = maxt;
  }

  if (  wgm == HWT_WGM_CTC_OCRA
     || wgm == HWT_WGM_PWM_PHASE_FRQ_OCRA
     || wgm == HWT_WGM_PWM_PHASE_OCRA
     || wgm == HWT_WGM_PWM_FAST_OCRA
     )
  {
    *HWT_OCRA (timer) = maxt;
  }

  /* Set clock, finally */
  *HWT_TCCRB (timer) |= clock;
  return 0;
}

/* Needed for implementation */
#define HWT_PERIOD_MAX_ (0xFFFFFFFF / (F_CPU / 1000000))
/* for 16-bit timer: */
#define HWT_TICKS_MAX_  0xFFFF
#define HWT_TICKS_MIN_  0xFF

/**
 * \brief Convenience function to initialize hardware timer for PWM
 * \param timer: Timer to use
 * \param pwm_type: See HWT_PWM* macros
 * \param period_us: Period of the timer in µs
 * \param ocra: Use OCRnA register if set, ICRn otherwise
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 *
 * This function can be called instead of hwtimer_ini and sets up the
 * timer for one of the PWM modes. There are fast, phase-correct and
 * phase- and frequency correct modes, refer to the datasheet for
 * semantics.
 *
 * The function tries to initialize the timer to a mode that doesn't use
 * one of the internal registers OCRnA or ICRn for specifying the upper
 * bound of the counter. For fast PWM and phase-correct PWM there are
 * fixed 8-, 9-, and 10-bit modes that can be used if the computed value
 * fits one of these setups.
 *
 * We try to get the *maximum* prescaler that still permits a tick
 * resolution of at least 8 bit. This will not work for very high
 * frequencies.
 *
 * If the specified period is too large to fit into a 16-bit timer we
 * take the maximum period that is still possible, this may be
 * substatially higher than specified.
 *
 * Note that when using OCRnA for the upper bound of the counter, the
 * pin associated with this register can not be used for PWM. Instead it
 * can be used to change the period.
 */
static inline int8_t
hwtimer_pwm_ini (uint8_t timer, uint32_t period_us, uint8_t pwm_type, uint8_t ocra)
{
  uint32_t ticks = 0;
  uint8_t  clock = HWT_CLOCK_PRESCALER_1024;
  uint8_t  wgm   = HWT_WGM_NORMAL;
  HWT_CHECK_TIMER (timer);
  if (period_us > HWT_PERIOD_MAX_) {
    period_us = HWT_PERIOD_MAX_;
  }
  ticks = (F_CPU / 1000000) * period_us;
  /* Non-fast PWM modes have half the frequency */
  if (pwm_type != HWT_PWM_FAST) {
    ticks >>= 1;
  }

  /*
   * Divisors are 1, 8, 64, 256, 1024, shifts between these are
   * 3, 3, 2, 2, respectively. We modify `ticks` in place, the AVR can
   * shift only one bit in one instruction, so shifting isn't cheap.
   * We try to get the *maximum* prescaler that still permits a tick
   * resolution of at least 8 bit.
   */
  if (ticks <= (HWT_TICKS_MIN_ << 3)) {
    clock = HWT_CLOCK_PRESCALER_1;
  }
  else if ((ticks >>= 3) <= (HWT_TICKS_MIN_ << 3)) {
    clock = HWT_CLOCK_PRESCALER_8;
  }
  else if ((ticks >>= 3) <= (HWT_TICKS_MIN_ << 2)) {
    clock = HWT_CLOCK_PRESCALER_64;
  }
  else if ((ticks >>= 2) <= (HWT_TICKS_MIN_ << 2)) {
    clock = HWT_CLOCK_PRESCALER_256;
  }
  else if ((ticks >>= 2) > HWT_TICKS_MAX_) {
    ticks = HWT_TICKS_MAX_;
  }
  switch (pwm_type) {
    case HWT_PWM_FAST:
      wgm = ocra ? HWT_WGM_PWM_FAST_OCRA : HWT_WGM_PWM_FAST_ICR;
      break;
    case HWT_PWM_PHASE_CORRECT:
      wgm = ocra ? HWT_WGM_PWM_PHASE_OCRA : HWT_WGM_PWM_PHASE_ICR;
      break;
    case HWT_PWM_PHASE_FRQ_CORRECT:
    default:
      wgm = ocra ? HWT_WGM_PWM_PHASE_FRQ_OCRA : HWT_WGM_PWM_PHASE_FRQ_ICR;
      break;
  }
  /* Special 8- 9- 10-bit modes */
  if (pwm_type == HWT_PWM_FAST || pwm_type == HWT_PWM_PHASE_CORRECT) {
    if (ticks == 0xFF) {
      wgm = (pwm_type == HWT_PWM_FAST)
          ? HWT_WGM_PWM_FAST_8_BIT
          : HWT_WGM_PWM_PHASE_8_BIT;
    }
    else if (ticks == 0x1FF) {
      wgm = (pwm_type == HWT_PWM_FAST)
          ? HWT_WGM_PWM_FAST_9_BIT
          : HWT_WGM_PWM_PHASE_9_BIT;
    }
    else if (ticks == 0x3FF) {
      wgm = (pwm_type == HWT_PWM_FAST)
          ? HWT_WGM_PWM_FAST_10_BIT
          : HWT_WGM_PWM_PHASE_10_BIT;
    }
  }
  return hwtimer_ini (timer, wgm, clock, ticks);
}

/*
 * Simple init macro for sane default values
 */
#define hwtimer_pwm_ini_simple (timer, period_us) \
  hwtimer_pwm_ini ((timer), HWT_PWM_PHASE_CORRECT, (period_us), 0)

/**
 * \brief  Maximum timer value usable in hwtimer_set_pwm
 * \param  timer: Timer to use
 * \return max. timer value according to current timer setup
 *         negative value if wrong timer given
 *         a positive value is guaranteed to fit into 16 bit unsigned.
 */
static inline int32_t hwtimer_pwm_max_ticks (uint8_t timer)
{
  uint8_t wgm = 0;
  HWT_CHECK_TIMER (timer);
  wgm = ((*HWT_TCCRA (timer) >> HWT_WGM_SHIFT_LOW)  & HWT_WGM_MASK_LOW)
      | ((*HWT_TCCRB (timer) >> HWT_WGM_SHIFT_HIGH) & HWT_WGM_MASK_HIGH)
      ;
  switch (wgm) {
    case HWT_WGM_PWM_PHASE_8_BIT:
    case HWT_WGM_PWM_FAST_8_BIT:
      return 0xFF;
    case HWT_WGM_PWM_PHASE_9_BIT:
    case HWT_WGM_PWM_FAST_9_BIT:
      return 0x1FF;
    case HWT_WGM_PWM_PHASE_10_BIT:
    case HWT_WGM_PWM_FAST_10_BIT:
      return 0x3FF;
    case HWT_WGM_CTC_OCRA:
    case HWT_WGM_PWM_PHASE_FRQ_OCRA:
    case HWT_WGM_PWM_PHASE_OCRA:
    case HWT_WGM_PWM_FAST_OCRA:
      return *HWT_OCRA (timer);
    case HWT_WGM_PWM_PHASE_FRQ_ICR:
    case HWT_WGM_PWM_PHASE_ICR:
    case HWT_WGM_CTC_ICR:
    case HWT_WGM_PWM_FAST_ICR:
      return *HWT_ICR (timer);
    case HWT_WGM_NORMAL:
      return 0xFFFF;
  }
  return HWT_ERR_INVALID_WGM;
}

/*
 * The following functions are defined inline to allow for compiler
 * optimizations if some of the parameters are constant.
 */

/**
 * \brief  Set PWM duty cycle
 * \param  timer: Timer to use
 * \param  channel: Channel to use, see HWT_CHANNEL definitions
 * \param  pwm: Duty cycle
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 *
 * Note that the available range for the duty cycle depends on the timer
 * setup and the chosen mode.
 */
static inline int8_t
hwtimer_set_pwm (uint8_t timer, uint8_t channel, uint16_t pwm)
{
  uint8_t sreg = 0;
  HWT_CHECK_TIMER (timer);
  HWT_CHECK_CHANNEL (channel);
  sreg = SREG;
  cli ();
  *HWT_OCR (timer, channel) = pwm;
  SREG = sreg;
  return 0;
}

/**
 * \brief  Set compare output mode
 * \param  timer: Timer to use
 * \param  channel: Channel to use, see HWT_CHANNEL definitions
 * \param  com: compare output mode for given channel
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 */
static inline int8_t
hwtimer_set_com (uint8_t timer, uint8_t channel, uint8_t com)
{
  HWT_CHECK_TIMER (timer);
  HWT_CHECK_CHANNEL (channel);
  if (com > HWT_COM_MASK) {
    return HWT_ERR_INVALID_COM;
  }
  HWT_SET_COM (timer, channel, com);
  return 0;
}

/**
 * \brief  Convenience function for setting compare output mode for PWM
 * \param  timer: Timer to use
 * \param  channel: Channel to use, see HWT_CHANNEL definitions
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 */
static inline int8_t
hwtimer_pwm_enable (uint8_t timer, uint8_t channel)
{
  return hwtimer_set_com (timer, channel, HWT_COM_CLEAR);
}

/**
 * \brief  Convenience function for inverse compare output mode for PWM
 * \param  timer: Timer to use
 * \param  channel: Channel to use, see HWT_CHANNEL definitions
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 */
static inline int8_t
hwtimer_pwm_inverse (uint8_t timer, uint8_t channel)
{
  return hwtimer_set_com (timer, channel, HWT_COM_SET);
}

/**
 * \brief  Convenience function for setting compare output mode to off
 * \param  timer: Timer to use
 * \param  channel: Channel to use, see HWT_CHANNEL definitions
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 */
static inline int8_t
hwtimer_pwm_disable (uint8_t timer, uint8_t channel)
{
  return hwtimer_set_com (timer, channel, HWT_COM_NORMAL);
}

/**
 * \brief Turn off the clock
 * \param timer: Timer to use
 * \return see HWT_ERR definitions for return codes, returns 0 if ok
 */
static inline int8_t
hwtimer_fin (uint8_t timer)
{
  HWT_CHECK_TIMER (timer);
  *HWT_TCCRB (timer) &= ~HWT_CLOCK_MASK;
  *HWT_TCCRB (timer) |= HWT_CLOCK_OFF; /* technically not necessary this is 0 */
  return 0;
}

#endif /* hw_timer_h */

/*
 * ex:ts=8:et:sw=2
 */

/** @} */
