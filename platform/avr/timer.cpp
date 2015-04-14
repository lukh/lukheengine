/*
 * timer.cpp
 *
 * Created: 22/03/2015 22:55:09
 *  Author: Vivien
 */ 

#include "timer.h"

Timer8b *tim0;

volatile uint8_t gTicks;

void ticksCallback(){
	//decount until 0, unless it's different of 0
	if(gTicks > 0)
		gTicks--;
}



ISR(TIMER0_COMPA_vect){
	tim0->callback(TIM_CB_OCA);
}

ISR(TIMER0_COMPB_vect){
	tim0->callback(TIM_CB_OCB);
}

ISR(TIMER0_OVF_vect){
	tim0->callback(TIM_CB_OVF);
}


Timer8b::Timer8b(uint8_t waveformGenerationMode, uint8_t compareMatchOutputsMode, uint8_t clockSelect, uint8_t interruptsEn, uint8_t outputCompareRegA, uint8_t outputCompareRegB,
void (*callbackOutpuCompareA)(void),
void (*callbackOutpuCompareB)(void),
void (*callbackOverflow)(void)) :
	mClockSelect(clockSelect), mCallbackOutpuCompareA(0), mCallbackOutpuCompareB(0), mCallbackOverflow(0)
{
	tim0 = this;
	
	if(callbackOutpuCompareA != 0)
		mCallbackOutpuCompareA = callbackOutpuCompareA;
	
	if(callbackOutpuCompareB != 0)
		mCallbackOutpuCompareB = callbackOutpuCompareB;
	
	if(callbackOverflow != 0)
		mCallbackOverflow = callbackOverflow;
		
	//lowLevelInit();
	
	configure(waveformGenerationMode, compareMatchOutputsMode, interruptsEn, outputCompareRegA, outputCompareRegB);
}

Timer8b::Timer8b(uint8_t clkDiv, uint8_t timResolution) :
	mClockSelect(clkDiv), mCallbackOutpuCompareA(0), mCallbackOutpuCompareB(0), mCallbackOverflow(0)
{
	tim0 = this;
	
	mCallbackOutpuCompareA = ticksCallback;
		
	//lowLevelInit();
		
	configure(TIM0_WGM_HIGH_PWM_MAXTOP|TIM0_WGM_LOW_CTC, TIM0_OC_A_DISCONNECTED|TIM0_OC_B_DISCONNECTED, TIM0_OC_MATCH_A_INTERRUPT_EN, timResolution, 0);
}

Timer8b::~Timer8b(){
	
}

void Timer8b::lowLevelInit(){
	//turn on
	PRR &= ~(1 << PRTIM0);
}

void Timer8b::configure(uint8_t waveformGenerationMode, uint8_t compareMatchOutputsMode, uint8_t interruptsEn, uint8_t outputCompareRegA, uint8_t outputCompareRegB)
{
	TCCR0A = compareMatchOutputsMode | (0x3 & waveformGenerationMode);
	TCCR0B = (waveformGenerationMode & 0x4);
	
	TIMSK0 = interruptsEn;
	
	TCNT0 = 0;
	
	OCR0A = outputCompareRegA;
	OCR0B = outputCompareRegB;
}


void Timer8b::start(){
	TCCR0B |= (TIM0_CLOCKSEL_MASK & mClockSelect);
}

void Timer8b::stop(){
	TCCR0B &= (~TIM0_CLOCKSEL_MASK);
}

void Timer8b::wait(uint8_t ticks){
	//set the global ticks variable.
	gTicks = ticks;
	
	//start the counter
	start();
	
	//wait until gTicks=0
	while(gTicks);
	
	//stop the timer
	stop();
}

void Timer8b::callback(uint8_t callbackType){
	switch(callbackType){
		case TIM_CB_OCA:
			if(mCallbackOutpuCompareA != 0) mCallbackOutpuCompareA();
			break;
			
		case TIM_CB_OCB:
			if(mCallbackOutpuCompareB != 0) mCallbackOutpuCompareB();
			break;
			
		case TIM_CB_OVF:
			if(mCallbackOverflow != 0) mCallbackOverflow();
			break;
			
		default:
			break;
	}
}