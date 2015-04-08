/*
 * SPI.cpp
 *
 * Created: 09/02/2015 19:40:29
 *  Author: Vivien
 */ 

#include "SPI.h"

SPI *spi = (SPI *)0;

/*
 * Interrupt
 */
ISR(SPI_vect){
	spi->callback();
}

SPI::SPI(uint8_t mapping,uint8_t dataOrder,uint8_t clockPol,uint8_t clockPhase,uint8_t clockRate,uint8_t doubleSpeed) :
	mPortBaseCS(0), mCsPin(0), mMode(SPI_SLAVEMODE), mData((uint8_t *)0), mDataPointer((uint8_t *)0), mSizeToSend(0), mSizeToReceive(0), mTransferComplete(SPI_TRANSFER_UNDONE)
{
	spi = this;
	
	//low level config in slave mode
	lowLevelInit(mapping);
	
	configure(SPI_SLAVEMODE, SPI_INTERRUPT_DIS, dataOrder, clockPol, clockPhase, clockRate, doubleSpeed);
}

SPI::SPI(uint8_t mapping,uint8_t portBaseCS,uint8_t csPin,uint8_t dataOrder,uint8_t clockPol,uint8_t clockPhase,uint8_t clockRate,uint8_t doubleSpeed) :
	mPortBaseCS(portBaseCS), mCsPin(csPin), mMode(SPI_MASTERMODE), mData((uint8_t *)0), mDataPointer((uint8_t *)0), mSizeToSend(0), mSizeToReceive(0)
{
	spi = this;
	
	//low level config in master mode
	lowLevelInit(mapping, portBaseCS, csPin);
	
	configure(SPI_MASTERMODE, SPI_INTERRUPT_DIS, dataOrder, clockPol, clockPhase, clockRate, doubleSpeed);
}

SPI::~SPI(){
	
}

//slave mode
void SPI::lowLevelInit(uint8_t mapping){
	//Start SPI peripheral
	PRR &= ~(1 << PRSPI);
	
	//set gpio registers
	if(mapping == SPI_DEFMAP){
		REMAP &= ~SPI_REMAPPED; //no remapping
		
		//init I/O
		//out
		P_DDR(PORTB_A) |= (1 << PORT5);
		//P_PUE(PORTB_A)
		
		//in : overwrited, no need to conf
		//P_DDR(PORTB_A) &= ~((1<<PORT7) | (1 << PORT6) | (1 << PORT4));
	}
	
	else{
		REMAP |= SPI_REMAPPED;
		
		//init I/O
		//out
		P_DDR(PORTB_A) |= (1 << PORT0);
		//P_PUE(PORTB_A)
		
		//in : overwrited, no need to conf
		//P_DDR(PORTB_A) &= ~((1<<PORT2) | (1 << PORT1) | (1 << PORT3));
	}
}
		
//master mode
void SPI::lowLevelInit(uint8_t mapping, uint8_t portBaseCS, uint8_t csPin){
	PRR &= ~(1 << PRSPI);
	
	//init of CS
	P_DDR(portBaseCS) |= (1 << csPin);
	//PPUE
	P_PORT(portBaseCS) |= (1 << csPin);
	
	//init of SPI
	if(mapping == SPI_DEFMAP){
		REMAP &= ~SPI_REMAPPED; //no remapping
		
		//init I/O
		//out
		P_DDR(PORTB_A) |= ((1 << PORT6) | (1 << PORT4) | (1 << PORT7)); //the las one for the /SS to avoid switching in slave mode
		//P_PUE(PORTB_A)
		
		//in overwrited
		//P_DDR(PORTB_A) &= ~(1 << PORT5);
	}
	
	else {
		REMAP |= SPI_REMAPPED;
		
		//init I/O
		//out
		P_DDR(PORTB_A) |= ((1 << PORT1) | (1 << PORT3) | (1 << PORT2)); //the las one for the /SS to avoid switching in slave mode
		//P_PUE(PORTB_A)
		
		//in overwrited
		//P_DDR(PORTB_A) &= ~(1 << PORT0);
	}
}


void SPI::configure(uint8_t maSlSelect, uint8_t intEn, uint8_t dataOrder, uint8_t clockPol, uint8_t clockPhase, uint8_t clockRate, uint8_t doubleSpeed){
	//peripheral config
	SPCR = intEn | SPI_ENABLE | dataOrder | maSlSelect | clockPol | clockPhase | clockRate;
	
	SPSR &= ~0x1;
	SPSR |= doubleSpeed;
		
	//clear SPIF
	SPSR;
	SPDR;
};


void SPI::sendByte(uint8_t byte){
	//pull CS low (start transmission in master mode)
	if(mMode == SPI_MASTERMODE)
		P_PORT(mPortBaseCS) &= ~(1 << mCsPin);
	
	/* Start transmission */
	SPDR = byte;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
	
	//pull CS high (stop transmission in master mode)
	if(mMode == SPI_MASTERMODE)
		P_PORT(mPortBaseCS) |= (1 << mCsPin);
}


uint8_t SPI::receiveByte(){
	//pull CS low (start transmission in master mode)
	if(mMode == SPI_MASTERMODE){
		//set CS low
		P_PORT(mPortBaseCS) &= ~(1 << mCsPin);
	
		//send data to get a byte after a shifting
		SPDR = 0x00;
	}
	
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)))
	;
	
	//pull CS high (stop transmission in master mode)
	if(mMode == SPI_MASTERMODE)
		P_PORT(mPortBaseCS) |= (1 << mCsPin);
	
	/* Return Data Register */
	return SPDR;
}



void SPI::sendReceive(uint8_t *pData, uint8_t sizeToSend, uint8_t sizeToReceive){
	if(mMode != SPI_MASTERMODE)
		return;
		
	//init data;
	mData = pData;
	mSizeToSend = sizeToSend;
	mSizeToReceive = sizeToReceive;
	mTransferComplete = SPI_TRANSFER_UNDONE;
	
	//enable interrupt
	SPCR |= SPI_INTERRUPT_EN;
	
	//pull CS low (start transmission in master mode)
	P_PORT(mPortBaseCS) &= ~(1 << mCsPin);
		
	/* Start transmission : send first byte */
	mDataPointer = mData;
	
	SPDR = *mDataPointer++;
	mSizeToSend--;
}

void SPI::callback(){
	//sending remaining data
	if(mSizeToSend > 0){
		SPDR = *mDataPointer++;
		mSizeToSend--;
		
		//data received is saved from the begining of the pointer given
		if(mSizeToSend == 0)
			mDataPointer = mData;
	}
	
	else if(mSizeToReceive > 0){
		*mDataPointer++ = SPDR;
		mSizeToReceive--;
		
		//end of transmission
		if(mSizeToReceive == 0){
			//disable interrupt
			SPCR &= ~SPI_INTERRUPT_EN;
					
			//pull CS high (stop transmission in master mode)
			P_PORT(mPortBaseCS) |= (1 << mCsPin);
			
			//flag
			mTransferComplete = SPI_TRANSFER_DONE;
			
			return;
		}
		
		//ask the slave to transmit
		SPDR = 0x00;
	}
}