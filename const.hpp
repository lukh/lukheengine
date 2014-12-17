/**
  ******************************************************************************
  * @file    const.hpp
  * @author  Vivien Henry
  * @version V1.0
  * @date    06/11/2014
  * @brief   Const declaration for the LUKHEngine.
  * Since there is different target, there is multiple declaration.
  * Change the preprocessor declaration to change target
  *
  @verbatim

  @endverbatim
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef CONST_HPP_
#define CONST_HPP_


#define NULLPTR ((void*)0)

typedef signed int Sample;

#if __cplusplus >= 201103L
#include <cstdint> //Si le compilo supporte C++ 2011 et les
                   // uint16_t, uint32_t sont automatiquement
                   //définis selon la plateforme
#else
//ou sinon http://minirighi.sourceforge.net/html/types_8h.html
typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int int16_t;
typedef unsigned short int uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;
#endif


typedef enum
{
	LE_OK = 0x00,
	LE_ERROR = 0x01,
	LE_BUSY = 0x02,
	LE_TIMEOUT = 0x03
}LEStatus;


#endif /* CONST_HPP_ */
