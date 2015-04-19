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
	TWSA = addr << 1;
	TWSD = 0xFF;
	
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
		
void I2c::startTransceiver(uint8_t msgExpectedSize){
	//wait until I2C is ready
	while(isBusy()){}

		
	//set the expected message size
	mMsgSize = msgExpectedSize;
	
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
	mBusy = I2C_BUSY;
}


uint8_t I2c::getDataFromTransceiver(uint8_t *msg, uint8_t msgSize){
	uint8_t i;
	
	while(isBusy()){}
	
		
	if(mStatus & I2C_RXDATAINBUF){
		for(i=0;i<mMsgSize;i++)
			msg[i] = mMsg[i];
			
		mStatus &= ~I2C_RXDATAINBUF;
	}
	
	return mStatus;
}


void I2c::callback(){
	static volatile uint8_t bufPtr;
	
	//Bus error case
	if(TWSSRA & I2C_BUS_ERROR){
		//store error
		mError = TWSSRA;
		
		//clear interrupt flags
		TWSSRA = TWSSRA;
		
		//define the next action
		TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_CONTINUE;
		
		//release the peripheral
		mBusy = I2C_READY;
	}
	
	//normal case
	else{
		switch(TWSSRA & 0xC0){
			case I2C_ADDR_STOP_FLAG:
				//bus collision
				if(TWSSRA & I2C_BUS_COLLISION){
					//clear flags;
					TWSSRA = TWSSRA;
				
					//update status
					mBusy = I2C_READY;
					mError = TWSSRA;
				
					//initialize the buffer pointer
					bufPtr = 0;
				
					//Wait for any start condition
					TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_COMPLETE;
				}
		
				else{
						//adress
						if(TWSSRA & I2C_ADDR_STO_IS_ADDR){						
							//update status
							mBusy = I2C_BUSY;
					
							//initialize the buffer pointer
							bufPtr = 0;
					
							//Execute ACK and then receive next byte or set TWDIF to send the data
							TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_CONTINUE;
						}
				
						//stop
						else{
							mBusy = I2C_READY;
					
							//Wait for any start condition
							TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_COMPLETE;
							
							//disable interruptions
							TWSCRA =
								I2C_INTERFACE_EN |                  // Enable TWI-interface and release TWI pins.
								
								I2C_DATA_INTERRUPT_DIS |			// disable TWI Interrupts.
								I2C_ADDR_STOP_INTERRUPT_DIS |
								I2C_STOP_INTERRUPT_DIS |
								
								I2C_SDA_HOLD_TIME_DIS |				//other configs
								I2C_PROMISC_DIS |
								I2C_SMARTMODE_DIS;
						}
					}
					break;
	

			case I2C_DATA_FLAG:
				//---  data transmitted   ---
				if(TWSSRA & I2C_MASTER_READ_FLAG){	
						
					//master ask for the end of the transmission or the slave reach the end of the buffer
					if(((TWSSRA & I2C_MASTER_NACK_FLAG) && (bufPtr > 0)) || (bufPtr == I2C_BUFFSIZE)){
					
						//do we have transmit everything ? (careful in the case of circular buffering, it doesn't work)
						if(bufPtr == mMsgSize)
							mStatus = I2C_LASTTRANSOK;
						else
							mError = TWSSRA;
							
						//since, we still have to finish it	(and clear interruptions, BTW)
						TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_COMPLETE;
					}
				
					else{			
						TWSD = mMsg[bufPtr++];
						
						//clear interruptions and ask for a NO ACTION (everything is done when loading data to TWSD
						TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_CONTINUE;
					}

				}
				
			
				// --- data received ---
				else{
					//get the new data
					mMsg[bufPtr++] = TWSD;
					
					//inform that there are data in the buffer
					mStatus = I2C_RXDATAINBUF;
				
					//if all data are transmitted or you reach the end of the buffer
					//you can received one more byte than msgSize, but then a NACK will be sent (error !)
					//you can't overflow the buffer (second assertion)
					if(bufPtr == mMsgSize + 1 || bufPtr == I2C_BUFFSIZE)
					{					
						//reset the bufPtr ?
						bufPtr = 0;
								
						//send NACK to the master
						TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_COMPLETE | I2C_ACK_ACTION_SENDNACK;				
					}
				
					//still data to received
					else{
						TWSCRB = I2C_HIGH_NOISE_DIS | I2C_CMD_CONTINUE;
					}
				}
				break;			
		}
	}
	
}