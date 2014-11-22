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



#define STM32F4AD_SR SR192000
#define STM32F4AD_FPB 128

#define SDM_OSR 64

/**
	* Globals Variables
	*/
	
// --- I2S ---
I2S_HandleTypeDef hi2s1;
I2S_HandleTypeDef hi2s2;
I2S_HandleTypeDef hi2s5;

DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi5_rx;

// --- PWM and SDM Part ----
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

DMA_HandleTypeDef hdma_tim2_ch1;
DMA_HandleTypeDef hdma_tim2_ch2_ch4;
DMA_HandleTypeDef hdma_tim3_ch1_trig;
DMA_HandleTypeDef hdma_tim3_ch4_up;	



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
	
		Sample mSDM1Buffer[STM32F4AD_FPB*2*SDM_OSR];
		Sample mSDM2Buffer[STM32F4AD_FPB*2*SDM_OSR];
	

		/**
			* peripherals structurs from ST
			*/
		// --- PWM and SDM Part ----
	  TIM_MasterConfigTypeDef sMasterConfig2;
		TIM_OC_InitTypeDef sConfigOC2;
	
		/*TIM_HandleTypeDef htim3;*/
	  TIM_MasterConfigTypeDef sMasterConfig3;
		TIM_OC_InitTypeDef sConfigOC3;
	
		/**
			* Link to the SDMs
			*/
		SigmaDeltaModulator *mSdm1, *mSdm2;
};


/**
  }
  */

#endif /* AUDIOCONF_HPP_ */
