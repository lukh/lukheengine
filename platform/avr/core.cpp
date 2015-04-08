/*
 * core.cpp
 *
 * Created: 29/03/2015 16:39:00
 *  Author: Vivien
 */ 

#include "core.h"

void Core::interruptsEnable(){
	sei();
}


void Core::interruptsDisable(){
	cli();
}