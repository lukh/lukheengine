/*
 * ADC.cpp
 *
 * Created: 08/03/2015 13:48:51
 *  Author: Vivien
 */ 

#include "ADC.h"

Adc *adc;

ISR(ADC_vect){
	adc->callback();
}

Adc::Adc(uint8_t ref, uint8_t prescaler, uint8_t autoTrigEn, uint8_t autoTrigSource, void (*cb)(uint16_t)){
	adc = this;
	
	if(cb != (void *)0)
		mCallback = cb;
		
	lowLevelInit();
	configure(ref, prescaler, autoTrigEn, autoTrigSource);
}

Adc::~Adc(){
}

void Adc::lowLevelInit(){
	//turn on the ADC module
	PRR &= ~(1 << PRADC);
	
	//debug purpose
	
	
}

void Adc::configure(uint8_t ref, uint8_t prescaler, uint8_t autoTrigEn, uint8_t autoTrigSource){
	//configure the peripheral
	ADCSRA = autoTrigEn | prescaler;
	ADCSRB = ADC_RIGHT_ADJUST_RESULT | autoTrigSource;
	
	ADMUXB = ref | ADC_MUXB_GAIN_1;
	ADMUXA = ADC_MUXA_CH0;
	
	ADCSRA |= ADC_ENABLE;
}

void Adc::setMuxChannel(uint8_t channel, uint8_t did){
	//set channel
	ADMUXA = channel;
	
	//disable digital input
	if(did == ADC_DI_DIS)
	{
		if(channel < ADC_MUXA_CH8)
			DIDR0 |= (1<<channel);
		else
			DIDR1 |= (1<<(channel-8));
		
	}
	
	//or enable digital input
	else
	{
		if(channel < ADC_MUXA_CH8)
			DIDR0 &= ~(1<<channel);
		else
			DIDR1 &= ~(1<<(channel-8));
	}
}


uint16_t Adc::convert(){
	//disable interruption and start a conversion and the peripheral
	ADCSRA &= ~(ADC_INTERRUPT_EN);
	ADCSRA |= ADC_START_CONV;
	
	//wait for the conversion
	while(ADCSRA & ADC_START_CONV);

	
	//clearing...
	//ADCSRA |= ADC_INTERRUPT_FLAG;
	
	//get value...
	mValue = ADCW;
	
	return mValue;
}


void Adc::startConversion(){
	//enable adc, interruption and start a conversion
	ADCSRA |= (ADC_ENABLE | ADC_START_CONV | ADC_INTERRUPT_EN);
}

void Adc::stopConversion(){
	//disable adc, interruptions
	ADCSRA &= (~ADC_ENABLE) & (~ADC_INTERRUPT_EN);
}

void Adc::callback(){
	//get value...
	mValue = ADCL;
	mValue = ADCH << 8;
	
	
	//callback
	if(mCallback != (void *)0)
		mCallback(mValue);
}
