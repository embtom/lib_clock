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
