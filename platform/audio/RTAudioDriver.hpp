/**
  ******************************************************************************
  * @file    RtAudioDriver.hpp
  * @author  Vivien Henry
  * @version V1.0
  * @date    09/11/2014
  * @brief   Implementation of the AbstractAudioDriver for the RtAudio engine.
  *
  @verbatim
  ==============================================================================
                        ##### RtAudioDriver #####
  ==============================================================================
  This implementation allow user to lauch the LuKHeEngine on a computer.
  It's not clean yet, the code is quite ugly, but it's work.
  I don't really care about RtAudio for the moment, regarding the purpose of LuKHeEngine

  configure() : init RtAudio
  terminate() : stop properly the driver.
  start() : start processing
  stop() : stop processing

  The callback call the driver process after set the intern buffers correctly
  @endverbatim

  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef RTAUDIODRIVER_HPP
#define RTAUDIODRIVER_HPP


#include "abstractaudiodriver.hpp"
#include "RtAudio.h"

//#define RTAUDIODRIVER_BYPASS
#define RTAUDIODRIVER_IN2OUT2_ENGINEMODE

const SampleRate sampleRate = SR44100;
const uint32_t framesPerBuffer = 512;
const uint8_t inDevId = 1, outDevId = 0, inDevIdAsio = 0, outDevIdAsio = 0;

class RtAudioDriver : public AbstractAudioDriver{
    public:
        RtAudioDriver();
        ~RtAudioDriver();

        virtual uint8_t configure();
        virtual uint8_t terminate();

        virtual uint8_t start();
        virtual uint8_t stop();

        static int callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *data );

    private:
        RtAudio rt;
        RtAudio::StreamParameters inParams, outParams;
};


#endif
