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



#define SP_OK 0
#define SP_ERROR -1

//#ifdef __TARGET_WIN__

#define NULLPTR ((void*)0)



typedef unsigned char UInt8_t;
typedef unsigned int UInt16_t;  //typedef unsigned short UInt16_t; //on PC
typedef unsigned long UInt32_t;

typedef char SInt8_t;
typedef int SInt16_t; //typedef short UInt16_t; //on PC
typedef long SInt32_t;

typedef double Sample;

//ou dans l'idéal utiliser
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
typedef unsigned long long uint64_t;
typedef long long int64_t;


#endif


//#endif


#endif /* CONST_HPP_ */
