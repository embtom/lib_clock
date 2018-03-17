/* ****************************************************************************************************
 * lib_clock.c within the following project: bld_device_cmake_Nucleo_STM32F401
 *	
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		thomas
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	06.03.2018			thomas			- creation of lib_clock.c
 *  
 */

/* https://stackoverflow.com/questions/29010847/how-to-make-a-microsecond-precise-timer-on-the-stm32l-discovery-arm-board*/

/* *******************************************************************
 * includes
 * ******************************************************************/
#include <lib_clock.h>
#include <lib_convention__errno.h>

#ifdef CORTEX_M3
	#include <stm32f1xx.h>
	#include <stm32f1xx_hal_rcc.h>		// RCC_* functions
	#include <stm32f1xx_hal_dma.h>		// Recursively required from tm32f1xx_hal_tim.h
	#include <stm32f1xx_hal_tim.h>		// TIM_* functions
#elif CORTEX_M4
	#include <stm32f4xx.h>
	#include <stm32f4xx_hal_rcc.h>		// RCC_* functions
	#include <stm32f4xx_hal_dma.h>		// Recursively required from tm32f1xx_hal_tim.h
	#include <stm32f4xx_hal_tim.h>		// TIM_* functions
#else
	#error No Architecture is set at lib_clock
#endif


static TIM_HandleTypeDef s_lib_delay__tim_hdl;
uint16_t Ftim_MHz;		// timer speed in MHz

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
	TIM_Base_InitTypeDef init_arg;	// universal temporary init structure for timer configuration
	int Ftim6_Hz;

	/* configure hardware timer 6 */

	/* TIM4 clock enable */
	__HAL_RCC_TIM4_CLK_ENABLE();

	/* setup Timer 4 for counting mode */
	/* Time Base configuration */
	init_arg.Prescaler 			= 65535;										// Specifies the prescaler value used to divide the TIM clock. (0 = div by 1) This parameter can be a number between 0x0000 and 0xFFFF
	init_arg.CounterMode 		= TIM_COUNTERMODE_UP;
	init_arg.Period 			= 4000;						// Auto reload register (upcounting mode => reset cnt when value is hit, and throw an overflow interrupt)
	init_arg.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;		// not available for TIM6 and 7 => will be ignored
	init_arg.RepetitionCounter 	= 0;							// start with 0 again after overflow

	s_lib_delay__tim_hdl.Init = init_arg;
	s_lib_delay__tim_hdl.Instance = TIM4;

	HAL_TIM_Base_Init(&s_lib_delay__tim_hdl);

	/* TIM6 counter enable (free running) */
	__HAL_TIM_ENABLE(&s_lib_delay__tim_hdl);
	__HAL_TIM_ENABLE_IT(&s_lib_delay__tim_hdl, TIM_IT_UPDATE);

	// TIM4->DIER = TIM_DIER_UIE;

	// clock tree: SYSCLK --AHBprescaler--> HCLK --APB1prescaler--> PCLK1 --TIM6multiplier--> to TIM 2,3,4,6,7
	// clock tim6: Input=PCLK1 (APB1 clock) (multiplied x2 in case of APB1 clock divider > 1 !!! (RCC_CFGR.PRE1[10:8].msb[10] = 1))
	if (RCC->CFGR & RCC_CFGR_PPRE1_2)
		Ftim6_Hz = HAL_RCC_GetPCLK1Freq() * 2;
	else
		Ftim6_Hz = HAL_RCC_GetPCLK2Freq();

	// Timer internal prescaler
	Ftim6_Hz /= ((TIM4->PSC) + 1);

	// set info global available
	Ftim_MHz = Ftim6_Hz / 1000000;


	NVIC_EnableIRQ(TIM4_IRQn); // Enable TIM16 IRQ


	return EOK;
}

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
uint32_t lib_clock__get_time_ms(void)
{
	return HAL_GetTick();
}

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
uint64_t lib_clock__get_time_ns(void)
{

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
uint64_t lib_clock__get_time_us(void)
{

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
uint32_t lib_clock__get_time_since_ms(uint32_t _lasttime)
{

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
void lib_clock__delay_us(uint32_t _delay)
{

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
uint64_t lib_clock__get_clock_ticks(void)
{

}


void TIM4_IRQHandler (void)
{
	static unsigned int tim4_count = 0;

	 __HAL_TIM_CLEAR_FLAG(&s_lib_delay__tim_hdl, TIM_IT_UPDATE);

//	TIM4->SR = 0; //~(TIM_IT_UPDATE);
	tim4_count ++;

}

