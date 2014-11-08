/**
  ******************************************************************************
  * @file    abstractaudiodriver.hpp
  * @author  Vivien Henry
  * @version V1.0
  * @date    06/11/2014
  * @brief   AbstractAudioDriver is the abstraction of the driver,
  * to be able to load the code on different targets, such computers or embedded
  *
  @verbatim
  ==============================================================================
                        ##### AbstractAudioDriver #####
  ==============================================================================
  The user has to create his own driver, and implement virtual methods :

  terminate() : stop properly the driver, and free memory, peripherals
  start() : start processing
  stop() : stop processing

  The user has to add methods depending on the target, like a callback.
  This callback should access to the depending hardware buffer(s), reorganized if needed and copy samples in the mIn/OutBuffers
  Then, it has to call mEngine->process() to start signal processing.
  @endverbatim



  getBuffer(): return in out, and FramePerBuffer interleaved stereo buffer depending on the channels given.
  this method should be used in Engine::process, to be able to give to the AudioComponent::process the right stereo buffer.

  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */



#ifndef ABSTRACTAUDIDRIVER_HPP_
#define ABSTRACTAUDIDRIVER_HPP_

#include "const.hpp"

#define AUDIOCONF_MAX_OUTPUT_CHANNELS 4
#define AUDIOCONF_MAX_INPUT_CHANNELS 4


class Engine;



enum SampleRate{
	SR44100 = 44100,
    SR96000 = 96000,
    SR192000 = 192000
};

enum FramePerBuffer{
	FPB256 = 256,
	FPB512 = 512,
	FPB1024 = 1024
};

enum AudioDriverState{
	NonInitialized,
	Initialized,
	Running
};

class AbstractAudioDriver{
	public:
		AbstractAudioDriver(SampleRate sr, FramePerBuffer fpb);
        virtual ~AbstractAudioDriver() {}

        //virtual uint8_t configure() = 0;
        virtual uint8_t terminate() = 0;

        virtual uint8_t start() = 0;
        virtual uint8_t stop() = 0;

        // Maybe this one will be pure virtual, I still don't know...
        virtual uint8_t getBuffer(uint8_t inStCh, Sample **inBuffer, uint8_t outStCh, Sample**outBuffer, uint32_t *bufferSize);

        inline AudioDriverState getState() const { return mState; }

        void setEngine(Engine *eng);

        inline FramePerBuffer getFramesPerBuffer() const {	return mFramesPerBuffer;}
        inline SampleRate getSampleRate() const { return mSampleRate;}

        inline uint16_t getNumStereoIn() const { return mNumStereoIn;}
        inline uint16_t getNumStereoOut() const {   return mNumStereoOut;}

    protected:
        void setNumStereoIn(uint16_t numStereoIn);
        void setNumStereoOut(uint16_t numStereoOut);



	private:
		SampleRate mSampleRate;
		FramePerBuffer mFramesPerBuffer;

        uint16_t mNumStereoIn;
        uint16_t mNumStereoOut;

		AudioDriverState mState;

        Engine *mEngine;

        Sample *mInBuffers[AUDIOCONF_MAX_INPUT_CHANNELS];
        Sample *mOutBuffers[AUDIOCONF_MAX_OUTPUT_CHANNELS];
};


#endif /* AUDIOCONF_HPP_ */
