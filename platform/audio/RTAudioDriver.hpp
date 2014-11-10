#ifndef RTAUDIODRIVER_HPP
#define RTAUDIODRIVER_HPP


#include "abstractaudiodriver.hpp"
#include "RtAudio.h"

//#define RTAUDIODRIVER_BYPASS
#define RTAUDIODRIVER_IN2OUT2_ENGINEMODE

const SampleRate sampleRate = SR44100;
const uint32_t framesPerBuffer = 512;

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
