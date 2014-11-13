/**
  ******************************************************************************
  * \file    RtAudioDriver.hpp
  * \author  Vivien Henry
  * \version V1.0
  * \date    09/11/2014
  * \brief   Implementation of the AbstractAudioDriver for the RtAudio engine.
  *

  ******************************************************************************
  * \attention
  *
  ******************************************************************************
  */

#ifndef RTAUDIODRIVER_HPP
#define RTAUDIODRIVER_HPP


/**
  \addtogroup AudioDriver
  @{
  */

#include "abstractaudiodriver.hpp"
#include "RtAudio.h"

//#define RTAUDIODRIVER_BYPASS
#define RTAUDIODRIVER_IN2OUT2_ENGINEMODE

const SampleRate sampleRate = SR44100;
const uint32_t framesPerBuffer = 512;
const uint8_t inDevId = 1, outDevId = 0, inDevIdAsio = 0, outDevIdAsio = 0;

/**
  \brief This implementation allow user to lauch the LuKHeEngine on a computer.
  \verbatim
  It's not clean yet, the code is quite ugly, but it's work.
  I don't really care about RtAudio for the moment, regarding the purpose of LuKHeEngine

  configure() : init RtAudio
  terminate() : stop properly the driver.
  start() : start processing
  stop() : stop processing

  The callback call the driver process after set the intern buffers correctly
  \endverbatim
 */
class RtAudioDriver : public AbstractAudioDriver{
    public:
        /**
         * Implementation of the constructor
         * Force the AbstractAudioDriver::AbstractAudioDriver with the const parameters
         */
        RtAudioDriver();

        ~RtAudioDriver();

        /**
         * Implementation of configure for RtAudio
         * \retval LE_OK or LE_ERROR
         */
        virtual uint8_t configure();


        /**
         * Implementation of terminate for RtAudio
         * \retval LE_OK or LE_ERROR
         */
        virtual uint8_t terminate();

        /**
         * implementation of start for RtAudio
         * \retval LE_OK or LE_ERROR
         */
        virtual uint8_t start();


        /**
         * Implementation of stop for RtAudio
         * \retval LE_OK or LE_ERROR
         */
        virtual uint8_t stop();


        /**
         * A RtAudio (API) dependent callback. static.
         * called each time a new frames buffer is available
         * \param outputBuffer pointer on the API out buffer
         * \param inputBuffer pointer on the API in buffer
         * \param nBufferFrames number of effective frames
         * \param streamTime
         * \param status
         * \param data : user data, void pointer.
         * in this case, contain the adress of the driver object
         * that's way, the callback is able to call setIn/OutBufferAddr to set the new adresses
         * and to call driver->process to start processing
         * \retval
         */
        static int callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *data );

    private:
        RtAudio rt;
        RtAudio::StreamParameters inParams, outParams;
};

/**
  }
  */


#endif
