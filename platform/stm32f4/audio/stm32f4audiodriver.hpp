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
	STM32F4_I2SInstance mI2s;
	int8_t mInBufferId;
	int8_t mOutBufferId;
};

// ------------------------------ PWM + SDM -----------------------------------

enum STM32F4_TimerInstance{
	NoTIMER = 0,
	TIM1,
	TIM2,
	TIM3,
	TIM4,
	TIM5
};

enum STM32F4_TimerChannel{
	TIMChannel0 = 0,
	TIMChannel1 = 1,
	TIMChannel2 = 2,
	TIMChannel3 = 3
};

class STMF4_TIMPWMConnectionConfig{
	STM32F4_TimerInstance mTimer;
  STM32F4_TimerChannel mPwmCh0;
  STM32F4_TimerChannel mPwmCh1;
	uint8_t mOutBufferId;
};


class STM32F4AudioDriver : public AbstractAudioDriver{
	public:
	        STM32F4AudioDriver(SampleRate sr, uint32_t fpb);

        	virtual ~STM32F4AudioDriver();

	        virtual uint8_t configure();
        	virtual uint8_t terminate();

	        virtual uint8_t start();
        	virtual uint8_t stop();

		void addI2S(STM32F4_I2SInstance i2s, int8_t inBufferId, int8_t outBufferId);
		void addSdmPwmDac(STM32F4_TimerInstance instance, STM32F4_TimerChannel pwmCh0, STM32F4_TimerChannel pwmCh1, uint8_t outBufferId,
				uint16_t OSR, uint16_t outBitDepth);

	protected:
		void mspInit();
		void mspDeInit();

	protected:
		//I2S Peripherals 
		STM32F4_I2SConnectionConfig i2sConnectionConfig[STM32F4I2S_MAXCONNCONF];

		//SDM + PWM periperal
		STMF4_TIMPWMConnectionConfig pwmConnectionConfig;
		SigmaDeltaModulator *sdm;

};


/**
  }
  */

#endif /* AUDIOCONF_HPP_ */
