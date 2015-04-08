/*
 * MUX.h
 *
 * Created: 07/02/2015 23:22:05
 *  Author: Vivien
 */ 


#ifndef MUX_H_
#define MUX_H_

#include <avr/io.h>
#include "GPIO.h"

/*
 * Configuration defines
 */
#define MUX_EN 0
#define MUX_DIS 1

#define MUX_ADDR_HIGH 1
#define MUX_ADDR_LOW 0

/*
 * Mux4 : 
 */
class Mux4{
	public:
		Mux4(uint8_t portBase, uint8_t enablePin, uint8_t addr0Pin, uint8_t addr1Pin);
		//~Mux4();
		
		/**
		 * Configure the low level of the peripheral : GPIO, etc
		 */
		void lowLevelInit();
		
		/**
		 * \brief Enable the Mux, 
		 * It sets/resets the defined enable pin, regarding to the configuration (the value of MUX_EN)
		 */
		void enable();
		
		/**
		 * \brief Disable the Mux, 
		 * It sets/resets the defined enable pin, regarding to the configuration (the value of MUX_DIS)
		 */
		void disable();
		
		
		/**
		 * \brief Set the address of the mux
		 * it changes the state of the pins regarding to the configuration (MUX_ADDR_LOW/HIGH) and the value of the address given
		 */
		void setAddress(uint8_t addr);
		
		/**
		 * \brief returns the address
		 */
		uint8_t getAdress();
		
	private:
		uint8_t mPortBase;
		
		/**
		 * \brief the pin dedicated to the enable pin of the Mux
		 */
		uint8_t mEnablePin;
		/**
		 * \brief the pin dedicated to the low bit address of the Mux
		 */
		uint8_t mAddr0Pin;
		/**
		 * \brief the pin dedicated to the high bit address of the Mux
		 */
		uint8_t mAddr1Pin;
		
		uint8_t mAddrValue;
};



#endif /* MUX_H_ */