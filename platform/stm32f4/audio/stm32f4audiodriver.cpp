#include "stm32f4audiodriver.hpp"

extern STM32F4AudioDriver *audioDriver;

/**
	* Privates Variables
	*/
	
//test for pwm
const uint32_t psc = 2;
const uint32_t period = 512;
uint32_t dutycycle = 0;
	
// --- I2S ---
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi5_rx;

// --- PWM and SDM Part ----
DMA_HandleTypeDef hdma_tim2_ch1;
DMA_HandleTypeDef hdma_tim2_ch2_ch4;
DMA_HandleTypeDef hdma_tim3_ch1_trig;
DMA_HandleTypeDef hdma_tim3_ch4_up;	


// -------------------------------- Audio Driver ------------
// for the moment I don't care about the parameters given
STM32F4AudioDriver::STM32F4AudioDriver(SampleRate sr, uint32_t fpb) : 
	AbstractAudioDriver(STM32F4AD_SR, STM32F4AD_FPB),
	mDMAAcks(0),
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
	
	//TIM2
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = psc;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = period;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

  sMasterConfig2.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig2.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  sConfigOC2.OCMode = TIM_OCMODE_PWM1;
  sConfigOC2.Pulse = 0;
  sConfigOC2.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC2.OCFastMode = TIM_OCFAST_ENABLE;
  
	
	//TIM3
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = psc;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = period;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  //

  sMasterConfig3.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig3.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  //

  sConfigOC3.OCMode = TIM_OCMODE_PWM1;
  sConfigOC3.Pulse = 0;
  sConfigOC3.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC3.OCFastMode = TIM_OCFAST_DISABLE;
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
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
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
	
	//TIMER2
	HAL_TIM_PWM_Init(&htim2);
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig2);
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC2, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC2, TIM_CHANNEL_2);
	
	//TIMER3
	HAL_TIM_PWM_Init(&htim3);
	HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig3);
	HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC3, TIM_CHANNEL_4);
	
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
	mDMAAcks = TIM2CH1_DMAFLAG | TIM2CH2_DMAFLAG | TIM3CH1_DMAFLAG | TIM3CH4_DMAFLAG;
	
	// --- Start I2S ---
	
	// --- Start PWM + SDM ---
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, mPWMBuffer21, STM32F4AD_FPB*SDM_OSR);
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, mPWMBuffer22, STM32F4AD_FPB*SDM_OSR);
	
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, mPWMBuffer31, STM32F4AD_FPB*SDM_OSR);
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, mPWMBuffer34, STM32F4AD_FPB*SDM_OSR);
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::stop(){
	
	//--- Start PWM + SDM ---
	HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
	
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_4);
	
	return LE_OK;
}

void STM32F4AudioDriver::process(){
	// ready for a new round
	mDMAAcks = TIM2CH1_DMAFLAG | TIM2CH2_DMAFLAG | TIM3CH1_DMAFLAG | TIM3CH4_DMAFLAG;
		
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
	hdma_spi1_rx.Instance = DMA2_Stream0;
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

	__HAL_LINKDMA(&hi2s1,hdmarx,hdma_spi1_rx);

  

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

	hdma_spi2_rx.Instance = DMA1_Stream3;
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

	__HAL_LINKDMA(&hi2s2,hdmarx,hdma_spi2_rx);


  

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

	hdma_spi5_rx.Instance = DMA2_Stream3;
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

	__HAL_LINKDMA(&hi2s5,hdmarx,hdma_spi5_rx);
	
	
	
	//**************************************************
	// -------------------------- PWM ------------------
	//**************************************************
	
    /* Peripheral clock enable */
    __TIM2_CLK_ENABLE();
  
    /**TIM2 GPIO Configuration    
    PA15     ------> TIM2_CH1
    PB3     ------> TIM2_CH2 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_tim2_ch1.Instance = DMA1_Stream5;
    hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;
    hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_tim2_ch1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim2_ch1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim2_ch1.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim2_ch1);

    __HAL_LINKDMA(&htim2,hdma[TIM_DMA_ID_CC1],hdma_tim2_ch1);

    hdma_tim2_ch2_ch4.Instance = DMA1_Stream6;
    hdma_tim2_ch2_ch4.Init.Channel = DMA_CHANNEL_3;
    hdma_tim2_ch2_ch4.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim2_ch2_ch4.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch2_ch4.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch2_ch4.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tim2_ch2_ch4.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_tim2_ch2_ch4.Init.Mode = DMA_CIRCULAR;
    hdma_tim2_ch2_ch4.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim2_ch2_ch4.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_tim2_ch2_ch4.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim2_ch2_ch4.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim2_ch2_ch4.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim2_ch2_ch4);

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(&htim2,hdma[TIM_DMA_ID_CC2],hdma_tim2_ch2_ch4);
    //__HAL_LINKDMA(&htim2,hdma[TIM_DMA_ID_CC4],hdma_tim2_ch2_ch4);

  


	
	
    /* Peripheral clock enable */
    __TIM3_CLK_ENABLE();
  
    /**TIM3 GPIO Configuration    
    PA6     ------> TIM3_CH1
    PC9     ------> TIM3_CH4 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_tim3_ch1_trig.Instance = DMA1_Stream4;
    hdma_tim3_ch1_trig.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3_ch1_trig.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch1_trig.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch1_trig.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch1_trig.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tim3_ch1_trig.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_tim3_ch1_trig.Init.Mode = DMA_CIRCULAR;
    hdma_tim3_ch1_trig.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim3_ch1_trig.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_tim3_ch1_trig.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim3_ch1_trig.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim3_ch1_trig.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim3_ch1_trig);

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(&htim3,hdma[TIM_DMA_ID_CC1],hdma_tim3_ch1_trig);
    //__HAL_LINKDMA(&htim3,hdma[TIM_DMA_ID_TRIGGER],hdma_tim3_ch1_trig);

    hdma_tim3_ch4_up.Instance = DMA1_Stream2;
    hdma_tim3_ch4_up.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3_ch4_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch4_up.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch4_up.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch4_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tim3_ch4_up.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_tim3_ch4_up.Init.Mode = DMA_CIRCULAR;
    hdma_tim3_ch4_up.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim3_ch4_up.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_tim3_ch4_up.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_tim3_ch4_up.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim3_ch4_up.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim3_ch4_up);

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one stream to perform all the requested DMAs. */
    __HAL_LINKDMA(&htim3,hdma[TIM_DMA_ID_CC4],hdma_tim3_ch4_up);
    //__HAL_LINKDMA(&htim3,hdma[TIM_DMA_ID_UPDATE],hdma_tim3_ch4_up);

  
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
		

  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __TIM2_CLK_DISABLE();
  
    /**TIM2 GPIO Configuration    
    PA15     ------> TIM2_CH1
    PB3     ------> TIM2_CH2 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(htim2.hdma[TIM_DMA_ID_CC1]);
    HAL_DMA_DeInit(htim2.hdma[TIM_DMA_ID_CC2]);
    HAL_DMA_DeInit(htim2.hdma[TIM_DMA_ID_CC4]);

 
    /* Peripheral clock disable */
    __TIM3_CLK_DISABLE();
  
    /**TIM3 GPIO Configuration    
    PA6     ------> TIM3_CH1
    PC9     ------> TIM3_CH4 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(htim3.hdma[TIM_DMA_ID_CC1]);
    HAL_DMA_DeInit(htim3.hdma[TIM_DMA_ID_TRIGGER]);
    HAL_DMA_DeInit(htim3.hdma[TIM_DMA_ID_CC4]);
    HAL_DMA_DeInit(htim3.hdma[TIM_DMA_ID_UPDATE]);


		
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
	if(htim->Instance == TIM2){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			audioDriver->acknDMAFlag(TIM2CH1_DMAFLAG);
		}
		
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			audioDriver->acknDMAFlag(TIM2CH2_DMAFLAG);
		}
	}
	
	else if(htim->Instance == TIM3){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			audioDriver->acknDMAFlag(TIM3CH1_DMAFLAG);
		}
		
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			audioDriver->acknDMAFlag(TIM3CH4_DMAFLAG);
		}
	}
	
	//in the case of all DMA are finished :
	if(audioDriver->getDMAAck() == 0){
		//change mOutBuffer : second half
		audioDriver->setOutBufferAddr(0, &(audioDriver->getBufferSDM1()[STM32F4AD_HALFFPB]));
		audioDriver->setOutBufferAddr(1, &(audioDriver->getBufferSDM2()[STM32F4AD_HALFFPB]));

		//call the audio processing
		audioDriver->process();
		
		//change pwm buffers : second half of the buffer
		uint32_t i;
		for(i=STM32F4AD_HALFFPB*SDM_OSR; i < STM32F4AD_FPB*SDM_OSR; i ++){
			audioDriver->mPWMBuffer21[i] = dutycycle;
			audioDriver->mPWMBuffer22[i] = dutycycle;
			audioDriver->mPWMBuffer31[i] = dutycycle;
			audioDriver->mPWMBuffer34[i] = dutycycle;
		}
		//call for the sdm processing
		
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			audioDriver->acknDMAFlag(TIM2CH1_DMAFLAG);
		}
		
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			audioDriver->acknDMAFlag(TIM2CH2_DMAFLAG);
		}
	}
	
	else if(htim->Instance == TIM3){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			audioDriver->acknDMAFlag(TIM3CH1_DMAFLAG);
		}
		
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			audioDriver->acknDMAFlag(TIM3CH4_DMAFLAG);
		}
	}
	
	//in the case of all DMA are finished :
	if(audioDriver->getDMAAck() == 0){
		//change mOutBuffer : first half
		audioDriver->setOutBufferAddr(0, audioDriver->getBufferSDM1());
		audioDriver->setOutBufferAddr(1, audioDriver->getBufferSDM2());

		//call the audio processing
		audioDriver->process();
		
		//change pwm buffers : first half of the buffer
		uint32_t i;
		for(i=0; i < STM32F4AD_HALFFPB*SDM_OSR; i ++){
			audioDriver->mPWMBuffer21[i] = dutycycle;
			audioDriver->mPWMBuffer22[i] = dutycycle;
			audioDriver->mPWMBuffer31[i] = dutycycle;
			audioDriver->mPWMBuffer34[i] = dutycycle;
		}
		dutycycle++;
		if(dutycycle == period) dutycycle=0;
		
		//call for the sdm processing
		
	}
}
