/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal 
 * (https://github.com/tom3333)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _LIB_CLOCK_H_
#define _LIB_CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif


/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>

/* *******************************************************************
 * Function Prototypes
 * ******************************************************************/

/* ************************************************************************//**
 * \brief	Initialization of the timing module
 *
 * This function initialize the time module component.
 *
 * \return	error code (currently always EOK)
 * ****************************************************************************/
int lib_clock__init(void);

/* ************************************************************************//**
 * \brief	get current timestamp in milliseconds
 *
 * This function returns a monotonic timestamp.
 * The returned value overflows after approximately 49.7 days.
 *
 * \return	current timestamp in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_ms(void);

/* ************************************************************************//**
 * \brief	get current timestamp in nanoseconds
 *
 * This function returns a monotonic timestamp 
 * 
 * \return	current timestamp in nanoseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_ns(void);

/* ************************************************************************//**
 * \brief	get current timestamp in microseconds
 *

 * \return	current timestamp in microseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_us(void);

/* ************************************************************************//**
 * \brief	get relative time difference since the given timestamp in milliseconds
 *
 * \param	_lasttime			timestamp to calculate the difference from
 * \return	relative time difference in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_since_ms(uint32_t _lasttime);

/* ************************************************************************//**
 * \brief blocking delay 
 *
 *  \param _delay : number of microseconds to delay the caller
 * ****************************************************************************/
void lib_clock__delay_us(uint32_t _delay);

/* ************************************************************************//**
 * \brief	get clock ticks since system startup
 *
 * \return	number of clock ticks as 64bit value
 * ****************************************************************************/
uint64_t lib_clock__get_clock_ticks(void);

#ifdef __cplusplus
}
#endif

#endif
