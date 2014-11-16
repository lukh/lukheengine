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

// -------------------------------- I2S -------------------------------------
#define  STM32F4I2S_MAXCONNCONF 5

enum  STM32F4_I2SInstance{
	NoI2S = 0,
	I2S1,
	I2S2,
	I2S3,
	I2S4,
	I2S5
};

class  STM32F4_I2SConnectionConfig{
	public:
		STM32F4_I2SConnectionConfig();
	
		STM32F4_I2SInstance mI2s;
		int8_t mInBufferId;
		int8_t mOutBufferId;
};

// ------------------------------ PWM + SDM -----------------------------------

enum STM32F4_TimerInstance{
	NoTimer = 0,
	Tim1,
	Tim2,
	Tim3,
	Tim4,
	Tim5
};

enum STM32F4_TimerChannel{
	TimNoChannel = 0,
	TimChannel0 = 1,
	TimChannel1 = 2,
	TimChannel2 = 3,
	TimChannel3 = 4
};

class STMF4_TIMPWMConnectionConfig{
	public:
		STMF4_TIMPWMConnectionConfig(	);
	
		STM32F4_TimerInstance mTimer;
		STM32F4_TimerChannel mPwmCh0;
		STM32F4_TimerChannel mPwmCh1;
		int8_t mOutBufferId;
};

/**
	\brief AbstractAudioDriver Implementation for STM32
 */
class STM32F4AudioDriver : public AbstractAudioDriver{
	public:
	  STM32F4AudioDriver(SampleRate sr, uint32_t fpb);
    virtual ~STM32F4AudioDriver();

		/**
		\brief Configure the driver
			Create and configure I2S, add buffers, etc
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


		/**
			\brief Configure a new i2s instance
			*/
		void addI2S(STM32F4_I2SInstance i2s, int8_t inBufferId, int8_t outBufferId);
		
		/**
			\brief Configure a new Sigma delta Converter
			*/
		void addSdmPwmDac(STM32F4_TimerInstance instance, STM32F4_TimerChannel pwmCh0, STM32F4_TimerChannel pwmCh1, uint8_t outBufferId,
				uint16_t OSR, uint16_t outBitDepth);

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
		//SDM + PWM periperal
		STMF4_TIMPWMConnectionConfig mPwmConnectionConfig;
		SigmaDeltaModulator *mSdm;
	
	
		//I2S Peripherals 
		STM32F4_I2SConnectionConfig *mI2sConnectionConfig;

};


/**
  }
  */

#endif /* AUDIOCONF_HPP_ */
