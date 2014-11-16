#include "stm32f4audiodriver.hpp"


// ------------------ config class -------------------
STM32F4_I2SConnectionConfig::STM32F4_I2SConnectionConfig() :
	mI2s(NoI2S), mInBufferId(-1), mOutBufferId(-1)
{
}

STMF4_TIMPWMConnectionConfig::STMF4_TIMPWMConnectionConfig() :
	mTimer(NoTimer), mPwmCh0(TimNoChannel), mPwmCh1(TimNoChannel), mOutBufferId(-1)
{
}




// -------------------------------- Audio Driver ------------
STM32F4AudioDriver::STM32F4AudioDriver(SampleRate sr, uint32_t fpb) : 
	AbstractAudioDriver(sr, fpb),

	mPwmConnectionConfig(),
	mSdm((SigmaDeltaModulator *)NULLPTR)
	
{
	mI2sConnectionConfig = new STM32F4_I2SConnectionConfig[STM32F4I2S_MAXCONNCONF];
}

STM32F4AudioDriver::~STM32F4AudioDriver(){
	delete mI2sConnectionConfig;
}

uint8_t STM32F4AudioDriver::configure(){
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::terminate(){
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::start(){
	
	return LE_OK;
}

uint8_t STM32F4AudioDriver::stop(){
	
	return LE_OK;
}

void STM32F4AudioDriver::addI2S(STM32F4_I2SInstance i2s, int8_t inBufferId, int8_t outBufferId){
}


void STM32F4AudioDriver::addSdmPwmDac(STM32F4_TimerInstance instance, STM32F4_TimerChannel pwmCh0, STM32F4_TimerChannel pwmCh1, uint8_t outBufferId,
				uint16_t OSR, uint16_t outBitDepth){
}

void STM32F4AudioDriver::mspInit(){
}


void STM32F4AudioDriver::mspDeInit(){
}

