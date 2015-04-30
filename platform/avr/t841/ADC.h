/*
 * ADC.h
 *
 * Created: 08/02/2015 21:51:28
 *  Author: Vivien
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "GPIO.h"


/*
 * Configuration defines
 */
//ADMUXA : Analog Channel Mux
#define ADC_MUXA_CH0 0x00
#define ADC_MUXA_CH1 0x01
#define ADC_MUXA_CH2 0x02
#define ADC_MUXA_CH3 0x03
#define ADC_MUXA_CH4 0x04
#define ADC_MUXA_CH5 0x05
#define ADC_MUXA_CH6 0x06
#define ADC_MUXA_CH7 0x07
#define ADC_MUXA_CH8 0x08
#define ADC_MUXA_CH9 0x09
#define ADC_MUXA_CH10 0x0A
#define ADC_MUXA_CH11 0x0B
	//don't care about the others now..
	
//ADMUXB : References selection 
#define ADC_MUXB_REF_VCC		0x00
#define ADC_MUXB_REF_1_1		0x20
#define ADC_MUXB_REF_2_2		0x40
#define ADC_MUXB_REF_4_096		0x60
#define ADC_MUXB_REF_AREF		0x80
#define ADC_MUXB_REF_1_1_CAP	0xA0
#define ADC_MUXB_REF_2_2_CAP	0xC0
#define ADC_MUXB_REF_4_096_CAP	0xE0

#define ADC_MUXB_GAIN_1			0x00
#define ADC_MUXB_GAIN_20		0x01
#define ADC_MUXB_GAIN_100		0x02

//ADCSRA
#define ADC_ENABLE				(1<<ADEN)
#define ADC_START_CONV			(1<<ADSC)
#define ADC_AUTO_TRIG_EN		(1<<ADATE)
#define ADC_AUTO_TRIG_DIS		0
#define ADC_INTERRUPT_FLAG		(1<<ADIF)
#define ADC_INTERRUPT_EN		(1<<ADIE)
//#define ADC_INTERRUPT_DIS		(0<<ADIE)
#define ADC_PRESCALER_DIV2			0x00
#define ADC_PRESCALER_DIV4			0x02
#define ADC_PRESCALER_DIV8			0x03
#define ADC_PRESCALER_DIV16			0x04
#define ADC_PRESCALER_DIV32			0x05
#define ADC_PRESCALER_DIV64			0x06
#define ADC_PRESCALER_DIV128		0x07

//ADCSRB
#define ADC_LEFT_ADJUST_RESULT		(1<<ADLAR)
#define ADC_RIGHT_ADJUST_RESULT		(0<<ADLAR)

#define ADC_ATS_FREE_RUNNING_MODE	0x00
#define ADC_ATS_ANALOG_COMPARATOR	0x01
#define ADC_ATS_EXINT0				0x02
#define ADC_ATS_TIM0_COMPAREMATCH_A	0x03
#define ADC_ATS_TIM0_OVERFLOW		0x04
#define ADC_ATS_TIM1_COMPAREMATCH_A	0x05
#define ADC_ATS_TIM1_OVERFLOW		0x06
#define ADC_ATS_TIM1_CAPTURE_EVENT	0x07

//DIDR0
#define ADC_DID_0	0x01
#define ADC_DID_1	0x02
#define ADC_DID_2	0x04
#define ADC_DID_3	0x08
#define ADC_DID_4	0x10
#define ADC_DID_5	0x20
#define ADC_DID_6	0x40
#define ADC_DID_7	0x80

//DIDR1
#define ADC_DID_8	0x04
#define ADC_DID_9	0x08
#define ADC_DID_10	0x02
#define ADC_DID_11	0x01

//define for the DID (digital input disable)
#define ADC_DI_DIS 1
#define ADC_DI_EN  0

/**
 * Interruption
 */
ISR(ADC_vect);


/**
 * \brief Handle the Adc peripheral
 * 
 */
class Adc{
	public:
		/**
		 * \brief Constructor of Adc
		 * \param ref select the voltage reference for the ADC 
		 * \param prescaler select the clock prescaler divider
		 * \param intEn interrupt enable
		 * \param autoTrigEn : enable/disable the auto trigger function
		 * \param autoTrigSource : select the auto trigger source
		 * \param cb : if wanted, a callback can be called at every interruption
		 */
		Adc(uint8_t ref, uint8_t prescaler, uint8_t autoTrigEn, uint8_t autoTrigSource, void (*cb)(uint16_t));
		~Adc();
		
		/**
		 * \brief Configure the low level of the peripheral : GPIO, etc
		 */
		void lowLevelInit();
		
		/**
		 * \brief Configure the peripheral regarding parameters given
		 * \param ref select the voltage reference for the ADC 
		 * \param prescaler select the clock prescaler divider
		 * \param intEn interrupt enable
		 * \param autoTrigEn : enable/disable the auto trigger function
		 * \param autoTrigSource : select the auto trigger source
		 * \param cb : if wanted, a callback can be called at every interruption
		 */
		void configure(uint8_t ref, uint8_t prescaler, uint8_t autoTrigEn, uint8_t autoTrigSource);
		
		/**
		 * \brief Select which analog in is connected to the ADC
		 * \param channel : select the mux channel
		 * \param did : digital input disabled ADC_DI_DIS/ADC_DI_EN
		 */
		void setMuxChannel(uint8_t channel, uint8_t did);
		
		
		
		/**
		 * \brief Start a conversion in blocking mode (start a conversion, wait for the end, and return the value)
		 */
		uint16_t convert();
		
		/**
		 * \brief Start a conversion in interruption mode. (if auto trigger mode enable or not)
		 */
		void startConversion();
		
		/**
		 * \brief Stop a conversion in interruption mode. (if auto trigger mode enable or not)
		 */
		void stopConversion();
		
		/** 
		 * \brief The general callback, used to updated value.
		 * Used with the auto trigger source.
		 * If a callback is set (see configure) then the callback given is called at the end of this function.
		 */
		void callback();
		
		
		
		/**
		 * \brief return the last value converted, saved locally
		 * used if interruption is enable
		 */
		inline uint16_t getValue() { return mValue; }
		
	private:
		uint16_t mValue;
		
		void (*mCallback)(uint16_t);
};



#endif /* ADC_H_ */