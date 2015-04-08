/*
 * Timer.h
 *
 * Created: 08/02/2015 21:51:47
 *  Author: Vivien
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "core_conf.h"

/*
 * Configuration defines
 */
// --- Global ---

//TIMER/Counter Output Compare Pin Mux Selection Registers
#define TIM_TOCC_OC0	0x00
#define TIM_TOCC_OC1	0x01
#define TIM_TOCC_OC2	0x02

//TIMER/Counter Output Compare Pin Mux Channel Output Enable
#define TIM_OC_CH_OUT_7_EN 0x80
#define TIM_OC_CH_OUT_6_EN 0x40
#define TIM_OC_CH_OUT_5_EN 0x20
#define TIM_OC_CH_OUT_4_EN 0x10
#define TIM_OC_CH_OUT_3_EN 0x08
#define TIM_OC_CH_OUT_2_EN 0x04
#define TIM_OC_CH_OUT_1_EN 0x02
#define TIM_OC_CH_OUT_0_EN 0x01

// --- TIMER0 ---

//TIMER/Counter Control Reg A
#define TIM0_OC_A_DISCONNECTED	0x00
#define TIM0_OC_A_TOGGLE		0x40
#define TIM0_OC_A_CLEAR			0x80
#define TIM0_OC_A_SET			0xC0

#define TIM0_OC_B_DISCONNECTED	0x00
#define TIM0_OC_B_TOGGLE		0x10
#define TIM0_OC_B_CLEAR			0x20
#define TIM0_OC_B_SET			0x30

#define TIM0_WGM_LOW_NORMAL		0x00
#define TIM0_WGM_LOW_PWM_PC		0x01
#define	TIM0_WGM_LOW_CTC		0x02
#define TIM0_WGM_LOW_FASTPWM	0x03

//TIMER/Counter Control Reg B
#define TIM0_FORCE_OUTPUT_COMPARE_A	(1<<FOC0A)
#define TIM0_FORCE_OUTPUT_COMPARE_B	(1<<FOC0B)

#define TIM0_WGM_HIGH_PWM_MAXTOP	0x00
#define TIM0_WGM_HIGH_PWM_VARTOP	0x08

#define TIM0_CLOCKSEL_MASK			0x07

#define TIM0_CLOCKSEL_NOCLOCK		0x00
#define TIM0_CLOCKSEL_CLK			0x01
#define TIM0_CLOCKSEL_CLKDIV8		0x02
#define TIM0_CLOCKSEL_CLKDIV64		0x03
#define TIM0_CLOCKSEL_CLKDIV256		0x04
#define TIM0_CLOCKSEL_CLKDIV1024	0x05
#define TIM0_CLOCKSEL_EXT_FALLING	0x06
#define TIM0_CLOCKSEL_EXT_RISING	0x07

//TIMER/Counter Interrupt Mask Register
#define TIM0_OC_MATCH_B_INTERRUPT_EN	(1<<OCIE0B)
#define TIM0_OC_MATCH_A_INTERRUPT_EN	(1<<OCIE0A)
#define TIM0_OVERFLOW_INTERRUPT_EN		(1<<TOIE0)

//TIMER/Counter Flag Register
#define TIM0_OC_FLAG_B		(1<<OCF0B)
#define TIM0_OC_FLAG_A		(1<<OCF0A)
#define TIM0_OVERFLOW_FLAG	(1<<TOV0)




//TIMER Misceallenous
#define TIM_CB_OCA 0
#define TIM_CB_OCB 1
#define TIM_CB_OVF 2



/*
 * Interruption
 */
ISR(TIMER0_COMPA_vect);
ISR(TIMER0_COMPB_vect);
ISR(TIMER0_OVF_vect);

/**
 * \brief Handle Timer peripheral
 */
class Timer8b{
	public:
		/**
		 * \brief A general constructor for the Timer
		 */
		Timer8b(uint8_t waveformGenerationMode, 
		uint8_t compareMatchOutputsMode, 
		uint8_t clockSelect, 
		uint8_t interruptsEn, 
		uint8_t outputCompareRegA, 
		uint8_t outputCompareRegB,
		
		void (*callbackOutpuCompareA)(void),
		void (*callbackOutpuCompareB)(void),
		void (*callbackOverflow)(void));
		
		/**
		 * \brief This constructor starts the peripheral for a "delay" purpose.
		 */
		Timer8b(uint8_t clkDiv, uint8_t timResolution);
		
		~Timer8b();
		
		/**
		 * \brief Perform the low level initialization of the peripheral
		 */
		void lowLevelInit();
		
		/**
		 * \brief Perform the configuration of the peripheral
		 */
		void configure(uint8_t waveformGenerationMode, uint8_t compareMatchOutputsMode, uint8_t interruptsEn, uint8_t outputCompareRegA, uint8_t outputCompareRegB);
			
		//void configureOutput();
		
		/**
		 * \brief set the Counter value (the maximum value of the counter
		 */
		inline void setCounterValue(uint8_t counter) { TCNT0 = counter; }
		
		/**
		 * \brief set the output compare A register value
		 */
		inline void setOCValueA(uint8_t value) { OCR0A = value; }
		
		/**
		 * \brief set the output compare B register value
		 */
		inline void setOCValueB(uint8_t value) { OCR0B = value; }
		
		/**
		 * \brief Start the counter
		 */
		void start();
		
		/**
		 * \brief Stop the counter
		 */
		void stop();
		
		/**
		 * \brief This function will start the timer for an active delay.
		 * It starts the timer, and a callback counts the number of tick asked, regarding to the previous conf
		 */
		void wait(uint8_t ticks);
		
		/**
		 * \brief Callback method
		 */
		void callback(uint8_t callbackType);
		
		
	private:
		uint8_t mClockSelect;
		
		void (*mCallbackOutpuCompareA)(void);
		void (*mCallbackOutpuCompareB)(void);
		void (*mCallbackOverflow)(void);
};


#endif /* TIMER_H_ */
