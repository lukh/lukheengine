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
DMA_HandleTypeDef hdma_tim1_ch1;
DMA_HandleTypeDef hdma_tim1_ch2;
DMA_HandleTypeDef hdma_tim1_ch3;
DMA_HandleTypeDef hdma_tim1_ch4_trig_com;

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
		htim1.Init.Period = 255;
		htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim1.Init.RepetitionCounter = 0;
		
		
		sMasterConfig1.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig1.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		
		
		sBreakDeadTimeConfig1.OffStateRunMode = TIM_OSSR_DISABLE;
		sBreakDeadTimeConfig1.OffStateIDLEMode = TIM_OSSI_DISABLE;
		sBreakDeadTimeConfig1.LockLevel = TIM_LOCKLEVEL_OFF;
		sBreakDeadTimeConfig1.DeadTime = 0;
		sBreakDeadTimeConfig1.BreakState = TIM_BREAK_DISABLE;
		sBreakDeadTimeConfig1.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
		sBreakDeadTimeConfig1.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
		
		sConfigOC1.OCMode = TIM_OCMODE_PWM1;
		sConfigOC1.Pulse = 128;
		sConfigOC1.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC1.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC1.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC1.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC1.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		

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

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	
		/* DMA interrupt init */
	//I2S
	HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);


	//PWM
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	
	HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
	
	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	
	HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
	
	
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
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig1);
	HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig1);
	
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC1, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC1, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC1, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC1, TIM_CHANNEL_4);

	
	
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
	if(HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, mPWMBuffer22, STM32F4AD_FPB) != HAL_OK)
		while(1);
	
	/*if(HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, mPWMBuffer32, STM32F4AD_FPB) != HAL_OK)
		while(1);
	
	if(HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_3, mPWMBuffer41, STM32F4AD_FPB) != HAL_OK)
		while(1);
	
	if(HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, mPWMBuffer52, STM32F4AD_FPB) != HAL_OK)
		while(1);*/
	
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::stop(){
	
	//--- Start PWM + SDM ---
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	/*HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);*/
	
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
		PA9     ------> TIM1_CH2 
		PA10    ------> TIM1_CH3 
		PA11    ------> TIM1_CH4 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		
		hdma_tim1_ch1.Instance = DMA2_Stream1;
    hdma_tim1_ch1.Init.Channel = DMA_CHANNEL_6;
    hdma_tim1_ch1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim1_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim1_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim1_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim1_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim1_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_tim1_ch1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim1_ch1.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_tim1_ch1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim1_ch1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim1_ch1.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim1_ch1);

    __HAL_LINKDMA(&htim1,hdma[TIM_DMA_ID_CC1],hdma_tim1_ch1);

    hdma_tim1_ch2.Instance = DMA2_Stream2;
    hdma_tim1_ch2.Init.Channel = DMA_CHANNEL_6;
    hdma_tim1_ch2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim1_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim1_ch2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim1_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim1_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim1_ch2.Init.Mode = DMA_CIRCULAR;
    hdma_tim1_ch2.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim1_ch2.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_tim1_ch2.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim1_ch2.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim1_ch2.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim1_ch2);

    __HAL_LINKDMA(&htim1,hdma[TIM_DMA_ID_CC2],hdma_tim1_ch2);

    hdma_tim1_ch3.Instance = DMA2_Stream6;
    hdma_tim1_ch3.Init.Channel = DMA_CHANNEL_6;
    hdma_tim1_ch3.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim1_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim1_ch3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim1_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim1_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim1_ch3.Init.Mode = DMA_CIRCULAR;
    hdma_tim1_ch3.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim1_ch3.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_tim1_ch3.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim1_ch3.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim1_ch3.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim1_ch3);

    __HAL_LINKDMA(&htim1,hdma[TIM_DMA_ID_CC3],hdma_tim1_ch3);

    hdma_tim1_ch4_trig_com.Instance = DMA2_Stream4;
    hdma_tim1_ch4_trig_com.Init.Channel = DMA_CHANNEL_6;
    hdma_tim1_ch4_trig_com.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim1_ch4_trig_com.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim1_ch4_trig_com.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim1_ch4_trig_com.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim1_ch4_trig_com.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim1_ch4_trig_com.Init.Mode = DMA_CIRCULAR;
    hdma_tim1_ch4_trig_com.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim1_ch4_trig_com.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_tim1_ch4_trig_com.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim1_ch4_trig_com.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim1_ch4_trig_com.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim1_ch4_trig_com);

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(&htim1,hdma[TIM_DMA_ID_CC4],hdma_tim1_ch4_trig_com);
    __HAL_LINKDMA(&htim1,hdma[TIM_DMA_ID_TRIGGER],hdma_tim1_ch4_trig_com);
    __HAL_LINKDMA(&htim1,hdma[TIM_DMA_ID_COMMUTATION],hdma_tim1_ch4_trig_com);

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
		PA9     ------> TIM1_CH2 
		PA10    ------> TIM1_CH3 
		PA11    ------> TIM1_CH4 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
		
		
		//DeInit DMA
		HAL_DMA_DeInit(htim1.hdma[TIM_DMA_ID_CC1]);
    HAL_DMA_DeInit(htim1.hdma[TIM_DMA_ID_CC2]);
    HAL_DMA_DeInit(htim1.hdma[TIM_DMA_ID_CC3]);
    HAL_DMA_DeInit(htim1.hdma[TIM_DMA_ID_CC4]);
    HAL_DMA_DeInit(htim1.hdma[TIM_DMA_ID_TRIGGER]);
    HAL_DMA_DeInit(htim1.hdma[TIM_DMA_ID_COMMUTATION]);
		
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
