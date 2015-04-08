/*
 * GPIO.h
 *
 * Created: 08/02/2015 22:13:39
 *  Author: Vivien
 */ 


#ifndef GPIO_H_
#define GPIO_H_


#include <avr/io.h>


/* 
 * GPIO Defines
 */
#define GPIO_INPUT 0
#define GPIO_OUTPUT 1

#define GPIO_PU_EN 1
#define GPIO_PU_DIS 0

#define GPIO_PIN_0 0
#define GPIO_PIN_1 1
#define GPIO_PIN_2 2
#define GPIO_PIN_3 3
#define GPIO_PIN_4 4
#define GPIO_PIN_5 5
#define GPIO_PIN_6 6
#define GPIO_PIN_7 7

/*
 * Port Base Addr - need to be redefined, because in io.h, it's already a pointer !
 * then it is not compatible with the further defines
 */
#define PORTB_A (0x19)
#define PORTB_B (0x16)

/*
 * Easy Register Access Macro
 */
#define P_PIN(portbase)		_SFR_IO8(portbase)
#define P_DDR(portbase)		_SFR_IO8(portbase + 0x01)
#define P_PORT(portbase)	_SFR_IO8(portbase + 0x02)
#define P_PUE(portbase)		((portbase == PORTB_A) ? PUEA : PUEB)

/*
 * 
 */


#endif /* CONST_H_ */