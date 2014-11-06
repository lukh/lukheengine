/*
 * audioconf.cpp
 *
 *  Created on: 28 juin 2014
 *      Author: Vivien
 */

#include "abstractaudiodriver.hpp"
#include "engine.hpp"

AbstractAudioDriver::AbstractAudioDriver(SampleRate sr, FramePerBuffer fpb) :
	mSampleRate(sr), mFramesPerBuffer(fpb),
    mNumStereoIn(0), mNumStereoOut(0),
    mState(NonInitialized),
    mEngine((Engine*)NULLPTR)
{
}


void AbstractAudioDriver::getBuffer(UInt8_t inStCh, Sample **inBuffer, UInt8_t outStCh, Sample**outBuffer, UInt32_t *bufferSize){
    if(inStCh >= 0 && inStCh < AUDIOCONF_MAX_INPUT_CHANNELS)
    {
        *inBuffer = mInBuffers[inStCh];
    }

    if(outStCh >= 0 && outStCh < AUDIOCONF_MAX_OUTPUT_CHANNELS)
    {
        *outBuffer = mOutBuffers[outStCh];
    }

    *bufferSize = mFramesPerBuffer;
}



void AbstractAudioDriver::setNumStereoIn(UInt16_t numStereoIn){
    if(mState == Running) return;

    if(numStereoIn >= 0 && numStereoIn < AUDIOCONF_MAX_INPUT_CHANNELS)
    {
        mNumStereoIn = numStereoIn;
    }
}

void AbstractAudioDriver::setNumStereoOut(UInt16_t numStereoOut){
    if(mState == Running) return;

    if(numStereoOut >= 0 && numStereoOut < AUDIOCONF_MAX_OUTPUT_CHANNELS)
    {
        mNumStereoOut = numStereoOut;
    }
}


void AbstractAudioDriver::setEngine(Engine *eng){
    if(eng == NULLPTR) return;

    mEngine = eng;
}



