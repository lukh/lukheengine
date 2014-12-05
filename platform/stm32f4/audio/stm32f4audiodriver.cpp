#include "stm32f4audiodriver.hpp"

extern STM32F4AudioDriver *audioDriver;

/**
	* Privates Variables
	*/
	
//test for pwm
const uint32_t psc = 2;
const uint32_t period = 512;
uint32_t dutycycle = 0;

// ---------------------  HAL Struct For DMA ------
// --- I2S ---
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi5_rx;

// --- PWM and SDM Part ----
DMA_HandleTypeDef hdma_tim2_ch2_ch4;
DMA_HandleTypeDef hdma_tim3_ch2;
DMA_HandleTypeDef hdma_tim4_ch1;
DMA_HandleTypeDef hdma_tim5_ch2;

// -------------------- Arrays for the buffers ----------------
/**
	* Effectives buffers
	* The Pointers in The abstracted class will point on theses ones.
	* The "*2" is because the drivers will used the buffers in Circular mode : 
	* the 2 half parts of the buffers works in parallel : one is filled, the other one is used
	*/
static __attribute__((zero_init))Sample mI2S1InBuffer[STM32F4AD_FPB*2];
static __attribute__((zero_init))Sample mI2S2InBuffer[STM32F4AD_FPB*2];
static __attribute__((zero_init))Sample mI2S5InBuffer[STM32F4AD_FPB*2];

static __attribute__((zero_init))Sample mSDM1Buffer[STM32F4AD_FPB*2];
static __attribute__((zero_init))Sample mSDM2Buffer[STM32F4AD_FPB*2];

/**
 * Buffers for the SDM and PWMs
 */
static __attribute__((zero_init))SDMOutputType mPWMBuffer22[STM32F4AD_FPB*SDM_OSR];
static __attribute__((zero_init))SDMOutputType mPWMBuffer32[STM32F4AD_FPB*SDM_OSR];
static __attribute__((zero_init))SDMOutputType mPWMBuffer41[STM32F4AD_FPB*SDM_OSR];
static __attribute__((zero_init))SDMOutputType mPWMBuffer52[STM32F4AD_FPB*SDM_OSR];


// -------------------------------- Audio Driver ------------
// for the moment I don't care about the parameters given
STM32F4AudioDriver::STM32F4AudioDriver(SampleRate sr, uint32_t fpb) : 
	AbstractAudioDriver(STM32F4AD_SR, STM32F4AD_FPB),
	mDMAAcks(STM32F4_NUMACKS),
	mSdm1((SigmaDeltaModulator *)NULLPTR),
	mSdm2((SigmaDeltaModulator *)NULLPTR)
{
		//-----------------------
		//--- Data Structures ---
		//-----------------------

		setNumStereoIn(3);
		setNumStereoOut(2);

		mInBuffers[0] = mI2S1InBuffer;
		mInBuffers[1] = mI2S1InBuffer;
		mInBuffers[2] = mI2S1InBuffer;

		mOutBuffers[0] = mSDM1Buffer;
		mOutBuffers[1] = mSDM2Buffer;

		//mSdm1, 2 = new....





		//----------------------
		// --- I2S : 1, 2, 5 ---
		//----------------------

		//I2S1
		hi2s1.Instance = SPI1;
		hi2s1.Init.Mode = I2S_MODE_MASTER_RX;
		hi2s1.Init.Standard = I2S_STANDARD_PHILLIPS;
		hi2s1.Init.DataFormat = I2S_DATAFORMAT_24B;
		hi2s1.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
		hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_96K;
		hi2s1.Init.CPOL = I2S_CPOL_LOW;
		hi2s1.Init.ClockSource = I2S_CLOCK_PLL;
		hi2s1.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

		//I2S2
		hi2s2.Instance = SPI2;
		hi2s2.Init.Mode = I2S_MODE_MASTER_RX;
		hi2s2.Init.Standard = I2S_STANDARD_PHILLIPS;
		hi2s2.Init.DataFormat = I2S_DATAFORMAT_24B;
		hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
		hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_96K;
		hi2s2.Init.CPOL = I2S_CPOL_LOW;
		hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
		hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;


		//I2S5
		hi2s5.Instance = SPI5;
		hi2s5.Init.Mode = I2S_MODE_MASTER_RX;
		hi2s5.Init.Standard = I2S_STANDARD_PHILLIPS;
		hi2s5.Init.DataFormat = I2S_DATAFORMAT_24B;
		hi2s5.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
		hi2s5.Init.AudioFreq = I2S_AUDIOFREQ_96K;
		hi2s5.Init.CPOL = I2S_CPOL_LOW;
		hi2s5.Init.ClockSource = I2S_CLOCK_PLL;
		hi2s5.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;


		//-----------------
		//--- PWM + SDM ---
		//-----------------

		//SDM
		//TODO !!!!
		
		//TIM1
		htim1.Instance = TIM1;
		htim1.Init.Prescaler = 1;
		htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim1.Init.Period = 63;
		htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim1.Init.RepetitionCounter = 0;
		
		
		sMasterConfig1.MasterOutputTrigger = TIM_TRGO_ENABLE;
		sMasterConfig1.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
		
		sSlaveConfig1.SlaveMode = TIM_SLAVEMODE_TRIGGER;
		sSlaveConfig1.InputTrigger = TIM_TS_TI1F_ED;
		
		sBreakDeadTimeConfig1.OffStateRunMode = TIM_OSSR_DISABLE;
		sBreakDeadTimeConfig1.OffStateIDLEMode = TIM_OSSI_DISABLE;
		sBreakDeadTimeConfig1.LockLevel = TIM_LOCKLEVEL_OFF;
		sBreakDeadTimeConfig1.DeadTime = 0;
		sBreakDeadTimeConfig1.BreakState = TIM_BREAK_DISABLE;
		sBreakDeadTimeConfig1.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
		sBreakDeadTimeConfig1.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
		
		sConfigOC1.OCMode = TIM_OCMODE_PWM1;
		sConfigOC1.Pulse = 4;
		sConfigOC1.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC1.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC1.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC1.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC1.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		

		//TIM2
		htim2.Instance = TIM2;
		htim2.Init.Prescaler = psc;
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim2.Init.Period = period;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		
		sSlaveConfig2.SlaveMode = TIM_SLAVEMODE_TRIGGER;
		sSlaveConfig2.InputTrigger = TIM_TS_ITR0;

		sMasterConfig2.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig2.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

		sConfigOC2.OCMode = TIM_OCMODE_PWM1;
		sConfigOC2.Pulse = period/2;
		sConfigOC2.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC2.OCFastMode = TIM_OCFAST_ENABLE;


		//TIM3
		htim3.Instance = TIM3;
		htim3.Init.Prescaler = psc;
		htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim3.Init.Period = period;
		htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		//

		sSlaveConfig3.SlaveMode = TIM_SLAVEMODE_TRIGGER;
		sSlaveConfig3.InputTrigger = TIM_TS_ITR0;

		sMasterConfig3.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig3.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		//

		sConfigOC3.OCMode = TIM_OCMODE_PWM1;
		sConfigOC3.Pulse = period/2;
		sConfigOC3.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC3.OCFastMode = TIM_OCFAST_ENABLE;

		//TIM4
		htim4.Instance = TIM4;
		htim4.Init.Prescaler = psc;
		htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim4.Init.Period = period;
		htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		//
		
		sSlaveConfig4.SlaveMode = TIM_SLAVEMODE_TRIGGER;
		sSlaveConfig4.InputTrigger = TIM_TS_ITR0;

		sMasterConfig4.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig4.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		//

		sConfigOC4.OCMode = TIM_OCMODE_PWM1;
		sConfigOC4.Pulse = period/2;
		sConfigOC4.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC4.OCFastMode = TIM_OCFAST_ENABLE;

		//TIM5
		htim5.Instance = TIM5;
		htim5.Init.Prescaler = psc;
		htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim5.Init.Period = period;
		htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		//
		
		sSlaveConfig5.SlaveMode = TIM_SLAVEMODE_TRIGGER;
		sSlaveConfig5.InputTrigger = TIM_TS_ITR0;

		sMasterConfig5.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig5.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		//

		sConfigOC5.OCMode = TIM_OCMODE_PWM1;
		sConfigOC5.Pulse = period/2;
		sConfigOC5.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC5.OCFastMode = TIM_OCFAST_ENABLE;
}

STM32F4AudioDriver::~STM32F4AudioDriver(){
	terminate();
}

uint8_t STM32F4AudioDriver::configure(){
	//------------------------
	//--- --- DMA INIT --- ---
	//------------------------
	/* DMA controller clock enable */
	__DMA1_CLK_ENABLE();
	__DMA2_CLK_ENABLE();

		/* DMA interrupt init */
	HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);


	//------------------------
	//--- --- Msp INIT --- ---
	//------------------------
	mspInit();	



	//-----------------------------------------
	//--- --- Peripheral Initialisation --- ---
	//-----------------------------------------

	//I2S1, 2, 5
	HAL_I2S_Init(&hi2s1);
	HAL_I2S_Init(&hi2s2);
	HAL_I2S_Init(&hi2s5);
	
	//TIMER1
	HAL_TIM_PWM_Init(&htim1);
	HAL_TIM_SlaveConfigSynchronization(&htim1, &sSlaveConfig1);
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig1);
	HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig1);
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC1, TIM_CHANNEL_1);

	//TIMER2
	HAL_TIM_PWM_Init(&htim2);
	HAL_TIM_SlaveConfigSynchronization(&htim2, &sSlaveConfig2);
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig2);
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC2, TIM_CHANNEL_2);

	//TIMER3
	HAL_TIM_PWM_Init(&htim3);
	HAL_TIM_SlaveConfigSynchronization(&htim3, &sSlaveConfig3);
	HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig3);
	HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_2);
	
	//TIMER4
	HAL_TIM_PWM_Init(&htim4);
	HAL_TIM_SlaveConfigSynchronization(&htim4, &sSlaveConfig4);
	HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig4);
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC4, TIM_CHANNEL_1);
	
	//TIMER5
	HAL_TIM_PWM_Init(&htim5);
	HAL_TIM_SlaveConfigSynchronization(&htim5, &sSlaveConfig5);
	HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig5);
	HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC5, TIM_CHANNEL_2);
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::terminate(){
	stop();
	
	//do stuff here ?
	
	mspDeInit();
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::start(){
	// --- general ----
	mDMAAcks = STM32F4_NUMACKS;
	// --- Start I2S ---
	
	// --- Start PWM + SDM ---
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, mPWMBuffer22, STM32F4AD_FPB*SDM_OSR);
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, mPWMBuffer32, STM32F4AD_FPB*SDM_OSR);
	
	HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1, mPWMBuffer41, STM32F4AD_FPB*SDM_OSR);
	HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_2, mPWMBuffer52, STM32F4AD_FPB*SDM_OSR);
	
  /*HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);*/
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::stop(){
	
	//--- Start PWM + SDM ---
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	
	HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_2);
	
	HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_2);
	
	return LE_OK;
}

void STM32F4AudioDriver::process(){
	// ready for a new round
	mDMAAcks = STM32F4_NUMACKS;
		
	mEngine->process();
}


void STM32F4AudioDriver::mspInit(){
		GPIO_InitTypeDef GPIO_InitStruct;


		//**************************************************
		// -------------------------- I2S ------------------
		//**************************************************

		/* Peripheral clock enable */
		__SPI1_CLK_ENABLE();

		/**I2S1 GPIO Configuration    
		PA4     ------> I2S1_WS
		PA5     ------> I2S1_CK
		PA7     ------> I2S1_SD 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Peripheral DMA init*/
		/*hdma_spi1_rx.Instance = DMA2_Stream0;
		hdma_spi1_rx.Init.Channel = DMA_CHANNEL_3;
		hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;			//checkthat !
		hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_spi1_rx.Init.Mode = DMA_CIRCULAR;
		hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_spi1_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_spi1_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_spi1_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;		
		HAL_DMA_Init(&hdma_spi1_rx);

		__HAL_LINKDMA(&hi2s1,hdmarx,hdma_spi1_rx);*/



		/* Peripheral clock enable */
		__SPI2_CLK_ENABLE();

		/**I2S2 GPIO Configuration    
		PC3     ------> I2S2_SD
		PB10     ------> I2S2_CK
		PB9     ------> I2S2_WS 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		/*hdma_spi2_rx.Instance = DMA1_Stream3;
		hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
		hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_spi2_rx.Init.Mode = DMA_CIRCULAR;
		hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_spi2_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_spi2_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_spi2_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		HAL_DMA_Init(&hdma_spi2_rx);

		__HAL_LINKDMA(&hi2s2,hdmarx,hdma_spi2_rx);*/




		/* Peripheral clock enable */
		__SPI5_CLK_ENABLE();

		/**I2S5 GPIO Configuration    
		PB0     ------> I2S5_CK
		PB1     ------> I2S5_WS
		PA10     ------> I2S5_SD 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF6_SPI5;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF6_SPI5;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		/*hdma_spi5_rx.Instance = DMA2_Stream3;
		hdma_spi5_rx.Init.Channel = DMA_CHANNEL_2;
		hdma_spi5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_spi5_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi5_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_spi5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_spi5_rx.Init.Mode = DMA_CIRCULAR;
		hdma_spi5_rx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_spi5_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_spi5_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_spi5_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_spi5_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		HAL_DMA_Init(&hdma_spi5_rx);

		__HAL_LINKDMA(&hi2s5,hdmarx,hdma_spi5_rx);*/



		//**************************************************
		// -------------------------- PWM ------------------
		//**************************************************
		
		__TIM1_CLK_ENABLE();
  
    /**TIM1 GPIO Configuration    
    PA8     ------> TIM1_CH1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		//tim2
		/* Peripheral clock enable */
		__TIM2_CLK_ENABLE();

		/**TIM2 GPIO Configuration    
		PB3     ------> TIM2_CH2 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral DMA init*/


		hdma_tim2_ch2_ch4.Instance = DMA1_Stream6;
		hdma_tim2_ch2_ch4.Init.Channel = DMA_CHANNEL_3;
		hdma_tim2_ch2_ch4.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tim2_ch2_ch4.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tim2_ch2_ch4.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tim2_ch2_ch4.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_tim2_ch2_ch4.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_tim2_ch2_ch4.Init.Mode = DMA_CIRCULAR;
		hdma_tim2_ch2_ch4.Init.Priority = DMA_PRIORITY_HIGH;
		hdma_tim2_ch2_ch4.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_tim2_ch2_ch4.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_tim2_ch2_ch4.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_tim2_ch2_ch4.Init.PeriphBurst = DMA_PBURST_SINGLE;
		HAL_DMA_Init(&hdma_tim2_ch2_ch4);

		/* Several peripheral DMA handle pointers point to the same DMA handle.
		Be aware that there is only one stream to perform all the requested DMAs. */
		__HAL_LINKDMA(&htim2,hdma[TIM_DMA_ID_CC2],hdma_tim2_ch2_ch4);



		//tim3


		/* Peripheral clock enable */
		__TIM3_CLK_ENABLE();

		/**TIM3 GPIO Configuration    
		PC7     ------> TIM3_CH2
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		hdma_tim3_ch2.Instance = DMA1_Stream5;
		hdma_tim3_ch2.Init.Channel = DMA_CHANNEL_5;
		hdma_tim3_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tim3_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tim3_ch2.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tim3_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_tim3_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_tim3_ch2.Init.Mode = DMA_CIRCULAR;
		hdma_tim3_ch2.Init.Priority = DMA_PRIORITY_HIGH;
		hdma_tim3_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_tim3_ch2.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_tim3_ch2.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_tim3_ch2.Init.PeriphBurst = DMA_PBURST_SINGLE;
		HAL_DMA_Init(&hdma_tim3_ch2);

		__HAL_LINKDMA(&htim3,hdma[TIM_DMA_ID_CC2],hdma_tim3_ch2);

		//tim4

		__TIM4_CLK_ENABLE();

		/**TIM4 GPIO Configuration    
		PB6     ------> TIM4_CH1
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		hdma_tim4_ch1.Instance = DMA1_Stream0;
		hdma_tim4_ch1.Init.Channel = DMA_CHANNEL_2;
		hdma_tim4_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tim4_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tim4_ch1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tim4_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_tim4_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_tim4_ch1.Init.Mode = DMA_CIRCULAR;
		hdma_tim4_ch1.Init.Priority = DMA_PRIORITY_HIGH;
		hdma_tim4_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_tim4_ch1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_tim4_ch1.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_tim4_ch1.Init.PeriphBurst = DMA_PBURST_SINGLE;
		HAL_DMA_Init(&hdma_tim4_ch1);

		__HAL_LINKDMA(&htim4,hdma[TIM_DMA_ID_CC1],hdma_tim4_ch1);




		//tim5

		__TIM5_CLK_ENABLE();

		/**TIM5 GPIO Configuration    
		PA1     ------> TIM5_CH2
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Peripheral DMA init*/

		hdma_tim5_ch2.Instance = DMA1_Stream4;
		hdma_tim5_ch2.Init.Channel = DMA_CHANNEL_6;
		hdma_tim5_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tim5_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tim5_ch2.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tim5_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_tim5_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_tim5_ch2.Init.Mode = DMA_CIRCULAR;
		hdma_tim5_ch2.Init.Priority = DMA_PRIORITY_HIGH;
		hdma_tim5_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_tim5_ch2.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_tim5_ch2.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_tim5_ch2.Init.PeriphBurst = DMA_PBURST_SINGLE;
		HAL_DMA_Init(&hdma_tim5_ch2);

		__HAL_LINKDMA(&htim5,hdma[TIM_DMA_ID_CC2],hdma_tim5_ch2);

}


void STM32F4AudioDriver::mspDeInit(){
	
		//**************************************************
		// -------------------------- I2S ------------------
		//**************************************************

		/* Peripheral clock disable */
		__SPI1_CLK_DISABLE();

		/**I2S1 GPIO Configuration    
		PA4     ------> I2S1_WS
		PA5     ------> I2S1_CK
		PA7     ------> I2S1_SD 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(hi2s1.hdmarx);

		/* Peripheral clock disable */
		__SPI2_CLK_DISABLE();

		/**I2S2 GPIO Configuration    
		PC3     ------> I2S2_SD
		PB10     ------> I2S2_CK
		PB9     ------> I2S2_WS 
		*/
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_9);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(hi2s2.hdmarx);


		/* Peripheral clock disable */
		__SPI5_CLK_DISABLE();

		/**I2S5 GPIO Configuration    
		PB0     ------> I2S5_CK
		PB1     ------> I2S5_WS
		PA10     ------> I2S5_SD 
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1);

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(hi2s5.hdmarx);



		//**************************************************
		// -------------------------- PWM ------------------
		//**************************************************


		__TIM1_CLK_DISABLE();
  
    /**TIM1 GPIO Configuration    
    PA8     ------> TIM1_CH1 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
		
		
		
		/* Peripheral clock disable */
		__TIM2_CLK_DISABLE();

		/**TIM2 GPIO Configuration    
		PB3     ------> TIM2_CH2 
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(htim2.hdma[TIM_DMA_ID_CC2]);


		/* Peripheral clock disable */
		__TIM3_CLK_DISABLE();

		/**TIM3 GPIO Configuration    
		PC7     ------> TIM3_CH2
		*/
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_7);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(htim3.hdma[TIM_DMA_ID_CC2]);


		/* Peripheral clock disable */
		__TIM4_CLK_DISABLE();

		/**TIM4 GPIO Configuration    
		PB6     ------> TIM4_CH1
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(htim4.hdma[TIM_DMA_ID_CC1]);



		/* Peripheral clock disable */
		__TIM5_CLK_DISABLE();

		/**TIM5 GPIO Configuration    
		PA1     ------> TIM5_CH2
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

		/* Peripheral DMA DeInit*/
		HAL_DMA_DeInit(htim5.hdma[TIM_DMA_ID_CC2]);
		
}





//---------------------------- Interruptions ------------------

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
	
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s){
	
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
	
	
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s){
	
}


void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s){
	
	
}

void HAL_TIM_PWM_PulseHalfFinishedCallback(TIM_HandleTypeDef *htim){
	audioDriver->acknOneDMA();
	
	//in the case of all PWM DMA are finished :
	if(audioDriver->getDMAAck() == 0){
		//change mOutBuffer : second half
		audioDriver->setOutBufferAddr(0, &mSDM1Buffer[STM32F4AD_HALFFPB]);
		audioDriver->setOutBufferAddr(1, &mSDM2Buffer[STM32F4AD_HALFFPB]);

		//call the audio processing
		audioDriver->process();
		
		//change pwm buffers : second half of the buffer
		uint32_t i;
		for(i=STM32F4AD_HALFFPB*SDM_OSR; i < STM32F4AD_FPB*SDM_OSR; i ++){
			mPWMBuffer22[i] = dutycycle;
			mPWMBuffer32[i] = dutycycle;
			mPWMBuffer41[i] = dutycycle;
			mPWMBuffer52[i] = dutycycle;
		}
		//call for the sdm processing
		
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	audioDriver->acknOneDMA();
	
	//in the case of all DMA are finished :
	if(audioDriver->getDMAAck() == 0){
		//change mOutBuffer : first half
		audioDriver->setOutBufferAddr(0, mSDM1Buffer);
		audioDriver->setOutBufferAddr(1, mSDM2Buffer);

		//call the audio processing
		audioDriver->process();
		
		//change pwm buffers : first half of the buffer
		uint32_t i;
		for(i=0; i < STM32F4AD_HALFFPB*SDM_OSR; i ++){
			mPWMBuffer22[i] = dutycycle;
			mPWMBuffer32[i] = dutycycle;
			mPWMBuffer41[i] = dutycycle;
			mPWMBuffer52[i] = dutycycle;
		}
		dutycycle++;
		if(dutycycle == (period)) dutycycle=0;
		
		//call for the sdm processing
		
	}
}
