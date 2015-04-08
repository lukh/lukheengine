#include "I2C.h"

I2c *i2c = (I2c *)0;

ISR(TWI_SLAVE_vect){
	i2c->callback();
}


I2c::I2c(uint8_t addr) : 
	mBusy(0), mStatus(0), mMsgSize(0)
{
	i2c = this;
	
	lowLevelInit();
	
	configure(addr);
}

I2c::~I2c(){
	
}

void I2c::lowLevelInit(){
	//turn on the TWI module
	PRR &= ~(1 << PRTWI);
	
	//configure SDA and SCL in open collector mode
	//overriden
}

void I2c::configure(uint8_t addr){
	//set the adress
	TWSA = addr;
	
	//set register TWSCRA
	TWSCRA = 
		I2C_INTERFACE_EN |                  // Enable TWI-interface and release TWI pins.
		
		I2C_DATA_INTERRUPT_DIS |			// Disable TWI Interrupts.
		I2C_ADDR_STOP_INTERRUPT_DIS |
		I2C_STOP_INTERRUPT_DIS |
		
		I2C_SDA_HOLD_TIME_DIS |				//other configs
		I2C_PROMISC_DIS |
		I2C_SMARTMODE_DIS;
		
	
	//set register TWSCRB
	TWSCRB = I2C_ACK_ACTION_SENDNACK |    // Do not ACK on any requests, yet.
		I2C_HIGH_NOISE_DIS;							//Hig Noise mode disabled
		
	//set register TWSAM
	TWSAM = I2C_SEC_ADDR_DIS;				//no mask
}


void I2c::startTransceiverWithData(uint8_t *msg, uint8_t msgSize){
	uint8_t i = 0;
	
	//wait until I2C is ready
	while(isBusy()){}
		
	//copy data to local buffer
	mMsgSize = msgSize;	
	for(i = 0; i < mMsgSize; i ++)
		mMsg[i] = msg[i];
		
	//reset status
	mStatus = 0;
	
	//reset error
	mError = 0;
		
	//set register to be ready for transmission
	TWSCRA =
		I2C_INTERFACE_EN |                  // Enable TWI-interface and release TWI pins.
		
		I2C_DATA_INTERRUPT_EN |			// Enable TWI Interrupts.
		I2C_ADDR_STOP_INTERRUPT_EN |
		I2C_STOP_INTERRUPT_EN |
		
		I2C_SDA_HOLD_TIME_DIS |				//other configs
		I2C_PROMISC_DIS |
		I2C_SMARTMODE_DIS;
		
		
	//set busy state
	mBusy = I2C_BUSY;
}
		
void I2c::startTransceiver(){
	//wait until I2C is ready
	while(isBusy()){}
	
	//set status
	mStatus = 0;
	
	//reset error
	mError = 0;
	
	//set register to be ready for transmission
	TWSCRA =
		I2C_INTERFACE_EN |                  // Enable TWI-interface and release TWI pins.
	
		I2C_DATA_INTERRUPT_EN |			// Enable TWI Interrupts.
		I2C_ADDR_STOP_INTERRUPT_EN |
		I2C_STOP_INTERRUPT_EN |
	
		I2C_SDA_HOLD_TIME_DIS |				//other configs
		I2C_PROMISC_DIS |
		I2C_SMARTMODE_DIS;
	
	
	//set ready state, we are waiting for the master
	mBusy = I2C_READY;
}


uint8_t I2c::getDataFromTransceiver(uint8_t *msg, uint8_t msgSize){
	uint8_t i;
	
	while(isBusy()){}
		
	if(mStatus & I2C_LASTTRANSOK){
		for(i=0;i<mMsgSize;i++)
			msg[i] = mMsg[i];
			
		mStatus &= ~I2C_RXDATAINBUF;
	}
	
	return mStatus;
}


void I2c::callback(){
	static uint8_t bufPtr;
	
	switch(TWSSRA){
		//---------------------------
		//--- address received ---
		//---------------------------
		
		//address packet accepted, SRX
		case I2C_SRX_ADR_ACK:
			mStatus |= I2C_RXDATAINBUF;
		//address packet accepted, STX
		case I2C_STX_ADR_ACK:
			bufPtr = 0;
			//ack interrupts flag, set the ack action
			TWSCRB = I2C_CMD_CONTINUE | I2C_ACK_ACTION_SENDACK | I2C_HIGH_NOISE_DIS;		
			//i2c is busy now...
			mBusy = I2C_BUSY;	
			break;

		
		//stop
		case I2C_SRX_STOP_RESTART:
		case I2C_STX_STOP_RESTART:
			//ack interrupts flag, set the ack action
			TWSCRB = I2C_CMD_NOACTION | I2C_ACK_ACTION_SENDACK | I2C_HIGH_NOISE_DIS;
			//i2c is ready now...
			mBusy = I2C_READY;
			break;
	
		//---------------------------
		//---  data received---
		//---------------------------
		case I2C_SRX_DATA_ACK:
		case I2C_SRX_DATA_NACK: //here we don't care about the master ACK/NACK
			mMsg[bufPtr++] = TWSD;
			mStatus |= I2C_LASTTRANSOK;
			
			//master has transmitted all the expected data : slave send NACK
			if(bufPtr == mMsgSize){
				TWSCRB = I2C_CMD_COMPLETE | I2C_ACK_ACTION_SENDNACK | I2C_HIGH_NOISE_DIS;
			}
			
			//master still has to transmit data : slave send ACK
			else{
				TWSCRB = I2C_CMD_CONTINUE | I2C_ACK_ACTION_SENDACK | I2C_HIGH_NOISE_DIS;
			}
			
			//mBusy = I2C_BUSY;
			break;
	
		//---------------------------
		//--- data transmitted ---
		//---------------------------
		case I2C_STX_DATA_ACK:
			TWSD = mMsg[bufPtr++];
		
			//ack interrupts flag, set the ack action
			TWSCRB = I2C_CMD_CONTINUE | I2C_HIGH_NOISE_DIS;
			break;
		
		case I2C_STX_DATA_NACK:
			//ack interrupts flag, set the ack action
			TWSCRB = I2C_CMD_COMPLETE | I2C_HIGH_NOISE_DIS;
			
			//all the expected byte have been transmitted
			if(bufPtr == mMsgSize){
				mStatus |= I2C_LASTTRANSOK;
			}
			
			else{
				//handle error code ?
				mError = TWSSRA;
			}
			
			//mBusy = I2C_READY;
			break;
		
		
		//--------------------------------
		//--- collision + default case and bus error ---
		//--------------------------------
		default:
			//handle error msg
			mError = TWSSRA;
			
			//clear flags
			TWSCRB = I2C_CMD_NOACTION | I2C_ACK_ACTION_SENDACK | I2C_HIGH_NOISE_DIS;	
			
			//---------------------------
			//--- bus error ---
			//---------------------------
			if(TWSSRA & I2C_S_BUS_ERROR)
				TWSSRA = I2C_S_BUS_ERROR;
			
			mBusy = I2C_READY;
			break;
			
		//collision
		/*Bit 3 – TWC: TWI Collision
		This bit is set when the slave was not able to transfer a high data bit or a NACK bit. When a collision is detected, the
		slave will commence its normal operation, and disable data and acknowledge output. No low values are shifted out onto
		the SDA line.
		This bit is cleared by writing a one to it. The bit is also cleared automatically when a START or Repeated START
		condition is detected.*/
	}
	
}