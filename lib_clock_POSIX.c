/***************************************************************************//**
 *	\file	lib_module_time.c
 *	\brief	implementation of timer functions based on POSIX timer functions
 *
 * ============================================================================
 *	Disclaimer:
 *	COPYRIGHT (c) MAN Diesel & Turbo SE, All Rights Reserved
 *	(Augsburg, Germany)
 *
 * ============================================================================
 *
 *	Project:	MAN | SaCoS 5000
 *	Compiler:	GNU GCC
 *	Target:		PowerPC / ARM /x86 with POSIX-compliant OS
 *
 ******************************************************************************/
/*
 *	04.12.2014	MK		creation
 *  29.10.2015  TW		Add the function "lib_module_time__get_time_us"
 *
 *	---
 *	AB: Albert Böswald, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	AH: Alexander Holzmann, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	AL: Andreas Lehner, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	BCL: Bianca-Charlotte Liehr, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	BL: Bernd Lindenmayr, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	BS: Benedikt Schmied, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	MK: Markus Kohler, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	MR: Martin Reichherzer, MAN Diesel & Turbo SE, www.mandieselturbo.com
 *	TW: Thomas Willetal, MAN Diesel & Turbo SE, www.mandieselturbo.com
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
 *
 * This function initialize the time module component. An init is only necessary
 * for the EOS module, because the Fixed Interval Timer (FIT) will be adjusted
 *
 * \return	error code (currently always EOK)
 * ****************************************************************************/
int lib_clock__init(void)
{
	return EOK;
}

/* ************************************************************************//**
 * \brief	get current timestamp in milliseconds
 *
 * This function returns a monotonic timestamp with no relation to the local
 * time and date. The returned value overflows after approximately 49.7 days.
 * The returned value may very well be 0. Yet, if every call returns 0, the
 * implementation is likely to be not properly supported by the underlying OS.
 *
 * \return	current timestamp in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_ms(void)
{
	struct timespec tp;


	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){
		/*
		 * According to POSIX, the following errors are defined:
		 *	EINVAL		"The clock_id argument does not specify a known clock."
		 *				This would only be the case if the implementation of clock_gettime did not support CLOCK_MONOTONIC.
		 *				Currently, this is only the case on Windows where we will use a special implementation therefore.
		 *	EOVERFLOW	"The number of seconds will not fit in an object of type time_t."
		 *				Assuming time_t to be of 32bit width, this is only the case if running for more than 136 years.
		 *
		 *	->	This case should actually never happen.
		 *		Nevertheless we cover it by returning 0 which should be noticed during SW development.
		 */
		return 0;
	}


	// convert returned timestamp structure to milliseconds
	return (uint32_t)(tp.tv_nsec / 1000000 + tp.tv_sec * 1000);
}

/* ************************************************************************//**
 * \brief	get current timestamp in nanoseconds
 *
 * This function returns a monotonic timestamp with no relation to the local
 * time and date. The returned value overflows after approximately 585 years.
  *
 * \return	current timestamp in nanoseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_ns(void) {
	struct timespec tp;

	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){
		/*
		 * According to POSIX, the following errors are defined:
		 *	EINVAL		"The clock_id argument does not specify a known clock."
		 *				This would only be the case if the implementation of clock_gettime did not support CLOCK_MONOTONIC.
		 *				Currently, this is only the case on Windows where we will use a special implementation therefore.
		 *	EOVERFLOW	"The number of seconds will not fit in an object of type time_t."
		 *				Assuming time_t to be of 32bit width, this is only the case if running for more than 136 years.
		 *
		 *	->	This case should actually never happen.
		 *		Nevertheless we cover it by returning 0 which should be noticed during SW development.
		 */
		return 0;
	}

	// convert returned timestamp structure to nanoseconds
	return (uint64_t)(tp.tv_nsec + tp.tv_sec * 1000000000ULL);	
}

/* ************************************************************************//**
 * \brief	get current timestamp in microseconds
 *
 * This function returns a monotonic timestamp with no relation to the local
 * time and date. The returned value overflows after approximately 585 years.
 * Note that the underlying timer's resolution is OS dependent and is usually
 * in the dimension of microseconds.
 *
 * \return	current timestamp in microseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_us(void){
	struct timespec tp;

	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){
		/*
		 * According to POSIX, the following errors are defined:
		 *	EINVAL		"The clock_id argument does not specify a known clock."
		 *				This would only be the case if the implementation of clock_gettime did not support CLOCK_MONOTONIC.
		 *				Currently, this is only the case on Windows where we will use a special implementation therefore.
		 *	EOVERFLOW	"The number of seconds will not fit in an object of type time_t."
		 *				Assuming time_t to be of 32bit width, this is only the case if running for more than 136 years.
		 *
		 *	->	This case should actually never happen.
		 *		Nevertheless we cover it by returning 0 which should be noticed during SW development.
		 */
		return 0;
	}

	// convert returned timestamp structure to microseconds
	return (uint64_t)(tp.tv_nsec / 1000ULL + tp.tv_sec * 1000000ULL);	
}

/* ************************************************************************//**
 * \brief	get relative time difference since the given timestamp in milliseconds
 *
 * The returned value may be 0 in the following cases:
 *	1.	The difference really is 0
 *		In that case, one should verify whether this function provides a
 *		sufficient time resolution under all possible circumstances.
 *	2.	The time difference is 49.7 days
 *		In that case, one should verify whether this function provides a
 *		sufficient value range under all possible circumstances.
 *	3.	An error has occurred (provided that _lasttime is also 0)
 *		This may actually only happen if this function is likely to be not
 *		properly supported by the underlying OS.
 *
 * WARNING:
 *	1.	If the actual time difference is greater than 49.7 days, the value of
 *		the difference will overflow, providing an incorrect result.
 *	2.	If _lasttime was provided by a different function than
 *		lib_module_time__get_time(), the result is likely to be invalid, as
 *		the utilized time bases may not be the same.
 *
 * \param	_lasttime			timestamp to calculate the difference from
 * \return	relative time difference in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_since_ms(uint32_t _lasttime){

	return lib_clock__get_time_ms() - _lasttime;
}

/* ************************************************************************//**
 * \brief delay the calling thread for the given amount of microseconds
 *
 * This function delays the calling thread for _delay microseconds.
 * On OSEK, this function implements a busy wait with an accuracy of 1us.
 * On HOSes, this function implements a thread suspension with an implementation
 * dependent accuracy between 1us and several milliseconds. Furthermore, this
 * function may get prematurely interrupted by a signal on HOSes.
 *
 *  \param _delay : number of microseconds to delay the caller
 * ****************************************************************************/
void lib_clock__delay_us(uint32_t _delay){
	struct timespec rqtp;


	// calculate timespec values
	rqtp.tv_sec = (time_t)_delay / 1000000;
	rqtp.tv_nsec = (long)(_delay % 1000000) * 1000;

	/*
	 * Now wait...
	 *
	 * According to POSIX, the following errors are defined:
	 *	EINTR		"The clock_nanosleep() function was interrupted by a signal."
	 *				If so, this function may either resume execution with the remaining amount of time to wait, or simply quit.
	 *				For now, we just let it quit.
	 *	EINVAL		"The rqtp argument specified a nanosecond value less than zero or greater than or equal to 1000 million;
	 *				This case should never apply.
	 *				"or the TIMER_ABSTIME flag was specified in flags and the rqtp argument is outside the range for the clock specified by clock_id;
	 *				This case should never apply.
	 *				"or the clock_id argument does not specify a known clock, or specifies the CPU-time clock of the calling thread."
	 *				The former case would only apply if CLOCK_MONOTONIC is not defined.
	 *				Currently, this is only the case on Windows where we will use a special implementation therefore.
	 *				The latter case should never apply.
	 *	ENOTSUP		"The clock_id argument specifies a clock for which clock_nanosleep() is not supported, such as a CPU-time clock."
	 *				The former case would only apply if the implementation of clock_nanosleep did not support CLOCK_MONOTONIC.
	 *				So far we know, this is not the case on any of our employed POSIX-compliant OSes.
	 *				The latter case should never apply.
	 */
	clock_nanosleep(CLOCK_MONOTONIC, 0, &rqtp, NULL);
}

/* ************************************************************************//**
 * \brief	get clock ticks since system startup
 *
 * The CPU time base will overrun after several dozens to several 1000 years,
 * depending on the clock resolution, e.g.:
 *	4679a @  80ns (= 125MHz system clock)
 *	  58a @ 100ps (=  10GHz system clock)
 * Therefore an overrun handling is not really necessary.
 *
 * \return	number of clock ticks as 64bit value
 * ****************************************************************************/
uint64_t lib_clock__get_clock_ticks(void){
	static uint64_t tick_res = 0;
	struct timespec tp;


	// get current absolute system time
	if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0){
		/*
		 * According to POSIX, the following errors are defined:
		 *	EINVAL		"The clock_id argument does not specify a known clock."
		 *				This would only be the case if the implementation of clock_gettime did not support CLOCK_MONOTONIC.
		 *				Currently, this is only the case on Windows where we will use a special implementation therefore.
		 *	EOVERFLOW	"The number of seconds will not fit in an object of type time_t."
		 *				Assuming time_t to be of 32bit width, this is only the case if running for more than 136 years.
		 *
		 *	->	This case should actually never happen.
		 *		Nevertheless we cover it by returning 0 which should be noticed during SW development.
		 */
		return 0;
	}

	// get resolution (only if called for the very first time, as the resolution should never change)
	if (tick_res == 0){
		struct timespec res;

		if (clock_getres(CLOCK_MONOTONIC, &res) != 0){
			/*
			 * According to POSIX, the following errors are defined:
			 *	EINVAL		"The clock_id argument does not specify a known clock."
			 *				This would only be the case if the implementation of clock_gettime did not support CLOCK_MONOTONIC.
			 *				Currently, this is only the case on Windows where we will use a special implementation therefore.
			 *
			 *	->	This case should actually never happen.
			 *		Nevertheless we cover it by returning 0 which should be noticed during SW development.
			 */
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


// ////////////////////////////////////////////////////////
// Static Functions
// ////////////////////////////////////////////////////////
