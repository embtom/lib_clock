/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2020 Thomas Willetal 
 * (https://github.com/embtom)
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

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */

/* system */
#include <time.h>

/* own libs */
#include <lib_convention__errno.h>

/* project */
#include "lib_clock.h"

/* *******************************************************************
 * function definitions
 * ******************************************************************/

/* ************************************************************************//**
 * \brief	Initialization of the timing module

 * ****************************************************************************/
int lib_clock__init(void)
{
	return EOK;
}

/* ************************************************************************//**
 * \brief	get current timestamp in milliseconds
 *
 * \return	current timestamp in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_ms(void)
{
	struct timespec tp;


	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){

		return 0;
	}


	// convert returned timestamp structure to milliseconds
	return (uint32_t)(tp.tv_nsec / 1000000 + tp.tv_sec * 1000);
}

/* ************************************************************************//**
 * \brief	get current timestamp in nanoseconds
 *
 *
 * \return	current timestamp in nanoseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_ns(void) {
	struct timespec tp;

	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){

		return 0;
	}

	// convert returned timestamp structure to nanoseconds
	return (uint64_t)(tp.tv_nsec + tp.tv_sec * 1000000000ULL);	
}

/* ************************************************************************//**
 * \brief	get current timestamp in microseconds
 *
 *
 * \return	current timestamp in microseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_us(void){
	struct timespec tp;

	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){

		return 0;
	}

	// convert returned timestamp structure to microseconds
	return (uint64_t)(tp.tv_nsec / 1000ULL + tp.tv_sec * 1000000ULL);	
}

/* ************************************************************************//**
 * \brief	get relative time difference since the given timestamp in milliseconds
 *
 * ****************************************************************************/
uint32_t lib_clock__get_time_since_ms(uint32_t _lasttime){

	return lib_clock__get_time_ms() - _lasttime;
}

/* ************************************************************************//**
 * \brief delay the calling thread for the given amount of microseconds
 *
 * ****************************************************************************/
void lib_clock__delay_us(uint32_t _delay){
	struct timespec rqtp;


	// calculate timespec values
	rqtp.tv_sec = (time_t)_delay / 1000000;
	rqtp.tv_nsec = (long)(_delay % 1000000) * 1000;


	clock_nanosleep(CLOCK_MONOTONIC, 0, &rqtp, NULL);
}

/* ************************************************************************//**
 * \brief	number of clock ticks as 64bit value
 * ****************************************************************************/
uint64_t lib_clock__get_clock_ticks(void){
	static uint64_t tick_res = 0;
	struct timespec tp;


	// get current absolute system time
	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){

		return 0;
	}

	// get resolution (only if called for the very first time, as the resolution should never change)
	if (tick_res == 0){
		struct timespec res;

		if (clock_getres(CLOCK_MONOTONIC, &res) != 0){

			return 0;
		}

		tick_res  = ((uint64_t)res.tv_nsec + (uint64_t)res.tv_sec * 1000000000);
		if (tick_res == 0){
			// just in case, to avoid division by 0; should actually never happen, though
			return 0;
		}
	}


	// calculate tick count by dividing the overall number of elapsed nanoseconds by the timer resolution
	return ((uint64_t)tp.tv_nsec + (uint64_t)tp.tv_sec * 1000000000) / tick_res;
}

