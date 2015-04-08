/*
 * I2C.h
 *
 * Created: 07/02/2015 23:21:34
 *  Author: Vivien
 */


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "GPIO.h"

/*
 * Configuration defines
 */
//class relative
#define I2C_BUSY 1
#define I2C_READY 0

#define I2C_BUFFSIZE 4

//I2C Status bits
#define I2C_LASTTRANSOK 0x01	//bit 0
#define I2C_RXDATAINBUF 0x02	//bit 1





//TWSCRA
#define I2C_SDA_HOLD_TIME_EN	(1<<TWSHE)
#define I2C_SDA_HOLD_TIME_DIS	0

#define I2C_DATA_INTERRUPT_EN	(1<<TWDIE)
#define I2C_DATA_INTERRUPT_DIS	0

#define I2C_ADDR_STOP_INTERRUPT_EN		(1<<TWASIE)
#define I2C_ADDR_STOP_INTERRUPT_DIS		0

#define I2C_INTERFACE_EN		(1<<TWEN)
#define I2C_INTERFACE_DIS		0

#define I2C_STOP_INTERRUPT_EN	(1<<TWSIE)
#define I2C_STOP_INTERRUPT_DIS	0

#define I2C_PROMISC_EN			(1<<TWPME)
#define I2C_PROMISC_DIS			0

#define I2C_SMARTMODE_EN		(1<<TWSME)
#define I2C_SMARTMODE_DIS		0

//TWSCRB
#define I2C_HIGH_NOISE_EN		(1<<TWHNM)
#define I2C_HIGH_NOISE_DIS		0

#define I2C_ACK_ACTION_SENDNACK	(1<<TWAA)
#define I2C_ACK_ACTION_SENDACK	0

#define I2C_CMD_COMPLETE		0x02
#define I2C_CMD_CONTINUE		0x03
#define I2C_CMD_NOACTION		0x00



//TWSAM
#define I2C_SEC_ADDR_EN	(1<<TWAE)
#define I2C_SEC_ADDR_DIS	0





//I2C States
// I2C Slave Transmitter status codes
#define I2C_STX_ADR_ACK				0x63  // Own SLA+R has been received; ACK has been returned

#define I2C_STX_DATA_ACK			0xA2  // Data byte in TWDR has been transmitted; ACK has been received
#define I2C_STX_DATA_NACK			0xB2  // Data byte in TWDR has been transmitted; NOT ACK has been received

#define I2C_STX_STOP_RESTART		0x62  // A STOP condition or repeated START condition has been received while still addressed as Slave

// I2C Slave Receiver status codes
#define I2C_SRX_ADR_ACK				0x61  // Own SLA+W has been received ACK has been returned

#define I2C_SRX_DATA_ACK			0xA0  // Previously addressed with own SLA+W; data has been received; ACK has been returned
#define I2C_SRX_DATA_NACK			0xB0  // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned

#define I2C_SRX_STOP_RESTART		0x60  // A STOP condition or repeated START condition has been received while still addressed as Slave

// I2C Miscellaneous status codes
#define I2C_COLLISION				0x08  // Collision detected
#define I2C_S_BUS_ERROR				0x04  // Bus error due to an illegal START or STOP condition





/**
 * Interruption Service Routine
 */
ISR(TWI_SLAVE_vect);


/**
 * Class I2c
 */
class I2c{
	public:
		/**
		 * \brief Create a new I2C instance
		 * Configure low level (I/O) and I2C register
		 * Peripheral is configured as a slave, and will answer when the master call the address addr
		 */
		I2c(uint8_t addr);
		
		~I2c();
		
		/**
		 * \brief Configure the low level (I/O) and start the peripheral clock
		 */
		void lowLevelInit();
		
		/**
		 * \brief Configure the i2c peripheral to act as a slave with interruptions
		 * Configuration is the following :
		 * (*) SDA Hold Time Disabled
		 * (*) Promiscutie Mode disabled
		 * (*) SmartMode Disabled
		 * (*) High Noise Mode disabled
		 * (*) Mask disable
		 * \param addr used to set the address of the slave
		 */
		void configure(uint8_t addr);
		
		
		/**
		 * \brief Start a transmission with interruptions with data.
		 * The slave will wait for master Start and transmit or received regarding the WR msg.
		 * the transmission is completed when mBusy equal 0, ie when getStatus or getError is released.
		 */
		void startTransceiverWithData(uint8_t *msg, uint8_t msgSize);
		
		/**
		 * \brief Start a transmission with interruptions without data
		 * the peripheral is not busy until master ask for a transmission.
		 * previous buffer is used.
		 */
		void startTransceiver();
		
		/**
		 * \brief get data from local buffer if the transmission was started with startTransceiver()
		 * It will wait the end of the transmission
		 */
		uint8_t getDataFromTransceiver(uint8_t *msg, uint8_t msgSize);
		
		
		/**
		 * \brief return busy information
		 */
		inline uint8_t isBusy() { return mBusy; }
		
		/**
		 * \brief wait the end of the transmission and return the status
		 */
		inline uint8_t getStatus() { while(isBusy()) {} return mStatus; }
			
		/**
		 * \brief wait the end of the transmission and return the error
		 */
		inline uint8_t getError() { while(isBusy()) {} return mError; }
			
		/**
		 * \brief This function is called at every interruption to handle transmission states
		 */
		void callback();
		
	private:
		uint8_t mBusy;
		
		/** 
		 * Specify the state of transmitter (LAST TRANSMISsION OK / RXDATAINBUFF)
		 */
		uint8_t mStatus;
		
		/**
		 * Specify the error code, corresponding to the status register of the peripheral, if an error happened.
		 */
		uint8_t mError;
		
		uint8_t mMsg[I2C_BUFFSIZE];
		uint8_t mMsgSize;
};


#endif /* I2C_H_ */