/**
  ******************************************************************************
  * \file    abstractaudiodriver.hpp
  * \author  Vivien Henry
  * \version V1.0
  * \date    06/11/2014
  *


  ******************************************************************************
  * \attention
  *
  ******************************************************************************
  */

#ifndef STM32F4AUDIDRIVER_HPP_
#define STM32F4AUDIDRIVER_HPP_

#include "const.hpp"

#include "stm32f4xx_hal.h"

#include "abstractaudiodriver.hpp"

#include "sigmadeltamodulator.hpp"


//here, just some defines cause i don't care about what the user wants, for now at least
#define STM32F4AD_SR SR192000
#define STM32F4AD_FPB 512
#define STM32F4AD_HALFFPB 256

#define SDM_OSR 64

#define TIM2CH1_DMAFLAG 0x01
#define TIM2CH2_DMAFLAG 0x02
#define TIM3CH1_DMAFLAG 0x04
#define TIM3CH4_DMAFLAG 0x08





/**
	\brief AbstractAudioDriver Implementation for STM32
 */
class STM32F4AudioDriver : public AbstractAudioDriver{
	public:
		/**
			\brief configure the peripherals, regarding to the application
			*/
	  STM32F4AudioDriver(SampleRate sr, uint32_t fpb);
    virtual ~STM32F4AudioDriver();

		/**
		\brief Start the configured peripherals
			configure I2S, add buffers, etc
			It is compulsory to add the I2S and other peripherals with their config params before calling this method
			*/
	  virtual uint8_t configure();
	
		/**
	  \brief Terminate the driver : free memory, etc
		*/
    virtual uint8_t terminate();

	
		/**
			\brief Start Streams
			*/
	  virtual uint8_t start();
		
		/**
			\brief Stop Streaming
			*/
    virtual uint8_t stop();
		
		virtual void process();
		
		inline uint32_t getDMAAck()  { return mDMAAcks;}
		
		/**
			\brief Acknowledge one or more local drivers interuptions
			*/
		inline void acknDMAFlag(uint32_t flag) { mDMAAcks &= (~flag); }
		
	public:
		inline Sample* getBufferSDM1(){ return mSDM1Buffer; }
		inline Sample* getBufferSDM2(){ return mSDM2Buffer; }

	protected:
		/**
			\brief A low level initialisation for the driver (GPIO, DMA)
			*/
		void mspInit();
	
		/**
			\brief A low level de initialisation
			*/
		void mspDeInit();

	protected:
		/**
			* Effectives buffers
			* The Pointers in The abstracted class will point on theses ones.
			* The "*2" is because the drivers will used the buffers in Circular mode : 
			* the 2 half parts of the buffers works in parallel : one is filled, the other one is used
			*/
		Sample mI2S1InBuffer[STM32F4AD_FPB*2];
		Sample mI2S2InBuffer[STM32F4AD_FPB*2];
		Sample mI2S5InBuffer[STM32F4AD_FPB*2];
	
		Sample mSDM1Buffer[STM32F4AD_FPB*2];
		Sample mSDM2Buffer[STM32F4AD_FPB*2];

		/**
			* Implemetation
			*/
	
		// ------------------ General -------------------
		uint32_t mDMAAcks;
	
		// ---------------------- I2S --------------------
		//ST STructure
		I2S_HandleTypeDef hi2s1;
		I2S_HandleTypeDef hi2s2;
		I2S_HandleTypeDef hi2s5;
		
		
		// ------------------ PWM and SDM Part -----------
		/**
			* Sigma Delta Modulator and Their buffers
			*/
		SigmaDeltaModulator *mSdm1, *mSdm2;
	public: //just for testing !!
		SDMOutputType mPWMBuffer21[STM32F4AD_FPB*SDM_OSR];
		SDMOutputType mPWMBuffer22[STM32F4AD_FPB*SDM_OSR];
		SDMOutputType mPWMBuffer31[STM32F4AD_FPB*SDM_OSR];
		SDMOutputType mPWMBuffer34[STM32F4AD_FPB*SDM_OSR];


	protected:
		//ST Structures
		TIM_HandleTypeDef htim2;
		TIM_HandleTypeDef htim3;
		
	  TIM_MasterConfigTypeDef sMasterConfig2;
		TIM_OC_InitTypeDef sConfigOC2;
	
		/*TIM_HandleTypeDef htim3;*/
	  TIM_MasterConfigTypeDef sMasterConfig3;
		TIM_OC_InitTypeDef sConfigOC3;
};


/**
  }
  */

#endif /* AUDIOCONF_HPP_ */
