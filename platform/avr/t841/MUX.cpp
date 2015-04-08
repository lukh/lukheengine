/*
 * MuX.cpp
 *
 * Created: 08/02/2015 22:04:15
 *  Author: Vivien
 */ 

#include "MUX.h"


Mux4::Mux4(uint8_t portBase, uint8_t enablePin, uint8_t addr0Pin, uint8_t addr1Pin) :
	mPortBase(portBase), mEnablePin(enablePin), mAddr0Pin(addr0Pin), mAddr1Pin(addr1Pin), mAddrValue(0)
{
	lowLevelInit();
}


void Mux4::lowLevelInit()
{
	//set the direction of the three pins as outputs;
	P_DDR(mPortBase) |= (GPIO_OUTPUT << mEnablePin | GPIO_OUTPUT << mAddr0Pin | GPIO_OUTPUT << mAddr1Pin);
	
	//pull-up on PUEA/B : no pull-up
	P_PUE(mPortBase) &= ~(GPIO_PU_EN << mEnablePin | GPIO_PU_EN << mAddr0Pin | GPIO_PU_EN << mAddr1Pin);
	
	//set mAddr, and enable to there init state
	P_PORT(mPortBase) &= (~(1 << mEnablePin | 1 << mAddr0Pin | 1 << mAddr1Pin)); //set to zeros
	P_PORT(mPortBase) |= (MUX_DIS << mEnablePin | MUX_ADDR_LOW << mAddr0Pin | MUX_ADDR_LOW << mAddr1Pin);
}


void Mux4::enable(){
	P_PORT(mPortBase) &= ~(1<<mEnablePin); //mask for reset
	P_PORT(mPortBase) |= (MUX_EN<<mEnablePin); //set value
}

void Mux4::disable(){
	P_PORT(mPortBase) &= ~(1<<mEnablePin); //mask for reset
	P_PORT(mPortBase) |= (MUX_DIS<<mEnablePin); //set value
}

void Mux4::setAddress(uint8_t addr){
	//assert
	
	//save value
	mAddrValue = addr;
	
	//set value
	//format addr regarding polarity
	if(MUX_ADDR_LOW == 1){
		addr = ~addr; //now, addr is inverted if MUX_ADDR_LOW = 1
	}
	
	P_PORT(mPortBase) &= (~(1 << mAddr0Pin | 1 << mAddr1Pin)); //set to zeros
	P_PORT(mPortBase) |= ((addr & 0x1) << mAddr0Pin | ((addr >> 1) & 0x1) << mAddr1Pin);
}

uint8_t Mux4::getAdress(){
	return mAddrValue;
}