/* ****************************************************************************************************
 * lib_clock.c within the following project: lib_clock
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	30.11.2017 		Tom			- creation of lib_clock
 *
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
 * This function initialize the time module component. An init is only necessary
 * for the EOS module, because the Fixed Interval Timer (FIT) will be adjusted
 *
 * \return	error code (currently always EOK)
 * ****************************************************************************/
int lib_clock__init(void);

/* ************************************************************************//**
 * \brief	get current timestamp in milliseconds
 *
 * This function returns a monotonic timestamp with no relation to the local
 * time and date. The returned value overflows after approximately 49.7 days.
 * The returned value may hence very well be 0. Yet, if every call returns 0,
 * the implementation is likely to be improperly supported by the underlying OS.
 *
 * \return	current timestamp in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_ms(void);

/* ************************************************************************//**
 * \brief	get current timestamp in nanoseconds
 *
 * This function returns a monotonic timestamp with no relation to the local
 * time and date. The returned value overflows after approximately 585 years.
 * Note that the underlying timer's resolution is OS dependent and is usually
 * in the dimension of nanoseconds.
 *
 * \return	current timestamp in nanoseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_ns(void);

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
uint64_t lib_clock__get_time_us(void);

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
uint32_t lib_clock__get_time_since_ms(uint32_t _lasttime);

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
void lib_clock__delay_us(uint32_t _delay);

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
uint64_t lib_clock__get_clock_ticks(void);


#ifdef __cplusplus
}
#endif

#endif
