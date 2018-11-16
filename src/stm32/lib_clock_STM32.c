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


/* *******************************************************************
 * includes
 * ******************************************************************/
#include <lib_clock.h>
#include <lib_convention__errno.h>

/* frame */
#include <lib_isr.h>


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

/* *******************************************************************
 * defines
 * ******************************************************************/

static inline __attribute__((always_inline)) void __DISABLE_IRQ(void)
{__asm volatile("CPSID i");}

/*enable all interrupts*/
static inline __attribute__((always_inline)) void __ENABLE_IRQ(void)
{__asm volatile("CPSIE i");}

#define JF_TIM_TIMER		  TIM4;
#define JF_MAX_TIM_VALUE      (0xFFFF)    // 16bit counters

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
typedef volatile int32_t   jiffy_t;    // Jiffy type 4 byte integer
typedef TIM_HandleTypeDef jf_timer_t;

typedef struct {
	TIM_HandleTypeDef	timer_hdl;
	jiffy_t		*value;        // Pointer to timers current value
	uint32_t    freq;          // timer's  frequency
	uint32_t    jiffies;       // jiffies max value (timer's max value)
	jiffy_t     jpus;          // Variable for the delay function
}jf_t;

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int lib_clock__jf_init (jf_t *_jf, uint32_t _jf_freq, uint32_t _jiffies);
static int lib_clock__jf_check_usec (int32_t _usec);
static jiffy_t lib_clock__jf_per_usec (jf_t *_jf);
static int lib_clock__jf_timer_setfreq (jf_t *_jf, uint32_t _jf_freq, uint32_t _jiffies);
static void lib_clock__jf_timer_event(IRQn_Type _isr_vector, unsigned int _vector, void *_arg);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static jf_t s_jf;
static lib_isr_hdl_t s_jf_isr;
static unsigned int s_milliseconds_ticks;
static uint64_t s_milliseconds_ticks_64bits;

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
	int ret;
	s_milliseconds_ticks_64bits = 0;
	s_milliseconds_ticks = 0;

	ret = lib_isr__attach(&s_jf_isr,TIM4_IRQn,&lib_clock__jf_timer_event, NULL);
	if (ret < EOK) {
		return -ESTD_FAULT;
	}

	lib_clock__jf_init(&s_jf,1000000, 1000);  // 1MHz timer, 1000 counts, 1 usec per count
	return EOK;
}

/* ************************************************************************//**
 * \brief	get current timestamp in milliseconds
 *
 * This function returns a monotonic timestamp 
 * The returned value overflows after approximately 49.7 days.
 *
 * \return	current timestamp in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_ms(void)
{
	return s_milliseconds_ticks;
}

/* ************************************************************************//**
 * \brief	get current timestamp in nanoseconds
 *
 * \return	current timestamp in nanoseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_ns(void)
{
	uint64_t ns;

	__DISABLE_IRQ();
	ns = s_milliseconds_ticks_64bits * 1000000ULL;
	if (s_jf.value) {
		ns += *((uint64_t*)s_jf.value) * 1000ULL;
	}
	__ENABLE_IRQ();

	return ns;
}

/* ************************************************************************//**
 * \brief	get current timestamp in microseconds
 *
 * \return	current timestamp in microseconds
 * ****************************************************************************/
uint64_t lib_clock__get_time_us(void)
{
	uint64_t us;

	__DISABLE_IRQ();
	us = s_milliseconds_ticks_64bits * 1000ULL;
	if (s_jf.value) {
		us += *s_jf.value;
	}
	__ENABLE_IRQ();
	return us;

}

/* ************************************************************************//**
 * \brief	get relative time difference since the given timestamp in milliseconds
 *
 * \param	_lasttime			timestamp to calculate the difference from
 * \return	relative time difference in milliseconds
 * ****************************************************************************/
uint32_t lib_clock__get_time_since_ms(uint32_t _lasttime)
{
	return (s_milliseconds_ticks - _lasttime);
}

/* ************************************************************************//**
 * \brief delay the calling thread for the given amount of microseconds
 *
 *  \param _delay : number of microseconds to delay the caller
 * ****************************************************************************/
void lib_clock__delay_us(uint32_t _delay)
{
	jiffy_t m, m2, m1 = *s_jf.value;

	_delay *= s_jf.jpus;
	if (*s_jf.value - m1 > _delay) // Very small delays will return here.
		return;

	// Delay loop: Eat the time difference from usec value.
	while (_delay > 0) {
		m2 = *s_jf.value;
	    m = m2 - m1;
	    _delay -= (m>0) ? m : s_jf.jiffies + m;
	    m1 = m2;
	}
}

/* ************************************************************************//**
 * \brief	get clock ticks since system startup
 *
 * \return	number of clock ticks as 64bit value
 * ****************************************************************************/
uint64_t lib_clock__get_clock_ticks(void)
{

}


// Initialise the jf to a desired jiffy frequency f
static int lib_clock__jf_init (jf_t *_jf, uint32_t _jf_freq, uint32_t _jiffies)
{
	int ret;
	ret = lib_clock__jf_timer_setfreq (_jf,_jf_freq, _jiffies);
    if (ret < EOK) {
    	return ret;
    }
    _jf->jiffies = _jiffies;
    _jf->freq = _jf_freq;
    _jf->jpus = lib_clock__jf_per_usec (_jf);
   return EOK;
}


/*!
 * \brief
 *    A code based polling version delay implementation, using jiffies for timing.
 *    This is NOT accurate but it ensures that the time passed is always
 *    more than the requested value.
 *    The delay values are multiplications of 1 usec.
 * \param
 *    usec     Time in usec for delay
 */
 static int lib_clock__jf_check_usec (int32_t _usec)
 {
   static jiffy_t m1=-1, cnt;
   jiffy_t m, m2;

   if (m1 == -1) {
      m1 = *s_jf.value;
      cnt = s_jf.jpus * _usec;
   }

   if (cnt>0) {
      m2 = *s_jf.value;
      m = m2-m1;
      cnt-= (m>0) ? m : s_jf.jiffies + m;
      m1 = m2;
      return 1;   // wait
   }
   else {
      m1 = -1;
      return 0;   // do not wait any more
   }
}

 // Return the systems best approximation for jiffies per usec
 static jiffy_t lib_clock__jf_per_usec (jf_t *_jf)
 {
    jiffy_t jf = _jf->freq / 1000000;

    if (jf <= _jf->jiffies)
       return jf;
    else
       // We can not count beyond timer's reload
       return 0;
 }

/*
 * Time base configuration using the TIM7
 * \param jf_freq  The TIMER's frequency
 * \param jiffies  The TIMER's max count value
 */
static int lib_clock__jf_timer_setfreq (jf_t *_jf, uint32_t _jf_freq, uint32_t _jiffies)
{
	TIM_Base_InitTypeDef init_arg;	// universal temporary init structure for timer configuration
	int Ftim_Hz;

	/* TIM4 clock enable */
	__HAL_RCC_TIM4_CLK_ENABLE();

		// clock tree: SYSCLK --AHBprescaler--> HCLK --APB1prescaler--> PCLK1 --TIM6multiplier--> to TIM 2,3,4,6,7
		// clock tim6: Input=PCLK1 (APB1 clock) (multiplied x2 in case of APB1 clock divider > 1 !!! (RCC_CFGR.PRE1[10:8].msb[10] = 1))
	if (RCC->CFGR & RCC_CFGR_PPRE1_2)
		Ftim_Hz = HAL_RCC_GetPCLK1Freq() * 2;
	else
		Ftim_Hz = HAL_RCC_GetPCLK2Freq();

	/* setup Timer 4 for counting mode */
	/* Time Base configuration */
	init_arg.Prescaler 			= Ftim_Hz /_jf_freq ;										// Specifies the prescaler value used to divide the TIM clock. (0 = div by 1) This parameter can be a number between 0x0000 and 0xFFFF
	init_arg.CounterMode 		= TIM_COUNTERMODE_UP;
	init_arg.Period 			= _jiffies & JF_MAX_TIM_VALUE;						// Auto reload register (upcounting mode => reset cnt when value is hit, and throw an overflow interrupt)
	init_arg.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;		// not available for TIM6 and 7 => will be ignored
	init_arg.RepetitionCounter 	= 0;							// start with 0 again after overflow

	_jf->timer_hdl.Init = init_arg;
	_jf->timer_hdl.Instance = JF_TIM_TIMER;

	HAL_TIM_Base_Init(&_jf->timer_hdl);
	_jf->value = (jiffy_t*) &_jf->timer_hdl.Instance->CNT;

	__HAL_TIM_CLEAR_FLAG(&_jf->timer_hdl, TIM_IT_UPDATE | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4);
	__HAL_TIM_ENABLE(&_jf->timer_hdl);
	__HAL_TIM_ENABLE_IT(&_jf->timer_hdl, TIM_IT_UPDATE);


	return EOK;
}

static void lib_clock__jf_timer_event(IRQn_Type _isr_vector, unsigned int _vector, void *_arg)
{
	unsigned int tick_time = 0;

	if (s_jf.freq) {
		tick_time = (s_jf.jiffies * 1000.0)/s_jf.freq;
	}
	s_milliseconds_ticks += tick_time;
	s_milliseconds_ticks_64bits += tick_time;
	 __HAL_TIM_CLEAR_FLAG(&s_jf.timer_hdl, TIM_IT_UPDATE);

}


