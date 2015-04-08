/*
 * core.h
 *
 * Created: 29/03/2015 16:38:45
 *  Author: Vivien
 */ 


#ifndef CORE_H_
#define CORE_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "core_conf.h"

class Core{
	public:
		static void interruptsEnable();
		static void interruptsDisable();
};


#endif /* CORE_H_ */