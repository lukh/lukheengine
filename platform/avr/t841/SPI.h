/*
 * SPI.h
 *
 * Created: 07/02/2015 23:21:49
 *  Author: Vivien
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "GPIO.h"

/*
 * Configuration Defines
 */

//SPCR
#define SPI_INTERRUPT_EN	(0x80)
#define SPI_INTERRUPT_DIS	(0x00)

#define SPI_ENABLE			(0x40)
#define SPI_DISABLE			(0x00)

#define SPI_LSB_FIRST		(0x20)
#define SPI_MSB_FIRST		(0x00)

#define SPI_MASTERMODE		(0x10)
#define SPI_SLAVEMODE		(0x00)

#define SPI_CLK_FALLING		(0x08)
#define SPI_CLK_RISING		(0x00)

#define SPI_PHASE_TRAILING	(0x04)
#define SPI_PHASE_LEADING	(0x00)

#define SPI_CLK_DIV_4		(0x00)
#define SPI_CLK_DIV_16		(0x01)
#define SPI_CLK_DIV_64		(0x02)
#define SPI_CLK_DIV_128		(0x03)

//SPSR
#define SPI_INTERRUPT_FLAG	(0x80)
#define SPI_COLL_FLAG		(0x40)

#define SPI_DOUBLECLK		(0x01)
#define SPI_SINGLECLK		(0x00)

//REMAP
#define SPI_REMAPPED		(0x02)
#define SPI_DEFMAP			(0x00)


//local
#define SPI_TRANSFER_DONE 1
#define SPI_TRANSFER_UNDONE 0

/*Interruption Service Routine*/
ISR(SPI_vect);

/*
 * Class SPI
 */
class SPI{
	public:
		/*
		 * \brief Configure the SPI in slave mode
		 */
		SPI(
			uint8_t mapping, 
			uint8_t dataOrder, 
			uint8_t clockPol, 
			uint8_t clockPhase, 
			uint8_t clockRate, 
			uint8_t doubleSpeed);
			
		/*
		 * \brief Configure the SPI in master mode
		 */
		SPI(
			uint8_t mapping,
			uint8_t portBaseCS, 
			uint8_t csPin,  
			uint8_t dataOrder, 
			uint8_t clockPol, 
			uint8_t clockPhase, 
			uint8_t clockRate, 
			uint8_t doubleSpeed);
		~SPI();
		
		/*
		 * \brief Configure I/O for the SpI in slave mode
		 * mapping: SPI_REMAP/SPI_DEFMAP
		 * in slave mode, there is no way to change between the both configuration (MAP/REMAP)
		 */
		void lowLevelInit(uint8_t mapping);
		
		/*
		 * \brief Configure I/O for the SpI in master mode
		 * mapping: SPI_REMAP/SPI_DEFMAP
		 * portBaseCS/pin : chip select pin, not controlled by hardware
		 */
		void lowLevelInit(uint8_t mapping, uint8_t portBaseCS, uint8_t csPin);
		
		/*
		 * \brief Configure the SPI peripheral
		 */
		void configure(uint8_t maSlSelect,
			uint8_t intEn,
			uint8_t dataOrder,
			uint8_t clockPol,
			uint8_t clockPhase,
			uint8_t clockRate,
			uint8_t doubleSpeed);
		
		/*
		 * Send a byte regarding to the mode
		 */
		void sendByte(uint8_t byte);
		
		/*
		 * receive a byte regarding to the mode
		 */
		uint8_t receiveByte();
		
		/*
		 * send a byte and receive some, in master mode, with interrupts
		 */
		void sendReceive(uint8_t *pData, uint8_t sizeToSend, uint8_t sizeToReceive);
		
		/*
		 * A callback function to manage transfert
		 */
		void callback();
		
		/*
		 * Return the value of transfert complete flag
		 */
		inline uint8_t isTransfertComplete() {return mTransferComplete; }
		
		
	private:
		//low level
		uint8_t mPortBaseCS;
		uint8_t mCsPin;
		
		uint8_t mMode;
		
		//dedicated for long transfert in master mode
		uint8_t *mData;
		uint8_t *mDataPointer;
		uint8_t mSizeToSend;
		uint8_t mSizeToReceive;
		uint8_t mTransferComplete;
};

#endif /* SPI_H_ */