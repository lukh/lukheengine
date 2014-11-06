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

  getBuffer(): return in out, and FramePerBuffer interleaved stereo buffer depending on the channels given.
  this method should be used in Engine::process, to be able to give to the AudioComponent::process the right stereo buffer.

  The user has to add methods depending on the target, like a callback.
  This callback should access to the buffer(s), and reorganize if needed.
  Then, it has to call mEngine->process() to start signal processing.
  @endverbatim
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */



#ifndef ABSTRACTAUDIDRIVER_HPP_
#define ABSTRACTAUDIDRIVER_HPP_

#include "const.hpp"

#define AUDIOCONF_MAX_OUTPUT_CHANNELS 16
#define AUDIOCONF_MAX_INPUT_CHANNELS 16


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

        //virtual UInt8_t configure() = 0;
		virtual UInt8_t terminate() = 0;

		virtual UInt8_t start() = 0;
		virtual UInt8_t stop() = 0;

        virtual void getBuffer(UInt8_t inStCh, Sample **inBuffer, UInt8_t outStCh, Sample**outBuffer, UInt32_t *bufferSize) = 0;




        inline AudioDriverState getState() const { return mState; }

        inline FramePerBuffer getFramesPerBuffer() const {	return mFramesPerBuffer;}
        inline SampleRate getSampleRate() const { return mSampleRate;}

        inline UInt16_t getNumStereoIn() const { return mNumStereoIn;}
        inline UInt16_t getNumStereoOut() const {   return mNumStereoOut;}

        void setNumStereoIn(UInt16_t numStereoIn);
        void setNumStereoOut(UInt16_t numStereoOut);


        void setEngine(Engine *eng);



	private:
		SampleRate mSampleRate;
		FramePerBuffer mFramesPerBuffer;

        UInt16_t mNumStereoIn;
        UInt16_t mNumStereoOut;

		AudioDriverState mState;

        Engine *mEngine;
};


#endif /* AUDIOCONF_HPP_ */
