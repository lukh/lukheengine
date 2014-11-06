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
typedef unsigned int UInt16_t;
typedef unsigned long UInt32_t;

typedef char SInt8_t;
typedef int SInt16_t;
typedef long SInt32_t;

typedef double Sample;

//#endif


#endif /* CONST_HPP_ */
